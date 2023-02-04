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

#include "RobotRaconteur/TcpTransport.h"
#include "TcpTransport_private.h"
#include <boost/algorithm/string.hpp>
#include <boost/shared_array.hpp>
#include <RobotRaconteur/ServiceIndexer.h>
#include "RobotRaconteur/StringTable.h"
#include "LocalTransport_private.h"

#include <set>

#ifdef ROBOTRACONTEUR_WINDOWS
#include <IPHlpApi.h>
#include <Shlobj.h>
#define SHUT_RDWR SD_BOTH
#undef SendMessage
#else
#define closesocket close
#define SOCKET_ERROR (-1)
#define INVALID_SOCKET (-1)
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <signal.h>
#include <netdb.h>
#include <sys/socket.h>
#include <ifaddrs.h>
#endif

#ifdef ROBOTRACONTEUR_APPLE
#include <CoreFoundation/CoreFoundation.h>
#include <Security/Security.h>
#endif

#ifdef ROBOTRACONTEUR_ANDROID
#include <ifaddrs.h>
#endif

#include <boost/range/adaptors.hpp>
#include <boost/range/algorithm.hpp>
#include <boost/foreach.hpp>

#if BOOST_ASIO_VERSION < 101200
#define RR_BOOST_ASIO_IP_ADDRESS_V6_FROM_STRING boost::asio::ip::address_v6::from_string
#else
#define RR_BOOST_ASIO_IP_ADDRESS_V6_FROM_STRING boost::asio::ip::make_address_v6
#endif

#if BOOST_ASIO_VERSION < 101200
#define RR_BOOST_ASIO_IP_ADDRESS_FROM_STRING boost::asio::ip::address::from_string
#else
#define RR_BOOST_ASIO_IP_ADDRESS_FROM_STRING boost::asio::ip::make_address
#endif

namespace RobotRaconteur
{

static std::string TcpTransport_socket_remote_endpoint(const RR_SHARED_PTR<boost::asio::ip::tcp::socket>& socket)
{
    boost::system::error_code ec;
    boost::asio::ip::tcp::endpoint ep = socket->remote_endpoint(ec);
    if (ec)
    {
        return "[unknown]";
    }
    std::stringstream ss;
    ss << ep;
    return ss.str();
}

static std::string TcpTransport_socket_local_endpoint(const RR_SHARED_PTR<boost::asio::ip::tcp::socket>& socket)
{
    boost::system::error_code ec;
    boost::asio::ip::tcp::endpoint ep = socket->local_endpoint(ec);
    if (ec)
    {
        return "[unknown]";
    }
    std::stringstream ss;
    ss << ep;
    return ss.str();
}

namespace detail
{
TcpConnector::TcpConnector(const RR_SHARED_PTR<TcpTransport>& parent)
{
    this->parent = parent;
    endpoint = 0;
    resolve_count = 0;
    connect_count = 0;
    connecting = false;
    active_count = 0;
    socket_connected = false;

    connect_timer = RR_SHARED_PTR<boost::asio::deadline_timer>(
        new boost::asio::deadline_timer(parent->GetNode()->GetThreadPool()->get_io_context()));
    backoff_timer = RR_SHARED_PTR<boost::asio::deadline_timer>(
        new boost::asio::deadline_timer(parent->GetNode()->GetThreadPool()->get_io_context()));
    node = parent->GetNode();
}

void TcpConnector::Connect(
    const std::vector<std::string>& url, uint32_t endpoint,
    boost::function<void(const RR_SHARED_PTR<ITransportConnection>&, const RR_SHARED_PTR<RobotRaconteurException>&)>
        handler)
{

    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, endpoint,
                                       "TcpConnector begin connection with candidate URLs: " << boost::join(url, ", "));

    this->callback = RR_MOVE(handler);
    this->endpoint = endpoint;
    this->url = url.at(0);

    {
        boost::mutex::scoped_lock lock(this_lock);
        connecting = true;
        _resolver = RR_SHARED_PTR<boost::asio::ip::tcp::resolver>(
            new boost::asio::ip::tcp::resolver(parent->GetNode()->GetThreadPool()->get_io_context()));
        parent->AddCloseListener(_resolver, &boost::asio::ip::tcp::resolver::cancel);
    }

    std::vector<boost::tuple<std::string, std::string> > queries;

    BOOST_FOREACH (const std::string& e, url)
    {
        try
        {

            ParseConnectionURLResult url_res = ParseConnectionURL(e);

            /*boost::smatch url_result;
            boost::regex_search(*e,url_result,boost::regex("^([^:]+)://((?:\\[[A-Fa-f0-9:]+(?:\\%\\w*)?\\])|(?:[^\\[\\]\\:]+)):([^:/]+)/?(.*)$"));

            if (url_result.size()<4) throw InvalidArgumentException("Malformed URL");

            if (url_result[1]!="tcp" && url_result[1]!="tcps") throw InvalidArgumentException("Invalid transport type
            for TcpTransport");*/

            if (url_res.scheme != "tcp" && url_res.scheme != "rr+tcp" && url_res.scheme != "rrs+tcp")
                throw InvalidArgumentException("Invalid transport type for TcpTransport");
            if (url_res.host.empty())
                throw ConnectionException("Invalid host for tcp transport");
            if (!url_res.path.empty() && url_res.path != "/")
                throw ConnectionException("Invalid host for tcp transport");
            std::string host = url_res.host;
            std::string port = boost::lexical_cast<std::string>(url_res.port);

            boost::trim_left_if(host, boost::is_from_range('[', '['));
            boost::trim_right_if(host, boost::is_from_range(']', ']'));
            queries.push_back(boost::make_tuple(host, port));
        }
        catch (std::exception& exp)
        {
            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, endpoint,
                                               "Candidate URL \"" << e << "\" error: " << exp.what());
            if (url.size() == 1)
                throw;
        }
    }

    if (queries.empty())
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, endpoint, "Could not find route to supplied address");
        throw ConnectionException("Could not find route to supplied address");
    }

    {
        {
            {
                boost::mutex::scoped_lock lock(this_lock);
                if (!connecting)
                    return;

                connect_timer->expires_from_now(boost::posix_time::milliseconds(parent->GetDefaultConnectTimeout()));
                RobotRaconteurNode::asio_async_wait(node, connect_timer,
                                                    boost::bind(&TcpConnector::connect_timer_callback,
                                                                shared_from_this(), boost::asio::placeholders::error));

                parent->AddCloseListener(connect_timer,
                                         boost::bind(&boost::asio::deadline_timer::cancel, RR_BOOST_PLACEHOLDERS(_1)));
            }

            typedef boost::tuple<std::string, std::string> e_type;
            BOOST_FOREACH (e_type& e, queries)
            {
                int32_t key2 = 0;
                {
                    boost::mutex::scoped_lock lock(this_lock);
                    active_count++;
                    key2 = active_count;
#if BOOST_ASIO_VERSION < 101200
                    boost::asio::ip::basic_resolver_query<boost::asio::ip::tcp> q(
                        e.get<0>(), e.get<1>(), boost::asio::ip::resolver_query_base::flags());
                    RobotRaconteurNode::asio_async_resolve(node, _resolver, q,
                                                           boost::bind(&TcpConnector::connect2, shared_from_this(),
                                                                       key2, boost::asio::placeholders::error,
                                                                       boost::asio::placeholders::iterator, callback));
#else
                    RobotRaconteurNode::asio_async_resolve(node, _resolver, e.get<0>(), e.get<1>(),
                                                           boost::bind(&TcpConnector::connect2, shared_from_this(),
                                                                       key2, boost::asio::placeholders::error,
                                                                       boost::asio::placeholders::results, callback));
#endif
                    // std::cout << "Begin resolve" << std::endl;

                    active.push_back(key2);
                }
            }
        }
    }
}

static std::string TcpConnector_log_candidate_endpoints(RR_SHARED_PTR<std::list<boost::asio::ip::tcp::endpoint> >& eps)
{
    std::stringstream ss;
    for (std::list<boost::asio::ip::tcp::endpoint>::iterator e = eps->begin();;)
    {
        ss << *e;
        ++e;
        if (e != eps->end())
        {
            ss << ", ";
        }
        else
        {
            break;
        }
    }
    return ss.str();
}

#if BOOST_ASIO_VERSION < 101200
void TcpConnector::connect2(int32_t key, const boost::system::error_code& err,
                            boost::asio::ip::tcp::resolver::iterator endpoint_iterator,
                            const boost::function<void(const RR_SHARED_PTR<TcpTransportConnection>&,
                                                       const RR_SHARED_PTR<RobotRaconteurException>&)>& callback)
{
    boost::asio::ip::basic_resolver_iterator<boost::asio::ip::tcp> end;
#else
void TcpConnector::connect2(int32_t key, const boost::system::error_code& err,
                            const boost::asio::ip::tcp::resolver::results_type& results,
                            const boost::function<void(const RR_SHARED_PTR<TcpTransportConnection>&,
                                                       const RR_SHARED_PTR<RobotRaconteurException>&)>& callback)
{
    boost::asio::ip::tcp::resolver::results_type::const_iterator endpoint_iterator = results.begin();
    boost::asio::ip::tcp::resolver::results_type::const_iterator end;
#endif
    RR_UNUSED(callback);
    if (err)
    {
        ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, endpoint, "Error resolving candidate host: " << err);
        handle_error(key, err);
        return;
    }

    // std::cout << "End resolve" << std::endl;
    try
    {

        RR_SHARED_PTR<std::list<boost::asio::ip::tcp::endpoint> > candidate_endpoints =
            RR_MAKE_SHARED<std::list<boost::asio::ip::tcp::endpoint> >();

        std::vector<boost::asio::ip::tcp::endpoint> ipv4;
        std::vector<boost::asio::ip::tcp::endpoint> ipv6;

        for (; endpoint_iterator != end; endpoint_iterator++)
        {
            if (endpoint_iterator->endpoint().address().is_v4())
                ipv4.push_back(endpoint_iterator->endpoint());
            if (endpoint_iterator->endpoint().address().is_v6())
                ipv6.push_back(endpoint_iterator->endpoint());
        }

        if (ipv4.empty() && ipv6.empty())
        {
            handle_error(
                key, boost::system::error_code(boost::system::errc::bad_address, boost::system::generic_category()));
            return;
        }

        BOOST_FOREACH (boost::asio::ip::tcp::endpoint& e, ipv4)
        {
            candidate_endpoints->push_back(e);
        }

        std::vector<boost::asio::ip::address> local_ip;

        TcpTransport::GetLocalAdapterIPAddresses(local_ip);

        std::vector<uint32_t> scopeids;
        BOOST_FOREACH (boost::asio::ip::address& ee, local_ip)
        {
            if (ee.is_v6())
            {
                boost::asio::ip::address_v6 a6 = ee.to_v6();
                if (a6.is_link_local())
                {
                    if (std::find(scopeids.begin(), scopeids.end(), a6.scope_id()) == scopeids.end())
                    {
                        scopeids.push_back(a6.scope_id());
                    }
                }
            }
        }

        BOOST_FOREACH (boost::asio::ip::tcp::endpoint& e, ipv6)
        {

            std::vector<boost::asio::ip::tcp::endpoint> ipv62;

            if (!e.address().is_v6())
                continue;

            boost::asio::ip::address_v6 addr = e.address().to_v6();
            uint16_t port = e.port();

            if (!addr.is_link_local() || (addr.is_link_local() && addr.scope_id() != 0))
            {
                ipv62.push_back(e);
            }
            else
            {
                // Link local address with no scope id, we need to try them all...

                BOOST_FOREACH (uint32_t e3, scopeids)
                {
                    boost::asio::ip::address_v6 addr3 = addr;
                    addr3.scope_id(e3);
                    ipv62.push_back(boost::asio::ip::tcp::endpoint(addr3, port));
                }
            }

            BOOST_FOREACH (boost::asio::ip::tcp::endpoint& e2, ipv62)
            {
                candidate_endpoints->push_back(e2);
            }
        }

        ROBOTRACONTEUR_LOG_TRACE_COMPONENT(
            node, Transport, endpoint,
            "Found candidate endpoints: " << TcpConnector_log_candidate_endpoints(candidate_endpoints));

        boost::system::error_code ec;
        connect3(candidate_endpoints, key, ec);
    }
    catch (std::exception& exp)
    {
        ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, endpoint,
                                           "Error resolving candidate endpoints: " << exp.what());
        handle_error(key, boost::system::error_code(boost::system::errc::io_error, boost::system::generic_category()));
        return;
    }

    connect4();
}

void TcpConnector::connect3(const RR_SHARED_PTR<std::list<boost::asio::ip::tcp::endpoint> >& candidate_endpoints,
                            int32_t key, const boost::system::error_code& e)
{
    RR_UNUSED(e);
    int32_t key2 = 0;
    try
    {
        boost::mutex::scoped_lock lock(this_lock);

        // This should never happen!
        if (candidate_endpoints->empty())
        {
            connect4();
            return;
        }

        active_count++;
        key2 = active_count;
        active.push_back(key2);

        boost::asio::ip::tcp::endpoint ep = candidate_endpoints->front();
        candidate_endpoints->pop_front();

        RR_SHARED_PTR<boost::asio::ip::tcp::socket> sock(
            new boost::asio::ip::tcp::socket(parent->GetNode()->GetThreadPool()->get_io_context()));

        {
            if (!connecting)
                return;

            RR_SHARED_PTR<boost::signals2::scoped_connection> sock_closer =
                RR_MAKE_SHARED<boost::signals2::scoped_connection>(parent->AddCloseListener(
                    sock, boost::bind(&boost::asio::ip::tcp::socket::close, RR_BOOST_PLACEHOLDERS(_1))));

            RobotRaconteurNode::asio_async_connect(node, sock, ep,
                                                   boost::bind(&TcpConnector::connected_callback, shared_from_this(),
                                                               sock, sock_closer, key2,
                                                               boost::asio::placeholders::error));

            ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, endpoint, "TCP begin connect to endpoint: " << ep);
        }
    }
    catch (std::exception&)
    {
        handle_error(key2, boost::system::error_code(boost::system::errc::io_error, boost::system::generic_category()));
        return;
    }

    if (!candidate_endpoints->empty())
    {
        backoff_timer->expires_from_now(boost::posix_time::milliseconds(5));
        RobotRaconteurNode::asio_async_wait(node, backoff_timer,
                                            boost::bind(&TcpConnector::connect3, shared_from_this(),
                                                        candidate_endpoints, key, boost::asio::placeholders::error));
        return;
    }

    {
        boost::mutex::scoped_lock lock(this_lock);
        active.remove(key);
    }

    connect4();
}

void TcpConnector::connect4()
{
    bool all_stopped = false;
    {
        boost::mutex::scoped_lock lock(this_lock);
        all_stopped = active.empty();
    }

    if (all_stopped)
    {
        boost::mutex::scoped_lock lock(this_lock);
        if (!connecting)
            return;
        connecting = false;
    }

    if (all_stopped)
    {
        if (errors.empty())
        {
            ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, endpoint, "Could not connect to remote node");
            callback(RR_SHARED_PTR<TcpTransportConnection>(),
                     RR_MAKE_SHARED<ConnectionException>("Could not connect to remote node"));
            return;
        }

        BOOST_FOREACH (RR_SHARED_PTR<RobotRaconteurException>& e, errors)
        {
            RR_SHARED_PTR<NodeNotFoundException> e2 = RR_DYNAMIC_POINTER_CAST<NodeNotFoundException>(e);
            if (e2)
            {
                ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, endpoint,
                                                   "Error connecting to remote host:" << e2->what());
                callback(RR_SHARED_PTR<TcpTransportConnection>(), e2);
                return;
            }
        }

        BOOST_FOREACH (RR_SHARED_PTR<RobotRaconteurException>& e, errors)
        {
            RR_SHARED_PTR<AuthenticationException> e2 = RR_DYNAMIC_POINTER_CAST<AuthenticationException>(e);
            if (e2)
            {
                ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, endpoint,
                                                   "Error connecting to remote host:" << e2->what());
                callback(RR_SHARED_PTR<TcpTransportConnection>(), e2);
                return;
            }
        }

        ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, endpoint,
                                           "Error connecting to remote host:" << errors.back()->what());
        callback(RR_SHARED_PTR<TcpTransportConnection>(), errors.back());
    }
}

void TcpConnector::connected_callback(const RR_SHARED_PTR<boost::asio::ip::tcp::socket>& socket,
                                      const RR_SHARED_PTR<boost::signals2::scoped_connection>& socket_closer,
                                      int32_t key, const boost::system::error_code& error)
{
    RR_UNUSED(socket_closer);
    if (error)
    {
        ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, endpoint,
                                           "Candidate endpoint " << TcpTransport_socket_remote_endpoint(socket)
                                                                 << " connection error: " << error.message());
        handle_error(key, error);
        return;
    }
    try
    {
        boost::mutex::scoped_lock lock(this_lock);
        bool c = connecting;

        if (!c)
        {
            /*std:: cout << "closing" << std::endl;
            try
            {
            socket->set_option(boost::asio::ip::tcp::socket::linger(true,5));
            }
            catch (...) {}
            try
            {
            socket->shutdown(boost::asio::ip::tcp::socket::shutdown_both);
            }
            catch(...) {}
            try
            {
            socket->close();
            }
            catch(...) {}*/

            return;
        }

        int32_t key2 = 0;
        {
            active_count++;
            key2 = active_count;
            socket_connected = true;

            ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, endpoint,
                                               "Candidate endpoint " << TcpTransport_socket_remote_endpoint(socket)
                                                                     << " connected!");

            socket->set_option(boost::asio::ip::tcp::socket::linger(true, 5));
            boost::function<void(const RR_SHARED_PTR<boost::asio::ip::tcp::socket>&,
                                 const RR_SHARED_PTR<ITransportConnection>&,
                                 const RR_SHARED_PTR<RobotRaconteurException>&)>
                cb = boost::bind(&TcpConnector::connected_callback2, shared_from_this(), RR_BOOST_PLACEHOLDERS(_1),
                                 key2, RR_BOOST_PLACEHOLDERS(_2), RR_BOOST_PLACEHOLDERS(_3));
            TcpTransport_attach_transport(parent, socket, url, false, endpoint, cb);

            active.push_back(key2);
        }

        active.remove(key);
    }
    catch (std::exception& exp)
    {
        ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, endpoint,
                                           "Candidate endpoint " << TcpTransport_socket_remote_endpoint(socket)
                                                                 << " connection error: " << exp.what());
        handle_error(key, boost::system::error_code(boost::system::errc::io_error, boost::system::generic_category()));
    }
}

void TcpConnector::connected_callback2(const RR_SHARED_PTR<boost::asio::ip::tcp::socket>& socket, int32_t key,
                                       const RR_SHARED_PTR<ITransportConnection>& connection,
                                       const RR_SHARED_PTR<RobotRaconteurException>& err)
{
    if (err)
    {
        if (connection)
        {
            try
            {
                ROBOTRACONTEUR_LOG_TRACE_COMPONENT(
                    node, Transport, endpoint,
                    "Closing losing candidate connection: " << TcpTransport_socket_remote_endpoint(socket));
                connection->Close();
            }
            catch (std::exception&)
            {}
        }
        // callback(RR_SHARED_PTR<TcpTransportConnection>(),err);
        ROBOTRACONTEUR_LOG_TRACE_COMPONENT(
            node, Transport, endpoint,
            "Error attaching transport to socket: " << TcpTransport_socket_remote_endpoint(socket));
        handle_error(key, err);
        return;
    }
    try
    {
        {

            bool c = false;
            {
                boost::mutex::scoped_lock lock(this_lock);
                c = connecting;
                connecting = false;
            }
            if (!c)
            {

                try
                {
                    // std::cout << "Closing 2" << std::endl;
                    connection->Close();
                }
                catch (std::exception&)
                {}

                return;
            }
        }
        parent->register_transport(connection);

        {
            boost::mutex::scoped_lock lock(this_lock);
            if (connect_timer)
                connect_timer->cancel();
            connect_timer.reset();
        }

        try
        {

            // std::cout << "connect callback" << std::endl;
            ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, endpoint,
                                               "TcpConnector connected transport to "
                                                   << TcpTransport_socket_remote_endpoint(socket));
            ROBOTRACONTEUR_LOG_INFO_COMPONENT(node, Transport, endpoint,
                                              "TcpTransport connected transport to "
                                                  << TcpTransport_socket_remote_endpoint(socket) << " from "
                                                  << TcpTransport_socket_local_endpoint(socket));
            callback(boost::dynamic_pointer_cast<ITransportConnection>(connection),
                     RR_SHARED_PTR<RobotRaconteurException>());
        }
        catch (std::exception& exp)
        {
            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, endpoint,
                                               "Error in transport connect handler: " << exp.what());
            RobotRaconteurNode::TryHandleException(node, &exp);
        }
    }
    catch (std::exception& exp2)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, endpoint,
                                           "Error finalizing transport connection " << exp2.what());
        handle_error(key, boost::system::error_code(boost::system::errc::io_error, boost::system::generic_category()));
    }
}

void TcpConnector::connect_timer_callback(const boost::system::error_code& e)
{

    if (e != boost::asio::error::operation_aborted)
    {
        {
            boost::mutex::scoped_lock lock(this_lock);
            if (!connecting)
                return;
            connecting = false;
        }
        try
        {
            ROBOTRACONTEUR_LOG_INFO_COMPONENT(node, Transport, endpoint, "TcpTransport connect operation timed out");
            callback(RR_SHARED_PTR<TcpTransportConnection>(),
                     RR_MAKE_SHARED<ConnectionException>("Connection timed out"));
        }
        catch (std::exception& exp)
        {
            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, endpoint,
                                               "Error in transport connect handler: " << exp.what());
            RobotRaconteurNode::TryHandleException(node, &exp);
        }
    }
}

void TcpConnector::handle_error(const int32_t& key, const boost::system::error_code& err)
{
    handle_error(key, RR_MAKE_SHARED<ConnectionException>(err.message()));
}

void TcpConnector::handle_error(const int32_t& key, const RR_SHARED_PTR<RobotRaconteurException>& err)
{
    bool s = false;
    bool c = false;
    {
        boost::mutex::scoped_lock lock(this_lock);
        if (!connecting)
            return;

        active.remove(key);
        errors.push_back(err);

        if (!active.empty())
            return;
        s = socket_connected;
        if (!active.empty())
            return;

        // return;
        // All activities have completed, assume failure

        c = connecting;
        connecting = false;

        if (!c)
            return;

        connect_timer.reset();
    }

    BOOST_FOREACH (const RR_SHARED_PTR<RobotRaconteurException>& e, errors)
    {
        RR_SHARED_PTR<NodeNotFoundException> e2 = RR_DYNAMIC_POINTER_CAST<NodeNotFoundException>(e);
        if (e2)
        {
            ROBOTRACONTEUR_LOG_INFO_COMPONENT(node, Transport, endpoint,
                                              "TcpTransport failed to connect: " << e2->what());
            callback(RR_SHARED_PTR<TcpTransportConnection>(), e2);
            return;
        }
    }

    BOOST_FOREACH (const RR_SHARED_PTR<RobotRaconteurException>& e, errors)
    {
        RR_SHARED_PTR<AuthenticationException> e2 = RR_DYNAMIC_POINTER_CAST<AuthenticationException>(e);
        if (e2)
        {
            ROBOTRACONTEUR_LOG_INFO_COMPONENT(node, Transport, endpoint,
                                              "TcpTransport failed to connect: " << e2->what());
            callback(RR_SHARED_PTR<TcpTransportConnection>(), e2);
            return;
        }
    }

    if (!s)
    {
        try
        {
            ROBOTRACONTEUR_LOG_INFO_COMPONENT(node, Transport, endpoint,
                                              "TcpTransport failed to connect: " << err->what());
            callback(RR_SHARED_PTR<TcpTransportConnection>(), err);
        }
        catch (std::exception& exp)
        {
            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, endpoint,
                                               "Error in transport connect handler: " << exp.what());
            RobotRaconteurNode::TryHandleException(node, &exp);
        }
    }
    else
    {
        try
        {
            ROBOTRACONTEUR_LOG_INFO_COMPONENT(node, Transport, endpoint,
                                              "TcpTransport failed to connect: " << err->what());
            callback(RR_SHARED_PTR<TcpTransportConnection>(),
                     RR_MAKE_SHARED<ConnectionException>("Could not connect to service"));
        }
        catch (std::exception& exp)
        {
            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, endpoint,
                                               "Error in transport connect handler: " << exp.what());
            RobotRaconteurNode::TryHandleException(node, &exp);
        }
    }
}

// class TcpAcceptor

void TcpAcceptor::AcceptSocket6(const RR_SHARED_PTR<RobotRaconteurException>& err,
                                const RR_SHARED_PTR<boost::asio::ip::tcp::socket>& socket,
                                const RR_SHARED_PTR<TcpTransportConnection>& t,
                                const boost::function<void(const RR_SHARED_PTR<boost::asio::ip::tcp::socket>&,
                                                           const RR_SHARED_PTR<ITransportConnection>&,
                                                           const RR_SHARED_PTR<RobotRaconteurException>&)>& handler)
{
    if (err)
    {
        ROBOTRACONTEUR_LOG_INFO_COMPONENT(node, Transport, 0,
                                          "TcpTransport accepted socket stream attach failed "
                                              << TcpTransport_socket_remote_endpoint(socket) << " to "
                                              << TcpTransport_socket_local_endpoint(socket)
                                              << " with error: " << err->what());
        handler(RR_SHARED_PTR<boost::asio::ip::tcp::socket>(), RR_SHARED_PTR<ITransportConnection>(), err);
        return;
    }

    ROBOTRACONTEUR_LOG_INFO_COMPONENT(node, Transport, 0,
                                      "TcpTransport accepted incoming transport connection "
                                          << TcpTransport_socket_remote_endpoint(socket) << " to "
                                          << TcpTransport_socket_local_endpoint(socket));
    handler(socket, t, err);
}

void TcpAcceptor::AcceptSocket5(const boost::system::error_code& ec,
                                const RR_SHARED_PTR<boost::asio::ip::tcp::socket>& socket,
                                const RR_SHARED_PTR<websocket_stream<boost::asio::ip::tcp::socket&> >& websocket,
                                const RR_SHARED_PTR<boost::signals2::scoped_connection>& socket_closer,
                                const boost::function<void(const RR_SHARED_PTR<boost::asio::ip::tcp::socket>&,
                                                           const RR_SHARED_PTR<ITransportConnection>&,
                                                           const RR_SHARED_PTR<RobotRaconteurException>&)>& handler)
{
    RR_UNUSED(socket_closer);
    if (ec)
    {
        ROBOTRACONTEUR_LOG_INFO_COMPONENT(node, Transport, 0,
                                          "TcpTransport accepted socket closed "
                                              << TcpTransport_socket_remote_endpoint(socket) << " to "
                                              << TcpTransport_socket_local_endpoint(socket)
                                              << " with error: " << ec.message());
        RR_SHARED_PTR<ConnectionException> err = RR_MAKE_SHARED<ConnectionException>("Connection closed");
        handler(RR_SHARED_PTR<boost::asio::ip::tcp::socket>(), RR_SHARED_PTR<ITransportConnection>(), err);
        return;
    }

    try
    {
        ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, 0,
                                           "TcpTransport accepted socket attaching websocket protocol "
                                               << TcpTransport_socket_remote_endpoint(socket) << " to "
                                               << TcpTransport_socket_local_endpoint(socket));
        RR_SHARED_PTR<TcpTransportConnection> t =
            RR_MAKE_SHARED<TcpTransportConnection>(parent, url, true, local_endpoint);
        boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>&)> h =
            boost::bind(handler, socket, t, RR_BOOST_PLACEHOLDERS(_1));
        t->AsyncAttachWebSocket(socket, websocket, h);
        parent->AddCloseListener(t, &TcpTransportConnection::Close);
    }
    catch (std::exception&)
    {
        RobotRaconteurNode::TryPostToThreadPool(
            parent->GetNode(),
            boost::bind(handler, RR_SHARED_PTR<boost::asio::ip::tcp::socket>(), RR_SHARED_PTR<TcpTransportConnection>(),
                        RR_MAKE_SHARED<ConnectionException>("Could not connect to service")),
            true);
    }
}

void TcpAcceptor::AcceptSocket4(const RR_SHARED_PTR<std::string>& dat,
                                const RR_SHARED_PTR<boost::asio::ip::tcp::socket>& socket,
                                const RR_SHARED_PTR<boost::signals2::scoped_connection>& socket_closer)
{}

void TcpAcceptor::AcceptSocket3(const boost::system::error_code& ec,
                                const RR_SHARED_PTR<boost::asio::deadline_timer>& timer,
                                const RR_SHARED_PTR<boost::asio::ip::tcp::socket>& socket,
                                const RR_SHARED_PTR<boost::signals2::scoped_connection>& socket_closer,
                                const boost::function<void(const RR_SHARED_PTR<boost::asio::ip::tcp::socket>&,
                                                           const RR_SHARED_PTR<ITransportConnection>&,
                                                           const RR_SHARED_PTR<RobotRaconteurException>&)>& handler)
{
    RR_UNUSED(ec);
    RR_UNUSED(timer);
    socket->async_receive(boost::asio::buffer(buf, 4), boost::asio::ip::tcp::socket::message_peek,
                          boost::bind(&TcpAcceptor::AcceptSocket2, shared_from_this(), boost::asio::placeholders::error,
                                      boost::asio::placeholders::bytes_transferred, socket, socket_closer, handler));
}

