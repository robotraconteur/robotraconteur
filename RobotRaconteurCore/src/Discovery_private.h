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

#include "RobotRaconteur/RobotRaconteurNode.h"
#include "RobotRaconteur/RobotRaconteurServiceIndex.h"

#pragma once

namespace RobotRaconteur
{
class ROBOTRACONTEUR_CORE_API ServiceSubscription;

namespace detail
{

class Discovery_updateserviceinfo;

class Discovery_nodestorage
{
  public:
    boost::mutex this_lock;
    RR_SHARED_PTR<NodeDiscoveryInfo> info;
    RR_SHARED_PTR<std::vector<ServiceInfo2> > services;
    std::string last_update_nonce;
    boost::posix_time::ptime last_update_time;
    RR_WEAK_PTR<Discovery_updateserviceinfo> updater;
    std::deque<std::string> recent_service_nonce;
    boost::initialized<uint32_t> retry_count;
    boost::posix_time::ptime retry_window_start;
};

class Discovery : public RR_ENABLE_SHARED_FROM_THIS<Discovery>
{
  protected:
    RR_WEAK_PTR<RobotRaconteurNode> node;

    boost::initialized<uint32_t> max_DiscoveredNodes;
    std::map<std::string, RR_SHARED_PTR<Discovery_nodestorage> > m_DiscoveredNodes;
    boost::mutex m_DiscoveredNodes_lock;

    std::list<RR_WEAK_PTR<IServiceSubscription> > subscriptions;

    boost::initialized<bool> is_shutdown;

  public:
    friend class ::RobotRaconteur::ServiceSubscription;
    friend class ::RobotRaconteur::ServiceInfo2Subscription;

    Discovery(const RR_SHARED_PTR<RobotRaconteurNode>& node);

    std::vector<NodeDiscoveryInfo> GetDetectedNodes();

    NodeInfo2 GetDetectedNodeCacheInfo(const RobotRaconteur::NodeID& nodeid);

    bool TryGetDetectedNodeCacheInfo(const RobotRaconteur::NodeID& nodeid, NodeInfo2& nodeinfo2);

    void NodeAnnouncePacketReceived(boost::string_ref packet);

    void NodeDetected(const NodeDiscoveryInfo& info);

    void UpdateDetectedNodes(const std::vector<std::string>& schemes);

    void AsyncUpdateDetectedNodes(const std::vector<std::string>& schemes, boost::function<void()>& handler,
                                  int32_t timeout = 5000);

    uint32_t GetNodeDiscoveryMaxCacheCount();
    void SetNodeDiscoveryMaxCacheCount(uint32_t count);

    void CleanDiscoveredNodes();

    std::vector<ServiceInfo2> FindServiceByType(boost::string_ref servicetype,
                                                const std::vector<std::string>& transportschemes);

    void AsyncFindServiceByType(boost::string_ref servicetype, const std::vector<std::string>& transportschemes,
                                boost::function<void(const RR_SHARED_PTR<std::vector<ServiceInfo2> >&)>& handler,
                                int32_t timeout = 5000);

    std::vector<NodeInfo2> FindNodeByID(const RobotRaconteur::NodeID& id,
                                        const std::vector<std::string>& transportschemes);

    void AsyncFindNodeByID(const RobotRaconteur::NodeID& id, const std::vector<std::string>& transportschemes,
                           boost::function<void(const RR_SHARED_PTR<std::vector<NodeInfo2> >&)>& handler,
                           int32_t timeout = 5000);

    std::vector<NodeInfo2> FindNodeByName(boost::string_ref name, const std::vector<std::string>& transportschemes);

    void AsyncFindNodeByName(boost::string_ref name, const std::vector<std::string>& transportschemes,
                             boost::function<void(const RR_SHARED_PTR<std::vector<NodeInfo2> >&)>& handler,
                             int32_t timeout = 5000);

    RR_SHARED_PTR<RobotRaconteurNode> GetNode();

    RR_SHARED_PTR<ServiceSubscription> SubscribeService(
        const std::vector<std::string>& url, boost::string_ref username = "",
        const RR_INTRUSIVE_PTR<RRMap<std::string, RRValue> >& credentials =
            (RR_INTRUSIVE_PTR<RRMap<std::string, RRValue> >()),
        boost::string_ref objecttype = "");

    RR_SHARED_PTR<ServiceSubscription> SubscribeServiceByType(const std::vector<std::string>& service_types,
                                                              const RR_SHARED_PTR<ServiceSubscriptionFilter>& filter);

    RR_SHARED_PTR<ServiceInfo2Subscription> SubscribeServiceInfo2(
        const std::vector<std::string>& service_types, const RR_SHARED_PTR<ServiceSubscriptionFilter>& filter);

    void Shutdown();

  protected:
    void EndAsyncFindNodeByID(const RobotRaconteur::NodeID& id, const std::vector<std::string>& transportschemes,
                              const boost::function<void(const RR_SHARED_PTR<std::vector<NodeInfo2> >&)>& handler);

    void EndAsyncFindNodeByName(const std::string& name, const std::vector<std::string>& transportschemes,
                                const boost::function<void(const RR_SHARED_PTR<std::vector<NodeInfo2> >&)>& handler);

    void EndUpdateServiceInfo(const RR_SHARED_PTR<Discovery_nodestorage>& storage,
                              const RR_SHARED_PTR<std::vector<ServiceInfo2> >& info, boost::string_ref nonce,
                              const RR_SHARED_PTR<RobotRaconteurException>& err);

    void RetryUpdateServiceInfo(const RR_SHARED_PTR<Discovery_nodestorage>& storage);

