/**
 * @file MemoryMember.h
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
#include "RobotRaconteur/Message.h"
#include "RobotRaconteur/RobotRaconteurNode.h"
#include "RobotRaconteur/Client.h"
#include "RobotRaconteur/Service.h"
#include <stdexcept>

namespace RobotRaconteur
{

namespace detail
{
template <typename Y, typename U>
static std::vector<Y> ConvertVectorType(const std::vector<U>& in)
{
    std::vector<Y> out(in.size());
    for (size_t i = 0; i < in.size(); i++)
        out[i] = boost::numeric_cast<Y>(in[i]);
    return out;
}

template <typename Y, typename U>
static RR_INTRUSIVE_PTR<RRArray<Y> > ConvertRRArrayType(const RR_INTRUSIVE_PTR<RRArray<U> >& in)
{
    RR_INTRUSIVE_PTR<RRArray<Y> > out = AllocateRRArray<Y>(in->size());
    for (size_t i = 0; i < in->size(); i++)
        (*out)[i] = boost::numeric_cast<Y>((*in)[i]);
    return out;
}

ROBOTRACONTEUR_CORE_API void CalculateMatrixBlocks(uint32_t element_size, std::vector<uint64_t> count,
                                                   uint64_t max_elems, uint32_t& split_dim, uint64_t& split_dim_block,
                                                   uint64_t& split_elem_count, uint32_t& splits_count,
                                                   uint32_t& split_remainder, std::vector<uint64_t>& block_count,
                                                   std::vector<uint64_t>& block_count_edge);
} // namespace detail

/**
 * @brief Base class for ArrayMemory
 *
 * Base class for templated ArrayMemory classes
 *
 */
class ROBOTRACONTEUR_CORE_API ArrayMemoryBase
{
  public:
    virtual uint64_t Length() = 0;
    virtual DataTypes ElementTypeID() = 0;
    virtual ~ArrayMemoryBase() {}
};

/**
 * @brief Single dimensional numeric primitive random access memory region
 *
 * Memories represent random access memory regions that are typically
 * represented as arrays of various shapes and types. Memories can be
 * declared in service definition files using the `memory` member keyword
 * within service definitions. Services expose memories to clients, and
 * the nodes will proxy read, write, and parameter requests between the client
 * and service. The node will also break up large requests to avoid the
 * message size limit of the transport.
 *
 * The ArrayMemory class is used to represent a single dimensional numeric
 * primitive array. Multidimensional numeric primitive arrays should use
 * MultiDimArrayMemory. Valid types for T are `double`, `float`, `int8_t`,
 * `uint8_t`, `int16_t`, `uint16_t`, `int32_t`, `uint32_t`, `int64_t`,
 * `uint64_t`, `rr_bool`, `cdouble`, and `csingle`.
 *
 * ArrayMemory instances are attached to an RRArrayPtr<T>, either when
 * constructed or later using Attach().
 *
 * ArrayMemory instances returned by clients are special implementations
 * designed to proxy requests to the service. They cannot be attached
 * to an arbitrary array.
 *
 * @tparam T The numeric primitive type of the array
 */
template <typename T>
class ArrayMemory : public virtual ArrayMemoryBase
{
  private:
    RR_INTRUSIVE_PTR<RRArray<T> > memory;
    boost::mutex memory_lock;

  public:
    /**
     * @brief Construct a new ArrayMemory instance
     *
     * New instance will not be attached to an array.
     *
     * ArrayMemory must be constructed with boost::make_shared<ArrayMemory<T> >();
     */
    ArrayMemory(){};

    /**
     * @brief Construct a new ArrayMemory instance attached to an RRArrayPtr<T>
     *
     * New instance will be constructed attached to an array.
     *
     * ArrayMemory must be constructed with boost::make_shared<ArrayMemory<T> >();
     *
     * @param memory The array to attach
     */
    ArrayMemory(const RR_INTRUSIVE_PTR<RRArray<T> >& memory) { this->memory = memory; }

    /**
     * @brief Attach ArrayMemory instance to an RRArrayPtr<T>
     *
     * @param memory The array to attach
     */
    virtual void Attach(const RR_INTRUSIVE_PTR<RRArray<T> >& memory)
    {
        boost::mutex::scoped_lock lock(memory_lock);
        this->memory = memory;
    }

    /**
     * @brief Return the length of the array memory
     *
     * When used with a memory returned by a client, this function will
     * call the service to execute the request.
     *
     * @return uint64_t The length of the array memory
     */
    RR_OVIRTUAL uint64_t Length() RR_OVERRIDE
    {
        boost::mutex::scoped_lock lock(memory_lock);
        return memory->size();
    }

    /**
     * @brief Read a segment from an array memory
     *
     * Read a segment of an array memory into a supplied buffer array. The start positions and length
     * of the read are specified.
     *
     * When used with a memory returned by a client, this function will call
     * the service to execute the request.
     *
     * @param memorypos The start index in the memory array to read
     * @param buffer The buffer to receive the read data
     * @param bufferpos The start index in the buffer to write the data
     * @param count The number of array elements to read
     */
    virtual void Read(uint64_t memorypos, RR_INTRUSIVE_PTR<RRArray<T> >& buffer, uint64_t bufferpos, uint64_t count)
    {
        boost::mutex::scoped_lock lock(memory_lock);
        if (memorypos + count > memory->size())
            throw OutOfRangeException("Index out of range");
        if (bufferpos + count > buffer->size())
            throw OutOfRangeException("Index out of range");

        memcpy(buffer->data() + bufferpos, memory->data() + memorypos, boost::numeric_cast<size_t>(count * sizeof(T)));
    }

    /**
     * @brief Write a segment to an array memory
     *
     * Writes a segment to an array memory from a supplied buffer array. The start positions and length
     * of the write are specified.
     *
     * When used with a memory returned by a client, this function will call
     * the service to execute the request.
     *
     * @param memorypos The start index in the memory array to write
     * @param buffer The buffer to write the data from
     * @param bufferpos The start index in the buffer to read the data
     * @param count The number of array elements to write
     */
    virtual void Write(uint64_t memorypos, const RR_INTRUSIVE_PTR<RRArray<T> >& buffer, uint64_t bufferpos,
                       uint64_t count)
    {
        boost::mutex::scoped_lock lock(memory_lock);
        if (memorypos + count > memory->size())
            throw OutOfRangeException("Index out of range");
        if (bufferpos + count > buffer->size())
            throw OutOfRangeException("Index out of range");
        memcpy(memory->data() + memorypos, buffer->data() + bufferpos, boost::numeric_cast<size_t>(count * sizeof(T)));
    }

    RR_OVIRTUAL DataTypes ElementTypeID() RR_OVERRIDE { return RRPrimUtil<T>::GetTypeID(); }
};

/**
 * @brief Base class for MultiDimArrayMemory
 *
 * Based class for templated MultiDimArrayMemory classes
 *
 */
class ROBOTRACONTEUR_CORE_API MultiDimArrayMemoryBase
{
  public:
    virtual std::vector<uint64_t> Dimensions() = 0;
    virtual uint64_t DimCount() = 0;
    virtual DataTypes ElementTypeID() = 0;
    virtual ~MultiDimArrayMemoryBase() {}
};

/**
 * @brief Multidimensional numeric primitive random access memory region
 *
 * Memories represent random access memory regions that are typically
 * represented as arrays of various shapes and types. Memories can be
 * declared in service definition files using the `memory` member keyword
 * within service definitions. Services expose memories to clients, and
 * the nodes will proxy read, write, and parameter requests between the client
 * and service. The node will also break up large requests to avoid the
 * message size limit of the transport.
 *
 * The MultiDimArrayMemory class is used to represent a multidimensional numeric
 * primitive array. Single dimensional numeric primitive arrays should use
 * ArrayMemory. Valid types for T are `double`, `float`, `int8_t`,
 * `uint8_t`, `int16_t`, `uint16_t`, `int32_t`, `uint32_t`, `int64_t`,
 * `uint64_t`, `rr_bool`, `cdouble`, and `csingle`.
 *
 * MultiDimArrayMemory instances are attached to an RRMultiDimArrayPtr<T>,
 * either when constructed or later using Attach().
 *
 * MultiDimArrayMemory instances returned by clients are special implementations
 * designed to proxy requests to the service. They cannot be attached
 * to an arbitrary array.
 *
 * @tparam T The numeric primitive type of the array
 */
template <typename T>
class MultiDimArrayMemory : public virtual MultiDimArrayMemoryBase
{
  private:
    RR_INTRUSIVE_PTR<RRMultiDimArray<T> > multimemory;
    boost::mutex memory_lock;

  public:
    /**
     * @brief Construct a new MultiDimArrayMemory instance
     *
     * New instance will not be attached to an array.
     *
     * MultiDimArrayMemory must be constructed with boost::make_shared<MultiDimArrayMemory<T> >();
     */
    MultiDimArrayMemory(){};

    /**
     * @brief Construct a new MultiDimArrayMemory instance attached to an RRMultiDimArrayPtr<T>
     *
     * New instance will be constructed attached to an array.
     *
     * MultiDimArrayMemory must be constructed with boost::make_shared<MultiDimArrayMemory<T> >();
     *
     * @param multimemory The array to attach
     */
    MultiDimArrayMemory(const RR_INTRUSIVE_PTR<RRMultiDimArray<T> >& multimemory) { this->multimemory = multimemory; }