void TcpAcceptor::AcceptSocket2(const boost::system::error_code& ec, size_t n,
                                const RR_SHARED_PTR<boost::asio::ip::tcp::socket>& socket,
                                const RR_SHARED_PTR<boost::signals2::scoped_connection>& socket_closer,
                                const boost::function<void(const RR_SHARED_PTR<boost::asio::ip::tcp::socket>&,
                                                           const RR_SHARED_PTR<ITransportConnection>&,
                                                           const RR_SHARED_PTR<RobotRaconteurException>&)>& handler)
{

    if (ec)
    {
        ROBOTRACONTEUR_LOG_INFO_COMPONENT(node, Transport, 0,
                                          "TcpTransport accepted socket closed "
                                              << TcpTransport_socket_remote_endpoint(socket) << " to "
                                              << TcpTransport_socket_local_endpoint(socket)
                                              << " with error: " << ec.message());
        RR_SHARED_PTR<ConnectionException> err = RR_MAKE_SHARED<ConnectionException>("Connection closed");
        handler(RR_SHARED_PTR<boost::asio::ip::tcp::socket>(), RR_SHARED_PTR<ITransportConnection>(), err);
        return;
    }

    if (n < 4)
    {
        boost::posix_time::time_duration diff = parent->GetNode()->NowNodeTime() - start_time;
        if (diff.total_milliseconds() > 5000)
        {
            ROBOTRACONTEUR_LOG_INFO_COMPONENT(node, Transport, 0,
                                              "TcpTransport accepted socket timed out "
                                                  << TcpTransport_socket_remote_endpoint(socket) << " to "
                                                  << TcpTransport_socket_local_endpoint(socket));
            RR_SHARED_PTR<ConnectionException> err = RR_MAKE_SHARED<ConnectionException>("Connection timed out");
            handler(RR_SHARED_PTR<boost::asio::ip::tcp::socket>(), RR_SHARED_PTR<ITransportConnection>(), err);
            return;
        }

        RR_SHARED_PTR<boost::asio::deadline_timer> timer(
            new boost::asio::deadline_timer(parent->GetNode()->GetThreadPool()->get_io_context()));
        timer->expires_from_now(boost::posix_time::milliseconds(10));
        RobotRaconteurNode::asio_async_wait(node, timer,
                                            boost::bind(&TcpAcceptor::AcceptSocket3, shared_from_this(),
                                                        boost::asio::placeholders::error, timer, socket, socket_closer,
                                                        handler));
        return;
    }

    std::string magic(buf.data(), 4);

    if (magic == "RRAC")
    {
        try
        {
            ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, 0,
                                               "TcpTransport accepted socket attaching raw protocol "
                                                   << TcpTransport_socket_remote_endpoint(socket) << " to "
                                                   << TcpTransport_socket_local_endpoint(socket));
            RR_SHARED_PTR<TcpTransportConnection> t =
                RR_MAKE_SHARED<TcpTransportConnection>(parent, url, true, local_endpoint);
            boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>&)> h = boost::bind(
                &TcpAcceptor::AcceptSocket6, shared_from_this(), RR_BOOST_PLACEHOLDERS(_1), socket, t, handler);
            t->AsyncAttachSocket(socket, h);
            parent->AddCloseListener(t, &TcpTransportConnection::Close);
        }
        catch (std::exception&)
        {
            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, 0,
                                               "TcpTransport accepted socket attach stream failed "
                                                   << TcpTransport_socket_remote_endpoint(socket) << " to "
                                                   << TcpTransport_socket_local_endpoint(socket));
            RobotRaconteurNode::TryPostToThreadPool(
                parent->GetNode(),
                boost::bind(handler, RR_SHARED_PTR<boost::asio::ip::tcp::socket>(),
                            RR_SHARED_PTR<TcpTransportConnection>(),
                            RR_MAKE_SHARED<ConnectionException>("Could not connect to service")),
                true);
        }
    }
    else if (parent->GetAcceptWebSockets())
    {
        if (magic == "GET " || magic == "GET\t")
        {
            ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, 0,
                                               "TcpTransport accepted socket begin websocket handshake "
                                                   << TcpTransport_socket_remote_endpoint(socket) << " to "
                                                   << TcpTransport_socket_local_endpoint(socket));
            RR_SHARED_PTR<websocket_stream<boost::asio::ip::tcp::socket&> > websocket =
                RR_MAKE_SHARED<websocket_stream<boost::asio::ip::tcp::socket&> >(boost::ref(*socket));
            boost::function<void(boost::string_ref protocol, const boost::system::error_code& ec)> h =
                boost::bind(&TcpAcceptor::AcceptSocket5, shared_from_this(), RR_BOOST_PLACEHOLDERS(_2), socket,
                            websocket, socket_closer, handler);
            websocket->async_server_handshake("robotraconteur.robotraconteur.com", parent->GetWebSocketAllowedOrigins(),
                                              h);
            return;
        }
        else
        {
            ROBOTRACONTEUR_LOG_INFO_COMPONENT(node, Transport, 0,
                                              "TcpTransport accepted socket invalid HTTP websocket header "
                                                  << TcpTransport_socket_remote_endpoint(socket) << " to "
                                                  << TcpTransport_socket_local_endpoint(socket));
            std::string response2_1 = "HTTP/1.1 404 File Not Found\r\n";
            RR_SHARED_PTR<std::string> response2 = RR_MAKE_SHARED<std::string>(response2_1);
            BOOST_AUTO(response2_buf, boost::asio::buffer(response2->c_str(), response2->size()));
            RobotRaconteurNode::asio_async_write_some(
                node, socket, response2_buf,
                boost::bind(&TcpAcceptor::AcceptSocket4, response2, socket, socket_closer));

            RR_SHARED_PTR<ConnectionException> err = RR_MAKE_SHARED<ConnectionException>("Invalid protocol");
            handler(RR_SHARED_PTR<boost::asio::ip::tcp::socket>(), RR_SHARED_PTR<ITransportConnection>(), err);
            return;
        }
    }
    else
    {
        ROBOTRACONTEUR_LOG_INFO_COMPONENT(node, Transport, 0,
                                          "TcpTransport accepted socket invalid protocol "
                                              << TcpTransport_socket_remote_endpoint(socket) << " to "
                                              << TcpTransport_socket_local_endpoint(socket));
        RR_SHARED_PTR<ConnectionException> err = RR_MAKE_SHARED<ConnectionException>("Invalid protocol");
        handler(RR_SHARED_PTR<boost::asio::ip::tcp::socket>(), RR_SHARED_PTR<ITransportConnection>(), err);
    }
}

TcpAcceptor::TcpAcceptor(const RR_SHARED_PTR<TcpTransport>& parent, boost::string_ref url, uint32_t local_endpoint)
    RR_MEMBER_ARRAY_INIT2(buf)
{
    this->parent = parent;
    this->url = RR_MOVE(url.to_string());
    this->local_endpoint = local_endpoint;
    this->node = parent->GetNode();
}

void TcpAcceptor::AcceptSocket(
    const RR_SHARED_PTR<boost::asio::ip::tcp::socket>& socket,
    boost::function<void(const RR_SHARED_PTR<boost::asio::ip::tcp::socket>&, const RR_SHARED_PTR<ITransportConnection>&,
                         const RR_SHARED_PTR<RobotRaconteurException>&)>
        handler)
{
    ROBOTRACONTEUR_LOG_INFO_COMPONENT(node, Transport, 0,
                                      "TcpTransport accepted socket from "
                                          << TcpTransport_socket_remote_endpoint(socket) << " to "
                                          << TcpTransport_socket_local_endpoint(socket));
    start_time = parent->GetNode()->NowNodeTime();

    RR_SHARED_PTR<boost::signals2::scoped_connection> socket_closer =
        RR_MAKE_SHARED<boost::signals2::scoped_connection>(parent->AddCloseListener(
            socket, boost::bind(&boost::asio::ip::tcp::socket::close, RR_BOOST_PLACEHOLDERS(_1))));

    socket->async_receive(boost::asio::buffer(buf, 4), boost::asio::ip::tcp::socket::message_peek,
                          boost::bind(&TcpAcceptor::AcceptSocket2, shared_from_this(), boost::asio::placeholders::error,
                                      boost::asio::placeholders::bytes_transferred, socket, socket_closer,
                                      RR_MOVE(handler)));
}

// end class TcpAcceptor

// class TcpWebSocketConnected

void TcpWebSocketConnector::Connect4(
    const RR_SHARED_PTR<RobotRaconteurException>& err, const RR_SHARED_PTR<ITransportConnection>& connection,
    const RR_SHARED_PTR<boost::asio::ip::tcp::socket>& socket,
    const RR_SHARED_PTR<websocket_stream<boost::asio::ip::tcp::socket&> >& websocket,
    const boost::function<void(const RR_SHARED_PTR<ITransportConnection>&,
                               const RR_SHARED_PTR<RobotRaconteurException>&)>& handler)
{
    RR_UNUSED(websocket);
    if (err)
    {
        try
        {
            if (connection)
            {
                connection->Close();
            }
        }
        catch (std::exception&)
        {}

        ROBOTRACONTEUR_LOG_INFO_COMPONENT(node, Transport, endpoint,
                                          "TcpTransport websocket stream attach failed: " << err->what());
        handler(RR_SHARED_PTR<ITransportConnection>(), err);
        return;
    }

    parent->register_transport(connection);

    ROBOTRACONTEUR_LOG_INFO_COMPONENT(node, Transport, endpoint,
                                      "TcpTransport connected websocket transport to "
                                          << TcpTransport_socket_remote_endpoint(socket) << " from "
                                          << TcpTransport_socket_local_endpoint(socket));
    handler(connection, RR_SHARED_PTR<RobotRaconteurException>());
}

void TcpWebSocketConnector::Connect3(
    const boost::system::error_code& ec, const RR_SHARED_PTR<boost::asio::ip::tcp::socket>& socket,
    const RR_SHARED_PTR<websocket_stream<boost::asio::ip::tcp::socket&> >& websocket,
    const RR_SHARED_PTR<boost::signals2::scoped_connection>& socket_closer,
    const boost::function<void(const RR_SHARED_PTR<ITransportConnection>&,
                               const RR_SHARED_PTR<RobotRaconteurException>&)>& handler)
{
    RR_UNUSED(socket_closer);
    if (ec)
    {
        ROBOTRACONTEUR_LOG_INFO_COMPONENT(node, Transport, endpoint, "TcpTransport websocket handshake failed: " << ec);
        RR_SHARED_PTR<ConnectionException> err =
            RR_MAKE_SHARED<ConnectionException>("Could not connect to remote websocket");
        handler(RR_SHARED_PTR<ITransportConnection>(), err);
        return;
    }

    socket->set_option(boost::asio::ip::tcp::socket::linger(true, 5));

    try
    {
        ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, endpoint, "Websocket connected, begin attach transport");
        RR_SHARED_PTR<TcpTransportConnection> t = RR_MAKE_SHARED<TcpTransportConnection>(parent, url, false, endpoint);
        boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>&)> h =
            boost::bind(&TcpWebSocketConnector::Connect4, shared_from_this(), RR_BOOST_PLACEHOLDERS(_1), t, socket,
                        websocket, boost::protect(handler));
        t->AsyncAttachWebSocket(socket, websocket, h);
        parent->AddCloseListener(t, &TcpTransportConnection::Close);
    }
    catch (std::exception& exp)
    {
        ROBOTRACONTEUR_LOG_INFO_COMPONENT(node, Transport, endpoint,
                                          "TcpTransport websocket could not connect to supplied URL: " << exp.what());
        handler(RR_SHARED_PTR<TcpTransportConnection>(),
                RR_MAKE_SHARED<ConnectionException>("Could not connect to service"));
    }
}

void TcpWebSocketConnector::Connect2(
    const boost::system::error_code& ec, const RR_SHARED_PTR<boost::asio::ip::tcp::socket>& socket,
    const RR_SHARED_PTR<websocket_tcp_connector>& socket_connector,
    const boost::function<void(const RR_SHARED_PTR<ITransportConnection>&,
                               const RR_SHARED_PTR<RobotRaconteurException>&)>& handler)
{
    RR_UNUSED(socket_connector);
    if (ec)
    {
        ROBOTRACONTEUR_LOG_INFO_COMPONENT(node, Transport, endpoint,
                                          "TcpTransport websocket could not connect to supplied URL: " << ec);
        RR_SHARED_PTR<ConnectionException> err = RR_MAKE_SHARED<ConnectionException>("Could not contact remote host");
        handler(RR_SHARED_PTR<ITransportConnection>(), err);
        return;
    }

    try
    {
        RR_SHARED_PTR<websocket_stream<boost::asio::ip::tcp::socket&> > websocket =
            RR_MAKE_SHARED<websocket_stream<boost::asio::ip::tcp::socket&> >(boost::ref(*socket));

        RR_SHARED_PTR<boost::signals2::scoped_connection> websocket_closer =
            RR_MAKE_SHARED<boost::signals2::scoped_connection>(
                parent->AddCloseListener(websocket, &websocket_stream<boost::asio::ip::tcp::socket&>::close));

        ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, endpoint,
                                           "Websocket tcp socket connected, begin handshake");
        websocket->async_client_handshake(ws_url, "robotraconteur.robotraconteur.com",
                                          boost::bind(&TcpWebSocketConnector::Connect3, shared_from_this(),
                                                      RR_BOOST_PLACEHOLDERS(_1), socket, websocket, websocket_closer,
                                                      boost::protect(handler)));
    }
    catch (std::exception& exp)
    {
        ROBOTRACONTEUR_LOG_INFO_COMPONENT(node, Transport, endpoint,
                                          "TcpTransport websocket could not connect to supplied URL: " << exp.what());
        RR_SHARED_PTR<ConnectionException> err = RR_MAKE_SHARED<ConnectionException>("Could not contact remote host");
        handler(RR_SHARED_PTR<ITransportConnection>(), err);
        return;
    }
}

TcpWebSocketConnector::TcpWebSocketConnector(const RR_SHARED_PTR<TcpTransport>& parent)
{
    this->parent = parent;
    this->node = parent->GetNode();
    endpoint = 0;
}

void TcpWebSocketConnector::Connect(
    boost::string_ref url, uint32_t endpoint,
    boost::function<void(const RR_SHARED_PTR<ITransportConnection>&, const RR_SHARED_PTR<RobotRaconteurException>&)>
        handler)
{

    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, endpoint,
                                       "TcpTransport begin websocket connection with URL: " << url);

    this->url = RR_MOVE(url.to_string());
    this->endpoint = endpoint;

    try
    {
        ParseConnectionURLResult url_res = ParseConnectionURL(url);

        if (url_res.scheme != "rr+ws" && url_res.scheme != "rrs+ws")
            throw InvalidArgumentException("Invalid transport type for TcpTransport");

        // if (url_result[1]!="tcp") throw InvalidArgumentException("Invalid transport type for TcpTransport");

        std::string host = url_res.host;
        std::string port = boost::lexical_cast<std::string>(url_res.port);

        std::string path = url_res.path;

        if (url_res.scheme == "rr+ws")
        {
            ws_url = boost::replace_first_copy(this->url, "rr+ws", "ws");
        }

        if (url_res.scheme == "rrs+ws")
        {
            ws_url = boost::replace_first_copy(this->url, "rrs+ws", "ws");
        }
        if (url_res.host.empty())
            throw ConnectionException("Invalid host for websocket transport");
        // std::cout << ws_url << std::endl;

        RR_SHARED_PTR<detail::websocket_tcp_connector> socket_connector =
            RR_MAKE_SHARED<detail::websocket_tcp_connector>(
                boost::ref(parent->GetNode()->GetThreadPool()->get_io_context()));

        socket_connector->connect(ws_url, boost::bind(&TcpWebSocketConnector::Connect2, shared_from_this(),
                                                      RR_BOOST_PLACEHOLDERS(_1), RR_BOOST_PLACEHOLDERS(_2),
                                                      socket_connector, boost::protect(RR_MOVE(handler))));
        parent->AddCloseListener(socket_connector, &detail::websocket_tcp_connector::cancel);
    }
    catch (std::exception& exp)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, endpoint,
                                           "invalid websocket URL: " << url << " " << exp.what());
        throw ConnectionException("Invalid URL for websocket connection");
    }
}

// end TcpWebSocketConnector

} // namespace detail

namespace detail
{

static void TcpTransportConnection_socket_read_adapter(
    RR_WEAK_PTR<RobotRaconteurNode> node, RR_WEAK_PTR<boost::asio::ip::tcp::socket> sock, mutable_buffers& b,
    const boost::function<void(const boost::system::error_code&, size_t)>& handler)
{
    RR_SHARED_PTR<boost::asio::ip::tcp::socket> sock1 = sock.lock();
    if (!sock1)
        return;
    RobotRaconteurNode::asio_async_read_some(node, sock1, b, handler);
}

static void TcpTransportConnection_socket_write_adapter(
    RR_WEAK_PTR<RobotRaconteurNode> node, RR_WEAK_PTR<boost::asio::ip::tcp::socket> sock, const_buffers& b,
    const boost::function<void(const boost::system::error_code&, size_t)>& handler)
{
    RR_SHARED_PTR<boost::asio::ip::tcp::socket> sock1 = sock.lock();
    if (!sock1)
        return;
    RobotRaconteurNode::asio_async_write_some(node, sock1, b, handler);
}

static void TcpTransportConnection_socket_close_adapter(RR_WEAK_PTR<boost::asio::ip::tcp::socket> sock)
{
    RR_SHARED_PTR<boost::asio::ip::tcp::socket> sock1 = sock.lock();
    if (!sock1)
        return;
    sock1->close();
}

#ifdef ROBOTRACONTEUR_USE_SCHANNEL

void TcpWSSWebSocketConnector::Connect4(
    const RR_SHARED_PTR<RobotRaconteurException>& err, const RR_SHARED_PTR<ITransportConnection>& connection,
    const RR_SHARED_PTR<boost::asio::ip::tcp::socket>& socket,
    const RR_SHARED_PTR<TlsSchannelAsyncStreamAdapter>& tls_stream,
    RR_SHARED_PTR<websocket_stream<TlsSchannelAsyncStreamAdapter_ASIO_adapter&> > websocket,
    const boost::function<void(const RR_SHARED_PTR<ITransportConnection>&,
                               const RR_SHARED_PTR<RobotRaconteurException>&)>& handler)
{
    if (err)
    {
        try
        {
            if (connection)
            {
                connection->Close();
            }
        }
        catch (std::exception&)
        {}

        ROBOTRACONTEUR_LOG_INFO_COMPONENT(node, Transport, endpoint,
                                          "TcpTransport wss stream attach failed: " << err->what());
        handler(RR_SHARED_PTR<ITransportConnection>(), err);
        return;
    }

    parent->register_transport(connection);

    ROBOTRACONTEUR_LOG_INFO_COMPONENT(node, Transport, endpoint,
                                      "TcpTransport connected wss transport to "
                                          << TcpTransport_socket_remote_endpoint(socket) << " from "
                                          << TcpTransport_socket_local_endpoint(socket));
    handler(connection, RR_SHARED_PTR<RobotRaconteurException>());
}

void TcpWSSWebSocketConnector::Connect3(
    const boost::system::error_code& ec, const RR_SHARED_PTR<boost::asio::ip::tcp::socket>& socket,
    const RR_SHARED_PTR<boost::signals2::scoped_connection>& socket_closer,
    const RR_SHARED_PTR<TlsSchannelAsyncStreamAdapter>& tls_stream,
    RR_SHARED_PTR<websocket_stream<TlsSchannelAsyncStreamAdapter_ASIO_adapter&> > websocket,
    const boost::function<void(const RR_SHARED_PTR<ITransportConnection>&,
                               const RR_SHARED_PTR<RobotRaconteurException>&)>& handler)
{

    if (ec)
    {
        ROBOTRACONTEUR_LOG_INFO_COMPONENT(node, Transport, endpoint,
                                          "TcpTransport wss websocket handshake failed: " << ec);
        RR_SHARED_PTR<ConnectionException> err =
            RR_MAKE_SHARED<ConnectionException>("Could not connect to remote websocket");
        handler(RR_SHARED_PTR<ITransportConnection>(), err);
        return;
    }

    socket->set_option(boost::asio::ip::tcp::socket::linger(true, 5));

    try
    {
        ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, endpoint, "wss connected, begin attach transport");
        RR_SHARED_PTR<TcpTransportConnection> t = RR_MAKE_SHARED<TcpTransportConnection>(parent, url, false, endpoint);
        boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>&)> h =
            boost::bind(&TcpWSSWebSocketConnector::Connect4, shared_from_this(), RR_BOOST_PLACEHOLDERS(_1), t, socket,
                        tls_stream, websocket, boost::protect(handler));
        t->AsyncAttachWSSWebSocket(socket, tls_stream, websocket, h);
        parent->AddCloseListener(t, &TcpTransportConnection::Close);
    }
    catch (std::exception& exp)
    {
        ROBOTRACONTEUR_LOG_INFO_COMPONENT(node, Transport, endpoint,
                                          "TcpTransport wss could not connect to supplied URL: " << exp.what());
        handler(RR_SHARED_PTR<TcpTransportConnection>(),
                RR_MAKE_SHARED<ConnectionException>("Could not connect to service"));
    }
}

void TcpWSSWebSocketConnector::Connect2_1(
    const boost::system::error_code& ec, const RR_SHARED_PTR<boost::asio::ip::tcp::socket>& socket,
    const RR_SHARED_PTR<boost::signals2::scoped_connection>& socket_closer,
    const RR_SHARED_PTR<TlsSchannelAsyncStreamAdapter>& tls_stream,
    const boost::function<void(const RR_SHARED_PTR<ITransportConnection>&,
                               const RR_SHARED_PTR<RobotRaconteurException>&)>& handler)
{
    if (ec)
    {
        ROBOTRACONTEUR_LOG_INFO_COMPONENT(node, Transport, endpoint,
                                          "TcpTransport wss HTTP TLS client handshake failed: " << ec.message());
        RR_SHARED_PTR<ConnectionException> err = RR_MAKE_SHARED<ConnectionException>("Could not contact remote host");
        handler(RR_SHARED_PTR<ITransportConnection>(), err);
        return;
    }

    if (!tls_stream->VerifyRemoteHostnameCertificate(servername))
    {
        ROBOTRACONTEUR_LOG_INFO_COMPONENT(
            node, Transport, endpoint, "TcpTransport wss could not verify server certificate \"" << servername << "\"");
        RR_SHARED_PTR<ConnectionException> err =
            RR_MAKE_SHARED<ConnectionException>("Could not verify server certificate");
        handler(RR_SHARED_PTR<ITransportConnection>(), err);
        return;
    }

    try
    {
        ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, endpoint, "wss tcp socket connected, begin handshake");
        RR_SHARED_PTR<websocket_stream<TlsSchannelAsyncStreamAdapter_ASIO_adapter&> > websocket =
            RR_MAKE_SHARED<websocket_stream<TlsSchannelAsyncStreamAdapter_ASIO_adapter&> >(
                boost::ref(tls_stream->get_asio_adapter()));
        websocket->async_client_handshake(ws_url, "robotraconteur.robotraconteur.com",
                                          boost::bind(&TcpWSSWebSocketConnector::Connect3, shared_from_this(),
                                                      RR_BOOST_PLACEHOLDERS(_1), socket, socket_closer, tls_stream,
                                                      websocket, boost::protect(handler)));
    }
    catch (std::exception& exp)
    {
        ROBOTRACONTEUR_LOG_INFO_COMPONENT(node, Transport, endpoint,
                                          "TcpTransport wss could not connect to supplied URL: " << exp.what());
        RR_SHARED_PTR<ConnectionException> err = RR_MAKE_SHARED<ConnectionException>("Could not contact remote host");
        handler(RR_SHARED_PTR<ITransportConnection>(), err);
        return;
    }
}

void TcpWSSWebSocketConnector::Connect2(
    const boost::system::error_code& ec, const RR_SHARED_PTR<boost::asio::ip::tcp::socket>& socket,
    const RR_SHARED_PTR<websocket_tcp_connector>& socket_connector,
    const boost::function<void(const RR_SHARED_PTR<ITransportConnection>&,
                               const RR_SHARED_PTR<RobotRaconteurException>&)>& handler)
{
    if (ec)
    {
        ROBOTRACONTEUR_LOG_INFO_COMPONENT(node, Transport, endpoint,
                                          "TcpTransport websocket could not connect to supplied URL: " << ec);
        RR_SHARED_PTR<ConnectionException> err = RR_MAKE_SHARED<ConnectionException>("Could not contact remote host");
        handler(RR_SHARED_PTR<ITransportConnection>(), err);
        return;
    }

    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, endpoint,
                                       "Websocket tcp socket connected, begin wss HTTP TLS handshake");
    try
    {

        RR_SHARED_PTR<boost::signals2::scoped_connection> socket_closer =
            RR_MAKE_SHARED<boost::signals2::scoped_connection>(parent->AddCloseListener(
                socket, boost::bind(&boost::asio::ip::tcp::socket::close, RR_BOOST_PLACEHOLDERS(_1))));

        RR_WEAK_PTR<boost::asio::ip::tcp::socket> socket1 = socket;
        RR_SHARED_PTR<TlsSchannelAsyncStreamAdapterContext> context =
            RR_MAKE_SHARED<TlsSchannelAsyncStreamAdapterContext>(parent->GetNode()->NodeID());
        RR_SHARED_PTR<RobotRaconteur::detail::TlsSchannelAsyncStreamAdapter> tls_stream =
            RR_MAKE_SHARED<RobotRaconteur::detail::TlsSchannelAsyncStreamAdapter>(
                boost::ref(parent->GetNode()->GetThreadPool()->get_io_context()), context,
                RobotRaconteur::detail::TlsSchannelAsyncStreamAdapter::client, servername,
                boost::bind(TcpTransportConnection_socket_read_adapter, node, socket1, RR_BOOST_PLACEHOLDERS(_1),
                            RR_BOOST_PLACEHOLDERS(_2)),
                boost::bind(TcpTransportConnection_socket_write_adapter, node, socket1, RR_BOOST_PLACEHOLDERS(_1),
                            RR_BOOST_PLACEHOLDERS(_2)),
                boost::bind(TcpTransportConnection_socket_close_adapter, socket1));

        RobotRaconteurNode::asio_async_handshake(node, tls_stream,
                                                 boost::bind(&TcpWSSWebSocketConnector::Connect2_1, shared_from_this(),
                                                             RR_BOOST_PLACEHOLDERS(_1), socket, socket_closer,
                                                             tls_stream, boost::protect(handler)));
    }
    catch (std::exception& exp)
    {
        ROBOTRACONTEUR_LOG_INFO_COMPONENT(node, Transport, endpoint,
                                          "TcpTransport wss HTTP TLS client handshake failed" << exp.what());
        RR_SHARED_PTR<ConnectionException> err = RR_MAKE_SHARED<ConnectionException>("Could not contact remote host");
        handler(RR_SHARED_PTR<ITransportConnection>(), err);
        return;
    }
}

TcpWSSWebSocketConnector::TcpWSSWebSocketConnector(const RR_SHARED_PTR<TcpTransport>& parent)
{
    this->parent = parent;
    this->node = parent->GetNode();
}

void TcpWSSWebSocketConnector::Connect(
    boost::string_ref url, uint32_t endpoint,
    boost::function<void(const RR_SHARED_PTR<ITransportConnection>&, const RR_SHARED_PTR<RobotRaconteurException>&)>
        handler)
{

    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, endpoint,
                                       "TcpTransport begin wss connection with URL: " << url);

    this->url = RR_MOVE(url.to_string());
    this->endpoint = endpoint;

    try
    {
        ParseConnectionURLResult url_res = ParseConnectionURL(url);

        if (url_res.scheme != "rr+wss" && url_res.scheme != "rrs+wss")
            throw InvalidArgumentException("Invalid transport type for TcpTransport");

        // if (url_result[1]!="tcp") throw InvalidArgumentException("Invalid transport type for TcpTransport");

        std::string host = url_res.host;
        servername = host;
        std::string port = boost::lexical_cast<std::string>(url_res.port);

        std::string path = url_res.path;

        if (url_res.scheme == "rr+wss")
        {
            ws_url = boost::replace_first_copy(this->url, "rr+wss", "wss");
        }

        if (url_res.scheme == "rrs+wss")
        {
            ws_url = boost::replace_first_copy(this->url, "rrs+wss", "wss");
        }
        if (url_res.host == "")
            throw ConnectionException("Invalid host for usb transport");
        // std::cout << ws_url << std::endl;

        RR_SHARED_PTR<detail::websocket_tcp_connector> socket_connector =
            RR_MAKE_SHARED<detail::websocket_tcp_connector>(
                boost::ref(parent->GetNode()->GetThreadPool()->get_io_context()));

        socket_connector->connect(ws_url, boost::bind(&TcpWSSWebSocketConnector::Connect2, shared_from_this(),
                                                      RR_BOOST_PLACEHOLDERS(_1), RR_BOOST_PLACEHOLDERS(_2),
                                                      socket_connector, boost::protect(handler)));
        parent->AddCloseListener(socket_connector, &detail::websocket_tcp_connector::cancel);
    }
    catch (std::exception& exp)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, endpoint,
                                           "Invalid websocket URL: " << url << " " << exp.what());
        throw ConnectionException("Invalid URL for websocket connection");
    }
}

#endif

#ifdef ROBOTRACONTEUR_USE_OPENSSL

void TcpWSSWebSocketConnector::Connect4(
    const RR_SHARED_PTR<RobotRaconteurException>& err, const RR_SHARED_PTR<ITransportConnection>& connection,
    const RR_SHARED_PTR<boost::asio::ip::tcp::socket>& socket,
    const RR_SHARED_PTR<boost::asio::ssl::stream<boost::asio::ip::tcp::socket&> >& tls_stream,
    const RR_SHARED_PTR<websocket_stream<boost::asio::ssl::stream<boost::asio::ip::tcp::socket&>&> >& websocket,
    const boost::function<void(const RR_SHARED_PTR<ITransportConnection>&,
                               const RR_SHARED_PTR<RobotRaconteurException>&)>& handler)
{
    RR_UNUSED(tls_stream);
    RR_UNUSED(websocket);
    if (err)
    {
        try
        {
            if (connection)
            {
                connection->Close();
            }
        }
        catch (std::exception&)
        {}

        ROBOTRACONTEUR_LOG_INFO_COMPONENT(node, Transport, endpoint,
                                          "TcpTransport wss stream attach failed: " << err->what());
        handler(RR_SHARED_PTR<ITransportConnection>(), err);
        return;
    }

    parent->register_transport(connection);

    ROBOTRACONTEUR_LOG_INFO_COMPONENT(node, Transport, endpoint,
                                      "TcpTransport connected wss transport to "
                                          << TcpTransport_socket_remote_endpoint(socket) << " from "
                                          << TcpTransport_socket_local_endpoint(socket));
    handler(connection, RR_SHARED_PTR<RobotRaconteurException>());
}

void TcpWSSWebSocketConnector::Connect3(
    const boost::system::error_code& ec, const RR_SHARED_PTR<boost::asio::ip::tcp::socket>& socket,
    const RR_SHARED_PTR<boost::signals2::scoped_connection>& socket_closer,
    const RR_SHARED_PTR<boost::asio::ssl::stream<boost::asio::ip::tcp::socket&> >& tls_stream,
    const RR_SHARED_PTR<websocket_stream<boost::asio::ssl::stream<boost::asio::ip::tcp::socket&>&> >& websocket,
    const boost::function<void(const RR_SHARED_PTR<ITransportConnection>&,
                               const RR_SHARED_PTR<RobotRaconteurException>&)>& handler)
{
    RR_UNUSED(socket_closer);
    if (ec)
    {
        ROBOTRACONTEUR_LOG_INFO_COMPONENT(node, Transport, endpoint,
                                          "TcpTransport wss websocket handshake failed: " << ec);
        RR_SHARED_PTR<ConnectionException> err =
            RR_MAKE_SHARED<ConnectionException>("Could not connect to remote websocket");
        handler(RR_SHARED_PTR<ITransportConnection>(), err);
        return;
    }

    socket->set_option(boost::asio::ip::tcp::socket::linger(true, 5));

    try
    {
        ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, endpoint, "wss connected, begin attach transport");
        RR_SHARED_PTR<TcpTransportConnection> t = RR_MAKE_SHARED<TcpTransportConnection>(parent, url, false, endpoint);
        boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>&)> h =
            boost::bind(&TcpWSSWebSocketConnector::Connect4, shared_from_this(), RR_BOOST_PLACEHOLDERS(_1), t, socket,
                        tls_stream, websocket, handler);
        t->AsyncAttachWSSWebSocket(socket, tls_stream, websocket, context, h);
        parent->AddCloseListener(t, &TcpTransportConnection::Close);
    }
    catch (std::exception& exp)
    {
        ROBOTRACONTEUR_LOG_INFO_COMPONENT(node, Transport, endpoint,
                                          "TcpTransport wss could not connect to supplied URL: " << exp.what());
        handler(RR_SHARED_PTR<TcpTransportConnection>(),
                RR_MAKE_SHARED<ConnectionException>("Could not connect to service"));
    }
}

