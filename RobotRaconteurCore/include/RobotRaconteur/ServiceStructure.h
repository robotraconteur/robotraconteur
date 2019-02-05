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

#include "RobotRaconteur/RobotRaconteurNode.h"
#include "RobotRaconteur/DataTypes.h"
#include "RobotRaconteur/ServiceFactory.h"
#include "RobotRaconteur/ServiceDefinition.h"

namespace RobotRaconteur
{

	//struct

	class ROBOTRACONTEUR_CORE_API StructureStub
	{
	public:
		virtual RR_SHARED_PTR<MessageElementStructure> PackStructure(RR_SHARED_PTR<RRValue> s) = 0;

		virtual RR_SHARED_PTR<RRStructure> UnpackStructure(RR_SHARED_PTR<MessageElementStructure> m) = 0;

		StructureStub(RR_SHARED_PTR<RobotRaconteurNode> node);

		RR_SHARED_PTR<RobotRaconteurNode> GetNode();
		RR_SHARED_PTR<RobotRaconteurNode> RRGetNode() { return GetNode(); }
		RR_WEAK_PTR<RobotRaconteurNode> RRGetNodeWeak() { return node; }

		virtual ~StructureStub() {}
	protected:

		RR_WEAK_PTR<RobotRaconteurNode> node;

	};

	// cstruct

	template<typename T>
	class CStructureStub
	{
		BOOST_STATIC_ASSERT(sizeof(T) == -1);
	};

	template<typename T, size_t N, bool varlength>
	class cstructure_field_array : public boost::array<T, N>
	{
	private:
		size_t len;		
	public:
		cstructure_field_array() : len(0) {}
		cstructure_field_array(size_t n) { resize(n); }
		typename boost::array<T,N>::iterator        end() { return boost::array<T,N>::elems + len; }
		typename boost::array<T,N>::const_iterator  end() const { return boost::array<T, N>::elems + len; }
		typename boost::array<T,N>::const_iterator cend() const { return boost::array<T, N>::elems + len; }
		void resize(size_t n) { if (n > N) { throw std::out_of_range("requested size exceeds array max size"); } len = n; }
		void clear() { resize(0); }
		size_t size() const { return len; }
		size_t max_size() const { return N; } 
		typename boost::array<T,N>::reference       at(size_t i) { return rangecheck(i), boost::array<T, N>::elems[i]; }
		typename boost::array<T,N>::const_reference at(size_t i) const { return rangecheck(i), boost::array<T, N>::elems[i]; }
		typename boost::array<T,N>::reference back() {	return at(size() - 1);	}
		typename boost::array<T,N>::const_reference back() const { return at(size() - 1); }
		template <typename T2>
		cstructure_field_array<T, N, varlength>& operator= (const cstructure_field_array<T2, N, varlength>& rhs) {
			std::copy(rhs.begin(), rhs.end(), boost::array<T, N>::begin());
			this->len = rhs->len;
			return *this;
		}
		bool rangecheck(size_t i) const {
			return i > size() ? boost::throw_exception(std::out_of_range("array<>: index out of range")), true : true;
		}
	};

	template<typename T, size_t N>
	class cstructure_field_array<T, N, false> : public boost::array<T, N>
	{
	public:
		void resize(size_t n) { if (n != N) throw std::out_of_range("requested size does not match fixed array size"); }
		size_t max_size() { return N; }
	};

	template <typename T, size_t N, bool varlength>
	RR_SHARED_PTR<RRArray<T> > cstructure_field_array_ToRRArray(const cstructure_field_array<T, N, varlength>& i)
	{
		return AttachRRArrayCopy<T>(&i[0], i.size());
	}

