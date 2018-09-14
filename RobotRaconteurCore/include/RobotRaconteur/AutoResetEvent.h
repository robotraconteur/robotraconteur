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

namespace RobotRaconteur
{
	class ROBOTRACONTEUR_CORE_API AutoResetEvent : private boost::noncopyable
	{
	public:

		AutoResetEvent();
		virtual ~AutoResetEvent();

		virtual void Set();

		virtual void Reset();

		virtual void WaitOne();

		virtual bool WaitOne(int32_t timeout);
#ifdef ROBOTRACONTEUR_WINDOWS
	private:
		void* ev;
#else
	private:
		volatile bool m_bSet;
		boost::condition_variable m_setCondition;
		boost::mutex m_mutex;
#endif

	};

	


}