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

#include "AsyncMessageWriter.h"
#include <boost/range.hpp>
#include <boost/foreach.hpp>

namespace RobotRaconteur
{
AsyncMessageWriterImpl::state_data::state_data()
{
    state = Message_init;
    pop_state = Message_init;
    param1 = 0;
    param2 = 0;
    limit = 0;
    ptrdata = NULL;
}

AsyncMessageWriterImpl::AsyncMessageWriterImpl()
{
    version = 0;
    message_pos = 0;
    quota_pos = 0;
    AsyncMessageWriterImpl::Reset();
}

size_t AsyncMessageWriterImpl::message_len() { return state_stack.front().limit; }

AsyncMessageWriterImpl::state_type& AsyncMessageWriterImpl::state() { return state_stack.back().state; }

size_t& AsyncMessageWriterImpl::param1() { return state_stack.back().param1; }

size_t& AsyncMessageWriterImpl::param2() { return state_stack.back().param2; }

size_t AsyncMessageWriterImpl::limit() { return state_stack.back().limit; }

size_t AsyncMessageWriterImpl::distance_from_limit() { return limit() - message_pos; }

void AsyncMessageWriterImpl::pop_state()
{
    if (state_stack.size() <= 1)
        throw InvalidOperationException("Message writer stack empty");
    state_type s = state_stack.back().pop_state;
    state_stack.pop_back();
    state_stack.back().state = s;
}

void AsyncMessageWriterImpl::push_state(AsyncMessageWriterImpl::state_type new_state,
                                        AsyncMessageWriterImpl::state_type pop_state, size_t relative_limit,
                                        const RR_INTRUSIVE_PTR<RRValue>& data, size_t param1, size_t param2)
{
    state_data d;
    d.state = new_state;
    d.pop_state = pop_state;
    d.data = data;
    d.param1 = param1;
    d.param2 = param2;
    d.limit = message_pos + relative_limit;
    if (d.limit > message_len())
        throw ProtocolException("Invalid message limit");

    state_stack.push_back(d);
}

void AsyncMessageWriterImpl::push_state(AsyncMessageWriterImpl::state_type new_state,
                                        AsyncMessageWriterImpl::state_type pop_state, size_t relative_limit,
                                        void* ptrdata, size_t param1, size_t param2)
{
    state_data d;
    d.state = new_state;
    d.pop_state = pop_state;
    d.ptrdata = ptrdata;
    d.param1 = param1;
    d.param2 = param2;
    d.limit = message_pos + relative_limit;
    if (d.limit > message_len())
        throw ProtocolException("Invalid message limit");

    state_stack.push_back(d);
}

void AsyncMessageWriterImpl::prepare_continue(mutable_buffers& work_bufs, size_t& work_bufs_used,
                                              const_buffers& write_bufs)
{
    size_t n1 = boost::asio::buffer_size(current_work_bufs_start) - boost::asio::buffer_size(current_work_bufs);
    size_t p1 = 0;
    BOOST_FOREACH (boost::asio::mutable_buffer& e, current_work_bufs_start)
    {
        if (p1 == n1)
            break;
        if ((n1 - p1) >= boost::asio::buffer_size(e))
        {
            write_bufs.push_back(e);
            p1 += boost::asio::buffer_size(e);
        }
        else
        {
            write_bufs.push_back(boost::asio::buffer(e, (n1 - p1)));
            p1 = n1;
        }
    }

    current_work_bufs_start = current_work_bufs;

    work_bufs_used = boost::asio::buffer_size(work_bufs) - boost::asio::buffer_size(current_work_bufs);
}

size_t AsyncMessageWriterImpl::quota_available() const { return quota_pos - message_pos; }

size_t AsyncMessageWriterImpl::work_bufs_available()
{
    size_t n1 = quota_pos - message_pos;
    size_t n2 = boost::asio::buffer_size(current_work_bufs);
    return std::min(n1, n2);
}

bool AsyncMessageWriterImpl::write_all_bytes(const void* p, size_t len)
{
    if (distance_from_limit() < len)
    {
        throw ProtocolException("Message limit error");
    }

    if (work_bufs_available() < len)
    {
        return false;
    }

    write_some_bytes(p, len);
    return true;
}

size_t AsyncMessageWriterImpl::write_some_bytes(const void* p, size_t len)
{
    if (len == 0)
        return 0;

    len = std::min(len, distance_from_limit());

    if (len == 0)
        throw ProtocolException("Message limit error");

    len = std::min(len, quota_available());

    size_t n = boost::asio::buffer_copy(current_work_bufs, boost::asio::buffer(p, len));
    buffers_consume(current_work_bufs, n);
    message_pos += n;
    return n;
}

bool AsyncMessageWriterImpl::write_uint_x(uint32_t number)
{
    // TODO: Handle big endian
    if (number <= 252)
    {
        uint8_t v1 = static_cast<uint8_t>(number);
        return write_all_bytes(&v1, 1);
    }
    if (number <= std::numeric_limits<uint16_t>::max())
    {
        boost::array<uint8_t, 3> v1 = {};
        v1[0] = 253;
        uint16_t v2 = static_cast<uint16_t>(number);
        memcpy(v1.data() + 1, &v2, 2);
        return write_all_bytes(v1.data(), 3);
    }

    boost::array<uint8_t, 5> v2 = {};
    v2[0] = 254;
    memcpy(v2.data() + 1, &number, 4);
    return write_all_bytes(v2.data(), 5);
}
bool AsyncMessageWriterImpl::write_uint_x2(uint64_t number)
{
    // TODO: Handle big endian
    if (number <= 252)
    {
        uint8_t v1 = static_cast<uint8_t>(number);
        return write_all_bytes(&v1, 1);
    }
    if (number <= std::numeric_limits<uint16_t>::max())
    {
        boost::array<uint8_t, 3> v1 = {};
        v1[0] = 253;
        uint16_t v2 = static_cast<uint16_t>(number);
        memcpy(v1.data() + 1, &v2, 2);
        return write_all_bytes(v1.data(), 3);
    }
    if (number <= std::numeric_limits<uint32_t>::max())
    {
        boost::array<uint8_t, 5> v1 = {};
        v1[0] = 254;
        uint32_t v2 = static_cast<uint32_t>(number);
        memcpy(v1.data() + 1, &v2, 4);
        return write_all_bytes(v1.data(), 5);
    }

    boost::array<uint8_t, 9> v3 = {};
    v3[0] = 255;
    memcpy(v3.data() + 1, &number, 8);
    return write_all_bytes(v3.data(), 9);
}

bool AsyncMessageWriterImpl::write_int_x(int32_t number)
{
    // TODO: Handle big endian
    if (number >= std::numeric_limits<int8_t>::min() && number <= 124)
    {
        int8_t v1 = static_cast<int8_t>(number);
        return write_all_bytes(&v1, 1);
    }
    if (number >= std::numeric_limits<int16_t>::min() && number <= std::numeric_limits<int16_t>::max())
    {
        boost::array<int8_t, 3> v1 = {};
        v1[0] = 125;
        int16_t v2 = static_cast<int16_t>(number);
        memcpy(v1.data() + 1, &v2, 2);
        return write_all_bytes(v1.data(), 3);
    }

    boost::array<int8_t, 5> v3 = {};
    v3[0] = 126;
    memcpy(v3.data() + 1, &number, 4);
    return write_all_bytes(v3.data(), 5);
}
bool AsyncMessageWriterImpl::write_int_x2(int64_t number)
{
    // TODO: Handle big endian
    if (number >= std::numeric_limits<int8_t>::min() && number <= 124)
    {
        int8_t v1 = static_cast<int8_t>(number);
        return write_all_bytes(&v1, 1);
    }
    if (number >= std::numeric_limits<int16_t>::min() && number <= std::numeric_limits<int16_t>::max())
    {
        boost::array<int8_t, 3> v1 = {};
        v1[0] = 125;
        int16_t v2 = static_cast<int16_t>(number);
        memcpy(v1.data() + 1, &v2, 2);
        return write_all_bytes(v1.data(), 3);
    }
    if (number >= std::numeric_limits<int32_t>::min() && number <= std::numeric_limits<int32_t>::max())
    {
        boost::array<uint8_t, 5> v1 = {};
        v1[0] = 126;
        int32_t v2 = static_cast<int32_t>(number);
        memcpy(v1.data() + 1, &v2, 4);
        return write_all_bytes(v1.data(), 5);
    }

    boost::array<int8_t, 9> v3 = {};
    v3[0] = 127;
    memcpy(v3.data() + 1, &number, 8);
    return write_all_bytes(v3.data(), 9);
}

static void null_str_deleter(std::string* s) {}

bool AsyncMessageWriterImpl::write_string(MessageStringPtr& str_ref, state_type next_state)
{
    boost::string_ref str = str_ref.str();
    size_t l = str.size();
    if (l > std::numeric_limits<uint16_t>::max())
        throw ProtocolException("Header string too long");
    uint16_t l1 = static_cast<uint16_t>(l);
    if (!write_number(l1))
        return false;

    size_t n = write_some_bytes(str.data(), l);
    if (n == l)
        return true;

    push_state(Header_writestring, next_state, l - n, &str_ref, n);
    return false;
}
bool AsyncMessageWriterImpl::write_string(MessageStringPtr& str)
{
    state_type next_state = state();
    next_state = static_cast<state_type>(static_cast<int>(next_state) + 1);
    return write_string(str, next_state);
}
bool AsyncMessageWriterImpl::write_string4(MessageStringPtr& str_ref, state_type next_state)
{
    boost::string_ref str = str_ref.str();
    size_t l = str.size();
    if (l > std::numeric_limits<uint32_t>::max())
        throw ProtocolException("Header string too long");
    uint32_t l1 = static_cast<uint32_t>(l);
    if (!write_uint_x(l1))
        return false;

    size_t n = write_some_bytes(str.data(), l);
    if (n == l)
        return true;

    push_state(Header_writestring, next_state, l - n, &str_ref, n);
    return false;
}
bool AsyncMessageWriterImpl::write_string4(MessageStringPtr& str)
{
    state_type next_state = state();
    next_state = static_cast<state_type>(static_cast<int>(next_state) + 1);
    return write_string4(str, next_state);
}

void AsyncMessageWriterImpl::Reset()
{
    version = 2;
    message_pos = 0;
    quota_pos = 0;
    state_stack.clear();
}
void AsyncMessageWriterImpl::BeginWrite(const RR_INTRUSIVE_PTR<Message>& m, uint16_t version)
{
    if (!state_stack.empty())
        throw InvalidOperationException("AsyncMessageWriter not in reset state");
    if (version == 2)
    {
        this->version = 2;

        size_t n = m->ComputeSize();

        state_data s;
        s.data = m;
        s.state = Message_init;
        s.limit = n;
        state_stack.push_back(s);

        message_pos = 0;
    }
    else if (version == 4)
    {
        this->version = 4;

        size_t n = m->ComputeSize4();

        state_data s;
        s.data = m;
        s.state = Message_init;
        s.limit = n;
        state_stack.push_back(s);

        message_pos = 0;
    }
    else
    {
        throw ProtocolException("Invalid message version");
    }
}

#define R(res)                                                                                                         \
    if (!(res))                                                                                                        \
    {                                                                                                                  \
        prepare_continue(work_bufs, work_bufs_used, write_bufs);                                                       \
        return WriteReturn_continue;                                                                                   \
    }

#define DO_POP_STATE()                                                                                                 \
    {                                                                                                                  \
        pop_state();                                                                                                   \
        continue;                                                                                                      \
    }

AsyncMessageWriterImpl::return_type AsyncMessageWriterImpl::Write(size_t write_quota, mutable_buffers& work_bufs,
                                                                  size_t& work_bufs_used, const_buffers& write_bufs)
{

    this->current_work_bufs = work_bufs;
    this->current_work_bufs_start = work_bufs;

    if ((message_len() - message_pos) == 0)
        return WriteReturn_done;

    quota_pos = message_pos + write_quota;

    while (true)
    {
        switch (state())
        {
        case Message_init: {
            if (state_stack.size() != 1)
                throw InvalidOperationException("AsyncMessageWriter not initialized");
            param1() = 0;
            state() = MessageHeader_init;
            continue;
        }
        case Message_done: {
            if (distance_from_limit() != 0)
                throw ProtocolException("Message did write all data");
            prepare_continue(work_bufs, work_bufs_used, write_bufs);
            if (boost::asio::buffer_size(write_bufs) > 0)
            {
                return WriteReturn_last;
            }
            else
            {
                return WriteReturn_done;
            }
        }
        case MessageHeader_init: {
            Message* m = data<Message>();
            RR_INTRUSIVE_PTR<MessageHeader> h = m->header;
            if (!h)
                throw InvalidOperationException("MessageHeader null in AsyncMessageWriter");

            push_state(MessageHeader_magic, Message_writeentries, h->HeaderSize, h);
            state() = MessageHeader_magic;
        }
        case MessageHeader_magic: {
            R(write_all_bytes("RRAC", 4));
            state() = MessageHeader_messagesize;
        }
        case MessageHeader_messagesize: {
            R(write_number(data<MessageHeader>()->MessageSize));
            state() = MessageHeader_version;
        }
        case MessageHeader_version: {
            uint16_t v = 2;
            R(write_number(v));
            state() = MessageHeader_headersize;
        }
        case MessageHeader_headersize: {
            R(write_number(boost::numeric_cast<uint16_t>(data<MessageHeader>()->HeaderSize)));
            state() = MessageHeader_routing1;
        }
        case MessageHeader_routing1: {
            R(write_all_bytes(&data<MessageHeader>()->SenderNodeID.ToByteArray().at(0), 16));
            state() = MessageHeader_routing2;
        }
        case MessageHeader_routing2: {
            R(write_all_bytes(&data<MessageHeader>()->ReceiverNodeID.ToByteArray().at(0), 16));
            state() = MessageHeader_endpoint1;
        }
        case MessageHeader_endpoint1: {
            R(write_number(data<MessageHeader>()->SenderEndpoint));
            state() = MessageHeader_endpoint2;
        }
        case MessageHeader_endpoint2: {
            R(write_number(data<MessageHeader>()->ReceiverEndpoint));
            state() = MessageHeader_routing3;
        }
        case MessageHeader_routing3: {
            R(write_string(data<MessageHeader>()->SenderNodeName, MessageHeader_routing4));
            state() = MessageHeader_routing4;
        }
        case MessageHeader_routing4: {
            R(write_string(data<MessageHeader>()->ReceiverNodeName, MessageHeader_metainfo));
            state() = MessageHeader_metainfo;
        }
        case MessageHeader_metainfo: {
            R(write_string(data<MessageHeader>()->MetaData, MessageHeader_entrycount));
            state() = MessageHeader_entrycount;
        }
        case MessageHeader_entrycount: {
            R(write_number(data<MessageHeader>()->EntryCount));
            state() = MessageHeader_messageid1;
        }
        case MessageHeader_messageid1: {
            R(write_number(data<MessageHeader>()->MessageID));
            state() = MessageHeader_messageid2;
        }
        case MessageHeader_messageid2: {
            R(write_number(data<MessageHeader>()->MessageResID));
            pop_state();
            state() = Message_writeentries;
        }
        case Message_writeentries: {
            Message* m = data<Message>();
            if (param1() >= m->entries.size())
            {
                state() = Message_done;
                continue;
            }

            state() = MessageEntry_init;
        }
        case MessageEntry_init: {
            RR_INTRUSIVE_PTR<MessageEntry> ee = data<Message>()->entries.at(param1());
            param1()++;
            push_state(MessageEntry_entrysize, Message_writeentries, ee->EntrySize, ee);
            continue;
        }
        case MessageEntry_finishwrite: {
            if (distance_from_limit() != 0)
                throw ProtocolException("MessageEntry did not write all data");
            DO_POP_STATE();
        }
        case MessageEntry_entrysize: {
            R(write_number(data<MessageEntry>()->EntrySize));
            state() = MessageEntry_entrytype;
        }
        case MessageEntry_entrytype: {
            uint16_t t = static_cast<uint16_t>(data<MessageEntry>()->EntryType);
            R(write_number(t));
            state() = MessageEntry_pad;
        }
        case MessageEntry_pad: {
            uint16_t v = 0;
            R(write_number(v));
            state() = MessageEntry_servicepathstr;
        }
        case MessageEntry_servicepathstr: {
            R(write_string(data<MessageEntry>()->ServicePath, MessageEntry_membernamestr));
            state() = MessageEntry_membernamestr;
        }
        case MessageEntry_membernamestr: {
            R(write_string(data<MessageEntry>()->MemberName, MessageEntry_requestid));
            state() = MessageEntry_requestid;
        }
        case MessageEntry_requestid: {
            R(write_number(data<MessageEntry>()->RequestID));
            state() = MessageEntry_error;
        }
        case MessageEntry_error: {
            uint16_t err = static_cast<uint16_t>(data<MessageEntry>()->Error);
            R(write_number(err));
            state() = MessageEntry_metainfo;
        }
        case MessageEntry_metainfo: {
            R(write_string(data<MessageEntry>()->MetaData, MessageEntry_elementcount));
            state() = MessageEntry_elementcount;
        }
        case MessageEntry_elementcount: {
            MessageEntry* ee = data<MessageEntry>();
            uint16_t c = boost::numeric_cast<uint16_t>(ee->elements.size());
            R(write_number(c));
            param1() = 0;
            state() = MessageEntry_writeelements;
        }
        case MessageEntry_writeelements: {
            MessageEntry* ee = data<MessageEntry>();
            if (param1() >= ee->elements.size())
            {
                state() = MessageEntry_finishwrite;
                continue;
            }

            state() = MessageElement_init;
        }
        case MessageElement_init: {
            MessageEntry* ee = data<MessageEntry>();
            RR_INTRUSIVE_PTR<MessageElement> el = ee->elements.at(param1());
            param1()++;
            push_state(MessageElement_elementsize, MessageEntry_writeelements, el->ElementSize, el);
        }
        case MessageElement_elementsize: {
            R(write_number(data<MessageElement>()->ElementSize));

            state() = MessageElement_elementnamestr;
        }
        case MessageElement_elementnamestr: {
            R(write_string(data<MessageElement>()->ElementName, MessageElement_elementtype));
            state() = MessageElement_elementtype;
        }
        case MessageElement_elementtype: {
            uint16_t t = static_cast<uint16_t>(data<MessageElement>()->ElementType);
            R(write_number(t));
            state() = MessageElement_elementtypestr;
        }
        case MessageElement_elementtypestr: {
            R(write_string(data<MessageElement>()->ElementTypeName, MessageElement_metainfo));
            state() = MessageElement_metainfo;
        }
        case MessageElement_metainfo: {
            R(write_string(data<MessageElement>()->MetaData, MessageElement_datacount));
            state() = MessageElement_datacount;
        }
        case MessageElement_datacount: {
            MessageElement* el = data<MessageElement>();
            R(write_number(el->DataCount));
            state() = MessageElement_writedata;
        }

        case MessageElement_writedata: {
            MessageElement* el = data<MessageElement>();
            switch (el->ElementType)
            {
            case DataTypes_void_t: {
                DO_POP_STATE();
            }
            case DataTypes_double_t:
            case DataTypes_single_t:
            case DataTypes_int8_t:
            case DataTypes_uint8_t:
            case DataTypes_int16_t:
            case DataTypes_uint16_t:
            case DataTypes_int32_t:
            case DataTypes_uint32_t:
            case DataTypes_int64_t:
            case DataTypes_uint64_t:
            case DataTypes_string_t:
            case DataTypes_cdouble_t:
            case DataTypes_csingle_t:
            case DataTypes_bool_t: {
                state() = MessageElement_writearray1;
                continue;
            }
            case DataTypes_structure_t:
            case DataTypes_vector_t:
            case DataTypes_dictionary_t:
            case DataTypes_multidimarray_t:
            case DataTypes_list_t:
            case DataTypes_pod_t:
            case DataTypes_pod_array_t:
            case DataTypes_pod_multidimarray_t:
            case DataTypes_namedarray_array_t:
            case DataTypes_namedarray_multidimarray_t: {
                state() = MessageElement_writenested1;
                continue;
            }
            default:
                throw DataTypeException("Invalid data type");
            }
        }
        case MessageElement_finishwritedata: {
            if (distance_from_limit() != 0)
                throw ProtocolException("Element did not write all data");
            DO_POP_STATE();
        }
        case MessageElement_writearray1: {
            MessageElement* el = data<MessageElement>();
            RR_INTRUSIVE_PTR<RRBaseArray> a = el->CastData<RRBaseArray>();
            size_t n = a->ElementSize() * a->size();
            if (n > distance_from_limit())
                throw ProtocolException("Invalid write array length");
            if (n <= 255)
            {
                if (write_all_bytes(a->void_ptr(), n))
                {
                    state() = MessageElement_finishwritedata;
                    continue;
                }
            }

            prepare_continue(work_bufs, work_bufs_used, write_bufs);

            size_t p = quota_available();
            if (n <= p)
            {
                write_bufs.push_back(boost::asio::buffer(a->void_ptr(), n));
                message_pos += n;
                state() = MessageElement_finishwritedata;
                continue;
            }

            write_bufs.push_back(boost::asio::buffer(a->void_ptr(), p));
            message_pos += p;
            push_state(MessageElement_writearray2, MessageElement_finishwritedata, n - p, a, p, n);
            return WriteReturn_continue;
        }
        case MessageElement_writearray2: {
            RRBaseArray* a = data<RRBaseArray>();

            size_t q = quota_available();
            size_t l = param2() - param1();
            size_t n = std::min(q, l);

            write_bufs.push_back(boost::asio::buffer((reinterpret_cast<uint8_t*>(a->void_ptr())) + param1(), n));
            message_pos += n;
            if (n >= l)
            {
                // Done
                DO_POP_STATE();
            }
            else
            {
                param1() += n;
                state() = MessageElement_writearray2;
                return WriteReturn_continue;
            }
        }

        // Write nested elements
        case MessageElement_writenested1: {
            MessageElement* el = data<MessageElement>();
            RR_INTRUSIVE_PTR<MessageElementNestedElementList> s = el->CastData<MessageElementNestedElementList>();
            push_state(MessageElement_writenested2, MessageElement_finishwritedata, limit() - message_pos, s, 0);
        }
        case MessageElement_writenested2: {
            MessageElementNestedElementList* s = data<MessageElementNestedElementList>();
            if (param1() >= s->Elements.size())
            {
                DO_POP_STATE();
            }

            RR_INTRUSIVE_PTR<MessageElement> el = s->Elements.at(param1());
            param1()++;
            push_state(MessageElement_elementsize, MessageElement_writenested2, el->ElementSize, el);
            continue;
        }

        // Handle string interruption
        case Header_writestring: {
            size_t& p1 = param1();
            boost::string_ref s = ptrdata<MessageStringPtr>()->str();
            size_t n = write_some_bytes(s.data() + p1, s.size() - p1);
            p1 += n;

            if (p1 == s.size())
            {
                DO_POP_STATE();
            }
            else
            {
                R(false);
            }
        }

        default:
            throw InvalidOperationException("Invalid write state");
        }
    }
}

AsyncMessageWriterImpl::return_type AsyncMessageWriterImpl::Write4(size_t write_quota, mutable_buffers& work_bufs,
                                                                   size_t& work_bufs_used, const_buffers& write_bufs)
{
    this->current_work_bufs = work_bufs;
    this->current_work_bufs_start = work_bufs;

    if ((message_len() - message_pos) == 0)
        return WriteReturn_done;

    quota_pos = message_pos + write_quota;

    while (true)
    {
        switch (state())
        {
        case Message_init: {
            if (state_stack.size() != 1)
                throw InvalidOperationException("AsyncMessageWriter not initialized");
            param1() = 0;
            state() = MessageHeader_init;
            continue;
        }
        case Message_done: {
            if (distance_from_limit() != 0)
                throw ProtocolException("Message did not write all data");
            prepare_continue(work_bufs, work_bufs_used, write_bufs);
            if (boost::asio::buffer_size(write_bufs) > 0)
            {
                return WriteReturn_last;
            }
            else
            {
                return WriteReturn_done;
            }
        }
        case MessageHeader_init: {
            Message* m = data<Message>();
            RR_INTRUSIVE_PTR<MessageHeader> h = m->header;
            if (!h)
                throw InvalidOperationException("MessageHeader null in AsyncMessageWriter");

            push_state(MessageHeader_magic, Message_writeentries, h->HeaderSize, h);
            state() = MessageHeader_magic;
        }
        case MessageHeader_magic: {
            R(write_all_bytes("RRAC", 4));
            state() = MessageHeader_messagesize;
        }
        case MessageHeader_messagesize: {
            R(write_number(data<MessageHeader>()->MessageSize));
            state() = MessageHeader_version;
        }
        case MessageHeader_version: {
            uint16_t v = 4;
            R(write_number(v));
            state() = MessageHeader_headersize;
        }
        case MessageHeader_headersize: {
            R(write_uint_x(data<MessageHeader>()->HeaderSize));
            state() = MessageHeader_flags;
        }
        case MessageHeader_flags: {
            R(write_number(data<MessageHeader>()->MessageFlags));
            state() = MessageHeader_routing1;
        }
        case MessageHeader_routing1: {
            MessageHeader* h = data<MessageHeader>();
            if (!(h->MessageFlags & MessageFlags_ROUTING_INFO))
            {
                state() = MessageHeader_endpoint1;
                continue;
            }

            R(write_all_bytes(&(h->SenderNodeID.ToByteArray().at(0)), 16));
            state() = MessageHeader_routing2;
        }
        case MessageHeader_routing2: {
            R(write_all_bytes(&(data<MessageHeader>()->ReceiverNodeID.ToByteArray().at(0)), 16));
            state() = MessageHeader_routing3;
        }
        case MessageHeader_routing3: {
            R(write_string4(data<MessageHeader>()->SenderNodeName));
            state() = MessageHeader_routing4;
        }
        case MessageHeader_routing4: {
            R(write_string4(data<MessageHeader>()->ReceiverNodeName));
            state() = MessageHeader_endpoint1;
        }
        case MessageHeader_endpoint1: {
            MessageHeader* h = data<MessageHeader>();
            if (!(h->MessageFlags & MessageFlags_ENDPOINT_INFO))
            {
                state() = MessageHeader_priority;
                continue;
            }
            R(write_uint_x(h->SenderEndpoint));
            state() = MessageHeader_endpoint2;
        }
        case MessageHeader_endpoint2: {
            R(write_uint_x(data<MessageHeader>()->ReceiverEndpoint));
            state() = MessageHeader_priority;
        }
        case MessageHeader_priority: {
            MessageHeader* h = data<MessageHeader>();
            if (!(h->MessageFlags & MessageFlags_PRIORITY))
            {
                state() = MessageHeader_metainfo;
                continue;
            }

            R(write_number(h->Priority));
            state() = MessageHeader_metainfo;
        }
        case MessageHeader_metainfo: {
            MessageHeader* h = data<MessageHeader>();
            if (!(h->MessageFlags & MessageFlags_META_INFO))
            {
                state() = MessageHeader_stringtable1;
                continue;
            }
            R(write_string4(h->MetaData));
            state() = MessageHeader_messageid1;
        }
        case MessageHeader_messageid1: {
            MessageHeader* h = data<MessageHeader>();
            R(write_number(h->MessageID));
            state() = MessageHeader_messageid2;
        }
        case MessageHeader_messageid2: {
            R(write_number(data<MessageHeader>()->MessageResID));
            state() = MessageHeader_stringtable1;
        }
        case MessageHeader_stringtable1: {
            MessageHeader* h = data<MessageHeader>();
            if (!(h->MessageFlags & MessageFlags_STRING_TABLE))
            {
                state() = MessageHeader_entrycount;
                continue;
            }
            uint32_t n = boost::numeric_cast<uint32_t>(h->StringTable.size());
            R(write_uint_x(n));
            param1() = 0;
            state() = MessageHeader_stringtable2;
        }
        case MessageHeader_stringtable2: {
            MessageHeader* h = data<MessageHeader>();
            if (param1() == h->StringTable.size())
            {
                state() = MessageHeader_entrycount;
                continue;
            }
            uint32_t code = h->StringTable.at(param1()).get<0>();
            R(write_uint_x(code));
            param2() = param1();
            param1()++;
            state() = MessageHeader_stringtable3;
        }
        case MessageHeader_stringtable3: {
            MessageHeader* h = data<MessageHeader>();
            MessageStringPtr& s1 = h->StringTable.at(param2()).get<1>();
            R(write_string4(s1, MessageHeader_stringtable2));
            state() = MessageHeader_stringtable2;
            continue;
        }
        case MessageHeader_entrycount: {
            MessageHeader* h = data<MessageHeader>();
            if (h->MessageFlags & MessageFlags_MULTIPLE_ENTRIES)
            {
                R(write_uint_x(h->EntryCount));
            }
            param1() = 0;
            state() = MessageHeader_extended1;
        }
        case MessageHeader_extended1: {
            MessageHeader* h = data<MessageHeader>();
            if (!(h->MessageFlags & MessageFlags_EXTENDED))
            {
                pop_state();
                param1() = 0;
                state() = Message_writeentries;
                continue;
            }
            uint32_t n = boost::numeric_cast<uint32_t>(h->Extended.size());
            R(write_uint_x(n));
            state() = MessageHeader_extended2;
        }
        case MessageHeader_extended2: {
            MessageHeader* h = data<MessageHeader>();
            if (!h->Extended.empty())
            {
                R(write_all_bytes(&h->Extended[0], h->Extended.size()));
            }
            pop_state();
            param1() = 0;
            state() = Message_writeentries;
        }
        case Message_writeentries: {
            Message* m = data<Message>();
            if (param1() >= m->entries.size())
            {
                state() = Message_done;
                continue;
            }

            state() = MessageEntry_init;
        }
        case MessageEntry_init: {
            RR_INTRUSIVE_PTR<MessageEntry> ee = data<Message>()->entries.at(param1());
            param1()++;
            push_state(MessageEntry_entrysize, Message_writeentries, ee->EntrySize, ee);
            continue;
        }
        case MessageEntry_finishwrite: {
            if (distance_from_limit() != 0)
                throw ProtocolException("MessageEntry did not write all data");
            DO_POP_STATE();
        }
        case MessageEntry_entrysize: {
            R(write_uint_x(data<MessageEntry>()->EntrySize));
            state() = MessageEntry_entryflags;
        }
        case MessageEntry_entryflags: {
            R(write_number(data<MessageEntry>()->EntryFlags));
            state() = MessageEntry_entrytype;
        }
        case MessageEntry_entrytype: {
            uint16_t t = static_cast<uint16_t>(data<MessageEntry>()->EntryType);
            R(write_number(t));
            state() = MessageEntry_servicepathstr;
        }
        case MessageEntry_servicepathstr: {
            MessageEntry* ee = data<MessageEntry>();
            if (ee->EntryFlags & MessageEntryFlags_SERVICE_PATH_STR)
            {
                R(write_string4(ee->ServicePath));
            }
            state() = MessageEntry_servicepathcode;
        }
        case MessageEntry_servicepathcode: {
            MessageEntry* ee = data<MessageEntry>();
            if (ee->EntryFlags & MessageEntryFlags_SERVICE_PATH_CODE)
            {
                R(write_uint_x(ee->ServicePathCode));
            }
            state() = MessageEntry_membernamestr;
        }
        case MessageEntry_membernamestr: {
            MessageEntry* ee = data<MessageEntry>();
            if (ee->EntryFlags & MessageEntryFlags_MEMBER_NAME_STR)
            {
                R(write_string4(ee->MemberName));
            }
            state() = MessageEntry_membernamecode;
        }
        case MessageEntry_membernamecode: {
            MessageEntry* ee = data<MessageEntry>();
            if (ee->EntryFlags & MessageEntryFlags_MEMBER_NAME_CODE)
            {
                R(write_uint_x(ee->MemberNameCode));
            }
            state() = MessageEntry_requestid;
        }
        case MessageEntry_requestid: {
            MessageEntry* ee = data<MessageEntry>();
            if (ee->EntryFlags & MessageEntryFlags_REQUEST_ID)
            {
                R(write_uint_x(ee->RequestID));
            }
            state() = MessageEntry_error;
        }
        case MessageEntry_error: {
            MessageEntry* ee = data<MessageEntry>();
            if (ee->EntryFlags & MessageEntryFlags_ERROR)
            {
                uint16_t err = static_cast<uint16_t>(ee->Error);
                R(write_number(err));
                ee->Error = static_cast<MessageErrorType>(err);
            }
            state() = MessageEntry_metainfo;
        }
        case MessageEntry_metainfo: {
            MessageEntry* ee = data<MessageEntry>();
            if (ee->EntryFlags & MessageEntryFlags_META_INFO)
            {
                R(write_string4(ee->MetaData));
            }
            state() = MessageEntry_extended1;
        }
        case MessageEntry_extended1: {
            MessageEntry* ee = data<MessageEntry>();
            if (!(ee->EntryFlags & MessageEntryFlags_EXTENDED))
            {
                state() = MessageEntry_elementcount;
                continue;
            }
            uint32_t n = boost::numeric_cast<uint32_t>(ee->Extended.size());
            R(write_uint_x(n));
            state() = MessageEntry_extended2;
        }
        case MessageEntry_extended2: {
            MessageEntry* ee = data<MessageEntry>();
            if (!ee->Extended.empty())
            {
                R(write_all_bytes(&ee->Extended[0], ee->Extended.size()));
            }
            state() = MessageEntry_elementcount;
        }
        case MessageEntry_elementcount: {
            MessageEntry* ee = data<MessageEntry>();
            uint32_t c = boost::numeric_cast<uint32_t>(ee->elements.size());
            R(write_uint_x(c));
            param1() = 0;
            state() = MessageEntry_writeelements;
        }
        case MessageEntry_writeelements: {
            MessageEntry* ee = data<MessageEntry>();
            if (param1() >= ee->elements.size())
            {
                state() = MessageEntry_finishwrite;
                continue;
            }

            state() = MessageElement_init;
        }
        case MessageElement_init: {
            MessageEntry* ee = data<MessageEntry>();
            RR_INTRUSIVE_PTR<MessageElement> el = ee->elements.at(param1());
            param1()++;
            push_state(MessageElement_elementsize, MessageEntry_writeelements, el->ElementSize, el);
        }
        case MessageElement_elementsize: {
            R(write_uint_x(data<MessageElement>()->ElementSize));
            state() = MessageElement_elementflags;
        }
        case MessageElement_elementflags: {
            R(write_number(data<MessageElement>()->ElementFlags));
            state() = MessageElement_elementnamestr;
        }
        case MessageElement_elementnamestr: {
            MessageElement* el = data<MessageElement>();
            if (el->ElementFlags & MessageElementFlags_ELEMENT_NAME_STR)
            {
                R(write_string4(el->ElementName));
            }
            state() = MessageElement_elementnamecode;
        }
        case MessageElement_elementnamecode: {
            MessageElement* el = data<MessageElement>();
            if (el->ElementFlags & MessageElementFlags_ELEMENT_NAME_CODE)
            {
                R(write_uint_x(el->ElementNameCode));
            }
            state() = MessageElement_elementnumber;
        }
        case MessageElement_elementnumber: {
            MessageElement* el = data<MessageElement>();
            if (el->ElementFlags & MessageElementFlags_ELEMENT_NUMBER)
            {
                R(write_int_x(el->ElementNumber));
            }
            state() = MessageElement_elementtype;
        }
        case MessageElement_elementtype: {
            uint16_t t = static_cast<uint16_t>(data<MessageElement>()->ElementType);
            R(write_number(t));
            state() = MessageElement_elementtypestr;
        }
        case MessageElement_elementtypestr: {
            MessageElement* el = data<MessageElement>();
            if (el->ElementFlags & MessageElementFlags_ELEMENT_TYPE_NAME_STR)
            {
                R(write_string4(el->ElementTypeName));
            }
            state() = MessageElement_elementtypecode;
        }
        case MessageElement_elementtypecode: {
            MessageElement* el = data<MessageElement>();
            if (el->ElementFlags & MessageElementFlags_ELEMENT_TYPE_NAME_CODE)
            {
                R(write_uint_x(el->ElementTypeNameCode));
            }
            state() = MessageElement_metainfo;
        }
        case MessageElement_metainfo: {
            MessageElement* el = data<MessageElement>();
            if (el->ElementFlags & MessageElementFlags_META_INFO)
            {
                R(write_string4(el->MetaData));
            }
            state() = MessageElement_extended1;
        }
        case MessageElement_extended1: {
            MessageElement* ee = data<MessageElement>();
            if (!(ee->ElementFlags & MessageElementFlags_EXTENDED))
            {
                state() = MessageElement_datacount;
                continue;
            }
            uint32_t n = boost::numeric_cast<uint32_t>(ee->Extended.size());
            R(write_uint_x(n));
            state() = MessageElement_extended2;
        }
        case MessageElement_extended2: {
            MessageElement* ee = data<MessageElement>();
            if (!ee->Extended.empty())
            {
                R(write_all_bytes(&ee->Extended[0], ee->Extended.size()));
            }
            state() = MessageElement_datacount;
        }
        case MessageElement_datacount: {
            MessageElement* el = data<MessageElement>();
            R(write_uint_x(el->DataCount));
            state() = MessageElement_writedata;
        }
        case MessageElement_writedata: {
            MessageElement* el = data<MessageElement>();
            switch (el->ElementType)
            {
            case DataTypes_void_t: {
                DO_POP_STATE();
            }
            case DataTypes_double_t:
            case DataTypes_single_t:
            case DataTypes_int8_t:
            case DataTypes_uint8_t:
            case DataTypes_int16_t:
            case DataTypes_uint16_t:
            case DataTypes_int32_t:
            case DataTypes_uint32_t:
            case DataTypes_int64_t:
            case DataTypes_uint64_t:
            case DataTypes_string_t:
            case DataTypes_cdouble_t:
            case DataTypes_csingle_t:
            case DataTypes_bool_t: {
                state() = MessageElement_writearray1;
                continue;
            }
            case DataTypes_structure_t:
            case DataTypes_vector_t:
            case DataTypes_dictionary_t:
            case DataTypes_multidimarray_t:
            case DataTypes_list_t:
            case DataTypes_pod_t:
            case DataTypes_pod_array_t:
            case DataTypes_pod_multidimarray_t:
            case DataTypes_namedarray_array_t:
            case DataTypes_namedarray_multidimarray_t: {
                state() = MessageElement_writenested1;
                continue;
            }
            default:
                throw DataTypeException("Invalid data type");
            }
        }
        case MessageElement_finishwritedata: {
            if (distance_from_limit() != 0)
                throw ProtocolException("Element did not write all data");
            DO_POP_STATE();
        }
        case MessageElement_writearray1: {
            MessageElement* el = data<MessageElement>();
            RR_INTRUSIVE_PTR<RRBaseArray> a = el->CastData<RRBaseArray>();
            size_t n = a->ElementSize() * a->size();
            if (n > distance_from_limit())
                throw ProtocolException("Invalid write array length");
            if (n <= 255)
            {
                if (write_all_bytes(a->void_ptr(), n))
                {
                    state() = MessageElement_finishwritedata;
                    continue;
                }
            }

            prepare_continue(work_bufs, work_bufs_used, write_bufs);

            size_t p = quota_available();
            if (n <= p)
            {
                write_bufs.push_back(boost::asio::buffer(a->void_ptr(), n));
                message_pos += n;
                state() = MessageElement_finishwritedata;
                continue;
            }

            write_bufs.push_back(boost::asio::buffer(a->void_ptr(), p));
            message_pos += p;
            push_state(MessageElement_writearray2, MessageElement_finishwritedata, n - p, a, p, n);
            return WriteReturn_continue;
        }
        case MessageElement_writearray2: {
            RRBaseArray* a = data<RRBaseArray>();

            size_t q = quota_available();
            size_t l = param2() - param1();
            size_t n = std::min(q, l);

            write_bufs.push_back(boost::asio::buffer((reinterpret_cast<uint8_t*>(a->void_ptr())) + param1(), n));
            message_pos += n;
            if (n >= l)
            {
                // Done
                DO_POP_STATE();
            }
            else
            {
                param1() += n;
                state() = MessageElement_writearray2;
                return WriteReturn_continue;
            }
        }

        // Handle string interruption
        case Header_writestring: {
            size_t& p1 = param1();
            boost::string_ref s = ptrdata<MessageStringPtr>()->str();
            size_t n = write_some_bytes(s.data() + p1, s.size() - p1);
            p1 += n;

            if (p1 == s.size())
            {
                DO_POP_STATE();
            }
            else
            {
                R(false);
            }
        }

        // Write structure
        case MessageElement_writenested1: {
            MessageElement* el = data<MessageElement>();
            RR_INTRUSIVE_PTR<MessageElementNestedElementList> s = el->CastData<MessageElementNestedElementList>();
            push_state(MessageElement_writenested2, MessageElement_finishwritedata, limit() - message_pos, s, 0);
        }
        case MessageElement_writenested2: {
            MessageElementNestedElementList* s = data<MessageElementNestedElementList>();
            if (param1() >= s->Elements.size())
            {
                DO_POP_STATE();
            }

            RR_INTRUSIVE_PTR<MessageElement> el = s->Elements.at(param1());
            param1()++;
            push_state(MessageElement_elementsize, MessageElement_writenested2, el->ElementSize, el);
            continue;
        }

        default:
            throw InvalidOperationException("Invalid write state");
        }
    }
}

size_t AsyncMessageWriterImpl::WriteRemaining() { return message_len() - message_pos; }
} // namespace RobotRaconteur