	template <typename T, size_t N, bool varlength>
	void RRArrayTo_cstructure_field_array(cstructure_field_array<T, N, varlength>& v, const RR_SHARED_PTR<RRArray<T> >& i)
	{
		if (!i) throw NullValueException("CStructure array must not be null");
		v.resize(i->size());
		memcpy(&v[0], i->ptr(), sizeof(T)*i->size());		
	}

#define RRCStructureStubNumberType(type) \
	template<> \
	class CStructureStub<type> \
	{ \
	public: \
		template<typename U> \
		static void PackField(const type& v, const std::string& name, U& out) \
		{ \
			out.push_back(RR_MAKE_SHARED<MessageElement>(name, ScalarToRRArray<type>(v))); \
		} \
		\
		template<typename U> \
		static void UnpackField(type& v, const std::string& name, U& in) \
		{ \
			v = RRArrayToScalar<type>(MessageElement::FindElement(in, name)->template CastData<RRArray<type> >()); \
		} \
	}; \
	\
	template<size_t N, bool varlength> \
	class CStructureStub<cstructure_field_array<type, N, varlength> > \
	{ \
	public: \
		template<typename U> \
		static void PackField(const cstructure_field_array<type, N, varlength>& v, const std::string& name, U& out) \
		{ \
			out.push_back(RR_MAKE_SHARED<MessageElement>(name, cstructure_field_array_ToRRArray(v))); \
		} \
		\
		template<typename U> \
		static void UnpackField(cstructure_field_array<type, N, varlength>& v, const std::string& name, U& in) \
		{ \
			RR_SHARED_PTR<RRArray<type> > a = MessageElement::FindElement(in, name)->template CastData<RRArray<type> >(); \
			RRArrayTo_cstructure_field_array(v, a); \
		} \
	}; \
	
	RRCStructureStubNumberType(double);
	RRCStructureStubNumberType(float);
	RRCStructureStubNumberType(int8_t);
	RRCStructureStubNumberType(uint8_t);
	RRCStructureStubNumberType(int16_t);
	RRCStructureStubNumberType(uint16_t);
	RRCStructureStubNumberType(int32_t);
	RRCStructureStubNumberType(uint32_t);
	RRCStructureStubNumberType(int64_t);
	RRCStructureStubNumberType(uint64_t);
		
	template <typename T, size_t N, bool varlength>
	RR_SHARED_PTR<RRAStructureArray<T> > cstructure_field_array_ToRRAStructureArray(const cstructure_field_array<T, N, varlength>& i)
	{
		typedef RRPrimUtil<T>::ElementArrayType element_type;
		RR_SHARED_PTR<RRArray<element_type> > a = AttachRRArrayCopy<element_type>((const element_type*)&i[0], i.size() * RRPrimUtil<T>::GetElementArrayCount());
		return RR_MAKE_SHARED<RRAStructureArray<T> >(a);
	}

