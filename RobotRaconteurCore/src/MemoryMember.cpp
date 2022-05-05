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

#include "RobotRaconteur/MemoryMember.h"

namespace RobotRaconteur
{

namespace detail
{

ROBOTRACONTEUR_CORE_API void CalculateMatrixBlocks(uint32_t element_size, std::vector<uint64_t> count,
                                                   uint64_t max_elems, uint32_t& split_dim, uint64_t& split_dim_block,
                                                   uint64_t& split_elem_count, uint32_t& splits_count,
                                                   uint32_t& split_remainder, std::vector<uint64_t>& block_count,
                                                   std::vector<uint64_t>& block_count_edge)
{

    split_elem_count = 1;
    split_dim = -1;
    split_dim_block = 0;
    bool split_dim_found = false;
    block_count = std::vector<uint64_t>(count.size());
    splits_count = 0;
    split_remainder = 0;
    for (size_t i = 0; i < count.size(); i++)
    {
        if (!split_dim_found)
        {
            uint64_t temp_elem_count1 = split_elem_count * count[i];
            if (temp_elem_count1 > max_elems)
            {
                split_dim = boost::numeric_cast<uint32_t>(i);
                split_dim_block = max_elems / split_elem_count;
                split_dim_found = true;
                block_count[i] = split_dim_block;
                splits_count = boost::numeric_cast<uint32_t>(count[i] / split_dim_block);
                split_remainder = boost::numeric_cast<uint32_t>(count[i] % split_dim_block);
            }
            else
            {
                split_elem_count = temp_elem_count1;
                block_count[i] = count[i];
            }
        }
        else
        {
            block_count[i] = 1;
        }
    }

    // block_count_edge = std::vector<uint64_t>(block_count.size());
    // Array::Copy(block_count, block_count_edge, block_count.size());
    block_count_edge = block_count;
    block_count_edge[split_dim] = count[split_dim] % split_dim_block;
}
} // namespace detail

RR_SHARED_PTR<RobotRaconteurNode> ArrayMemoryServiceSkelBase::GetNode()
{
    RR_SHARED_PTR<RobotRaconteurNode> n = node.lock();
    if (!n)
        throw InvalidOperationException("Node has been released");
    return n;
}

std::string ArrayMemoryServiceSkelBase::GetMemberName() const { return m_MemberName; }

ArrayMemoryServiceSkelBase::ArrayMemoryServiceSkelBase(boost::string_ref membername, RR_SHARED_PTR<ServiceSkel> skel,
                                                       DataTypes element_type, size_t element_size,
                                                       MemberDefinition_Direction direction)
{
    this->m_MemberName = RR_MOVE(membername.to_string());
    this->skel = skel;
    this->node = skel->RRGetNode();
    this->direction = direction;
    this->element_type = element_type;
    this->element_size = element_size;
}

ArrayMemoryServiceSkelBase::~ArrayMemoryServiceSkelBase() {}

RR_INTRUSIVE_PTR<MessageEntry> ArrayMemoryServiceSkelBase::CallMemoryFunction(RR_INTRUSIVE_PTR<MessageEntry> m,
                                                                              RR_SHARED_PTR<Endpoint> e,
                                                                              RR_SHARED_PTR<ArrayMemoryBase> mem)
{

    switch (m->EntryType)
    {
    case MessageEntryType_MemoryRead: {
        try
        {
            if (direction == MemberDefinition_Direction_writeonly)
            {
                throw WriteOnlyMemberException("Write only member");
            }

            uint64_t memorypos = RRArrayToScalar(m->FindElement("memorypos")->CastData<RRArray<uint64_t> >());
            uint64_t count = RRArrayToScalar(m->FindElement("count")->CastData<RRArray<uint64_t> >());
            RR_INTRUSIVE_PTR<MessageElementData> data = DoRead(memorypos, 0, boost::numeric_cast<size_t>(count), mem);
            RR_INTRUSIVE_PTR<MessageEntry> ret = CreateMessageEntry(MessageEntryType_MemoryReadRet, GetMemberName());
            ret->AddElement("memorypos", ScalarToRRArray(memorypos));
            ret->AddElement("count", ScalarToRRArray(count));
            ret->AddElement("data", data);
            return ret;
        }
        catch (std::exception& exp)
        {
            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(node, Service, e->GetLocalEndpoint(), service_path, m_MemberName,
                                                    "Memory read operation failed: " << exp.what());
            throw;
        }
    }
    case MessageEntryType_MemoryWrite: {
        try
        {
            if (direction == MemberDefinition_Direction_readonly)
            {
                throw ReadOnlyMemberException("Read only member");
            }

            uint64_t memorypos = RRArrayToScalar(m->FindElement("memorypos")->CastData<RRArray<uint64_t> >());
            uint64_t count = RRArrayToScalar(m->FindElement("count")->CastData<RRArray<uint64_t> >());
            RR_INTRUSIVE_PTR<MessageElementData> data = m->FindElement("data")->CastData<MessageElementData>();
            DoWrite(memorypos, data, 0, boost::numeric_cast<size_t>(count), mem);
            RR_INTRUSIVE_PTR<MessageEntry> ret = CreateMessageEntry(MessageEntryType_MemoryReadRet, GetMemberName());
            ret->AddElement("memorypos", ScalarToRRArray(memorypos));
            ret->AddElement("count", ScalarToRRArray(count));
            return ret;
        }
        catch (std::exception& exp)
        {
            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(node, Service, e->GetLocalEndpoint(), service_path, m_MemberName,
                                                    "Memory write operation failed: " << exp.what());
            throw;
        }
    }
    case MessageEntryType_MemoryGetParam: {
        try
        {
            std::string param = m->FindElement("parameter")->CastDataToString();
            if (param == "Length")
            {
                RR_INTRUSIVE_PTR<MessageEntry> ret =
                    CreateMessageEntry(MessageEntryType_MemoryGetParamRet, GetMemberName());
                ret->AddElement("return", ScalarToRRArray(mem->Length()));
                return ret;
            }
            else if (param == "MaxTransferSize")
            {
                RR_INTRUSIVE_PTR<MessageEntry> ret =
                    CreateMessageEntry(MessageEntryType_MemoryGetParamRet, GetMemberName());
                uint32_t MaxTransferSize = GetNode()->GetMemoryMaxTransferSize();
                ret->AddElement("return", ScalarToRRArray(MaxTransferSize));
                return ret;
            }
            else
            {
                throw InvalidOperationException("Unknown parameter");
            }
        }
        catch (std::exception& exp)
        {
            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(node, Service, e->GetLocalEndpoint(), service_path, m_MemberName,
                                                    "Memory get param operation failed: " << exp.what());
            throw;
        }
    }
    default:
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(node, Service, e->GetLocalEndpoint(), service_path, m_MemberName,
                                                "Invalid memory command");
        throw ProtocolException("Invalid command");
    }
}

RR_SHARED_PTR<RobotRaconteurNode> MultiDimArrayMemoryServiceSkelBase::GetNode()
{
    RR_SHARED_PTR<RobotRaconteurNode> n = node.lock();
    if (!n)
        throw InvalidOperationException("Node has been released");
    return n;
}

std::string MultiDimArrayMemoryServiceSkelBase::GetMemberName() const { return m_MemberName; }

MultiDimArrayMemoryServiceSkelBase::MultiDimArrayMemoryServiceSkelBase(boost::string_ref membername,
                                                                       RR_SHARED_PTR<ServiceSkel> skel,
                                                                       DataTypes element_type, size_t element_size,
                                                                       MemberDefinition_Direction direction)
{
    this->m_MemberName = RR_MOVE(membername.to_string());
    this->skel = skel;
    this->node = skel->RRGetNode();
    this->direction = direction;
    this->element_type = element_type;
    this->element_size = element_size;
    this->service_path = skel->GetServicePath();
}

MultiDimArrayMemoryServiceSkelBase::~MultiDimArrayMemoryServiceSkelBase() {}

RR_INTRUSIVE_PTR<MessageEntry> MultiDimArrayMemoryServiceSkelBase::CallMemoryFunction(
    RR_INTRUSIVE_PTR<MessageEntry> m, RR_SHARED_PTR<Endpoint> e, RR_SHARED_PTR<MultiDimArrayMemoryBase> mem)
{

    switch (m->EntryType)
    {
    case MessageEntryType_MemoryRead: {
        try
        {
            if (direction == MemberDefinition_Direction_writeonly)
            {
                throw WriteOnlyMemberException("Write only member");
            }

            RR_INTRUSIVE_PTR<RRArray<uint64_t> > memorypos =
                m->FindElement("memorypos")->CastData<RRArray<uint64_t> >();
            RR_INTRUSIVE_PTR<RRArray<uint64_t> > count = m->FindElement("count")->CastData<RRArray<uint64_t> >();
            uint32_t elemcount = 1;
            for (size_t i = 0; i < count->size(); i++)
                elemcount *= boost::numeric_cast<uint32_t>((*count)[i]);

            RR_INTRUSIVE_PTR<MessageElementData> data =
                DoRead(RRArrayToVector<uint64_t>(memorypos), std::vector<uint64_t>(count->size()),
                       RRArrayToVector<uint64_t>(count), elemcount, mem);

            RR_INTRUSIVE_PTR<MessageEntry> ret = CreateMessageEntry(MessageEntryType_MemoryReadRet, GetMemberName());
            ret->AddElement("memorypos", memorypos);
            ret->AddElement("count", memorypos);
            ret->AddElement("data", data);
            return ret;
        }
        catch (std::exception& exp)
        {
            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(node, Service, e->GetLocalEndpoint(), service_path, m_MemberName,
                                                    "Memory read operation failed: " << exp.what());
            throw;
        }
    }
    case MessageEntryType_MemoryWrite: {
        try
        {
            if (direction == MemberDefinition_Direction_readonly)
            {
                throw ReadOnlyMemberException("Read only member");
            }

            RR_INTRUSIVE_PTR<RRArray<uint64_t> > memorypos =
                m->FindElement("memorypos")->CastData<RRArray<uint64_t> >();
            RR_INTRUSIVE_PTR<RRArray<uint64_t> > count = m->FindElement("count")->CastData<RRArray<uint64_t> >();
            uint32_t elemcount = 1;
            for (size_t i = 0; i < count->size(); i++)
                elemcount *= boost::numeric_cast<uint32_t>((*count)[i]);

            RR_INTRUSIVE_PTR<MessageElementData> data = m->FindElement("data")->CastData<MessageElementData>();

            DoWrite(RRArrayToVector<uint64_t>(memorypos), data, std::vector<uint64_t>(count->size()),
                    RRArrayToVector<uint64_t>(count), elemcount, mem);
            RR_INTRUSIVE_PTR<MessageEntry> ret = CreateMessageEntry(MessageEntryType_MemoryReadRet, GetMemberName());
            ret->AddElement("memorypos", memorypos);
            ret->AddElement("count", count);

            return ret;
        }
        catch (std::exception& exp)
        {
            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(node, Service, e->GetLocalEndpoint(), service_path, m_MemberName,
                                                    "Memory write operation failed: " << exp.what());
            throw;
        }
    }
    case MessageEntryType_MemoryGetParam: {
        try
        {
            std::string param = m->FindElement("parameter")->CastDataToString();

            if (param == "Dimensions")
            {
                RR_INTRUSIVE_PTR<MessageEntry> ret =
                    CreateMessageEntry(MessageEntryType_MemoryGetParamRet, GetMemberName());
                ret->AddElement("return", VectorToRRArray<uint64_t>(mem->Dimensions()));
                return ret;
            }

            else if (param == "DimCount")
            {
                RR_INTRUSIVE_PTR<MessageEntry> ret =
                    CreateMessageEntry(MessageEntryType_MemoryGetParamRet, GetMemberName());
                ret->AddElement("return", ScalarToRRArray(mem->DimCount()));
                return ret;
            }

            else if (param == "MaxTransferSize")
            {
                RR_INTRUSIVE_PTR<MessageEntry> ret =
                    CreateMessageEntry(MessageEntryType_MemoryGetParamRet, GetMemberName());
                uint32_t MaxTransferSize = GetNode()->GetMemoryMaxTransferSize();
                ret->AddElement("return", ScalarToRRArray(MaxTransferSize));
                return ret;
            }
            else
            {
                throw InvalidOperationException("Unknown parameter");
            }
        }
        catch (std::exception& exp)
        {
            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(node, Service, e->GetLocalEndpoint(), service_path, m_MemberName,
                                                    "Memory get param operation failed: " << exp.what());
            throw;
        }
    }
    default:
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(node, Service, e->GetLocalEndpoint(), service_path, m_MemberName,
                                                "Invalid memory command");
        throw ProtocolException("Invalid command");
    }
}

const std::string ArrayMemoryClientBase::GetMemberName() const { return m_MemberName; }

RR_SHARED_PTR<RobotRaconteurNode> ArrayMemoryClientBase::GetNode()
{
    RR_SHARED_PTR<RobotRaconteurNode> n = node.lock();
    if (!n)
        throw InvalidOperationException("Node has been released");
    return n;
}

ArrayMemoryClientBase::ArrayMemoryClientBase(boost::string_ref membername, RR_SHARED_PTR<ServiceStub> stub,
                                             DataTypes element_type, size_t element_size,
                                             MemberDefinition_Direction direction)
{
    this->stub = stub;
    this->node = stub->RRGetNode();
    m_MemberName = RR_MOVE(membername.to_string());
    this->direction = direction;
    this->element_type = element_type;
    this->element_size = element_size;
    max_size_read = false;
    remote_max_size = 0;
    service_path = stub->ServicePath;
    endpoint = stub->GetContext()->GetLocalEndpoint();

    ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Member, endpoint, service_path, m_MemberName,
                                            "ArrayMemoryClientBase created");
}

