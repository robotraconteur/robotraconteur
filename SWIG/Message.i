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

//DataTypes



%shared_ptr(RobotRaconteur::MessageElementMap<int32_t>)
%shared_ptr(RobotRaconteur::MessageElementMap<std::string>)
%shared_ptr(RobotRaconteur::MessageElementList)



%shared_ptr(RobotRaconteur::Message)
%shared_ptr(RobotRaconteur::MessageEntry)
%shared_ptr(RobotRaconteur::MessageHeader)
%shared_ptr(RobotRaconteur::MessageElement)
%shared_ptr(RobotRaconteur::MessageElementStructure)
%shared_ptr(RobotRaconteur::MessageElementMultiDimArray)
%shared_ptr(RobotRaconteur::MessageElementCStructure)
%shared_ptr(RobotRaconteur::MessageElementCStructureArray)
%shared_ptr(RobotRaconteur::MessageElementCStructureMultiDimArray)
%shared_ptr(RobotRaconteur::MessageElementAStructureArray)
%shared_ptr(RobotRaconteur::MessageElementAStructureMultiDimArray)

//Message



%template(vectorptr_messageentry) std::vector<boost::shared_ptr<RobotRaconteur::MessageEntry> > ;
%template(vectorptr_messageelement) std::vector<boost::shared_ptr<RobotRaconteur::MessageElement> > ;


namespace RobotRaconteur
{

class TimeSpec;

class Message : public virtual RRValue
{
public:
	boost::shared_ptr<RobotRaconteur::MessageHeader> header;
	std::vector<boost::shared_ptr<RobotRaconteur::MessageEntry> > entries;
	Message();
	uint32_t ComputeSize();
	//void Write(ArrayBinaryWriter &w);
	boost::shared_ptr<RobotRaconteur::MessageEntry> FindEntry(const std::string& name);
	boost::shared_ptr<RobotRaconteur::MessageEntry> AddEntry(MessageEntryType t, const std::string& name);
	//void Read(ArrayBinaryReader &r);
};


struct MessageFragmentHeader
{
	uint32_t FragmentMessageNumber;
	uint32_t FragmentMessageSize;
	uint32_t FragmentOffset;
};

class MessageHeader : public virtual RRValue
{
public:
	uint16_t HeaderSize;
	uint16_t MessageFlags;
	uint16_t SubstreamID;
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
	uint16_t ComputeSize();
	//std::vector < boost::tuple<uint32_t, std::string> > StringTable; //Type not SWIG compatible?
	void UpdateHeader(uint32_t message_size, uint16_t entry_count);
	//void Write(ArrayBinaryWriter &w);
	//void Read(ArrayBinaryReader &r);
	MessageHeader();
};

class MessageElement;

class MessageEntry : public virtual RRValue
{
public:
	uint32_t EntrySize;
	uint8_t EntryFlags;
	MessageEntryType EntryType;
	std::string ServicePath;
	std::string MemberName;
	uint32_t MemberNameCode;
	uint32_t EntryStreamID;
	uint32_t RequestID;
	MessageErrorType Error;
	std::string MetaData;
	TimeSpec EntryTimeSpec;
	std::vector<boost::shared_ptr<RobotRaconteur::MessageElement> > elements;
	MessageEntry();
	MessageEntry(MessageEntryType t, const std::string& n);
	uint32_t ComputeSize();
	boost::shared_ptr<MessageElement> FindElement(const std::string& name);
	//boost::shared_ptr<MessageElement> AddElement(const std::string& name, boost::shared_ptr<RobotRaconteur::MessageElementData> data);
	boost::shared_ptr<MessageElement> AddElement(boost::shared_ptr<RobotRaconteur::MessageElement> m);
	//void Write(ArrayBinaryWriter &w);
	//void Read(ArrayBinaryReader &r);

};





class MessageElement : public virtual RRValue
{
public:
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
	MessageElement();
	//MessageElement(std::string name, boost::shared_ptr<RobotRaconteur::MessageElementData> datin);


	RR_MAKE_METHOD_PRIVATE(GetData);
	RR_MAKE_METHOD_PRIVATE(SetData);
	boost::shared_ptr<RobotRaconteur::MessageElementData> GetData();
	void SetData(boost::shared_ptr<RobotRaconteur::MessageElementData> value);

	uint32_t ComputeSize();
	void UpdateData();
	//void Write(ArrayBinaryWriter &w);
	//void Read(ArrayBinaryReader &r);
	static boost::shared_ptr<RobotRaconteur::MessageElement> FindElement(std::vector<boost::shared_ptr<RobotRaconteur::MessageElement> > &m, const std::string& name);
	static bool ContainsElement(std::vector<boost::shared_ptr<RobotRaconteur::MessageElement> > &m, const std::string& name);
};




class  MessageElementStructure : public virtual MessageElementData
{
public:

