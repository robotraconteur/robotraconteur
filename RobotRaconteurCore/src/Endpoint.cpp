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

#include "RobotRaconteur/Endpoint.h"
#include "RobotRaconteur/RobotRaconteurNode.h"
#include "RobotRaconteur/DataTypes.h"
#include "RobotRaconteur/ErrorUtil.h"

namespace RobotRaconteur
{

uint32_t Endpoint::GetLocalEndpoint() { return m_LocalEndpoint.load(); }

void Endpoint::SetLocalEndpoint(uint32_t endpoint) { m_LocalEndpoint.store(endpoint); }

uint32_t Endpoint::GetRemoteEndpoint() { return m_RemoteEndpoint.load(); }

void Endpoint::SetRemoteEndpoint(uint32_t endpoint) { m_RemoteEndpoint.store(endpoint); }

std::string Endpoint::GetRemoteNodeName()
{
    boost::shared_lock<boost::shared_mutex> lock(m_RemoteNodeName_lock);
    std::string ret = m_RemoteNodeName;
    return ret;
}

void Endpoint::SetRemoteNodeName(boost::string_ref name)
{
    boost::unique_lock<boost::shared_mutex> lock(m_RemoteNodeName_lock);
    m_RemoteNodeName = RR_MOVE(name.to_string());
}

NodeID Endpoint::GetRemoteNodeID()
{
    boost::shared_lock<boost::shared_mutex> lock(m_RemoteNodeID_lock);
    NodeID ret = m_RemoteNodeID;
    return ret;
}

void Endpoint::SetRemoteNodeID(const NodeID& id)
{
    boost::unique_lock<boost::shared_mutex> lock(m_RemoteNodeName_lock);
    m_RemoteNodeID = id;
}

uint32_t Endpoint::GetTransport() { return m_transport.load(); }

void Endpoint::SetTransport(uint32_t transport) { m_transport.store(transport); }

RR_SHARED_PTR<ITransportConnection> Endpoint::GetTransportConnection()
{
    boost::mutex::scoped_lock lock(m_TransportConnection_lock);
    return m_TransportConnection.lock();
}
void Endpoint::SetTransportConnection(const RR_SHARED_PTR<ITransportConnection>& c)
{
    boost::mutex::scoped_lock lock(m_TransportConnection_lock);
    m_TransportConnection = c;
}

boost::posix_time::ptime Endpoint::GetLastMessageReceivedTime() { return m_LastMessageReceivedTime.load(); }

void Endpoint::SetLastMessageReceivedTime(boost::posix_time::ptime time) { m_LastMessageReceivedTime.store(time); }

boost::posix_time::ptime Endpoint::GetLastMessageSentTime() { return m_LastMessageSentTime.load(); }

void Endpoint::SetLastMessageSentTime(boost::posix_time::ptime time) { m_LastMessageSentTime.store(time); }

void Endpoint::AsyncSendMessage(const RR_INTRUSIVE_PTR<Message>& m,
                                const boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>&)>& callback)
{
    if (!m->header)
        m->header = CreateMessageHeader();

    if (m->entries.size() == 1 && m->entries.at(0)->EntryType <= 500)
    {
        m->header->ReceiverNodeName = GetRemoteNodeName();
        m->header->SenderNodeName = GetNode()->NodeName();
    }
    m->header->SenderEndpoint = GetLocalEndpoint();
    m->header->ReceiverEndpoint = GetRemoteEndpoint();

    m->header->SenderNodeID = GetNode()->NodeID();
    m->header->ReceiverNodeID = GetRemoteNodeID();

    {
        m->header->MessageID = MessageNumber.load();

        MessageNumber = static_cast<uint16_t>(
            (MessageNumber == (static_cast<uint16_t>(std::numeric_limits<uint16_t>::max()))) ? 0 : MessageNumber + 1);
    }

    SetLastMessageSentTime(boost::posix_time::microsec_clock::universal_time());

    GetNode()->AsyncSendMessage(m, callback);
}

