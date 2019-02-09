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
		static RR_SHARED_PTR<RRArray<Y> > ConvertRRArrayType(RR_SHARED_PTR<RRArray<U> > in)
		{
			RR_SHARED_PTR<RRArray<Y> > out=AllocateRRArray<Y>(in->size());
			for (size_t i=0; i<in->size(); i++) (*out)[i]=(Y)(*in)[i];
			return out;
		}

		ROBOTRACONTEUR_CORE_API void CalculateMatrixBlocks(uint32_t element_size, std::vector<uint64_t> count, uint64_t max_elems, int32_t &split_dim, uint64_t &split_dim_block, uint64_t &split_elem_count, int32_t &splits_count, int32_t &split_remainder, std::vector<uint64_t>& block_count, std::vector<uint64_t>& block_count_edge);
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
		RR_SHARED_PTR<RRArray<T> > memory;
		boost::mutex memory_lock;

	public:

		ArrayMemory() {};

		ArrayMemory(RR_SHARED_PTR<RRArray<T> > memory)
		{
			this->memory=memory;
		}

		virtual void Attach(RR_SHARED_PTR<RRArray<T> > memory)
		{
			boost::mutex::scoped_lock lock(memory_lock);
			this->memory=memory;
		}

		virtual uint64_t Length()
		{
			boost::mutex::scoped_lock lock(memory_lock);
			return memory->Length();
		}

		virtual void Read(uint64_t memorypos, RR_SHARED_PTR<RRArray<T> > buffer, uint64_t bufferpos, uint64_t count)
		{
			boost::mutex::scoped_lock lock(memory_lock);
			if (memorypos+count > memory->Length()) throw OutOfRangeException("Index out of range");
			if (bufferpos+count > buffer->Length()) throw OutOfRangeException("Index out of range");

			memcpy(buffer->ptr()+bufferpos,memory->ptr()+memorypos,(size_t)count*sizeof(T));
		}

		virtual void Write(uint64_t memorypos, RR_SHARED_PTR<RRArray<T> > buffer, uint64_t bufferpos, uint64_t count)
		{
			boost::mutex::scoped_lock lock(memory_lock);
			if (memorypos+count > memory->Length()) throw OutOfRangeException("Index out of range");
			if (bufferpos+count > buffer->Length()) throw OutOfRangeException("Index out of range");
			memcpy(memory->ptr()+memorypos,buffer->ptr()+bufferpos,(size_t)count*sizeof(T));
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
		virtual bool Complex()=0;
		virtual DataTypes ElementTypeID()=0;
		virtual ~MultiDimArrayMemoryBase() {}
	};


	template <typename T>
	class MultiDimArrayMemory : public virtual MultiDimArrayMemoryBase
	{
	private:
		RR_SHARED_PTR<RRMultiDimArray<T> > multimemory;
		boost::mutex memory_lock;
	public:

		MultiDimArrayMemory() {};

		MultiDimArrayMemory(RR_SHARED_PTR<RRMultiDimArray<T> > multimemory)
		{
			this->multimemory =multimemory;
		}

		virtual ~MultiDimArrayMemory() {}

		virtual void Attach(RR_SHARED_PTR<RRMultiDimArray<T> > multimemory)
		{
			boost::mutex::scoped_lock lock(memory_lock);
			this->multimemory =multimemory;
		}

		virtual std::vector<uint64_t> Dimensions()
		{
			boost::mutex::scoped_lock lock(memory_lock);
			RR_SHARED_PTR<RRArray<int32_t> > dims=multimemory->Dims;
			std::vector<uint64_t> s(dims->Length());
			for (size_t i=0; i<s.size(); i++)
			{
				s[i]=(*dims)[i];
			}

			return s;

		}

		virtual uint64_t DimCount()
		{
			boost::mutex::scoped_lock lock(memory_lock);
			return multimemory->DimCount;
		}

		virtual bool Complex()
		{
			boost::mutex::scoped_lock lock(memory_lock);
			return multimemory->Complex;
		}

		virtual void Read(const std::vector<uint64_t>& memorypos, RR_SHARED_PTR<RRMultiDimArray<T> > buffer, const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count)
		{
			boost::mutex::scoped_lock lock(memory_lock);
			multimemory->RetrieveSubArray(detail::ConvertVectorType<int32_t>(memorypos),buffer,detail::ConvertVectorType<int32_t>(bufferpos),detail::ConvertVectorType<int32_t>(count));
		}

		virtual void Write(const std::vector<uint64_t>& memorypos, RR_SHARED_PTR<RRMultiDimArray<T> > buffer, const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count)
		{
			boost::mutex::scoped_lock lock(memory_lock);
			multimemory->AssignSubArray(detail::ConvertVectorType<int32_t>(memorypos),buffer,detail::ConvertVectorType<int32_t>(bufferpos),detail::ConvertVectorType<int32_t>(count));
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
		virtual RR_SHARED_PTR<MessageEntry> CallMemoryFunction(RR_SHARED_PTR<MessageEntry> m, RR_SHARED_PTR<Endpoint> e, RR_SHARED_PTR<ArrayMemoryBase> mem);
		
	protected:
		virtual RR_SHARED_PTR<MessageElementData> DoRead(uint64_t memorypos, uint64_t bufferpos, uint64_t count, RR_SHARED_PTR<ArrayMemoryBase> mem) = 0;
		virtual void DoWrite(uint64_t memorypos, RR_SHARED_PTR<MessageElementData> buffer, uint64_t bufferpos, uint64_t count, RR_SHARED_PTR<ArrayMemoryBase> mem) = 0;
	};

	template<typename T>
	class ArrayMemoryServiceSkel : public ArrayMemoryServiceSkelBase
	{
	public:
		ArrayMemoryServiceSkel(const std::string& membername, RR_SHARED_PTR<ServiceSkel> skel, MemberDefinition_Direction direction)
			: ArrayMemoryServiceSkelBase(membername, skel, RRPrimUtil<T>::GetTypeID(), sizeof(T), direction)
		{

		}

		virtual RR_SHARED_PTR<MessageElementData> DoRead(uint64_t memorypos, uint64_t bufferpos, uint64_t count, RR_SHARED_PTR<ArrayMemoryBase> mem)
		{
			RR_SHARED_PTR<ArrayMemory<T> > mem1 = rr_cast<ArrayMemory<T> >(mem);
			RR_SHARED_PTR<RRArray<T> > buf1 = AllocateRRArray<T>((size_t)count);
			mem1->Read(memorypos, buf1, 0, (size_t)count);
			return buf1;
		}

		virtual void DoWrite(uint64_t memorypos, RR_SHARED_PTR<MessageElementData> buffer, uint64_t bufferpos, uint64_t count, RR_SHARED_PTR<ArrayMemoryBase> mem)
		{
			RR_SHARED_PTR<ArrayMemory<T> > mem1 = rr_cast<ArrayMemory<T> >(mem);
			RR_SHARED_PTR<RRArray<T> > buf1 = rr_cast<RRArray<T> >(buffer);
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
		virtual RR_SHARED_PTR<MessageEntry> CallMemoryFunction(RR_SHARED_PTR<MessageEntry> m, RR_SHARED_PTR<Endpoint> e, RR_SHARED_PTR<MultiDimArrayMemoryBase > mem);
		virtual RR_SHARED_PTR<MessageElementData>  DoRead(const std::vector<uint64_t>& memorypos, const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count, int32_t elem_count, RR_SHARED_PTR<MultiDimArrayMemoryBase> mem) = 0;
		virtual void DoWrite(const std::vector<uint64_t>& memorypos, RR_SHARED_PTR<MessageElementData> buffer, const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count, int32_t elem_count, RR_SHARED_PTR<MultiDimArrayMemoryBase> mem) = 0;
	};

	template<typename T>
	class MultiDimArrayMemoryServiceSkel : public MultiDimArrayMemoryServiceSkelBase
	{
	public:
		MultiDimArrayMemoryServiceSkel(const std::string& membername, RR_SHARED_PTR<ServiceSkel> skel, MemberDefinition_Direction direction)
			: MultiDimArrayMemoryServiceSkelBase(membername, skel, RRPrimUtil<T>::GetTypeID(), sizeof(T), direction)
		{

		}

		virtual RR_SHARED_PTR<MessageElementData>  DoRead(const std::vector<uint64_t>& memorypos, const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count, int32_t elemcount, RR_SHARED_PTR<MultiDimArrayMemoryBase> mem)
		{
			RR_SHARED_PTR<MultiDimArrayMemory<T> > mem1 = rr_cast<MultiDimArrayMemory<T> >(mem);

			RR_SHARED_PTR<RRArray<T> > real = AllocateRRArray<T>((size_t)elemcount);
			RR_SHARED_PTR<RRMultiDimArray<T> > data;
			if (!mem1->Complex())
			{
				data = RR_MAKE_SHARED<RRMultiDimArray<T> >(VectorToRRArray<int32_t>(count), real);
			}
			else
			{
				RR_SHARED_PTR<RRArray<T> > imag = AllocateRRArray<T>((size_t)elemcount);
				data = RR_MAKE_SHARED<RRMultiDimArray<T> >(VectorToRRArray<int32_t>(count), real, imag);
			}
			mem1->Read(memorypos, data, bufferpos, count);
			return GetNode()->PackMultiDimArray(data);
		}
		virtual void DoWrite(const std::vector<uint64_t>& memorypos, RR_SHARED_PTR<MessageElementData> buffer, const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count, int32_t elemcount, RR_SHARED_PTR<MultiDimArrayMemoryBase> mem)
		{
			RR_SHARED_PTR<MultiDimArrayMemory<T> > mem1 = rr_cast<MultiDimArrayMemory<T> >(mem);

			RR_SHARED_PTR<RRMultiDimArray<T> > data = GetNode()->template UnpackMultiDimArray<T>(rr_cast<MessageElementMultiDimArray>(buffer));
			mem1->Write(memorypos, data, bufferpos, count);
		}
	};
	
	class ROBOTRACONTEUR_CORE_API ArrayMemoryClientBase : private boost::noncopyable
	{
	private:
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
		virtual void UnpackReadResult(RR_SHARED_PTR<MessageElementData> res, void* buffer, uint64_t offset, uint64_t count) = 0;
		virtual RR_SHARED_PTR<MessageElementData> PackWriteRequest(void* buffer, uint64_t offset, uint64_t count) = 0;
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

		virtual void Attach(RR_SHARED_PTR<RRArray<T> > memory)
		{
			throw InvalidOperationException("Invalid for client");
		}

		virtual void Read(uint64_t memorypos, RR_SHARED_PTR<RRArray<T> > buffer, uint64_t bufferpos, uint64_t count)
		{
			if (!buffer) throw NullValueException("Buffer must not be null");
			ReadBase(memorypos, &buffer, bufferpos, count);
		}

		virtual void Write(uint64_t memorypos, RR_SHARED_PTR<RRArray<T> > buffer, uint64_t bufferpos, uint64_t count)
		{
			if (!buffer) throw NullValueException("Buffer must not be null");
			WriteBase(memorypos, &buffer, bufferpos, count);
		}
				
		virtual uint64_t Length()
		{
			return ArrayMemoryClientBase::Length();
		}

	protected:
		virtual void UnpackReadResult(RR_SHARED_PTR<MessageElementData> res, void* buffer, uint64_t bufferpos, uint64_t count)
		{
			RR_SHARED_PTR<RRArray<T> > data = rr_cast<RRArray<T> >(res);
			RR_SHARED_PTR <RRArray<T> >& buffer1 = *static_cast<RR_SHARED_PTR<RRArray<T> >*>(buffer);
			memcpy(buffer1->ptr() + bufferpos, data->ptr(), (size_t)count * sizeof(T));
		}

		virtual RR_SHARED_PTR<MessageElementData> PackWriteRequest(void* buffer, uint64_t bufferpos, uint64_t count)
		{
			RR_SHARED_PTR <RRArray<T> >& buffer1 = *static_cast<RR_SHARED_PTR<RRArray<T> >*>(buffer);
			if (bufferpos == 0 && buffer1->size() == static_cast<size_t>(count))
			{
				return buffer1;
			}
			else if ((buffer1->size() - static_cast<size_t>(bufferpos)) >= static_cast<size_t>(count))
			{
				RR_SHARED_PTR<RRArray<T> > data = AllocateRRArray<T>((size_t)count);				
				memcpy(data->ptr(), buffer1->ptr() + bufferpos, (size_t)count * sizeof(T));
				return data;
			}
			else
				throw OutOfRangeException("");
		}

		virtual size_t GetBufferLength(void* buffer)
		{
			RR_SHARED_PTR <RRArray<T> >& buffer1 = *static_cast<RR_SHARED_PTR<RRArray<T> >*>(buffer);
			return buffer1->size();
		}
	};
	
	class ROBOTRACONTEUR_CORE_API MultiDimArrayMemoryClientBase : private boost::noncopyable
	{
	private:
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
		virtual bool Complex();
		
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
		virtual void UnpackReadResult(RR_SHARED_PTR<MessageElementData> res, void* buffer, const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count, uint64_t elemcount) = 0;
		virtual RR_SHARED_PTR<MessageElementData> PackWriteRequest(void* buffer, const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count, uint64_t elemcount) = 0;		
	};

	template <typename T>
	class MultiDimArrayMemoryClient : public virtual MultiDimArrayMemory<T>, public virtual MultiDimArrayMemoryClientBase
	{
	public:

		MultiDimArrayMemoryClient(const std::string& membername, RR_SHARED_PTR<ServiceStub> stub, MemberDefinition_Direction direction)
			: MultiDimArrayMemoryClientBase(membername, stub, RRPrimUtil<T>::GetTypeID(), sizeof(T), direction)
		{
		}

		virtual void Attach(RR_SHARED_PTR<RRMultiDimArray<T> > memory)
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

		virtual bool Complex()
		{
			return MultiDimArrayMemoryClientBase::Complex();
		}

		virtual void Read(const std::vector<uint64_t>& memorypos, RR_SHARED_PTR<RRMultiDimArray<T> > buffer, const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count)
		{
			if (!buffer) throw NullValueException("Buffer must not be null");
			ReadBase(memorypos, &buffer, bufferpos, count);
		}

		virtual void Write(const std::vector<uint64_t>& memorypos, RR_SHARED_PTR<RRMultiDimArray<T> > buffer, const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count)
		{
			if (!buffer) throw NullValueException("Buffer must not be null");
			WriteBase(memorypos, &buffer, bufferpos, count);
		}

	protected:
		virtual void UnpackReadResult(RR_SHARED_PTR<MessageElementData> res, void* buffer, const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count, uint64_t elemcount)
		{
			RR_SHARED_PTR<RRMultiDimArray<T> >& buffer1 = *static_cast<RR_SHARED_PTR<RRMultiDimArray<T> >* > (buffer);
			RR_SHARED_PTR<RRMultiDimArray<T> > data = GetNode()->template UnpackMultiDimArray<T>(rr_cast<MessageElementMultiDimArray>(res));

			RR_SHARED_PTR<MultiDimArrayMemory<T> > data2 = RR_MAKE_SHARED<MultiDimArrayMemory<T> >(data);
			data2->Read(std::vector<uint64_t>(count.size()), buffer1, bufferpos, count);
		}
		virtual RR_SHARED_PTR<MessageElementData> PackWriteRequest(void* buffer, const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count, uint64_t elemcount)
		{
			RR_SHARED_PTR<RRMultiDimArray<T> >& buffer1 = *static_cast<RR_SHARED_PTR<RRMultiDimArray<T> >* > (buffer);

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
				RR_SHARED_PTR<RRMultiDimArray<T> > data;
				if (!buffer1->Complex)
				{
					data = RR_MAKE_SHARED<RRMultiDimArray<T> >(VectorToRRArray<int32_t>(count), AllocateRRArray<T>((size_t)elemcount));
				}
				else
				{
					data = RR_MAKE_SHARED<RRMultiDimArray<T> >(VectorToRRArray<int32_t>(count), AllocateRRArray<T>((size_t)elemcount), AllocateRRArray<T>((size_t)elemcount));
				}

				buffer1->RetrieveSubArray(detail::ConvertVectorType<int32_t>(bufferpos), data, std::vector<int32_t>(count.size()), detail::ConvertVectorType<int32_t>(count));
				return GetNode()->PackMultiDimArray(data);
			}
		}
	};

	// cstruct

	template <typename T>
	class CStructureArrayMemory : public virtual ArrayMemoryBase
	{
	private:
		RR_SHARED_PTR<RRCStructureArray<T> > memory;
		boost::mutex memory_lock;

	public:

		CStructureArrayMemory() {};

		CStructureArrayMemory(RR_SHARED_PTR<RRCStructureArray<T> > memory)
		{
			this->memory = memory;
		}

		virtual void Attach(RR_SHARED_PTR<RRCStructureArray<T> > memory)
		{
			boost::mutex::scoped_lock lock(memory_lock);
			this->memory = memory;
		}

		virtual uint64_t Length()
		{
			boost::mutex::scoped_lock lock(memory_lock);
			return memory->cstruct_array.size();
		}

		virtual void Read(uint64_t memorypos, RR_SHARED_PTR<RRCStructureArray<T> > buffer, uint64_t bufferpos, uint64_t count)
		{
			boost::mutex::scoped_lock lock(memory_lock);
			if (memorypos + count > memory->cstruct_array.size()) throw OutOfRangeException("Index out of range");
			if (bufferpos + count > buffer->cstruct_array.size()) throw OutOfRangeException("Index out of range");

			for (size_t i = 0; i < count; i++)
			{
				buffer->cstruct_array.at(i + bufferpos) = memory->cstruct_array.at(i + memorypos);
			}
		}

		virtual void Write(uint64_t memorypos, RR_SHARED_PTR<RRCStructureArray<T> > buffer, uint64_t bufferpos, uint64_t count)
		{
			boost::mutex::scoped_lock lock(memory_lock);
			if (memorypos + count > memory->cstruct_array.size()) throw OutOfRangeException("Index out of range");
			if (bufferpos + count > buffer->cstruct_array.size()) throw OutOfRangeException("Index out of range");			
			for (size_t i = 0; i < count; i++)
			{
				memory->cstruct_array.at(i + memorypos) = buffer->cstruct_array.at(i + bufferpos);
			}
		}

		virtual DataTypes ElementTypeID()
		{
			return DataTypes_cstructure_t;
		}
	};

	template<typename T>
	class CStructureArrayMemoryServiceSkel : public ArrayMemoryServiceSkelBase
	{
	public:
		CStructureArrayMemoryServiceSkel(const std::string& membername, RR_SHARED_PTR<ServiceSkel> skel, size_t element_size, MemberDefinition_Direction direction)
			: ArrayMemoryServiceSkelBase(membername, skel, DataTypes_cstructure_t, element_size, direction)
		{

		}

		virtual RR_SHARED_PTR<MessageElementData> DoRead(uint64_t memorypos, uint64_t bufferpos, uint64_t count, RR_SHARED_PTR<ArrayMemoryBase> mem)
		{
			RR_SHARED_PTR<CStructureArrayMemory<T> > mem1 = rr_cast<CStructureArrayMemory<T> >(mem);
			RR_SHARED_PTR<RRCStructureArray<T> > buf1 = RR_MAKE_SHARED<RRCStructureArray<T> >();
			buf1->cstruct_array.resize(count);				
			mem1->Read(memorypos, buf1, 0, (size_t)count);
			return CStructureStub_PackCStructureArray(buf1);
		}

		virtual void DoWrite(uint64_t memorypos, RR_SHARED_PTR<MessageElementData> buffer, uint64_t bufferpos, uint64_t count, RR_SHARED_PTR<ArrayMemoryBase> mem)
		{
			RR_SHARED_PTR<CStructureArrayMemory<T> > mem1 = rr_cast<CStructureArrayMemory<T> >(mem);
			RR_SHARED_PTR<RRCStructureArray<T> > buf1 = CStructureStub_UnpackCStructureArray<T>(rr_cast<MessageElementCStructureArray>(buffer));
			mem1->Write(memorypos, buf1, 0, (size_t)count);
		}
	};

	template<typename T>
	class CStructureArrayMemoryClient : public virtual ArrayMemoryClientBase, public virtual CStructureArrayMemory<T>
	{
	public:

		CStructureArrayMemoryClient(const std::string& membername, RR_SHARED_PTR<ServiceStub> stub, size_t element_size, MemberDefinition_Direction direction)
			: ArrayMemoryClientBase(membername, stub, RRPrimUtil<T>::GetTypeID(), element_size, direction)
		{
		}

		virtual void Attach(RR_SHARED_PTR<RRCStructureArray<T> > memory)
		{
			throw InvalidOperationException("Invalid for client");
		}

		virtual void Read(uint64_t memorypos, RR_SHARED_PTR<RRCStructureArray<T> > buffer, uint64_t bufferpos, uint64_t count)
		{
			if (!buffer) throw NullValueException("Buffer must not be null");
			ReadBase(memorypos, &buffer, bufferpos, count);
		}

		virtual void Write(uint64_t memorypos, RR_SHARED_PTR<RRCStructureArray<T> > buffer, uint64_t bufferpos, uint64_t count)
		{
			if (!buffer) throw NullValueException("Buffer must not be null");
			WriteBase(memorypos, &buffer, bufferpos, count);
		}

		virtual uint64_t Length()
		{
			return ArrayMemoryClientBase::Length();
		}

	protected:
		virtual void UnpackReadResult(RR_SHARED_PTR<MessageElementData> res,void* buffer, uint64_t bufferpos, uint64_t count)
		{
			RR_SHARED_PTR<RRCStructureArray<T> >& buffer1 = *static_cast<RR_SHARED_PTR<RRCStructureArray<T> >* >(buffer);
			RR_SHARED_PTR<RRCStructureArray<T> > res1 = CStructureStub_UnpackCStructureArray<T>(rr_cast<RobotRaconteur::MessageElementCStructureArray>(res));

			for (size_t i = 0; i < count; i++)
			{
				buffer1->cstruct_array.at(i + bufferpos) = res1->cstruct_array.at(i);
			}			
		}

		virtual RR_SHARED_PTR<MessageElementData> PackWriteRequest(void* buffer, uint64_t bufferpos, uint64_t count)
		{
			RR_SHARED_PTR<RRCStructureArray<T> >& buffer1 = *static_cast<RR_SHARED_PTR<RRCStructureArray<T> >* >(buffer);
			RR_SHARED_PTR<RRCStructureArray<T> > o = RR_MAKE_SHARED<RRCStructureArray<T> >();
			o->cstruct_array.resize(count);

			for (size_t i = 0; i < count; i++)
			{
				o->cstruct_array.at(i) = buffer1->cstruct_array.at(i + bufferpos);
			}
			return CStructureStub_PackCStructureArray(o);
		}

		virtual size_t GetBufferLength(void* buffer)
		{
			RR_SHARED_PTR<RRCStructureArray<T> >& buffer1 = *static_cast<RR_SHARED_PTR<RRCStructureArray<T> >*>(buffer);
			return buffer1->cstruct_array.size();
		}
	};

	template <typename T>
	class CStructureMultiDimArrayMemory : public virtual MultiDimArrayMemoryBase
	{
	private:
		RR_SHARED_PTR<RRCStructureMultiDimArray<T> > multimemory;
		boost::mutex memory_lock;
	public:

		CStructureMultiDimArrayMemory() {};

		CStructureMultiDimArrayMemory(RR_SHARED_PTR<RRCStructureMultiDimArray<T> > multimemory)
		{
			this->multimemory = multimemory;
		}

		virtual ~CStructureMultiDimArrayMemory() {}

		virtual void Attach(RR_SHARED_PTR<RRCStructureMultiDimArray<T> > multimemory)
		{
			boost::mutex::scoped_lock lock(memory_lock);
			this->multimemory = multimemory;
		}

		virtual std::vector<uint64_t> Dimensions()
		{
			boost::mutex::scoped_lock lock(memory_lock);
			RR_SHARED_PTR<RRArray<int32_t> > dims = multimemory->Dims;
			std::vector<uint64_t> s(dims->Length());
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
		
		virtual bool Complex()
		{
			return false;
		}

		virtual void Read(const std::vector<uint64_t>& memorypos, RR_SHARED_PTR<RRCStructureMultiDimArray<T> > buffer, const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count)
		{
			boost::mutex::scoped_lock lock(memory_lock);
			multimemory->RetrieveSubArray(detail::ConvertVectorType<int32_t>(memorypos), buffer, detail::ConvertVectorType<int32_t>(bufferpos), detail::ConvertVectorType<int32_t>(count));
		}

		virtual void Write(const std::vector<uint64_t>& memorypos, RR_SHARED_PTR<RRCStructureMultiDimArray<T> > buffer, const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count)
		{
			boost::mutex::scoped_lock lock(memory_lock);
			multimemory->AssignSubArray(detail::ConvertVectorType<int32_t>(memorypos), buffer, detail::ConvertVectorType<int32_t>(bufferpos), detail::ConvertVectorType<int32_t>(count));
		}

		virtual DataTypes ElementTypeID()
		{
			return DataTypes_cstructure_t;
		}
	};

	template <typename T>
	class CStructureMultiDimArrayMemoryClient : public virtual CStructureMultiDimArrayMemory<T>, public virtual MultiDimArrayMemoryClientBase
	{
	public:

		CStructureMultiDimArrayMemoryClient(const std::string& membername, RR_SHARED_PTR<ServiceStub> stub, size_t element_size, MemberDefinition_Direction direction)
			: MultiDimArrayMemoryClientBase(membername, stub, DataTypes_cstructure_t, element_size, direction)
		{
		}

		virtual void Attach(RR_SHARED_PTR<RRCStructureMultiDimArray<T> > memory)
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

		virtual bool Complex()
		{
			return MultiDimArrayMemoryClientBase::Complex();
		}

		virtual void Read(const std::vector<uint64_t>& memorypos, RR_SHARED_PTR<RRCStructureMultiDimArray<T> > buffer, const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count)
		{
			if (!buffer) throw NullValueException("Buffer must not be null");
			ReadBase(memorypos, &buffer, bufferpos, count);
		}

		virtual void Write(const std::vector<uint64_t>& memorypos, RR_SHARED_PTR<RRCStructureMultiDimArray<T> > buffer, const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count)
		{
			if (!buffer) throw NullValueException("Buffer must not be null");
			WriteBase(memorypos, &buffer, bufferpos, count);
		}

	protected:
		virtual void UnpackReadResult(RR_SHARED_PTR<MessageElementData> res, void* buffer, const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count, uint64_t elemcount)
		{
			RR_SHARED_PTR<RRCStructureMultiDimArray<T> >& buffer1 = *static_cast<RR_SHARED_PTR<RRCStructureMultiDimArray<T> >* > (buffer);
			RR_SHARED_PTR<RRCStructureMultiDimArray<T> > data = rr_cast<RRCStructureMultiDimArray<T> >(GetNode()->UnpackCStructureMultiDimArray(rr_cast<MessageElementCStructureMultiDimArray>(res)));

			RR_SHARED_PTR<CStructureMultiDimArrayMemory<T> > data2 = RR_MAKE_SHARED<CStructureMultiDimArrayMemory<T> >(data);
			data2->Read(std::vector<uint64_t>(count.size()), buffer1, bufferpos, count);
		}
		virtual RR_SHARED_PTR<MessageElementData> PackWriteRequest(void* buffer, const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count, uint64_t elemcount)
		{
			RR_SHARED_PTR<RRCStructureMultiDimArray<T> >& buffer1 = *static_cast<RR_SHARED_PTR<RRCStructureMultiDimArray<T> >* >(buffer);

			RR_SHARED_PTR<RRCStructureMultiDimArray<T> > data = RR_MAKE_SHARED<RRCStructureMultiDimArray<T> >();
			data->Dims = VectorToRRArray<int32_t>(count);
			data->CStructArray = RR_MAKE_SHARED<RRCStructureArray<T> >();
			data->CStructArray->cstruct_array.resize(elemcount);
						
			buffer1->RetrieveSubArray(detail::ConvertVectorType<int32_t>(bufferpos), data, std::vector<int32_t>(count.size()), detail::ConvertVectorType<int32_t>(count));
			return GetNode()->PackCStructureMultiDimArray(data);
			
		}
	};

	template<typename T>
	class CStructureMultiDimArrayMemoryServiceSkel : public MultiDimArrayMemoryServiceSkelBase
	{
	public:
		CStructureMultiDimArrayMemoryServiceSkel(const std::string& membername, RR_SHARED_PTR<ServiceSkel> skel, size_t element_size, MemberDefinition_Direction direction)
			: MultiDimArrayMemoryServiceSkelBase(membername, skel, DataTypes_structure_t, element_size, direction)
		{

		}

		virtual RR_SHARED_PTR<MessageElementData>  DoRead(const std::vector<uint64_t>& memorypos, const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count, int32_t elemcount, RR_SHARED_PTR<MultiDimArrayMemoryBase> mem)
		{
			RR_SHARED_PTR<CStructureMultiDimArrayMemory<T> > mem1 = rr_cast<CStructureMultiDimArrayMemory<T> >(mem);
						
			RR_SHARED_PTR<RRCStructureMultiDimArray<T> > data = RR_MAKE_SHARED<RRCStructureMultiDimArray<T> >();
			data->Dims = VectorToRRArray<int32_t>(count);
			data->CStructArray = RR_MAKE_SHARED<RRCStructureArray<T> >();
			data->CStructArray->cstruct_array.resize((size_t)elemcount);				
			
			mem1->Read(memorypos, data, bufferpos, count);
			return GetNode()->PackCStructureMultiDimArray(data);
		}
		virtual void DoWrite(const std::vector<uint64_t>& memorypos, RR_SHARED_PTR<MessageElementData> buffer, const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count, int32_t elemcount, RR_SHARED_PTR<MultiDimArrayMemoryBase> mem)
		{
			RR_SHARED_PTR<CStructureMultiDimArrayMemory<T> > mem1 = rr_cast<CStructureMultiDimArrayMemory<T> >(mem);

			RR_SHARED_PTR<RRCStructureMultiDimArray<T> > data = rr_cast<RRCStructureMultiDimArray<T> >(GetNode()->UnpackCStructureMultiDimArray(rr_cast<MessageElementCStructureMultiDimArray>(buffer)));
			mem1->Write(memorypos, data, bufferpos, count);
		}
	};



	// astruct

	template <typename T>
	class AStructureArrayMemory : public virtual ArrayMemoryBase
	{
	private:
		RR_SHARED_PTR<RRAStructureArray<T> > memory;
		boost::mutex memory_lock;

	public:

		AStructureArrayMemory() {};

		AStructureArrayMemory(RR_SHARED_PTR<RRAStructureArray<T> > memory)
		{
			this->memory = memory;
		}

		virtual void Attach(RR_SHARED_PTR<RRAStructureArray<T> > memory)
		{
			boost::mutex::scoped_lock lock(memory_lock);
			this->memory = memory;
		}

		virtual uint64_t Length()
		{
			boost::mutex::scoped_lock lock(memory_lock);
			return memory->Length();
		}

		virtual void Read(uint64_t memorypos, RR_SHARED_PTR<RRAStructureArray<T> > buffer, uint64_t bufferpos, uint64_t count)
		{
			boost::mutex::scoped_lock lock(memory_lock);
			if (memorypos + count > memory->Length()) throw OutOfRangeException("Index out of range");
			if (bufferpos + count > buffer->Length()) throw OutOfRangeException("Index out of range");

			for (size_t i = 0; i < count; i++)
			{
				(*buffer)[(i + bufferpos)] = (*memory)[(i + memorypos)];
			}
		}

		virtual void Write(uint64_t memorypos, RR_SHARED_PTR<RRAStructureArray<T> > buffer, uint64_t bufferpos, uint64_t count)
		{
			boost::mutex::scoped_lock lock(memory_lock);
			if (memorypos + count > memory->Length()) throw OutOfRangeException("Index out of range");
			if (bufferpos + count > buffer->Length()) throw OutOfRangeException("Index out of range");
			for (size_t i = 0; i < count; i++)
			{
				(*memory)[(i + memorypos)] = (*buffer)[(i + bufferpos)];
			}
		}

		virtual DataTypes ElementTypeID()
		{
			return DataTypes_cstructure_t;
		}
	};

	template<typename T>
	class AStructureArrayMemoryServiceSkel : public ArrayMemoryServiceSkelBase
	{
	public:
		AStructureArrayMemoryServiceSkel(const std::string& membername, RR_SHARED_PTR<ServiceSkel> skel, size_t element_size, MemberDefinition_Direction direction)
			: ArrayMemoryServiceSkelBase(membername, skel, DataTypes_cstructure_t, element_size, direction)
		{

		}

		virtual RR_SHARED_PTR<MessageElementData> DoRead(uint64_t memorypos, uint64_t bufferpos, uint64_t count, RR_SHARED_PTR<ArrayMemoryBase> mem)
		{
			RR_SHARED_PTR<AStructureArrayMemory<T> > mem1 = rr_cast<AStructureArrayMemory<T>>(mem);
			RR_SHARED_PTR<RRAStructureArray<T> > buf1 = AllocateEmptyRRAStructureArray<T>(count);
			mem1->Read(memorypos, buf1, 0, (size_t)count);
			return AStructureStub_PackAStructureArray(buf1);
		}

		virtual void DoWrite(uint64_t memorypos, RR_SHARED_PTR<MessageElementData> buffer, uint64_t bufferpos, uint64_t count, RR_SHARED_PTR<ArrayMemoryBase> mem)
		{
			RR_SHARED_PTR<AStructureArrayMemory<T> > mem1 = rr_cast<AStructureArrayMemory<T>>(mem);
			RR_SHARED_PTR<RRAStructureArray<T> > buf1 = AStructureStub_UnpackAStructureArray<T>(rr_cast<MessageElementAStructureArray>(buffer));
			mem1->Write(memorypos, buf1, 0, (size_t)count);
		}
	};

	template<typename T>
	class AStructureArrayMemoryClient : public virtual ArrayMemoryClientBase, public virtual AStructureArrayMemory<T>
	{
	public:

		AStructureArrayMemoryClient(const std::string& membername, RR_SHARED_PTR<ServiceStub> stub, size_t element_size, MemberDefinition_Direction direction)
			: ArrayMemoryClientBase(membername, stub, RRPrimUtil<T>::GetTypeID(), element_size, direction)
		{
		}

		virtual void Attach(RR_SHARED_PTR<RRAStructureArray<T> > memory)
		{
			throw InvalidOperationException("Invalid for client");
		}

		virtual void Read(uint64_t memorypos, RR_SHARED_PTR<RRAStructureArray<T> > buffer, uint64_t bufferpos, uint64_t count)
		{
			if (!buffer) throw NullValueException("Buffer must not be null");
			ReadBase(memorypos, &buffer, bufferpos, count);
		}

		virtual void Write(uint64_t memorypos, RR_SHARED_PTR<RRAStructureArray<T> > buffer, uint64_t bufferpos, uint64_t count)
		{
			if (!buffer) throw NullValueException("Buffer must not be null");
			WriteBase(memorypos, &buffer, bufferpos, count);
		}

		virtual uint64_t Length()
		{
			return ArrayMemoryClientBase::Length();
		}

	protected:
		virtual void UnpackReadResult(RR_SHARED_PTR<MessageElementData> res, void* buffer, uint64_t bufferpos, uint64_t count)
		{
			RR_SHARED_PTR<RRAStructureArray<T> >& buffer1 = *static_cast<RR_SHARED_PTR<RRAStructureArray<T> >*>(buffer);
			RR_SHARED_PTR<RRAStructureArray<T> > res1 = AStructureStub_UnpackAStructureArray<T>(rr_cast<RobotRaconteur::MessageElementAStructureArray>(res));

			for (size_t i = 0; i < count; i++)
			{
				(*buffer1)[(i + bufferpos)] = (*res1)[i];
			}
		}

		virtual RR_SHARED_PTR<MessageElementData> PackWriteRequest(void* buffer, uint64_t bufferpos, uint64_t count)
		{
			RR_SHARED_PTR<RRAStructureArray<T> >& buffer1 = *static_cast<RR_SHARED_PTR<RRAStructureArray<T> >*>(buffer);
			RR_SHARED_PTR<RRAStructureArray<T> > o = AllocateEmptyRRAStructureArray<T>(count);

			for (size_t i = 0; i < count; i++)
			{
				(*o)[i] = (*buffer1)[(i + bufferpos)];
			}
			return AStructureStub_PackAStructureArray(o);
		}

		virtual size_t GetBufferLength(void* buffer)
		{
			RR_SHARED_PTR<RRAStructureArray<T> >& buffer1 = *static_cast<RR_SHARED_PTR<RRAStructureArray<T> >*>(buffer);
			return buffer1->Length();
		}
	};

	template <typename T>
	class AStructureMultiDimArrayMemory : public virtual MultiDimArrayMemoryBase
	{
	private:
		RR_SHARED_PTR<RRAStructureMultiDimArray<T> > multimemory;
		boost::mutex memory_lock;
	public:

		AStructureMultiDimArrayMemory() {};

		AStructureMultiDimArrayMemory(RR_SHARED_PTR<RRAStructureMultiDimArray<T> > multimemory)
		{
			this->multimemory = multimemory;
		}

		virtual ~AStructureMultiDimArrayMemory() {}

		virtual void Attach(RR_SHARED_PTR<RRAStructureMultiDimArray<T> > multimemory)
		{
			boost::mutex::scoped_lock lock(memory_lock);
			this->multimemory = multimemory;
		}

		virtual std::vector<uint64_t> Dimensions()
		{
			boost::mutex::scoped_lock lock(memory_lock);
			RR_SHARED_PTR<RRArray<int32_t> > dims = multimemory->Dims;
			std::vector<uint64_t> s(dims->Length());
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

		virtual bool Complex()
		{
			return false;
		}

		virtual void Read(const std::vector<uint64_t>& memorypos, RR_SHARED_PTR<RRAStructureMultiDimArray<T> > buffer, const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count)
		{
			boost::mutex::scoped_lock lock(memory_lock);
			multimemory->RetrieveSubArray(detail::ConvertVectorType<int32_t>(memorypos), buffer, detail::ConvertVectorType<int32_t>(bufferpos), detail::ConvertVectorType<int32_t>(count));
		}

		virtual void Write(const std::vector<uint64_t>& memorypos, RR_SHARED_PTR<RRAStructureMultiDimArray<T> > buffer, const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count)
		{
			boost::mutex::scoped_lock lock(memory_lock);
			multimemory->AssignSubArray(detail::ConvertVectorType<int32_t>(memorypos), buffer, detail::ConvertVectorType<int32_t>(bufferpos), detail::ConvertVectorType<int32_t>(count));
		}

		virtual DataTypes ElementTypeID()
		{
			return DataTypes_cstructure_t;
		}
	};

	template <typename T>
	class AStructureMultiDimArrayMemoryClient : public virtual AStructureMultiDimArrayMemory<T>, public virtual MultiDimArrayMemoryClientBase
	{
	public:

		AStructureMultiDimArrayMemoryClient(const std::string& membername, RR_SHARED_PTR<ServiceStub> stub, size_t element_size, MemberDefinition_Direction direction)
			: MultiDimArrayMemoryClientBase(membername, stub, DataTypes_cstructure_t, element_size, direction)
		{
		}

		virtual void Attach(RR_SHARED_PTR<RRAStructureMultiDimArray<T> > memory)
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

		virtual bool Complex()
		{
			return MultiDimArrayMemoryClientBase::Complex();
		}

		virtual void Read(const std::vector<uint64_t>& memorypos, RR_SHARED_PTR<RRAStructureMultiDimArray<T> > buffer, const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count)
		{
			if (!buffer) throw NullValueException("Buffer must not be null");
			ReadBase(memorypos, &buffer, bufferpos, count);
		}

		virtual void Write(const std::vector<uint64_t>& memorypos, RR_SHARED_PTR<RRAStructureMultiDimArray<T> > buffer, const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count)
		{
			if (!buffer) throw NullValueException("Buffer must not be null");
			WriteBase(memorypos, &buffer, bufferpos, count);
		}

	protected:
		virtual void UnpackReadResult(RR_SHARED_PTR<MessageElementData> res, void* buffer, const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count, uint64_t elemcount)
		{
			RR_SHARED_PTR<RRAStructureMultiDimArray<T> >& buffer1 = *static_cast<RR_SHARED_PTR<RRAStructureMultiDimArray<T> >*> (buffer);
			RR_SHARED_PTR<RRAStructureMultiDimArray<T> > data = rr_cast<RRAStructureMultiDimArray<T>>(GetNode()->UnpackAStructureMultiDimArray(rr_cast<MessageElementAStructureMultiDimArray>(res)));

			RR_SHARED_PTR<AStructureMultiDimArrayMemory<T> > data2 = RR_MAKE_SHARED<AStructureMultiDimArrayMemory<T> >(data);
			data2->Read(std::vector<uint64_t>(count.size()), buffer1, bufferpos, count);
		}
		virtual RR_SHARED_PTR<MessageElementData> PackWriteRequest(void* buffer, const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count, uint64_t elemcount)
		{
			RR_SHARED_PTR<RRAStructureMultiDimArray<T> >& buffer1 = *static_cast<RR_SHARED_PTR<RRAStructureMultiDimArray<T> >*>(buffer);

			RR_SHARED_PTR<RRAStructureMultiDimArray<T> > data = RR_MAKE_SHARED<RRAStructureMultiDimArray<T> >();
			data->Dims = VectorToRRArray<int32_t>(count);			
			data->AStructArray = AllocateEmptyRRAStructureArray<T>(elemcount);

			buffer1->RetrieveSubArray(detail::ConvertVectorType<int32_t>(bufferpos), data, std::vector<int32_t>(count.size()), detail::ConvertVectorType<int32_t>(count));
			return GetNode()->PackAStructureMultiDimArray(data);

		}
	};

	template<typename T>
	class AStructureMultiDimArrayMemoryServiceSkel : public MultiDimArrayMemoryServiceSkelBase
	{
	public:
		AStructureMultiDimArrayMemoryServiceSkel(const std::string& membername, RR_SHARED_PTR<ServiceSkel> skel, size_t element_size, MemberDefinition_Direction direction)
			: MultiDimArrayMemoryServiceSkelBase(membername, skel, DataTypes_structure_t, element_size, direction)
		{

		}

		virtual RR_SHARED_PTR<MessageElementData>  DoRead(const std::vector<uint64_t>& memorypos, const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count, int32_t elemcount, RR_SHARED_PTR<MultiDimArrayMemoryBase> mem)
		{
			RR_SHARED_PTR<AStructureMultiDimArrayMemory<T> > mem1 = rr_cast<AStructureMultiDimArrayMemory<T>>(mem);

			RR_SHARED_PTR<RRAStructureMultiDimArray<T> > data = RR_MAKE_SHARED<RRAStructureMultiDimArray<T> >();
			data->Dims = VectorToRRArray<int32_t>(count);			
			data->AStructArray = AllocateEmptyRRAStructureArray<T>((size_t)elemcount);

			mem1->Read(memorypos, data, bufferpos, count);
			return GetNode()->PackAStructureMultiDimArray(data);
		}
		virtual void DoWrite(const std::vector<uint64_t>& memorypos, RR_SHARED_PTR<MessageElementData> buffer, const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count, int32_t elemcount, RR_SHARED_PTR<MultiDimArrayMemoryBase> mem)
		{
			RR_SHARED_PTR<AStructureMultiDimArrayMemory<T> > mem1 = rr_cast<AStructureMultiDimArrayMemory<T>>(mem);

			RR_SHARED_PTR<RRAStructureMultiDimArray<T> > data = rr_cast<RRAStructureMultiDimArray<T>>(GetNode()->UnpackAStructureMultiDimArray(rr_cast<MessageElementAStructureMultiDimArray>(buffer)));
			mem1->Write(memorypos, data, bufferpos, count);
		}
	};
}
