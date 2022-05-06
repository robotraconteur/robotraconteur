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

%shared_ptr(RobotRaconteur::RobotRaconteurNodeSetup)
%shared_ptr(RobotRaconteur::CommandLineConfigParser)

%include "RobotRaconteur/NodeSetup.h"

namespace RobotRaconteur
{

	class CommandLineConfigParser
	{		
	public:
				
		CommandLineConfigParser(uint32_t allowed_overrides, const std::string& prefix="robotraconteur-");

		void SetDefaults(const std::string& node_name, uint16_t tcp_port, uint32_t default_flags);

		void AddStringOption(const std::string& name, const std::string& descr);
		void AddBoolOption(const std::string& name, const std::string& descr);
		void AddIntOption(const std::string& name, const std::string& descr);

		void ParseCommandLine(const std::vector<std::string>& args);
		
		std::string GetOptionOrDefaultAsString(const std::string& option);
		std::string GetOptionOrDefaultAsString(const std::string& option, const std::string& default_value);
		bool GetOptionOrDefaultAsBool(const std::string& option);
		bool GetOptionOrDefaultAsBool(const std::string& option, bool default_value);
		int32_t GetOptionOrDefaultAsInt(const std::string& option);
		int32_t GetOptionOrDefaultAsInt(const std::string& option, int32_t default_value);
	};


	%nodefaultctor RobotRaconteurNodeSetup;
	%rename(WrappedRobotRaconteurNodeSetup) RobotRaconteurNodeSetup;
    class RobotRaconteurNodeSetup
	{
	public:
		//RobotRaconteurNodeSetup(const RR_SHARED_PTR<RobotRaconteurNode>& node, const std::vector<RR_SHARED_PTR<ServiceFactory> >& service_types, 
		//	const std::string& node_name, uint16_t tcp_port, uint32_t flags);
		
		boost::shared_ptr<RobotRaconteur::LocalTransport> GetLocalTransport();
		boost::shared_ptr<RobotRaconteur::IntraTransport> GetIntraTransport();
		boost::shared_ptr<RobotRaconteur::TcpTransport> GetTcpTransport();
		boost::shared_ptr<RobotRaconteur::HardwareTransport> GetHardwareTransport();
		boost::shared_ptr<RobotRaconteur::CommandLineConfigParser> GetCommandLineConfig();

		void ReleaseNode();
		
		virtual ~RobotRaconteurNodeSetup();
	};
	
	%extend RobotRaconteurNodeSetup
	{
		RobotRaconteurNodeSetup(const boost::shared_ptr<RobotRaconteur::RobotRaconteurNode>& node,
			const std::string& node_name, uint16_t tcp_port, uint32_t flags)
			{
				std::vector<RR_SHARED_PTR<ServiceFactory> > s;
				RobotRaconteurNodeSetup* n = new RobotRaconteurNodeSetup(node, s, node_name, tcp_port, flags);
				return n;
			}	

		RobotRaconteurNodeSetup(const boost::shared_ptr<RobotRaconteur::RobotRaconteurNode>& node,
			const boost::shared_ptr<RobotRaconteur::CommandLineConfigParser>& config)
			{
				std::vector<RR_SHARED_PTR<ServiceFactory> > s;
				RobotRaconteurNodeSetup* n = new RobotRaconteurNodeSetup(node, s, config);
				return n;
			}

		RobotRaconteurNodeSetup(const boost::shared_ptr<RobotRaconteur::RobotRaconteurNode>& node, 
			const std::string& node_name, uint16_t tcp_port, uint32_t flags, uint32_t allowed_overrides, 
			const std::vector<std::string>& args)
			{
				std::vector<RR_SHARED_PTR<ServiceFactory> > s;
				RobotRaconteurNodeSetup* n = new RobotRaconteurNodeSetup(node, s, node_name, tcp_port, flags, allowed_overrides, args);
				return n;
			}
	}

}