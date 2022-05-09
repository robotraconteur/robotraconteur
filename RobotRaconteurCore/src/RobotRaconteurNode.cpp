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

#include "RobotRaconteur/RobotRaconteurNode.h"
#include <boost/algorithm/string.hpp>
#include "RobotRaconteur/Client.h"
#include "RobotRaconteur/Service.h"
#include "RobotRaconteur/RobotRaconteurServiceIndex.h"
#include "RobotRaconteur/ServiceIndexer.h"
#include "RobotRaconteur/DataTypes.h"
#include <boost/regex.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/foreach.hpp>
#include <boost/range/adaptors.hpp>
#include <boost/range/algorithm.hpp>
#include <boost/filesystem.hpp>

#include "Discovery_private.h"
#include "RobotRaconteurNode_connector_private.h"

namespace RobotRaconteur
{
static void RobotRaconteurNode_empty_handler() {}

static void RobotRaconteurNode_empty_handler(const RR_SHARED_PTR<RobotRaconteurException>&) {}

RobotRaconteurNode RobotRaconteurNode::m_s;
RR_SHARED_PTR<RobotRaconteurNode> RobotRaconteurNode::m_sp;
RR_WEAK_PTR<RobotRaconteurNode> RobotRaconteurNode::m_weak_sp;

bool RobotRaconteurNode::is_init = false;
boost::mutex RobotRaconteurNode::init_lock;

RobotRaconteurNode::RobotRaconteurNode()
{
    is_shutdown = false;
    NodeID_set = false;
    NodeName_set = false;
    PeriodicCleanupTask_timerstarted = false;

    transport_count = 0;

    EndpointInactivityTimeout = 600000;
    TransportInactivityTimeout = 600000;
    RequestTimeout = 15000;
    MemoryMaxTransferSize = 102400;
    instance_is_init = false;

    log_level = RobotRaconteur_LogLevel_Warning;

    // ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Node, -1, "RobotRaconteurNode created");
}

void RobotRaconteurNode::Init()
{

    boost::mutex::scoped_lock lock(init_lock);
    if (instance_is_init)
        return;

    weak_this = shared_from_this();

    {
        boost::mutex::scoped_lock lock(random_generator_lock);
        random_generator = RR_MAKE_SHARED<boost::random::random_device>();
    }

    node_sync_time = boost::posix_time::microsec_clock::universal_time();
    node_internal_start_time = boost::chrono::steady_clock::now();
    node_sync_timespec = ptimeToTimeSpec(node_sync_time);

    RegisterServiceType(RR_MAKE_SHARED<RobotRaconteurServiceIndex::RobotRaconteurServiceIndexFactory>());
    RegisterService("RobotRaconteurServiceIndex", "RobotRaconteurServiceIndex",
                    RR_MAKE_SHARED<ServiceIndexer>(shared_from_this()));

    service_state_nonce = GetRandomString(16);

    instance_is_init = true;

    // Deal with possible race in boost::filesystem::path
    boost::filesystem::path::codecvt();

    m_Discovery = RR_MAKE_SHARED<detail::Discovery>(shared_from_this());

    ROBOTRACONTEUR_LOG_INFO_COMPONENT(weak_this, Node, -1,
                                      "RobotRaconteurNode version " << ROBOTRACONTEUR_VERSION_TEXT << " initialized");
}

static void RobotRaconteurNode_emptydeleter(RobotRaconteurNode* n) {}

RobotRaconteurNode* RobotRaconteurNode::s()
{
    if (!is_init)
    {
        is_init = true;

        m_sp.reset(&m_s, &RobotRaconteurNode_emptydeleter);
        m_s._internal_accept_owner(&m_sp, &m_s);
        m_weak_sp = m_sp;
        m_s.Init();
    }
    return &m_s;
}

RR_SHARED_PTR<RobotRaconteurNode> RobotRaconteurNode::sp()
{
    RobotRaconteurNode::s();
    return m_sp;
}

RR_WEAK_PTR<RobotRaconteurNode> RobotRaconteurNode::weak_sp() { return m_weak_sp; }

NodeID RobotRaconteurNode::NodeID()
{
    boost::mutex::scoped_lock lock(id_lock);
    if (!NodeID_set)
    {
        m_NodeID = RobotRaconteur::NodeID::NewUniqueID();
        NodeID_set = true;
        ::RobotRaconteur::NodeID n = m_NodeID;
        lock.unlock();
        ROBOTRACONTEUR_LOG_INFO_COMPONENT(weak_this, Node, -1,
                                          "RobotRaconteurNode NodeID configured with random UUID " << n.ToString());
        return n;
    }

    return m_NodeID;
}

std::string RobotRaconteurNode::NodeName()
{
    boost::mutex::scoped_lock lock(id_lock);
    if (!NodeName_set)
    {
        m_NodeName = "";
        NodeName_set = true;
    }
    return m_NodeName;
}

bool RobotRaconteurNode::TryGetNodeID(RobotRaconteur::NodeID& id)
{
    boost::mutex::scoped_lock lock(id_lock);
    if (!NodeID_set)
    {
        return false;
    }

    id = m_NodeID;
    return true;
}

bool RobotRaconteurNode::TryGetNodeName(std::string& node_name)
{
    boost::mutex::scoped_lock lock(id_lock);
    if (!NodeName_set)
    {
        return false;
    }

    node_name = m_NodeName;
    return true;
}

void RobotRaconteurNode::SetNodeID(const RobotRaconteur::NodeID& id)
{
    boost::mutex::scoped_lock lock(id_lock);
    if (NodeID_set)
    {
        lock.unlock();
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(weak_this, Node, -1,
                                           "RobotRaconteurNode attempt to set NodeID when already set");
        throw InvalidOperationException("NodeID already set");
    }
    m_NodeID = id;
    NodeID_set = true;
    lock.unlock();
    ROBOTRACONTEUR_LOG_INFO_COMPONENT(weak_this, Node, -1,
                                      "RobotRaconteurNode NodeID set to UUID " << m_NodeID.ToString());
}

void RobotRaconteurNode::SetNodeName(boost::string_ref name)
{
    if (name.size() > 1024)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(weak_this, Node, -1, "NodeName too long");
        throw InvalidArgumentException("NodeName too long");
    }
    if (!boost::regex_match(name.begin(), name.end(), boost::regex("^[a-zA-Z][a-zA-Z0-9_\\.\\-]*$")))
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(weak_this, Node, -1, "Invalid NodeName \"" << name << "\"");
        throw InvalidArgumentException("\"" + name + "\" is an invalid NodeName");
    }

    boost::mutex::scoped_lock lock(id_lock);
    if (NodeName_set)
    {
        lock.unlock();
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(weak_this, Node, -1,
                                           "RobotRaconteurNode attempt to set NodeName when already set");
        throw InvalidOperationException("NodeName already set");
    }
    m_NodeName = RR_MOVE(name.to_string());
    NodeName_set = true;

    lock.unlock();
    ROBOTRACONTEUR_LOG_INFO_COMPONENT(weak_this, Node, -1,
                                      "RobotRaconteurNode NodeName set to \"" << m_NodeName << "\"");
}

RR_SHARED_PTR<ServiceFactory> RobotRaconteurNode::GetServiceType(boost::string_ref servicename)
{

    boost::shared_lock<boost::shared_mutex> lock(service_factories_lock);
    RR_UNORDERED_MAP<std::string, RR_SHARED_PTR<ServiceFactory> >::iterator e1 =
        service_factories.find(servicename.to_string());
    if (e1 == service_factories.end())
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(weak_this, Node, -1, "Unknown service type \"" << servicename << "\"");
        throw ServiceException("Unknown service type");
    }
    return e1->second;
}

bool RobotRaconteurNode::IsServiceTypeRegistered(boost::string_ref servicetype)
{
    boost::shared_lock<boost::shared_mutex> lock(service_factories_lock);
    RR_UNORDERED_MAP<std::string, RR_SHARED_PTR<ServiceFactory> >::iterator e1 =
        service_factories.find(servicetype.to_string());
    return e1 != service_factories.end();
}

void RobotRaconteurNode::RegisterServiceType(const RR_SHARED_PTR<ServiceFactory>& factory)
{
    boost::unique_lock<boost::shared_mutex> lock(service_factories_lock);

    if (boost::ends_with(factory->GetServiceName(), "_signed"))
        throw ServiceException("Could not verify signed service definition");

    if (service_factories.count(factory->GetServiceName()) != 0)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(weak_this, Node, -1,
                                           "Service type already registered \"" << factory->GetServiceName() << "\"");
        throw ServiceException("Service type already registered");
    }

    factory->ServiceDef()->CheckVersion();

    factory->SetNode(shared_from_this());

    service_factories.insert(std::make_pair(factory->GetServiceName(), factory));

    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(weak_this, Node, -1,
                                       "Service type registered \"" << factory->GetServiceName() << "\"");
}

void RobotRaconteurNode::UnregisterServiceType(boost::string_ref type)
{
    boost::unique_lock<boost::shared_mutex> lock(service_factories_lock);
    RR_UNORDERED_MAP<std::string, RR_SHARED_PTR<ServiceFactory> >::iterator e1 =
        service_factories.find(type.to_string());
    if (e1 == service_factories.end())
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(weak_this, Node, -1,
                                           "Cannot unregister nonexistant service type \"" << type << "\"");
        throw InvalidArgumentException("Service type not registered");
    }
    service_factories.erase(e1);
    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(weak_this, Node, -1, "Service type unregistered \"" << type << "\"");
}

std::vector<std::string> RobotRaconteurNode::GetRegisteredServiceTypes()
{
    boost::shared_lock<boost::shared_mutex> lock(service_factories_lock);
    std::vector<std::string> o;
    boost::copy(service_factories | boost::adaptors::map_keys, std::back_inserter(o));
    return o;
}

uint32_t RobotRaconteurNode::RegisterTransport(const RR_SHARED_PTR<Transport>& transport)
{
    {
        boost::mutex::scoped_lock lock(transports_lock);
        if (transport_count >= std::numeric_limits<uint32_t>::max())
            transport_count = 0;
        else
            transport_count++;
        transport->TransportID = transport_count;
        transports.insert(std::make_pair(transport_count, transport));
    }

    RR_SHARED_PTR<ITransportTimeProvider> t = RR_DYNAMIC_POINTER_CAST<ITransportTimeProvider>(transport);
    if (t)
    {
        boost::unique_lock<boost::shared_mutex> lock(time_provider_lock);
        RR_SHARED_PTR<ITransportTimeProvider> t2 = time_provider.lock();
        if (!t2)
        {
            time_provider = t;
        }
    }

    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(weak_this, Node, -1,
                                       "Node " << transport->GetUrlSchemeString() << " registered");

    return transport->TransportID;
}

RR_INTRUSIVE_PTR<MessageElementNestedElementList> RobotRaconteurNode::PackStructure(
    const RR_INTRUSIVE_PTR<RRStructure>& structure)
{
    return detail::packing::PackStructure(structure, this);
}

RR_INTRUSIVE_PTR<RRStructure> RobotRaconteurNode::UnpackStructure(
    const RR_INTRUSIVE_PTR<MessageElementNestedElementList>& structure)
{
    return detail::packing::UnpackStructure(structure, this);
}

RR_INTRUSIVE_PTR<MessageElementNestedElementList> RobotRaconteurNode::PackPodArray(
    const RR_INTRUSIVE_PTR<RRPodBaseArray>& a)
{
    return detail::packing::PackPodArray(a, this);
}

RR_INTRUSIVE_PTR<RRPodBaseArray> RobotRaconteurNode::UnpackPodArray(
    const RR_INTRUSIVE_PTR<MessageElementNestedElementList>& a)
{
    return detail::packing::UnpackPodArray(a, this);
}

RR_INTRUSIVE_PTR<MessageElementNestedElementList> RobotRaconteurNode::PackPodMultiDimArray(
    const RR_INTRUSIVE_PTR<RRPodBaseMultiDimArray>& a)
{
    return detail::packing::PackPodMultiDimArray(a, this);
}

RR_INTRUSIVE_PTR<RRPodBaseMultiDimArray> RobotRaconteurNode::UnpackPodMultiDimArray(
    const RR_INTRUSIVE_PTR<MessageElementNestedElementList>& a)
{
    return detail::packing::UnpackPodMultiDimArray(a, this);
}

RR_INTRUSIVE_PTR<MessageElementNestedElementList> RobotRaconteurNode::PackNamedArray(
    const RR_INTRUSIVE_PTR<RRNamedBaseArray>& a)
{
    return detail::packing::PackNamedArray(a, this);
}

RR_INTRUSIVE_PTR<RRNamedBaseArray> RobotRaconteurNode::UnpackNamedArray(
    const RR_INTRUSIVE_PTR<MessageElementNestedElementList>& a)
{
    return detail::packing::UnpackNamedArray(a, this);
}

RR_INTRUSIVE_PTR<MessageElementNestedElementList> RobotRaconteurNode::PackNamedMultiDimArray(
    const RR_INTRUSIVE_PTR<RRNamedBaseMultiDimArray>& a)
{
    return detail::packing::PackNamedMultiDimArray(a, this);
}

RR_INTRUSIVE_PTR<RRNamedBaseMultiDimArray> RobotRaconteurNode::UnpackNamedMultiDimArray(
    const RR_INTRUSIVE_PTR<MessageElementNestedElementList>& a)
{
    return detail::packing::UnpackNamedMultiDimArray(a, this);
}

RR_INTRUSIVE_PTR<MessageElementData> RobotRaconteurNode::PackVarType(const RR_INTRUSIVE_PTR<RRValue>& vardata)
{
    return detail::packing::PackVarType(vardata, this);
}

RR_INTRUSIVE_PTR<RRValue> RobotRaconteurNode::UnpackVarType(const RR_INTRUSIVE_PTR<MessageElement>& mvardata1)
{
    return detail::packing::UnpackVarType(mvardata1, this);
}