ArrayMemoryClientBase::~ArrayMemoryClientBase() {}

RR_SHARED_PTR<ServiceStub> ArrayMemoryClientBase::GetStub()
{
    RR_SHARED_PTR<ServiceStub> out = stub.lock();
    if (!out)
        throw InvalidOperationException("Memory client has been closed");
    return out;
}

uint64_t ArrayMemoryClientBase::Length()
{
    ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Client, endpoint, service_path, m_MemberName,
                                            "Requesting memory length");
    RR_INTRUSIVE_PTR<MessageEntry> m = CreateMessageEntry(MessageEntryType_MemoryGetParam, GetMemberName());
    m->AddElement("parameter", stringToRRArray("Length"));
    RR_INTRUSIVE_PTR<MessageEntry> ret = GetStub()->ProcessRequest(m);
    return RRArrayToScalar(ret->FindElement("return")->CastData<RRArray<uint64_t> >());
}

RobotRaconteur::MemberDefinition_Direction ArrayMemoryClientBase::Direction() { return direction; }

uint32_t ArrayMemoryClientBase::GetMaxTransferSize()
{

    {

        boost::mutex::scoped_lock lock(max_size_lock);
        if (!max_size_read)
        {
            ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Client, endpoint, service_path, m_MemberName,
                                                    "Requesting memory service MaxTransferSize");
            RR_INTRUSIVE_PTR<MessageEntry> m = CreateMessageEntry(MessageEntryType_MemoryGetParam, GetMemberName());
            m->AddElement("parameter", stringToRRArray("MaxTransferSize"));
            RR_INTRUSIVE_PTR<MessageEntry> ret = GetStub()->ProcessRequest(m);
            remote_max_size = RRArrayToScalar(ret->FindElement("return")->CastData<RRArray<uint32_t> >());
        }
        uint32_t my_max_size = GetNode()->GetMemoryMaxTransferSize();
        if (remote_max_size > my_max_size)
        {
            ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Client, endpoint, service_path, m_MemberName,
                                                    "Negotiated MaxTransferSize: " << my_max_size << " bytes");
            return my_max_size;
        }
        else
        {
            ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Client, endpoint, service_path, m_MemberName,
                                                    "Negotiated MaxTransferSize: " << remote_max_size << " bytes");
            return remote_max_size;
        }
    }
}

