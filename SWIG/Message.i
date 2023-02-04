// Copyright 2011-2020 Wason Technology, LLC
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



%rr_intrusive_ptr(RobotRaconteur::Message)
%rr_intrusive_ptr(RobotRaconteur::MessageEntry)
%rr_intrusive_ptr(RobotRaconteur::MessageHeader)
%rr_intrusive_ptr(RobotRaconteur::MessageElement)
%rr_intrusive_ptr(RobotRaconteur::MessageElementNestedElementList)

//Message



%template(vectorptr_messageentry) std::vector<boost::intrusive_ptr<RobotRaconteur::MessageEntry> > ;
%template(vectorptr_messageelement) std::vector<boost::intrusive_ptr<RobotRaconteur::MessageElement> > ;


namespace RobotRaconteur
{

class TimeSpec;

class Message : public virtual RRValue
{
public:
	boost::intrusive_ptr<RobotRaconteur::MessageHeader> header;
	std::vector<boost::intrusive_ptr<RobotRaconteur::MessageEntry> > entries;
	Message();
	uint32_t ComputeSize();
	//void Write(ArrayBinaryWriter &w);
	boost::intrusive_ptr<RobotRaconteur::MessageEntry> FindEntry(const std::string& name);
	boost::intrusive_ptr<RobotRaconteur::MessageEntry> AddEntry(MessageEntryType t, const std::string& name);
	//void Read(ArrayBinaryReader &r);
};


class MessageHeader : public virtual RRValue
{
public:
	uint16_t HeaderSize;
	uint8_t MessageFlags;
	uint32_t SenderEndpoint;
	uint32_t ReceiverEndpoint;
	MessageStringPtr SenderNodeName;
	MessageStringPtr ReceiverNodeName;
	NodeID SenderNodeID;
	NodeID ReceiverNodeID;
	MessageStringPtr MetaData;
	uint16_t EntryCount;
	uint16_t MessageID;
	uint16_t MessageResID;
	uint32_t MessageSize;
	uint16_t Priority;
	std::vector<uint8_t> Extended;
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
	MessageStringPtr ServicePath;
	MessageStringPtr MemberName;
	uint32_t MemberNameCode;
	uint32_t RequestID;
	MessageErrorType Error;
	MessageStringPtr MetaData;
	std::vector<uint8_t> Extended;
	std::vector<boost::intrusive_ptr<RobotRaconteur::MessageElement> > elements;
	MessageEntry();
	MessageEntry(MessageEntryType t, const std::string& n);
	uint32_t ComputeSize();
	boost::intrusive_ptr<MessageElement> FindElement(const std::string& name);
	//boost::intrusive_ptr<MessageElement> AddElement(const std::string& name, const boost::intrusive_ptr<RobotRaconteur::MessageElementData>& data);
	boost::intrusive_ptr<MessageElement> AddElement(const boost::intrusive_ptr<RobotRaconteur::MessageElement>& m);
	//void Write(ArrayBinaryWriter &w);
	//void Read(ArrayBinaryReader &r);

};





class MessageElement : public virtual RRValue
{
public:
	uint32_t ElementSize;
	uint8_t ElementFlags;
	MessageStringPtr ElementName;
	uint32_t ElementNameCode;
	int32_t ElementNumber;
	DataTypes ElementType;
	MessageStringPtr ElementTypeName;
	uint32_t ElementTypeNameCode;
	MessageStringPtr MetaData;
	std::vector<uint8_t> Extended;
	uint32_t DataCount;
	MessageElement();
	//MessageElement(const std::string& name, const boost::intrusive_ptr<RobotRaconteur::MessageElementData>& datin);


	RR_MAKE_METHOD_PRIVATE(GetData);
	RR_MAKE_METHOD_PRIVATE(SetData);
	boost::intrusive_ptr<RobotRaconteur::MessageElementData> GetData();
	void SetData(const boost::intrusive_ptr<RobotRaconteur::MessageElementData>& value);