void RobotRaconteurNode::Shutdown()
{
    ROBOTRACONTEUR_ASSERT_MULTITHREADED(shared_from_this());

    ROBOTRACONTEUR_LOG_INFO_COMPONENT(weak_this, Node, -1, "RobotRaconteurNode begin shutdown");

    {
        if (!instance_is_init)
            return;
        boost::mutex::scoped_lock lock2(shutdown_lock);
        if (is_shutdown)
            return;
        {
            boost::unique_lock<boost::shared_mutex> lock(thread_pool_lock);
            is_shutdown = true;
        }

        {
            boost::unique_lock<boost::shared_mutex> lock(services_lock);

            BOOST_FOREACH (RR_SHARED_PTR<ServerContext>& e, services | boost::adaptors::map_values)
            {
                try
                {
                    e->Close();
                }
                catch (std::exception&)
                {}
            }

            services.clear();
        }

        std::vector<RR_SHARED_PTR<Endpoint> > endpointsv;
        {
            boost::mutex::scoped_lock lock(endpoint_lock);
            boost::copy(endpoints | boost::adaptors::map_values, std::back_inserter(endpointsv));
        }

        BOOST_FOREACH (RR_SHARED_PTR<Endpoint>& e, endpointsv)
        {
            try
            {
                RR_SHARED_PTR<ClientContext> e1 = RR_DYNAMIC_POINTER_CAST<ClientContext>(e);
                if (e1)
                    e1->AsyncClose(boost::bind(&RobotRaconteurNode_empty_handler));
            }
            catch (std::exception&)
            {}
        }

        {
            boost::mutex::scoped_lock lock(endpoint_lock);
            endpoints.clear();
        }

        // std::cout << "start transport close" << std::endl;

        {
            if (m_Discovery)
            {
                m_Discovery->Shutdown();
            }
        }

        {
            boost::mutex::scoped_lock lock(transports_lock);
            BOOST_FOREACH (RR_SHARED_PTR<Transport>& e, transports | boost::adaptors::map_values)
            {
                try
                {
                    e->Close();
                }
                catch (std::exception&)
                {}
            }

            transports.clear();
        }

        {
            boost::mutex::scoped_lock lock(cleanupobjs_lock);

            cleanupobjs.clear();
        }
    }

    shutdown_listeners();

    {
        boost::unique_lock<boost::shared_mutex> lock(PeriodicCleanupTask_timer_lock);
        if (this->PeriodicCleanupTask_timer)
        {
            try
            {
                this->PeriodicCleanupTask_timer->Stop();
            }
            catch (std::exception&)
            {}
            this->PeriodicCleanupTask_timer->Clear();
            this->PeriodicCleanupTask_timer.reset();
        }
    }

    {
        RR_SHARED_PTR<ThreadPool> thread_pool1;

        {
            boost::unique_lock<boost::shared_mutex> lock(thread_pool_lock);
            thread_pool1 = thread_pool;
        }
        if (thread_pool1)
        {
            thread_pool1->Shutdown();
        }
    }

    {
        boost::mutex::scoped_lock lock(exception_handler_lock);
        exception_handler.clear();
    }

    discovery_updated_listeners.disconnect_all_slots();
    discovery_lost_listeners.disconnect_all_slots();

    {
        boost::unique_lock<boost::shared_mutex> lock(tap_lock);
        if (tap)
        {
            tap->Close();
            tap.reset();
        }
    }

    ROBOTRACONTEUR_LOG_INFO_COMPONENT(weak_this, Node, -1, "RobotRaconteurNode shutdown complete");
}

void RobotRaconteurNode::ReleaseThreadPool()
{
    boost::mutex::scoped_lock lock2(shutdown_lock);

    if (!is_shutdown)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(weak_this, Node, -1, "Node must be shut down to release thread pool");
        throw InvalidOperationException("Node must be shut down to release thread pool");
    }

    {
        boost::unique_lock<boost::shared_mutex> lock(thread_pool_lock);
        thread_pool.reset();
    }
    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(weak_this, Node, -1, "Thread pool released");
}

RobotRaconteurNode::~RobotRaconteurNode()
{
    // Shutdown();
}

static std::string RobotRaconteurNode_log_msg_servicepath(const RR_INTRUSIVE_PTR<Message>& m)
{
    if (m->entries.empty())
    {
        return "";
    }
    return m->entries[0]->ServicePath.str().to_string();
}

static std::string RobotRaconteurNode_log_msg_member(const RR_INTRUSIVE_PTR<Message>& m)
{
    if (m->entries.empty())
    {
        return "";
    }
    return m->entries[0]->MemberName.str().to_string();
}

static uint16_t RobotRaconteurNode_log_msg_entrytype(const RR_INTRUSIVE_PTR<Message>& m)
{
    if (m->entries.empty())
    {
        return 0;
    }
    return (uint16_t)m->entries[0]->EntryType;
}

static uint16_t RobotRaconteurNode_log_msg_error(const RR_INTRUSIVE_PTR<Message>& m)
{
    if (m->entries.empty())
    {
        return 0;
    }
    return (uint16_t)m->entries[0]->Error;
}

// NOLINTBEGIN(bugprone-macro-parentheses)
#define ROBOTRACONTEUR_LOG_MESSAGE(log_cmd, node, source, msg_txt, m)                                                  \
    log_cmd(node, source, e->GetLocalEndpoint(), RobotRaconteurNode_log_msg_servicepath(m),                            \
            RobotRaconteurNode_log_msg_member(m),                                                                      \
            msg_txt << " from " << m->header->SenderNodeID.ToString() << " ep " << m->header->SenderEndpoint << " to " \
                    << m->header->ReceiverNodeID.ToString() << " ep " << m->header->ReceiverEndpoint << " EntryType "  \
                    << RobotRaconteurNode_log_msg_entrytype(m) << " Error " << RobotRaconteurNode_log_msg_error(m))
// NOLINTEND(bugprone-macro-parentheses)

void RobotRaconteurNode::SendMessage(const RR_INTRUSIVE_PTR<Message>& m)
{

    ROBOTRACONTEUR_ASSERT_MULTITHREADED(shared_from_this());

    if (m->header->SenderNodeID != NodeID())
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(weak_this, Node, -1, "Attempt to send message with invalid SenderNodeID");
        throw ConnectionException("Could not route message");
    }

    {
        boost::shared_lock<boost::shared_mutex> lock3(tap_lock);
        if (tap)
        {
            tap->RecordMessage(m);
        }
    }

    RR_SHARED_PTR<Endpoint> e;
    {
        boost::mutex::scoped_lock lock(endpoint_lock);
        RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<Endpoint> >::iterator e1 = endpoints.find(m->header->SenderEndpoint);
        if (e1 == endpoints.end())
        {
            if (is_shutdown)
            {
                ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(weak_this, Node, -1, "Attempt to send message after node shutdown");
                throw InvalidEndpointException("Attempt to send message after node shutdown");
            }

            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(
                weak_this, Node, -1, "Attempt to send message using invalid endpoint " << m->header->SenderEndpoint);
            throw InvalidEndpointException("Could not find endpoint");
        }
        e = e1->second;
    }

    RR_SHARED_PTR<Transport> c;
    {
        boost::shared_lock<boost::shared_mutex> lock(transport_lock);
        RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<Transport> >::iterator e1 = transports.find(e->GetTransport());
        if (e1 == transports.end())
        {
            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(weak_this, Transport, e->GetLocalEndpoint(),
                                               "Could not find transport to send message from endpoint "
                                                   << e->GetLocalEndpoint());
            throw ConnectionException("Could not find transport");
        }
        c = e1->second;
    }

    c->SendMessage(m);

    ROBOTRACONTEUR_LOG_MESSAGE(ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH, weak_this, Node, "Sending message", m)
}

void RobotRaconteurNode::AsyncSendMessage(
    const RR_INTRUSIVE_PTR<Message>& m,
    const boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>&)>& handler)
{
    if (m->header->SenderNodeID != NodeID())
    {

        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(weak_this, Node, -1, "Attempt to send message with invalid SenderNodeID");
        throw ConnectionException("Could not route message");
    }

    {
        boost::shared_lock<boost::shared_mutex> lock3(tap_lock);
        if (tap)
        {
            tap->RecordMessage(m);
        }
    }

    RR_SHARED_PTR<Endpoint> e;
    {
        boost::mutex::scoped_lock lock(endpoint_lock);
        RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<Endpoint> >::iterator e1 = endpoints.find(m->header->SenderEndpoint);
        if (e1 == endpoints.end())
        {
            if (is_shutdown)
            {
                ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(weak_this, Node, -1, "Attempt to send message after node shutdown");
                throw InvalidOperationException("Attempt to send message after node shutdown");
            }

            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(
                weak_this, Node, -1, "Attempt to send message using invalid endpoint " << m->header->SenderEndpoint);
            throw InvalidEndpointException("Could not find endpoint");
        }
        e = e1->second;
    }

    RR_SHARED_PTR<Transport> c;
    {
        boost::shared_lock<boost::shared_mutex> lock(transport_lock);
        RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<Transport> >::iterator e1 = transports.find(e->GetTransport());
        if (e1 == transports.end())
        {
            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(weak_this, Transport, e->GetLocalEndpoint(),
                                               "Could not find transport to send message from endpoint "
                                                   << e->GetLocalEndpoint());
            throw ConnectionException("Could not find transport");
        }
        c = e1->second;
    }

    c->AsyncSendMessage(m, handler);

    ROBOTRACONTEUR_LOG_MESSAGE(ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH, weak_this, Node, "Sending message", m)
}

void RobotRaconteurNode::MessageReceived(const RR_INTRUSIVE_PTR<Message>& m)
{
    {
        boost::shared_lock<boost::shared_mutex> lock3(tap_lock);
        if (tap)
        {
            tap->RecordMessage(m);
        }
    }

    try
    {
        if (m->header->ReceiverNodeID != NodeID())
        {
            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(
                weak_this, Node, -1,
                "Received message with invalid ReceiverNodeID: " << m->header->ReceiverNodeID.ToString());
            RR_INTRUSIVE_PTR<Message> eret = GenerateErrorReturnMessage(
                m, MessageErrorType_NodeNotFound, "RobotRaconteur.NodeNotFound", "Could not find route to remote node");
            if (!eret->entries.empty())
                SendMessage(eret);
        }

        else
        {

            RR_SHARED_PTR<Endpoint> e;

            {
                boost::mutex::scoped_lock lock(endpoint_lock);
                RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<Endpoint> >::iterator e1 =
                    endpoints.find(m->header->ReceiverEndpoint);
                if (e1 != endpoints.end())
                {
                    e = e1->second;
                }
            }

            if (e)
            {
                ROBOTRACONTEUR_LOG_MESSAGE(ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH, weak_this, Node, "Received message",
                                           m)
                e->MessageReceived(m);
            }
            else
            {
                ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(
                    weak_this, Node, -1,
                    "Received message with invalid ReceiverEndpoint: " << m->header->ReceiverEndpoint);
                RR_INTRUSIVE_PTR<Message> eret =
                    GenerateErrorReturnMessage(m, MessageErrorType_InvalidEndpoint, "RobotRaconteur.InvalidEndpoint",
                                               "Invalid destination endpoint");
                if (!eret->entries.empty())
                    SendMessage(eret);
            }
        }
    }
    catch (std::exception& e)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(weak_this, Node, -1, "Exception receiving message: " << e.what());
        HandleException(&e);
    }
}

void RobotRaconteurNode::TransportConnectionClosed(uint32_t endpoint)
{
    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(weak_this, Node, endpoint, "Node notified that transport connection was closed");

    RR_SHARED_PTR<Endpoint> e;
    {
        boost::mutex::scoped_lock lock(endpoint_lock);
        RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<Endpoint> >::iterator e1 = endpoints.find(endpoint);
        if (e1 != endpoints.end())
        {
            e = e1->second;
        }
        else
        {
            return;
        }
    }

    e->TransportConnectionClosed(endpoint);
}

uint32_t RobotRaconteurNode::GetRequestTimeout()
{
    boost::mutex::scoped_lock lock(RequestTimeout_lock);
    return RequestTimeout;
}
void RobotRaconteurNode::SetRequestTimeout(uint32_t timeout)
{
    boost::mutex::scoped_lock lock(RequestTimeout_lock);
    RequestTimeout = timeout;
    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(weak_this, Node, -1, "RequestTimeout set to: " << timeout << " ms");
}

uint32_t RobotRaconteurNode::GetTransportInactivityTimeout()
{
    boost::mutex::scoped_lock lock(TransportInactivityTimeout_lock);
    return TransportInactivityTimeout;
}
void RobotRaconteurNode::SetTransportInactivityTimeout(uint32_t timeout)
{
    boost::mutex::scoped_lock lock(TransportInactivityTimeout_lock);
    TransportInactivityTimeout = timeout;
    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(weak_this, Node, -1, "TransportInactivityTimeout set to: " << timeout << " ms");
}

uint32_t RobotRaconteurNode::GetEndpointInactivityTimeout()
{
    boost::mutex::scoped_lock lock(EndpointInactivityTimeout_lock);
    return EndpointInactivityTimeout;
}

void RobotRaconteurNode::SetEndpointInactivityTimeout(uint32_t timeout)
{
    boost::mutex::scoped_lock lock(EndpointInactivityTimeout_lock);
    EndpointInactivityTimeout = timeout;
    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(weak_this, Node, -1, "EndpointInactivityTimeout set to: " << timeout << " ms");
}

uint32_t RobotRaconteurNode::GetMemoryMaxTransferSize()
{
    boost::mutex::scoped_lock lock(MemoryMaxTransferSize_lock);
    return MemoryMaxTransferSize;
}

void RobotRaconteurNode::SetMemoryMaxTransferSize(uint32_t size)
{
    boost::mutex::scoped_lock lock(MemoryMaxTransferSize_lock);
    MemoryMaxTransferSize = size;
    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(weak_this, Node, -1, "MemoryMaxTransferSize set to: " << size << " bytes");
}

