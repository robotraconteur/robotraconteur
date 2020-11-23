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

#ifdef ROBOTRACONTEUR_CORE_USE_STDAFX
#include "stdafx.h"
#endif

#include "RobotRaconteur/DataTypes.h"
#include <wchar.h>
#include <boost/algorithm/string.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "RobotRaconteur/RobotRaconteurNode.h"
#include <boost/range/algorithm.hpp>

namespace RobotRaconteur
{




ROBOTRACONTEUR_CORE_API RR_INTRUSIVE_PTR<RRArray<char> > stringToRRArray(boost::string_ref str)
{
	size_t s=str.size();
	RR_INTRUSIVE_PTR<RRArray<char> > ret=AllocateRRArray<char>(s);
	memcpy(ret->data(),str.data(),s);
	return ret;


}

ROBOTRACONTEUR_CORE_API std::string RRArrayToString(RR_INTRUSIVE_PTR<RRArray<char> > arr)
{
	if (!arr)
	{
		throw DataTypeException("Null pointer");
	}
	size_t s=arr->size();
	//char* str=new char[arr->Length()+1];

	return std::string(arr->data(),s);
}


RRObject::RRObject()
{
	
}

RRValue::RRValue()
{

}

std::string RRBaseArray::RRType()
{
	switch (GetTypeID())
	{	
	case DataTypes_double_t:
		return "RobotRaconteur.RRArray<double>";
	case DataTypes_single_t:
		return "RobotRaconteur.RRArray<single>";
	case DataTypes_int8_t:
		return "RobotRaconteur.RRArray<int8>";
	case DataTypes_uint8_t:
		return "RobotRaconteur.RRArray<uint8>";
	case DataTypes_int16_t:
		return "RobotRaconteur.RRArray<int16>";
	case DataTypes_uint16_t:
		return "RobotRaconteur.RRArray<uint16>";
	case DataTypes_int32_t:
		return "RobotRaconteur.RRArray<int32>";
	case DataTypes_uint32_t:
		return "RobotRaconteur.RRArray<uint32>";
	case DataTypes_int64_t:
		return "RobotRaconteur.RRArray<int64>";
	case DataTypes_uint64_t:
		return "RobotRaconteur.RRArray<uint64>";
	case DataTypes_string_t:
		return "RobotRaconteur.RRArray<char>";
	case DataTypes_cdouble_t:
		return "RobotRaconteur.RRArray<cdouble>";
	case DataTypes_csingle_t:
		return "RobotRaconteur.RRArray<csingle>";
	case DataTypes_bool_t:
		return "RobotRaconteur.RRArray<bool>";
	default:
		throw DataTypeException("Invalid data type");
	}

	throw DataTypeException("Invalid data type");

}

ROBOTRACONTEUR_CORE_API std::string GetRRDataTypeString(DataTypes type)
{
	switch (type)
	{
	case DataTypes_void_t:
		return "void";
	case DataTypes_double_t:
		return "double";
	case DataTypes_single_t:
		return "single";
	case DataTypes_int8_t:
		return "int8";
	case DataTypes_uint8_t:
		return "uint8";
	case DataTypes_int16_t:
		return "int16";
	case DataTypes_uint16_t:
		return "uint16";
	case DataTypes_int32_t:
		return "int32";
	case DataTypes_uint32_t:
		return "uint32";
	case DataTypes_int64_t:
		return "int64";
	case DataTypes_uint64_t:
		return "uint64";
	case DataTypes_string_t:
		return "string";
	case DataTypes_cdouble_t:
		return "cdouble";
	case DataTypes_csingle_t:
		return "csingle";
	case DataTypes_bool_t:
		return "bool";
	default:
		throw DataTypeException("Invalid data type");
	}

	throw DataTypeException("Invalid data type");



}

ROBOTRACONTEUR_CORE_API bool IsTypeRRArray(DataTypes type)
{
	switch (type)
	{
	case DataTypes_double_t:
	case DataTypes_single_t:
	case DataTypes_int8_t:
	case DataTypes_uint8_t:
	case DataTypes_int16_t:
	case DataTypes_uint16_t:
	case DataTypes_int32_t:
	case DataTypes_uint32_t:
	case DataTypes_int64_t:
	case DataTypes_uint64_t:
	case DataTypes_string_t:
	case DataTypes_cdouble_t:		
	case DataTypes_csingle_t:		
	case DataTypes_bool_t:		
		return true;
	default:
		return false;
	}

}

ROBOTRACONTEUR_CORE_API bool IsTypeNumeric(DataTypes type)
{
	switch (type)
	{
	case DataTypes_double_t:
	case DataTypes_single_t:
	case DataTypes_int8_t:
	case DataTypes_uint8_t:
	case DataTypes_int16_t:
	case DataTypes_uint16_t:
	case DataTypes_int32_t:
	case DataTypes_uint32_t:
	case DataTypes_int64_t:
	case DataTypes_uint64_t:
	case DataTypes_cdouble_t:
	case DataTypes_csingle_t:
	case DataTypes_bool_t:
		return true;
	default:
		return false;
	}

}

ROBOTRACONTEUR_CORE_API RR_INTRUSIVE_PTR<RRBaseArray> AllocateRRArrayByType(DataTypes type, size_t length)
{
	switch (type)
	{
	
		
	case DataTypes_double_t:
		return AllocateRRArray<double>(length);
	case DataTypes_single_t:
		return AllocateRRArray<float>(length);
	case DataTypes_int8_t:
		return AllocateRRArray<int8_t>(length);
	case DataTypes_uint8_t:
		return AllocateRRArray<uint8_t>(length);
	case DataTypes_int16_t:
		return AllocateRRArray<int16_t>(length);
	case DataTypes_uint16_t:
		return AllocateRRArray<uint16_t>(length);
	case DataTypes_int32_t:
		return AllocateRRArray<int32_t>(length);
	case DataTypes_uint32_t:
		return AllocateRRArray<uint32_t>(length);
	case DataTypes_int64_t:
		return AllocateRRArray<int64_t>(length);
	case DataTypes_uint64_t:
		return AllocateRRArray<uint64_t>(length);
	case DataTypes_string_t:
		return AllocateRRArray<char>(length);
	case DataTypes_cdouble_t:
		return AllocateRRArray<cdouble>(length);
	case DataTypes_csingle_t:
		return AllocateRRArray<cfloat>(length);
	case DataTypes_bool_t:
		return AllocateRRArray<rr_bool>(length);	
	default:
		throw DataTypeException("Invalid data type");
		
	}

	throw DataTypeException("Invalid data type");

}

ROBOTRACONTEUR_CORE_API size_t RRArrayElementSize(DataTypes type)
{
	switch (type)
	{
		case DataTypes_double_t:
			return 8;

		case DataTypes_single_t:
			return 4;
		case DataTypes_int8_t:
		case DataTypes_uint8_t:
			return 1;
		case DataTypes_int16_t:
		case DataTypes_uint16_t:
			return 2;
		case DataTypes_int32_t:
		case DataTypes_uint32_t:
			return 4;
		case DataTypes_int64_t:
		case DataTypes_uint64_t:
			return 8;
		case DataTypes_string_t:
			return 1;
		case DataTypes_cdouble_t:
			return 16;
		case DataTypes_csingle_t:
			return 8;
		case DataTypes_bool_t:
			return 1;
		default:
			throw DataTypeException("Invalid data type");
	}
	throw DataTypeException("Invalid data type");
	//return 0;
}

TimeSpec::TimeSpec()
{
	seconds=0;
	nanoseconds=0;
}

TimeSpec::TimeSpec(int64_t seconds, int32_t nanoseconds)
{
	this->seconds=seconds;
	this->nanoseconds=nanoseconds;
	cleanup_nanosecs();
}

boost::posix_time::ptime TimeSpecToPTime(const TimeSpec& ts)
{
	boost::posix_time::ptime epoch(boost::gregorian::date(1970,boost::gregorian::Jan,1),boost::posix_time::time_duration(0,0,0));
	// TODO: increase precision
	return epoch + boost::posix_time::seconds(ts.seconds) + boost::posix_time::microseconds(ts.nanoseconds/1000);
}
TimeSpec ptimeToTimeSpec(const boost::posix_time::ptime& t)
{
	boost::posix_time::ptime epoch(boost::gregorian::date(1970,boost::gregorian::Jan,1),boost::posix_time::time_duration(0,0,0));
	boost::posix_time::time_duration diff=t-epoch;	

	int64_t sec=diff.total_seconds();
	int32_t nanosec=boost::numeric_cast<int32_t>((diff.fractional_seconds() * boost::numeric_cast<int32_t>(pow(10.0,(9-diff.num_fractional_digits())))));

	return TimeSpec(sec,nanosec);
}

void TimeSpec::cleanup_nanosecs()
{
	int32_t nano_div = nanoseconds / static_cast<int32_t>(1e9);
	nanoseconds = nanoseconds % static_cast<int32_t>(1e9);
	seconds += nano_div;



	if (seconds > 0 && nanoseconds < 0)
	{
		seconds = seconds - 1;
		nanoseconds = static_cast<int32_t>(1e9) + nanoseconds;
	}

	if (seconds < 0 && nanoseconds > 0)
	{
		seconds = seconds + 1;
		nanoseconds = nanoseconds - static_cast<int32_t>(1e9);
	}

}

bool TimeSpec::operator == (const TimeSpec &t2)
{
	
	return (this->seconds == t2.seconds && this->nanoseconds == t2.nanoseconds);
}

bool TimeSpec::operator != (const TimeSpec &t2)
{
	return !(*this == t2);
}

TimeSpec TimeSpec::operator - (const TimeSpec &t2)
{
	return TimeSpec(this->seconds - t2.seconds, this->nanoseconds - t2.nanoseconds);
}

TimeSpec TimeSpec::operator + (const TimeSpec &t2)
{
	return TimeSpec(this->seconds + t2.seconds, this->nanoseconds + t2.nanoseconds);
}

bool TimeSpec::operator > (const TimeSpec &t2)
{
	TimeSpec diff = *this - t2;
	if (diff.seconds == 0)
		return diff.nanoseconds > 0;
	return diff.seconds > 0;
}

bool TimeSpec::operator >= (const TimeSpec &t2)
{
	if (*this == t2)
		return true;
	return *this > t2;
}

bool TimeSpec::operator < (const TimeSpec &t2)
{
	TimeSpec diff = *this - t2;
	if (diff.seconds == 0)
		return diff.nanoseconds < 0;
	return diff.seconds < 0;
}

bool TimeSpec::operator <= (const TimeSpec &t2)
{
	if (*this == t2)
		return true;
	return *this < t2;
}


namespace detail
{
	MultiDimArray_CalculateCopyIndicesIter::~MultiDimArray_CalculateCopyIndicesIter()
	{

	}

