#include "RobotRaconteur/NodeSetup.h"

namespace RobotRaconteur
{
	RobotRaconteurNodeSetup::RobotRaconteurNodeSetup(RR_SHARED_PTR<RobotRaconteurNode> node, const std::vector<RR_SHARED_PTR<ServiceFactory> > service_types, 
		boost::string_ref node_name, uint16_t tcp_port, uint32_t flags)
	{
		this->node = node;

		if (flags & RobotRaconteurNodeSetupFlags_ENABLE_LOCAL_TRANSPORT)
		{
			local_transport = RR_MAKE_SHARED<LocalTransport>(node);
			if (flags & RobotRaconteurNodeSetupFlags_LOCAL_TRANSPORT_START_SERVER)
			{
				local_transport->StartServerAsNodeName(node_name);
			}
			else if (flags & RobotRaconteurNodeSetupFlags_LOCAL_TRANSPORT_START_CLIENT && !node_name.empty())
			{
				local_transport->StartClientAsNodeName(node_name);
			}

			if (flags & RobotRaconteurNodeSetupFlags_DISABLE_MESSAGE3)
			{
				local_transport->SetDisableMessage3(true);
			}

			if (flags & RobotRaconteurNodeSetupFlags_DISABLE_STRINGTABLE)
			{
				local_transport->SetDisableStringTable(true);
			}

			node->RegisterTransport(local_transport);
		}

		if (flags & RobotRaconteurNodeSetupFlags_ENABLE_TCP_TRANSPORT)
		{
			tcp_transport = RR_MAKE_SHARED<TcpTransport>(node);
			if (flags & RobotRaconteurNodeSetupFlags_TCP_TRANSPORT_START_SERVER)
			{
				tcp_transport->StartServer(tcp_port);
			}
			else if (flags & RobotRaconteurNodeSetupFlags_TCP_TRANSPORT_START_SERVER_PORT_SHARER)
			{
				tcp_transport->StartServerUsingPortSharer();
			}

			if (flags & RobotRaconteurNodeSetupFlags_DISABLE_MESSAGE3)
			{
				tcp_transport->SetDisableMessage3(true);
			}

			if (flags & RobotRaconteurNodeSetupFlags_DISABLE_STRINGTABLE)
			{
				tcp_transport->SetDisableStringTable(true);
			}

			if (flags & RobotRaconteurNodeSetupFlags_ENABLE_NODE_DISCOVERY_LISTENING)
			{
				tcp_transport->EnableNodeDiscoveryListening();
			}

			if (flags & RobotRaconteurNodeSetupFlags_ENABLE_NODE_ANNOUNCE)
			{
				tcp_transport->EnableNodeAnnounce();
			}

			if (flags & RobotRaconteurNodeSetupFlags_LOAD_TLS_CERT)
			{
				tcp_transport->LoadTlsNodeCertificate();
			}

			if (flags & RobotRaconteurNodeSetupFlags_REQUIRE_TLS)
			{
				tcp_transport->SetRequireTls(true);
			}

			node->RegisterTransport(tcp_transport);
		}

		if (flags & RobotRaconteurNodeSetupFlags_ENABLE_HARDWARE_TRANSPORT)
		{
			hardware_transport = RR_MAKE_SHARED<HardwareTransport>(node);
			
			if (flags & RobotRaconteurNodeSetupFlags_DISABLE_MESSAGE3)
			{
				hardware_transport->SetDisableMessage3(true);
			}

			if (flags & RobotRaconteurNodeSetupFlags_DISABLE_STRINGTABLE)
			{
				hardware_transport->SetDisableStringTable(true);
			}

			
			node->RegisterTransport(hardware_transport);
		}

		BOOST_FOREACH(RR_SHARED_PTR<ServiceFactory> f, service_types)
		{
			node->RegisterServiceType(f);
		}

		if (flags & RobotRaconteurNodeSetupFlags_DISABLE_TIMEOUTS)
		{
			node->SetRequestTimeout(std::numeric_limits<uint32_t>::max());
			node->SetTransportInactivityTimeout(std::numeric_limits<uint32_t>::max());
			node->SetEndpointInactivityTimeout(std::numeric_limits<uint32_t>::max());			
		}

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

	RobotRaconteurNodeSetup::~RobotRaconteurNodeSetup()
	{
		if (node)
		{
			node->Shutdown();
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
}