const RR_SHARED_PTR<RobotRaconteur::DynamicServiceFactory> RobotRaconteurNode::GetDynamicServiceFactory()
{
    boost::mutex::scoped_lock lock(dynamic_factory_lock);
    return dynamic_factory;
}

void RobotRaconteurNode::SetDynamicServiceFactory(const RR_SHARED_PTR<RobotRaconteur::DynamicServiceFactory>& f)
{
    boost::mutex::scoped_lock lock(dynamic_factory_lock);

    if (this->dynamic_factory != 0)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(weak_this, Node, -1, "Dynamic service factory already set");
        throw InvalidOperationException("Dynamic service factory already set");
    }
    this->dynamic_factory = f;
}

RR_INTRUSIVE_PTR<Message> RobotRaconteurNode::GenerateErrorReturnMessage(const RR_INTRUSIVE_PTR<Message>& m,
                                                                         MessageErrorType err,
                                                                         boost::string_ref errname,
                                                                         boost::string_ref errdesc)
{
    RR_INTRUSIVE_PTR<Message> ret = CreateMessage();
    ret->header = CreateMessageHeader();
    ret->header->ReceiverNodeName = m->header->SenderNodeName;
    ret->header->SenderNodeName = m->header->ReceiverNodeName;
    ret->header->ReceiverNodeID = m->header->SenderNodeID;
    ret->header->ReceiverEndpoint = m->header->SenderEndpoint;
    ret->header->SenderEndpoint = m->header->ReceiverEndpoint;
    ret->header->SenderNodeID = m->header->ReceiverNodeID;
    BOOST_FOREACH (RR_INTRUSIVE_PTR<MessageEntry>& me, m->entries)
    {
        if ((static_cast<int32_t>(me->EntryType)) % 2 == 1)
        {
            RR_INTRUSIVE_PTR<MessageEntry> eret =
                CreateMessageEntry(static_cast<MessageEntryType>(me->EntryType + 1), me->MemberName);
            eret->RequestID = me->RequestID;
            eret->ServicePath = me->ServicePath;
            eret->AddElement("errorname", stringToRRArray(errname));
            eret->AddElement("errorstring", stringToRRArray(errdesc));
            eret->Error = err;
            ret->entries.push_back(eret);
        }
    }
    return ret;
}

RR_SHARED_PTR<ServerContext> RobotRaconteurNode::RegisterService(
    boost::string_ref name, boost::string_ref servicetype, const RR_SHARED_PTR<RRObject>& obj,
    const RR_SHARED_PTR<ServiceSecurityPolicy>& securitypolicy)
{

    if (!boost::regex_match(name.begin(), name.end(), boost::regex("^[a-zA-Z][a-zA-Z0-9_]*$")))
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(weak_this, Node, -1, "\"" << name << "\" is an invalid service name");
        throw InvalidArgumentException("\"" + name + "\" is an invalid service name");
    }

    RR_SHARED_PTR<ServerContext> c;

    {
        boost::unique_lock<boost::shared_mutex> lock(services_lock);

        if (services.count(name.to_string()) != 0)
        {
            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(weak_this, Node, -1, "Service name \"" << name << "\" in use");
            throw InvalidOperationException("Service name in use");
        }

        c = RR_MAKE_SHARED<ServerContext>(GetServiceType(servicetype), shared_from_this());
        c->SetBaseObject(name, obj, securitypolicy);

        // RegisterEndpoint(c);
        services.insert(make_pair(RR_MOVE(name.to_string()), c));
    }

    UpdateServiceStateNonce();

    ROBOTRACONTEUR_LOG_INFO_COMPONENT(weak_this, Node, -1, "Service \"" << name << "\" registered");

    return c;
}

RR_SHARED_PTR<ServerContext> RobotRaconteurNode::RegisterService(const RR_SHARED_PTR<ServerContext>& c)
{

    if (!boost::regex_match(c->GetServiceName(), boost::regex("^[a-zA-Z][a-zA-Z0-9_]*$")))
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(weak_this, Node, -1,
                                           "\"" << c->GetServiceName() << "\" is an invalid service name");
        throw InvalidArgumentException("\"" + c->GetServiceName() + "\" is an invalid NodeName");
    }

    {
        boost::unique_lock<boost::shared_mutex> lock(services_lock);
        if (services.count(c->GetServiceName()) != 0)
        {
            CloseService(c->GetServiceName());
        }

        services.insert(std::make_pair(c->GetServiceName(), c));
    }

    UpdateServiceStateNonce();

    ROBOTRACONTEUR_LOG_INFO_COMPONENT(weak_this, Node, -1, "Service \"" << c->GetServiceName() << "\" registered");

    return c;
}

void RobotRaconteurNode::CloseService(boost::string_ref sname)
{

    RR_SHARED_PTR<ServerContext> s;
    {
        boost::unique_lock<boost::shared_mutex> lock(services_lock);
        RR_UNORDERED_MAP<std::string, RR_SHARED_PTR<ServerContext> >::iterator e1 = services.find(sname.to_string());
        if (e1 == services.end())
        {
            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(weak_this, Node, -1,
                                               "Cannot close nonexistent service \"" << sname << "\"");
            throw ServiceException("Service not found");
        }
        s = e1->second;
        s->Close();
        // DeleteEndpoint(s);
        services.erase(sname.to_string());
    }

    ROBOTRACONTEUR_LOG_INFO_COMPONENT(weak_this, Node, -1, "Service \"" << sname << "\" closed");
}

std::vector<std::string> RobotRaconteurNode::GetRegisteredServiceNames()
{
    boost::shared_lock<boost::shared_mutex> lock(services_lock);
    std::vector<std::string> o;
    BOOST_FOREACH (RR_SHARED_PTR<ServerContext>& e, services | boost::adaptors::map_values)
    {
        o.push_back(e->GetServiceName());
    }

    return o;
}

RR_SHARED_PTR<ServerContext> RobotRaconteurNode::GetService(boost::string_ref name)
{

    boost::shared_lock<boost::shared_mutex> lock(services_lock);
    RR_UNORDERED_MAP<std::string, RR_SHARED_PTR<ServerContext> >::iterator e1 = services.find(name.to_string());
    if (e1 != services.end())
    {
        return e1->second;
    }

    ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(weak_this, Node, -1, "Service \"" << name << "\" not found");
    throw ServiceNotFoundException("Service " + name + " not found");
}

