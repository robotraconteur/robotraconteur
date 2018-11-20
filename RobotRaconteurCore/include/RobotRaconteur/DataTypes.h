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

#include <string>
#include <vector>

#include <stdint.h>
#include <map>
#include <deque>
#include <list>

#include <boost/array.hpp>
#include <boost/tuple/tuple.hpp>

#include "RobotRaconteur/RobotRaconteurConstants.h"
#include "RobotRaconteur/Error.h"

#include <stdexcept>

#include "RobotRaconteur/RobotRaconteurConfig.h"

#ifdef ROBOTRACONTEUR_USE_SMALL_VECTOR
#include <boost/container/small_vector.hpp>
#endif

#include <boost/asio/buffer.hpp>

#include <boost/utility/value_init.hpp>
#include <boost/container/static_vector.hpp>
#include <boost/array.hpp>
#include <boost/foreach.hpp>
#include <boost/range/adaptor/map.hpp>

#pragma once

#define RR_NULL_CHECK(ptr) {if (!ptr) throw NullValueException("Null pointer");}

namespace RobotRaconteur
{
	template<typename T, typename U>
	class rr_cast_support
	{
	public:
		static RR_SHARED_PTR<T> rr_cast(const RR_SHARED_PTR<U>& objin)
		{
			if (!objin) return RR_SHARED_PTR<T>();

			RR_SHARED_PTR<T> c = RR_DYNAMIC_POINTER_CAST<T>(objin);
			if (!c)
			{
				throw DataTypeMismatchException("Data type cast error");
			}

			return c;
		}
	};

	template<typename T>
	class rr_cast_support<T,T>
	{
	public:
		static const RR_SHARED_PTR<T>& rr_cast(const RR_SHARED_PTR<T>& objin)
		{			
			return objin;
		}
	};

	template<typename T, typename U> static RR_SHARED_PTR<T> rr_cast(const RR_SHARED_PTR<U>& objin)
	{
		return rr_cast_support<T, U>::rr_cast(objin);
	}

	class ROBOTRACONTEUR_CORE_API RRObject : boost::noncopyable
	{
	public:
		RRObject();

		virtual ~RRObject() {}

		virtual std::string RRType()=0;
	};

	class ROBOTRACONTEUR_CORE_API RRValue : boost::noncopyable
	{
	public:
		RRValue();

		virtual ~RRValue() {}

		virtual std::string RRType() = 0;
	};

	class ROBOTRACONTEUR_CORE_API  MessageElementData : public RRValue
	{
	public:
		virtual std::string GetTypeString()=0;
		virtual DataTypes GetTypeID()=0;
	};


	template<typename T>
	class RRArray;

	template<typename T>
	static RR_SHARED_PTR<RRArray<T> > ScalarToRRArray(T value);

	template<typename T>
	static T RRArrayToScalar(RR_SHARED_PTR<RRArray<T> > value);


	ROBOTRACONTEUR_CORE_API RR_SHARED_PTR<RRArray<char> > stringToRRArray(const std::string& str);

	ROBOTRACONTEUR_CORE_API std::string RRArrayToString(RR_SHARED_PTR<RRArray<char> > arr);


	

#ifdef ROBOTRACONTEUR_USE_WSTRING
	ROBOTRACONTEUR_CORE_API RR_SHARED_PTR<RRArray<char> > wstringToRRArray(const std::wstring& str);

	ROBOTRACONTEUR_CORE_API std::wstring RRArrayToWString(RR_SHARED_PTR<RRArray<char> > arr);

	// Convert a wide Unicode string to an UTF8 string
	ROBOTRACONTEUR_CORE_API std::string utf8_encode(const std::wstring &wstr);

	// Convert an UTF8 string to a wide Unicode String
	ROBOTRACONTEUR_CORE_API std::wstring utf8_decode(const std::string &str);
#endif


	template <typename T> class RRPrimUtil  
	{ 
	public:
		static DataTypes GetTypeID() {return DataTypes_void_t;} 		
		
		static RR_SHARED_PTR<T> PrePack(const T& val) {return val;} 
		
		template <typename U>
		static T PreUnpack(U& val) {return rr_cast<T>(val);} 

		virtual ~RRPrimUtil() {};

		typedef RR_SHARED_PTR<RRValue> BoxedType;
	};
	
