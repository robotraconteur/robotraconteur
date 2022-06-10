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

#include "RobotRaconteur/IntraTransport.h"
#include "RobotRaconteur/Service.h"

#include "IntraTransport_private.h"

#include <boost/range/adaptors.hpp>
#include <boost/range/algorithm.hpp>
#include <boost/foreach.hpp>

namespace RobotRaconteur
{

std::list<RR_WEAK_PTR<IntraTransport> > IntraTransport::peer_transports;
boost::mutex IntraTransport::peer_transports_lock;

IntraTransport::IntraTransport(const RR_SHARED_PTR<RobotRaconteurNode>& node) : Transport(node)
{

    if (!node)
        throw InvalidArgumentException("Node cannot be null");

    transportopen = false;
    this->node = node;
    is_server = false;

    closed = false;
    is_init = false;

    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, -1, "IntraTransport created");
}

void IntraTransport::Init()
{
    boost::mutex::scoped_lock lock(init_lock);
    if (is_init)
        return;
    is_init = true;

    boost::mutex::scoped_lock lock2(peer_transports_lock);
    peer_transports.push_back(shared_from_this());

    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, -1, "IntraTransport registered node");
}

IntraTransport::~IntraTransport() {}

void IntraTransport::Close()
{
    {
        boost::mutex::scoped_lock lock(closed_lock);
        if (closed)
            return;
        closed = true;
    }

    {
        boost::mutex::scoped_lock lock(peer_transports_lock);
        for (std::list<RR_WEAK_PTR<IntraTransport> >::iterator ee = peer_transports.begin();
             ee != peer_transports.end();)
        {
            if (ee->lock() != shared_from_this())
            {
                ++ee;
            }
            else
            {
                ee = peer_transports.erase(ee);
                ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, -1, "IntraTransport unregistered node");
            }
        }
    }

    std::vector<RR_SHARED_PTR<ITransportConnection> > t;

    {
        boost::mutex::scoped_lock lock(TransportConnections_lock);
        boost::copy(TransportConnections | boost::adaptors::map_values, std::back_inserter(t));
    }

    BOOST_FOREACH (RR_SHARED_PTR<ITransportConnection>& e, t)
    {
        try
        {
            e->Close();
        }
        catch (std::exception&)
        {}
    }

    close_signal();

    ROBOTRACONTEUR_LOG_INFO_COMPONENT(node, Transport, -1, "IntraTransport closed");
}

bool IntraTransport::IsServer() const { return is_server; }

bool IntraTransport::IsClient() const { return true; }

std::string IntraTransport::GetUrlSchemeString() const { return "rr+intra"; }

bool IntraTransport::CanConnectService(boost::string_ref url) { return (boost::starts_with(url, "rr+intra://")); }