RR_INTRUSIVE_PTR<Message> RobotRaconteurNode::SpecialRequest(const RR_INTRUSIVE_PTR<Message>& m, uint32_t transportid,
                                                             const RR_SHARED_PTR<ITransportConnection>& tc)
{

    if (!(m->header->ReceiverNodeID == NodeID::GetAny() &&
          (m->header->ReceiverNodeName == "" || m->header->ReceiverNodeName == NodeName())) &&
        !(m->header->ReceiverNodeID == NodeID()))
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(
            weak_this, Node, -1,
            "Received SpecialRequest with invalid ReceiverNodeID: " << m->header->ReceiverNodeID.ToString());
        return GenerateErrorReturnMessage(m, MessageErrorType_NodeNotFound, "RobotRaconteur.NodeNotFound",
                                          "Could not find route to remote node");
    }

    {
        boost::shared_lock<boost::shared_mutex> lock3(tap_lock);
        if (tap)
        {
            tap->RecordMessage(m);
        }
    }

    if (m->header->ReceiverEndpoint != 0 && m->entries.size() == 1 &&
        m->entries.at(0)->EntryType == MessageEntryType_ObjectTypeName)
    {
        // Workaround for security of getting object types
        MessageReceived(m);
        return RR_INTRUSIVE_PTR<Message>();
    }

    RR_INTRUSIVE_PTR<Message> ret = CreateMessage();
    ret->header = CreateMessageHeader();
    ret->header->ReceiverNodeName = m->header->SenderNodeName;
    ret->header->SenderNodeName = this->NodeName();
    ret->header->ReceiverNodeID = m->header->SenderNodeID;
    ret->header->ReceiverEndpoint = m->header->SenderEndpoint;
    ret->header->SenderEndpoint = m->header->ReceiverEndpoint;
    ret->header->SenderNodeID = this->NodeID();

    BOOST_FOREACH (RR_INTRUSIVE_PTR<MessageEntry>& e, m->entries)
    {
        RR_INTRUSIVE_PTR<MessageEntry> eret =
            ret->AddEntry(static_cast<MessageEntryType>(static_cast<uint16_t>(e->EntryType) + 1), e->MemberName);
        eret->RequestID = e->RequestID;
        eret->ServicePath = e->ServicePath;

        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(
            weak_this, Node, m->header->ReceiverEndpoint, e->ServicePath, e->MemberName,
            "SpecialRequest received from " << m->header->SenderNodeID.ToString() << " ep " << m->header->SenderEndpoint
                                            << " to " << m->header->ReceiverNodeID.ToString() << " ep "
                                            << m->header->ReceiverEndpoint << " EntryType " << e->EntryType << " Error "
                                            << e->Error);

        switch (e->EntryType)
        {
        case MessageEntryType_GetNodeInfo:
            break;
        case MessageEntryType_ObjectTypeName: {
            boost::string_ref path = e->ServicePath.str();

            std::vector<std::string> s1;
            boost::split(s1, path, boost::is_from_range('.', '.'));

            try
            {
                RR_SHARED_PTR<ServerContext> s;

                s = GetService(s1.at(0));

                RobotRaconteurVersion v;
                RR_INTRUSIVE_PTR<MessageElement> m_ver;
                if (e->TryFindElement("clientversion", m_ver))
                {
                    v.FromString(m_ver->CastDataToString());
                }

                std::string objtype = s->GetObjectType(path, v);
                eret->AddElement("objecttype", stringToRRArray(objtype));

                boost::tuple<boost::string_ref, boost::string_ref> objtype_s = SplitQualifiedName(objtype);
                RR_SHARED_PTR<ServiceEntryDefinition> def =
                    TryFindByName(GetServiceType(objtype_s.get<0>())->ServiceDef()->Objects, objtype_s.get<1>());
                if (!def)
                    throw ServiceException("Invalid service object");

                if (!def->Implements.empty())
                {
                    RR_INTRUSIVE_PTR<RRList<RRArray<char> > > implements = stringVectorToRRList(def->Implements);
                    eret->AddElement("objectimplements", PackListType<RRArray<char> >(implements));
                }
            }
            catch (std::exception&)
            {
                ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(weak_this, Node, m->header->ReceiverEndpoint, e->ServicePath,
                                                        e->MemberName,
                                                        "Client requested type of an invalid service path")
                eret->AddElement("errorname", stringToRRArray("RobotRaconteur.ObjectNotFoundException"));
                eret->AddElement("errorstring", stringToRRArray("Object not found"));
                eret->Error = MessageErrorType_ObjectNotFound;
            }
        }
        break;
        case MessageEntryType_GetServiceDesc: {
            // string name = (string)e.FindElement("servicename").Data;
            std::string name = e->ServicePath.str().to_string();
            try
            {
                std::string servicedef;

                bool is_service_type = false;
                bool is_service_type2 = false;

                RR_INTRUSIVE_PTR<MessageElement> el1;
                RR_INTRUSIVE_PTR<MessageElement> el2;
                is_service_type = e->TryFindElement("ServiceType", el1);
                is_service_type2 = e->TryFindElement("servicetype", el2);

                if (is_service_type)
                {
                    name = el1->CastDataToString();
                    servicedef = GetServiceType(name)->DefString();
                }
                else if (is_service_type2)
                {
                    name = el2->CastDataToString();
                    servicedef = GetServiceType(name)->DefString();
                }
                else
                {
                    RobotRaconteurVersion v;
                    RR_INTRUSIVE_PTR<MessageElement> m_ver;
                    if (e->TryFindElement("clientversion", m_ver))
                    {
                        v.FromString(m_ver->CastDataToString());
                    }

                    RR_SHARED_PTR<ServerContext> service = GetService(name);

                    servicedef = GetService(name)->GetRootObjectServiceDef(v)->DefString();
                    RR_INTRUSIVE_PTR<RRMap<std::string, RRValue> > attr = AllocateEmptyRRMap<std::string, RRValue>();
                    attr->GetStorageContainer() = service->GetAttributes();
                    eret->AddElement("attributes", PackMapType<std::string, RRValue>(attr));

                    std::vector<std::string> extra_imports = service->GetExtraImports();
                    if (!extra_imports.empty())
                    {
                        RR_INTRUSIVE_PTR<RRList<RRArray<char> > > extra_imports2 = stringVectorToRRList(extra_imports);
                        eret->AddElement("extraimports", PackListType<RRArray<char> >(extra_imports2));
                    }
                }
                eret->AddElement("servicedef", stringToRRArray(servicedef));
            }
            catch (std::exception&)
            {
                ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(
                    weak_this, Node, m->header->ReceiverEndpoint, e->ServicePath, e->MemberName,
                    "Client requested type of an invalid service type: \"" << name << "\"")
                eret->AddElement("errorname", stringToRRArray("RobotRaconteur.ServiceNotFoundException"));
                eret->AddElement("errorstring", stringToRRArray("Service not found"));
                eret->Error = MessageErrorType_ServiceNotFound;
            }
        }
        break;
        case MessageEntryType_ConnectClient: {
            boost::string_ref name = e->ServicePath.str();

            try
            {
                RR_SHARED_PTR<ServerEndpoint> se = RR_MAKE_SHARED<ServerEndpoint>(shared_from_this());

                RR_SHARED_PTR<ServerContext> c = GetService(name);
                se->service = c;

                se->SetRemoteEndpoint(m->header->SenderEndpoint);
                se->SetRemoteNodeID(m->header->SenderNodeID);
                RegisterEndpoint(se);

                se->SetTransport(transportid);
                se->SetTransportConnection(tc);

                c->AddClient(se);

                ret->header->SenderEndpoint = se->GetLocalEndpoint();
            }
            catch (std::exception& exp)
            {
                ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(weak_this, Node, m->header->ReceiverEndpoint, e->ServicePath,
                                                        e->MemberName, "Error connecting client: " << exp.what())
                eret->AddElement("errorname", stringToRRArray("RobotRaconteur.ServiceNotFoundException"));
                eret->AddElement("errorstring", stringToRRArray("Service not found"));
                eret->Error = MessageErrorType_ServiceNotFound;
            }
        }
        break;
        case MessageEntryType_DisconnectClient: {

            try
            {
                std::string name = e->FindElement("servicename")->CastDataToString();
                RR_SHARED_PTR<ServerEndpoint> se;

                {
                    boost::mutex::scoped_lock lock(endpoint_lock);
                    se = RR_DYNAMIC_POINTER_CAST<ServerEndpoint>(endpoints.at(m->header->ReceiverEndpoint));
                    if (se == 0)
                        throw InternalErrorException("");
                }

                {
                    // boost::mutex::scoped_lock lock(services_lock);
                    try
                    {
                        GetService(name)->RemoveClient(se);
                    }
                    catch (std::out_of_range&)
                    {
                        throw ServiceNotFoundException("Service " + name + " not found");
                    }
                }

                try
                {
                    RobotRaconteurNode::DeleteEndpoint(se);
                }
                catch (std::exception&)
                {}
                // eret.AddElement("servicedef", servicedef);
            }
            catch (std::exception& exp)
            {
                ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(weak_this, Node, m->header->ReceiverEndpoint, e->ServicePath,
                                                        e->MemberName, "Error disconnecting client: " << exp.what())
                eret->AddElement("errorname", stringToRRArray("RobotRaconteur.ServiceNotFoundException"));
                eret->AddElement("errorstring", stringToRRArray("Service not found"));
                eret->Error = MessageErrorType_ServiceNotFound;
            }
        }
        break;
        case MessageEntryType_ConnectionTest:
            break;

        case MessageEntryType_NodeCheckCapability:
            eret->AddElement("return", ScalarToRRArray(static_cast<uint32_t>(0)));
            break;
        case MessageEntryType_GetServiceAttributes: {
            const MessageStringPtr& path = e->ServicePath;

            std::vector<std::string> s1;
            boost::string_ref s2 = path.str();
            boost::split(s1, s2, boost::is_from_range('.', '.'));
            try
            {
                RR_SHARED_PTR<ServerContext> s;

                s = GetService(s1.at(0));

                RR_INTRUSIVE_PTR<RRMap<std::string, RRValue> > attr = AllocateEmptyRRMap<std::string, RRValue>();
                attr->GetStorageContainer() = (s->GetAttributes());
                eret->AddElement("return", PackMapType<std::string, RRValue>(attr));
            }
            catch (std::exception& exp)
            {
                ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(weak_this, Node, m->header->ReceiverEndpoint, e->ServicePath,
                                                        e->MemberName,
                                                        "Error returning service attributes: " << exp.what())
                eret->AddElement("errorname", stringToRRArray("RobotRaconteur.ServiceError"));
                eret->AddElement("errorstring", stringToRRArray("Service not found"));
                eret->Error = MessageErrorType_ServiceError;
            }
        }
        break;

        case MessageEntryType_ConnectClientCombined: {
            boost::string_ref name = e->ServicePath.str();

            RR_SHARED_PTR<ServerContext> c;

            RobotRaconteurVersion v;
            RR_INTRUSIVE_PTR<MessageElement> m_ver;
            if (e->TryFindElement("clientversion", m_ver))
            {
                v.FromString(m_ver->CastDataToString());
            }

            try
            {
                c = GetService(name);
                std::string objtype = c->GetRootObjectType(v);
                eret->AddElement("objecttype", stringToRRArray(objtype));

                boost::tuple<boost::string_ref, boost::string_ref> objtype_s = SplitQualifiedName(objtype);
                RR_SHARED_PTR<ServiceEntryDefinition> def =
                    TryFindByName(GetServiceType(objtype_s.get<0>())->ServiceDef()->Objects, objtype_s.get<1>());
                if (!def)
                    throw ServiceException("Invalid service object");

                if (!def->Implements.empty())
                {
                    RR_INTRUSIVE_PTR<RRList<RRArray<char> > > implements = stringVectorToRRList(def->Implements);
                    eret->AddElement("objectimplements", PackListType<RRArray<char> >(implements));
                }
            }
            catch (std::exception& exp)
            {
                ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(weak_this, Node, m->header->ReceiverEndpoint, e->ServicePath,
                                                        e->MemberName, "Error connecting client: " << exp.what())
                eret->elements.clear();
                eret->AddElement("errorname", stringToRRArray("RobotRaconteur.ServiceNotFoundException"));
                eret->AddElement("errorstring", stringToRRArray("Service not found"));
                eret->Error = MessageErrorType_ServiceNotFound;
                break;
            }

            try
            {
                bool returnservicedef = true;

                try
                {
                    RR_INTRUSIVE_PTR<MessageElement> returnservicedefs_el;
                    if (e->TryFindElement("returnservicedefs", returnservicedefs_el))
                    {
                        std::string returnservicedef_str = returnservicedefs_el->CastDataToString();
                        boost::trim(returnservicedef_str);
                        if (boost::to_lower_copy(returnservicedef_str) == "false" || returnservicedef_str == "0")
                        {
                            returnservicedef = false;
                        }
                    }
                }
                catch (std::exception&)
                {}

                if (returnservicedef)
                {
                    RR_SHARED_PTR<ServiceFactory> servicedef1 = c->GetRootObjectServiceDef(v);
                    std::map<std::string, RR_SHARED_PTR<ServiceFactory> > defs;
                    defs.insert(std::make_pair(servicedef1->GetServiceName(), servicedef1));

                    std::vector<std::string> extra_imports = c->GetExtraImports();
                    BOOST_FOREACH (const std::string& e, extra_imports)
                    {
                        if (defs.find(e) == defs.end())
                        {
                            defs.insert(std::make_pair(e, GetServiceType(e)));
                        }
                    }

                    while (true)
                    {
                        bool new_found = false;
                        std::vector<std::string> v1;
                        boost::range::copy(defs | boost::adaptors::map_keys, std::back_inserter(v1));
                        BOOST_FOREACH (std::string& e, v1)
                        {
                            RR_SHARED_PTR<ServiceFactory> d1 = defs.at(e);
                            BOOST_FOREACH (std::string& e2, d1->ServiceDef()->Imports)
                            {
                                if (defs.find(e2) == defs.end())
                                {
                                    RR_SHARED_PTR<ServiceFactory> d2 = GetServiceType(e2);
                                    defs.insert(std::make_pair(d2->GetServiceName(), d2));
                                    new_found = true;
                                }
                            }
                        }

                        if (!new_found)
                            break;
                    }

                    uint32_t n = 0;

                    std::vector<RR_INTRUSIVE_PTR<MessageElement> > servicedef_list;
                    BOOST_FOREACH (const RR_SHARED_PTR<ServiceFactory>& d1, defs | boost::adaptors::map_values)
                    {
                        RR_INTRUSIVE_PTR<MessageElement> e1 =
                            CreateMessageElement(boost::numeric_cast<int32_t>(n), stringToRRArray(d1->DefString()));
                        servicedef_list.push_back(e1);
                        n++;
                    }

                    eret->AddElement("servicedefs", CreateMessageElementNestedElementList(DataTypes_list_t, "",
                                                                                          RR_MOVE(servicedef_list)));
                }
            }
            catch (std::exception& exp)
            {
                ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(weak_this, Node, m->header->ReceiverEndpoint, e->ServicePath,
                                                        e->MemberName, "Error connecting client: " << exp.what())
                eret->elements.clear();
                eret->AddElement("errorname", stringToRRArray("RobotRaconteur.ServiceNotFoundException"));
                eret->AddElement("errorstring", stringToRRArray("Service factory configuration error"));
                eret->Error = MessageErrorType_ServiceNotFound;
                break;
            }

            RR_SHARED_PTR<ServerEndpoint> se;

            try
            {

                se = RR_MAKE_SHARED<ServerEndpoint>(shared_from_this());

                RegisterEndpoint(se);

                se->service = c;

                se->SetRemoteEndpoint(m->header->SenderEndpoint);
                se->SetRemoteNodeID(m->header->SenderNodeID);

                se->SetTransport(transportid);
                se->SetTransportConnection(tc);

                se->SetClientVersion(v);

                c->AddClient(se);

                ret->header->SenderEndpoint = se->GetLocalEndpoint();
            }
            catch (std::exception& exp)
            {
                if (se)
                {
                    try
                    {
                        DeleteEndpoint(se);
                    }
                    catch (std::exception&)
                    {}
                }

                ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(weak_this, Node, m->header->ReceiverEndpoint, e->ServicePath,
                                                        e->MemberName, "Error connecting client: " << exp.what())
                eret->elements.clear();
                eret->AddElement("errorname", stringToRRArray("RobotRaconteur.ServiceNotFoundException"));
                eret->AddElement("errorstring", stringToRRArray("Service not found"));
                eret->Error = MessageErrorType_ServiceNotFound;
                break;
            }

            try
            {
                if (c->RequireValidUser())
                {
                    RR_INTRUSIVE_PTR<MessageElement> username_el;
                    RR_INTRUSIVE_PTR<MessageElement> credentials_el;
                    if (!e->TryFindElement("username", username_el))
                    {
                        throw AuthenticationException("Username not provided");
                    }

                    if (!e->TryFindElement("credentials", credentials_el))
                    {
                        throw AuthenticationException("Credentials not provided");
                    }

                    RR_INTRUSIVE_PTR<RRMap<std::string, RRValue> > credentials =
                        rr_cast<RRMap<std::string, RRValue> >(UnpackMapType<std::string, RRValue>(
                            credentials_el->CastDataToNestedList(DataTypes_dictionary_t)));
                    if (!credentials)
                    {
                        throw AuthenticationException("Credentials cannot be null");
                    }

                    std::string username = username_el->CastDataToString();

                    se->AuthenticateUser(username, credentials->GetStorageContainer());
                }
                else
                {
                    RR_INTRUSIVE_PTR<MessageElement> username_el;
                    RR_INTRUSIVE_PTR<MessageElement> credentials_el;
                    if (e->TryFindElement("username", username_el) && e->TryFindElement("credentials", credentials_el))
                    {
                        throw AuthenticationException("Authentication not enabled for service");
                    }
                }
            }
            catch (std::exception& exp)
            {
                ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(weak_this, Node, m->header->ReceiverEndpoint, e->ServicePath,
                                                        e->MemberName, "Error authenticating client: " << exp.what())
                try
                {
                    c->RemoveClient(se);
                    DeleteEndpoint(se);
                }
                catch (std::exception&)
                {}

                eret->elements.clear();
                eret->AddElement("errorname", stringToRRArray("RobotRaconteur.AuthenticationError"));
                eret->AddElement("errorstring", stringToRRArray("Authentication Failed"));
                eret->Error = MessageErrorType_AuthenticationError;
                break;
            }
        }
        break;

        default:
            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(weak_this, Node, m->header->ReceiverEndpoint, e->ServicePath,
                                                    e->MemberName,
                                                    "Invalid special request EntryType: " << e->EntryType)
            eret->Error = MessageErrorType_ProtocolError;
            eret->AddElement("errorname", stringToRRArray("RobotRaconteur.ProtocolError"));
            eret->AddElement("errorstring", stringToRRArray("Invalid Special Operation"));
            break;
        }
    }

    {
        boost::shared_lock<boost::shared_mutex> lock3(tap_lock);
        if (tap)
        {
            tap->RecordMessage(ret);
        }
    }

    return ret;
}

void RobotRaconteurNode::AsyncConnectService(
    boost::string_ref url, boost::string_ref username,
    const RR_INTRUSIVE_PTR<RRMap<std::string, RRValue> >& credentials,
    boost::function<void(const RR_SHARED_PTR<ClientContext>&, ClientServiceListenerEventType,
                         const RR_SHARED_PTR<void>&)>
        listener,
    boost::string_ref objecttype,
    boost::function<void(const RR_SHARED_PTR<RRObject>&, const RR_SHARED_PTR<RobotRaconteurException>&)> handler,
    int32_t timeout)
{
    std::vector<std::string> urls;
    urls.push_back(RR_MOVE(url.to_string()));
    AsyncConnectService(urls, username, credentials, RR_MOVE(listener), objecttype, RR_MOVE(handler), timeout);
}

void RobotRaconteurNode::AsyncConnectService(
    const std::vector<std::string>& url, boost::string_ref username,
    const RR_INTRUSIVE_PTR<RRMap<std::string, RRValue> >& credentials,
    boost::function<void(const RR_SHARED_PTR<ClientContext>&, ClientServiceListenerEventType,
                         const RR_SHARED_PTR<void>&)>
        listener,
    boost::string_ref objecttype,
    boost::function<void(const RR_SHARED_PTR<RRObject>&, const RR_SHARED_PTR<RobotRaconteurException>&)> handler,
    int32_t timeout)
{

    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(weak_this, Node, -1,
                                       "Begin ConnectService with candidate urls: " << boost::join(url, ", "));

    if (url.empty())
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(weak_this, Node, -1, "No urls specified for ConnectService");
        throw InvalidArgumentException("URL vector must not be empty for AsyncConnectService");
    }

    std::vector<RR_SHARED_PTR<Transport> > atransports;

    std::map<std::string, RR_WEAK_PTR<Transport> > connectors;
    {
        boost::shared_lock<boost::shared_mutex> lock(transport_lock);
        boost::copy(transports | boost::adaptors::map_values, std::back_inserter(atransports));
    }

    BOOST_FOREACH (const std::string& e, url)
    {
        BOOST_FOREACH (const RR_SHARED_PTR<Transport>& end, atransports)
        {
            if (end == 0)
                continue;
            if (end->IsClient())
            {
                if (end->CanConnectService(e))
                {
                    connectors.insert(std::make_pair(e, RR_WEAK_PTR<Transport>(end)));
                }
            }
        }
    }

    if (connectors.empty())
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(
            weak_this, Node, -1,
            "ConnectService could not find any valid transports for urls: " << boost::join(url, ", "));
        throw ConnectionException("Could not find any valid transports for requested connection URLs");
    }

    RR_SHARED_PTR<detail::RobotRaconteurNode_connector> connector =
        RR_MAKE_SHARED<detail::RobotRaconteurNode_connector>(shared_from_this());
    GetThreadPool()->Post(boost::bind(&detail::RobotRaconteurNode_connector::connect, connector, connectors,
                                      username.to_string(), credentials, RR_MOVE(listener), objecttype.to_string(),
                                      boost::protect(RR_MOVE(handler)), timeout));
}

