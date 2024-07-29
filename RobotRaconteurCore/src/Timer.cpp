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

#include "RobotRaconteur/Timer.h"
#include "RobotRaconteur/AutoResetEvent.h"
#include "RobotRaconteur/RobotRaconteurNode.h"

#ifdef ROBOTRACONTEUR_WINDOWS
#ifndef CREATE_WAITABLE_TIMER_HIGH_RESOLUTION
#define CREATE_WAITABLE_TIMER_HIGH_RESOLUTION 0x00000002
#endif
#endif

#ifdef ROBOTRACONTEUR_LINUX
// Taken from clock_nanosleep man page
#if _XOPEN_SOURCE >= 600 || _POSIX_C_SOURCE >= 200112L
#define ROBOTRACONTEUR_USE_CLOCK_NANOSLEEP
#endif
#endif

namespace RobotRaconteur
{

TimerEvent::TimerEvent() { stopped = false; }

void WallTimer::timer_handler(const boost::system::error_code& ec)
{
    TimerEvent ev;

    RR_SHARED_PTR<RobotRaconteurNode> n = node.lock();
    if (!n)
        throw InvalidOperationException("Node released");

    boost::function<void(const TimerEvent&)> h;

    {

        boost::mutex::scoped_lock lock(running_lock);

        if (ec)
        {
            running = false;
        }
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
        n->HandleException(&exp);
    }

    boost::mutex::scoped_lock lock(running_lock);
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

            timer->expires_at(last_time + period);
            RobotRaconteurNode::asio_async_wait(
                node, timer,
                boost::bind(&WallTimer::timer_handler, shared_from_this(), boost::asio::placeholders::error));
        }
    }
    else
    {
        running = false;
        timer.reset();
    }
}

WallTimer::WallTimer(const boost::posix_time::time_duration& period, boost::function<void(const TimerEvent&)> handler,
                     bool oneshot, const RR_SHARED_PTR<RobotRaconteurNode>& node)
{
    this->period = period;
    this->oneshot = oneshot;
    this->handler = RR_MOVE(handler);
    running = false;
    if (!node)
    {
        this->node = RobotRaconteurNode::sp();
    }
    else
    {
        this->node = node;
    }
}

void WallTimer::Start()
{
    boost::mutex::scoped_lock lock(running_lock);
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

    timer = RR_SHARED_PTR<boost::asio::deadline_timer>(
        new boost::asio::deadline_timer(n->GetThreadPool()->get_io_context()));

    timer->expires_at(last_time + period);
    if (!RobotRaconteurNode::asio_async_wait(
            node, timer, boost::bind(&WallTimer::timer_handler, shared_from_this(), boost::asio::placeholders::error)))
    {
        throw InvalidOperationException("Node released");
    }
    running = true;
}

void WallTimer::Stop()
{
    boost::mutex::scoped_lock lock(running_lock);
    if (!running)
        throw InvalidOperationException("Not running");

    boost::system::error_code ec;
    timer->cancel(ec);

    timer.reset();
    running = false;

    if (oneshot)
        handler.clear();
}

void WallTimer::TryStop()
{
    boost::mutex::scoped_lock lock(running_lock);
    if (!running)
        return;

    boost::system::error_code ec;
    timer->cancel(ec);

    timer.reset();
    running = false;

    if (oneshot)
        handler.clear();
}

boost::posix_time::time_duration WallTimer::GetPeriod()
{
    boost::mutex::scoped_lock lock(running_lock);
    return this->period;
}

void WallTimer::SetPeriod(const boost::posix_time::time_duration& period)
{
    boost::mutex::scoped_lock lock(running_lock);
    this->period = period;
}

bool WallTimer::IsRunning()
{
    boost::mutex::scoped_lock lock(running_lock);
    return running;
}

void WallTimer::Clear()
{
    boost::mutex::scoped_lock lock(running_lock);
    handler.clear();
}

// NOLINTBEGIN(cppcoreguidelines-pro-type-member-init)
WallRate::WallRate(double frequency, const RR_SHARED_PTR<RobotRaconteurNode>& node)
#ifndef ROBOTRACONTEUR_WINDOWS
    : timer(node->GetThreadPool()->get_io_context())
#endif
{
#ifdef ROBOTRACONTEUR_LINUX
    memset(&ts, 0, sizeof(ts));
#endif
    if (!node)
    {
        this->node = RobotRaconteurNode::sp();
    }
    else
    {
        this->node = node;
    }
    this->period = boost::posix_time::microseconds(boost::lexical_cast<int64_t>(1000000.0 / frequency));
#ifdef ROBOTRACONTEUR_WINDOWS
    HANDLE timer = CreateWaitableTimerExA(NULL, NULL, CREATE_WAITABLE_TIMER_HIGH_RESOLUTION, TIMER_ALL_ACCESS);
    if (timer == NULL)
    {
        // Try again without high resolution
        timer = CreateWaitableTimerExA(NULL, NULL, 0, TIMER_ALL_ACCESS);
    }

    if (timer == NULL)
    {
        ROBOTRACONTEUR_LOG_ERROR_COMPONENT(node, Node, -1, "Could not create waitable timer");
        throw SystemResourceException("Could not create waitable timer");
    }

    timer_handle = boost::shared_ptr<void>(timer, CloseHandle);
#endif
}
// NOLINTEND(cppcoreguidelines-pro-type-member-init)