	template <typename T> class RRPrimUtil<RR_SHARED_PTR<T> >
	{ 
	public:
		static DataTypes GetTypeID() {return DataTypes_void_t;}
		static std::string GetElementTypeString() {
			return "";
		}
		
		static RR_SHARED_PTR<RRValue> PrePack(const RR_SHARED_PTR<T>& val) {return rr_cast<RRValue>(val);}
		
		template<typename U>
		static RR_SHARED_PTR<T> PreUnpack(const U& val) {return rr_cast<T>(val);} 
		
		virtual ~RRPrimUtil() {};

		typedef RR_SHARED_PTR<T> BoxedType;
	};

	template<> class RRPrimUtil<std::string>
	{
	public:
		static DataTypes GetTypeID() {return DataTypes_string_t;}
		static std::string GetElementTypeString() {
			return "";
		}
		static RR_SHARED_PTR<RRValue> PrePack(const std::string& val) {return rr_cast<RRValue>(stringToRRArray(val));} 
		static std::string PreUnpack(RR_SHARED_PTR<RRValue> val) {return RRArrayToString(rr_cast<RRArray<char> >(val));} 

		virtual ~RRPrimUtil() {};

		typedef RR_SHARED_PTR<RRArray<char> > BoxedType;

	};

#ifdef ROBOTRACONTEUR_USE_WSTRING

	template<> class RRPrimUtil<std::wstring>
	{
	public:
		static DataTypes GetTypeID() {return DataTypes_string_t;} 
		static RR_SHARED_PTR<RRValue> PrePack(std::wstring val) {return rr_cast<RRValue>(wstringToRRArray(val));} 
		static std::wstring PreUnpack(RR_SHARED_PTR<RRValue> val) {return RRArrayToWString(rr_cast<RRArray<char> >(val));} 

		typedef RR_SHARED_PTR<RRArary<wchar_t> > BoxedType;
	};
#endif

#define RRPrimUtilNumeric(x,code) \
	template<> class RRPrimUtil<x> \
	{ \
		public: \
		static DataTypes GetTypeID() {return code;}  \
		static std::string GetElementTypeString() {return ""; } \
		static RR_SHARED_PTR<RRArray<x> > PrePack(const x& val) {return ScalarToRRArray(val);}\
		template<typename U> \
		static x PreUnpack(const U& val) {return RRArrayToScalar(rr_cast<RRArray<x> >(val));} \
		typedef RR_SHARED_PTR<RRArray<x> > BoxedType; \
	};

	RRPrimUtilNumeric(double,DataTypes_double_t);
	RRPrimUtilNumeric(float,DataTypes_single_t);

	RRPrimUtilNumeric(int8_t,DataTypes_int8_t);
	RRPrimUtilNumeric(uint8_t,DataTypes_uint8_t);
	RRPrimUtilNumeric(int16_t,DataTypes_int16_t);
	RRPrimUtilNumeric(uint16_t,DataTypes_uint16_t);
	RRPrimUtilNumeric(int32_t,DataTypes_int32_t);
	RRPrimUtilNumeric(uint32_t,DataTypes_uint32_t);
	RRPrimUtilNumeric(int64_t,DataTypes_int64_t);
	RRPrimUtilNumeric(uint64_t,DataTypes_uint64_t);
	RRPrimUtilNumeric(char,DataTypes_string_t);

	ROBOTRACONTEUR_CORE_API std::string GetRRDataTypeString(DataTypes type);
	ROBOTRACONTEUR_CORE_API bool IsTypeRRArray(DataTypes type);
	ROBOTRACONTEUR_CORE_API bool IsTypeNumeric(DataTypes type);

	class ROBOTRACONTEUR_CORE_API RRBaseArray : public MessageElementData
	{
	public:
		

		virtual std::string GetTypeString()
		{
			std::string type=GetRRDataTypeString(GetTypeID());	
			return type+"[]";
		}

		virtual ~RRBaseArray() {}

		virtual size_t Length()=0;
		
		virtual std::string RRType();

		virtual void* void_ptr()=0;

