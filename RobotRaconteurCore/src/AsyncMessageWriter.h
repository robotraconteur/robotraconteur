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

#include "RobotRaconteur/Message.h"
#include <vector>
#include <queue>
#include <boost/shared_array.hpp>

#include "RobotRaconteur/AsyncMessageIO.h"

#pragma once

namespace RobotRaconteur
{
class ROBOTRACONTEUR_CORE_API AsyncMessageWriterImpl : public AsyncMessageWriter
{
  public:
    enum state_type
    {
        // Write message
        Message_init = 0,
        Message_done,

        // Write header
        MessageHeader_init,
        MessageHeader_magic,
        MessageHeader_messagesize,
        MessageHeader_version,
        MessageHeader_headersize,
        MessageHeader_flags,
        MessageHeader_routing1,
        MessageHeader_routing2,
        MessageHeader_routing3,
        MessageHeader_routing4,
        MessageHeader_endpoint1,
        MessageHeader_endpoint2,
        MessageHeader_priority,
        MessageHeader_metainfo,
        MessageHeader_messageid1,
        MessageHeader_messageid2,
        MessageHeader_stringtable1,
        MessageHeader_stringtable2,
        MessageHeader_stringtable3,
        MessageHeader_entrycount,
        MessageHeader_extended1,
        MessageHeader_extended2,
        Message_writeentries,

        // Write entry
        MessageEntry_init,
        MessageEntry_finishwrite,
        MessageEntry_entrysize,
        MessageEntry_entryflags,
        MessageEntry_entrytype,
        MessageEntry_servicepathstr,
        MessageEntry_servicepathcode,
        MessageEntry_membernamestr,
        MessageEntry_membernamecode,
        MessageEntry_requestid,
        MessageEntry_error,
        MessageEntry_metainfo,
        MessageEntry_extended1,
        MessageEntry_extended2,
        MessageEntry_elementcount,
        MessageEntry_writeelements,

        // Write element
        MessageElement_init,
        MessageElement_elementsize,
        MessageElement_elementflags,
        MessageElement_elementnamestr,
        MessageElement_elementnamecode,
        MessageElement_elementnumber,
        MessageElement_elementtype,
        MessageElement_elementtypestr,
        MessageElement_elementtypecode,
        MessageElement_metainfo,
        MessageElement_extended1,
        MessageElement_extended2,
        MessageElement_datacount,
        MessageElement_writedata,
        MessageElement_finishwritedata,

        // Write data
        MessageElement_writearray1,
        MessageElement_writearray2,
        MessageElement_writenested1,
        MessageElement_writenested2,

        // String handling
        Header_writestring,

        // Version 2 pad
        MessageEntry_pad
    };

    class state_data
    {
      public:
        state_type state;
        state_type pop_state;
        size_t limit;
        RR_INTRUSIVE_PTR<RRValue> data;
        void* ptrdata;
        size_t param1;
        size_t param2;

        state_data();
    };

  protected:
    uint16_t version;
    std::vector<state_data> state_stack;
    size_t message_pos;
    size_t quota_pos;

    mutable_buffers current_work_bufs;
    mutable_buffers current_work_bufs_start;

  public:
    AsyncMessageWriterImpl();
    RR_OVIRTUAL ~AsyncMessageWriterImpl() RR_OVERRIDE {}

    size_t message_len();

    state_type& state();

    // TODO: Use const data
    template <typename T>
    T* data()
    {
        return static_cast<T*>(state_stack.back().data.get());
    }

    template <typename T>
    T* ptrdata()
    {
        return static_cast<T*>(state_stack.back().ptrdata);
    }

    size_t& param1();
    size_t& param2();

    size_t limit();
    size_t distance_from_limit();

    void pop_state();

    // TODO: use const data
    void push_state(state_type new_state, state_type pop_state, size_t relative_limit,
                    const RR_INTRUSIVE_PTR<RRValue>& data, size_t param1 = 0, size_t param2 = 0);
    void push_state(state_type new_state, state_type pop_state, size_t relative_limit, void* ptrdata, size_t param1 = 0,
                    size_t param2 = 0);

    void prepare_continue(mutable_buffers& work_bufs, size_t& work_bufs_used, const_buffers& write_bufs);

    size_t quota_available() const;
    size_t work_bufs_available();

    bool write_all_bytes(const void* p, size_t len);
    size_t write_some_bytes(const void* p, size_t len);

    template <typename T>
    bool write_number(T number)
    { /*TODO: handle big endian*/
        return write_all_bytes(&number, sizeof(T));
    }

    bool write_uint_x(uint32_t number);
    bool write_uint_x2(uint64_t number);
    bool write_int_x(int32_t number);
    bool write_int_x2(int64_t number);

    // TODO: Use const string
    bool write_string(MessageStringPtr& str, state_type next_state);
    bool write_string(MessageStringPtr& str); // next_state=state()++
    bool write_string4(MessageStringPtr& str, state_type next_state);
    bool write_string4(MessageStringPtr& str); // next_state=state()++

    RR_OVIRTUAL void Reset() RR_OVERRIDE;
    RR_OVIRTUAL void BeginWrite(const RR_INTRUSIVE_PTR<Message>& m, uint16_t version) RR_OVERRIDE;

    RR_OVIRTUAL return_type Write(size_t write_quota, mutable_buffers& work_bufs, size_t& work_bufs_used,
                                  const_buffers& write_bufs) RR_OVERRIDE;
    RR_OVIRTUAL return_type Write4(size_t write_quota, mutable_buffers& work_bufs, size_t& work_bufs_used,
                                   const_buffers& write_bufs) RR_OVERRIDE;

    RR_OVIRTUAL size_t WriteRemaining() RR_OVERRIDE;
};

} // namespace RobotRaconteur
