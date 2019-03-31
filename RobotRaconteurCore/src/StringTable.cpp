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

#include "RobotRaconteur/StringTable.h"
#include "RobotRaconteur/Message.h"
#include "RobotRaconteur/IOUtils.h"

#include <boost/range/adaptors.hpp>
#include <boost/range/algorithm.hpp>
#include <boost/foreach.hpp>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <fstream>

namespace RobotRaconteur
{
	namespace detail
	{

		StringTableEntry::StringTableEntry()
		{
			code = 0;
			confirmed = false;
		}

		StringTable::StringTable(bool server)
		{
			this->server = server;
			max_entry_count = 512;
			max_str_len = 128;
			next_code = 2;
			flags = 0;
			load_defaults();
		}

		uint32_t StringTable::GetCodeForString(const std::string& str)
		{
			RR_SHARED_PTR<const StringTableEntry> e = GetEntryForString(str);
			if (!e) return 0;
			return e->code;
		}

		bool StringTable::GetStringForCode(uint32_t code, std::string& str)
		{
			RR_SHARED_PTR<const StringTableEntry> e = GetEntryForCode(code);
			if (!e) return false;
			str= e->value;
			return true;
		}

		RR_SHARED_PTR<const StringTableEntry> StringTable::GetEntryForString(const std::string& str)
		{
			boost::mutex::scoped_lock lock(this_lock);
			RR_UNORDERED_MAP<std::string, RR_SHARED_PTR<StringTableEntry> >::iterator e = string_table.find(str);
			if (e == string_table.end())
			{
				if (!(this->flags & TransportCapabilityCode_MESSAGE3_STRINGTABLE_DYNAMIC_TABLE))
				{
					return RR_SHARED_PTR<const StringTableEntry>();
				}

				if (!server)
				{
					if (unconfirmed_string_table.find(str) == unconfirmed_string_table.end())
					{
						if (str.size() <= max_str_len)
						{
							if (unconfirmed_code_table.size() + code_table.size() < max_entry_count)
							{
								RR_SHARED_PTR<StringTableEntry> entry = RR_MAKE_SHARED<StringTableEntry>();
								entry->code = next_code;
								next_code += 4;
								entry->confirmed = false;
								entry->value = str;
								unconfirmed_code_table.insert(std::make_pair(entry->code, entry));
								unconfirmed_string_table.insert(std::make_pair(entry->value, entry));
							}
						}
					}
				}

				return RR_SHARED_PTR < const StringTableEntry >();
			}

			if (!(this->flags & TransportCapabilityCode_MESSAGE3_STRINGTABLE_STANDARD_TABLE))
			{
				if ((e->second->code & (0x2)) == 0)
				{
					return RR_SHARED_PTR < const StringTableEntry >();
				}
			}

			return e->second;
		}
		RR_SHARED_PTR<const StringTableEntry> StringTable::GetEntryForCode(uint32_t code)
		{
			boost::mutex::scoped_lock lock(this_lock);
			RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<StringTableEntry> >::iterator e = code_table.find(code);
			if (e != code_table.end())
			{
				return e->second;
			}
			e = unconfirmed_code_table.find(code);
			if (e == code_table.end()) return RR_SHARED_PTR < const StringTableEntry >();
			return e->second;
		}

		size_t StringTable::GetUnconfirmedCodeCount()
		{
			boost::mutex::scoped_lock lock(this_lock);
			return unconfirmed_code_table.size();
		}

		std::vector<RR_SHARED_PTR<const StringTableEntry> > StringTable::GetUnconfirmedCodes(uint32_t max_count)
		{
			boost::mutex::scoped_lock lock(this_lock);
			std::vector<RR_SHARED_PTR<const StringTableEntry> > o;
			o.reserve(unconfirmed_code_table.size());
			
			boost::copy(unconfirmed_code_table | boost::adaptors::map_values, std::back_inserter(o));
			
			return o;
		}
		void StringTable::ConfirmCodes(const std::vector<uint32_t> codes)
		{
			boost::mutex::scoped_lock lock(this_lock);

			BOOST_FOREACH (uint32_t e, codes)
			{
				RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<StringTableEntry> >::iterator e2 = unconfirmed_code_table.find(e);
				if (e2 != code_table.end())
				{
					RR_SHARED_PTR<StringTableEntry> entry = e2->second;
					unconfirmed_code_table.erase(e2);
					RR_UNORDERED_MAP<std::string, RR_SHARED_PTR<StringTableEntry> >::iterator e3 = unconfirmed_string_table.find(entry->value);
					if (e3->second == entry)
					{
						unconfirmed_string_table.erase(e3);
					}

					RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<StringTableEntry> >::iterator e4 = code_table.find(e);
					if (e4 == code_table.end())
					{
						entry->confirmed = true;
						code_table.insert(std::make_pair(entry->code, entry));
						string_table.insert(std::make_pair(entry->value, entry));
					}						
				}
			}
		}

