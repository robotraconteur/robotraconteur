/**
 * @file Timer.h
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

#include <boost/date_time.hpp>
#include "RobotRaconteur/DataTypes.h"
#include <boost/asio/deadline_timer.hpp>

#pragma once

namespace RobotRaconteur
{

/**
 * @brief Timer event structure
 *
 * Contains information about the state of the timer. Passed to the
 * callback on invocation.
 *
 */
struct ROBOTRACONTEUR_CORE_API TimerEvent
{
    /** @brief true if timer has been stopped */
    bool stopped;
    /** @brief The last expected callback invocation time */
    boost::posix_time::ptime last_expected;
    /** @brief The real last callback invocation time */
    boost::posix_time::ptime last_real;
    /** @brief The current expected invocation time */
    boost::posix_time::ptime current_expected;
    /** @brief The current invocation time */
    boost::posix_time::ptime current_real;
};

/**
 * @brief A timer to invoke a callback
 *
 * Timers invoke a callback at a specified rate. The timer
 * can either be one-short, or repeating.
 *
 * Use RobotRaconteurNode::CreateTimer() to create timers.
 *
 */
class ROBOTRACONTEUR_CORE_API Timer : private boost::noncopyable
{
  public:
    /**
     * @brief Start the timer
     *
     * Must be called after RobotRaconteurNode::CreateTimer()
     *
     */
    virtual void Start() = 0;

    /**
     * @brief Stop the timer
     *
     */
    virtual void Stop() = 0;

    /**
     * @brief Get the period of the timer
     *
     * @return boost::posix_time::time_duration
     */
    virtual boost::posix_time::time_duration GetPeriod() = 0;

    /**
     * @brief Set the period of the timer
     *
     * @param period
     */
    virtual void SetPeriod(const boost::posix_time::time_duration& period) = 0;

    /**
     * @brief Check if timer is running
     *
     * @return true
     * @return false
     */
    virtual bool IsRunning() = 0;

    /**
     * @brief Clear the timer
     *
     */
    virtual void Clear() = 0;

    virtual ~Timer() {}
};

class ROBOTRACONTEUR_CORE_API RobotRaconteurNode;

/**
 * @brief Rate to stabilize a loop
 *
 * Rate is used to stabilize the period of a loop. Use
 * RobotRaconteur::CreateRate() to create rates.
 *
 */
class ROBOTRACONTEUR_CORE_API Rate : private boost::noncopyable
{

  public:
    /**
     * @brief Sleep the calling thread until the current loop period expires
     *
     */
    virtual void Sleep() = 0;

    virtual ~Rate(){};
};

class ROBOTRACONTEUR_CORE_API WallRate : public Rate
{
  protected:
    RR_WEAK_PTR<RobotRaconteurNode> node;
    boost::posix_time::time_duration period;
    boost::posix_time::ptime start_time;
    boost::posix_time::ptime last_time;

    boost::asio::deadline_timer timer;

  public:
    WallRate(double frequency, RR_SHARED_PTR<RobotRaconteurNode> node = RR_SHARED_PTR<RobotRaconteurNode>());

    virtual void Sleep();

    virtual ~WallRate() {}
};

class ROBOTRACONTEUR_CORE_API WallTimer : public Timer, public RR_ENABLE_SHARED_FROM_THIS<WallTimer>
{
  protected:
    boost::posix_time::time_duration period;
    boost::posix_time::ptime start_time;
    boost::posix_time::ptime actual_last_time;
    boost::posix_time::ptime last_time;
    bool oneshot;

    bool running;
    boost::mutex running_lock;

    boost::function<void(const TimerEvent&)> handler;

    RR_SHARED_PTR<boost::asio::deadline_timer> timer;

    RR_WEAK_PTR<RobotRaconteurNode> node;

    void timer_handler(const boost::system::error_code& ec);

  public:
    WallTimer(const boost::posix_time::time_duration& period, boost::function<void(const TimerEvent&)> handler,
              bool oneshot, RR_SHARED_PTR<RobotRaconteurNode> node = RR_SHARED_PTR<RobotRaconteurNode>());

    virtual void Start();

    virtual void Stop();

    virtual boost::posix_time::time_duration GetPeriod();

    virtual void SetPeriod(const boost::posix_time::time_duration& period);

    virtual bool IsRunning();

    virtual void Clear();

    virtual ~WallTimer() {}
};

#ifndef BOOST_NO_CXX11_TEMPLATE_ALIASES
/** @brief Convenience alias for Timer shared_ptr */
using TimerPtr = RR_SHARED_PTR<Timer>;
/** @brief Convenience alias for Rate shared_ptr */
using RatePtr = RR_SHARED_PTR<Rate>;
#endif

} // namespace RobotRaconteur