    void SubscriptionClosed(const RR_SHARED_PTR<IServiceSubscription>& subscription);

    void DoSubscribe(const std::vector<std::string>& service_types,
                     const RR_SHARED_PTR<ServiceSubscriptionFilter>& filter,
                     const RR_SHARED_PTR<IServiceSubscription>& subscription);
};

class Discovery_updatediscoverednodes : public RR_ENABLE_SHARED_FROM_THIS<Discovery_updatediscoverednodes>
{
    boost::mutex active_lock;
    std::list<int32_t> active;
    int32_t active_count;
    boost::function<void()> handler;
    // boost::mutex searching_lock;
    bool searching;
    std::list<RR_SHARED_PTR<RobotRaconteurException> > errors;

    RR_SHARED_PTR<Timer> timeout_timer;
    boost::mutex timeout_timer_lock;

    boost::mutex work_lock;

    std::vector<std::string> schemes;

    RR_SHARED_PTR<RobotRaconteurNode> node;

  public:
    Discovery_updatediscoverednodes(const RR_SHARED_PTR<RobotRaconteurNode>& node);

    void timeout_timer_callback(const TimerEvent& e);

    void getdetectednodes_callback(const RR_SHARED_PTR<std::vector<NodeDiscoveryInfo> >& ret, int32_t key);

    void UpdateDiscoveredNodes(const std::vector<std::string>& schemes,
                               const std::vector<RR_SHARED_PTR<Transport> >& transports,
                               RR_MOVE_ARG(boost::function<void()>) handler, int32_t timeout);
};

class Discovery_findservicebytype : public RR_ENABLE_SHARED_FROM_THIS<Discovery_findservicebytype>
{
    boost::mutex active_lock;
    std::list<int32_t> active;
    int32_t active_count;
    boost::function<void(const RR_SHARED_PTR<std::vector<ServiceInfo2> >&)> handler;
    int32_t timeout;
    // boost::mutex searching_lock;
    bool searching;
    std::list<RR_SHARED_PTR<RobotRaconteurException> > errors;

    RR_SHARED_PTR<Timer> timeout_timer;
    boost::mutex timeout_timer_lock;

    boost::mutex ret_lock;
    RR_SHARED_PTR<std::vector<ServiceInfo2> > ret;

    std::string servicetype;

    RR_SHARED_PTR<RobotRaconteurNode> node;

    boost::recursive_mutex work_lock;

    std::vector<std::string> schemes;

  public:
    Discovery_findservicebytype(const RR_SHARED_PTR<RobotRaconteurNode>& node);

    void handle_error(const int32_t& key, const RR_SHARED_PTR<RobotRaconteurException>& err);

    void timeout_timer_callback(const TimerEvent& e);

    static void rr_empty_handler();

    void serviceinfo_callback(const RR_INTRUSIVE_PTR<MessageEntry>& ret1,
                              const RR_SHARED_PTR<RobotRaconteurException>& err,
                              const RR_SHARED_PTR<ServiceStub>& client, const std::string& url, int32_t key);

    void connect_callback(const RR_SHARED_PTR<RRObject>& client, const RR_SHARED_PTR<RobotRaconteurException>& err,
                          const std::string& url, int32_t key);

    void find2();

    void AsyncFindServiceByType(boost::string_ref servicetype, const std::vector<std::string>& schemes,
                                RR_MOVE_ARG(boost::function<void(const RR_SHARED_PTR<std::vector<ServiceInfo2> >&)>)
                                    handler,
                                int32_t timeout);
};

class Discovery_updateserviceinfo : public RR_ENABLE_SHARED_FROM_THIS<Discovery_updateserviceinfo>
{
  protected:
    boost::mutex this_lock;
    boost::function<void(const RR_SHARED_PTR<Discovery_nodestorage>&, const RR_SHARED_PTR<std::vector<ServiceInfo2> >&,
                         boost::string_ref, const RR_SHARED_PTR<RobotRaconteurException>&)>
        handler;
    RR_SHARED_PTR<Timer> timeout_timer;
    RR_SHARED_PTR<Discovery_nodestorage> storage;
    int32_t backoff;
    RR_SHARED_PTR<RRObject> client;
    RR_WEAK_PTR<RobotRaconteurNode> node;
    int32_t retry_count;
    NodeID remote_nodeid;
    std::string remote_nodename;
    std::string service_nonce;

  public:
    static void rr_empty_handler();

    Discovery_updateserviceinfo(RR_WEAK_PTR<RobotRaconteurNode> node);

    void handle_error(const RR_SHARED_PTR<RobotRaconteurException>& err);

    void serviceinfo_handler(const RR_INTRUSIVE_PTR<MessageEntry>& ret1,
                             const RR_SHARED_PTR<RobotRaconteurException>& err);

    void connect_handler(const RR_SHARED_PTR<RRObject>& client, const RR_SHARED_PTR<RobotRaconteurException>& err);

    void backoff_timer_handler(const TimerEvent& evt);

    void AsyncUpdateServiceInfo(
        const RR_SHARED_PTR<Discovery_nodestorage>& storage, boost::string_ref service_nonce,
        RR_MOVE_ARG(boost::function<void(const RR_SHARED_PTR<Discovery_nodestorage>&,
                                         const RR_SHARED_PTR<std::vector<ServiceInfo2> >&, boost::string_ref,
                                         const RR_SHARED_PTR<RobotRaconteurException>&)>) handler,
        int32_t extra_backoff = 0);
};

} // namespace detail
} // namespace RobotRaconteur