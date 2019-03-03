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
#include "RobotRaconteur/NodeID.h"





namespace RobotRaconteur
{

	class ROBOTRACONTEUR_CORE_API ArrayBinaryReader;
	class ROBOTRACONTEUR_CORE_API ArrayBinaryWriter;
	class ROBOTRACONTEUR_CORE_API NodeID;

	class ROBOTRACONTEUR_CORE_API Message;
	class ROBOTRACONTEUR_CORE_API MessageHeader;
	class ROBOTRACONTEUR_CORE_API MessageEntry;
	class ROBOTRACONTEUR_CORE_API MessageElement;

	class ROBOTRACONTEUR_CORE_API Message : public RRValue
	{
	public:
		RR_INTRUSIVE_PTR<MessageHeader> header;
		std::vector<RR_INTRUSIVE_PTR<MessageEntry> > entries;

		Message();
		

		RR_INTRUSIVE_PTR<MessageEntry> FindEntry(const std::string& name);

		RR_INTRUSIVE_PTR<MessageEntry> AddEntry(MessageEntryType t, const std::string& name);

		//Version 2 Message
		uint32_t ComputeSize();
		void Write(ArrayBinaryWriter &w);
		void Read(ArrayBinaryReader &r);

		//Version 3 Message
		uint32_t ComputeSize3();
		void Write3(ArrayBinaryWriter &w, const uint16_t& version_minor);
		void Read3(ArrayBinaryReader &r, uint16_t& version_minor);

		virtual std::string RRType()
		{
			return "RobotRaconteur::Message";
		}
	};

	struct MessageFragmentHeader
	{
		uint32_t FragmentMessageNumber;
		uint32_t FragmentMessageSize;
		uint32_t FragmentOffset;
	};

	struct SubstreamSequenceNumberHeader
	{
		uint32_t SequenceNumber;
		uint32_t RecvSequenceNumber;
	};

	class ROBOTRACONTEUR_CORE_API MessageHeader : public RRValue
	{
	public:

		virtual ~MessageHeader() {}

		uint32_t HeaderSize;

		uint16_t MessageFlags;
		
		uint16_t SubstreamID;

		SubstreamSequenceNumberHeader SubstreamSequenceNumber;

		MessageFragmentHeader FragmentHeader;
				
		uint32_t SenderEndpoint;

		uint32_t ReceiverEndpoint;

		std::string SenderNodeName;

		std::string ReceiverNodeName;

		NodeID SenderNodeID;

		NodeID ReceiverNodeID;

		std::string MetaData;

		uint16_t EntryCount;

		uint16_t MessageID;

		uint16_t MessageResID;

		uint32_t MessageSize;

		std::vector < boost::tuple<uint32_t, std::string> > StringTable;

		TimeSpec UnreliableExpiration;

		uint16_t Priority;

		std::vector<uint8_t> TransportSpecific;

		//Version 2 Message
		uint16_t ComputeSize();
		void UpdateHeader(uint32_t message_size, uint16_t entry_count);
		void Write(ArrayBinaryWriter &w);
		void Read(ArrayBinaryReader &r);

		//Version 3 Message
	protected:
		uint32_t ComputeSize3();
	public:
		void UpdateHeader3(uint32_t message_entry_size, uint16_t entry_count);
		void Write3(ArrayBinaryWriter &w, const uint16_t& version_minor);
		void Read3(ArrayBinaryReader &r, uint16_t& version_minor);

		virtual std::string RRType()
		{
			return "RobotRaconteur::MessageHeader";
		}	

public:
	MessageHeader();
		
	};


	class ROBOTRACONTEUR_CORE_API MessageEntry : public RRValue
	{
	public:
		virtual ~MessageEntry() {}

		uint32_t EntrySize;

		uint8_t EntryFlags;

		MessageEntryType EntryType;

		std::string ServicePath;

		uint32_t ServicePathCode;

		std::string MemberName;

		uint32_t MemberNameCode;

