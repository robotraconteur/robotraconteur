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

#include "RobotRaconteur/Message.h"
#include "RobotRaconteur/Error.h"

#include "RobotRaconteur/IOUtils.h"
#include "RobotRaconteur/NodeID.h"
#include "RobotRaconteur/DataTypes.h"
#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>
#include <boost/range.hpp>
#include <boost/range/algorithm.hpp>

namespace RobotRaconteur
{

Message::Message() { entries.clear(); }

RR_INTRUSIVE_PTR<MessageEntry> Message::FindEntry(MessageStringRef name)
{
    std::vector<RR_INTRUSIVE_PTR<MessageEntry> >::iterator m =
        boost::find_if(entries, boost::bind(&MessageEntry::MemberName, RR_BOOST_PLACEHOLDERS(_1)) == name);

    if (m == entries.end())
        throw MessageEntryNotFoundException("Element " + name.str() + " not found.");

    return *m;
}

RR_INTRUSIVE_PTR<MessageEntry> Message::AddEntry(MessageEntryType t, MessageStringRef name)
{
    RR_INTRUSIVE_PTR<MessageEntry> m = CreateMessageEntry();
    m->MemberName = name;
    m->EntryType = t;

    entries.push_back(m);

    return m;
}

uint32_t Message::ComputeSize()
{
    uint64_t s = header->ComputeSize();
    BOOST_FOREACH (RR_INTRUSIVE_PTR<MessageEntry>& e, entries)
    {
        e->UpdateData();
        s += e->EntrySize;
    }

    if (s > std::numeric_limits<uint32_t>::max())
        throw ProtocolException("Message exceeds maximum length");
    header->UpdateHeader(boost::numeric_cast<uint32_t>(s), boost::numeric_cast<uint16_t>(entries.size()));
    return boost::numeric_cast<uint32_t>(s);
}

void Message::Write(ArrayBinaryWriter& w)
{

    uint32_t s = ComputeSize();

    w.PushRelativeLimit(s);

    header->UpdateHeader(s, boost::numeric_cast<uint16_t>(entries.size()));
    header->Write(w);
    BOOST_FOREACH (RR_INTRUSIVE_PTR<MessageEntry>& e, entries)
    {
        e->Write(w);
    }

    w.PopLimit();
    // if (w.DistanceFromLimit()!=0) throw ProtocolException("Error in message format");
}

void Message::Read(ArrayBinaryReader& r)
{
    header = CreateMessageHeader();
    header->Read(r);

    r.PushRelativeLimit(header->MessageSize - header->HeaderSize);

    uint16_t s = header->EntryCount;
    entries.clear();
    for (int32_t i = 0; i < s; i++)
    {
        RR_INTRUSIVE_PTR<MessageEntry> e = CreateMessageEntry();
        e->Read(r);
        entries.push_back(e);
    }
}

uint32_t Message::ComputeSize4()
{
    header->EntryCount = boost::numeric_cast<uint16_t>(entries.size());
    uint64_t s = 0;
    BOOST_FOREACH (RR_INTRUSIVE_PTR<MessageEntry>& e, entries)
    {
        e->UpdateData4();
        s += e->EntrySize;
    }

    if (s > std::numeric_limits<uint32_t>::max())
        throw ProtocolException("Message exceeds maximum length");

    header->UpdateHeader4(boost::numeric_cast<uint32_t>(s), boost::numeric_cast<uint16_t>(entries.size()));

    uint32_t s1 = header->MessageSize;

    if (s1 > std::numeric_limits<uint32_t>::max())
        throw ProtocolException("Message exceeds maximum length");
    return boost::numeric_cast<uint32_t>(s1);
}

void Message::Write4(ArrayBinaryWriter& w)
{

    uint32_t s = ComputeSize4();

    w.PushRelativeLimit(s);

    header->Write4(w);
    BOOST_FOREACH (RR_INTRUSIVE_PTR<MessageEntry>& e, entries)
    {
        e->Write4(w);
    }

    w.PopLimit();
    // if (w.DistanceFromLimit()!=0) throw ProtocolException("Error in message format");
}

void Message::Read4(ArrayBinaryReader& r)
{
    header = CreateMessageHeader();
    header->Read4(r);

    r.PushRelativeLimit(header->MessageSize - header->HeaderSize);

    uint16_t s = header->EntryCount;
    entries.clear();
    for (int32_t i = 0; i < s; i++)
    {
        RR_INTRUSIVE_PTR<MessageEntry> e = CreateMessageEntry();
        e->Read4(r);
        entries.push_back(e);
    }
}

uint16_t MessageHeader::ComputeSize() // NOLINT(readability-make-member-function-const)
{
    uint32_t s1 = boost::numeric_cast<uint32_t>(ArrayBinaryWriter::GetStringByteCount8(SenderNodeName));
    uint32_t s2 = boost::numeric_cast<uint32_t>(ArrayBinaryWriter::GetStringByteCount8(ReceiverNodeName));
    uint32_t s3 = boost::numeric_cast<uint32_t>(ArrayBinaryWriter::GetStringByteCount8(MetaData));

    if (s1 > std::numeric_limits<uint16_t>::max())
        throw ProtocolException("SenderNodeName exceeds maximum length");
    if (s2 > std::numeric_limits<uint16_t>::max())
        throw ProtocolException("ReceiverNodeName exceeds maximum length");
    if (s3 > std::numeric_limits<uint16_t>::max())
        throw ProtocolException("Header MetaData exceeds maximum length");

    uint32_t s = 64 + s1 + s2 + s3;

    if (s > std::numeric_limits<uint16_t>::max())
        throw ProtocolException("MessageHeader exceeds maximum length");

    return boost::numeric_cast<uint16_t>(s);
}

void MessageHeader::UpdateHeader(uint32_t message_size, uint16_t entry_count)
{
    if (MessageFlags != MessageFlags_Version2Compat)
    {
        throw ProtocolException("Invalid message flags for Version 2 message");
    }

    HeaderSize = ComputeSize();
    MessageSize = message_size;
    EntryCount = entry_count;
}

void MessageHeader::Write(ArrayBinaryWriter& w) // NOLINT(readability-make-member-function-const)
{
    w.PushRelativeLimit(HeaderSize);
    w.WriteString8("RRAC");
    w.WriteNumber(MessageSize);
    w.WriteNumber(boost::numeric_cast<uint16_t>(2));

    if (HeaderSize > std::numeric_limits<uint16_t>::max())
    {
        throw ProtocolException("Header too large for Message 2");
    }
    w.WriteNumber(boost::numeric_cast<uint16_t>(HeaderSize));

    const boost::array<uint8_t, 16> bSenderNodeID = SenderNodeID.ToByteArray();
    const boost::array<uint8_t, 16> bReceiverNodeID = ReceiverNodeID.ToByteArray();
    for (int32_t i = 0; i < 16; i++)
    {
        w.WriteNumber(bSenderNodeID[i]);
    };
    for (int32_t i = 0; i < 16; i++)
    {
        w.WriteNumber(bReceiverNodeID[i]);
    };
    w.WriteNumber(SenderEndpoint);
    w.WriteNumber(ReceiverEndpoint);
    w.WriteNumber(boost::numeric_cast<uint16_t>(ArrayBinaryWriter::GetStringByteCount8(SenderNodeName)));
    w.WriteString8(SenderNodeName);
    w.WriteNumber(boost::numeric_cast<uint16_t>(ArrayBinaryWriter::GetStringByteCount8(ReceiverNodeName)));
    w.WriteString8(ReceiverNodeName);
    w.WriteNumber(boost::numeric_cast<uint16_t>(ArrayBinaryWriter::GetStringByteCount8(MetaData)));
    w.WriteString8(MetaData);
    w.WriteNumber(boost::numeric_cast<uint16_t>(EntryCount));
    w.WriteNumber(MessageID);
    w.WriteNumber(MessageResID);

    if (w.DistanceFromLimit() != 0)
        throw DataSerializationException("Error in message format");
    w.PopLimit();
}

void MessageHeader::Read(ArrayBinaryReader& r)
{
    MessageStringPtr magic = r.ReadString8(4).str();
    if (magic != "RRAC")
        throw ProtocolException("Incorrect message magic");
    MessageSize = r.ReadNumber<uint32_t>();
    uint16_t version = r.ReadNumber<uint16_t>();
    if (version != 2)
        throw ProtocolException("Uknown protocol version");

    HeaderSize = r.ReadNumber<uint16_t>();

    r.PushRelativeLimit(HeaderSize - 12);

    boost::array<uint8_t, 16> bSenderNodeID = {};
    for (int32_t i = 0; i < 16; i++)
    {
        bSenderNodeID[i] = r.ReadNumber<uint8_t>();
    };
    SenderNodeID = NodeID(bSenderNodeID);

    boost::array<uint8_t, 16> bReceiverNodeID = {};
    for (int32_t i = 0; i < 16; i++)
    {
        bReceiverNodeID[i] = r.ReadNumber<uint8_t>();
    };
    ReceiverNodeID = NodeID(bReceiverNodeID);
    SenderEndpoint = r.ReadNumber<uint32_t>();
    ReceiverEndpoint = r.ReadNumber<uint32_t>();
    uint16_t pname_s = r.ReadNumber<uint16_t>();
    SenderNodeName = r.ReadString8(pname_s);
    uint16_t pname_r = r.ReadNumber<uint16_t>();
    ReceiverNodeName = r.ReadString8(pname_r);
    uint16_t meta_s = r.ReadNumber<uint16_t>();
    MetaData = r.ReadString8(meta_s);

    EntryCount = r.ReadNumber<uint16_t>();
    MessageID = r.ReadNumber<uint16_t>();
    MessageResID = r.ReadNumber<uint16_t>();

    if (r.DistanceFromLimit() != 0)
        throw DataSerializationException("Error in message format");
    r.PopLimit();
}

uint32_t MessageHeader::ComputeSize4()
{
    size_t s = 11;

    if (MessageFlags & MessageFlags_ROUTING_INFO)
    {
        s += 32;
        s += ArrayBinaryWriter::GetStringByteCount8WithXLen(SenderNodeName);
        s += ArrayBinaryWriter::GetStringByteCount8WithXLen(ReceiverNodeName);
    }

    if (MessageFlags & MessageFlags_ENDPOINT_INFO)
    {
        s += ArrayBinaryWriter::GetUintXByteCount(SenderEndpoint);
        s += ArrayBinaryWriter::GetUintXByteCount(ReceiverEndpoint);
    }

    if (MessageFlags & MessageFlags_PRIORITY)
    {
        s += 2;
    }

    if (MessageFlags & MessageFlags_META_INFO)
    {
        s += ArrayBinaryWriter::GetStringByteCount8WithXLen(MetaData);
        s += 4;
    }

    if (MessageFlags & MessageFlags_STRING_TABLE)
    {
        uint32_t s1 = 0;
        for (std::vector<boost::tuple<uint32_t, MessageStringPtr> >::iterator e = StringTable.begin();
             e != StringTable.end(); e++)
        {
            s1 += ArrayBinaryWriter::GetUintXByteCount(e->get<0>());
            s1 += ArrayBinaryWriter::GetStringByteCount8WithXLen(e->get<1>());
        }
        if (s1 > 1024)
            throw ProtocolException("String table too large");
        s += s1 + ArrayBinaryWriter::GetUintXByteCount(StringTable.size());
    }

    if (MessageFlags & MessageFlags_MULTIPLE_ENTRIES)
    {
        s += ArrayBinaryWriter::GetUintXByteCount(EntryCount);
    }

    if (MessageFlags & MessageFlags_EXTENDED)
    {
        s += ArrayBinaryWriter::GetUintXByteCount(Extended.size());
        s += Extended.size();
    }

    s = ArrayBinaryWriter::GetSizePlusUintX(s);

    if (s > std::numeric_limits<uint32_t>::max())
        throw ProtocolException("MessageHeader exceeds maximum length");

    return boost::numeric_cast<uint32_t>(s);
}

void MessageHeader::UpdateHeader4(uint32_t message_entry_size, uint16_t entry_count)
{
    if (entry_count == 1)
    {
        MessageFlags &= ~MessageFlags_MULTIPLE_ENTRIES;
    }
    else
    {
        MessageFlags |= MessageFlags_MULTIPLE_ENTRIES;
    }

    if (MetaData.str().empty() && MessageID == 0 && MessageResID == 0)
    {
        MessageFlags &= ~MessageFlags_META_INFO;
    }
    else
    {
        MessageFlags |= MessageFlags_META_INFO;
    }

    if (Extended.empty())
    {
        MessageFlags &= ~MessageFlags_EXTENDED;
    }
    else
    {
        MessageFlags |= MessageFlags_EXTENDED;
    }

    EntryCount = entry_count;
    HeaderSize = ComputeSize4();
    MessageSize = message_entry_size + HeaderSize;
}

void MessageHeader::Write4(ArrayBinaryWriter& w)
{
    w.PushRelativeLimit(HeaderSize);
    w.WriteString8("RRAC");
    w.WriteNumber(MessageSize);
    w.WriteNumber(boost::numeric_cast<uint16_t>(4));

    w.WriteUintX(HeaderSize);
    w.WriteNumber(MessageFlags);

    if (MessageFlags & MessageFlags_ROUTING_INFO)
    {

        const boost::array<uint8_t, 16> bSenderNodeID = SenderNodeID.ToByteArray();
        const boost::array<uint8_t, 16> bReceiverNodeID = ReceiverNodeID.ToByteArray();
        for (int32_t i = 0; i < 16; i++)
        {
            w.WriteNumber(bSenderNodeID[i]);
        };
        for (int32_t i = 0; i < 16; i++)
        {
            w.WriteNumber(bReceiverNodeID[i]);
        };

        w.WriteString8WithXLen(SenderNodeName);
        w.WriteString8WithXLen(ReceiverNodeName);
    }

    if (MessageFlags & MessageFlags_ENDPOINT_INFO)
    {
        w.WriteUintX(SenderEndpoint);
        w.WriteUintX(ReceiverEndpoint);
    }

    if (MessageFlags & MessageFlags_PRIORITY)
    {
        w.WriteNumber(Priority);
    }

    if (MessageFlags & MessageFlags_META_INFO)
    {
        w.WriteString8WithXLen(MetaData);
        w.WriteNumber(MessageID);
        w.WriteNumber(MessageResID);
    }

    if (MessageFlags & MessageFlags_STRING_TABLE)
    {
        w.WriteUintX(StringTable.size());
        for (std::vector<boost::tuple<uint32_t, MessageStringPtr> >::iterator e = StringTable.begin();
             e != StringTable.end(); e++)
        {
            w.WriteUintX(e->get<0>());
            w.WriteString8WithXLen(e->get<1>());
        }
    }

    if (MessageFlags & MessageFlags_MULTIPLE_ENTRIES)
    {
        w.WriteUintX(EntryCount);
    }

    if (MessageFlags & MessageFlags_EXTENDED)
    {
        w.WriteUintX(Extended.size());
        if (!Extended.empty())
        {
            w.Write(&Extended[0], 0, Extended.size());
        }
    }

    if (w.DistanceFromLimit() != 0)
        throw DataSerializationException("Error in message format");
    w.PopLimit();
}

void MessageHeader::Read4(ArrayBinaryReader& r)
{
    MessageStringPtr magic = r.ReadString8(4).str();
    if (magic != "RRAC")
        throw ProtocolException("Incorrect message magic");
    MessageSize = r.ReadNumber<uint32_t>();
    uint16_t version = r.ReadNumber<uint16_t>();
    if (version != 4)
        throw ProtocolException("Unknown protocol version");

    HeaderSize = r.ReadUintX();

    r.PushRelativeLimit(HeaderSize - 10 - ArrayBinaryWriter::GetUintXByteCount(HeaderSize));

    MessageFlags = r.ReadNumber<uint8_t>();

    if (MessageFlags & MessageFlags_ROUTING_INFO)
    {

        boost::array<uint8_t, 16> bSenderNodeID = {};
        for (int32_t i = 0; i < 16; i++)
        {
            bSenderNodeID[i] = r.ReadNumber<uint8_t>();
        };
        SenderNodeID = NodeID(bSenderNodeID);

        boost::array<uint8_t, 16> bReceiverNodeID = {};
        for (int32_t i = 0; i < 16; i++)
        {
            bReceiverNodeID[i] = r.ReadNumber<uint8_t>();
        };
        ReceiverNodeID = NodeID(bReceiverNodeID);

        uint32_t pname_s = r.ReadUintX();
        SenderNodeName = r.ReadString8(pname_s);
        uint32_t pname_r = r.ReadUintX();
        ReceiverNodeName = r.ReadString8(pname_r);
    }

    if (MessageFlags & MessageFlags_ENDPOINT_INFO)
    {
        SenderEndpoint = r.ReadUintX();
        ReceiverEndpoint = r.ReadUintX();
    }

    if (MessageFlags & MessageFlags_PRIORITY)
    {
        Priority = r.ReadNumber<uint16_t>();
    }

    if (MessageFlags & MessageFlags_META_INFO)
    {
        uint32_t meta_s = r.ReadUintX();
        MetaData = r.ReadString8(meta_s);
        MessageID = r.ReadNumber<uint16_t>();
        MessageResID = r.ReadNumber<uint16_t>();
    }

    if (MessageFlags & MessageFlags_STRING_TABLE)
    {
        uint32_t s1 = r.ReadUintX();
        for (uint32_t i = 0; i < s1; i++)
        {
            uint32_t c = r.ReadUintX();
            uint32_t l = r.ReadUintX();
            MessageStringPtr v = r.ReadString8(l);
            StringTable.push_back(boost::make_tuple(c, v));
        }
    }

    if (MessageFlags & MessageFlags_MULTIPLE_ENTRIES)
    {
        uint32_t c = r.ReadUintX();
        if (c > std::numeric_limits<uint16_t>::max())
            throw ProtocolException("Too many entries in message");
        EntryCount = boost::numeric_cast<uint16_t>(c);
    }
    else
    {
        EntryCount = 1;
    }

    if (MessageFlags & MessageFlags_EXTENDED)
    {
        size_t l = r.ReadUintX();
        Extended.resize(l);
        if (l != 0)
        {
            r.Read(&Extended[0], 0, l);
        }
    }

    if (r.DistanceFromLimit() != 0)
        throw DataSerializationException("Error in message format");
    r.PopLimit();
}

MessageHeader::MessageHeader()
{
    SenderNodeName.reset();
    ReceiverNodeName.reset();

    MetaData.reset();
    ReceiverNodeID = NodeID();
    SenderNodeID = NodeID();

    MessageSize = 0;
    HeaderSize = 0;
    SenderEndpoint = 0;
    ReceiverEndpoint = 0;
    MessageID = 0;
    MessageResID = 0;
    EntryCount = 0;

    Extended.clear();

    MessageFlags = MessageFlags_Version2Compat;
    Priority = 0;
}

MessageEntry::MessageEntry()
{
    ServicePathCode = 0;
    MemberNameCode = 0;
    RequestID = 0;
    Error = MessageErrorType_None;
    EntryType = MessageEntryType_Null;
    EntrySize = 0;
    EntryFlags = MessageEntryFlags_Version2Compat;
    Extended.clear();
    elements.clear();
}

MessageEntry::MessageEntry(MessageEntryType t, MessageStringRef n)
{
    ServicePathCode = 0;
    MemberNameCode = 0;
    RequestID = 0;
    Error = MessageErrorType_None;
    EntryType = MessageEntryType_Null;
    EntrySize = 0;
    EntryFlags = MessageEntryFlags_Version2Compat;
    Extended.clear();
    elements.clear();
    EntryType = t;
    MemberName = n;
}

uint32_t MessageEntry::ComputeSize()
{
    uint64_t s = 22;
    BOOST_FOREACH (RR_INTRUSIVE_PTR<MessageElement>& e, elements)
    {
        e->UpdateData();
        s += e->ElementSize;
    }

    uint32_t s1 = boost::numeric_cast<uint32_t>(ArrayBinaryWriter::GetStringByteCount8(ServicePath));
    uint32_t s2 = boost::numeric_cast<uint32_t>(ArrayBinaryWriter::GetStringByteCount8(MemberName));
    uint32_t s3 = boost::numeric_cast<uint32_t>(ArrayBinaryWriter::GetStringByteCount8(MetaData));

    if (s1 > std::numeric_limits<uint16_t>::max())
        throw ProtocolException("ServicePath exceeds maximum length");
    if (s2 > std::numeric_limits<uint16_t>::max())
        throw ProtocolException("MemberName exceeds maximum length");
    if (s3 > std::numeric_limits<uint16_t>::max())
        throw ProtocolException("MessageEntry MetaData exceeds maximum length");

    s += s1 + s2 + s3;

    if (s > std::numeric_limits<uint32_t>::max())
        throw ProtocolException("MessageEntry exceeds maximum length");

    return boost::numeric_cast<uint32_t>(s);
}

RR_INTRUSIVE_PTR<MessageElement> MessageEntry::FindElement(MessageStringRef name)
{
    std::vector<RR_INTRUSIVE_PTR<MessageElement> >::iterator m =
        boost::find_if(elements, boost::bind(&MessageElement::ElementName, RR_BOOST_PLACEHOLDERS(_1)) == name);

    if (m == elements.end())
        throw MessageElementNotFoundException("Element " + name.str() + " not found.");

    return *m;
}

bool MessageEntry::TryFindElement(MessageStringRef name, RR_INTRUSIVE_PTR<MessageElement>& elem)
{
    std::vector<RR_INTRUSIVE_PTR<MessageElement> >::iterator m =
        boost::find_if(elements, boost::bind(&MessageElement::ElementName, RR_BOOST_PLACEHOLDERS(_1)) == name);

    if (m == elements.end())
        return false;

    elem = *m;
    return true;
}

RR_INTRUSIVE_PTR<MessageElement> MessageEntry::AddElement(MessageStringRef name,
                                                          const RR_INTRUSIVE_PTR<MessageElementData>& data)
{
    RR_INTRUSIVE_PTR<MessageElement> m = CreateMessageElement();
    m->ElementName = name;
    m->SetData(data);

    elements.push_back(m);

    return m;
}

RR_INTRUSIVE_PTR<MessageElement> MessageEntry::AddElement(const RR_INTRUSIVE_PTR<MessageElement>& m)
{

    elements.push_back(m);

    return m;
}

void MessageEntry::UpdateData()
{
    if (EntryFlags != MessageEntryFlags_Version2Compat)
    {
        throw ProtocolException("Invalid message flags for Version 2 message");
    }

    EntrySize = ComputeSize();
}

void MessageEntry::Write(ArrayBinaryWriter& w)
{
    UpdateData();

    w.PushRelativeLimit(EntrySize);

    w.WriteNumber(EntrySize);
    w.WriteNumber(static_cast<uint16_t>(EntryType));
    w.WriteNumber(static_cast<uint16_t>(0));

    w.WriteNumber(boost::numeric_cast<uint16_t>(ArrayBinaryWriter::GetStringByteCount8(ServicePath)));
    w.WriteString8(ServicePath);
    w.WriteNumber(boost::numeric_cast<uint16_t>(ArrayBinaryWriter::GetStringByteCount8(MemberName)));
    w.WriteString8(MemberName);
    w.WriteNumber(RequestID);
    w.WriteNumber(static_cast<uint16_t>(Error));
    w.WriteNumber(boost::numeric_cast<uint16_t>(ArrayBinaryWriter::GetStringByteCount8(MetaData)));
    w.WriteString8(MetaData);
    w.WriteNumber(boost::numeric_cast<uint16_t>(elements.size()));

    BOOST_FOREACH (RR_INTRUSIVE_PTR<MessageElement>& e, elements)
    {
        e->Write(w);
    }

    if (w.DistanceFromLimit() != 0)
        throw DataSerializationException("Error in message format");
    w.PopLimit();
}

void MessageEntry::Read(ArrayBinaryReader& r)
{
    EntrySize = r.ReadNumber<uint32_t>();

    r.PushRelativeLimit(EntrySize - 4);

    EntryType = boost::numeric_cast<MessageEntryType>(r.ReadNumber<uint16_t>());
    r.ReadNumber<uint16_t>();

    uint16_t sname_s = r.ReadNumber<uint16_t>();
    ServicePath = r.ReadString8(sname_s);
    uint16_t mname_s = r.ReadNumber<uint16_t>();
    MemberName = r.ReadString8(mname_s);
    RequestID = r.ReadNumber<uint32_t>();
    Error = boost::numeric_cast<MessageErrorType>(r.ReadNumber<uint16_t>());

    uint16_t metadata_s = r.ReadNumber<uint16_t>();
    MetaData = r.ReadString8(metadata_s);

    uint16_t ecount = r.ReadNumber<uint16_t>();

    elements = std::vector<RR_INTRUSIVE_PTR<MessageElement> >();
    elements.reserve(ecount);
    for (int32_t i = 0; i < ecount; i++)
    {
        RR_INTRUSIVE_PTR<MessageElement> e = CreateMessageElement();
        e->Read(r);
        elements.push_back(e);
    }

    if (r.DistanceFromLimit() != 0)
        throw DataSerializationException("Error in message format");
    r.PopLimit();
}

uint32_t MessageEntry::ComputeSize4()
{
    size_t s = 3;
    BOOST_FOREACH (RR_INTRUSIVE_PTR<MessageElement>& e, elements)
    {
        e->UpdateData4();
        s += e->ElementSize;
    }

    if (EntryFlags & MessageEntryFlags_SERVICE_PATH_STR)
    {
        s += boost::numeric_cast<uint32_t>(ArrayBinaryWriter::GetStringByteCount8WithXLen(ServicePath));
    }
    if (EntryFlags & MessageEntryFlags_SERVICE_PATH_CODE)
    {
        s += ArrayBinaryWriter::GetUintXByteCount(ServicePathCode);
    }

    if (EntryFlags & MessageEntryFlags_MEMBER_NAME_STR)
    {
        s += boost::numeric_cast<uint32_t>(ArrayBinaryWriter::GetStringByteCount8WithXLen(MemberName));
    }
    if (EntryFlags & MessageEntryFlags_MEMBER_NAME_CODE)
    {
        s += ArrayBinaryWriter::GetUintXByteCount(MemberNameCode);
    }

    if (EntryFlags & MessageEntryFlags_REQUEST_ID)
    {
        s += ArrayBinaryWriter::GetUintXByteCount(RequestID);
    }

    if (EntryFlags & MessageEntryFlags_ERROR)
    {
        s += 2;
    }

    if (EntryFlags & MessageEntryFlags_META_INFO)
    {
        s += boost::numeric_cast<uint32_t>(ArrayBinaryWriter::GetStringByteCount8WithXLen(MetaData));
    }

    if (EntryFlags & MessageEntryFlags_EXTENDED)
    {
        s += ArrayBinaryWriter::GetUintXByteCount(Extended.size());
        s += Extended.size();
    }

    s += ArrayBinaryWriter::GetUintXByteCount(elements.size());

    s = ArrayBinaryWriter::GetSizePlusUintX(s);

    return boost::numeric_cast<uint32_t>(s);
}

void MessageEntry::UpdateData4()
{

    if (RequestID != 0)
    {
        EntryFlags |= MessageEntryFlags_REQUEST_ID;
    }
    else
    {
        EntryFlags &= ~MessageEntryFlags_REQUEST_ID;
    }

    if (Error != 0)
    {
        EntryFlags |= MessageEntryFlags_ERROR;
    }
    else
    {
        EntryFlags &= ~MessageEntryFlags_ERROR;
    }

    if (!MetaData.str().empty())
    {
        EntryFlags |= MessageEntryFlags_META_INFO;
    }
    else
    {
        EntryFlags &= ~MessageEntryFlags_META_INFO;
    }

    if (Extended.empty())
    {
        EntryFlags &= ~MessageFlags_EXTENDED;
    }
    else
    {
        EntryFlags |= MessageFlags_EXTENDED;
    }

    EntrySize = ComputeSize4();
}

void MessageEntry::Write4(ArrayBinaryWriter& w)
{

    UpdateData4();

    w.PushRelativeLimit(EntrySize);

    w.WriteUintX(EntrySize);
    w.WriteNumber(EntryFlags);
    w.WriteNumber(static_cast<uint16_t>(EntryType));

    if (EntryFlags & MessageEntryFlags_SERVICE_PATH_STR)
    {
        w.WriteString8WithXLen(ServicePath);
    }

    if (EntryFlags & MessageEntryFlags_SERVICE_PATH_CODE)
    {
        w.WriteUintX(ServicePathCode);
    }

    if (EntryFlags & MessageEntryFlags_MEMBER_NAME_STR)
    {
        w.WriteString8WithXLen(MemberName);
    }

    if (EntryFlags & MessageEntryFlags_MEMBER_NAME_CODE)
    {
        w.WriteUintX(MemberNameCode);
    }

    if (EntryFlags & MessageEntryFlags_REQUEST_ID)
    {
        w.WriteUintX(RequestID);
    }

    if (EntryFlags & MessageEntryFlags_ERROR)
    {
        w.WriteNumber(static_cast<uint16_t>(Error));
    }

    if (EntryFlags & MessageEntryFlags_META_INFO)
    {
        w.WriteString8WithXLen(MetaData);
    }

    if (EntryFlags & MessageFlags_EXTENDED)
    {
        w.WriteUintX(Extended.size());
        if (!Extended.empty())
        {
            w.Write(&Extended[0], 0, Extended.size());
        }
    }

    w.WriteUintX(boost::numeric_cast<uint32_t>(elements.size()));

    BOOST_FOREACH (RR_INTRUSIVE_PTR<MessageElement>& e, elements)
    {
        e->Write4(w);
    }

    if (w.DistanceFromLimit() != 0)
        throw DataSerializationException("Error in message format");
    w.PopLimit();
}

void MessageEntry::Read4(ArrayBinaryReader& r)
{
    EntrySize = r.ReadUintX();

    r.PushRelativeLimit(EntrySize - ArrayBinaryWriter::GetUintXByteCount(EntrySize));

    EntryFlags = r.ReadNumber<uint8_t>();
    EntryType = boost::numeric_cast<MessageEntryType>(r.ReadNumber<uint16_t>());

    if (EntryFlags & MessageEntryFlags_SERVICE_PATH_STR)
    {
        uint32_t sname_s = r.ReadUintX();
        ServicePath = r.ReadString8(sname_s);
    }

    if (EntryFlags & MessageEntryFlags_SERVICE_PATH_CODE)
    {
        ServicePathCode = r.ReadUintX();
    }

    if (EntryFlags & MessageEntryFlags_MEMBER_NAME_STR)
    {
        uint32_t mname_s = r.ReadUintX();
        MemberName = r.ReadString8(mname_s);
    }

    if (EntryFlags & MessageEntryFlags_MEMBER_NAME_CODE)
    {
        MemberNameCode = r.ReadUintX();
    }

    if (EntryFlags & MessageEntryFlags_REQUEST_ID)
    {
        RequestID = r.ReadUintX();
    }

    if (EntryFlags & MessageEntryFlags_ERROR)
    {
        Error = boost::numeric_cast<MessageErrorType>(r.ReadNumber<uint16_t>());
    }

    if (EntryFlags & MessageEntryFlags_META_INFO)
    {
        uint32_t metadata_s = r.ReadUintX();
        MetaData = r.ReadString8(metadata_s);
    }

    if (EntryFlags & MessageFlags_EXTENDED)
    {
        size_t l = r.ReadUintX();
        Extended.resize(l);
        if (l != 0)
        {
            r.Read(&Extended[0], 0, l);
        }
    }

    uint32_t ecount = r.ReadUintX();

    elements = std::vector<RR_INTRUSIVE_PTR<MessageElement> >();
    elements.reserve(ecount);
    for (int32_t i = 0; i < ecount; i++)
    {
        RR_INTRUSIVE_PTR<MessageElement> e = CreateMessageElement();
        e->Read4(r);
        elements.push_back(e);
    }

    if (r.DistanceFromLimit() != 0)
        throw DataSerializationException("Error in message format");
    r.PopLimit();
}

MessageElement::MessageElement()
{
    ElementSize = 0;
    DataCount = 0;

    ElementFlags = MessageElementFlags_Version2Compat;
    ElementNameCode = 0;
    ElementNumber = 0;
    ElementTypeNameCode = 0;

    ElementType = DataTypes_void_t;
    Extended.clear();
}

MessageElement::MessageElement(MessageStringRef name, const RR_INTRUSIVE_PTR<MessageElementData>& datin)
{
    ElementSize = 0;
    DataCount = 0;

    ElementFlags = MessageElementFlags_Version2Compat;
    ElementNameCode = 0;
    ElementNumber = 0;
    ElementTypeNameCode = 0;

    ElementType = DataTypes_void_t;

    ElementName = name;
    SetData(datin);
    // UpdateData();
}

RR_INTRUSIVE_PTR<MessageElementData> MessageElement::GetData() { return dat; }

void MessageElement::SetData(const RR_INTRUSIVE_PTR<MessageElementData>& value)
{
    dat = value;

    if (value)
    {
        ElementType = value->GetTypeID();
    }
    else
    {
        ElementType = DataTypes_void_t;
    }

    ElementSize = std::numeric_limits<uint32_t>::max();
    // UpdateData();
}

uint32_t MessageElement::ComputeSize()
{
    uint64_t s = 16;
    uint32_t s1 = boost::numeric_cast<uint32_t>(ArrayBinaryWriter::GetStringByteCount8(ElementName));
    uint32_t s2 = boost::numeric_cast<uint32_t>(ArrayBinaryWriter::GetStringByteCount8(ElementTypeName));
    uint32_t s3 = boost::numeric_cast<uint32_t>(ArrayBinaryWriter::GetStringByteCount8(MetaData));

    if (s1 > std::numeric_limits<uint16_t>::max())
        throw ProtocolException("ElementName exceeds maximum length");
    if (s2 > std::numeric_limits<uint16_t>::max())
        throw ProtocolException("ElementTypeName exceeds maximum length");
    if (s3 > std::numeric_limits<uint16_t>::max())
        throw ProtocolException("MessageElement MetaData exceeds maximum length");

    s += s1 + s2 + s3;

    switch (ElementType)
    {
    case DataTypes_void_t:
        break;
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
    case DataTypes_bool_t:
        s += DataCount * RRArrayElementSize(ElementType);
        break;
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
        RR_INTRUSIVE_PTR<MessageElementNestedElementList> d = rr_cast<MessageElementNestedElementList>(GetData());
        BOOST_FOREACH (RR_INTRUSIVE_PTR<MessageElement>& e, d->Elements)
        {
            e->UpdateData();
            s += e->ElementSize;
        }
        break;
    }
    default:
        throw DataTypeException("Unknown data type");
    }