		virtual size_t ElementSize()=0;
		
	};

	template <typename T>
	class  RRArray : public RRBaseArray
	{
	public:		

		virtual DataTypes GetTypeID()
		{
			return RRPrimUtil<T>::GetTypeID();
		}		
		
		RRArray(T* data, size_t length, bool owned)
		{
			this->data=data;
			this->owned=owned;
			this->element_count=length;
		}

		virtual ~RRArray()
		{
			if (owned)
				delete[] data;
		}

		virtual T* ptr()
		{
			return data;
		}

		virtual void* void_ptr()
		{
			return ptr();
		}

		T *operator->() const {
            return data;
        }

		inline T& operator[](size_t pos) const
		{
			if (pos >= element_count)
			{
				throw OutOfRangeException("Index out of range");
			}

			return data[pos];
		}

		
		virtual size_t Length()
		{
			return element_count;
		}

		virtual size_t size()
		{
			return Length();
		}

		virtual size_t ElementSize()
		{
			return sizeof(T);
		}

	private:
		T* data;
		size_t element_count;
		bool owned;
	};



	template <typename T>
	class RRMap_keytype
	{
	public:
		static std::string get ()
		{
			BOOST_STATIC_ASSERT_MSG(sizeof(T) == 0, "Invalid key type for Robot Raconteur map");
			throw DataTypeException("Unknown data type");
		}
	};

	template <>
	class RRMap_keytype<int32_t>
	{
	public:
		static std::string get ()
		{			
			return "int32_t";
		}
	};

	template <>
	class RRMap_keytype<std::string>
	{
	public:
		static std::string get ()
		{
			return "string";
		}
	};

	template<typename K, typename T>
	class RRMap : public RRValue
	{
	public:
		std::map<K,RR_SHARED_PTR<T> > map;

		RRMap() {}

		RRMap(std::map<K,RR_SHARED_PTR<T> > mapin)
		{
			map=mapin;
		}

		virtual ~RRMap() {}

		virtual std::string RRType()
		{
			std::string keytype=RRMap_keytype<K>::get();			

			return "RobotRaconteur.RRMap<" +keytype +">";
		}
	};
		
	template<typename T>
	class RRList : public RRValue
	{
	public:
		std::vector<RR_SHARED_PTR<T> > list;

		RRList() {}

		RRList(std::list<RR_SHARED_PTR<T> > listin)
		{
			list=listin;
		}

		virtual ~RRList() {}

		virtual std::string RRType()
		{
			return "RobotRaconteur.RRList";
		}
	};
		
	class ROBOTRACONTEUR_CORE_API RRStructure : public RRValue
	{
	public:
		virtual ~RRStructure() {}

	};

	template<typename T>
	static RR_SHARED_PTR<RRArray<T> > AllocateRRArray(size_t length)
	{
		T* data=new T[length];
		return RR_MAKE_SHARED<RRArray<T> >(data,length,true);	
	}

	template<typename T>
	static RR_SHARED_PTR<RRArray<T> > AttachRRArray(T* data,size_t length,bool owned)
	{	
		return RR_MAKE_SHARED<RRArray<T> >(data,length,owned);		
	}

	template<typename T>
	static RR_SHARED_PTR<RRArray<T> > AttachRRArrayCopy(const T* data,const size_t length)
	{
		T* data_copy=new T[length];
		memcpy(data_copy,data,length*sizeof(T));
		return RR_MAKE_SHARED<RRArray<T> >(data_copy,length,true);		
	}

	ROBOTRACONTEUR_CORE_API RR_SHARED_PTR<RRBaseArray> AllocateRRArrayByType(DataTypes type, size_t length);


	ROBOTRACONTEUR_CORE_API size_t RRArrayElementSize(DataTypes type);
    
    template<typename T>
	static RR_SHARED_PTR<RRArray<T> > ScalarToRRArray(T value)
	{
		T* data=new T[1];
		data[0]=value;
		return AttachRRArray(data,1,true);
	}
    
	template<typename T>
	static T RRArrayToScalar(RR_SHARED_PTR<RRArray<T> > value)
	{
		if (!value)
		{
			throw NullValueException("Null pointer");
		}
        
		if (value->Length()==0) throw OutOfRangeException("Index out of range");
        
		return (*value)[0];
        
	}

