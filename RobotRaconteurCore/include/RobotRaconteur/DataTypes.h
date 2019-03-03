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
#include <boost/range/numeric.hpp>

#include <boost/smart_ptr/intrusive_ref_counter.hpp>

#pragma once

#define RR_NULL_CHECK(ptr) {if (!ptr) throw NullValueException("Null pointer");}

namespace RobotRaconteur
{
	struct cdouble
	{
		double real; double imag;
		cdouble() : real(0.0), imag(0.0) {}
		cdouble(double r, double i) : real(r), imag(i) {}
	};
	struct cfloat
	{
		float real; float imag;
		cfloat() : real(0.0), imag(0.0) {}
		cfloat(float r, float i) : real(r), imag(i) {}
	};
	struct rr_bool
	{
		uint8_t value;
		rr_bool() : value(0) {}
		rr_bool(uint8_t b) : value(b) {}
	};
	
	bool operator== (const cdouble &c1, const cdouble &c2);
	bool operator!= (const cdouble &c1, const cdouble &c2);
	bool operator== (const cfloat &c1, const cfloat &c2);
	bool operator!= (const cfloat &c1, const cfloat &c2);
	bool operator== (const rr_bool &c1, const rr_bool &c2);
	bool operator!= (const rr_bool &c1, const rr_bool &c2);
	
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

		static RR_INTRUSIVE_PTR<T> rr_cast(const RR_INTRUSIVE_PTR<U>& objin)
		{
			if (!objin) return RR_INTRUSIVE_PTR<T>();

			RR_INTRUSIVE_PTR<T> c = RR_DYNAMIC_POINTER_CAST<T>(objin);
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

		static const RR_INTRUSIVE_PTR<T>& rr_cast(const RR_INTRUSIVE_PTR<T>& objin)
		{
			return objin;
		}
	};

	template<typename T, typename U> static RR_SHARED_PTR<T> rr_cast(const RR_SHARED_PTR<U>& objin)
	{
		return rr_cast_support<T, U>::rr_cast(objin);
	}

	template<typename T, typename U> static RR_INTRUSIVE_PTR<T> rr_cast(const RR_INTRUSIVE_PTR<U>& objin)
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

	class ROBOTRACONTEUR_CORE_API RRValue : public boost::intrusive_ref_counter<RRValue>, boost::noncopyable
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
	static RR_INTRUSIVE_PTR<RRArray<T> > ScalarToRRArray(T value);

	template<typename T>
	static T RRArrayToScalar(RR_INTRUSIVE_PTR<RRArray<T> > value);


	ROBOTRACONTEUR_CORE_API RR_INTRUSIVE_PTR<RRArray<char> > stringToRRArray(const std::string& str);

	ROBOTRACONTEUR_CORE_API std::string RRArrayToString(RR_INTRUSIVE_PTR<RRArray<char> > arr);


	

#ifdef ROBOTRACONTEUR_USE_WSTRING
	ROBOTRACONTEUR_CORE_API RR_INTRUSIVE_PTR<RRArray<char> > wstringToRRArray(const std::wstring& str);

	ROBOTRACONTEUR_CORE_API std::wstring RRArrayToWString(RR_INTRUSIVE_PTR<RRArray<char> > arr);

	// Convert a wide Unicode string to an UTF8 string
	ROBOTRACONTEUR_CORE_API std::string utf8_encode(const std::wstring &wstr);

	// Convert an UTF8 string to a wide Unicode String
	ROBOTRACONTEUR_CORE_API std::wstring utf8_decode(const std::string &str);
#endif


	template <typename T> class RRPrimUtil  
	{ 
	public:
		static DataTypes GetTypeID() {return DataTypes_void_t;} 		
		
		static RR_INTRUSIVE_PTR<T> PrePack(const T& val) {return val;} 
		
		template <typename U>
		static T PreUnpack(U& val) {return rr_cast<T>(val);} 

		virtual ~RRPrimUtil() {};

		typedef RR_INTRUSIVE_PTR<RRValue> BoxedType;
	};
	
	template <typename T> class RRPrimUtil<RR_INTRUSIVE_PTR<T> >
	{ 
	public:
		static DataTypes GetTypeID() {return DataTypes_void_t;}
		static std::string GetElementTypeString() {
			return "";
		}
		
		static RR_INTRUSIVE_PTR<RRValue> PrePack(const RR_INTRUSIVE_PTR<T>& val) {return rr_cast<RRValue>(val);}
		
		template<typename U>
		static RR_INTRUSIVE_PTR<T> PreUnpack(const U& val) {return rr_cast<T>(val);} 
		
		virtual ~RRPrimUtil() {};

		typedef RR_INTRUSIVE_PTR<T> BoxedType;
	};

	template<> class RRPrimUtil<std::string>
	{
	public:
		static DataTypes GetTypeID() {return DataTypes_string_t;}
		static std::string GetElementTypeString() {
			return "";
		}
		static RR_INTRUSIVE_PTR<RRValue> PrePack(const std::string& val) {return rr_cast<RRValue>(stringToRRArray(val));} 
		static std::string PreUnpack(RR_INTRUSIVE_PTR<RRValue> val) {return RRArrayToString(rr_cast<RRArray<char> >(val));} 

		virtual ~RRPrimUtil() {};

		typedef RR_INTRUSIVE_PTR<RRArray<char> > BoxedType;

	};

#ifdef ROBOTRACONTEUR_USE_WSTRING

	template<> class RRPrimUtil<std::wstring>
	{
	public:
		static DataTypes GetTypeID() {return DataTypes_string_t;} 
		static RR_INTRUSIVE_PTR<RRValue> PrePack(std::wstring val) {return rr_cast<RRValue>(wstringToRRArray(val));} 
		static std::wstring PreUnpack(RR_INTRUSIVE_PTR<RRValue> val) {return RRArrayToWString(rr_cast<RRArray<char> >(val));} 

		typedef RR_SHARED_PTR<RRArary<wchar_t> > BoxedType;
	};
#endif

#define RRPrimUtilNumeric(x,code) \
	template<> class RRPrimUtil<x> \
	{ \
		public: \
		static DataTypes GetTypeID() {return code;}  \
		static std::string GetElementTypeString() {return ""; } \
		static RR_INTRUSIVE_PTR<RRArray<x> > PrePack(const x& val) {return ScalarToRRArray(val);}\
		template<typename U> \
		static x PreUnpack(const U& val) {return RRArrayToScalar(rr_cast<RRArray<x> >(val));} \
		typedef RR_INTRUSIVE_PTR<RRArray<x> > BoxedType; \
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
	RRPrimUtilNumeric(cdouble, DataTypes_cdouble_t);
	RRPrimUtilNumeric(cfloat, DataTypes_csingle_t);
	RRPrimUtilNumeric(rr_bool, DataTypes_bool_t);
	
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

		virtual size_t size()=0;
		
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
			this->data_=data;
			this->owned=owned;
			this->element_count=length;
		}