    RR_OVIRTUAL ~MultiDimArrayMemory() RR_OVERRIDE {}

    /**
     * @brief Attach MultiDimArrayMemory instance to an RRMultiDimArrayPtr<T>
     *
     * @param multimemory The array to attach
     */
    virtual void Attach(const RR_INTRUSIVE_PTR<RRMultiDimArray<T> >& multimemory)
    {
        boost::mutex::scoped_lock lock(memory_lock);
        this->multimemory = multimemory;
    }

    /**
     * @brief Dimensions of the memory array
     *
     * Returns the dimensions (shape) of the memory array
     *
     * When used with a memory returned by a client, this function will
     * call the service to execute the request.
     *
     * @return std::vector<uint64_t> The dimensions of the memory array
     */
    RR_OVIRTUAL std::vector<uint64_t> Dimensions() RR_OVERRIDE
    {
        boost::mutex::scoped_lock lock(memory_lock);
        RR_INTRUSIVE_PTR<RRArray<uint32_t> > dims = multimemory->Dims;
        std::vector<uint64_t> s(dims->size());
        for (size_t i = 0; i < s.size(); i++)
        {
            s[i] = (*dims)[i];
        }

        return s;
    }

    /**
     * @brief The number of dimensions in the memory array
     *
     * When used with a memory returned by a client, this function will
     * call the service to execute the request.
     *
     * @return uint64_t Number of dimensions
     */
    RR_OVIRTUAL uint64_t DimCount() RR_OVERRIDE
    {
        boost::mutex::scoped_lock lock(memory_lock);
        return multimemory->Dims->size();
    }

    /**
     * @brief Read a block from a multidimensional array memory
     *
     * Read a block of a multidimensional array memory into a supplied buffer multidimensional array.
     * The start positions and count of the read are specified.
     *
     * When used with a memory returned by a client, this function will call
     * the service to execute the request.
     *
     * @param memorypos The start position in the memory array to read
     * @param buffer The buffer to receive the read data
     * @param bufferpos The start position in the buffer to write the data
     * @param count The count of array elements to read
     */
    virtual void Read(const std::vector<uint64_t>& memorypos, RR_INTRUSIVE_PTR<RRMultiDimArray<T> >& buffer,
                      const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count)
    {
        boost::mutex::scoped_lock lock(memory_lock);
        multimemory->RetrieveSubArray(detail::ConvertVectorType<uint32_t>(memorypos), buffer,
                                      detail::ConvertVectorType<uint32_t>(bufferpos),
                                      detail::ConvertVectorType<uint32_t>(count));
    }

    /**
     * @brief Write a segment to a multidimensional array memory
     *
     * Writes a segment to a multidimensional array memory from a supplied buffer
     * multidimensional array. The start positions and count
     * of the write are specified.
     *
     * When used with a memory returned by a client, this function will call
     * the service to execute the request.
     *
     * @param memorypos The start position in the memory array to write
     * @param buffer The buffer to write the data from
     * @param bufferpos The start position in the buffer to read the data
     * @param count The count of array elements to write
     */
    virtual void Write(const std::vector<uint64_t>& memorypos, const RR_INTRUSIVE_PTR<RRMultiDimArray<T> >& buffer,
                       const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count)
    {
        boost::mutex::scoped_lock lock(memory_lock);
        multimemory->AssignSubArray(detail::ConvertVectorType<uint32_t>(memorypos), buffer,
                                    detail::ConvertVectorType<uint32_t>(bufferpos),
                                    detail::ConvertVectorType<uint32_t>(count));
    }

    RR_OVIRTUAL DataTypes ElementTypeID() RR_OVERRIDE { return RRPrimUtil<T>::GetTypeID(); }
};

class ROBOTRACONTEUR_CORE_API ArrayMemoryServiceSkelBase : private boost::noncopyable
{

  private:
    std::string m_MemberName;
    RR_WEAK_PTR<ServiceSkel> skel;
    RR_WEAK_PTR<RobotRaconteurNode> node;
    MemberDefinition_Direction direction;
    DataTypes element_type;
    size_t element_size;
    std::string service_path;

  public:
    RR_SHARED_PTR<RobotRaconteurNode> GetNode();
    std::string GetMemberName() const;
    ArrayMemoryServiceSkelBase(boost::string_ref membername, const RR_SHARED_PTR<ServiceSkel>& skel,
                               DataTypes element_type, size_t element_size, MemberDefinition_Direction direction);
    virtual ~ArrayMemoryServiceSkelBase();
    virtual RR_INTRUSIVE_PTR<MessageEntry> CallMemoryFunction(const RR_INTRUSIVE_PTR<MessageEntry>& m,
                                                              const RR_SHARED_PTR<Endpoint>& e,
                                                              const RR_SHARED_PTR<ArrayMemoryBase>& mem);

  protected:
    virtual RR_INTRUSIVE_PTR<MessageElementData> DoRead(uint64_t memorypos, uint64_t bufferpos, uint64_t count,
                                                        const RR_SHARED_PTR<ArrayMemoryBase>& mem) = 0;
    virtual void DoWrite(uint64_t memorypos, const RR_INTRUSIVE_PTR<MessageElementData>& buffer, uint64_t bufferpos,
                         uint64_t count, const RR_SHARED_PTR<ArrayMemoryBase>& mem) = 0;
};

template <typename T>
class ArrayMemoryServiceSkel : public ArrayMemoryServiceSkelBase
{
  public:
    ArrayMemoryServiceSkel(boost::string_ref membername, const RR_SHARED_PTR<ServiceSkel>& skel,
                           MemberDefinition_Direction direction)
        : ArrayMemoryServiceSkelBase(membername, skel, RRPrimUtil<T>::GetTypeID(), sizeof(T), direction)
    {}

    RR_OVIRTUAL RR_INTRUSIVE_PTR<MessageElementData> DoRead(uint64_t memorypos, uint64_t bufferpos, uint64_t count,
                                                            const RR_SHARED_PTR<ArrayMemoryBase>& mem) RR_OVERRIDE
    {
        RR_UNUSED(bufferpos);
        RR_SHARED_PTR<ArrayMemory<T> > mem1 = rr_cast<ArrayMemory<T> >(mem);
        RR_INTRUSIVE_PTR<RRArray<T> > buf1 = AllocateRRArray<T>(boost::numeric_cast<size_t>(count));
        mem1->Read(memorypos, buf1, 0, boost::numeric_cast<size_t>(count));
        return buf1;
    }

    RR_OVIRTUAL void DoWrite(uint64_t memorypos, const RR_INTRUSIVE_PTR<MessageElementData>& buffer, uint64_t bufferpos,
                             uint64_t count, const RR_SHARED_PTR<ArrayMemoryBase>& mem) RR_OVERRIDE
    {
        RR_UNUSED(bufferpos);
        RR_SHARED_PTR<ArrayMemory<T> > mem1 = rr_cast<ArrayMemory<T> >(mem);
        RR_INTRUSIVE_PTR<RRArray<T> > buf1 = rr_cast<RRArray<T> >(buffer);
        mem1->Write(memorypos, buf1, 0, boost::numeric_cast<size_t>(count));
    }
};

class ROBOTRACONTEUR_CORE_API MultiDimArrayMemoryServiceSkelBase : private boost::noncopyable
{

  private:
    std::string m_MemberName;
    RR_WEAK_PTR<ServiceSkel> skel;
    RR_WEAK_PTR<RobotRaconteurNode> node;
    MemberDefinition_Direction direction;
    DataTypes element_type;
    size_t element_size;
    std::string service_path;

  public:
    RR_SHARED_PTR<RobotRaconteurNode> GetNode();
    std::string GetMemberName() const;
    MultiDimArrayMemoryServiceSkelBase(boost::string_ref membername, const RR_SHARED_PTR<ServiceSkel>& skel,
                                       DataTypes element_type, size_t element_size,
                                       MemberDefinition_Direction direction);
    virtual ~MultiDimArrayMemoryServiceSkelBase();
    virtual RR_INTRUSIVE_PTR<MessageEntry> CallMemoryFunction(const RR_INTRUSIVE_PTR<MessageEntry>& m,
                                                              const RR_SHARED_PTR<Endpoint>& e,
                                                              const RR_SHARED_PTR<MultiDimArrayMemoryBase>& mem);
    virtual RR_INTRUSIVE_PTR<MessageElementData> DoRead(const std::vector<uint64_t>& memorypos,
                                                        const std::vector<uint64_t>& bufferpos,
                                                        const std::vector<uint64_t>& count, uint32_t elem_count,
                                                        const RR_SHARED_PTR<MultiDimArrayMemoryBase>& mem) = 0;
    virtual void DoWrite(const std::vector<uint64_t>& memorypos, const RR_INTRUSIVE_PTR<MessageElementData>& buffer,
                         const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count,
                         uint32_t elem_count, const RR_SHARED_PTR<MultiDimArrayMemoryBase>& mem) = 0;
};

template <typename T>
class MultiDimArrayMemoryServiceSkel : public MultiDimArrayMemoryServiceSkelBase
{
  public:
    MultiDimArrayMemoryServiceSkel(boost::string_ref membername, const RR_SHARED_PTR<ServiceSkel>& skel,
                                   MemberDefinition_Direction direction)
        : MultiDimArrayMemoryServiceSkelBase(membername, skel, RRPrimUtil<T>::GetTypeID(), sizeof(T), direction)
    {}