	uint32_t ComputeSize();
	void UpdateData();
	//void Write(ArrayBinaryWriter &w);
	//void Read(ArrayBinaryReader &r);
	static boost::intrusive_ptr<RobotRaconteur::MessageElement> FindElement(std::vector<boost::intrusive_ptr<RobotRaconteur::MessageElement> > &m, const std::string& name);
	static bool ContainsElement(std::vector<boost::intrusive_ptr<RobotRaconteur::MessageElement> > &m, const std::string& name);
};

class MessageElementNestedElementList : public MessageElementData
{
public:

	DataTypes Type;
	MessageStringPtr TypeName;
	std::vector<boost::intrusive_ptr<MessageElement> > Elements;
	
	MessageElementNestedElementList(DataTypes type_, const std::string& type_name_, const std::vector<boost::intrusive_ptr<MessageElement> > &elements_);

	virtual MessageStringPtr GetTypeString();
	virtual DataTypes GetTypeID();		
	//virtual boost::string_ref RRType();
};

}


%inline
{

class MessageElementDataUtil
{
public:
	static boost::intrusive_ptr<RobotRaconteur::RRBaseArray> ToRRBaseArray(const boost::intrusive_ptr<RobotRaconteur::MessageElementData>& m)
	{
		return boost::dynamic_pointer_cast<RRBaseArray>(m);
	}
		
	static boost::intrusive_ptr<RobotRaconteur::MessageElementNestedElementList> ToMessageElementNestedElementList(const boost::intrusive_ptr<RobotRaconteur::MessageElementData>& m)
	{
		return boost::dynamic_pointer_cast<MessageElementNestedElementList>(m);
	}


#ifdef RR_MESSAEGE_ELEMENT_DATA_UTIL_EXTRAS
	static std::string RRBaseArrayToString(const boost::intrusive_ptr<RobotRaconteur::RRBaseArray>& rrarray)
	{
		return RRArrayToString(rr_cast<RRArray<char> >(rrarray));
	}
	
	static void RRBaseArrayToDoubles(const boost::intrusive_ptr<RobotRaconteur::RRBaseArray>& rrarray, double* array_out, int32_t len)
	{
		if (rrarray->GetTypeID()!=DataTypes_double_t) throw InvalidArgumentException("RRArray error");
		if (rrarray->size()!=len) throw InvalidArgumentException("RRArray error");
		memcpy(array_out,rrarray->void_ptr(),len*sizeof(double));
	}
	
	static void RRBaseArrayToFloats(const boost::intrusive_ptr<RobotRaconteur::RRBaseArray>& rrarray, float* array_out, int32_t len)
	{
		if (rrarray->GetTypeID()!=DataTypes_single_t) throw InvalidArgumentException("RRArray error");
		if (rrarray->size()!=len) throw InvalidArgumentException("RRArray error");
		memcpy(array_out,rrarray->void_ptr(),len*sizeof(float));
	}
	
	static void RRBaseArrayToBytes(const boost::intrusive_ptr<RobotRaconteur::RRBaseArray>& rrarray, int8_t* array_out, int32_t len)
	{
		if (rrarray->GetTypeID()!=DataTypes_int8_t && rrarray->GetTypeID()!=DataTypes_uint8_t) throw InvalidArgumentException("RRArray error");
		if (rrarray->size()!=len) throw InvalidArgumentException("RRArray error");
		memcpy(array_out,rrarray->void_ptr(),len*sizeof(int8_t));
		
	}
	
	static void RRBaseArrayToBytes(const boost::intrusive_ptr<RobotRaconteur::RRBaseArray>& rrarray, uint8_t* array_out, int32_t len)
	{
		if (rrarray->GetTypeID()!=DataTypes_uint8_t) throw InvalidArgumentException("RRArray error");
		if (rrarray->size()!=len) throw InvalidArgumentException("RRArray error");
		memcpy(array_out,rrarray->void_ptr(),len*sizeof(uint8_t));		
	}
	
	static void RRBaseArrayToShorts(const boost::intrusive_ptr<RobotRaconteur::RRBaseArray>& rrarray, int16_t* array_out, int32_t len)
	{
		if (rrarray->GetTypeID()!=DataTypes_int16_t && rrarray->GetTypeID()!=DataTypes_uint16_t) throw InvalidArgumentException("RRArray error");
		if (rrarray->size()!=len) throw InvalidArgumentException("RRArray error");
		memcpy(array_out,rrarray->void_ptr(),len*sizeof(int16_t));
	}
	
	static void RRBaseArrayToShorts(const boost::intrusive_ptr<RobotRaconteur::RRBaseArray>& rrarray, uint16_t* array_out, int32_t len)
	{
		if (rrarray->GetTypeID()!=DataTypes_uint16_t) throw InvalidArgumentException("RRArray error");
		if (rrarray->size()!=len) throw InvalidArgumentException("RRArray error");
		memcpy(array_out,rrarray->void_ptr(),len*sizeof(uint16_t));
	}
	
	static void RRBaseArrayToInts(const boost::intrusive_ptr<RobotRaconteur::RRBaseArray>& rrarray, int32_t* array_out, int32_t len)
	{
		if (rrarray->GetTypeID()!=DataTypes_int32_t && rrarray->GetTypeID()!=DataTypes_uint32_t) throw InvalidArgumentException("RRArray error");
		if (rrarray->size()!=len) throw InvalidArgumentException("RRArray error");
		memcpy(array_out,rrarray->void_ptr(),len*sizeof(int32_t));
	}
	
	static void RRBaseArrayToInts(const boost::intrusive_ptr<RobotRaconteur::RRBaseArray>& rrarray, uint32_t* array_out, int32_t len)
	{
		if (rrarray->GetTypeID()!=DataTypes_uint32_t) throw InvalidArgumentException("RRArray error");
		if (rrarray->size()!=len) throw InvalidArgumentException("RRArray error");
		memcpy(array_out,rrarray->void_ptr(),len*sizeof(uint32_t));
	}
	
	static void RRBaseArrayToLongs(const boost::intrusive_ptr<RobotRaconteur::RRBaseArray>& rrarray, int64_t* array_out, int32_t len)
	{
		if (rrarray->GetTypeID()!=DataTypes_int64_t && rrarray->GetTypeID()!=DataTypes_uint64_t) throw InvalidArgumentException("RRArray error");
		if (rrarray->size()!=len) throw InvalidArgumentException("RRArray error");
		memcpy(array_out,rrarray->void_ptr(),len*sizeof(int64_t));
	}
	
#ifndef SWIGJAVA
	static void RRBaseArrayToLongs(const boost::intrusive_ptr<RobotRaconteur::RRBaseArray>& rrarray, uint64_t* array_out, int32_t len)
	{
		if (rrarray->GetTypeID()!=DataTypes_uint64_t) throw InvalidArgumentException("RRArray error");
		if (rrarray->size()!=len) throw InvalidArgumentException("RRArray error");
		memcpy(array_out,rrarray->void_ptr(),len*sizeof(uint64_t));
	}
#endif
	static void RRBaseArrayComplexToDoubles(const boost::intrusive_ptr<RobotRaconteur::RRBaseArray>& rrarray, double* array_out, int32_t len)
	{
		if (rrarray->GetTypeID()!=DataTypes_cdouble_t) throw InvalidArgumentException("RRArray error");
		if (rrarray->size()!=len/2) throw InvalidArgumentException("RRArray error");
		memcpy(array_out,rrarray->void_ptr(),len*sizeof(double));
	}
	
	static void RRBaseArrayComplexToFloats(const boost::intrusive_ptr<RobotRaconteur::RRBaseArray>& rrarray, float* array_out, int32_t len)
	{
		if (rrarray->GetTypeID()!=DataTypes_csingle_t) throw InvalidArgumentException("RRArray error");
		if (rrarray->size()!=len/2) throw InvalidArgumentException("RRArray error");
		memcpy(array_out,rrarray->void_ptr(),len*sizeof(float));
	}
	
	static void RRBaseArrayBoolToBytes(const boost::intrusive_ptr<RobotRaconteur::RRBaseArray>& rrarray, uint8_t* array_out, int32_t len)
	{
		if (rrarray->GetTypeID()!=DataTypes_bool_t) throw InvalidArgumentException("RRArray error");
		if (rrarray->size()!=len) throw InvalidArgumentException("RRArray error");
		memcpy(array_out,rrarray->void_ptr(),len*sizeof(uint8_t));
	}
	
	static void RRBaseArrayBoolToBytes(const boost::intrusive_ptr<RobotRaconteur::RRBaseArray>& rrarray, int8_t* array_out, int32_t len)
	{
		if (rrarray->GetTypeID()!=DataTypes_bool_t) throw InvalidArgumentException("RRArray error");
		if (rrarray->size()!=len) throw InvalidArgumentException("RRArray error");
		memcpy(array_out,rrarray->void_ptr(),len*sizeof(int8_t));
	}
	
	static boost::intrusive_ptr<RobotRaconteur::RRBaseArray> stringToRRBaseArray(const std::string& str)
	{
		return stringToRRArray(str);
	}
	
	static boost::intrusive_ptr<RobotRaconteur::RRBaseArray> DoublesToRRBaseArray(double* array_in, int32_t len)
	{
		return AttachRRArrayCopy(array_in,len);
	}
	
	static boost::intrusive_ptr<RobotRaconteur::RRBaseArray> FloatsToRRBaseArray(float* array_in, int32_t len)
	{
		return AttachRRArrayCopy(array_in,len);
	}
	
	static boost::intrusive_ptr<RobotRaconteur::RRBaseArray>  BytesToRRBaseArray( int8_t* array_in, int32_t len, RobotRaconteur::DataTypes type)
	{
		if (type == DataTypes_int8_t) return AttachRRArrayCopy(array_in,len);
		if (type == DataTypes_uint8_t) return AttachRRArrayCopy((uint8_t*)array_in,len);
		throw DataTypeException("Invalid data type");
				
	}
	
	static boost::intrusive_ptr<RobotRaconteur::RRBaseArray>  BytesToRRBaseArray( uint8_t* array_in, int32_t len)
	{
		return AttachRRArrayCopy(array_in,len);				
	}
	
	static boost::intrusive_ptr<RobotRaconteur::RRBaseArray>  ShortsToRRBaseArray( int16_t* array_in, int32_t len, RobotRaconteur::DataTypes type)
	{
		if (type == DataTypes_int16_t) return AttachRRArrayCopy(array_in,len);
		if (type == DataTypes_uint16_t) return AttachRRArrayCopy((uint16_t*)array_in,len);
		throw DataTypeException("Invalid data type");				
	}
	
	static boost::intrusive_ptr<RobotRaconteur::RRBaseArray>  ShortsToRRBaseArray( uint16_t* array_in, int32_t len)
	{
		return AttachRRArrayCopy(array_in,len);						
	}
	
	static boost::intrusive_ptr<RobotRaconteur::RRBaseArray>  IntsToRRBaseArray( int32_t* array_in, int32_t len, RobotRaconteur::DataTypes type)
	{
		if (type == DataTypes_int32_t) return AttachRRArrayCopy(array_in,len);
		if (type == DataTypes_uint32_t) return AttachRRArrayCopy((uint32_t*)array_in,len);
		throw DataTypeException("Invalid data type");
				
	}
	
	static boost::intrusive_ptr<RobotRaconteur::RRBaseArray>  IntsToRRBaseArray( uint32_t* array_in, int32_t len)
	{
		return AttachRRArrayCopy(array_in,len);				
	}
	
	static boost::intrusive_ptr<RobotRaconteur::RRBaseArray>  LongsToRRBaseArray( int64_t* array_in, int32_t len, RobotRaconteur::DataTypes type)
	{
		if (type == DataTypes_int64_t) return AttachRRArrayCopy(array_in,len);
		if (type == DataTypes_uint64_t) return AttachRRArrayCopy((uint64_t*)array_in,len);
		throw DataTypeException("Invalid data type");
				
	}
#ifndef SWIGJAVA
	static boost::intrusive_ptr<RobotRaconteur::RRBaseArray>  LongsToRRBaseArray( uint64_t* array_in, int32_t len)
	{
		return AttachRRArrayCopy(array_in,len);		
				
	}
#endif
	static boost::intrusive_ptr<RobotRaconteur::RRBaseArray> DoublesToComplexRRBaseArray(double* array_in, int32_t len)
	{
		if (len %2 != 0) throw InvalidArgumentException("Array must be even length");
		return AttachRRArrayCopy((cdouble*)array_in,len/2);
	}
	
	static boost::intrusive_ptr<RobotRaconteur::RRBaseArray> FloatsToComplexRRBaseArray(float* array_in, int32_t len)
	{
		if (len %2 != 0) throw InvalidArgumentException("Array must be even length");
		return AttachRRArrayCopy((cfloat*)array_in,len/2);
	}
	
	static boost::intrusive_ptr<RobotRaconteur::RRBaseArray> BytesToBoolRRBaseArray(uint8_t* array_in, int32_t len)
	{
		return AttachRRArrayCopy((rr_bool*)array_in,len);
	}
	
	static boost::intrusive_ptr<RobotRaconteur::RRBaseArray> BytesToBoolRRBaseArray(int8_t* array_in, int32_t len)
	{
		return AttachRRArrayCopy((rr_bool*)array_in,len);
	}
			
	static void DoublesToRRBaseArray(double* array_in, int32_t len,const boost::intrusive_ptr<RobotRaconteur::RRBaseArray>& rrarray)
	{
		if (rrarray->GetTypeID()!=DataTypes_double_t) throw InvalidArgumentException("RRArray error");
		if (rrarray->size()!=len) throw InvalidArgumentException("RRArray error");
		memcpy(rrarray->void_ptr(),array_in,len*sizeof(double));
	}
	
	static void FloatsToRRBaseArray(float* array_in, int32_t len,const boost::intrusive_ptr<RobotRaconteur::RRBaseArray>& rrarray)
	{
		if (rrarray->GetTypeID()!=DataTypes_single_t) throw InvalidArgumentException("RRArray error");
		if (rrarray->size()!=len) throw InvalidArgumentException("RRArray error");
		memcpy(rrarray->void_ptr(),array_in,len*sizeof(float));
	}
	
	static  void BytesToRRBaseArray( int8_t* array_in, int32_t len,const boost::intrusive_ptr<RobotRaconteur::RRBaseArray>& rrarray)
	{
		if (rrarray->GetTypeID()!=DataTypes_int8_t && rrarray->GetTypeID()!=DataTypes_uint8_t) throw InvalidArgumentException("RRArray error");
		if (rrarray->size()!=len) throw InvalidArgumentException("RRArray error");
		memcpy(rrarray->void_ptr(),array_in,len*sizeof(int8_t));
				
	}
	
	static  void BytesToRRBaseArray( uint8_t* array_in, int32_t len,const boost::intrusive_ptr<RobotRaconteur::RRBaseArray>& rrarray)
	{
		if (rrarray->GetTypeID()!=DataTypes_uint8_t) throw InvalidArgumentException("RRArray error");
		if (rrarray->size()!=len) throw InvalidArgumentException("RRArray error");
		memcpy(rrarray->void_ptr(),array_in,len*sizeof(uint8_t));				
	}
	
	
	static  void ShortsToRRBaseArray( int16_t* array_in, int32_t len,const boost::intrusive_ptr<RobotRaconteur::RRBaseArray>& rrarray)
	{
		if (rrarray->GetTypeID()!=DataTypes_int16_t && rrarray->GetTypeID()!=DataTypes_uint16_t) throw InvalidArgumentException("RRArray error");
		if (rrarray->size()!=len) throw InvalidArgumentException("RRArray error");
		memcpy(rrarray->void_ptr(),array_in,len*sizeof(int16_t));
				
	}
	
	static  void ShortsToRRBaseArray( uint16_t* array_in, int32_t len,const boost::intrusive_ptr<RobotRaconteur::RRBaseArray>& rrarray)
	{
		if (rrarray->GetTypeID()!=DataTypes_uint16_t) throw InvalidArgumentException("RRArray error");
		if (rrarray->size()!=len) throw InvalidArgumentException("RRArray error");
		memcpy(rrarray->void_ptr(),array_in,len*sizeof(uint16_t));
				
	}
	
	static void  IntsToRRBaseArray( int32_t* array_in, int32_t len,const boost::intrusive_ptr<RobotRaconteur::RRBaseArray>& rrarray)
	{
		if (rrarray->GetTypeID()!=DataTypes_int32_t && rrarray->GetTypeID()!=DataTypes_uint32_t) throw InvalidArgumentException("RRArray error");
		if (rrarray->size()!=len) throw InvalidArgumentException("RRArray error");
		memcpy(rrarray->void_ptr(),array_in,len*sizeof(int32_t));
				
	}
	
	static void  IntsToRRBaseArray( uint32_t* array_in, int32_t len,const boost::intrusive_ptr<RobotRaconteur::RRBaseArray>& rrarray)
	{
		if (rrarray->GetTypeID()!=DataTypes_uint32_t) throw InvalidArgumentException("RRArray error");
		if (rrarray->size()!=len) throw InvalidArgumentException("RRArray error");
		memcpy(rrarray->void_ptr(),array_in,len*sizeof(uint32_t));
				
	}
	
	static void LongsToRRBaseArray( int64_t* array_in, int32_t len,const boost::intrusive_ptr<RobotRaconteur::RRBaseArray>& rrarray)
	{
		if (rrarray->GetTypeID()!=DataTypes_int64_t && rrarray->GetTypeID()!=DataTypes_uint64_t) throw InvalidArgumentException("RRArray error");
		if (rrarray->size()!=len) throw InvalidArgumentException("RRArray error");
		memcpy(rrarray->void_ptr(),array_in,len*sizeof(int64_t));
				
	}
#ifndef SWIGJAVA
	static void LongsToRRBaseArray( uint64_t* array_in, int32_t len,const boost::intrusive_ptr<RobotRaconteur::RRBaseArray>& rrarray)
	{
		if (rrarray->GetTypeID()!=DataTypes_uint64_t) throw InvalidArgumentException("RRArray error");
		if (rrarray->size()!=len) throw InvalidArgumentException("RRArray error");
		memcpy(rrarray->void_ptr(),array_in,len*sizeof(uint64_t));
				
	}
#endif
	static void DoublesToComplexRRBaseArray(double* array_in, int32_t len,const boost::intrusive_ptr<RobotRaconteur::RRBaseArray>& rrarray)
	{
		if (rrarray->GetTypeID()!=DataTypes_cdouble_t) throw InvalidArgumentException("RRArray error");
		if (rrarray->size()!=len/2) throw InvalidArgumentException("RRArray error");
		memcpy(rrarray->void_ptr(),array_in,len*sizeof(double));
	}
	
	static void FloatsToComplexRRBaseArray(float* array_in, int32_t len,const boost::intrusive_ptr<RobotRaconteur::RRBaseArray>& rrarray)
	{
		if (rrarray->GetTypeID()!=DataTypes_csingle_t) throw InvalidArgumentException("RRArray error");
		if (rrarray->size()!=len/2) throw InvalidArgumentException("RRArray error");
		memcpy(rrarray->void_ptr(),array_in,len*sizeof(float));
	}
	
	static  void BytesToBoolRRBaseArray( int8_t* array_in, int32_t len,const boost::intrusive_ptr<RobotRaconteur::RRBaseArray>& rrarray)
	{
		if (rrarray->GetTypeID()!=DataTypes_bool_t) throw InvalidArgumentException("RRArray error");
		if (rrarray->size()!=len) throw InvalidArgumentException("RRArray error");
		memcpy((rr_bool*)rrarray->void_ptr(),array_in,len*sizeof(int8_t));				
	}
	
	static  void BytesToBoolRRBaseArray( uint8_t* array_in, int32_t len,const boost::intrusive_ptr<RobotRaconteur::RRBaseArray>& rrarray)
	{
		if (rrarray->GetTypeID()!=DataTypes_bool_t) throw InvalidArgumentException("RRArray error");
		if (rrarray->size()!=len) throw InvalidArgumentException("RRArray error");
		memcpy((rr_bool*)rrarray->void_ptr(),array_in,len*sizeof(uint8_t));				
	}
	
#endif
};

}