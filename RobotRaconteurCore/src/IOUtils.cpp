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

#ifdef ROBOTRACONTEUR_CORE_USE_STDAFX
#include "stdafx.h"
#endif

#include "RobotRaconteur/IOUtils.h"
#include "RobotRaconteur/DataTypes.h"

namespace RobotRaconteur
{

ArrayBinaryReader::ArrayBinaryReader(uint8_t* buffer, size_t start_position, size_t length, bool nativeorder)
{
	this->buffer=buffer;
	this->position=start_position;
	this->length=length;

	this->limits.push_back(length);
	this->nativeorder=nativeorder;	
}

size_t ArrayBinaryReader::Length()
{
	return length;
}

size_t ArrayBinaryReader::Position()
{
	return position;
}

void ArrayBinaryReader::Seek(size_t position)
{
	if (!(position<=CurrentLimit()))
	{
		throw BufferLimitViolationException("Range outside of buffer");

	}

	this->position=position;
}

size_t ArrayBinaryReader::Read(void* buffer, size_t index, size_t length)
{
	if (!((position+length)<=CurrentLimit()))
	{
		throw BufferLimitViolationException("Range outside of buffer");

	}

	memcpy(((uint8_t*)buffer)+index,this->buffer+position,length);

	position+=length;

	return length;

}

void ArrayBinaryReader::ReadArray(RR_INTRUSIVE_PTR<RRBaseArray>& arr)
{
	DataTypes type=arr->GetTypeID();
	size_t len=arr->size();

	switch (type)
	{		
	case DataTypes_double_t:
		{
		RR_INTRUSIVE_PTR<RRArray<double> > o=rr_cast<RRArray<double> >(arr);
		Read((uint8_t*)o->data(),0,len*sizeof(double));
		break;	
		}
	case DataTypes_single_t:
		{
		RR_INTRUSIVE_PTR<RRArray<float> > o=rr_cast<RRArray<float> >(arr);
		Read((uint8_t*)o->data(),0,len*sizeof(float));
		break;
		}
	case DataTypes_int8_t:
		{
		RR_INTRUSIVE_PTR<RRArray<int8_t> > o=rr_cast<RRArray<int8_t> >(arr);
		Read((uint8_t*)o->data(),0,len*sizeof(int8_t));
		break;
		}
	case DataTypes_uint8_t:
		{
		RR_INTRUSIVE_PTR<RRArray<uint8_t> > o=rr_cast<RRArray<uint8_t> >(arr);
		Read((uint8_t*)o->data(),0,len*sizeof(uint8_t));
		break;
		}
	case DataTypes_int16_t:
		{
		RR_INTRUSIVE_PTR<RRArray<int16_t> > o=rr_cast<RRArray<int16_t> >(arr);
		Read((uint8_t*)o->data(),0,len*sizeof(int16_t));
		break;
		}
	case DataTypes_uint16_t:
		{
		RR_INTRUSIVE_PTR<RRArray<uint16_t> > o=rr_cast<RRArray<uint16_t> >(arr);
		Read((uint8_t*)o->data(),0,len*sizeof(uint16_t));
		break;
		}
	case DataTypes_int32_t:
		{
		RR_INTRUSIVE_PTR<RRArray<int32_t> > o=rr_cast<RRArray<int32_t> >(arr);
		Read((uint8_t*)o->data(),0,len*sizeof(int32_t));
		break;
		}
	case DataTypes_uint32_t:
		{
		RR_INTRUSIVE_PTR<RRArray<uint32_t> > o=rr_cast<RRArray<uint32_t> >(arr);
		Read((uint8_t*)o->data(),0,len*sizeof(uint32_t));
		break;
		}
	case DataTypes_int64_t:
		{
		RR_INTRUSIVE_PTR<RRArray<int64_t> > o=rr_cast<RRArray<int64_t> >(arr);
		Read((uint8_t*)o->data(),0,len*sizeof(int64_t));
		break;
		}
	case DataTypes_uint64_t:
		{
		RR_INTRUSIVE_PTR<RRArray<uint64_t> > o=rr_cast<RRArray<uint64_t> >(arr);
		Read((uint8_t*)o->data(),0,len*sizeof(uint64_t));
		break;
		}
	case DataTypes_string_t:
		{
		RR_INTRUSIVE_PTR<RRArray<char> > o=rr_cast<RRArray<char> >(arr);
		Read((uint8_t*)o->data(),0,len*sizeof(char));
		break;
		}
	case DataTypes_cdouble_t:
	{
		RR_INTRUSIVE_PTR<RRArray<cdouble> > o = rr_cast<RRArray<cdouble> >(arr);
		Read((uint8_t*)o->data(), 0, len * sizeof(cdouble));
		break;
	}
	case DataTypes_csingle_t:
	{
		RR_INTRUSIVE_PTR<RRArray<cfloat> > o = rr_cast<RRArray<cfloat> >(arr);
		Read((uint8_t*)o->data(), 0, len * sizeof(cfloat));
		break;
	}
	case DataTypes_bool_t:
	{
		RR_INTRUSIVE_PTR<RRArray<rr_bool> > o = rr_cast<RRArray<rr_bool> >(arr);
		Read((uint8_t*)o->data(), 0, len * sizeof(rr_bool));
		break;
	}	
	default:
		throw DataTypeException("Invalid data type");
	}

#ifdef BOOST_BIG_ENDIAN
#error Big endian not configured
	if (!nativeorder)
	{
		uint8_t* pos=(uint8_t*)arr->void_ptr();
		size_t elemsize=arr->ElementSize();
	
		for (size_t i=0; i<arr->Length(); i++)
		{		
			std::reverse(pos,pos+elemsize);
			pos+=elemsize;
		}
	}
#endif
}

std::string ArrayBinaryReader::ReadString8(size_t length)
{
	std::string strin;
	strin.resize(length);
	Read(&strin[0], 0, length);	
	return strin;
}

uint32_t ArrayBinaryReader::ReadUintX()
{
	uint8_t a = ReadNumber<uint8_t>();
	if (a <= 252)
	{
		return a;
	}

	if (a == 253)
	{
		return ReadNumber<uint16_t>();
	}

	if (a == 254)
	{
		return ReadNumber<uint32_t>();
	}

	throw InvalidArgumentException("Invalid number size");
}


uint64_t ArrayBinaryReader::ReadUintX2()
{
	uint8_t a = ReadNumber<uint8_t>();
	if (a <= 252)
	{
		return a;
	}

	if (a == 253)
	{
		return ReadNumber<uint16_t>();
	}

	if (a == 254)
	{
		return ReadNumber<uint32_t>();
	}

	return ReadNumber<uint64_t>();
}

int32_t ArrayBinaryReader::ReadIntX()
{
	int8_t a = ReadNumber<int8_t>();
	if (a <= 124)
	{
		return a;
	}

	if (a == 125)
	{
		return ReadNumber<int16_t>();
	}

	if (a == 126)
	{
		return ReadNumber<int32_t>();
	}

	throw InvalidArgumentException("Invalid number size");
}


int64_t ArrayBinaryReader::ReadIntX2()
{
	int8_t a = ReadNumber<int8_t>();
	if (a <= 124)
	{
		return a;
	}

	if (a == 125)
	{
		return ReadNumber<int16_t>();
	}

	if (a == 126)
	{
		return ReadNumber<int32_t>();
	}

	return ReadNumber<int64_t>();
}

size_t ArrayBinaryReader::CurrentLimit()
{
	return limits.back();
}

void ArrayBinaryReader::PushRelativeLimit(size_t limit)
{
	if ((Position()+limit) > CurrentLimit()) throw BufferLimitViolationException("Binary reader error");


	limits.push_back(Position() + limit);
}

void ArrayBinaryReader::PushAbsoluteLimit(size_t limit)
{
	if (limit > CurrentLimit()) throw BufferLimitViolationException("Binary reader error");

	limits.push_back(limit);
}

void ArrayBinaryReader::PopLimit()
{
	limits.pop_back();
}

int32_t ArrayBinaryReader::DistanceFromLimit()
{
	return ((int32_t)CurrentLimit()) - ((int32_t)Position());
}

ArrayBinaryWriter::ArrayBinaryWriter(uint8_t* buffer, size_t start_position, size_t length, bool nativeorder)
{
	this->buffer=buffer;
	this->position=start_position;
	this->length=length;
	this->limits.push_back(length);
	this->nativeorder=nativeorder;
}




size_t ArrayBinaryWriter::Length()
{
	return length;
}

size_t ArrayBinaryWriter::Position()
{
	return position;
}

void ArrayBinaryWriter::Seek(size_t position)
{
	if (!(position<=CurrentLimit()))
	{
		throw BufferLimitViolationException("Range outside of buffer");

	}

	this->position=position;
}

size_t ArrayBinaryWriter::Write(const void* buffer, size_t index, size_t length)
{
	if (!((position+length)<=CurrentLimit()))
	{
		throw BufferLimitViolationException("Range outside of buffer");

	}

	memcpy(this->buffer+position,((uint8_t*)buffer)+index,length);
	
	position+=length;

	return length;

}

void ArrayBinaryWriter::WriteArray(RR_INTRUSIVE_PTR<RRBaseArray>& arr)
{
	DataTypes type=arr->GetTypeID();
	size_t len=arr->size();

	switch (type)
	{		
	case DataTypes_double_t:
		{
		RR_INTRUSIVE_PTR<RRArray<double> > o=rr_cast<RRArray<double> >(arr);
#ifdef BOOST_BIG_ENDIAN
		if (!nativeorder) for (size_t i=0; i< o->Length(); i++) WriteNumber((*o)[i]);
#else
		Write((uint8_t*)o->data(),0,len*sizeof(double));
#endif
		break;
		}
	case DataTypes_single_t:
		{
		RR_INTRUSIVE_PTR<RRArray<float> > o=rr_cast<RRArray<float> >(arr);
#ifdef BOOST_BIG_ENDIAN
		if (!nativeorder) for (size_t i=0; i< o->Length(); i++) WriteNumber((*o)[i]);
#else
		Write((uint8_t*)o->data(),0,len*sizeof(float));
#endif
		break;
		}
	case DataTypes_int8_t:
		{
		RR_INTRUSIVE_PTR<RRArray<int8_t> > o=rr_cast<RRArray<int8_t> >(arr);

		Write((uint8_t*)o->data(),0,len*sizeof(int8_t));

		break;
		}
	case DataTypes_uint8_t:
		{
		RR_INTRUSIVE_PTR<RRArray<uint8_t> > o=rr_cast<RRArray<uint8_t> >(arr);

		Write((uint8_t*)o->data(),0,len*sizeof(uint8_t));

		break;
		}
	case DataTypes_int16_t:
		{
		RR_INTRUSIVE_PTR<RRArray<int16_t> > o=rr_cast<RRArray<int16_t> >(arr);
#ifdef BOOST_BIG_ENDIAN
		if (!nativeorder) for (size_t i=0; i< o->Length(); i++) WriteNumber((*o)[i]);
#else
		Write((uint8_t*)o->data(),0,len*sizeof(int16_t));
#endif
		break;
		}
	case DataTypes_uint16_t:
		{
		RR_INTRUSIVE_PTR<RRArray<uint16_t> > o=rr_cast<RRArray<uint16_t> >(arr);
#ifdef BOOST_BIG_ENDIAN
		if (!nativeorder) for (size_t i=0; i< o->Length(); i++) WriteNumber((*o)[i]);
#else
		Write((uint8_t*)o->data(),0,len*sizeof(uint16_t));
#endif
		break;
		}
	case DataTypes_int32_t:
		{
		RR_INTRUSIVE_PTR<RRArray<int32_t> > o=rr_cast<RRArray<int32_t> >(arr);
#ifdef BOOST_BIG_ENDIAN
		if (!nativeorder) for (size_t i=0; i< o->Length(); i++) WriteNumber((*o)[i]);
#else
		Write((uint8_t*)o->data(),0,len*sizeof(int32_t));
#endif
		break;
		}
	case DataTypes_uint32_t:
		{
		RR_INTRUSIVE_PTR<RRArray<uint32_t> > o=rr_cast<RRArray<uint32_t> >(arr);
#ifdef BOOST_BIG_ENDIAN
		if (!nativeorder) for (size_t i=0; i< o->Length(); i++) WriteNumber((*o)[i]);
#else
		Write((uint8_t*)o->data(),0,len*sizeof(uint32_t));
#endif
		break;
		}
	case DataTypes_int64_t:
		{
		RR_INTRUSIVE_PTR<RRArray<int64_t> > o=rr_cast<RRArray<int64_t> >(arr);
#ifdef BOOST_BIG_ENDIAN
		if (!nativeorder) for (size_t i=0; i< o->Length(); i++) WriteNumber((*o)[i]);
#else
		Write((uint8_t*)o->data(),0,len*sizeof(int64_t));
#endif
		break;
		}
	case DataTypes_uint64_t:
		{
		RR_INTRUSIVE_PTR<RRArray<uint64_t> > o=rr_cast<RRArray<uint64_t> >(arr);
#ifdef BOOST_BIG_ENDIAN
		if (!nativeorder) for (size_t i=0; i< o->Length(); i++) WriteNumber((*o)[i]);
#else
		Write((uint8_t*)o->data(),0,len*sizeof(uint64_t));
#endif
		break;
		}
	case DataTypes_string_t:
		{
		RR_INTRUSIVE_PTR<RRArray<char> > o=rr_cast<RRArray<char> >(arr);
		Write((uint8_t*)o->data(),0,len*sizeof(char));
		break;
		}
	case DataTypes_cdouble_t:
	{
		RR_INTRUSIVE_PTR<RRArray<cdouble> > o = rr_cast<RRArray<cdouble> >(arr);
		Write((uint8_t*)o->data(), 0, len * sizeof(cdouble));
		break;
	}
	case DataTypes_csingle_t:
	{
		RR_INTRUSIVE_PTR<RRArray<cfloat> > o = rr_cast<RRArray<cfloat> >(arr);
		Write((uint8_t*)o->data(), 0, len * sizeof(cfloat));
		break;
	}
	case DataTypes_bool_t:
	{
		RR_INTRUSIVE_PTR<RRArray<rr_bool> > o = rr_cast<RRArray<rr_bool> >(arr);
		Write((uint8_t*)o->data(), 0, len * sizeof(rr_bool));
		break;
	}	
	default:
		throw DataTypeException("Invalid data type");
	}
}

void ArrayBinaryWriter::WriteString8(const std::string& str)
{
	const char* datout=str.c_str();
	Write((const uint8_t*)datout,0,str.size());	
}

void ArrayBinaryWriter::WriteString8WithXLen(const std::string& str)
{
	size_t a = str.size();
	WriteUintX((uint32_t)a);
	const char* datout = str.c_str();
	Write((const uint8_t*)datout, 0, a);
}

void ArrayBinaryWriter::WriteUintX(uint32_t v)
{
	if (v <= 252)
	{
		WriteNumber((uint8_t)v);
		return;
	}
	if (v <= std::numeric_limits<uint16_t>::max())
	{
		WriteNumber((uint8_t)253);
		WriteNumber((uint16_t)v);
		return;
	}
	
	WriteNumber((uint8_t)254);
	WriteNumber(v);
}

void ArrayBinaryWriter::WriteUintX2(uint64_t v)
{
	if (v <= std::numeric_limits<uint32_t>::max())
	{
		WriteUintX((uint32_t)v);
	}
	else
	{
		WriteNumber((uint8_t)255);
		WriteNumber(v);
	}
}

void ArrayBinaryWriter::WriteIntX(int32_t v)
{
	if (v >= std::numeric_limits<int8_t>::min() && v <= 124)
	{
		WriteNumber((int8_t)v);
		return;
	}
	if (v >= std::numeric_limits<int16_t>::min() && v <= std::numeric_limits<int16_t>::max())
	{
		WriteNumber((int8_t)125);
		WriteNumber((int16_t)v);
		return;
	}

	WriteNumber((int8_t)126);
	WriteNumber(v);
}

void ArrayBinaryWriter::WriteIntX2(int64_t v)
{
	if (v >= std::numeric_limits<int32_t>::min() && v <= std::numeric_limits<int32_t>::max())
	{
		WriteIntX((int32_t)v);
	}
	else
	{
		WriteNumber((int8_t)127);
		WriteNumber(v);
	}
}

size_t ArrayBinaryWriter::GetStringByteCount8(const std::string& str)
{
	return str.size();
}

size_t ArrayBinaryWriter::GetStringByteCount8WithXLen(const std::string& str)
{
	size_t a = str.size();
	if (a >= std::numeric_limits<uint32_t>::max() - 5) throw InvalidArgumentException("String too large");
	a += GetUintXByteCount(a);
	return a;
}

size_t ArrayBinaryWriter::GetUintXByteCount(uint32_t v)
{
	if (v <= 252) return 1;
	if (v <= std::numeric_limits<uint16_t>::max()) return 3;
	return 5;	
}

size_t ArrayBinaryWriter::GetUintX2ByteCount(uint64_t v)
{
	if (v <= 252) return 1;
	if (v <= std::numeric_limits<uint16_t>::max()) return 3;
	if (v <= std::numeric_limits<uint32_t>::max()) return 5;
	return 9;
}

size_t ArrayBinaryWriter::GetIntXByteCount(int32_t v)
{
	if (v >= std::numeric_limits<int8_t>::min() && v <= 124) return 1;
	if (v >= std::numeric_limits<int16_t>::min() && v <= std::numeric_limits<int16_t>::max()) return 3;
	return 5;
}

size_t ArrayBinaryWriter::GetIntX2ByteCount(int64_t v)
{
	if (v >= std::numeric_limits<int8_t>::min() && v <= 124) return 1;
	if (v >= std::numeric_limits<int16_t>::min() && v <= std::numeric_limits<int16_t>::max()) return 3;
	if (v >= std::numeric_limits<int32_t>::min() && v <= std::numeric_limits<int32_t>::max()) return 5;
	return 9;
}

size_t ArrayBinaryWriter::GetSizePlusUintX(size_t s)
{
	if (s <= 251) return s + 1;
	if (s <= std::numeric_limits<uint16_t>::max() - 3) return s + 3;
	if (s > std::numeric_limits<uint32_t>::max() - 5) throw BufferLimitViolationException("Message too large");
	return s + 5;
}

size_t ArrayBinaryWriter::CurrentLimit()
{
	return limits.back();
}

void ArrayBinaryWriter::PushRelativeLimit(size_t limit)
{
	if ((Position()+limit) > CurrentLimit()) throw BufferLimitViolationException("Binary reader error");

	limits.push_back(Position() + limit);
}

void ArrayBinaryWriter::PushAbsoluteLimit(size_t limit)
{
	if ((limit) > CurrentLimit()) throw BufferLimitViolationException("Binary reader error");
	limits.push_back(limit);
}

void ArrayBinaryWriter::PopLimit()
{
	limits.pop_back();
}

int32_t ArrayBinaryWriter::DistanceFromLimit()
{
	return ((int32_t)CurrentLimit()) - ((int32_t)Position());
}


}