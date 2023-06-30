// Copyright 2011-2020 Wason Technology, LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifdef ROBOTRACONTEUR_CORE_USE_STDAFX
#include "stdafx.h"
#endif

#include "RobotRaconteurNode_connector_private.h"

#include <boost/foreach.hpp>
#include <boost/range/adaptors.hpp>
#include <boost/range/algorithm.hpp>
#include <boost/algorithm/string.hpp>

#include "RobotRaconteur/Client.h"
#include "RobotRaconteur/Service.h"

namespace RobotRaconteur
{
namespace detail
{

static void RobotRaconteurNode_connector_empty_handler() {}

static void RobotRaconteurNode_connector_empty_handler(const RR_SHARED_PTR<RobotRaconteurException>&) {}

RobotRaconteurNode_connector::endpoint_cleanup::endpoint_cleanup(const RR_SHARED_PTR<ClientContext>& ep,
                                                                 const RR_SHARED_PTR<RobotRaconteurNode>& node)
{
    this->ep = ep;
    this->node = node;
}

// NOLINTBEGIN(bugprone-exception-escape)
RobotRaconteurNode_connector::endpoint_cleanup::~endpoint_cleanup()
{
    RR_SHARED_PTR<ClientContext> ep1;
    {
        boost::mutex::scoped_lock lock(eplock);
        ep1 = ep;
        ep.reset();
    }

    if (!ep1)
        return;
    try
    {
        node->DeleteEndpoint(ep1);
    }
    catch (std::exception&)
    {}
}
// NOLINTEND(bugprone-exception-escape)

void RobotRaconteurNode_connector::endpoint_cleanup::release()
{
    boost::mutex::scoped_lock lock(eplock);
    ep.reset();
}

RobotRaconteurNode_connector::RobotRaconteurNode_connector(const RR_SHARED_PTR<RobotRaconteurNode>& node)
{
    active_count = 0;
    connecting = true;
    transport_connected = false;
    timeout = -1;
    this->node = node;
}

void RobotRaconteurNode_connector::handle_error(const int32_t& key, const RR_SHARED_PTR<RobotRaconteurException>& err)
{
    // std::cout << "Got error" << std::endl;
    {
        boost::mutex::scoped_lock lock(connecting_lock);
        if (!connecting)
            return;
    }

    {
        boost::mutex::scoped_lock lock(active_lock);
        active.remove(key);
        errors.push_back(err);

        // if(active.size()!=0) return;
    }

    boost::mutex::scoped_lock lock2(handler_lock);

    {
        boost::mutex::scoped_lock lock(active_lock);
        // std::cout << active.size() << std::endl;
        if (!active.empty())
            return;
    }

    {
        boost::mutex::scoped_lock lock(connect_timer_lock);

        connect_timer.reset();
        connect_backoff_timer.reset();
    }

    // All activities have completed, assume failure

    bool c = false;
    {
        boost::mutex::scoped_lock lock(connecting_lock);
        c = connecting;
        connecting = false;
    }

    if (!c)
        return;
    ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Node, -1, "Client connection failed: " << err->what());

    detail::InvokeHandlerWithException(node, handler, err);
}

void RobotRaconteurNode_connector::connected_client(const RR_SHARED_PTR<RRObject>& client,
                                                    const RR_SHARED_PTR<RobotRaconteurException>& err,
                                                    const std::string& url, const RR_SHARED_PTR<endpoint_cleanup>& ep,
                                                    int32_t key)
{
    RR_UNUSED(key);
    // std::cout << "Connected client" << std::endl;
    if (err)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Node, -1, "Client connection failed: " << err->what());

        bool c = false;
        {
            boost::mutex::scoped_lock lock(connecting_lock);
            c = connecting;
            connecting = false;
        }

        {
            boost::mutex::scoped_lock lock(connect_timer_lock);
            connect_backoff_timer.reset();
            connect_timer.reset();
        }

        try
        {
            if (client)
            {
                node->AsyncDisconnectService(client, boost::bind(&RobotRaconteurNode_connector_empty_handler));
            }
        }
        catch (std::exception&)
        {}