void ArrayMemoryClientBase::ReadBase(uint64_t memorypos, void* buffer, uint64_t bufferpos, uint64_t count)
{
    ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Client, endpoint, service_path, m_MemberName,
                                            "Begin memory read with " << count << " elements");
    if (direction == MemberDefinition_Direction_writeonly)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(node, Client, endpoint, service_path, m_MemberName,
                                                "Attempt to read a write only memory");
        throw WriteOnlyMemberException("Write only member");
    }

    uint32_t max_transfer_size = GetMaxTransferSize();
    uint32_t max_elems = boost::numeric_cast<uint32_t>(max_transfer_size / element_size);

    if (count <= max_elems)
    {
        try
        {
            ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Client, endpoint, service_path, m_MemberName,
                                                    "Requesting memory read with " << count << " elements");
            // Transfer all data in one block
            RR_INTRUSIVE_PTR<MessageEntry> e = CreateMessageEntry(MessageEntryType_MemoryRead, GetMemberName());
            e->AddElement("memorypos", ScalarToRRArray(memorypos));
            e->AddElement("count", ScalarToRRArray(count));
            RR_INTRUSIVE_PTR<MessageEntry> ret = GetStub()->ProcessRequest(e);
            UnpackReadResult(ret->FindElement("data")->CastData<MessageElementData>(), buffer, bufferpos, count);
        }
        catch (std::exception& exp)
        {
            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(node, Client, endpoint, service_path, m_MemberName,
                                                    "Memory read request failed: " << exp.what());
            throw;
        }
    }
    else
    {
        uint64_t blocks = count / boost::numeric_cast<uint64_t>(max_elems);
        uint64_t blockrem = count % boost::numeric_cast<uint64_t>(max_elems);

        ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Client, endpoint, service_path, m_MemberName,
                                                "Memory read request too large, reading in " << blocks << " blocks");

        for (uint64_t i = 0; i < blocks; i++)
        {
            uint64_t bufferpos_i = bufferpos + max_elems * i;
            uint64_t memorypos_i = memorypos + max_elems * i;

            ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Client, endpoint, service_path, m_MemberName,
                                                    "Memory read block " << i);
            ReadBase(memorypos_i, buffer, bufferpos_i, max_elems);
        }

        if (blockrem > 0)
        {
            uint64_t bufferpos_i = bufferpos + max_elems * blocks;
            uint64_t memorypos_i = memorypos + max_elems * blocks;

            ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Client, endpoint, service_path, m_MemberName,
                                                    "Memory read block remaining");
            ReadBase(memorypos_i, buffer, bufferpos_i, blockrem);
        }
    }
}

