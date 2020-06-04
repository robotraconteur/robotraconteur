/** 
 * @file Tap.h
 * 
 * @author Dr. John Wason
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

#pragma once

#include "RobotRaconteur/Logging.h"
#include "RobotRaconteur/Message.h"
#include <boost/bind/placeholders.hpp>
#include <boost/asio.hpp>

namespace RobotRaconteur
{
    class ROBOTRACONTEUR_CORE_API MessageTap
    {
    public:
        virtual void Open() = 0;
        virtual void Close() = 0;

        virtual void RecordLogRecord(const RRLogRecord& log_record) = 0;
        virtual void RecordMessage(RR_INTRUSIVE_PTR<Message> message) = 0;

        virtual ~MessageTap() {}
    };

    namespace detail
    {
        class LocalMessageTapImpl;
    }

    class ROBOTRACONTEUR_CORE_API LocalMessageTap : public MessageTap
    {
        RR_WEAK_PTR<detail::LocalMessageTapImpl> tap_impl;
        std::string tap_name;        
    public:

        LocalMessageTap(const std::string& tap_name);
        ~LocalMessageTap();

        virtual void Open();
        virtual void Close();

        virtual void RecordLogRecord(const RRLogRecord& log_record);
        virtual void RecordMessage(RR_INTRUSIVE_PTR<Message> message);
    };



};