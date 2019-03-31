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

			std::string value;
			uint32_t code;
			bool confirmed;			
		};

		class ROBOTRACONTEUR_CORE_API StringTable : private boost::noncopyable
		{
		public:

			StringTable(bool server);
			virtual ~StringTable();

			uint32_t GetCodeForString(const std::string& str);
			bool GetStringForCode(uint32_t code, std::string& str);

			RR_SHARED_PTR<const StringTableEntry> GetEntryForString(const std::string& str);
			RR_SHARED_PTR<const StringTableEntry> GetEntryForCode(uint32_t code);

			size_t GetUnconfirmedCodeCount();
			std::vector<RR_SHARED_PTR<const StringTableEntry> > GetUnconfirmedCodes(uint32_t max_count);
			void ConfirmCodes(const std::vector<uint32_t> codes);
			void DropUnconfirmedCodes(const std::vector<uint32_t>& code);

			bool AddCode(uint32_t code, const std::string& str, bool default_=false);

			bool IsTableFull();

		public:

			void MessageReplaceStringsWithCodes(RR_INTRUSIVE_PTR<Message> m);
			void MessageReplaceCodesWithStrings(RR_INTRUSIVE_PTR<Message> m);

			std::vector<uint32_t> GetTableFlags();
			void SetTableFlags(std::vector<uint32_t> flags);

		protected:

			void MessageEntryReplaceStringsWithCodes(RR_INTRUSIVE_PTR<MessageEntry> e, boost::unordered_map<std::string, uint32_t>& local_table, uint32_t& next_local_code, uint32_t& table_size);
			void MessageElementReplaceStringsWithCodes(RR_INTRUSIVE_PTR<MessageElement> e, boost::unordered_map<std::string, uint32_t>& local_table, uint32_t& next_local_code, uint32_t& table_size);
			void MessageEntryReplaceCodesWithStrings(RR_INTRUSIVE_PTR<MessageEntry> e, boost::unordered_map<uint32_t, std::string>& local_table);
			void MessageElementReplaceCodesWithStrings(RR_INTRUSIVE_PTR<MessageElement> e, boost::unordered_map<uint32_t, std::string>& local_table);

			void DoReplaceString(std::string& str, uint32_t& code, uint8_t& flags, uint32_t flag_str, uint32_t flag_code, boost::unordered_map<std::string, uint32_t>& local_table, uint32_t& next_local_code, uint32_t& table_size);
			void DoReplaceCode(std::string& str, uint32_t& code, uint8_t& flags, uint32_t flag_str, uint32_t flag_code, boost::unordered_map<uint32_t, std::string>& local_table);

		protected:

			bool server;

			size_t max_entry_count;
			size_t max_str_len;
			uint32_t next_code;

			boost::mutex this_lock;

			RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<StringTableEntry> > code_table;
			RR_UNORDERED_MAP<std::string, RR_SHARED_PTR<StringTableEntry> > string_table;
			RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<StringTableEntry> > unconfirmed_code_table;
			RR_UNORDERED_MAP<std::string, RR_SHARED_PTR<StringTableEntry> > unconfirmed_string_table;

			void load_defaults();

			uint32_t flags;
						
		};

	}
}