void ArrayMemoryClientBase::WriteBase(uint64_t memorypos, void* buffer, uint64_t bufferpos, uint64_t count)
{
    ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Client, endpoint, service_path, m_MemberName,
                                            "Begin memory write with " << count << " elements");
    if (direction == MemberDefinition_Direction_readonly)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(node, Client, endpoint, service_path, m_MemberName,
                                                "Attempt to write a read only memory");
        throw ReadOnlyMemberException("Read only member");
    }

    uint32_t max_transfer_size = GetMaxTransferSize();

    uint32_t max_elems = boost::numeric_cast<uint32_t>(max_transfer_size / element_size);

    if (count <= max_elems)
    {
        try
        {
            ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Client, endpoint, service_path, m_MemberName,
                                                    "Requesting memory write with " << count << " elements");
            // Transfer all data in one block
            RR_INTRUSIVE_PTR<MessageEntry> e = CreateMessageEntry(MessageEntryType_MemoryWrite, GetMemberName());
            e->AddElement("memorypos", ScalarToRRArray(memorypos));
            e->AddElement("count", ScalarToRRArray(count));

            e->AddElement("data", PackWriteRequest(buffer, bufferpos, count));

            RR_INTRUSIVE_PTR<MessageEntry> ret = GetStub()->ProcessRequest(e);
        }
        catch (std::exception& exp)
        {
            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(node, Client, endpoint, service_path, m_MemberName,
                                                    "Memory write request failed: " << exp.what());
            throw;
        }
    }
    else
    {
        if ((boost::numeric_cast<int64_t>(GetBufferLength(buffer)) - boost::numeric_cast<int64_t>(bufferpos)) <
            boost::numeric_cast<int64_t>(count))
            throw OutOfRangeException("Invalid buffer length");

        uint64_t blocks = count / boost::numeric_cast<uint64_t>(max_elems);
        uint64_t blockrem = count % boost::numeric_cast<uint64_t>(max_elems);

        ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Client, endpoint, service_path, m_MemberName,
                                                "Memory write request too large, writing in " << blocks << " blocks");

        for (uint64_t i = 0; i < blocks; i++)
        {
            uint64_t bufferpos_i = bufferpos + max_elems * i;
            uint64_t memorypos_i = memorypos + max_elems * i;

            ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Client, endpoint, service_path, m_MemberName,
                                                    "Memory write block " << i);
            WriteBase(memorypos_i, buffer, bufferpos_i, max_elems);
        }

        if (blockrem > 0)
        {
            uint64_t bufferpos_i = bufferpos + max_elems * blocks;
            uint64_t memorypos_i = memorypos + max_elems * blocks;

            ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Client, endpoint, service_path, m_MemberName,
                                                    "Memory write block remaining");
            WriteBase(memorypos_i, buffer, bufferpos_i, blockrem);
        }
    }
}