	template <typename Y,typename U>
	static std::vector<Y> RRArrayToVector(RR_SHARED_PTR<RRArray<U> > in)
	{
		if (!in) throw NullValueException("Unexpected null array");
		std::vector<Y> out(in->size());
		for (size_t i=0; i<in->size(); i++) out[i]=(Y)(*in)[i];
		return out;
	}

	template <typename Y,typename U>
	static RR_SHARED_PTR<RRArray<Y> > VectorToRRArray(std::vector<U> in)
	{
		RR_SHARED_PTR<RRArray<Y> > out=AllocateRRArray<Y>(in.size());
		for (size_t i=0; i<in.size(); i++) (*out)[i]=(Y)in[i];
		return out;
	}

	template <typename Y,std::size_t N,typename U>
	static boost::array<Y,N> RRArrayToArray(RR_SHARED_PTR<RRArray<U> > in)
	{
		if (!in) throw NullValueException("Unexpected null array");
		if (in->size()!=N) throw OutOfRangeException("Array is incorrect size");
		boost::array<Y,N> out;
		for (size_t i=0; i<N; i++) out[i]=(Y)(*in)[i];
		return out;
	}

	template <typename Y, std::size_t N, typename U>
	static void RRArrayToArray(boost::array<Y, N>& out, RR_SHARED_PTR<RRArray<U> > in)
	{
		if (!in) throw NullValueException("Unexpected null array");
		if (in->size() != N) throw OutOfRangeException("Array is incorrect size");		
		for (size_t i = 0; i<N; i++) out[i] = (Y)(*in)[i];		
	}

	template <typename Y, typename U,std::size_t N>
	static RR_SHARED_PTR<RRArray<Y> > ArrayToRRArray(boost::array<U,N> in)
	{
		RR_SHARED_PTR<RRArray<Y> > out=AllocateRRArray<Y>(N);
		for (size_t i=0; i<N; i++) (*out)[i]=(Y)in[i];
		return out;
	}

	template <typename Y, std::size_t N, typename U>
	static boost::container::static_vector<Y, N> RRArrayToStaticVector(RR_SHARED_PTR<RRArray<U> > in)
	{
		if (!in) throw NullValueException("Unexpected null array");
		if (in->size() > N) throw OutOfRangeException("Array is too large for static vector size");
		boost::container::static_vector<Y, N> out(in->size());
		for (size_t i = 0; i<in->size(); i++) out[i] = (Y)(*in)[i];
		return out;
	}

	template <typename Y, std::size_t N, typename U>
	static void RRArrayToStaticVector(boost::container::static_vector<Y, N>& out, RR_SHARED_PTR<RRArray<U> > in)
	{
		if (!in) throw NullValueException("Unexpected null array");
		if (in->size() > N) throw OutOfRangeException("Array is too large for static vector size");		
		out.resize(in->size());
		for (size_t i = 0; i<in->size(); i++) out[i] = (Y)(*in)[i];		
	}

	template <typename Y, typename U, std::size_t N>
	static RR_SHARED_PTR<RRArray<Y> > StaticVectorToRRArray(boost::container::static_vector<U, N> in)
	{
		RR_SHARED_PTR<RRArray<Y> > out = AllocateRRArray<Y>(in.size());
		for (size_t i = 0; i<in.size(); i++) (*out)[i] = (Y)in[i];
		return out;
	}

	template<typename T>
	static RR_SHARED_PTR<RRArray<T> > VerifyRRArrayLength(RR_SHARED_PTR<RRArray<T> > a, size_t len, bool varlength)
	{
		if (!a) throw NullValueException("Arrays must not be null");
		if (len != 0)
		{
			if (varlength && (a->size() > len))
			{
				throw DataTypeException("Array dimension mismatch");
			}
			if (!varlength && (a->size() != len))
			{
				throw DataTypeException("Array dimension mismatch");
			}
		}
		return a;
	}