void TcpWSSWebSocketConnector::Connect2_1(
    const boost::system::error_code& ec, const RR_SHARED_PTR<boost::asio::ip::tcp::socket>& socket,
    const RR_SHARED_PTR<boost::signals2::scoped_connection>& socket_closer,
    const RR_SHARED_PTR<boost::asio::ssl::stream<boost::asio::ip::tcp::socket&> >& tls_stream,
    const boost::function<void(const RR_SHARED_PTR<ITransportConnection>&,
                               const RR_SHARED_PTR<RobotRaconteurException>&)>& handler)
{
    if (ec)
    {
        ROBOTRACONTEUR_LOG_INFO_COMPONENT(node, Transport, endpoint,
                                          "TcpTransport wss HTTP TLS client handshake failed: " << ec.message());
        RR_SHARED_PTR<ConnectionException> err = RR_MAKE_SHARED<ConnectionException>("Could not contact remote host");
        handler(RR_SHARED_PTR<ITransportConnection>(), err);
        return;
    }

    try
    {
        ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, endpoint, "wss tcp socket connected, begin handshake");
        RR_SHARED_PTR<websocket_stream<boost::asio::ssl::stream<boost::asio::ip::tcp::socket&>&> > websocket =
            RR_MAKE_SHARED<websocket_stream<boost::asio::ssl::stream<boost::asio::ip::tcp::socket&>&> >(
                boost::ref(*tls_stream));
        websocket->async_client_handshake(ws_url, "robotraconteur.robotraconteur.com",
                                          boost::bind(&TcpWSSWebSocketConnector::Connect3, shared_from_this(),
                                                      RR_BOOST_PLACEHOLDERS(_1), socket, socket_closer, tls_stream,
                                                      websocket, boost::protect(handler)));
    }
    catch (std::exception& exp)
    {
        ROBOTRACONTEUR_LOG_INFO_COMPONENT(node, Transport, endpoint,
                                          "TcpTransport wss could not connect to supplied URL: " << exp.what());
        RR_SHARED_PTR<ConnectionException> err = RR_MAKE_SHARED<ConnectionException>("Could not contact remote host");
        handler(RR_SHARED_PTR<ITransportConnection>(), err);
        return;
    }
}
#ifdef ROBOTRACONTEUR_APPLE
bool TcpWSSWebSocketConnector::verify_callback(bool preverified, boost::asio::ssl::verify_context& ctx,
                                               boost::string_ref servername)
{
    if (preverified)
    {
        return true;
    }

    STACK_OF(X509)* certs = X509_STORE_CTX_get1_chain(ctx.native_handle());

    std::vector<SecCertificateRef> certarray1;
    for (int i = 0; i < sk_X509_num(certs); i++)
    {
        BIO* bio = BIO_new(BIO_s_mem());
        X509* cert = sk_X509_value(certs, i);
        i2d_X509_bio(bio, cert);
        int err = ERR_get_error();
        if (err)
        {
            BOOST_FOREACH (SecCertificateRef& e, certarray1)
            {
                CFRelease(e);
            }
            sk_X509_pop_free(certs, X509_free);
            BIO_free(bio);
            return false;
        }

        BUF_MEM* mem = NULL;
        BIO_get_mem_ptr(bio, &mem);
        err = ERR_get_error();
        if (err)
        {
            BOOST_FOREACH (SecCertificateRef& e, certarray1)
            {
                CFRelease(e);
            }
            sk_X509_pop_free(certs, X509_free);
            BIO_free(bio);
            return false;
        }
        CFDataRef d = CFDataCreate(NULL, (UInt8*)mem->data, mem->length);

        SecCertificateRef caRef = SecCertificateCreateWithData(NULL, d);

        BIO_free(bio);
        if (!caRef)
        {
            BOOST_FOREACH (SecCertificateRef& e, certarray1)
            {
                CFRelease(e);
            }
            sk_X509_pop_free(certs, X509_free);
            return false;
        }
        certarray1.push_back(caRef);
    }
    sk_X509_pop_free(certs, X509_free);

    CFArrayRef certarray = CFArrayCreate(NULL, (const void**)&certarray1[0], certarray1.size(), &kCFTypeArrayCallBacks);
    std::string servername1 = servername.to_string();
    CFStringRef host2 = CFStringCreateWithCString(NULL, servername1.c_str(), kCFStringEncodingUTF8);
    SecPolicyRef policy = SecPolicyCreateSSL(true, host2);

    SecTrustResultType result = kSecTrustResultDeny;

    SecTrustRef trust;
    int code = SecTrustCreateWithCertificates(certarray, policy, &trust);

    if (code == 0)
    {
        code = SecTrustEvaluate(trust, &result);
    }

    CFRelease(certarray);
    CFRelease(host2);
    CFRelease(policy);
    CFRelease(trust);

    if (result == kSecTrustResultUnspecified || result == kSecTrustResultProceed)
    {
        return true;
    }

    return false;
}
#endif
void TcpWSSWebSocketConnector::Connect2(
    const boost::system::error_code& ec, const RR_SHARED_PTR<boost::asio::ip::tcp::socket>& socket,
    const RR_SHARED_PTR<websocket_tcp_connector>& socket_connector,
    const boost::function<void(const RR_SHARED_PTR<ITransportConnection>&,
                               const RR_SHARED_PTR<RobotRaconteurException>&)>& handler)
{
    RR_UNUSED(socket_connector);
    if (ec)
    {
        ROBOTRACONTEUR_LOG_INFO_COMPONENT(node, Transport, endpoint,
                                          "TcpTransport websocket could not connect to supplied URL: " << ec);
        RR_SHARED_PTR<ConnectionException> err = RR_MAKE_SHARED<ConnectionException>("Could not contact remote host");
        handler(RR_SHARED_PTR<ITransportConnection>(), err);
        return;
    }

    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, endpoint,
                                       "wss tcp socket connected, begin wss HTTP TLS handshake");
    try
    {
        RR_SHARED_PTR<boost::signals2::scoped_connection> socket_closer =
            RR_MAKE_SHARED<boost::signals2::scoped_connection>(parent->AddCloseListener(
                socket, boost::bind(&boost::asio::ip::tcp::socket::close, RR_BOOST_PLACEHOLDERS(_1))));

        context = RR_MAKE_SHARED<boost::asio::ssl::context>(boost::asio::ssl::context::tlsv1);
        context->set_default_verify_paths();

        context->set_verify_mode(boost::asio::ssl::verify_peer);
#ifndef ROBOTRACONTEUR_APPLE
        context->set_verify_callback(boost::asio::ssl::rfc2818_verification(servername));
#else
        context->set_verify_callback(boost::bind(&TcpWSSWebSocketConnector::verify_callback, RR_BOOST_PLACEHOLDERS(_1),
                                                 RR_BOOST_PLACEHOLDERS(_2), servername));
#endif

        RR_SHARED_PTR<boost::asio::ssl::stream<boost::asio::ip::tcp::socket&> > tls_stream =
            RR_MAKE_SHARED<boost::asio::ssl::stream<boost::asio::ip::tcp::socket&> >(boost::ref(*socket),
                                                                                     boost::ref(*context));

        RobotRaconteurNode::asio_async_handshake(
            node, tls_stream, boost::asio::ssl::stream<boost::asio::ip::tcp::socket&>::client,
            boost::bind(&TcpWSSWebSocketConnector::Connect2_1, shared_from_this(), RR_BOOST_PLACEHOLDERS(_1), socket,
                        socket_closer, tls_stream, boost::protect(handler)));
    }
    catch (std::exception& exp)
    {
        ROBOTRACONTEUR_LOG_INFO_COMPONENT(node, Transport, endpoint,
                                          "TcpTransport wss HTTP TLS client handshake failed" << exp.what());
        RR_SHARED_PTR<ConnectionException> err = RR_MAKE_SHARED<ConnectionException>("Could not contact remote host");
        handler(RR_SHARED_PTR<ITransportConnection>(), err);
        return;
    }
}

TcpWSSWebSocketConnector::TcpWSSWebSocketConnector(const RR_SHARED_PTR<TcpTransport>& parent)
{
    endpoint = 0;
    this->parent = parent;
    this->node = parent->GetNode();
}

void TcpWSSWebSocketConnector::Connect(
    boost::string_ref url, uint32_t endpoint,
    boost::function<void(const RR_SHARED_PTR<ITransportConnection>&, const RR_SHARED_PTR<RobotRaconteurException>&)>
        handler)
{

    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, endpoint,
                                       "TcpTransport begin wss connection with URL: " << url);

    this->url = RR_MOVE(url.to_string());
    this->endpoint = endpoint;

    try
    {
        ParseConnectionURLResult url_res = ParseConnectionURL(url);

        if (url_res.scheme != "rr+wss" && url_res.scheme != "rrs+wss")
            throw InvalidArgumentException("Invalid transport type for TcpTransport");

        // if (url_result[1]!="tcp") throw InvalidArgumentException("Invalid transport type for TcpTransport");

        std::string host = url_res.host;
        servername = host;
        std::string port = boost::lexical_cast<std::string>(url_res.port);

        std::string path = url_res.path;

        if (url_res.scheme == "rr+wss")
        {
            std::string url1 = RR_MOVE(url.to_string());
            ws_url = boost::replace_first_copy(url1, "rr+wss", "wss");
        }

        if (url_res.scheme == "rrs+wss")
        {
            std::string url1 = RR_MOVE(url.to_string());
            ws_url = boost::replace_first_copy(url1, "rrs+wss", "wss");
        }

        // std::cout << ws_url << std::endl;

        RR_SHARED_PTR<detail::websocket_tcp_connector> socket_connector =
            RR_MAKE_SHARED<detail::websocket_tcp_connector>(
                boost::ref(parent->GetNode()->GetThreadPool()->get_io_context()));

        socket_connector->connect(ws_url, boost::bind(&TcpWSSWebSocketConnector::Connect2, shared_from_this(),
                                                      RR_BOOST_PLACEHOLDERS(_1), RR_BOOST_PLACEHOLDERS(_2),
                                                      socket_connector, boost::protect(RR_MOVE(handler))));
        parent->AddCloseListener(socket_connector, &detail::websocket_tcp_connector::cancel);
    }
    catch (std::exception& exp)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, endpoint,
                                           "Invalid websocket URL: " << url << " " << exp.what());
        throw ConnectionException("Invalid URL for websocket connection");
    }
}

#endif

} // namespace detail

TcpTransport::TcpTransport(const RR_SHARED_PTR<RobotRaconteurNode>& node) : Transport(node)
{
    if (!node)
        throw InvalidArgumentException("Node cannot be null");

    m_Port = 0;
    default_connect_timeout = 5000;
    default_receive_timeout = 15000;
    max_message_size = 12 * 1024 * 1024;
    max_connection_count = 0;
    this->node = node;
    this->heartbeat_period = 10000;
    this->require_tls = false;
    ipv4_acceptor_paused = false;
    ipv6_acceptor_paused = false;
    accept_websockets = true;

    allowed_websocket_origins.push_back("null");
    allowed_websocket_origins.push_back("file://");
    allowed_websocket_origins.push_back("chrome-extension://");
    allowed_websocket_origins.push_back("http://robotraconteur.com");
    allowed_websocket_origins.push_back("http://robotraconteur.com:80");
    allowed_websocket_origins.push_back("http://*.robotraconteur.com");
    allowed_websocket_origins.push_back("http://*.robotraconteur.com:80");
    allowed_websocket_origins.push_back("https://robotraconteur.com");
    allowed_websocket_origins.push_back("https://robotraconteur.com:443");
    allowed_websocket_origins.push_back("https://*.robotraconteur.com");
    allowed_websocket_origins.push_back("https://*.robotraconteur.com:443");

#ifndef ROBOTRACONTEUR_DISABLE_MESSAGE4
    disable_message4 = false;
#else
    disable_message4 = true;
#endif
#ifndef ROBOTRACONTEUR_DISABLE_STRINGTABLE
    disable_string_table = false;
#else
    disable_string_table = true;
#endif
    disable_async_message_io = false;
    closed = false;

    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, -1, "TcpTransport created");
}

TcpTransport::~TcpTransport() {}

void TcpTransport::Close()
{
    {
        boost::mutex::scoped_lock lock(closed_lock);
        if (closed)
            return;
        closed = true;
    }

    try
    {
        boost::mutex::scoped_lock lock(port_sharer_client_lock);
        if (port_sharer_client)
        {
            RR_SHARED_PTR<detail::TcpTransportPortSharerClient> c =
                RR_STATIC_POINTER_CAST<detail::TcpTransportPortSharerClient>(port_sharer_client);
            if (c)
            {
                c->Close();
            }
        }
    }
    catch (std::exception&)
    {}

    try
    {
        boost::mutex::scoped_lock lock(acceptor_lock);
        if (ipv4_acceptor)
            ipv4_acceptor->close();
        if (ipv6_acceptor)
            ipv6_acceptor->close();
    }
    catch (std::exception&)
    {}

    std::vector<RR_SHARED_PTR<ITransportConnection> > t;

    {
        boost::mutex::scoped_lock lock(TransportConnections_lock);
        boost::copy(TransportConnections | boost::adaptors::map_values, std::back_inserter(t));
        BOOST_FOREACH (RR_WEAK_PTR<ITransportConnection>& e, incoming_TransportConnections)
        {
            RR_SHARED_PTR<ITransportConnection> t2 = e.lock();
            if (t2)
                t.push_back(t2);
        }
    }

    BOOST_FOREACH (RR_SHARED_PTR<ITransportConnection>& e, t)
    {
        try
        {
            e->Close();
            RR_SHARED_PTR<TcpTransportConnection> tt = RR_DYNAMIC_POINTER_CAST<TcpTransportConnection>(e);
            if (!tt->IsClosed())
            {
                closing_TransportConnections.push_back(tt);
            }
        }
        catch (std::exception&)
        {}
    }

    try
    {
        DisableNodeAnnounce();
    }
    catch (std::exception&)
    {}

    try
    {
        DisableNodeDiscoveryListening();
    }
    catch (std::exception&)
    {}

    boost::posix_time::ptime t1 = boost::posix_time::microsec_clock::universal_time();
    boost::posix_time::ptime t2 = t1;

    while ((t2 - t1).total_milliseconds() < 500)
    {
        bool stillopen = false;
        BOOST_FOREACH (RR_WEAK_PTR<ITransportConnection>& e, closing_TransportConnections)
        {
            try
            {
                RR_SHARED_PTR<ITransportConnection> t1 = e.lock();
                if (!t1)
                    continue;
                RR_SHARED_PTR<TcpTransportConnection> t2 = RR_DYNAMIC_POINTER_CAST<TcpTransportConnection>(t1);
                if (!t2->IsClosed())
                {
                    stillopen = true;
                }
            }
            catch (std::exception&)
            {}
        }

        if (!stillopen)
            return;
        boost::this_thread::sleep(boost::posix_time::milliseconds(25));
        t2 = boost::posix_time::microsec_clock::universal_time();
    }

    BOOST_FOREACH (RR_WEAK_PTR<ITransportConnection>& e, closing_TransportConnections)
    {
        try
        {
            RR_SHARED_PTR<ITransportConnection> t1 = e.lock();
            if (!t1)
                continue;
            RR_SHARED_PTR<TcpTransportConnection> t2 = RR_DYNAMIC_POINTER_CAST<TcpTransportConnection>(t1);
            if (!t2->IsClosed())
            {
                t2->ForceClose();
            }
        }
        catch (std::exception&)
        {}
    }

    close_signal();

    ROBOTRACONTEUR_LOG_INFO_COMPONENT(node, Transport, -1, "TcpTransport closed");
}

bool TcpTransport::IsServer() const { return true; }

bool TcpTransport::IsClient() const { return true; }

std::string TcpTransport::GetUrlSchemeString() const { return "rr+tcp"; }

int32_t TcpTransport::GetListenPort()
{
    boost::mutex::scoped_lock lock(port_sharer_client_lock);
    if (port_sharer_client)
    {
        RR_SHARED_PTR<detail::TcpTransportPortSharerClient> c =
            RR_STATIC_POINTER_CAST<detail::TcpTransportPortSharerClient>(port_sharer_client);
        if (c)
        {
            return c->GetListenPort();
        }
    }

    return m_Port;
}

bool TcpTransport::CanConnectService(boost::string_ref url)
{

    if (url.size() < 6)
        return false;

    if (boost::starts_with(url, "tcp://"))
        return true;

    if (boost::starts_with(url, "rr+tcp://"))
        return true;

    if (boost::starts_with(url, "rrs+tcp://"))
        return true;

    if (boost::starts_with(url, "rr+ws://"))
        return true;

    if (boost::starts_with(url, "rrs+ws://"))
        return true;

    if (boost::starts_with(url, "rr+wss://"))
        return true;

    if (boost::starts_with(url, "rrs+wss://"))
        return true;

    return false;
}

void TcpTransport::AsyncCreateTransportConnection(
    boost::string_ref url, const RR_SHARED_PTR<Endpoint>& e,
    boost::function<void(const RR_SHARED_PTR<ITransportConnection>&, const RR_SHARED_PTR<RobotRaconteurException>&)>&
        callback)
{

    ROBOTRACONTEUR_LOG_INFO_COMPONENT(node, Transport, e->GetLocalEndpoint(),
                                      "TcpTransport begin create transport connection with URL: " << url);

    {
        int32_t max_connections = GetMaxConnectionCount();
        if (max_connections > 0)
        {
            boost::mutex::scoped_lock lock(TransportConnections_lock);
            if (boost::numeric_cast<int32_t>(TransportConnections.size()) > max_connections)
            {
                ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, e->GetLocalEndpoint(),
                                                   "Too many active TCP connections");
                throw ConnectionException("Too many active TCP connections");
            }
        }
    }

    if (boost::starts_with(url, "rr+ws://") || boost::starts_with(url, "rrs+ws://"))
    {
        RR_SHARED_PTR<detail::TcpWebSocketConnector> cw =
            RR_MAKE_SHARED<detail::TcpWebSocketConnector>(shared_from_this());
        cw->Connect(url, e->GetLocalEndpoint(), callback);
        return;
    }

    if (boost::starts_with(url, "rr+wss://") || boost::starts_with(url, "rrs+wss://"))
    {
        RR_SHARED_PTR<detail::TcpWSSWebSocketConnector> cw =
            RR_MAKE_SHARED<detail::TcpWSSWebSocketConnector>(shared_from_this());
        cw->Connect(url, e->GetLocalEndpoint(), callback);
        return;
    }

    RR_SHARED_PTR<detail::TcpConnector> c = RR_MAKE_SHARED<detail::TcpConnector>(shared_from_this());
    std::vector<std::string> url2;
    url2.push_back(url.to_string());
    c->Connect(url2, e->GetLocalEndpoint(), callback);
}

RR_SHARED_PTR<ITransportConnection> TcpTransport::CreateTransportConnection(boost::string_ref url,
                                                                            const RR_SHARED_PTR<Endpoint>& e)
{
    ROBOTRACONTEUR_ASSERT_MULTITHREADED(node);

    RR_SHARED_PTR<detail::sync_async_handler<ITransportConnection> > d =
        RR_MAKE_SHARED<detail::sync_async_handler<ITransportConnection> >(
            RR_MAKE_SHARED<ConnectionException>("Timeout exception"));

    boost::function<void(const RR_SHARED_PTR<ITransportConnection>&, const RR_SHARED_PTR<RobotRaconteurException>&)> h =
        boost::bind(&detail::sync_async_handler<ITransportConnection>::operator(), d, RR_BOOST_PLACEHOLDERS(_1),
                    RR_BOOST_PLACEHOLDERS(_2));
    AsyncCreateTransportConnection(url, e, h);

    return d->end();
}

void TcpTransport::CloseTransportConnection(const RR_SHARED_PTR<Endpoint>& e)
{

    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, e->GetLocalEndpoint(),
                                       "TcpTransport request close transport connection");

    RR_SHARED_PTR<ServerEndpoint> e2 = boost::dynamic_pointer_cast<ServerEndpoint>(e);
    if (e2)
    {
        {
            try
            {
                boost::mutex::scoped_lock lock(TransportConnections_lock);
                RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<ITransportConnection> >::iterator e1 =
                    TransportConnections.find(e->GetLocalEndpoint());
                if (e1 != TransportConnections.end())
                {
                    closing_TransportConnections.push_back(e1->second);
                }
            }
            catch (std::exception&)
            {}
        }
        RR_SHARED_PTR<boost::asio::deadline_timer> timer(
            new boost::asio::deadline_timer(GetNode()->GetThreadPool()->get_io_context()));
        timer->expires_from_now(boost::posix_time::milliseconds(100));
        RobotRaconteurNode::asio_async_wait(node, timer,
                                            boost::bind(&TcpTransport::CloseTransportConnection_timed,
                                                        shared_from_this(), boost::asio::placeholders::error, e,
                                                        timer));
        return;
    }

    RR_SHARED_PTR<ITransportConnection> t;
    {
        boost::mutex::scoped_lock lock(TransportConnections_lock);
        RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<ITransportConnection> >::iterator e1 =
            TransportConnections.find(e->GetLocalEndpoint());
        if (e1 == TransportConnections.end())
            return;
        t = e1->second;
        TransportConnections.erase(e1);
    }

    if (t)
    {
        try
        {
            t->Close();
        }
        catch (std::exception&)
        {}

        RR_SHARED_PTR<TcpTransportConnection> tt = RR_DYNAMIC_POINTER_CAST<TcpTransportConnection>(t);
        if (tt)
        {
            if (!tt->IsClosed())
            {
                boost::mutex::scoped_lock lock(TransportConnections_lock);
                closing_TransportConnections.push_back(t);
            }
        }
    }
}

void TcpTransport::CloseTransportConnection_timed(const boost::system::error_code& err,
                                                  const RR_SHARED_PTR<Endpoint>& e, const RR_SHARED_PTR<void>& timer)
{
    RR_UNUSED(timer);
    if (err)
        return;

    RR_SHARED_PTR<ITransportConnection> t;

    {
        boost::mutex::scoped_lock lock(TransportConnections_lock);
        RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<ITransportConnection> >::iterator e1 =
            TransportConnections.find(e->GetLocalEndpoint());
        if (e1 == TransportConnections.end())
            return;
        t = e1->second;
    }

    if (t)
    {
        try
        {
            t->Close();
        }
        catch (std::exception&)
        {}
    }
}

void TcpTransport::SendMessage(const RR_INTRUSIVE_PTR<Message>& m)
{

    RR_SHARED_PTR<ITransportConnection> t;

    {
        boost::mutex::scoped_lock lock(TransportConnections_lock);
        RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<ITransportConnection> >::iterator e1 =
            TransportConnections.find(m->header->SenderEndpoint);
        if (e1 == TransportConnections.end())
        {
            ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, m->header->SenderEndpoint,
                                               "Transport connection to remote host not found");
            throw ConnectionException("Transport connection to remote host not found");
        }
        t = e1->second;
    }

    t->SendMessage(m);
}

uint32_t TcpTransport::TransportCapability(boost::string_ref name)
{
    RR_UNUSED(name);
    return 0;
}

void TcpTransport::PeriodicCleanupTask()
{
    boost::mutex::scoped_lock lock(TransportConnections_lock);
    for (RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<ITransportConnection> >::iterator e = TransportConnections.begin();
         e != TransportConnections.end();)
    {
        try
        {
            RR_SHARED_PTR<TcpTransportConnection> e2 = rr_cast<TcpTransportConnection>(e->second);
            if (!e2->IsConnected())
            {
                e = TransportConnections.erase(e);
            }
            else
            {
                e++;
            }
        }
        catch (std::exception&)
        {}
    }

    for (std::list<RR_WEAK_PTR<ITransportConnection> >::iterator e = closing_TransportConnections.begin();
         e != closing_TransportConnections.end();)
    {
        try
        {
            if (e->expired())
            {
                e = closing_TransportConnections.erase(e);
            }
            else
            {
                e++;
            }
        }
        catch (std::exception&)
        {}
    }

    for (std::list<RR_WEAK_PTR<ITransportConnection> >::iterator e = incoming_TransportConnections.begin();
         e != incoming_TransportConnections.end();)
    {
        try
        {
            if (e->expired())
            {
                e = incoming_TransportConnections.erase(e);
            }
            else
            {
                e++;
            }
        }
        catch (std::exception&)
        {}
    }
}

void TcpTransport::AsyncSendMessage(const RR_INTRUSIVE_PTR<Message>& m,
                                    const boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>&)>& handler)
{

    RR_SHARED_PTR<ITransportConnection> t;

    {
        boost::mutex::scoped_lock lock(TransportConnections_lock);
        RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<ITransportConnection> >::iterator e1 =
            TransportConnections.find(m->header->SenderEndpoint);
        if (e1 == TransportConnections.end())
        {
            ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, m->header->SenderEndpoint,
                                               "Transport connection to remote host not found");
            throw ConnectionException("Transport connection to remote host not found");
        }
        t = e1->second;
    }

    t->AsyncSendMessage(m, handler);
}

void TcpTransport::StartServer(int32_t port)
{

#ifndef ROBOTRACONTEUR_ALLOW_TCP_LISTEN_PORT_48653
    if (port == 48653)
    {
        ROBOTRACONTEUR_LOG_ERROR_COMPONENT(node, Transport, -1,
                                           "TCP port 48653 is reserved for Robot Raconteur port sharer");
        throw InvalidArgumentException("Port 48653 is reserved");
    }
#endif

    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, -1, "TcpTransport starting server on port: " << port);

    try
    {
        boost::mutex::scoped_lock lock(acceptor_lock);
        std::vector<boost::asio::ip::address> local_addresses;

        TcpTransport::GetLocalAdapterIPAddresses(local_addresses);

        bool has_ip6 = false;
        bool has_ip4 = false;

        BOOST_FOREACH (boost::asio::ip::address& e, local_addresses)
        {
            if (e.is_v6())
                has_ip6 = true;
            if (e.is_v4())
                has_ip4 = true;
        }

        bool reuse_addr = false;

        const char* reuse_addr_env1 = std::getenv("ROBOTRACONTEUR_TCP_SERVER_REUSEADDR");
        if (reuse_addr_env1)
        {
            std::string reuse_addr_env(reuse_addr_env1);
            boost::to_lower(reuse_addr_env);
            if (reuse_addr_env == "true" || reuse_addr_env == "on" || reuse_addr_env == "1")
            {
                reuse_addr = true;
            }
        }

        if (has_ip4)
        {

            ipv4_acceptor = RR_SHARED_PTR<boost::asio::ip::tcp::acceptor>(
                new boost::asio::ip::tcp::acceptor(GetNode()->GetThreadPool()->get_io_context(),
                                                   boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)));

            ipv4_acceptor->set_option(boost::asio::ip::tcp::socket::linger(false, 5));
            if (reuse_addr)
            {
                ipv4_acceptor->set_option(boost::asio::ip::tcp::socket::reuse_address(true));
            }

            RR_SHARED_PTR<boost::asio::ip::tcp::socket> socket(
                new boost::asio::ip::tcp::socket(GetNode()->GetThreadPool()->get_io_context()));

            ipv4_acceptor->async_accept(*socket, boost::bind(&TcpTransport::handle_v4_accept, shared_from_this(),
                                                             ipv4_acceptor, socket, boost::asio::placeholders::error));

            if (port == 0)
            {
                port = ipv4_acceptor->local_endpoint().port();
            }

            ipv4_acceptor_paused = false;
        }

        if (has_ip6)
        {
            ipv6_acceptor = RR_SHARED_PTR<boost::asio::ip::tcp::acceptor>(new boost::asio::ip::tcp::acceptor(
                GetNode()->GetThreadPool()->get_io_context(), boost::asio::ip::tcp::v6()));

            ipv6_acceptor->set_option(boost::asio::ip::tcp::socket::linger(false, 5));
            if (reuse_addr)
            {
                ipv6_acceptor->set_option(boost::asio::ip::tcp::socket::reuse_address(true));
            }

            int on = 1;
            setsockopt(ipv6_acceptor->native_handle(), IPPROTO_IPV6, IPV6_V6ONLY, reinterpret_cast<const char*>(&on),
                       sizeof(on));

            ipv6_acceptor->bind(boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v6(), port));

            ipv6_acceptor->listen();

            RR_SHARED_PTR<boost::asio::ip::tcp::socket> socket2(
                new boost::asio::ip::tcp::socket(GetNode()->GetThreadPool()->get_io_context()));

            ipv6_acceptor->async_accept(*socket2,
                                        boost::bind(&TcpTransport::handle_v6_accept, shared_from_this(), ipv6_acceptor,
                                                    socket2, boost::asio::placeholders::error));

            if (port == 0)
            {
                port = ipv6_acceptor->local_endpoint().port();
            }

            ipv6_acceptor_paused = false;
        }

        m_Port = port;

        ROBOTRACONTEUR_LOG_INFO_COMPONENT(node, Transport, -1, "TcpTransport server started on port: " << port);
    }
    catch (std::exception& exp)
    {
        ROBOTRACONTEUR_LOG_ERROR_COMPONENT(
            node, Transport, -1, "TcpTransport starting server on port : " << port << " failed: " << exp.what());
        throw;
    }
}

void TcpTransport::StartServerUsingPortSharer()
{
    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, -1,
                                       "TcpTransport starting server using Robot Raconteur port sharer");
    boost::mutex::scoped_lock lock(port_sharer_client_lock);
    RR_SHARED_PTR<detail::TcpTransportPortSharerClient> c;
    if (port_sharer_client)
    {
        c = RR_STATIC_POINTER_CAST<detail::TcpTransportPortSharerClient>(port_sharer_client);
    }
    if (!c)
    {
        c = RR_MAKE_SHARED<detail::TcpTransportPortSharerClient>(shared_from_this());
    }

    port_sharer_client = c;
    c->Start();

    ROBOTRACONTEUR_LOG_INFO_COMPONENT(node, Transport, -1,
                                      "TcpTransport server started using Robot Raconteur port sharer");
}

bool TcpTransport::IsPortSharerRunning()
{
    boost::mutex::scoped_lock lock(port_sharer_client_lock);
    if (!port_sharer_client)
        return false;
    RR_SHARED_PTR<detail::TcpTransportPortSharerClient> c =
        RR_STATIC_POINTER_CAST<detail::TcpTransportPortSharerClient>(port_sharer_client);
    if (!c)
        return false;
    return c->IsPortSharerConnected();
}

void TcpTransport::GetLocalAdapterIPAddresses(std::vector<boost::asio::ip::address>& addresses)
{

    addresses.clear();

#ifdef ROBOTRACONTEUR_WINDOWS

    /* Declare and initialize variables */

    DWORD dwSize = 0;
    DWORD dwRetVal = 0;

    unsigned int i = 0;

    // Set the flags to pass to GetAdaptersAddresses
    ULONG flags = GAA_FLAG_INCLUDE_PREFIX;

    // default to unspecified address family (both)
    ULONG family = AF_UNSPEC;

    LPVOID lpMsgBuf = NULL;

    PIP_ADAPTER_ADDRESSES pAddresses = NULL;
    ULONG outBufLen = 0;
    ULONG Iterations = 0;

    PIP_ADAPTER_ADDRESSES pCurrAddresses = NULL;
    PIP_ADAPTER_UNICAST_ADDRESS pUnicast = NULL;
    PIP_ADAPTER_ANYCAST_ADDRESS pAnycast = NULL;
    PIP_ADAPTER_MULTICAST_ADDRESS pMulticast = NULL;
    IP_ADAPTER_DNS_SERVER_ADDRESS* pDnServer = NULL;
    IP_ADAPTER_PREFIX* pPrefix = NULL;

    pAddresses = 0;

    outBufLen = 15000;

    do
    {

        pAddresses = (IP_ADAPTER_ADDRESSES*)HeapAlloc(GetProcessHeap(), 0, outBufLen);
        if (pAddresses == NULL)
        {
            HeapFree(GetProcessHeap(), 0, pAddresses);
            throw SystemResourceException("Could not get local IP addresses");
        }

        dwRetVal = GetAdaptersAddresses(family, flags, NULL, pAddresses, &outBufLen);

        if (dwRetVal == ERROR_BUFFER_OVERFLOW)
        {
            HeapFree(GetProcessHeap(), 0, pAddresses);
            pAddresses = NULL;
        }
        else
        {
            break;
        }

        Iterations++;

    } while ((dwRetVal == ERROR_BUFFER_OVERFLOW) && (Iterations < 5));

    if (dwRetVal != NO_ERROR)
    {
        HeapFree(GetProcessHeap(), 0, pAddresses);
        throw SystemResourceException("Could not get local IP addresses");
    }

    pCurrAddresses = pAddresses;
    while (pCurrAddresses)
    {

        pUnicast = pCurrAddresses->FirstUnicastAddress;
        if (pUnicast != NULL)
        {
            for (i = 0; pUnicast != NULL; i++)
            {

                if (pUnicast->Address.lpSockaddr->sa_family == AF_INET)
                {
                    struct sockaddr_in* ip1 = (struct sockaddr_in*)pUnicast->Address.lpSockaddr;
                    boost::asio::ip::address_v4::bytes_type b;
                    memcpy((uint8_t*)&b[0], (uint8_t*)&ip1->sin_addr, 4);
                    addresses.push_back(boost::asio::ip::address_v4(b));
                }

                if (pUnicast->Address.lpSockaddr->sa_family == AF_INET6)
                {

                    struct sockaddr_in6* ip1 = (struct sockaddr_in6*)pUnicast->Address.lpSockaddr;
                    boost::asio::ip::address_v6::bytes_type b;
                    memcpy(&b[0], &ip1->sin6_addr, 16);

                    boost::asio::ip::address_v6 addr(b, ip1->sin6_scope_id);
                    if (!(/*addr.is_v4_compatible() ||*/ addr.is_v4_mapped()))
                    {
                        addresses.push_back(addr);
                    }
                }

                pUnicast = pUnicast->Next;
            }
        }

        pCurrAddresses = pCurrAddresses->Next;
    }

    if (pAddresses)
    {
        HeapFree(GetProcessHeap(), 0, pAddresses);
    }
#else
    // TODO fill this in
    struct ifaddrs* ifaddr = NULL;
    struct ifaddrs* ifa = NULL;

    if (getifaddrs(&ifaddr) == -1)
    {
        throw SystemResourceException("Could not get any local IP addresses");
    }

    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next)
    {
        if (ifa->ifa_addr == NULL)
            continue;

        if (ifa->ifa_addr->sa_family == AF_INET)
        {
            struct sockaddr_in* ip1 = reinterpret_cast<struct sockaddr_in*>(ifa->ifa_addr);
            boost::asio::ip::address_v4::bytes_type b;
            memcpy(&b[0], &ip1->sin_addr, 4);
            addresses.push_back(boost::asio::ip::address_v4(b));
        }

        if (ifa->ifa_addr->sa_family == AF_INET6)
        {
            struct sockaddr_in6* ip1 = reinterpret_cast<struct sockaddr_in6*>(ifa->ifa_addr);
            boost::asio::ip::address_v6::bytes_type b;
            memcpy(&b[0], &ip1->sin6_addr, 16);
            addresses.push_back(boost::asio::ip::address_v6(b, ip1->sin6_scope_id));
        }
    }

    freeifaddrs(ifaddr);