void ArrayMemoryClientBase::Shutdown() {}

RR_SHARED_PTR<RobotRaconteurNode> MultiDimArrayMemoryClientBase::GetNode()
{
    RR_SHARED_PTR<RobotRaconteurNode> n = node.lock();
    if (!n)
        throw InvalidOperationException("Node has been released");
    return n;
}

const std::string MultiDimArrayMemoryClientBase::GetMemberName() const { return m_MemberName; }

MultiDimArrayMemoryClientBase::MultiDimArrayMemoryClientBase(boost::string_ref membername,
                                                             RR_SHARED_PTR<ServiceStub> stub, DataTypes element_type,
                                                             size_t element_size, MemberDefinition_Direction direction)
{
    this->stub = stub;
    this->node = stub->RRGetNode();
    m_MemberName = RR_MOVE(membername.to_string());
    this->direction = direction;
    max_size_read = false;
    remote_max_size = 0;
    this->element_type = element_type;
    this->element_size = element_size;
    this->service_path = stub->ServicePath;
    this->endpoint = stub->GetContext()->GetLocalEndpoint();

    ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Member, endpoint, service_path, m_MemberName,
                                            "MultiDimArrayMemoryClientBase created");
}

MultiDimArrayMemoryClientBase::~MultiDimArrayMemoryClientBase() {}