	template<typename T>
	static RR_SHARED_PTR<RRList<T> > VerifyRRArrayLength(RR_SHARED_PTR<RRList<T> > a, size_t len, bool varlength)
	{
		BOOST_FOREACH(RR_SHARED_PTR<T>& aa, a->list)
		{
			VerifyRRArrayLength(aa, len, varlength);
		}
		return a;
	}

	template<typename K, typename T>
	static RR_SHARED_PTR<RRMap<K,T > > VerifyRRArrayLength(RR_SHARED_PTR<RRMap<K,T> > a, size_t len, bool varlength)
	{
		BOOST_FOREACH(RR_SHARED_PTR<T>& aa, a->map | boost::adaptors::map_values)
		{
			VerifyRRArrayLength(aa, len, varlength);
		}
		return a;
	}


	class ROBOTRACONTEUR_CORE_API RRMultiDimBaseArray : public RRValue
	{
	public:
		virtual ~RRMultiDimBaseArray() {}
	};

	namespace detail
	{
		ROBOTRACONTEUR_CORE_API class MultiDimArray_CalculateCopyIndicesIter
		{
		public:
			virtual bool Next(int32_t& indexa, int32_t& indexb, int32_t& len) = 0;

			virtual ~MultiDimArray_CalculateCopyIndicesIter();
		};

		ROBOTRACONTEUR_CORE_API RR_SHARED_PTR<MultiDimArray_CalculateCopyIndicesIter> MultiDimArray_CalculateCopyIndicesBeginIter(int32_t mema_dimcount, const std::vector<int32_t>& mema_dims, const std::vector<int32_t>& mema_pos, int32_t memb_dimcount, const std::vector<int32_t>& memb_dims, const std::vector<int32_t>& memb_pos, const std::vector<int32_t>& count);
	}

	template<typename T>
	class RRMultiDimArray : public RRMultiDimBaseArray
	{
	public:
		int32_t DimCount;
		RR_SHARED_PTR<RRArray<int32_t> > Dims;

		bool Complex;
		RR_SHARED_PTR<RRArray<T> > Real;
		RR_SHARED_PTR<RRArray<T> > Imag;


		RRMultiDimArray() {};

		RRMultiDimArray(RR_SHARED_PTR<RRArray<int32_t> > Dims, RR_SHARED_PTR<RRArray<T> > Real, RR_SHARED_PTR<RRArray<T> > Imag = RR_SHARED_PTR<RRArray<T> >())
		{
			this->Complex = false;
			this->DimCount = (int32_t)Dims->Length();
			this->Dims = Dims;
			this->Real = Real;
			if (Imag)
			{
				this->Complex = true;
				this->Imag = Imag;
			}

		}

		virtual ~RRMultiDimArray() {}

		virtual std::string RRType()
		{
			std::string stype;
			DataTypes type = RRPrimUtil<T>::GetTypeID();
			stype = GetRRDataTypeString(type);
			return "RobotRaconteur.RRMultiDimArray<" + stype + ">";
		}

		virtual void RetrieveSubArray(std::vector<int32_t> memorypos, RR_SHARED_PTR<RRMultiDimArray<T> > buffer, std::vector<int32_t> bufferpos, std::vector<int32_t> count)
		{

			//TODO: put in bounds checks
			if (Complex != buffer->Complex) throw InvalidArgumentException("Complex mismatch");



			std::vector<int32_t> mema_dims = RRArrayToVector<int32_t>(Dims);
			std::vector<int32_t> memb_dims = RRArrayToVector<int32_t>(buffer->Dims);
			RR_SHARED_PTR<detail::MultiDimArray_CalculateCopyIndicesIter> iter = detail::MultiDimArray_CalculateCopyIndicesBeginIter(DimCount, mema_dims, memorypos, buffer->DimCount, memb_dims, bufferpos, count);

			int32_t len;
			int32_t indexa;
			int32_t indexb;

			while (iter->Next(indexa, indexb, len))
			{
				memcpy((buffer->Real->ptr() + indexb), (Real->ptr()) + indexa, len * sizeof(T));
				if (Complex)
				{
					memcpy((buffer->Imag->ptr() + indexb), (Imag->ptr()) + indexa, len * sizeof(T));
				}
			}

		}

