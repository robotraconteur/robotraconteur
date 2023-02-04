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

#include "AsyncMessageReader.h"
#include <boost/range.hpp>

namespace RobotRaconteur
{

AsyncMessageReaderImpl::state_data::state_data()
{
    state = Message_init;
    pop_state = Message_init;
    param1 = 0;
    param2 = 0;
    limit = 0;
    ptrdata = NULL;
}

AsyncMessageReaderImpl::AsyncMessageReaderImpl()
{
    AsyncMessageReaderImpl::Reset();
    buf = boost::shared_array<uint8_t>(new uint8_t[128]);
    buf_len = 128;
    version = 0;
    buf_avail_pos = 0;
    buf_read_pos = 0;
    message_pos = 0;
}

size_t& AsyncMessageReaderImpl::message_len() { return state_stack.front().limit; }

AsyncMessageReaderImpl::state_type& AsyncMessageReaderImpl::state() { return state_stack.back().state; }

size_t& AsyncMessageReaderImpl::param1() { return state_stack.back().param1; }

size_t& AsyncMessageReaderImpl::param2() { return state_stack.back().param2; }

std::string& AsyncMessageReaderImpl::param3() { return state_stack.back().param3; }

size_t& AsyncMessageReaderImpl::limit() { return state_stack.back().limit; }

size_t AsyncMessageReaderImpl::distance_from_limit() { return limit() - message_pos; }

void AsyncMessageReaderImpl::pop_state()
{
    if (state_stack.size() == 1)
        throw InvalidOperationException("Message read stack empty");
    state_type s = state_stack.back().pop_state;
    state_stack.pop_back();
    state_stack.back().state = s;
}
void AsyncMessageReaderImpl::push_state(AsyncMessageReaderImpl::state_type new_state,
                                        AsyncMessageReaderImpl::state_type pop_state, size_t relative_limit,
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

void AsyncMessageReaderImpl::push_state(AsyncMessageReaderImpl::state_type new_state,
                                        AsyncMessageReaderImpl::state_type pop_state, size_t relative_limit,
                                        void* ptrdata, size_t param1, size_t param2, std::string& param3)
{
    state_data d;
    d.state = new_state;
    d.pop_state = pop_state;
    d.ptrdata = ptrdata;
    d.param1 = param1;
    d.param2 = param2;
    d.param3.swap(param3);
    d.limit = message_pos + relative_limit;
    if (d.limit > message_len())
        throw ProtocolException("Invalid message limit");

    state_stack.push_back(d);
}

void AsyncMessageReaderImpl::prepare_continue(const const_buffers& other_bufs1, size_t& other_bufs_used)
{
    if (buf_avail_pos > 0)
    {
        if (buf_read_pos == buf_avail_pos)
        {
            buf_avail_pos = 0;
            buf_read_pos = 0;
        }
        else
        {
            // Move any remaining data to the beginning of buffer
            size_t n = buf_avail_pos - buf_read_pos;
            std::memmove(buf.get(), buf.get() + buf_read_pos, n);
            buf_read_pos = 0;
            buf_avail_pos = n;
        }
    }

    size_t n2 = boost::asio::buffer_size(this->other_bufs);
    if (n2 > 0)
    {
        boost::asio::mutable_buffer p2(buf.get(), buf_len);
        p2 = p2 + buf_avail_pos;
        size_t n2 = boost::asio::buffer_copy(p2, other_bufs);
        buf_avail_pos += n2;
        buffers_consume(this->other_bufs, n2);
    }
    else
    {
        this->other_bufs.clear();
    }

    other_bufs_used = boost::asio::buffer_size(other_bufs1) - boost::asio::buffer_size(this->other_bufs);
    // other_bufs.clear();
}

size_t AsyncMessageReaderImpl::available()
{
    size_t s = (buf_avail_pos - buf_read_pos);
    s += (boost::asio::buffer_size(other_bufs));
    return s;
}

bool AsyncMessageReaderImpl::read_all_bytes(void* p, size_t len)
{
    if (distance_from_limit() < len)
    {
        throw ProtocolException("Message limit error");
    }

    if (available() < len)
    {
        return false;
    }

    read_some_bytes(p, len);
    return true;
}

size_t AsyncMessageReaderImpl::read_some_bytes(void* p, size_t len)
{
    if (len == 0)
        return 0;

    len = std::min(len, distance_from_limit());

    if (len == 0)
        throw ProtocolException("Message limit error");

    boost::asio::mutable_buffer p2(p, len);
    size_t c = 0;

    if (buf_avail_pos - buf_read_pos > 0)
    {
        boost::asio::const_buffer p3(buf.get(), buf_avail_pos);
        p3 = p3 + buf_read_pos;
        c = boost::asio::buffer_copy(p2, p3);
        p2 = p2 + c;
        p3 = p3 + c;
        if (boost::asio::buffer_size(p3) == 0)
        {
            buf_read_pos = 0;
            buf_avail_pos = 0;
        }
        else
        {
            buf_read_pos += c;
        }

        if (boost::asio::buffer_size(p2) == 0)
        {
            message_pos += c;
            return c;
        }
    }

    size_t c2 = boost::asio::buffer_copy(p2, other_bufs);
    if (c2 == 0)
    {
        message_pos += c;
        return c;
    }

    buffers_consume(other_bufs, c2);
    size_t c3 = c + c2;
    message_pos += c3;
    return c3;
}

bool AsyncMessageReaderImpl::peek_byte(uint8_t& b)
{
    if (distance_from_limit() == 0)
        throw ProtocolException("Message limit error");
    if (available() == 0)
        return false;

    if (buf_avail_pos - buf_read_pos > 0)
    {
        b = *(buf.get() + buf_read_pos);
        return true;
    }

    size_t a1 = boost::asio::buffer_copy(boost::asio::buffer(&b, 1), other_bufs);
    return (a1 == 1);
}

bool AsyncMessageReaderImpl::read_uint_x(uint32_t& num)
{

    uint8_t b1 = 0;
    if (!peek_byte(b1))
        return false;
    if (b1 <= 252)
    {
        read_number(b1);
        num = b1;
        return true;
    }
    if (b1 == 253)
    {
        size_t a1 = available();
        if (a1 < 3)
            return false;
        read_number(b1);
        uint16_t num2 = 0;
        read_number(num2);
        num = num2;
        return true;
    }
    if (b1 == 254)
    {
        size_t a1 = available();
        if (a1 < 5)
            return false;
        read_number(b1);
        read_number(num);
        return true;
    }
    throw ProtocolException("Invalid uint_x in read");
}

bool AsyncMessageReaderImpl::read_uint_x2(uint64_t& num)
{
    uint8_t b1 = 0;
    if (!peek_byte(b1))
        return false;
    if (b1 <= 252)
    {
        read_number(b1);
        num = b1;
        return true;
    }
    if (b1 == 253)
    {
        size_t a1 = available();
        if (a1 < 3)
            return false;
        read_number(b1);
        uint16_t num2 = 0;
        read_number(num2);
        num = num2;
        return true;
    }
    if (b1 == 254)
    {
        size_t a1 = available();
        if (a1 < 5)
            return false;
        read_number(b1);
        uint32_t num2 = 0;
        read_number(num2);
        num = num2;
        return true;
    }
    if (b1 == 255)
    {
        size_t a1 = available();
        if (a1 < 9)
            return false;
        read_number(b1);
        read_number(num);
        return true;
    }
    return false;
}

bool AsyncMessageReaderImpl::read_int_x(int32_t& num)
{

    uint8_t b1_1 = 0;
    if (!peek_byte(b1_1))
        return false;
    int8_t b1 = *reinterpret_cast<int8_t*>(&b1_1);
    if (b1 <= 124)
    {
        read_number(b1);
        num = b1;
        return true;
    }
    if (b1 == 125)
    {
        size_t a1 = available();
        if (a1 < 3)
            return false;
        read_number(b1);
        int16_t num2 = 0;
        read_number(num2);
        num = num2;
        return true;
    }
    if (b1 == 126)
    {
        size_t a1 = available();
        if (a1 < 5)
            return false;
        read_number(b1);
        read_number(num);
        return true;
    }
    throw ProtocolException("Invalid uint_x in read");
}

bool AsyncMessageReaderImpl::read_int_x2(int64_t& num)
{
    uint8_t b1_1 = 0;
    if (!peek_byte(b1_1))
        return false;
    int8_t b1 = *reinterpret_cast<int8_t*>(&b1_1);
    if (b1 <= 124)
    {
        read_number(b1);
        num = b1;
        return true;
    }
    if (b1 == 125)
    {
        size_t a1 = available();
        if (a1 < 3)
            return false;
        read_number(b1);
        int16_t num2 = 0;
        read_number(num2);
        num = num2;
        return true;
    }
    if (b1 == 126)
    {
        size_t a1 = available();
        if (a1 < 5)
            return false;
        read_number(b1);
        int32_t num2 = 0;
        read_number(num2);
        num = num2;
        return true;
    }
    if (b1 == 127)
    {
        size_t a1 = available();
        if (a1 < 9)
            return false;
        read_number(b1);
        read_number(num);
        return true;
    }
    return false;
}

static void null_str_deleter(std::string* s) {}

bool AsyncMessageReaderImpl::read_string(MessageStringPtr& str, state_type next_state)
{
    uint16_t l = 0;
    if (!read_number(l))
        return false;
    // TODO: avoid swap
    std::string s;
    s.resize(l);

    size_t n = read_some_bytes(&s[0], l);
    if (n == l)
    {
        str = MessageStringPtr(RR_MOVE(s));
        return true;
    }

    push_state(Header_readstring, next_state, l - n, &str, n, 0, s);
    return false;
}

bool AsyncMessageReaderImpl::read_string(MessageStringPtr& str)
{
    state_type next_state = state();
    next_state = static_cast<state_type>(static_cast<int>(next_state) + 1);
    return read_string(str, next_state);
}

bool AsyncMessageReaderImpl::read_string4(MessageStringPtr& str, state_type next_state)
{
    uint32_t l = 0;
    if (!read_uint_x(l))
        return false;
    std::string s;
    s.resize(l);

    size_t n = read_some_bytes(&s[0], l);
    if (n == l)
    {
        str = MessageStringPtr(s);
        return true;
    }

    push_state(Header_readstring, next_state, l - n, &str, n, 0, s);
    return false;
}

bool AsyncMessageReaderImpl::read_string4(MessageStringPtr& str)
{
    state_type next_state = state();
    next_state = static_cast<state_type>(static_cast<int>(next_state) + 1);
    return read_string4(str, next_state);
}

void AsyncMessageReaderImpl::Reset()
{
    this->version = 2;
    buf_avail_pos = 0;
    buf_read_pos = 0;

    while (!read_messages.empty())
        read_messages.pop();
    state_stack.clear();

    state_data s;
    RR_INTRUSIVE_PTR<Message> m = CreateMessage();
    s.data = m;
    s.state = Message_init;
    s.limit = 12;
    message_pos = 0;

    state_stack.push_back(s);

    message_pos = 0;
}

#define R(res)                                                                                                         \
    if (!(res))                                                                                                        \
    {                                                                                                                  \
        prepare_continue(other_bufs, other_bufs_used);                                                                 \
        return ReadReturn_continue_nobuffers;                                                                          \
    }

#define DO_POP_STATE()                                                                                                 \
    {                                                                                                                  \
        pop_state();                                                                                                   \
        continue;                                                                                                      \
    }

AsyncMessageReaderImpl::return_type AsyncMessageReaderImpl::Read(const const_buffers& other_bufs,
                                                                 size_t& other_bufs_used, size_t continue_read_len,
                                                                 mutable_buffers& next_continue_read_bufs)
{
    this->other_bufs = other_bufs;

    while (true)
    {
        switch (state())
        {
        case Message_init: {
            RR_INTRUSIVE_PTR<Message> m = CreateMessage();
            state_stack[0].data = m;
            state() = MessageHeader_init;
            continue;
        }
        case Message_done:
            prepare_continue(other_bufs, other_bufs_used);
            if (distance_from_limit() != 0)
                throw ProtocolException("Message did not consume all data");
            read_messages.push(RR_STATIC_POINTER_CAST<Message>(state_stack.back().data));
            return ReadReturn_done;
        case MessageHeader_init: {
            if (available() < 12)
            {
                throw ProtocolException("Initial message header not available");
            }

            std::string magic;
            magic.resize(4);
            read_all_bytes(&magic[0], 4);
            if (magic != "RRAC")
            {
                throw ProtocolException("Invalid message magic");
            }
            RR_INTRUSIVE_PTR<MessageHeader> h = CreateMessageHeader();
            read_number(h->MessageSize);
            read_number(version);
            uint16_t header_size = 0;
            read_number(header_size);
            h->HeaderSize = header_size;
            message_len() = h->MessageSize;
            data<Message>()->header = h;

            push_state(MessageHeader_routing1, Message_readentries, h->HeaderSize - 12, h);
        }
        case MessageHeader_routing1: {
            boost::array<uint8_t, 16> nodeid = {};
            R(read_all_bytes(&nodeid[0], 16));
            data<MessageHeader>()->SenderNodeID = NodeID(nodeid);
            state() = MessageHeader_routing2;
        }
        case MessageHeader_routing2: {
            boost::array<uint8_t, 16> nodeid = {};
            R(read_all_bytes(&nodeid[0], 16));
            data<MessageHeader>()->ReceiverNodeID = NodeID(nodeid);
            state() = MessageHeader_endpoint1;
        }
        case MessageHeader_endpoint1: {
            R(read_number(data<MessageHeader>()->SenderEndpoint));
            state() = MessageHeader_endpoint2;
        }
        case MessageHeader_endpoint2: {
            R(read_number(data<MessageHeader>()->ReceiverEndpoint));
            state() = MessageHeader_routing3;
        }
        case MessageHeader_routing3: {
            R(read_string(data<MessageHeader>()->SenderNodeName, MessageHeader_routing4));
            state() = MessageHeader_routing4;
        }
        case MessageHeader_routing4: {
            R(read_string(data<MessageHeader>()->ReceiverNodeName, MessageHeader_metainfo));
            state() = MessageHeader_metainfo;
        }
        case MessageHeader_metainfo: {
            R(read_string(data<MessageHeader>()->MetaData, MessageHeader_entrycount));
            state() = MessageHeader_entrycount;
        }
        case MessageHeader_entrycount: {
            R(read_number(data<MessageHeader>()->EntryCount));
            state() = MessageHeader_messageid1;
        }
        case MessageHeader_messageid1: {
            R(read_number(data<MessageHeader>()->MessageID));
            state() = MessageHeader_messageid2;
        }
        case MessageHeader_messageid2: {
            R(read_number(data<MessageHeader>()->MessageResID));
            pop_state();
            state() = Message_readentries;
        }
        case Message_readentries: {
            Message* m = data<Message>();
            m->entries.reserve(m->header->EntryCount);
            if (m->entries.size() >= m->header->EntryCount)
            {
                state() = Message_done;
                continue;
            }

            state() = MessageEntry_init;
        }
        case MessageEntry_init: {
            RR_INTRUSIVE_PTR<MessageEntry> ee = CreateMessageEntry();
            data<Message>()->entries.push_back(ee);
            push_state(MessageEntry_entrysize, Message_readentries, limit() - message_pos, ee);
            continue;
        }
        case MessageEntry_finishread: {
            if (distance_from_limit() != 0)
                throw ProtocolException("MessageEntry did not consume all data");
            DO_POP_STATE();
        }
        case MessageEntry_entrysize: {
            uint32_t p = message_pos;
            MessageEntry* ee = data<MessageEntry>();
            R(read_number(ee->EntrySize));
            if (ee->EntrySize < 4)
                throw ProtocolException("Message entry too short");
            if (p + ee->EntrySize > message_len())
                throw ProtocolException("Message entry out of bounds");
            limit() = p + ee->EntrySize;
            state() = MessageEntry_entrytype;
        }
        case MessageEntry_entrytype: {
            uint16_t t = 0;
            R(read_number(t));
            data<MessageEntry>()->EntryType = static_cast<MessageEntryType>(t);
            state() = MessageEntry_pad;
        }
        case MessageEntry_pad: {
            uint16_t v = 0;
            R(read_number(v));
            state() = MessageEntry_servicepathstr;
        }
        case MessageEntry_servicepathstr: {
            R(read_string(data<MessageEntry>()->ServicePath, MessageEntry_membernamestr));
            state() = MessageEntry_membernamestr;
        }
        case MessageEntry_membernamestr: {
            R(read_string(data<MessageEntry>()->MemberName, MessageEntry_requestid));
            state() = MessageEntry_requestid;
        }
        case MessageEntry_requestid: {
            R(read_number(data<MessageEntry>()->RequestID));
            state() = MessageEntry_error;
        }
        case MessageEntry_error: {
            uint16_t err = 0;
            R(read_number(err));
            data<MessageEntry>()->Error = static_cast<MessageErrorType>(err);
            state() = MessageEntry_metainfo;
        }
        case MessageEntry_metainfo: {
            R(read_string(data<MessageEntry>()->MetaData, MessageEntry_elementcount));
            state() = MessageEntry_elementcount;
        }
        case MessageEntry_elementcount: {
            uint16_t c = 0;
            R(read_number(c));
            param1() = c;
            data<MessageEntry>()->elements.reserve(c);
            state() = MessageEntry_readelements;
        }
        case MessageEntry_readelements: {
            MessageEntry* ee = data<MessageEntry>();
            if (ee->elements.size() >= param1())
            {
                state() = MessageEntry_finishread;
                continue;
            }

            state() = MessageElement_init;
        }
        case MessageElement_init: {
            RR_INTRUSIVE_PTR<MessageElement> el = CreateMessageElement();
            MessageEntry* ee = data<MessageEntry>();
            ee->elements.push_back(el);
            push_state(MessageElement_elementsize, MessageEntry_readelements, limit() - message_pos, el);
        }
        case MessageElement_elementsize: {
            size_t p = message_pos;
            uint32_t l = 0;
            R(read_number(l));
            if (l < 4)
                throw ProtocolException("Message element too short");
            data<MessageElement>()->ElementSize = l;
            if (p + l > limit())
                throw ProtocolException("Message element out of bounds");
            limit() = p + l;
            state() = MessageElement_elementnamestr;
        }
        case MessageElement_elementnamestr: {
            R(read_string(data<MessageElement>()->ElementName, MessageElement_elementtype));
            state() = MessageElement_elementtype;
        }
        case MessageElement_elementtype: {
            uint16_t t = 0;
            R(read_number(t));
            data<MessageElement>()->ElementType = static_cast<DataTypes>(t);
            state() = MessageElement_elementtypestr;
        }
        case MessageElement_elementtypestr: {
            R(read_string(data<MessageElement>()->ElementTypeName, MessageElement_metainfo));
            state() = MessageElement_metainfo;
        }
        case MessageElement_metainfo: {
            R(read_string(data<MessageElement>()->MetaData, MessageElement_datacount));
            state() = MessageElement_datacount;
        }
        case MessageElement_datacount: {
            MessageElement* el = data<MessageElement>();
            R(read_number(el->DataCount));
            state() = MessageElement_readdata;
        }
        case MessageElement_readdata: {
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
                state() = MessageElement_readarray1;
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
                state() = MessageElement_readnested1;
                continue;
            }
            default:
                throw DataTypeException("Invalid data type");
            }
        }
        case MessageElement_finishreaddata: {
            if (distance_from_limit() != 0)
                throw ProtocolException("Element did not consume all data");
            DO_POP_STATE();
        }

        case MessageElement_readarray1: {
            MessageElement* el = data<MessageElement>();
            RR_INTRUSIVE_PTR<RRBaseArray> a = AllocateRRArrayByType(el->ElementType, el->DataCount);
            size_t n = a->ElementSize() * a->size();
            size_t p = read_some_bytes(a->void_ptr(), n);
            size_t l = el->ElementSize;
            el->SetData(a);
            el->ElementSize = l;
            if (p >= n)
            {
                state() = MessageElement_finishreaddata;
                continue;
            }
            push_state(MessageElement_readarray2, MessageElement_finishreaddata, n - p, a, p, n);
            boost::asio::mutable_buffer b(a->void_ptr(), n);
            b = b + p;
            next_continue_read_bufs.push_back(b);
            state() = MessageElement_readarray2;
            prepare_continue(other_bufs, other_bufs_used);
            return ReadReturn_continue_buffers;
        }
        case MessageElement_readarray2: {
            if (buf_avail_pos != 0)
            {
                throw InvalidOperationException("Invalid stream position for async read");
            }
            size_t l = param2() - param1();

            size_t n1 = boost::asio::buffer_size(other_bufs);
            if (n1 != 0 && continue_read_len != 0)
            {
                throw InvalidOperationException("Cannot use other_bufs and continue_read_bufs at the same time");
            }

            if (continue_read_len == 0)
            {
                boost::asio::mutable_buffer b(data<RRBaseArray>()->void_ptr(), param2());
                b = b + param1();
                size_t n2 = boost::asio::buffer_copy(b, this->other_bufs);
                buffers_consume(this->other_bufs, n2);
                message_pos += n2;
                if (n2 >= l)
                {
                    // Done
                    DO_POP_STATE();
                }
                param1() += n2;
                b = b + n2;
                next_continue_read_bufs.push_back(b);
                state() = MessageElement_readarray2;
                prepare_continue(other_bufs, other_bufs_used);
                return ReadReturn_continue_buffers;
            }
            else
            {
                param1() += continue_read_len;
                message_pos += continue_read_len;
                if (param1() > param2())
                    throw ProtocolException("Stream reading error");
                if (param1() < param2())
                {
                    boost::asio::mutable_buffer b(data<RRBaseArray>()->void_ptr(), param2());
                    b = b + param1();
                    next_continue_read_bufs.push_back(b);
                    state() = MessageElement_readarray2;
                    prepare_continue(other_bufs, other_bufs_used);
                    return ReadReturn_continue_buffers;
                }

                // Done
                DO_POP_STATE();
            }
        }

        // Read nested element list
        case MessageElement_readnested1: {
            MessageElement* el = data<MessageElement>();
            std::vector<RR_INTRUSIVE_PTR<MessageElement> > v;
            v.reserve(el->DataCount);
            RR_INTRUSIVE_PTR<MessageElementNestedElementList> s =
                CreateMessageElementNestedElementList(el->ElementType, el->ElementTypeName, RR_MOVE(v));
            uint32_t l = el->ElementSize;
            el->SetData(s);
            el->ElementSize = l;
            push_state(MessageElement_readnested2, MessageElement_finishreaddata, limit() - message_pos, s,
                       el->DataCount);
        }
        case MessageElement_readnested2: {
            MessageElementNestedElementList* s = data<MessageElementNestedElementList>();
            if (s->Elements.size() >= param1())
            {
                DO_POP_STATE();
            }

            state() = MessageElement_readnested3;
        }
        case MessageElement_readnested3: {
            RR_INTRUSIVE_PTR<MessageElement> el = CreateMessageElement();
            MessageElementNestedElementList* s = data<MessageElementNestedElementList>();
            s->Elements.push_back(el);
            push_state(MessageElement_elementsize, MessageElement_readnested2, limit() - message_pos, el);
            continue;
        }

        // Read header string
        case Header_readstring: {
            size_t& p1 = param1();
            MessageStringPtr* ptr_s = ptrdata<MessageStringPtr>();
            std::string& s = param3();
            // TODO: avoid swaps
            size_t n = read_some_bytes(&(s).at(p1), s.size() - p1);
            p1 += n;
            size_t s_size = s.size();

            if (p1 == s_size)
            {
                (*ptr_s) = MessageStringPtr(RR_MOVE(s));
                DO_POP_STATE();
            }
            else
            {
                R(false);
            }
        }
        default:
            throw InvalidOperationException("Invalid read state");
        }
    }
}