RR_SHARED_PTR<RRObject> RobotRaconteurNode::ConnectService(
    const std::vector<std::string>& urls, boost::string_ref username,
    const RR_INTRUSIVE_PTR<RRMap<std::string, RRValue> >& credentials,
    boost::function<void(const RR_SHARED_PTR<ClientContext>&, ClientServiceListenerEventType,
                         const RR_SHARED_PTR<void>&)>
        listener,
    boost::string_ref objecttype)
{
    ROBOTRACONTEUR_ASSERT_MULTITHREADED(shared_from_this());

    RR_SHARED_PTR<detail::sync_async_handler<RRObject> > h = RR_MAKE_SHARED<detail::sync_async_handler<RRObject> >(
        RR_MAKE_SHARED<ConnectionException>("Connection timed out"));
    AsyncConnectService(urls, username, credentials, RR_MOVE(listener), objecttype,
                        boost::bind(&detail::sync_async_handler<RRObject>::operator(), h, RR_BOOST_PLACEHOLDERS(_1),
                                    RR_BOOST_PLACEHOLDERS(_2)),
                        boost::numeric_cast<int32_t>(this->GetRequestTimeout() * 2));
    return h->end();
}

RR_SHARED_PTR<RRObject> RobotRaconteurNode::ConnectService(
    boost::string_ref url, boost::string_ref username,
    const RR_INTRUSIVE_PTR<RRMap<std::string, RRValue> >& credentials,
    boost::function<void(const RR_SHARED_PTR<ClientContext>&, ClientServiceListenerEventType,
                         const RR_SHARED_PTR<void>&)>
        listener,
    boost::string_ref objecttype)
{
    ROBOTRACONTEUR_ASSERT_MULTITHREADED(shared_from_this());

    std::vector<std::string> urls;
    urls.push_back(RR_MOVE(url.to_string()));
    return ConnectService(urls, username, credentials, RR_MOVE(listener), objecttype);
}

void RobotRaconteurNode::DisconnectService(const RR_SHARED_PTR<RRObject>& obj)
{
    ROBOTRACONTEUR_ASSERT_MULTITHREADED(shared_from_this());

    RR_SHARED_PTR<ClientContext> c;
    try
    {

        if (!obj)
            return;
        RR_SHARED_PTR<ServiceStub> stub = rr_cast<ServiceStub>(obj);
        c = stub->GetContext();
        c->Close();
    }
    catch (std::exception& e)
    {
        ROBOTRACONTEUR_LOG_INFO_COMPONENT(weak_this, Node, -1, "DisconnectClient failed: " << e.what());
        return;
    }

    ROBOTRACONTEUR_LOG_INFO_COMPONENT(weak_this, Node, c->GetLocalEndpoint(), "Client Disconnected");
}

void RobotRaconteurNode::AsyncDisconnectService(const RR_SHARED_PTR<RRObject>& obj, boost::function<void()> handler)
{
    if (!obj)
        return;
    RR_SHARED_PTR<ServiceStub> stub = rr_cast<ServiceStub>(obj);

    RR_SHARED_PTR<ClientContext> c = stub->GetContext();

    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(weak_this, Node, c->GetLocalEndpoint(), "Client Disconnecting");
    c->AsyncClose(RR_MOVE(handler));
}

std::map<std::string, RR_INTRUSIVE_PTR<RRValue> > RobotRaconteurNode::GetServiceAttributes(
    const RR_SHARED_PTR<RRObject>& obj)
{
    if (!obj)
        throw InvalidArgumentException("obj must not be null");
    RR_SHARED_PTR<ServiceStub> stub = rr_cast<ServiceStub>(obj);
    return stub->GetContext()->GetAttributes();
}

RobotRaconteur::NodeID RobotRaconteurNode::GetServiceNodeID(const RR_SHARED_PTR<RRObject>& obj)
{
    if (!obj)
        throw InvalidArgumentException("obj must not be null");
    RR_SHARED_PTR<ServiceStub> stub = rr_cast<ServiceStub>(obj);
    return stub->GetContext()->GetRemoteNodeID();
}

std::string RobotRaconteurNode::GetServiceNodeName(const RR_SHARED_PTR<RRObject>& obj)
{
    if (!obj)
        throw InvalidArgumentException("obj must not be null");
    RR_SHARED_PTR<ServiceStub> stub = rr_cast<ServiceStub>(obj);
    return stub->GetContext()->GetRemoteNodeName();
}

std::string RobotRaconteurNode::GetServiceName(const RR_SHARED_PTR<RRObject>& obj)
{
    if (!obj)
        throw InvalidArgumentException("obj must not be null");
    RR_SHARED_PTR<ServiceStub> stub = rr_cast<ServiceStub>(obj);
    return stub->GetContext()->GetServiceName();
}

std::string RobotRaconteurNode::GetObjectServicePath(const RR_SHARED_PTR<RRObject>& obj)
{
    if (!obj)
        throw InvalidArgumentException("obj must not be null");
    RR_SHARED_PTR<ServiceStub> stub = rr_cast<ServiceStub>(obj);
    return stub->ServicePath;
}

uint32_t RobotRaconteurNode::RegisterEndpoint(const RR_SHARED_PTR<Endpoint>& e)
{

    {
        boost::random::uniform_int_distribution<uint32_t> distribution(0, std::numeric_limits<uint32_t>::max());

        boost::mutex::scoped_lock lock(endpoint_lock);
        uint32_t id = 0;
        {
            boost::mutex::scoped_lock lock(random_generator_lock);
            do
            {
                id = distribution(*random_generator);
            } while (endpoints.count(id) != 0 || recent_endpoints.count(id) != 0);
        }
        e->SetLocalEndpoint(id);
        endpoints.insert(std::make_pair(id, e));

        ROBOTRACONTEUR_LOG_TRACE_COMPONENT(weak_this, Node, e->GetLocalEndpoint(),
                                           "Endpoint registered, RemoteNodeID " << e->GetRemoteNodeID().ToString()
                                                                                << " ep " << e->GetRemoteEndpoint());

        return id;
    }
}

void RobotRaconteurNode::DeleteEndpoint(const RR_SHARED_PTR<Endpoint>& e)
{

    try
    {

        {
            boost::mutex::scoped_lock lock(endpoint_lock);
            RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<Endpoint> >::iterator e1 = endpoints.find(e->GetLocalEndpoint());
            if (e1 != endpoints.end())
            {
                endpoints.erase(e1);
                recent_endpoints.insert(std::make_pair(e->GetLocalEndpoint(), NowNodeTime()));
            }
        }
    }
    catch (std::exception& exp)
    {
        ROBOTRACONTEUR_LOG_TRACE_COMPONENT(weak_this, Node, e->GetLocalEndpoint(),
                                           "Error deleting endpoint: " << exp.what());
    }

    try
    {
        RR_SHARED_PTR<Transport> c;
        {
            boost::shared_lock<boost::shared_mutex> lock(transport_lock);
            RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<Transport> >::iterator e1 = transports.find(e->GetTransport());
            if (e1 != transports.end())
            {
                c = e1->second;
            }
        }
        if (c)
            c->CloseTransportConnection(e);
    }
    catch (std::exception& exp)
    {
        ROBOTRACONTEUR_LOG_TRACE_COMPONENT(weak_this, Node, e->GetLocalEndpoint(),
                                           "Error closing transport connection for deleted endpoint: " << exp.what());
    }

    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(weak_this, Node, e->GetLocalEndpoint(), "Endpoint deleted");
}

void RobotRaconteurNode::CheckConnection(uint32_t endpoint)
{
    RR_SHARED_PTR<Endpoint> e;
    {
        boost::mutex::scoped_lock lock(endpoint_lock);
        RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<Endpoint> >::iterator e1 = endpoints.find(endpoint);
        if (e1 == endpoints.end())
        {
            if (is_shutdown)
            {
                throw InvalidOperationException("Node has been shut down");
            }

            throw InvalidEndpointException("Invalid Endpoint");
        }
        e = e1->second;
    }

    RR_SHARED_PTR<Transport> c;
    {
        boost::shared_lock<boost::shared_mutex> lock(transport_lock);
        RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<Transport> >::iterator e1 = transports.find(e->GetTransport());
        if (e1 == transports.end())
            throw ConnectionException("Transport connection not found");
        c = e1->second;
    }
    c->CheckConnection(endpoint);
}

std::vector<NodeDiscoveryInfo> RobotRaconteurNode::GetDetectedNodes()
{
    if (!m_Discovery)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(weak_this, Node, -1, "Node not init");
        throw InvalidOperationException("Node not init");
    }
    return m_Discovery->GetDetectedNodes();
}

NodeInfo2 RobotRaconteurNode::GetDetectedNodeCacheInfo(const RobotRaconteur::NodeID& nodeid)
{
    if (!m_Discovery)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(weak_this, Node, -1, "Node not init");
        throw InvalidOperationException("Node not init");
    }
    return m_Discovery->GetDetectedNodeCacheInfo(nodeid);
}

bool RobotRaconteurNode::TryGetDetectedNodeCacheInfo(const RobotRaconteur::NodeID& nodeid, NodeInfo2& nodeinfo2)
{
    if (!m_Discovery)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(weak_this, Node, -1, "Node not init");
        throw InvalidOperationException("Node not init");
    }
    return m_Discovery->TryGetDetectedNodeCacheInfo(nodeid, nodeinfo2);
}

void RobotRaconteurNode::NodeDetected(const NodeDiscoveryInfo& info)
{
    if (!m_Discovery)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(weak_this, Node, -1, "Node not init");
        throw InvalidOperationException("Node not init");
    }
    m_Discovery->NodeDetected(info);
}

void RobotRaconteurNode::UpdateDetectedNodes(const std::vector<std::string>& schemes)
{
    ROBOTRACONTEUR_ASSERT_MULTITHREADED(shared_from_this());

    if (!m_Discovery)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(weak_this, Node, -1, "Node not init");
        throw InvalidOperationException("Node not init");
    }
    m_Discovery->UpdateDetectedNodes(schemes);
}

void RobotRaconteurNode::AsyncUpdateDetectedNodes(const std::vector<std::string>& schemes,
                                                  boost::function<void()> handler, int32_t timeout)
{
    if (!m_Discovery)
        throw InvalidOperationException("Node not init");
    m_Discovery->AsyncUpdateDetectedNodes(schemes, handler, timeout);
}

void RobotRaconteurNode::NodeAnnouncePacketReceived(boost::string_ref packet)
{
    if (!m_Discovery)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(weak_this, Node, -1, "Node not init");
        throw InvalidOperationException("Node not init");
    }
    m_Discovery->NodeAnnouncePacketReceived(packet);
}

void RobotRaconteurNode::CleanDiscoveredNodes()
{
    if (!m_Discovery)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(weak_this, Node, -1, "Node not init");
        throw InvalidOperationException("Node not init");
    }
    m_Discovery->CleanDiscoveredNodes();
}

uint32_t RobotRaconteurNode::GetNodeDiscoveryMaxCacheCount()
{
    if (!m_Discovery)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(weak_this, Node, -1, "Node not init");
        throw InvalidOperationException("Node not init");
    }
    return m_Discovery->GetNodeDiscoveryMaxCacheCount();
}
void RobotRaconteurNode::SetNodeDiscoveryMaxCacheCount(uint32_t count)
{
    if (!m_Discovery)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(weak_this, Node, -1, "Node not init");
        throw InvalidOperationException("Node not init");
    }
    m_Discovery->SetNodeDiscoveryMaxCacheCount(count);
}

RR_SHARED_PTR<ServiceSubscription> RobotRaconteurNode::SubscribeServiceByType(
    const std::vector<std::string>& service_types, const RR_SHARED_PTR<ServiceSubscriptionFilter>& filter)
{
    if (!m_Discovery)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(weak_this, Node, -1, "Node not init");
        throw InvalidOperationException("Node not init");
    }
    return m_Discovery->SubscribeServiceByType(service_types, filter);
}

RR_SHARED_PTR<ServiceSubscription> RobotRaconteurNode::SubscribeService(
    const std::vector<std::string>& url, boost::string_ref username,
    const RR_INTRUSIVE_PTR<RRMap<std::string, RRValue> >& credentials, boost::string_ref objecttype)
{
    if (!m_Discovery)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(weak_this, Node, -1, "Node not init");
        throw InvalidOperationException("Node not init");
    }
    return m_Discovery->SubscribeService(url, username, credentials, objecttype);
}

RR_SHARED_PTR<ServiceSubscription> RobotRaconteurNode::SubscribeService(
    const std::string& url, boost::string_ref username,
    const RR_INTRUSIVE_PTR<RRMap<std::string, RRValue> >& credentials, boost::string_ref objecttype)
{
    if (!m_Discovery)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(weak_this, Node, -1, "Node not init");
        throw InvalidOperationException("Node not init");
    }
    std::vector<std::string> url2;
    url2.push_back(url);
    return m_Discovery->SubscribeService(url2, username, credentials, objecttype);
}

RR_SHARED_PTR<ServiceInfo2Subscription> RobotRaconteurNode::SubscribeServiceInfo2(
    const std::vector<std::string>& service_types, const RR_SHARED_PTR<ServiceSubscriptionFilter>& filter)
{
    if (!m_Discovery)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(weak_this, Node, -1, "Node not init");
        throw InvalidOperationException("Node not init");
    }
    return m_Discovery->SubscribeServiceInfo2(service_types, filter);
}