    RR_OVIRTUAL RR_INTRUSIVE_PTR<MessageElementData> DoRead(
        const std::vector<uint64_t>& memorypos, const std::vector<uint64_t>& bufferpos,
        const std::vector<uint64_t>& count, uint32_t elemcount,
        const RR_SHARED_PTR<MultiDimArrayMemoryBase>& mem) RR_OVERRIDE
    {
        RR_SHARED_PTR<MultiDimArrayMemory<T> > mem1 = rr_cast<MultiDimArrayMemory<T> >(mem);

        RR_INTRUSIVE_PTR<RRArray<T> > real = AllocateRRArray<T>(boost::numeric_cast<size_t>(elemcount));
        RR_INTRUSIVE_PTR<RRMultiDimArray<T> > data;

        data = AllocateRRMultiDimArray<T>(VectorToRRArray<uint32_t>(count), real);

        mem1->Read(memorypos, data, bufferpos, count);
        return GetNode()->PackMultiDimArray(data);
    }
    RR_OVIRTUAL void DoWrite(const std::vector<uint64_t>& memorypos, const RR_INTRUSIVE_PTR<MessageElementData>& buffer,
                             const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count,
                             uint32_t elemcount, const RR_SHARED_PTR<MultiDimArrayMemoryBase>& mem) RR_OVERRIDE
    {
        RR_UNUSED(elemcount);
        RR_SHARED_PTR<MultiDimArrayMemory<T> > mem1 = rr_cast<MultiDimArrayMemory<T> >(mem);

        RR_INTRUSIVE_PTR<RRMultiDimArray<T> > data =
            GetNode()->template UnpackMultiDimArray<T>(rr_cast<MessageElementNestedElementList>(buffer));
        mem1->Write(memorypos, data, bufferpos, count);
    }
};

class ROBOTRACONTEUR_CORE_API ArrayMemoryClientBase : private boost::noncopyable
{
  protected:
    std::string m_MemberName;
    RR_WEAK_PTR<ServiceStub> stub;
    RR_WEAK_PTR<RobotRaconteurNode> node;
    MemberDefinition_Direction direction;
    DataTypes element_type;
    size_t element_size;
    std::string service_path;
    uint32_t endpoint;

  public:
    const std::string GetMemberName() const;
    RR_SHARED_PTR<RobotRaconteurNode> GetNode();
    ArrayMemoryClientBase(boost::string_ref membername, const RR_SHARED_PTR<ServiceStub>& stub, DataTypes element_type,
                          size_t element_size, MemberDefinition_Direction direction);
    virtual ~ArrayMemoryClientBase();

    RR_SHARED_PTR<ServiceStub> GetStub();
    virtual uint64_t Length();
    RobotRaconteur::MemberDefinition_Direction Direction();

  protected:
    bool max_size_read;
    uint32_t remote_max_size;
    boost::mutex max_size_lock;
    uint32_t GetMaxTransferSize();
    virtual void ReadBase(uint64_t memorypos, void* buffer, uint64_t bufferpos, uint64_t count);
    virtual void WriteBase(uint64_t memorypos, const void* buffer, uint64_t bufferpos, uint64_t count);

  public:
    void Shutdown();

  protected:
    virtual void UnpackReadResult(const RR_INTRUSIVE_PTR<MessageElementData>& res, void* buffer, uint64_t offset,
                                  uint64_t count) = 0;
    virtual RR_INTRUSIVE_PTR<MessageElementData> PackWriteRequest(const void* buffer, uint64_t offset,
                                                                  uint64_t count) = 0;
    virtual size_t GetBufferLength(const void* buffer) = 0;
};

template <typename T>
class ArrayMemoryClient : public virtual ArrayMemoryClientBase, public virtual ArrayMemory<T>
{
  public:
    ArrayMemoryClient(boost::string_ref membername, const RR_SHARED_PTR<ServiceStub>& stub,
                      MemberDefinition_Direction direction)
        : ArrayMemoryClientBase(membername, stub, RRPrimUtil<T>::GetTypeID(), sizeof(T), direction)
    {}

    RR_OVIRTUAL void Attach(const RR_INTRUSIVE_PTR<RRArray<T> >& memory) RR_OVERRIDE
    {
        RR_UNUSED(memory);
        throw InvalidOperationException("Invalid for client");
    }

    RR_OVIRTUAL void Read(uint64_t memorypos, RR_INTRUSIVE_PTR<RRArray<T> >& buffer, uint64_t bufferpos,
                          uint64_t count) RR_OVERRIDE
    {
        if (!buffer)
            throw NullValueException("Buffer must not be null");
        ReadBase(memorypos, &buffer, bufferpos, count);
    }

    RR_OVIRTUAL void Write(uint64_t memorypos, const RR_INTRUSIVE_PTR<RRArray<T> >& buffer, uint64_t bufferpos,
                           uint64_t count) RR_OVERRIDE
    {
        if (!buffer)
            throw NullValueException("Buffer must not be null");
        WriteBase(memorypos, &buffer, bufferpos, count);
    }

    RR_OVIRTUAL uint64_t Length() RR_OVERRIDE { return ArrayMemoryClientBase::Length(); }

  protected:
    RR_OVIRTUAL void UnpackReadResult(const RR_INTRUSIVE_PTR<MessageElementData>& res, void* buffer, uint64_t bufferpos,
                                      uint64_t count) RR_OVERRIDE
    {
        RR_INTRUSIVE_PTR<RRArray<T> > data = rr_cast<RRArray<T> >(res);
        RR_INTRUSIVE_PTR<RRArray<T> >& buffer1 = *static_cast<RR_INTRUSIVE_PTR<RRArray<T> >*>(buffer);
        memcpy(buffer1->data() + bufferpos, data->data(), boost::numeric_cast<size_t>(count * sizeof(T)));
    }

    RR_OVIRTUAL RR_INTRUSIVE_PTR<MessageElementData> PackWriteRequest(const void* buffer, uint64_t bufferpos,
                                                                      uint64_t count) RR_OVERRIDE
    {
        const RR_INTRUSIVE_PTR<RRArray<T> >& buffer1 = *static_cast<const RR_INTRUSIVE_PTR<RRArray<T> >*>(buffer);
        if (bufferpos == 0 && buffer1->size() == boost::numeric_cast<size_t>(count))
        {
            return buffer1;
        }
        else if ((buffer1->size() - boost::numeric_cast<size_t>(bufferpos)) >= boost::numeric_cast<size_t>(count))
        {
            RR_INTRUSIVE_PTR<RRArray<T> > data = AllocateRRArray<T>(boost::numeric_cast<size_t>(count));
            memcpy(data->data(), buffer1->data() + bufferpos, boost::numeric_cast<size_t>(count * sizeof(T)));
            return data;
        }
        else
            throw OutOfRangeException("");
    }

    RR_OVIRTUAL size_t GetBufferLength(const void* buffer) RR_OVERRIDE
    {
        const RR_INTRUSIVE_PTR<RRArray<T> >& buffer1 = *static_cast<const RR_INTRUSIVE_PTR<RRArray<T> >*>(buffer);
        return buffer1->size();
    }
};

class ROBOTRACONTEUR_CORE_API MultiDimArrayMemoryClientBase : private boost::noncopyable
{
  protected:
    std::string m_MemberName;
    RR_WEAK_PTR<ServiceStub> stub;
    RR_WEAK_PTR<RobotRaconteurNode> node;
    MemberDefinition_Direction direction;
    DataTypes element_type;
    size_t element_size;
    std::string service_path;
    uint32_t endpoint;

  public:
    RR_SHARED_PTR<RobotRaconteurNode> GetNode();
    const std::string GetMemberName() const;

    MultiDimArrayMemoryClientBase(boost::string_ref membername, const RR_SHARED_PTR<ServiceStub>& stub,
                                  DataTypes element_type, size_t element_size, MemberDefinition_Direction direction);
    virtual ~MultiDimArrayMemoryClientBase();
    RR_SHARED_PTR<ServiceStub> GetStub();
    RobotRaconteur::MemberDefinition_Direction Direction();
    virtual std::vector<uint64_t> Dimensions();
    virtual uint64_t DimCount();

  protected:
    bool max_size_read;
    uint32_t remote_max_size;
    boost::mutex max_size_lock;

    uint32_t GetMaxTransferSize();
    virtual void ReadBase(const std::vector<uint64_t>& memorypos, void* buffer, const std::vector<uint64_t>& bufferpos,
                          const std::vector<uint64_t>& count);
    virtual void WriteBase(const std::vector<uint64_t>& memorypos, const void* buffer,
                           const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count);

  public:
    void Shutdown();

  protected:
    virtual void UnpackReadResult(const RR_INTRUSIVE_PTR<MessageElementData>& res, void* buffer,
                                  const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count,
                                  uint64_t elemcount) = 0;
    virtual RR_INTRUSIVE_PTR<MessageElementData> PackWriteRequest(const void* buffer,
                                                                  const std::vector<uint64_t>& bufferpos,
                                                                  const std::vector<uint64_t>& count,
                                                                  uint64_t elemcount) = 0;
};

template <typename T>
class MultiDimArrayMemoryClient : public virtual MultiDimArrayMemory<T>, public virtual MultiDimArrayMemoryClientBase
{
  public:
    MultiDimArrayMemoryClient(boost::string_ref membername, const RR_SHARED_PTR<ServiceStub>& stub,
                              MemberDefinition_Direction direction)
        : MultiDimArrayMemoryClientBase(membername, stub, RRPrimUtil<T>::GetTypeID(), sizeof(T), direction)
    {}