		uint32_t EntryStreamID;

		uint32_t RequestID;

		MessageErrorType Error;

		std::string MetaData;

		TimeSpec EntryTimeSpec;

		std::vector<RR_INTRUSIVE_PTR<MessageElement> > elements;

		MessageEntry();

		MessageEntry(MessageEntryType t, const std::string& n);

		RR_INTRUSIVE_PTR<MessageElement> FindElement(const std::string& name);

		bool TryFindElement(const std::string& name, RR_INTRUSIVE_PTR<MessageElement>& elem);

		RR_INTRUSIVE_PTR<MessageElement> AddElement(const std::string& name, RR_INTRUSIVE_PTR<MessageElementData> data);

		RR_INTRUSIVE_PTR<MessageElement> AddElement(RR_INTRUSIVE_PTR<MessageElement> m);

		//Version 2 Message
		uint32_t ComputeSize();
		void UpdateData();
		void Write(ArrayBinaryWriter &w);		
		void Read(ArrayBinaryReader &r);

		//Version 3 Message
		uint32_t ComputeSize3();
		void UpdateData3();
		void Write3(ArrayBinaryWriter &w, const uint16_t& version_minor);
		void Read3(ArrayBinaryReader &r, const uint16_t& version_minor);

		virtual std::string RRType()
		{
			return "RobotRaconteur::MessageEntry";
		}			
	};

	

	class ROBOTRACONTEUR_CORE_API MessageElement : public RRValue
	{
	public:

		virtual ~MessageElement() {}

		uint32_t ElementSize;

		uint8_t ElementFlags;

		std::string ElementName;

		uint32_t ElementNameCode;

		int32_t ElementNumber;

		DataTypes ElementType;

		std::string ElementTypeName;

		uint32_t ElementTypeNameCode;

		uint32_t SequenceNumber;

		std::string MetaData;

		uint32_t DataCount;

	private:
		RR_INTRUSIVE_PTR<MessageElementData> dat;
		
	public:
		MessageElement();
				
		MessageElement(const std::string& name, RR_INTRUSIVE_PTR<MessageElementData> datin);
				
		RR_INTRUSIVE_PTR<MessageElementData> GetData();
		void SetData(RR_INTRUSIVE_PTR<MessageElementData> value);

		//Version 2 Message
		uint32_t ComputeSize();
		void UpdateData();
		void Write(ArrayBinaryWriter &w);
		void Read(ArrayBinaryReader &r);

		//Version 3 Message
		uint32_t ComputeSize3();
		void UpdateData3();
		void Write3(ArrayBinaryWriter &w, const uint16_t& version_minor);
		void Read3(ArrayBinaryReader &r, const uint16_t& version_minor);

		static RR_INTRUSIVE_PTR<MessageElement> FindElement(std::vector<RR_INTRUSIVE_PTR<MessageElement> > &m, const std::string& name);

		static bool TryFindElement(std::vector<RR_INTRUSIVE_PTR<MessageElement> > &m, const std::string& name, RR_INTRUSIVE_PTR<MessageElement>& elem);

		static bool ContainsElement(std::vector<RR_INTRUSIVE_PTR<MessageElement> > &m, const std::string& name);

		template<typename T>
		RR_INTRUSIVE_PTR<T> CastData()
		{
			return rr_cast<T>(dat);
		}

		std::string CastDataToString();

		template<typename T>
		static RR_INTRUSIVE_PTR<T> CastData(RR_INTRUSIVE_PTR<MessageElementData> Data)
		{
			return rr_cast<T>(Data);
		}

		virtual std::string RRType()
		{
			return "RobotRaconteur::MessageElement";
		}
	
	};

	class ROBOTRACONTEUR_CORE_API MessageElementStructure : public MessageElementData
	{
	public:

		std::string Type;
		std::vector<RR_INTRUSIVE_PTR<MessageElement> > Elements;

