#include "RobotRaconteur/NodeSetup.h"

namespace RobotRaconteur
{

	void RobotRaconteurNodeSetup::DoSetup(RR_SHARED_PTR<RobotRaconteurNode> node, const std::vector<RR_SHARED_PTR<ServiceFactory> > service_types, 
			RR_SHARED_PTR<CommandLineConfigParser>& config)
	{
		this->node = node;

		node->SetLogLevelFromEnvVariable();

		std::string log_level_str = config->GetOptionOrDefaultAsString("log-level");
		if (!log_level_str.empty())
		{
			node->SetLogLevelFromString(log_level_str);
		}

		std::string node_name = config->GetOptionOrDefaultAsString("nodename");
		std::string node_id = config->GetOptionOrDefaultAsString("nodeid","");
		uint16_t tcp_port = boost::numeric_cast<uint16_t>(config->GetOptionOrDefaultAsInt("tcp-port"));


		ROBOTRACONTEUR_LOG_INFO_COMPONENT(node, NodeSetup, -1, "Setting up RobotRaconteurNode version " << node->GetRobotRaconteurVersion() 
			<< " with NodeName: \"" << node_name << "\" TCP port: " << tcp_port);

		BOOST_FOREACH(RR_SHARED_PTR<ServiceFactory> f, service_types)
		{
			node->RegisterServiceType(f);
		}

		bool nodename_set=false;
		bool nodeid_set=false;

		if (config->GetOptionOrDefaultAsBool("local-enable"))
		{
			local_transport = RR_MAKE_SHARED<LocalTransport>(node);
			if (config->GetOptionOrDefaultAsBool("local-start-server"))
			{
				bool public_server = config->GetOptionOrDefaultAsBool("local-server-public");

				if (!node_name.empty())
				{
					local_transport->StartServerAsNodeName(node_name, public_server);
					nodename_set = true;
					nodeid_set = true;
				}
				else if (!node_id.empty())
				{
					local_transport->StartServerAsNodeID(NodeID(node_id), public_server);
					nodename_set = true;
					nodeid_set = true;
				}
				else
				{
					ROBOTRACONTEUR_LOG_ERROR_COMPONENT(node, NodeSetup, -1, "Could not start Local transport server, neither NodeName or NodeID specified");	
				}
			}
			else if (config->GetOptionOrDefaultAsBool("local-start-client") && !node_name.empty())
			{
				local_transport->StartClientAsNodeName(node_name);
				nodename_set = true;
				nodeid_set = true;
			}

			if (config->GetOptionOrDefaultAsBool("discovery-listening-enable"))
			{
				local_transport->EnableNodeDiscoveryListening();
			}

			//if (config->GetOptionOrDefaultAsBool("discovery-announce-enable"))
			//{
				// Always announces due to file watching by clients
			//}

			if (config->GetOptionOrDefaultAsBool("disable-message3"))
			{
				local_transport->SetDisableMessage3(true);
			}

			if (config->GetOptionOrDefaultAsBool("disable-stringtable"))
			{
				local_transport->SetDisableStringTable(true);
			}

			node->RegisterTransport(local_transport);
		}

		if (!node_id.empty())
		{
			if (!nodeid_set)
			{
				node->SetNodeID(NodeID(node_id));
			}
			else
			{
				if (node->NodeID() != NodeID(node_id))
				{
					ROBOTRACONTEUR_LOG_ERROR_COMPONENT(node, NodeSetup, -1, "User requested NodeID " << node_id << " but node was assigned " << node->NodeID().ToString());	
				}
			}
		}

		if (!node_name.empty())
		{
			if (!nodename_set)
			{
				node->SetNodeName(node_name);
			}
			else
			{
				if (node->NodeName() != node_name)
				{
					ROBOTRACONTEUR_LOG_ERROR_COMPONENT(node, NodeSetup, -1, "User requested NodeName " << node_name << " but node was assigned " << node->NodeName());	
				}
			}

		}

		if (config->GetOptionOrDefaultAsBool("tcp-enable"))
		{
			tcp_transport = RR_MAKE_SHARED<TcpTransport>(node);
			if (config->GetOptionOrDefaultAsBool("tcp-start-server-sharer"))
			{
				tcp_transport->StartServerUsingPortSharer();				
			}
			else if (config->GetOptionOrDefaultAsBool("tcp-start-server"))
			{
				tcp_transport->StartServer(tcp_port);
			}

			if (config->GetOptionOrDefaultAsBool("disable-message3"))
			{
				tcp_transport->SetDisableMessage3(true);
			}

			if (config->GetOptionOrDefaultAsBool("disable-stringtable"))
			{
				tcp_transport->SetDisableStringTable(true);
			}

			if (config->GetOptionOrDefaultAsBool("discovery-listening-enable"))
			{
				tcp_transport->EnableNodeDiscoveryListening();
			}

			if (config->GetOptionOrDefaultAsBool("discovery-announce-enable"))
			{
				tcp_transport->EnableNodeAnnounce();
			}

			if (config->GetOptionOrDefaultAsBool("load-tls"))
			{
				tcp_transport->LoadTlsNodeCertificate();
			}

			if (config->GetOptionOrDefaultAsBool("require-tls"))
			{
				tcp_transport->SetRequireTls(true);
			}

			std::string ws_add_origin = config->GetOptionOrDefaultAsString("tcp-ws-add-origins");
			if (!ws_add_origin.empty())
			{
				std::vector<std::string> ws_add_origin_split;
				try
				{
					boost::split(ws_add_origin_split, ws_add_origin, boost::is_any_of(","));
				}
				catch (std::exception&)
				{
					ROBOTRACONTEUR_LOG_ERROR_COMPONENT(node, NodeSetup, -1, "Invalid tcp-ws-add-origin specified: " << ws_add_origin);	
				}

				BOOST_FOREACH(std::string ws_origin, ws_add_origin_split)
				{
					try
					{
						tcp_transport->AddWebSocketAllowedOrigin(ws_origin);
					}
					catch (std::exception& exp)
					{
						ROBOTRACONTEUR_LOG_ERROR_COMPONENT(node, NodeSetup, -1, "Adding tcp-ws-add-origin failed " << ws_origin << ": " << exp.what());	
					}
				}
			}

			std::string ws_remove_origin = config->GetOptionOrDefaultAsString("tcp-ws-remove-origins");
			if (!ws_remove_origin.empty())
			{
				std::vector<std::string> ws_remove_origin_split;
				try
				{
					boost::split(ws_remove_origin_split, ws_remove_origin, boost::is_any_of(","));
				}
				catch (std::exception&)
				{
					ROBOTRACONTEUR_LOG_ERROR_COMPONENT(node, NodeSetup, -1, "Invalid tcp-ws-remove-origin specified: " << ws_remove_origin);	
				}

				BOOST_FOREACH(std::string ws_origin, ws_remove_origin_split)
				{
					try
					{
						tcp_transport->RemoveWebSocketAllowedOrigin(ws_origin);
					}
					catch (std::exception& exp)
					{
						ROBOTRACONTEUR_LOG_ERROR_COMPONENT(node, NodeSetup, -1, "Removing tcp-ws-remove-origin failed " << ws_origin << ": " << exp.what());	
					}
				}
			}

			node->RegisterTransport(tcp_transport);
		}

		if (config->GetOptionOrDefaultAsBool("hardware-enable"))
		{
			hardware_transport = RR_MAKE_SHARED<HardwareTransport>(node);
			
			if (config->GetOptionOrDefaultAsBool("disable-message3"))
			{
				hardware_transport->SetDisableMessage3(true);
			}

			if (config->GetOptionOrDefaultAsBool("disable-stringtable"))
			{
				hardware_transport->SetDisableStringTable(true);
			}

			
			node->RegisterTransport(hardware_transport);
		}
		
		if (config->GetOptionOrDefaultAsBool("disable-timeouts"))
		{
			node->SetRequestTimeout(std::numeric_limits<uint32_t>::max());
			node->SetTransportInactivityTimeout(std::numeric_limits<uint32_t>::max());
			node->SetEndpointInactivityTimeout(std::numeric_limits<uint32_t>::max());			

			ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, NodeSetup, -1, "Timeouts disabled");
		}

