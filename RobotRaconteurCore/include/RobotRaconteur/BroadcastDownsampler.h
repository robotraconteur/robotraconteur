/** 
 * @file Generator.h
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

#include "RobotRaconteur/RobotRaconteurConfig.h"
#include <boost/unordered_map.hpp>

namespace RobotRaconteur
{
    class ROBOTRACONTEUR_CORE_API ServerContext;
    class ROBOTRACONTEUR_CORE_API PipeBroadcasterBase;
    class ROBOTRACONTEUR_CORE_API WireBroadcasterBase;

    class ROBOTRACONTEUR_CORE_API BroadcastDownsampler : public RR_ENABLE_SHARED_FROM_THIS<BroadcastDownsampler>
    {
    public:
        BroadcastDownsampler();

        void Init(RR_SHARED_PTR<ServerContext> context, uint32_t default_downsample = 0);

        uint32_t GetClientDownsample(uint32_t ep);
        void SetClientDownsample(uint32_t ep, uint32_t downsample);

        void BeginStep();
        void EndStep();

        void AddPipeBroadcaster(RR_SHARED_PTR<PipeBroadcasterBase> broadcaster);
        void AddWireBroadcaster(RR_SHARED_PTR<WireBroadcasterBase> broadcaster);

    protected:

        RR_WEAK_PTR<ServerContext> context;

        uint32_t default_downsample;
        uint64_t step_count;
        RR_UNORDERED_MAP<uint32_t, uint32_t> client_downsamples;

        boost::mutex this_lock;

        static bool wire_predicate(RR_WEAK_PTR<BroadcastDownsampler> this_, RR_SHARED_PTR<WireBroadcasterBase>& wire, uint32_t ep);
        static bool pipe_predicate(RR_WEAK_PTR<BroadcastDownsampler> this_, RR_SHARED_PTR<PipeBroadcasterBase>& wire, uint32_t ep, uint32_t index);

        static void server_event(RR_WEAK_PTR<BroadcastDownsampler> this_, RR_SHARED_PTR<ServerContext> ctx, ServerServiceListenerEventType evt, RR_SHARED_PTR<void> p);
    };

    class ROBOTRACONTEUR_CORE_API BroadcastDownsamplerStep
    {
        RR_SHARED_PTR<BroadcastDownsampler>& _downsampler;
    public:
        BroadcastDownsamplerStep(RR_SHARED_PTR<BroadcastDownsampler>& downsampler);
        ~BroadcastDownsamplerStep();
    };
}