void IntraTransport::AsyncCreateTransportConnection(
    boost::string_ref url, const RR_SHARED_PTR<Endpoint>& ep,
    boost::function<void(const RR_SHARED_PTR<ITransportConnection>&, const RR_SHARED_PTR<RobotRaconteurException>&)>&
        handler)
{
    ROBOTRACONTEUR_LOG_INFO_COMPONENT(node, Transport, ep->GetLocalEndpoint(),
                                      "IntraTransport begin create transport connection with URL: " << url);

    Init();

    ParseConnectionURLResult url_res = ParseConnectionURL(url);

    if (url_res.nodename.empty() && url_res.nodeid.IsAnyNode())
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, ep->GetLocalEndpoint(),
                                           "IntraTransport NodeID and/or NodeName not specified in URL: " << url);
        throw ConnectionException("NodeID and/or NodeName must be specified for IntraTransport");
    }

    std::string host = url_res.host;

    if (url_res.port != -1)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, ep->GetLocalEndpoint(),
                                           "IntraTransport must not contain port, invalid URL: " << url);
        throw ConnectionException("Invalid url for IntraTransport");
    }
    if (!url_res.path.empty() && url_res.path != "/")
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, ep->GetLocalEndpoint(),
                                           "IntraTransport must not contain a path, invalid URL: " << url);
        throw ConnectionException("Invalid url for IntraTransport");
    }
    if (!url_res.host.empty())
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, ep->GetLocalEndpoint(),
                                           "IntraTransport must not contain a hostname, invalid URL: " << url);
        throw ConnectionException("Invalid url for IntraTransport");
    }

    RR_SHARED_PTR<IntraTransport> peer_transport;

    {
        boost::mutex::scoped_lock lock(peer_transports_lock);
        for (std::list<RR_WEAK_PTR<IntraTransport> >::iterator ee = peer_transports.begin();
             ee != peer_transports.end();)
        {
            RR_SHARED_PTR<IntraTransport> peer = ee->lock();
            if (!peer)
            {
                ee = peer_transports.erase(ee);
                continue;
            }
            ++ee;

            if (!peer->IsServer())
            {
                continue;
            }

            NodeID node_id;
            std::string node_name;
            std::string service_nonce;

            if (!peer->TryGetNodeInfo(node_id, node_name, service_nonce))
            {
                continue;
            }

            if (!url_res.nodeid.IsAnyNode() && !url_res.nodename.empty())
            {
                if (url_res.nodeid == node_id && url_res.nodename == node_name)
                {
                    peer_transport = peer;
                    break;
                }
            }

            if (!url_res.nodeid.IsAnyNode())
            {
                if (url_res.nodeid == node_id)
                {
                    peer_transport = peer;
                    break;
                }
            }

            if (!url_res.nodename.empty())
            {
                if (url_res.nodename == node_name)
                {
                    peer_transport = peer;
                    break;
                }
            }
        }
    }

    if (!peer_transport)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, ep->GetLocalEndpoint(),
                                           "IntraTransport could not connect to URL: " << url);
        throw ConnectionException("Could not connect to service");
    }

    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, ep->GetLocalEndpoint(),
                                       "IntraTransport found peer transport for URL: " << url);

    std::string noden;
    if (!(url_res.nodeid.IsAnyNode() && !url_res.nodename.empty()))
    {
        noden = url_res.nodeid.ToString();
    }
    else
    {
        noden = url_res.nodename;
    }

    RR_SHARED_PTR<IntraTransportConnection> local_connection =
        RR_MAKE_SHARED<IntraTransportConnection>(shared_from_this(), false, ep->GetLocalEndpoint());
    RR_SHARED_PTR<IntraTransportConnection> peer_connection =
        RR_MAKE_SHARED<IntraTransportConnection>(peer_transport, true, 0);
    local_connection->SetPeer(peer_connection);
    peer_connection->SetPeer(local_connection);

    register_transport(local_connection);

    RR_SHARED_PTR<ITransportConnection> res = rr_cast<ITransportConnection>(local_connection);

    detail::PostHandler<RR_SHARED_PTR<ITransportConnection> >(node, handler, res);
}