	class MultiDimArray_CalculateCopyIndicesIterImpl : public MultiDimArray_CalculateCopyIndicesIter
	{
	protected:

		std::vector<uint32_t> mema_dims;
		std::vector<uint32_t> memb_dims;
		std::vector<uint32_t> mema_pos;
		std::vector<uint32_t> memb_pos;
		std::vector<uint32_t> count;

		std::vector<uint32_t> stridea;
		std::vector<uint32_t> strideb;

		std::vector<uint32_t> current_count;

		bool done;

	public:

		MultiDimArray_CalculateCopyIndicesIterImpl(const std::vector<uint32_t>& mema_dims, const std::vector<uint32_t>& mema_pos, const std::vector<uint32_t>& memb_dims, const std::vector<uint32_t>& memb_pos, const std::vector<uint32_t>& count)
		{
			//Error checking

			if (count.size() == 0) throw InvalidArgumentException("MultiDimArray count invalid");

			if (count.size() > mema_dims.size() || count.size() > memb_dims.size()) throw InvalidArgumentException("MultiDimArray copy count invalid");
			if (count.size() > memb_dims.size() || count.size() > memb_dims.size()) throw InvalidArgumentException("MultiDimArray copy count invalid");

			for (size_t i = 0; i < mema_dims.size(); i++)
			{
				if (mema_dims[i] < 0) throw InvalidArgumentException("MultiDimArray mema_dims invalid");
			}

			for (size_t i = 0; i < memb_dims.size(); i++)
			{
				if (memb_dims[i] < 0) throw InvalidArgumentException("MultiDimArray memb_dims invalid");
			}

			for (size_t i = 0; i < count.size(); i++)
			{
				if (count[i] < 0) throw InvalidArgumentException("MultiDimArray count invalid");
			}

			for (size_t i = 0; i < mema_dims.size() && i < count.size(); i++)
			{
				if (mema_pos[i] > mema_dims[i] || (mema_pos[i] + count[i]) > mema_dims[i])
					throw InvalidArgumentException("MultiDimArray A count invalid");
			}

			for (size_t i = 0; i < memb_dims.size() && i < count.size(); i++)
			{
				if (memb_pos[i] > memb_dims[i] || (memb_pos[i] + count[i]) > memb_dims[i])
					throw InvalidArgumentException("MultiDimArray B count invalid");
			}

			stridea.resize(count.size());
			stridea[0] = 1;
			for (uint32_t i = 1; i < boost::numeric_cast<uint32_t>(count.size()); i++)
			{
				stridea[i] = stridea[i - 1] * mema_dims[i - 1];				
			}

			strideb.resize(count.size());
			strideb[0] = 1;
			for (uint32_t i = 1; i < boost::numeric_cast<uint32_t>(count.size()); i++)
			{
				strideb[i] = strideb[i - 1] * memb_dims[i - 1];
			}

			this->mema_dims = mema_dims;
			this->memb_dims = memb_dims;
			this->mema_pos = mema_pos;
			this->memb_pos = memb_pos;
			this->count = count;
			this->current_count.resize(count.size());
			boost::range::fill(this->current_count, 0);
			this->done = false;
		}

