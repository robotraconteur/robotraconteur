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

#include "RobotRaconteur/Timer.h"
#include "RobotRaconteur/RobotRaconteurNode.h"

#include <boost/asio/placeholders.hpp>

#include <emscripten/html5.h>

namespace RobotRaconteur
{
TimerEvent::TimerEvent() { stopped = false; }

std::map<void*, RR_SHARED_PTR<WallTimer> > WallTimer::em_timers;

void WallTimer::node_shutdown(RobotRaconteurNode* node)
{
    for (std::map<void*, RR_SHARED_PTR<WallTimer> >::iterator e = em_timers.begin(); e != em_timers.end();)
    {
        RR_SHARED_PTR<RobotRaconteurNode> node1 = e->second->node.lock();
        if (!node1)
        {
            e = em_timers.erase(e);
            continue;
        }
        if (e->second->node.lock().get() == node)
        {
            emscripten_clear_timeout(e->second->em_timer);
            e = em_timers.erase(e);
            continue;
        }
        ++e;
    }
}

void timer_handler(void* userData)
{
    std::map<void*, RR_SHARED_PTR<WallTimer> >::iterator e = WallTimer::em_timers.find(userData);
    if (e == WallTimer::em_timers.end())
    {
        return;
    }

    const RR_SHARED_PTR<WallTimer> t = e->second;
    if (!t->running || t->oneshot)
    {
        WallTimer::em_timers.erase(e);
    }

    RR_WEAK_PTR<RobotRaconteurNode> node = t->node;
    try
    {
        t->timer_handler1();
    }
    catch (std::exception& exp)
    {
        RobotRaconteurNode::TryHandleException(node, &exp);
    }
}

void WallTimer::timer_handler1()
{
    TimerEvent ev;

    RR_SHARED_PTR<RobotRaconteurNode> n = node.lock();
    if (!n)
        throw InvalidOperationException("Node released");

    boost::function<void(const TimerEvent&)> h;

    {
        ev.stopped = !running;
        ev.last_expected = last_time;
        ev.last_real = actual_last_time;
        ev.current_expected = last_time + period;
        ev.current_real = n->NowNodeTime();
        h = handler;

        if (oneshot)
        {
            handler.clear();
        }

        if (oneshot)
        {
            running = false;
        }
    }

    try
    {
        if (h)
            h(ev);
    }
    catch (std::exception& exp)
    {
        RobotRaconteurNode::TryHandleException(node, &exp);
    }
    if (!oneshot)
    {
        if (running)
        {
            last_time = ev.current_expected;
            actual_last_time = ev.current_real;

            while (last_time + period < actual_last_time)
            {
                last_time += period;
            }
        }
    }
    else
    {
        em_timer.data() = 0;
    }
}

WallTimer::WallTimer(const boost::posix_time::time_duration& period, boost::function<void(const TimerEvent&)> handler,
                     bool oneshot, const RR_SHARED_PTR<RobotRaconteurNode>& node)
{
    this->period = period;
    this->oneshot = oneshot;
    this->handler = handler;
    running = false;
    if (!node)
        this->node = RobotRaconteurNode::weak_sp();
    this->node = node;
}

void WallTimer::Start()
{
    if (running)
        throw InvalidOperationException("Already running");

    if (!handler)
        throw InvalidOperationException("Timer has expired");

    RR_SHARED_PTR<RobotRaconteurNode> n = node.lock();
    if (!n)
        throw InvalidOperationException("Node released");

    start_time = n->NowNodeTime();
    last_time = start_time;
    actual_last_time = last_time;

    em_timer.data() = 0;

    if (oneshot)
    {
        em_timer.data() = emscripten_set_timeout(&timer_handler, period.total_milliseconds(), this);
    }
    else
    {
        em_timer.data() = emscripten_set_interval(&timer_handler, period.total_milliseconds(), this);
    }
    em_timers.insert(std::make_pair(this, shared_from_this()));
    running = true;
}

void WallTimer::TryStop()
{
    try
    {
        Stop();
    }
    catch (InvalidOperationException&)
    {}
}

void WallTimer::Stop()
{
    em_timers.erase(this);

    if (!running)
        throw InvalidOperationException("Not running");

    try
    {
        if (em_timer.data() != 0)
        {
            if (oneshot)
            {
                emscripten_clear_timeout(em_timer.data());
            }
            else
            {
                emscripten_clear_interval(em_timer.data());
            }
        }
    }
    catch (std::exception&)
    {}
    running = false;

    TimerEvent ev;
    ev.stopped = true;
    RR_SHARED_PTR<RobotRaconteurNode> n = node.lock();
    if (n)
    {
        RR_BOOST_ASIO_IO_CONTEXT ctx;
        ctx.post(boost::bind(handler, ev));
    }
    handler.clear();
}

boost::posix_time::time_duration WallTimer::GetPeriod() { return this->period; }

void WallTimer::SetPeriod(const boost::posix_time::time_duration& period)
{
    throw NotImplementedException("Not available on Pyodide");
}

bool WallTimer::IsRunning() { return running; }

void WallTimer::Clear() { handler.clear(); }

WallRate::WallRate(double frequency, const RR_SHARED_PTR<RobotRaconteurNode>& node) {}

void WallRate::Sleep() { throw std::runtime_error("Rate not supported for single threaded emscripten"); }
} // namespace RobotRaconteur