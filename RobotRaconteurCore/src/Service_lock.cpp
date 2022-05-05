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

#include "Service_lock_private.h"

#include <boost/foreach.hpp>

namespace RobotRaconteur
{
ServerContext_ObjectLock::ServerContext_ObjectLock(boost::string_ref username,
                                                   const RR_SHARED_PTR<ServiceSkel>& root_skel, uint32_t endpoint)
{
    InitializeInstanceFields();

    {
        boost::mutex::scoped_lock lock(skels_lock);
        m_Locked = true;
        m_RootSkel = root_skel;
        m_Username = RR_MOVE(username.to_string());
        m_RootServicePath = root_skel->GetServicePath();
        m_Endpoint = endpoint;
    }
}

std::string ServerContext_ObjectLock::GetUsername() const { return m_Username; }

bool ServerContext_ObjectLock::IsLocked() const { return m_Locked; }

std::string ServerContext_ObjectLock::GetRootServicePath() const { return m_RootServicePath; }

uint32_t ServerContext_ObjectLock::GetEndpoint() const { return m_Endpoint; }

void ServerContext_ObjectLock::AddSkel(RR_SHARED_PTR<ServiceSkel> skel)
{

    {
        boost::mutex::scoped_lock lock(skels_lock);
        boost::mutex::scoped_lock lock2(skel->objectlock_lock);
        skel->objectlock = shared_from_this();
        std::string sp = skel->GetServicePath();
        if (sp == m_RootServicePath)
        {
            RR_SHARED_PTR<ServiceSkel> mr = m_RootSkel.lock();

            if (skel.get() != mr.get())
                return;

            try
            {
                if (mr != 0)
                {
                    if (mr == skel)
                    {
                        // boost::mutex::scoped_lock lock2(mr->objectlock_lock);
                        mr->objectlock = shared_from_this();
                    }
                    else
                    {
                        boost::mutex::scoped_lock lock2(mr->objectlock_lock);
                        mr->objectlock = shared_from_this();
                    }
                }
            }
            catch (std::exception&)
            {}
            mr = skel;
            return;
        }

        skels.push_back(skel);
    }
}

void ServerContext_ObjectLock::ReleaseSkel(RR_SHARED_PTR<ServiceSkel> skel)
{

    {
        boost::mutex::scoped_lock lock(skels_lock);
        try
        {
            boost::mutex::scoped_lock lock2(skel->objectlock_lock);
            skel->objectlock.reset();
        }
        catch (std::exception&)
        {}

        std::string sp = skel->GetServicePath();
        if (sp == m_RootServicePath)
        {
            try
            {
                RR_SHARED_PTR<ServiceSkel> mr = m_RootSkel.lock();
                if (mr != 0)
                {
                    boost::mutex::scoped_lock lock2(mr->objectlock_lock);
                    mr->objectlock.reset();
                }
            }
            catch (std::exception&)
            {}
            m_RootSkel.reset();
            return;
        }

        // skels.erase(std::remove(skels.begin(),skels.end(),skel_weak),skels.end());

        for (std::vector<RR_WEAK_PTR<ServiceSkel> >::iterator e = skels.begin(); e != skels.end();)
        {
            RR_SHARED_PTR<ServiceSkel> s = e->lock();
            if (!s)
            {
                e = skels.erase(e);
            }
            else
            {
                if (s == skel)
                {
                    e = skels.erase(e);
                }
                else
                {
                    e++;
                }
            }
        }
    }
}

void ServerContext_ObjectLock::ReleaseLock()
{

    {
        boost::mutex::scoped_lock lock(skels_lock);
        m_Locked = false;

        RR_SHARED_PTR<ServiceSkel> mr = m_RootSkel.lock();
        if (!mr)
            return;

        try
        {
            boost::mutex::scoped_lock lock2(mr->objectlock_lock);
            mr->objectlock.reset();
        }
        catch (std::exception&)
        {}

        BOOST_FOREACH (RR_WEAK_PTR<ServiceSkel> s, skels)
        {
            try
            {
                RR_SHARED_PTR<ServiceSkel> ss = s.lock();
                if (ss)
                    ss->objectlock.reset();
            }
            catch (std::exception&)
            {}
        }

        skels.clear();
    }
}

void ServerContext_ObjectLock::InitializeInstanceFields()
{
    m_Locked = true;
    m_Username.clear();

    m_Endpoint = 0;
    m_RootServicePath.clear();
}

uint32_t ServerContext_MonitorObjectSkel::GetLocalEndpoint() const { return local_endpoint; }

bool ServerContext_MonitorObjectSkel::IsLocked() const { return monitor_acquired; }

ServerContext_MonitorObjectSkel::ServerContext_MonitorObjectSkel(RR_SHARED_PTR<ServiceSkel> skel)
{
    InitializeInstanceFields();
    this->monitor_thread_event = skel->RRGetNode()->CreateAutoResetEvent();
    this->wait_event = skel->RRGetNode()->CreateAutoResetEvent();
    RR_SHARED_PTR<RRObject> obj = skel->GetUncastObject();
    if ((dynamic_cast<IRobotRaconteurMonitorObject*>(obj.get()) == 0))
    {
        throw InvalidArgumentException("Object is not monitor lockable");
    }

    this->obj = rr_cast<IRobotRaconteurMonitorObject>(obj);
    this->skel = skel;
}

std::string ServerContext_MonitorObjectSkel::MonitorEnter(uint32_t local_endpoint, int32_t timeout)
{

    this->timeout = timeout;
    this->local_endpoint = local_endpoint;
    // wait_event = AutoResetEvent();
    // monitor_thread_event = AutoResetEvent();
    maintain_lock = true;

    {
        RR_SHARED_PTR<ServiceSkel> s = skel.lock();
        if (!s)
            throw InvalidOperationException("Object lost");
        boost::mutex::scoped_lock lock(s->monitorlocks_lock);
        s->monitorlocks.insert(std::make_pair(local_endpoint, shared_from_this()));
    }
    RR_SHARED_PTR<ServiceSkel> s1 = skel.lock();
    if (!s1)
        throw InvalidOperationException("Object has been closed");
    RR_SHARED_PTR<ServerContext> c1 = s1->GetContext();
    c1->GetMonitorThreadPool()->Post(boost::bind(&ServerContext_MonitorObjectSkel::thread_func, shared_from_this()));

    if (detail::ThreadPool_IsNodeMultithreaded(s1->RRGetNodeWeak()))
    {
        wait_event->WaitOne(5000);
    }

    if (monitor_acquire_exception != 0)
    {
        maintain_lock = false;

        throw monitor_acquire_exception;
    }

    return (monitor_acquired ? "OK" : "Continue");
}

std::string ServerContext_MonitorObjectSkel::MonitorContinueEnter(uint32_t localendpoint)
{
    if (monitor_acquired)
        return "OK";

    if (monitor_acquire_exception != 0)
    {
        maintain_lock = false;
        throw monitor_acquire_exception;
    }

    RR_SHARED_PTR<ServiceSkel> s1 = skel.lock();
    if (!s1)
        throw InvalidOperationException("Object has been closed");
    if (detail::ThreadPool_IsNodeMultithreaded(s1->RRGetNodeWeak()))
    {
        wait_event->WaitOne(5000);
    }

    if (monitor_acquire_exception != 0)
    {
        maintain_lock = false;
        throw monitor_acquire_exception;
    }

    return (monitor_acquired ? "OK" : "Continue");
}

void ServerContext_MonitorObjectSkel::MonitorRefresh(uint32_t localendpoint) { monitor_thread_event->Set(); }

std::string ServerContext_MonitorObjectSkel::MonitorExit(uint32_t local_endpoint)
{

    maintain_lock = false;
    monitor_thread_event->Set();
    while (monitor_acquired)
    {
        monitor_thread_event->Set();
    }

    boost::recursive_mutex::scoped_lock(close_lock);

    return "OK";
}

void ServerContext_MonitorObjectSkel::Shutdown()
{
    maintain_lock = false;
    monitor_thread_event->Set();
}

void ServerContext_MonitorObjectSkel::thread_func()
{
    try
    {
        RR_SHARED_PTR<IRobotRaconteurMonitorObject> o = obj.lock();
        if (!o)
            return;
        RR_SHARED_PTR<ServiceSkel> s = skel.lock();
        if (!s)
            return;
        o->RobotRaconteurMonitorEnter(timeout);
        boost::mutex::scoped_lock lock2(s->monitorlocks_lock);
        monitor_acquired = true;
        s->monitorlock = shared_from_this();
    }
    catch (std::exception& e)
    {
        monitor_acquire_exception = RR_MAKE_SHARED<std::exception>(e);
        wait_event->Set();

        {
            RR_SHARED_PTR<ServiceSkel> s = skel.lock();
            if (!s)
                return;
            boost::mutex::scoped_lock lock(s->monitorlocks_lock);
            s->monitorlocks.erase(local_endpoint);
        }

        return;
    }

    try
    {
        wait_event->Set();

        while (maintain_lock)
        {
            if (!monitor_thread_event->WaitOne(30000))
            {
                // Timout occured in the lock
                maintain_lock = false;
            }
        }
    }
    catch (std::exception&)
    {}

    {

        {
            boost::recursive_mutex::scoped_lock(close_lock);
            {
                RR_SHARED_PTR<ServiceSkel> s = skel.lock();
                if (!s)
                {
                    wait_event->Set();
                    return;
                }
                boost::mutex::scoped_lock lock(s->monitorlocks_lock);
                s->monitorlocks.erase(local_endpoint);
            }
            try
            {

                RR_SHARED_PTR<IRobotRaconteurMonitorObject> o = obj.lock();
                if (!o)
                {
                    wait_event->Set();
                    return;
                }
                o->RobotRaconteurMonitorExit();
            }
            catch (std::exception& e)
            {
                monitor_acquire_exception = RR_MAKE_SHARED<std::exception>(e);
            }
            monitor_acquired = false;
            wait_event->Set();
        }
    }
}

void ServerContext_MonitorObjectSkel::InitializeInstanceFields()
{
    wait_started = false;
    local_endpoint = 0;
    timeout = 0;
    monitor_acquire_exception.reset();
    monitor_acquired = false;
    maintain_lock = false;
}

} // namespace RobotRaconteur