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

#pragma once

#include "RobotRaconteur/DataTypes.h"

#include "RobotRaconteur/Message.h"

namespace RobotRaconteur
{
	class ROBOTRACONTEUR_CORE_API RobotRaconteurExceptionUtil
	{
	public:
		static void ExceptionToMessageEntry(std::exception &exception, RR_INTRUSIVE_PTR<MessageEntry> entry);

		static RR_SHARED_PTR<RobotRaconteurException> MessageEntryToException(RR_INTRUSIVE_PTR<MessageEntry> entry);

		static void ThrowMessageEntryException(RR_INTRUSIVE_PTR<MessageEntry> entry);

		static RR_SHARED_PTR<RobotRaconteurException> DownCastException(RobotRaconteurException& err);

		static RR_SHARED_PTR<RobotRaconteurException> ExceptionToSharedPtr(std::exception& err, MessageErrorType default_type=MessageErrorType_UnknownError);

		static void DownCastAndThrowException(RR_SHARED_PTR<RobotRaconteurException> err);
	};

}