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
class ROBOTRACONTEUR_CORE_API AsyncMessageReaderImpl : public AsyncMessageReader
{
  public:
    enum state_type
    {
        // Read message
        Message_init = 0,
        Message_done,

        // Read header
        MessageHeader_init,
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
        Message_readentries,

        // Read entry
        MessageEntry_init,
        MessageEntry_finishread,
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
        MessageEntry_readelements,

        // Read element
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
        MessageElement_readdata,
        MessageElement_finishreaddata,
        // Read Data
        MessageElement_readarray1,
        MessageElement_readarray2,
        MessageElement_readnested1,
        MessageElement_readnested2,
        MessageElement_readnested3,
        // Read header string
        Header_readstring,

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
        std::string param3;

        state_data();
    };

  protected:
    uint16_t version;
    std::vector<state_data> state_stack;

    // Buffer to read headers
    boost::shared_array<uint8_t> buf;
    size_t buf_len;
    size_t buf_avail_pos;
    size_t buf_read_pos;

    const_buffers other_bufs;

    std::queue<RR_INTRUSIVE_PTR<Message> > read_messages;

    size_t message_pos;

  public:
    AsyncMessageReaderImpl();
    RR_OVIRTUAL ~AsyncMessageReaderImpl() RR_OVERRIDE {}

    size_t& message_len();

    state_type& state();

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
    std::string& param3();

    size_t& limit();
    size_t distance_from_limit();

    void pop_state();
    void push_state(state_type new_state, state_type pop_state, size_t relative_limit,
                    const RR_INTRUSIVE_PTR<RRValue>& data, size_t param1 = 0, size_t param2 = 0);
    void push_state(state_type new_state, state_type pop_state, size_t relative_limit, void* ptrdata, size_t param1,
                    size_t param2, std::string& param3);

    void prepare_continue(const const_buffers& other_buf, size_t& other_bufs_used);

    size_t available();
    bool read_all_bytes(void* p, size_t len);
    size_t read_some_bytes(void* p, size_t len);
    bool peek_byte(uint8_t& b);
    template <typename T>
    bool read_number(T& number)
    { /*TODO: handle big endian*/
        return read_all_bytes(&number, sizeof(T));
    }
    bool read_uint_x(uint32_t& number);
    bool read_uint_x2(uint64_t& number);
    bool read_int_x(int32_t& number);
    bool read_int_x2(int64_t& number);
    bool read_string(MessageStringPtr& str, state_type next_state);
    bool read_string(MessageStringPtr& str); // next_state=state()++
    bool read_string4(MessageStringPtr& str, state_type next_state);
    bool read_string4(MessageStringPtr& str); // next_state=state()++

    RR_OVIRTUAL void Reset() RR_OVERRIDE;
    RR_OVIRTUAL return_type Read(const const_buffers& other_bufs, size_t& other_bufs_used, size_t continue_read_len,
                                 mutable_buffers& next_continue_read_bufs) RR_OVERRIDE;
    RR_OVIRTUAL return_type Read4(const const_buffers& other_bufs, size_t& other_bufs_used, size_t continue_read_len,
                                  mutable_buffers& next_continue_read_bufs) RR_OVERRIDE;

    RR_OVIRTUAL bool MessageReady() RR_OVERRIDE;
    RR_OVIRTUAL RR_INTRUSIVE_PTR<Message> GetNextMessage() RR_OVERRIDE;
};
} // namespace RobotRaconteur
