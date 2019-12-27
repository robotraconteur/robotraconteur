
#ifndef SWIG
#include "RobotRaconteur/RobotRaconteurNode.h"
#include "RobotRaconteur/TcpTransport.h"
#include "RobotRaconteur/LocalTransport.h"
#include "RobotRaconteur/HardwareTransport.h"

#include <boost/assign/list_of.hpp>

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
		RobotRaconteurNodeSetupFlags_DISABLE_MESSAGE3  = 0x200,
		RobotRaconteurNodeSetupFlags_DISABLE_STRINGTABLE = 0x400,
		RobotRaconteurNodeSetupFlags_DISABLE_TIMEOUTS = 0x800,
		RobotRaconteurNodeSetupFlags_LOAD_TLS_CERT = 0x1000,
		RobotRaconteurNodeSetupFlags_REQUIRE_TLS = 0x2000,

		RobotRaconteurNodeSetupFlags_ENABLE_ALL_TRANSPORTS = 0x1C,
		/*RobotRaconteurNodeSetupFlags_ENABLE_LOCAL_TRANSPORT 
		| RobotRaconteurNodeSetupFlags_ENABLE_TCP_TRANSPORT 
		| RobotRaconteurNodeSetupFlags_ENABLE_HARDWARE_TRANSPORT,*/

		RobotRaconteurNodeSetupFlags_CLIENT_DEFAULT = 0x5D,
		/*RobotRaconteurNodeSetupFlags_ENABLE_ALL_TRANSPORTS 
		| RobotRaconteurNodeSetupFlags_ENABLE_NODE_DISCOVERY_LISTENING 
		| RobotRaconteurNodeSetupFlags_LOCAL_TRANSPORT_START_CLIENT,*/

		RobotRaconteurNodeSetupFlags_SERVER_DEFAULT = 0xBF,
		/*RobotRaconteurNodeSetupFlags_ENABLE_ALL_TRANSPORTS 
		| RobotRaconteurNodeSetupFlags_LOCAL_TRANSPORT_START_SERVER 
		| RobotRaconteurNodeSetupFlags_TCP_TRANSPORT_START_SERVER 
		| RobotRaconteurNodeSetupFlags_ENABLE_NODE_ANNOUNCE 
		| RobotRaconteurNodeSetupFlags_ENABLE_NODE_DISCOVERY_LISTENING,*/

		RobotRaconteurNodeSetupFlags_SERVER_DEFAULT_PORT_SHARER = 0x13F,
		/*RobotRaconteurNodeSetupFlags_ENABLE_ALL_TRANSPORTS 
		| RobotRaconteurNodeSetupFlags_LOCAL_TRANSPORT_START_SERVER 
		| RobotRaconteurNodeSetupFlags_TCP_TRANSPORT_START_SERVER_PORT_SHARER 
		| RobotRaconteurNodeSetupFlags_ENABLE_NODE_ANNOUNCE 
		| RobotRaconteurNodeSetupFlags_ENABLE_NODE_DISCOVERY_LISTENING*/

		RobotRaconteurNodeSetupFlags_SECURE_SERVER_DEFAULT = 0x30BF,
		/*RobotRaconteurNodeSetupFlags_ENABLE_ALL_TRANSPORTS
		| RobotRaconteurNodeSetupFlags_LOCAL_TRANSPORT_START_SERVER
		| RobotRaconteurNodeSetupFlags_TCP_TRANSPORT_START_SERVER
		| RobotRaconteurNodeSetupFlags_ENABLE_NODE_ANNOUNCE
		| RobotRaconteurNodeSetupFlags_ENABLE_NODE_DISCOVERY_LISTENING,
		| RobotRaconteurNodeSetupFlags_ENABLE_LOAD_TLS,
		| RobotRaconteurNodeSetupFlags_REQUIRE_TLS*/

		RobotRaconteurNodeSetupFlags_SECURE_SERVER_DEFAULT_PORT_SHARER = 0x313F,
		/*RobotRaconteurNodeSetupFlags_ENABLE_ALL_TRANSPORTS
		| RobotRaconteurNodeSetupFlags_LOCAL_TRANSPORT_START_SERVER
		| RobotRaconteurNodeSetupFlags_TCP_TRANSPORT_START_SERVER_PORT_SHARER
		| RobotRaconteurNodeSetupFlags_ENABLE_NODE_ANNOUNCE
		| RobotRaconteurNodeSetupFlags_ENABLE_NODE_DISCOVERY_LISTENING
		| RobotRaconteurNodeSetupFlags_ENABLE_LOAD_TLS,
		| RobotRaconteurNodeSetupFlags_REQUIRE_TLS*/
	};

