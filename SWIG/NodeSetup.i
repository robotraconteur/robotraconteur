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

%shared_ptr(RobotRaconteur::RobotRaconteurNodeSetup)

%include "RobotRaconteur/NodeSetup.h"

namespace RobotRaconteur
{
	%nodefaultctor RobotRaconteurNodeSetup;
	%rename(WrappedRobotRaconteurNodeSetup) RobotRaconteurNodeSetup;
    class RobotRaconteurNodeSetup
	{
	public:
		//RobotRaconteurNodeSetup(RR_SHARED_PTR<RobotRaconteurNode> node, const std::vector<RR_SHARED_PTR<ServiceFactory> > service_types, 
		//	const std::string& node_name, uint16_t tcp_port, uint32_t flags);
		
		boost::shared_ptr<LocalTransport> GetLocalTransport();
		boost::shared_ptr<TcpTransport> GetTcpTransport();
		boost::shared_ptr<HardwareTransport> GetHardwareTransport();
		
		virtual ~RobotRaconteurNodeSetup();
	};
	
	%extend RobotRaconteurNodeSetup
	{
		RobotRaconteurNodeSetup(boost::shared_ptr<RobotRaconteur::RobotRaconteurNode> node,
			const std::string& node_name, uint16_t tcp_port, uint32_t flags)
			{
				std::vector<RR_SHARED_PTR<ServiceFactory> > s;
				RobotRaconteurNodeSetup* n = new RobotRaconteurNodeSetup(node, s, node_name, tcp_port, flags);
				return n;
			}	
	}

}