    RR_OVIRTUAL void Attach(const RR_INTRUSIVE_PTR<RRMultiDimArray<T> >& memory) RR_OVERRIDE
    {
        RR_UNUSED(memory);
        throw InvalidOperationException("Not valid for client");
    }

    RR_OVIRTUAL std::vector<uint64_t> Dimensions() RR_OVERRIDE { return MultiDimArrayMemoryClientBase::Dimensions(); }

    RR_OVIRTUAL uint64_t DimCount() RR_OVERRIDE { return MultiDimArrayMemoryClientBase::DimCount(); }

    RR_OVIRTUAL void Read(const std::vector<uint64_t>& memorypos, RR_INTRUSIVE_PTR<RRMultiDimArray<T> >& buffer,
                          const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count) RR_OVERRIDE
    {
        if (!buffer)
            throw NullValueException("Buffer must not be null");
        ReadBase(memorypos, &buffer, bufferpos, count);
    }

    RR_OVIRTUAL void Write(const std::vector<uint64_t>& memorypos, const RR_INTRUSIVE_PTR<RRMultiDimArray<T> >& buffer,
                           const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count) RR_OVERRIDE
    {
        if (!buffer)
            throw NullValueException("Buffer must not be null");
        WriteBase(memorypos, &buffer, bufferpos, count);
    }

  protected:
    RR_OVIRTUAL void UnpackReadResult(const RR_INTRUSIVE_PTR<MessageElementData>& res, void* buffer,
                                      const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count,
                                      uint64_t elemcount) RR_OVERRIDE
    {
        RR_UNUSED(elemcount);
        RR_INTRUSIVE_PTR<RRMultiDimArray<T> >& buffer1 = *static_cast<RR_INTRUSIVE_PTR<RRMultiDimArray<T> >*>(buffer);
        RR_INTRUSIVE_PTR<RRMultiDimArray<T> > data =
            GetNode()->template UnpackMultiDimArray<T>(rr_cast<MessageElementNestedElementList>(res));

        RR_SHARED_PTR<MultiDimArrayMemory<T> > data2 = RR_MAKE_SHARED<MultiDimArrayMemory<T> >(data);
        data2->Read(std::vector<uint64_t>(count.size()), buffer1, bufferpos, count);
    }
    RR_OVIRTUAL RR_INTRUSIVE_PTR<MessageElementData> PackWriteRequest(const void* buffer,
                                                                      const std::vector<uint64_t>& bufferpos,
                                                                      const std::vector<uint64_t>& count,
                                                                      uint64_t elemcount) RR_OVERRIDE
    {
        const RR_INTRUSIVE_PTR<RRMultiDimArray<T> >& buffer1 =
            *static_cast<const RR_INTRUSIVE_PTR<RRMultiDimArray<T> >*>(buffer);

        bool equ = true;
        for (size_t i = 0; i < count.size(); i++)
        {
            if (bufferpos[i] != 0 || static_cast<uint64_t>((*buffer1->Dims)[i]) != count[i])
            {
                equ = false;
                break;
            }
        }

        if (equ)
        {
            return GetNode()->PackMultiDimArray(buffer1);
        }
        else
        {
            RR_INTRUSIVE_PTR<RRMultiDimArray<T> > data;

            data = AllocateRRMultiDimArray<T>(VectorToRRArray<uint32_t>(count),
                                              AllocateRRArray<T>(boost::numeric_cast<size_t>(elemcount)));

            buffer1->RetrieveSubArray(detail::ConvertVectorType<uint32_t>(bufferpos), data,
                                      std::vector<uint32_t>(count.size()), detail::ConvertVectorType<uint32_t>(count));
            return GetNode()->PackMultiDimArray(data);
        }
    }
};

// pod

/**
 * @brief Single dimensional pod random access memory region
 *
 * Memories represent random access memory regions that are typically
 * represented as arrays of various shapes and types. Memories can be
 * declared in service definition files using the `memory` member keyword
 * within service definitions. Services expose memories to clients, and
 * the nodes will proxy read, write, and parameter requests between the client
 * and service. The node will also break up large requests to avoid the
 * message size limit of the transport.
 *
 * The PodArrayMemory class is used to represent a single dimensional pod
 * array. Multidimensional pod arrays should use PodMultiDimArrayMemory.
 * Type T must be declared in a service definition using the `pod`
 * keyword, and generated using RobotRaconteurGen.
 *
 * PodArrayMemory instances are attached to an RRPodArrayPtr<T>, either when
 * constructed or later using Attach().
 *
 * PodArrayMemory instances returned by clients are special implementations
 * designed to proxy requests to the service. They cannot be attached
 * to an arbitrary array.
 *
 * @tparam T The pod type of the array
 */
template <typename T>
class PodArrayMemory : public virtual ArrayMemoryBase
{
  private:
    RR_INTRUSIVE_PTR<RRPodArray<T> > memory;
    boost::mutex memory_lock;

  public:
    /**
     * @brief Construct a new PodArrayMemory instance
     *
     * New instance will not be attached to an array.
     *
     * PodArrayMemory must be constructed with boost::make_shared<PodArrayMemory<T> >();
     */
    PodArrayMemory(){};

    /**
     * @brief Construct a new PodArrayMemory instance attached to an RRPodArrayPtr<T>
     *
     * New instance will be constructed attached to an array.
     *
     * PodArrayMemory must be constructed with boost::make_shared<PodArrayMemory<T> >();
     *
     * @param memory The array to attach
     */
    PodArrayMemory(const RR_INTRUSIVE_PTR<RRPodArray<T> >& memory) { this->memory = memory; }

    /**
     * @brief Attach PodArrayMemory instance to an RRPodArrayPtr<T>
     *
     * @param memory The array to attach
     */
    virtual void Attach(const RR_INTRUSIVE_PTR<RRPodArray<T> >& memory)
    {
        boost::mutex::scoped_lock lock(memory_lock);
        this->memory = memory;
    }

    /** @copydoc ArrayMemory::Length() */
    RR_OVIRTUAL uint64_t Length() RR_OVERRIDE
    {
        boost::mutex::scoped_lock lock(memory_lock);
        return memory->size();
    }

    /** @copydoc ArrayMemory::Read() */
    virtual void Read(uint64_t memorypos, RR_INTRUSIVE_PTR<RRPodArray<T> >& buffer, uint64_t bufferpos, uint64_t count)
    {
        boost::mutex::scoped_lock lock(memory_lock);
        if (memorypos + count > memory->size())
            throw OutOfRangeException("Index out of range");
        if (bufferpos + count > buffer->size())
            throw OutOfRangeException("Index out of range");

        for (size_t i = 0; i < count; i++)
        {
            buffer->at(boost::numeric_cast<typename RRPodArray<T>::size_type>(i + bufferpos)) =
                memory->at(boost::numeric_cast<typename RRPodArray<T>::size_type>(i + memorypos));
        }
    }

    /** @copydoc ArrayMemory::Write() */
    virtual void Write(uint64_t memorypos, const RR_INTRUSIVE_PTR<RRPodArray<T> >& buffer, uint64_t bufferpos,
                       uint64_t count)
    {
        boost::mutex::scoped_lock lock(memory_lock);
        if (memorypos + count > memory->size())
            throw OutOfRangeException("Index out of range");
        if (bufferpos + count > buffer->size())
            throw OutOfRangeException("Index out of range");
        for (size_t i = 0; i < count; i++)
        {
            memory->at(boost::numeric_cast<typename RRPodArray<T>::size_type>(i + memorypos)) =
                buffer->at(boost::numeric_cast<typename RRPodArray<T>::size_type>(i + bufferpos));
        }
    }

    RR_OVIRTUAL DataTypes ElementTypeID() RR_OVERRIDE { return DataTypes_pod_t; }
};

template <typename T>
class PodArrayMemoryServiceSkel : public ArrayMemoryServiceSkelBase
{
  public:
    PodArrayMemoryServiceSkel(boost::string_ref membername, const RR_SHARED_PTR<ServiceSkel>& skel, size_t element_size,
                              MemberDefinition_Direction direction)
        : ArrayMemoryServiceSkelBase(membername, skel, DataTypes_pod_t, element_size, direction)
    {}

    RR_OVIRTUAL RR_INTRUSIVE_PTR<MessageElementData> DoRead(uint64_t memorypos, uint64_t bufferpos, uint64_t count,
                                                            const RR_SHARED_PTR<ArrayMemoryBase>& mem) RR_OVERRIDE
    {
        RR_UNUSED(bufferpos);
        RR_SHARED_PTR<PodArrayMemory<T> > mem1 = rr_cast<PodArrayMemory<T> >(mem);
        RR_INTRUSIVE_PTR<RRPodArray<T> > buf1 = AllocateEmptyRRPodArray<T>(boost::numeric_cast<size_t>(count));
        mem1->Read(memorypos, buf1, 0, boost::numeric_cast<size_t>(count));
        return PodStub_PackPodArray(buf1);
    }