    if (s > std::numeric_limits<uint32_t>::max())
        throw ProtocolException("MessageElement exceeds maximum length");

    return boost::numeric_cast<uint32_t>(s);
}

void MessageElement::UpdateData()
{
    if (ElementFlags & MessageElementFlags_ELEMENT_NUMBER && !(ElementFlags & MessageElementFlags_ELEMENT_NAME_STR))
    {
        ElementName = boost::lexical_cast<std::string>(ElementNumber);
        ElementFlags &= ~MessageElementFlags_ELEMENT_NUMBER;
        ElementFlags |= MessageElementFlags_ELEMENT_NAME_STR;
    }

    if (ElementFlags != MessageElementFlags_Version2Compat)
    {
        throw ProtocolException("Invalid message flags for Version 2 message");
    }

    std::string datatype;
    if (!dat)
        ElementType = DataTypes_void_t;
    else
        ElementType = dat->GetTypeID();

    ElementTypeName = "";
    switch (ElementType)
    {
    case DataTypes_void_t:
        DataCount = 0;
        break;
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
        RR_INTRUSIVE_PTR<RRBaseArray> rdat = RR_DYNAMIC_POINTER_CAST<RRBaseArray>(dat);
        if (!rdat)
            throw DataTypeException("");
        DataCount = boost::numeric_cast<uint32_t>(rdat->size());
        break;
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
        RR_INTRUSIVE_PTR<MessageElementNestedElementList> sdat =
            RR_DYNAMIC_POINTER_CAST<MessageElementNestedElementList>(dat);
        if (!sdat)
            throw DataTypeException("");
        DataCount = boost::numeric_cast<uint32_t>(sdat->Elements.size());
        ElementTypeName = sdat->GetTypeString();
        break;
    }

    default:
        throw DataTypeException("Unknown data type");
    }

    ElementSize = ComputeSize();
}