void RobotRaconteurNode::FireNodeDetected(const RR_SHARED_PTR<NodeDiscoveryInfo>& node,
                                          const RR_SHARED_PTR<std::vector<ServiceInfo2> >& services)
{
    discovery_updated_listeners(*node, *services);
}

void RobotRaconteurNode::FireNodeLost(const RR_SHARED_PTR<NodeDiscoveryInfo>& node) { discovery_lost_listeners(*node); }

std::string RobotRaconteurNode::SelectRemoteNodeURL(const std::vector<std::string>& urls)
{
    BOOST_FOREACH (const std::string& e, urls)
    {
        if (boost::starts_with(e, "rr+intra://"))
            return e;
    }

    BOOST_FOREACH (const std::string& e, urls)
    {
        if (boost::starts_with(e, "rr+local://"))
            return e;
    }

    BOOST_FOREACH (const std::string& e, urls)
    {
        if (boost::starts_with(e, "rr+pci://"))
            return e;
    }

    BOOST_FOREACH (const std::string& e, urls)
    {
        if (boost::starts_with(e, "rr+usb://"))
            return e;
    }

    BOOST_FOREACH (const std::string& e, urls)
    {
        if (boost::starts_with(boost::to_lower_copy(e), "rrs+tcp://127.0.0.1"))
            return e;
    }

    BOOST_FOREACH (const std::string& e, urls)
    {
        if (boost::starts_with(boost::to_lower_copy(e), "rrs+tcp://[::1]"))
            return e;
    }

    BOOST_FOREACH (const std::string& e, urls)
    {
        if (boost::starts_with(boost::to_lower_copy(e), "rrs+tcp://localhost"))
            return e;
    }

    BOOST_FOREACH (const std::string& e, urls)
    {
        if (boost::starts_with(boost::to_lower_copy(e), "rrs+tcp://[fe80"))
            return e;
    }

    BOOST_FOREACH (const std::string& e, urls)
    {
        if (boost::starts_with(boost::to_lower_copy(e), "rrs+tcp://"))
            return e;
    }

    BOOST_FOREACH (const std::string& e, urls)
    {
        if (boost::starts_with(boost::to_lower_copy(e), "rr+tcp://127.0.0.1"))
            return e;
    }

    BOOST_FOREACH (const std::string& e, urls)
    {
        if (boost::starts_with(boost::to_lower_copy(e), "rr+tcp://[::1]"))
            return e;
    }

    BOOST_FOREACH (const std::string& e, urls)
    {
        if (boost::starts_with(boost::to_lower_copy(e), "rr+tcp://localhost"))
            return e;
    }

    BOOST_FOREACH (const std::string& e, urls)
    {
        if (boost::starts_with(boost::to_lower_copy(e), "rr+tcp://[fe80"))
            return e;
    }

    BOOST_FOREACH (const std::string& e, urls)
    {
        if (boost::starts_with(boost::to_lower_copy(e), "rr+tcp://"))
            return e;
    }

    return urls.at(0);
}

void RobotRaconteurNode::AsyncFindServiceByType(
    boost::string_ref servicetype, const std::vector<std::string>& transportschemes,
    boost::function<void(const RR_SHARED_PTR<std::vector<ServiceInfo2> >&)> handler, int32_t timeout)
{
    if (!m_Discovery)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(weak_this, Node, -1, "Node not init");
        throw InvalidOperationException("Node not init");
    }
    m_Discovery->AsyncFindServiceByType(servicetype, transportschemes, handler, timeout);
}

std::vector<ServiceInfo2> RobotRaconteurNode::FindServiceByType(boost::string_ref servicetype,
                                                                const std::vector<std::string>& transportschemes)
{
    ROBOTRACONTEUR_ASSERT_MULTITHREADED(shared_from_this());

    if (!m_Discovery)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(weak_this, Node, -1, "Node not init");
        throw InvalidOperationException("Node not init");
    }
    return m_Discovery->FindServiceByType(servicetype, transportschemes);
}

std::vector<NodeInfo2> RobotRaconteurNode::FindNodeByID(const RobotRaconteur::NodeID& id,
                                                        const std::vector<std::string>& transportschemes)
{
    ROBOTRACONTEUR_ASSERT_MULTITHREADED(shared_from_this());

    if (!m_Discovery)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(weak_this, Node, -1, "Node not init");
        throw InvalidOperationException("Node not init");
    }
    return m_Discovery->FindNodeByID(id, transportschemes);
}

void RobotRaconteurNode::AsyncFindNodeByID(const RobotRaconteur::NodeID& id,
                                           const std::vector<std::string>& transportschemes,
                                           boost::function<void(const RR_SHARED_PTR<std::vector<NodeInfo2> >&)> handler,
                                           int32_t timeout)
{
    if (!m_Discovery)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(weak_this, Node, -1, "Node not init");
        throw InvalidOperationException("Node not init");
    }
    m_Discovery->AsyncFindNodeByID(id, transportschemes, handler, timeout);
}

std::vector<NodeInfo2> RobotRaconteurNode::FindNodeByName(boost::string_ref name,
                                                          const std::vector<std::string>& transportschemes)
{
    ROBOTRACONTEUR_ASSERT_MULTITHREADED(shared_from_this());

    if (!m_Discovery)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(weak_this, Node, -1, "Node not init");
        throw InvalidOperationException("Node not init");
    }
    return m_Discovery->FindNodeByName(name, transportschemes);
}

void RobotRaconteurNode::AsyncFindNodeByName(
    boost::string_ref name, const std::vector<std::string>& transportschemes,
    boost::function<void(const RR_SHARED_PTR<std::vector<NodeInfo2> >&)> handler, int32_t timeout)
{
    if (!m_Discovery)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(weak_this, Node, -1, "Node not init");
        throw InvalidOperationException("Node not init");
    }
    m_Discovery->AsyncFindNodeByName(name, transportschemes, handler, timeout);
}

std::string RobotRaconteurNode::RequestObjectLock(const RR_SHARED_PTR<RRObject>& obj,
                                                  RobotRaconteurObjectLockFlags flags)
{
    ROBOTRACONTEUR_ASSERT_MULTITHREADED(shared_from_this());

    if (!(dynamic_cast<ServiceStub*>(obj.get()) != 0))
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(weak_this, Node, -1, "Can only lock object opened through Robot Raconteur");
        throw InvalidArgumentException("Can only lock object opened through Robot Raconteur");
    }
    RR_SHARED_PTR<ServiceStub> s = rr_cast<ServiceStub>(obj);

    return s->GetContext()->RequestObjectLock(obj, flags);
}

std::string RobotRaconteurNode::ReleaseObjectLock(const RR_SHARED_PTR<RRObject>& obj)
{
    ROBOTRACONTEUR_ASSERT_MULTITHREADED(shared_from_this());

    if (!(dynamic_cast<ServiceStub*>(obj.get()) != 0))
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(weak_this, Node, -1,
                                           "Can only unlock object opened through Robot Raconteur");
        throw InvalidArgumentException("Can only unlock object opened through Robot Raconteur");
    }
    RR_SHARED_PTR<ServiceStub> s = rr_cast<ServiceStub>(obj);

    return s->GetContext()->ReleaseObjectLock(obj);
}

void RobotRaconteurNode::AsyncRequestObjectLock(
    const RR_SHARED_PTR<RRObject>& obj, RobotRaconteurObjectLockFlags flags,
    boost::function<void(const RR_SHARED_PTR<std::string>&, const RR_SHARED_PTR<RobotRaconteurException>&)> handler,
    int32_t timeout)
{
    RR_SHARED_PTR<ServiceStub> s = RR_DYNAMIC_POINTER_CAST<ServiceStub>(obj);
    if (!s)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(weak_this, Node, -1, "Can only lock object opened through Robot Raconteur");
        throw InvalidArgumentException("Can only lock object opened through Robot Raconteur");
    }
    s->GetContext()->AsyncRequestObjectLock(obj, flags, RR_MOVE(handler), timeout);
}

void RobotRaconteurNode::AsyncReleaseObjectLock(
    const RR_SHARED_PTR<RRObject>& obj,
    boost::function<void(const RR_SHARED_PTR<std::string>&, const RR_SHARED_PTR<RobotRaconteurException>&)> handler,
    int32_t timeout)
{
    RR_SHARED_PTR<ServiceStub> s = RR_DYNAMIC_POINTER_CAST<ServiceStub>(obj);
    if (!s)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(weak_this, Node, -1, "Can only lock object opened through Robot Raconteur");
        throw InvalidArgumentException("Can only unlock object opened through Robot Raconteur");
    }
    s->GetContext()->AsyncReleaseObjectLock(obj, RR_MOVE(handler), timeout);
}

void RobotRaconteurNode::MonitorEnter(const RR_SHARED_PTR<RRObject>& obj, int32_t timeout)
{
    ROBOTRACONTEUR_ASSERT_MULTITHREADED(shared_from_this());

    RR_SHARED_PTR<ServiceStub> s = RR_DYNAMIC_POINTER_CAST<ServiceStub>(obj);
    if (!s)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(weak_this, Node, -1,
                                           "Only service stubs can be monitored by RobotRaconteurNode");
        throw InvalidArgumentException("Only service stubs can be monitored by RobotRaconteurNode");
    }
    s->GetContext()->MonitorEnter(obj, timeout);
}

void RobotRaconteurNode::MonitorExit(const RR_SHARED_PTR<RRObject>& obj)
{
    ROBOTRACONTEUR_ASSERT_MULTITHREADED(shared_from_this());

    RR_SHARED_PTR<ServiceStub> s = RR_DYNAMIC_POINTER_CAST<ServiceStub>(obj);
    if (!s)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(weak_this, Node, -1,
                                           "Only service stubs can be monitored by RobotRaconteurNode");
        throw InvalidArgumentException("Only service stubs can be monitored by RobotRaconteurNode");
    }
    s->GetContext()->MonitorExit(obj);
}

RR_SHARED_PTR<RobotRaconteurNode> RobotRaconteurNode::ScopedMonitorLock::GetNode()
{
    RR_SHARED_PTR<RobotRaconteurNode> n = node.lock();
    if (!n)
        throw InvalidOperationException("Node has been released");
    return n;
}

RobotRaconteurNode::ScopedMonitorLock::ScopedMonitorLock(const RR_SHARED_PTR<RRObject>& obj, int32_t timeout)
{
    this->obj = obj;
    RR_SHARED_PTR<ServiceStub> s = RR_DYNAMIC_POINTER_CAST<ServiceStub>(obj);
    if (!s)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Node, -1, "Only service stubs can be monitored by RobotRaconteurNode");
        throw InvalidArgumentException("Only service stubs can be monitored by RobotRaconteurNode");
    }
    this->node = s->RRGetNode();
    this->GetNode()->MonitorEnter(obj, timeout);
    locked = true;
}

void RobotRaconteurNode::ScopedMonitorLock::lock(int32_t timeout)
{
    if (!obj)
        return;
    if (locked)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Node, -1, "Already locked");
        throw InvalidOperationException("Already locked");
    }
    this->GetNode()->MonitorEnter(obj, timeout);
    locked = true;
}

void RobotRaconteurNode::ScopedMonitorLock::unlock()
{
    if (!obj)
        return;
    if (!locked)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Node, -1, "Node locked");
        throw InvalidOperationException("Not locked");
    }
    this->GetNode()->MonitorExit(obj);
    locked = false;
}

void RobotRaconteurNode::ScopedMonitorLock::release() { obj.reset(); }

RobotRaconteurNode::ScopedMonitorLock::~ScopedMonitorLock()
{
    if (locked)
    {
        try
        {
            unlock();
        }
        catch (std::exception&)
        {}
    }
}

void RobotRaconteurNode::PeriodicCleanupTask(const TimerEvent& err)
{

    if (err.stopped)
        return;

    {
        boost::posix_time::ptime now = NowNodeTime();

        std::vector<RR_SHARED_PTR<Endpoint> > e;

        {
            boost::mutex::scoped_lock lock(endpoint_lock);
            boost::copy(endpoints | boost::adaptors::map_values, std::back_inserter(e));

            for (std::map<uint32_t, boost::posix_time::ptime>::iterator e = recent_endpoints.begin();
                 e != recent_endpoints.end();)
            {
                int32_t seconds = boost::numeric_cast<int32_t>((now - e->second).total_seconds());
                if (seconds > 300)
                {
                    recent_endpoints.erase(e++);
                }
                else
                {
                    e++;
                }
            }
        }

        BOOST_FOREACH (RR_SHARED_PTR<Endpoint>& ee, e)
        {
            try
            {
                ee->PeriodicCleanupTask();
            }
            catch (std::exception&)
            {}
        }

        std::vector<RR_SHARED_PTR<Transport> > c;
        {
            boost::shared_lock<boost::shared_mutex> lock(transport_lock);
            boost::copy(transports | boost::adaptors::map_values, std::back_inserter(c));
        }

        BOOST_FOREACH (RR_SHARED_PTR<Transport>& cc, c)
        {
            try
            {
                cc->PeriodicCleanupTask();
            }
            catch (std::exception&)
            {}
        }

        std::vector<RR_SHARED_PTR<ServerContext> > sc;
        {
            boost::shared_lock<boost::shared_mutex> lock(services_lock);
            boost::copy(services | boost::adaptors::map_values, std::back_inserter(sc));
        }

        BOOST_FOREACH (RR_SHARED_PTR<ServerContext>& scc, sc)
        {
            try
            {
                scc->PeriodicCleanupTask();
            }
            catch (std::exception&)
            {}
        }

        try
        {

            CleanDiscoveredNodes();
        }
        catch (std::exception&)
        {}

        std::vector<RR_SHARED_PTR<IPeriodicCleanupTask> > cleanobjs;
        {
            boost::mutex::scoped_lock lock(cleanupobjs_lock);
            boost::copy(cleanupobjs, std::back_inserter(cleanobjs));
        }
        BOOST_FOREACH (RR_SHARED_PTR<IPeriodicCleanupTask>& t, cleanobjs)
        {
            try
            {
                t->PeriodicCleanupTask();
            }
            catch (std::exception&)
            {}
        }
    }
}