    RR_OVIRTUAL void DoWrite(uint64_t memorypos, const RR_INTRUSIVE_PTR<MessageElementData>& buffer, uint64_t bufferpos,
                             uint64_t count, const RR_SHARED_PTR<ArrayMemoryBase>& mem) RR_OVERRIDE
    {
        RR_UNUSED(bufferpos);
        RR_SHARED_PTR<PodArrayMemory<T> > mem1 = rr_cast<PodArrayMemory<T> >(mem);
        RR_INTRUSIVE_PTR<RRPodArray<T> > buf1 =
            PodStub_UnpackPodArray<T>(rr_cast<MessageElementNestedElementList>(buffer));
        mem1->Write(memorypos, buf1, 0, boost::numeric_cast<size_t>(count));
    }
};

template <typename T>
class PodArrayMemoryClient : public virtual ArrayMemoryClientBase, public virtual PodArrayMemory<T>
{
  public:
    PodArrayMemoryClient(boost::string_ref membername, const RR_SHARED_PTR<ServiceStub>& stub, size_t element_size,
                         MemberDefinition_Direction direction)
        : ArrayMemoryClientBase(membername, stub, RRPrimUtil<T>::GetTypeID(), element_size, direction)
    {}

    RR_OVIRTUAL void Attach(const RR_INTRUSIVE_PTR<RRPodArray<T> >& memory) RR_OVERRIDE
    {
        RR_UNUSED(memory);
        throw InvalidOperationException("Invalid for client");
    }

    RR_OVIRTUAL void Read(uint64_t memorypos, RR_INTRUSIVE_PTR<RRPodArray<T> >& buffer, uint64_t bufferpos,
                          uint64_t count) RR_OVERRIDE
    {
        if (!buffer)
            throw NullValueException("Buffer must not be null");
        ReadBase(memorypos, &buffer, bufferpos, count);
    }

    RR_OVIRTUAL void Write(uint64_t memorypos, const RR_INTRUSIVE_PTR<RRPodArray<T> >& buffer, uint64_t bufferpos,
                           uint64_t count) RR_OVERRIDE
    {
        if (!buffer)
            throw NullValueException("Buffer must not be null");
        WriteBase(memorypos, &buffer, bufferpos, count);
    }

    RR_OVIRTUAL uint64_t Length() RR_OVERRIDE { return ArrayMemoryClientBase::Length(); }

  protected:
    RR_OVIRTUAL void UnpackReadResult(const RR_INTRUSIVE_PTR<MessageElementData>& res, void* buffer, uint64_t bufferpos,
                                      uint64_t count) RR_OVERRIDE
    {
        RR_INTRUSIVE_PTR<RRPodArray<T> >& buffer1 = *static_cast<RR_INTRUSIVE_PTR<RRPodArray<T> >*>(buffer);
        RR_INTRUSIVE_PTR<RRPodArray<T> > res1 =
            PodStub_UnpackPodArray<T>(rr_cast<RobotRaconteur::MessageElementNestedElementList>(res));

        for (size_t i = 0; i < count; i++)
        {
            buffer1->at(boost::numeric_cast<typename RRPodArray<T>::size_type>(i + bufferpos)) = res1->at(i);
        }
    }

    RR_OVIRTUAL RR_INTRUSIVE_PTR<MessageElementData> PackWriteRequest(const void* buffer, uint64_t bufferpos,
                                                                      uint64_t count) RR_OVERRIDE
    {
        RR_INTRUSIVE_PTR<RRPodArray<T> > buffer1 = *static_cast<const RR_INTRUSIVE_PTR<RRPodArray<T> >*>(buffer);
        RR_INTRUSIVE_PTR<RRPodArray<T> > o = AllocateEmptyRRPodArray<T>(boost::numeric_cast<size_t>(count));

        for (size_t i = 0; i < count; i++)
        {
            o->at(i) = buffer1->at(boost::numeric_cast<typename RRPodArray<T>::size_type>(i + bufferpos));
        }
        return PodStub_PackPodArray(o);
    }

    RR_OVIRTUAL size_t GetBufferLength(const void* buffer) RR_OVERRIDE
    {
        const RR_INTRUSIVE_PTR<RRPodArray<T> >& buffer1 = *static_cast<const RR_INTRUSIVE_PTR<RRPodArray<T> >*>(buffer);
        return buffer1->size();
    }
};

/**
 * @brief Multidimensional pod random access memory region
 *
 * Memories represent random access memory regions that are typically
 * represented as arrays of various shapes and types. Memories can be
 * declared in service definition files using the `memory` member keyword
 * within service definitions. Services expose memories to clients, and
 * the nodes will proxy read, write, and parameter requests between the client
 * and service. The node will also break up large requests to avoid the
 * message size limit of the transport.
 *
 * The PodMultiDimArrayMemory class is used to represent a multidimensional
 * pod array. Single dimensional pod arrays should use PodArrayMemory.
 * Type T must be declared in a service definition using the `pod`
 * keyword, and generated using RobotRaconteurGen.
 *
 * PodMultiDimArrayMemory instances are attached to an RRPodMultiDimArrayPtr<T>,
 * either when constructed or later using Attach().
 *
 * PodMultiDimArrayMemory instances returned by clients are special implementations
 * designed to proxy requests to the service. They cannot be attached
 * to an arbitrary array.
 *
 * @tparam T The pod type of the array
 */
template <typename T>
class PodMultiDimArrayMemory : public virtual MultiDimArrayMemoryBase
{
  private:
    RR_INTRUSIVE_PTR<RRPodMultiDimArray<T> > multimemory;
    boost::mutex memory_lock;

  public:
    /**
     * @brief Construct a new PodMultiDimArrayMemory instance
     *
     * New instance will not be attached to an array.
     *
     * PodMultiDimArrayMemory must be constructed with boost::make_shared<PodMultiDimArrayMemory<T> >();
     */
    PodMultiDimArrayMemory(){};

    /**
     * @brief Construct a new PodMultiDimArrayMemory instance attached to an RRPodMultiDimArrayPtr<T>
     *
     * New instance will be constructed attached to an array.
     *
     * PodMultiDimArrayMemory must be constructed with boost::make_shared<PodMultiDimArrayMemory<T> >();
     *
     * @param multimemory The array to attach
     */
    PodMultiDimArrayMemory(const RR_INTRUSIVE_PTR<RRPodMultiDimArray<T> >& multimemory)
    {
        this->multimemory = multimemory;
    }

    RR_OVIRTUAL ~PodMultiDimArrayMemory() RR_OVERRIDE {}

    /**
     * @brief Attach PodMultiDimArrayMemory instance to an RRPodMultiDimArrayPtr<T>
     *
     * @param multimemory The array to attach
     */
    virtual void Attach(const RR_INTRUSIVE_PTR<RRPodMultiDimArray<T> >& multimemory)
    {
        boost::mutex::scoped_lock lock(memory_lock);
        this->multimemory = multimemory;
    }

    /** @copydoc MultiDimArrayMemory::Dimensions() */
    RR_OVIRTUAL std::vector<uint64_t> Dimensions() RR_OVERRIDE
    {
        boost::mutex::scoped_lock lock(memory_lock);
        RR_INTRUSIVE_PTR<RRArray<uint32_t> > dims = multimemory->Dims;
        std::vector<uint64_t> s(dims->size());
        for (size_t i = 0; i < s.size(); i++)
        {
            s[i] = (*dims)[i];
        }

        return s;
    }

    /** @copydoc MultiDimArrayMemory::DimCount() */
    RR_OVIRTUAL uint64_t DimCount() RR_OVERRIDE
    {
        boost::mutex::scoped_lock lock(memory_lock);
        return multimemory->Dims->size();
    }

    /** @copydoc MultiDimArrayMemory::Read() */
    virtual void Read(const std::vector<uint64_t>& memorypos, RR_INTRUSIVE_PTR<RRPodMultiDimArray<T> >& buffer,
                      const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count)
    {
        boost::mutex::scoped_lock lock(memory_lock);
        multimemory->RetrieveSubArray(detail::ConvertVectorType<uint32_t>(memorypos), buffer,
                                      detail::ConvertVectorType<uint32_t>(bufferpos),
                                      detail::ConvertVectorType<uint32_t>(count));
    }

    /** @copydoc MultiDimArrayMemory::Write() */
    virtual void Write(const std::vector<uint64_t>& memorypos, const RR_INTRUSIVE_PTR<RRPodMultiDimArray<T> >& buffer,
                       const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count)
    {
        boost::mutex::scoped_lock lock(memory_lock);
        multimemory->AssignSubArray(detail::ConvertVectorType<uint32_t>(memorypos), buffer,
                                    detail::ConvertVectorType<uint32_t>(bufferpos),
                                    detail::ConvertVectorType<uint32_t>(count));
    }

    RR_OVIRTUAL DataTypes ElementTypeID() RR_OVERRIDE { return DataTypes_pod_t; }
};

