/**
 * @file AutoResetEvent.h
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

#include "RobotRaconteur/DataTypes.h"

namespace RobotRaconteur
{
/** @brief Synchronization event for thread synchronization. Resets automatically after
 * being triggered
 *
 * Construct using RobotRaconteurNode::CreateAutoResetEvent()
 */
class ROBOTRACONTEUR_CORE_API AutoResetEvent : private boost::noncopyable
{
  public:
    AutoResetEvent();
    virtual ~AutoResetEvent();

    /** @brief Set the event, releasing waiting threads */
    virtual void Set();

    /** @brief Reset the event */
    virtual void Reset();

    /** @brief Block the current thread infinitely until Set() is called */
    virtual void WaitOne();

    /**
     * @brief Block the current thread until Set() is called, or timeout
     * expires
     *
     * Timeout is based on the RobotRaconteurNode time provider
     *
     * @param timeout The timeout in milliseconds
     * @return true Set() was called
     * @return false The wait timed out
     */
    virtual bool WaitOne(int32_t timeout);
#ifdef ROBOTRACONTEUR_WINDOWS
  private:
    void* ev;
#else
  private:
    volatile bool m_bSet;
    boost::condition_variable m_setCondition;
    boost::mutex m_mutex;
#endif
};

} // namespace RobotRaconteur