void RobotRaconteurNode::AddPeriodicCleanupTask(const RR_SHARED_PTR<IPeriodicCleanupTask>& task)
{
    boost::mutex::scoped_lock lock(cleanupobjs_lock);

    cleanupobjs.push_back(task);
}

void RobotRaconteurNode::RemovePeriodicCleanupTask(const RR_SHARED_PTR<IPeriodicCleanupTask>& task)
{
    boost::mutex::scoped_lock lock(cleanupobjs_lock);
    cleanupobjs.remove(task);
}

RR_SHARED_PTR<RRObject> RobotRaconteurNode::FindObjRefTyped(const RR_SHARED_PTR<RRObject>& obj,
                                                            boost::string_ref objref, boost::string_ref objecttype)
{
    ROBOTRACONTEUR_ASSERT_MULTITHREADED(shared_from_this());

    RR_SHARED_PTR<ServiceStub> s = RR_DYNAMIC_POINTER_CAST<ServiceStub>(obj);
    if (!s)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(weak_this, Node, -1, "Only service stubs can be have objrefs");
        throw InvalidArgumentException("Only service stubs can be have objrefs");
    }
    return s->FindObjRefTyped(objref, objecttype);
}

RR_SHARED_PTR<RRObject> RobotRaconteurNode::FindObjRefTyped(const RR_SHARED_PTR<RRObject>& obj,
                                                            boost::string_ref objref, boost::string_ref index,
                                                            boost::string_ref objecttype)
{
    ROBOTRACONTEUR_ASSERT_MULTITHREADED(shared_from_this());

    RR_SHARED_PTR<ServiceStub> s = RR_DYNAMIC_POINTER_CAST<ServiceStub>(obj);
    if (!s)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(weak_this, Node, -1, "Only service stubs can be have objrefs");
        throw InvalidArgumentException("Only service stubs can be have objrefs");
    }
    return s->FindObjRefTyped(objref, index, objecttype);
}

void RobotRaconteurNode::AsyncFindObjRefTyped(
    const RR_SHARED_PTR<RRObject>& obj, boost::string_ref objref, boost::string_ref objecttype,
    boost::function<void(const RR_SHARED_PTR<RRObject>&, const RR_SHARED_PTR<RobotRaconteurException>&)> handler,
    int32_t timeout)
{
    RR_SHARED_PTR<ServiceStub> s = RR_DYNAMIC_POINTER_CAST<ServiceStub>(obj);
    if (!s)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(weak_this, Node, -1, "Only service stubs can be have objrefs");
        throw InvalidArgumentException("Only service stubs can be have objrefs");
    }
    s->AsyncFindObjRefTyped(objref, objecttype, RR_MOVE(handler), timeout);
}

void RobotRaconteurNode::AsyncFindObjRefTyped(
    const RR_SHARED_PTR<RRObject>& obj, boost::string_ref objref, boost::string_ref index, boost::string_ref objecttype,
    boost::function<void(const RR_SHARED_PTR<RRObject>&, const RR_SHARED_PTR<RobotRaconteurException>&)> handler,
    int32_t timeout)
{
    RR_SHARED_PTR<ServiceStub> s = RR_DYNAMIC_POINTER_CAST<ServiceStub>(obj);
    if (!s)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(weak_this, Node, -1, "Only service stubs can be have objrefs");
        throw InvalidArgumentException("Only service stubs can be have objrefs");
    }
    s->AsyncFindObjRefTyped(objref, index, objecttype, RR_MOVE(handler), timeout);
}

std::string RobotRaconteurNode::FindObjectType(const RR_SHARED_PTR<RRObject>& obj, boost::string_ref objref)
{

    ROBOTRACONTEUR_ASSERT_MULTITHREADED(shared_from_this());

    RR_SHARED_PTR<ServiceStub> s = RR_DYNAMIC_POINTER_CAST<ServiceStub>(obj);
    if (!s)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(weak_this, Node, -1, "Only service stubs can be have objrefs");
        throw InvalidArgumentException("Only service stubs can be have objrefs");
    }
    return s->FindObjectType(objref);
}

std::string RobotRaconteurNode::FindObjectType(const RR_SHARED_PTR<RRObject>& obj, boost::string_ref objref,
                                               boost::string_ref index)
{
    ROBOTRACONTEUR_ASSERT_MULTITHREADED(shared_from_this());

    RR_SHARED_PTR<ServiceStub> s = RR_DYNAMIC_POINTER_CAST<ServiceStub>(obj);
    if (!s)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(weak_this, Node, -1, "Only service stubs can be have objrefs");
        throw InvalidArgumentException("Only service stubs can be have objrefs");
    }
    return s->FindObjectType(objref, index);
}

void RobotRaconteurNode::AsyncFindObjectType(
    const RR_SHARED_PTR<RRObject>& obj, boost::string_ref objref,
    boost::function<void(const RR_SHARED_PTR<std::string>&, const RR_SHARED_PTR<RobotRaconteurException>&)> handler,
    int32_t timeout)
{
    RR_SHARED_PTR<ServiceStub> s = RR_DYNAMIC_POINTER_CAST<ServiceStub>(obj);
    if (!s)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(weak_this, Node, -1, "Only service stubs can be have objrefs");
        throw InvalidArgumentException("Only service stubs can be have objrefs");
    }
    s->AsyncFindObjectType(objref, RR_MOVE(handler), timeout);
}

void RobotRaconteurNode::AsyncFindObjectType(
    const RR_SHARED_PTR<RRObject>& obj, boost::string_ref objref, boost::string_ref index,
    boost::function<void(const RR_SHARED_PTR<std::string>&, const RR_SHARED_PTR<RobotRaconteurException>&)> handler,
    int32_t timeout)
{
    RR_SHARED_PTR<ServiceStub> s = RR_DYNAMIC_POINTER_CAST<ServiceStub>(obj);
    if (!s)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(weak_this, Node, -1, "Only service stubs can be have objrefs");
        throw InvalidArgumentException("Only service stubs can be have objrefs");
    }
    s->AsyncFindObjectType(objref, index, RR_MOVE(handler), timeout);
}

std::vector<std::string> RobotRaconteurNode::GetPulledServiceTypes(const RR_SHARED_PTR<RRObject>& obj)
{
    RR_SHARED_PTR<ServiceStub> s = RR_DYNAMIC_POINTER_CAST<ServiceStub>(obj);
    if (!s)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(weak_this, Node, -1, "Only service stubs can be have objrefs");
        throw InvalidArgumentException("Only service stubs can be have objrefs");
    }
    return s->GetContext()->GetPulledServiceTypes();
}

RR_SHARED_PTR<ServiceFactory> RobotRaconteurNode::GetPulledServiceType(const RR_SHARED_PTR<RRObject>& obj,
                                                                       boost::string_ref type)
{
    RR_SHARED_PTR<ServiceStub> s = RR_DYNAMIC_POINTER_CAST<ServiceStub>(obj);
    if (!s)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(weak_this, Node, -1, "Only service stubs can be have objrefs");
        throw InvalidArgumentException("Only service stubs can be have objrefs");
    }
    return s->GetContext()->GetPulledServiceType(type);
}

void RobotRaconteurNode::StartPeriodicCleanupTask(const RR_SHARED_PTR<RobotRaconteurNode>& node)
{
    boost::unique_lock<boost::shared_mutex> lock(node->PeriodicCleanupTask_timer_lock);
    node->PeriodicCleanupTask_timer =
        node->CreateTimer(boost::posix_time::seconds(5),
                          boost::bind(&RobotRaconteurNode::PeriodicCleanupTask, node, RR_BOOST_PLACEHOLDERS(_1)));
    node->PeriodicCleanupTask_timer->Start();
}

RR_SHARED_PTR<ThreadPool> RobotRaconteurNode::GetThreadPool()
{
    {
        boost::shared_lock<boost::shared_mutex> lock(thread_pool_lock);
        if (thread_pool)
            return thread_pool;
    }

    InitThreadPool(20);

    {
        boost::unique_lock<boost::shared_mutex> lock(thread_pool_lock);
        if (!thread_pool)
        {
            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(weak_this, Node, -1, "Could not initialize thread pool");
            throw InternalErrorException("Could not initialize thread pool");
        }

        return thread_pool;
    }
}

bool RobotRaconteurNode::TryGetThreadPool(RR_SHARED_PTR<ThreadPool>& pool)
{
    {
        boost::shared_lock<boost::shared_mutex> t_lock(thread_pool_lock);
        pool = thread_pool;
    }
    if (pool)
    {
        return true;
    }
    else
    {
        try
        {
            pool = GetThreadPool();
        }
        catch (std::exception&)
        {
            return false;
        }
    }

    return pool != NULL;
}

void RobotRaconteurNode::SetThreadPool(const RR_SHARED_PTR<ThreadPool>& pool)
{

    if (is_shutdown)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(weak_this, Node, -1, "Attempt to set thread pool after node shutdown");
        throw InvalidOperationException("Node has been shutdown");
    }
    boost::unique_lock<boost::shared_mutex> lock(thread_pool_lock);
    if (thread_pool)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(weak_this, Node, -1, "Thread pool already set");
        throw InvalidOperationException("Thread pool already set");
    }
    thread_pool = pool;

    lock.unlock();

    boost::unique_lock<boost::shared_mutex> lock2(PeriodicCleanupTask_timer_lock);
    if (!PeriodicCleanupTask_timerstarted)
    {
        PeriodicCleanupTask_timerstarted = true;
        thread_pool->Post(boost::bind(&StartPeriodicCleanupTask, shared_from_this()));
    }

    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(weak_this, Node, -1, "ThreadPool set");
}

RR_SHARED_PTR<ThreadPoolFactory> RobotRaconteurNode::GetThreadPoolFactory()
{

    boost::mutex::scoped_lock lock(thread_pool_factory_lock);

    if (!thread_pool && !thread_pool_factory)
    {
        thread_pool_factory = RR_MAKE_SHARED<ThreadPoolFactory>();
    }

    if (!thread_pool_factory)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(weak_this, Node, -1, "ThreadPool set but ThreadPoolFactory not configured");
        throw InvalidOperationException("ThreadPool set but ThreadPoolFactory not configured");
    }

    return thread_pool_factory;
}

void RobotRaconteurNode::SetThreadPoolFactory(const RR_SHARED_PTR<ThreadPoolFactory>& factory)
{

    boost::mutex::scoped_lock lock(thread_pool_factory_lock);
    if (thread_pool_factory)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(weak_this, Node, -1, "Thread pool already set");
        throw InvalidOperationException("Thread pool already set");
    }
    thread_pool_factory = factory;

    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(weak_this, Node, -1, "ThreadPoolFactory set");
}

int32_t RobotRaconteurNode::GetThreadPoolCount()
{
    return boost::numeric_cast<int32_t>(GetThreadPool()->GetThreadPoolCount());
}

void RobotRaconteurNode::SetThreadPoolCount(int32_t count)
{
    InitThreadPool(count);
    GetThreadPool()->SetThreadPoolCount(count);
}

bool RobotRaconteurNode::InitThreadPool(int32_t thread_count)
{
    boost::unique_lock<boost::shared_mutex> lock(thread_pool_lock);
    if (thread_pool)
        return false;

    if (is_shutdown)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(weak_this, Node, -1,
                                           "Attempt to initialize thread pool after node shutdown");
        throw InvalidOperationException("Node has been shutdown");
    }

    thread_pool = GetThreadPoolFactory()->NewThreadPool(shared_from_this());
    thread_pool->SetThreadPoolCount(thread_count);

    if (!PeriodicCleanupTask_timerstarted)
    {
        PeriodicCleanupTask_timerstarted = true;
        thread_pool->Post(boost::bind(&StartPeriodicCleanupTask, shared_from_this()));
    }

    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(weak_this, Node, -1,
                                       "ThreadPool initialized with " << thread_count << " threads");

    return true;
}

void RobotRaconteurNode::SetExceptionHandler(boost::function<void(const std::exception*)> handler)
{
    boost::mutex::scoped_lock lock(exception_handler_lock);
    exception_handler = RR_MOVE(handler);
}

boost::function<void(const std::exception*)> RobotRaconteurNode::GetExceptionHandler()
{
    boost::mutex::scoped_lock lock(exception_handler_lock);
    return exception_handler;
}

void RobotRaconteurNode::HandleException(const std::exception* exp)
{
    if (exp == NULL)
        return;

    boost::function<void(const std::exception*)> h;
    {
        boost::mutex::scoped_lock lock(exception_handler_lock);
        h = exception_handler;
    }

    if (h)
    {
        try
        {
            h(exp);
        }
        catch (...)
        {}
    }
    else
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(weak_this, Node, -1,
                                           "Uncaught exception in unknown handler: " << exp->what())
    }
}

bool RobotRaconteurNode::TryHandleException(RR_WEAK_PTR<RobotRaconteurNode> node, const std::exception* exp)
{
    RR_SHARED_PTR<RobotRaconteurNode> node1 = node.lock();
    if (!node1)
        return false;
    node1->HandleException(exp);
    return true;
}

boost::posix_time::ptime RobotRaconteurNode::NowUTC()
{
    boost::shared_lock<boost::shared_mutex> lock(time_provider_lock);

    RR_SHARED_PTR<ITransportTimeProvider> t = time_provider.lock();

    if (t)
    {
        return t->NowUTC();
    }
    else
    {
        return boost::posix_time::microsec_clock::universal_time();
    }
}