#endif

    if (addresses.empty())
    {
        throw SystemResourceException("Could not get any local IP addresses");
    }

    /*for(std::vector<boost::asio::ip::address>::iterator e=addresses.begin(); e!=addresses.end(); ++e)
    {
        std::cout << e->to_string() << std::endl;
    }

    std::cout << std::endl;*/
}

void TcpTransport::handle_v4_accept(const RR_SHARED_PTR<TcpTransport>& parent,
                                    const RR_SHARED_PTR<boost::asio::ip::tcp::acceptor>& acceptor,
                                    const RR_SHARED_PTR<boost::asio::ip::tcp::socket>& socket,
                                    const boost::system::error_code& error)
{
    if (error)
        return;

    try
    {
        socket->set_option(boost::asio::ip::tcp::socket::linger(true, 5));

        // TcpTransport_attach_transport(parent,socket,"",true,0,boost::bind(&TcpTransport_connected_callback2,parent,RR_BOOST_PLACEHOLDERS(_1),RR_BOOST_PLACEHOLDERS(_2),RR_BOOST_PLACEHOLDERS(_3)));
        RR_SHARED_PTR<detail::TcpAcceptor> a = RR_MAKE_SHARED<detail::TcpAcceptor>(parent, "", 0);
        boost::function<void(RR_SHARED_PTR<boost::asio::ip::tcp::socket>, const RR_SHARED_PTR<ITransportConnection>&,
                             const RR_SHARED_PTR<RobotRaconteurException>&)>
            h = boost::bind(&TcpTransport_connected_callback2, parent, RR_BOOST_PLACEHOLDERS(_1),
                            RR_BOOST_PLACEHOLDERS(_2), RR_BOOST_PLACEHOLDERS(_3));
        a->AcceptSocket(socket, h);
    }
    catch (std::exception& exp)
    {
        RobotRaconteurNode::TryHandleException(parent->node, &exp);
    }

    int32_t connection_count = 0;
    {
        boost::mutex::scoped_lock lock(parent->TransportConnections_lock);
        connection_count = boost::numeric_cast<int32_t>(parent->TransportConnections.size());
    }

    int32_t max_connection_count = parent->GetMaxConnectionCount();

    boost::mutex::scoped_lock lock(parent->acceptor_lock);
    if (max_connection_count > 0)
    {
        if (connection_count > max_connection_count)
        {
            ROBOTRACONTEUR_LOG_INFO_COMPONENT(parent->node, Transport, -1,
                                              "TcpTransport pausing IPv4 accept due to exceeding MaxConnectionCount");
            parent->ipv4_acceptor_paused = true;
            return;
        }
    }

    RR_SHARED_PTR<boost::asio::ip::tcp::socket> socket2(
        new boost::asio::ip::tcp::socket(parent->GetNode()->GetThreadPool()->get_io_context()));

    acceptor->async_accept(*socket2, boost::bind(&TcpTransport::handle_v4_accept, parent, acceptor, socket2,
                                                 boost::asio::placeholders::error));
}

void TcpTransport::handle_v6_accept(const RR_SHARED_PTR<TcpTransport>& parent,
                                    const RR_SHARED_PTR<boost::asio::ip::tcp::acceptor>& acceptor,
                                    const RR_SHARED_PTR<boost::asio::ip::tcp::socket>& socket,
                                    const boost::system::error_code& error)
{
    if (error)
        return;

    try
    {
        socket->set_option(boost::asio::ip::tcp::socket::linger(true, 5));
        // TcpTransport_attach_transport(parent,socket,"",true,0,boost::bind(&TcpTransport_connected_callback2,parent,RR_BOOST_PLACEHOLDERS(_1),RR_BOOST_PLACEHOLDERS(_2),RR_BOOST_PLACEHOLDERS(_3)));
        RR_SHARED_PTR<detail::TcpAcceptor> a = RR_MAKE_SHARED<detail::TcpAcceptor>(parent, "", 0);
        boost::function<void(RR_SHARED_PTR<boost::asio::ip::tcp::socket>, const RR_SHARED_PTR<ITransportConnection>&,
                             const RR_SHARED_PTR<RobotRaconteurException>&)>
            h = boost::bind(&TcpTransport_connected_callback2, parent, RR_BOOST_PLACEHOLDERS(_1),
                            RR_BOOST_PLACEHOLDERS(_2), RR_BOOST_PLACEHOLDERS(_3));
        a->AcceptSocket(socket, h);
    }
    catch (std::exception& exp)
    {
        RobotRaconteurNode::TryHandleException(parent->node, &exp);
    }

    int32_t connection_count = 0;
    {
        boost::mutex::scoped_lock lock(parent->TransportConnections_lock);
        connection_count = boost::numeric_cast<int32_t>(parent->TransportConnections.size());
    }

    int32_t max_connection_count = parent->GetMaxConnectionCount();

    boost::mutex::scoped_lock lock(parent->acceptor_lock);
    if (max_connection_count > 0)
    {
        if (connection_count > max_connection_count)
        {
            ROBOTRACONTEUR_LOG_INFO_COMPONENT(parent->node, Transport, -1,
                                              "TcpTransport pausing IPv6 accept due to exceeding MaxConnectionCount");
            parent->ipv6_acceptor_paused = true;
            return;
        }
    }

    RR_SHARED_PTR<boost::asio::ip::tcp::socket> socket2(
        new boost::asio::ip::tcp::socket(parent->GetNode()->GetThreadPool()->get_io_context()));

    acceptor->async_accept(*socket2, boost::bind(&TcpTransport::handle_v6_accept, parent, acceptor, socket2,
                                                 boost::asio::placeholders::error));
}

int32_t TcpTransport::GetDefaultHeartbeatPeriod()
{
    boost::mutex::scoped_lock lock(parameter_lock);
    return heartbeat_period;
}

void TcpTransport::SetDefaultHeartbeatPeriod(int32_t milliseconds)
{
    if (!(milliseconds > 0))
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, -1, "Heartbeat period must be positive");
        throw InvalidArgumentException("Heartbeat period must be positive");
    }
    boost::mutex::scoped_lock lock(parameter_lock);
    heartbeat_period = milliseconds;
    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, -1, "Heartbeat period set to " << milliseconds << " ms");
}

int32_t TcpTransport::GetDefaultReceiveTimeout()
{
    boost::mutex::scoped_lock lock(parameter_lock);
    return default_receive_timeout;
}
void TcpTransport::SetDefaultReceiveTimeout(int32_t milliseconds)
{
    if (!(milliseconds > 0))
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, -1, "Receive timeout must be positive");
        throw InvalidArgumentException("Timeout must be positive");
    }
    boost::mutex::scoped_lock lock(parameter_lock);
    default_receive_timeout = milliseconds;

    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, -1, "Receive timeout set to " << milliseconds << " ms");
}
int32_t TcpTransport::GetDefaultConnectTimeout()
{
    boost::mutex::scoped_lock lock(parameter_lock);
    return default_connect_timeout;
}
void TcpTransport::SetDefaultConnectTimeout(int32_t milliseconds)
{
    if (!(milliseconds > 0))
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, -1, "Default connect timeout must be positive");
        throw InvalidArgumentException("Timeout must be positive");
    }
    boost::mutex::scoped_lock lock(parameter_lock);
    default_connect_timeout = milliseconds;

    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, -1, "Default connect timeout set to " << milliseconds << " ms");
}

int32_t TcpTransport::GetMaxMessageSize()
{
    boost::mutex::scoped_lock lock(parameter_lock);
    return max_message_size;
}

void TcpTransport::SetMaxMessageSize(int32_t size)
{
    if (size < 16 * 1024 || size > 100 * 1024 * 1024)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, -1, "Invalid MaxMessageSize: " << size);
        throw InvalidArgumentException("Invalid maximum message size");
    }
    boost::mutex::scoped_lock lock(parameter_lock);
    max_message_size = size;

    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, -1, "MaxMessageSize set to " << size << " bytes");
}

int32_t TcpTransport::GetMaxConnectionCount()
{
    boost::mutex::scoped_lock lock(parameter_lock);
    return max_connection_count;
}

void TcpTransport::SetMaxConnectionCount(int32_t count)
{
    if (count < -1)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, -1, "Invalid MaxConnectionCount: " << count);
        throw InvalidArgumentException("Invalid maximum connection count");
    }
    boost::mutex::scoped_lock lock(parameter_lock);
    max_connection_count = count;

    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, -1, "MaxConnectionCount set to " << count);
}

bool TcpTransport::GetAcceptWebSockets()
{
    boost::mutex::scoped_lock lock(parameter_lock);
    return accept_websockets;
}

void TcpTransport::SetAcceptWebSockets(bool value)
{
    boost::mutex::scoped_lock lock(parameter_lock);
    accept_websockets = value;

    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, -1, "AcceptWebSockets set to: " << value);
}

std::vector<std::string> TcpTransport::GetWebSocketAllowedOrigins()
{
    boost::mutex::scoped_lock lock(parameter_lock);
    return allowed_websocket_origins;
}
void TcpTransport::AddWebSocketAllowedOrigin(boost::string_ref origin)
{
    boost::mutex::scoped_lock lock(parameter_lock);

    boost::smatch origin_result;
    std::string origin1 = origin.to_string();
    if (!boost::regex_search(origin1, origin_result,
                             boost::regex("^([^:\\s]+)://(?:((?:\\[[A-Fa-f0-9\\:]+(?:\\%\\w*)?\\])|(?:[^\\[\\]\\:/"
                                          "\\?\\s]+))(?::([^\\:/\\?\\s]+))?)?$")))
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, -1, "Invalid WebSocket origin: " << origin1);
        throw InvalidArgumentException("Invalid WebSocket origin");
    }

    if (origin_result.size() < 4)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, -1, "Invalid WebSocket origin: " << origin1);
        throw InvalidArgumentException("Invalid WebSocket origin");
    }

    std::string host;
    std::string port;

    std::string scheme = origin_result[1];

    if (origin_result[2] != "")
    {
        host = origin_result[2];
        if (boost::starts_with(host, "*"))
        {
            std::string host2 = host.substr(1);

            if (host2.find('*') != std::string::npos)
            {
                ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, -1, "Invalid WebSocket origin: " << origin1);
                throw InvalidArgumentException("Invalid WebSocket origin");
            }

            if (!host2.empty())
            {
                if (!boost::starts_with(host2, "."))
                {
                    ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, -1, "Invalid WebSocket origin: " << origin1);
                    throw InvalidArgumentException("Invalid WebSocket origin");
                }
            }
        }
        else
        {
            if (host.find('*') != std::string::npos)
            {
                ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, -1, "Invalid WebSocket origin: " << origin1);
                throw InvalidArgumentException("Invalid WebSocket origin");
            }
        }

        port = origin_result[3];

        if (!port.empty())
        {
            try
            {
                boost::lexical_cast<int>(port);
            }
            catch (std::exception&)
            {
                ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, -1, "Invalid WebSocket origin: " << origin1);
                throw InvalidArgumentException("Invalid WebSocket origin");
            }
        }
    }
    else if (origin_result[3] != "")
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, -1, "Invalid WebSocket origin: " << origin1);
        throw InvalidArgumentException("Invalid WebSocket origin");
    }

    allowed_websocket_origins.push_back(RR_MOVE(origin.to_string()));

    if (scheme == "http" && port.empty())
    {
        allowed_websocket_origins.push_back(RR_MOVE(origin.to_string()) + ":80");
    }

    if (scheme == "https" && port.empty())
    {
        allowed_websocket_origins.push_back(RR_MOVE(origin.to_string()) + ":443");
    }

    if (scheme == "http" && port == "80")
    {
        allowed_websocket_origins.push_back(boost::replace_last_copy(origin.to_string(), ":80", ""));
    }

    if (scheme == "https" && port == "443")
    {
        allowed_websocket_origins.push_back(boost::replace_last_copy(origin.to_string(), ":443", ""));
    }

    ROBOTRACONTEUR_LOG_INFO_COMPONENT(node, Transport, -1, "Added WebSocket origin: " << origin1);
}
void TcpTransport::RemoveWebSocketAllowedOrigin(boost::string_ref origin)
{
    boost::mutex::scoped_lock lock(parameter_lock);
    allowed_websocket_origins.erase(
        std::remove(allowed_websocket_origins.begin(), allowed_websocket_origins.end(), origin),
        allowed_websocket_origins.end());
    ROBOTRACONTEUR_LOG_INFO_COMPONENT(node, Transport, -1, "Removed WebSocket origin: " << origin);
}

void TcpTransport::LoadTlsNodeCertificate()
{
    try
    {
#ifdef ROBOTRACONTEUR_WINDOWS

#ifdef ROBOTRACONTEUR_USE_SCHANNEL
        RR_SHARED_PTR<detail::TlsSchannelAsyncStreamAdapterContext> c =
            RR_STATIC_POINTER_CAST<detail::TlsSchannelAsyncStreamAdapterContext>(GetTlsContext());
        c->LoadCertificateFromMyStore();
#endif
#ifdef ROBOTRACONTEUR_USE_OPENSSL
        NodeDirectories node_dirs = GetNode()->GetNodeDirectories();
        boost::filesystem::path certstore = node_dirs.user_config_dir / "certificates";

        boost::filesystem::path certpath = certstore / (GetNode()->NodeID().ToString() + ".p12");

        if (!(boost::filesystem::is_regular(certpath) || boost::filesystem::is_symlink(certpath)))
        {
            throw SystemResourceException("Could not load node certificate");
        }
        RR_SHARED_PTR<detail::OpenSSLAuthContext> c =
            RR_STATIC_POINTER_CAST<detail::OpenSSLAuthContext>(GetTlsContext());
        c->LoadPKCS12FromFile(certpath.string());
#endif

#else
        NodeDirectories node_dirs = GetNode()->GetNodeDirectories();
        boost::filesystem::path certstore = node_dirs.user_config_dir / "certificates";

        boost::filesystem::path certpath = certstore / (GetNode()->NodeID().ToString() + ".p12");
        RR_SHARED_PTR<detail::OpenSSLAuthContext> c =
            RR_STATIC_POINTER_CAST<detail::OpenSSLAuthContext>(GetTlsContext());
        c->LoadPKCS12FromFile(certpath.string());
#endif
        ROBOTRACONTEUR_LOG_INFO_COMPONENT(node, Transport, -1,
                                          "Loaded TLS certificate for NodeID: " << GetNode()->NodeID().ToString());
    }
    catch (std::exception& exp)
    {
        ROBOTRACONTEUR_LOG_WARNING_COMPONENT(node, Transport, -1,
                                             "Loading TLS certificate for NodeID: "
                                                 << GetNode()->NodeID().ToString() << " failed: " << exp.what()
                                                 << ". This warning can be ignored if TLS not in use");
    }
}

boost::shared_ptr<void> TcpTransport::GetTlsContext()
{
    boost::mutex::scoped_lock lock(tls_context_lock);
    if (!tls_context)
    {
#ifdef ROBOTRACONTEUR_USE_SCHANNEL
        NodeID id = GetNode()->NodeID();
        tls_context = RR_MAKE_SHARED<detail::TlsSchannelAsyncStreamAdapterContext>(id);
#endif

#ifdef ROBOTRACONTEUR_USE_OPENSSL
        NodeID id = GetNode()->NodeID();
        tls_context = RR_MAKE_SHARED<detail::OpenSSLAuthContext>(id);
#endif
        if (!tls_context)
        {
            ROBOTRACONTEUR_LOG_ERROR_COMPONENT(node, Transport, -1, "Could not initialize TLS context");
            throw SystemResourceException("Could not initialize TLS");
        }
    }

    return tls_context;
}

bool TcpTransport::GetRequireTls()
{
    boost::mutex::scoped_lock lock(tls_context_lock);
    return require_tls;
}

void TcpTransport::SetRequireTls(bool require_tls)
{
    boost::mutex::scoped_lock lock(tls_context_lock);
    this->require_tls = require_tls;

    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, -1, "RequireTls set to: " << require_tls);
}

bool TcpTransport::IsTlsNodeCertificateLoaded()
{
#ifdef ROBOTRACONTEUR_USE_SCHANNEL
    RR_SHARED_PTR<detail::TlsSchannelAsyncStreamAdapterContext> c =
        RR_STATIC_POINTER_CAST<detail::TlsSchannelAsyncStreamAdapterContext>(GetTlsContext());
    return c->IsCertificateLoaded();
#endif

#ifdef ROBOTRACONTEUR_USE_OPENSSL
    RR_SHARED_PTR<detail::OpenSSLAuthContext> c = RR_STATIC_POINTER_CAST<detail::OpenSSLAuthContext>(GetTlsContext());
    return c->IsCertificateLoaded();
#endif

    throw NotImplementedException("Not implemented");
}

void TcpTransport_connected_callback2(const RR_SHARED_PTR<TcpTransport>& parent,
                                      const RR_SHARED_PTR<boost::asio::ip::tcp::socket>& socket,
                                      const RR_SHARED_PTR<ITransportConnection>& connection,
                                      const RR_SHARED_PTR<RobotRaconteurException>& err)
{
    // This is just an empty method.  The connected transport will register when it has a local endpoint.
}

void TcpTransport_attach_transport(
    const RR_SHARED_PTR<TcpTransport>& parent, const RR_SHARED_PTR<boost::asio::ip::tcp::socket>& socket,
    boost::string_ref url, bool server, uint32_t endpoint,
    boost::function<void(const RR_SHARED_PTR<boost::asio::ip::tcp::socket>&, const RR_SHARED_PTR<ITransportConnection>&,
                         const RR_SHARED_PTR<RobotRaconteurException>&)>& callback)
{
    try
    {
        RR_SHARED_PTR<TcpTransportConnection> t = RR_MAKE_SHARED<TcpTransportConnection>(parent, url, server, endpoint);
        boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>&)> h =
            boost::bind(callback, socket, t, RR_BOOST_PLACEHOLDERS(_1));
        t->AsyncAttachSocket(socket, h);
        parent->AddCloseListener(t, &TcpTransportConnection::Close);
    }
    catch (std::exception&)
    {
        RobotRaconteurNode::TryPostToThreadPool(
            parent->GetNode(),
            boost::bind(callback, RR_SHARED_PTR<boost::asio::ip::tcp::socket>(),
                        RR_SHARED_PTR<TcpTransportConnection>(),
                        RR_MAKE_SHARED<ConnectionException>("Could not connect to service")),
            true);
    }
}

void TcpTransport::register_transport(const RR_SHARED_PTR<ITransportConnection>& connection)
{
    boost::mutex::scoped_lock lock(TransportConnections_lock);
    TransportConnections.insert(std::make_pair(connection->GetLocalEndpoint(), connection));
    // RR_WEAK_PTR<ITransportConnection> w=connection;
    // std::remove(incoming_TransportConnections.begin(), incoming_TransportConnections.end(), w);
}

void TcpTransport::incoming_transport(const RR_SHARED_PTR<ITransportConnection>& connection)
{
    RR_UNUSED(connection);
    boost::mutex::scoped_lock lock(TransportConnections_lock);
    // incoming_TransportConnections.push_back(connection);
}

void TcpTransport::erase_transport(const RR_SHARED_PTR<ITransportConnection>& connection)
{
    try
    {
        boost::mutex::scoped_lock lock(TransportConnections_lock);
        if (TransportConnections.count(connection->GetLocalEndpoint()) != 0)
        {
            if (TransportConnections.at(connection->GetLocalEndpoint()) == connection)
            {

                TransportConnections.erase(connection->GetLocalEndpoint());
            }
        }
    }
    catch (std::exception&)
    {}

    int32_t connection_count = 0;
    {
        boost::mutex::scoped_lock lock(TransportConnections_lock);
        connection_count = boost::numeric_cast<int32_t>(TransportConnections.size());
    }

    int32_t max_connection_count = GetMaxConnectionCount();

    boost::mutex::scoped_lock lock(acceptor_lock);
    if (max_connection_count > 0)
    {
        if (connection_count < max_connection_count)
        {
            if (ipv4_acceptor_paused)
            {
                RR_SHARED_PTR<boost::asio::ip::tcp::socket> socket2(
                    new boost::asio::ip::tcp::socket(GetNode()->GetThreadPool()->get_io_context()));
                ipv4_acceptor->async_accept(*socket2,
                                            boost::bind(&TcpTransport::handle_v4_accept, shared_from_this(),
                                                        ipv4_acceptor, socket2, boost::asio::placeholders::error));
                ipv4_acceptor_paused = false;
                ROBOTRACONTEUR_LOG_INFO_COMPONENT(node, Transport, -1, "TcpTransport resuming IPv4 accept");
            }

            if (ipv6_acceptor_paused)
            {
                RR_SHARED_PTR<boost::asio::ip::tcp::socket> socket2(
                    new boost::asio::ip::tcp::socket(GetNode()->GetThreadPool()->get_io_context()));
                ipv6_acceptor->async_accept(*socket2,
                                            boost::bind(&TcpTransport::handle_v6_accept, shared_from_this(),
                                                        ipv6_acceptor, socket2, boost::asio::placeholders::error));
                ipv6_acceptor_paused = false;
                ROBOTRACONTEUR_LOG_INFO_COMPONENT(node, Transport, -1, "TcpTransport resuming IPv6 accept");
            }
        }
    }

    TransportConnectionClosed(connection->GetLocalEndpoint());
}

void TcpTransport::MessageReceived(const RR_INTRUSIVE_PTR<Message>& m) { GetNode()->MessageReceived(m); }

void TcpTransport::EnableNodeDiscoveryListening(uint32_t flags)
{

    boost::mutex::scoped_lock lock(node_discovery_lock);
    if (!node_discovery)
    {
        node_discovery = RR_MAKE_SHARED<detail::IPNodeDiscovery>(shared_from_this());
    }
    boost::reinterpret_pointer_cast<detail::IPNodeDiscovery>(node_discovery)->StartListeningForNodes(flags);

    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, -1,
                                       "Node discovery listening enabled with flags 0x" << std::hex << flags);
}

void TcpTransport::DisableNodeDiscoveryListening()
{
    boost::mutex::scoped_lock lock(node_discovery_lock);
    if (!node_discovery)
        return;
    boost::reinterpret_pointer_cast<detail::IPNodeDiscovery>(node_discovery)->StopListeningForNodes();

    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, -1, "Node discovery listening disabled");
}

void TcpTransport::EnableNodeAnnounce(uint32_t flags)
{
    boost::mutex::scoped_lock lock(node_discovery_lock);
    if (!node_discovery)
    {
        node_discovery = RR_MAKE_SHARED<detail::IPNodeDiscovery>(shared_from_this());
    }
    boost::reinterpret_pointer_cast<detail::IPNodeDiscovery>(node_discovery)->StartAnnouncingNode(flags);

    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, -1, "Node announce enabled with flags 0x" << std::hex << flags);
}

void TcpTransport::DisableNodeAnnounce()
{
    boost::mutex::scoped_lock lock(node_discovery_lock);
    if (node_discovery == 0)
        return;
    boost::reinterpret_pointer_cast<detail::IPNodeDiscovery>(node_discovery)->StopAnnouncingNode();
    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, -1, "Node announce disabled");
}

int32_t TcpTransport::GetNodeAnnouncePeriod()
{
    boost::mutex::scoped_lock lock(node_discovery_lock);
    if (!node_discovery)
    {
        node_discovery = RR_MAKE_SHARED<detail::IPNodeDiscovery>(shared_from_this());
    }
    return boost::reinterpret_pointer_cast<detail::IPNodeDiscovery>(node_discovery)->GetNodeAnnouncePeriod();
}

void TcpTransport::SetNodeAnnouncePeriod(int32_t millis)
{
    boost::mutex::scoped_lock lock(node_discovery_lock);
    if (!node_discovery)
    {
        node_discovery = RR_MAKE_SHARED<detail::IPNodeDiscovery>(shared_from_this());
    }
    boost::reinterpret_pointer_cast<detail::IPNodeDiscovery>(node_discovery)->SetNodeAnnouncePeriod(millis);

    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, -1, "Node announce period set to " << millis << " ms");
}

void TcpTransport::SendDiscoveryRequest()
{
    boost::mutex::scoped_lock lock(node_discovery_lock);
    if (node_discovery)
    {
        boost::reinterpret_pointer_cast<detail::IPNodeDiscovery>(node_discovery)->SendDiscoveryRequestNow();
    }
}

bool TcpTransport::IsTransportConnectionSecure(uint32_t endpoint)
{
    RR_SHARED_PTR<ITransportConnection> t;
    {
        boost::mutex::scoped_lock lock(TransportConnections_lock);
        RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<ITransportConnection> >::iterator e1 =
            TransportConnections.find(endpoint);
        if (e1 == TransportConnections.end())
        {
            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, endpoint,
                                               "Transport connection to remote host not found");
            throw ConnectionException("Transport connection to remote host not found");
        }
        t = e1->second;
    }

    return RR_DYNAMIC_POINTER_CAST<TcpTransportConnection>(t)->IsSecure();
}

bool TcpTransport::IsTransportConnectionSecure(const RR_SHARED_PTR<Endpoint>& endpoint)
{
    if (!endpoint)
        return false;

    RR_SHARED_PTR<ITransportConnection> t;
    {
        boost::mutex::scoped_lock lock(TransportConnections_lock);
        RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<ITransportConnection> >::iterator e1 =
            TransportConnections.find(endpoint->GetLocalEndpoint());
        if (e1 == TransportConnections.end())
        {
            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, endpoint->GetLocalEndpoint(),
                                               "Transport connection to remote host not found");
            throw ConnectionException("Transport connection to remote host not found");
        }
        t = e1->second;
    }

    return RR_DYNAMIC_POINTER_CAST<TcpTransportConnection>(t)->IsSecure();
}

bool TcpTransport::IsTransportConnectionSecure(const RR_SHARED_PTR<RRObject>& obj)
{
    RR_SHARED_PTR<ServiceStub> stub = RR_DYNAMIC_POINTER_CAST<ServiceStub>(obj);
    if (!stub)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, -1, "Object is not a connection");
        throw InvalidArgumentException("Object is not a connection");
    }
    RR_SHARED_PTR<Endpoint> endpoint = stub->GetContext();

    RR_SHARED_PTR<ITransportConnection> t;

    {
        boost::mutex::scoped_lock lock(TransportConnections_lock);
        RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<ITransportConnection> >::iterator e1 =
            TransportConnections.find(endpoint->GetLocalEndpoint());
        if (e1 == TransportConnections.end())
        {
            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, endpoint->GetLocalEndpoint(),
                                               "Transport connection to remote host not found");
            throw ConnectionException("Transport connection to remote host not found");
        }
        t = e1->second;
    }

    return RR_DYNAMIC_POINTER_CAST<TcpTransportConnection>(t)->IsSecure();
}

bool TcpTransport::IsTransportConnectionSecure(const RR_SHARED_PTR<ITransportConnection>& transport)
{
    RR_SHARED_PTR<TcpTransportConnection> t = RR_DYNAMIC_POINTER_CAST<TcpTransportConnection>(transport);
    if (!t)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, -1, "Invalid transport connection type");
        throw InvalidArgumentException("Invalid transport connection type");
    }

    return t->IsSecure();
}

bool TcpTransport::IsSecurePeerIdentityVerified(uint32_t endpoint)
{
    RR_SHARED_PTR<ITransportConnection> t;
    {
        boost::mutex::scoped_lock lock(TransportConnections_lock);
        RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<ITransportConnection> >::iterator e1 =
            TransportConnections.find(endpoint);
        if (e1 == TransportConnections.end())
        {
            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, endpoint,
                                               "Transport connection to remote host not found");
            throw ConnectionException("Transport connection to remote host not found");
        }
        t = e1->second;
    }

    return RR_DYNAMIC_POINTER_CAST<TcpTransportConnection>(t)->IsSecurePeerIdentityVerified();
}

bool TcpTransport::IsSecurePeerIdentityVerified(const RR_SHARED_PTR<Endpoint>& endpoint)
{

    if (!endpoint)
        return false;

    RR_SHARED_PTR<ITransportConnection> t;
    {
        boost::mutex::scoped_lock lock(TransportConnections_lock);
        RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<ITransportConnection> >::iterator e1 =
            TransportConnections.find(endpoint->GetLocalEndpoint());
        if (e1 == TransportConnections.end())
        {
            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, endpoint->GetLocalEndpoint(),
                                               "Transport connection to remote host not found");
            throw ConnectionException("Transport connection to remote host not found");
        }
        t = e1->second;
    }

    return RR_DYNAMIC_POINTER_CAST<TcpTransportConnection>(t)->IsSecurePeerIdentityVerified();
}

bool TcpTransport::IsSecurePeerIdentityVerified(const RR_SHARED_PTR<RRObject>& obj)
{
    RR_SHARED_PTR<ServiceStub> stub = RR_DYNAMIC_POINTER_CAST<ServiceStub>(obj);
    if (!stub)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, -1, "Object is not a connection");
        throw InvalidArgumentException("Object is not a connection");
    }
    RR_SHARED_PTR<Endpoint> endpoint = stub->GetContext();

    RR_SHARED_PTR<ITransportConnection> t;
    {
        boost::mutex::scoped_lock lock(TransportConnections_lock);
        RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<ITransportConnection> >::iterator e1 =
            TransportConnections.find(endpoint->GetLocalEndpoint());
        if (e1 == TransportConnections.end())
        {
            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, endpoint->GetLocalEndpoint(),
                                               "Transport connection to remote host not found");
            throw ConnectionException("Transport connection to remote host not found");
        }
        t = e1->second;
    }

    return RR_DYNAMIC_POINTER_CAST<TcpTransportConnection>(t)->IsSecurePeerIdentityVerified();
}

bool TcpTransport::IsSecurePeerIdentityVerified(const RR_SHARED_PTR<ITransportConnection>& transport)
{
    RR_SHARED_PTR<TcpTransportConnection> t = RR_DYNAMIC_POINTER_CAST<TcpTransportConnection>(transport);
    if (!t)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, -1, "Invalid transport connection type");
        throw InvalidArgumentException("Invalid transport connection type");
    }

    return t->IsSecurePeerIdentityVerified();
}

std::string TcpTransport::GetSecurePeerIdentity(uint32_t endpoint)
{
    RR_SHARED_PTR<ITransportConnection> t;
    {
        boost::mutex::scoped_lock lock(TransportConnections_lock);
        RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<ITransportConnection> >::iterator e1 =
            TransportConnections.find(endpoint);
        if (e1 == TransportConnections.end())
        {
            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, endpoint,
                                               "Transport connection to remote host not found");
            throw ConnectionException("Transport connection to remote host not found");
        }
        t = e1->second;
    }

    return RR_DYNAMIC_POINTER_CAST<TcpTransportConnection>(t)->GetSecurePeerIdentity();
}