RR_SHARED_PTR<ITransportConnection> IntraTransport::CreateTransportConnection(boost::string_ref url,
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

void IntraTransport::CloseTransportConnection(const RR_SHARED_PTR<Endpoint>& e)
{
    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, e->GetLocalEndpoint(),
                                       "IntraTransport request close transport connection");

    RR_SHARED_PTR<ServerEndpoint> e2 = boost::dynamic_pointer_cast<ServerEndpoint>(e);
    if (e2)
    {
        RR_SHARED_PTR<boost::asio::deadline_timer> timer(
            new boost::asio::deadline_timer(GetNode()->GetThreadPool()->get_io_context()));
        timer->expires_from_now(boost::posix_time::milliseconds(1000));
        RobotRaconteurNode::asio_async_wait(node, timer,
                                            boost::bind(&IntraTransport::CloseTransportConnection_timed,
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
    }
}

void IntraTransport::CloseTransportConnection_timed(const boost::system::error_code& err,
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

void IntraTransport::SendMessage(const RR_INTRUSIVE_PTR<Message>& m)
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

uint32_t IntraTransport::TransportCapability(boost::string_ref name)
{
    RR_UNUSED(name);
    return 0;
}

void IntraTransport::PeriodicCleanupTask()
{
    boost::mutex::scoped_lock lock(TransportConnections_lock);
    for (RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<ITransportConnection> >::iterator e = TransportConnections.begin();
         e != TransportConnections.end();)
    {
        try
        {
            RR_SHARED_PTR<IntraTransportConnection> e2 = rr_cast<IntraTransportConnection>(e->second);
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
}

void IntraTransport::AsyncSendMessage(
    const RR_INTRUSIVE_PTR<Message>& m,
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

void IntraTransport::CheckConnection(uint32_t endpoint)
{
    RR_SHARED_PTR<ITransportConnection> t;
    {
        boost::mutex::scoped_lock lock(TransportConnections_lock);
        RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<ITransportConnection> >::iterator e =
            TransportConnections.find(endpoint);
        if (e == TransportConnections.end())
        {
            ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, endpoint,
                                               "Transport connection to remote host not found");
            throw ConnectionException("Transport connection to remote host not found");
        }
        t = e->second;
    }
    t->CheckConnection(endpoint);
}

void IntraTransport::register_transport(const RR_SHARED_PTR<ITransportConnection>& connection)
{
    boost::mutex::scoped_lock lock(TransportConnections_lock);
    TransportConnections.insert(std::make_pair(connection->GetLocalEndpoint(), connection));
}

void IntraTransport::erase_transport(const RR_SHARED_PTR<ITransportConnection>& connection)
{
    try
    {
        boost::mutex::scoped_lock lock(TransportConnections_lock);
        RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<ITransportConnection> >::iterator e1 =
            TransportConnections.find(connection->GetLocalEndpoint());
        if (e1 == TransportConnections.end())
            return;
        if (e1->second == connection)
        {
            TransportConnections.erase(e1);
        }
    }
    catch (std::exception&)
    {}

    TransportConnectionClosed(connection->GetLocalEndpoint());
}

void IntraTransport::MessageReceived(const RR_INTRUSIVE_PTR<Message>& m) { GetNode()->MessageReceived(m); }

void IntraTransport::AsyncGetDetectedNodes(
    const std::vector<std::string>& schemes,
    const boost::function<void(const RR_SHARED_PTR<std::vector<NodeDiscoveryInfo> >&)>& handler, int32_t timeout)
{
    RR_UNUSED(timeout);
    if (boost::range::find(schemes, "rr+intra") == schemes.end() || schemes.empty())
    {
        RR_SHARED_PTR<std::vector<NodeDiscoveryInfo> > n = RR_MAKE_SHARED<std::vector<NodeDiscoveryInfo> >();
        detail::PostHandler(node, handler, n, true);
        return;
    }

    Init();

    RR_SHARED_PTR<std::vector<NodeDiscoveryInfo> > o = RR_MAKE_SHARED<std::vector<NodeDiscoveryInfo> >();

    {
        boost::mutex::scoped_lock lock(peer_transports_lock);
        for (std::list<RR_WEAK_PTR<IntraTransport> >::iterator ee = peer_transports.begin();
             ee != peer_transports.end();)
        {
            RR_SHARED_PTR<IntraTransport> peer = ee->lock();
            if (!peer)
            {
                ee = peer_transports.erase(ee);
                continue;
            }
            ++ee;

            if (!peer->IsServer())
            {
                continue;
            }

            NodeDiscoveryInfo n;
            if (peer->TryGetNodeInfo(n.NodeID, n.NodeName, n.ServiceStateNonce))
            {
                NodeDiscoveryInfoURL u;
                u.URL = "rr+intra:///?nodeid=" + n.NodeID.ToString("B") + "&service=RobotRaconteurServiceIndex";
                u.LastAnnounceTime = boost::posix_time::microsec_clock::universal_time();
                n.URLs.push_back(u);

                o->push_back(n);
            }
        }
    }

    detail::PostHandler(node, handler, o, true);
}

void IntraTransport::SendNodeDiscovery()
{
    if (!is_server)
        return;

    RR_SHARED_PTR<RobotRaconteurNode> node = this->node.lock();
    if (!node)
        return;

    NodeDiscoveryInfo info;
    if (!node->TryGetNodeID(info.NodeID))
        return;

    node->TryGetNodeName(info.NodeName);

    info.ServiceStateNonce = node->GetServiceStateNonce();

    NodeDiscoveryInfoURL u;
    u.URL = "rr+intra:///?nodeid=" + info.NodeID.ToString("B") + "&service=RobotRaconteurServiceIndex";
    u.LastAnnounceTime = boost::posix_time::microsec_clock::universal_time();
    info.URLs.push_back(u);

    {
        boost::mutex::scoped_lock lock(peer_transports_lock);
        for (std::list<RR_WEAK_PTR<IntraTransport> >::iterator ee = peer_transports.begin();
             ee != peer_transports.end();)
        {
            RR_SHARED_PTR<IntraTransport> peer = ee->lock();
            if (!peer)
            {
                ee = peer_transports.erase(ee);
                continue;
            }
            ++ee;

            peer->NodeDetected(info);
        }
    }

    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, -1, "Node discovery sent");
}

void IntraTransport::LocalNodeServicesChanged() { SendNodeDiscovery(); }

void IntraTransport::StartServer()
{
    GetNode()->NodeID();
    GetNode()->NodeName();
    is_server = true;
    Init();
    SendNodeDiscovery();
}

static void IntraTransport_NodeDetected1(RR_WEAK_PTR<RobotRaconteurNode> node, const NodeDiscoveryInfo& info)
{
    RR_SHARED_PTR<RobotRaconteurNode> node1 = node.lock();
    if (!node1)
        return;
    node1->NodeDetected(info);
}

void IntraTransport::NodeDetected(const NodeDiscoveryInfo& info)
{
    RobotRaconteurNode::TryPostToThreadPool(node, boost::bind(&IntraTransport_NodeDetected1, node, info), false);
}

bool IntraTransport::TryGetNodeInfo(NodeID& node_id, std::string& node_name, std::string& service_nonce)
{
    RR_SHARED_PTR<RobotRaconteurNode> node1 = node.lock();
    if (!node1)
        return false;

    if (!node1->TryGetNodeID(node_id))
        return false;

    node1->TryGetNodeName(node_name);

    service_nonce = node1->GetServiceStateNonce();

    return true;
}

IntraTransportConnection::IntraTransportConnection(const RR_SHARED_PTR<IntraTransport>& parent, bool server,
                                                   uint32_t local_endpoint)
    : connected(false)
{
    this->parent = parent;
    this->server = server;
    this->m_LocalEndpoint = local_endpoint;
    m_RemoteEndpoint = 0;
    this->recv_queue_post_requested = false;
    this->node = parent->GetNode();
}

void IntraTransportConnection::AcceptMessage(const RR_INTRUSIVE_PTR<Message>& m)
{
    boost::mutex::scoped_lock lock(recv_queue_lock);
    recv_queue.push_back(m);
    if (!recv_queue_post_requested)
    {
        RR_WEAK_PTR<IntraTransportConnection> c = shared_from_this();
        RobotRaconteurNode::TryPostToThreadPool(node,
                                                boost::bind(&IntraTransportConnection::ProcessNextRecvMessage, c));
    }
}

void IntraTransportConnection::ProcessNextRecvMessage(RR_WEAK_PTR<IntraTransportConnection> c)
{
    RR_SHARED_PTR<IntraTransportConnection> c1 = c.lock();
    if (!c1)
        return;

    RR_INTRUSIVE_PTR<Message> m;

    {
        boost::mutex::scoped_lock lock(c1->recv_queue_lock);
        if (c1->recv_queue.empty())
        {
            c1->recv_queue_post_requested = false;
            return;
        }
        m = c1->recv_queue.front();
        c1->recv_queue.pop_front();
        if (c1->recv_queue.empty())
        {
            c1->recv_queue_post_requested = false;
        }
        else
        {
            RobotRaconteurNode::TryPostToThreadPool(c1->node,
                                                    boost::bind(&IntraTransportConnection::ProcessNextRecvMessage, c));
        }
    }

    c1->MessageReceived(m);
}

void IntraTransportConnection::MessageReceived(const RR_INTRUSIVE_PTR<Message>& m)
{
    RR_SHARED_PTR<IntraTransport> p = parent.lock();
    if (!p)
        return;

    RR_INTRUSIVE_PTR<Message> ret = p->SpecialRequest(m, shared_from_this());
    if (ret != 0)
    {
        ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, m_LocalEndpoint, "Sending special request response");
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

                        p->register_transport(RR_STATIC_POINTER_CAST<IntraTransportConnection>(shared_from_this()));
                        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(
                            node, Transport, m_LocalEndpoint,
                            "IntraTransport connection assigned LocalEndpoint: " << m_LocalEndpoint);
                    }
                }
            }

            boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>&)> h = boost::bind(
                &IntraTransportConnection::SimpleAsyncEndSendMessage,
                RR_STATIC_POINTER_CAST<IntraTransportConnection>(shared_from_this()), RR_BOOST_PLACEHOLDERS(_1));
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
        std::string connecturl = "rr+intra:///";
        // NOLINTBEGIN(cppcoreguidelines-owning-memory)
        Transport::m_CurrentThreadTransportConnectionURL.reset(new std::string(connecturl));
        Transport::m_CurrentThreadTransport.reset(new RR_SHARED_PTR<ITransportConnection>(
            RR_STATIC_POINTER_CAST<IntraTransportConnection>(shared_from_this())));
        // NOLINTEND(cppcoreguidelines-owning-memory)
        p->MessageReceived(m);
    }
    catch (std::exception& exp)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, m_LocalEndpoint,
                                           "IntraTransport failed receiving message: " << exp.what());
        RobotRaconteurNode::TryHandleException(node, &exp);
        Close();
    }

    Transport::m_CurrentThreadTransportConnectionURL.reset(0);
    Transport::m_CurrentThreadTransport.reset(0);
}

