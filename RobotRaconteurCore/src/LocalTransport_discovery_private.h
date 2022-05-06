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

#include <boost/filesystem.hpp>

#pragma once

namespace RobotRaconteur
{
namespace detail
{

class LocalTransportDiscovery
{
  public:
    LocalTransportDiscovery(RR_SHARED_PTR<RobotRaconteurNode> node);
    virtual void Init() = 0;
    virtual void Shutdown() = 0;
    virtual ~LocalTransportDiscovery() {}
    virtual void Refresh();

  protected:
    RR_WEAK_PTR<RobotRaconteurNode> node;

    boost::optional<boost::filesystem::path> private_path;
    boost::optional<boost::filesystem::path> public_path;
};
} // namespace detail
} // namespace RobotRaconteur