        if (c)
        {
            detail::InvokeHandlerWithException(node, handler, err);
        }

        return;
    }

    bool c = false;
    {
        boost::mutex::scoped_lock lock(connecting_lock);
        c = connecting;
        connecting = false;
    }
    if (!c)
    {
        ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Node, -1, "Already connected, closing candidate: " << err->what());
        try
        {
            if (client)
            {
                node->AsyncDisconnectService(client, boost::bind(&RobotRaconteurNode_connector_empty_handler));
            }
        }
        catch (std::exception& exp)
        {
            RobotRaconteurNode::TryHandleException(node, &exp);
        }
    }
    else
    {
        {
            boost::mutex::scoped_lock lock(connect_timer_lock);

            connect_timer.reset();
            connect_backoff_timer.reset();
        }

        try
        {
            ep->release();
            RR_SHARED_PTR<ClientContext> client_context = rr_cast<ServiceStub>(client)->GetContext();
            if (listener != 0)
                client_context->ClientServiceListener.connect(listener);
            // std::cout << "call connect handler" << std::endl;
            ROBOTRACONTEUR_LOG_INFO_COMPONENT(node, Node,
                                              rr_cast<ServiceStub>(client)->GetContext()->GetLocalEndpoint(),
                                              "Client connected: " << url);
            if (client_context->GetUserAuthenticated())
            {
                ROBOTRACONTEUR_LOG_INFO_COMPONENT(
                    node, Node, rr_cast<ServiceStub>(client)->GetContext()->GetLocalEndpoint(),
                    "Client authenticated with username: \"" << client_context->GetAuthenticatedUsername() << "\"");
            }

            detail::InvokeHandler(node, handler, client);
        }
        catch (std::exception& exp)
        {
            RobotRaconteurNode::TryHandleException(node, &exp);
        }
    }
}

void RobotRaconteurNode_connector::connected_transport(const RR_SHARED_PTR<Transport>& transport,
                                                       const RR_SHARED_PTR<ITransportConnection>& connection,
                                                       const RR_SHARED_PTR<RobotRaconteurException>& err,
                                                       const std::string& url,
                                                       const RR_SHARED_PTR<endpoint_cleanup>& ep, int32_t key)
{

    if (err)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Node, -1,
                                           "Error connecting to candidate service url " << url << " : " << err->what());
        // std::cout << "Transport connect error" << std::endl;
        handle_error(key, err);
        return;
    }

    boost::mutex::scoped_lock lock(connecting_lock);

    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Node, -1, "Transport connected: " << url);
    // std::cout << "Transport connected" << std::endl;
    bool c = false;
    bool tc = false;
    {

        c = connecting;
        tc = transport_connected;
    }
    if (!c)
    {
        ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Node, -1, "Already connected, closing candidate url: " << url);
        // std::cout << "Node connector close transport" << std::endl;
        try
        {
            connection->Close();
        }
        catch (std::exception&)
        {}
    }
    else if (tc)
    {
        ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Node, -1, "Already connected, closing candidate url: " << url);
        boost::mutex::scoped_lock lock(active_lock);
        active.remove(key);
        try
        {
            connection->Close();
        }
        catch (std::exception&)
        {}
    }
    else
    {
        try
        {
            int32_t key2 = 0;
            {
                boost::mutex::scoped_lock lock(active_lock);
                active_count++;
                key2 = active_count;

                ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Node, -1, "Transport connected, connect service: " << url);

                // std::cout << "Node connector connect service" << std::endl;
                ep->ep->AsyncConnectService(
                    transport, connection, url, username, credentials, objecttype,
                    boost::protect(boost::bind(&RobotRaconteurNode_connector::connected_client, shared_from_this(),
                                               RR_BOOST_PLACEHOLDERS(_1), RR_BOOST_PLACEHOLDERS(_2), url, ep, key2)),
                    timeout);

                transport_connected = true;

                active.push_back(key2);
                active.remove(key);
            }
        }
        catch (RobotRaconteurException& err2)
        {
            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Node, -1,
                                               "Error connecting to candidate service url: " << err2.what());
            if (transport)
            {
                try
                {
                    transport->Close();
                }
                catch (std::exception&)
                {}
            }

            connecting_lock.unlock();
            handle_error(key, RobotRaconteurExceptionUtil::DownCastException(err2));
        }
        catch (std::exception& err2)
        {
            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Node, -1,
                                               "Error connecting to candidate service url: " << err2.what());
            if (transport)
            {
                try
                {
                    transport->Close();
                }
                catch (std::exception&)
                {}
            }
            connecting_lock.unlock();
            handle_error(key, RR_MAKE_SHARED<ConnectionException>(err2.what()));
        }
    }
}

