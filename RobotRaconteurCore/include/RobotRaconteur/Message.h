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
		RR_SHARED_PTR<MessageHeader> header;
		std::vector<RR_SHARED_PTR<MessageEntry> > entries;

		Message();
		

		RR_SHARED_PTR<MessageEntry> FindEntry(const std::string& name);

		RR_SHARED_PTR<MessageEntry> AddEntry(MessageEntryType t, const std::string& name);

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

		std::vector<RR_SHARED_PTR<MessageElement> > elements;

		MessageEntry();

		MessageEntry(MessageEntryType t, const std::string& n);

		RR_SHARED_PTR<MessageElement> FindElement(const std::string& name);

		bool TryFindElement(const std::string& name, RR_SHARED_PTR<MessageElement>& elem);

		RR_SHARED_PTR<MessageElement> AddElement(const std::string& name, RR_SHARED_PTR<MessageElementData> data);

		RR_SHARED_PTR<MessageElement> AddElement(RR_SHARED_PTR<MessageElement> m);

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
		RR_SHARED_PTR<MessageElementData> dat;
		
	public:
		MessageElement();
				
		MessageElement(const std::string& name, RR_SHARED_PTR<MessageElementData> datin);
				
		RR_SHARED_PTR<MessageElementData> GetData();
		void SetData(RR_SHARED_PTR<MessageElementData> value);

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

		static RR_SHARED_PTR<MessageElement> FindElement(std::vector<RR_SHARED_PTR<MessageElement> > &m, const std::string& name);

		static bool TryFindElement(std::vector<RR_SHARED_PTR<MessageElement> > &m, const std::string& name, RR_SHARED_PTR<MessageElement>& elem);

		static bool ContainsElement(std::vector<RR_SHARED_PTR<MessageElement> > &m, const std::string& name);

		template<typename T>
		RR_SHARED_PTR<T> CastData()
		{
			return rr_cast<T>(dat);
		}

		std::string CastDataToString();

		template<typename T>
		static RR_SHARED_PTR<T> CastData(RR_SHARED_PTR<MessageElementData> Data)
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
		std::vector<RR_SHARED_PTR<MessageElement> > Elements;

		MessageElementStructure(const std::string& type_, const std::vector<RR_SHARED_PTR<MessageElement> > &elements_);

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
		std::vector<RR_SHARED_PTR<MessageElement> > Elements;


		MessageElementMap(const std::vector<RR_SHARED_PTR<MessageElement> >& e)
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
		std::vector<RR_SHARED_PTR<MessageElement> > Elements;


		MessageElementList(const std::vector<RR_SHARED_PTR<MessageElement> >& e)
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


		std::vector<RR_SHARED_PTR<MessageElement> > Elements;

		virtual ~MessageElementMultiDimArray() {}

		MessageElementMultiDimArray(const std::vector<RR_SHARED_PTR<MessageElement> > &e);

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

	class ROBOTRACONTEUR_CORE_API MessageElementCStructure : public MessageElementData
	{
	public:
				
		std::vector<RR_SHARED_PTR<MessageElement> > Elements;

		MessageElementCStructure(const std::vector<RR_SHARED_PTR<MessageElement> > &elements_);

		virtual std::string GetTypeString()
		{
			return "";
		}
		virtual DataTypes GetTypeID()
		{
			return DataTypes_cstructure_t;
		}
		virtual std::string RRType()
		{
			return "RobotRaconteur::MessageElementCStructure";
		}
	};

	class ROBOTRACONTEUR_CORE_API MessageElementCStructureArray : public MessageElementData
	{
	public:

		std::string Type;
		std::vector<RR_SHARED_PTR<MessageElement> > Elements;

		MessageElementCStructureArray(const std::string& type_, const std::vector<RR_SHARED_PTR<MessageElement> > &elements_);

		virtual std::string GetTypeString()
		{
			return Type;
		}
		virtual DataTypes GetTypeID()
		{
			return DataTypes_cstructure_array_t;
		}
		virtual std::string RRType()
		{
			return "RobotRaconteur::MessageElementCStructureArray";
		}
	};

	class ROBOTRACONTEUR_CORE_API MessageElementCStructureMultiDimArray : public MessageElementData
	{
	public:

		std::string Type;
		std::vector<RR_SHARED_PTR<MessageElement> > Elements;

		MessageElementCStructureMultiDimArray(const std::string& type_, const std::vector<RR_SHARED_PTR<MessageElement> > &elements_);

		virtual std::string GetTypeString()
		{
			return Type;
		}
		virtual DataTypes GetTypeID()
		{
			return DataTypes_cstructure_multidimarray_t;
		}
		virtual std::string RRType()
		{
			return "RobotRaconteur::MessageElementCStructureMultiDimArray";
		}
	};

	ROBOTRACONTEUR_CORE_API RR_SHARED_PTR<Message> ShallowCopyMessage(RR_SHARED_PTR<Message> m);
	ROBOTRACONTEUR_CORE_API RR_SHARED_PTR<MessageEntry> ShallowCopyMessageEntry(RR_SHARED_PTR<MessageEntry> mm);
	ROBOTRACONTEUR_CORE_API RR_SHARED_PTR<MessageElement> ShallowCopyMessageElement(RR_SHARED_PTR<MessageElement> mm);
	

}