		virtual void AssignSubArray(std::vector<int32_t> memorypos, RR_SHARED_PTR<RRMultiDimArray<T> > buffer, std::vector<int32_t> bufferpos, std::vector<int32_t> count)
		{
			//TODO: put in bounds checks
			if (Complex != buffer->Complex) throw InvalidArgumentException("Complex mismatch");

			std::vector<int32_t> mema_dims = RRArrayToVector<int32_t>(Dims);
			std::vector<int32_t> memb_dims = RRArrayToVector<int32_t>(buffer->Dims);
			RR_SHARED_PTR<detail::MultiDimArray_CalculateCopyIndicesIter> iter = detail::MultiDimArray_CalculateCopyIndicesBeginIter(DimCount, mema_dims, memorypos, buffer->DimCount, memb_dims, bufferpos, count);

			int32_t len;
			int32_t indexa;
			int32_t indexb;

			while (iter->Next(indexa, indexb, len))
			{
				memcpy((Real->ptr() + indexa), (buffer->Real->ptr() + indexb), len * sizeof(T));

				if (Complex)
				{
					memcpy(((Imag->ptr()) + indexa), ((buffer->Imag->ptr()) + indexb), len * sizeof(T));
				}
			}

		}

	};

	template<size_t Ndims, typename T>
	static RR_SHARED_PTR<RRMultiDimArray<T> > VerifyRRMultiDimArrayLength(RR_SHARED_PTR<RRMultiDimArray<T> > a, size_t n_elems, boost::array<int32_t,Ndims> dims)
	{
		if (!a) throw NullValueException("Arrays must not be null");

		if (a->Dims->size() != Ndims)
		{
			throw DataTypeException("Array dimension mismatch");
		}

		if (a->Real->size() != n_elems)
		{
			throw DataTypeException("Array dimension mismatch");
		}

		if (a->Imag)
		{
			throw DataTypeException("Fixed size arrays must not be complex");
		}

		for (size_t i = 0; i < Ndims; i++)
		{
			if ((*a->Dims)[i] != dims[i])
			{
				throw DataTypeException("Array dimension mismatch");
			}
		}
				
		return a;
	}

	template<size_t Ndims, typename T>
	static RR_SHARED_PTR<RRList<T> > VerifyRRMultiDimArrayLength(RR_SHARED_PTR<RRList<T> > a, size_t n_elems, boost::array<int32_t, Ndims> dims)
	{
		BOOST_FOREACH(RR_SHARED_PTR<T>& aa, a->list)
		{
			VerifyRRMultiDimArrayLength<Ndims>(aa, n_elems, dims);
		}
		return a;
	}

	template<size_t Ndims, typename K, typename T>
	static RR_SHARED_PTR<RRMap<K,T> > VerifyRRMultiDimArrayLength(RR_SHARED_PTR<RRMap<K,T> > a, size_t n_elems, boost::array<int32_t, Ndims> dims)
	{
		BOOST_FOREACH(RR_SHARED_PTR<T>& aa, a->map | boost::adaptors::map_values)
		{
			VerifyRRMultiDimArrayLength<Ndims>(aa, n_elems, dims);
		}
		return a;
	}

	class ROBOTRACONTEUR_CORE_API RRCStructure
	{
	public:
		
	};

	class RRCStructureBaseArray : public RRValue
	{
	public:
		virtual std::string RRElementTypeString() = 0;
	};

	template<typename T>
	class RRCStructureArray : public RRCStructureBaseArray
	{
	public:
		typename std::vector<T> cstruct_array;

		RRCStructureArray() {}

		RRCStructureArray(typename std::vector<T>& array_in)
		{
			cstruct_array = array_in;
		}

		RRCStructureArray(const T& value_in)
		{
			cstruct_array.push_back(value_in);
		}

		virtual ~RRCStructureArray() {}

		virtual std::string RRType()
		{
			return "RobotRaconteur.RRCStructureArray";
		}