template <typename T>
class PodMultiDimArrayMemoryClient : public virtual PodMultiDimArrayMemory<T>,
                                     public virtual MultiDimArrayMemoryClientBase
{
  public:
    PodMultiDimArrayMemoryClient(boost::string_ref membername, const RR_SHARED_PTR<ServiceStub>& stub,
                                 size_t element_size, MemberDefinition_Direction direction)
        : MultiDimArrayMemoryClientBase(membername, stub, DataTypes_pod_t, element_size, direction)
    {}

    RR_OVIRTUAL void Attach(const RR_INTRUSIVE_PTR<RRPodMultiDimArray<T> >& memory) RR_OVERRIDE
    {
        RR_UNUSED(memory);
        throw InvalidOperationException("Not valid for client");
    }

    RR_OVIRTUAL std::vector<uint64_t> Dimensions() RR_OVERRIDE { return MultiDimArrayMemoryClientBase::Dimensions(); }

    RR_OVIRTUAL uint64_t DimCount() RR_OVERRIDE { return MultiDimArrayMemoryClientBase::DimCount(); }

    RR_OVIRTUAL void Read(const std::vector<uint64_t>& memorypos, RR_INTRUSIVE_PTR<RRPodMultiDimArray<T> >& buffer,
                          const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count) RR_OVERRIDE
    {
        if (!buffer)
            throw NullValueException("Buffer must not be null");
        ReadBase(memorypos, &buffer, bufferpos, count);
    }

    RR_OVIRTUAL void Write(const std::vector<uint64_t>& memorypos,
                           const RR_INTRUSIVE_PTR<RRPodMultiDimArray<T> >& buffer,
                           const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count) RR_OVERRIDE
    {
        if (!buffer)
            throw NullValueException("Buffer must not be null");
        WriteBase(memorypos, &buffer, bufferpos, count);
    }

  protected:
    RR_OVIRTUAL void UnpackReadResult(const RR_INTRUSIVE_PTR<MessageElementData>& res, void* buffer,
                                      const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count,
                                      uint64_t elemcount) RR_OVERRIDE
    {
        RR_UNUSED(elemcount);
        RR_INTRUSIVE_PTR<RRPodMultiDimArray<T> >& buffer1 =
            *static_cast<RR_INTRUSIVE_PTR<RRPodMultiDimArray<T> >*>(buffer);
        RR_INTRUSIVE_PTR<RRPodMultiDimArray<T> > data = rr_cast<RRPodMultiDimArray<T> >(
            GetNode()->UnpackPodMultiDimArray(rr_cast<MessageElementNestedElementList>(res)));

        RR_SHARED_PTR<PodMultiDimArrayMemory<T> > data2 = RR_MAKE_SHARED<PodMultiDimArrayMemory<T> >(data);
        data2->Read(std::vector<uint64_t>(count.size()), buffer1, bufferpos, count);
    }
    RR_OVIRTUAL RR_INTRUSIVE_PTR<MessageElementData> PackWriteRequest(const void* buffer,
                                                                      const std::vector<uint64_t>& bufferpos,
                                                                      const std::vector<uint64_t>& count,
                                                                      uint64_t elemcount) RR_OVERRIDE
    {
        const RR_INTRUSIVE_PTR<RRPodMultiDimArray<T> >& buffer1 =
            *static_cast<const RR_INTRUSIVE_PTR<RRPodMultiDimArray<T> >*>(buffer);

        RR_INTRUSIVE_PTR<RRPodMultiDimArray<T> > data = AllocateEmptyRRPodMultiDimArray<T>();
        data->Dims = VectorToRRArray<uint32_t>(count);
        data->PodArray = AllocateEmptyRRPodArray<T>(boost::numeric_cast<size_t>(elemcount));

        buffer1->RetrieveSubArray(detail::ConvertVectorType<uint32_t>(bufferpos), data,
                                  std::vector<uint32_t>(count.size()), detail::ConvertVectorType<uint32_t>(count));
        return GetNode()->PackPodMultiDimArray(data);
    }
};

template <typename T>
class PodMultiDimArrayMemoryServiceSkel : public MultiDimArrayMemoryServiceSkelBase
{
  public:
    PodMultiDimArrayMemoryServiceSkel(boost::string_ref membername, const RR_SHARED_PTR<ServiceSkel>& skel,
                                      size_t element_size, MemberDefinition_Direction direction)
        : MultiDimArrayMemoryServiceSkelBase(membername, skel, DataTypes_structure_t, element_size, direction)
    {}

    RR_OVIRTUAL RR_INTRUSIVE_PTR<MessageElementData> DoRead(
        const std::vector<uint64_t>& memorypos, const std::vector<uint64_t>& bufferpos,
        const std::vector<uint64_t>& count, uint32_t elemcount,
        const RR_SHARED_PTR<MultiDimArrayMemoryBase>& mem) RR_OVERRIDE
    {
        RR_SHARED_PTR<PodMultiDimArrayMemory<T> > mem1 = rr_cast<PodMultiDimArrayMemory<T> >(mem);

        RR_INTRUSIVE_PTR<RRPodMultiDimArray<T> > data = AllocateEmptyRRPodMultiDimArray<T>();
        data->Dims = VectorToRRArray<uint32_t>(count);
        data->PodArray = AllocateEmptyRRPodArray<T>(boost::numeric_cast<size_t>(elemcount));

        mem1->Read(memorypos, data, bufferpos, count);
        return GetNode()->PackPodMultiDimArray(data);
    }
    RR_OVIRTUAL void DoWrite(const std::vector<uint64_t>& memorypos, const RR_INTRUSIVE_PTR<MessageElementData>& buffer,
                             const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count,
                             uint32_t elemcount, const RR_SHARED_PTR<MultiDimArrayMemoryBase>& mem) RR_OVERRIDE
    {
        RR_UNUSED(elemcount);
        RR_SHARED_PTR<PodMultiDimArrayMemory<T> > mem1 = rr_cast<PodMultiDimArrayMemory<T> >(mem);

        RR_INTRUSIVE_PTR<RRPodMultiDimArray<T> > data = rr_cast<RRPodMultiDimArray<T> >(
            GetNode()->UnpackPodMultiDimArray(rr_cast<MessageElementNestedElementList>(buffer)));
        mem1->Write(memorypos, data, bufferpos, count);
    }
};

// namedarray

/**
 * @brief Single dimensional namedarray random access memory region
 *
 * Memories represent random access memory regions that are typically
 * represented as arrays of various shapes and types. Memories can be
 * declared in service definition files using the `memory` member keyword
 * within service definitions. Services expose memories to clients, and
 * the nodes will proxy read, write, and parameter requests between the client
 * and service. The node will also break up large requests to avoid the
 * message size limit of the transport.
 *
 * The NamedArrayMemory class is used to represent a single dimensional named
 * array. Multidimensional named arrays should use NamedMultiDimArrayMemory.
 * Type T must be declared in a service definition using the `namedarray`
 * keyword, and generated using RobotRaconteurGen.
 *
 * NamedArrayMemory instances are attached to an RRNamedArrayPtr<T>, either when
 * constructed or later using Attach().
 *
 * NamedArrayMemory instances returned by clients are special implementations
 * designed to proxy requests to the service. They cannot be attached
 * to an arbitrary array.
 *
 * @tparam T The namedarray type of the array
 */
template <typename T>
class NamedArrayMemory : public virtual ArrayMemoryBase
{
  private:
    RR_INTRUSIVE_PTR<RRNamedArray<T> > memory;
    boost::mutex memory_lock;

  public:
    /**
     * @brief Construct a new NamedArrayMemory instance
     *
     * New instance will not be attached to an array.
     *
     * NamedArrayMemory must be constructed with boost::make_shared<NamedArrayMemory<T> >();
     */
    NamedArrayMemory(){};

    /**
     * @brief Construct a new NamedArrayMemory instance attached to an RRNamedArrayPtr<T>
     *
     * New instance will be constructed attached to an array.
     *
     * NamedArrayMemory must be constructed with boost::make_shared<NamedArrayMemory<T> >();
     *
     * @param memory The array to attach
     */
    NamedArrayMemory(const RR_INTRUSIVE_PTR<RRNamedArray<T> >& memory) { this->memory = memory; }

    /**
     * @brief Attach NamedArrayMemory instance to an RRNamedArrayPtr<T>
     *
     * @param memory The array to attach
     */
    virtual void Attach(const RR_INTRUSIVE_PTR<RRNamedArray<T> >& memory)
    {
        boost::mutex::scoped_lock lock(memory_lock);
        this->memory = memory;
    }

    /** @copydoc ArrayMemory::Length() */
    RR_OVIRTUAL uint64_t Length() RR_OVERRIDE
    {
        boost::mutex::scoped_lock lock(memory_lock);
        return memory->size();
    }

    /** @copydoc ArrayMemory::Read() */
    virtual void Read(uint64_t memorypos, RR_INTRUSIVE_PTR<RRNamedArray<T> >& buffer, uint64_t bufferpos,
                      uint64_t count)
    {
        boost::mutex::scoped_lock lock(memory_lock);
        if (memorypos + count > memory->size())
            throw OutOfRangeException("Index out of range");
        if (bufferpos + count > buffer->size())
            throw OutOfRangeException("Index out of range");

        for (size_t i = 0; i < count; i++)
        {
            (*buffer)[boost::numeric_cast<typename RRNamedArray<T>::size_type>(i + bufferpos)] =
                (*memory)[boost::numeric_cast<typename RRNamedArray<T>::size_type>(i + memorypos)];
        }
    }

    /** @copydoc ArrayMemory::Write() */
    virtual void Write(uint64_t memorypos, const RR_INTRUSIVE_PTR<RRNamedArray<T> >& buffer, uint64_t bufferpos,
                       uint64_t count)
    {
        boost::mutex::scoped_lock lock(memory_lock);
        if (memorypos + count > memory->size())
            throw OutOfRangeException("Index out of range");
        if (bufferpos + count > buffer->size())
            throw OutOfRangeException("Index out of range");
        for (size_t i = 0; i < count; i++)
        {
            (*memory)[boost::numeric_cast<typename RRNamedArray<T>::size_type>(i + memorypos)] =
                (*buffer)[boost::numeric_cast<typename RRNamedArray<T>::size_type>(i + bufferpos)];
        }
    }