std::string TcpTransport::GetSecurePeerIdentity(const RR_SHARED_PTR<Endpoint>& endpoint)
{

    if (!endpoint)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, -1, "Transport connection to remote host not found");
        throw ConnectionException("Transport connection to remote host not found");
    }

    RR_SHARED_PTR<ITransportConnection> t;
    {
        boost::mutex::scoped_lock lock(TransportConnections_lock);
        RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<ITransportConnection> >::iterator e1 =
            TransportConnections.find(endpoint->GetLocalEndpoint());
        if (e1 == TransportConnections.end())
        {
            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, endpoint->GetLocalEndpoint(),
                                               "Transport connection to remote host not found");
            throw ConnectionException("Transport connection to remote host not found");
        }
        t = e1->second;
    }

    return RR_DYNAMIC_POINTER_CAST<TcpTransportConnection>(t)->GetSecurePeerIdentity();
}

std::string TcpTransport::GetSecurePeerIdentity(const RR_SHARED_PTR<RRObject>& obj)
{
    RR_SHARED_PTR<ServiceStub> stub = RR_DYNAMIC_POINTER_CAST<ServiceStub>(obj);
    if (!stub)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, -1, "Object is not a connection");
        throw InvalidArgumentException("Object is not a connection");
    }
    RR_SHARED_PTR<Endpoint> endpoint = stub->GetContext();

    RR_SHARED_PTR<ITransportConnection> t;
    {
        boost::mutex::scoped_lock lock(TransportConnections_lock);
        RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<ITransportConnection> >::iterator e1 =
            TransportConnections.find(endpoint->GetLocalEndpoint());
        if (e1 == TransportConnections.end())
        {
            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, endpoint->GetLocalEndpoint(),
                                               "Transport connection to remote host not found");
            throw ConnectionException("Transport connection to remote host not found");
        }
        t = e1->second;
    }

    return RR_DYNAMIC_POINTER_CAST<TcpTransportConnection>(t)->GetSecurePeerIdentity();
}

std::string TcpTransport::GetSecurePeerIdentity(const RR_SHARED_PTR<ITransportConnection>& transport)
{
    RR_SHARED_PTR<TcpTransportConnection> t = RR_DYNAMIC_POINTER_CAST<TcpTransportConnection>(transport);
    if (!t)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, -1, "Invalid transport connection type");
        throw InvalidArgumentException("Invalid transport connection type");
    }

    return t->GetSecurePeerIdentity();
}

bool TcpTransport::GetDisableMessage4()
{
    boost::mutex::scoped_lock lock(parameter_lock);
    return disable_message4;
}
void TcpTransport::SetDisableMessage4(bool d)
{
    boost::mutex::scoped_lock lock(parameter_lock);
    disable_message4 = d;
    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, -1, "DisableMessage4 set to: " << d);
}

bool TcpTransport::GetDisableStringTable()
{
    boost::mutex::scoped_lock lock(parameter_lock);
    return disable_string_table;
}
void TcpTransport::SetDisableStringTable(bool d)
{
    boost::mutex::scoped_lock lock(parameter_lock);
    disable_string_table = d;
    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, -1, "DisableStringTable set to: " << d);
}

bool TcpTransport::GetDisableAsyncMessageIO()
{
    boost::mutex::scoped_lock lock(parameter_lock);
    return disable_async_message_io;
}
void TcpTransport::SetDisableAsyncMessageIO(bool d)
{
    boost::mutex::scoped_lock lock(parameter_lock);
    disable_async_message_io = d;
    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, -1, "DisableAsyncMessageIO set to: " << d);
}

void TcpTransport::LocalNodeServicesChanged()
{
    boost::mutex::scoped_lock lock(node_discovery_lock);
    if (node_discovery)
    {
        boost::reinterpret_pointer_cast<detail::IPNodeDiscovery>(node_discovery)->SendAnnounceNow();
    }
}

TcpTransportConnection::TcpTransportConnection(const RR_SHARED_PTR<TcpTransport>& parent, boost::string_ref url,
                                               bool server, uint32_t local_endpoint)
    : ASIOStreamBaseTransport(parent->GetNode())
{
    this->parent = parent;
    this->server = server;
    this->m_LocalEndpoint = local_endpoint;
    this->m_RemoteEndpoint = 0;
    this->ReceiveTimeout = parent->GetDefaultReceiveTimeout();
    this->HeartbeatPeriod = parent->GetDefaultHeartbeatPeriod();
    this->disable_message4 = parent->GetDisableMessage4();
    this->disable_string_table = parent->GetDisableStringTable();
    this->disable_async_io = parent->GetDisableAsyncMessageIO();
    this->url = RR_MOVE(url.to_string());

    this->is_tls = false;
    this->require_tls = parent->GetRequireTls();
    this->closing = false;
    this->max_message_size = parent->GetMaxMessageSize();
    this->tls_mutual_auth = false;
    this->tls_handshaking = false;
    this->use_websocket = false;
    this->use_wss_websocket = false;
}

void TcpTransportConnection::AsyncAttachSocket(
    const RR_SHARED_PTR<boost::asio::ip::tcp::socket>& socket,
    const boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>&)>& callback)
{
    this->socket = socket;

    try
    {
        this->socket->set_option(boost::asio::ip::tcp::socket::linger(true, 5));
    }
    catch (std::exception&)
    {}

    int send_timeout = 15000;
    setsockopt(socket->native_handle(), SOL_SOCKET, SO_SNDTIMEO, reinterpret_cast<const char*>(&send_timeout),
               sizeof(send_timeout));
    socket->set_option(boost::asio::ip::tcp::no_delay(true));

    std::string noden;
    if (!server)
    {
        ParseConnectionURLResult url_res = ParseConnectionURL(url);

        target_nodeid = url_res.nodeid;

        target_nodename = url_res.nodename;
        if (!(url_res.nodeid.IsAnyNode() && !url_res.nodename.empty()))
        {
            noden = url_res.nodeid.ToString();
        }
        else
        {
            noden = url_res.nodename;
        }
    }
    if (!server && boost::starts_with(url, "rrs"))
    {
        this->send_paused = true;
        this->send_pause_request = true;

        heartbeat_timer = RR_SHARED_PTR<boost::asio::deadline_timer>(new boost::asio::deadline_timer(_io_context));
        {
            boost::mutex::scoped_lock lock(recv_lock);
            BeginReceiveMessage1();
        }

        string_table4 = RR_MAKE_SHARED<detail::StringTable>(server);

        boost::system::error_code ec;
        if (!RobotRaconteurNode::TryPostToThreadPool(
                node,
                boost::bind(&TcpTransportConnection::do_starttls1,
                            RR_STATIC_POINTER_CAST<TcpTransportConnection>(shared_from_this()), noden, ec, callback)))
        {
            RobotRaconteurNode::TryPostToThreadPool(
                node, boost::bind(callback, RR_MAKE_SHARED<ConnectionException>("Node closed")), true);
        }

        return;
    }

    if (!server)
    {
        ASIOStreamBaseTransport::AsyncAttachStream(server, target_nodeid, target_nodename, callback);
        return;
    }
    else
    {
        ASIOStreamBaseTransport::AsyncAttachStream(server, target_nodeid, target_nodename, callback);
    }
}

void TcpTransportConnection::AsyncAttachWebSocket(
    const RR_SHARED_PTR<boost::asio::ip::tcp::socket>& socket,
    const RR_SHARED_PTR<detail::websocket_stream<boost::asio::ip::tcp::socket&> >& websocket,
    const boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>&)>& callback)
{
    this->socket = socket;
    this->websocket = websocket;
    use_websocket = true;

    try
    {
        this->socket->set_option(boost::asio::ip::tcp::socket::linger(true, 5));
    }
    catch (std::exception&)
    {}

    int send_timeout = 15000;
    setsockopt(socket->native_handle(), SOL_SOCKET, SO_SNDTIMEO, reinterpret_cast<const char*>(&send_timeout),
               sizeof(send_timeout));
    socket->set_option(boost::asio::ip::tcp::no_delay(true));

    std::string noden;
    if (!server)
    {
        ParseConnectionURLResult url_res = ParseConnectionURL(url);

        target_nodeid = url_res.nodeid;

        target_nodename = url_res.nodename;

        if (!(url_res.nodeid.IsAnyNode() && !url_res.nodename.empty()))
        {
            noden = url_res.nodeid.ToString();
        }
        else
        {
            noden = url_res.nodename;
        }
    }
    if (!server && boost::starts_with(url, "rrs+ws://"))
    {
        this->send_paused = true;
        this->send_pause_request = true;

        string_table4 = RR_MAKE_SHARED<detail::StringTable>(server);

        heartbeat_timer = RR_SHARED_PTR<boost::asio::deadline_timer>(new boost::asio::deadline_timer(_io_context));
        {
            boost::mutex::scoped_lock lock(recv_lock);
            BeginReceiveMessage1();
        }

        boost::system::error_code ec;
        if (!RobotRaconteurNode::TryPostToThreadPool(
                node,
                boost::bind(&TcpTransportConnection::do_starttls1,
                            RR_STATIC_POINTER_CAST<TcpTransportConnection>(shared_from_this()), noden, ec, callback)))
        {
            RobotRaconteurNode::TryPostToThreadPool(
                node, boost::bind(callback, RR_MAKE_SHARED<ConnectionException>("Node closed")), true);
        }

        return;
    }

    if (!server)
    {
        ASIOStreamBaseTransport::AsyncAttachStream(server, target_nodeid, target_nodename, callback);
        return;
    }
    else
    {
        ASIOStreamBaseTransport::AsyncAttachStream(server, target_nodeid, target_nodename, callback);
    }
}

#ifdef ROBOTRACONTEUR_USE_SCHANNEL
void TcpTransportConnection::AsyncAttachWSSWebSocket(
    const RR_SHARED_PTR<boost::asio::ip::tcp::socket>& socket,
    const RR_SHARED_PTR<detail::TlsSchannelAsyncStreamAdapter>& wss_websocket_tls,
    const RR_SHARED_PTR<detail::websocket_stream<detail::TlsSchannelAsyncStreamAdapter_ASIO_adapter&> >& wss_websocket,
    const boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>&)>& callback)
{
    this->socket = socket;

    this->wss_websocket = wss_websocket;
    this->wss_websocket_tls = wss_websocket_tls;
    use_wss_websocket = true;

    try
    {
        this->socket->set_option(boost::asio::ip::tcp::socket::linger(true, 5));
    }
    catch (std::exception&)
    {}

    int send_timeout = 15000;
    setsockopt(socket->native_handle(), SOL_SOCKET, SO_SNDTIMEO, (const char*)&send_timeout, sizeof(send_timeout));
    socket->set_option(boost::asio::ip::tcp::no_delay(true));

    std::string noden;
    if (!server)
    {
        ParseConnectionURLResult url_res = ParseConnectionURL(url);

        target_nodeid = url_res.nodeid;

        target_nodename = url_res.nodename;

        if (!(url_res.nodeid.IsAnyNode() && url_res.nodename != ""))
        {
            noden = url_res.nodeid.ToString();
        }
        else
        {
            noden = url_res.nodename;
        }
    }
    if (!server && boost::starts_with(url, "rrs+wss://"))
    {
        this->send_paused = true;
        this->send_pause_request = true;

        string_table4 = RR_MAKE_SHARED<detail::StringTable>(server);

        heartbeat_timer.reset(new boost::asio::deadline_timer(_io_context));
        {
            boost::mutex::scoped_lock lock(recv_lock);
            BeginReceiveMessage1();
        }

        boost::system::error_code ec;
        if (!RobotRaconteurNode::TryPostToThreadPool(
                node,
                boost::bind(&TcpTransportConnection::do_starttls1,
                            RR_STATIC_POINTER_CAST<TcpTransportConnection>(shared_from_this()), noden, ec, callback)))
        {
            RobotRaconteurNode::TryPostToThreadPool(
                node, boost::bind(callback, RR_MAKE_SHARED<ConnectionException>("Node closed")), true);
        }
        return;
    }

    if (!server)
    {
        ASIOStreamBaseTransport::AsyncAttachStream(server, target_nodeid, target_nodename, callback);
        return;
    }
    else
    {
        ASIOStreamBaseTransport::AsyncAttachStream(server, target_nodeid, target_nodename, callback);
    }
}
#endif

#ifdef ROBOTRACONTEUR_USE_OPENSSL
void TcpTransportConnection::AsyncAttachWSSWebSocket(
    const RR_SHARED_PTR<boost::asio::ip::tcp::socket>& socket,
    const RR_SHARED_PTR<boost::asio::ssl::stream<boost::asio::ip::tcp::socket&> >& wss_websocket_tls,
    const RR_SHARED_PTR<detail::websocket_stream<boost::asio::ssl::stream<boost::asio::ip::tcp::socket&>&> >&
        wss_websocket,
    const RR_SHARED_PTR<boost::asio::ssl::context>& wss_context,
    const boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>&)>& callback)
{
    this->socket = socket;

    this->wss_websocket = wss_websocket;
    this->wss_websocket_tls = wss_websocket_tls;
    this->wss_context = wss_context;
    use_wss_websocket = true;

    try
    {
        this->socket->set_option(boost::asio::ip::tcp::socket::linger(true, 5));
    }
    catch (std::exception&)
    {}

    int send_timeout = 15000;
    setsockopt(socket->native_handle(), SOL_SOCKET, SO_SNDTIMEO, reinterpret_cast<const char*>(&send_timeout),
               sizeof(send_timeout));
    socket->set_option(boost::asio::ip::tcp::no_delay(true));

    std::string noden;
    if (!server)
    {
        ParseConnectionURLResult url_res = ParseConnectionURL(url);

        target_nodeid = url_res.nodeid;

        target_nodename = url_res.nodename;

        if (!(url_res.nodeid.IsAnyNode() && !url_res.nodename.empty()))
        {
            noden = url_res.nodeid.ToString();
        }
        else
        {
            noden = url_res.nodename;
        }
    }
    if (!server && boost::starts_with(url, "rrs+wss://"))
    {
        this->send_paused = true;
        this->send_pause_request = true;

        heartbeat_timer = RR_SHARED_PTR<boost::asio::deadline_timer>(new boost::asio::deadline_timer(_io_context));
        {
            boost::mutex::scoped_lock lock(recv_lock);
            BeginReceiveMessage1();
        }

        boost::system::error_code ec;
        if (!RobotRaconteurNode::TryPostToThreadPool(
                node,
                boost::bind(&TcpTransportConnection::do_starttls1,
                            RR_STATIC_POINTER_CAST<TcpTransportConnection>(shared_from_this()), noden, ec, callback)))
        {
            detail::PostHandlerWithException(node, callback, RR_MAKE_SHARED<ConnectionException>("Node closed"), true,
                                             false);
        }
        return;
    }

    if (!server)
    {
        ASIOStreamBaseTransport::AsyncAttachStream(server, target_nodeid, target_nodename, callback);
        return;
    }
    else
    {
        ASIOStreamBaseTransport::AsyncAttachStream(server, target_nodeid, target_nodename, callback);
    }
}
#endif

/*void TcpTransportConnection::do_starttls3(const RR_SHARED_PTR<RobotRaconteurException>& error,
const boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>&)>& callback)
{
    if (error)
    {
        callback(error);
        return;
    }

    AsyncPauseSend(boost::bind(&TcpTransportConnection::do_starttls1,
RR_STATIC_POINTER_CAST<TcpTransportConnection>(shared_from_this()),
RR_BOOST_PLACEHOLDERS(_1),boost::protect(callback)));
}*/

void TcpTransportConnection::do_starttls1(
    const std::string& noden, const boost::system::error_code& error,
    const boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>&)>& callback)
{
    if (error)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, m_LocalEndpoint,
                                           "Could not initialize TLS connection: " << error.message());
        callback(RR_MAKE_SHARED<ConnectionException>("Could not initialize TLS connection"));
        return;
    }

    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, m_LocalEndpoint, "Begin client send STARTTLS");

    try
    {
        // TODO: Fix the node id
        boost::mutex::scoped_lock lock(streamop_lock);
        // TODO: cancel if closed

        NodeID target_nodeid = NodeID::GetAny();
        std::string target_nodename;
        if (noden.find('{') != std::string::npos)
        {
            target_nodeid = NodeID(noden);
        }
        else
        {
            target_nodename = noden;
        }

        RR_INTRUSIVE_PTR<Message> m = CreateMessage();
        m->header = CreateMessageHeader();
        m->header->ReceiverNodeName = target_nodename;
        // m->header->SenderNodeName = GetNode()->NodeName();
        // m->header->SenderNodeID = GetNode()->NodeID();
        m->header->ReceiverNodeID = target_nodeid;

        RR_INTRUSIVE_PTR<MessageEntry> mm = CreateMessageEntry(MessageEntryType_StreamOp, "STARTTLS");

        RR_SHARED_PTR<TcpTransport> p = parent.lock();
        if (!p)
            throw InvalidOperationException("Transport shutdown");

#ifdef ROBOTRACONTEUR_USE_SCHANNEL
        RR_SHARED_PTR<detail::TlsSchannelAsyncStreamAdapterContext> context =
            RR_STATIC_POINTER_CAST<detail::TlsSchannelAsyncStreamAdapterContext>(p->GetTlsContext());
        if (context->IsCertificateLoaded())
        {
            ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, m_LocalEndpoint,
                                               "Client requesting mutual certificate authentication");
            mm->AddElement("mutualauth", stringToRRArray("true"));
        }
#endif

#ifdef ROBOTRACONTEUR_USE_OPENSSL
        RR_SHARED_PTR<detail::OpenSSLAuthContext> context =
            RR_STATIC_POINTER_CAST<detail::OpenSSLAuthContext>(p->GetTlsContext());
        if (context->IsCertificateLoaded())
        {
            ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, m_LocalEndpoint,
                                               "Client requesting mutual certificate authentication");
            mm->AddElement("mutualauth", stringToRRArray("true"));
        }
#endif

        m->entries.push_back(mm);

        if (starttls_timer)
        {
            lock.unlock();
            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, m_LocalEndpoint, "Could not initialize TLS connection");
            callback(RR_MAKE_SHARED<ConnectionException>("Could not initialize TLS connection"));
            return;
        }

        starttls_handler = callback;

        starttls_timer = RR_SHARED_PTR<boost::asio::deadline_timer>(
            new boost::asio::deadline_timer(_io_context, boost::posix_time::milliseconds(5000)));
        // RR_WEAK_PTR<ASIOStreamBaseTransport> t=RR_STATIC_POINTER_CAST<ASIOStreamBaseTransport>(shared_from_this());
        boost::system::error_code ec1(boost::system::errc::timed_out, boost::system::generic_category());
        RobotRaconteurNode::asio_async_wait(
            node, starttls_timer,
            boost::bind(&TcpTransportConnection::do_starttls4,
                        RR_STATIC_POINTER_CAST<TcpTransportConnection>(shared_from_this()), "", ec1));
        streamop_waiting = true;
        tls_handshaking = true;
        lock.unlock();
        {
            boost::mutex::scoped_lock lock2(send_lock);
            boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>&)> h =
                boost::bind(&TcpTransportConnection::do_starttls2,
                            RR_STATIC_POINTER_CAST<TcpTransportConnection>(shared_from_this()),
                            RR_BOOST_PLACEHOLDERS(_1), callback);
            BeginSendMessage(m, h);
        }

        ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, m_LocalEndpoint, "Client sending STARTTLS");
    }
    catch (std::exception& exp)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, m_LocalEndpoint,
                                           "Client failed sending STARTTLS: " << exp.what());
        {
            boost::mutex::scoped_lock lock(streamop_lock);
            starttls_handler.clear();
            starttls_timer.reset();
            streamop_waiting = false;
            tls_handshaking = false;
        }
        callback(RR_MAKE_SHARED<ConnectionException>("Could not initialize TLS connection"));
        return;
    }

    // callback(RR_SHARED_PTR<RobotRaconteurException>());
}

void TcpTransportConnection::do_starttls2(
    const RR_SHARED_PTR<RobotRaconteurException>& error,
    const boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>&)>& callback)
{
    if (error)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, m_LocalEndpoint,
                                           "Client failed sending STARTTLS: " << error->what());
        {
            boost::mutex::scoped_lock lock(streamop_lock);
            starttls_handler.clear();
            starttls_timer.reset();
        }
        callback(RR_MAKE_SHARED<ConnectionException>("Could not initialize TLS connection"));
        return;
    }

    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, m_LocalEndpoint, "Client sent STARTTLS");
}

namespace detail
{
static void TcpTransportConnection_websocket_read_adapter(
    detail::websocket_stream<boost::asio::ip::tcp::socket&>* sock, mutable_buffers& b,
    const boost::function<void(const boost::system::error_code&, size_t)>& handler)
{
    sock->async_read_some(b, handler);
}

static void TcpTransportConnection_websocket_write_adapter(
    detail::websocket_stream<boost::asio::ip::tcp::socket&>* sock, const_buffers& b,
    const boost::function<void(const boost::system::error_code&, size_t)>& handler)
{
    sock->async_write_some(b, handler);
}

static void TcpTransportConnection_websocket_close_adapter(
    detail::websocket_stream<boost::asio::ip::tcp::socket&>* sock)
{
    sock->close();
}

#ifdef ROBOTRACONTEUR_USE_SCHANNEL
static void TcpTransportConnection_wss_websocket_read_adapter(
    detail::websocket_stream<detail::TlsSchannelAsyncStreamAdapter_ASIO_adapter&>* sock, mutable_buffers& b,
    const boost::function<void(const boost::system::error_code&, size_t)>& handler)
{
    sock->async_read_some(b, handler);
}

static void TcpTransportConnection_wss_websocket_write_adapter(
    detail::websocket_stream<detail::TlsSchannelAsyncStreamAdapter_ASIO_adapter&>* sock, const_buffers& b,
    const boost::function<void(const boost::system::error_code&, size_t)>& handler)
{
    sock->async_write_some(b, handler);
}

static void TcpTransportConnection_wss_websocket_close_adapter(
    detail::websocket_stream<detail::TlsSchannelAsyncStreamAdapter_ASIO_adapter&>* sock)
{
    sock->close();
}
#endif
} // namespace detail

void TcpTransportConnection::do_starttls4(const std::string& servername, const boost::system::error_code& error)
{
    RR_UNUSED(servername);
    boost::mutex::scoped_lock lock(streamop_lock);

    boost::system::error_code ec2 = error;

    if (!tls_handshaking)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, m_LocalEndpoint, "Client received unexpected STARTTLS");
        return;
    }
    tls_handshaking = false;

    if (ec2)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, m_LocalEndpoint,
                                           "Client failed receiving STARTTLS: " << ec2.message());
        boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>&)> starttls_handler1 = starttls_handler;
        starttls_handler.clear();
        starttls_timer.reset();
        lock.unlock();
        detail::InvokeHandlerWithException(node, starttls_handler,
                                           RR_MAKE_SHARED<ConnectionException>("Could not initialize TLS connection"));
        return;
    }

    try
    {
        ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, m_LocalEndpoint, "Client begin TLS handshake");
        starttls_timer.reset();

        RR_SHARED_PTR<TcpTransport> p = parent.lock();
        if (!p)
            throw InvalidOperationException("Transport shutdown");

#ifdef ROBOTRACONTEUR_USE_SCHANNEL
        if (!use_websocket && !use_wss_websocket)
        {
            RR_WEAK_PTR<boost::asio::ip::tcp::socket> socket1 = socket;
            RR_SHARED_PTR<detail::TlsSchannelAsyncStreamAdapterContext> context =
                RR_STATIC_POINTER_CAST<detail::TlsSchannelAsyncStreamAdapterContext>(p->GetTlsContext());
            tls_socket = RR_MAKE_SHARED<RobotRaconteur::detail::TlsSchannelAsyncStreamAdapter>(
                boost::ref(GetNode()->GetThreadPool()->get_io_context()), context,
                RobotRaconteur::detail::TlsSchannelAsyncStreamAdapter::client, servername,
                boost::bind(detail::TcpTransportConnection_socket_read_adapter, node, socket1,
                            RR_BOOST_PLACEHOLDERS(_1), RR_BOOST_PLACEHOLDERS(_2)),
                boost::bind(detail::TcpTransportConnection_socket_write_adapter, node, socket1,
                            RR_BOOST_PLACEHOLDERS(_1), RR_BOOST_PLACEHOLDERS(_2)),
                boost::bind(detail::TcpTransportConnection_socket_close_adapter, socket1));
            RobotRaconteurNode::asio_async_handshake(
                node, tls_socket,
                boost::bind(&TcpTransportConnection::do_starttls5,
                            RR_STATIC_POINTER_CAST<TcpTransportConnection>(shared_from_this()),
                            RR_BOOST_PLACEHOLDERS(_1)));
        }
        else if (use_websocket)
        {
            RR_SHARED_PTR<detail::TlsSchannelAsyncStreamAdapterContext> context =
                RR_STATIC_POINTER_CAST<detail::TlsSchannelAsyncStreamAdapterContext>(p->GetTlsContext());
            tls_websocket = RR_MAKE_SHARED<RobotRaconteur::detail::TlsSchannelAsyncStreamAdapter>(
                boost::ref(GetNode()->GetThreadPool()->get_io_context()), context,
                RobotRaconteur::detail::TlsSchannelAsyncStreamAdapter::client, servername,
                boost::bind(detail::TcpTransportConnection_websocket_read_adapter, websocket.get(),
                            RR_BOOST_PLACEHOLDERS(_1), RR_BOOST_PLACEHOLDERS(_2)),
                boost::bind(detail::TcpTransportConnection_websocket_write_adapter, websocket.get(),
                            RR_BOOST_PLACEHOLDERS(_1), RR_BOOST_PLACEHOLDERS(_2)),
                boost::bind(detail::TcpTransportConnection_websocket_close_adapter, websocket.get()));
            RobotRaconteurNode::asio_async_handshake(
                node, tls_websocket,
                boost::bind(&TcpTransportConnection::do_starttls5,
                            RR_STATIC_POINTER_CAST<TcpTransportConnection>(shared_from_this()),
                            RR_BOOST_PLACEHOLDERS(_1)));
        }
        else
        {
            RR_SHARED_PTR<detail::TlsSchannelAsyncStreamAdapterContext> context =
                RR_STATIC_POINTER_CAST<detail::TlsSchannelAsyncStreamAdapterContext>(p->GetTlsContext());
            tls_wss_websocket = RR_MAKE_SHARED<RobotRaconteur::detail::TlsSchannelAsyncStreamAdapter>(
                boost::ref(GetNode()->GetThreadPool()->get_io_context()), context,
                RobotRaconteur::detail::TlsSchannelAsyncStreamAdapter::client, servername,
                boost::bind(detail::TcpTransportConnection_wss_websocket_read_adapter, wss_websocket.get(),
                            RR_BOOST_PLACEHOLDERS(_1), RR_BOOST_PLACEHOLDERS(_2)),
                boost::bind(detail::TcpTransportConnection_wss_websocket_write_adapter, wss_websocket.get(),
                            RR_BOOST_PLACEHOLDERS(_1), RR_BOOST_PLACEHOLDERS(_2)),
                boost::bind(detail::TcpTransportConnection_wss_websocket_close_adapter, wss_websocket.get()));
            RobotRaconteurNode::asio_async_handshake(
                node, tls_wss_websocket,
                boost::bind(&TcpTransportConnection::do_starttls5,
                            RR_STATIC_POINTER_CAST<TcpTransportConnection>(shared_from_this()),
                            RR_BOOST_PLACEHOLDERS(_1)));
        }
#endif

#ifdef ROBOTRACONTEUR_USE_OPENSSL
        if (!use_websocket && !use_wss_websocket)
        {
            tls_context = RR_STATIC_POINTER_CAST<detail::OpenSSLAuthContext>(p->GetTlsContext());
            tls_socket = RR_MAKE_SHARED<boost::asio::ssl::stream<boost::asio::ip::tcp::socket&> >(
                boost::ref(*socket), boost::ref(*(tls_context->GetClientCredentials())));
            RobotRaconteurNode::asio_async_handshake(
                node, tls_socket, boost::asio::ssl::stream<boost::asio::ip::tcp::socket&>::client,
                boost::bind(&TcpTransportConnection::do_starttls5,
                            RR_STATIC_POINTER_CAST<TcpTransportConnection>(shared_from_this()),
                            RR_BOOST_PLACEHOLDERS(_1)));
        }
        else if (use_websocket)
        {
            tls_context = RR_STATIC_POINTER_CAST<detail::OpenSSLAuthContext>(p->GetTlsContext());
            tls_websocket =
                RR_MAKE_SHARED<boost::asio::ssl::stream<detail::websocket_stream<boost::asio::ip::tcp::socket&>&> >(
                    boost::ref(*websocket), boost::ref(*(tls_context->GetClientCredentials())));
            RobotRaconteurNode::asio_async_handshake(
                node, tls_websocket,
                boost::asio::ssl::stream<detail::websocket_stream<boost::asio::ip::tcp::socket&>&>::client,
                boost::bind(&TcpTransportConnection::do_starttls5,
                            RR_STATIC_POINTER_CAST<TcpTransportConnection>(shared_from_this()),
                            RR_BOOST_PLACEHOLDERS(_1)));
        }
        else
        {

            tls_context = RR_STATIC_POINTER_CAST<detail::OpenSSLAuthContext>(p->GetTlsContext());
            tls_wss_websocket = RR_MAKE_SHARED<boost::asio::ssl::stream<
                detail::websocket_stream<boost::asio::ssl::stream<boost::asio::ip::tcp::socket&>&>&> >(
                boost::ref(*wss_websocket), boost::ref(*(tls_context->GetClientCredentials())));
            RobotRaconteurNode::asio_async_handshake(
                node, tls_wss_websocket,
                boost::asio::ssl::stream<
                    detail::websocket_stream<boost::asio::ssl::stream<boost::asio::ip::tcp::socket&>&>&>::client,
                boost::bind(&TcpTransportConnection::do_starttls5,
                            RR_STATIC_POINTER_CAST<TcpTransportConnection>(shared_from_this()),
                            RR_BOOST_PLACEHOLDERS(_1)));
        }

#endif
    }
    catch (std::exception& exp)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, m_LocalEndpoint,
                                           "Client failed starting TLS handshake: " << exp.what());
        boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>&)> starttls_handler1 = starttls_handler;
        starttls_handler.clear();
        starttls_timer.reset();
        lock.unlock();
        detail::InvokeHandlerWithException(node, starttls_handler1,
                                           RR_MAKE_SHARED<ConnectionException>("Could not initialize TLS connection"));
        return;
    }
}

void TcpTransportConnection::do_starttls5(const boost::system::error_code& error)
{
    boost::mutex::scoped_lock lock(streamop_lock);
    if (error)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, m_LocalEndpoint,
                                           "Client TLS handshake failed: " << error.message());
        boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>&)> starttls_handler1 = starttls_handler;
        starttls_handler.clear();
        starttls_timer.reset();
        lock.unlock();
        detail::InvokeHandlerWithException(node, starttls_handler1,
                                           RR_MAKE_SHARED<ConnectionException>("Could not initialize TLS connection"));
        return;
    }

    starttls_timer.reset();

    {
        boost::mutex::scoped_lock lock1(socket_lock);
        is_tls = true;
    }

    boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>&)> starttls_handler1 = starttls_handler;
    starttls_handler.clear();
    lock.unlock();

    NodeID RemoteNodeID1;
    {
        boost::shared_lock<boost::shared_mutex> lock(RemoteNodeID_lock);
        RemoteNodeID1 = RemoteNodeID;
    }