		virtual std::string RRElementTypeString()
		{
			return RRPrimUtil<T>::GetElementTypeString();
		}
	};

#define RRPrimUtilCStructure(x,type_string) \
	template<> class RRPrimUtil<x> \
	{ \
		public: \
		static DataTypes GetTypeID() {return DataTypes_cstructure_t;}  \
		static std::string GetElementTypeString() {return type_string; } \
		static RR_SHARED_PTR<RRCStructureArray<x> > PrePack(const x& val) {return ScalarToRRCStructureArray(val);}\
		template<typename U> \
		static x PreUnpack(const U& val) {return RRArrayToScalar(rr_cast<RRCStructureArray<x> >(val));} \
		typedef RR_SHARED_PTR<RRCStructureArray<x> > BoxedType; \
	};

	class RRCStructureBaseMultiDimArray : public RRValue
	{
	public:
		RR_SHARED_PTR<RRArray<int32_t> > Dims;
		virtual std::string RRElementTypeString() = 0;
	};

	template<typename T>
	class RRCStructureMultiDimArray : public RRCStructureBaseMultiDimArray
	{
	public:
		
		typename RR_SHARED_PTR<RRCStructureArray<T> > CStructArray;

		virtual ~RRCStructureMultiDimArray() {}

		virtual std::string RRType()
		{
			return "RobotRaconteur.RRCStructureMultiDimArray";
		}

		virtual std::string RRElementTypeString()
		{
			return RRPrimUtil<T>::GetElementTypeString();
		}

		virtual void RetrieveSubArray(std::vector<int32_t> memorypos, RR_SHARED_PTR<RRCStructureMultiDimArray<T> > buffer, std::vector<int32_t> bufferpos, std::vector<int32_t> count)
		{

			std::vector<int32_t> mema_dims = RRArrayToVector<int32_t>(Dims);
			std::vector<int32_t> memb_dims = RRArrayToVector<int32_t>(buffer->Dims);
			RR_SHARED_PTR<detail::MultiDimArray_CalculateCopyIndicesIter> iter = detail::MultiDimArray_CalculateCopyIndicesBeginIter(Dims->size(), mema_dims, memorypos, buffer->Dims->size(), memb_dims, bufferpos, count);

			int32_t len;
			int32_t indexa;
			int32_t indexb;

			while (iter->Next(indexa, indexb, len))
			{
				for (size_t i = 0; i < len; i++)
				{
					buffer->CStructArray->cstruct_array.at(indexb + i) = CStructArray->cstruct_array.at(indexa + i);
				}				
			}

		}

		virtual void AssignSubArray(std::vector<int32_t> memorypos, RR_SHARED_PTR<RRCStructureMultiDimArray<T> > buffer, std::vector<int32_t> bufferpos, std::vector<int32_t> count)
		{
			
			std::vector<int32_t> mema_dims = RRArrayToVector<int32_t>(Dims);
			std::vector<int32_t> memb_dims = RRArrayToVector<int32_t>(buffer->Dims);
			RR_SHARED_PTR<detail::MultiDimArray_CalculateCopyIndicesIter> iter = detail::MultiDimArray_CalculateCopyIndicesBeginIter(Dims->size(), mema_dims, memorypos, buffer->Dims->size(), memb_dims, bufferpos, count);

			int32_t len;
			int32_t indexa;
			int32_t indexb;

			while (iter->Next(indexa, indexb, len))
			{
				for (size_t i = 0; i < len; i++)
				{
					CStructArray->cstruct_array.at(indexa + i) = buffer->CStructArray->cstruct_array.at(indexb + i);
				}
			}

		}
	};

	template<typename T>
	static RR_SHARED_PTR<RRCStructureArray<T> > ScalarToRRCStructureArray(const T& value)
	{
		return RR_MAKE_SHARED<RRCStructureArray<T> >(value);
	}

	template<typename T>
	static T RRCStructureArrayToScalar(RR_SHARED_PTR<RRCStructureArray<T> > value)
	{
		if (!value)
		{
			throw NullValueException("Null pointer");
		}

		if (value->cstruct_array.size() == 0) throw OutOfRangeException("Index out of range");

		return value->cstruct_array.at(0);

	}

	class ROBOTRACONTEUR_CORE_API RobotRaconteurNode;

	class ROBOTRACONTEUR_CORE_API TimeSpec
	{
	public:
		int64_t seconds;
		int32_t nanoseconds;

		TimeSpec();

		TimeSpec(int64_t seconds, int32_t nanoseconds);

		static TimeSpec Now();

