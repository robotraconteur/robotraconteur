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

#include "RobotRaconteur/Subscription.h"
#include "RobotRaconteur/Discovery.h"
#include "Discovery_private.h"
#include "WireMember_private.h"
#include "PipeMember_private.h"

#pragma once

namespace RobotRaconteur
{
namespace detail
{
class ServiceSubscription_client : private boost::noncopyable
{
  public:
    NodeID nodeid;
    std::string nodename;
    std::string service_name;
    std::string service_type;
    std::vector<std::string> urls;

    RR_WEAK_PTR<RRObject> client;
    RR_SHARED_PTR<ServiceSubscription_retrytimer> retry_timer;
    boost::posix_time::ptime last_node_update;

    boost::initialized<bool> connecting;
    boost::initialized<uint32_t> error_count;

    std::string username;
    RR_INTRUSIVE_PTR<RRMap<std::string, RRValue> > credentials;

    boost::initialized<bool> claimed;

    ServiceSubscription_client();
};

class ServiceInfo2Subscription_client : private boost::noncopyable
{
  public:
    NodeID nodeid;
    std::string service_name;
    ServiceInfo2 service_info2;

    boost::posix_time::ptime last_node_update;
};

class ServiceSubscription_retrytimer : public RR_ENABLE_SHARED_FROM_THIS<ServiceSubscription_retrytimer>,
                                       private boost::noncopyable
{
  public:
    RR_SHARED_PTR<boost::asio::deadline_timer> timer;
    RR_WEAK_PTR<ServiceSubscription> parent;
    RR_WEAK_PTR<ServiceSubscription_client> c2;
    boost::mutex this_lock;
    boost::initialized<bool> cancelled;

    ServiceSubscription_retrytimer(RR_SHARED_PTR<ServiceSubscription> parent);

    void Start(RR_SHARED_PTR<ServiceSubscription_client> c2, uint32_t timeout);
    void Cancel();
    int64_t MillisecondsRemaining();

    static void timer_handler(RR_WEAK_PTR<ServiceSubscription_retrytimer> this_, const boost::system::error_code& ec);

    RR_WEAK_PTR<RobotRaconteurNode> node;
};

class WireSubscription_connection : public RR_ENABLE_SHARED_FROM_THIS<WireSubscription_connection>,
                                    public ::RobotRaconteur::WireConnectionBaseListener,
                                    private boost::noncopyable
{
  public:
    friend class ::RobotRaconteur::WireSubscriptionBase;
    friend class WireSubscription_send_iterator;

    WireSubscription_connection();
    void Init(RR_SHARED_PTR<WireSubscriptionBase> parent, RR_SHARED_PTR<RRObject> client);

    void ClientConnected1(RR_SHARED_PTR<ServiceStub> stub);

    void ClientConnected2(RR_SHARED_PTR<WireConnectionBase> connection, RR_SHARED_PTR<RobotRaconteurException> err);

    void WireConnectionClosed(RR_SHARED_PTR<WireConnectionBase> connection);
    void WireValueChanged(RR_SHARED_PTR<WireConnectionBase> connection, RR_INTRUSIVE_PTR<RRValue> value,
                          const TimeSpec& time);
    ~WireSubscription_connection();

    void SetOutValue(const RR_INTRUSIVE_PTR<RRValue>& value);

    void Close();
    void RetryConnect();

    void RetryConnect1(const TimerEvent& ev);

  protected:
    RR_WEAK_PTR<WireSubscriptionBase> parent;
    RR_WEAK_PTR<WireConnectionBase> connection;
    RR_WEAK_PTR<RRObject> client;
    RR_WEAK_PTR<RobotRaconteurNode> node;

    RR_SHARED_PTR<Timer> retry_timer;
    bool closed;
};

class PipeSubscription_connection : public RR_ENABLE_SHARED_FROM_THIS<PipeSubscription_connection>,
                                    public ::RobotRaconteur::PipeEndpointBaseListener
{
  public:
    friend class ::RobotRaconteur::PipeSubscriptionBase;
    friend class PipeSubscription_send_iterator;

    PipeSubscription_connection();
    void Init(RR_SHARED_PTR<PipeSubscriptionBase> parent, RR_SHARED_PTR<RRObject> client);

    void ClientConnected1(RR_SHARED_PTR<ServiceStub> stub);

    void ClientConnected2(RR_SHARED_PTR<PipeEndpointBase> connection, RR_SHARED_PTR<RobotRaconteurException> err);

    virtual void PipeEndpointClosed(RR_SHARED_PTR<PipeEndpointBase> endpoint);
    virtual void PipePacketReceived(RR_SHARED_PTR<PipeEndpointBase> endpoint,
                                    boost::function<bool(RR_INTRUSIVE_PTR<RRValue>&)> receive_packet_func);
    virtual void PipePacketAckReceived(RR_SHARED_PTR<PipeEndpointBase> endpoint, uint32_t pnum);

    // Call with PipeSubscription::this_lock locked
    bool DoSendPacket();
    // Call with PipeSubscription::this_lock locked
    void AsyncSendPacket(const RR_INTRUSIVE_PTR<RRValue>& packet);

    void RetryConnect();
    void RetryConnect1(const TimerEvent& ev);

    void Close();

    ~PipeSubscription_connection();

  protected:
    RR_WEAK_PTR<PipeSubscriptionBase> parent;
    RR_WEAK_PTR<PipeEndpointBase> connection;
    RR_WEAK_PTR<RRObject> client;
    RR_WEAK_PTR<RobotRaconteurNode> node;

    std::list<uint32_t> backlog;
    std::list<uint32_t> forward_backlog;
    boost::initialized<int32_t> active_send_count;
    std::list<int32_t> active_sends;

    boost::initialized<bool> send_copy_element;

    static void pipe_packet_send_handler(RR_WEAK_PTR<PipeSubscription_connection> connection, int32_t pnum,
                                         RR_SHARED_PTR<RobotRaconteurException> err, int32_t send_key);

    RR_SHARED_PTR<Timer> retry_timer;
};
} // namespace detail
} // namespace RobotRaconteur