		virtual bool Next(uint32_t& indexa, uint32_t& indexb, uint32_t& len)
		{

			if (done)
			{
				return false;
			}

			int a = 0;
			int b = 0;

			indexa = 0;
			for (uint32_t j = 0; j < boost::numeric_cast<uint32_t>(count.size()); j++)
				indexa += (current_count[j] + mema_pos[j]) * stridea[j];
			indexb = 0;
			for (uint32_t j = 0; j < boost::numeric_cast<uint32_t>(count.size()); j++)
				indexb += (current_count[j] + memb_pos[j]) * strideb[j];
						
			len = count[0];

			if (count.size() <= 1)
			{
				done = true;
				return true;
			}

			current_count[1]++;
			for (uint32_t j = 1; j < boost::numeric_cast<uint32_t>(count.size()); j++)
			{
				if (current_count[j] >(count[j] - 1))
				{
					current_count[j] = current_count[j] - count[j];
					if (j < boost::numeric_cast<uint32_t>(count.size() - 1))
					{
						current_count[j + 1]++;
					}
					else
					{
						done = true;
						return true;
					}
				}
			}

			return true;
		}

		virtual ~MultiDimArray_CalculateCopyIndicesIterImpl()
		{

		}
	};

	RR_SHARED_PTR<MultiDimArray_CalculateCopyIndicesIter> MultiDimArray_CalculateCopyIndicesBeginIter(const std::vector<uint32_t>& mema_dims, const std::vector<uint32_t>& mema_pos, const std::vector<uint32_t>& memb_dims, const std::vector<uint32_t>& memb_pos, const std::vector<uint32_t>& count)
	{
		RR_SHARED_PTR<MultiDimArray_CalculateCopyIndicesIterImpl> o = RR_MAKE_SHARED<MultiDimArray_CalculateCopyIndicesIterImpl>(mema_dims, mema_pos, memb_dims, memb_pos, count);
		return o;
	}
}

namespace detail
{
	ROBOTRACONTEUR_CORE_API std::string encode_index(boost::string_ref index)
	{
		std::stringstream out;

		for (size_t i = 0; i<index.length(); i++)
		{
			char c = index[i];
			if (std::isalnum(c))
			{
				out << std::dec << c;
			}
			else
			{
				out << std::hex << '%' << std::setw(2) << int(c) << std::setw(0);
			}
		}

		return out.str();
	}

