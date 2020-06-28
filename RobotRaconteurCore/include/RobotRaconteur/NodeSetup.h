/** 
 * @file NodeSetup.h
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

#ifndef SWIG
#include "RobotRaconteur/RobotRaconteurNode.h"
#include "RobotRaconteur/TcpTransport.h"
#include "RobotRaconteur/LocalTransport.h"
#include "RobotRaconteur/HardwareTransport.h"
#include "RobotRaconteur/IntraTransport.h"

#include <boost/assign/list_of.hpp>
#include <boost/program_options.hpp>

#endif

#pragma once

namespace RobotRaconteur
{


	enum RobotRaconteurNodeSetupFlags
	{
		RobotRaconteurNodeSetupFlags_NONE = 0x0,
		RobotRaconteurNodeSetupFlags_ENABLE_NODE_DISCOVERY_LISTENING = 0x1,
		RobotRaconteurNodeSetupFlags_ENABLE_NODE_ANNOUNCE = 0x2,
		RobotRaconteurNodeSetupFlags_ENABLE_LOCAL_TRANSPORT = 0x4,
		RobotRaconteurNodeSetupFlags_ENABLE_TCP_TRANSPORT = 0x8,
		RobotRaconteurNodeSetupFlags_ENABLE_HARDWARE_TRANSPORT = 0x10,
		RobotRaconteurNodeSetupFlags_LOCAL_TRANSPORT_START_SERVER = 0x20,
		RobotRaconteurNodeSetupFlags_LOCAL_TRANSPORT_START_CLIENT = 0x40,
		RobotRaconteurNodeSetupFlags_TCP_TRANSPORT_START_SERVER = 0x80,
		RobotRaconteurNodeSetupFlags_TCP_TRANSPORT_START_SERVER_PORT_SHARER = 0x100,
		RobotRaconteurNodeSetupFlags_DISABLE_MESSAGE4  = 0x200,
		RobotRaconteurNodeSetupFlags_DISABLE_STRINGTABLE = 0x400,
		RobotRaconteurNodeSetupFlags_DISABLE_TIMEOUTS = 0x800,
		RobotRaconteurNodeSetupFlags_LOAD_TLS_CERT = 0x1000,
		RobotRaconteurNodeSetupFlags_REQUIRE_TLS = 0x2000,
		RobotRaconteurNodeSetupFlags_LOCAL_TRANSPORT_SERVER_PUBLIC = 0x4000,

		RobotRaconteurNodeSetupFlags_NODENAME_OVERRIDE = 0x10000,
		RobotRaconteurNodeSetupFlags_NODEID_OVERRIDE = 0x20000,
		RobotRaconteurNodeSetupFlags_TCP_PORT_OVERRIDE = 0x40000,
		RobotRaconteurNodeSetupFlags_TCP_WEBSOCKET_ORIGIN_OVERRIDE = 0x80000,

		RobotRaconteurNodeSetupFlags_ENABLE_INTRA_TRANSPORT = 0x100000,
		RobotRaconteurNodeSetupFlags_INTRA_TRANSPORT_START_SERVER = 0x200000,

		RobotRaconteurNodeSetupFlags_LOCAL_TAP_ENABLE = 0x1000000,
		RobotRaconteurNodeSetupFlags_LOCAL_TAP_NAME = 0x2000000,

		RobotRaconteurNodeSetupFlags_ENABLE_ALL_TRANSPORTS = 0x10001C,
		/*RobotRaconteurNodeSetupFlags_ENABLE_LOCAL_TRANSPORT 
		| RobotRaconteurNodeSetupFlags_ENABLE_TCP_TRANSPORT 
		| RobotRaconteurNodeSetupFlags_ENABLE_HARDWARE_TRANSPORT,
		| RobotRaconteurNodeSetupFlags_ENABLE_INTRA_TRANSPORT*/

		RobotRaconteurNodeSetupFlags_CLIENT_DEFAULT = 0x10004D,
		/*RobotRaconteurNodeSetupFlags_ENABLE_TCP_TRANSPORT
		| RobotRaconteurNodeSetupFlags_ENABLE_LOCAL_TRANSPORT
		| RobotRaconteurNodeSetupFlags_ENABLE_INTRA_TRANSPORT 
		| RobotRaconteurNodeSetupFlags_ENABLE_NODE_DISCOVERY_LISTENING 
		| RobotRaconteurNodeSetupFlags_LOCAL_TRANSPORT_START_CLIENT,*/

		RobotRaconteurNodeSetupFlags_CLIENT_DEFAULT_ALLOWED_OVERRIDE = 0x3133E5D,
		/*RobotRaconteurNodeSetupFlags_ENABLE_ALL_TRANSPORTS 
		| RobotRaconteurNodeSetupFlags_ENABLE_NODE_DISCOVERY_LISTENING 
		| RobotRaconteurNodeSetupFlags_LOCAL_TRANSPORT_START_CLIENT
		| RobotRaconteurNodeSetupFlags_DISABLE_MESSAGE4
		| RobotRaconteurNodeSetupFlags_DISABLE_STRINGTABLE
		| RobotRaconteurNodeSetupFlags_DISABLE_TIMEOUTS
		| RobotRaconteurNodeSetupFlags_LOAD_TLS_CERT
		| RobotRaconteurNodeSetupFlags_REQUIRE_TLS
		| RobotRaconteurNodeSetupFlags_NODENAME_OVERRIDE,
		| RobotRaconteurNodeSetupFlags_NODEID_OVERRIDE */

		RobotRaconteurNodeSetupFlags_SERVER_DEFAULT = 0x3004AF,
		/*RobotRaconteurNodeSetupFlags_ENABLE_TCP_TRANSPORT
		| RobotRaconteurNodeSetupFlags_ENABLE_LOCAL_TRANSPORT
		| RobotRaconteurNodeSetupFlags_ENABLE_INTRA_TRANSPORT 
		| RobotRaconteurNodeSetupFlags_LOCAL_TRANSPORT_START_SERVER 
		| RobotRaconteurNodeSetupFlags_TCP_TRANSPORT_START_SERVER
		| RobotRaconteurNodeSetupFlags_INTRA_TRANSPORT_START_SERVER
		| RobotRaconteurNodeSetupFlags_ENABLE_NODE_ANNOUNCE 
		| RobotRaconteurNodeSetupFlags_ENABLE_NODE_DISCOVERY_LISTENING,
		| RobotRaconteurNodeSetupFlags_DISABLE_STRINGTABLE */

		RobotRaconteurNodeSetupFlags_SERVER_DEFAULT_ALLOWED_OVERRIDE = 0x33F7FFF,
		/*RobotRaconteurNodeSetupFlags_ENABLE_ALL_TRANSPORTS 
		| RobotRaconteurNodeSetupFlags_LOCAL_TRANSPORT_START_SERVER 
		| RobotRaconteurNodeSetupFlags_TCP_TRANSPORT_START_SERVER
		| RobotRaconteurNodeSetupFlags_INTRA_TRANSPORT_START_SERVER
		| RobotRaconteurNodeSetupFlags_ENABLE_NODE_ANNOUNCE 
		| RobotRaconteurNodeSetupFlags_ENABLE_NODE_DISCOVERY_LISTENING
		| RobotRaconteurNodeSetupFlags_DISABLE_MESSAGE4
		| RobotRaconteurNodeSetupFlags_DISABLE_STRINGTABLE
		| RobotRaconteurNodeSetupFlags_DISABLE_TIMEOUTS
		| RobotRaconteurNodeSetupFlags_LOAD_TLS_CERT
		| RobotRaconteurNodeSetupFlags_REQUIRE_TLS
		| RobotRaconteurNodeSetupFlags_NODENAME_OVERRIDE
		| RobotRaconteurNodeSetupFlags_NODEID_OVERRIDE
		| RobotRaconteurNodeSetupFlags_TCP_PORT_OVERRIDE
		| RobotRaconteurNodeSetupFlags_TCP_PORT_OVERRIDE 
        | RobotRaconteurNodeSetupFlags_TCP_WEBSOCKET_ORIGIN_OVERRIDE 
        | RobotRaconteurNodeSetupFlags_LOCAL_TRANSPORT_SERVER_PUBLIC 
        | RobotRaconteurNodeSetupFlags_TCP_TRANSPORT_START_SERVER_PORT_SHARER */


		RobotRaconteurNodeSetupFlags_SECURE_SERVER_DEFAULT = 0x3034AF,
		/*RobotRaconteurNodeSetupFlags_ENABLE_TCP_TRANSPORT
		| RobotRaconteurNodeSetupFlags_ENABLE_LOCAL_TRANSPORT
		| RobotRaconteurNodeSetupFlags_ENABLE_INTRA_TRANSPORT
		| RobotRaconteurNodeSetupFlags_LOCAL_TRANSPORT_START_SERVER
		| RobotRaconteurNodeSetupFlags_TCP_TRANSPORT_START_SERVER
		| RobotRaconteurNodeSetupFlags_INTRA_TRANSPORT_START_SERVER
		| RobotRaconteurNodeSetupFlags_ENABLE_NODE_ANNOUNCE
		| RobotRaconteurNodeSetupFlags_ENABLE_NODE_DISCOVERY_LISTENING
		| RobotRaconteurNodeSetupFlags_LOAD_TLS_CERT
		| RobotRaconteurNodeSetupFlags_REQUIRE_TLS
		| RobotRaconteurNodeSetupFlags_DISABLE_STRINGTABLE*/

		RobotRaconteurNodeSetupFlags_SECURE_SERVER_DEFAULT_ALLOWED_OVERRIDE = 0x33F4FFF
		/*RobotRaconteurNodeSetupFlags_ENABLE_ALL_TRANSPORTS 
		| RobotRaconteurNodeSetupFlags_LOCAL_TRANSPORT_START_SERVER 
		| RobotRaconteurNodeSetupFlags_TCP_TRANSPORT_START_SERVER
		| RobotRaconteurNodeSetupFlags_INTRA_TRANSPORT_START_SERVER
		| RobotRaconteurNodeSetupFlags_ENABLE_NODE_ANNOUNCE 
		| RobotRaconteurNodeSetupFlags_ENABLE_NODE_DISCOVERY_LISTENING
		| RobotRaconteurNodeSetupFlags_DISABLE_MESSAGE4
		| RobotRaconteurNodeSetupFlags_DISABLE_STRINGTABLE
		| RobotRaconteurNodeSetupFlags_DISABLE_TIMEOUTS
		| RobotRaconteurNodeSetupFlags_NODENAME_OVERRIDE
		| RobotRaconteurNodeSetupFlags_NODEID_OVERRIDE
		| RobotRaconteurNodeSetupFlags_TCP_PORT_OVERRIDE
		| RobotRaconteurNodeSetupFlags_TCP_PORT_OVERRIDE 
        | RobotRaconteurNodeSetupFlags_TCP_WEBSOCKET_ORIGIN_OVERRIDE 
        | RobotRaconteurNodeSetupFlags_LOCAL_TRANSPORT_SERVER_PUBLIC 
        | RobotRaconteurNodeSetupFlags_TCP_TRANSPORT_START_SERVER_PORT_SHARER */

	};