	template <typename T, size_t N, bool varlength>
	void RRAStructureArrayTo_cstructure_field_array(cstructure_field_array<T, N, varlength>& v, const RR_SHARED_PTR<RRAStructureArray<T> >& i)
	{
		if (!i) throw NullValueException("CStructure array must not be null");
		v.resize(i->size());
		memcpy(&v[0], i->GetNumericArray()->ptr(), sizeof(T)*i->size());
	}

#define RRCStructureStubAStructureType(type) \
	template<> \
	class CStructureStub<type> \
	{ \
	public: \
		template<typename U> \
		static void PackField(const type& v, const std::string& name, U& out) \
		{ \
			std::vector<RR_SHARED_PTR<MessageElement> > v1; \
			v1.push_back(RR_MAKE_SHARED<MessageElement>("array",ScalarToRRAStructureArray<type>(v)->GetNumericArray())); \
			out.push_back(RR_MAKE_SHARED<MessageElement>(name, RR_MAKE_SHARED<MessageElementAStructureArray>(RRPrimUtil<type>::GetElementTypeString(),v1))); \
		} \
		\
		template<typename U> \
		static void UnpackField(type& v, const std::string& name, U& in) \
		{ \
			typedef RRPrimUtil<type>::ElementArrayType element_type; \
			RR_SHARED_PTR<MessageElementAStructureArray> m = MessageElement::FindElement(in,name)->CastData<MessageElementAStructureArray>(); \
			if (m->Type != RRPrimUtil<type>::GetElementTypeString()) throw DataTypeException("Invalid astruct"); \
			RR_SHARED_PTR<RRArray<element_type> > a=MessageElement::FindElement(m->Elements, "array")->CastData<RRArray<element_type> >(); \
			if (a->Length() != RRPrimUtil<type>::GetElementArrayCount()) throw DataTypeException("Invalid astruct"); \
			memcpy(&v, a->void_ptr(), sizeof(v)); \
		} \
	}; \
	\
	template<size_t N, bool varlength> \
	class CStructureStub<cstructure_field_array<type, N, varlength> > \
	{ \
	public: \
		template<typename U> \
		static void PackField(const cstructure_field_array<type, N, varlength>& v, const std::string& name, U& out) \
		{ \
			RR_SHARED_PTR<RRAStructureArray<type> > a = cstructure_field_array_ToRRAStructureArray(v); \
			std::vector<RR_SHARED_PTR<MessageElement> > a1; \
			a1.push_back(RR_MAKE_SHARED<MessageElement>("array", a->GetNumericArray())); \
			out.push_back(RR_MAKE_SHARED<MessageElement>(name, RR_MAKE_SHARED<MessageElementAStructureArray>(RRPrimUtil<type>::GetElementTypeString(), a1))); \
		} \
		\
		template<typename U> \
		static void UnpackField(cstructure_field_array<type, N, varlength>& v, const std::string& name, U& in) \
		{ \
			typedef RRPrimUtil<type>::ElementArrayType element_type; \
			RR_SHARED_PTR<MessageElementAStructureArray> a = MessageElement::FindElement(in, name)->CastData<MessageElementAStructureArray>(); \
			RR_SHARED_PTR<RRArray<element_type> > a1 = MessageElement::FindElement(a->Elements, "array")->template CastData<RRArray<element_type> >(); \
			v.resize(a1->size() / RRPrimUtil<type>::GetElementArrayCount()); \
			memcpy(&v, a1->ptr(), a1->size() * sizeof(element_type)); \
		} \
	}; 

	template<typename T, size_t N, bool varlength>
	class CStructureStub<cstructure_field_array<T, N, varlength> >
	{
	public:
		template<typename U>
		static void PackField(const cstructure_field_array<T, N, varlength>& v, const std::string& name, U& out)
		{
			std::vector<RR_SHARED_PTR<MessageElement> > o;
			for (size_t j = 0; j < v.size(); j++)
			{
				RR_SHARED_PTR<MessageElement> m = RR_MAKE_SHARED<MessageElement>("", CStructureStub<T>::PackToMessageElementCStructure(v[j]));
				m->ElementFlags &= ~MessageElementFlags_ELEMENT_NAME_STR;
				m->ElementFlags |= MessageElementFlags_ELEMENT_NUMBER;
				m->ElementNumber = j;
				o.push_back(m);
			}
			out.push_back(RR_MAKE_SHARED<MessageElement>(name, RR_MAKE_SHARED<MessageElementCStructureArray>(RRPrimUtil<T>::GetElementTypeString(), o)));
		}

		template<typename U>
		static void UnpackField(cstructure_field_array<T, N, varlength>& v, const std::string& name, U& in)
		{		
			RR_SHARED_PTR<MessageElementCStructureArray> a = MessageElement::FindElement(in, name)->template CastData<MessageElementCStructureArray>();
			if (!a) throw NullValueException("Unexpected null array");
			if (a->Type != RRPrimUtil<T>::GetElementTypeString()) throw DataTypeException("CStructure data type mismatch");
			//if (a->Elements.size() > N) throw OutOfRangeException("Array is too large for static vector size");
			v.resize(a->Elements.size());
			for (int32_t i = 0; i<(int32_t)a->Elements.size(); i++)
			{
				RR_SHARED_PTR<MessageElement> m = a->Elements.at(i);
				int32_t key;
				if (m->ElementFlags & MessageElementFlags_ELEMENT_NUMBER)
				{
					key = m->ElementNumber;
				}
				else if (m->ElementFlags & MessageElementFlags_ELEMENT_NAME_STR)
				{
					key = boost::lexical_cast<int32_t>(m->ElementName);
				}
				else
				{
					throw DataTypeException("Invalid cstructure array format");
				}

				if (key != i) throw DataTypeException("Invalid cstructure array format");

				CStructureStub<T>::UnpackFromMessageElementCStructure(v[i], m->CastData<MessageElementCStructure>());
			}
		}
	};
		