RR_SHARED_PTR<ServiceStub> MultiDimArrayMemoryClientBase::GetStub()
{
    RR_SHARED_PTR<ServiceStub> out = stub.lock();
    if (!out)
        throw InvalidOperationException("Memory client has been closed");
    return out;
}

RobotRaconteur::MemberDefinition_Direction MultiDimArrayMemoryClientBase::Direction() { return direction; }

std::vector<uint64_t> MultiDimArrayMemoryClientBase::Dimensions()
{
    ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Client, endpoint, service_path, m_MemberName,
                                            "Requesting memory dimensions");
    RR_INTRUSIVE_PTR<MessageEntry> m = CreateMessageEntry(MessageEntryType_MemoryGetParam, GetMemberName());
    m->AddElement("parameter", stringToRRArray("Dimensions"));
    RR_INTRUSIVE_PTR<MessageEntry> ret = GetStub()->ProcessRequest(m);
    return RRArrayToVector<uint64_t>(ret->FindElement("return")->CastData<RRArray<uint64_t> >());
}

uint64_t MultiDimArrayMemoryClientBase::DimCount()
{
    ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Client, endpoint, service_path, m_MemberName,
                                            "Requesting memory dimension count");
    RR_INTRUSIVE_PTR<MessageEntry> m = CreateMessageEntry(MessageEntryType_MemoryGetParam, GetMemberName());
    m->AddElement("parameter", stringToRRArray("DimCount"));
    RR_INTRUSIVE_PTR<MessageEntry> ret = GetStub()->ProcessRequest(m);
    return RRArrayToScalar(ret->FindElement("return")->CastData<RRArray<uint64_t> >());
}

uint32_t MultiDimArrayMemoryClientBase::GetMaxTransferSize()
{
    {
        boost::mutex::scoped_lock lock(max_size_lock);
        if (!max_size_read)
        {
            ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Client, endpoint, service_path, m_MemberName,
                                                    "Requesting memory service MaxTransferSize");
            RR_INTRUSIVE_PTR<MessageEntry> m = CreateMessageEntry(MessageEntryType_MemoryGetParam, GetMemberName());
            m->AddElement("parameter", stringToRRArray("MaxTransferSize"));
            RR_INTRUSIVE_PTR<MessageEntry> ret = GetStub()->ProcessRequest(m);
            remote_max_size = RRArrayToScalar(ret->FindElement("return")->CastData<RRArray<uint32_t> >());
        }
        uint32_t my_max_size = GetNode()->GetMemoryMaxTransferSize();
        if (remote_max_size > my_max_size)
        {
            ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Client, endpoint, service_path, m_MemberName,
                                                    "Negotiated MaxTransferSize: " << my_max_size << " bytes");
            return my_max_size;
        }
        else
        {
            ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Client, endpoint, service_path, m_MemberName,
                                                    "Negotiated MaxTransferSize: " << remote_max_size << " bytes");
            return remote_max_size;
        }
    }
}

static std::string std_vec_uint64_t_to_string(const std::vector<uint64_t>& v)
{
    return boost::join(v | boost::adaptors::transformed(boost::lexical_cast<std::string, uint64_t>), ",");
}