#ifdef ROBOTRACONTEUR_USE_SCHANNEL
    if (!use_websocket && !use_wss_websocket)
    {
        if (!tls_socket->VerifyRemoteNodeCertificate(RemoteNodeID1))
        {
            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, m_LocalEndpoint,
                                               "Client could not verify server TLS certificate");
            detail::InvokeHandlerWithException(
                node, starttls_handler1,
                RR_MAKE_SHARED<AuthenticationException>("Could not verify server TLS certificate"));
            return;
        }
    }
    else if (use_websocket)
    {
        if (!tls_websocket->VerifyRemoteNodeCertificate(RemoteNodeID1))
        {
            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, m_LocalEndpoint,
                                               "Client could not verify server TLS certificate");
            detail::InvokeHandlerWithException(
                node, starttls_handler1,
                RR_MAKE_SHARED<AuthenticationException>("Could not verify server TLS certificate"));
            return;
        }
    }
    else
    {
        if (!tls_wss_websocket->VerifyRemoteNodeCertificate(RemoteNodeID1))
        {
            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, m_LocalEndpoint,
                                               "Client could not verify server TLS certificate");
            detail::InvokeHandlerWithException(
                node, starttls_handler1,
                RR_MAKE_SHARED<AuthenticationException>("Could not verify server TLS certificate"));
            return;
        }
    }
#endif

#ifdef ROBOTRACONTEUR_USE_OPENSSL

    if (!use_websocket && !use_wss_websocket)
    {
        if (!tls_context->VerifyRemoteNodeCertificate(tls_socket->native_handle(), RemoteNodeID1))
        {
            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, m_LocalEndpoint,
                                               "Client could not verify server TLS certificate");
            detail::InvokeHandlerWithException(
                node, starttls_handler1,
                RR_MAKE_SHARED<AuthenticationException>("Could not verify server TLS certificate"));
            return;
        }
    }
    else if (use_websocket)
    {
        if (!tls_context->VerifyRemoteNodeCertificate(tls_websocket->native_handle(), RemoteNodeID1))
        {
            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, m_LocalEndpoint,
                                               "Client could not verify server TLS certificate");
            detail::InvokeHandlerWithException(
                node, starttls_handler1,
                RR_MAKE_SHARED<AuthenticationException>("Could not verify server TLS certificate"));
            return;
        }
    }
    else
    {
        if (!tls_context->VerifyRemoteNodeCertificate(tls_wss_websocket->native_handle(), RemoteNodeID1))
        {
            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, m_LocalEndpoint,
                                               "Client could not verify server TLS certificate");
            detail::InvokeHandlerWithException(
                node, starttls_handler1,
                RR_MAKE_SHARED<AuthenticationException>("Could not verify server TLS certificate"));
            return;
        }
    }

#endif

    // lock.unlock();

    try
    {
        AsyncResumeSend();
        AsyncResumeReceive();
    }
    catch (std::exception& exp)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, m_LocalEndpoint,
                                           "Client failed resuming messages: " << exp.what());
        detail::InvokeHandlerWithException(node, starttls_handler1,
                                           RR_MAKE_SHARED<ConnectionException>("Could not initialize TLS connection"));
        return;
    }

    try
    {
        RR_SHARED_PTR<AsyncAttachStream_args> args =
            RR_MAKE_SHARED<AsyncAttachStream_args>(target_nodeid, target_nodename);

        boost::function<void(const RR_SHARED_PTR<RRObject>&, const RR_SHARED_PTR<RobotRaconteurException>&)> h =
            boost::bind(&TcpTransportConnection::do_starttls5_1,
                        RR_STATIC_POINTER_CAST<TcpTransportConnection>(shared_from_this()), RR_BOOST_PLACEHOLDERS(_1),
                        RR_BOOST_PLACEHOLDERS(_2), starttls_handler1);
        AsyncStreamOp("CreateConnection", args, h);

        ROBOTRACONTEUR_LOG_INFO_COMPONENT(node, Transport, m_LocalEndpoint,
                                          "TcpTransport TLS activated on client connection to "
                                              << TcpTransport_socket_remote_endpoint(socket) << " from "
                                              << TcpTransport_socket_local_endpoint(socket));
        // starttls_handler1(RR_SHARED_PTR<RobotRaconteurException>());
        return;
    }
    catch (std::exception& e)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, m_LocalEndpoint,
                                           "Client failed CreateConnection after activating TLS: " << e.what());
        detail::InvokeHandlerWithException(node, starttls_handler1, RR_MAKE_SHARED<ConnectionException>(e.what()));
    }
}

void TcpTransportConnection::do_starttls5_1(
    const RR_SHARED_PTR<RRObject>& parameter, const RR_SHARED_PTR<RobotRaconteurException>& err,
    const boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>&)>& callback)
{
    RR_UNUSED(parameter);
    // std::cout << "AsyncAttachStream1" << std::endl;
    if (err)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, m_LocalEndpoint,
                                           "Client failed creating connection after activating TLS: " << err->what());
        try
        {
            Close();
        }
        catch (std::exception&)
        {}
        RobotRaconteurNode::TryPostToThreadPool(node, boost::bind(callback, err), true);
        return;
    }

    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, m_LocalEndpoint,
                                       "CreateConnection complete after activating TLS");

    try
    {
        {
            boost::mutex::scoped_lock lock(heartbeat_timer_lock);
            if (heartbeat_timer)
            {
                heartbeat_timer->expires_from_now(boost::posix_time::milliseconds(400));
                heartbeat_timer->async_wait(
                    boost::bind(&TcpTransportConnection::heartbeat_timer_func,
                                RR_STATIC_POINTER_CAST<TcpTransportConnection>(shared_from_this()),
                                boost::asio::placeholders::error));
            }
        }

        RobotRaconteurNode::TryPostToThreadPool(node, boost::bind(callback, RR_SHARED_PTR<ConnectionException>()),
                                                true);
    }
    catch (std::exception& e)
    {
        RobotRaconteurNode::TryPostToThreadPool(
            node, boost::bind(callback, RR_MAKE_SHARED<ConnectionException>(e.what())), true);
    }
}

void TcpTransportConnection::do_starttls6(const boost::system::error_code& error,
                                          const RR_INTRUSIVE_PTR<Message>& request)
{
    if (error)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, m_LocalEndpoint,
                                           "Could not initialize TLS connection: " << error.message());
        Close();
        return;
    }

    boost::function<void(const boost::system::error_code&)> h = boost::bind(
        &TcpTransportConnection::do_starttls7, RR_STATIC_POINTER_CAST<TcpTransportConnection>(shared_from_this()),
        RR_BOOST_PLACEHOLDERS(_1), request);
    AsyncPauseSend(h);
}

void TcpTransportConnection::do_starttls7(const boost::system::error_code& error,
                                          const RR_INTRUSIVE_PTR<Message>& request)
{
    if (error)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, m_LocalEndpoint,
                                           "Could not initialize TLS connection: " << error.message());
        Close();
        return;
    }

    boost::mutex::scoped_lock lock(send_lock);

    /*{
        boost::mutex::scoped_lock lock1(socket_lock);
        is_tls=true;
    }*/

    RR_INTRUSIVE_PTR<Message> mret = CreateMessage();
    mret->header = CreateMessageHeader();
    mret->header->SenderNodeName = GetNode()->NodeName();
    mret->header->ReceiverNodeName = request->header->SenderNodeName;
    mret->header->SenderNodeID = GetNode()->NodeID();
    mret->header->ReceiverNodeID = request->header->SenderNodeID;
    RR_INTRUSIVE_PTR<MessageEntry> mmret = CreateMessageEntry(MessageEntryType_StreamOpRet, "STARTTLS");
    RR_SHARED_PTR<TcpTransport> p = parent.lock();
    if (!p)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, m_LocalEndpoint, "Parent transport released");
        Close();
        return;
    }

    if (!p->IsTlsNodeCertificateLoaded())
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, m_LocalEndpoint, "Server TLS certificate not loaded");
        mmret->Error = MessageErrorType_ConnectionError;
        mmret->AddElement("errorname", stringToRRArray("RobotRaconteur.ConnectionError"));
        mmret->AddElement("errorstring", stringToRRArray("Server certificate not loaded"));
    }
    else
    {

        if (tls_mutual_auth)
        {
            ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, m_LocalEndpoint,
                                               "Server requesting mutual certificate authentication");
            mmret->AddElement("mutualauth", stringToRRArray("true"));
        }
    }
    mret->entries.push_back(mmret);
    boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>&)> h = boost::bind(
        &TcpTransportConnection::do_starttls8, RR_STATIC_POINTER_CAST<TcpTransportConnection>(shared_from_this()),
        RR_BOOST_PLACEHOLDERS(_1), request);
    BeginSendMessage(mret, h);

    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, m_LocalEndpoint, "Server sending STARTTLS");
}

void TcpTransportConnection::do_starttls8(const RR_SHARED_PTR<RobotRaconteurException>& error,
                                          const RR_INTRUSIVE_PTR<Message>& request)
{
    RR_UNUSED(request);
    if (error)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, m_LocalEndpoint,
                                           "Server failed sending STARTTLS: " << error->what());
        Close();
        return;
    }

    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, m_LocalEndpoint, "Server sent STARTTLS");

    try
    {
        ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, m_LocalEndpoint, "Server begin TLS handshake");
        RR_SHARED_PTR<TcpTransport> p = parent.lock();
        if (!p)
            throw InvalidOperationException("Transport shutdown");
#ifdef ROBOTRACONTEUR_USE_SCHANNEL
        RR_SHARED_PTR<detail::TlsSchannelAsyncStreamAdapterContext> context =
            RR_STATIC_POINTER_CAST<detail::TlsSchannelAsyncStreamAdapterContext>(p->GetTlsContext());

        if (!use_websocket && !use_wss_websocket)
        {
            RR_WEAK_PTR<boost::asio::ip::tcp::socket> socket1 = socket;
            tls_socket = RR_MAKE_SHARED<RobotRaconteur::detail::TlsSchannelAsyncStreamAdapter>(
                boost::ref(GetNode()->GetThreadPool()->get_io_context()), context,
                RobotRaconteur::detail::TlsSchannelAsyncStreamAdapter::server, "",
                boost::bind(detail::TcpTransportConnection_socket_read_adapter, node, socket1,
                            RR_BOOST_PLACEHOLDERS(_1), RR_BOOST_PLACEHOLDERS(_2)),
                boost::bind(detail::TcpTransportConnection_socket_write_adapter, node, socket1,
                            RR_BOOST_PLACEHOLDERS(_1), RR_BOOST_PLACEHOLDERS(_2)),
                boost::bind(detail::TcpTransportConnection_socket_close_adapter, socket1));
            tls_socket->set_mutual_auth(tls_mutual_auth);
            RobotRaconteurNode::asio_async_handshake(
                node, tls_socket,
                boost::bind(&TcpTransportConnection::do_starttls9,
                            RR_STATIC_POINTER_CAST<TcpTransportConnection>(shared_from_this()),
                            RR_BOOST_PLACEHOLDERS(_1)));
        }
        else if (use_websocket)
        {
            tls_websocket = RR_MAKE_SHARED<RobotRaconteur::detail::TlsSchannelAsyncStreamAdapter>(
                boost::ref(GetNode()->GetThreadPool()->get_io_context()), context,
                RobotRaconteur::detail::TlsSchannelAsyncStreamAdapter::server, "",
                boost::bind(detail::TcpTransportConnection_websocket_read_adapter, websocket.get(),
                            RR_BOOST_PLACEHOLDERS(_1), RR_BOOST_PLACEHOLDERS(_2)),
                boost::bind(detail::TcpTransportConnection_websocket_write_adapter, websocket.get(),
                            RR_BOOST_PLACEHOLDERS(_1), RR_BOOST_PLACEHOLDERS(_2)),
                boost::bind(detail::TcpTransportConnection_websocket_close_adapter, websocket.get()));
            tls_websocket->set_mutual_auth(tls_mutual_auth);
            RobotRaconteurNode::asio_async_handshake(
                node, tls_websocket,
                boost::bind(&TcpTransportConnection::do_starttls9,
                            RR_STATIC_POINTER_CAST<TcpTransportConnection>(shared_from_this()),
                            RR_BOOST_PLACEHOLDERS(_1)));
        }
        else
        {
            tls_wss_websocket = RR_MAKE_SHARED<RobotRaconteur::detail::TlsSchannelAsyncStreamAdapter>(
                boost::ref(GetNode()->GetThreadPool()->get_io_context()), context,
                RobotRaconteur::detail::TlsSchannelAsyncStreamAdapter::server, "",
                boost::bind(detail::TcpTransportConnection_wss_websocket_read_adapter, wss_websocket.get(),
                            RR_BOOST_PLACEHOLDERS(_1), RR_BOOST_PLACEHOLDERS(_2)),
                boost::bind(detail::TcpTransportConnection_wss_websocket_write_adapter, wss_websocket.get(),
                            RR_BOOST_PLACEHOLDERS(_1), RR_BOOST_PLACEHOLDERS(_2)),
                boost::bind(detail::TcpTransportConnection_wss_websocket_close_adapter, wss_websocket.get()));
            tls_wss_websocket->set_mutual_auth(tls_mutual_auth);
            RobotRaconteurNode::asio_async_handshake(
                node, tls_wss_websocket,
                boost::bind(&TcpTransportConnection::do_starttls9,
                            RR_STATIC_POINTER_CAST<TcpTransportConnection>(shared_from_this()),
                            RR_BOOST_PLACEHOLDERS(_1)));
        }
#endif

#ifdef ROBOTRACONTEUR_USE_OPENSSL
        if (!use_websocket && !use_wss_websocket)
        {
            tls_context = RR_STATIC_POINTER_CAST<detail::OpenSSLAuthContext>(p->GetTlsContext());
            tls_socket = RR_MAKE_SHARED<boost::asio::ssl::stream<boost::asio::ip::tcp::socket&> >(
                boost::ref(*socket), boost::ref(*(tls_context->GetServerCredentials())));

            if (tls_mutual_auth)
            {
                ::SSL_set_verify(tls_socket->native_handle(), SSL_VERIFY_PEER,
                                 ::SSL_CTX_get_verify_callback(tls_context->GetServerCredentials()->native_handle()));
            }
            RobotRaconteurNode::asio_async_handshake(
                node, tls_socket, boost::asio::ssl::stream<boost::asio::ip::tcp::socket&>::server,
                boost::bind(&TcpTransportConnection::do_starttls9,
                            RR_STATIC_POINTER_CAST<TcpTransportConnection>(shared_from_this()),
                            RR_BOOST_PLACEHOLDERS(_1)));
        }
        else if (use_websocket)
        {
            tls_context = RR_STATIC_POINTER_CAST<detail::OpenSSLAuthContext>(p->GetTlsContext());
            tls_websocket =
                RR_MAKE_SHARED<boost::asio::ssl::stream<detail::websocket_stream<boost::asio::ip::tcp::socket&>&> >(
                    boost::ref(*websocket), boost::ref(*(tls_context->GetServerCredentials())));

            if (tls_mutual_auth)
            {
                ::SSL_set_verify(tls_websocket->native_handle(), SSL_VERIFY_PEER,
                                 ::SSL_CTX_get_verify_callback(tls_context->GetServerCredentials()->native_handle()));
            }
            RobotRaconteurNode::asio_async_handshake(
                node, tls_websocket, boost::asio::ssl::stream<boost::asio::ip::tcp::socket&>::server,
                boost::bind(&TcpTransportConnection::do_starttls9,
                            RR_STATIC_POINTER_CAST<TcpTransportConnection>(shared_from_this()),
                            RR_BOOST_PLACEHOLDERS(_1)));
        }
        else
        {
            tls_context = RR_STATIC_POINTER_CAST<detail::OpenSSLAuthContext>(p->GetTlsContext());
            tls_wss_websocket = RR_MAKE_SHARED<boost::asio::ssl::stream<
                detail::websocket_stream<boost::asio::ssl::stream<boost::asio::ip::tcp::socket&>&>&> >(
                boost::ref(*wss_websocket), boost::ref(*(tls_context->GetServerCredentials())));

            if (tls_mutual_auth)
            {
                ::SSL_set_verify(tls_wss_websocket->native_handle(), SSL_VERIFY_PEER,
                                 ::SSL_CTX_get_verify_callback(tls_context->GetServerCredentials()->native_handle()));
            }
            RobotRaconteurNode::asio_async_handshake(
                node, tls_wss_websocket,
                boost::asio::ssl::stream<
                    detail::websocket_stream<boost::asio::ssl::stream<boost::asio::ip::tcp::socket&>&>&>::server,
                boost::bind(&TcpTransportConnection::do_starttls9,
                            RR_STATIC_POINTER_CAST<TcpTransportConnection>(shared_from_this()),
                            RR_BOOST_PLACEHOLDERS(_1)));
        }

#endif
    }
    catch (std::exception& exp)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, m_LocalEndpoint,
                                           "Server failed starting TLS handshake: " << exp.what());
    }
}

void TcpTransportConnection::do_starttls9(const boost::system::error_code& error)
{
    if (error)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, m_LocalEndpoint,
                                           "Server TLS handshake failed: " << error.message());
        Close();
        return;
    }

    {
        boost::mutex::scoped_lock lock1(socket_lock);
        is_tls = true;
    }

    ROBOTRACONTEUR_LOG_INFO_COMPONENT(node, Transport, m_LocalEndpoint,
                                      "TcpTransport TLS activated on server connection to "
                                          << TcpTransport_socket_remote_endpoint(socket) << " from "
                                          << TcpTransport_socket_local_endpoint(socket));

    try
    {
        AsyncResumeSend();
        AsyncResumeReceive();
    }
    catch (std::exception& exp)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, m_LocalEndpoint,
                                           "Server failed resuming messages: " << exp.what());
        Close();
        return;
    }
}

void TcpTransportConnection::MessageReceived(const RR_INTRUSIVE_PTR<Message>& m)
{
    NodeID RemoteNodeID1;
    uint32_t local_ep = 0;
    uint32_t remote_ep = 0;
    {
        boost::shared_lock<boost::shared_mutex> lock(RemoteNodeID_lock);
        RemoteNodeID1 = RemoteNodeID;
        local_ep = m_LocalEndpoint;
        remote_ep = m_RemoteEndpoint;
    }

    if (require_tls && !is_tls)
    {
        // If tls is required, tls is not active, and the message received is not "STARTTLS", abort the connection
        bool bad_message = true;
        if (m->entries.size() == 1)
        {
            if (m->entries[0]->EntryType == MessageEntryType_StreamOp && m->entries[0]->MemberName == "STARTTLS")
            {
                bad_message = false;
            }
        }

        if (bad_message)
        {
            ROBOTRACONTEUR_LOG_INFO_COMPONENT(
                node, Transport, m_LocalEndpoint,
                "Message received without TLS active when TLS is required, closing transport");
            // This may not be the nicest way to handle the situation but it is the safest
            Close();
        }
    }

    // TODO: Finish the endpoint checking procedure

    if (is_tls)
    {
        if (!RemoteNodeID1.IsAnyNode())
        {
            if (RemoteNodeID1 != m->header->SenderNodeID)
            {
                RR_INTRUSIVE_PTR<Message> ret1 = GetNode()->GenerateErrorReturnMessage(
                    m, MessageErrorType_NodeNotFound, "RobotRaconteur.NodeNotFound", "Invalid sender node");
                ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, m_LocalEndpoint,
                                                   "Message with invalid SenderNodeID received: "
                                                       << m->header->SenderNodeID.ToString()
                                                       << " expected: " << RemoteNodeID1.ToString());
                if (!ret1->entries.empty())
                {
                    boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>&)> h = boost::bind(
                        &TcpTransportConnection::SimpleAsyncEndSendMessage,
                        RR_STATIC_POINTER_CAST<TcpTransportConnection>(shared_from_this()), RR_BOOST_PLACEHOLDERS(_1));
                    AsyncSendMessage(ret1, h);
                    return;
                }
            }
        }

        if (local_ep != 0 && remote_ep != 0)
        {
            if (local_ep != m->header->ReceiverEndpoint || remote_ep != m->header->SenderEndpoint)
            {
                RR_INTRUSIVE_PTR<Message> ret1;
                if (remote_ep != m->header->SenderEndpoint)
                {
                    ret1 = GetNode()->GenerateErrorReturnMessage(m, MessageErrorType_InvalidEndpoint,
                                                                 "RobotRaconteur.InvalidEndpoint",
                                                                 "Invalid sender endpoint");
                    ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, m_LocalEndpoint,
                                                       "Message received with invalid SenderEndpoint received: "
                                                           << m->header->SenderEndpoint << " expected: " << remote_ep);
                }
                if (local_ep != m->header->ReceiverEndpoint)
                {
                    ret1 = GetNode()->GenerateErrorReturnMessage(m, MessageErrorType_InvalidEndpoint,
                                                                 "RobotRaconteur.InvalidEndpoint",
                                                                 "Invalid receiver endpoint");
                    ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, m_LocalEndpoint,
                                                       "Message received with invalid ReceiverEndpoint received: "
                                                           << m->header->ReceiverEndpoint << " expected: " << local_ep);
                }
                if (!ret1->entries.empty())
                {
                    boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>&)> h = boost::bind(
                        &TcpTransportConnection::SimpleAsyncEndSendMessage,
                        RR_STATIC_POINTER_CAST<TcpTransportConnection>(shared_from_this()), RR_BOOST_PLACEHOLDERS(_1));
                    AsyncSendMessage(ret1, h);
                    return;
                }
            }
        }
    }

    RR_SHARED_PTR<TcpTransport> p = parent.lock();
    if (!p)
        return;

    RR_INTRUSIVE_PTR<Message> ret = p->SpecialRequest(m, shared_from_this());
    if (ret != 0)
    {
        ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, m_LocalEndpoint,
                                           "TcpTransport sending special request response to "
                                               << TcpTransport_socket_remote_endpoint(socket) << " from "
                                               << TcpTransport_socket_local_endpoint(socket));
        try
        {
            if ((m->entries.at(0)->EntryType == MessageEntryType_ConnectionTest ||
                 m->entries.at(0)->EntryType == MessageEntryType_ConnectionTestRet))
            {
                if (m->entries.at(0)->Error != MessageErrorType_None)
                {
                    ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, m_LocalEndpoint, "SpecialRequest failed");
                    Close();
                    return;
                }
            }

            if ((ret->entries.at(0)->EntryType == MessageEntryType_ConnectClientRet ||
                 ret->entries.at(0)->EntryType == MessageEntryType_ReconnectClient ||
                 ret->entries.at(0)->EntryType == MessageEntryType_ConnectClientCombinedRet) &&
                ret->entries.at(0)->Error == MessageErrorType_None)
            {
                if (ret->entries.at(0)->Error == MessageErrorType_None)
                {
                    if (ret->header->SenderNodeID == GetNode()->NodeID())
                    {
                        {
                            boost::unique_lock<boost::shared_mutex> lock(RemoteNodeID_lock);
                            if (m_LocalEndpoint != 0)
                            {
                                throw InvalidOperationException("Already connected");
                            }

                            m_RemoteEndpoint = ret->header->ReceiverEndpoint;
                            m_LocalEndpoint = ret->header->SenderEndpoint;
                        }

                        p->register_transport(RR_STATIC_POINTER_CAST<TcpTransportConnection>(shared_from_this()));

                        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, m_LocalEndpoint,
                                                           "TcpTransport connection to "
                                                               << TcpTransport_socket_remote_endpoint(socket)
                                                               << " from " << TcpTransport_socket_local_endpoint(socket)
                                                               << " assigned LocalEndpoint: " << m_LocalEndpoint);
                    }
                }
            }

            boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>&)> h = boost::bind(
                &TcpTransportConnection::SimpleAsyncEndSendMessage,
                RR_STATIC_POINTER_CAST<TcpTransportConnection>(shared_from_this()), RR_BOOST_PLACEHOLDERS(_1));
            AsyncSendMessage(ret, h);
        }
        catch (std::exception& exp)
        {
            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, m_LocalEndpoint,
                                               "SpecialRequest failed: " << exp.what());
            Close();
        }

        return;
    }

    try
    {

        if (m->entries.size() == 1)
        {
            if ((m->entries[0]->EntryType == MessageEntryType_ConnectClientRet ||
                 m->entries[0]->EntryType == MessageEntryType_ConnectClientCombinedRet) &&
                remote_ep == 0)
            {
                boost::unique_lock<boost::shared_mutex> lock(RemoteNodeID_lock);
                if (m_RemoteEndpoint == 0)
                {
                    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(
                        node, Transport, m_LocalEndpoint,
                        "TcpTransport connection to " << TcpTransport_socket_remote_endpoint(socket) << " from "
                                                      << TcpTransport_socket_local_endpoint(socket)
                                                      << " assigned RemoteEndpoint: " << m->header->SenderEndpoint);
                    m_RemoteEndpoint = m->header->SenderEndpoint;
                }
                remote_ep = m_RemoteEndpoint;
            }
        }

        // We shouldn't get here without having a service connection
        if (is_tls)
        {
            if (local_ep == 0 || remote_ep == 0)
            {
                if (m->entries.size() != 1)
                {
                    ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(
                        node, Transport, m_LocalEndpoint,
                        "TcpTransport connection invalid message before client connection created");
                    // Someone is doing something nasty
                    Close();
                    return;
                }

                uint16_t command = m->entries[0]->EntryType;

                if (command > 500) // Check if special command
                {
                    ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(
                        node, Transport, m_LocalEndpoint,
                        "TcpTransport connection invalid message before client connection created");
                    // Someone is doing something nasty
                    Close();
                    return;
                }
            }
        }

        boost::asio::ip::address addr = socket->local_endpoint().address();
        uint16_t port = socket->local_endpoint().port();

        std::string scheme = "rr+tcp";

        if (is_tls)
        {
            scheme = "rrs+tcp";
        }

        if (use_websocket)
        {
            scheme = "rr+ws";
        }

        if (use_websocket && is_tls)
        {
            scheme = "rrs+ws";
        }

        if (use_wss_websocket)
        {
            scheme = "rr+wss";
        }

        if (use_wss_websocket & is_tls)
        {
            scheme = "rrs+wss";
        }

        std::string connecturl;
        if (addr.is_v4())
        {
            connecturl = scheme + "://" + addr.to_string() + ":" + boost::lexical_cast<std::string>(port) + "/";
        }
        else
        {
            boost::asio::ip::address_v6 addr2 = addr.to_v6();
            addr2.scope_id(0);
            /*if (addr2.is_v4_mapped())
            {
                connecturl = scheme + "://" + addr2.to_v4() + ":" + boost::lexical_cast<std::string>(port)+"/";
            }
            else
            {*/
            connecturl = scheme + "://[" + addr2.to_string() + "]:" + boost::lexical_cast<std::string>(port) + "/";
            //}
        }

        // NOLINTBEGIN(cppcoreguidelines-owning-memory)
        Transport::m_CurrentThreadTransportConnectionURL.reset(new std::string(connecturl));
        Transport::m_CurrentThreadTransport.reset(new RR_SHARED_PTR<ITransportConnection>(
            RR_STATIC_POINTER_CAST<TcpTransportConnection>(shared_from_this())));
        // NOLINTEND(cppcoreguidelines-owning-memory)
        ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, m_LocalEndpoint,
                                           "TcpTransport received message from "
                                               << TcpTransport_socket_local_endpoint(socket) << " passing to node");
        p->MessageReceived(m);
    }
    catch (std::exception& exp)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, m_LocalEndpoint,
                                           "TcpTransport failed receiving message from "
                                               << TcpTransport_socket_local_endpoint(socket) << ": " << exp.what());
        RobotRaconteurNode::TryHandleException(node, &exp);
        Close();
    }

    Transport::m_CurrentThreadTransportConnectionURL.reset(0);
    Transport::m_CurrentThreadTransport.reset(0);
}

void TcpTransportConnection::StreamOpMessageReceived(const RR_INTRUSIVE_PTR<Message>& m)
{
    if (m->entries.size() == 1)
    {
        RR_INTRUSIVE_PTR<MessageEntry> mm = m->entries.at(0);
        if (mm->MemberName == "STARTTLS")
        {
            if (mm->EntryType == MessageEntryType_StreamOp)
            {
                if ((m->header->ReceiverNodeID == NodeID::GetAny() && m->header->ReceiverNodeName == "") ||
                    (m->header->ReceiverNodeID == GetNode()->NodeID() && m->header->ReceiverNodeName == "") ||
                    (m->header->ReceiverNodeID == NodeID::GetAny() &&
                     m->header->ReceiverNodeName == GetNode()->NodeName()) ||
                    (m->header->ReceiverNodeID == GetNode()->NodeID() &&
                     m->header->ReceiverNodeName == GetNode()->NodeName()))
                {
                    {
                        boost::unique_lock<boost::shared_mutex> lock(RemoteNodeID_lock);
                        RemoteNodeID = m->header->SenderNodeID;
                    }

                    RR_INTRUSIVE_PTR<MessageElement> e;
                    if (mm->TryFindElement("mutualauth", e))
                    {
                        std::string dat = e->CastDataToString();
                        if (dat == "true")
                        {
                            ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, m_LocalEndpoint,
                                                               "Client requesting mutual certificate authentication");
                            tls_mutual_auth = true;
                        }
                    }

                    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, m_LocalEndpoint,
                                                       "Server received STARTTLS request from client");

                    // We have a match!
                    boost::function<void(const boost::system::error_code&)> h =
                        boost::bind(&TcpTransportConnection::do_starttls6,
                                    RR_STATIC_POINTER_CAST<TcpTransportConnection>(shared_from_this()),
                                    RR_BOOST_PLACEHOLDERS(_1), m);
                    AsyncPauseReceive(h);
                    return;
                }
                else
                {
                    // Oops, requesting a bad nodeid/nodename query

                    ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, m_LocalEndpoint,
                                                       "Server requested an invalid STARTTLS request");

                    RR_INTRUSIVE_PTR<Message> mret = CreateMessage();
                    mret->header = CreateMessageHeader();
                    mret->header->SenderNodeName = GetNode()->NodeName();
                    mret->header->ReceiverNodeName = m->header->SenderNodeName;
                    mret->header->SenderNodeID = GetNode()->NodeID();
                    mret->header->ReceiverNodeID = m->header->SenderNodeID;

                    RR_INTRUSIVE_PTR<MessageEntry> mmret =
                        CreateMessageEntry(MessageEntryType_StreamOpRet, mm->MemberName);
                    mmret->Error = MessageErrorType_NodeNotFound;
                    mmret->AddElement("errorname", stringToRRArray("RobotRaconteur.NodeNotFound"));
                    mmret->AddElement("errorstring", stringToRRArray("Node not found"));
                    mret->entries.push_back(mmret);

                    boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>&)> h = boost::bind(
                        &TcpTransportConnection::StreamOp_EndSendMessage,
                        RR_STATIC_POINTER_CAST<TcpTransportConnection>(shared_from_this()), RR_BOOST_PLACEHOLDERS(_1));
                    AsyncSendMessage(mret, h);

                    return;
                }
            }

            if (mm->EntryType == MessageEntryType_StreamOpRet)
            {
                streamop_waiting = false;
                if (mm->Error != 0)
                {
                    RR_SHARED_PTR<RobotRaconteurException> ee =
                        RobotRaconteurExceptionUtil::MessageEntryToException(mm);

                    ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(
                        node, Transport, m_LocalEndpoint,
                        "Server returned an error during STARTTLS request: " << ee->what());

                    boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>&)> starttls_handler1 =
                        starttls_handler;
                    starttls_handler.clear();
                    detail::InvokeHandlerWithException(node, starttls_handler1, ee);
                    return;
                }

                std::string servername = m->header->SenderNodeID.ToString();
                if (target_nodeid == NodeID::GetAny() && target_nodename.empty())
                {
                    boost::unique_lock<boost::shared_mutex> lock(RemoteNodeID_lock);
                    RemoteNodeID = m->header->SenderNodeID;
                }
                else if (target_nodeid == m->header->SenderNodeID)
                {
                    boost::unique_lock<boost::shared_mutex> lock(RemoteNodeID_lock);
                    RemoteNodeID = target_nodeid;
                }
                else if (m->header->SenderNodeName == target_nodename && target_nodeid == NodeID::GetAny())
                {
                    boost::unique_lock<boost::shared_mutex> lock(RemoteNodeID_lock);
                    RemoteNodeID = m->header->SenderNodeID;
                }
                else
                {
                    ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, m_LocalEndpoint,
                                                       "Server returned an invalid STARTTLS response");

                    boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>&)> starttls_handler1 =
                        starttls_handler;
                    starttls_handler.clear();
                    detail::InvokeHandlerWithException(
                        node, starttls_handler1,
                        RR_MAKE_SHARED<ConnectionException>("Could not create TLS connection"));
                    return;
                }

                RR_INTRUSIVE_PTR<MessageElement> e;
                if (mm->TryFindElement("mutualauth", e))
                {
                    std::string dat = e->CastDataToString();
                    if (dat == "true")
                    {
                        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, m_LocalEndpoint,
                                                           "Mutual certificate authentication activated");
                        tls_mutual_auth = true;
                    }
                }

                ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, m_LocalEndpoint,
                                                   "Client received success STARTTLS from server");

                boost::function<void(const boost::system::error_code&)> h =
                    boost::bind(&TcpTransportConnection::do_starttls4,
                                RR_STATIC_POINTER_CAST<TcpTransportConnection>(shared_from_this()), servername,
                                RR_BOOST_PLACEHOLDERS(_1));
                AsyncPauseReceive(h);
                return;
            }
        }
    }

    return ASIOStreamBaseTransport::StreamOpMessageReceived(m);
}

