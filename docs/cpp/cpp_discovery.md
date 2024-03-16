# C++ Discovery {#cpp_discovery}

Discovery is used by clients to find available services on the local machine or local network. See \ref discovery for an introduction to discovery.

Nodes and transports must be configured to enable discovery. The RobotRaconteur::ClientNodeSetup, RobotRaconteur::ServerNodeSetup, and RobotRaconteur::SecureNodeSetup enable node discovery be default. See \ref cpp_node_setup for more information.

Subscriptions are often a better option than the basic discovery functions since the track services and automatically create connections. See \ref cpp_subscriptions.

The C++ library provides "service discovery" and "node discovery".

## Service Discovery {#cpp_service_discovery}

Service discovery is used to find available services on the network. The function RobotRaconteur::RobotRaconteurNode::FindServiceByType() and its asynchronous version RobotRaconteur::RobotRaconteurNode::AsyncFindServiceByType() are provided by the C++ library for service discovery. They take the fully qualified type of the service type to search for, and the schemes of the transports to search. The functions return a vector of RobotRaconteur::ServiceInfo2. The information in `RobotRaconteur::ServiceInfo2` can be used to create connections to the service.

An example using `FindServiceByType()`:

    using namespace RobotRaconteur;
    void main(int argc, char* argv[])
    {
        ClientNodeSetup node_setup(ROBOTRACONTEUR_SERVICE_TYPES, argc, argv);

        // Give the node a few seconds to warm up before searching
        boost::this_thread::sleep(boost::posix_time::milliseconds(5000));

        // Parameters for FindServiceByType
        std::string obj_type = "experimental.example_service1.MyObject";
        std::vector<std::string> schemes = {"rr+tcp", "rr+local"};

        std::vector<ServiceInfo2> detected_services = RobotRaconteurNode::s()->FindServiceByType(obj_type, schemes);

        if (detected_services.empty())
        {
            std::cerr << "Did not detect any services!" << std::endl;
            return 1;
        }

        // Print out the discovered services
        for(auto e : detected_services)
        {
            std::cout << "Name: " << e.Name << std::endl;
            std::cout << "RootObjectType: " << e.RootObjectType << std::endl;
            std::cout << "RootObjectImplements: " << boost::join(e.RootObjectImplements, ", ") << std::endl;
            std::cout << "ConnectionURL: " << boost::join(e.ConnectionURL,", ") << std::endl << std::endl;
        }

        // Connect to the first detected service
        MyObjectPtr c = rr_cast<MyObject>(
            RobotRaconteurNode::s()->ConnectService(detected_services.front().ConnectionURL);
        );

        // Client c is now connected

        return 0;

    }

## Node Discovery {#cpp_node_discovery}

Nodes can be discovered directly, without considering which services are available on the nodes. The functions RobotRaconteur::RobotRaconteurNode::FindNodeByID() and RobotRaconteur::RobotRaconteurNode::FindNodeByName() are provided by the C++ library for detecting nodes by NodeID and NodeName, respectively. See \ref nodeid for more information on NodeID and NodeName. The asynchronous versions RobotRaconteur::RobotRaconteurNode::AsyncFindNodeByID() and RobotRaconteur::RobotRaconteurNode::AsyncFindNodeByName() are also provided.

The node discovery functions return a vector of RobotRaconteur::NodeInfo2.

**The results of node discovery are not verified, and are raw discovery information received on the network.**

An example using discovery by NodeName:

    using namespace RobotRaconteur;
    void main(int argc, char* argv[])
    {
        ClientNodeSetup node_setup(ROBOTRACONTEUR_SERVICE_TYPES, argc, argv);

        // Give the node a few seconds to warm up before searching
        boost::this_thread::sleep(boost::posix_time::milliseconds(5000));

        // Parameters for FindNodeByName
        std::string node_name = "my_node_name";
        std::vector<std::string> schemes = {"rr+tcp", "rr+local"};

        std::vector<NodeInfo2> detected_nodes = RobotRaconteurNode::s()->FindNodeByName(node_name, schemes);

        if (detected_nodes.empty())
        {
            std::cerr << "Did not detect any services!" << std::endl;
            return 1;
        }

        // Print out the discovered nodes
        for(auto e : detected_nodes)
        {
            std::cout << "NodeID: " << e.NodeID.ToString() << std::endl;
            std::cout << "NodeName: " << e.NodeName << std::endl;
            std::cout << "ConnectionURL: " << boost::join(e.ConnectionURL,", ") << std::endl << std::endl;
        }

        return 0;

    }

The results of node discovery cannot be used directly for connecting to services, since the resulting URLs do not contain the service information.