	ROBOTRACONTEUR_CORE_API std::string decode_index(boost::string_ref index)
	{
		std::stringstream in(index.to_string());
		std::stringstream out;

		while (in.tellg()<boost::numeric_cast<int>(index.length()) && (static_cast<int>(in.tellg())) != -1)
		{
			int c = in.get();

			if (static_cast<char>(c) != '%')
			{
				out.put(c);
			}
			else if (static_cast<char>(c) == '%')
			{
				char in2_c[3];
				in.read(in2_c, 2);
				if (in.fail())
				{
					throw InvalidArgumentException("Invalid encoded index");
				}
				in2_c[2] = 0;
				std::stringstream in2(in2_c);
				
				int v;
				in2 >> std::hex >> v;
				if (in2.fail() || !in2.eof())
				{
					throw InvalidArgumentException("Invalid encoded index");
				}
				out.put(v);
			}

		}
		//std::cout << "decode: " << out.str() << out.str().length() << std::endl;

		return out.str();


	}

}

bool operator== (const cdouble &c1, const cdouble &c2)
{
	return (c1.real == c2.real) && (c1.imag == c2.imag);
}
bool operator!= (const cdouble &c1, const cdouble &c2) { return !(c1 == c2); }
bool operator== (const cfloat &c1, const cfloat &c2)
{
	return (c1.real == c2.real) && (c1.imag == c2.imag);
}
bool operator!= (const cfloat &c1, const cfloat &c2) { return !(c1 == c2); }
bool operator== (const rr_bool &c1, const rr_bool &c2)
{
	return (c1.value == c2.value);
}
bool operator!= (const rr_bool &c1, const rr_bool &c2) { return !(c1 == c2); }


RR_INTRUSIVE_PTR<RRList<RRArray<char> > > stringVectorToRRList(const std::vector<std::string>& string_vector)
{
	RR_INTRUSIVE_PTR<RRList<RRArray<char> > > o = AllocateEmptyRRList<RRArray<char> >();
	BOOST_FOREACH(const std::string& s, string_vector)
	{
		o->push_back(stringToRRArray(s));
	}
	return o;
}

std::vector<std::string> RRListToStringVector(RR_INTRUSIVE_PTR<RRList<RRArray<char> > > list)
{
	rr_null_check(list, "Unexected null string list");
	std::vector<std::string> o;
	BOOST_FOREACH(RR_INTRUSIVE_PTR<RRArray<char> > e, *list)
	{
		o.push_back(RRArrayToString(e));
	}
	return o;
}

namespace detail
{

class MessageStringRef_ptr_to_str : public boost::static_visitor<boost::string_ref>
{
public:
	boost::string_ref operator()(const detail::MessageStringData_string_ref& str) const {return str.ref;}
	boost::string_ref operator()(const detail::MessageStringData_static_string& str) const {return str.ref;}
	boost::string_ref operator()(const detail::MessageStringData* str_ptr) const
	{
		if (!str_ptr)
		{
			return "";
		}
		return str_ptr->str;
	}
	boost::string_ref operator()(const detail::MessageStringData& str_ptr) const
	{
		return str_ptr.str;
	}
};

class MessageStringRef_from_ptr : public boost::static_visitor<boost::variant<const detail::MessageStringData*,
		    detail::MessageStringData_static_string,
			detail::MessageStringData_string_ref> >
{
public:

	boost::variant<const detail::MessageStringData*,
		    detail::MessageStringData_static_string,
			detail::MessageStringData_string_ref> operator()(const detail::MessageStringData_static_string& str) const
	{
		return str;
	}
	boost::variant<const detail::MessageStringData*,
		    detail::MessageStringData_static_string,
			detail::MessageStringData_string_ref> operator()(const detail::MessageStringData& str_ptr) const {return &str_ptr;}
};

class MessageStringPtr_from_ref : public boost::static_visitor<boost::variant<detail::MessageStringData, 
		    detail::MessageStringData_static_string> >
{
public:

	boost::variant<detail::MessageStringData, 
		    detail::MessageStringData_static_string> operator()(const detail::MessageStringData_string_ref& str) const
	{
		detail::MessageStringData dat;
		dat.str = str.ref.to_string();
		return dat;
	}
	boost::variant<detail::MessageStringData, 
		    detail::MessageStringData_static_string> operator()(const detail::MessageStringData_static_string& str) const
	{
		return str;
	}
	boost::variant<detail::MessageStringData, 
		    detail::MessageStringData_static_string> operator()(const detail::MessageStringData* str_ptr) const {return *str_ptr;}
};

}

MessageStringPtr::MessageStringPtr() 
{
	_str_ptr = detail::MessageStringData_static_string("");	
}
MessageStringPtr::MessageStringPtr(const std::string& str)
{
	detail::MessageStringData dat;
	dat.str = str;
	this->_str_ptr = RR_MOVE(dat);
}
MessageStringPtr::MessageStringPtr(boost::string_ref str, bool is_static)
{
	if (is_static)
	{
		_str_ptr = detail::MessageStringData_static_string(str);
	}
	else
	{
		detail::MessageStringData dat;
		dat.str = str.to_string();
		this->_str_ptr = RR_MOVE(dat);
	}
}

MessageStringPtr::MessageStringPtr(const MessageStringPtr& str_ptr)
{
	this->_str_ptr = str_ptr._str_ptr;
}
MessageStringPtr::MessageStringPtr(const MessageStringRef& str_ref)
{
	this->_str_ptr = boost::apply_visitor(detail::MessageStringPtr_from_ref(), str_ref._str);
}
#if !defined(BOOST_NO_CXX11_RVALUE_REFERENCES)
MessageStringPtr::MessageStringPtr(std::string&& str)
{
	detail::MessageStringData dat;
	dat.str = RR_MOVE(str);
	this->_str_ptr = RR_MOVE(dat);
}
#endif

void MessageStringPtr::init_literal(const char* str, size_t len)
{
	_str_ptr = detail::MessageStringData_static_string(boost::string_ref(str,len));
}

boost::string_ref MessageStringPtr::str() const
{	
	return boost::apply_visitor(detail::MessageStringRef_ptr_to_str(), _str_ptr);
}

void MessageStringPtr::reset()
{	
	_str_ptr = detail::MessageStringData_static_string("");	
}

bool MessageStringPtr::operator ==(MessageStringRef b) const
{
	return this->str() == b.str();
}
bool MessageStringPtr::operator !=(MessageStringRef b) const
{
	return this->str() != b.str();
}
bool MessageStringPtr::operator <(MessageStringRef b) const
{
	return this->str() <= b.str();
}

MessageStringRef::MessageStringRef(const std::string& str)
{
	boost::string_ref str1(str);
	_str = detail::MessageStringData_string_ref(str1);
}

MessageStringRef::MessageStringRef(boost::string_ref str, bool is_static)
{
	if (is_static)
	{
		_str = detail::MessageStringData_static_string(str);
	}
	else
	{
		_str = detail::MessageStringData_string_ref(str);
	}
}
MessageStringRef::MessageStringRef(const MessageStringPtr& str_ptr)
{
	_str = boost::apply_visitor(detail::MessageStringRef_from_ptr(),str_ptr._str_ptr);
}
MessageStringRef::MessageStringRef(const MessageStringRef& str_ref)
{
	_str = str_ref._str;
}

void MessageStringRef::init_literal(const char* str, size_t len)
{
	_str = detail::MessageStringData_static_string(boost::string_ref(str,len));
}

bool MessageStringRef::operator ==(MessageStringRef b) const
{
	return this->str() == b.str();
}
bool MessageStringRef::operator !=(MessageStringRef b) const
{
	return this->str() != b.str();
}

boost::string_ref MessageStringRef::str() const
{
	return boost::apply_visitor(detail::MessageStringRef_ptr_to_str(), _str);
}

std::size_t hash_value(const RobotRaconteur::MessageStringPtr& k)
{
	boost::string_ref k1 = k.str();
	return boost::hash_range(k1.begin(), k1.end());
}

std::ostream & operator << (std::ostream &out, const MessageStringPtr &str)
{
	out << str.str();
	return out;
}
std::ostream & operator << (std::ostream &out, const MessageStringRef &str)
{
	out << str.str();
	return out;
}

}