void MultiDimArrayMemoryClientBase::ReadBase(const std::vector<uint64_t>& memorypos, void* buffer,
                                             const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count)
{
    ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Client, endpoint, service_path, m_MemberName,
                                            "Begin memory read with " << std_vec_uint64_t_to_string(count) << " count");
    if (direction == MemberDefinition_Direction_writeonly)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(node, Client, endpoint, service_path, m_MemberName,
                                                "Attempt to read a write only memory");
        throw WriteOnlyMemberException("Write only member");
    }

    uint32_t max_transfer_size = GetMaxTransferSize();

    uint64_t elemcount = 1;
    for (size_t i = 0; i < count.size(); i++)
        elemcount *= count[i];
    uint32_t max_elems = boost::numeric_cast<uint32_t>(max_transfer_size / element_size);

    if (elemcount <= max_elems)
    {
        try
        {
            ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Client, endpoint, service_path, m_MemberName,
                                                    "Requesting memory read with " << std_vec_uint64_t_to_string(count)
                                                                                   << " count");
            // Transfer all data in one block
            RR_INTRUSIVE_PTR<MessageEntry> e = CreateMessageEntry(MessageEntryType_MemoryRead, GetMemberName());
            e->AddElement("memorypos", VectorToRRArray<uint64_t>(memorypos));
            e->AddElement("count", VectorToRRArray<uint64_t>(count));
            RR_INTRUSIVE_PTR<MessageEntry> ret = GetStub()->ProcessRequest(e);

            UnpackReadResult(ret->FindElement("data")->CastData<MessageElementData>(), buffer, bufferpos, count,
                             elemcount);
        }
        catch (std::exception& exp)
        {
            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(node, Client, endpoint, service_path, m_MemberName,
                                                    "Memory read request failed: " << exp.what());
            throw;
        }
    }
    else
    {
        // We need to read the array in chunks.  This is a little complicated...

        uint32_t split_dim;
        uint64_t split_dim_block;
        uint64_t split_elem_count;
        uint32_t splits_count;
        uint32_t split_remainder;
        std::vector<uint64_t> block_count;
        std::vector<uint64_t> block_count_edge;

        detail::CalculateMatrixBlocks(element_size, count, max_elems, split_dim, split_dim_block, split_elem_count,
                                      splits_count, split_remainder, block_count, block_count_edge);

        bool done = false;
        std::vector<uint64_t> current_pos = std::vector<uint64_t>(count.size());

        ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Client, endpoint, service_path, m_MemberName,
                                                "Memory read request too large, reading in blocks");

        while (!done)
        {
            for (uint32_t i = 0; i < splits_count; i++)
            {
                current_pos[split_dim] = split_dim_block * boost::numeric_cast<uint64_t>(i);

                std::vector<uint64_t> current_buf_pos = std::vector<uint64_t>(bufferpos.size());
                std::vector<uint64_t> current_mem_pos = std::vector<uint64_t>(bufferpos.size());

                for (size_t j = 0; j < current_buf_pos.size(); j++)
                {
                    current_buf_pos[j] = current_pos[j] + bufferpos[j];
                    current_mem_pos[j] = current_pos[j] + memorypos[j];
                }

                ReadBase(current_mem_pos, buffer, current_buf_pos, block_count);
            }

            if (split_remainder != 0)
            {
                current_pos[split_dim] = split_dim_block * boost::numeric_cast<uint64_t>(splits_count);
                std::vector<uint64_t> current_buf_pos = std::vector<uint64_t>(bufferpos.size());
                std::vector<uint64_t> current_mem_pos = std::vector<uint64_t>(bufferpos.size());

                for (size_t j = 0; j < current_buf_pos.size(); j++)
                {
                    current_buf_pos[j] = current_pos[j] + bufferpos[j];
                    current_mem_pos[j] = current_pos[j] + memorypos[j];
                }

                ReadBase(current_mem_pos, buffer, current_buf_pos, block_count_edge);
            }

            if (split_dim == boost::numeric_cast<uint32_t>(count.size() - 1))
            {
                done = true;
            }
            else
            {
                current_pos[split_dim + 1]++;
                if (current_pos[split_dim + 1] >= count[split_dim + 1])
                {
                    if (split_dim + 1 == boost::numeric_cast<uint32_t>(count.size() - 1))
                    {
                        done = true;
                    }
                    else
                    {
                        current_pos[split_dim + 1] = 0;
                        for (size_t j = split_dim + 2; j < count.size(); j++)
                        {
                            if (current_pos[j - 1] >= count[j - 1])
                            {
                                current_pos[j]++;
                            }
                        }
                        if (current_pos[count.size() - 1] >= count[count.size() - 1])
                            done = true;
                    }
                }
            }
        }
    }
}