		virtual ~RRArray()
		{
			if (owned)
				delete[] data_;
		}
				
		virtual void* void_ptr()
		{
			return data();
		}

		T *operator->() const {
            return data;
        }
								
		virtual size_t size()
		{
			return element_count;
		}

		virtual size_t ElementSize()
		{
			return sizeof(T);
		}

		// C++ container support functions based on boost::array

		// type definitions
		typedef T              value_type;
		typedef T*             iterator;
		typedef const T*       const_iterator;
		typedef T&             reference;
		typedef const T&       const_reference;
		typedef std::size_t    size_type;
		typedef std::ptrdiff_t difference_type;

		// iterator support
		iterator        begin() { return data_; }
		const_iterator  begin() const { return data_; }
		const_iterator cbegin() const { return data_; }

		iterator        end() { return data_ + element_count; }
		const_iterator  end() const { return data_ + element_count; }
		const_iterator cend() const { return data_ + element_count; }
		typedef boost::reverse_iterator<iterator> reverse_iterator;
		typedef boost::reverse_iterator<const_iterator> const_reverse_iterator;

		reverse_iterator rbegin() { return reverse_iterator(end()); }
		const_reverse_iterator rbegin() const {
			return const_reverse_iterator(end());
		}
		const_reverse_iterator crbegin() const {
			return const_reverse_iterator(end());
		}

		reverse_iterator rend() { return reverse_iterator(begin()); }
		const_reverse_iterator rend() const {
			return const_reverse_iterator(begin());
		}
		const_reverse_iterator crend() const {
			return const_reverse_iterator(begin());
		}

		// operator[]
		reference operator[](size_type i)
		{
			BOOST_ASSERT_MSG(i < element_count, "out of range");
			return data_[i];
		}

		const_reference operator[](size_type i) const
		{
			BOOST_ASSERT_MSG(i < element_count, "out of range");
			return data_[i];
		}

		// at() with range check
		reference at(size_type i) { rangecheck(i); return data_[i]; }
		const_reference at(size_type i) const { rangecheck(i); return data_[i]; }

		// front() and back()
		reference front()
		{
			return data_[0];
		}

		const_reference front() const
		{
			return data_[0];
		}

		reference back()
		{
			return data_[element_count - 1];
		}

		const_reference back() const
		{
			return data_[element_count - 1];
		}

		bool empty() { return false; }
		size_type max_size() { return element_count; }

		// direct access to data (read-only)
		const T* data() const { return data_; }
		T* data() { return data_; }

		// use array as C array (direct read/write access to data)
		T* c_array() { return data_; }

		// assignment with type conversion
		template <typename T2>
		RRArray<T>& operator= (const RRArray<T2>& rhs) {
			std::copy(rhs.begin(), rhs.end(), begin());
			return *this;
		}

		// assign one value to all elements
		void assign(const T& value) { fill(value); }    // A synonym for fill
		void fill(const T& value)
		{
			std::fill_n(begin(), size(), value);
		}
				
		void rangecheck(size_type i) {
			if (i >= size()) {
				std::out_of_range e("array<>: index out of range");
				boost::throw_exception(e);
			}
		}

	private:
		T* data_;
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
	protected:
		std::map<K,RR_INTRUSIVE_PTR<T> > map;

	public:

		RRMap() {}

		RRMap(std::map<K,RR_INTRUSIVE_PTR<T> > mapin)
		{
			map=mapin;
		}

		virtual ~RRMap() {}

		virtual std::string RRType()
		{
			std::string keytype=RRMap_keytype<K>::get();			

			return "RobotRaconteur.RRMap<" +keytype +">";
		}

		// C++ container support based on boost::container::map

		typedef typename std::map<K, RR_INTRUSIVE_PTR<T> >::key_type                   key_type;
		typedef typename std::map<K, RR_INTRUSIVE_PTR<T> >::mapped_type                mapped_type;
		typedef typename std::map<K, RR_INTRUSIVE_PTR<T> >::value_type                 value_type;
		typedef typename std::map<K, RR_INTRUSIVE_PTR<T> >::pointer                    pointer;
		typedef typename std::map<K, RR_INTRUSIVE_PTR<T> >::const_pointer              const_pointer;
		typedef typename std::map<K, RR_INTRUSIVE_PTR<T> >::reference                  reference;
		typedef typename std::map<K, RR_INTRUSIVE_PTR<T> >::const_reference            const_reference;
		typedef typename std::map<K, RR_INTRUSIVE_PTR<T> >::size_type                  size_type;
		typedef typename std::map<K, RR_INTRUSIVE_PTR<T> >::iterator                   iterator;
		typedef typename std::map<K, RR_INTRUSIVE_PTR<T> >::const_iterator             const_iterator;
		typedef typename std::map<K, RR_INTRUSIVE_PTR<T> >::reverse_iterator           reverse_iterator;
		typedef typename std::map<K, RR_INTRUSIVE_PTR<T> >::const_reverse_iterator     const_reverse_iterator;