void RobotRaconteurNode_connector::connect_timer_callback(const boost::system::error_code& e)
{
    RR_UNUSED(e);
    // if (!e.stopped) //Allow the cancellation of the timer to kill the connect attempt
    {
        {
            boost::mutex::scoped_lock lock(connecting_lock);
            if (!connecting)
                return;
            connecting = false;
        }

        {
            boost::mutex::scoped_lock lock(connect_timer_lock);
            connect_backoff_timer.reset();
            connect_timer.reset();
        }

        ROBOTRACONTEUR_LOG_INFO_COMPONENT(node, Node, -1, "Connect timed out");
        boost::mutex::scoped_lock lock2(handler_lock);
        detail::InvokeHandlerWithException(node, handler, RR_MAKE_SHARED<ConnectionException>("Connection timed out"));
    }
}

void RobotRaconteurNode_connector::start_connect_timer()
{
    if (timeout != RR_TIMEOUT_INFINITE)
    {
        boost::mutex::scoped_lock lock(connect_timer_lock);
        connect_timer = RR_SHARED_PTR<boost::asio::deadline_timer>(
            new boost::asio::deadline_timer(node->GetThreadPool()->get_io_context()));
        connect_timer->expires_from_now(boost::posix_time::milliseconds(timeout));
        RobotRaconteurNode::asio_async_wait(node, connect_timer,
                                            boost::bind(&RobotRaconteurNode_connector::connect_timer_callback,
                                                        shared_from_this(), boost::asio::placeholders::error));
    }
}

void RobotRaconteurNode_connector::connect2(const RR_SHARED_PTR<std::vector<std::string> >& urls, int32_t main_key,
                                            const boost::system::error_code& e)
{
    RR_UNUSED(e);
    {
        boost::mutex::scoped_lock lock(connecting_lock);
        if (!connecting)
        {
            return;
        }
    }

    std::string url;
    {
        boost::mutex::scoped_lock lock2(active_lock);
        try
        {
            url = node->SelectRemoteNodeURL(*urls);
            urls->erase(std::remove(urls->begin(), urls->end(), url), urls->end());
        }
        catch (RobotRaconteurException& err2)
        {
            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Node, -1,
                                               "Error connecting to candidate service url: " << err2.what());
            start_connect_timer();
            lock2.unlock();
            handle_error(main_key, RobotRaconteurExceptionUtil::DownCastException(err2));
            return;
        }
        catch (std::exception& err2)
        {
            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Node, -1,
                                               "Error connecting to candidate service url: " << err2.what());
            start_connect_timer();
            lock2.unlock();
            handle_error(main_key, RR_MAKE_SHARED<ConnectionException>(err2.what()));
            return;
        }
    }

    RR_SHARED_PTR<Transport> t = connectors.at(url).lock();
    if (t)
    {
        int32_t key2 = 0;

        try
        {
            RR_SHARED_PTR<ClientContext> c = RR_MAKE_SHARED<ClientContext>(node);
            node->RegisterEndpoint(rr_cast<Endpoint>(c));
            RR_SHARED_PTR<endpoint_cleanup> ep = RR_MAKE_SHARED<endpoint_cleanup>(c, node);

            boost::mutex::scoped_lock lock2(active_lock);
            active_count++;
            key2 = active_count;

            boost::function<void(const RR_SHARED_PTR<ITransportConnection>&,
                                 const RR_SHARED_PTR<RobotRaconteurException>&)>
                h = boost::protect(boost::bind(&RobotRaconteurNode_connector::connected_transport, shared_from_this(),
                                               t, RR_BOOST_PLACEHOLDERS(_1), RR_BOOST_PLACEHOLDERS(_2), url, ep, key2));
            t->AsyncCreateTransportConnection(url, c, h);

            active.push_back(key2);
        }
        catch (RobotRaconteurException& err2)
        {
            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Node, -1,
                                               "Error connecting to candidate service url: " << err2.what());
            boost::mutex::scoped_lock lock2(active_lock);
            if (urls->empty())
            {
                lock2.unlock();
                handle_error(main_key, RobotRaconteurExceptionUtil::DownCastException(err2));
                return;
            }
        }
        catch (std::exception& err2)
        {
            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Node, -1,
                                               "Error connecting to candidate service url: " << err2.what());
            boost::mutex::scoped_lock lock2(active_lock);
            if (urls->empty())
            {
                lock2.unlock();
                handle_error(main_key, RR_MAKE_SHARED<ConnectionException>(err2.what()));
                return;
            }
        }
    }
    {
        boost::mutex::scoped_lock lock2(active_lock);
        if (!urls->empty())
        {

            lock2.unlock();
            boost::mutex::scoped_lock lock3(connect_timer_lock);
            if (!connect_backoff_timer)
            {
                connect_backoff_timer = RR_SHARED_PTR<boost::asio::deadline_timer>(
                    new boost::asio::deadline_timer(node->GetThreadPool()->get_io_context()));
            }
            connect_backoff_timer->expires_from_now(boost::posix_time::milliseconds(15));
            RobotRaconteurNode::asio_async_wait(node, connect_backoff_timer,
                                                boost::bind(&RobotRaconteurNode_connector::connect2, shared_from_this(),
                                                            urls, main_key, boost::asio::placeholders::error));

            return;
        }
    }

    start_connect_timer();
    {
        boost::mutex::scoped_lock lock(active_lock);
        active.remove(main_key);
    }
}