void WallRate::Sleep()
{
    if (start_time.is_not_a_date_time())
    {
        RR_SHARED_PTR<RobotRaconteurNode> node2 = this->node.lock();
        if (!node2)
        {
            throw InvalidOperationException("Node released");
        }
        boost::posix_time::ptime p4 = node2->NowNodeTime();
        start_time = p4;
        last_time = p4;
#ifdef ROBOTRACONTEUR_USE_CLOCK_NANOSLEEP
        if (clock_gettime(CLOCK_MONOTONIC, &ts) < 0)
        {
            // This is very unlikely to happen
            ROBOTRACONTEUR_LOG_ERROR_COMPONENT(node, Node, -1,
                                               "Could not get monotonic clock time for WallRate::Sleep()");
            throw SystemResourceException("Could not get monotonic clock time");
        }
#endif
    }
#ifndef ROBOTRACONTEUR_USE_CLOCK_NANOSLEEP
    boost::posix_time::ptime p2 = last_time + period;
    RR_SHARED_PTR<RobotRaconteurNode> node1 = this->node.lock();
    if (!node1)
    {
        throw InvalidOperationException("Node released");
    }
    boost::posix_time::ptime p3 = node1->NowNodeTime();
    boost::posix_time::time_duration d = p2 - p3;
#endif
#ifdef ROBOTRACONTEUR_WINDOWS
    if (!d.is_negative())
    {
        HANDLE timer = timer_handle.get();
        LARGE_INTEGER due_time;
        memset(&due_time, 0, sizeof(due_time));
        due_time.QuadPart = -d.total_microseconds() * 10;
        if (SetWaitableTimer(timer, &due_time, 0, NULL, NULL, FALSE) != TRUE)
        {
            ROBOTRACONTEUR_LOG_ERROR_COMPONENT(node, Node, -1, "Could not set waitable timer for WallRate::Sleep()");
            throw SystemResourceException("Could not set waitable timer for WallRate::Sleep()");
        }
        WaitForSingleObject(timer, INFINITE);
    }
    last_time = p2;
#elif defined(ROBOTRACONTEUR_USE_CLOCK_NANOSLEEP)

    while (clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &ts, NULL) != 0)
        ;

    // Increment the timespec by the period
    ts.tv_nsec += period.total_nanoseconds();
    while (ts.tv_nsec >= 1000000000)
    {
        ts.tv_nsec -= 1000000000;
        ts.tv_sec++;
    }
#else
    // Use boost::asio::deadline_timer
    timer.expires_from_now(d);
    timer.wait();
    last_time = p2;
#endif
}

void HighResolutionSleep(const boost::posix_time::time_duration& duration)
{
#ifdef ROBOTRACONTEUR_WINDOWS
    HANDLE timer = CreateWaitableTimerExA(NULL, NULL, CREATE_WAITABLE_TIMER_HIGH_RESOLUTION, TIMER_ALL_ACCESS);
    if (timer == NULL)
    {
        // Try again without high resolution
        timer = CreateWaitableTimerExA(NULL, NULL, 0, TIMER_ALL_ACCESS);
    }

    if (timer == NULL)
    {
        ROBOTRACONTEUR_LOG_ERROR_COMPONENT(RobotRaconteurNode::weak_sp(), Node, -1, "Could not create waitable timer");
        throw SystemResourceException("Could not create waitable timer");
    }

    LARGE_INTEGER due_time;
    memset(&due_time, 0, sizeof(due_time));
    due_time.QuadPart = -duration.total_microseconds() * 10;
    if (SetWaitableTimer(timer, &due_time, 0, NULL, NULL, FALSE) != TRUE)
    {
        CloseHandle(timer);
        ROBOTRACONTEUR_LOG_ERROR_COMPONENT(RobotRaconteurNode::weak_sp(), Node, -1,
                                           "Could not set waitable timer for HighResolutionSleep()");
        throw SystemResourceException("Could not set waitable timer for HighResolutionSleep()");
    }
    WaitForSingleObject(timer, INFINITE);
    CloseHandle(timer);
#elif defined(ROBOTRACONTEUR_USE_CLOCK_NANOSLEEP)
    timespec ts; // NOLINT(cppcoreguidelines-pro-type-member-init)
    memset(&ts, 0, sizeof(ts));
    if (clock_gettime(CLOCK_MONOTONIC, &ts) < 0)
    {
        // This is very unlikely to happen
        ROBOTRACONTEUR_LOG_ERROR_COMPONENT(RobotRaconteurNode::weak_sp(), Node, -1,
                                           "Could not get monotonic clock time for HighResolutionSleep()");
        throw SystemResourceException("Could not get monotonic clock time");
    }

    ts.tv_nsec += duration.total_nanoseconds();
    while (ts.tv_nsec >= 1000000000)
    {
        ts.tv_nsec -= 1000000000;
        ts.tv_sec++;
    }

    while (clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &ts, NULL) != 0)
        ;

#else
    // Use select for high resolution sleep
    timeval tv;
    tv.tv_sec = duration.total_seconds();
    tv.tv_usec = duration.total_microseconds() % 1000000;
    select(0, NULL, NULL, NULL, &tv);
#endif
}

} // namespace RobotRaconteur
