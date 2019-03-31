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
		virtual RR_INTRUSIVE_PTR<MessageElementStructure> PackStructure(RR_INTRUSIVE_PTR<RRValue> s) = 0;

		virtual RR_INTRUSIVE_PTR<RRStructure> UnpackStructure(RR_INTRUSIVE_PTR<MessageElementStructure> m) = 0;

		StructureStub(RR_SHARED_PTR<RobotRaconteurNode> node);

		RR_SHARED_PTR<RobotRaconteurNode> GetNode();
		RR_SHARED_PTR<RobotRaconteurNode> RRGetNode() { return GetNode(); }
		RR_WEAK_PTR<RobotRaconteurNode> RRGetNodeWeak() { return node; }

		virtual ~StructureStub() {}
	protected:

		RR_WEAK_PTR<RobotRaconteurNode> node;

	};

	// pod

	template<typename T>
	class PodStub
	{
		BOOST_STATIC_ASSERT(sizeof(T) == -1);
	};

	template<typename T, size_t N, bool varlength>
	class pod_field_array : public boost::array<T, N>
	{
	private:
		size_t len;		
	public:
		pod_field_array() : len(0) {}
		pod_field_array(size_t n) { resize(n); }
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
		pod_field_array<T, N, varlength>& operator= (const pod_field_array<T2, N, varlength>& rhs) {
			std::copy(rhs.begin(), rhs.end(), boost::array<T, N>::begin());
			this->len = rhs->len;
			return *this;
		}
		bool rangecheck(size_t i) const {
			return i > size() ? boost::throw_exception(std::out_of_range("array<>: index out of range")), true : true;
		}
	};

	template<typename T, size_t N>
	class pod_field_array<T, N, false> : public boost::array<T, N>
	{
	public:
		void resize(size_t n) { if (n != N) throw std::out_of_range("requested size does not match fixed array size"); }
		size_t max_size() { return N; }
	};

	template <typename T, size_t N, bool varlength>
	RR_INTRUSIVE_PTR<RRArray<T> > pod_field_array_ToRRArray(const pod_field_array<T, N, varlength>& i)
	{
		return AttachRRArrayCopy<T>(&i[0], i.size());
	}

	template <typename T, size_t N, bool varlength>
	void RRArrayTo_pod_field_array(pod_field_array<T, N, varlength>& v, const RR_INTRUSIVE_PTR<RRArray<T> >& i)
	{
		if (!i) throw NullValueException("Pod array must not be null");
		v.resize(i->size());
		memcpy(&v[0], i->data(), sizeof(T)*i->size());		
	}