void MessageElement::Write(ArrayBinaryWriter& w)
{

    UpdateData();

    w.PushRelativeLimit(ElementSize);

    w.WriteNumber(ElementSize);
    w.WriteNumber(boost::numeric_cast<uint16_t>(ArrayBinaryWriter::GetStringByteCount8(ElementName)));
    w.WriteString8(ElementName);
    w.WriteNumber(static_cast<uint16_t>(ElementType));
    w.WriteNumber(boost::numeric_cast<uint16_t>(ArrayBinaryWriter::GetStringByteCount8(ElementTypeName)));
    w.WriteString8(ElementTypeName);
    w.WriteNumber(boost::numeric_cast<uint16_t>(ArrayBinaryWriter::GetStringByteCount8(MetaData)));
    w.WriteString8(MetaData);
    w.WriteNumber(boost::numeric_cast<uint32_t>(DataCount));

    switch (ElementType)
    {
    case DataTypes_void_t:
        DataCount = 0;
        break;
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
        RR_INTRUSIVE_PTR<RRBaseArray> rdat = RR_STATIC_POINTER_CAST<RRBaseArray>(dat);
        if (!rdat)
            throw DataTypeException("");
        w.WriteArray(rdat);
        break;
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
        RR_INTRUSIVE_PTR<MessageElementNestedElementList> sdat =
            RR_STATIC_POINTER_CAST<MessageElementNestedElementList>(dat);
        if (!sdat)
            throw DataTypeException("Expected MessageElementNestedElementList");
        BOOST_FOREACH (RR_INTRUSIVE_PTR<MessageElement>& e, sdat->Elements)
            e->Write(w);
        break;
    }
    default:
        throw DataTypeException("Unknown data type");
    }

    if (w.DistanceFromLimit() != 0)
        throw DataSerializationException("Error in message format");
    w.PopLimit();
}

