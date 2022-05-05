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

#include "RobotRaconteur/StringTable.h"
#include "RobotRaconteur/Message.h"
#include "RobotRaconteur/IOUtils.h"

#include <boost/range/adaptors.hpp>
#include <boost/range/algorithm.hpp>
#include <boost/foreach.hpp>

#include <boost/algorithm/string.hpp>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <fstream>

#include "StringTable_default_table.h"

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
    max_entry_count = 8192;
    max_str_len = 128;
    next_code = 2;
    flags = 0;
}

uint32_t StringTable::GetCodeForString(MessageStringRef str)
{
    RR_SHARED_PTR<const StringTableEntry> e = GetEntryForString(str);
    if (!e)
        return 0;
    return e->code;
}

bool StringTable::GetStringForCode(uint32_t code, MessageStringPtr& str)
{
    RR_SHARED_PTR<const StringTableEntry> e = GetEntryForCode(code);
    if (!e)
        return false;
    str = e->value;
    return true;
}

RR_SHARED_PTR<const StringTableEntry> StringTable::GetEntryForString(MessageStringRef str)
{
    boost::mutex::scoped_lock lock(this_lock);
    RR_UNORDERED_MAP<MessageStringPtr, RR_SHARED_PTR<StringTableEntry> >::iterator e = string_table.find(str);
    if (e == string_table.end())
    {
        return RR_SHARED_PTR<const StringTableEntry>();
    }

    if (e->second->table_flags.empty())
    {
        return RR_SHARED_PTR<const StringTableEntry>();
    }

    bool flags_enabled = false;
    BOOST_FOREACH (uint32_t f, e->second->table_flags)
    {
        if ((f & flags) == f)
        {
            flags_enabled = true;
            break;
        }
    }
    if (!flags_enabled)
    {
        return RR_SHARED_PTR<const StringTableEntry>();
    }

    return e->second;
}
RR_SHARED_PTR<const StringTableEntry> StringTable::GetEntryForCode(uint32_t code)
{
    boost::mutex::scoped_lock lock(this_lock);
    RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<StringTableEntry> >::iterator e = code_table.find(code);
    if (e != code_table.end())
    {
        bool flags_enabled = false;
        BOOST_FOREACH (uint32_t f, e->second->table_flags)
        {
            if ((f & flags) == f)
            {
                flags_enabled = true;
                break;
            }
        }
        if (!flags_enabled)
        {
            return RR_SHARED_PTR<const StringTableEntry>();
        }
        return e->second;
    }
    return RR_SHARED_PTR<const StringTableEntry>();
}

void StringTable::MessageReplaceStringsWithCodes(RR_INTRUSIVE_PTR<Message> m)
{
    if (m->header->MessageFlags & MessageFlags_STRING_TABLE || m->header->StringTable.size() > 0)
    {
        return;
    }

    boost::unordered_map<MessageStringPtr, uint32_t> local_table;
    uint32_t next_local_code = 1;
    uint32_t table_size = 0;

    BOOST_FOREACH (RR_INTRUSIVE_PTR<MessageEntry>& e, m->entries)
    {
        MessageEntryReplaceStringsWithCodes(e, local_table, next_local_code, table_size);
    }

    if (local_table.size() > 0)
    {
        m->header->MessageFlags |= MessageFlags_STRING_TABLE;
        typedef boost::unordered_map<MessageStringPtr, uint32_t>::value_type e_type;
        BOOST_FOREACH (e_type& e, local_table)
        {
            m->header->StringTable.push_back(boost::make_tuple(e.second, e.first));
        }
    }
}

