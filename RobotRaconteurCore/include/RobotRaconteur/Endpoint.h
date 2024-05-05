/**
 * @file Endpoint.h
 *
 * @author John Wason, PhD
 *
 * @copyright Copyright 2011-2020 Wason Technology, LLC
 *
 * @par License
 * Software License Agreement (Apache License)
 * @par
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * @par
 * http://www.apache.org/licenses/LICENSE-2.0
 * @par
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include "RobotRaconteur/NodeID.h"

#include "RobotRaconteur/Error.h"
#include "RobotRaconteur/DataTypes.h"
#include "RobotRaconteur/Message.h"
#include <boost/date_time.hpp>
#include <boost/atomic.hpp>

// Workaround for Windows headers
#ifdef SendMessage
#undef SendMessage
#endif

namespace RobotRaconteur
{

class ROBOTRACONTEUR_CORE_API RobotRaconteurNode;
class ROBOTRACONTEUR_CORE_API ITransportConnection;

class ROBOTRACONTEUR_CORE_API Endpoint : private boost::noncopyable
{
  private:
    boost::atomic<uint32_t> m_LocalEndpoint;
    boost::atomic<uint32_t> m_RemoteEndpoint;
    boost::shared_mutex m_RemoteNodeName_lock;
    std::string m_RemoteNodeName;
    boost::shared_mutex m_RemoteNodeID_lock;
    NodeID m_RemoteNodeID;
    boost::atomic<uint32_t> m_transport;

    boost::mutex m_TransportConnection_lock;
    RR_WEAK_PTR<ITransportConnection> m_TransportConnection;

    boost::atomic<boost::posix_time::ptime> m_LastMessageReceivedTime;
    boost::atomic<boost::posix_time::ptime> m_LastMessageSentTime;

    boost::atomic<uint16_t> MessageNumber;

  public:
    uint32_t GetLocalEndpoint();
    void SetLocalEndpoint(uint32_t endpoint);

    uint32_t GetRemoteEndpoint();
    void SetRemoteEndpoint(uint32_t endpoint);

    std::string GetRemoteNodeName();
    void SetRemoteNodeName(boost::string_ref name);

    NodeID GetRemoteNodeID();
    void SetRemoteNodeID(const NodeID& id);

    uint32_t GetTransport();
    void SetTransport(uint32_t transport);

    virtual RR_SHARED_PTR<ITransportConnection> GetTransportConnection();
    virtual void SetTransportConnection(const RR_SHARED_PTR<ITransportConnection>& c);

    boost::posix_time::ptime GetLastMessageReceivedTime();
    void SetLastMessageReceivedTime(boost::posix_time::ptime time);

    boost::posix_time::ptime GetLastMessageSentTime();
    void SetLastMessageSentTime(boost::posix_time::ptime time);

    virtual void SendMessage(const RR_INTRUSIVE_PTR<Message>& m);

    virtual void AsyncSendMessage(const RR_INTRUSIVE_PTR<Message>& m,
                                  const boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>&)>& callback);

    virtual void MessageReceived(const RR_INTRUSIVE_PTR<Message>& m) = 0;

    virtual void PeriodicCleanupTask();

    virtual void TransportConnectionClosed(uint32_t endpoint);

  protected:
    virtual void CheckEndpointCapabilityMessage(const RR_INTRUSIVE_PTR<Message>& m);

  public:
    virtual uint32_t EndpointCapability(boost::string_ref name);

    Endpoint(const RR_SHARED_PTR<RobotRaconteurNode>& node);

    virtual ~Endpoint() {}

    RR_SHARED_PTR<RobotRaconteurNode> GetNode();

  protected:
    RR_WEAK_PTR<RobotRaconteurNode> node;
};

#ifndef ROBOTRACONTEUR_NO_CXX11_TEMPLATE_ALIASES
using EndpointPtr = RR_SHARED_PTR<Endpoint>;
using EndpointConstPtr = RR_SHARED_PTR<const Endpoint>;
#endif

} // namespace RobotRaconteur