void IntraTransportConnection::SendMessage(const RR_INTRUSIVE_PTR<Message>& m)
{
    ROBOTRACONTEUR_ASSERT_MULTITHREADED(node);

    RR_SHARED_PTR<detail::sync_async_handler<void> > s =
        RR_MAKE_SHARED<detail::sync_async_handler<void> >(RR_MAKE_SHARED<ConnectionException>("Send timeout"));
    boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>&)> h =
        boost::bind(&detail::sync_async_handler<void>::operator(), s, RR_BOOST_PLACEHOLDERS(_1));
    AsyncSendMessage(m, h);
    s->end_void();
}

void IntraTransportConnection::AsyncSendMessage(
    const RR_INTRUSIVE_PTR<Message>& m,
    const boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>&)>& handler)
{
    RR_SHARED_PTR<IntraTransportConnection> peer1 = peer.lock();
    if (!peer1)
    {
        ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, m_LocalEndpoint, "Connection lost");
        throw ConnectionException("Connection lost");
    }

    peer1->AcceptMessage(m);

    detail::PostHandler(node, handler);
}

void IntraTransportConnection::SimpleAsyncEndSendMessage(const RR_SHARED_PTR<RobotRaconteurException>& err)
{
    if (err)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Transport, GetLocalEndpoint(),
                                           "Failed sending internal message: " << err->what());
        Close();
    }
}