		iterator begin() { return map.begin(); }
		const_iterator begin() const { return map.begin(); }
		iterator end() { return map.end(); }
		const_iterator end() const { return map.end(); }
		reverse_iterator rbegin() { return map.rbegin(); };
		const_reverse_iterator rbegin() const { return map.rbegin(); };
		reverse_iterator rend() { return map.rend(); }
		const_reverse_iterator rend() const { return map.rend(); }
		bool empty() const { return map.empty(); }
		size_type size() const { return map.size(); }
		size_type max_size() const { return map.max_size(); };
		mapped_type& operator[](const key_type &k) { return map[k]; }
		mapped_type& at(const key_type& k) { return map.at(k); }
		const mapped_type& at(const key_type& k) const { return map.at(k); }
		std::pair<iterator, bool> insert(const value_type& x) { return map.insert(x); }
		iterator insert(const_iterator p, const value_type& x) { return map.insert(x); }
		template <class InputIterator>
		void insert(InputIterator first, InputIterator last) { map.insert(first,last); }
		void erase(iterator p) { map.erase(p); }
		size_type erase(const key_type& x) { return map.erase(x); }
		void erase(iterator first, iterator last) { map.erase(first,last); }
		void clear() { map.clear(); }
		iterator find(const key_type& x) { return map.find(x); }
		const_iterator find(const key_type& x) const { return map.find(x); }
		size_type count(const key_type& x) const { return map.count(x); }