void MultiDimArrayMemoryClientBase::WriteBase(const std::vector<uint64_t>& memorypos, void* buffer,
                                              const std::vector<uint64_t>& bufferpos,
                                              const std::vector<uint64_t>& count)
{
    ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Client, endpoint, service_path, m_MemberName,
                                            "Begin memory write with " << std_vec_uint64_t_to_string(count)
                                                                       << " count");
    if (direction == MemberDefinition_Direction_readonly)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(node, Client, endpoint, service_path, m_MemberName,
                                                "Attempt to write a read only memory");
        throw ReadOnlyMemberException("Read only member");
    }

    uint32_t max_transfer_size = GetMaxTransferSize();

    uint64_t elemcount = 1;
    for (size_t i = 0; i < count.size(); i++)
        elemcount *= count[i];
    uint32_t max_elems = boost::numeric_cast<uint32_t>(max_transfer_size / element_size);

    if (elemcount <= max_elems)
    {
        try
        {
            ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Client, endpoint, service_path, m_MemberName,
                                                    "Requesting memory write with " << std_vec_uint64_t_to_string(count)
                                                                                    << " count");
            // Transfer all data in one block
            RR_INTRUSIVE_PTR<MessageEntry> e = CreateMessageEntry(MessageEntryType_MemoryWrite, GetMemberName());
            e->AddElement("memorypos", VectorToRRArray<uint64_t>(memorypos));
            e->AddElement("count", VectorToRRArray<uint64_t>(count));

            e->AddElement("data", PackWriteRequest(buffer, bufferpos, count, elemcount));

            RR_INTRUSIVE_PTR<MessageEntry> ret = GetStub()->ProcessRequest(e);
        }
        catch (std::exception& exp)
        {
            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(node, Client, endpoint, service_path, m_MemberName,
                                                    "Memory write request failed: " << exp.what());
            throw;
        }
    }
    else
    {
        uint32_t split_dim;
        uint64_t split_dim_block;
        uint64_t split_elem_count;
        uint32_t splits_count;
        uint32_t split_remainder;
        std::vector<uint64_t> block_count;
        std::vector<uint64_t> block_count_edge;

        detail::CalculateMatrixBlocks(element_size, count, max_elems, split_dim, split_dim_block, split_elem_count,
                                      splits_count, split_remainder, block_count, block_count_edge);

        bool done = false;
        std::vector<uint64_t> current_pos = std::vector<uint64_t>(count.size());

        ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Client, endpoint, service_path, m_MemberName,
                                                "Memory write request too large, reading in blocks");

        while (!done)
        {
            for (uint32_t i = 0; i < splits_count; i++)
            {
                current_pos[split_dim] = split_dim_block * boost::numeric_cast<uint64_t>(i);

                std::vector<uint64_t> current_buf_pos = std::vector<uint64_t>(bufferpos.size());
                std::vector<uint64_t> current_mem_pos = std::vector<uint64_t>(bufferpos.size());

                for (size_t j = 0; j < current_buf_pos.size(); j++)
                {
                    current_buf_pos[j] = current_pos[j] + bufferpos[j];
                    current_mem_pos[j] = current_pos[j] + memorypos[j];
                }

                WriteBase(current_mem_pos, buffer, current_buf_pos, block_count);
            }

            if (split_remainder != 0)
            {
                current_pos[split_dim] = split_dim_block * boost::numeric_cast<uint64_t>(splits_count);
                std::vector<uint64_t> current_buf_pos(bufferpos.size());
                std::vector<uint64_t> current_mem_pos(bufferpos.size());

                for (size_t j = 0; j < current_buf_pos.size(); j++)
                {
                    current_buf_pos[j] = current_pos[j] + bufferpos[j];
                    current_mem_pos[j] = current_pos[j] + memorypos[j];
                }

                WriteBase(current_mem_pos, buffer, current_buf_pos, block_count_edge);
            }

            if (split_dim == boost::numeric_cast<uint32_t>(count.size() - 1))
            {
                done = true;
            }
            else
            {
                current_pos[split_dim + 1]++;
                if (current_pos[split_dim + 1] >= count[split_dim + 1])
                {
                    if (split_dim + 1 == boost::numeric_cast<uint32_t>(count.size() - 1))
                    {
                        done = true;
                    }
                    else
                    {
                        current_pos[split_dim + 1] = 0;
                        for (size_t j = split_dim + 2; j < count.size(); j++)
                        {
                            if (current_pos[j - 1] >= count[j - 1])
                            {
                                current_pos[j]++;
                            }
                        }
                        if (current_pos[count.size() - 1] >= count[count.size() - 1])
                            done = true;
                    }
                }
            }
        }
    }
}

void MultiDimArrayMemoryClientBase::Shutdown() {}

} // namespace RobotRaconteur