#ifndef SWIG
	class ROBOTRACONTEUR_CORE_API RobotRaconteurNodeSetup : boost::noncopyable
	{
		RR_SHARED_PTR<TcpTransport> tcp_transport;
		RR_SHARED_PTR<LocalTransport> local_transport;
		RR_SHARED_PTR<HardwareTransport> hardware_transport;
		RR_SHARED_PTR<RobotRaconteurNode> node;

	public:
		RobotRaconteurNodeSetup(RR_SHARED_PTR<RobotRaconteurNode> node, const std::vector<RR_SHARED_PTR<ServiceFactory> > service_types, 
			boost::string_ref node_name, uint16_t tcp_port, uint32_t flags);

		RR_SHARED_PTR<LocalTransport> GetLocalTransport();
		RR_SHARED_PTR<TcpTransport> GetTcpTransport();
		RR_SHARED_PTR<HardwareTransport> GetHardwareTransport();

		virtual ~RobotRaconteurNodeSetup();
	};

	class ROBOTRACONTEUR_CORE_API ClientNodeSetup : public RobotRaconteurNodeSetup
	{
	public:
		ClientNodeSetup(RR_SHARED_PTR<RobotRaconteurNode> node, const std::vector<RR_SHARED_PTR<ServiceFactory> > service_types, 
			boost::string_ref node_name = "", uint32_t flags = RobotRaconteurNodeSetupFlags_CLIENT_DEFAULT);

		ClientNodeSetup(const std::vector<RR_SHARED_PTR<ServiceFactory> > service_types, boost::string_ref node_name = "",
			uint32_t flags = RobotRaconteurNodeSetupFlags_CLIENT_DEFAULT);
	};

	class ROBOTRACONTEUR_CORE_API ServerNodeSetup : public RobotRaconteurNodeSetup
	{
	public:
		ServerNodeSetup(RR_SHARED_PTR<RobotRaconteurNode> node, const std::vector<RR_SHARED_PTR<ServiceFactory> > service_types, 
			boost::string_ref node_name, uint16_t tcp_port = 0, uint32_t flags = RobotRaconteurNodeSetupFlags_SERVER_DEFAULT);

		ServerNodeSetup(std::vector<RR_SHARED_PTR<ServiceFactory> > service_types, boost::string_ref node_name, uint16_t tcp_port = 0,
			uint32_t flags = RobotRaconteurNodeSetupFlags_SERVER_DEFAULT);
	};

	class ROBOTRACONTEUR_CORE_API SecureServerNodeSetup : public RobotRaconteurNodeSetup
	{
	public:
		SecureServerNodeSetup(RR_SHARED_PTR<RobotRaconteurNode> node, const std::vector<RR_SHARED_PTR<ServiceFactory> > service_types,
			boost::string_ref node_name, uint16_t tcp_port = 0, uint32_t flags = RobotRaconteurNodeSetupFlags_SECURE_SERVER_DEFAULT);

		SecureServerNodeSetup(std::vector<RR_SHARED_PTR<ServiceFactory> > service_types, boost::string_ref node_name, uint16_t tcp_port = 0,
			uint32_t flags = RobotRaconteurNodeSetupFlags_SECURE_SERVER_DEFAULT);
	};
#endif
	
}