		void StringTable::DropUnconfirmedCodes(const std::vector<uint32_t>& code)
		{
			boost::mutex::scoped_lock lock(this_lock);
			BOOST_FOREACH (uint32_t e, code)
			{
				RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<StringTableEntry> >::iterator e2=unconfirmed_code_table.find(e);
				if (e2 != unconfirmed_code_table.end())
				{
					unconfirmed_string_table.erase(e2->second->value);
					unconfirmed_code_table.erase(e2);
				}

			}

		}

		bool StringTable::IsTableFull()
		{
			boost::mutex::scoped_lock lock(this_lock);

			return !(unconfirmed_code_table.size() + code_table.size() < max_entry_count);
		}

		void StringTable::MessageReplaceStringsWithCodes(RR_INTRUSIVE_PTR<Message> m)
		{
			if (m->header->MessageFlags & MessageFlags_STRING_TABLE || m->header->StringTable.size() > 0)
			{
				return;
			}

			boost::unordered_map<std::string, uint32_t> local_table;
			uint32_t next_local_code = 1;
			uint32_t table_size = 0;

			BOOST_FOREACH (RR_INTRUSIVE_PTR<MessageEntry>& e, m->entries)
			{
				MessageEntryReplaceStringsWithCodes(e, local_table, next_local_code, table_size);
			}

			if (local_table.size() > 0)
			{
				m->header->MessageFlags |= MessageFlags_STRING_TABLE;
				typedef boost::unordered_map<std::string, uint32_t>::value_type e_type;
				BOOST_FOREACH (e_type& e, local_table)
				{
					m->header->StringTable.push_back(boost::make_tuple(e.second, e.first));
				}
			}
		}

		void StringTable::MessageReplaceCodesWithStrings(RR_INTRUSIVE_PTR<Message> m)
		{
			boost::unordered_map<uint32_t,std::string> local_table;
			
			if (m->header->MessageFlags & MessageFlags_STRING_TABLE)
			{
				typedef boost::tuple<uint32_t, std::string> e_type;
				BOOST_FOREACH (e_type& e, m->header->StringTable)
				{
					local_table.insert(std::make_pair(e.get<0>(), e.get<1>()));
				}
			}

			BOOST_FOREACH (RR_INTRUSIVE_PTR<MessageEntry>& e, m->entries)
			{
				MessageEntryReplaceCodesWithStrings(e, local_table);
			}
		}
		
		void StringTable::MessageEntryReplaceStringsWithCodes(RR_INTRUSIVE_PTR<MessageEntry> e, boost::unordered_map<std::string, uint32_t>& local_table, uint32_t& next_local_code, uint32_t& table_size)
		{
			DoReplaceString(e->MemberName, e->MemberNameCode, e->EntryFlags,
				MessageEntryFlags_MEMBER_NAME_STR, MessageEntryFlags_MEMBER_NAME_CODE,
				local_table, next_local_code, table_size);

			DoReplaceString(e->ServicePath, e->ServicePathCode, e->EntryFlags,
				MessageEntryFlags_SERVICE_PATH_STR, MessageEntryFlags_SERVICE_PATH_CODE,
				local_table, next_local_code, table_size);

			BOOST_FOREACH (RR_INTRUSIVE_PTR<MessageElement>& ee, e->elements)
			{
				MessageElementReplaceStringsWithCodes(ee, local_table, next_local_code, table_size);
			}
			
		}

