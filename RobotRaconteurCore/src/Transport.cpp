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

#include "RobotRaconteur/Transport.h"
#include "RobotRaconteur/RobotRaconteurNode.h"
#include "RobotRaconteur/DataTypes.h"
#include <boost/algorithm/string.hpp>
#include "RobotRaconteur/Client.h"
#include "RobotRaconteur/Service.h"

#include <boost/foreach.hpp>

namespace RobotRaconteur
{

Transport::Transport(const RR_SHARED_PTR<RobotRaconteurNode>& node)
{
    this->node = node;
    TransportID = 0;
}

RR_SHARED_PTR<RobotRaconteurNode> Transport::GetNode() const
{
    RR_SHARED_PTR<RobotRaconteurNode> n = node.lock();
    if (!n)
        throw InvalidOperationException("Node has been released");
    return n;
}

// template <class T>
// void thread_null_deleter(T* a) {}
boost::thread_specific_ptr<std::string> Transport::m_CurrentThreadTransportConnectionURL;

std::string Transport::GetCurrentTransportConnectionURL()
{
    if (!m_CurrentThreadTransportConnectionURL.get())
        throw InvalidOperationException("Not set");
    return std::string(*m_CurrentThreadTransportConnectionURL);
}

boost::thread_specific_ptr<RR_SHARED_PTR<ITransportConnection> > Transport::m_CurrentThreadTransport;

RR_SHARED_PTR<ITransportConnection> Transport::GetCurrentThreadTransport()
{

    if (!m_CurrentThreadTransport.get())
        throw InvalidOperationException("Not set");
    return *m_CurrentThreadTransport;
}

void Transport::PeriodicCleanupTask() {}

uint32_t Transport::TransportCapability(boost::string_ref name)
{
    RR_UNUSED(name);
    return 0;
}

void Transport::FireTransportEventListener(const RR_SHARED_PTR<Transport>& shared_this, TransportListenerEventType ev,
                                           const RR_SHARED_PTR<void>& parameter)
{
    TransportListeners(shared_this, ev, parameter);
}

RR_INTRUSIVE_PTR<Message> Transport::SpecialRequest(const RR_INTRUSIVE_PTR<Message>& m,
                                                    const RR_SHARED_PTR<ITransportConnection>& tc)
{
    if (!m->entries.empty())
    {
        uint32_t type = (static_cast<uint32_t>(m->entries[0]->EntryType));
        if (type < 500 && (type % 2 == 1))
        {
            RR_INTRUSIVE_PTR<Message> r = GetNode()->SpecialRequest(m, TransportID, tc);
            return r;
        }
    }

    return RR_INTRUSIVE_PTR<Message>();
}

void Transport::Close() {}

std::vector<NodeDiscoveryInfo> Transport::GetDetectedNodes(const std::vector<std::string>& schemes)
{
    ROBOTRACONTEUR_ASSERT_MULTITHREADED(node);

    RR_SHARED_PTR<detail::sync_async_handler<std::vector<NodeDiscoveryInfo> > > t =
        RR_MAKE_SHARED<detail::sync_async_handler<std::vector<NodeDiscoveryInfo> > >();
    boost::function<void(const RR_SHARED_PTR<std::vector<NodeDiscoveryInfo> >&)> h =
        boost::bind(&detail::sync_async_handler<std::vector<NodeDiscoveryInfo> >::operator(), t,
                    RR_BOOST_PLACEHOLDERS(_1), RR_SHARED_PTR<RobotRaconteurException>());
    AsyncGetDetectedNodes(schemes, h);
    return *t->end();
}

void Transport::AsyncGetDetectedNodes(
    const std::vector<std::string>& schemes,
    const boost::function<void(const RR_SHARED_PTR<std::vector<NodeDiscoveryInfo> >&)>& handler, int32_t timeout)
{
    RR_UNUSED(schemes);
    RR_UNUSED(timeout);
    RR_SHARED_PTR<std::vector<NodeDiscoveryInfo> > n = RR_MAKE_SHARED<std::vector<NodeDiscoveryInfo> >();
    RobotRaconteurNode::TryPostToThreadPool(node, boost::bind(handler, n), true);
}

void Transport::LocalNodeServicesChanged() {}

ROBOTRACONTEUR_CORE_API ParseConnectionURLResult ParseConnectionURL(boost::string_ref url)
{
    ParseConnectionURLResult o;

    boost::match_results<boost::string_ref::const_iterator> url_result;
    boost::regex_search(url.begin(), url.end(), url_result,
                        boost::regex("^([^:\\s]+)://(?:((?:\\[[A-Fa-f0-9\\:]+(?:\\%\\w*)?\\])|(?:[^\\[\\]\\:/"
                                     "\\?\\s]+))(?::([^:/\\?\\s]+))?|/)(?:/([^\\?\\s]*))?\\?\?([^\\s]*)$"));

    if (url_result.size() < 4)
        throw ConnectionException("Invalid Connection URL");

    o.scheme = url_result[1];
    if (o.scheme.empty())
        throw ConnectionException("Invalid Connection URL");

    if (o.scheme == "tcp")
    {
        if (url_result.size() < 5)
            throw ConnectionException("Invalid Connection URL");

        o.host = url_result[2];
        if (o.host.empty())
            throw ConnectionException("Invalid Connection URL");

        std::string ap = url_result[4];
        if (ap.at(0) == '/')
            ap = ap.erase(0, 1);

        std::vector<std::string> s;
        boost::split(s, ap, boost::is_from_range('/', '/'));
        // s = ap.Split(std::vector<int8_t>(tempVector4, tempVector4 + sizeof(tempVector4) / sizeof(tempVector4[0])),3);
        if (s.size() < 2)
            throw ConnectionException("Invalid Connection URL");
        std::string noden = s.at(0);
        if (noden.find('{') != std::string::npos || noden.find('[') != std::string::npos)
        {
            o.nodeid = NodeID(noden);
        }
        else
        {
            o.nodename = noden;
        }
        o.path = "/";
        try
        {
            o.port = boost::lexical_cast<int32_t>(std::string(url_result[3]));
        }
        catch (std::exception&)
        {
            throw ConnectionException("Invalid Connection URL");
        }
        o.service = s.at(1);

        return o;
    }

    if (url_result.size() < 6)
        throw ConnectionException("Invalid Connection URL");

    if (boost::starts_with(url, o.scheme + ":////"))
        throw ConnectionException("Invalid Connection URL");

    o.host = url_result[2];
    std::string port_str = url_result[3];
    if (port_str.empty())
    {
        o.port = -1;

        if (o.scheme == "rr+tcp" || o.scheme == "rrs+tcp")
        {
            o.port = 48653;
        }

        if (o.scheme == "rr+ws" || o.scheme == "rrs+ws")
        {
            o.port = 80;
        }

        if (o.scheme == "rr+wss" || o.scheme == "rrs+wss")
        {
            o.port = 443;
        }
    }
    else
    {
        try
        {
            o.port = boost::lexical_cast<int32_t>(std::string(url_result[3]));
        }
        catch (std::exception&)
        {
            throw ConnectionException("Invalid Connection URL");
        }
    }

    o.path = url_result[4];

    std::string query = url_result[5];

    std::map<std::string, std::string> query_params;

    std::vector<std::string> q2;
    boost::split(q2, query, boost::is_from_range('&', '&'));
    BOOST_FOREACH (std::string& e, q2)
    {
        std::vector<std::string> q3;
        boost::split(q3, e, boost::is_from_range('=', '='));
        if (q3.size() != 2)
            throw ConnectionException("Invalid Connection URL");

        std::string param_name = q3.at(0);
        std::string param_value = detail::decode_index(q3.at(1));
        query_params.insert(std::make_pair(param_name, param_value));
    }

    if (query_params.count("service") == 0)
    {
        throw ConnectionException("Invalid Connection URL: Target service not specified");
    }

    o.service = query_params.at("service");

    if (query_params.count("nodeid") != 0)
    {
        o.nodeid = NodeID(query_params.at("nodeid"));
    }

    if (query_params.count("nodename") != 0)
    {
        o.nodename = query_params.at("nodename");
        if (!boost::regex_match(o.nodename, boost::regex("^[a-zA-Z][a-zA-Z0-9_\\.\\-]*$")))
        {
            throw InvalidArgumentException("\"" + o.nodename + "\" is an invalid NodeName");
        }
    }

    return o;
}

void Transport::TransportConnectionClosed(uint32_t endpoint)
{
    RR_SHARED_PTR<RobotRaconteurNode> n = node.lock();
    if (n)
    {
        n->TransportConnectionClosed(endpoint);
    }
}

} // namespace RobotRaconteur