void StringTable::MessageReplaceCodesWithStrings(RR_INTRUSIVE_PTR<Message> m)
{
    boost::unordered_map<uint32_t, MessageStringPtr> local_table;

    if (m->header->MessageFlags & MessageFlags_STRING_TABLE)
    {
        typedef boost::tuple<uint32_t, MessageStringPtr> e_type;
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

void StringTable::MessageEntryReplaceStringsWithCodes(RR_INTRUSIVE_PTR<MessageEntry> e,
                                                      boost::unordered_map<MessageStringPtr, uint32_t>& local_table,
                                                      uint32_t& next_local_code, uint32_t& table_size)
{
    DoReplaceString(e->MemberName, e->MemberNameCode, e->EntryFlags, MessageEntryFlags_MEMBER_NAME_STR,
                    MessageEntryFlags_MEMBER_NAME_CODE, local_table, next_local_code, table_size);

    DoReplaceString(e->ServicePath, e->ServicePathCode, e->EntryFlags, MessageEntryFlags_SERVICE_PATH_STR,
                    MessageEntryFlags_SERVICE_PATH_CODE, local_table, next_local_code, table_size);

    BOOST_FOREACH (RR_INTRUSIVE_PTR<MessageElement>& ee, e->elements)
    {
        MessageElementReplaceStringsWithCodes(ee, local_table, next_local_code, table_size);
    }
}

void StringTable::MessageElementReplaceStringsWithCodes(RR_INTRUSIVE_PTR<MessageElement> e,
                                                        boost::unordered_map<MessageStringPtr, uint32_t>& local_table,
                                                        uint32_t& next_local_code, uint32_t& table_size)
{
    DoReplaceString(e->ElementName, e->ElementNameCode, e->ElementFlags, MessageElementFlags_ELEMENT_NAME_STR,
                    MessageElementFlags_ELEMENT_NAME_CODE, local_table, next_local_code, table_size);

    DoReplaceString(e->ElementTypeName, e->ElementTypeNameCode, e->ElementFlags,
                    MessageElementFlags_ELEMENT_TYPE_NAME_STR, MessageElementFlags_ELEMENT_TYPE_NAME_CODE, local_table,
                    next_local_code, table_size);

    switch (e->ElementType)
    {

    case DataTypes_structure_t:
    case DataTypes_vector_t:
    case DataTypes_dictionary_t:
    case DataTypes_multidimarray_t:
    case DataTypes_list_t:
    case DataTypes_pod_t:
    case DataTypes_pod_array_t:
    case DataTypes_pod_multidimarray_t:
    case DataTypes_namedarray_array_t:
    case DataTypes_namedarray_multidimarray_t: {
        RR_INTRUSIVE_PTR<MessageElementNestedElementList> sdat = e->CastDataToNestedList();
        if (sdat)
        {
            BOOST_FOREACH (RR_INTRUSIVE_PTR<MessageElement>& ee, sdat->Elements)
                MessageElementReplaceStringsWithCodes(ee, local_table, next_local_code, table_size);
        }
        break;
    }
    default:
        break;
    }
}

void StringTable::MessageEntryReplaceCodesWithStrings(RR_INTRUSIVE_PTR<MessageEntry> e,
                                                      boost::unordered_map<uint32_t, MessageStringPtr>& local_table)
{
    DoReplaceCode(e->MemberName, e->MemberNameCode, e->EntryFlags, MessageEntryFlags_MEMBER_NAME_STR,
                  MessageEntryFlags_MEMBER_NAME_CODE, local_table);

    DoReplaceCode(e->ServicePath, e->ServicePathCode, e->EntryFlags, MessageEntryFlags_SERVICE_PATH_STR,
                  MessageEntryFlags_SERVICE_PATH_CODE, local_table);

    BOOST_FOREACH (RR_INTRUSIVE_PTR<MessageElement>& ee, e->elements)
    {
        MessageElementReplaceCodesWithStrings(ee, local_table);
    }
}

void StringTable::MessageElementReplaceCodesWithStrings(RR_INTRUSIVE_PTR<MessageElement> e,
                                                        boost::unordered_map<uint32_t, MessageStringPtr>& local_table)
{
    DoReplaceCode(e->ElementName, e->ElementNameCode, e->ElementFlags, MessageElementFlags_ELEMENT_NAME_STR,
                  MessageElementFlags_ELEMENT_NAME_CODE, local_table);

    DoReplaceCode(e->ElementTypeName, e->ElementTypeNameCode, e->ElementFlags,
                  MessageElementFlags_ELEMENT_TYPE_NAME_STR, MessageElementFlags_ELEMENT_TYPE_NAME_CODE, local_table);

    switch (e->ElementType)
    {

    case DataTypes_structure_t:
    case DataTypes_vector_t:
    case DataTypes_dictionary_t:
    case DataTypes_multidimarray_t:
    case DataTypes_list_t:
    case DataTypes_pod_t:
    case DataTypes_pod_array_t:
    case DataTypes_pod_multidimarray_t:
    case DataTypes_namedarray_array_t:
    case DataTypes_namedarray_multidimarray_t: {
        RR_INTRUSIVE_PTR<MessageElementNestedElementList> sdat = e->CastDataToNestedList();
        if (sdat)
        {
            BOOST_FOREACH (RR_INTRUSIVE_PTR<MessageElement>& ee, sdat->Elements)
                MessageElementReplaceCodesWithStrings(ee, local_table);
        }
        sdat->TypeName = e->ElementTypeName;
        break;
    }

    default:
        break;
    }
}

void StringTable::DoReplaceString(MessageStringPtr& str, uint32_t& code, uint8_t& flags, uint32_t flag_str,
                                  uint32_t flag_code, boost::unordered_map<MessageStringPtr, uint32_t>& local_table,
                                  uint32_t& next_local_code, uint32_t& table_size)
{
    if (str.str().size() > max_str_len)
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
            str.reset();
            code = s->code;
        }
        else
        {
            if (!(this->flags & TransportCapabilityCode_MESSAGE4_STRINGTABLE_MESSAGE_LOCAL))
            {
                return;
            }

            uint32_t c;
            boost::unordered_map<MessageStringPtr, uint32_t>::iterator e2 = local_table.find(str);
            if (e2 == local_table.end())
            {
                c = next_local_code;
                uint32_t l =
                    ArrayBinaryWriter::GetUintXByteCount(c) + ArrayBinaryWriter::GetStringByteCount8WithXLen(str);

                if (table_size + l > 1024)
                {
                    // Table is full, can't swap out value
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
            str.reset();
            code = c;
        }
    }
}

void StringTable::DoReplaceCode(MessageStringPtr& str, uint32_t& code, uint8_t& flags, uint32_t flag_str,
                                uint32_t flag_code, boost::unordered_map<uint32_t, MessageStringPtr>& local_table)
{
    if (!(flags & flag_str) && (flags & flag_code) && str.str().size() == 0)
    {
        if (code & 0x1)
        {
            boost::unordered_map<uint32_t, MessageStringPtr>::iterator ee = local_table.find(code);
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

bool StringTable::AddCode(uint32_t code, MessageStringRef str, const std::vector<uint32_t>& table_flags)
{
    boost::mutex::scoped_lock lock(this_lock);
    return _AddCode(code, str, table_flags);
}

bool StringTable::_AddCode(uint32_t code, MessageStringRef str, const std::vector<uint32_t>& table_flags)
{
    if (code & 0x1)
        return false;

    if (str.str().size() > max_str_len)
        return false;

    if (code_table.size() >= max_entry_count)
        return false;

    RR_SHARED_PTR<StringTableEntry> entry = RR_MAKE_SHARED<StringTableEntry>();
    entry->code = code;
    entry->confirmed = true;
    entry->value = str;
    entry->table_flags = table_flags;
    if (code_table.find(code) != code_table.end())
    {
        return false;
    }
    code_table.insert(std::make_pair(entry->code, entry));
    if (string_table.find(entry->value) == string_table.end())
    {
        string_table.insert(std::make_pair(entry->value, entry));
    }
    return true;
}

void StringTable::AddCodesCSV(const std::string& csv, const std::vector<uint32_t>& table_flags)
{
    boost::mutex::scoped_lock lock(this_lock);
    _AddCodesCSV(csv, table_flags);
}

void StringTable::_AddCodesCSV(const std::string& csv, const std::vector<uint32_t>& table_flags)
{
    typedef boost::split_iterator<std::string::const_iterator> string_split_iterator;

    boost::regex r_csv_line("^(\\d+),(.*)$");

    for (string_split_iterator e =
             boost::make_split_iterator(csv, boost::token_finder(boost::is_any_of("\n\r"), boost::token_compress_on));
         e != string_split_iterator(); e++)
    {
        boost::smatch line_match;
        if (!boost::regex_match(e->begin(), e->end(), line_match, r_csv_line))
        {
            throw InvalidArgumentException("Invalid CSV code table specified");
        }

        uint32_t code = boost::lexical_cast<uint32_t>(line_match[1].str());
        std::string str = line_match[2].str();

        _AddCode(code, str, table_flags);
    }
}

StringTable::~StringTable()
{
    // Diagnostic table dump

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
    o.push_back(flags & TransportCapabilityCode_MESSAGE4_STRINGTABLE_PAGE);
    return o;
}
void StringTable::SetTableFlags(std::vector<uint32_t> flags_)
{
    boost::mutex::scoped_lock lock(this_lock);
    BOOST_FOREACH (uint32_t f, flags_)
    {
        if ((f & TranspartCapabilityCode_PAGE_MASK) == TransportCapabilityCode_MESSAGE4_STRINGTABLE_PAGE)
        {
            flags = (f & (~TranspartCapabilityCode_PAGE_MASK));
        }
    }

    if (flags & TransportCapabilityCode_MESSAGE4_STRINGTABLE_STANDARD_TABLE)
    {
        std::vector<uint32_t> table_flags;
        table_flags.push_back(TransportCapabilityCode_MESSAGE4_STRINGTABLE_STANDARD_TABLE);
        _AddCodesCSV(RobotRaconteur::detail::StringTable_default_table_csv, table_flags);
    }
}
} // namespace detail
} // namespace RobotRaconteur