void MessageElement::Read(ArrayBinaryReader& r)
{
    ElementSize = r.ReadNumber<uint32_t>();

    r.PushRelativeLimit(ElementSize - 4);

    uint16_t name_s = r.ReadNumber<uint16_t>();
    ElementName = r.ReadString8(name_s);
    ElementType = static_cast<DataTypes>(r.ReadNumber<uint16_t>());
    uint16_t nametype_s = r.ReadNumber<uint16_t>();
    ElementTypeName = r.ReadString8(nametype_s);
    uint16_t metadata_s = r.ReadNumber<uint16_t>();
    MetaData = r.ReadString8(metadata_s);
    DataCount = r.ReadNumber<uint32_t>();

    switch (ElementType)
    {
    case DataTypes_void_t:
        break;
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
        if (boost::numeric_cast<int32_t>(RRArrayElementSize(ElementType) * DataCount) > r.DistanceFromLimit())
        {
            throw DataSerializationException("Error in message format");
        }

        RR_INTRUSIVE_PTR<RRBaseArray> d = AllocateRRArrayByType(ElementType, DataCount);
        r.ReadArray(d);
        dat = d;
        break;
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
        std::vector<RR_INTRUSIVE_PTR<MessageElement> > l;
        l.reserve(DataCount);
        for (size_t i = 0; i < DataCount; i++)
        {
            RR_INTRUSIVE_PTR<MessageElement> m = CreateMessageElement();
            m->Read(r);
            l.push_back(m);
        }

        dat = CreateMessageElementNestedElementList(ElementType, ElementTypeName, RR_MOVE(l));
        break;
    }
    default:
        throw DataTypeException("Unknown data type");
    }

    if (r.DistanceFromLimit() != 0)
        throw DataSerializationException("Error in message format");
    r.PopLimit();
}