	template<typename T, typename U>
	void CStructureStub_PackField(const T& v, const std::string& name, U& out)
	{
		CStructureStub<T>::PackField(v, name, out);
	}

	template<typename T, typename U>
	void CStructureStub_UnpackField(T& v, const std::string& name, U& out)
	{		
		CStructureStub<T>::UnpackField(v, name, out);
	}

	template<typename T>
	RR_SHARED_PTR<MessageElementCStructureArray> CStructureStub_PackCStructureToArray(const T& v)
	{
		std::vector<RR_SHARED_PTR<MessageElement> > o;
		
		RR_SHARED_PTR<MessageElement> m = RR_MAKE_SHARED<MessageElement>("", CStructureStub<T>::PackToMessageElementCStructure(v));
		m->ElementFlags &= ~MessageElementFlags_ELEMENT_NAME_STR;
		m->ElementFlags |= MessageElementFlags_ELEMENT_NUMBER;
		m->ElementNumber = 0;
		o.push_back(m);
		
		return RR_MAKE_SHARED<MessageElementCStructureArray>(RRPrimUtil<T>::GetElementTypeString(), o);		
	}

	template<typename T>
	void CStructureStub_UnpackCStructureFromArray(T& v, RR_SHARED_PTR<MessageElementCStructureArray> a)
	{
		if (!a) throw DataTypeException("CStructure scalar array must not be null");
		if (a->Type != RRPrimUtil<T>::GetElementTypeString()) throw DataTypeException("CStructure data type mismatch");
		if (a->Elements.size() != 1) throw DataTypeException("Invalid cstructure scalar array format");

		RR_SHARED_PTR<MessageElement> m = a->Elements.at(0);
		int32_t key;
		if (m->ElementFlags & MessageElementFlags_ELEMENT_NUMBER)
		{
			key = m->ElementNumber;
		}
		else if (m->ElementFlags & MessageElementFlags_ELEMENT_NAME_STR)
		{
			key = boost::lexical_cast<int32_t>(m->ElementName);
		}
		else
		{
			throw DataTypeException("Invalid cstructure scalar array format");
		}

		if (key != 0) throw DataTypeException("Invalid cstructure scalar array format");

		CStructureStub<T>::UnpackFromMessageElementCStructure(v, m->CastData<MessageElementCStructure>());
	}

	template<typename T>
	T CStructureStub_UnpackCStructureFromArray(RR_SHARED_PTR<MessageElementCStructureArray> a)
	{
		T v;
		CStructureStub_UnpackCStructureFromArray<T>(v, a);
		return v;
	}

	template<typename T>
	RR_SHARED_PTR<MessageElementCStructureArray> CStructureStub_PackCStructureArray(RR_SHARED_PTR<RRCStructureArray<T> > a)
	{
		if (!a) return RR_SHARED_PTR<MessageElementCStructureArray>();		
		std::vector<RR_SHARED_PTR<MessageElement> > o;
		for (size_t i = 0; i < a->cstruct_array.size(); i++)
		{
			RR_SHARED_PTR<MessageElement> m = RR_MAKE_SHARED<MessageElement>("", CStructureStub<T>::PackToMessageElementCStructure(a->cstruct_array[i]));
			m->ElementFlags &= ~MessageElementFlags_ELEMENT_NAME_STR;
			m->ElementFlags |= MessageElementFlags_ELEMENT_NUMBER;
			m->ElementNumber = i;
			o.push_back(m);
		}
		return RR_MAKE_SHARED<MessageElementCStructureArray>(RRPrimUtil<T>::GetElementTypeString(), o);
	}

