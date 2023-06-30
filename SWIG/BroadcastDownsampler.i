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

%shared_ptr(RobotRaconteur::BroadcastDownsampler)

namespace RobotRaconteur
{
    class BroadcastDownsampler
    {
    public:
        BroadcastDownsampler();

        void Init(const boost::shared_ptr<RobotRaconteur::ServerContext>& context, uint32_t default_downsample = 0);

        uint32_t GetClientDownsample(uint32_t ep);
        void SetClientDownsample(uint32_t ep, uint32_t downsample);

        void BeginStep();
        void EndStep();

        RR_MAKE_METHOD_PRIVATE(AddPipeBroadcaster)
        void AddPipeBroadcaster(const boost::shared_ptr<RobotRaconteur::WrappedPipeBroadcaster>& broadcaster);
        RR_MAKE_METHOD_PRIVATE(AddWireBroadcaster)
        void AddWireBroadcaster(const boost::shared_ptr<RobotRaconteur::WrappedWireBroadcaster>& broadcaster);
    };
}