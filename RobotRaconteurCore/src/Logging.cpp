// Copyright 2011-2019 Wason Technology, LLC
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

#include "RobotRaconteur/Logging.h"
#include "RobotRaconteur/RobotRaconteurNode.h"

namespace RobotRaconteur
{
    namespace detail
    {
        std::string rr_get_node_log_ident()
        {
            return "";
        }

        std::string rr_get_node_log_ident(RR_WEAK_PTR<RobotRaconteur::RobotRaconteurNode> node)
        {
            RR_SHARED_PTR<RobotRaconteurNode> node1 = node.lock();
            if (!node1)
            {
                return "";
            }

            NodeID id;
            std::string name;
            if (!node1->TryGetNodeID(id))
            {
                return "";
            }
            if (!node1->TryGetNodeName(name))
            {
                return "node: " + id.ToString("B") + " ";
            }
            return "node: " + id.ToString("B") + "," + name + " ";
        }
    }
}