	template<typename T>
	RR_SHARED_PTR<RRCStructureArray<T> > CStructureStub_UnpackCStructureArray(RR_SHARED_PTR<MessageElementCStructureArray> a)
	{
		if (!a) return RR_SHARED_PTR<RRCStructureArray<T> >();

		RR_SHARED_PTR<RRCStructureArray<T> > o = RR_MAKE_SHARED<RRCStructureArray<T> >();
		o->cstruct_array.resize(a->Elements.size());
		for (size_t i = 0; i < a->Elements.size(); i++)
		{
			RR_SHARED_PTR<MessageElement> m = a->Elements.at(i);
			int32_t key;
			if (m->ElementFlags & MessageElementFlags_ELEMENT_NUMBER)
			{
				key = m->ElementNumber;
			}
			else if (m->ElementFlags & MessageElementFlags_ELEMENT_NAME_STR)
			{
				key = boost::lexical_cast<int32_t>(m->ElementName);
			}
			else
			{
				throw DataTypeException("Invalid cstructure array format");
			}

			if (key != i) throw DataTypeException("Invalid cstructure array format");
			CStructureStub<T>::UnpackFromMessageElementCStructure(o->cstruct_array[i], m->CastData<MessageElementCStructure>());
		}

		return o;		
	}

	template<typename T>
	RR_SHARED_PTR<MessageElementCStructureMultiDimArray> CStructureStub_PackCStructureMultiDimArray(RR_SHARED_PTR<RRCStructureMultiDimArray<T> > a)
	{
		if (!a) return RR_SHARED_PTR<MessageElementCStructureMultiDimArray>();

		std::vector<RR_SHARED_PTR<MessageElement> > m;
		m.push_back(RR_MAKE_SHARED<MessageElement>("dims", a->Dims));
		if (!a->CStructArray) throw NullValueException("Multidimarray array must not be null");
		m.push_back(RR_MAKE_SHARED<MessageElement>("array", CStructureStub_PackCStructureArray(a->CStructArray)));
		return RR_MAKE_SHARED<MessageElementCStructureMultiDimArray>(RRPrimUtil<T>::GetElementTypeString(), m);		
	}

	template<typename T>
	RR_SHARED_PTR<RRCStructureMultiDimArray<T> > CStructureStub_UnpackCStructureMultiDimArray(RR_SHARED_PTR<MessageElementCStructureMultiDimArray> m)
	{
		if (!m) return RR_SHARED_PTR<RRCStructureMultiDimArray<T> >();

		RR_SHARED_PTR<RRCStructureMultiDimArray<T> > o = RR_MAKE_SHARED<RRCStructureMultiDimArray<T> >();
		o->Dims = (MessageElement::FindElement(m->Elements, "dims")->CastData<RRArray<int32_t> >());
		o->CStructArray = CStructureStub_UnpackCStructureArray<T>(MessageElement::FindElement(m->Elements, "array")->CastData<MessageElementCStructureArray>());
		if (!o->CStructArray) throw NullValueException("Multidimarray array must not be null");
		return o;
	}


	//MessageElement pack helper functions for cstruct
	template<typename T>
	RR_SHARED_PTR<MessageElement> MessageElement_PackCStructureToArrayElement(const std::string& elementname, const T& s)
	{
		return RR_MAKE_SHARED<MessageElement>(elementname, CStructureStub_PackCStructureToArray(s));
	}

	template<typename T>
	RR_SHARED_PTR<MessageElement> MessageElement_PackCStructureArrayElement(const std::string& elementname, const RR_SHARED_PTR<RRCStructureArray<T> >& s)
	{
		if (!s) throw NullValueException("Arrays must not be null");
		return RR_MAKE_SHARED<MessageElement>(elementname, CStructureStub_PackCStructureArray(s));
	}