	std::string Type;
	std::vector<boost::shared_ptr<MessageElement> > Elements;
	MessageElementStructure(const std::string& type_, std::vector<boost::shared_ptr<MessageElement> > &elements_);
	virtual std::string GetTypeString();
	virtual DataTypes GetTypeID();
};

class MessageElementMultiDimArray  : public virtual MessageElementData
{
public:
	std::vector<boost::shared_ptr<MessageElement> > Elements;
	MessageElementMultiDimArray(std::vector<boost::shared_ptr<MessageElement> > &e);
	virtual std::string GetTypeString();
	virtual DataTypes GetTypeID();
};

template<class T>
class MessageElementMap : public virtual MessageElementData
{
public:
	std::vector<boost::shared_ptr<MessageElement> > Elements;
	MessageElementMap(std::vector<boost::shared_ptr<MessageElement> >& e);
	virtual std::string GetTypeString();
	virtual DataTypes GetTypeID();
};

class MessageElementList : public virtual MessageElementData
{
public:
	std::vector<boost::shared_ptr<MessageElement> > Elements;
	MessageElementList(std::vector<boost::shared_ptr<MessageElement> >& e);
	virtual std::string GetTypeString();
	virtual DataTypes GetTypeID();
};

class MessageElementCStructure : public MessageElementData
{
public:
	
	std::vector<boost::shared_ptr<MessageElement> > Elements;

	MessageElementCStructure(const std::vector<boost::shared_ptr<MessageElement> > &elements_);
	virtual std::string GetTypeString();		
	virtual DataTypes GetTypeID();		
};

class MessageElementCStructureArray : public MessageElementData
{
public:

	std::string Type;
	std::vector<boost::shared_ptr<MessageElement> > Elements;
	MessageElementCStructureArray(const std::string& type_, const std::vector<boost::shared_ptr<MessageElement> > &elements_);
	virtual std::string GetTypeString();	
	virtual DataTypes GetTypeID();	
};

class MessageElementCStructureMultiDimArray : public MessageElementData
{
public:
	std::string Type;
	std::vector<boost::shared_ptr<MessageElement> > Elements;
	MessageElementCStructureMultiDimArray(const std::string& type_, const std::vector<boost::shared_ptr<MessageElement> > &elements_);
	virtual std::string GetTypeString();	
	virtual DataTypes GetTypeID();	
};

class MessageElementAStructureArray : public MessageElementData
{
public:

	std::string Type;
	std::vector<boost::shared_ptr<MessageElement> > Elements;
	MessageElementAStructureArray(const std::string& type_, const std::vector<boost::shared_ptr<MessageElement> > &elements_);
	virtual std::string GetTypeString();	
	virtual DataTypes GetTypeID();	
};

class MessageElementAStructureMultiDimArray : public MessageElementData
{
public:
	std::string Type;
	std::vector<boost::shared_ptr<MessageElement> > Elements;
	MessageElementAStructureMultiDimArray(const std::string& type_, const std::vector<boost::shared_ptr<MessageElement> > &elements_);
	virtual std::string GetTypeString();	
	virtual DataTypes GetTypeID();	
};

}

%template(MessageElementMap_int32_t) RobotRaconteur::MessageElementMap<int32_t>;
%template(MessageElementMap_string) RobotRaconteur::MessageElementMap<std::string>;

