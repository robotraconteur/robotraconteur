/**
 * @file AsyncMessageIO.h
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

#include "RobotRaconteur/Message.h"

#pragma once

namespace RobotRaconteur
{
class ROBOTRACONTEUR_CORE_API AsyncMessageReader : private boost::noncopyable
{
  public:
    enum return_type
    {
        ReadReturn_done = 0,
        ReadReturn_continue_nobuffers,
        ReadReturn_continue_buffers
    };

    virtual void Reset() = 0;
    virtual return_type Read(const const_buffers& other_bufs, size_t& other_bufs_used, size_t continue_read_len,
                             mutable_buffers& next_continue_read_bufs) = 0;
    virtual return_type Read4(const const_buffers& other_bufs, size_t& other_bufs_used, size_t continue_read_len,
                              mutable_buffers& next_continue_read_bufs) = 0;

    virtual bool MessageReady() = 0;
    virtual RR_INTRUSIVE_PTR<Message> GetNextMessage() = 0;

    static RR_SHARED_PTR<AsyncMessageReader> CreateInstance();

    virtual ~AsyncMessageReader();
};

class ROBOTRACONTEUR_CORE_API AsyncMessageWriter : private boost::noncopyable
{
  public:
    enum return_type
    {
        WriteReturn_done = 0,
        WriteReturn_continue,
        WriteReturn_last,
    };

    virtual void Reset() = 0;
    virtual void BeginWrite(RR_INTRUSIVE_PTR<Message> m, uint16_t version) = 0;

    virtual return_type Write(size_t write_quota, mutable_buffers& work_bufs, size_t& work_bufs_used,
                              const_buffers& write_bufs) = 0;
    virtual return_type Write4(size_t write_quota, mutable_buffers& work_bufs, size_t& work_bufs_used,
                               const_buffers& write_bufs) = 0;

    virtual size_t WriteRemaining() = 0;

    static RR_SHARED_PTR<AsyncMessageWriter> CreateInstance();

    virtual ~AsyncMessageWriter();
};

} // namespace RobotRaconteur