	template<typename T>
	RR_SHARED_PTR<MessageElement> MessageElement_PackCStructureMultiDimArrayElement(const std::string& elementname, const RR_SHARED_PTR<RRCStructureMultiDimArray<T> >& s)
	{
		if (!s) throw NullValueException("Arrays must not be null");
		return RR_MAKE_SHARED<MessageElement>(elementname, CStructureStub_PackCStructureMultiDimArray(s));
	}

	//MessageElement unpack helper functions for cstruct
	template<typename T>
	T MessageElement_UnpackCStructureFromArray(const RR_SHARED_PTR<MessageElement>& m)
	{
		return RobotRaconteur::CStructureStub_UnpackCStructureFromArray<T>(m->CastData<RobotRaconteur::MessageElementCStructureArray>());
	}

	template<typename T>
	RR_SHARED_PTR<RRCStructureArray<T> > MessageElement_UnpackCStructureArray(const RR_SHARED_PTR<MessageElement>& m)
	{
		RR_SHARED_PTR<RRCStructureArray<T> > a = RobotRaconteur::CStructureStub_UnpackCStructureArray<T>(m->CastData<RobotRaconteur::MessageElementCStructureArray>());
		if (!a) throw NullValueException("Arrays must not be null");
		return a;
	}

	template<typename T>
	RR_SHARED_PTR<RRCStructureMultiDimArray<T> > MessageElement_UnpackCStructureMultiDimArray(const RR_SHARED_PTR<MessageElement>& m)
	{
		RR_SHARED_PTR<RRCStructureMultiDimArray<T> > a = RobotRaconteur::CStructureStub_UnpackCStructureMultiDimArray<T>(m->CastData<RobotRaconteur::MessageElementCStructureMultiDimArray>());
		if (!a) throw NullValueException("Arrays must not be null");
		return a;
	}

	template<typename T>
	static RR_SHARED_PTR<RRCStructureArray<T> > VerifyRRArrayLength(RR_SHARED_PTR<RRCStructureArray<T> > a, size_t len, bool varlength)
	{
		if (!a) throw NullValueException("Arrays must not be null");
		if (len != 0)
		{
			if (varlength && (a->cstruct_array.size() > len))
			{
				throw DataTypeException("Array dimension mismatch");
			}
			if (!varlength && (a->cstruct_array.size() != len))
			{
				throw DataTypeException("Array dimension mismatch");
			}
		}
		return a;
	}