uint32_t MessageElement::ComputeSize4()
{
    size_t s = 3;

    if (ElementFlags & MessageElementFlags_ELEMENT_NAME_STR)
    {
        s += boost::numeric_cast<uint32_t>(ArrayBinaryWriter::GetStringByteCount8WithXLen(ElementName));
    }
    if (ElementFlags & MessageElementFlags_ELEMENT_NAME_CODE)
    {
        s += ArrayBinaryWriter::GetUintXByteCount(ElementNameCode);
    }

    if (ElementFlags & MessageElementFlags_ELEMENT_NUMBER)
    {
        s += ArrayBinaryWriter::GetIntXByteCount(ElementNumber);
    }

    if (ElementFlags & MessageElementFlags_ELEMENT_TYPE_NAME_STR)
    {
        s += boost::numeric_cast<uint32_t>(ArrayBinaryWriter::GetStringByteCount8WithXLen(ElementTypeName));
    }
    if (ElementFlags & MessageElementFlags_ELEMENT_TYPE_NAME_CODE)
    {
        s += ArrayBinaryWriter::GetUintXByteCount(ElementTypeNameCode);
    }

    if (ElementFlags & MessageElementFlags_META_INFO)
    {
        s += boost::numeric_cast<uint32_t>(ArrayBinaryWriter::GetStringByteCount8WithXLen(MetaData));
    }

    if (ElementFlags & MessageElementFlags_EXTENDED)
    {
        s += ArrayBinaryWriter::GetUintXByteCount(Extended.size());
        s += Extended.size();
    }

    switch (ElementType)
    {
    case DataTypes_void_t:
        break;
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
    case DataTypes_bool_t:
        s += DataCount * RRArrayElementSize(ElementType);
        break;
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
        RR_INTRUSIVE_PTR<MessageElementNestedElementList> d = rr_cast<MessageElementNestedElementList>(GetData());
        BOOST_FOREACH (RR_INTRUSIVE_PTR<MessageElement>& e, d->Elements)
        {
            e->UpdateData4();
            s += e->ElementSize;
        }
        break;
    }
    default:
        throw DataTypeException("Unknown data type");
    }

    s += ArrayBinaryWriter::GetUintXByteCount(DataCount);

    s = ArrayBinaryWriter::GetSizePlusUintX(s);

    return boost::numeric_cast<uint32_t>(s);
}