		MessageElementStructure(const std::string& type_, const std::vector<RR_INTRUSIVE_PTR<MessageElement> > &elements_);

		virtual std::string GetTypeString()
		{
			return Type;
		}
		virtual DataTypes GetTypeID()
		{
			return DataTypes_structure_t;
		}
		virtual std::string RRType()
		{
			return "RobotRaconteur::CMessageElementStructure";
		}

	};

	
	template<typename T>
	class MessageElementMap : public MessageElementData
	{
	public:
		std::vector<RR_INTRUSIVE_PTR<MessageElement> > Elements;


		MessageElementMap(const std::vector<RR_INTRUSIVE_PTR<MessageElement> >& e)
		{
			Elements=e;
		}

		virtual std::string GetTypeString()
		{
			if (boost::is_same<T,int32_t>::value)
			{
				return "varvalue{int32}";
			}

			if (boost::is_same<T,std::string>::value)
			{
				return "varvalue{string}";
			}
			throw DataTypeException("Invalid set keytype");
		}
		virtual DataTypes GetTypeID()
		{
			if (boost::is_same<T,int32_t>::value)
			{
				return DataTypes_vector_t;
			}

			if (boost::is_same<T,std::string>::value)
			{
				return DataTypes_dictionary_t;
			}
			throw DataTypeException("Invalid object keytype");	
		}
		virtual std::string RRType()
		{
			return "RobotRaconteur::CMessageElementStructure";
		}



	};

	
	class ROBOTRACONTEUR_CORE_API MessageElementList : public MessageElementData
	{
	public:
		std::vector<RR_INTRUSIVE_PTR<MessageElement> > Elements;


		MessageElementList(const std::vector<RR_INTRUSIVE_PTR<MessageElement> >& e)
		{
			Elements=e;
		}

		virtual std::string GetTypeString()
		{
			return "varvalue{list}";
		}
		virtual DataTypes GetTypeID()
		{
			return DataTypes_list_t;	
		}
		virtual std::string RRType()
		{
			return "RobotRaconteur::CMessageElementStructure";
		}



	};

	class ROBOTRACONTEUR_CORE_API MessageElementMultiDimArray : public MessageElementData
	{
	public:


		std::vector<RR_INTRUSIVE_PTR<MessageElement> > Elements;

		virtual ~MessageElementMultiDimArray() {}

		MessageElementMultiDimArray(const std::vector<RR_INTRUSIVE_PTR<MessageElement> > &e);

		virtual std::string GetTypeString()
		{
			DataTypes rrt=GetTypeID();

			
			std::string elementtype=GetRRDataTypeString(rrt);

			return elementtype + "[*]";

		}
		virtual DataTypes GetTypeID()
		{
			return DataTypes_multidimarray_t;
		}
		virtual std::string RRType()
		{
			return "RobotRaconteur::MessageElementMultiDimArray";
		}
	};

	class ROBOTRACONTEUR_CORE_API MessageElementPod : public MessageElementData
	{
	public:
				
		std::vector<RR_INTRUSIVE_PTR<MessageElement> > Elements;

		MessageElementPod(const std::vector<RR_INTRUSIVE_PTR<MessageElement> > &elements_);

		virtual std::string GetTypeString()
		{
			return "";
		}
		virtual DataTypes GetTypeID()
		{
			return DataTypes_pod_t;
		}
		virtual std::string RRType()
		{
			return "RobotRaconteur::MessageElementPod";
		}
	};

	class ROBOTRACONTEUR_CORE_API MessageElementPodArray : public MessageElementData
	{
	public:

		std::string Type;
		std::vector<RR_INTRUSIVE_PTR<MessageElement> > Elements;

		MessageElementPodArray(const std::string& type_, const std::vector<RR_INTRUSIVE_PTR<MessageElement> > &elements_);

		virtual std::string GetTypeString()
		{
			return Type;
		}
		virtual DataTypes GetTypeID()
		{
			return DataTypes_pod_array_t;
		}
		virtual std::string RRType()
		{
			return "RobotRaconteur::MessageElementPodArray";
		}
	};

