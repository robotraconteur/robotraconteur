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

#pragma once

namespace RobotRaconteur
{
class ROBOTRACONTEUR_CORE_API IntraTransportConnection : public ITransportConnection,
                                                         public boost::enable_shared_from_this<IntraTransportConnection>
{
  public:
    IntraTransportConnection(const RR_SHARED_PTR<IntraTransport>& parent, bool server, uint32_t local_endpoint);

    virtual void MessageReceived(const RR_INTRUSIVE_PTR<Message>& m);

    virtual void AcceptMessage(const RR_INTRUSIVE_PTR<Message>& m);

    RR_OVIRTUAL void SendMessage(const RR_INTRUSIVE_PTR<Message>& m) RR_OVERRIDE;

    RR_OVIRTUAL void AsyncSendMessage(
        const RR_INTRUSIVE_PTR<Message>& m,
        const boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>&)>& handler) RR_OVERRIDE;

    RR_OVIRTUAL void Close() RR_OVERRIDE;

    RR_OVIRTUAL uint32_t GetLocalEndpoint() RR_OVERRIDE;

    RR_OVIRTUAL uint32_t GetRemoteEndpoint() RR_OVERRIDE;

    RR_OVIRTUAL NodeID GetRemoteNodeID() RR_OVERRIDE;

    RR_OVIRTUAL RR_SHARED_PTR<RobotRaconteurNode> GetNode() RR_OVERRIDE;

    RR_OVIRTUAL void CheckConnection(uint32_t endpoint) RR_OVERRIDE;

    RR_OVIRTUAL bool CheckCapabilityActive(uint32_t flag) RR_OVERRIDE;

    void SetPeer(const RR_SHARED_PTR<IntraTransportConnection>& peer);

    bool IsConnected();

    RR_OVIRTUAL RR_SHARED_PTR<Transport> GetTransport() RR_OVERRIDE;

  protected:
    static void ProcessNextRecvMessage(RR_WEAK_PTR<IntraTransportConnection> c);

    void SimpleAsyncEndSendMessage(const RR_SHARED_PTR<RobotRaconteurException>& err);

    void RemoteClose();

    static void RemoteClose1(RR_WEAK_PTR<IntraTransportConnection> c);

    bool server;

    RR_WEAK_PTR<IntraTransport> parent;
    RR_WEAK_PTR<RobotRaconteurNode> node;

    uint32_t m_RemoteEndpoint;
    uint32_t m_LocalEndpoint;
    NodeID RemoteNodeID;
    boost::shared_mutex RemoteNodeID_lock;

    boost::recursive_mutex close_lock;

    RR_WEAK_PTR<IntraTransportConnection> peer;
    RR_SHARED_PTR<IntraTransportConnection> peer_storage;

    boost::atomic<bool> connected;

    boost::mutex recv_queue_lock;
    std::list<RR_INTRUSIVE_PTR<Message> > recv_queue;
    bool recv_queue_post_requested;
};
} // namespace RobotRaconteur