void MessageElement::UpdateData4()
{

    std::string datatype;
    if (!dat)
        ElementType = DataTypes_void_t;
    else
        ElementType = dat->GetTypeID();

    ElementTypeName.reset();
    switch (ElementType)
    {
    case DataTypes_void_t:
        DataCount = 0;
        break;
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
        RR_INTRUSIVE_PTR<RRBaseArray> rdat = RR_STATIC_POINTER_CAST<RRBaseArray>(dat);
        if (!rdat)
            throw DataTypeException("");
        DataCount = boost::numeric_cast<uint32_t>(rdat->size());
        break;
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
        RR_INTRUSIVE_PTR<MessageElementNestedElementList> sdat =
            RR_STATIC_POINTER_CAST<MessageElementNestedElementList>(dat);
        if (!sdat)
            throw DataTypeException("");
        DataCount = boost::numeric_cast<uint32_t>(sdat->Elements.size());
        // If flags have ElementTypeNameCode set, assume that the ElementTypeName has already been encoded
        if ((ElementFlags & MessageElementFlags_ELEMENT_TYPE_NAME_CODE) == 0)
        {
            ElementTypeName = sdat->GetTypeString();
        }
        break;
    }

    default:
        throw DataTypeException("Unknown data type");
    }

    // TODO: string table here

    /*if (ElementName.size() > 0)
    {
        ElementFlags |= MessageElementFlags_ELEMENT_NAME_STR;
    }
    else
    {
        ElementFlags &= ~MessageElementFlags_ELEMENT_NAME_STR;
    }
    if (ElementNameCode != 0)
    {
        throw NotImplementedException("Not implemented");
    }
    ElementFlags &= ~MessageElementFlags_ELEMENT_NAME_CODE;
    */

    if ((ElementFlags & (MessageElementFlags_ELEMENT_NAME_STR | MessageElementFlags_ELEMENT_NAME_CODE)) &&
        (ElementFlags & MessageElementFlags_ELEMENT_NUMBER))
    {
        throw ProtocolException("Cannot set both element name and number");
    }

    if (!ElementTypeName.str().empty())
    {
        ElementFlags |= MessageElementFlags_ELEMENT_TYPE_NAME_STR;
    }
    else
    {
        ElementFlags &= ~MessageElementFlags_ELEMENT_TYPE_NAME_STR;
    }

    if (!MetaData.str().empty())
    {
        ElementFlags |= MessageElementFlags_META_INFO;
    }
    else
    {
        ElementFlags &= ~MessageElementFlags_META_INFO;
    }

    if (Extended.empty())
    {
        ElementFlags &= ~MessageElementFlags_EXTENDED;
    }
    else
    {
        ElementFlags |= MessageElementFlags_EXTENDED;
    }

    ElementSize = ComputeSize4();
}

