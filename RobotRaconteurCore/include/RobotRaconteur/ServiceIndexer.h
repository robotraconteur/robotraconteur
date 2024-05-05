/**
 * @file ServiceIndexer.h
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

#include "RobotRaconteurServiceIndex_stubskel.h"

#pragma once

namespace RobotRaconteur
{
class ServiceIndexer : public virtual RobotRaconteurServiceIndex::ServiceIndex
{
    RR_WEAK_PTR<RobotRaconteurNode> node;

  public:
    ServiceIndexer(const RR_SHARED_PTR<RobotRaconteurNode>& node);

    RR_SHARED_PTR<RobotRaconteurNode> GetNode();

    RR_OVIRTUAL RR_INTRUSIVE_PTR<RobotRaconteur::RRMap<int32_t, RobotRaconteurServiceIndex::ServiceInfo> >
    GetLocalNodeServices() RR_OVERRIDE;

    RR_OVIRTUAL RR_INTRUSIVE_PTR<RobotRaconteur::RRMap<int32_t, RobotRaconteurServiceIndex::NodeInfo> > GetRoutedNodes()
        RR_OVERRIDE;

    RR_OVIRTUAL RR_INTRUSIVE_PTR<RobotRaconteur::RRMap<int32_t, RobotRaconteurServiceIndex::NodeInfo> >
    GetDetectedNodes() RR_OVERRIDE;

    RR_OVIRTUAL boost::signals2::signal<void()>& get_LocalNodeServicesChanged() RR_OVERRIDE;

  private:
    boost::signals2::signal<void()> ev;
};
} // namespace RobotRaconteur