TimeSpec RobotRaconteurNode::NowTimeSpec()
{
    boost::shared_lock<boost::shared_mutex> lock(time_provider_lock);

    RR_SHARED_PTR<ITransportTimeProvider> t = time_provider.lock();

    if (t)
    {
        return t->NowTimeSpec();
    }
    else
    {
        boost::chrono::nanoseconds node_time = boost::chrono::duration_cast<boost::chrono::nanoseconds>(
            boost::chrono::steady_clock::now() - node_internal_start_time);
        TimeSpec ts1 = node_sync_timespec;
        ts1.seconds += node_time.count() / 1000000000;
        ts1.nanoseconds += boost::numeric_cast<int32_t>(node_time.count() % 1000000000);
        ts1.cleanup_nanosecs();
        return ts1;
    }
}

boost::posix_time::ptime RobotRaconteurNode::NowNodeTime()
{
    boost::shared_lock<boost::shared_mutex> lock(time_provider_lock);

    RR_SHARED_PTR<ITransportTimeProvider> t = time_provider.lock();

    if (t)
    {
        return t->NowNodeTime();
    }
    else
    {
        boost::chrono::microseconds node_time = boost::chrono::duration_cast<boost::chrono::microseconds>(
            boost::chrono::steady_clock::now() - node_internal_start_time);
        return node_sync_time + boost::posix_time::microseconds(node_time.count());
    }
}

boost::posix_time::ptime RobotRaconteurNode::NodeSyncTimeUTC()
{
    boost::shared_lock<boost::shared_mutex> lock(time_provider_lock);

    RR_SHARED_PTR<ITransportTimeProvider> t = time_provider.lock();

    if (t)
    {
        return t->NodeSyncTimeUTC();
    }
    else
    {
        return node_sync_time;
    }
}

TimeSpec RobotRaconteurNode::NodeSyncTimeSpec()
{
    boost::shared_lock<boost::shared_mutex> lock(time_provider_lock);

    RR_SHARED_PTR<ITransportTimeProvider> t = time_provider.lock();

    if (t)
    {
        return t->NodeSyncTimeSpec();
    }
    else
    {
        return node_sync_timespec;
    }
}

RR_SHARED_PTR<Timer> RobotRaconteurNode::CreateTimer(const boost::posix_time::time_duration& period,
                                                     boost::function<void(const TimerEvent&)> handler, bool oneshot)
{

    boost::shared_lock<boost::shared_mutex> lock(time_provider_lock);
    RR_SHARED_PTR<ITransportTimeProvider> t = time_provider.lock();
    if (!t)
    {
        RR_SHARED_PTR<Timer> timer = RR_MAKE_SHARED<WallTimer>(period, handler, oneshot, shared_from_this());
        ROBOTRACONTEUR_LOG_TRACE_COMPONENT(weak_this, Node, -1, "Timer created using WallTimer");
        return timer;
    }
    else
    {
        RR_SHARED_PTR<Timer> timer = t->CreateTimer(period, handler, oneshot);
        ROBOTRACONTEUR_LOG_TRACE_COMPONENT(weak_this, Node, -1, "Timer created using transport provider");
        return timer;
    }
}

RR_SHARED_PTR<Rate> RobotRaconteurNode::CreateRate(double frequency)
{
    boost::shared_lock<boost::shared_mutex> lock(time_provider_lock);
    RR_SHARED_PTR<ITransportTimeProvider> t = time_provider.lock();
    if (!t)
    {
        RR_SHARED_PTR<Rate> rate = RR_MAKE_SHARED<WallRate>(frequency, shared_from_this());
        ROBOTRACONTEUR_LOG_TRACE_COMPONENT(weak_this, Node, -1, "Rate created using WallRate");
        return rate;
    }
    else
    {
        RR_SHARED_PTR<Rate> rate = t->CreateRate(frequency);
        ROBOTRACONTEUR_LOG_TRACE_COMPONENT(weak_this, Node, -1, "Rate created using transport provider");
        return rate;
    }
}

void RobotRaconteurNode::Sleep(const boost::posix_time::time_duration& duration)
{

    ROBOTRACONTEUR_ASSERT_MULTITHREADED(shared_from_this());

    RR_SHARED_PTR<ITransportTimeProvider> t;
    {
        boost::shared_lock<boost::shared_mutex> lock(time_provider_lock);
        t = time_provider.lock();
    }
    if (!t)
    {
        boost::this_thread::sleep(duration);
    }
    else
    {
        t->Sleep(duration);
    }
}

RR_SHARED_PTR<AutoResetEvent> RobotRaconteurNode::CreateAutoResetEvent()
{
    boost::shared_lock<boost::shared_mutex> lock(time_provider_lock);
    RR_SHARED_PTR<ITransportTimeProvider> t = time_provider.lock();
    if (!t)
    {
        RR_SHARED_PTR<AutoResetEvent> evt = RR_MAKE_SHARED<AutoResetEvent>();
        ROBOTRACONTEUR_LOG_TRACE_COMPONENT(weak_this, Node, -1, "AutoResetEvent created");
        return evt;
    }
    else
    {
        RR_SHARED_PTR<AutoResetEvent> evt = t->CreateAutoResetEvent();
        ROBOTRACONTEUR_LOG_TRACE_COMPONENT(weak_this, Node, -1, "AutoResetEvent created using transport provider");
        return evt;
    }
}

void RobotRaconteurNode::DownCastAndThrowException(RobotRaconteurException& exp)
{
    if (exp.ErrorCode != MessageErrorType_RemoteError)
    {
        RobotRaconteurExceptionUtil::DownCastAndThrowException(exp);
    }

    std::string type = exp.Error;
    if (!boost::contains(type, "."))
    {
        throw exp;
    }
    boost::tuple<boost::string_ref, boost::string_ref> stype = SplitQualifiedName(type);
    if (!IsServiceTypeRegistered(stype.get<0>()))
    {
        throw exp;
    }

    GetServiceType(stype.get<0>())->DownCastAndThrowException(exp);
}

RR_SHARED_PTR<RobotRaconteurException> RobotRaconteurNode::DownCastException(
    const RR_SHARED_PTR<RobotRaconteurException>& exp)
{
    if (!exp)
        return exp;

    if (exp->ErrorCode != MessageErrorType_RemoteError)
    {
        return RobotRaconteurExceptionUtil::DownCastException(exp);
    }

    std::string type = exp->Error;
    if (!boost::contains(type, "."))
    {
        return exp;
    }
    boost::tuple<boost::string_ref, boost::string_ref> stype = SplitQualifiedName(type);
    if (!IsServiceTypeRegistered(stype.get<0>()))
    {
        return exp;
    }

    return GetServiceType(stype.get<0>())->DownCastException(exp);
}

std::string RobotRaconteurNode::GetObjectType(const RR_SHARED_PTR<RRObject>& obj)
{
    if (!(dynamic_cast<ServiceStub*>(obj.get()) != 0))
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(weak_this, Node, -1, "obj is not a service stub");
        throw InvalidArgumentException("obj is not a service stub");
    }
    RR_SHARED_PTR<ServiceStub> s = rr_cast<ServiceStub>(obj);
    return s->RRType();
}

bool RobotRaconteurNode::IsEndpointLargeTransferAuthorized(uint32_t endpoint)
{
    try
    {
        RR_SHARED_PTR<Endpoint> e;
        {
            boost::mutex::scoped_lock lock(endpoint_lock);
            RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<Endpoint> >::iterator e1 = endpoints.find(endpoint);
            if (e1 == endpoints.end())
                return false;
            e = e1->second;
        }

        RR_SHARED_PTR<ClientContext> c = RR_DYNAMIC_POINTER_CAST<ClientContext>(e);
        if (c)
        {
            return true;
        }

        RR_SHARED_PTR<ServerEndpoint> s = RR_DYNAMIC_POINTER_CAST<ServerEndpoint>(e);
        if (s)
        {

            if (s->service->RequireValidUser())
            {
                return (bool)s->GetCurrentAuthenticatedUser();
            }
            else
            {
                return true;
            }

            return false;
        }

        return false;
    }
    catch (std::exception&)
    {
        return false;
    }
}

std::string RobotRaconteurNode::GetRobotRaconteurVersion() { return ROBOTRACONTEUR_VERSION_TEXT; }

std::string RobotRaconteurNode::GetRandomString(size_t count)
{
    std::string o;
    boost::mutex::scoped_lock lock(random_generator_lock);
    std::string strvals = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    boost::random::uniform_int_distribution<uint32_t> distribution(0,
                                                                   boost::numeric_cast<uint32_t>(strvals.size() - 1));
    for (size_t i = 0; i < count; i++)
    {
        o += strvals.at(distribution(*random_generator));
    }
    return o;
}

std::string RobotRaconteurNode::GetServiceStateNonce()
{
    boost::mutex::scoped_lock lock(service_state_nonce_lock);
    return service_state_nonce;
}

void RobotRaconteurNode::UpdateServiceStateNonce()
{
    std::string new_nonce;
    {
        boost::mutex::scoped_lock lock(service_state_nonce_lock);

        do
        {
            new_nonce = GetRandomString(16);
        } while (new_nonce == service_state_nonce);

        service_state_nonce = new_nonce;
        ROBOTRACONTEUR_LOG_TRACE_COMPONENT(weak_this, Node, -1,
                                           "Service state nonce updated to \"" << new_nonce << "\"");
    }

    {
        boost::mutex::scoped_lock lock(transports_lock);
        BOOST_FOREACH (RR_SHARED_PTR<Transport>& t, transports | boost::adaptors::map_values)
        {
            t->LocalNodeServicesChanged();
        }
    }
}

bool RobotRaconteurNode::CompareLogLevel(RobotRaconteur_LogLevel log_level) { return log_level >= this->log_level; }

void RobotRaconteurNode::LogMessage(RobotRaconteur_LogLevel level, const std::string& message)
{
    RRLogRecord r;
    r.Node = shared_from_this();
    r.Level = level;
    r.Component = RobotRaconteur_LogComponent_Default;
    r.Endpoint = 0;
    r.Message = message;

    LogRecord(r);
}

void RobotRaconteurNode::LogRecord(const RRLogRecord& record)
{
    {
        boost::shared_lock<boost::shared_mutex> lock(log_level_mutex);
        if (record.Level < log_level)
        {
            return;
        }
    }

    {
        boost::shared_lock<boost::shared_mutex> lock3(tap_lock);
        if (tap)
        {
            tap->RecordLogRecord(record);
        }
    }

    {
        boost::upgrade_lock<boost::shared_mutex> lock2(log_handler_mutex);

        if (log_handler)
        {
            log_handler->HandleLogRecord(record);
            return;
        }

        boost::upgrade_to_unique_lock<boost::shared_mutex> lock3(lock2);
        std::cerr << record << std::endl;
    }
}

RobotRaconteur_LogLevel RobotRaconteurNode::GetLogLevel()
{
    boost::shared_lock<boost::shared_mutex> lock(log_level_mutex);
    return log_level;
}
void RobotRaconteurNode::SetLogLevel(RobotRaconteur_LogLevel level)
{
    boost::unique_lock<boost::shared_mutex> lock(log_level_mutex);
    log_level = level;
}

RobotRaconteur_LogLevel RobotRaconteurNode::SetLogLevelFromEnvVariable(const std::string& env_variable_name)
{
    boost::unique_lock<boost::shared_mutex> lock(log_level_mutex);
    char* loglevel_c = std::getenv(env_variable_name.c_str());
    if (!loglevel_c)
        return RobotRaconteur_LogLevel_Warning;
    std::string loglevel(loglevel_c);
    lock.unlock();
    return SetLogLevelFromString(loglevel);
}

RobotRaconteur_LogLevel RobotRaconteurNode::SetLogLevelFromString(boost::string_ref loglevel)
{
    boost::unique_lock<boost::shared_mutex> lock(log_level_mutex);
    if (loglevel == "DISABLE")
    {
        log_level = RobotRaconteur_LogLevel_Disable;
        return RobotRaconteur_LogLevel_Disable;
    }

    if (loglevel == "FATAL")
    {
        log_level = RobotRaconteur_LogLevel_Fatal;
        return RobotRaconteur_LogLevel_Fatal;
    }

    if (loglevel == "ERROR")
    {
        log_level = RobotRaconteur_LogLevel_Error;
        return RobotRaconteur_LogLevel_Error;
    }

    if (loglevel == "WARNING")
    {
        log_level = RobotRaconteur_LogLevel_Warning;
        return RobotRaconteur_LogLevel_Warning;
    }

    if (loglevel == "INFO")
    {
        log_level = RobotRaconteur_LogLevel_Info;
        return RobotRaconteur_LogLevel_Info;
    }

    if (loglevel == "DEBUG")
    {
        log_level = RobotRaconteur_LogLevel_Debug;
        return RobotRaconteur_LogLevel_Debug;
    }

    if (loglevel == "TRACE")
    {
        log_level = RobotRaconteur_LogLevel_Trace;
        return RobotRaconteur_LogLevel_Trace;
    }

    lock.unlock();

    ROBOTRACONTEUR_LOG_WARNING_COMPONENT(
        weak_this, Node, -1, "Invalid log level specified in environmental variable or command line: " << loglevel);

    return log_level;
}

RR_SHARED_PTR<LogRecordHandler> RobotRaconteurNode::GetLogRecordHandler()
{
    boost::shared_lock<boost::shared_mutex> lock(log_handler_mutex);
    return log_handler;
}
void RobotRaconteurNode::SetLogRecordHandler(const RR_SHARED_PTR<LogRecordHandler>& handler)
{
    boost::unique_lock<boost::shared_mutex> lock(log_handler_mutex);
    log_handler = handler;
}

RR_SHARED_PTR<MessageTap> RobotRaconteurNode::GetMessageTap()
{
    boost::shared_lock<boost::shared_mutex> lock(tap_lock);
    return tap;
}

void RobotRaconteurNode::SetMessageTap(const RR_SHARED_PTR<MessageTap>& message_tap)
{
    boost::unique_lock<boost::shared_mutex> lock(tap_lock);
    tap = message_tap;
}

} // namespace RobotRaconteur