	class ROBOTRACONTEUR_CORE_API MessageElementPodMultiDimArray : public MessageElementData
	{
	public:

		std::string Type;
		std::vector<RR_INTRUSIVE_PTR<MessageElement> > Elements;

		MessageElementPodMultiDimArray(const std::string& type_, const std::vector<RR_INTRUSIVE_PTR<MessageElement> > &elements_);

		virtual std::string GetTypeString()
		{
			return Type;
		}
		virtual DataTypes GetTypeID()
		{
			return DataTypes_pod_multidimarray_t;
		}
		virtual std::string RRType()
		{
			return "RobotRaconteur::MessageElementPodMultiDimArray";
		}
	};

	class ROBOTRACONTEUR_CORE_API MessageElementNamedArray : public MessageElementData
	{
	public:

		std::string Type;
		std::vector<RR_INTRUSIVE_PTR<MessageElement> > Elements;

		MessageElementNamedArray(const std::string& type_, const std::vector<RR_INTRUSIVE_PTR<MessageElement> > &elements_);

		virtual std::string GetTypeString()
		{
			return Type;
		}
		virtual DataTypes GetTypeID()
		{
			return DataTypes_namedarray_array_t;
		}
		virtual std::string RRType()
		{
			return "RobotRaconteur::MessageElementNamedArray";
		}
	};

	class ROBOTRACONTEUR_CORE_API MessageElementNamedMultiDimArray : public MessageElementData
	{
	public:

		std::string Type;
		std::vector<RR_INTRUSIVE_PTR<MessageElement> > Elements;

		MessageElementNamedMultiDimArray(const std::string& type_, const std::vector<RR_INTRUSIVE_PTR<MessageElement> > &elements_);

		virtual std::string GetTypeString()
		{
			return Type;
		}
		virtual DataTypes GetTypeID()
		{
			return DataTypes_namedarray_multidimarray_t;
		}
		virtual std::string RRType()
		{
			return "RobotRaconteur::MessageElementNamedMultiDimArray";
		}
	};


	ROBOTRACONTEUR_CORE_API RR_INTRUSIVE_PTR<Message> CreateMessage();
	ROBOTRACONTEUR_CORE_API RR_INTRUSIVE_PTR<MessageHeader> CreateMessageHeader();
	ROBOTRACONTEUR_CORE_API RR_INTRUSIVE_PTR<MessageEntry> CreateMessageEntry();
	ROBOTRACONTEUR_CORE_API RR_INTRUSIVE_PTR<MessageEntry> CreateMessageEntry(MessageEntryType t, const std::string& n);
	ROBOTRACONTEUR_CORE_API RR_INTRUSIVE_PTR<MessageElement> CreateMessageElement();
	ROBOTRACONTEUR_CORE_API RR_INTRUSIVE_PTR<MessageElement> CreateMessageElement(const std::string& name, RR_INTRUSIVE_PTR<MessageElementData> datin);
	ROBOTRACONTEUR_CORE_API RR_INTRUSIVE_PTR<MessageElementStructure> CreateMessageElementStructure(const std::string& type_, const std::vector<RR_INTRUSIVE_PTR<MessageElement> > &elements_);
	template <typename T>
	ROBOTRACONTEUR_CORE_API RR_INTRUSIVE_PTR<MessageElementMap<T> > CreateMessageElementMap(const std::vector<RR_INTRUSIVE_PTR<MessageElement> >& e)
	{
		return new MessageElementMap<T>(e);
	}
	ROBOTRACONTEUR_CORE_API RR_INTRUSIVE_PTR<MessageElementMultiDimArray> CreateMessageElementMultiDimArray(const std::vector<RR_INTRUSIVE_PTR<MessageElement> > &e);
	ROBOTRACONTEUR_CORE_API RR_INTRUSIVE_PTR<MessageElementList> CreateMessageElementList(const std::vector<RR_INTRUSIVE_PTR<MessageElement> >& e);
	ROBOTRACONTEUR_CORE_API RR_INTRUSIVE_PTR<MessageElementPod> CreateMessageElementPod(const std::vector<RR_INTRUSIVE_PTR<MessageElement> > &elements_);
	ROBOTRACONTEUR_CORE_API RR_INTRUSIVE_PTR<MessageElementPodArray> CreateMessageElementPodArray(const std::string& type_, const std::vector<RR_INTRUSIVE_PTR<MessageElement> > &elements_);
	ROBOTRACONTEUR_CORE_API RR_INTRUSIVE_PTR<MessageElementPodMultiDimArray> CreateMessageElementPodMultiDimArray(const std::string& type_, const std::vector<RR_INTRUSIVE_PTR<MessageElement> > &elements_);
	ROBOTRACONTEUR_CORE_API RR_INTRUSIVE_PTR<MessageElementNamedArray> CreateMessageElementNamedArray(const std::string& type_, const std::vector<RR_INTRUSIVE_PTR<MessageElement> > &elements_);
	ROBOTRACONTEUR_CORE_API RR_INTRUSIVE_PTR<MessageElementNamedMultiDimArray> CreateMessageElementNamedMultiDimArray(const std::string& type_, const std::vector<RR_INTRUSIVE_PTR<MessageElement> > &elements_);


