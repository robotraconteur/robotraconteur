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
	class ROBOTRACONTEUR_CORE_API StructureStub
	{
	public:
		virtual RR_SHARED_PTR<MessageElementStructure> PackStructure(RR_SHARED_PTR<RRValue> s) = 0;

		virtual RR_SHARED_PTR<RRStructure> UnpackStructure(RR_SHARED_PTR<MessageElementStructure> m) = 0;

		StructureStub(RR_SHARED_PTR<RobotRaconteurNode> node);

		RR_SHARED_PTR<RobotRaconteurNode> GetNode();
		RR_SHARED_PTR<RobotRaconteurNode> RRGetNode() { return GetNode(); }

		virtual ~StructureStub() {}
	protected:

		RR_WEAK_PTR<RobotRaconteurNode> node;

	};

	template<typename T>
	class CStructureStub
	{
		BOOST_STATIC_ASSERT(sizeof(T) == -1);
	};

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
			v = RRArrayToScalar<type>(MessageElement::FindElement(in, name)->CastData<RRArray<type> >()); \
		} \
	}; \
	\
	template<size_t N> \
	class CStructureStub<boost::container::static_vector<type, N> > \
	{ \
	public: \
		template<typename U> \
		static void PackField(const boost::container::static_vector<type, N>& v, const std::string& name, U& out) \
		{ \
			out.push_back(RR_MAKE_SHARED<MessageElement>(name, StaticVectorToRRArray<type>(v))); \
		} \
		\
		template<typename U> \
		static void UnpackField(boost::container::static_vector<type, N>& v, const std::string& name, U& in) \
		{ \
			RR_SHARED_PTR<RRArray<type> > a=MessageElement::FindElement(in, name)->CastData<RRArray<type> >(); \
			RRArrayToStaticVector(v, a); \
		} \
	}; \
	\
	template<size_t N> \
	class CStructureStub<boost::array<type, N> > \
	{ \
	public: \
		template<typename U> \
		static void PackField(const boost::array<type, N>& v, const std::string& name, U& out) \
		{ \
			out.push_back(RR_MAKE_SHARED<MessageElement>(name, ArrayToRRArray<type>(v))); \
		} \
		\
		template<typename U> \
		static void UnpackField(boost::array<type, N>& v, const std::string& name, U& in) \
		{ \
			RR_SHARED_PTR<RRArray<type> > a = MessageElement::FindElement(in, name)->CastData<RRArray<type> >(); \
			RRArrayToArray<type>(v, a); \
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

	template<typename T, size_t N>
	class CStructureStub<boost::container::static_vector<T, N> >
	{
	public:
		template<typename U>
		static void PackField(const boost::container::static_vector<T, N>& v, const std::string& name, U& out)
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
		static void UnpackField(boost::container::static_vector<T, N>& v, const std::string& name, U& in)
		{		
			RR_SHARED_PTR<MessageElementCStructureArray> a = MessageElement::FindElement(in, name)->CastData<MessageElementCStructureArray>();
			if (!a) throw NullValueException("Unexpected null array");
			if (a->Type != RRPrimUtil<T>::GetElementTypeString()) throw DataTypeException("CStructure data type mismatch");
			if (a->Elements.size() > N) throw OutOfRangeException("Array is too large for static vector size");
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

	template<typename T, size_t N>
	class CStructureStub<boost::array<T, N> >
	{
	public:
		template<typename U>
		static void PackField(const boost::array<T, N>& v, const std::string& name, U& out)
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
		static void UnpackField(boost::array<T, N>& v, const std::string& name, U& in)
		{
			RR_SHARED_PTR<MessageElementCStructureArray> a = MessageElement::FindElement(in, name)->CastData<MessageElementCStructureArray>();
			if (!a) throw NullValueException("Unexpected null array");
			if (a->Type != RRPrimUtil<T>::GetElementTypeString()) throw DataTypeException("CStructure data type mismatch");
			if (a->Elements.size() != N) throw OutOfRangeException("Array size mismatch");
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
		CStructureStub_UnpackCStructureFromArray(v, a);
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
}