void TcpTransportConnection::async_write_some(
    const_buffers& b,
    const boost::function<void(const boost::system::error_code& error, size_t bytes_transferred)>& handler)
{
    boost::mutex::scoped_lock lock(socket_lock);

    if (is_tls)
    {
        if (!use_websocket && !use_wss_websocket)
        {
            RobotRaconteurNode::asio_async_write_some(node, tls_socket, b, handler);
            return;
        }
        else if (use_websocket)
        {
            RobotRaconteurNode::asio_async_write_some(node, tls_websocket, b, handler);
            return;
        }
        else
        {
            RobotRaconteurNode::asio_async_write_some(node, tls_wss_websocket, b, handler);
            return;
        }
    }

    if (use_websocket)
    {
        RobotRaconteurNode::asio_async_write_some(node, websocket, b, handler);
        return;
    }

    if (use_wss_websocket)
    {
        RobotRaconteurNode::asio_async_write_some(node, wss_websocket, b, handler);
        return;
    }

    RobotRaconteurNode::asio_async_write_some(node, socket, b, handler);
}

void TcpTransportConnection::async_read_some(
    mutable_buffers& b,
    const boost::function<void(const boost::system::error_code& error, size_t bytes_transferred)>& handler)
{
    boost::mutex::scoped_lock lock(socket_lock);

    if (is_tls)
    {
        if (!use_websocket && !use_wss_websocket)
        {
            RobotRaconteurNode::asio_async_read_some(node, tls_socket, b, handler);
            return;
        }
        else if (use_websocket)
        {
            RobotRaconteurNode::asio_async_read_some(node, tls_websocket, b, handler);
            return;
        }
        else
        {
            RobotRaconteurNode::asio_async_read_some(node, tls_wss_websocket, b, handler);
            return;
        }
    }

    if (use_websocket)
    {
        RobotRaconteurNode::asio_async_read_some(node, websocket, b, handler);
        return;
    }

    if (use_wss_websocket)
    {
        RobotRaconteurNode::asio_async_read_some(node, wss_websocket, b, handler);

        return;
    }

    RobotRaconteurNode::asio_async_read_some(node, socket, b, handler);
}

size_t TcpTransportConnection::available()
{
    boost::mutex::scoped_lock lock(socket_lock);

    return socket->available();
}

void TcpTransportConnection::Close()
{
    boost::recursive_mutex::scoped_lock lock(close_lock);
    if (closing)
        return;
    closing = true;
    {
        ROBOTRACONTEUR_LOG_INFO_COMPONENT(node, Transport, m_LocalEndpoint,
                                          "TcpTransport closing transport connection to "
                                              << TcpTransport_socket_remote_endpoint(socket) << " from "
                                              << TcpTransport_socket_local_endpoint(socket));
        boost::mutex::scoped_lock lock(socket_lock);

        // NOLINTNEXTLINE(readability-simplify-boolean-expr)
        if (false /*is_tls*/)
        {
            try
            {
                if (!use_websocket && !use_wss_websocket)
                {
                    RobotRaconteurNode::asio_async_shutdown(
                        node, tls_socket,
                        boost::bind(&TcpTransportConnection::Close1,
                                    RR_STATIC_POINTER_CAST<TcpTransportConnection>(shared_from_this()),
                                    RR_BOOST_PLACEHOLDERS(_1)));
                }
                else if (use_websocket)
                {
                    RobotRaconteurNode::asio_async_shutdown(
                        node, tls_websocket,
                        boost::bind(&TcpTransportConnection::Close1,
                                    RR_STATIC_POINTER_CAST<TcpTransportConnection>(shared_from_this()),
                                    RR_BOOST_PLACEHOLDERS(_1)));
                }
                else
                {
                    RobotRaconteurNode::asio_async_shutdown(
                        node, tls_wss_websocket,
                        boost::bind(&TcpTransportConnection::Close1,
                                    RR_STATIC_POINTER_CAST<TcpTransportConnection>(shared_from_this()),
                                    RR_BOOST_PLACEHOLDERS(_1)));
                }
            }
            catch (std::exception&)
            {
                RobotRaconteurNode::TryPostToThreadPool(
                    node, boost::bind(&TcpTransportConnection::ForceClose,
                                      RR_STATIC_POINTER_CAST<TcpTransportConnection>(shared_from_this())));
            }
        }
        else
        {

            try
            {
                if (socket->is_open())
                {
                    socket->shutdown(boost::asio::ip::tcp::socket::shutdown_both);
                }
            }
            catch (std::exception&)
            {}

            try
            {
                // NOLINTBEGIN(bugprone-branch-clone)
                if (is_tls && !use_websocket && !use_wss_websocket)
                {
                    socket->close();
                }
                else if (use_websocket)
                {
                    websocket->close();
                }
                else if (use_wss_websocket)
                {
                    wss_websocket->close();
                }
                else
                {
                    socket->close();
                }
                // NOLINTEND(bugprone-branch-clone)
            }
            catch (std::exception&)
            {}
        }
    }

    {
        boost::mutex::scoped_lock lock2(streamop_lock);
        starttls_timer.reset();
        if (starttls_handler)
        {
            boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>&)> starttls_handler1 = starttls_handler;
            starttls_handler.clear();
            detail::PostHandlerWithException(node, starttls_handler1,
                                             RR_MAKE_SHARED<ConnectionException>("Transport connection closed"), true);
        }
    }

    try
    {
        RR_SHARED_PTR<TcpTransport> p = parent.lock();
        if (p)
            p->erase_transport(RR_STATIC_POINTER_CAST<TcpTransportConnection>(shared_from_this()));
    }
    catch (std::exception&)
    {}

    ASIOStreamBaseTransport::Close();

    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, m_LocalEndpoint, "TcpTransport closed transport connection");
}

void TcpTransportConnection::Close1(const boost::system::error_code& ec)
{
    RR_UNUSED(ec);
    ForceClose();
}

bool TcpTransportConnection::IsClosed()
{
    boost::mutex::scoped_lock lock(socket_lock);
    return socket->is_open();
}

void TcpTransportConnection::ForceClose()
{
    boost::mutex::scoped_lock lock(socket_lock);

    ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, m_LocalEndpoint,
                                       "TcpTransport force closing transport connection");

    try
    {

        if (is_tls)
        {
            // tls_socket.shutdown();
            // tls_socket.reset();
            is_tls = false;
        }
    }
    catch (std::exception&)
    {}

    try
    {
        if (socket->is_open())
        {
            socket->shutdown(boost::asio::ip::tcp::socket::shutdown_both);
        }
    }
    catch (std::exception&)
    {}

    try
    {
        socket->close();
    }
    catch (std::exception&)
    {}
}

void TcpTransport::CheckConnection(uint32_t endpoint)
{
    RR_SHARED_PTR<ITransportConnection> t;
    {
        boost::mutex::scoped_lock lock(TransportConnections_lock);
        RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<ITransportConnection> >::iterator e1 =
            TransportConnections.find(endpoint);
        if (e1 == TransportConnections.end())
        {
            ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, endpoint,
                                               "Transport connection to remote host not found");
            throw ConnectionException("Transport connection to remote host not found");
        }
        t = e1->second;
    }
    t->CheckConnection(endpoint);
}

uint32_t TcpTransportConnection::GetLocalEndpoint()
{
    boost::shared_lock<boost::shared_mutex> lock(RemoteNodeID_lock);
    return m_LocalEndpoint;
}

uint32_t TcpTransportConnection::GetRemoteEndpoint()
{
    boost::shared_lock<boost::shared_mutex> lock(RemoteNodeID_lock);
    return m_RemoteEndpoint;
}

void TcpTransportConnection::CheckConnection(uint32_t endpoint)
{
    if (endpoint != m_LocalEndpoint || !connected.load())
    {
        ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, m_LocalEndpoint, "Connection lost");
        throw ConnectionException("Connection lost");
    }
}

bool TcpTransportConnection::IsSecure() { return is_tls; }

// TODO finish this
bool TcpTransportConnection::IsSecurePeerIdentityVerified()
{
    boost::mutex::scoped_lock lock(socket_lock);
    if (!is_tls)
        return false;
    if (!tls_socket && !tls_websocket)
        return false;
#ifdef ROBOTRACONTEUR_USE_SCHANNEL
    if (!use_websocket && !use_wss_websocket)
    {
        RR_SHARED_PTR<detail::TlsSchannelAsyncStreamAdapter> s = tls_socket;
        lock.unlock();
        boost::shared_lock<boost::shared_mutex> lock2(RemoteNodeID_lock);
        return s->VerifyRemoteNodeCertificate(RemoteNodeID);
    }
    else if (use_websocket)
    {
        RR_SHARED_PTR<detail::TlsSchannelAsyncStreamAdapter> s = tls_websocket;
        lock.unlock();
        boost::shared_lock<boost::shared_mutex> lock2(RemoteNodeID_lock);
        return s->VerifyRemoteNodeCertificate(RemoteNodeID);
    }
    else
    {
        RR_SHARED_PTR<detail::TlsSchannelAsyncStreamAdapter> s = tls_wss_websocket;
        lock.unlock();
        boost::shared_lock<boost::shared_mutex> lock2(RemoteNodeID_lock);
        return s->VerifyRemoteNodeCertificate(RemoteNodeID);
    }

#endif

#ifdef ROBOTRACONTEUR_USE_OPENSSL
    if (!use_websocket && !use_wss_websocket)
    {
        long res = SSL_get_verify_result(tls_socket->native_handle());
        if (res != X509_V_OK && res != X509_V_ERR_UNHANDLED_CRITICAL_EXTENSION)
            return false;
        X509* cert = SSL_get_peer_certificate(tls_socket->native_handle());
        if (!cert)
            return false;

        boost::array<char, 256> buf = {};
        memset(buf.data(), 0, 256);

        X509_NAME_oneline(X509_get_subject_name(cert), buf.data(), 256);

        lock.unlock();
        std::string buf2(buf.data());

        boost::shared_lock<boost::shared_mutex> lock2(RemoteNodeID_lock);
        if (buf2 != "/CN=Robot Raconteur Node " + RemoteNodeID.ToString())
        {
            return false;
        }

        // lock2.unlock();

        RR_SHARED_PTR<TcpTransport> p = parent.lock();
        if (!p)
            throw InvalidOperationException("Transport shutdown");

        RR_SHARED_PTR<detail::OpenSSLAuthContext> c =
            RR_STATIC_POINTER_CAST<detail::OpenSSLAuthContext>(p->GetTlsContext());

        return c->VerifyRemoteNodeCertificate(tls_socket->native_handle(), RemoteNodeID);
    }
    else if (use_websocket)
    {
        long res = SSL_get_verify_result(tls_websocket->native_handle());
        if (res != X509_V_OK && res != X509_V_ERR_UNHANDLED_CRITICAL_EXTENSION)
            return false;
        X509* cert = SSL_get_peer_certificate(tls_websocket->native_handle());
        if (!cert)
            return false;

        boost::array<char, 256> buf = {};
        memset(buf.data(), 0, 256);

        X509_NAME_oneline(X509_get_subject_name(cert), buf.data(), 256);

        lock.unlock();
        std::string buf2(buf.data());

        boost::shared_lock<boost::shared_mutex> lock2(RemoteNodeID_lock);
        if (buf2 != "/CN=Robot Raconteur Node " + RemoteNodeID.ToString())
        {
            return false;
        }

        // lock2.unlock();

        RR_SHARED_PTR<TcpTransport> p = parent.lock();
        if (!p)
            throw InvalidOperationException("Transport shutdown");

        RR_SHARED_PTR<detail::OpenSSLAuthContext> c =
            RR_STATIC_POINTER_CAST<detail::OpenSSLAuthContext>(p->GetTlsContext());

        return c->VerifyRemoteNodeCertificate(tls_websocket->native_handle(), RemoteNodeID);
    }
    else
    {
        long res = SSL_get_verify_result(tls_wss_websocket->native_handle());
        if (res != X509_V_OK && res != X509_V_ERR_UNHANDLED_CRITICAL_EXTENSION)
            return false;
        X509* cert = SSL_get_peer_certificate(tls_wss_websocket->native_handle());
        if (!cert)
            return false;

        boost::array<char, 256> buf = {};
        memset(buf.data(), 0, 256);

        X509_NAME_oneline(X509_get_subject_name(cert), buf.data(), 256);

        lock.unlock();
        std::string buf2(buf.data());

        boost::shared_lock<boost::shared_mutex> lock2(RemoteNodeID_lock);
        if (buf2 != "/CN=Robot Raconteur Node " + RemoteNodeID.ToString())
        {
            return false;
        }

        // lock2.unlock();

        RR_SHARED_PTR<TcpTransport> p = parent.lock();
        if (!p)
            throw InvalidOperationException("Transport shutdown");

        RR_SHARED_PTR<detail::OpenSSLAuthContext> c =
            RR_STATIC_POINTER_CAST<detail::OpenSSLAuthContext>(p->GetTlsContext());

        return c->VerifyRemoteNodeCertificate(tls_wss_websocket->native_handle(), RemoteNodeID);
    }

#endif
}

std::string TcpTransportConnection::GetSecurePeerIdentity()
{
    boost::shared_lock<boost::shared_mutex> lock2(RemoteNodeID_lock);
    if (!IsSecurePeerIdentityVerified())
        throw AuthenticationException("Peer identity is not verified");
    return RemoteNodeID.ToString();
}

bool TcpTransportConnection::IsLargeTransferAuthorized()
{
    boost::shared_lock<boost::shared_mutex> lock2(RemoteNodeID_lock);
    if (m_RemoteEndpoint == 0 || m_LocalEndpoint == 0)
        return false;
    return GetNode()->IsEndpointLargeTransferAuthorized(m_LocalEndpoint);
}

RR_SHARED_PTR<Transport> TcpTransportConnection::GetTransport()
{
    RR_SHARED_PTR<Transport> p = parent.lock();
    if (!p)
        throw InvalidOperationException("Transport has been released");
    return p;
}

namespace detail
{
IPNodeDiscovery::IPNodeDiscovery(const RR_SHARED_PTR<TcpTransport>& parent)
{
    listening = false;
    broadcasting = false;
    broadcast_flags = 0;
    listen_flags = 0;
    listen_socket_flags = 0;

    this->parent = parent;
    this->node = parent->GetNode();
    this->broadcast_timer_period = 55000;
}

IPNodeDiscovery::IPNodeDiscovery(const RR_SHARED_PTR<RobotRaconteurNode>& node)
{
    listening = false;
    broadcasting = false;
    broadcast_flags = 0;
    listen_flags = 0;
    listen_socket_flags = 0;
    this->node = node;
    this->broadcast_timer_period = 10000;
}

RR_SHARED_PTR<RobotRaconteurNode> IPNodeDiscovery::GetNode()
{
    RR_SHARED_PTR<RobotRaconteurNode> t = node.lock();
    if (!t)
        throw InvalidOperationException("Transport has been released");
    return t;
}

void IPNodeDiscovery::start_listen_sockets()
{
    int32_t key = 0;
    while (backoff_timers.find(key) != backoff_timers.end())
    {
        key++;
    }

    RR_SHARED_PTR<boost::asio::deadline_timer> backoff_timer(
        new boost::asio::deadline_timer(GetNode()->GetThreadPool()->get_io_context()));
    backoff_timer->expires_from_now(boost::posix_time::milliseconds(10));
    backoff_timer->async_wait(boost::bind(&IPNodeDiscovery::start_listen_sockets2, shared_from_this(), key,
                                          boost::asio::placeholders::error));
    backoff_timers.insert(std::make_pair(key, backoff_timer));
}

void IPNodeDiscovery::start_listen_sockets2(int32_t key, const boost::system::error_code& ec)
{
    try
    {
        boost::mutex::scoped_lock lock(change_lock);
        backoff_timers.erase(key);
        if (ec)
        {
            return;
        }
        uint32_t flags = broadcast_flags | listen_flags;

        if (flags == listen_socket_flags)
            return;

        ip4_listen.reset();
        ip6_listen.clear();

        std::vector<boost::asio::ip::address> local_addresses;

        TcpTransport::GetLocalAdapterIPAddresses(local_addresses);

        bool has_ip6 = false;

        BOOST_FOREACH (boost::asio::ip::address& e, local_addresses)
        {
            if (e.is_v6())
                has_ip6 = true;
        }

        // has_ip6=true;

        ip4_listen = RR_SHARED_PTR<boost::asio::ip::udp::socket>(
            new boost::asio::ip::udp::socket(GetNode()->GetThreadPool()->get_io_context()));

        ip4_listen->open(boost::asio::ip::udp::v4());

        // ip4_listen->set_option(boost::asio::ip::udp::socket::linger(false,10));

        ip4_listen->set_option(boost::asio::ip::udp::socket::reuse_address(true));
#ifdef ROBOTRACONTEUR_WINDOWS
        u_long exclusiveaddruse = 0;
        ::setsockopt(ip4_listen->native_handle(), SOL_SOCKET, SO_EXCLUSIVEADDRUSE, (const char*)&exclusiveaddruse,
                     sizeof(exclusiveaddruse));
#endif

        ip4_listen->set_option(boost::asio::ip::udp::socket::broadcast(true));
        ip4_listen->bind(boost::asio::ip::udp::endpoint(boost::asio::ip::address_v4::any(), ANNOUNCE_PORT));

        ip4_listen->set_option(boost::asio::ip::multicast::enable_loopback(true));

        boost::shared_array<uint8_t> data(new uint8_t[8192]);

        RR_SHARED_PTR<boost::asio::ip::udp::endpoint> ep4 =
            RR_MAKE_SHARED<boost::asio::ip::udp::endpoint>(boost::asio::ip::address_v4::any(), ANNOUNCE_PORT);

        ip4_listen->async_receive_from(
            boost::asio::buffer(data.get(), 8192), *ep4,
            boost::bind(&IPNodeDiscovery::handle_receive, shared_from_this(), boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred, ip4_listen, ep4, data));

        if (has_ip6)
        {

            receive_update_timer = RR_SHARED_PTR<boost::asio::deadline_timer>(
                new boost::asio::deadline_timer(GetNode()->GetThreadPool()->get_io_context()));

            try
            {
                start_ipv6_listen_socket(
                    boost::asio::ip::udp::endpoint(boost::asio::ip::address_v6::any(), ANNOUNCE_PORT));
            }
            catch (std::exception& exp2)
            {
                ROBOTRACONTEUR_LOG_WARNING_COMPONENT(
                    node, Transport, -1, "TcpTransport discovery error starting IPv6 listen socket: " << exp2.what());
            }

            RobotRaconteurNode::TryPostToThreadPool(node,
                                                    boost::bind(&IPNodeDiscovery::handle_receive_update_timer,
                                                                shared_from_this(), boost::system::error_code()),
                                                    true);
        }
    }
    catch (std::exception& exp)
    {
        ROBOTRACONTEUR_LOG_WARNING_COMPONENT(node, Transport, -1,
                                             "TcpTransport discovery error starting listen socket: " << exp.what());
    }
}

void IPNodeDiscovery::start_ipv6_listen_socket(const boost::asio::ip::udp::endpoint& ep)
{
    uint32_t flags = broadcast_flags | listen_flags;

    if (boost::range::count(ip6_listen_scope_ids, ep.address().to_v6().scope_id()) != 0)
    {
        return;
    }

    RR_SHARED_PTR<boost::asio::ip::udp::socket> ip6_listen1;

    ip6_listen1 = RR_SHARED_PTR<boost::asio::ip::udp::socket>(
        new boost::asio::ip::udp::socket(GetNode()->GetThreadPool()->get_io_context()));

    ip6_listen1->open(boost::asio::ip::udp::v6());
    ip6_listen1->set_option(boost::asio::ip::udp::socket::reuse_address(true));
#ifdef ROBOTRACONTEUR_WINDOWS
    u_long exclusiveaddruse = 0;
    ::setsockopt(ip6_listen1->native_handle(), SOL_SOCKET, SO_EXCLUSIVEADDRUSE, (const char*)&exclusiveaddruse,
                 sizeof(exclusiveaddruse));
#endif
    // ip6_listen->set_option(boost::asio::ip::udp::socket::broadcast(true));

    // ip6_listen->set_option(boost::asio::ip::udp::socket::linger(false,10));
    ip6_listen1->bind(ep);

    ip6_listen1->set_option(boost::asio::ip::multicast::enable_loopback(true));

    if ((static_cast<uint32_t>(flags) & (static_cast<uint32_t>(IPNodeDiscoveryFlags_NODE_LOCAL))) != 0)
    {
        boost::system::error_code ec;
        Ip6AddMembership(*ip6_listen1, RR_BOOST_ASIO_IP_ADDRESS_V6_FROM_STRING("FF01::BA86"),
                         ep.address().to_v6().scope_id());
    }

    if ((static_cast<uint32_t>(flags) & (static_cast<uint32_t>(IPNodeDiscoveryFlags_LINK_LOCAL))) != 0)
    {
        boost::system::error_code ec;
        Ip6AddMembership(*ip6_listen1, RR_BOOST_ASIO_IP_ADDRESS_V6_FROM_STRING("FF02::BA86"),
                         ep.address().to_v6().scope_id());
    }

    if ((static_cast<uint32_t>(flags) & (static_cast<uint32_t>(IPNodeDiscoveryFlags_SITE_LOCAL))) != 0)
    {
        boost::system::error_code ec;
        Ip6AddMembership(*ip6_listen1, RR_BOOST_ASIO_IP_ADDRESS_V6_FROM_STRING("FF05::BA86"),
                         ep.address().to_v6().scope_id());
    }

    boost::shared_array<uint8_t> data2 = boost::shared_array<uint8_t>(new uint8_t[8192]);
    RR_SHARED_PTR<boost::asio::ip::udp::endpoint> ep6 =
        RR_MAKE_SHARED<boost::asio::ip::udp::endpoint>(boost::asio::ip::address_v6::any(), ANNOUNCE_PORT);
    ip6_listen1->async_receive_from(boost::asio::buffer(data2.get(), 8192), *ep6,
                                    boost::bind(&IPNodeDiscovery::handle_receive, shared_from_this(),
                                                boost::asio::placeholders::error,
                                                boost::asio::placeholders::bytes_transferred, ip6_listen1, ep6, data2));

    ip6_listen.push_back(ip6_listen1);
    ip6_listen_scope_ids.push_back(ep.address().to_v6().scope_id());
}

void IPNodeDiscovery::StartListeningForNodes(uint32_t flags)
{
    {
        boost::mutex::scoped_lock lock(change_lock);

        this_request_id = NodeID::NewUniqueID();

        if (flags == 0)
            flags = IPNodeDiscoveryFlags_NODE_LOCAL | IPNodeDiscoveryFlags_SITE_LOCAL |
                    IPNodeDiscoveryFlags_LINK_LOCAL | IPNodeDiscoveryFlags_IPV4_BROADCAST;

        if (listening)
        {
            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, -1,
                                               "TcpTransport discovery already listening for nodes");
            throw InvalidOperationException("Already listening for nodes");
        }

        // if (!flags & IPNodeDiscoveryFlags_IPV4_BROADCAST)
        //	flags |= IPNodeDiscoveryFlags_IPV4_BROADCAST;
        listening = true;
        listen_flags = flags;

        start_listen_sockets();
    }

    SendDiscoveryRequestNow();
}

void IPNodeDiscovery::handle_receive(const boost::system::error_code& error, std::size_t bytes_transferred,
                                     const RR_SHARED_PTR<boost::asio::ip::udp::socket>& socket,
                                     const RR_SHARED_PTR<boost::asio::ip::udp::endpoint>& ep,
                                     const boost::shared_array<uint8_t>& buffer)
{
    {
        boost::mutex::scoped_lock lock(change_lock);
        if (!listening)
            return;
    }

    // if (error) return;

    if (!error)
    {
        ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, -1,
                                           "TcpTransport discovery received packet length " << bytes_transferred
                                                                                            << " from " << *ep);

        std::string s(reinterpret_cast<char*>(buffer.get()), bytes_transferred);
        try
        {

            NodeAnnounceReceived(s, *ep);
        }
        catch (std::exception& exp)
        {
            ROBOTRACONTEUR_LOG_TRACE_COMPONENT(
                node, Transport, -1, "TcpTransport discovery error processing node announce packet: " << exp.what());
        }
    }

    boost::mutex::scoped_lock lock(change_lock);

    socket->async_receive_from(boost::asio::buffer(buffer.get(), 8192), *ep,
                               boost::bind(&IPNodeDiscovery::handle_receive, shared_from_this(),
                                           boost::asio::placeholders::error,
                                           boost::asio::placeholders::bytes_transferred, socket, ep, buffer));
}

void IPNodeDiscovery::handle_receive_update_timer(const boost::system::error_code& error)
{

    if (error == boost::asio::error::operation_aborted)
    {
        return;
    }

    boost::mutex::scoped_lock lock1(change_lock);

    if (!listening)
        return;

    std::vector<boost::asio::ip::address> local_addresses;

    TcpTransport::GetLocalAdapterIPAddresses(local_addresses);

    BOOST_FOREACH (boost::asio::ip::address& e, local_addresses)
    {
        if (e.is_v6())
        {
            try
            {
                size_t count = 0;

                count = std::count(ip6_listen_scope_ids.begin(), ip6_listen_scope_ids.end(), e.to_v6().scope_id());

                if (count == 0)
                {
                    boost::asio::ip::address_v6 addr6 = boost::asio::ip::address_v6::any();
                    addr6.scope_id(e.to_v6().scope_id());
                    start_ipv6_listen_socket(boost::asio::ip::udp::endpoint(e, ANNOUNCE_PORT));
                }
            }
            catch (std::exception&)
            {}
        }
    }

    receive_update_timer->expires_from_now(boost::posix_time::seconds(5));
    RobotRaconteurNode::asio_async_wait(node, receive_update_timer,
                                        boost::bind(&IPNodeDiscovery::handle_receive_update_timer, shared_from_this(),
                                                    boost::asio::placeholders::error));
}

void IPNodeDiscovery::NodeAnnounceReceived(boost::string_ref packet, const boost::asio::ip::udp::endpoint& send_ep)
{
    if (listening)
    {
        try
        {
            std::string magic = "Robot Raconteur Node Discovery Packet";
            if (packet.substr(0, magic.length()) == magic)
            {
                std::vector<std::string> s1;
                boost::split(s1, packet, boost::is_from_range('\n', '\n'));
                if (s1.size() < 3)
                    return;
                std::vector<std::string> s2;
                boost::split(s2, s1.at(1), boost::is_from_range(',', ','));
                NodeID id(s2.at(0));

                std::string url = s1.at(2);

                // If the URL or nodename is excessively long, just ignore it
                if (url.size() > 256)
                {
                    throw InvalidArgumentException("Invalid URL in packet");
                }

                ParseConnectionURLResult u = ParseConnectionURL(url);

                std::string u_host = u.host;
                if (boost::starts_with(u_host, "[") && boost::ends_with(u_host, "]"))
                {
                    u_host = u_host.substr(1, u_host.size() - 2);
                }
                boost::system::error_code address_ec;
                boost::asio::ip::address addr = RR_BOOST_ASIO_IP_ADDRESS_FROM_STRING(u_host);
                if (address_ec)
                {
                    throw InvalidArgumentException("Invalid IP address in packet");
                }
                if (!addr.is_loopback())
                {
                    if (addr.is_v4() != send_ep.address().is_v4() || addr.is_v6() != send_ep.address().is_v6())
                    {
                        throw InvalidOperationException("Discovery packet address family mismatch");
                    }

                    if (addr.is_v6())
                    {
                        boost::asio::ip::address_v6 addr_v6 = addr.to_v6();
                        if (!addr_v6.is_link_local() && !addr_v6.is_site_local())
                        {
                            throw InvalidArgumentException(
                                "Only link and site local addresses may be used for discovery");
                        }
                    }

                    if (addr.is_v4())
                    {
                        boost::asio::ip::address_v4::bytes_type addr_v4 = addr.to_v4().to_bytes();
                        boost::asio::ip::address_v4::bytes_type sender_v4 = send_ep.address().to_v4().to_bytes();

                        if (addr_v4[0] != sender_v4[0] || addr_v4[1] != sender_v4[1])
                        {
                            throw InvalidArgumentException("Subnet mismatch for discovery");
                        }
                    }
                }

                if (id != GetNode()->NodeID())
                {
                    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(
                        node, Transport, -1, "TcpTransport discovery received node announce, forwarding to node");
                    GetNode()->NodeAnnouncePacketReceived(packet);
                }
            }
        }
        catch (std::exception&)
        {}
    }

    if (broadcasting)
    {
        try
        {
            std::string magic = "Robot Raconteur Discovery Request Packet";
            if (packet.substr(0, magic.length()) == magic)
            {
                std::vector<std::string> s1;
                boost::split(s1, packet, boost::is_from_range('\n', '\n'));
                if (s1.size() < 3)
                    return;
                std::vector<std::string> s2;
                boost::split(s2, s1.at(1), boost::is_from_range(',', ','));
                NodeID id(s2.at(0));
                if (id != this_request_id)
                {
                    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(
                        node, Transport, -1, "TcpTransport discovery received discovery request, sending announce now");
                    SendAnnounceNow();
                }
            }
        }
        catch (std::exception&)
        {}
    }
}

void IPNodeDiscovery::StopListeningForNodes()
{

    boost::mutex::scoped_lock lock(change_lock);
    if (!listening)
        return;
    listening = false;

    if (ip4_listen)
    {
        ip4_listen->shutdown(boost::asio::ip::udp::socket::shutdown_both);
        ip4_listen->close();
    }

    boost::mutex ip6_listen_lock;
    BOOST_FOREACH (RR_SHARED_PTR<boost::asio::ip::udp::socket>& ip6_listen1, ip6_listen)
    {
        if (ip6_listen1)
        {
            (ip6_listen1)->shutdown(boost::asio::ip::udp::socket::shutdown_both);
            (ip6_listen1)->close();
        }
    }
    if (receive_update_timer)
        receive_update_timer->cancel();
}

void IPNodeDiscovery::StartAnnouncingNode(uint32_t flags)
{
    boost::mutex::scoped_lock lock(change_lock);
    if (broadcasting)
        return;
    broadcasting = true;
    if (flags == 0)
        flags = IPNodeDiscoveryFlags_NODE_LOCAL | IPNodeDiscoveryFlags_SITE_LOCAL | IPNodeDiscoveryFlags_LINK_LOCAL;

    broadcast_flags = flags;

    broadcast_timer = RR_SHARED_PTR<boost::asio::deadline_timer>(
        new boost::asio::deadline_timer(GetNode()->GetThreadPool()->get_io_context()));

    broadcast_timer->expires_from_now(boost::posix_time::milliseconds(500));
    RobotRaconteurNode::asio_async_wait(
        node, broadcast_timer,
        boost::bind(&IPNodeDiscovery::handle_broadcast_timer, shared_from_this(), boost::asio::placeholders::error));

    // RobotRaconteurNode::TryPostToThreadPool(node,
    // boost::bind(&IPNodeDiscovery::handle_broadcast_timer,shared_from_this(),boost::system::error_code()));

    start_listen_sockets();
}