    RR_OVIRTUAL DataTypes ElementTypeID() RR_OVERRIDE { return DataTypes_pod_t; }
};

template <typename T>
class NamedArrayMemoryServiceSkel : public ArrayMemoryServiceSkelBase
{
  public:
    NamedArrayMemoryServiceSkel(boost::string_ref membername, const RR_SHARED_PTR<ServiceSkel>& skel,
                                size_t element_size, MemberDefinition_Direction direction)
        : ArrayMemoryServiceSkelBase(membername, skel, DataTypes_pod_t, element_size, direction)
    {}

    RR_OVIRTUAL RR_INTRUSIVE_PTR<MessageElementData> DoRead(uint64_t memorypos, uint64_t bufferpos, uint64_t count,
                                                            const RR_SHARED_PTR<ArrayMemoryBase>& mem) RR_OVERRIDE
    {
        RR_UNUSED(bufferpos);
        RR_SHARED_PTR<NamedArrayMemory<T> > mem1 = rr_cast<NamedArrayMemory<T> >(mem);
        RR_INTRUSIVE_PTR<RRNamedArray<T> > buf1 = AllocateEmptyRRNamedArray<T>(boost::numeric_cast<size_t>(count));
        mem1->Read(memorypos, buf1, 0, boost::numeric_cast<size_t>(count));
        return NamedArrayStub_PackNamedArray(buf1);
    }

    RR_OVIRTUAL void DoWrite(uint64_t memorypos, const RR_INTRUSIVE_PTR<MessageElementData>& buffer, uint64_t bufferpos,
                             uint64_t count, const RR_SHARED_PTR<ArrayMemoryBase>& mem) RR_OVERRIDE
    {
        RR_UNUSED(bufferpos);
        RR_SHARED_PTR<NamedArrayMemory<T> > mem1 = rr_cast<NamedArrayMemory<T> >(mem);
        RR_INTRUSIVE_PTR<RRNamedArray<T> > buf1 =
            NamedArrayStub_UnpackNamedArray<T>(rr_cast<MessageElementNestedElementList>(buffer));
        mem1->Write(memorypos, buf1, 0, boost::numeric_cast<size_t>(count));
    }
};

template <typename T>
class NamedArrayMemoryClient : public virtual ArrayMemoryClientBase, public virtual NamedArrayMemory<T>
{
  public:
    NamedArrayMemoryClient(boost::string_ref membername, const RR_SHARED_PTR<ServiceStub>& stub, size_t element_size,
                           MemberDefinition_Direction direction)
        : ArrayMemoryClientBase(membername, stub, RRPrimUtil<T>::GetTypeID(), element_size, direction)
    {}

    RR_OVIRTUAL void Attach(const RR_INTRUSIVE_PTR<RRNamedArray<T> >& memory) RR_OVERRIDE
    {
        RR_UNUSED(memory);
        throw InvalidOperationException("Invalid for client");
    }

    RR_OVIRTUAL void Read(uint64_t memorypos, RR_INTRUSIVE_PTR<RRNamedArray<T> >& buffer, uint64_t bufferpos,
                          uint64_t count) RR_OVERRIDE
    {
        if (!buffer)
            throw NullValueException("Buffer must not be null");
        ReadBase(memorypos, &buffer, bufferpos, count);
    }

    RR_OVIRTUAL void Write(uint64_t memorypos, const RR_INTRUSIVE_PTR<RRNamedArray<T> >& buffer, uint64_t bufferpos,
                           uint64_t count) RR_OVERRIDE
    {
        if (!buffer)
            throw NullValueException("Buffer must not be null");
        WriteBase(memorypos, &buffer, bufferpos, count);
    }

    RR_OVIRTUAL uint64_t Length() RR_OVERRIDE { return ArrayMemoryClientBase::Length(); }

  protected:
    RR_OVIRTUAL void UnpackReadResult(const RR_INTRUSIVE_PTR<MessageElementData>& res, void* buffer, uint64_t bufferpos,
                                      uint64_t count) RR_OVERRIDE
    {
        RR_INTRUSIVE_PTR<RRNamedArray<T> >& buffer1 = *static_cast<RR_INTRUSIVE_PTR<RRNamedArray<T> >*>(buffer);
        RR_INTRUSIVE_PTR<RRNamedArray<T> > res1 =
            NamedArrayStub_UnpackNamedArray<T>(rr_cast<RobotRaconteur::MessageElementNestedElementList>(res));

        for (size_t i = 0; i < count; i++)
        {
            (*buffer1)[boost::numeric_cast<typename RRNamedArray<T>::size_type>(i + bufferpos)] = (*res1)[i];
        }
    }

    RR_OVIRTUAL RR_INTRUSIVE_PTR<MessageElementData> PackWriteRequest(const void* buffer, uint64_t bufferpos,
                                                                      uint64_t count) RR_OVERRIDE
    {
        RR_INTRUSIVE_PTR<RRNamedArray<T> > buffer1 = *static_cast<const RR_INTRUSIVE_PTR<RRNamedArray<T> >*>(buffer);
        RR_INTRUSIVE_PTR<RRNamedArray<T> > o = AllocateEmptyRRNamedArray<T>(boost::numeric_cast<size_t>(count));

        for (size_t i = 0; i < count; i++)
        {
            (*o)[i] = (*buffer1)[boost::numeric_cast<typename RRNamedArray<T>::size_type>(i + bufferpos)];
        }
        return NamedArrayStub_PackNamedArray(o);
    }

    RR_OVIRTUAL size_t GetBufferLength(const void* buffer) RR_OVERRIDE
    {
        RR_INTRUSIVE_PTR<RRNamedArray<T> > buffer1 = *static_cast<const RR_INTRUSIVE_PTR<RRNamedArray<T> >*>(buffer);
        return buffer1->size();
    }
};

/**
 * @brief Multidimensional namedarray random access memory region
 *
 * Memories represent random access memory regions that are typically
 * represented as arrays of various shapes and types. Memories can be
 * declared in service definition files using the `memory` member keyword
 * within service definitions. Services expose memories to clients, and
 * the nodes will proxy read, write, and parameter requests between the client
 * and service. The node will also break up large requests to avoid the
 * message size limit of the transport.
 *
 * The NamedMultiDimArrayMemory class is used to represent a multidimensional
 * named array. Single dimensional named arrays should use NamedArrayMemory.
 * Type T must be declared in a service definition using the `namedarray`
 * keyword, and generated using RobotRaconteurGen.
 *
 * NamedMultiDimArrayMemory instances are attached to an RRNamedMultiDimArrayPtr<T>,
 * either when constructed or later using Attach().
 *
 * NamedMultiDimArrayMemory instances returned by clients are special implementations
 * designed to proxy requests to the service. They cannot be attached
 * to an arbitrary array.
 *
 * @tparam T The namedarray type of the array
 */
template <typename T>
class NamedMultiDimArrayMemory : public virtual MultiDimArrayMemoryBase
{
  private:
    RR_INTRUSIVE_PTR<RRNamedMultiDimArray<T> > multimemory;
    boost::mutex memory_lock;

  public:
    /**
     * @brief Construct a new NamedMultiDimArrayMemory instance
     *
     * New instance will not be attached to an array.
     *
     * NamedMultiDimArrayMemory must be constructed with boost::make_shared<NamedMultiDimArrayMemory<T> >();
     */
    NamedMultiDimArrayMemory(){};

    /**
     * @brief Construct a new NamedMultiDimArrayMemory instance attached to an RRNamedMultiDimArrayPtr<T>
     *
     * New instance will be constructed attached to an array.
     *
     * NamedMultiDimArrayMemory must be constructed with boost::make_shared<NamedMultiDimArrayMemory<T> >();
     *
     * @param multimemory The array to attach
     */
    NamedMultiDimArrayMemory(const RR_INTRUSIVE_PTR<RRNamedMultiDimArray<T> >& multimemory)
    {
        this->multimemory = multimemory;
    }

    RR_OVIRTUAL ~NamedMultiDimArrayMemory() RR_OVERRIDE {}

    /**
     * @brief Attach NamedMultiDimArrayMemory instance to an RRNamedMultiDimArrayPtr<T>
     *
     * @param multimemory The array to attach
     */
    virtual void Attach(const RR_INTRUSIVE_PTR<RRNamedMultiDimArray<T> >& multimemory)
    {
        boost::mutex::scoped_lock lock(memory_lock);
        this->multimemory = multimemory;
    }

    /** @copydoc MultiDimArrayMemory::Dimensions() */
    RR_OVIRTUAL std::vector<uint64_t> Dimensions() RR_OVERRIDE
    {
        boost::mutex::scoped_lock lock(memory_lock);
        RR_INTRUSIVE_PTR<RRArray<uint32_t> > dims = multimemory->Dims;
        std::vector<uint64_t> s(dims->size());
        for (size_t i = 0; i < s.size(); i++)
        {
            s[i] = (*dims)[i];
        }

        return s;
    }

    /** @copydoc MultiDimArrayMemory::DimCount() */
    RR_OVIRTUAL uint64_t DimCount() RR_OVERRIDE
    {
        boost::mutex::scoped_lock lock(memory_lock);
        return multimemory->Dims->size();
    }