		void StringTable::MessageElementReplaceStringsWithCodes(RR_INTRUSIVE_PTR<MessageElement> e, boost::unordered_map<std::string, uint32_t>& local_table, uint32_t& next_local_code, uint32_t& table_size)
		{
			DoReplaceString(e->ElementName, e->ElementNameCode, e->ElementFlags,
				MessageElementFlags_ELEMENT_NAME_STR, MessageElementFlags_ELEMENT_NAME_CODE,
				local_table, next_local_code, table_size);

			DoReplaceString(e->ElementTypeName, e->ElementTypeNameCode, e->ElementFlags,
				MessageElementFlags_ELEMENT_TYPE_NAME_STR, MessageElementFlags_ELEMENT_TYPE_NAME_CODE,
				local_table, next_local_code, table_size);

			switch (e->ElementType)
			{
			
			case DataTypes_structure_t:
			{
				RR_INTRUSIVE_PTR<MessageElementStructure> sdat = e->CastData<MessageElementStructure>();
				if (sdat)
				{
					BOOST_FOREACH (RR_INTRUSIVE_PTR<MessageElement>& ee, sdat->Elements)
						MessageElementReplaceStringsWithCodes(ee, local_table, next_local_code, table_size);
				}
				break;
			}
			case DataTypes_vector_t:
			{
				RR_INTRUSIVE_PTR<MessageElementMap<int32_t> > vdat = e->CastData<MessageElementMap<int32_t> >();
				if (vdat)
				{
					BOOST_FOREACH (RR_INTRUSIVE_PTR<MessageElement>& ee, vdat->Elements)
						MessageElementReplaceStringsWithCodes(ee, local_table, next_local_code, table_size);
				}
				break;
			}
			case DataTypes_dictionary_t:
			{
				RR_INTRUSIVE_PTR<MessageElementMap<std::string> > ddat = e->CastData<MessageElementMap<std::string> >();
				if (ddat)
				{
					BOOST_FOREACH (RR_INTRUSIVE_PTR<MessageElement>& ee, ddat->Elements)
						MessageElementReplaceStringsWithCodes(ee, local_table, next_local_code, table_size);
				}
				break;
			}
			case DataTypes_multidimarray_t:
			{
				RR_INTRUSIVE_PTR<MessageElementMultiDimArray> mdat = e->CastData<MessageElementMultiDimArray>();
				if (mdat)
				{
					BOOST_FOREACH (RR_INTRUSIVE_PTR<MessageElement>& ee, mdat->Elements)
						MessageElementReplaceStringsWithCodes(ee, local_table, next_local_code, table_size);
				}
				break;
			}
			case DataTypes_list_t:
			{
				RR_INTRUSIVE_PTR<MessageElementList> ddat = e->CastData<MessageElementList>();
				if (ddat)
				{
					BOOST_FOREACH (RR_INTRUSIVE_PTR<MessageElement>& ee, ddat->Elements)
						MessageElementReplaceStringsWithCodes(ee, local_table, next_local_code, table_size);					
				}
				break;
			}
			default:
				break;
			}

		}

		void StringTable::MessageEntryReplaceCodesWithStrings(RR_INTRUSIVE_PTR<MessageEntry> e, boost::unordered_map<uint32_t, std::string>& local_table)
		{
			DoReplaceCode(e->MemberName, e->MemberNameCode, e->EntryFlags,
				MessageEntryFlags_MEMBER_NAME_STR, MessageEntryFlags_MEMBER_NAME_CODE,
				local_table);

			DoReplaceCode(e->ServicePath, e->ServicePathCode, e->EntryFlags,
				MessageEntryFlags_SERVICE_PATH_STR, MessageEntryFlags_SERVICE_PATH_CODE,
				local_table);

			BOOST_FOREACH (RR_INTRUSIVE_PTR<MessageElement>& ee, e->elements)
			{
				MessageElementReplaceCodesWithStrings(ee, local_table);
			}
		}

