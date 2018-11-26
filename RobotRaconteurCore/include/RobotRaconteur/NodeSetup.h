#include "RobotRaconteur/RobotRaconteurNode.h"
#include "RobotRaconteur/TcpTransport.h"
#include "RobotRaconteur/LocalTransport.h"
#include "RobotRaconteur/HardwareTransport.h"

#include <boost/assign/list_of.hpp>

#pragma once

namespace RobotRaconteur
{

#define RobotRaconteurNodeSetupFlags_NONE 0x0
#define RobotRaconteurNodeSetupFlags_ENABLE_NODE_DISCOVERY_LISTENING 0x1
#define RobotRaconteurNodeSetupFlags_ENABLE_NODE_ANNOUNCE 0x2
#define RobotRaconteurNodeSetupFlags_ENABLE_LOCAL_TRANSPORT 0x4
#define RobotRaconteurNodeSetupFlags_ENABLE_TCP_TRANSPORT 0x8
#define RobotRaconteurNodeSetupFlags_ENABLE_HARDWARE_TRANSPORT 0x10
#define RobotRaconteurNodeSetupFlags_LOCAL_TRANSPORT_START_SERVER 0x20
#define RobotRaconteurNodeSetupFlags_LOCAL_TRANSPORT_START_CLIENT 0x40
#define RobotRaconteurNodeSetupFlags_TCP_TRANSPORT_START_SERVER 0x80
#define RobotRaconteurNodeSetupFlags_TCP_TRANSPORT_START_SERVER_PORT_SHARER 0x100
#define RobotRaconteurNodeSetupFlags_DISABLE_MESSAGE3  0x200
#define RobotRaconteurNodeSetupFlags_DISABLE_STRINGTABLE  0x400
#define RobotRaconteurNodeSetupFlags_DISABLE_TIMEOUTS 0x800


#define RobotRaconteurNodeSetupFlags_ENABLE_ALL_TRANSPORTS   RobotRaconteurNodeSetupFlags_ENABLE_LOCAL_TRANSPORT \
                                                              | RobotRaconteurNodeSetupFlags_ENABLE_TCP_TRANSPORT \
                                                              | RobotRaconteurNodeSetupFlags_ENABLE_HARDWARE_TRANSPORT
#define RobotRaconteurNodeSetupFlags_CLIENT_DEFAULT  RobotRaconteurNodeSetupFlags_ENABLE_ALL_TRANSPORTS \
                                                      | RobotRaconteurNodeSetupFlags_ENABLE_NODE_DISCOVERY_LISTENING \
                                                      | RobotRaconteurNodeSetupFlags_LOCAL_TRANSPORT_START_CLIENT

#define RobotRaconteurNodeSetupFlags_SERVER_DEFAULT  RobotRaconteurNodeSetupFlags_ENABLE_ALL_TRANSPORTS \
                                                      | RobotRaconteurNodeSetupFlags_LOCAL_TRANSPORT_START_SERVER \
                                                      | RobotRaconteurNodeSetupFlags_TCP_TRANSPORT_START_SERVER \
                                                      | RobotRaconteurNodeSetupFlags_ENABLE_NODE_ANNOUNCE \
	                                                  | RobotRaconteurNodeSetupFlags_ENABLE_NODE_DISCOVERY_LISTENING

#define RobotRaconteurNodeSetupFlags_SERVER_DEFAULT_PORT_SHARER  RobotRaconteurNodeSetupFlags_ENABLE_ALL_TRANSPORTS \
                                                      | RobotRaconteurNodeSetupFlags_LOCAL_TRANSPORT_START_SERVER \
                                                      | RobotRaconteurNodeSetupFlags_TCP_TRANSPORT_START_SERVER_PORT_SHARER \
                                                      | RobotRaconteurNodeSetupFlags_ENABLE_NODE_ANNOUNCE \
	                                                  | RobotRaconteurNodeSetupFlags_ENABLE_NODE_DISCOVERY_LISTENING

	class ROBOTRACONTEUR_CORE_API RobotRaconteurNodeSetup : boost::noncopyable
	{
		RR_SHARED_PTR<TcpTransport> tcp_transport;
		RR_SHARED_PTR<LocalTransport> local_transport;
		RR_SHARED_PTR<HardwareTransport> hardware_transport;
		RR_SHARED_PTR<RobotRaconteurNode> node;

	public:
		RobotRaconteurNodeSetup(RR_SHARED_PTR<RobotRaconteurNode> node, const std::vector<RR_SHARED_PTR<ServiceFactory> > service_types, 
			const std::string& node_name, uint16_t tcp_port, uint32_t flags);

		RR_SHARED_PTR<LocalTransport> GetLocalTransport();
		RR_SHARED_PTR<TcpTransport> GetTcpTransport();
		RR_SHARED_PTR<HardwareTransport> GetHardwareTransport();

		virtual ~RobotRaconteurNodeSetup();
	};

	class ROBOTRACONTEUR_CORE_API ClientNodeSetup : public RobotRaconteurNodeSetup
	{
	public:
		ClientNodeSetup(RR_SHARED_PTR<RobotRaconteurNode> node, const std::vector<RR_SHARED_PTR<ServiceFactory> > service_types, 
			const std::string& node_name = "", uint32_t flags = RobotRaconteurNodeSetupFlags_CLIENT_DEFAULT);

		ClientNodeSetup(const std::vector<RR_SHARED_PTR<ServiceFactory> > service_types, const std::string& node_name = "",
			uint32_t flags = RobotRaconteurNodeSetupFlags_CLIENT_DEFAULT);
	};

	class ROBOTRACONTEUR_CORE_API ServerNodeSetup : public RobotRaconteurNodeSetup
	{
	public:
		ServerNodeSetup(RR_SHARED_PTR<RobotRaconteurNode> node, const std::vector<RR_SHARED_PTR<ServiceFactory> > service_types, 
			const std::string& node_name, uint16_t tcp_port = 0, uint32_t flags = RobotRaconteurNodeSetupFlags_SERVER_DEFAULT);

		ServerNodeSetup(std::vector<RR_SHARED_PTR<ServiceFactory> > service_types, const std::string& node_name, uint16_t tcp_port = 0,
			uint32_t flags = RobotRaconteurNodeSetupFlags_SERVER_DEFAULT);
	};
	
}