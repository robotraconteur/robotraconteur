/**
 * @file ErrorUtil.h
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

#pragma once

#include "RobotRaconteur/DataTypes.h"

#include "RobotRaconteur/Message.h"

namespace RobotRaconteur
{
class ROBOTRACONTEUR_CORE_API RobotRaconteurExceptionUtil
{
  public:
    static void ExceptionToMessageEntry(std::exception& exception, const RR_INTRUSIVE_PTR<MessageEntry>& entry);

    static RR_SHARED_PTR<RobotRaconteurException> MessageEntryToException(const RR_INTRUSIVE_PTR<MessageEntry>& entry);

    static void ThrowMessageEntryException(const RR_INTRUSIVE_PTR<MessageEntry>& entry);

    static RR_SHARED_PTR<RobotRaconteurException> DownCastException(RobotRaconteurException& err);

    static RR_SHARED_PTR<RobotRaconteurException> DownCastException(const RR_SHARED_PTR<RobotRaconteurException>& err);

    static RR_SHARED_PTR<RobotRaconteurException> ExceptionToSharedPtr(
        std::exception& err, MessageErrorType default_type = MessageErrorType_UnknownError);

    static void DownCastAndThrowException(RobotRaconteurException& err);

    static void DownCastAndThrowException(const RR_SHARED_PTR<RobotRaconteurException>& err);
};

} // namespace RobotRaconteur