	ROBOTRACONTEUR_CORE_API RR_INTRUSIVE_PTR<Message> ShallowCopyMessage(RR_INTRUSIVE_PTR<Message> m);
	ROBOTRACONTEUR_CORE_API RR_INTRUSIVE_PTR<MessageEntry> ShallowCopyMessageEntry(RR_INTRUSIVE_PTR<MessageEntry> mm);
	ROBOTRACONTEUR_CORE_API RR_INTRUSIVE_PTR<MessageElement> ShallowCopyMessageElement(RR_INTRUSIVE_PTR<MessageElement> mm);
	
	
	
	//MessageElement packing functions
	template<typename T>
	RR_INTRUSIVE_PTR<MessageElementData> MessageElement_PackScalar(const T& value)
	{
		return RobotRaconteur::ScalarToRRArray<T>(value);
	}

	template<typename T>
	RR_INTRUSIVE_PTR<MessageElement> MessageElement_PackScalarElement(const std::string& elementname, const T& value)
	{
		return CreateMessageElement(elementname, MessageElement_PackScalar(value));
	}

	template<typename T>
	RR_INTRUSIVE_PTR<MessageElementData> MessageElement_PackArray(const RR_INTRUSIVE_PTR<RRArray<T> >& value)
	{
		if (!value) throw NullValueException("Arrays must not be null");
		return value;
	}

	template<typename T>
	RR_INTRUSIVE_PTR<MessageElement> MessageElement_PackArrayElement(const std::string& elementname, const RR_INTRUSIVE_PTR<RRArray<T> >& value)
	{
		return CreateMessageElement(elementname, MessageElement_PackArray(value));
	}

	template<typename T, typename N>
	RR_INTRUSIVE_PTR<MessageElementData> MessageElement_PackMultiDimArray(RR_WEAK_PTR<N> node, const RR_INTRUSIVE_PTR<RRMultiDimArray<T> >& value)
	{
		if (!value) throw NullValueException("Arrays must not be null");
		RR_SHARED_PTR<N> node1 = node.lock();
		if (!node1) throw InvalidOperationException("Node has been released");
		return node1->template PackMultiDimArray<T>(RobotRaconteur::rr_cast<RobotRaconteur::RRMultiDimArray<T> >(value));
	}

	template<typename T, typename N>
	RR_INTRUSIVE_PTR<MessageElement> MessageElement_PackMultiDimArrayElement(RR_WEAK_PTR<N> node, const std::string& elementname, const RR_INTRUSIVE_PTR<RRMultiDimArray<T> >& value)
	{
		return CreateMessageElement(elementname, MessageElement_PackMultiDimArray(node, value));
	}