void IPNodeDiscovery::StopAnnouncingNode()
{
    boost::mutex::scoped_lock lock(change_lock);

    if (!broadcasting)
        return;

    broadcasting = false;

    broadcast_timer->cancel();
}

void IPNodeDiscovery::handle_broadcast_timer(const boost::system::error_code& error)
{
    if (error == boost::asio::error::operation_aborted)
    {
        return;
    }

    RR_SHARED_PTR<TcpTransport> p = parent.lock();
    if (!p)
        return;
    int port = p->GetListenPort();

    if (port != 0)
    {
        std::vector<boost::asio::ip::address> local_addresses1;

        TcpTransport::GetLocalAdapterIPAddresses(local_addresses1);

        local_addresses1.push_back(boost::asio::ip::address_v4::loopback());

        std::set<boost::asio::ip::address> local_addresses(local_addresses1.begin(), local_addresses1.end());

        /*Transport::m_CurrentThreadTransportConnectionURL.reset(new std::string("tcp://localhost:0/"));
        RR_SHARED_PTR<ServiceIndexer> indexer=RR_MAKE_SHARED<ServiceIndexer>();
        RR_INTRUSIVE_PTR<RobotRaconteur::RRMap<int32_t,RobotRaconteurServiceIndex::ServiceInfo> >
        services=indexer->GetLocalNodeServices(); Transport::m_CurrentThreadTransportConnectionURL.reset(0);

        std::vector<std::string> service_data2;
        for (std::map<int32_t,RR_INTRUSIVE_PTR<RobotRaconteurServiceIndex::ServiceInfo> >::iterator
        e=services->map.begin(); e!=services->map.end(); e++)
        {
        std::vector<std::string> implements;
        implements.push_back(e->second->RootObjectType);
        RR_INTRUSIVE_PTR<RobotRaconteur::RRMap<int32_t,RobotRaconteur::RRArray<char> > > impl=
        e->second->RootObjectImplements; for (std::map<int32_t,RR_INTRUSIVE_PTR<RRArray<char> > >::iterator
        e2=impl->map.begin(); e2!=impl->map.end(); e2++)
        {
        implements.push_back(RRArrayToString(e2->second));
        }

        std::string one="Service: " + e->second->Name + "=" + boost::join(implements,",");
        service_data2.push_back(one);

        }
        std::string service_data=boost::join(service_data2,"\n");
        */

        std::vector<std::string> tcpschemes;
        // tcpschemes.push_back("tcp");

        tcpschemes.push_back("rr+tcp");

        if (p->IsTlsNodeCertificateLoaded())
        {
            tcpschemes.push_back("rrs+tcp");
        }

        BOOST_FOREACH (const boost::asio::ip::address& e, local_addresses)
        {
            BOOST_FOREACH (std::string& ee, tcpschemes)
            {

                try
                {
                    if (e.is_v6())
                    {
                        boost::asio::ip::address_v6 v6addr = e.to_v6();
                        if (!(v6addr.is_link_local() || v6addr.is_loopback()))
                            continue;
                    }

                    std::string packetdata = generate_response_packet(e, ee, port);

                    broadcast_discovery_packet(e, packetdata, static_cast<IPNodeDiscoveryFlags>(broadcast_flags));
                    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, -1,
                                                       "TcpTransport discovery sent broadcast packet to "
                                                           << e << " \""
                                                           << boost::replace_all_copy(packetdata, "\n", "\\n"));
                }
                catch (std::exception& exp)
                {
                    ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(
                        node, Transport, -1, "TcpTransport discovery failed sending broadcast packet: " << exp.what());
                }
            }
        }
    }

    boost::mutex::scoped_lock lock(change_lock);
    if (!broadcasting)
        return;
    boost::posix_time::time_duration fromnow = broadcast_timer->expires_from_now();
    if (fromnow > boost::posix_time::milliseconds(broadcast_timer_period) ||
        fromnow <= boost::posix_time::milliseconds(0))
    {
        broadcast_timer->expires_from_now(boost::posix_time::milliseconds(broadcast_timer_period));
        RobotRaconteurNode::asio_async_wait(node, broadcast_timer,
                                            boost::bind(&IPNodeDiscovery::handle_broadcast_timer, shared_from_this(),
                                                        boost::asio::placeholders::error));
    }
}

void IPNodeDiscovery::handle_send(const boost::system::error_code& /*error*/, std::size_t /*bytes_transferred*/,
                                  const RR_SHARED_PTR<std::string>& /*message*/)
{}

void IPNodeDiscovery::broadcast_discovery_packet(const boost::asio::ip::address& source, boost::string_ref packet,
                                                 IPNodeDiscoveryFlags flags)
{
    RR_BOOST_ASIO_IO_CONTEXT ios;
    boost::asio::ip::udp::socket s(ios);

    if (source.is_v4())
    {
        s.open(boost::asio::ip::udp::v4());
    }
    else
    {
        s.open(boost::asio::ip::udp::v6());
    }

    // s.set_option(boost::asio::ip::udp::socket::linger(false,10));

    RR_SHARED_PTR<std::string> shared_message = RR_MAKE_SHARED<std::string>(packet.to_string());

    s.set_option(boost::asio::ip::udp::socket::reuse_address(true));
#ifdef ROBOTRACONTEUR_WINDOWS
    u_long exclusiveaddruse = 0;
    ::setsockopt(s.native_handle(), SOL_SOCKET, SO_EXCLUSIVEADDRUSE, (const char*)&exclusiveaddruse,
                 sizeof(exclusiveaddruse));
#endif

    boost::asio::ip::udp::endpoint bind_ep(boost::asio::ip::udp::endpoint(source, ANNOUNCE_PORT));

    boost::system::error_code bind_ec;
    s.bind(bind_ep, bind_ec);
    if (bind_ec)
        return;

    if (source.is_v4())
    {
        if ((static_cast<uint32_t>(flags) & (static_cast<uint32_t>(IPNodeDiscoveryFlags_IPV4_BROADCAST))) != 0)
        {
            boost::system::error_code ec2;
            boost::asio::ip::udp::endpoint dest_ep(boost::asio::ip::address_v4::broadcast(), ANNOUNCE_PORT);
            s.set_option(boost::asio::ip::multicast::enable_loopback(true));
            s.set_option(boost::asio::ip::udp::socket::broadcast(true));
            // s.async_send_to(boost::asio::buffer(*shared_message),dest_ep,boost::bind(&IPNodeDiscovery::handle_send,shared_from_this(),boost::asio::placeholders::error,boost::asio::placeholders::bytes_transferred,shared_message));
            s.send_to(boost::asio::buffer(*shared_message), dest_ep, 0, ec2);
        }
    }
    else
    {
        boost::asio::ip::address_v6 e6 = source.to_v6();
        s.set_option(boost::asio::ip::multicast::enable_loopback(true));

        if ((static_cast<uint32_t>(flags) & (static_cast<uint32_t>(IPNodeDiscoveryFlags_NODE_LOCAL))) != 0)
        {
            boost::system::error_code ec1;
            boost::system::error_code ec2;
            Ip6AddMembership(s, RR_BOOST_ASIO_IP_ADDRESS_V6_FROM_STRING("FF01::BA86"), e6.scope_id());
            boost::asio::ip::address_v6 ip6addra = RR_BOOST_ASIO_IP_ADDRESS_V6_FROM_STRING("FF01::BA86");
            ip6addra.scope_id(e6.scope_id());
            boost::asio::ip::udp::endpoint dest_ep(ip6addra, ANNOUNCE_PORT);

            // s.async_send_to(boost::asio::buffer(*shared_message),dest_ep,boost::bind(&IPNodeDiscovery::handle_send,shared_from_this(),boost::asio::placeholders::error,boost::asio::placeholders::bytes_transferred,shared_message));
            s.send_to(boost::asio::buffer(*shared_message), dest_ep, 0, ec2);
        }

        boost::asio::ip::address_v6 e6_2 = e6;
        e6_2.scope_id(0);
        if (!source.is_loopback() && e6_2 != RR_BOOST_ASIO_IP_ADDRESS_V6_FROM_STRING("FF80::1"))
        {

            if ((static_cast<uint32_t>(flags) & (static_cast<uint32_t>(IPNodeDiscoveryFlags_LINK_LOCAL))) != 0)
            {
                boost::system::error_code ec1;
                boost::system::error_code ec2;
                Ip6AddMembership(s, RR_BOOST_ASIO_IP_ADDRESS_V6_FROM_STRING("FF02::BA86"), e6.scope_id());
                boost::asio::ip::address_v6 ip6addra = RR_BOOST_ASIO_IP_ADDRESS_V6_FROM_STRING("FF02::BA86");
                ip6addra.scope_id(e6.scope_id());
                boost::asio::ip::udp::endpoint dest_ep(ip6addra, ANNOUNCE_PORT);
                // s.async_send_to(boost::asio::buffer(*shared_message),dest_ep,boost::bind(&IPNodeDiscovery::handle_send,shared_from_this(),boost::asio::placeholders::error,boost::asio::placeholders::bytes_transferred,shared_message));
                s.send_to(boost::asio::buffer(*shared_message), dest_ep, 0, ec2);
            }

            if ((static_cast<uint32_t>(flags) & (static_cast<uint32_t>(IPNodeDiscoveryFlags_SITE_LOCAL))) != 0)
            {
                boost::system::error_code ec1;
                boost::system::error_code ec2;
                Ip6AddMembership(s, RR_BOOST_ASIO_IP_ADDRESS_V6_FROM_STRING("FF05::BA86"), e6.scope_id());
                boost::asio::ip::address_v6 ip6addra = RR_BOOST_ASIO_IP_ADDRESS_V6_FROM_STRING("FF05::BA86");
                ip6addra.scope_id(e6.scope_id());
                boost::asio::ip::udp::endpoint dest_ep(ip6addra, ANNOUNCE_PORT);
                // s.async_send_to(boost::asio::buffer(*shared_message),dest_ep,boost::bind(&IPNodeDiscovery::handle_send,shared_from_this(),boost::asio::placeholders::error,boost::asio::placeholders::bytes_transferred,shared_message));
                s.send_to(boost::asio::buffer(*shared_message), dest_ep, 0, ec2);
            }
        }
    }

    try
    {
        s.shutdown(boost::asio::ip::udp::socket::shutdown_both);
        s.close();
    }
    catch (std::exception&)
    {}
}

std::string IPNodeDiscovery::generate_response_packet(const boost::asio::ip::address& source, boost::string_ref scheme,
                                                      int port)
{
    std::string nodeidstring = GetNode()->NodeID().ToString();
    std::string packetdata = "Robot Raconteur Node Discovery Packet\n";
    packetdata +=
        (GetNode()->NodeName().empty()) ? nodeidstring + "\n" : nodeidstring + "," + GetNode()->NodeName() + "\n";

    std::string nodeidstr2 = GetNode()->NodeID().ToString("D");

    if (source.is_v4())
    {
        packetdata += scheme + "://" + source.to_string() + ":" + boost::lexical_cast<std::string>(port) +
                      "/?nodeid=" + nodeidstr2 + "&service=RobotRaconteurServiceIndex\n";
    }
    else
    {
        boost::asio::ip::address_v6 e2 = source.to_v6();
        e2.scope_id(0);
        packetdata += scheme + "://[" + e2.to_string() + "]:" + boost::lexical_cast<std::string>(port) +
                      "/?nodeid=" + nodeidstr2 + "&service=RobotRaconteurServiceIndex\n";
    }

    std::string service_data = "ServiceStateNonce: " + GetNode()->GetServiceStateNonce();

    std::string packetdata2 = packetdata + service_data + "\n";
    if (packetdata2.size() <= 2048)
    {
        packetdata = packetdata2;
    }

    return packetdata;
}

void IPNodeDiscovery::SendAnnounceNow()
{
    boost::mutex::scoped_lock lock(change_lock);

    if (!broadcasting)
        return;

    if (broadcast_timer)
    {
        boost::posix_time::time_duration fromnow = broadcast_timer->expires_from_now();
        if (fromnow > boost::posix_time::milliseconds(500) ||
            broadcast_timer->expires_from_now() < boost::posix_time::milliseconds(0))
        {
            broadcast_timer->expires_from_now(boost::posix_time::milliseconds(500));
            RobotRaconteurNode::asio_async_wait(node, broadcast_timer,
                                                boost::bind(&IPNodeDiscovery::handle_broadcast_timer,
                                                            shared_from_this(), boost::asio::placeholders::error));
        }
    }
}

void IPNodeDiscovery::SendDiscoveryRequestNow()
{
    boost::mutex::scoped_lock lock(change_lock);
    if (!listening)
        return;
    last_request_send_time = boost::posix_time::microsec_clock::universal_time();

    if (!discovery_request_timer)
    {
        RR_SHARED_PTR<TcpTransport> p1 = parent.lock();
        if (!p1)
            return;

        discovery_request_timer = RR_SHARED_PTR<boost::asio::deadline_timer>(
            new boost::asio::deadline_timer(p1->GetNode()->GetThreadPool()->get_io_context()));
        uint32_t delay = p1->GetNode()->GetRandomInt<uint32_t>(250, 1000);
        discovery_request_timer->expires_from_now(boost::posix_time::milliseconds(delay));
        RobotRaconteurNode::asio_async_wait(
            node, discovery_request_timer,
            boost::bind(&IPNodeDiscovery::handle_request_timer, this, boost::asio::placeholders::error, 3));
    }
}

void IPNodeDiscovery::handle_request_timer(const boost::system::error_code& error, int32_t c)
{
    if (error == boost::asio::error::operation_aborted)
    {
        return;
    }

    RR_SHARED_PTR<TcpTransport> p1 = parent.lock();
    if (!p1)
        return;

    boost::mutex::scoped_lock lock(change_lock);

    std::string packetdata = "Robot Raconteur Discovery Request Packet\n";
    packetdata += this_request_id.ToString() + "\n";

    std::vector<boost::asio::ip::address> local_addresses1;

    TcpTransport::GetLocalAdapterIPAddresses(local_addresses1);

    local_addresses1.push_back(boost::asio::ip::address_v4::loopback());

    std::set<boost::asio::ip::address> local_addresses(local_addresses1.begin(), local_addresses1.end());

    BOOST_FOREACH (const boost::asio::ip::address& e, local_addresses)
    {
        try
        {
            if (e.is_v6())
            {
                boost::asio::ip::address_v6 v6addr = e.to_v6();
                if (!(v6addr.is_link_local() || v6addr.is_loopback()))
                    continue;
            }

            broadcast_discovery_packet(e, packetdata, static_cast<IPNodeDiscoveryFlags>(listen_flags));
            ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, -1,
                                               "TcpTransport discovery sent broadcast packet to "
                                                   << e << " \"" << boost::replace_all_copy(packetdata, "\n", "\\n"));
        }
        catch (std::exception& exp)
        {
            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(
                node, Transport, -1, "TcpTransport discovery failed sending discovery request packet: " << exp.what());
        }
    }

    c--;

    if (c > 0)
    {
        uint32_t delay = p1->GetNode()->GetRandomInt<uint32_t>(900, 1500);
        discovery_request_timer->expires_from_now(boost::posix_time::milliseconds(delay));
        RobotRaconteurNode::asio_async_wait(node, discovery_request_timer,
                                            boost::bind(&IPNodeDiscovery::handle_request_timer, shared_from_this(),
                                                        boost::asio::placeholders::error, c));
    }
    else
    {
        if ((last_request_send_time + boost::posix_time::milliseconds(1000)) >
            boost::posix_time::microsec_clock::universal_time())
        {
            discovery_request_timer->expires_from_now(boost::posix_time::seconds(5));
            RobotRaconteurNode::asio_async_wait(node, discovery_request_timer,
                                                boost::bind(&IPNodeDiscovery::handle_request_timer, shared_from_this(),
                                                            boost::asio::placeholders::error, 3));
        }
        else
        {
            discovery_request_timer.reset();
        }
    }
}

int32_t IPNodeDiscovery::GetNodeAnnouncePeriod()
{
    boost::mutex::scoped_lock lock(change_lock);
    return broadcast_timer_period;
}

void IPNodeDiscovery::SetNodeAnnouncePeriod(int32_t millis)
{
    boost::mutex::scoped_lock lock(change_lock);
    if (millis < 500)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, -1, "Invalid node announce period " << millis << " ms");
        throw InvalidOperationException("Invalid node announce period");
    }
    broadcast_timer_period = millis;
    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, -1, "NodeAnnouncePeriod set to " << millis << " ms");
}

const int32_t IPNodeDiscovery::ANNOUNCE_PORT = 48653;

TcpTransportPortSharerClient::TcpTransportPortSharerClient(const RR_SHARED_PTR<TcpTransport>& parent)
{
    this->parent = parent;
    this->node = parent->GetNode();
    open = false;
    delay_event = RR_MAKE_SHARED<AutoResetEvent>();
    port = 0;
    sharer_connected = false;
}

void TcpTransportPortSharerClient::Start()
{
    boost::mutex::scoped_lock lock(this_lock);
    if (open)
        throw InvalidOperationException("Already running server");
    open = true;
    boost::thread(boost::bind(&TcpTransportPortSharerClient::client_thread, shared_from_this()));
}

int32_t TcpTransportPortSharerClient::GetListenPort()
{
    boost::mutex::scoped_lock lock(this_lock);
    return boost::numeric_cast<int32_t>(port);
}

void TcpTransportPortSharerClient::Close()
{
    boost::mutex::scoped_lock lock(this_lock);
    open = false;
    if (localsocket)
    {
        boost::system::error_code ec;
        if (localsocket)
        {
            localsocket->close(ec);
        }
        localsocket.reset();
    }

    delay_event->Set();
}

RR_SHARED_PTR<TcpTransport> TcpTransportPortSharerClient::GetParent()
{
    RR_SHARED_PTR<TcpTransport> p = parent.lock();
    if (!p)
        throw InvalidOperationException("Parent released");
    return p;
}

void TcpTransportPortSharerClient::client_thread()
{
    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, -1, "TcpTransport port sharer client started");

    NodeDirectories node_dirs;
    bool node_dirs_init = false;
    while (true)
    {
        {
            boost::mutex::scoped_lock lock(this_lock);
            if (!open)
                break;
            localsocket.reset();
        }

        try
        {
            NodeID nodeid;
            std::string nodename;
            {
                RR_SHARED_PTR<RobotRaconteurNode> node = GetParent()->GetNode();
                nodeid = node->NodeID();
                nodename = node->NodeName();
            }

            if (nodeid.IsAnyNode())
                throw InternalErrorException("Internal error");

            std::string outdata = nodeid.ToString() + " " + nodename;
            boost::trim(outdata);

            if (!node_dirs_init)
            {
                node_dirs = GetParent()->GetNode()->GetNodeDirectories();
            }

#ifdef ROBOTRACONTEUR_WINDOWS

            std::string fname;

            boost::optional<boost::filesystem::path> p1_1 = node_dirs.system_run_dir;
            if (p1_1)
            {
                boost::filesystem::path p1 = *p1_1;
                p1 /= "transport";
                p1 /= "tcp";
                p1 /= "portsharer";
                p1 /= "portsharer.info";
                p1.normalize();

                std::map<std::string, std::string> info;
                if (NodeDirectoriesUtil::ReadInfoFile(p1, info))
                {

                    std::map<std::string, std::string>::iterator fname1 = info.find("socket");
                    if (fname1 != info.end())
                    {
                        fname = fname1->second;
                    }
                }
            }

            if (fname.empty())
            {
                boost::filesystem::path p1 = detail::LocalTransportUtil::GetTransportPrivateSocketPath(node_dirs);
                p1 /= "..";
                p1 /= "tcp";
                p1 /= "portsharer";
                p1 /= "portsharer.info";
                p1.normalize();

                std::map<std::string, std::string> info;
                if (NodeDirectoriesUtil::ReadInfoFile(p1, info))
                {

                    std::map<std::string, std::string>::iterator fname1 = info.find("socket");
                    if (fname1 != info.end())
                    {
                        fname = fname1->second;
                    }
                }
            }

            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(
                node, Transport, -1, "TcpTransport port sharer connecting named pipe to: \"" << fname << "\"");

            ::WaitNamedPipeA(fname.c_str(), 100);
            HANDLE h = CreateFileA(fname.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING,
                                   FILE_FLAG_OVERLAPPED | SECURITY_SQOS_PRESENT | SECURITY_IDENTIFICATION, NULL);
            if (h == INVALID_HANDLE_VALUE)
            {
                delay_event->WaitOne(1000);
                continue;
            }

            DWORD params = PIPE_READMODE_MESSAGE;
            if (!SetNamedPipeHandleState(h, &params, NULL, NULL))
            {
                ::CloseHandle(h);
                throw InternalErrorException("Internal error");
            }

            RR_SHARED_PTR<boost::asio::windows::stream_handle> l;
            {
                boost::mutex::scoped_lock lock(this_lock);
                RR_SHARED_PTR<RobotRaconteurNode> node = GetParent()->GetNode();
                l.reset(new boost::asio::windows::stream_handle(node->GetThreadPool()->get_io_context(), h));

                /*if (!detail::LocalTransportUtil::IsPipeSameUserOrService(l->native_handle(), true))
                {
                    l.reset();
                    delay_event->WaitOne(1000);
                    continue;
                }*/

                localsocket = l;
            }

#else
            std::string fname = "/var/run/robotraconteur/transport/tcp/portsharer/portsharer.sock";
            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(
                node, Transport, -1, "TcpTransport port sharer connecting named pipe to: \"" << fname << "\"");
            RR_SHARED_PTR<boost::asio::local::stream_protocol::socket> l(
                new boost::asio::local::stream_protocol::socket(
                    GetParent()->GetNode()->GetThreadPool()->get_io_context()));
            l->connect(boost::asio::local::stream_protocol::endpoint(fname));

            {
                boost::mutex::scoped_lock lock(this_lock);
                localsocket = l;
            }
#endif

            if (!l)
            {
                delay_event->WaitOne(1000);
                continue;
            }

            ROBOTRACONTEUR_LOG_INFO_COMPONENT(node, Transport, -1, "TcpTransport port sharer successfully connected");

            boost::asio::write(*l, boost::asio::buffer(outdata.c_str(), outdata.size()));

            int port1 = 0;

            {
                boost::array<uint8_t, 4096> indata2 = {};

                int32_t nread =
                    boost::numeric_cast<int32_t>(l->read_some(boost::asio::buffer(indata2.data(), sizeof(indata2))));

                if (nread == 0)
                    throw InvalidOperationException("Connection closed");
                std::string indata1(reinterpret_cast<char*>(indata2.data()), nread);

                if (!boost::starts_with(indata1, "OK"))
                {
                    throw OperationFailedException("Return error");
                }

                std::vector<std::string> indata_s;
                boost::split(indata_s, indata1, boost::is_any_of(" \t"), boost::algorithm::token_compress_on);
                if (indata_s.size() != 2)
                    throw OperationFailedException("Return error");

                port1 = boost::lexical_cast<int32_t>(indata_s.at(1));

                {
                    boost::mutex::scoped_lock lock(this_lock);
                    port = port1;
                }
            }

            while (true)
            {
                {
                    boost::mutex::scoped_lock lock(this_lock);
                    if (!open)
                        break;
                    sharer_connected = true;
                }

#ifdef ROBOTRACONTEUR_WINDOWS
                boost::array<uint8_t, 4096> buf = {};
                int bytes_read = (int32_t)l->read_some(boost::asio::buffer(buf.data(), 4096));

                if (bytes_read != sizeof(WSAPROTOCOL_INFOW))
                    throw InternalErrorException("Invalid data");

                LPWSAPROTOCOL_INFOW prot = (LPWSAPROTOCOL_INFOW)buf.data();

                SOCKET sock =
                    ::WSASocketW(prot->iAddressFamily, SOCK_STREAM, IPPROTO_TCP, prot, 0, WSA_FLAG_OVERLAPPED);
                if (sock == INVALID_SOCKET)
                    continue;
                try
                {
                    boost::mutex::scoped_lock lock(this_lock);
                    RR_SHARED_PTR<RobotRaconteurNode> node = GetParent()->GetNode();
                    RR_SHARED_PTR<boost::asio::ip::tcp::socket> ssocket;
                    if (prot->iAddressFamily == AF_INET)
                    {
                        ssocket.reset(new boost::asio::ip::tcp::socket(node->GetThreadPool()->get_io_context(),
                                                                       boost::asio::ip::tcp::v4(), sock));
                    }
                    else if (prot->iAddressFamily == AF_INET6)
                    {
                        ssocket.reset(new boost::asio::ip::tcp::socket(node->GetThreadPool()->get_io_context(),
                                                                       boost::asio::ip::tcp::v6(), sock));
                    }
                    else
                    {
                        continue;
                    }
                    IncomingSocket(ssocket);
                }
                catch (std::exception& exp2)
                {
                    ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(
                        node, Transport, -1, "TcpTransport port sharer socket handoff failed: " << exp2.what());
                }
#else

                int sock = 0;

                boost::array<uint8_t, 1024> buf = {};

                if (detail::TcpTransportUtil::read_fd(l->native_handle(), buf.data(), sizeof(buf), &sock) != 1)
                {
                    break;
                }

                if (sock == 0)
                    break;
                if (sock < 0)
                    continue;

                try
                {
                    boost::mutex::scoped_lock lock(this_lock);
                    RR_SHARED_PTR<RobotRaconteurNode> node = GetParent()->GetNode();
                    RR_SHARED_PTR<boost::asio::ip::tcp::socket> ssocket;

                    struct sockaddr sock_addr = {};
                    socklen_t address_len = sizeof(struct sockaddr);

                    if (getsockname(sock, &sock_addr, &address_len) < 0)
                        continue;

                    if (sock_addr.sa_family == AF_INET)
                    {
                        ssocket = RR_SHARED_PTR<boost::asio::ip::tcp::socket>(new boost::asio::ip::tcp::socket(
                            node->GetThreadPool()->get_io_context(), boost::asio::ip::tcp::v4(), sock));
                    }
                    else if (sock_addr.sa_family == AF_INET6)
                    {
                        ssocket = RR_SHARED_PTR<boost::asio::ip::tcp::socket>(new boost::asio::ip::tcp::socket(
                            node->GetThreadPool()->get_io_context(), boost::asio::ip::tcp::v6(), sock));
                    }
                    else
                    {
                        continue;
                    }

                    IncomingSocket(ssocket);
                }
                catch (std::exception& exp2)
                {
                    ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(
                        node, Transport, -1, "TcpTransport port sharer socket handoff failed: " << exp2.what());
                }
#endif
            }
        }
        catch (std::exception& exp)
        {
            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, -1,
                                               "TcpTransport port sharer connect failed: " << exp.what());
        }

        {
            boost::mutex::scoped_lock lock(this_lock);
            ROBOTRACONTEUR_LOG_INFO_COMPONENT(node, Transport, -1, "TcpTransport port sharer disconnected");
            sharer_connected = false;
            boost::system::error_code ec;
            if (localsocket)
            {
                localsocket->close();
            }
            localsocket.reset();
            port = 0;
        }

        delay_event->WaitOne(1000);
    }

    {
        boost::mutex::scoped_lock lock(this_lock);
        boost::system::error_code ec;
        if (localsocket)
        {
            ROBOTRACONTEUR_LOG_INFO_COMPONENT(node, Transport, -1, "TcpTransport port sharer disconnected");
            localsocket->close();
        }
        localsocket.reset();
        port = 0;
    }
}

void TcpTransportPortSharerClient::IncomingSocket(const RR_SHARED_PTR<boost::asio::ip::tcp::socket>& socket)
{
    ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, -1,
                                       "TcpTransport port sharer accepted socket from "
                                           << TcpTransport_socket_remote_endpoint(socket) << " to "
                                           << TcpTransport_socket_local_endpoint(socket));

    RR_SHARED_PTR<TcpTransport> parent1 = GetParent();
    try
    {
        socket->set_option(boost::asio::ip::tcp::socket::linger(true, 5));

        // TcpTransport_attach_transport(parent1, socket, "", true, 0, boost::bind(&TcpTransport_connected_callback2,
        // parent1, RR_BOOST_PLACEHOLDERS(_1), RR_BOOST_PLACEHOLDERS(_2), RR_BOOST_PLACEHOLDERS(_3)));
        RR_SHARED_PTR<detail::TcpAcceptor> a = RR_MAKE_SHARED<detail::TcpAcceptor>(parent1, "", 0);
        boost::function<void(RR_SHARED_PTR<boost::asio::ip::tcp::socket>, const RR_SHARED_PTR<ITransportConnection>&,
                             const RR_SHARED_PTR<RobotRaconteurException>&)>
            h = boost::bind(&TcpTransport_connected_callback2, parent1, RR_BOOST_PLACEHOLDERS(_1),
                            RR_BOOST_PLACEHOLDERS(_2), RR_BOOST_PLACEHOLDERS(_3));
        a->AcceptSocket(socket, h);
    }
    catch (std::exception& exp)
    {
        try
        {
            RobotRaconteurNode::TryHandleException(parent1->GetNode(), &exp);
        }
        catch (std::exception&)
        {}
    }
}

bool TcpTransportPortSharerClient::IsPortSharerConnected()
{
    boost::mutex::scoped_lock lock(this_lock);
    return sharer_connected;
}

#ifndef ROBOTRACONTEUR_WINDOWS
#ifdef SUNOS5
#undef HAVE_MSGHDR_MSG_CONTROL
#else
#define HAVE_MSGHDR_MSG_CONTROL
#endif

namespace TcpTransportUtil
{

// NOLINTBEGIN(cppcoreguidelines-avoid-c-arrays,cppcoreguidelines-pro-type-cstyle-cast)
// NOLINTBEGIN(cppcoreguidelines-pro-type-member-init)
ssize_t read_fd(int fd, void* ptr, size_t nbytes, int* recvfd)
{
    struct msghdr msg = {};
    struct iovec iov[1];
    ssize_t n = 0;
#ifndef HAVE_MSGHDR_MSG_CONTROL
    int newfd;
#endif

#ifdef HAVE_MSGHDR_MSG_CONTROL
    union {
        struct cmsghdr cm;
        char control[CMSG_SPACE(sizeof(int))];
    } control_un;
    struct cmsghdr* cmptr = NULL;

    msg.msg_control = control_un.control;
    msg.msg_controllen = sizeof(control_un.control);
#else
    msg.msg_accrights = (caddr_t)&newfd;
    msg.msg_accrightslen = sizeof(int);
#endif

    msg.msg_name = NULL;
    msg.msg_namelen = 0;

    iov[0].iov_base = ptr;
    iov[0].iov_len = nbytes;
    msg.msg_iov = iov;
    msg.msg_iovlen = 1;

    if ((n = recvmsg(fd, &msg, 0)) <= 0)
        return (n);

#ifdef HAVE_MSGHDR_MSG_CONTROL
    if ((cmptr = CMSG_FIRSTHDR(&msg)) != NULL && cmptr->cmsg_len == CMSG_LEN(sizeof(int)))
    {
        if (cmptr->cmsg_level != SOL_SOCKET)
            throw SystemResourceException("control level != SOL_SOCKET");
        if (cmptr->cmsg_type != SCM_RIGHTS)
            throw SystemResourceException("control type != SCM_RIGHTS");
        *recvfd = *((int*)CMSG_DATA(cmptr));
    }
    else
        *recvfd = -1; /* descriptor was not passed */
#else
    /* *INDENT-OFF* */
    if (msg.msg_accrightslen == sizeof(int))
        *recvfd = newfd;
    else
        *recvfd = -1; /* descriptor was not passed */
                      /* *INDENT-ON* */
#endif

    return (n);
}
// NOLINTEND(cppcoreguidelines-pro-type-member-init)
// NOLINTEND(cppcoreguidelines-avoid-c-arrays,cppcoreguidelines-pro-type-cstyle-cast)
/* end read_fd */

} // namespace TcpTransportUtil

#endif

} // namespace detail

} // namespace RobotRaconteur
