# C++ Node Setup {#cpp_node_setup}

Robot Raconteur nodes must be set up before they can be used to create or accept connections. The following steps must be completed to set up a node:

1. Configure log levels and configure taps
2. Initialize and register service type factories
3. Initialize, configure, and register transports
4. Configure timeouts (optional)

Once steps 1-4 are complete, services can be registered or client connections initiated. When it is time to shut down the program, the node must be shut down before freeing the node.

A helper class RobotRaconteur::RobotRaconteurNodeSetup and several subclasses are available to simplify the process of setting up and shutting down nodes. These classes take flags, arguments, and command line options to configure the node. See RobotRaconteur::RobotRaconteurNodeSetup for more information on the available flags, options, and subclasses. Command line options allow the user to override the default settings when starting the program. See \ref command_line_options for a list of the available options. This helper class and its subclasses set up the node on construction, and shut down the node when destructed (RAII design pattern).

The subclasses RobotRaconteurNode::ClientNodeSetup, RobotRaconteur::ServerNodeSetup, and RobotRaconteur::SecureServerNodeSetup are typically used to set up the node. These classes configure the node with default options for client nodes, server nodes, and secure server nodes that require clients to use TLS. Server nodes are also configured to create client connections as well as accept incoming connections.

## Client Node Setup {#cpp_client_node_setup}

The class RobotRaconteur::ClientNodeSetup is used when the node is only required to create outgoing client connections. By default it configures the `TcpTransport`, `LocalTransport`, and `IntraTransport` transports and transport discovery. By default the `HardwareTransport` is disabled. It can be enabled using the `--robotraconteur-hardware-enable=true` command line option.

An example of a `main()` function using the `ClientNodeSetup`:

    using namespace RobotRaconteur;
    int main(int argc, char* argv)
    {
        try
        {
            ClientNodeSetup node_setup(ROBOTRACONTEUR_SERVICE_TYPES, argc, argv);

            // Node is now ready to create connections

            auto c = RobotRaconteurNode::s()->ConnectService("rr+tcp://localhost:62222?service=my_service");

            // When node_setup is destroyed on scope exit, node is shut down
        }
        catch (std::exception& exp)
        {
            std::cerr << "Error occurred in client: " << string(exp.what()) << std::endl;
            return -1;
        }
    }

## Server Node Setup {#cpp_server_node_setup}

RobotRaconteur::ServerNodeSetup and RobotRaconteur::SecureServerNodeSetup are similar to RobotRaconteur::ClientNodeSetup, but take a default TCP port and a default NodeName as arguments to the constructor.

    using namespace RobotRaconteur;
    using namespace experimental::my_service
    int main(int argc, char* argv)
    {
        try
        {
            // Initialize service object

            auto my_service_obj = boost::make_shared<MyServiceObject>();

            ServerNodeSetup node_setup(ROBOTRACONTEUR_SERVICE_TYPES, "experimental.my_node", 62222, argc, argv);

            // Register the service to make it available to clients
            RobotRaconteurNode::s()->RegisterService("my_service", "experimental.my_service", my_service_obj);


            // Server node may also be used to create client connections

            auto c = RobotRaconteurNode::s()->ConnectService("rr+tcp://192.168.124.125:62222?service=my_service");

            // When node_setup is destroyed on scope exit, node is shut down
        }
        catch (std::exception& exp)
        {
            std::cerr << "Error occurred in service: " << string(exp.what()) << std::endl;
            return -1;
        }
    }

## Advanced Use of Node Setup {#cpp_advanced_node_setup}

The node setup classes shown above use default configurations. More advanced use cases are possible.

### Store Node Setup in Smart Pointer

The node setup classes can be stored in a `boost::shared_ptr`. This may be necessary if Robot Raconteur is used as a plugin in a different program. The node setup class can be created using `boost::make_shared`, and then stored in a class field. When the pointer is reset, or the containing class is destroyed, the node setup will shut down the node. This is an example of "heap allocating" the node setup class, instead of "stack allocating" as shown above.

### Change Flags and Allowed Overrides

The node setup classes use the flags in RobotRaconteur::RobotRaconteurNodeSetupFlags to control how the node is setup, and which options can be overridden from the command line. These flags are passed to the constructor of RobotRaconteur::RobotRaconteurNodeSetup. The client and server node setup subclasses use default flags. These flags can be customized to change how the node is initialized.

### Add Custom Command Line Options

The class RobotRaconteur::CommandLineConfigParser uses a default set up command line options. Custom options can be added to the parser, and read after node setup is complete. To use custom options, construct a RobotRaconteur::CommandLineConfigParser, configure the default options flags, add the extra options, parse the command line, and pass the config parser to the RobotRaconteurNodeSetup constructor overload that accepts the config parser. This is only recommended for advanced use cases.