	template<typename T>
	RR_INTRUSIVE_PTR<MessageElementData> MessageElement_PackString(const T& str)
	{
		return RobotRaconteur::stringToRRArray(str);
	}

	template<typename T>
	RR_INTRUSIVE_PTR<MessageElement> MessageElement_PackStringElement(const std::string& elementname, const T& str)
	{
		return CreateMessageElement(elementname, MessageElement_PackString(str));
	}

	template<typename T, typename N>
	RR_INTRUSIVE_PTR<MessageElementData> MessageElement_PackVarType(RR_WEAK_PTR<N> node, const RR_INTRUSIVE_PTR<T>& s)
	{
		RR_SHARED_PTR<N> node1 = node.lock();
		if (!node1) throw InvalidOperationException("Node has been released");
		return node1->PackVarType(s);
	}

	template<typename T, typename N>
	RR_INTRUSIVE_PTR<MessageElement> MessageElement_PackVarTypeElement(RR_WEAK_PTR<N> node, const std::string& elementname, const RR_INTRUSIVE_PTR<T>& s)
	{
		return CreateMessageElement(elementname, MessageElement_PackVarType(node, s));
	}

	template<typename T, typename N>
	RR_INTRUSIVE_PTR<MessageElementData> MessageElement_PackStruct(RR_WEAK_PTR<N> node, const RR_INTRUSIVE_PTR<T>& s)
	{
		RR_SHARED_PTR<N> node1 = node.lock();
		if (!node1) throw InvalidOperationException("Node has been released");
		return node1->PackStructure(RobotRaconteur::rr_cast<RobotRaconteur::RRStructure>(s));
	}

	template<typename T, typename N>
	RR_INTRUSIVE_PTR<MessageElement> MessageElement_PackStructElement(RR_WEAK_PTR<N> node, const std::string& elementname, const RR_INTRUSIVE_PTR<T>& s)
	{
		return CreateMessageElement(elementname, MessageElement_PackStruct(node, s));
	}

	template<typename T>
	RR_INTRUSIVE_PTR<MessageElementData> MessageElement_PackEnum(const T& value)
	{
		return RobotRaconteur::ScalarToRRArray<int32_t>((int32_t)value);
	}

	template<typename T>
	RR_INTRUSIVE_PTR<MessageElement> MessageElement_PackEnumElement(const std::string& elementname, const T& value)
	{
		return CreateMessageElement(elementname, MessageElement_PackEnum(value));
	}
		
	template<typename K, typename T, typename N>
	RR_INTRUSIVE_PTR<MessageElementData> MessageElement_PackMap(RR_WEAK_PTR<N> node, const RR_INTRUSIVE_PTR<RRMap<K, T> >& m)
	{
		RR_SHARED_PTR<N> node1 = node.lock();
		if (!node1) throw InvalidOperationException("Node has been released");
		return node1->template PackMapType<K, T>(m);
	}

	template<typename K, typename T, typename N>
	RR_INTRUSIVE_PTR<MessageElement> MessageElement_PackMapElement(RR_WEAK_PTR<N> node, const std::string& elementname, const RR_INTRUSIVE_PTR<RRMap<K, T> >& m)
	{
		return CreateMessageElement(elementname, MessageElement_PackMap(node, m));			
	}

	template<typename T, typename N>
	RR_INTRUSIVE_PTR<MessageElementData> MessageElement_PackList(RR_WEAK_PTR<N> node, const RR_INTRUSIVE_PTR<RRList<T> >& m)
	{
		RR_SHARED_PTR<N> node1 = node.lock();
		if (!node1) throw InvalidOperationException("Node has been released");
		return node1->template PackListType<T>(m);
	}