void Endpoint::SendMessage(const RR_INTRUSIVE_PTR<Message>& m)
{
    if (!m->header)
        m->header = CreateMessageHeader();
    if (m->entries.size() == 1 && m->entries.at(0)->EntryType <= 500)
    {
        m->header->ReceiverNodeName = GetRemoteNodeName();
        m->header->SenderNodeName = GetNode()->NodeName();
    }
    m->header->SenderEndpoint = GetLocalEndpoint();
    m->header->ReceiverEndpoint = GetRemoteEndpoint();

    m->header->SenderNodeID = GetNode()->NodeID();
    m->header->ReceiverNodeID = GetRemoteNodeID();

    {
        m->header->MessageID = MessageNumber.load();

        MessageNumber = static_cast<uint16_t>(
            (MessageNumber == (static_cast<uint16_t>(std::numeric_limits<uint16_t>::max()))) ? 0 : MessageNumber + 1);
    }

    SetLastMessageSentTime(boost::posix_time::microsec_clock::universal_time());

    GetNode()->SendMessage(m);
}

void Endpoint::PeriodicCleanupTask() {}

void Endpoint::CheckEndpointCapabilityMessage(const RR_INTRUSIVE_PTR<Message>& m)
{
    uint32_t capability = 0;
    RR_INTRUSIVE_PTR<MessageEntry> e;

    RR_INTRUSIVE_PTR<Message> ret = CreateMessage();
    ret->header = CreateMessageHeader();
    ret->header->ReceiverNodeName = m->header->SenderNodeName;
    ret->header->SenderNodeName = GetNode()->NodeName();
    ret->header->ReceiverNodeID = m->header->SenderNodeID;
    ret->header->ReceiverEndpoint = m->header->SenderEndpoint;
    ret->header->SenderEndpoint = m->header->ReceiverEndpoint;
    ret->header->SenderNodeID = GetNode()->NodeID();

    RR_INTRUSIVE_PTR<MessageEntry> eret;

    try
    {
        eret = ret->AddEntry(MessageEntryType_EndpointCheckCapabilityRet, m->entries.at(0)->MemberName);
        if (m->entries.empty())
            throw InvalidArgumentException("");

        e = m->entries.at(0);

        eret->RequestID = e->RequestID;
        eret->ServicePath = e->ServicePath;

        if (e->EntryType != MessageEntryType_EndpointCheckCapability)
            throw InvalidArgumentException("");
        boost::string_ref name = e->MemberName.str();
        capability = EndpointCapability(name);

        eret->AddElement("return", ScalarToRRArray<uint32_t>(capability));
    }
    catch (std::exception& e)
    {
        RobotRaconteurExceptionUtil::ExceptionToMessageEntry(e, eret);
    }

    SendMessage(ret);
}

uint32_t Endpoint::EndpointCapability(boost::string_ref name)
{
    RR_UNUSED(name);
    return static_cast<uint32_t>(0);
}

Endpoint::Endpoint(const RR_SHARED_PTR<RobotRaconteurNode>& node)
    : m_LocalEndpoint(0), m_RemoteEndpoint(0), MessageNumber(0), m_transport(std::numeric_limits<uint32_t>::max())

{

    m_RemoteNodeName.clear();
    m_RemoteNodeID = NodeID::GetAny();
    // TransportConnection = 0;
    m_LastMessageReceivedTime.store(boost::posix_time::microsec_clock::universal_time());
    m_LastMessageSentTime.store(boost::posix_time::microsec_clock::universal_time());

    // MessageNumberLock = RR_MAKE_SHARED<Object>();
    this->node = node;
}

RR_SHARED_PTR<RobotRaconteurNode> Endpoint::GetNode()
{
    RR_SHARED_PTR<RobotRaconteurNode> n = node.lock();
    if (!n)
        throw InvalidOperationException("Node has been released");
    return n;
}

void Endpoint::TransportConnectionClosed(uint32_t endpoint) {}

} // namespace RobotRaconteur