#define RRPodStubNumberType(type) \
	template<> \
	class PodStub<type> \
	{ \
	public: \
		template<typename U> \
		static void PackField(const type& v, const std::string& name, U& out) \
		{ \
			out.push_back(CreateMessageElement(name, ScalarToRRArray<type>(v))); \
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
	class PodStub<pod_field_array<type, N, varlength> > \
	{ \
	public: \
		template<typename U> \
		static void PackField(const pod_field_array<type, N, varlength>& v, const std::string& name, U& out) \
		{ \
			out.push_back(CreateMessageElement(name, pod_field_array_ToRRArray(v))); \
		} \
		\
		template<typename U> \
		static void UnpackField(pod_field_array<type, N, varlength>& v, const std::string& name, U& in) \
		{ \
			RR_INTRUSIVE_PTR<RRArray<type> > a = MessageElement::FindElement(in, name)->template CastData<RRArray<type> >(); \
			RRArrayTo_pod_field_array(v, a); \
		} \
	}; \
	
	RRPodStubNumberType(double);
	RRPodStubNumberType(float);
	RRPodStubNumberType(int8_t);
	RRPodStubNumberType(uint8_t);
	RRPodStubNumberType(int16_t);
	RRPodStubNumberType(uint16_t);
	RRPodStubNumberType(int32_t);
	RRPodStubNumberType(uint32_t);
	RRPodStubNumberType(int64_t);
	RRPodStubNumberType(uint64_t);
		
	template <typename T, size_t N, bool varlength>
	RR_INTRUSIVE_PTR<RRNamedArray<T> > pod_field_array_ToRRNamedArray(const pod_field_array<T, N, varlength>& i)
	{
		typedef typename RRPrimUtil<T>::ElementArrayType element_type;
		typename RR_INTRUSIVE_PTR<RRArray<element_type> > a = AttachRRArrayCopy<element_type>((const element_type*)&i[0], i.size() * RRPrimUtil<T>::GetElementArrayCount());
		return new RRNamedArray<T>(a);
	}

	template <typename T, size_t N, bool varlength>
	void RRNamedArrayTo_pod_field_array(pod_field_array<T, N, varlength>& v, const RR_INTRUSIVE_PTR<RRNamedArray<T> >& i)
	{
		if (!i) throw NullValueException("Pod array must not be null");
		v.resize(i->size());
		memcpy(&v[0], i->GetNumericArray()->data(), sizeof(T)*i->size());
	}

#define RRPodStubNamedArrayType(type) \
	template<> \
	class PodStub<type> \
	{ \
	public: \
		template<typename U> \
		static void PackField(const type& v, const std::string& name, U& out) \
		{ \
			std::vector<RR_INTRUSIVE_PTR<MessageElement> > v1; \
			v1.push_back(CreateMessageElement("array",ScalarToRRNamedArray<type>(v)->GetNumericArray())); \
			out.push_back(CreateMessageElement(name, CreateMessageElementNamedArray(RRPrimUtil<type>::GetElementTypeString(),v1))); \
		} \
		\
		template<typename U> \
		static void UnpackField(type& v, const std::string& name, U& in) \
		{ \
			typedef typename RRPrimUtil<type>::ElementArrayType element_type; \
			RR_INTRUSIVE_PTR<MessageElementNamedArray> m = MessageElement::FindElement(in,name)->template CastData<MessageElementNamedArray>(); \
			if (m->Type != RRPrimUtil<type>::GetElementTypeString()) throw DataTypeException("Invalid namedarray"); \
			RR_INTRUSIVE_PTR<RRArray<element_type> > a=MessageElement::FindElement(m->Elements, "array")->CastData<RRArray<element_type> >(); \
			if (a->size() != RRPrimUtil<type>::GetElementArrayCount()) throw DataTypeException("Invalid namedarray"); \
			memcpy(&v, a->void_ptr(), sizeof(v)); \
		} \
	}; \
	\
	template<size_t N, bool varlength> \
	class PodStub<pod_field_array<type, N, varlength> > \
	{ \
	public: \
		template<typename U> \
		static void PackField(const pod_field_array<type, N, varlength>& v, const std::string& name, U& out) \
		{ \
			RR_INTRUSIVE_PTR<RRNamedArray<type> > a = pod_field_array_ToRRNamedArray(v); \
			std::vector<RR_INTRUSIVE_PTR<MessageElement> > a1; \
			a1.push_back(CreateMessageElement("array", a->GetNumericArray())); \
			out.push_back(CreateMessageElement(name, CreateMessageElementNamedArray(RRPrimUtil<type>::GetElementTypeString(), a1))); \
		} \
		\
		template<typename U> \
		static void UnpackField(pod_field_array<type, N, varlength>& v, const std::string& name, U& in) \
		{ \
			typedef RRPrimUtil<type>::ElementArrayType element_type; \
			RR_INTRUSIVE_PTR<MessageElementNamedArray> a = MessageElement::FindElement(in, name)->template CastData<MessageElementNamedArray>(); \
			RR_INTRUSIVE_PTR<RRArray<element_type> > a1 = MessageElement::FindElement(a->Elements, "array")->template CastData<RRArray<element_type> >(); \
			v.resize(a1->size() / RRPrimUtil<type>::GetElementArrayCount()); \
			memcpy(&v, a1->data(), a1->size() * sizeof(element_type)); \
		} \
	}; 

	template<typename T, size_t N, bool varlength>
	class PodStub<pod_field_array<T, N, varlength> >
	{
	public:
		template<typename U>
		static void PackField(const pod_field_array<T, N, varlength>& v, const std::string& name, U& out)
		{
			std::vector<RR_INTRUSIVE_PTR<MessageElement> > o;
			for (size_t j = 0; j < v.size(); j++)
			{
				RR_INTRUSIVE_PTR<MessageElement> m = CreateMessageElement("", PodStub<T>::PackToMessageElementPod(v[j]));
				m->ElementFlags &= ~MessageElementFlags_ELEMENT_NAME_STR;
				m->ElementFlags |= MessageElementFlags_ELEMENT_NUMBER;
				m->ElementNumber = j;
				o.push_back(m);
			}
			out.push_back(CreateMessageElement(name, CreateMessageElementPodArray(RRPrimUtil<T>::GetElementTypeString(), o)));
		}

		template<typename U>
		static void UnpackField(pod_field_array<T, N, varlength>& v, const std::string& name, U& in)
		{		
			RR_INTRUSIVE_PTR<MessageElementPodArray> a = MessageElement::FindElement(in, name)->template CastData<MessageElementPodArray>();
			if (!a) throw NullValueException("Unexpected null array");
			if (a->Type != RRPrimUtil<T>::GetElementTypeString()) throw DataTypeException("Pod data type mismatch");
			//if (a->Elements.size() > N) throw OutOfRangeException("Array is too large for static vector size");
			v.resize(a->Elements.size());
			for (int32_t i = 0; i<(int32_t)a->Elements.size(); i++)
			{
				RR_INTRUSIVE_PTR<MessageElement> m = a->Elements.at(i);
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
					throw DataTypeException("Invalid pod array format");
				}

				if (key != i) throw DataTypeException("Invalid pod array format");

				PodStub<T>::UnpackFromMessageElementPod(v[i], m->CastData<MessageElementPod>());
			}
		}
	};
		
	template<typename T, typename U>
	void PodStub_PackField(const T& v, const std::string& name, U& out)
	{
		PodStub<T>::PackField(v, name, out);
	}

	template<typename T, typename U>
	void PodStub_UnpackField(T& v, const std::string& name, U& out)
	{		
		PodStub<T>::UnpackField(v, name, out);
	}

	template<typename T>
	RR_INTRUSIVE_PTR<MessageElementPodArray> PodStub_PackPodToArray(const T& v)
	{
		std::vector<RR_INTRUSIVE_PTR<MessageElement> > o;
		
		RR_INTRUSIVE_PTR<MessageElement> m = CreateMessageElement("", PodStub<T>::PackToMessageElementPod(v));
		m->ElementFlags &= ~MessageElementFlags_ELEMENT_NAME_STR;
		m->ElementFlags |= MessageElementFlags_ELEMENT_NUMBER;
		m->ElementNumber = 0;
		o.push_back(m);
		
		return CreateMessageElementPodArray(RRPrimUtil<T>::GetElementTypeString(), o);		
	}

	template<typename T>
	void PodStub_UnpackPodFromArray(T& v, RR_INTRUSIVE_PTR<MessageElementPodArray> a)
	{
		if (!a) throw DataTypeException("Pod scalar array must not be null");
		if (a->Type != RRPrimUtil<T>::GetElementTypeString()) throw DataTypeException("Pod data type mismatch");
		if (a->Elements.size() != 1) throw DataTypeException("Invalid pod scalar array format");

		RR_INTRUSIVE_PTR<MessageElement> m = a->Elements.at(0);
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
			throw DataTypeException("Invalid pod scalar array format");
		}

		if (key != 0) throw DataTypeException("Invalid pod scalar array format");

		PodStub<T>::UnpackFromMessageElementPod(v, m->CastData<MessageElementPod>());
	}

	template<typename T>
	T PodStub_UnpackPodFromArray(RR_INTRUSIVE_PTR<MessageElementPodArray> a)
	{
		T v;
		PodStub_UnpackPodFromArray<T>(v, a);
		return v;
	}

	template<typename T>
	RR_INTRUSIVE_PTR<MessageElementPodArray> PodStub_PackPodArray(RR_INTRUSIVE_PTR<RRPodArray<T> > a)
	{
		if (!a) return RR_INTRUSIVE_PTR<MessageElementPodArray>();		
		std::vector<RR_INTRUSIVE_PTR<MessageElement> > o;
		for (size_t i = 0; i < a->size(); i++)
		{
			RR_INTRUSIVE_PTR<MessageElement> m = CreateMessageElement("", PodStub<T>::PackToMessageElementPod(a->at(i)));
			m->ElementFlags &= ~MessageElementFlags_ELEMENT_NAME_STR;
			m->ElementFlags |= MessageElementFlags_ELEMENT_NUMBER;
			m->ElementNumber = i;
			o.push_back(m);
		}
		return CreateMessageElementPodArray(RRPrimUtil<T>::GetElementTypeString(), o);
	}

	template<typename T>
	RR_INTRUSIVE_PTR<RRPodArray<T> > PodStub_UnpackPodArray(RR_INTRUSIVE_PTR<MessageElementPodArray> a)
	{
		if (!a) return RR_INTRUSIVE_PTR<RRPodArray<T> >();

		RR_INTRUSIVE_PTR<RRPodArray<T> > o = AllocateEmptyRRPodArray<T>(a->Elements.size());
		for (size_t i = 0; i < a->Elements.size(); i++)
		{
			RR_INTRUSIVE_PTR<MessageElement> m = a->Elements.at(i);
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
				throw DataTypeException("Invalid pod array format");
			}

			if (key != i) throw DataTypeException("Invalid pod array format");
			PodStub<T>::UnpackFromMessageElementPod(o->at(i), m->CastData<MessageElementPod>());
		}

		return o;		
	}

	template<typename T>
	RR_INTRUSIVE_PTR<MessageElementPodMultiDimArray> PodStub_PackPodMultiDimArray(RR_INTRUSIVE_PTR<RRPodMultiDimArray<T> > a)
	{
		if (!a) return RR_INTRUSIVE_PTR<MessageElementPodMultiDimArray>();

		std::vector<RR_INTRUSIVE_PTR<MessageElement> > m;
		m.push_back(CreateMessageElement("dims", a->Dims));
		if (!a->PodArray) throw NullValueException("Multidimarray array must not be null");
		m.push_back(CreateMessageElement("array", PodStub_PackPodArray(a->PodArray)));
		return CreateMessageElementPodMultiDimArray(RRPrimUtil<T>::GetElementTypeString(), m);		
	}

	template<typename T>
	RR_INTRUSIVE_PTR<RRPodMultiDimArray<T> > PodStub_UnpackPodMultiDimArray(RR_INTRUSIVE_PTR<MessageElementPodMultiDimArray> m)
	{
		if (!m) return RR_INTRUSIVE_PTR<RRPodMultiDimArray<T> >();

		RR_INTRUSIVE_PTR<RRPodMultiDimArray<T> > o = AllocateEmptyRRPodMultiDimArray<T>();
		o->Dims = (MessageElement::FindElement(m->Elements, "dims")->CastData<RRArray<uint32_t> >());
		o->PodArray = PodStub_UnpackPodArray<T>(MessageElement::FindElement(m->Elements, "array")->CastData<MessageElementPodArray>());
		if (!o->PodArray) throw NullValueException("Multidimarray array must not be null");
		return o;
	}


	//MessageElement pack helper functions for pod
	template<typename T>
	RR_INTRUSIVE_PTR<MessageElement> MessageElement_PackPodToArrayElement(const std::string& elementname, const T& s)
	{
		return CreateMessageElement(elementname, PodStub_PackPodToArray(s));
	}

	template<typename T>
	RR_INTRUSIVE_PTR<MessageElement> MessageElement_PackPodArrayElement(const std::string& elementname, const RR_INTRUSIVE_PTR<RRPodArray<T> >& s)
	{
		if (!s) throw NullValueException("Arrays must not be null");
		return CreateMessageElement(elementname, PodStub_PackPodArray(s));
	}

	template<typename T>
	RR_INTRUSIVE_PTR<MessageElement> MessageElement_PackPodMultiDimArrayElement(const std::string& elementname, const RR_INTRUSIVE_PTR<RRPodMultiDimArray<T> >& s)
	{
		if (!s) throw NullValueException("Arrays must not be null");
		return CreateMessageElement(elementname, PodStub_PackPodMultiDimArray(s));
	}

	//MessageElement unpack helper functions for pod
	template<typename T>
	T MessageElement_UnpackPodFromArray(const RR_INTRUSIVE_PTR<MessageElement>& m)
	{
		return RobotRaconteur::PodStub_UnpackPodFromArray<T>(m->CastData<RobotRaconteur::MessageElementPodArray>());
	}

	template<typename T>
	RR_INTRUSIVE_PTR<RRPodArray<T> > MessageElement_UnpackPodArray(const RR_INTRUSIVE_PTR<MessageElement>& m)
	{
		RR_INTRUSIVE_PTR<RRPodArray<T> > a = RobotRaconteur::PodStub_UnpackPodArray<T>(m->CastData<RobotRaconteur::MessageElementPodArray>());
		if (!a) throw NullValueException("Arrays must not be null");
		return a;
	}

	template<typename T>
	RR_INTRUSIVE_PTR<RRPodMultiDimArray<T> > MessageElement_UnpackPodMultiDimArray(const RR_INTRUSIVE_PTR<MessageElement>& m)
	{
		RR_INTRUSIVE_PTR<RRPodMultiDimArray<T> > a = RobotRaconteur::PodStub_UnpackPodMultiDimArray<T>(m->CastData<RobotRaconteur::MessageElementPodMultiDimArray>());
		if (!a) throw NullValueException("Arrays must not be null");
		return a;
	}

	template<typename T>
	static RR_INTRUSIVE_PTR<RRPodArray<T> > VerifyRRArrayLength(RR_INTRUSIVE_PTR<RRPodArray<T> > a, size_t len, bool varlength)
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

	template<size_t Ndims, typename T>
	static RR_INTRUSIVE_PTR<RRPodMultiDimArray<T> > VerifyRRMultiDimArrayLength(RR_INTRUSIVE_PTR<RRPodMultiDimArray<T> > a, size_t n_elems, boost::array<uint32_t, Ndims> dims)
	{
		if (!a) throw NullValueException("Arrays must not be null");

		if (a->Dims->size() != Ndims)
		{
			throw DataTypeException("Array dimension mismatch");
		}

		if (a->PodArray->size() != n_elems)
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


	// namedarray

	template<typename T>
	RR_INTRUSIVE_PTR<MessageElementNamedArray> NamedArrayStub_PackNamedArrayToArray(const T& v)
	{
		typedef typename RRPrimUtil<T>::ElementArrayType element_type;
		RR_INTRUSIVE_PTR<RRArray<element_type> > a = AllocateRRArray<element_type>(RRPrimUtil<T>::GetElementArrayCount());
		memcpy(a->void_ptr(), &v, sizeof(T));
		std::vector<RR_INTRUSIVE_PTR<MessageElement> > a1;
		a1.push_back(CreateMessageElement("array", a));
		return CreateMessageElementNamedArray(RRPrimUtil<T>::GetElementTypeString(), a1);

	}

	template<typename T>
	void NamedArrayStub_UnpackNamedArrayFromArray(T& v, RR_INTRUSIVE_PTR<MessageElementNamedArray> a)
	{
		typedef typename RRPrimUtil<T>::ElementArrayType element_type;
		if (!a) throw DataTypeException("NamedArray scalar array must not be null");
		if (a->Type != RRPrimUtil<T>::GetElementTypeString()) throw DataTypeException("NamedArray data type mismatch");
		if (a->Elements.size() != 1) throw DataTypeException("Invalid namedarray array format");
		typename RR_INTRUSIVE_PTR<RRArray<element_type> > a1 = MessageElement::FindElement(a->Elements, "array")->template CastData<RRArray<element_type> >();
		if (a1->size() != sizeof(T) / sizeof(element_type)) throw DataTypeException("Invalid scalar namedarray array format");

		v = *((T*)a1->void_ptr());
	}

	template<typename T>
	T NamedArrayStub_UnpackNamedArrayFromArray(RR_INTRUSIVE_PTR<MessageElementNamedArray> a)
	{
		T o;
		NamedArrayStub_UnpackNamedArrayFromArray(o, a);
		return o;
	}

	template<typename T>
	RR_INTRUSIVE_PTR<MessageElementNamedArray> NamedArrayStub_PackNamedArray(RR_INTRUSIVE_PTR<RRNamedArray<T> > a)
	{
		if (!a) return RR_INTRUSIVE_PTR<MessageElementNamedArray>();
		std::vector<RR_INTRUSIVE_PTR<MessageElement> > a1;
		a1.push_back(CreateMessageElement("array",a->GetNumericArray()));
		return CreateMessageElementNamedArray(RRPrimUtil<T>::GetElementTypeString(), a1);
	}

	template<typename T>
	RR_INTRUSIVE_PTR<RRNamedArray<T> > NamedArrayStub_UnpackNamedArray(RR_INTRUSIVE_PTR<MessageElementNamedArray> a)
	{
		typedef typename RRPrimUtil<T>::ElementArrayType element_type;
		if (!a) return RR_INTRUSIVE_PTR<RRNamedArray<T> >();
		if (a->Type != RRPrimUtil<T>::GetElementTypeString()) throw DataTypeException("Invalid namedarray type");
		typename RR_INTRUSIVE_PTR<RRArray<element_type> > a2 = MessageElement::FindElement(a->Elements, "array")->CastData<RRArray<element_type> >();
		return new RRNamedArray<T>(a2);
	}

	template<typename T>
	RR_INTRUSIVE_PTR<MessageElementNamedMultiDimArray> NamedArrayStub_PackNamedMultiDimArray(RR_INTRUSIVE_PTR<RRNamedMultiDimArray<T> > a)
	{
		if (!a) return RR_INTRUSIVE_PTR<MessageElementNamedMultiDimArray>();

		std::vector<RR_INTRUSIVE_PTR<MessageElement> > m;
		m.push_back(CreateMessageElement("dims", a->Dims));
		if (!a->NamedArray) throw NullValueException("Multidimarray array must not be null");
		m.push_back(CreateMessageElement("array", NamedArrayStub_PackNamedArray(a->NamedArray)));
		return CreateMessageElementNamedMultiDimArray(RRPrimUtil<T>::GetElementTypeString(), m);
	}

	template<typename T>
	RR_INTRUSIVE_PTR<RRNamedMultiDimArray<T> > NamedArrayStub_UnpackNamedMultiDimArray(RR_INTRUSIVE_PTR<MessageElementNamedMultiDimArray> m)
	{
		if (!m) return RR_INTRUSIVE_PTR<RRNamedMultiDimArray<T> >();

		typename RR_INTRUSIVE_PTR<RRNamedMultiDimArray<T> > o = AllocateEmptyRRNamedMultiDimArray<T>();
		o->Dims = (MessageElement::FindElement(m->Elements, "dims")->CastData<RRArray<uint32_t> >());
		o->NamedArray = NamedArrayStub_UnpackNamedArray<T>(MessageElement::FindElement(m->Elements, "array")->CastData<MessageElementNamedArray>());
		if (!o->NamedArray) throw NullValueException("Multidimarray array must not be null");
		return o;
	}

	//MessageElement pack helper functions for namedarray
	template<typename T>
	RR_INTRUSIVE_PTR<MessageElement> MessageElement_PackNamedArrayToArrayElement(const std::string& elementname, const T& s)
	{
		return CreateMessageElement(elementname, NamedArrayStub_PackNamedArrayToArray(s));
	}

	template<typename T>
	RR_INTRUSIVE_PTR<MessageElement> MessageElement_PackNamedArrayElement(const std::string& elementname, const RR_INTRUSIVE_PTR<RRNamedArray<T> >& s)
	{
		if (!s) throw NullValueException("Arrays must not be null");
		return CreateMessageElement(elementname, NamedArrayStub_PackNamedArray(s));
	}

	template<typename T>
	RR_INTRUSIVE_PTR<MessageElement> MessageElement_PackNamedMultiDimArrayElement(const std::string& elementname, const RR_INTRUSIVE_PTR<RRNamedMultiDimArray<T> >& s)
	{
		if (!s) throw NullValueException("Arrays must not be null");
		return CreateMessageElement(elementname, NamedArrayStub_PackNamedMultiDimArray(s));
	}

	//MessageElement unpack helper functions for namedarray
	template<typename T>
	T MessageElement_UnpackNamedArrayFromArray(const RR_INTRUSIVE_PTR<MessageElement>& m)
	{
		return RobotRaconteur::NamedArrayStub_UnpackNamedArrayFromArray<T>(m->CastData<RobotRaconteur::MessageElementNamedArray>());
	}

	template<typename T>
	RR_INTRUSIVE_PTR<RRNamedArray<T> > MessageElement_UnpackNamedArray(const RR_INTRUSIVE_PTR<MessageElement>& m)
	{
		RR_INTRUSIVE_PTR<RRNamedArray<T> > a = RobotRaconteur::NamedArrayStub_UnpackNamedArray<T>(m->CastData<RobotRaconteur::MessageElementNamedArray>());
		if (!a) throw NullValueException("Arrays must not be null");
		return a;
	}

	template<typename T>
	RR_INTRUSIVE_PTR<RRNamedMultiDimArray<T> > MessageElement_UnpackNamedMultiDimArray(const RR_INTRUSIVE_PTR<MessageElement>& m)
	{
		RR_INTRUSIVE_PTR<RRNamedMultiDimArray<T> > a = RobotRaconteur::NamedArrayStub_UnpackNamedMultiDimArray<T>(m->CastData<RobotRaconteur::MessageElementNamedMultiDimArray>());
		if (!a) throw NullValueException("Arrays must not be null");
		return a;
	}

	template<typename T>
	static RR_INTRUSIVE_PTR<RRNamedArray<T> > VerifyRRArrayLength(RR_INTRUSIVE_PTR<RRNamedArray<T> > a, size_t len, bool varlength)
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

	template<size_t Ndims, typename T>
	static RR_INTRUSIVE_PTR<RRNamedMultiDimArray<T> > VerifyRRMultiDimArrayLength(RR_INTRUSIVE_PTR<RRNamedMultiDimArray<T> > a, size_t n_elems, boost::array<uint32_t, Ndims> dims)
	{
		if (!a) throw NullValueException("Arrays must not be null");

		if (a->Dims->size() != Ndims)
		{
			throw DataTypeException("Array dimension mismatch");
		}

		if (a->NamedArray->size() != n_elems)
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
