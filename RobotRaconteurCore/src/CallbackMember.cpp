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

#include "RobotRaconteur/CallbackMember.h"
#include "RobotRaconteur/Service.h"
#include "RobotRaconteur/Client.h"

namespace RobotRaconteur
{
RR_SHARED_PTR<void> CallbackServerBase::GetClientFunction_internal(uint32_t e)
{

    RR_SHARED_PTR<ServiceSkel> s = skel.lock();
    if (!s)
        throw InvalidOperationException("Callback server closed");
    return (s->GetCallbackFunction(e, GetMemberName()));
}

} // namespace RobotRaconteur