// Copyright 2011-2018 Wason Technology, LLC
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
		template <typename Y,typename U>
		static std::vector<Y> ConvertVectorType(std::vector<U> in)
		{
			std::vector<Y> out(in.size());
			for (size_t i=0; i<in.size(); i++) out[i]=(Y)in[i];
			return out;
		}

		

		template <typename Y,typename U>
		static RR_INTRUSIVE_PTR<RRArray<Y> > ConvertRRArrayType(RR_INTRUSIVE_PTR<RRArray<U> > in)
		{
			RR_INTRUSIVE_PTR<RRArray<Y> > out=AllocateRRArray<Y>(in->size());
			for (size_t i=0; i<in->size(); i++) (*out)[i]=(Y)(*in)[i];
			return out;
		}

		ROBOTRACONTEUR_CORE_API void CalculateMatrixBlocks(uint32_t element_size, std::vector<uint64_t> count, uint64_t max_elems, uint32_t &split_dim, uint64_t &split_dim_block, uint64_t &split_elem_count, uint32_t &splits_count, uint32_t &split_remainder, std::vector<uint64_t>& block_count, std::vector<uint64_t>& block_count_edge);
	}



	class ROBOTRACONTEUR_CORE_API ArrayMemoryBase
	{
	public:
		virtual uint64_t Length()=0;
		virtual DataTypes ElementTypeID()=0;
		virtual ~ArrayMemoryBase() {}

	};

	template <typename T>
	class ArrayMemory : public virtual ArrayMemoryBase
	{
	private:
		RR_INTRUSIVE_PTR<RRArray<T> > memory;
		boost::mutex memory_lock;

	public:

		ArrayMemory() {};

		ArrayMemory(RR_INTRUSIVE_PTR<RRArray<T> > memory)
		{
			this->memory=memory;
		}

		virtual void Attach(RR_INTRUSIVE_PTR<RRArray<T> > memory)
		{
			boost::mutex::scoped_lock lock(memory_lock);
			this->memory=memory;
		}

		virtual uint64_t Length()
		{
			boost::mutex::scoped_lock lock(memory_lock);
			return memory->size();
		}

		virtual void Read(uint64_t memorypos, RR_INTRUSIVE_PTR<RRArray<T> > buffer, uint64_t bufferpos, uint64_t count)
		{
			boost::mutex::scoped_lock lock(memory_lock);
			if (memorypos+count > memory->size()) throw OutOfRangeException("Index out of range");
			if (bufferpos+count > buffer->size()) throw OutOfRangeException("Index out of range");

			memcpy(buffer->data()+bufferpos,memory->data()+memorypos,(size_t)count*sizeof(T));
		}

		virtual void Write(uint64_t memorypos, RR_INTRUSIVE_PTR<RRArray<T> > buffer, uint64_t bufferpos, uint64_t count)
		{
			boost::mutex::scoped_lock lock(memory_lock);
			if (memorypos+count > memory->size()) throw OutOfRangeException("Index out of range");
			if (bufferpos+count > buffer->size()) throw OutOfRangeException("Index out of range");
			memcpy(memory->data()+memorypos,buffer->data()+bufferpos,(size_t)count*sizeof(T));
		}
		
		virtual DataTypes ElementTypeID()
		{
			return RRPrimUtil<T>::GetTypeID();

		}

	};


	class ROBOTRACONTEUR_CORE_API MultiDimArrayMemoryBase
	{
	public:
		virtual std::vector<uint64_t> Dimensions()=0;
		virtual uint64_t DimCount()=0;		
		virtual DataTypes ElementTypeID()=0;
		virtual ~MultiDimArrayMemoryBase() {}
	};


	template <typename T>
	class MultiDimArrayMemory : public virtual MultiDimArrayMemoryBase
	{
	private:
		RR_INTRUSIVE_PTR<RRMultiDimArray<T> > multimemory;
		boost::mutex memory_lock;
	public:

		MultiDimArrayMemory() {};

		MultiDimArrayMemory(RR_INTRUSIVE_PTR<RRMultiDimArray<T> > multimemory)
		{
			this->multimemory =multimemory;
		}

		virtual ~MultiDimArrayMemory() {}

		virtual void Attach(RR_INTRUSIVE_PTR<RRMultiDimArray<T> > multimemory)
		{
			boost::mutex::scoped_lock lock(memory_lock);
			this->multimemory =multimemory;
		}

		virtual std::vector<uint64_t> Dimensions()
		{
			boost::mutex::scoped_lock lock(memory_lock);
			RR_INTRUSIVE_PTR<RRArray<uint32_t> > dims=multimemory->Dims;
			std::vector<uint64_t> s(dims->size());
			for (size_t i=0; i<s.size(); i++)
			{
				s[i]=(*dims)[i];
			}

			return s;

		}

		virtual uint64_t DimCount()
		{
			boost::mutex::scoped_lock lock(memory_lock);
			return multimemory->Dims->size();
		}
				
		virtual void Read(const std::vector<uint64_t>& memorypos, RR_INTRUSIVE_PTR<RRMultiDimArray<T> > buffer, const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count)
		{
			boost::mutex::scoped_lock lock(memory_lock);
			multimemory->RetrieveSubArray(detail::ConvertVectorType<uint32_t>(memorypos),buffer,detail::ConvertVectorType<uint32_t>(bufferpos),detail::ConvertVectorType<uint32_t>(count));
		}

		virtual void Write(const std::vector<uint64_t>& memorypos, RR_INTRUSIVE_PTR<RRMultiDimArray<T> > buffer, const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count)
		{
			boost::mutex::scoped_lock lock(memory_lock);
			multimemory->AssignSubArray(detail::ConvertVectorType<uint32_t>(memorypos),buffer,detail::ConvertVectorType<uint32_t>(bufferpos),detail::ConvertVectorType<uint32_t>(count));
		}

		virtual DataTypes ElementTypeID()
		{
			return RRPrimUtil<T>::GetTypeID();

		}
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

	public:
		RR_SHARED_PTR<RobotRaconteurNode> GetNode();		
		std::string GetMemberName() const;		
		ArrayMemoryServiceSkelBase(const std::string& membername, RR_SHARED_PTR<ServiceSkel> skel, DataTypes element_type, size_t element_size, MemberDefinition_Direction direction);		
		virtual ~ArrayMemoryServiceSkelBase();
		virtual RR_INTRUSIVE_PTR<MessageEntry> CallMemoryFunction(RR_INTRUSIVE_PTR<MessageEntry> m, RR_SHARED_PTR<Endpoint> e, RR_SHARED_PTR<ArrayMemoryBase> mem);
		
	protected:
		virtual RR_INTRUSIVE_PTR<MessageElementData> DoRead(uint64_t memorypos, uint64_t bufferpos, uint64_t count, RR_SHARED_PTR<ArrayMemoryBase> mem) = 0;
		virtual void DoWrite(uint64_t memorypos, RR_INTRUSIVE_PTR<MessageElementData> buffer, uint64_t bufferpos, uint64_t count, RR_SHARED_PTR<ArrayMemoryBase> mem) = 0;
	};

	template<typename T>
	class ArrayMemoryServiceSkel : public ArrayMemoryServiceSkelBase
	{
	public:
		ArrayMemoryServiceSkel(const std::string& membername, RR_SHARED_PTR<ServiceSkel> skel, MemberDefinition_Direction direction)
			: ArrayMemoryServiceSkelBase(membername, skel, RRPrimUtil<T>::GetTypeID(), sizeof(T), direction)
		{

		}

		virtual RR_INTRUSIVE_PTR<MessageElementData> DoRead(uint64_t memorypos, uint64_t bufferpos, uint64_t count, RR_SHARED_PTR<ArrayMemoryBase> mem)
		{
			RR_SHARED_PTR<ArrayMemory<T> > mem1 = rr_cast<ArrayMemory<T> >(mem);
			RR_INTRUSIVE_PTR<RRArray<T> > buf1 = AllocateRRArray<T>((size_t)count);
			mem1->Read(memorypos, buf1, 0, (size_t)count);
			return buf1;
		}

		virtual void DoWrite(uint64_t memorypos, RR_INTRUSIVE_PTR<MessageElementData> buffer, uint64_t bufferpos, uint64_t count, RR_SHARED_PTR<ArrayMemoryBase> mem)
		{
			RR_SHARED_PTR<ArrayMemory<T> > mem1 = rr_cast<ArrayMemory<T> >(mem);
			RR_INTRUSIVE_PTR<RRArray<T> > buf1 = rr_cast<RRArray<T> >(buffer);
			mem1->Write(memorypos, buf1, 0, (size_t)count);
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

	public:

		RR_SHARED_PTR<RobotRaconteurNode> GetNode();
		std::string GetMemberName() const;		
		MultiDimArrayMemoryServiceSkelBase(const std::string& membername, RR_SHARED_PTR<ServiceSkel> skel, DataTypes element_type, size_t element_size, MemberDefinition_Direction direction);
		virtual ~MultiDimArrayMemoryServiceSkelBase();
		virtual RR_INTRUSIVE_PTR<MessageEntry> CallMemoryFunction(RR_INTRUSIVE_PTR<MessageEntry> m, RR_SHARED_PTR<Endpoint> e, RR_SHARED_PTR<MultiDimArrayMemoryBase > mem);
		virtual RR_INTRUSIVE_PTR<MessageElementData>  DoRead(const std::vector<uint64_t>& memorypos, const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count, uint32_t elem_count, RR_SHARED_PTR<MultiDimArrayMemoryBase> mem) = 0;
		virtual void DoWrite(const std::vector<uint64_t>& memorypos, RR_INTRUSIVE_PTR<MessageElementData> buffer, const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count, uint32_t elem_count, RR_SHARED_PTR<MultiDimArrayMemoryBase> mem) = 0;
	};

	template<typename T>
	class MultiDimArrayMemoryServiceSkel : public MultiDimArrayMemoryServiceSkelBase
	{
	public:
		MultiDimArrayMemoryServiceSkel(const std::string& membername, RR_SHARED_PTR<ServiceSkel> skel, MemberDefinition_Direction direction)
			: MultiDimArrayMemoryServiceSkelBase(membername, skel, RRPrimUtil<T>::GetTypeID(), sizeof(T), direction)
		{

		}

		virtual RR_INTRUSIVE_PTR<MessageElementData>  DoRead(const std::vector<uint64_t>& memorypos, const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count, uint32_t elemcount, RR_SHARED_PTR<MultiDimArrayMemoryBase> mem)
		{
			RR_SHARED_PTR<MultiDimArrayMemory<T> > mem1 = rr_cast<MultiDimArrayMemory<T> >(mem);

			RR_INTRUSIVE_PTR<RRArray<T> > real = AllocateRRArray<T>((size_t)elemcount);
			RR_INTRUSIVE_PTR<RRMultiDimArray<T> > data;
			
			data = AllocateRRMultiDimArray<T>(VectorToRRArray<uint32_t>(count), real);
			
			mem1->Read(memorypos, data, bufferpos, count);
			return GetNode()->PackMultiDimArray(data);
		}
		virtual void DoWrite(const std::vector<uint64_t>& memorypos, RR_INTRUSIVE_PTR<MessageElementData> buffer, const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count, uint32_t elemcount, RR_SHARED_PTR<MultiDimArrayMemoryBase> mem)
		{
			RR_SHARED_PTR<MultiDimArrayMemory<T> > mem1 = rr_cast<MultiDimArrayMemory<T> >(mem);

			RR_INTRUSIVE_PTR<RRMultiDimArray<T> > data = GetNode()->template UnpackMultiDimArray<T>(rr_cast<MessageElementMultiDimArray>(buffer));
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

	public:
		const std::string GetMemberName() const;
		RR_SHARED_PTR<RobotRaconteurNode> GetNode();
		ArrayMemoryClientBase(const std::string& membername, RR_SHARED_PTR<ServiceStub> stub, DataTypes element_type, size_t element_size, MemberDefinition_Direction direction);
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
		virtual void WriteBase(uint64_t memorypos, void* buffer, uint64_t bufferpos, uint64_t count);
	public:
		void Shutdown();
		
	protected:
		virtual void UnpackReadResult(RR_INTRUSIVE_PTR<MessageElementData> res, void* buffer, uint64_t offset, uint64_t count) = 0;
		virtual RR_INTRUSIVE_PTR<MessageElementData> PackWriteRequest(void* buffer, uint64_t offset, uint64_t count) = 0;
		virtual size_t GetBufferLength(void* buffer) = 0;
	};


	template<typename T>
	class ArrayMemoryClient : public virtual ArrayMemoryClientBase, public virtual ArrayMemory<T>
	{
	public:

		ArrayMemoryClient(const std::string& membername, RR_SHARED_PTR<ServiceStub> stub, MemberDefinition_Direction direction)
			: ArrayMemoryClientBase(membername, stub, RRPrimUtil<T>::GetTypeID(), sizeof(T), direction)
		{
		}

		virtual void Attach(RR_INTRUSIVE_PTR<RRArray<T> > memory)
		{
			throw InvalidOperationException("Invalid for client");
		}

		virtual void Read(uint64_t memorypos, RR_INTRUSIVE_PTR<RRArray<T> > buffer, uint64_t bufferpos, uint64_t count)
		{
			if (!buffer) throw NullValueException("Buffer must not be null");
			ReadBase(memorypos, &buffer, bufferpos, count);
		}

		virtual void Write(uint64_t memorypos, RR_INTRUSIVE_PTR<RRArray<T> > buffer, uint64_t bufferpos, uint64_t count)
		{
			if (!buffer) throw NullValueException("Buffer must not be null");
			WriteBase(memorypos, &buffer, bufferpos, count);
		}
				
		virtual uint64_t Length()
		{
			return ArrayMemoryClientBase::Length();
		}

	protected:
		virtual void UnpackReadResult(RR_INTRUSIVE_PTR<MessageElementData> res, void* buffer, uint64_t bufferpos, uint64_t count)
		{
			RR_INTRUSIVE_PTR<RRArray<T> > data = rr_cast<RRArray<T> >(res);
			RR_INTRUSIVE_PTR<RRArray<T> >& buffer1 = *static_cast<RR_INTRUSIVE_PTR<RRArray<T> >*>(buffer);
			memcpy(buffer1->data() + bufferpos, data->data(), (size_t)count * sizeof(T));
		}

		virtual RR_INTRUSIVE_PTR<MessageElementData> PackWriteRequest(void* buffer, uint64_t bufferpos, uint64_t count)
		{
			RR_INTRUSIVE_PTR<RRArray<T> >& buffer1 = *static_cast<RR_INTRUSIVE_PTR<RRArray<T> >*>(buffer);
			if (bufferpos == 0 && buffer1->size() == static_cast<size_t>(count))
			{
				return buffer1;
			}
			else if ((buffer1->size() - static_cast<size_t>(bufferpos)) >= static_cast<size_t>(count))
			{
				RR_INTRUSIVE_PTR<RRArray<T> > data = AllocateRRArray<T>((size_t)count);				
				memcpy(data->data(), buffer1->data() + bufferpos, (size_t)count * sizeof(T));
				return data;
			}
			else
				throw OutOfRangeException("");
		}

		virtual size_t GetBufferLength(void* buffer)
		{
			RR_INTRUSIVE_PTR <RRArray<T> >& buffer1 = *static_cast<RR_INTRUSIVE_PTR<RRArray<T> >*>(buffer);
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

	public:
		RR_SHARED_PTR<RobotRaconteurNode> GetNode();
		const std::string GetMemberName() const;
		
		MultiDimArrayMemoryClientBase(const std::string &membername, RR_SHARED_PTR<ServiceStub> stub, DataTypes element_type, size_t element_size, MemberDefinition_Direction direction);
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
		virtual void ReadBase(const std::vector<uint64_t>& memorypos, void* buffer, const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count);
		virtual void WriteBase(const std::vector<uint64_t>& memorypos, void* buffer, const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count);

	public:
		void Shutdown();
		
	protected:
		virtual void UnpackReadResult(RR_INTRUSIVE_PTR<MessageElementData> res, void* buffer, const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count, uint64_t elemcount) = 0;
		virtual RR_INTRUSIVE_PTR<MessageElementData> PackWriteRequest(void* buffer, const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count, uint64_t elemcount) = 0;		
	};

	template <typename T>
	class MultiDimArrayMemoryClient : public virtual MultiDimArrayMemory<T>, public virtual MultiDimArrayMemoryClientBase
	{
	public:

		MultiDimArrayMemoryClient(const std::string& membername, RR_SHARED_PTR<ServiceStub> stub, MemberDefinition_Direction direction)
			: MultiDimArrayMemoryClientBase(membername, stub, RRPrimUtil<T>::GetTypeID(), sizeof(T), direction)
		{
		}

		virtual void Attach(RR_INTRUSIVE_PTR<RRMultiDimArray<T> > memory)
		{
			throw InvalidOperationException("Not valid for client");
		}

		virtual std::vector<uint64_t> Dimensions()
		{
			return MultiDimArrayMemoryClientBase::Dimensions();
		}

		virtual uint64_t DimCount()
		{
			return MultiDimArrayMemoryClientBase::DimCount();
		}
				
		virtual void Read(const std::vector<uint64_t>& memorypos, RR_INTRUSIVE_PTR<RRMultiDimArray<T> > buffer, const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count)
		{
			if (!buffer) throw NullValueException("Buffer must not be null");
			ReadBase(memorypos, &buffer, bufferpos, count);
		}

		virtual void Write(const std::vector<uint64_t>& memorypos, RR_INTRUSIVE_PTR<RRMultiDimArray<T> > buffer, const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count)
		{
			if (!buffer) throw NullValueException("Buffer must not be null");
			WriteBase(memorypos, &buffer, bufferpos, count);
		}

	protected:
		virtual void UnpackReadResult(RR_INTRUSIVE_PTR<MessageElementData> res, void* buffer, const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count, uint64_t elemcount)
		{
			RR_INTRUSIVE_PTR<RRMultiDimArray<T> >& buffer1 = *static_cast<RR_INTRUSIVE_PTR<RRMultiDimArray<T> >* > (buffer);
			RR_INTRUSIVE_PTR<RRMultiDimArray<T> > data = GetNode()->template UnpackMultiDimArray<T>(rr_cast<MessageElementMultiDimArray>(res));

			RR_SHARED_PTR<MultiDimArrayMemory<T> > data2 = RR_MAKE_SHARED<MultiDimArrayMemory<T> >(data);
			data2->Read(std::vector<uint64_t>(count.size()), buffer1, bufferpos, count);
		}
		virtual RR_INTRUSIVE_PTR<MessageElementData> PackWriteRequest(void* buffer, const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count, uint64_t elemcount)
		{
			RR_INTRUSIVE_PTR<RRMultiDimArray<T> >& buffer1 = *static_cast<RR_INTRUSIVE_PTR<RRMultiDimArray<T> >* > (buffer);

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
				
				data = AllocateRRMultiDimArray<T>(VectorToRRArray<uint32_t>(count), AllocateRRArray<T>((size_t)elemcount));
				
				buffer1->RetrieveSubArray(detail::ConvertVectorType<uint32_t>(bufferpos), data, std::vector<uint32_t>(count.size()), detail::ConvertVectorType<uint32_t>(count));
				return GetNode()->PackMultiDimArray(data);
			}
		}
	};

	// pod

	template <typename T>
	class PodArrayMemory : public virtual ArrayMemoryBase
	{
	private:
		RR_INTRUSIVE_PTR<RRPodArray<T> > memory;
		boost::mutex memory_lock;

	public:

		PodArrayMemory() {};

		PodArrayMemory(RR_INTRUSIVE_PTR<RRPodArray<T> > memory)
		{
			this->memory = memory;
		}

		virtual void Attach(RR_INTRUSIVE_PTR<RRPodArray<T> > memory)
		{
			boost::mutex::scoped_lock lock(memory_lock);
			this->memory = memory;
		}

		virtual uint64_t Length()
		{
			boost::mutex::scoped_lock lock(memory_lock);
			return memory->size();
		}

		virtual void Read(uint64_t memorypos, RR_INTRUSIVE_PTR<RRPodArray<T> > buffer, uint64_t bufferpos, uint64_t count)
		{
			boost::mutex::scoped_lock lock(memory_lock);
			if (memorypos + count > memory->size()) throw OutOfRangeException("Index out of range");
			if (bufferpos + count > buffer->size()) throw OutOfRangeException("Index out of range");

			for (size_t i = 0; i < count; i++)
			{
				buffer->at(i + bufferpos) = memory->at(i + memorypos);
			}
		}

		virtual void Write(uint64_t memorypos, RR_INTRUSIVE_PTR<RRPodArray<T> > buffer, uint64_t bufferpos, uint64_t count)
		{
			boost::mutex::scoped_lock lock(memory_lock);
			if (memorypos + count > memory->size()) throw OutOfRangeException("Index out of range");
			if (bufferpos + count > buffer->size()) throw OutOfRangeException("Index out of range");			
			for (size_t i = 0; i < count; i++)
			{
				memory->at(i + memorypos) = buffer->at(i + bufferpos);
			}
		}

		virtual DataTypes ElementTypeID()
		{
			return DataTypes_pod_t;
		}
	};

	template<typename T>
	class PodArrayMemoryServiceSkel : public ArrayMemoryServiceSkelBase
	{
	public:
		PodArrayMemoryServiceSkel(const std::string& membername, RR_SHARED_PTR<ServiceSkel> skel, size_t element_size, MemberDefinition_Direction direction)
			: ArrayMemoryServiceSkelBase(membername, skel, DataTypes_pod_t, element_size, direction)
		{

		}

		virtual RR_INTRUSIVE_PTR<MessageElementData> DoRead(uint64_t memorypos, uint64_t bufferpos, uint64_t count, RR_SHARED_PTR<ArrayMemoryBase> mem)
		{
			RR_SHARED_PTR<PodArrayMemory<T> > mem1 = rr_cast<PodArrayMemory<T> >(mem);
			RR_INTRUSIVE_PTR<RRPodArray<T> > buf1 = AllocateEmptyRRPodArray<T>(count);			
			mem1->Read(memorypos, buf1, 0, (size_t)count);
			return PodStub_PackPodArray(buf1);
		}

		virtual void DoWrite(uint64_t memorypos, RR_INTRUSIVE_PTR<MessageElementData> buffer, uint64_t bufferpos, uint64_t count, RR_SHARED_PTR<ArrayMemoryBase> mem)
		{
			RR_SHARED_PTR<PodArrayMemory<T> > mem1 = rr_cast<PodArrayMemory<T> >(mem);
			RR_INTRUSIVE_PTR<RRPodArray<T> > buf1 = PodStub_UnpackPodArray<T>(rr_cast<MessageElementPodArray>(buffer));
			mem1->Write(memorypos, buf1, 0, (size_t)count);
		}
	};

	template<typename T>
	class PodArrayMemoryClient : public virtual ArrayMemoryClientBase, public virtual PodArrayMemory<T>
	{
	public:

		PodArrayMemoryClient(const std::string& membername, RR_SHARED_PTR<ServiceStub> stub, size_t element_size, MemberDefinition_Direction direction)
			: ArrayMemoryClientBase(membername, stub, RRPrimUtil<T>::GetTypeID(), element_size, direction)
		{
		}

		virtual void Attach(RR_INTRUSIVE_PTR<RRPodArray<T> > memory)
		{
			throw InvalidOperationException("Invalid for client");
		}

		virtual void Read(uint64_t memorypos, RR_INTRUSIVE_PTR<RRPodArray<T> > buffer, uint64_t bufferpos, uint64_t count)
		{
			if (!buffer) throw NullValueException("Buffer must not be null");
			ReadBase(memorypos, &buffer, bufferpos, count);
		}

		virtual void Write(uint64_t memorypos, RR_INTRUSIVE_PTR<RRPodArray<T> > buffer, uint64_t bufferpos, uint64_t count)
		{
			if (!buffer) throw NullValueException("Buffer must not be null");
			WriteBase(memorypos, &buffer, bufferpos, count);
		}

		virtual uint64_t Length()
		{
			return ArrayMemoryClientBase::Length();
		}

	protected:
		virtual void UnpackReadResult(RR_INTRUSIVE_PTR<MessageElementData> res,void* buffer, uint64_t bufferpos, uint64_t count)
		{
			RR_INTRUSIVE_PTR<RRPodArray<T> >& buffer1 = *static_cast<RR_INTRUSIVE_PTR<RRPodArray<T> >* >(buffer);
			RR_INTRUSIVE_PTR<RRPodArray<T> > res1 = PodStub_UnpackPodArray<T>(rr_cast<RobotRaconteur::MessageElementPodArray>(res));

			for (size_t i = 0; i < count; i++)
			{
				buffer1->at(i + bufferpos) = res1->at(i);
			}			
		}

		virtual RR_INTRUSIVE_PTR<MessageElementData> PackWriteRequest(void* buffer, uint64_t bufferpos, uint64_t count)
		{
			RR_INTRUSIVE_PTR<RRPodArray<T> >& buffer1 = *static_cast<RR_INTRUSIVE_PTR<RRPodArray<T> >* >(buffer);
			RR_INTRUSIVE_PTR<RRPodArray<T> > o = AllocateEmptyRRPodArray<T>(count);
			
			for (size_t i = 0; i < count; i++)
			{
				o->at(i) = buffer1->at(i + bufferpos);
			}
			return PodStub_PackPodArray(o);
		}

		virtual size_t GetBufferLength(void* buffer)
		{
			RR_INTRUSIVE_PTR<RRPodArray<T> >& buffer1 = *static_cast<RR_INTRUSIVE_PTR<RRPodArray<T> >*>(buffer);
			return buffer1->size();
		}
	};

	template <typename T>
	class PodMultiDimArrayMemory : public virtual MultiDimArrayMemoryBase
	{
	private:
		RR_INTRUSIVE_PTR<RRPodMultiDimArray<T> > multimemory;
		boost::mutex memory_lock;
	public:

		PodMultiDimArrayMemory() {};

		PodMultiDimArrayMemory(RR_INTRUSIVE_PTR<RRPodMultiDimArray<T> > multimemory)
		{
			this->multimemory = multimemory;
		}

		virtual ~PodMultiDimArrayMemory() {}

		virtual void Attach(RR_INTRUSIVE_PTR<RRPodMultiDimArray<T> > multimemory)
		{
			boost::mutex::scoped_lock lock(memory_lock);
			this->multimemory = multimemory;
		}

		virtual std::vector<uint64_t> Dimensions()
		{
			boost::mutex::scoped_lock lock(memory_lock);
			RR_INTRUSIVE_PTR<RRArray<uint32_t> > dims = multimemory->Dims;
			std::vector<uint64_t> s(dims->size());
			for (size_t i = 0; i<s.size(); i++)
			{
				s[i] = (*dims)[i];
			}

			return s;
		}

		virtual uint64_t DimCount()
		{
			boost::mutex::scoped_lock lock(memory_lock);
			return multimemory->Dims->size();
		}
				
		virtual void Read(const std::vector<uint64_t>& memorypos, RR_INTRUSIVE_PTR<RRPodMultiDimArray<T> > buffer, const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count)
		{
			boost::mutex::scoped_lock lock(memory_lock);
			multimemory->RetrieveSubArray(detail::ConvertVectorType<uint32_t>(memorypos), buffer, detail::ConvertVectorType<uint32_t>(bufferpos), detail::ConvertVectorType<uint32_t>(count));
		}

		virtual void Write(const std::vector<uint64_t>& memorypos, RR_INTRUSIVE_PTR<RRPodMultiDimArray<T> > buffer, const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count)
		{
			boost::mutex::scoped_lock lock(memory_lock);
			multimemory->AssignSubArray(detail::ConvertVectorType<uint32_t>(memorypos), buffer, detail::ConvertVectorType<uint32_t>(bufferpos), detail::ConvertVectorType<uint32_t>(count));
		}

		virtual DataTypes ElementTypeID()
		{
			return DataTypes_pod_t;
		}
	};

	template <typename T>
	class PodMultiDimArrayMemoryClient : public virtual PodMultiDimArrayMemory<T>, public virtual MultiDimArrayMemoryClientBase
	{
	public:

		PodMultiDimArrayMemoryClient(const std::string& membername, RR_SHARED_PTR<ServiceStub> stub, size_t element_size, MemberDefinition_Direction direction)
			: MultiDimArrayMemoryClientBase(membername, stub, DataTypes_pod_t, element_size, direction)
		{
		}

		virtual void Attach(RR_INTRUSIVE_PTR<RRPodMultiDimArray<T> > memory)
		{
			throw InvalidOperationException("Not valid for client");
		}

		virtual std::vector<uint64_t> Dimensions()
		{
			return MultiDimArrayMemoryClientBase::Dimensions();
		}

		virtual uint64_t DimCount()
		{
			return MultiDimArrayMemoryClientBase::DimCount();
		}
				
		virtual void Read(const std::vector<uint64_t>& memorypos, RR_INTRUSIVE_PTR<RRPodMultiDimArray<T> > buffer, const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count)
		{
			if (!buffer) throw NullValueException("Buffer must not be null");
			ReadBase(memorypos, &buffer, bufferpos, count);
		}

		virtual void Write(const std::vector<uint64_t>& memorypos, RR_INTRUSIVE_PTR<RRPodMultiDimArray<T> > buffer, const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count)
		{
			if (!buffer) throw NullValueException("Buffer must not be null");
			WriteBase(memorypos, &buffer, bufferpos, count);
		}

	protected:
		virtual void UnpackReadResult(RR_INTRUSIVE_PTR<MessageElementData> res, void* buffer, const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count, uint64_t elemcount)
		{
			RR_INTRUSIVE_PTR<RRPodMultiDimArray<T> >& buffer1 = *static_cast<RR_INTRUSIVE_PTR<RRPodMultiDimArray<T> >* > (buffer);
			RR_INTRUSIVE_PTR<RRPodMultiDimArray<T> > data = rr_cast<RRPodMultiDimArray<T> >(GetNode()->UnpackPodMultiDimArray(rr_cast<MessageElementPodMultiDimArray>(res)));

			RR_SHARED_PTR<PodMultiDimArrayMemory<T> > data2 = RR_MAKE_SHARED<PodMultiDimArrayMemory<T> >(data);
			data2->Read(std::vector<uint64_t>(count.size()), buffer1, bufferpos, count);
		}
		virtual RR_INTRUSIVE_PTR<MessageElementData> PackWriteRequest(void* buffer, const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count, uint64_t elemcount)
		{
			RR_INTRUSIVE_PTR<RRPodMultiDimArray<T> >& buffer1 = *static_cast<RR_INTRUSIVE_PTR<RRPodMultiDimArray<T> >* >(buffer);

			RR_INTRUSIVE_PTR<RRPodMultiDimArray<T> > data = AllocateEmptyRRPodMultiDimArray<T>();
			data->Dims = VectorToRRArray<uint32_t>(count);
			data->PodArray = AllocateEmptyRRPodArray<T>(elemcount);
									
			buffer1->RetrieveSubArray(detail::ConvertVectorType<uint32_t>(bufferpos), data, std::vector<uint32_t>(count.size()), detail::ConvertVectorType<uint32_t>(count));
			return GetNode()->PackPodMultiDimArray(data);
			
		}
	};

	template<typename T>
	class PodMultiDimArrayMemoryServiceSkel : public MultiDimArrayMemoryServiceSkelBase
	{
	public:
		PodMultiDimArrayMemoryServiceSkel(const std::string& membername, RR_SHARED_PTR<ServiceSkel> skel, size_t element_size, MemberDefinition_Direction direction)
			: MultiDimArrayMemoryServiceSkelBase(membername, skel, DataTypes_structure_t, element_size, direction)
		{

		}

		virtual RR_INTRUSIVE_PTR<MessageElementData>  DoRead(const std::vector<uint64_t>& memorypos, const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count, uint32_t elemcount, RR_SHARED_PTR<MultiDimArrayMemoryBase> mem)
		{
			RR_SHARED_PTR<PodMultiDimArrayMemory<T> > mem1 = rr_cast<PodMultiDimArrayMemory<T> >(mem);
						
			RR_INTRUSIVE_PTR<RRPodMultiDimArray<T> > data = AllocateEmptyRRPodMultiDimArray<T>();
			data->Dims = VectorToRRArray<uint32_t>(count);
			data->PodArray = AllocateEmptyRRPodArray<T>((size_t)elemcount);
						
			mem1->Read(memorypos, data, bufferpos, count);
			return GetNode()->PackPodMultiDimArray(data);
		}
		virtual void DoWrite(const std::vector<uint64_t>& memorypos, RR_INTRUSIVE_PTR<MessageElementData> buffer, const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count, uint32_t elemcount, RR_SHARED_PTR<MultiDimArrayMemoryBase> mem)
		{
			RR_SHARED_PTR<PodMultiDimArrayMemory<T> > mem1 = rr_cast<PodMultiDimArrayMemory<T> >(mem);

			RR_INTRUSIVE_PTR<RRPodMultiDimArray<T> > data = rr_cast<RRPodMultiDimArray<T> >(GetNode()->UnpackPodMultiDimArray(rr_cast<MessageElementPodMultiDimArray>(buffer)));
			mem1->Write(memorypos, data, bufferpos, count);
		}
	};



	// namedarray

	template <typename T>
	class NamedArrayMemory : public virtual ArrayMemoryBase
	{
	private:
		RR_INTRUSIVE_PTR<RRNamedArray<T> > memory;
		boost::mutex memory_lock;

	public:

		NamedArrayMemory() {};

		NamedArrayMemory(RR_INTRUSIVE_PTR<RRNamedArray<T> > memory)
		{
			this->memory = memory;
		}

		virtual void Attach(RR_INTRUSIVE_PTR<RRNamedArray<T> > memory)
		{
			boost::mutex::scoped_lock lock(memory_lock);
			this->memory = memory;
		}

		virtual uint64_t Length()
		{
			boost::mutex::scoped_lock lock(memory_lock);
			return memory->size();
		}

		virtual void Read(uint64_t memorypos, RR_INTRUSIVE_PTR<RRNamedArray<T> > buffer, uint64_t bufferpos, uint64_t count)
		{
			boost::mutex::scoped_lock lock(memory_lock);
			if (memorypos + count > memory->size()) throw OutOfRangeException("Index out of range");
			if (bufferpos + count > buffer->size()) throw OutOfRangeException("Index out of range");

			for (size_t i = 0; i < count; i++)
			{
				(*buffer)[(i + bufferpos)] = (*memory)[(i + memorypos)];
			}
		}

		virtual void Write(uint64_t memorypos, RR_INTRUSIVE_PTR<RRNamedArray<T> > buffer, uint64_t bufferpos, uint64_t count)
		{
			boost::mutex::scoped_lock lock(memory_lock);
			if (memorypos + count > memory->size()) throw OutOfRangeException("Index out of range");
			if (bufferpos + count > buffer->size()) throw OutOfRangeException("Index out of range");
			for (size_t i = 0; i < count; i++)
			{
				(*memory)[(i + memorypos)] = (*buffer)[(i + bufferpos)];
			}
		}

		virtual DataTypes ElementTypeID()
		{
			return DataTypes_pod_t;
		}
	};

	template<typename T>
	class NamedArrayMemoryServiceSkel : public ArrayMemoryServiceSkelBase
	{
	public:
		NamedArrayMemoryServiceSkel(const std::string& membername, RR_SHARED_PTR<ServiceSkel> skel, size_t element_size, MemberDefinition_Direction direction)
			: ArrayMemoryServiceSkelBase(membername, skel, DataTypes_pod_t, element_size, direction)
		{

		}

		virtual RR_INTRUSIVE_PTR<MessageElementData> DoRead(uint64_t memorypos, uint64_t bufferpos, uint64_t count, RR_SHARED_PTR<ArrayMemoryBase> mem)
		{
			RR_SHARED_PTR<NamedArrayMemory<T> > mem1 = rr_cast<NamedArrayMemory<T> >(mem);
			RR_INTRUSIVE_PTR<RRNamedArray<T> > buf1 = AllocateEmptyRRNamedArray<T>(count);
			mem1->Read(memorypos, buf1, 0, (size_t)count);
			return NamedArrayStub_PackNamedArray(buf1);
		}

		virtual void DoWrite(uint64_t memorypos, RR_INTRUSIVE_PTR<MessageElementData> buffer, uint64_t bufferpos, uint64_t count, RR_SHARED_PTR<ArrayMemoryBase> mem)
		{
			RR_SHARED_PTR<NamedArrayMemory<T> > mem1 = rr_cast<NamedArrayMemory<T> >(mem);
			RR_INTRUSIVE_PTR<RRNamedArray<T> > buf1 = NamedArrayStub_UnpackNamedArray<T>(rr_cast<MessageElementNamedArray>(buffer));
			mem1->Write(memorypos, buf1, 0, (size_t)count);
		}
	};

	template<typename T>
	class NamedArrayMemoryClient : public virtual ArrayMemoryClientBase, public virtual NamedArrayMemory<T>
	{
	public:

		NamedArrayMemoryClient(const std::string& membername, RR_SHARED_PTR<ServiceStub> stub, size_t element_size, MemberDefinition_Direction direction)
			: ArrayMemoryClientBase(membername, stub, RRPrimUtil<T>::GetTypeID(), element_size, direction)
		{
		}

		virtual void Attach(RR_INTRUSIVE_PTR<RRNamedArray<T> > memory)
		{
			throw InvalidOperationException("Invalid for client");
		}

		virtual void Read(uint64_t memorypos, RR_INTRUSIVE_PTR<RRNamedArray<T> > buffer, uint64_t bufferpos, uint64_t count)
		{
			if (!buffer) throw NullValueException("Buffer must not be null");
			ReadBase(memorypos, &buffer, bufferpos, count);
		}

		virtual void Write(uint64_t memorypos, RR_INTRUSIVE_PTR<RRNamedArray<T> > buffer, uint64_t bufferpos, uint64_t count)
		{
			if (!buffer) throw NullValueException("Buffer must not be null");
			WriteBase(memorypos, &buffer, bufferpos, count);
		}

		virtual uint64_t Length()
		{
			return ArrayMemoryClientBase::Length();
		}

	protected:
		virtual void UnpackReadResult(RR_INTRUSIVE_PTR<MessageElementData> res, void* buffer, uint64_t bufferpos, uint64_t count)
		{
			RR_INTRUSIVE_PTR<RRNamedArray<T> >& buffer1 = *static_cast<RR_INTRUSIVE_PTR<RRNamedArray<T> >*>(buffer);
			RR_INTRUSIVE_PTR<RRNamedArray<T> > res1 = NamedArrayStub_UnpackNamedArray<T>(rr_cast<RobotRaconteur::MessageElementNamedArray>(res));

			for (size_t i = 0; i < count; i++)
			{
				(*buffer1)[(i + bufferpos)] = (*res1)[i];
			}
		}

		virtual RR_INTRUSIVE_PTR<MessageElementData> PackWriteRequest(void* buffer, uint64_t bufferpos, uint64_t count)
		{
			RR_INTRUSIVE_PTR<RRNamedArray<T> >& buffer1 = *static_cast<RR_INTRUSIVE_PTR<RRNamedArray<T> >*>(buffer);
			RR_INTRUSIVE_PTR<RRNamedArray<T> > o = AllocateEmptyRRNamedArray<T>(count);

			for (size_t i = 0; i < count; i++)
			{
				(*o)[i] = (*buffer1)[(i + bufferpos)];
			}
			return NamedArrayStub_PackNamedArray(o);
		}

		virtual size_t GetBufferLength(void* buffer)
		{
			RR_INTRUSIVE_PTR<RRNamedArray<T> >& buffer1 = *static_cast<RR_INTRUSIVE_PTR<RRNamedArray<T> >*>(buffer);
			return buffer1->size();
		}
	};

	template <typename T>
	class NamedMultiDimArrayMemory : public virtual MultiDimArrayMemoryBase
	{
	private:
		RR_INTRUSIVE_PTR<RRNamedMultiDimArray<T> > multimemory;
		boost::mutex memory_lock;
	public:

		NamedMultiDimArrayMemory() {};

		NamedMultiDimArrayMemory(RR_INTRUSIVE_PTR<RRNamedMultiDimArray<T> > multimemory)
		{
			this->multimemory = multimemory;
		}

		virtual ~NamedMultiDimArrayMemory() {}

		virtual void Attach(RR_INTRUSIVE_PTR<RRNamedMultiDimArray<T> > multimemory)
		{
			boost::mutex::scoped_lock lock(memory_lock);
			this->multimemory = multimemory;
		}

		virtual std::vector<uint64_t> Dimensions()
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

		virtual uint64_t DimCount()
		{
			boost::mutex::scoped_lock lock(memory_lock);
			return multimemory->Dims->size();
		}
		
		virtual void Read(const std::vector<uint64_t>& memorypos, RR_INTRUSIVE_PTR<RRNamedMultiDimArray<T> > buffer, const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count)
		{
			boost::mutex::scoped_lock lock(memory_lock);
			multimemory->RetrieveSubArray(detail::ConvertVectorType<uint32_t>(memorypos), buffer, detail::ConvertVectorType<uint32_t>(bufferpos), detail::ConvertVectorType<uint32_t>(count));
		}

		virtual void Write(const std::vector<uint64_t>& memorypos, RR_INTRUSIVE_PTR<RRNamedMultiDimArray<T> > buffer, const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count)
		{
			boost::mutex::scoped_lock lock(memory_lock);
			multimemory->AssignSubArray(detail::ConvertVectorType<uint32_t>(memorypos), buffer, detail::ConvertVectorType<uint32_t>(bufferpos), detail::ConvertVectorType<uint32_t>(count));
		}

		virtual DataTypes ElementTypeID()
		{
			return DataTypes_pod_t;
		}
	};

	template <typename T>
	class NamedMultiDimArrayMemoryClient : public virtual NamedMultiDimArrayMemory<T>, public virtual MultiDimArrayMemoryClientBase
	{
	public:

		NamedMultiDimArrayMemoryClient(const std::string& membername, RR_SHARED_PTR<ServiceStub> stub, size_t element_size, MemberDefinition_Direction direction)
			: MultiDimArrayMemoryClientBase(membername, stub, DataTypes_pod_t, element_size, direction)
		{
		}

		virtual void Attach(RR_INTRUSIVE_PTR<RRNamedMultiDimArray<T> > memory)
		{
			throw InvalidOperationException("Not valid for client");
		}

		virtual std::vector<uint64_t> Dimensions()
		{
			return MultiDimArrayMemoryClientBase::Dimensions();
		}

		virtual uint64_t DimCount()
		{
			return MultiDimArrayMemoryClientBase::DimCount();
		}
				
		virtual void Read(const std::vector<uint64_t>& memorypos, RR_INTRUSIVE_PTR<RRNamedMultiDimArray<T> > buffer, const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count)
		{
			if (!buffer) throw NullValueException("Buffer must not be null");
			ReadBase(memorypos, &buffer, bufferpos, count);
		}

		virtual void Write(const std::vector<uint64_t>& memorypos, RR_INTRUSIVE_PTR<RRNamedMultiDimArray<T> > buffer, const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count)
		{
			if (!buffer) throw NullValueException("Buffer must not be null");
			WriteBase(memorypos, &buffer, bufferpos, count);
		}

	protected:
		virtual void UnpackReadResult(RR_INTRUSIVE_PTR<MessageElementData> res, void* buffer, const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count, uint64_t elemcount)
		{
			RR_INTRUSIVE_PTR<RRNamedMultiDimArray<T> >& buffer1 = *static_cast<RR_INTRUSIVE_PTR<RRNamedMultiDimArray<T> >*> (buffer);
			RR_INTRUSIVE_PTR<RRNamedMultiDimArray<T> > data = rr_cast<RRNamedMultiDimArray<T> >(GetNode()->UnpackNamedMultiDimArray(rr_cast<MessageElementNamedMultiDimArray>(res)));

			RR_SHARED_PTR<NamedMultiDimArrayMemory<T> > data2 = RR_MAKE_SHARED<NamedMultiDimArrayMemory<T> >(data);
			data2->Read(std::vector<uint64_t>(count.size()), buffer1, bufferpos, count);
		}
		virtual RR_INTRUSIVE_PTR<MessageElementData> PackWriteRequest(void* buffer, const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count, uint64_t elemcount)
		{
			RR_INTRUSIVE_PTR<RRNamedMultiDimArray<T> >& buffer1 = *static_cast<RR_INTRUSIVE_PTR<RRNamedMultiDimArray<T> >*>(buffer);

			RR_INTRUSIVE_PTR<RRNamedMultiDimArray<T> > data = AllocateEmptyRRNamedMultiDimArray<T>();
			data->Dims = VectorToRRArray<uint32_t>(count);			
			data->NamedArray = AllocateEmptyRRNamedArray<T>(elemcount);

			buffer1->RetrieveSubArray(detail::ConvertVectorType<uint32_t>(bufferpos), data, std::vector<uint32_t>(count.size()), detail::ConvertVectorType<uint32_t>(count));
			return GetNode()->PackNamedMultiDimArray(data);

		}
	};

	template<typename T>
	class NamedMultiDimArrayMemoryServiceSkel : public MultiDimArrayMemoryServiceSkelBase
	{
	public:
		NamedMultiDimArrayMemoryServiceSkel(const std::string& membername, RR_SHARED_PTR<ServiceSkel> skel, size_t element_size, MemberDefinition_Direction direction)
			: MultiDimArrayMemoryServiceSkelBase(membername, skel, DataTypes_structure_t, element_size, direction)
		{

		}

		virtual RR_INTRUSIVE_PTR<MessageElementData>  DoRead(const std::vector<uint64_t>& memorypos, const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count, uint32_t elemcount, RR_SHARED_PTR<MultiDimArrayMemoryBase> mem)
		{
			RR_SHARED_PTR<NamedMultiDimArrayMemory<T> > mem1 = rr_cast<NamedMultiDimArrayMemory<T> >(mem);

			RR_INTRUSIVE_PTR<RRNamedMultiDimArray<T> > data = AllocateEmptyRRNamedMultiDimArray<T>();
			data->Dims = VectorToRRArray<uint32_t>(count);			
			data->NamedArray = AllocateEmptyRRNamedArray<T>((size_t)elemcount);

			mem1->Read(memorypos, data, bufferpos, count);
			return GetNode()->PackNamedMultiDimArray(data);
		}
		virtual void DoWrite(const std::vector<uint64_t>& memorypos, RR_INTRUSIVE_PTR<MessageElementData> buffer, const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count, uint32_t elemcount, RR_SHARED_PTR<MultiDimArrayMemoryBase> mem)
		{
			RR_SHARED_PTR<NamedMultiDimArrayMemory<T> > mem1 = rr_cast<NamedMultiDimArrayMemory<T> >(mem);

			RR_INTRUSIVE_PTR<RRNamedMultiDimArray<T> > data = rr_cast<RRNamedMultiDimArray<T> >(GetNode()->UnpackNamedMultiDimArray(rr_cast<MessageElementNamedMultiDimArray>(buffer)));
			mem1->Write(memorypos, data, bufferpos, count);
		}
	};
}