	template<typename T, typename N>
	RR_INTRUSIVE_PTR<MessageElement> MessageElement_PackListElement(RR_WEAK_PTR<N> node, const std::string& elementname, const RR_INTRUSIVE_PTR<RRList<T> >& m)
	{
		return CreateMessageElement(elementname, MessageElement_PackList(node, m));
	}

	//MessageElement unpacking functions
	template<typename T>
	T MessageElement_UnpackScalar(const RR_INTRUSIVE_PTR<MessageElement>& m)
	{
		return RRArrayToScalar<T>(m->CastData<RRArray<T> >());
	}

	template<typename T>
	RR_INTRUSIVE_PTR<RRArray<T> > MessageElement_UnpackArray(const RR_INTRUSIVE_PTR<MessageElement>& m)
	{
		RR_INTRUSIVE_PTR<RRArray<T> > a = (m->CastData<RRArray<T> >());
		if (!a) throw NullValueException("Arrays must not be null");
		return a;
	}

	template<typename T, typename N>
	RR_INTRUSIVE_PTR<RRMultiDimArray<T> > MessageElement_UnpackMultiDimArray(RR_WEAK_PTR<N> node, const RR_INTRUSIVE_PTR<MessageElement>& m)
	{
		RR_SHARED_PTR<N> node1 = node.lock();
		if (!node1) throw InvalidOperationException("Node has been released");
		RR_INTRUSIVE_PTR<RRMultiDimArray<T> > a= RobotRaconteur::rr_cast<RRMultiDimArray<T> >(node1->template UnpackMultiDimArray<T>(m->CastData<MessageElementMultiDimArray>()));		
		if (!a) throw NullValueException("Arrays must not be null");
		return a;
	}

	static std::string MessageElement_UnpackString(const RR_INTRUSIVE_PTR<MessageElement>& m)
	{
		return RRArrayToString(m->CastData<RRArray<char> >());
	}

	template<typename T, typename N>
	RR_INTRUSIVE_PTR<T> MessageElement_UnpackStructure(RR_WEAK_PTR<N> node, const RR_INTRUSIVE_PTR<MessageElement>& m)
	{
		RR_SHARED_PTR<N> node1 = node.lock();
		if (!node1) throw InvalidOperationException("Node has been released");
		return RobotRaconteur::rr_cast<T>(node1->UnpackStructure(m->CastData<RobotRaconteur::MessageElementStructure>()));
	}

	template<typename T>
	T MessageElement_UnpackEnum(const RR_INTRUSIVE_PTR<MessageElement>& m)
	{
		return (T)RRArrayToScalar<int32_t>(m->CastData<RRArray<int32_t> >());
	}
		
	template<typename N>
	RR_INTRUSIVE_PTR<RRValue> MessageElement_UnpackVarValue(RR_WEAK_PTR<N> node, const RR_INTRUSIVE_PTR<MessageElement>& m)
	{
		RR_SHARED_PTR<N> node1 = node.lock();
		if (!node1) throw InvalidOperationException("Node has been released");
		return node1->UnpackVarType(m);
	}

	template<typename K, typename T, typename N>
	RR_INTRUSIVE_PTR<RRMap<K, T> > MessageElement_UnpackMap(RR_WEAK_PTR<N> node, const RR_INTRUSIVE_PTR<MessageElement>& m)
	{
		RR_SHARED_PTR<N> node1 = node.lock();
		if (!node1) throw InvalidOperationException("Node has been released");
		return node1->template UnpackMapType<K,T>(m->CastData<RobotRaconteur::MessageElementMap<K> >());
	}

	template<typename T, typename N>
	RR_INTRUSIVE_PTR<RRList<T> > MessageElement_UnpackList(RR_WEAK_PTR<N> node, const RR_INTRUSIVE_PTR<MessageElement>& m)
	{
		RR_SHARED_PTR<N> node1 = node.lock();
		if (!node1) throw InvalidOperationException("Node has been released");
		return node1->template UnpackListType<T>(m->CastData<RobotRaconteur::MessageElementList>());
	}
}