void RobotRaconteurNode_connector::connect(
    const std::map<std::string, RR_WEAK_PTR<Transport> >& connectors, boost::string_ref username,
    const RR_INTRUSIVE_PTR<RRMap<std::string, RRValue> >& credentials,
    boost::function<void(const RR_SHARED_PTR<ClientContext>&, ClientServiceListenerEventType,
                         const RR_SHARED_PTR<void>&)>
        listener,
    boost::string_ref objecttype,
    boost::function<void(const RR_SHARED_PTR<RRObject>&, const RR_SHARED_PTR<RobotRaconteurException>&)> handler,
    int32_t timeout)
{

    ROBOTRACONTEUR_LOG_INFO_COMPONENT(
        node, Node, -1,
        "Begin connect with candidate urls: " << boost::join(connectors | boost::adaptors::map_keys, ", "));

    this->connectors = connectors;
    this->username = RR_MOVE(username.to_string());
    this->credentials = credentials;
    this->listener = RR_MOVE(listener);
    this->objecttype = RR_MOVE(objecttype.to_string());
    this->handler = RR_MOVE(handler);
    this->timeout = timeout;

    int32_t key = 0;
    {
        boost::mutex::scoped_lock lock(active_lock);
        active_count++;
        key = active_count;
        active.push_back(key);
    }

    try
    {
        boost::mutex::scoped_lock lock2(handler_lock);

        RR_SHARED_PTR<std::vector<std::string> > urls = RR_MAKE_SHARED<std::vector<std::string> >();
        boost::copy(connectors | boost::adaptors::map_keys, std::back_inserter(*urls));

        if (urls->empty())
        {
            throw ConnectionException("No URLs specified");
        }

        boost::system::error_code ec2;
        lock2.unlock();
        connect2(urls, key, ec2);
    }
    catch (RobotRaconteurException& err2)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Node, -1, "Error connecting to service: " << err2.what());
        handle_error(key, RobotRaconteurExceptionUtil::DownCastException(err2));
    }
    catch (std::exception& err2)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Node, -1, "Error connecting to service: " << err2.what());
        handle_error(key, RR_MAKE_SHARED<ConnectionException>(err2.what()));
    }
}

} // namespace detail

} // namespace RobotRaconteur