		void StringTable::MessageElementReplaceCodesWithStrings(RR_INTRUSIVE_PTR<MessageElement> e, boost::unordered_map<uint32_t, std::string>& local_table)
		{
			DoReplaceCode(e->ElementName, e->ElementNameCode, e->ElementFlags,
				MessageElementFlags_ELEMENT_NAME_STR, MessageElementFlags_ELEMENT_NAME_CODE,
				local_table);

			DoReplaceCode(e->ElementTypeName, e->ElementTypeNameCode, e->ElementFlags,
				MessageElementFlags_ELEMENT_TYPE_NAME_STR, MessageElementFlags_ELEMENT_TYPE_NAME_CODE,
				local_table);

			switch (e->ElementType)
			{

			case DataTypes_structure_t:
			{
				RR_INTRUSIVE_PTR<MessageElementStructure> sdat = e->CastData<MessageElementStructure>();
				if (sdat)
				{
					BOOST_FOREACH (RR_INTRUSIVE_PTR<MessageElement>& ee, sdat->Elements)
						MessageElementReplaceCodesWithStrings(ee, local_table);
				}
				sdat->Type = e->ElementTypeName;
				break;
			}
			case DataTypes_vector_t:
			{
				RR_INTRUSIVE_PTR<MessageElementMap<int32_t> > vdat = e->CastData<MessageElementMap<int32_t> >();
				if (vdat)
				{
					BOOST_FOREACH (RR_INTRUSIVE_PTR<MessageElement>& ee, vdat->Elements)
						MessageElementReplaceCodesWithStrings(ee, local_table);
				}
				break;
			}
			case DataTypes_dictionary_t:
			{
				RR_INTRUSIVE_PTR<MessageElementMap<std::string> > ddat = e->CastData<MessageElementMap<std::string> >();
				if (ddat)
				{
					BOOST_FOREACH (RR_INTRUSIVE_PTR<MessageElement>& ee, ddat->Elements)
						MessageElementReplaceCodesWithStrings(ee, local_table);
				}
				break;
			}
			case DataTypes_multidimarray_t:
			{
				RR_INTRUSIVE_PTR<MessageElementMultiDimArray> mdat = e->CastData<MessageElementMultiDimArray>();
				if (mdat)
				{
					BOOST_FOREACH (RR_INTRUSIVE_PTR<MessageElement>& ee, mdat->Elements)
						MessageElementReplaceCodesWithStrings(ee, local_table);
				}
				break;
			}
			case DataTypes_list_t:
			{
				RR_INTRUSIVE_PTR<MessageElementList> ddat = e->CastData<MessageElementList>();
				if (ddat)
				{
					BOOST_FOREACH (RR_INTRUSIVE_PTR<MessageElement>& ee, ddat->Elements)
						MessageElementReplaceCodesWithStrings(ee, local_table);
				}
				break;
			}
			default:
				break;
			}
		}

		void StringTable::DoReplaceString(std::string& str, uint32_t& code, uint8_t& flags, uint32_t flag_str, uint32_t flag_code, boost::unordered_map<std::string, uint32_t>& local_table, uint32_t& next_local_code, uint32_t& table_size)
		{
			if (str.size() > max_str_len)
			{
				return;
			}

			if (flags & flag_str && !(flags & flag_code))
			{
				RR_SHARED_PTR<const StringTableEntry> s = GetEntryForString(str);
				if (s)
				{
					flags &= ~flag_str;
					flags |= flag_code;
					str = "";
					code = s->code;
				}
				else
				{
					if (!(this->flags & TransportCapabilityCode_MESSAGE3_STRINGTABLE_MESSAGE_LOCAL))
					{
						return;
					}

					uint32_t c;
					boost::unordered_map<std::string, uint32_t>::iterator e2 = local_table.find(str);
					if (e2 == local_table.end())
					{
						c = next_local_code;
						uint32_t l = ArrayBinaryWriter::GetUintXByteCount(c)
							+ ArrayBinaryWriter::GetStringByteCount8WithXLen(str);							

						if (table_size + l > 1024)
						{
							//Table is full, can't swap out value
							return;
						}
						local_table.insert(std::make_pair(str, c));
						next_local_code += 2;

						table_size += l;						
					}
					else
					{
						c = e2->second;
					}

					flags &= ~flag_str;
					flags |= flag_code;
					str = "";
					code = c;
				}
			}			
		}

		void StringTable::DoReplaceCode(std::string& str, uint32_t& code, uint8_t& flags, uint32_t flag_str, uint32_t flag_code, boost::unordered_map<uint32_t, std::string>& local_table)
		{
			if (!(flags & flag_str) && (flags & flag_code) && str.size() == 0)
			{
				if (code & 0x1)
				{
					boost::unordered_map<uint32_t, std::string>::iterator ee = local_table.find(code);
					if (ee == local_table.end())
					{
						throw ProtocolException("Invalid local string table code");
					}

					code = 0;
					str = ee->second;
					flags &= ~flag_code;
					flags |= flag_str;
				}
				else
				{
					RR_SHARED_PTR<const StringTableEntry> ee = GetEntryForCode(code);
					if (!ee)
					{
						throw ProtocolException("Invalid string table code");
					}

					code = 0;
					str = ee->value;
					flags &= ~flag_code;
					flags |= flag_str;
				}
			}
		}