    /** @copydoc MultiDimArrayMemory::Read() */
    virtual void Read(const std::vector<uint64_t>& memorypos, RR_INTRUSIVE_PTR<RRNamedMultiDimArray<T> >& buffer,
                      const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count)
    {
        boost::mutex::scoped_lock lock(memory_lock);
        multimemory->RetrieveSubArray(detail::ConvertVectorType<uint32_t>(memorypos), buffer,
                                      detail::ConvertVectorType<uint32_t>(bufferpos),
                                      detail::ConvertVectorType<uint32_t>(count));
    }

    /** @copydoc MultiDimArrayMemory::Write() */
    virtual void Write(const std::vector<uint64_t>& memorypos, const RR_INTRUSIVE_PTR<RRNamedMultiDimArray<T> >& buffer,
                       const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count)
    {
        boost::mutex::scoped_lock lock(memory_lock);
        multimemory->AssignSubArray(detail::ConvertVectorType<uint32_t>(memorypos), buffer,
                                    detail::ConvertVectorType<uint32_t>(bufferpos),
                                    detail::ConvertVectorType<uint32_t>(count));
    }

    RR_OVIRTUAL DataTypes ElementTypeID() RR_OVERRIDE { return DataTypes_pod_t; }
};

template <typename T>
class NamedMultiDimArrayMemoryClient : public virtual NamedMultiDimArrayMemory<T>,
                                       public virtual MultiDimArrayMemoryClientBase
{
  public:
    NamedMultiDimArrayMemoryClient(boost::string_ref membername, const RR_SHARED_PTR<ServiceStub>& stub,
                                   size_t element_size, MemberDefinition_Direction direction)
        : MultiDimArrayMemoryClientBase(membername, stub, DataTypes_pod_t, element_size, direction)
    {}

    RR_OVIRTUAL void Attach(const RR_INTRUSIVE_PTR<RRNamedMultiDimArray<T> >& memory) RR_OVERRIDE
    {
        RR_UNUSED(memory);
        throw InvalidOperationException("Not valid for client");
    }

    RR_OVIRTUAL std::vector<uint64_t> Dimensions() RR_OVERRIDE { return MultiDimArrayMemoryClientBase::Dimensions(); }

    RR_OVIRTUAL uint64_t DimCount() RR_OVERRIDE { return MultiDimArrayMemoryClientBase::DimCount(); }

    RR_OVIRTUAL void Read(const std::vector<uint64_t>& memorypos, RR_INTRUSIVE_PTR<RRNamedMultiDimArray<T> >& buffer,
                          const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count) RR_OVERRIDE
    {
        if (!buffer)
            throw NullValueException("Buffer must not be null");
        ReadBase(memorypos, &buffer, bufferpos, count);
    }

    RR_OVIRTUAL void Write(const std::vector<uint64_t>& memorypos,
                           const RR_INTRUSIVE_PTR<RRNamedMultiDimArray<T> >& buffer,
                           const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count) RR_OVERRIDE
    {
        if (!buffer)
            throw NullValueException("Buffer must not be null");
        WriteBase(memorypos, &buffer, bufferpos, count);
    }

  protected:
    RR_OVIRTUAL void UnpackReadResult(const RR_INTRUSIVE_PTR<MessageElementData>& res, void* buffer,
                                      const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count,
                                      uint64_t elemcount) RR_OVERRIDE
    {
        RR_UNUSED(elemcount);
        RR_INTRUSIVE_PTR<RRNamedMultiDimArray<T> >& buffer1 =
            *static_cast<RR_INTRUSIVE_PTR<RRNamedMultiDimArray<T> >*>(buffer);
        RR_INTRUSIVE_PTR<RRNamedMultiDimArray<T> > data = rr_cast<RRNamedMultiDimArray<T> >(
            GetNode()->UnpackNamedMultiDimArray(rr_cast<MessageElementNestedElementList>(res)));

        RR_SHARED_PTR<NamedMultiDimArrayMemory<T> > data2 = RR_MAKE_SHARED<NamedMultiDimArrayMemory<T> >(data);
        data2->Read(std::vector<uint64_t>(count.size()), buffer1, bufferpos, count);
    }
    RR_OVIRTUAL RR_INTRUSIVE_PTR<MessageElementData> PackWriteRequest(const void* buffer,
                                                                      const std::vector<uint64_t>& bufferpos,
                                                                      const std::vector<uint64_t>& count,
                                                                      uint64_t elemcount) RR_OVERRIDE
    {
        RR_INTRUSIVE_PTR<RRNamedMultiDimArray<T> > buffer1 =
            *static_cast<const RR_INTRUSIVE_PTR<RRNamedMultiDimArray<T> >*>(buffer);

        RR_INTRUSIVE_PTR<RRNamedMultiDimArray<T> > data = AllocateEmptyRRNamedMultiDimArray<T>();
        data->Dims = VectorToRRArray<uint32_t>(count);
        data->NamedArray = AllocateEmptyRRNamedArray<T>(boost::numeric_cast<size_t>(elemcount));

        buffer1->RetrieveSubArray(detail::ConvertVectorType<uint32_t>(bufferpos), data,
                                  std::vector<uint32_t>(count.size()), detail::ConvertVectorType<uint32_t>(count));
        return GetNode()->PackNamedMultiDimArray(data);
    }
};

template <typename T>
class NamedMultiDimArrayMemoryServiceSkel : public MultiDimArrayMemoryServiceSkelBase
{
  public:
    NamedMultiDimArrayMemoryServiceSkel(boost::string_ref membername, const RR_SHARED_PTR<ServiceSkel>& skel,
                                        size_t element_size, MemberDefinition_Direction direction)
        : MultiDimArrayMemoryServiceSkelBase(membername, skel, DataTypes_structure_t, element_size, direction)
    {}

    RR_OVIRTUAL RR_INTRUSIVE_PTR<MessageElementData> DoRead(
        const std::vector<uint64_t>& memorypos, const std::vector<uint64_t>& bufferpos,
        const std::vector<uint64_t>& count, uint32_t elemcount,
        const RR_SHARED_PTR<MultiDimArrayMemoryBase>& mem) RR_OVERRIDE
    {
        RR_SHARED_PTR<NamedMultiDimArrayMemory<T> > mem1 = rr_cast<NamedMultiDimArrayMemory<T> >(mem);

        RR_INTRUSIVE_PTR<RRNamedMultiDimArray<T> > data = AllocateEmptyRRNamedMultiDimArray<T>();
        data->Dims = VectorToRRArray<uint32_t>(count);
        data->NamedArray = AllocateEmptyRRNamedArray<T>(boost::numeric_cast<size_t>(elemcount));

        mem1->Read(memorypos, data, bufferpos, count);
        return GetNode()->PackNamedMultiDimArray(data);
    }
    RR_OVIRTUAL void DoWrite(const std::vector<uint64_t>& memorypos, const RR_INTRUSIVE_PTR<MessageElementData>& buffer,
                             const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count,
                             uint32_t elemcount, const RR_SHARED_PTR<MultiDimArrayMemoryBase>& mem) RR_OVERRIDE
    {
        RR_UNUSED(elemcount);
        RR_SHARED_PTR<NamedMultiDimArrayMemory<T> > mem1 = rr_cast<NamedMultiDimArrayMemory<T> >(mem);

        RR_INTRUSIVE_PTR<RRNamedMultiDimArray<T> > data = rr_cast<RRNamedMultiDimArray<T> >(
            GetNode()->UnpackNamedMultiDimArray(rr_cast<MessageElementNestedElementList>(buffer)));
        mem1->Write(memorypos, data, bufferpos, count);
    }
};

#ifndef BOOST_NO_CXX11_TEMPLATE_ALIASES
/** @brief Convenience alias for ArrayMemoryBase shared_ptr */
using ArrayMemoryBasePtr = RR_SHARED_PTR<ArrayMemoryBase>;
/** @brief Convenience alias for ArrayMemory shared_ptr */
template <typename T>
using ArrayMemoryPtr = RR_SHARED_PTR<ArrayMemory<T> >;
/** @brief Convenience alias for MultiDimArrayMemoryBase shared_ptr */
using MultiDimArrayMemoryBasePtr = RR_SHARED_PTR<MultiDimArrayMemoryBase>;
/** @brief Convenience alias for MultiDimArrayMemory shared_ptr */
template <typename T>
using MultiDimArrayMemoryPtr = RR_SHARED_PTR<MultiDimArrayMemory<T> >;
/** @brief Convenience alias for PodArrayMemory shared_ptr */
template <typename T>
using PodArrayMemoryPtr = RR_SHARED_PTR<PodArrayMemory<T> >;
/** @brief Convenience alias for PodMultiDimArrayMemory shared_ptr */
template <typename T>
using PodMultiDimArrayMemoryPtr = RR_SHARED_PTR<PodMultiDimArrayMemory<T> >;
/** @brief Convenience alias for NamedArrayMemory shared_ptr */
template <typename T>
using NamedArrayMemoryPtr = RR_SHARED_PTR<NamedArrayMemory<T> >;
/** @brief Convenience alias for NamedMultiDimArrayMemory shared_ptr */
template <typename T>
using NamedMultiDimArrayMemoryPtr = RR_SHARED_PTR<NamedMultiDimArrayMemory<T> >;
#endif
} // namespace RobotRaconteur