	template<size_t Ndims, typename T>
	static RR_SHARED_PTR<RRCStructureMultiDimArray<T> > VerifyRRMultiDimArrayLength(RR_SHARED_PTR<RRCStructureMultiDimArray<T> > a, size_t n_elems, boost::array<int32_t, Ndims> dims)
	{
		if (!a) throw NullValueException("Arrays must not be null");

		if (a->Dims->size() != Ndims)
		{
			throw DataTypeException("Array dimension mismatch");
		}

		if (a->CStructArray->cstruct_array.size() != n_elems)
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


	// astruct

	template<typename T>
	RR_SHARED_PTR<MessageElementAStructureArray> AStructureStub_PackAStructureToArray(const T& v)
	{
		typedef RRPrimUtil<T>::ElementArrayType element_type;
		RR_SHARED_PTR<RRArray<element_type> > a = AllocateRRArray<element_type>(RRPrimUtil<T>::GetElementArrayCount());
		memcpy(a->void_ptr(), &v, sizeof(T));
		std::vector<RR_SHARED_PTR<MessageElement> > a1;
		a1.push_back(RR_MAKE_SHARED<MessageElement>("array", a));
		return RR_MAKE_SHARED<MessageElementAStructureArray>(RRPrimUtil<T>::GetElementTypeString(), a1);

	}

	template<typename T>
	void AStructureStub_UnpackAStructureFromArray(T& v, RR_SHARED_PTR<MessageElementAStructureArray> a)
	{
		typedef RRPrimUtil<T>::ElementArrayType element_type;
		if (!a) throw DataTypeException("AStructure scalar array must not be null");
		if (a->Type != RRPrimUtil<T>::GetElementTypeString()) throw DataTypeException("AStructure data type mismatch");
		if (a->Elements.size() != 1) throw DataTypeException("Invalid astructure array format");
		RR_SHARED_PTR<RRArray<element_type> > a1 = MessageElement::FindElement(a->Elements, "array")->CastData<RRArray<element_type> >();
		if (a1->Length() != sizeof(T) / sizeof(element_type)) throw DataTypeException("Invalid scalar astructure array format");

		v = *((T*)a1->void_ptr());
	}

	template<typename T>
	T AStructureStub_UnpackAStructureFromArray(RR_SHARED_PTR<MessageElementAStructureArray> a)
	{
		T o;
		AStructureStub_UnpackAStructureFromArray(o, a);
		return o;
	}

	template<typename T>
	RR_SHARED_PTR<MessageElementAStructureArray> AStructureStub_PackAStructureArray(RR_SHARED_PTR<RRAStructureArray<T> > a)
	{
		if (!a) return RR_SHARED_PTR<MessageElementAStructureArray>();
		std::vector<RR_SHARED_PTR<MessageElement> > a1;
		a1.push_back(RR_MAKE_SHARED<MessageElement>("array",a->GetNumericArray()));
		return RR_MAKE_SHARED<MessageElementAStructureArray>(RRPrimUtil<T>::GetElementTypeString(), a1);
	}

	template<typename T>
	RR_SHARED_PTR<RRAStructureArray<T> > AStructureStub_UnpackAStructureArray(RR_SHARED_PTR<MessageElementAStructureArray> a)
	{
		typedef RRPrimUtil<T>::ElementArrayType element_type;
		if (!a) return RR_SHARED_PTR<RRAStructureArray<T> >();
		if (a->Type != RRPrimUtil<T>::GetElementTypeString()) throw DataTypeException("Invalid astruct type");
		RR_SHARED_PTR<RRArray<element_type> > a2 = MessageElement::FindElement(a->Elements, "array")->CastData<RRArray<element_type> >();
		return RR_MAKE_SHARED<RRAStructureArray<T> >(a2);
	}

	template<typename T>
	RR_SHARED_PTR<MessageElementAStructureMultiDimArray> AStructureStub_PackAStructureMultiDimArray(RR_SHARED_PTR<RRAStructureMultiDimArray<T> > a)
	{
		if (!a) return RR_SHARED_PTR<MessageElementAStructureMultiDimArray>();

		std::vector<RR_SHARED_PTR<MessageElement> > m;
		m.push_back(RR_MAKE_SHARED<MessageElement>("dims", a->Dims));
		if (!a->AStructArray) throw NullValueException("Multidimarray array must not be null");
		m.push_back(RR_MAKE_SHARED<MessageElement>("array", AStructureStub_PackAStructureArray(a->AStructArray)));
		return RR_MAKE_SHARED<MessageElementAStructureMultiDimArray>(RRPrimUtil<T>::GetElementTypeString(), m);
	}

	template<typename T>
	RR_SHARED_PTR<RRAStructureMultiDimArray<T> > AStructureStub_UnpackAStructureMultiDimArray(RR_SHARED_PTR<MessageElementAStructureMultiDimArray> m)
	{
		if (!m) return RR_SHARED_PTR<RRAStructureMultiDimArray<T> >();

		RR_SHARED_PTR<RRAStructureMultiDimArray<T> > o = RR_MAKE_SHARED<RRAStructureMultiDimArray<T> >();
		o->Dims = (MessageElement::FindElement(m->Elements, "dims")->CastData<RRArray<int32_t> >());
		o->AStructArray = AStructureStub_UnpackAStructureArray<T>(MessageElement::FindElement(m->Elements, "array")->CastData<MessageElementAStructureArray>());
		if (!o->AStructArray) throw NullValueException("Multidimarray array must not be null");
		return o;
	}

	//MessageElement pack helper functions for astruct
	template<typename T>
	RR_SHARED_PTR<MessageElement> MessageElement_PackAStructureToArrayElement(const std::string& elementname, const T& s)
	{
		return RR_MAKE_SHARED<MessageElement>(elementname, AStructureStub_PackAStructureToArray(s));
	}

	template<typename T>
	RR_SHARED_PTR<MessageElement> MessageElement_PackAStructureArrayElement(const std::string& elementname, const RR_SHARED_PTR<RRAStructureArray<T> >& s)
	{
		if (!s) throw NullValueException("Arrays must not be null");
		return RR_MAKE_SHARED<MessageElement>(elementname, AStructureStub_PackAStructureArray(s));
	}

	template<typename T>
	RR_SHARED_PTR<MessageElement> MessageElement_PackAStructureMultiDimArrayElement(const std::string& elementname, const RR_SHARED_PTR<RRAStructureMultiDimArray<T> >& s)
	{
		if (!s) throw NullValueException("Arrays must not be null");
		return RR_MAKE_SHARED<MessageElement>(elementname, AStructureStub_PackAStructureMultiDimArray(s));
	}

	//MessageElement unpack helper functions for astruct
	template<typename T>
	T MessageElement_UnpackAStructureFromArray(const RR_SHARED_PTR<MessageElement>& m)
	{
		return RobotRaconteur::AStructureStub_UnpackAStructureFromArray<T>(m->CastData<RobotRaconteur::MessageElementAStructureArray>());
	}

	template<typename T>
	RR_SHARED_PTR<RRAStructureArray<T> > MessageElement_UnpackAStructureArray(const RR_SHARED_PTR<MessageElement>& m)
	{
		RR_SHARED_PTR<RRAStructureArray<T> > a = RobotRaconteur::AStructureStub_UnpackAStructureArray<T>(m->CastData<RobotRaconteur::MessageElementAStructureArray>());
		if (!a) throw NullValueException("Arrays must not be null");
		return a;
	}

	template<typename T>
	RR_SHARED_PTR<RRAStructureMultiDimArray<T> > MessageElement_UnpackAStructureMultiDimArray(const RR_SHARED_PTR<MessageElement>& m)
	{
		RR_SHARED_PTR<RRAStructureMultiDimArray<T> > a = RobotRaconteur::AStructureStub_UnpackAStructureMultiDimArray<T>(m->CastData<RobotRaconteur::MessageElementAStructureMultiDimArray>());
		if (!a) throw NullValueException("Arrays must not be null");
		return a;
	}

	template<typename T>
	static RR_SHARED_PTR<RRAStructureArray<T> > VerifyRRArrayLength(RR_SHARED_PTR<RRAStructureArray<T> > a, size_t len, bool varlength)
	{
		if (!a) throw NullValueException("Arrays must not be null");
		if (len != 0)
		{
			if (varlength && (a->Length() > len))
			{
				throw DataTypeException("Array dimension mismatch");
			}
			if (!varlength && (a->Length() != len))
			{
				throw DataTypeException("Array dimension mismatch");
			}
		}
		return a;
	}

	template<size_t Ndims, typename T>
	static RR_SHARED_PTR<RRAStructureMultiDimArray<T> > VerifyRRMultiDimArrayLength(RR_SHARED_PTR<RRAStructureMultiDimArray<T> > a, size_t n_elems, boost::array<int32_t, Ndims> dims)
	{
		if (!a) throw NullValueException("Arrays must not be null");

		if (a->Dims->size() != Ndims)
		{
			throw DataTypeException("Array dimension mismatch");
		}

		if (a->AStructArray->Length() != n_elems)
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

}