		void StringTable::load_defaults()
		{
			AddCode(0x00, "", true);
			AddCode(0x04, "value", true);
			AddCode(0x08, "true", true);
			AddCode(0x0C, "false", true);
			AddCode(0x10, "return", true);
			AddCode(0x14, "errorname", true);
			AddCode(0x18, "errorstring", true);
			AddCode(0x1C, "dimcount", true);
			AddCode(0x20, "dims", true);
			AddCode(0x24, "real", true);
			AddCode(0x28, "imag", true);
			AddCode(0x2C, "RobotRaconteur.TimeSpec", true);
			AddCode(0x30, "seconds", true);
			AddCode(0x34, "nanoseconds", true);
			AddCode(0x38, "registerclient", true);
			AddCode(0x3C, "connectclientcombined", true);
			AddCode(0x40, "servicename", true);
			AddCode(0x44, "servicedef", true);
			AddCode(0x48, "servicedefs", true);
			AddCode(0x4C, "attributes", true);
			AddCode(0x50, "servicepath", true);
			AddCode(0x54, "clientversion", true);
			AddCode(0x58, "objecttype", true);
			AddCode(0x5C, "ServiceType", true);
			AddCode(0x60, "returnservicedefs", true);
			AddCode(0x64, "timeout", true);
			AddCode(0x68, "AuthenticateUser", true);
			AddCode(0x6C, "LogoutUser", true);
			AddCode(0x70, "username", true);
			AddCode(0x74, "credentials", true);
			AddCode(0x78, "RequestObjectLock", true);
			AddCode(0x7C, "RequestClientObjectLock", true);
			AddCode(0x80, "ReleaseObjectLock", true);
			AddCode(0x84, "MonitorEnter", true);
			AddCode(0x88, "MonitorContinueEnter", true);
			AddCode(0x8C, "MonitorExit", true);
			AddCode(0x90, "packet", true);
			AddCode(0x94, "index", true);
			AddCode(0x98, "packetnumber", true);
			AddCode(0x9C, "requestack", true);
			AddCode(0xA0, "unreliable", true);
			AddCode(0xA4, "packettime", true);
			AddCode(0xA8, "data", true);
			AddCode(0xAC, "count", true);
			AddCode(0xB0, "parameter", true);
			AddCode(0xB4, "memorypos", true);
			AddCode(0xB8, "confirmcodes", true);
			AddCode(0xBC, "pause", true);
			AddCode(0xC0, "resume", true);
			AddCode(0xC4, "CreateConnection", true);
			AddCode(0xC8, "GetRemoteNodeID", true);
			AddCode(0xCC, "messageversion", true);
			AddCode(0xD0, "stringtable", true);

		}

		bool StringTable::AddCode(uint32_t code, const std::string& str, bool default_)
		{
			if (code & 0x1) return false;
			if (default_)
			{
				if (code & 0x2) return false;
			}
			else
			{
				if (!(code & 0x2)) return false;
			}			

			if (str.size() > max_str_len) return false;

			boost::mutex::scoped_lock lock(this_lock);

			if (code_table.size() >= max_entry_count) return false;

			RR_SHARED_PTR<StringTableEntry> entry = RR_MAKE_SHARED<StringTableEntry>();
			entry->code = code;			
			entry->confirmed = true;
			entry->value = str;
			if (code_table.find(code) != code_table.end()) return false;
			code_table.insert(std::make_pair(entry->code, entry));
			string_table.insert(std::make_pair(entry->value, entry));
			return true;
		}

		StringTable::~StringTable()
		{
			//Diagnostic table dump

			/*boost::posix_time::ptime n = boost::posix_time::microsec_clock::local_time();
			std::string n1 = boost::posix_time::to_iso_string(n) + ".txt";

			std::ofstream f(n1, std::ofstream::out);
			
			
			typedef RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<StringTableEntry> >::value_type e_type;

			BOOST_FOREACH(const e_type& e, code_table)
			{
				f << e.first << " " << e.second->value << std::endl;
			}

			f.close();*/
		}

		std::vector<uint32_t> StringTable::GetTableFlags()
		{
			boost::mutex::scoped_lock lock(this_lock);
			std::vector<uint32_t> o;
			o.push_back(flags & TransportCapabilityCode_MESSAGE3_STRINGTABLE_PAGE);
			return o;
		}
		void StringTable::SetTableFlags(std::vector<uint32_t> flags_)
		{
			boost::mutex::scoped_lock lock(this_lock);
			BOOST_FOREACH(uint32_t f, flags_)
			{
				if ((f & TranspartCapabilityCode_PAGE_MASK) == TransportCapabilityCode_MESSAGE3_STRINGTABLE_PAGE)
				{
					flags = (f & (~TranspartCapabilityCode_PAGE_MASK));
				}
			}
		}
	}	
}