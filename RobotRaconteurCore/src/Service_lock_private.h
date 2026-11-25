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

#include "RobotRaconteur/Service.h"
#include <boost/atomic.hpp>

#pragma once

namespace RobotRaconteur
{
class ROBOTRACONTEUR_CORE_API ServerContext_ObjectLock : public RR_ENABLE_SHARED_FROM_THIS<ServerContext_ObjectLock>
{

  private:
    bool m_Locked;
    std::string m_Username;
    std::vector<RR_WEAK_PTR<ServiceSkel> > skels;
    RR_WEAK_PTR<ServiceSkel> m_RootSkel;
    uint32_t m_Endpoint;

    std::string m_RootServicePath;
    boost::mutex skels_lock;

  public:
    ServerContext_ObjectLock(boost::string_ref username, const RR_SHARED_PTR<ServiceSkel>& root_skel,
                             uint32_t endpoint = 0);

    std::string GetUsername() const;
    bool IsLocked() const;
    std::string GetRootServicePath() const;
    uint32_t GetEndpoint() const;

    void AddSkel(const RR_SHARED_PTR<ServiceSkel>& skel);

    void ReleaseSkel(const RR_SHARED_PTR<ServiceSkel>& skel);

    void ReleaseLock();
};

class ROBOTRACONTEUR_CORE_API ServerContext_MonitorObjectSkel
    : public RR_ENABLE_SHARED_FROM_THIS<ServerContext_MonitorObjectSkel>
{
  private:
    RR_SHARED_PTR<AutoResetEvent> wait_event;
    bool wait_started;
    RR_WEAK_PTR<IRobotRaconteurMonitorObject> obj;
    uint32_t local_endpoint;
    int32_t timeout;
    RR_SHARED_PTR<RobotRaconteurException> monitor_acquire_exception;
    // Coordination flags shared between the monitor-lock worker thread (thread_func) and the request
    // threads (MonitorEnter/MonitorExit/IsLocked); atomic to avoid a data race on the plain bool
    boost::atomic<bool> monitor_acquired;
    RR_SHARED_PTR<AutoResetEvent> monitor_thread_event;
    boost::atomic<bool> maintain_lock;
    RR_WEAK_PTR<ServiceSkel> skel;

  public:
    uint32_t GetLocalEndpoint() const;

    bool IsLocked() const;

    ServerContext_MonitorObjectSkel(const RR_SHARED_PTR<ServiceSkel>& skel);

    std::string MonitorEnter(uint32_t local_endpoint, int32_t timeout);

    std::string MonitorContinueEnter(uint32_t localendpoint);

    void MonitorRefresh(uint32_t localendpoint);

    std::string MonitorExit(uint32_t local_endpoint);

    void Shutdown();

  private:
    void thread_func();

    boost::mutex close_lock;
};

} // namespace RobotRaconteur
