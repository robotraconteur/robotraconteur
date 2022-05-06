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

#include "RobotRaconteur/PipeMember.h"

#pragma once

namespace RobotRaconteur
{
class ROBOTRACONTEUR_CORE_API PipeEndpointBaseListener
{
  public:
    virtual void PipeEndpointClosed(RR_SHARED_PTR<PipeEndpointBase> endpoint) = 0;
    virtual void PipePacketReceived(RR_SHARED_PTR<PipeEndpointBase> endpoint,
                                    boost::function<bool(RR_INTRUSIVE_PTR<RRValue>&)> receive_packet_func) = 0;
    virtual void PipePacketAckReceived(RR_SHARED_PTR<PipeEndpointBase> endpoint, uint32_t pnum) = 0;
    virtual ~PipeEndpointBaseListener() {}
};
} // namespace RobotRaconteur