AsyncMessageReaderImpl::return_type AsyncMessageReaderImpl::Read4(const const_buffers& other_bufs,
                                                                  size_t& other_bufs_used, size_t continue_read_len,
                                                                  mutable_buffers& next_continue_read_bufs)
{
    this->other_bufs = other_bufs;

    while (true)
    {
        switch (state())
        {
        case Message_init: {
            RR_INTRUSIVE_PTR<Message> m = CreateMessage();
            state_stack[0].data = m;
            state() = MessageHeader_init;
            continue;
        }
        case Message_done:
            prepare_continue(other_bufs, other_bufs_used);
            if (distance_from_limit() != 0)
                throw ProtocolException("Message did not consume all data");
            read_messages.push(RR_STATIC_POINTER_CAST<Message>(state_stack.back().data));
            return ReadReturn_done;
        case MessageHeader_init: {
            if (available() < 12)
            {
                throw ProtocolException("Initial message header not available");
            }

            std::string magic;
            magic.resize(4);
            read_all_bytes(&magic[0], 4);
            if (magic != "RRAC")
            {
                throw ProtocolException("Invalid message magic");
            }
            RR_INTRUSIVE_PTR<MessageHeader> h = CreateMessageHeader();
            read_number(h->MessageSize);
            read_number(version);
            message_len() = h->MessageSize;
            data<Message>()->header = h;

            push_state(MessageHeader_headersize, Message_readentries, h->MessageSize - 10, h);
        }
        case MessageHeader_headersize: {
            uint32_t l = 0;
            R(read_uint_x(l));
            limit() = l;
            data<MessageHeader>()->HeaderSize = l;
            state() = MessageHeader_flags;
        }
        case MessageHeader_flags: {
            R(read_number(data<MessageHeader>()->MessageFlags));
            state() = MessageHeader_routing1;
        }

        case MessageHeader_routing1: {
            MessageHeader* h = data<MessageHeader>();
            if (!(h->MessageFlags & MessageFlags_ROUTING_INFO))
            {
                state() = MessageHeader_endpoint1;
                continue;
            }

            boost::array<uint8_t, 16> nodeid = {};
            R(read_all_bytes(&nodeid[0], 16));
            h->SenderNodeID = NodeID(nodeid);
            state() = MessageHeader_routing2;
        }
        case MessageHeader_routing2: {
            boost::array<uint8_t, 16> nodeid = {};
            R(read_all_bytes(&nodeid[0], 16));
            data<MessageHeader>()->ReceiverNodeID = NodeID(nodeid);
            state() = MessageHeader_routing3;
        }
        case MessageHeader_routing3: {
            R(read_string4(data<MessageHeader>()->SenderNodeName));
            state() = MessageHeader_routing4;
        }
        case MessageHeader_routing4: {
            R(read_string4(data<MessageHeader>()->ReceiverNodeName));
            state() = MessageHeader_endpoint1;
        }
        case MessageHeader_endpoint1: {
            MessageHeader* h = data<MessageHeader>();
            if (!(h->MessageFlags & MessageFlags_ENDPOINT_INFO))
            {
                state() = MessageHeader_priority;
                continue;
            }
            R(read_uint_x(h->SenderEndpoint));
            state() = MessageHeader_endpoint2;
        }
        case MessageHeader_endpoint2: {
            R(read_uint_x(data<MessageHeader>()->ReceiverEndpoint));
            state() = MessageHeader_priority;
        }
        case MessageHeader_priority: {
            MessageHeader* h = data<MessageHeader>();
            if (!(h->MessageFlags & MessageFlags_PRIORITY))
            {
                state() = MessageHeader_metainfo;
                continue;
            }

            R(read_number(h->Priority));
            state() = MessageHeader_metainfo;
        }
        case MessageHeader_metainfo: {
            MessageHeader* h = data<MessageHeader>();
            if (!(h->MessageFlags & MessageFlags_META_INFO))
            {
                state() = MessageHeader_stringtable1;
                continue;
            }

            R(read_string4(h->MetaData));
            state() = MessageHeader_messageid1;
        }
        case MessageHeader_messageid1: {
            MessageHeader* h = data<MessageHeader>();
            R(read_number(h->MessageID));
            state() = MessageHeader_messageid2;
        }
        case MessageHeader_messageid2: {
            R(read_number(data<MessageHeader>()->MessageResID));
            state() = MessageHeader_stringtable1;
        }
        case MessageHeader_stringtable1: {
            MessageHeader* h = data<MessageHeader>();
            if (!(h->MessageFlags & MessageFlags_STRING_TABLE))
            {
                state() = MessageHeader_entrycount;
                continue;
            }
            uint32_t n = 0;
            R(read_uint_x(n));
            param1() = n;
            state() = MessageHeader_stringtable2;
        }
        case MessageHeader_stringtable2: {
            if (param1() == 0)
            {
                state() = MessageHeader_entrycount;
                continue;
            }
            uint32_t code = 0;
            R(read_uint_x(code));
            data<MessageHeader>()->StringTable.push_back(boost::make_tuple(code, ""));
            param1()--;
            state() = MessageHeader_stringtable3;
        }
        case MessageHeader_stringtable3: {
            R(read_string4(data<MessageHeader>()->StringTable.back().get<1>(), MessageHeader_stringtable2));
            state() = MessageHeader_stringtable2;
            continue;
        }
        case MessageHeader_entrycount: {
            MessageHeader* h = data<MessageHeader>();
            if (!(h->MessageFlags & MessageFlags_MULTIPLE_ENTRIES))
            {
                h->EntryCount = 1;
            }
            else
            {
                uint32_t c = 0;
                R(read_uint_x(c));
                if (c > std::numeric_limits<uint16_t>::max())
                    throw ProtocolException("Too many entries in message");
                h->EntryCount = boost::numeric_cast<uint16_t>(c);
            }
            state() = MessageHeader_extended1;
        }
        case MessageHeader_extended1: {
            MessageHeader* h = data<MessageHeader>();
            if (!(h->MessageFlags & MessageFlags_EXTENDED))
            {
                pop_state();
                state() = Message_readentries;
                continue;
            }
            uint32_t n = 0;
            R(read_uint_x(n));
            h->Extended.resize(n);
            state() = MessageHeader_extended2;
        }
        case MessageHeader_extended2: {
            MessageHeader* h = data<MessageHeader>();
            if (!h->Extended.empty())
            {
                R(read_all_bytes(&h->Extended[0], h->Extended.size()));
            }
            pop_state();
            state() = Message_readentries;
        }
        case Message_readentries: {
            Message* m = data<Message>();
            m->entries.reserve(m->header->EntryCount);
            if (m->entries.size() >= m->header->EntryCount)
            {
                state() = Message_done;
                continue;
            }

            state() = MessageEntry_init;
        }
        case MessageEntry_init: {
            RR_INTRUSIVE_PTR<MessageEntry> ee = CreateMessageEntry();
            data<Message>()->entries.push_back(ee);
            push_state(MessageEntry_entrysize, Message_readentries, limit() - message_pos, ee);
            continue;
        }
        case MessageEntry_finishread: {
            if (distance_from_limit() != 0)
                throw ProtocolException("MessageEntry did not consume all data");
            DO_POP_STATE();
        }
        case MessageEntry_entrysize: {
            uint32_t p = message_pos;
            MessageEntry* ee = data<MessageEntry>();
            R(read_uint_x(ee->EntrySize));
            if (ee->EntrySize < 4)
                throw ProtocolException("Message entry too short");
            if (p + ee->EntrySize > message_len())
                throw ProtocolException("Message entry out of bounds");
            limit() = p + ee->EntrySize;
            state() = MessageEntry_entryflags;
        }
        case MessageEntry_entryflags: {
            R(read_number(data<MessageEntry>()->EntryFlags));
            state() = MessageEntry_entrytype;
        }
        case MessageEntry_entrytype: {
            uint16_t t = 0;
            R(read_number(t));
            data<MessageEntry>()->EntryType = static_cast<MessageEntryType>(t);
            state() = MessageEntry_servicepathstr;
        }
        case MessageEntry_servicepathstr: {
            MessageEntry* ee = data<MessageEntry>();
            if (ee->EntryFlags & MessageEntryFlags_SERVICE_PATH_STR)
            {
                R(read_string4(ee->ServicePath));
            }
            state() = MessageEntry_servicepathcode;
        }
        case MessageEntry_servicepathcode: {
            MessageEntry* ee = data<MessageEntry>();
            if (ee->EntryFlags & MessageEntryFlags_SERVICE_PATH_CODE)
            {
                R(read_uint_x(ee->ServicePathCode));
            }
            state() = MessageEntry_membernamestr;
        }
        case MessageEntry_membernamestr: {
            MessageEntry* ee = data<MessageEntry>();
            if (ee->EntryFlags & MessageEntryFlags_MEMBER_NAME_STR)
            {
                R(read_string4(ee->MemberName));
            }
            state() = MessageEntry_membernamecode;
        }
        case MessageEntry_membernamecode: {
            MessageEntry* ee = data<MessageEntry>();
            if (ee->EntryFlags & MessageEntryFlags_MEMBER_NAME_CODE)
            {
                R(read_uint_x(ee->MemberNameCode));
            }
            state() = MessageEntry_requestid;
        }
        case MessageEntry_requestid: {
            MessageEntry* ee = data<MessageEntry>();
            if (ee->EntryFlags & MessageEntryFlags_REQUEST_ID)
            {
                R(read_uint_x(ee->RequestID));
            }
            state() = MessageEntry_error;
        }
        case MessageEntry_error: {
            MessageEntry* ee = data<MessageEntry>();
            if (ee->EntryFlags & MessageEntryFlags_ERROR)
            {
                uint16_t err = 0;
                R(read_number(err));
                ee->Error = static_cast<MessageErrorType>(err);
            }
            state() = MessageEntry_metainfo;
        }
        case MessageEntry_metainfo: {
            MessageEntry* ee = data<MessageEntry>();
            if (ee->EntryFlags & MessageEntryFlags_META_INFO)
            {
                R(read_string4(ee->MetaData));
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
            uint32_t n = 0;
            R(read_uint_x(n));
            ee->Extended.resize(n);
            state() = MessageEntry_extended2;
        }
        case MessageEntry_extended2: {
            MessageEntry* ee = data<MessageEntry>();
            if (!ee->Extended.empty())
            {
                R(read_all_bytes(&ee->Extended[0], ee->Extended.size()));
            }
            state() = MessageEntry_elementcount;
        }
        case MessageEntry_elementcount: {
            MessageEntry* ee = data<MessageEntry>();
            uint32_t c = 0;
            R(read_uint_x(c));
            param1() = c;
            ee->elements.reserve(c);
            state() = MessageEntry_readelements;
        }
        case MessageEntry_readelements: {
            MessageEntry* ee = data<MessageEntry>();
            if (ee->elements.size() >= param1())
            {
                state() = MessageEntry_finishread;
                continue;
            }

            state() = MessageElement_init;
        }
        case MessageElement_init: {
            RR_INTRUSIVE_PTR<MessageElement> el = CreateMessageElement();
            MessageEntry* ee = data<MessageEntry>();
            ee->elements.push_back(el);
            push_state(MessageElement_elementsize, MessageEntry_readelements, limit() - message_pos, el);
        }
        case MessageElement_elementsize: {
            size_t p = message_pos;
            uint32_t l = 0;
            R(read_uint_x(l));
            if (l < 4)
                throw ProtocolException("Message element too short");
            data<MessageElement>()->ElementSize = l;
            if (p + l > limit())
                throw ProtocolException("Message element out of bounds");
            limit() = p + l;
            state() = MessageElement_elementflags;
        }
        case MessageElement_elementflags: {
            R(read_number(data<MessageElement>()->ElementFlags));
            state() = MessageElement_elementnamestr;
        }
        case MessageElement_elementnamestr: {
            MessageElement* el = data<MessageElement>();
            if (el->ElementFlags & MessageElementFlags_ELEMENT_NAME_STR)
            {
                R(read_string4(el->ElementName));
            }
            state() = MessageElement_elementnamecode;
        }
        case MessageElement_elementnamecode: {
            MessageElement* el = data<MessageElement>();
            if (el->ElementFlags & MessageElementFlags_ELEMENT_NAME_CODE)
            {
                R(read_uint_x(el->ElementNameCode));
            }
            state() = MessageElement_elementnumber;
        }
        case MessageElement_elementnumber: {
            MessageElement* el = data<MessageElement>();
            if (el->ElementFlags & MessageElementFlags_ELEMENT_NUMBER)
            {
                R(read_int_x(el->ElementNumber));
            }
            state() = MessageElement_elementtype;
        }
        case MessageElement_elementtype: {
            uint16_t t = 0;
            R(read_number(t));
            data<MessageElement>()->ElementType = static_cast<DataTypes>(t);
            state() = MessageElement_elementtypestr;
        }
        case MessageElement_elementtypestr: {
            MessageElement* el = data<MessageElement>();
            if (el->ElementFlags & MessageElementFlags_ELEMENT_TYPE_NAME_STR)
            {
                R(read_string4(el->ElementTypeName));
            }
            state() = MessageElement_elementtypecode;
        }
        case MessageElement_elementtypecode: {
            MessageElement* el = data<MessageElement>();
            if (el->ElementFlags & MessageElementFlags_ELEMENT_TYPE_NAME_CODE)
            {
                R(read_uint_x(el->ElementTypeNameCode));
            }
            state() = MessageElement_metainfo;
        }
        case MessageElement_metainfo: {
            MessageElement* el = data<MessageElement>();
            if (el->ElementFlags & MessageElementFlags_META_INFO)
            {
                R(read_string4(el->MetaData));
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
            uint32_t n = 0;
            R(read_uint_x(n));
            ee->Extended.resize(n);
            state() = MessageElement_extended2;
        }
        case MessageElement_extended2: {
            MessageElement* ee = data<MessageElement>();
            if (!ee->Extended.empty())
            {
                R(read_all_bytes(&ee->Extended[0], ee->Extended.size()));
            }
            state() = MessageElement_datacount;
        }
        case MessageElement_datacount: {
            MessageElement* el = data<MessageElement>();
            R(read_uint_x(el->DataCount));
            state() = MessageElement_readdata;
        }
        case MessageElement_readdata: {
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
                state() = MessageElement_readarray1;
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
                state() = MessageElement_readnested1;
                continue;
            }
            default:
                throw DataTypeException("Invalid data type");
            }
        }
        case MessageElement_finishreaddata: {
            if (distance_from_limit() != 0)
                throw ProtocolException("Element did not consume all data");
            DO_POP_STATE();
        }

        case MessageElement_readarray1: {
            MessageElement* el = data<MessageElement>();
            RR_INTRUSIVE_PTR<RRBaseArray> a = AllocateRRArrayByType(el->ElementType, el->DataCount);
            size_t n = a->ElementSize() * a->size();
            size_t p = read_some_bytes(a->void_ptr(), n);
            size_t l = el->ElementSize;
            el->SetData(a);
            el->ElementSize = l;
            if (p >= n)
            {
                state() = MessageElement_finishreaddata;
                continue;
            }
            push_state(MessageElement_readarray2, MessageElement_finishreaddata, n - p, a, p, n);
            boost::asio::mutable_buffer b(a->void_ptr(), n);
            b = b + p;
            next_continue_read_bufs.push_back(b);
            state() = MessageElement_readarray2;
            prepare_continue(other_bufs, other_bufs_used);
            return ReadReturn_continue_buffers;
        }
        case MessageElement_readarray2: {
            if (buf_avail_pos != 0)
            {
                throw InvalidOperationException("Invalid stream position for async read");
            }
            size_t l = param2() - param1();

            size_t n1 = boost::asio::buffer_size(other_bufs);
            if (n1 != 0 && continue_read_len != 0)
            {
                throw InvalidOperationException("Cannot use other_bufs and continue_read_bufs at the same time");
            }

            if (continue_read_len == 0)
            {
                boost::asio::mutable_buffer b(data<RRBaseArray>()->void_ptr(), param2());
                b = b + param1();
                size_t n2 = boost::asio::buffer_copy(b, this->other_bufs);
                buffers_consume(this->other_bufs, n2);
                message_pos += n2;
                if (n2 >= l)
                {
                    // Done
                    DO_POP_STATE();
                }
                param1() += n2;
                b = b + n2;
                next_continue_read_bufs.push_back(b);
                state() = MessageElement_readarray2;
                prepare_continue(other_bufs, other_bufs_used);
                return ReadReturn_continue_buffers;
            }
            else
            {
                param1() += continue_read_len;
                message_pos += continue_read_len;
                if (param1() > param2())
                    throw ProtocolException("Stream reading error");
                if (param1() < param2())
                {
                    boost::asio::mutable_buffer b(data<RRBaseArray>()->void_ptr(), param2());
                    b = b + param1();
                    next_continue_read_bufs.push_back(b);
                    state() = MessageElement_readarray2;
                    prepare_continue(other_bufs, other_bufs_used);
                    return ReadReturn_continue_buffers;
                }

                // Done
                DO_POP_STATE();
            }
        }

        // Read nested elements
        case MessageElement_readnested1: {
            MessageElement* el = data<MessageElement>();
            std::vector<RR_INTRUSIVE_PTR<MessageElement> > v;
            v.reserve(el->DataCount);
            RR_INTRUSIVE_PTR<MessageElementNestedElementList> s =
                CreateMessageElementNestedElementList(el->ElementType, el->ElementTypeName, RR_MOVE(v));
            uint32_t l = el->ElementSize;
            el->SetData(s);
            el->ElementSize = l;
            push_state(MessageElement_readnested2, MessageElement_finishreaddata, limit() - message_pos, s,
                       el->DataCount);
        }
        case MessageElement_readnested2: {
            MessageElementNestedElementList* s = data<MessageElementNestedElementList>();
            if (s->Elements.size() >= param1())
            {
                DO_POP_STATE();
            }

            state() = MessageElement_readnested3;
        }
        case MessageElement_readnested3: {
            RR_INTRUSIVE_PTR<MessageElement> el = CreateMessageElement();
            MessageElementNestedElementList* s = data<MessageElementNestedElementList>();
            s->Elements.push_back(el);
            push_state(MessageElement_elementsize, MessageElement_readnested2, limit() - message_pos, el);
            continue;
        }

        // Read header string
        case Header_readstring: {
            size_t& p1 = param1();
            MessageStringPtr* ptr_s = ptrdata<MessageStringPtr>();
            // TODO: avoid swaps;
            std::string& s = param3();
            size_t n = read_some_bytes(&(s).at(p1), s.size() - p1);
            p1 += n;
            size_t s_size = s.size();

            if (p1 == s_size)
            {
                (*ptr_s) = MessageStringPtr(RR_MOVE(s));
                DO_POP_STATE();
            }
            else
            {
                R(false);
            }
        }
        default:
            throw InvalidOperationException("Invalid read state");
        }
    }
}

bool AsyncMessageReaderImpl::MessageReady() { return !read_messages.empty(); }

RR_INTRUSIVE_PTR<Message> AsyncMessageReaderImpl::GetNextMessage()
{
    if (read_messages.empty())
        throw InvalidOperationException("Message not ready");

    RR_INTRUSIVE_PTR<Message> m = read_messages.front();
    read_messages.pop();
    return m;
}

} // namespace RobotRaconteur