		// WARNING: this may change, use with caution!
		typename std::map<K, RR_INTRUSIVE_PTR<T> >& GetStorageContainer()
		{
			return map;
		}
	};
		
	template<typename T>
	class RRList : public RRValue
	{
	protected:

		std::list<RR_INTRUSIVE_PTR<T> > list;

	public:

		RRList() {}

		RRList(std::list<RR_INTRUSIVE_PTR<T> > listin)
		{
			list=listin;
		}

		virtual ~RRList() {}

		virtual std::string RRType()
		{
			return "RobotRaconteur.RRList";
		}

		// C++ container support based on boost::container::list

		typedef RR_INTRUSIVE_PTR<T>                                         value_type;
		typedef typename std::list<RR_INTRUSIVE_PTR<T> >::pointer           pointer;
		typedef typename std::list<RR_INTRUSIVE_PTR<T> >::const_pointer     const_pointer;
		typedef typename std::list<RR_INTRUSIVE_PTR<T> >::reference         reference;
		typedef typename std::list<RR_INTRUSIVE_PTR<T> >::const_reference   const_reference;
		typedef typename std::list<RR_INTRUSIVE_PTR<T> >::size_type         size_type;
		typedef typename std::list<RR_INTRUSIVE_PTR<T> >::difference_type   difference_type;
		typedef typename std::list<RR_INTRUSIVE_PTR<T> >::iterator                   iterator;
		typedef typename std::list<RR_INTRUSIVE_PTR<T> >::const_iterator             const_iterator;
		typedef typename std::list<RR_INTRUSIVE_PTR<T> >::reverse_iterator           reverse_iterator;
		typedef typename std::list<RR_INTRUSIVE_PTR<T> >::const_reverse_iterator     const_reverse_iterator;

		iterator begin() { return list.begin(); }
		const_iterator begin() const { return list.begin(); }
		iterator end() { return list.end(); }
		const_iterator end() const { return list.end(); }
		reverse_iterator rbegin() { return list.rbegin(); }
		const_reverse_iterator rbegin() const { return list.rbegin(); }
		reverse_iterator rend() { return list.rend(); }
		const_reverse_iterator rend() const { return list.rend(); }
		const_iterator cbegin() const { return list.cbegin(); }
		const_iterator cend() const { return list.cend(); }
		const_reverse_iterator const crbegin() { return list.crbegin(); }
		const_reverse_iterator const crend() { return list.crend(); }
		bool empty() const { return list.empty(); }
		virtual size_type size() const { return list.size(); }
		size_type max_size() const { return list.max_size(); }
		reference front() { return list.front(); }
		const_reference front() const { return list.front(); }
		reference back() { return list.back(); }
		const_reference back() const { return list.back(); }
		void push_front(const RR_INTRUSIVE_PTR<T> &x) { list.push_front(x); }
		void push_back(const RR_INTRUSIVE_PTR<T> &x) { list.push_back(x); }
		void pop_front() { list.pop_front(); }
		void pop_back() { list.pop_back(); }
		iterator insert(const_iterator p, const RR_INTRUSIVE_PTR<T> &x) { list.insert(p, x); }
		iterator erase(const_iterator p) { list.erase(p); }
		iterator erase(const_iterator first, const_iterator last) { list.erase(first, last); }
		void clear() { list.clear(); }
		void remove(const RR_INTRUSIVE_PTR<T>& value) { list.remove(value); }

		// WARNING: this may change, use with caution!
		typename std::list<RR_INTRUSIVE_PTR<T> >& GetStorageContainer()
		{
			return list;
		}

	};
		
	class ROBOTRACONTEUR_CORE_API RRStructure : public RRValue
	{
	public:
		virtual ~RRStructure() {}

	};

	template<typename T>
	static RR_INTRUSIVE_PTR<RRArray<T> > AllocateRRArray(size_t length)
	{
		T* data=new T[length];
		return RR_INTRUSIVE_PTR<RRArray<T> >(new RRArray<T>(data,length,true));	
	}

	template<typename T>
	static RR_INTRUSIVE_PTR<RRArray<T> > AttachRRArray(T* data,size_t length,bool owned)
	{	
		return RR_INTRUSIVE_PTR<RRArray<T> >(new RRArray<T>(data,length,owned));		
	}

	template<typename T>
	static RR_INTRUSIVE_PTR<RRArray<T> > AttachRRArrayCopy(const T* data,const size_t length)
	{
		T* data_copy=new T[length];
		memcpy(data_copy,data,length*sizeof(T));
		return RR_INTRUSIVE_PTR<RRArray<T> >(new RRArray<T>(data_copy,length,true));		
	}

	ROBOTRACONTEUR_CORE_API RR_INTRUSIVE_PTR<RRBaseArray> AllocateRRArrayByType(DataTypes type, size_t length);

	template<typename T>
	static RR_INTRUSIVE_PTR<RRArray<T> > AllocateEmptyRRArray(size_t length)
	{
		RR_INTRUSIVE_PTR<RRArray<T> > o = AllocateRRArray<T>(length);
		if (length > 0)
		{
			memset(o->data(), 0, length * sizeof(T));
		}
		return o;
	}
		
	ROBOTRACONTEUR_CORE_API size_t RRArrayElementSize(DataTypes type);
    
    template<typename T>
	static RR_INTRUSIVE_PTR<RRArray<T> > ScalarToRRArray(T value)
	{
		T* data=new T[1];
		data[0]=value;
		return AttachRRArray(data,1,true);
	}
    
	template<typename T>
	static T RRArrayToScalar(RR_INTRUSIVE_PTR<RRArray<T> > value)
	{
		if (!value)
		{
			throw NullValueException("Null pointer");
		}
        
		if (value->size()==0) throw OutOfRangeException("Index out of range");
        
		return (*value)[0];
        
	}

	template <typename Y,typename U>
	static std::vector<Y> RRArrayToVector(RR_INTRUSIVE_PTR<RRArray<U> > in)
	{
		if (!in) throw NullValueException("Unexpected null array");
		std::vector<Y> out(in->size());
		for (size_t i=0; i<in->size(); i++) out[i]=(Y)(*in)[i];
		return out;
	}

	template <typename Y,typename U>
	static RR_INTRUSIVE_PTR<RRArray<Y> > VectorToRRArray(std::vector<U> in)
	{
		RR_INTRUSIVE_PTR<RRArray<Y> > out=AllocateRRArray<Y>(in.size());
		for (size_t i=0; i<in.size(); i++) (*out)[i]=(Y)in[i];
		return out;
	}

	template <typename Y,std::size_t N,typename U>
	static boost::array<Y,N> RRArrayToArray(RR_INTRUSIVE_PTR<RRArray<U> > in)
	{
		if (!in) throw NullValueException("Unexpected null array");
		if (in->size()!=N) throw OutOfRangeException("Array is incorrect size");
		boost::array<Y,N> out;
		for (size_t i=0; i<N; i++) out[i]=(Y)(*in)[i];
		return out;
	}

	template <typename Y, std::size_t N, typename U>
	static void RRArrayToArray(boost::array<Y, N>& out, RR_INTRUSIVE_PTR<RRArray<U> > in)
	{
		if (!in) throw NullValueException("Unexpected null array");
		if (in->size() != N) throw OutOfRangeException("Array is incorrect size");		
		for (size_t i = 0; i<N; i++) out[i] = (Y)(*in)[i];		
	}

	template <typename Y, typename U,std::size_t N>
	static RR_INTRUSIVE_PTR<RRArray<Y> > ArrayToRRArray(boost::array<U,N> in)
	{
		RR_INTRUSIVE_PTR<RRArray<Y> > out=AllocateRRArray<Y>(N);
		for (size_t i=0; i<N; i++) (*out)[i]=(Y)in[i];
		return out;
	}

	template <typename Y, std::size_t N, typename U>
	static boost::container::static_vector<Y, N> RRArrayToStaticVector(RR_INTRUSIVE_PTR<RRArray<U> > in)
	{
		if (!in) throw NullValueException("Unexpected null array");
		if (in->size() > N) throw OutOfRangeException("Array is too large for static vector size");
		boost::container::static_vector<Y, N> out(in->size());
		for (size_t i = 0; i<in->size(); i++) out[i] = (Y)(*in)[i];
		return out;
	}

	template <typename Y, std::size_t N, typename U>
	static void RRArrayToStaticVector(boost::container::static_vector<Y, N>& out, RR_INTRUSIVE_PTR<RRArray<U> > in)
	{
		if (!in) throw NullValueException("Unexpected null array");
		if (in->size() > N) throw OutOfRangeException("Array is too large for static vector size");		
		out.resize(in->size());
		for (size_t i = 0; i<in->size(); i++) out[i] = (Y)(*in)[i];		
	}

	template <typename Y, typename U, std::size_t N>
	static RR_INTRUSIVE_PTR<RRArray<Y> > StaticVectorToRRArray(boost::container::static_vector<U, N> in)
	{
		RR_INTRUSIVE_PTR<RRArray<Y> > out = AllocateRRArray<Y>(in.size());
		for (size_t i = 0; i<in.size(); i++) (*out)[i] = (Y)in[i];
		return out;
	}

	template<typename T>
	static RR_INTRUSIVE_PTR<RRArray<T> > VerifyRRArrayLength(RR_INTRUSIVE_PTR<RRArray<T> > a, size_t len, bool varlength)
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
	static RR_INTRUSIVE_PTR<RRList<T> > VerifyRRArrayLength(RR_INTRUSIVE_PTR<RRList<T> > a, size_t len, bool varlength)
	{
		if (!a) return a;
		BOOST_FOREACH(RR_INTRUSIVE_PTR<T>& aa, (*a))
		{
			VerifyRRArrayLength(aa, len, varlength);
		}
		return a;
	}

	template<typename K, typename T>
	static RR_INTRUSIVE_PTR<RRMap<K,T > > VerifyRRArrayLength(RR_INTRUSIVE_PTR<RRMap<K,T> > a, size_t len, bool varlength)
	{
		if (!a) return a;
		BOOST_FOREACH(RR_INTRUSIVE_PTR<T>& aa, *a | boost::adaptors::map_values)
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
			virtual bool Next(uint32_t& indexa, uint32_t& indexb, uint32_t& len) = 0;

			virtual ~MultiDimArray_CalculateCopyIndicesIter();
		};

		ROBOTRACONTEUR_CORE_API RR_SHARED_PTR<MultiDimArray_CalculateCopyIndicesIter> MultiDimArray_CalculateCopyIndicesBeginIter(const std::vector<uint32_t>& mema_dims, const std::vector<uint32_t>& mema_pos, const std::vector<uint32_t>& memb_dims, const std::vector<uint32_t>& memb_pos, const std::vector<uint32_t>& count);
	}

	template<typename T>
	class RRMultiDimArray : public RRMultiDimBaseArray
	{
	public:
		
		RR_INTRUSIVE_PTR<RRArray<uint32_t> > Dims;
				
		RR_INTRUSIVE_PTR<RRArray<T> > Array;
		
		RRMultiDimArray() {};

		RRMultiDimArray(RR_INTRUSIVE_PTR<RRArray<uint32_t> > Dims, RR_INTRUSIVE_PTR<RRArray<T> > Array)
		{			
			this->Dims = Dims;
			this->Array = Array;
		}

		virtual ~RRMultiDimArray() {}

		virtual std::string RRType()
		{
			std::string stype;
			DataTypes type = RRPrimUtil<T>::GetTypeID();
			stype = GetRRDataTypeString(type);
			return "RobotRaconteur.RRMultiDimArray<" + stype + ">";
		}

		virtual void RetrieveSubArray(std::vector<uint32_t> memorypos, RR_INTRUSIVE_PTR<RRMultiDimArray<T> > buffer, std::vector<uint32_t> bufferpos, std::vector<uint32_t> count)
		{
						
			std::vector<uint32_t> mema_dims = RRArrayToVector<uint32_t>(Dims);
			std::vector<uint32_t> memb_dims = RRArrayToVector<uint32_t>(buffer->Dims);
			RR_SHARED_PTR<detail::MultiDimArray_CalculateCopyIndicesIter> iter = detail::MultiDimArray_CalculateCopyIndicesBeginIter(mema_dims, memorypos, memb_dims, bufferpos, count);

			uint32_t len;
			uint32_t indexa;
			uint32_t indexb;

			while (iter->Next(indexa, indexb, len))
			{				
				memcpy((buffer->Array->data() + indexb), (Array->data()) + indexa, len * sizeof(T));				
			}

		}

		virtual void AssignSubArray(std::vector<uint32_t> memorypos, RR_INTRUSIVE_PTR<RRMultiDimArray<T> > buffer, std::vector<uint32_t> bufferpos, std::vector<uint32_t> count)
		{
						
			std::vector<uint32_t> mema_dims = RRArrayToVector<uint32_t>(Dims);
			std::vector<uint32_t> memb_dims = RRArrayToVector<uint32_t>(buffer->Dims);
			RR_SHARED_PTR<detail::MultiDimArray_CalculateCopyIndicesIter> iter = detail::MultiDimArray_CalculateCopyIndicesBeginIter(mema_dims, memorypos, memb_dims, bufferpos, count);

			uint32_t len;
			uint32_t indexa;
			uint32_t indexb;

			while (iter->Next(indexa, indexb, len))
			{
				memcpy((Array->data() + indexa), (buffer->Array->data() + indexb), len * sizeof(T));
								
			}

		}

	};

	template<size_t Ndims, typename T>
	static RR_INTRUSIVE_PTR<RRMultiDimArray<T> > VerifyRRMultiDimArrayLength(RR_INTRUSIVE_PTR<RRMultiDimArray<T> > a, size_t n_elems, boost::array<uint32_t,Ndims> dims)
	{
		if (!a) throw NullValueException("Arrays must not be null");

		if (a->Dims->size() != Ndims)
		{
			throw DataTypeException("Array dimension mismatch");
		}

		if (a->Array->size() != n_elems)
		{
			throw DataTypeException("Array dimension mismatch");
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
	static RR_INTRUSIVE_PTR<RRList<T> > VerifyRRMultiDimArrayLength(RR_INTRUSIVE_PTR<RRList<T> > a, size_t n_elems, boost::array<uint32_t, Ndims> dims)
	{
		if (!a) throw NullValueException("Arrays must not be null");
		else
		{
			BOOST_FOREACH(RR_INTRUSIVE_PTR<T>& aa, (*a))
			{
				VerifyRRMultiDimArrayLength<Ndims>(aa, n_elems, dims);
			}
		}
		return a;
	}

	template<size_t Ndims, typename K, typename T>
	static RR_INTRUSIVE_PTR<RRMap<K,T> > VerifyRRMultiDimArrayLength(RR_INTRUSIVE_PTR<RRMap<K,T> > a, size_t n_elems, boost::array<uint32_t, Ndims> dims)
	{
		if (!a) throw NullValueException("Arrays must not be null");
		else
		{
			BOOST_FOREACH(RR_INTRUSIVE_PTR<T>& aa, *a | boost::adaptors::map_values)
			{
				VerifyRRMultiDimArrayLength<Ndims>(aa, n_elems, dims);
			}
			return a;
		}
	}

	template<typename T>
	static RR_INTRUSIVE_PTR<RRMultiDimArray<T> > AllocateEmptyRRMultiDimArray(std::vector<uint32_t> length)
	{
		uint32_t n_elems = boost::accumulate(length, 1, std::multiplies<uint32_t>());
		return new RRMultiDimArray<T>(VectorToRRArray<uint32_t>(length), AllocateEmptyRRArray<T>(n_elems));
	}

	template<typename T>
	static RR_INTRUSIVE_PTR<RRMultiDimArray<T> > AllocateEmptyRRMultiDimArray()
	{
		std::vector<uint32_t> length;
		length.push_back(0);
		return new RRMultiDimArray<T>(VectorToRRArray<uint32_t>(length), AllocateEmptyRRArray<T>(0));
	}

	template<typename T>
	static RR_INTRUSIVE_PTR<RRMultiDimArray<T> > AllocateRRMultiDimArray(RR_INTRUSIVE_PTR<RRArray<uint32_t> > dims, RR_INTRUSIVE_PTR<RRArray<T> > array_)
	{		
		return new RRMultiDimArray<T>(dims, array_);
	}

	template<typename T>
	RR_INTRUSIVE_PTR<RRList<T> > AllocateEmptyRRList()
	{
		return new RRList<T>();
	}

	template<typename T, typename U>
	RR_INTRUSIVE_PTR<RRList<T> > AllocateRRList(const U& c)
	{
		return new RRList<T>(c);
	}

	template<typename K, typename T>
	RR_INTRUSIVE_PTR<RRMap<K,T> > AllocateEmptyRRMap()
	{
		return new RRMap<K,T>();
	}

	template<typename K, typename T, typename U>
	RR_INTRUSIVE_PTR<RRMap<K, T> > AllocateRRMap(const U& c)
	{
		return new RRMap<K, T>(c);
	}

	class ROBOTRACONTEUR_CORE_API RRPod
	{
	public:
		
	};

	class RRPodBaseArray : public RRValue
	{
	public:
		virtual std::string RRElementTypeString() = 0;
	};

	template<typename T>
	class RRPodArray : public RRPodBaseArray
	{
	protected:
		typename std::vector<T> pod_array;

	public:

		RRPodArray(size_t n) 
		{
			pod_array.resize(n);
		}

		RRPodArray(typename std::vector<T>& array_in)
		{
			pod_array = array_in;
		}

		RRPodArray(const T& value_in)
		{
			pod_array.push_back(value_in);
		}

		virtual ~RRPodArray() {}

		virtual std::string RRType()
		{
			return "RobotRaconteur.RRPodArray";
		}

		virtual std::string RRElementTypeString()
		{
			return RRPrimUtil<T>::GetElementTypeString();
		}

		// C++ container support based on boost::container::vector

		typedef T                                                value_type;
		typedef typename std::vector<T>::pointer                 pointer;
		typedef typename std::vector<T>::const_pointer           const_pointer;
		typedef typename std::vector<T>::reference               reference;
		typedef typename std::vector<T>::const_reference         const_reference;
		typedef typename std::vector<T>::size_type               size_type;
		typedef typename std::vector<T>::iterator                iterator;
		typedef typename std::vector<T>::const_iterator          const_iterator;
		typedef typename std::vector<T>::reverse_iterator       reverse_iterator;
		typedef typename std::vector<T>::const_reverse_iterator  const_reverse_iterator;

		iterator begin() { return pod_array.begin(); }
		const_iterator begin() const { return pod_array.begin(); }
		iterator end() { return pod_array.end(); }
		const_iterator end() const { return pod_array.end(); }
		reverse_iterator rbegin() { return pod_array.rbegin(); }
		const_reverse_iterator rbegin() const { return pod_array.rbegin(); }
		reverse_iterator rend() { return pod_array.rend(); }
		const_reverse_iterator rend() const { return pod_array.rend(); }
		const_iterator cbegin() const { return pod_array.cbegin(); }
		const_iterator cend() const { return pod_array.cend(); }
		const_reverse_iterator const crbegin() { return pod_array.crbegin(); }
		const_reverse_iterator const crend() { return pod_array.crend(); }
		bool empty() const { return pod_array.empty(); }
		virtual size_type size() const { return pod_array.size(); }
		size_type max_size() const { return size(); }
		reference front() { return pod_array.front(); }
		const_reference front() const { return pod_array.front(); }
		reference back() { return pod_array.back(); }
		const_reference back() const { return pod_array.back(); }
		reference operator[](size_type i) { return pod_array[i]; }
		const_reference operator[](size_type i) const { return pod_array[i]; }
		reference at(size_type i) { return pod_array.at(i); }
		const_reference at(size_type i) const { return pod_array.at(i); }
		T * data() { return pod_array.data(); }
		const T * data() const { return pod_array.data(); }

		// WARNING: this may change, use with caution!
		typename std::vector<T>& GetStorageContainer()
		{
			return pod_array;
		}
	};

#define RRPrimUtilPod(x,type_string) \
	template<> class RRPrimUtil<x> \
	{ \
		public: \
		static DataTypes GetTypeID() {return DataTypes_pod_t;}  \
		static std::string GetElementTypeString() {return type_string; } \
		static RR_INTRUSIVE_PTR<RRPodArray<x> > PrePack(const x& val) {return ScalarToRRPodArray(val);}\
		template<typename U> \
		static x PreUnpack(const U& val) {return RRArrayToScalar(rr_cast<RRPodArray<x> >(val));} \
		typedef RR_INTRUSIVE_PTR<RRPodArray<x> > BoxedType; \
	};

	class RRPodBaseMultiDimArray : public RRValue
	{
	public:
		RR_INTRUSIVE_PTR<RRArray<uint32_t> > Dims;
		virtual std::string RRElementTypeString() = 0;
	};

	template<typename T>
	class RRPodMultiDimArray : public RRPodBaseMultiDimArray
	{
	public:
		
		typename RR_INTRUSIVE_PTR<RRPodArray<T> > PodArray;

		RRPodMultiDimArray() {}

		RRPodMultiDimArray(RR_INTRUSIVE_PTR<RRArray<uint32_t> > dims, RR_INTRUSIVE_PTR<RRPodArray<T> > a)
		{
			this->Dims = dims;
			this->PodArray = a;
		}

		virtual ~RRPodMultiDimArray() {}

		virtual std::string RRType()
		{
			return "RobotRaconteur.RRPodMultiDimArray";
		}

		virtual std::string RRElementTypeString()
		{
			return RRPrimUtil<T>::GetElementTypeString();
		}

		virtual void RetrieveSubArray(std::vector<uint32_t> memorypos, RR_INTRUSIVE_PTR<RRPodMultiDimArray<T> > buffer, std::vector<uint32_t> bufferpos, std::vector<uint32_t> count)
		{

			std::vector<uint32_t> mema_dims = RRArrayToVector<uint32_t>(Dims);
			std::vector<uint32_t> memb_dims = RRArrayToVector<uint32_t>(buffer->Dims);
			RR_SHARED_PTR<detail::MultiDimArray_CalculateCopyIndicesIter> iter = detail::MultiDimArray_CalculateCopyIndicesBeginIter(mema_dims, memorypos, memb_dims, bufferpos, count);

			uint32_t len;
			uint32_t indexa;
			uint32_t indexb;

			while (iter->Next(indexa, indexb, len))
			{
				for (size_t i = 0; i < len; i++)
				{
					buffer->PodArray->at(indexb + i) = PodArray->at(indexa + i);
				}				
			}

		}

		virtual void AssignSubArray(std::vector<uint32_t> memorypos, RR_INTRUSIVE_PTR<RRPodMultiDimArray<T> > buffer, std::vector<uint32_t> bufferpos, std::vector<uint32_t> count)
		{
			
			std::vector<uint32_t> mema_dims = RRArrayToVector<uint32_t>(Dims);
			std::vector<uint32_t> memb_dims = RRArrayToVector<uint32_t>(buffer->Dims);
			RR_SHARED_PTR<detail::MultiDimArray_CalculateCopyIndicesIter> iter = detail::MultiDimArray_CalculateCopyIndicesBeginIter(mema_dims, memorypos, memb_dims, bufferpos, count);

			uint32_t len;
			uint32_t indexa;
			uint32_t indexb;

			while (iter->Next(indexa, indexb, len))
			{
				for (size_t i = 0; i < len; i++)
				{
					PodArray->at(indexa + i) = buffer->PodArray->at(indexb + i);
				}
			}

		}
	};

	template<typename T>
	static RR_INTRUSIVE_PTR<RRPodArray<T> > ScalarToRRPodArray(const T& value)
	{
		return new RRPodArray<T>(value);
	}

	template<typename T>
	static T RRPodArrayToScalar(RR_INTRUSIVE_PTR<RRPodArray<T> > value)
	{
		if (!value)
		{
			throw NullValueException("Null pointer");
		}

		if (value->size() == 0) throw OutOfRangeException("Index out of range");

		return value->at(0);
	}

	template<typename T>
	static RR_INTRUSIVE_PTR<RRPodArray<T> > AllocateEmptyRRPodArray(size_t length)
	{
		return new RRPodArray<T>(length);		
	}

	template<typename T>
	static RR_INTRUSIVE_PTR<RRPodMultiDimArray<T> > AllocateEmptyRRPodMultiDimArray(std::vector<uint32_t> length)
	{
		uint32_t n_elems = boost::accumulate(length, 1, std::multiplies<uint32_t>());
		return new RRPodMultiDimArray<T>(VectorToRRArray<uint32_t>(length), AllocateEmptyRRPodArray<T>(n_elems));
	}

	template<typename T>
	static RR_INTRUSIVE_PTR<RRPodMultiDimArray<T> > AllocateEmptyRRPodMultiDimArray()
	{
		std::vector<uint32_t> length;
		length.push_back(0);
		return new RRPodMultiDimArray<T>(VectorToRRArray<uint32_t>(length), AllocateEmptyRRPodArray<T>(0));
	}

#define RRPrimUtilNamedArray(x,type_string,array_type) \
	template<> class RRPrimUtil<x> \
	{ \
		public: \
		static DataTypes GetTypeID() {return DataTypes_pod_t;}  \
		static std::string GetElementTypeString() {return type_string; } \
		static RR_INTRUSIVE_PTR<RRNamedArray<x> > PrePack(const x& val) {return ScalarToRRNamedArray(val);}\
		template<typename U> \
		static x PreUnpack(const U& val) {return RRNamedArrayToScalar(rr_cast<RRNamedArray<x> >(val));} \
		typedef RR_INTRUSIVE_PTR<RRNamedArray<x> > BoxedType; \
		typedef array_type ElementArrayType; \
		static const size_t ElementArrayCount = sizeof(x) / sizeof(array_type); \
		static size_t GetElementArrayCount() {return ElementArrayCount;} \
		static DataTypes GetElementArrayTypeID() {return RRPrimUtil<array_type>::GetTypeID();} \
	};

	class ROBOTRACONTEUR_CORE_API RRNamedBaseArray : public RRValue
	{
	public:
		virtual DataTypes ElementArrayType() = 0;

		virtual size_t ElementSize() = 0;

		virtual size_t ElementArrayCount() = 0;

		virtual RR_INTRUSIVE_PTR<RRBaseArray> GetNumericBaseArray() = 0;

		virtual std::string RRElementTypeString() = 0;
	};

	template<typename T>
	class RRNamedArray : public RRNamedBaseArray
	{	
	protected:
		RR_INTRUSIVE_PTR<RRArray<typename RRPrimUtil<T>::ElementArrayType> > rr_array;

	public:

		RRNamedArray(RR_INTRUSIVE_PTR<RRArray<typename RRPrimUtil<T>::ElementArrayType> > rr_array)
		{
			if (!rr_array) throw NullValueException("Numeric array for namedarray must not be null");
			this->rr_array = rr_array;
		}

		virtual DataTypes GetTypeID()
		{
			return RRPrimUtil<T>::GetTypeID();
		}

		virtual size_t size() const
		{
			return rr_array->size() / (RRPrimUtil<T>::ElementArrayCount);
		}

		virtual size_t size()
		{
			return rr_array->size() / (RRPrimUtil<T>::ElementArrayCount);
		}

		virtual void* void_ptr()
		{
			return rr_array->void_ptr();
		}

		virtual size_t ElementSize()
		{
			return sizeof(T);
		}

		virtual DataTypes ElementArrayType()
		{
			return RRPrimUtil<T>::GetElementArrayTypeID();
		}
		
		virtual size_t ElementArrayCount()
		{
			return RRPrimUtil<T>::ElementArrayCount;
		}

		virtual RR_INTRUSIVE_PTR<RRArray<typename RRPrimUtil<T>::ElementArrayType> > GetNumericArray()
		{
			return rr_array;
		}

		virtual RR_INTRUSIVE_PTR<RRBaseArray> GetNumericBaseArray()
		{
			return rr_array;
		}
				
		virtual std::string RRType()
		{
			return "RobotRaconteur.RRNamedArray";
		}
		
		virtual std::string RRElementTypeString()
		{
			return RRPrimUtil<T>::GetElementTypeString();
		}

		// C++ container support functions based on boost::array

		// type definitions
		typedef T              value_type;
		typedef T*             iterator;
		typedef const T*       const_iterator;
		typedef T&             reference;
		typedef const T&       const_reference;
		typedef std::size_t    size_type;
		typedef std::ptrdiff_t difference_type;

		// iterator support
		iterator        begin() { return (T*)rr_array->begin(); }
		const_iterator  begin() const { return (const T*)rr_array->begin(); }
		const_iterator cbegin() const { return (const T*)rr_array->begin(); }

		iterator        end() { return (T*)rr_array->end(); }
		const_iterator  end() const { return (T*)rr_array->end(); }
		const_iterator cend() const { return (T*)rr_array->end(); }
		typedef boost::reverse_iterator<iterator> reverse_iterator;
		typedef boost::reverse_iterator<const_iterator> const_reverse_iterator;

		reverse_iterator rbegin() { return reverse_iterator(end()); }
		const_reverse_iterator rbegin() const {
			return const_reverse_iterator(end());
		}
		const_reverse_iterator crbegin() const {
			return const_reverse_iterator(end());
		}

		reverse_iterator rend() { return reverse_iterator(begin()); }
		const_reverse_iterator rend() const {
			return const_reverse_iterator(begin());
		}
		const_reverse_iterator crend() const {
			return const_reverse_iterator(begin());
		}

		// operator[]
		reference operator[](size_type i)
		{
			BOOST_ASSERT_MSG(i < size(), "out of range");
			return ((T*)rr_array->void_ptr())[i];
		}

		const_reference operator[](size_type i) const
		{
			BOOST_ASSERT_MSG(i < size(), "out of range");
			return ((T*)rr_array->void_ptr())[i];
		}

		// at() with range check
		reference at(size_type i) { rangecheck(i); return ((T*)rr_array->void_ptr())[i]; }
		const_reference at(size_type i) const { rangecheck(i); ((T*)rr_array->void_ptr())[i]; }

		// front() and back()
		reference front()
		{
			return this->at(0);
		}

		const_reference front() const
		{
			return this->at(0);
		}

		reference back()
		{
			return this->at(size() - 1);
		}

		const_reference back() const
		{
			return this->at(size() - 1);
		}

		bool empty() { return rr_array->empty(); }
		size_type max_size() { return size(); }
												
		void rangecheck(size_type i) {
			if (i >= size()) {
				std::out_of_range e("array<>: index out of range");
				boost::throw_exception(e);
			}
		}

	};

	class RRNamedBaseMultiDimArray : public RRValue
	{
	public:
		RR_INTRUSIVE_PTR<RRArray<uint32_t> > Dims;
		virtual std::string RRElementTypeString() = 0;
	};

	template<typename T>
	class RRNamedMultiDimArray : public RRNamedBaseMultiDimArray
	{
	public:

		typename RR_INTRUSIVE_PTR<RRNamedArray<T> > NamedArray;

		RRNamedMultiDimArray() {}

		RRNamedMultiDimArray(RR_INTRUSIVE_PTR<RRArray<uint32_t> > dims, RR_INTRUSIVE_PTR<RRNamedArray<T> > a)
		{
			this->Dims = dims;
			this->NamedArray = a;
		}

		virtual ~RRNamedMultiDimArray() {}

		virtual std::string RRType()
		{
			return "RobotRaconteur.RRNamedMultiDimArray";
		}

		virtual std::string RRElementTypeString()
		{
			return RRPrimUtil<T>::GetElementTypeString();
		}

		virtual void RetrieveSubArray(std::vector<uint32_t> memorypos, RR_INTRUSIVE_PTR<RRNamedMultiDimArray<T> > buffer, std::vector<uint32_t> bufferpos, std::vector<uint32_t> count)
		{

			std::vector<uint32_t> mema_dims = RRArrayToVector<uint32_t>(Dims);
			std::vector<uint32_t> memb_dims = RRArrayToVector<uint32_t>(buffer->Dims);
			RR_SHARED_PTR<detail::MultiDimArray_CalculateCopyIndicesIter> iter = detail::MultiDimArray_CalculateCopyIndicesBeginIter(mema_dims, memorypos, memb_dims, bufferpos, count);

			uint32_t len;
			uint32_t indexa;
			uint32_t indexb;

			while (iter->Next(indexa, indexb, len))
			{
				for (size_t i = 0; i < len; i++)
				{
					(*buffer->NamedArray)[(indexb + i)] = (*NamedArray)[(indexa + i)];
				}
			}

		}

		virtual void AssignSubArray(std::vector<uint32_t> memorypos, RR_INTRUSIVE_PTR<RRNamedMultiDimArray<T> > buffer, std::vector<uint32_t> bufferpos, std::vector<uint32_t> count)
		{

			std::vector<uint32_t> mema_dims = RRArrayToVector<uint32_t>(Dims);
			std::vector<uint32_t> memb_dims = RRArrayToVector<uint32_t>(buffer->Dims);
			RR_SHARED_PTR<detail::MultiDimArray_CalculateCopyIndicesIter> iter = detail::MultiDimArray_CalculateCopyIndicesBeginIter(mema_dims, memorypos, memb_dims, bufferpos, count);

			uint32_t len;
			uint32_t indexa;
			uint32_t indexb;

			while (iter->Next(indexa, indexb, len))
			{
				for (size_t i = 0; i < len; i++)
				{
					(*NamedArray)[(indexa + i)] = (*buffer->NamedArray)[(indexb + i)];
				}
			}

		}
	};
		
	template<typename T>
	static RR_INTRUSIVE_PTR<RRNamedArray<T> > AllocateEmptyRRNamedArray(size_t length)
	{
		typedef typename RRPrimUtil<T>::ElementArrayType a_type;
		RR_INTRUSIVE_PTR<RRArray<a_type> > a = AllocateRRArray<a_type> (length * RRPrimUtil<T>::GetElementArrayCount());
		return new RRNamedArray<T>(a);		
	}

	template<typename T>
	static RR_INTRUSIVE_PTR<RRNamedMultiDimArray<T> > AllocateEmptyRRNamedMultiDimArray(std::vector<uint32_t> length)
	{
		uint32_t n_elems = boost::accumulate(length, 1, std::multiplies<uint32_t>());
		return new RRNamedMultiDimArray<T>(VectorToRRArray<uint32_t>(length), AllocateEmptyRRNamedArray<T>(n_elems));
	}

	template<typename T>
	static RR_INTRUSIVE_PTR<RRNamedMultiDimArray<T> > AllocateEmptyRRNamedMultiDimArray()
	{		
		std::vector<uint32_t> length;
		length.push_back(0);
		return new RRNamedMultiDimArray<T>(VectorToRRArray<uint32_t>(length), AllocateEmptyRRNamedArray<T>(0));
	}

	template<typename T>
	static RR_INTRUSIVE_PTR<RRNamedArray<T> > ScalarToRRNamedArray(const T& value)
	{
		RR_INTRUSIVE_PTR<RRNamedArray<T> > a = AllocateEmptyRRNamedArray<T>(1);
		(*a)[0] = value;
		return a;
	}

	template<typename T>
	static T RRNamedArrayToScalar(RR_INTRUSIVE_PTR<RRNamedArray<T> > value)
	{
		if (!value)
		{
			throw NullValueException("Null pointer");
		}

		if (value->size() == 0) throw OutOfRangeException("Index out of range");

		return (*value)[0];
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