		this->config = config;

		ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, NodeSetup, -1, "Node setup complete");
	}


	RobotRaconteurNodeSetup::RobotRaconteurNodeSetup(RR_SHARED_PTR<RobotRaconteurNode> node, const std::vector<RR_SHARED_PTR<ServiceFactory> > service_types, 
		boost::string_ref node_name, uint16_t tcp_port, uint32_t flags)
	{

		RR_SHARED_PTR<CommandLineConfigParser> c = RR_MAKE_SHARED<CommandLineConfigParser>(0);
		c->SetDefaults(node_name, tcp_port, flags);
		DoSetup(node, service_types, c);
	}

	RobotRaconteurNodeSetup::RobotRaconteurNodeSetup(RR_SHARED_PTR<RobotRaconteurNode> node, const std::vector<RR_SHARED_PTR<ServiceFactory> > service_types, 
			boost::string_ref node_name, uint16_t tcp_port, uint32_t flags, uint32_t allowed_overrides, int argc, char* argv[])
	{
		RR_SHARED_PTR<CommandLineConfigParser> c = RR_MAKE_SHARED<CommandLineConfigParser>(allowed_overrides);
		c->SetDefaults(node_name, tcp_port, flags);
		try
		{
			c->ParseCommandLine(argc, argv);
		}
		catch (std::exception& exp)
		{
			ROBOTRACONTEUR_LOG_ERROR_COMPONENT(node, NodeSetup, -1, "Command line parsing error: " << exp.what());
			throw;
		}
		DoSetup(node, service_types, c);
	}

	RobotRaconteurNodeSetup::RobotRaconteurNodeSetup(RR_SHARED_PTR<RobotRaconteurNode> node, const std::vector<RR_SHARED_PTR<ServiceFactory> > service_types, 
		RR_SHARED_PTR<CommandLineConfigParser> config)
	{
		DoSetup(node, service_types, config);
	}


	RR_SHARED_PTR<LocalTransport> RobotRaconteurNodeSetup::GetLocalTransport()
	{
		return local_transport;
	}
	RR_SHARED_PTR<TcpTransport> RobotRaconteurNodeSetup::GetTcpTransport()
	{
		return tcp_transport;
	}
	RR_SHARED_PTR<HardwareTransport> RobotRaconteurNodeSetup::GetHardwareTransport()
	{
		return hardware_transport;
	}

	RR_SHARED_PTR<CommandLineConfigParser> RobotRaconteurNodeSetup::GetCommandLineConfig()
	{
		return config;
	}

	RobotRaconteurNodeSetup::~RobotRaconteurNodeSetup()
	{
		if (node)
		{
			if (detail::ThreadPool_IsNodeMultithreaded(node))
			{
				node->Shutdown();
			}
		}
	}

	ClientNodeSetup::ClientNodeSetup(RR_SHARED_PTR<RobotRaconteurNode> node, const std::vector<RR_SHARED_PTR<ServiceFactory> > service_types, 
		boost::string_ref node_name, uint32_t flags)
		: RobotRaconteurNodeSetup(node, service_types, node_name, 0, flags)
	{

	}

	ClientNodeSetup::ClientNodeSetup(const std::vector<RR_SHARED_PTR<ServiceFactory> > service_types, boost::string_ref node_name, uint32_t flags)
		: RobotRaconteurNodeSetup(RobotRaconteurNode::sp(), service_types, node_name, 0, flags)
	{

	}

	ClientNodeSetup::ClientNodeSetup(RR_SHARED_PTR<RobotRaconteurNode> node, const std::vector<RR_SHARED_PTR<ServiceFactory> > service_types, 
			int argc, char* argv[])
			: RobotRaconteurNodeSetup(node, service_types, "", 0, RobotRaconteurNodeSetupFlags_CLIENT_DEFAULT,
			RobotRaconteurNodeSetupFlags_CLIENT_DEFAULT_ALLOWED_OVERRIDE, argc, argv)
	{
		
	}

	ClientNodeSetup::ClientNodeSetup(const std::vector<RR_SHARED_PTR<ServiceFactory> > service_types, int argc, char* argv[])
		: RobotRaconteurNodeSetup(RobotRaconteurNode::sp(), service_types, "", 0, RobotRaconteurNodeSetupFlags_CLIENT_DEFAULT,
			RobotRaconteurNodeSetupFlags_CLIENT_DEFAULT_ALLOWED_OVERRIDE, argc, argv)
	{

	}

	ServerNodeSetup::ServerNodeSetup(RR_SHARED_PTR<RobotRaconteurNode> node, const std::vector<RR_SHARED_PTR<ServiceFactory> > service_types, boost::string_ref node_name,
		uint16_t tcp_port, uint32_t flags)
		: RobotRaconteurNodeSetup(node, service_types, node_name, tcp_port, flags)
	{

	}

	ServerNodeSetup::ServerNodeSetup(const std::vector<RR_SHARED_PTR<ServiceFactory> > service_types, boost::string_ref node_name, uint16_t tcp_port,
		uint32_t flags)
		: RobotRaconteurNodeSetup(RobotRaconteurNode::sp(), service_types, node_name, tcp_port, flags)
	{

	}

	ServerNodeSetup::ServerNodeSetup(RR_SHARED_PTR<RobotRaconteurNode> node, const std::vector<RR_SHARED_PTR<ServiceFactory> > service_types, 
			boost::string_ref node_name, uint16_t tcp_port, int argc, char* argv[])
		: RobotRaconteurNodeSetup(node, service_types, node_name, tcp_port, RobotRaconteurNodeSetupFlags_SERVER_DEFAULT,
			RobotRaconteurNodeSetupFlags_SERVER_DEFAULT_ALLOWED_OVERRIDE, argc, argv)
	{

	}

	ServerNodeSetup::ServerNodeSetup(std::vector<RR_SHARED_PTR<ServiceFactory> > service_types, boost::string_ref node_name, uint16_t tcp_port,
		int argc, char* argv[])
		: RobotRaconteurNodeSetup(RobotRaconteurNode::sp(), service_types, node_name, tcp_port, RobotRaconteurNodeSetupFlags_SERVER_DEFAULT,
			RobotRaconteurNodeSetupFlags_SERVER_DEFAULT_ALLOWED_OVERRIDE, argc, argv)
	{
		
	}

	SecureServerNodeSetup::SecureServerNodeSetup(RR_SHARED_PTR<RobotRaconteurNode> node, const std::vector<RR_SHARED_PTR<ServiceFactory> > service_types, boost::string_ref node_name,
		uint16_t tcp_port, uint32_t flags)
		: RobotRaconteurNodeSetup(node, service_types, node_name, tcp_port, flags)
	{

	}

	SecureServerNodeSetup::SecureServerNodeSetup(const std::vector<RR_SHARED_PTR<ServiceFactory> > service_types, boost::string_ref node_name, uint16_t tcp_port,
		uint32_t flags)
		: RobotRaconteurNodeSetup(RobotRaconteurNode::sp(), service_types, node_name, tcp_port, flags)
	{

	}

	SecureServerNodeSetup::SecureServerNodeSetup(RR_SHARED_PTR<RobotRaconteurNode> node, const std::vector<RR_SHARED_PTR<ServiceFactory> > service_types, 
			boost::string_ref node_name, uint16_t tcp_port, int argc, char* argv[])
		: RobotRaconteurNodeSetup(node, service_types, node_name, tcp_port, RobotRaconteurNodeSetupFlags_SECURE_SERVER_DEFAULT,
			RobotRaconteurNodeSetupFlags_SECURE_SERVER_DEFAULT_ALLOWED_OVERRIDE, argc, argv)
	{

	}

	SecureServerNodeSetup::SecureServerNodeSetup(std::vector<RR_SHARED_PTR<ServiceFactory> > service_types, boost::string_ref node_name, uint16_t tcp_port,
		int argc, char* argv[])
		: RobotRaconteurNodeSetup(RobotRaconteurNode::sp(), service_types, node_name, tcp_port, RobotRaconteurNodeSetupFlags_SECURE_SERVER_DEFAULT,
			RobotRaconteurNodeSetupFlags_SECURE_SERVER_DEFAULT_ALLOWED_OVERRIDE, argc, argv)
	{
		
	}

	class FillOptionsDescription_add_helper
	{
	public:

		boost::string_ref& prefix;
		uint32_t& allowed_overrides;
		boost::program_options::options_description& desc;
		FillOptionsDescription_add_helper(boost::program_options::options_description& desc_, boost::string_ref prefix_, uint32_t allowed_overrides_)
			: desc(desc_), prefix(prefix_), allowed_overrides(allowed_overrides_) {}

		template<typename T>
		void add(boost::string_ref name, boost::string_ref descr, uint32_t flag)
		{
			if (flag & allowed_overrides)
			{
				std::string name1 = prefix.to_string() + name;
				desc.add_options()((name1).c_str(), boost::program_options::value<T>(), descr.data());
			}
		}

		template<typename T>
		void add(boost::string_ref name, boost::string_ref descr)
		{			
			std::string name1 = prefix.to_string() + name;
			desc.add_options()((name1).c_str(), boost::program_options::value<T>(), descr.data());			
		}
	};

	
	void CommandLineConfigParser::FillOptionsDescription(boost::program_options::options_description& desc, uint32_t allowed_overrides, boost::string_ref prefix)
	{
		FillOptionsDescription_add_helper h(desc, prefix, allowed_overrides);
		h.add<bool>("discovery-listening-enable","enable node discovery listening", RobotRaconteurNodeSetupFlags_ENABLE_NODE_DISCOVERY_LISTENING);
		h.add<bool>("discovery-announce-enable","enable node discovery announce", RobotRaconteurNodeSetupFlags_ENABLE_NODE_ANNOUNCE);
		h.add<bool>("local-enable", "enable Local transport", RobotRaconteurNodeSetupFlags_ENABLE_LOCAL_TRANSPORT);
		h.add<bool>("tcp-enable", "enable TCP transport", RobotRaconteurNodeSetupFlags_ENABLE_TCP_TRANSPORT);
		h.add<bool>("hardware-enable", "enable Hardware transport", RobotRaconteurNodeSetupFlags_ENABLE_HARDWARE_TRANSPORT);
		h.add<bool>("local-start-server", "start Local server listening", RobotRaconteurNodeSetupFlags_LOCAL_TRANSPORT_START_SERVER);
		h.add<bool>("local-start-client", "start Local client with node name", RobotRaconteurNodeSetupFlags_LOCAL_TRANSPORT_START_CLIENT);
		h.add<bool>("local-server-public", "local server is public on system", RobotRaconteurNodeSetupFlags_LOCAL_TRANSPORT_SERVER_PUBLIC);
		h.add<bool>("tcp-start-server", "start TCP server listening", RobotRaconteurNodeSetupFlags_TCP_TRANSPORT_START_SERVER);
		h.add<std::string>("tcp-ws-add-origins", "add websocket origins (comma separated)", RobotRaconteurNodeSetupFlags_TCP_WEBSOCKET_ORIGIN_OVERRIDE);
		h.add<std::string>("tcp-ws-remove-origins", "remove websocket origins (comma separated)", RobotRaconteurNodeSetupFlags_TCP_WEBSOCKET_ORIGIN_OVERRIDE);
		h.add<bool>("tcp-start-server-sharer", "start TCP server listening using port sharer", RobotRaconteurNodeSetupFlags_TCP_TRANSPORT_START_SERVER_PORT_SHARER);
		h.add<bool>("disable-timeouts", "disable timeouts for debugging", RobotRaconteurNodeSetupFlags_DISABLE_TIMEOUTS);
		h.add<bool>("disable-message3", "disable message v3", RobotRaconteurNodeSetupFlags_DISABLE_MESSAGE3);
		h.add<bool>("disable-stringtable", "disable message v3 string table", RobotRaconteurNodeSetupFlags_DISABLE_STRINGTABLE);
		h.add<bool>("load-tls", "load TLS certificate", RobotRaconteurNodeSetupFlags_LOAD_TLS_CERT);
		h.add<bool>("require-tls", "require TLS for network communication", RobotRaconteurNodeSetupFlags_REQUIRE_TLS);

		h.add<std::string>("nodename", "node name to use for node", RobotRaconteurNodeSetupFlags_NODENAME_OVERRIDE);
		h.add<std::string>("nodeid", "node id to use fore node", RobotRaconteurNodeSetupFlags_NODEID_OVERRIDE);
		h.add<int32_t>("tcp-port", "port to listen on for TCP server", RobotRaconteurNodeSetupFlags_TCP_PORT_OVERRIDE);

		h.add<std::string>("log-level", "log level for node");
		

	}

	CommandLineConfigParser::CommandLineConfigParser(uint32_t allowed_overrides, boost::string_ref prefix)
	{
		this->prefix=prefix.to_string();
		FillOptionsDescription(desc, allowed_overrides, prefix);
	}

	void CommandLineConfigParser::SetDefaults(boost::string_ref node_name, uint16_t tcp_port, uint32_t default_flags)
	{
		this->default_node_name = node_name.to_string();
		this->default_tcp_port = tcp_port;
		this->default_flags = default_flags;
	}

	void CommandLineConfigParser::AddStringOption(boost::string_ref name, boost::string_ref descr)
	{
		desc.add_options()((this->prefix + name).c_str(), boost::program_options::value<std::string>(), descr.data());		
	}

	void CommandLineConfigParser::AddBoolOption(boost::string_ref name, boost::string_ref descr)
	{
		desc.add_options()((this->prefix + name).c_str(), boost::program_options::value<bool>(), descr.data());	
	}

	void CommandLineConfigParser::AddIntOption(boost::string_ref name, boost::string_ref descr)
	{
		desc.add_options()((this->prefix + name).c_str(), boost::program_options::value<int32_t>(), descr.data());	
	}

	void CommandLineConfigParser::ParseCommandLine(int argc, char* argv[])
	{
		boost::program_options::store(boost::program_options::command_line_parser(argc, argv).options(desc).allow_unregistered().run(), vm);
		boost::program_options::notify(vm);
	}

	void CommandLineConfigParser::ParseCommandLine(const std::vector<std::string>& args)
	{
		boost::program_options::store(boost::program_options::command_line_parser(args).options(desc).allow_unregistered().run(), vm);
		boost::program_options::notify(vm);
	}

	void CommandLineConfigParser::AcceptParsedResult(const boost::program_options::variables_map& vm)
	{
		this->vm = vm;
	}

	std::string CommandLineConfigParser::GetOptionOrDefaultAsString(const std::string& option)
	{
		std::string option1 = prefix + option;
		if (vm.count(option1) != 0)
		{
			return vm[option1].as<std::string>();			
		}

		if (option == "nodename")
		{
			return this->default_node_name;
		}

		if (option == "log-level")
		{
			return "";
		}

		if (option == "tcp-ws-add-origins")
		{
			return "";
		}

		if (option == "tcp-ws-remove-origins")
		{
			return "";
		}

		throw boost::program_options::required_option(option);
	}

	std::string CommandLineConfigParser::GetOptionOrDefaultAsString(const std::string& option, const std::string& default_value)
	{
		std::string option1 = prefix + option;
		if (vm.count(option1) != 0)
		{
			return vm[option1].as<std::string>();			
		}

		return default_value;
	}

	bool CommandLineConfigParser::GetOptionOrDefaultAsBool(const std::string& option)
	{
		std::string option1 = prefix + option;
		if (vm.count(option1) != 0)
		{
			return vm[option1].as<bool>();			
		}

		if (option == "discovery-listening-enable")
		{
			return (this->default_flags & RobotRaconteurNodeSetupFlags_ENABLE_NODE_DISCOVERY_LISTENING) != 0;
		}

		if (option == "discovery-announce-enable")
		{
			return (this->default_flags & RobotRaconteurNodeSetupFlags_ENABLE_NODE_ANNOUNCE) != 0;
		}

		if (option == "local-enable")
		{
			return (this->default_flags & RobotRaconteurNodeSetupFlags_ENABLE_LOCAL_TRANSPORT) != 0;
		}

		if (option == "tcp-enable")
		{
			return (this->default_flags & RobotRaconteurNodeSetupFlags_ENABLE_TCP_TRANSPORT) != 0;
		}

		if (option == "hardware-enable")
		{
			return (this->default_flags & RobotRaconteurNodeSetupFlags_ENABLE_HARDWARE_TRANSPORT) != 0;
		}

		if (option == "local-start-server")
		{
			return (this->default_flags & RobotRaconteurNodeSetupFlags_LOCAL_TRANSPORT_START_SERVER) != 0;
		}

		if (option == "local-start-client")
		{
			return (this->default_flags & RobotRaconteurNodeSetupFlags_LOCAL_TRANSPORT_START_CLIENT) != 0;
		}		

		if (option == "local-server-public")
		{
			return (this->default_flags & RobotRaconteurNodeSetupFlags_LOCAL_TRANSPORT_SERVER_PUBLIC) != 0;
		}

		if (option == "tcp-start-server")
		{
			return (this->default_flags & RobotRaconteurNodeSetupFlags_TCP_TRANSPORT_START_SERVER) != 0;
		}

		if (option == "tcp-start-server-sharer")
		{
			return (this->default_flags & RobotRaconteurNodeSetupFlags_TCP_TRANSPORT_START_SERVER_PORT_SHARER) != 0;
		}

		if (option == "disable-timeouts")
		{
			return (this->default_flags & RobotRaconteurNodeSetupFlags_DISABLE_TIMEOUTS) != 0;
		}

		if (option == "disable-message3")
		{
			return (this->default_flags & RobotRaconteurNodeSetupFlags_DISABLE_MESSAGE3) != 0;
		}

		if (option == "disable-stringtable")
		{
			return (this->default_flags & RobotRaconteurNodeSetupFlags_DISABLE_STRINGTABLE) != 0;
		}

		if (option == "load-tls")
		{
			return (this->default_flags & RobotRaconteurNodeSetupFlags_LOAD_TLS_CERT) != 0;
		}

		if (option == "require-tls")
		{
			return (this->default_flags & RobotRaconteurNodeSetupFlags_REQUIRE_TLS) != 0;
		}

		throw boost::program_options::required_option(option);
	}

	bool CommandLineConfigParser::GetOptionOrDefaultAsBool(const std::string& option, bool default_value)
	{
		std::string option1 = prefix + option;
		if (vm.count(option1) != 0)
		{
			return vm[option1].as<bool>();			
		}

		return default_value;
	}

	int32_t CommandLineConfigParser::GetOptionOrDefaultAsInt(const std::string& option)
	{
		std::string option1 = prefix + option;
		if (vm.count(option1) != 0)
		{
			return vm[option1].as<int32_t>();			
		}

		if (option == "tcp-port")
		{
			return default_tcp_port;
		}

		throw boost::program_options::required_option(option);
	}

	int32_t CommandLineConfigParser::GetOptionOrDefaultAsInt(const std::string& option, int32_t default_value)
	{
		std::string option1 = prefix + option;
		if (vm.count(option1) != 0)
		{
			return vm[option1].as<int32_t>();			
		}

		return default_value;
	}

}