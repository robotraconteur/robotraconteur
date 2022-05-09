/**
 * @file Message.h
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
#include "RobotRaconteur/NodeID.h"

#include <boost/intrusive/list.hpp>

namespace RobotRaconteur
{

class ROBOTRACONTEUR_CORE_API ArrayBinaryReader;
class ROBOTRACONTEUR_CORE_API ArrayBinaryWriter;
class ROBOTRACONTEUR_CORE_API NodeID;

class ROBOTRACONTEUR_CORE_API Message;
class ROBOTRACONTEUR_CORE_API MessageHeader;
class ROBOTRACONTEUR_CORE_API MessageEntry;
class ROBOTRACONTEUR_CORE_API MessageElement;
class ROBOTRACONTEUR_CORE_API MessageElementNestedElementList;

class ROBOTRACONTEUR_CORE_API Message : public RRValue
{
  public:
    RR_INTRUSIVE_PTR<MessageHeader> header;
    std::vector<RR_INTRUSIVE_PTR<MessageEntry> > entries;

    Message();

    RR_INTRUSIVE_PTR<MessageEntry> FindEntry(MessageStringRef name);

    RR_INTRUSIVE_PTR<MessageEntry> AddEntry(MessageEntryType t, MessageStringRef name);

    // Version 2 Message
    uint32_t ComputeSize();
    void Write(ArrayBinaryWriter& w);
    void Read(ArrayBinaryReader& r);

    // Version 4 Message
    uint32_t ComputeSize4();
    void Write4(ArrayBinaryWriter& w);
    void Read4(ArrayBinaryReader& r);

    RR_OVIRTUAL std::string RRType() RR_OVERRIDE { return "RobotRaconteur::Message"; }
};

class ROBOTRACONTEUR_CORE_API MessageHeader : public RRValue
{
  public:
    RR_OVIRTUAL ~MessageHeader() RR_OVERRIDE {}

    uint32_t HeaderSize;

    uint8_t MessageFlags;

    uint32_t SenderEndpoint;

    uint32_t ReceiverEndpoint;

    MessageStringPtr SenderNodeName;

    MessageStringPtr ReceiverNodeName;

    NodeID SenderNodeID;

    NodeID ReceiverNodeID;

    MessageStringPtr MetaData;

    uint16_t EntryCount;

    uint16_t MessageID;

    uint16_t MessageResID;

    uint32_t MessageSize;

    std::vector<boost::tuple<uint32_t, MessageStringPtr> > StringTable;

    uint16_t Priority;

    std::vector<uint8_t> Extended;

    // Version 2 Message
    uint16_t ComputeSize();
    void UpdateHeader(uint32_t message_size, uint16_t entry_count);
    void Write(ArrayBinaryWriter& w);
    void Read(ArrayBinaryReader& r);

    // Version 4 Message
  protected:
    uint32_t ComputeSize4();

  public:
    void UpdateHeader4(uint32_t message_entry_size, uint16_t entry_count);
    void Write4(ArrayBinaryWriter& w);
    void Read4(ArrayBinaryReader& r);

    RR_OVIRTUAL std::string RRType() RR_OVERRIDE { return "RobotRaconteur::MessageHeader"; }

    MessageHeader();
};

class ROBOTRACONTEUR_CORE_API MessageEntry : public RRValue
{
  public:
    RR_OVIRTUAL ~MessageEntry() RR_OVERRIDE {}

    uint32_t EntrySize;

    uint8_t EntryFlags;

    MessageEntryType EntryType;

    MessageStringPtr ServicePath;

    uint32_t ServicePathCode;

    MessageStringPtr MemberName;

    uint32_t MemberNameCode;

    uint32_t RequestID;

    MessageErrorType Error;

    MessageStringPtr MetaData;

    std::vector<uint8_t> Extended;

    std::vector<RR_INTRUSIVE_PTR<MessageElement> > elements;

    MessageEntry();

    MessageEntry(MessageEntryType t, MessageStringRef n);

    RR_INTRUSIVE_PTR<MessageElement> FindElement(MessageStringRef name);

    bool TryFindElement(MessageStringRef name, RR_INTRUSIVE_PTR<MessageElement>& elem);

    RR_INTRUSIVE_PTR<MessageElement> AddElement(MessageStringRef name,
                                                const RR_INTRUSIVE_PTR<MessageElementData>& data);

    RR_INTRUSIVE_PTR<MessageElement> AddElement(const RR_INTRUSIVE_PTR<MessageElement>& m);

    // Version 2 Message
    uint32_t ComputeSize();
    void UpdateData();
    void Write(ArrayBinaryWriter& w);
    void Read(ArrayBinaryReader& r);

    // Version 4 Message
    uint32_t ComputeSize4();
    void UpdateData4();
    void Write4(ArrayBinaryWriter& w);
    void Read4(ArrayBinaryReader& r);

    RR_OVIRTUAL std::string RRType() RR_OVERRIDE { return "RobotRaconteur::MessageEntry"; }
};

class ROBOTRACONTEUR_CORE_API MessageElement : public RRValue
{
  public:
    RR_OVIRTUAL ~MessageElement() RR_OVERRIDE {}

    uint32_t ElementSize;

    uint8_t ElementFlags;

    MessageStringPtr ElementName;

    uint32_t ElementNameCode;

    int32_t ElementNumber;

    DataTypes ElementType;

    MessageStringPtr ElementTypeName;

    uint32_t ElementTypeNameCode;

    MessageStringPtr MetaData;

    std::vector<uint8_t> Extended;

    uint32_t DataCount;

  private:
    RR_INTRUSIVE_PTR<MessageElementData> dat;

  public:
    MessageElement();

    MessageElement(MessageStringRef name, const RR_INTRUSIVE_PTR<MessageElementData>& datin);

    RR_INTRUSIVE_PTR<MessageElementData> GetData();
    void SetData(const RR_INTRUSIVE_PTR<MessageElementData>& value);

    // Version 2 Message
    uint32_t ComputeSize();
    void UpdateData();
    void Write(ArrayBinaryWriter& w);
    void Read(ArrayBinaryReader& r);

    // Version 4 Message
    uint32_t ComputeSize4();
    void UpdateData4();
    void Write4(ArrayBinaryWriter& w);
    void Read4(ArrayBinaryReader& r);

    static RR_INTRUSIVE_PTR<MessageElement> FindElement(std::vector<RR_INTRUSIVE_PTR<MessageElement> >& m,
                                                        MessageStringRef name);

    static bool TryFindElement(std::vector<RR_INTRUSIVE_PTR<MessageElement> >& m, MessageStringRef name,
                               RR_INTRUSIVE_PTR<MessageElement>& elem);

    static bool ContainsElement(std::vector<RR_INTRUSIVE_PTR<MessageElement> >& m, MessageStringRef name);

    template <typename T>
    RR_INTRUSIVE_PTR<T> CastData()
    {
        return rr_cast<T>(dat);
    }

    std::string CastDataToString();

    RR_INTRUSIVE_PTR<MessageElementNestedElementList> CastDataToNestedList();

    RR_INTRUSIVE_PTR<MessageElementNestedElementList> CastDataToNestedList(DataTypes expected_type);

    template <typename T>
    static RR_INTRUSIVE_PTR<T> CastData(const RR_INTRUSIVE_PTR<MessageElementData>& Data)
    {
        return rr_cast<T>(Data);
    }

    RR_OVIRTUAL std::string RRType() RR_OVERRIDE { return "RobotRaconteur::MessageElement"; }
};

class ROBOTRACONTEUR_CORE_API MessageElementNestedElementList : public MessageElementData
{
  public:
    DataTypes Type;
    MessageStringPtr TypeName;
    std::vector<RR_INTRUSIVE_PTR<MessageElement> > Elements;

    MessageElementNestedElementList(DataTypes type_, MessageStringRef type_name_,
                                    const std::vector<RR_INTRUSIVE_PTR<MessageElement> >& elements_);
#if !defined(BOOST_NO_CXX11_RVALUE_REFERENCES)
    MessageElementNestedElementList(DataTypes type_, MessageStringRef type_name_,
                                    std::vector<RR_INTRUSIVE_PTR<MessageElement> >&& elements_);
#endif
    RR_OVIRTUAL MessageStringPtr GetTypeString() RR_OVERRIDE;
    RR_OVIRTUAL DataTypes GetTypeID() RR_OVERRIDE;
    RR_OVIRTUAL std::string RRType() RR_OVERRIDE;
};

ROBOTRACONTEUR_CORE_API RR_INTRUSIVE_PTR<Message> CreateMessage();
ROBOTRACONTEUR_CORE_API RR_INTRUSIVE_PTR<MessageHeader> CreateMessageHeader();
ROBOTRACONTEUR_CORE_API RR_INTRUSIVE_PTR<MessageEntry> CreateMessageEntry();
ROBOTRACONTEUR_CORE_API RR_INTRUSIVE_PTR<MessageEntry> CreateMessageEntry(MessageEntryType t, MessageStringRef n);
ROBOTRACONTEUR_CORE_API RR_INTRUSIVE_PTR<MessageElement> CreateMessageElement();
ROBOTRACONTEUR_CORE_API RR_INTRUSIVE_PTR<MessageElement> CreateMessageElement(
    MessageStringRef name, const RR_INTRUSIVE_PTR<MessageElementData>& datin);
ROBOTRACONTEUR_CORE_API RR_INTRUSIVE_PTR<MessageElement> CreateMessageElement(
    int32_t number, const RR_INTRUSIVE_PTR<MessageElementData>& datin);
ROBOTRACONTEUR_CORE_API RR_INTRUSIVE_PTR<MessageElementNestedElementList> CreateMessageElementNestedElementList(
    DataTypes type_, MessageStringRef type_name_, const std::vector<RR_INTRUSIVE_PTR<MessageElement> >& elements_);
#if !defined(BOOST_NO_CXX11_RVALUE_REFERENCES)
ROBOTRACONTEUR_CORE_API RR_INTRUSIVE_PTR<MessageElementNestedElementList> CreateMessageElementNestedElementList(
    DataTypes type_, MessageStringRef type_name_, std::vector<RR_INTRUSIVE_PTR<MessageElement> >&& elements_);
#endif
ROBOTRACONTEUR_CORE_API RR_INTRUSIVE_PTR<Message> ShallowCopyMessage(const RR_INTRUSIVE_PTR<Message>& m);
ROBOTRACONTEUR_CORE_API RR_INTRUSIVE_PTR<MessageEntry> ShallowCopyMessageEntry(
    const RR_INTRUSIVE_PTR<MessageEntry>& mm);
ROBOTRACONTEUR_CORE_API RR_INTRUSIVE_PTR<MessageElement> ShallowCopyMessageElement(
    const RR_INTRUSIVE_PTR<MessageElement>& mm);

// MessageElement packing functions
template <typename T>
RR_INTRUSIVE_PTR<MessageElementData> MessageElement_PackScalar(const T& value)
{
    return RobotRaconteur::ScalarToRRArray<T>(value);
}

template <typename T>
RR_INTRUSIVE_PTR<MessageElement> MessageElement_PackScalarElement(MessageStringRef elementname, const T& value)
{
    return CreateMessageElement(elementname, MessageElement_PackScalar(value));
}

template <typename T>
RR_INTRUSIVE_PTR<MessageElementData> MessageElement_PackArray(const RR_INTRUSIVE_PTR<RRArray<T> >& value)
{
    if (!value)
        throw NullValueException("Arrays must not be null");
    return value;
}

template <typename T>
RR_INTRUSIVE_PTR<MessageElement> MessageElement_PackArrayElement(MessageStringRef elementname,
                                                                 const RR_INTRUSIVE_PTR<RRArray<T> >& value)
{
    return CreateMessageElement(elementname, MessageElement_PackArray(value));
}

template <typename T, typename N>
RR_INTRUSIVE_PTR<MessageElementData> MessageElement_PackMultiDimArray(
    RR_WEAK_PTR<N> node, const RR_INTRUSIVE_PTR<RRMultiDimArray<T> >& value)
{
    if (!value)
        throw NullValueException("Arrays must not be null");
    RR_SHARED_PTR<N> node1 = node.lock();
    if (!node1)
        throw InvalidOperationException("Node has been released");
    return node1->template PackMultiDimArray<T>(RobotRaconteur::rr_cast<RobotRaconteur::RRMultiDimArray<T> >(value));
}

template <typename T, typename N>
RR_INTRUSIVE_PTR<MessageElement> MessageElement_PackMultiDimArrayElement(
    RR_WEAK_PTR<N> node, MessageStringRef elementname, const RR_INTRUSIVE_PTR<RRMultiDimArray<T> >& value)
{
    return CreateMessageElement(elementname, MessageElement_PackMultiDimArray(node, value));
}

template <typename T>
RR_INTRUSIVE_PTR<MessageElementData> MessageElement_PackString(const T& str)
{
    return RobotRaconteur::stringToRRArray(str);
}

template <typename T>
RR_INTRUSIVE_PTR<MessageElement> MessageElement_PackStringElement(MessageStringRef elementname, const T& str)
{
    return CreateMessageElement(elementname, MessageElement_PackString(str));
}

template <typename T, typename N>
RR_INTRUSIVE_PTR<MessageElementData> MessageElement_PackVarType(RR_WEAK_PTR<N> node, const RR_INTRUSIVE_PTR<T>& s)
{
    RR_SHARED_PTR<N> node1 = node.lock();
    if (!node1)
        throw InvalidOperationException("Node has been released");
    return node1->PackVarType(s);
}

template <typename T, typename N>
RR_INTRUSIVE_PTR<MessageElement> MessageElement_PackVarTypeElement(RR_WEAK_PTR<N> node, MessageStringRef elementname,
                                                                   const RR_INTRUSIVE_PTR<T>& s)
{
    return CreateMessageElement(elementname, MessageElement_PackVarType(node, s));
}

template <typename T, typename N>
RR_INTRUSIVE_PTR<MessageElementData> MessageElement_PackStruct(RR_WEAK_PTR<N> node, const RR_INTRUSIVE_PTR<T>& s)
{
    RR_SHARED_PTR<N> node1 = node.lock();
    if (!node1)
        throw InvalidOperationException("Node has been released");
    return node1->PackStructure(RobotRaconteur::rr_cast<RobotRaconteur::RRStructure>(s));
}

template <typename T, typename N>
RR_INTRUSIVE_PTR<MessageElement> MessageElement_PackStructElement(RR_WEAK_PTR<N> node, MessageStringRef elementname,
                                                                  const RR_INTRUSIVE_PTR<T>& s)
{
    return CreateMessageElement(elementname, MessageElement_PackStruct(node, s));
}

template <typename T>
RR_INTRUSIVE_PTR<MessageElementData> MessageElement_PackEnum(const T& value)
{
    return RobotRaconteur::ScalarToRRArray<int32_t>(static_cast<int32_t>(value));
}

template <typename T>
RR_INTRUSIVE_PTR<MessageElement> MessageElement_PackEnumElement(MessageStringRef elementname, const T& value)
{
    return CreateMessageElement(elementname, MessageElement_PackEnum(value));
}

template <typename K, typename T, typename N>
RR_INTRUSIVE_PTR<MessageElementData> MessageElement_PackMap(RR_WEAK_PTR<N> node,
                                                            const RR_INTRUSIVE_PTR<RRMap<K, T> >& m)
{
    RR_SHARED_PTR<N> node1 = node.lock();
    if (!node1)
        throw InvalidOperationException("Node has been released");
    return node1->template PackMapType<K, T>(m);
}

template <typename K, typename T, typename N>
RR_INTRUSIVE_PTR<MessageElement> MessageElement_PackMapElement(RR_WEAK_PTR<N> node, MessageStringRef elementname,
                                                               const RR_INTRUSIVE_PTR<RRMap<K, T> >& m)
{
    return CreateMessageElement(elementname, MessageElement_PackMap(node, m));
}

template <typename T, typename N>
RR_INTRUSIVE_PTR<MessageElementData> MessageElement_PackList(RR_WEAK_PTR<N> node, const RR_INTRUSIVE_PTR<RRList<T> >& m)
{
    RR_SHARED_PTR<N> node1 = node.lock();
    if (!node1)
        throw InvalidOperationException("Node has been released");
    return node1->template PackListType<T>(m);
}

template <typename T, typename N>
RR_INTRUSIVE_PTR<MessageElement> MessageElement_PackListElement(RR_WEAK_PTR<N> node, MessageStringRef elementname,
                                                                const RR_INTRUSIVE_PTR<RRList<T> >& m)
{
    return CreateMessageElement(elementname, MessageElement_PackList(node, m));
}

// MessageElement unpacking functions
template <typename T>
T MessageElement_UnpackScalar(const RR_INTRUSIVE_PTR<MessageElement>& m)
{
    return RRArrayToScalar<T>(m->CastData<RRArray<T> >());
}

template <typename T>
RR_INTRUSIVE_PTR<RRArray<T> > MessageElement_UnpackArray(const RR_INTRUSIVE_PTR<MessageElement>& m)
{
    RR_INTRUSIVE_PTR<RRArray<T> > a = (m->CastData<RRArray<T> >());
    if (!a)
        throw NullValueException("Arrays must not be null");
    return a;
}

template <typename T, typename N>
RR_INTRUSIVE_PTR<RRMultiDimArray<T> > MessageElement_UnpackMultiDimArray(RR_WEAK_PTR<N> node,
                                                                         const RR_INTRUSIVE_PTR<MessageElement>& m)
{
    RR_SHARED_PTR<N> node1 = node.lock();
    if (!node1)
        throw InvalidOperationException("Node has been released");
    RR_INTRUSIVE_PTR<RRMultiDimArray<T> > a = RobotRaconteur::rr_cast<RRMultiDimArray<T> >(
        node1->template UnpackMultiDimArray<T>(m->CastDataToNestedList(DataTypes_multidimarray_t)));
    if (!a)
        throw NullValueException("Arrays must not be null");
    return a;
}

static std::string MessageElement_UnpackString(const RR_INTRUSIVE_PTR<MessageElement>& m)
{
    return RRArrayToString(m->CastData<RRArray<char> >());
}

template <typename T, typename N>
RR_INTRUSIVE_PTR<T> MessageElement_UnpackStructure(RR_WEAK_PTR<N> node, const RR_INTRUSIVE_PTR<MessageElement>& m)
{
    RR_SHARED_PTR<N> node1 = node.lock();
    if (!node1)
        throw InvalidOperationException("Node has been released");
    return RobotRaconteur::rr_cast<T>(
        node1->UnpackStructure(m->CastData<RobotRaconteur::MessageElementNestedElementList>()));
}

template <typename T>
T MessageElement_UnpackEnum(const RR_INTRUSIVE_PTR<MessageElement>& m)
{
    return static_cast<T>(RRArrayToScalar<int32_t>(m->CastData<RRArray<int32_t> >()));
}

template <typename N>
RR_INTRUSIVE_PTR<RRValue> MessageElement_UnpackVarValue(RR_WEAK_PTR<N> node, const RR_INTRUSIVE_PTR<MessageElement>& m)
{
    RR_SHARED_PTR<N> node1 = node.lock();
    if (!node1)
        throw InvalidOperationException("Node has been released");
    return node1->UnpackVarType(m);
}

template <typename K, typename T, typename N>
RR_INTRUSIVE_PTR<RRMap<K, T> > MessageElement_UnpackMap(RR_WEAK_PTR<N> node, const RR_INTRUSIVE_PTR<MessageElement>& m)
{
    RR_SHARED_PTR<N> node1 = node.lock();
    if (!node1)
        throw InvalidOperationException("Node has been released");
    return node1->template UnpackMapType<K, T>(m->CastData<RobotRaconteur::MessageElementNestedElementList>());
}

template <typename T, typename N>
RR_INTRUSIVE_PTR<RRList<T> > MessageElement_UnpackList(RR_WEAK_PTR<N> node, const RR_INTRUSIVE_PTR<MessageElement>& m)
{
    RR_SHARED_PTR<N> node1 = node.lock();
    if (!node1)
        throw InvalidOperationException("Node has been released");
    return node1->template UnpackListType<T>(m->CastDataToNestedList());
}

ROBOTRACONTEUR_CORE_API bool MessageElement_GetElementNumber(const RR_INTRUSIVE_PTR<MessageElement>& m,
                                                             int32_t& number);
ROBOTRACONTEUR_CORE_API void MessageElement_SetElementNumber(const RR_INTRUSIVE_PTR<MessageElement>& m, int32_t number);
ROBOTRACONTEUR_CORE_API bool MessageElement_GetElementName(const RR_INTRUSIVE_PTR<MessageElement>& m,
                                                           MessageStringPtr& name);

#ifndef BOOST_NO_CXX11_TEMPLATE_ALIASES
using MessagePtr = RR_INTRUSIVE_PTR<Message>;
using MessageHeaderPtr = RR_INTRUSIVE_PTR<MessageHeader>;
using MessageEntryPtr = RR_INTRUSIVE_PTR<MessageEntry>;
using MessageElementPtr = RR_INTRUSIVE_PTR<MessageElement>;
using MessageElementNestedElementListPtr = RR_INTRUSIVE_PTR<MessageElementNestedElementList>;
#endif
} // namespace RobotRaconteur