		static TimeSpec Now(RR_SHARED_PTR<RobotRaconteurNode> node);

	public:
		bool operator == (const TimeSpec &t2);

		bool operator != (const TimeSpec &t2);

		TimeSpec operator - (const TimeSpec &t2);

		TimeSpec operator + (const TimeSpec &t2);

		bool operator > (const TimeSpec &t2);

		bool operator >= (const TimeSpec &t2);

		bool operator < (const TimeSpec &t2);

		bool operator <= (const TimeSpec &t2);

	public:

		void cleanup_nanosecs();

	};

#ifdef ROBOTRACONTEUR_USE_SMALL_VECTOR
	typedef boost::container::small_vector<boost::asio::const_buffer, 4> const_buffers;
	typedef boost::container::small_vector<boost::asio::mutable_buffer, 4> mutable_buffers;
#else
	typedef std::vector<boost::asio::const_buffer> const_buffers;
	typedef std::vector<boost::asio::mutable_buffer> mutable_buffers;
#endif

	template<typename BufferSequence>
	void buffers_consume(BufferSequence& b, size_t count)
	{		
		typename BufferSequence::iterator e = b.begin();;
		typename BufferSequence::iterator end = b.end();
		for (; e != end;)
		{
			if (count > 0)
			{
				if (count > boost::asio::buffer_size(*e))
				{
					count -= boost::asio::buffer_size(*e);
					e = b.erase(e);
				}
				else
				{
					*e = *e + count;					
					count = 0;
					++e;
				}
			}
			else
			{
				break;
			}
		}		
	}


	template<typename BufferSequence>
	BufferSequence buffers_consume_copy(BufferSequence& b, size_t count)
	{
		BufferSequence o;
		typename BufferSequence::iterator e = b.begin();;
		typename BufferSequence::iterator end = b.end();
		for (; e != end; e++)
		{
			if (count > 0)
			{
				if (count > boost::asio::buffer_size(*e))
				{
					count -= boost::asio::buffer_size(*e);
				}
				else
				{
					o.push_back(*e + count);
					count = 0;
				}
			}
			else
			{
				o.push_back(*e);
			}
		}

		return o;
	}

	template<typename BufferSequence>
	BufferSequence buffers_consume_copy(const BufferSequence& b, size_t count)
	{
		BufferSequence o;
		typename BufferSequence::const_iterator e = b.begin();;
		typename BufferSequence::const_iterator end = b.end();
		for (; e != end; e++)
		{
			if (count > 0)
			{
				if (count > boost::asio::buffer_size(*e))
				{
					count -= boost::asio::buffer_size(*e);
				}
				else
				{
					o.push_back(*e + count);
					count = 0;
				}
			}
			else
			{
				o.push_back(*e);
			}
		}

		return o;
	}

	template<typename BufferSequence>
	BufferSequence buffers_truncate(BufferSequence& b, size_t count)
	{
		BufferSequence o;
		typename BufferSequence::iterator e = b.begin();;
		typename BufferSequence::iterator end = b.end();
		size_t p = 0;
		for (; e != end; e++)
		{
			if (p + boost::asio::buffer_size(*e) <= count)
			{
				o.push_back(*e);
				p += boost::asio::buffer_size(*e);
			}
			else
			{
				o.push_back(boost::asio::buffer(*e, count - p));
				break;
			}
		}

		return o;
	}

	template<typename BufferSequence>
	BufferSequence buffers_truncate(const BufferSequence& b, size_t count)
	{
		BufferSequence o;
		typename BufferSequence::const_iterator e = b.begin();;
		typename BufferSequence::const_iterator end = b.end();
		size_t p = 0;
		for (; e != end; e++)
		{
			if (p + boost::asio::buffer_size(*e) <= count)
			{
				o.push_back(*e);
				p += boost::asio::buffer_size(*e);
			}
			else
			{
				o.push_back(boost::asio::buffer(*e, count - p));
				break;
			}
		}

		return o;
	}

	namespace detail
	{
		ROBOTRACONTEUR_CORE_API std::string encode_index(const std::string& index);

		ROBOTRACONTEUR_CORE_API std::string decode_index(const std::string& index);
	}
}