%inline
{

class MessageElementDataUtil
{
public:
	static boost::shared_ptr<RobotRaconteur::RRBaseArray> ToRRBaseArray(boost::shared_ptr<RobotRaconteur::MessageElementData> m)
	{
		return boost::dynamic_pointer_cast<RRBaseArray>(m);
	}
	
	static boost::shared_ptr<RobotRaconteur::RRBaseArray> VoidToRRBaseArray(void* m)
	{
		return *reinterpret_cast<boost::shared_ptr<RRBaseArray>* >(m);
	}

	static boost::shared_ptr<RobotRaconteur::MessageElementStructure> ToMessageElementStructure(boost::shared_ptr<RobotRaconteur::MessageElementData> m)
	{
		return boost::dynamic_pointer_cast<MessageElementStructure>(m);
	}

	static boost::shared_ptr<RobotRaconteur::MessageElementMap<int32_t> > ToMessageElementMap_int32_t(boost::shared_ptr<RobotRaconteur::MessageElementData> m)
	{
		return boost::dynamic_pointer_cast<MessageElementMap<int32_t> >(m);
	}

	static boost::shared_ptr<RobotRaconteur::MessageElementMap<std::string> > ToMessageElementMap_string(boost::shared_ptr<RobotRaconteur::MessageElementData> m)
	{
		return boost::dynamic_pointer_cast<MessageElementMap<std::string> >(m);
	}

	static boost::shared_ptr<RobotRaconteur::MessageElementList > ToMessageElementList(boost::shared_ptr<RobotRaconteur::MessageElementData> m)
	{
		return boost::dynamic_pointer_cast<MessageElementList >(m);
	}
	
	static boost::shared_ptr<RobotRaconteur::MessageElementMultiDimArray> ToMessageElementMultiDimArray(boost::shared_ptr<RobotRaconteur::MessageElementData> m)
	{
		return boost::dynamic_pointer_cast<MessageElementMultiDimArray>(m);
	}
	
	static boost::shared_ptr<RobotRaconteur::MessageElementCStructure> ToMessageElementCStructure(boost::shared_ptr<RobotRaconteur::MessageElementData> m)
	{
		return boost::dynamic_pointer_cast<MessageElementCStructure>(m);
	}

	static boost::shared_ptr<RobotRaconteur::MessageElementCStructureArray> ToMessageElementCStructureArray(boost::shared_ptr<RobotRaconteur::MessageElementData> m)
	{
		return boost::dynamic_pointer_cast<MessageElementCStructureArray>(m);
	}

	static boost::shared_ptr<RobotRaconteur::MessageElementCStructureMultiDimArray> ToMessageElementCStructureMultiDimArray(boost::shared_ptr<RobotRaconteur::MessageElementData> m)
	{
		return boost::dynamic_pointer_cast<MessageElementCStructureMultiDimArray>(m);
	}

	static boost::shared_ptr<RobotRaconteur::MessageElementAStructureArray> ToMessageElementAStructureArray(boost::shared_ptr<RobotRaconteur::MessageElementData> m)
	{
		return boost::dynamic_pointer_cast<MessageElementAStructureArray>(m);
	}

	static boost::shared_ptr<RobotRaconteur::MessageElementAStructureMultiDimArray> ToMessageElementAStructureMultiDimArray(boost::shared_ptr<RobotRaconteur::MessageElementData> m)
	{
		return boost::dynamic_pointer_cast<MessageElementAStructureMultiDimArray>(m);
	}

#ifdef RR_MESSAEGE_ELEMENT_DATA_UTIL_EXTRAS
	static std::string RRBaseArrayToString(boost::shared_ptr<RobotRaconteur::RRBaseArray> rrarray)
	{
		return RRArrayToString(rr_cast<RRArray<char> >(rrarray));
	}
	
	static void RRBaseArrayToDoubles(boost::shared_ptr<RobotRaconteur::RRBaseArray> rrarray, double array[], int32_t len)
	{
		memcpy(array,rrarray->void_ptr(),len*sizeof(double));
	}
	
	static void RRBaseArrayToFloats(boost::shared_ptr<RobotRaconteur::RRBaseArray> rrarray, float array[], int32_t len)
	{
		memcpy(array,rrarray->void_ptr(),len*sizeof(float));
	}
	
	static void RRBaseArrayToBytes(boost::shared_ptr<RobotRaconteur::RRBaseArray> rrarray, int8_t array[], int32_t len)
	{
		
		memcpy(array,rrarray->void_ptr(),len*sizeof(int8_t));
		
	}
	
	static void RRBaseArrayToShorts(boost::shared_ptr<RobotRaconteur::RRBaseArray> rrarray, int16_t array[], int32_t len)
	{
		memcpy(array,rrarray->void_ptr(),len*sizeof(int16_t));
	}
	
	static void RRBaseArrayToInts(boost::shared_ptr<RobotRaconteur::RRBaseArray> rrarray, int32_t array[], int32_t len)
	{
		memcpy(array,rrarray->void_ptr(),len*sizeof(int32_t));
	}
	
	static void RRBaseArrayToLongs(boost::shared_ptr<RobotRaconteur::RRBaseArray> rrarray, int64_t array[], int32_t len)
	{
		memcpy(array,rrarray->void_ptr(),len*sizeof(int64_t));
	}
	
	static boost::shared_ptr<RobotRaconteur::RRBaseArray> stringToRRBaseArray(const std::string& str)
	{
		return stringToRRArray(str);
	}
	
	static boost::shared_ptr<RobotRaconteur::RRBaseArray> DoublesToRRBaseArray(double array[], int32_t len)
	{
		return AttachRRArrayCopy(array,len);
	}
	
	static boost::shared_ptr<RobotRaconteur::RRBaseArray> FloatsToRRBaseArray(float array[], int32_t len)
	{
		return AttachRRArrayCopy(array,len);
	}
	
	static boost::shared_ptr<RobotRaconteur::RRBaseArray>  BytesToRRBaseArray( int8_t array[], int32_t len, RobotRaconteur::DataTypes type)
	{
		if (type == DataTypes_int8_t) return AttachRRArrayCopy(array,len);
		if (type == DataTypes_uint8_t) return AttachRRArrayCopy((uint8_t*)array,len);
		throw DataTypeException("Invalid data type");
				
	}
	
	static boost::shared_ptr<RobotRaconteur::RRBaseArray>  ShortsToRRBaseArray( int16_t array[], int32_t len, RobotRaconteur::DataTypes type)
	{
		if (type == DataTypes_int16_t) return AttachRRArrayCopy(array,len);
		if (type == DataTypes_uint16_t) return AttachRRArrayCopy((uint16_t*)array,len);
		throw DataTypeException("Invalid data type");
				
	}
	
	static boost::shared_ptr<RobotRaconteur::RRBaseArray>  IntsToRRBaseArray( int32_t array[], int32_t len, RobotRaconteur::DataTypes type)
	{
		if (type == DataTypes_int32_t) return AttachRRArrayCopy(array,len);
		if (type == DataTypes_uint32_t) return AttachRRArrayCopy((uint32_t*)array,len);
		throw DataTypeException("Invalid data type");
				
	}
	
	static boost::shared_ptr<RobotRaconteur::RRBaseArray>  LongsToRRBaseArray( int64_t array[], int32_t len, RobotRaconteur::DataTypes type)
	{
		if (type == DataTypes_int64_t) return AttachRRArrayCopy(array,len);
		if (type == DataTypes_uint64_t) return AttachRRArrayCopy((uint64_t*)array,len);
		throw DataTypeException("Invalid data type");
				
	}
	
	static void DoublesToRRBaseArray(double array[], int32_t len, boost::shared_ptr<RobotRaconteur::RRBaseArray> rrarray)
	{
		if (rrarray->GetTypeID()!=DataTypes_double_t) throw InvalidArgumentException("RRArray error");
		if (rrarray->Length()!=len) throw InvalidArgumentException("RRArray error");
		memcpy(rrarray->void_ptr(),array,len*sizeof(double));
	}
	
	static void FloatsToRRBaseArray(float array[], int32_t len, boost::shared_ptr<RobotRaconteur::RRBaseArray> rrarray)
	{
		if (rrarray->GetTypeID()!=DataTypes_single_t) throw InvalidArgumentException("RRArray error");
		if (rrarray->Length()!=len) throw InvalidArgumentException("RRArray error");
		memcpy(rrarray->void_ptr(),array,len*sizeof(float));
	}
	
	static  void BytesToRRBaseArray( int8_t array[], int32_t len, boost::shared_ptr<RobotRaconteur::RRBaseArray> rrarray)
	{
		if (rrarray->GetTypeID()!=DataTypes_int8_t && rrarray->GetTypeID()!=DataTypes_uint8_t) throw InvalidArgumentException("RRArray error");
		if (rrarray->Length()!=len) throw InvalidArgumentException("RRArray error");
		memcpy(rrarray->void_ptr(),array,len*sizeof(int8_t));
				
	}
	
	static  void ShortsToRRBaseArray( int16_t array[], int32_t len, boost::shared_ptr<RobotRaconteur::RRBaseArray> rrarray)
	{
		if (rrarray->GetTypeID()!=DataTypes_int16_t && rrarray->GetTypeID()!=DataTypes_uint16_t) throw InvalidArgumentException("RRArray error");
		if (rrarray->Length()!=len) throw InvalidArgumentException("RRArray error");
		memcpy(rrarray->void_ptr(),array,len*sizeof(int16_t));
				
	}
	
	static void  IntsToRRBaseArray( int32_t array[], int32_t len, boost::shared_ptr<RobotRaconteur::RRBaseArray> rrarray)
	{
		if (rrarray->GetTypeID()!=DataTypes_int32_t && rrarray->GetTypeID()!=DataTypes_uint32_t) throw InvalidArgumentException("RRArray error");
		if (rrarray->Length()!=len) throw InvalidArgumentException("RRArray error");
		memcpy(rrarray->void_ptr(),array,len*sizeof(int32_t));
				
	}
	
	static void LongsToRRBaseArray( int64_t array[], int32_t len, boost::shared_ptr<RobotRaconteur::RRBaseArray> rrarray)
	{
		if (rrarray->GetTypeID()!=DataTypes_int64_t && rrarray->GetTypeID()!=DataTypes_uint64_t) throw InvalidArgumentException("RRArray error");
		if (rrarray->Length()!=len) throw InvalidArgumentException("RRArray error");
		memcpy(rrarray->void_ptr(),array,len*sizeof(int64_t));
				
	}
#endif
};

}