#ifndef SWIG

	class ROBOTRACONTEUR_CORE_API CommandLineConfigParser
	{
		boost::program_options::options_description desc;
		boost::program_options::variables_map vm;
		std::string prefix;
		
		std::string default_node_name;
		uint16_t default_tcp_port;
		uint32_t default_flags;

	public:
		static void FillOptionsDescription(boost::program_options::options_description& desc, uint32_t allowed_overrides, const std::string& prefix="robotraconteur-");
		
		CommandLineConfigParser(uint32_t allowed_overrides, const std::string& prefix="robotraconteur-");

		void SetDefaults(const std::string& node_name, uint16_t tcp_port, uint32_t default_flags);

		void AddStringOption(const std::string& name, const std::string& descr);
		void AddBoolOption(const std::string& name, const std::string& descr);
		void AddIntOption(const std::string& name, const std::string& descr);

		void ParseCommandLine(int argc, char* argv[]);
		void ParseCommandLine(const std::vector<std::string>& args);
		void AcceptParsedResult(const boost::program_options::variables_map& vm);

		std::string GetOptionOrDefaultAsString(const std::string& option);
		std::string GetOptionOrDefaultAsString(const std::string& option, const std::string& default_value);
		bool GetOptionOrDefaultAsBool(const std::string& option);
		bool GetOptionOrDefaultAsBool(const std::string& option, bool default_value);
		int32_t GetOptionOrDefaultAsInt(const std::string& option);
		int32_t GetOptionOrDefaultAsInt(const std::string& option, int32_t default_value);
	};

	class ROBOTRACONTEUR_CORE_API RobotRaconteurNodeSetup : boost::noncopyable
	{
		RR_SHARED_PTR<TcpTransport> tcp_transport;
		RR_SHARED_PTR<LocalTransport> local_transport;
		RR_SHARED_PTR<HardwareTransport> hardware_transport;
		RR_SHARED_PTR<IntraTransport> intra_transport;
		RR_SHARED_PTR<RobotRaconteurNode> node;
		RR_SHARED_PTR<CommandLineConfigParser> config;

		bool release_node;

		void DoSetup(RR_SHARED_PTR<RobotRaconteurNode> node, const std::vector<RR_SHARED_PTR<ServiceFactory> > service_types, 
			RR_SHARED_PTR<CommandLineConfigParser>& config);

	public:
		RobotRaconteurNodeSetup(RR_SHARED_PTR<RobotRaconteurNode> node, const std::vector<RR_SHARED_PTR<ServiceFactory> > service_types, 
			const std::string& node_name, uint16_t tcp_port, uint32_t flags);

		RobotRaconteurNodeSetup(RR_SHARED_PTR<RobotRaconteurNode> node, const std::vector<RR_SHARED_PTR<ServiceFactory> > service_types, 
			const std::string& node_name, uint16_t tcp_port, uint32_t flags, uint32_t allowed_overrides, int argc, char* argv[]);

		RobotRaconteurNodeSetup(RR_SHARED_PTR<RobotRaconteurNode> node, const std::vector<RR_SHARED_PTR<ServiceFactory> > service_types, 
			const std::string& node_name, uint16_t tcp_port, uint32_t flags, uint32_t allowed_overrides, const std::vector<std::string>& args);

		RobotRaconteurNodeSetup(RR_SHARED_PTR<RobotRaconteurNode> node, const std::vector<RR_SHARED_PTR<ServiceFactory> > service_types, 
			RR_SHARED_PTR<CommandLineConfigParser> config);

		RR_SHARED_PTR<LocalTransport> GetLocalTransport();
		RR_SHARED_PTR<TcpTransport> GetTcpTransport();
		RR_SHARED_PTR<HardwareTransport> GetHardwareTransport();
		RR_SHARED_PTR<IntraTransport> GetIntraTransport();

		RR_SHARED_PTR<CommandLineConfigParser> GetCommandLineConfig();

		void ReleaseNode();

		virtual ~RobotRaconteurNodeSetup();
	};

	class ROBOTRACONTEUR_CORE_API ClientNodeSetup : public RobotRaconteurNodeSetup
	{
	public:
		ClientNodeSetup(RR_SHARED_PTR<RobotRaconteurNode> node, const std::vector<RR_SHARED_PTR<ServiceFactory> > service_types, 
			const std::string& node_name = "", uint32_t flags = RobotRaconteurNodeSetupFlags_CLIENT_DEFAULT);

		ClientNodeSetup(const std::vector<RR_SHARED_PTR<ServiceFactory> > service_types, const std::string& node_name = "",
			uint32_t flags = RobotRaconteurNodeSetupFlags_CLIENT_DEFAULT);

		ClientNodeSetup(RR_SHARED_PTR<RobotRaconteurNode> node, const std::vector<RR_SHARED_PTR<ServiceFactory> > service_types, 
			int argc, char* argv[]);

		ClientNodeSetup(const std::vector<RR_SHARED_PTR<ServiceFactory> > service_types, int argc, char* argv[]);

		ClientNodeSetup(RR_SHARED_PTR<RobotRaconteurNode> node, const std::vector<RR_SHARED_PTR<ServiceFactory> > service_types, 
			const std::vector<std::string>& args);

		ClientNodeSetup(const std::vector<RR_SHARED_PTR<ServiceFactory> > service_types, const std::vector<std::string>& args);
	};

	class ROBOTRACONTEUR_CORE_API ServerNodeSetup : public RobotRaconteurNodeSetup
	{
	public:
		ServerNodeSetup(RR_SHARED_PTR<RobotRaconteurNode> node, const std::vector<RR_SHARED_PTR<ServiceFactory> > service_types, 
			const std::string& node_name, uint16_t tcp_port = 0, uint32_t flags = RobotRaconteurNodeSetupFlags_SERVER_DEFAULT);

		ServerNodeSetup(std::vector<RR_SHARED_PTR<ServiceFactory> > service_types, const std::string& node_name, uint16_t tcp_port = 0,
			uint32_t flags = RobotRaconteurNodeSetupFlags_SERVER_DEFAULT);

		ServerNodeSetup(RR_SHARED_PTR<RobotRaconteurNode> node, const std::vector<RR_SHARED_PTR<ServiceFactory> > service_types, 
			const std::string& node_name, uint16_t tcp_port, int argc, char* argv[]);

		ServerNodeSetup(std::vector<RR_SHARED_PTR<ServiceFactory> > service_types, const std::string& node_name, uint16_t tcp_port,
			int argc, char* argv[]);

		ServerNodeSetup(RR_SHARED_PTR<RobotRaconteurNode> node, const std::vector<RR_SHARED_PTR<ServiceFactory> > service_types, 
			const std::string& node_name, uint16_t tcp_port, const std::vector<std::string>& args);

		ServerNodeSetup(std::vector<RR_SHARED_PTR<ServiceFactory> > service_types, const std::string& node_name, uint16_t tcp_port,
			const std::vector<std::string>& args);
	};

	class ROBOTRACONTEUR_CORE_API SecureServerNodeSetup : public RobotRaconteurNodeSetup
	{
	public:
		SecureServerNodeSetup(RR_SHARED_PTR<RobotRaconteurNode> node, const std::vector<RR_SHARED_PTR<ServiceFactory> > service_types,
			const std::string& node_name, uint16_t tcp_port = 0, uint32_t flags = RobotRaconteurNodeSetupFlags_SECURE_SERVER_DEFAULT);

		SecureServerNodeSetup(std::vector<RR_SHARED_PTR<ServiceFactory> > service_types, const std::string& node_name, uint16_t tcp_port = 0,
			uint32_t flags = RobotRaconteurNodeSetupFlags_SECURE_SERVER_DEFAULT);

		SecureServerNodeSetup(RR_SHARED_PTR<RobotRaconteurNode> node, const std::vector<RR_SHARED_PTR<ServiceFactory> > service_types,
			const std::string& node_name, uint16_t tcp_port, int argc, char* argv[]);

		SecureServerNodeSetup(std::vector<RR_SHARED_PTR<ServiceFactory> > service_types, const std::string& node_name, uint16_t tcp_port,
			int argc, char* argv[]);

		SecureServerNodeSetup(RR_SHARED_PTR<RobotRaconteurNode> node, const std::vector<RR_SHARED_PTR<ServiceFactory> > service_types,
			const std::string& node_name, uint16_t tcp_port, const std::vector<std::string>& args);

		SecureServerNodeSetup(std::vector<RR_SHARED_PTR<ServiceFactory> > service_types, const std::string& node_name, uint16_t tcp_port,
			const std::vector<std::string>& args);
	};
#endif
	
}