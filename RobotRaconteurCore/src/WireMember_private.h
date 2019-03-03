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

#include "RobotRaconteur/WireMember.h"

#pragma once

namespace RobotRaconteur
{
	class ROBOTRACONTEUR_CORE_API WireConnectionBaseListener
	{
	public:
		virtual void WireConnectionClosed(RR_SHARED_PTR<WireConnectionBase> connection) = 0;
		virtual void WireValueChanged(RR_SHARED_PTR<WireConnectionBase> connection, RR_INTRUSIVE_PTR<RRValue> value, const TimeSpec& time) = 0;
		virtual ~WireConnectionBaseListener() {}
	};
}