/**
 * @file StringTable.h
 *
 * @author John Wason, PhD
 *
 * @copyright Copyright 2011-2020 Wason Technology, LLC
 *
 * @par License
 * Software License Agreement (Apache License)
 * @par
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * @par
 * http://www.apache.org/licenses/LICENSE-2.0
 * @par
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "RobotRaconteur/DataTypes.h"
#include <boost/unordered_map.hpp>

#pragma once

namespace RobotRaconteur
{
class Message;
class MessageEntry;
class MessageElement;

namespace detail
{
class ROBOTRACONTEUR_CORE_API StringTableEntry
{
  public:
    StringTableEntry();

    MessageStringPtr value;
    uint32_t code;
    bool confirmed;
    std::vector<uint32_t> table_flags;
};

class ROBOTRACONTEUR_CORE_API StringTable : private boost::noncopyable
{
  public:
    StringTable(bool server);
    virtual ~StringTable();

    uint32_t GetCodeForString(MessageStringRef str);
    bool GetStringForCode(uint32_t code, MessageStringPtr& str);

    RR_SHARED_PTR<const StringTableEntry> GetEntryForString(MessageStringRef str);
    RR_SHARED_PTR<const StringTableEntry> GetEntryForCode(uint32_t code);

    bool AddCode(uint32_t code, MessageStringRef str, const std::vector<uint32_t>& table_flags);
    void AddCodesCSV(const std::string& csv, const std::vector<uint32_t>& table_flags);

  public:
    void MessageReplaceStringsWithCodes(RR_INTRUSIVE_PTR<Message> m);
    void MessageReplaceCodesWithStrings(RR_INTRUSIVE_PTR<Message> m);

    std::vector<uint32_t> GetTableFlags();
    void SetTableFlags(std::vector<uint32_t> flags);

  protected:
    void MessageEntryReplaceStringsWithCodes(RR_INTRUSIVE_PTR<MessageEntry> e,
                                             boost::unordered_map<MessageStringPtr, uint32_t>& local_table,
                                             uint32_t& next_local_code, uint32_t& table_size);
    void MessageElementReplaceStringsWithCodes(RR_INTRUSIVE_PTR<MessageElement> e,
                                               boost::unordered_map<MessageStringPtr, uint32_t>& local_table,
                                               uint32_t& next_local_code, uint32_t& table_size);
    void MessageEntryReplaceCodesWithStrings(RR_INTRUSIVE_PTR<MessageEntry> e,
                                             boost::unordered_map<uint32_t, MessageStringPtr>& local_table);
    void MessageElementReplaceCodesWithStrings(RR_INTRUSIVE_PTR<MessageElement> e,
                                               boost::unordered_map<uint32_t, MessageStringPtr>& local_table);

    void DoReplaceString(MessageStringPtr& str, uint32_t& code, uint8_t& flags, uint32_t flag_str, uint32_t flag_code,
                         boost::unordered_map<MessageStringPtr, uint32_t>& local_table, uint32_t& next_local_code,
                         uint32_t& table_size);
    void DoReplaceCode(MessageStringPtr& str, uint32_t& code, uint8_t& flags, uint32_t flag_str, uint32_t flag_code,
                       boost::unordered_map<uint32_t, MessageStringPtr>& local_table);

    bool _AddCode(uint32_t code, MessageStringRef str, const std::vector<uint32_t>& table_flags);
    void _AddCodesCSV(const std::string& csv, const std::vector<uint32_t>& table_flags);

  protected:
    bool server;

    size_t max_entry_count;
    size_t max_str_len;
    uint32_t next_code;

    boost::mutex this_lock;

    RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<StringTableEntry> > code_table;
    RR_UNORDERED_MAP<MessageStringPtr, RR_SHARED_PTR<StringTableEntry> > string_table;

    uint32_t flags;
};

} // namespace detail
} // namespace RobotRaconteur