void MessageElement::Write4(ArrayBinaryWriter& w)
{
    UpdateData4();

    w.PushRelativeLimit(ElementSize);

    w.WriteUintX(ElementSize);
    w.WriteNumber(ElementFlags);
    if (ElementFlags & MessageElementFlags_ELEMENT_NAME_STR)
        w.WriteString8WithXLen(ElementName);
    if (ElementFlags & MessageElementFlags_ELEMENT_NAME_CODE)
        w.WriteUintX(ElementNameCode);
    if (ElementFlags & MessageElementFlags_ELEMENT_NUMBER)
        w.WriteIntX(ElementNumber);
    w.WriteNumber(static_cast<uint16_t>(ElementType));
    if (ElementFlags & MessageElementFlags_ELEMENT_TYPE_NAME_STR)
        w.WriteString8WithXLen(ElementTypeName);
    if (ElementFlags & MessageElementFlags_ELEMENT_TYPE_NAME_CODE)
        w.WriteUintX(ElementTypeNameCode);
    if (ElementFlags & MessageElementFlags_META_INFO)
        w.WriteString8WithXLen(MetaData);
    if (ElementFlags & MessageElementFlags_EXTENDED)
    {
        w.WriteUintX(Extended.size());
        if (!Extended.empty())
        {
            w.Write(&Extended[0], 0, Extended.size());
        }
    }
    w.WriteUintX(boost::numeric_cast<uint32_t>(DataCount));

    switch (ElementType)
    {
    case DataTypes_void_t:
        break;
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
        RR_INTRUSIVE_PTR<RRBaseArray> rdat = RR_STATIC_POINTER_CAST<RRBaseArray>(dat);
        if (!rdat)
            throw DataTypeException("");
        w.WriteArray(rdat);
        break;
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
        RR_INTRUSIVE_PTR<MessageElementNestedElementList> sdat =
            RR_STATIC_POINTER_CAST<MessageElementNestedElementList>(dat);
        if (!sdat)
            throw DataTypeException("");
        BOOST_FOREACH (RR_INTRUSIVE_PTR<MessageElement>& e, sdat->Elements)
            e->Write4(w);
        break;
    }

    default:
        throw DataTypeException("Unknown data type");
    }

    if (w.DistanceFromLimit() != 0)
        throw DataSerializationException("Error in message format");
    w.PopLimit();
}

void MessageElement::Read4(ArrayBinaryReader& r)
{

    ElementSize = r.ReadUintX();

    r.PushRelativeLimit(ElementSize - ArrayBinaryWriter::GetUintXByteCount(ElementSize));

    ElementFlags = r.ReadNumber<uint8_t>();

    if (ElementFlags & MessageElementFlags_ELEMENT_NAME_STR)
    {
        uint32_t name_s = r.ReadUintX();
        ElementName = r.ReadString8(name_s);
    }

    if (ElementFlags & MessageElementFlags_ELEMENT_NAME_CODE)
    {
        ElementNameCode = r.ReadUintX();
    }

    if (ElementFlags & MessageElementFlags_ELEMENT_NUMBER)
    {
        ElementNumber = r.ReadIntX();
    }

    ElementType = static_cast<DataTypes>(r.ReadNumber<uint16_t>());

    if (ElementFlags & MessageElementFlags_ELEMENT_TYPE_NAME_STR)
    {
        uint32_t nametype_s = r.ReadUintX();
        ElementTypeName = r.ReadString8(nametype_s);
    }

    if (ElementFlags & MessageElementFlags_ELEMENT_TYPE_NAME_CODE)
    {
        ElementTypeNameCode = r.ReadUintX();
    }

    if (ElementFlags & MessageElementFlags_META_INFO)
    {
        uint32_t metadata_s = r.ReadUintX();
        MetaData = r.ReadString8(metadata_s);
    }

    if (ElementFlags & MessageElementFlags_EXTENDED)
    {
        size_t l = r.ReadUintX();
        Extended.resize(l);
        if (l != 0)
        {
            r.Read(&Extended[0], 0, l);
        }
    }

    DataCount = r.ReadUintX();

    switch (ElementType)
    {
    case DataTypes_void_t:
        break;
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
        if (boost::numeric_cast<int32_t>(RRArrayElementSize(ElementType) * DataCount) > r.DistanceFromLimit())
        {
            throw DataSerializationException("Error in message format");
        }

        RR_INTRUSIVE_PTR<RRBaseArray> d = AllocateRRArrayByType(ElementType, DataCount);
        r.ReadArray(d);
        dat = d;
        break;
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
        std::vector<RR_INTRUSIVE_PTR<MessageElement> > l;
        l.reserve(DataCount);
        for (size_t i = 0; i < DataCount; i++)
        {
            RR_INTRUSIVE_PTR<MessageElement> m = CreateMessageElement();
            m->Read4(r);
            l.push_back(m);
        }

        dat = CreateMessageElementNestedElementList(ElementType, ElementTypeName, RR_MOVE(l));
        break;
    }
    default:
        throw ProtocolException("Invalid message element type");
    }

    if (r.DistanceFromLimit() != 0)
        throw DataSerializationException("Error in message format");
    r.PopLimit();
}

RR_INTRUSIVE_PTR<MessageElement> MessageElement::FindElement(std::vector<RR_INTRUSIVE_PTR<MessageElement> >& m,
                                                             MessageStringRef name)
{
    std::vector<RR_INTRUSIVE_PTR<MessageElement> >::iterator m1 =
        boost::find_if(m, boost::bind(&MessageElement::ElementName, RR_BOOST_PLACEHOLDERS(_1)) == name);

    if (m1 == m.end())
        throw MessageElementNotFoundException("Element " + name.str() + " not found.");

    return *m1;
}

bool MessageElement::TryFindElement(std::vector<RR_INTRUSIVE_PTR<MessageElement> >& m, MessageStringRef name,
                                    RR_INTRUSIVE_PTR<MessageElement>& elem)
{
    std::vector<RR_INTRUSIVE_PTR<MessageElement> >::iterator m1 =
        boost::find_if(m, boost::bind(&MessageElement::ElementName, RR_BOOST_PLACEHOLDERS(_1)) == name);

    if (m1 == m.end())
        return false;

    elem = *m1;
    return true;
}

bool MessageElement::ContainsElement(std::vector<RR_INTRUSIVE_PTR<MessageElement> >& m, MessageStringRef name)
{
    std::vector<RR_INTRUSIVE_PTR<MessageElement> >::iterator m1 =
        boost::find_if(m, boost::bind(&MessageElement::ElementName, RR_BOOST_PLACEHOLDERS(_1)) == name);

    return (m1 != m.end());
}

std::string MessageElement::CastDataToString()
{
    RR_INTRUSIVE_PTR<RRArray<char> > datarr = CastData<RRArray<char> >();
    return RRArrayToString(datarr);
}

RR_INTRUSIVE_PTR<MessageElementNestedElementList> MessageElement::CastDataToNestedList()
{
    return CastData<MessageElementNestedElementList>();
}

RR_INTRUSIVE_PTR<MessageElementNestedElementList> MessageElement::CastDataToNestedList(DataTypes expected_type)
{
    RR_INTRUSIVE_PTR<MessageElementNestedElementList> l = CastData<MessageElementNestedElementList>();
    if (l && l->GetTypeID() != expected_type)
    {
        throw DataTypeMismatchException("Unexpected MessageElementNestedElementList type");
    }
    return l;
}

MessageElementNestedElementList::MessageElementNestedElementList(
    DataTypes type_, MessageStringRef type_name_, const std::vector<RR_INTRUSIVE_PTR<MessageElement> >& elements_)
{
    Elements = elements_;
    TypeName = type_name_;
    Type = type_;
}
#if !defined(BOOST_NO_CXX11_RVALUE_REFERENCES)
MessageElementNestedElementList::MessageElementNestedElementList(
    DataTypes type_, MessageStringRef type_name_, std::vector<RR_INTRUSIVE_PTR<MessageElement> >&& elements_)
    : Elements(std::move(elements_))
{
    TypeName = type_name_;
    Type = type_;
}
#endif

MessageStringPtr MessageElementNestedElementList::GetTypeString() { return TypeName; }
DataTypes MessageElementNestedElementList::GetTypeID() { return Type; }
std::string MessageElementNestedElementList::RRType() { return "RobotRaconteur::MessageElementNestedElementList"; }