void IntraTransportConnection::Close()
{
    RR_SHARED_PTR<IntraTransportConnection> peer1 = peer.lock();
    peer.reset();
    peer_storage.reset();

    bool connected1 = connected.exchange(false);

    if (!connected1)
        return;

    ROBOTRACONTEUR_LOG_INFO_COMPONENT(node, Transport, m_LocalEndpoint, "IntraTransport closing connection");

    try
    {
        RR_SHARED_PTR<IntraTransport> p = parent.lock();
        if (p)
            p->erase_transport(shared_from_this());
    }
    catch (std::exception&)
    {}

    if (peer1)
    {
        peer1->RemoteClose();
    }
}

void IntraTransportConnection::RemoteClose()
{
    RR_WEAK_PTR<IntraTransportConnection> c = shared_from_this();
    RobotRaconteurNode::TryPostToThreadPool(node, boost::bind(&IntraTransportConnection::RemoteClose1, c));
}

void IntraTransportConnection::RemoteClose1(RR_WEAK_PTR<IntraTransportConnection> c)
{
    RR_SHARED_PTR<IntraTransportConnection> c1 = c.lock();
    if (!c1)
        return;
    c1->Close();
}

uint32_t IntraTransportConnection::GetLocalEndpoint() { return m_LocalEndpoint; }

uint32_t IntraTransportConnection::GetRemoteEndpoint() { return m_RemoteEndpoint; }

NodeID IntraTransportConnection::GetRemoteNodeID()
{
    boost::shared_lock<boost::shared_mutex> lock(RemoteNodeID_lock);
    return RemoteNodeID;
}

RR_SHARED_PTR<RobotRaconteurNode> IntraTransportConnection::GetNode()
{
    RR_SHARED_PTR<RobotRaconteurNode> n = node.lock();
    if (!n)
        throw InvalidOperationException("Node has been released");
    return n;
}

void IntraTransportConnection::CheckConnection(uint32_t endpoint)
{
    RR_SHARED_PTR<IntraTransportConnection> peer1 = peer.lock();
    if (endpoint != m_LocalEndpoint || !connected.load() || !peer1)
    {
        ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, m_LocalEndpoint, "Connection lost");
        throw ConnectionException("Connection lost");
    }
}

bool IntraTransportConnection::CheckCapabilityActive(uint32_t flag)
{
    RR_UNUSED(flag);
    return 0;
}

void IntraTransportConnection::SetPeer(const RR_SHARED_PTR<IntraTransportConnection>& peer)
{
    boost::unique_lock<boost::shared_mutex> lock(RemoteNodeID_lock);
    this->peer = peer;
    if (!server)
    {
        this->peer_storage = peer;
    }
    this->RemoteNodeID = peer->GetNode()->NodeID();
    this->m_RemoteEndpoint = peer->GetLocalEndpoint();
    connected.exchange(true);

    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, m_LocalEndpoint, "Peer set for IntraTransport connection");
}

bool IntraTransportConnection::IsConnected()
{
    RR_SHARED_PTR<IntraTransportConnection> peer1 = peer.lock();
    if (!peer1)
        return false;
    return connected.load();
}

RR_SHARED_PTR<Transport> IntraTransportConnection::GetTransport()
{
    RR_SHARED_PTR<Transport> p = parent.lock();
    if (!p)
        throw InvalidOperationException("Transport has been released");
    return p;
}

} // namespace RobotRaconteur