ROBOTRACONTEUR_CORE_API RR_INTRUSIVE_PTR<Message> CreateMessage() { return new Message(); }
ROBOTRACONTEUR_CORE_API RR_INTRUSIVE_PTR<MessageHeader> CreateMessageHeader() { return new MessageHeader(); }
ROBOTRACONTEUR_CORE_API RR_INTRUSIVE_PTR<MessageEntry> CreateMessageEntry() { return new MessageEntry(); }
ROBOTRACONTEUR_CORE_API RR_INTRUSIVE_PTR<MessageEntry> CreateMessageEntry(MessageEntryType t, MessageStringRef n)
{
    return new MessageEntry(t, n);
}
ROBOTRACONTEUR_CORE_API RR_INTRUSIVE_PTR<MessageElement> CreateMessageElement() { return new MessageElement(); }
ROBOTRACONTEUR_CORE_API RR_INTRUSIVE_PTR<MessageElement> CreateMessageElement(
    MessageStringRef name, const RR_INTRUSIVE_PTR<MessageElementData>& datin)
{
    return new MessageElement(name, datin);
}
ROBOTRACONTEUR_CORE_API RR_INTRUSIVE_PTR<MessageElement> CreateMessageElement(
    int32_t number, const RR_INTRUSIVE_PTR<MessageElementData>& datin)
{
    MessageElement* m = new MessageElement("", datin);
    m->ElementFlags &= ~MessageElementFlags_ELEMENT_NAME_STR;
    m->ElementFlags |= MessageElementFlags_ELEMENT_NUMBER;
    m->ElementNumber = number;
    return m;
}
ROBOTRACONTEUR_CORE_API RR_INTRUSIVE_PTR<MessageElementNestedElementList> CreateMessageElementNestedElementList(
    DataTypes type_, MessageStringRef type_name_, const std::vector<RR_INTRUSIVE_PTR<MessageElement> >& elements_)
{
    return new MessageElementNestedElementList(type_, type_name_, elements_);
}
#if !defined(BOOST_NO_CXX11_RVALUE_REFERENCES)
ROBOTRACONTEUR_CORE_API RR_INTRUSIVE_PTR<MessageElementNestedElementList> CreateMessageElementNestedElementList(
    DataTypes type_, MessageStringRef type_name_, std::vector<RR_INTRUSIVE_PTR<MessageElement> >&& elements_)
{
    return new MessageElementNestedElementList(type_, type_name_, std::move(elements_));
}
#endif
RR_INTRUSIVE_PTR<Message> ShallowCopyMessage(const RR_INTRUSIVE_PTR<Message>& m)
{
    if (!m)
        return RR_INTRUSIVE_PTR<Message>();

    RR_INTRUSIVE_PTR<Message> m2 = CreateMessage();
    if (m->header)
    {
        RR_INTRUSIVE_PTR<MessageHeader>& h = m->header;
        RR_INTRUSIVE_PTR<MessageHeader> h2 = CreateMessageHeader();
        h2->MessageSize = h->MessageSize;
        h2->HeaderSize = h->HeaderSize;
        h2->MessageFlags = h->MessageFlags;
        h2->SenderEndpoint = h->SenderEndpoint;
        h2->ReceiverEndpoint = h->ReceiverEndpoint;
        h2->SenderNodeName = h->SenderNodeName;
        h2->ReceiverNodeName = h->ReceiverNodeName;
        h2->SenderNodeID = h->SenderNodeID;
        h2->ReceiverNodeID = h->ReceiverNodeID;
        h2->MetaData = h->MetaData;
        h2->EntryCount = h->EntryCount;
        h2->MessageID = h->MessageID;
        h2->MessageResID = h->MessageResID;
        h2->StringTable = h->StringTable;
        h2->Extended = h->Extended;
        m2->header = h2;
    }

    BOOST_FOREACH (RR_INTRUSIVE_PTR<MessageEntry>& e, m->entries)
    {
        m2->entries.push_back(ShallowCopyMessageEntry(e));
    }

    return m2;
}

RR_INTRUSIVE_PTR<MessageEntry> ShallowCopyMessageEntry(const RR_INTRUSIVE_PTR<MessageEntry>& mm)
{
    if (!mm)
        return RR_INTRUSIVE_PTR<MessageEntry>();

    RR_INTRUSIVE_PTR<MessageEntry> mm2 = CreateMessageEntry();
    mm2->EntrySize = mm->EntrySize;
    mm2->EntryFlags = mm->EntryFlags;
    mm2->EntryType = mm->EntryType;
    mm2->ServicePath = mm->ServicePath;
    mm2->ServicePathCode = mm->ServicePathCode;
    mm2->MemberName = mm->MemberName;
    mm2->MemberNameCode = mm->MemberNameCode;
    mm2->RequestID = mm->RequestID;
    mm2->Error = mm->Error;
    mm2->MetaData = mm->MetaData;
    mm2->Extended = mm->Extended;

    BOOST_FOREACH (RR_INTRUSIVE_PTR<MessageElement>& e, mm->elements)
    {
        mm2->elements.push_back(ShallowCopyMessageElement(e));
    }

    return mm2;
}

RR_INTRUSIVE_PTR<MessageElement> ShallowCopyMessageElement(const RR_INTRUSIVE_PTR<MessageElement>& mm)
{
    if (!mm)
        return RR_INTRUSIVE_PTR<MessageElement>();

    RR_INTRUSIVE_PTR<MessageElement> mm2 = CreateMessageElement();
    mm2->ElementSize = mm->ElementSize;
    mm2->ElementFlags = mm->ElementFlags;
    mm2->ElementName = mm->ElementName;
    mm2->ElementNameCode = mm->ElementNameCode;
    mm2->ElementNumber = mm->ElementNumber;
    mm2->ElementType = mm->ElementType;
    mm2->ElementTypeName = mm->ElementTypeName;
    mm2->ElementTypeNameCode = mm->ElementTypeNameCode;
    mm2->MetaData = mm->MetaData;
    mm2->DataCount = mm->DataCount;
    mm2->Extended = mm->Extended;

    switch (mm->ElementType)
    {

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
        RR_INTRUSIVE_PTR<MessageElementNestedElementList> sdat = mm->CastData<MessageElementNestedElementList>();
        if (sdat)
        {
            std::vector<RR_INTRUSIVE_PTR<MessageElement> > v;
            v.reserve(sdat->Elements.size());
            BOOST_FOREACH (RR_INTRUSIVE_PTR<MessageElement>& ee, sdat->Elements)
                v.push_back(ShallowCopyMessageElement(ee));

            RR_INTRUSIVE_PTR<MessageElementNestedElementList> sdat2 =
                CreateMessageElementNestedElementList(sdat->Type, sdat->TypeName, RR_MOVE(v));
            mm2->SetData(sdat2);
        }
        break;
    }
    default:
        mm2->SetData(mm->GetData());
        break;
    }

    return mm2;
}

bool MessageElement_GetElementNumber(const RR_INTRUSIVE_PTR<MessageElement>& m, int32_t& number)
{
    if (m->ElementFlags & MessageElementFlags_ELEMENT_NUMBER)
    {
        number = m->ElementNumber;
        return true;
    }
    else if (m->ElementFlags & MessageElementFlags_ELEMENT_NAME_STR)
    {
        int32_t _number = 0;
        if (!boost::conversion::try_lexical_convert<int32_t>(m->ElementName.str(), _number))
        {
            return false;
        }
        number = _number;
        return true;
    }
    else
    {
        return false;
    }
}

void MessageElement_SetElementNumber(const RR_INTRUSIVE_PTR<MessageElement>& m, int32_t number)
{
    m->ElementFlags &= ~MessageElementFlags_ELEMENT_NAME_STR;
    m->ElementFlags |= MessageElementFlags_ELEMENT_NUMBER;
    m->ElementNumber = number;
}

bool MessageElement_GetElementName(const RR_INTRUSIVE_PTR<MessageElement>& m, MessageStringPtr& name)
{
    if (!(m->ElementFlags & MessageElementFlags_ELEMENT_NAME_STR))
    {
        return false;
    }

    name = m->ElementName;
    return true;
}
} // namespace RobotRaconteur
