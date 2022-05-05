/**
 * @file NodeSetup.h
 *
 * @author John Wason, PhD
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

/**
 * @brief Setup option flags
 *
 * Setup option flags passed to node setup classes to select options to enable
 * and disable. Flags are used to configure the following types of options:
 *
 * 1. Enable and disable transport types
 * 2. Modify transport options including discovery, security requirements, and connection listening
 * 3. Configure TLS behavior
 * 4. Enable local tap for logging
 *
 * Node setup classes also allow options and flags to be "overridden" using
 * command line options. Use the `*_ALLOW_OVERRIDE` options to configure
 * when these overrides are allowed.
 *
 * The ClientNodeSetup, ServerNodeSetup, and SecureServerNodeSetup
 * are convenience classes for the most commonly used options.
 *
 */
enum RobotRaconteurNodeSetupFlags
{
    /** @brief No options enabled */
    RobotRaconteurNodeSetupFlags_NONE = 0x0,
    /** @brief Enable node discovery listening on all transports */
    RobotRaconteurNodeSetupFlags_ENABLE_NODE_DISCOVERY_LISTENING = 0x1,
    /** @brief Enable node announce on all transports */
    RobotRaconteurNodeSetupFlags_ENABLE_NODE_ANNOUNCE = 0x2,
    /** @brief Enable LocalTransport */
    RobotRaconteurNodeSetupFlags_ENABLE_LOCAL_TRANSPORT = 0x4,
    /** @brief Enable TcpTransport */
    RobotRaconteurNodeSetupFlags_ENABLE_TCP_TRANSPORT = 0x8,
    /** @brief Enable HardwareTransport */
    RobotRaconteurNodeSetupFlags_ENABLE_HARDWARE_TRANSPORT = 0x10,
    /** @brief Start the LocalTransport server to listen for incoming clients */
    RobotRaconteurNodeSetupFlags_LOCAL_TRANSPORT_START_SERVER = 0x20,
    /** @brief Start the LocalTransport client with specified node name */
    RobotRaconteurNodeSetupFlags_LOCAL_TRANSPORT_START_CLIENT = 0x40,
    /** @brief Start the TcpTransport server to listen for incoming clients on the specified port */
    RobotRaconteurNodeSetupFlags_TCP_TRANSPORT_START_SERVER = 0x80,
    /** @brief Start the TcpTransport server to incoming for incoming clients using the port sharer */
    RobotRaconteurNodeSetupFlags_TCP_TRANSPORT_START_SERVER_PORT_SHARER = 0x100,
    /** @brief Disable Message Format Version 4 on all transports */
    RobotRaconteurNodeSetupFlags_DISABLE_MESSAGE4 = 0x200,
    /** @brief Disable Message Format Version 4 string table on all transports */
    RobotRaconteurNodeSetupFlags_DISABLE_STRINGTABLE = 0x400,
    /** @brief Disable all timeouts (useful for debugging) */
    RobotRaconteurNodeSetupFlags_DISABLE_TIMEOUTS = 0x800,
    /** @brief Load the TLS certificate for TcpTransport */
    RobotRaconteurNodeSetupFlags_LOAD_TLS_CERT = 0x1000,
    /** @brief Require TLS for all clients on TcpTransport */
    RobotRaconteurNodeSetupFlags_REQUIRE_TLS = 0x2000,
    /** @brief Make LocalTransport server listen for incoming clients from all users */
    RobotRaconteurNodeSetupFlags_LOCAL_TRANSPORT_SERVER_PUBLIC = 0x4000,

    /** @brief Allow NodeName to be configured using command line options */
    RobotRaconteurNodeSetupFlags_NODENAME_OVERRIDE = 0x10000,
    /** @brief Allow NodeID to be configured using command line options */
    RobotRaconteurNodeSetupFlags_NODEID_OVERRIDE = 0x20000,
    /** @brief Allow TCP port to be configured using command line options */
    RobotRaconteurNodeSetupFlags_TCP_PORT_OVERRIDE = 0x40000,
    /** @brief Allow TCP WebSocket origin control to be configured using command line options */
    RobotRaconteurNodeSetupFlags_TCP_WEBSOCKET_ORIGIN_OVERRIDE = 0x80000,

    /** @brief Enable IntraTransport */
    RobotRaconteurNodeSetupFlags_ENABLE_INTRA_TRANSPORT = 0x100000,
    /** @brief Start the IntraTransport server to listen for incoming clients */
    RobotRaconteurNodeSetupFlags_INTRA_TRANSPORT_START_SERVER = 0x200000,

    /** @brief Enable TcpTransport IPv4 discovery */
    RobotRaconteurNodeSetupFlags_TCP_TRANSPORT_IPV4_DISCOVERY = 0x400000,
    /** @brief Enable TcpTransport IPv6 discovery */
    RobotRaconteurNodeSetupFlags_TCP_TRANSPORT_IPV6_DISCOVERY = 0x800000,

    /** @brief Enable the LocalTap debug logging system */
    RobotRaconteurNodeSetupFlags_LOCAL_TAP_ENABLE = 0x1000000,
    /** @brief Allow the user to set the LocalTap name */
    RobotRaconteurNodeSetupFlags_LOCAL_TAP_NAME = 0x2000000,

    /** @brief Enable jumbo messages (up to 100 MB per message) */
    RobotRaconteurNodeSetupFlags_JUMBO_MESSAGE = 0x4000000,

    /** @brief Convenience flag to enable all transports */
    RobotRaconteurNodeSetupFlags_ENABLE_ALL_TRANSPORTS = 0x10001C,
    /*RobotRaconteurNodeSetupFlags_ENABLE_LOCAL_TRANSPORT
    | RobotRaconteurNodeSetupFlags_ENABLE_TCP_TRANSPORT
    | RobotRaconteurNodeSetupFlags_ENABLE_HARDWARE_TRANSPORT,
    | RobotRaconteurNodeSetupFlags_ENABLE_INTRA_TRANSPORT*/

    /** @brief Default configuration for client nodes (See ClientNodeSetup) */
    RobotRaconteurNodeSetupFlags_CLIENT_DEFAULT = 0x90004D,
    /*RobotRaconteurNodeSetupFlags_ENABLE_TCP_TRANSPORT
    | RobotRaconteurNodeSetupFlags_ENABLE_LOCAL_TRANSPORT
    | RobotRaconteurNodeSetupFlags_ENABLE_INTRA_TRANSPORT
    | RobotRaconteurNodeSetupFlags_ENABLE_NODE_DISCOVERY_LISTENING
    | RobotRaconteurNodeSetupFlags_TCP_TRANSPORT_IPV6_DISCOVERY
    | RobotRaconteurNodeSetupFlags_LOCAL_TRANSPORT_START_CLIENT,*/

    RobotRaconteurNodeSetupFlags_CLIENT_DEFAULT_ALLOWED_OVERRIDE = 0x7D33E5D,
    /*RobotRaconteurNodeSetupFlags_ENABLE_ALL_TRANSPORTS
    | RobotRaconteurNodeSetupFlags_ENABLE_NODE_DISCOVERY_LISTENING
    | RobotRaconteurNodeSetupFlags_TCP_TRANSPORT_IPV6_DISCOVERY
    | RobotRaconteurNodeSetupFlags_TCP_TRANSPORT_IPV4_DISCOVERY
    | RobotRaconteurNodeSetupFlags_LOCAL_TRANSPORT_START_CLIENT
    | RobotRaconteurNodeSetupFlags_DISABLE_MESSAGE4
    | RobotRaconteurNodeSetupFlags_DISABLE_STRINGTABLE
    | RobotRaconteurNodeSetupFlags_DISABLE_TIMEOUTS
    | RobotRaconteurNodeSetupFlags_LOAD_TLS_CERT
    | RobotRaconteurNodeSetupFlags_REQUIRE_TLS
    | RobotRaconteurNodeSetupFlags_NODENAME_OVERRIDE,
    | RobotRaconteurNodeSetupFlags_NODEID_OVERRIDE
    | RobotRaconteurNodeSetupFlags_JUMBO_MESSAGE */

    /** @brief Default configuration for server nodes */
    RobotRaconteurNodeSetupFlags_SERVER_DEFAULT = 0xB004AF,
    /*RobotRaconteurNodeSetupFlags_ENABLE_TCP_TRANSPORT
    | RobotRaconteurNodeSetupFlags_ENABLE_LOCAL_TRANSPORT
    | RobotRaconteurNodeSetupFlags_ENABLE_INTRA_TRANSPORT
    | RobotRaconteurNodeSetupFlags_LOCAL_TRANSPORT_START_SERVER
    | RobotRaconteurNodeSetupFlags_TCP_TRANSPORT_START_SERVER
    | RobotRaconteurNodeSetupFlags_INTRA_TRANSPORT_START_SERVER
    | RobotRaconteurNodeSetupFlags_ENABLE_NODE_ANNOUNCE
    | RobotRaconteurNodeSetupFlags_ENABLE_NODE_DISCOVERY_LISTENING,
    | RobotRaconteurNodeSetupFlags_DISABLE_STRINGTABLE
    | RobotRaconteurNodeSetupFlags_TCP_TRANSPORT_IPV6_DISCOVERY */

    /** @brief Default allowed overrides for server nodes */
    RobotRaconteurNodeSetupFlags_SERVER_DEFAULT_ALLOWED_OVERRIDE = 0x7FF7FFF,
    /*RobotRaconteurNodeSetupFlags_ENABLE_ALL_TRANSPORTS
    | RobotRaconteurNodeSetupFlags_TCP_TRANSPORT_IPV6_DISCOVERY
    | RobotRaconteurNodeSetupFlags_TCP_TRANSPORT_IPV4_DISCOVERY
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
    | RobotRaconteurNodeSetupFlags_TCP_TRANSPORT_START_SERVER_PORT_SHARER
    | RobotRaconteurNodeSetupFlags_JUMBO_MESSAGE */

    /** @brief Default configuration for server nodes requiring TLS network transports */
    RobotRaconteurNodeSetupFlags_SECURE_SERVER_DEFAULT = 0xB034AF,
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
    | RobotRaconteurNodeSetupFlags_DISABLE_STRINGTABLE
    | RobotRaconteurNodeSetupFlags_TCP_TRANSPORT_IPV6_DISCOVERY */

    /** @brief Default allowed overrides for server nodes requiring TLS network transports */
    RobotRaconteurNodeSetupFlags_SECURE_SERVER_DEFAULT_ALLOWED_OVERRIDE = 0x73F4FFF
    /*RobotRaconteurNodeSetupFlags_ENABLE_ALL_TRANSPORTS
    | RobotRaconteurNodeSetupFlags_LOCAL_TRANSPORT_START_SERVER
    | RobotRaconteurNodeSetupFlags_TCP_TRANSPORT_START_SERVER
    | RobotRaconteurNodeSetupFlags_INTRA_TRANSPORT_START_SERVER
    | RobotRaconteurNodeSetupFlags_ENABLE_NODE_ANNOUNCE
    | RobotRaconteurNodeSetupFlags_ENABLE_NODE_DISCOVERY_LISTENING
    | RobotRaconteurNodeSetupFlags_TCP_TRANSPORT_IPV6_DISCOVERY
    | RobotRaconteurNodeSetupFlags_TCP_TRANSPORT_IPV4_DISCOVERY
    | RobotRaconteurNodeSetupFlags_DISABLE_MESSAGE4
    | RobotRaconteurNodeSetupFlags_DISABLE_STRINGTABLE
    | RobotRaconteurNodeSetupFlags_DISABLE_TIMEOUTS
    | RobotRaconteurNodeSetupFlags_NODENAME_OVERRIDE
    | RobotRaconteurNodeSetupFlags_NODEID_OVERRIDE
    | RobotRaconteurNodeSetupFlags_TCP_PORT_OVERRIDE
    | RobotRaconteurNodeSetupFlags_TCP_PORT_OVERRIDE
    | RobotRaconteurNodeSetupFlags_TCP_WEBSOCKET_ORIGIN_OVERRIDE
    | RobotRaconteurNodeSetupFlags_LOCAL_TRANSPORT_SERVER_PUBLIC
    | RobotRaconteurNodeSetupFlags_TCP_TRANSPORT_START_SERVER_PORT_SHARER
    | RobotRaconteurNodeSetupFlags_JUMBO_MESSAGE */
};

#ifndef SWIG

/**
 * @brief Command line parser for node setup classes
 *
 * The CommandLineConfigParser is used to parse command line options specified
 * when a program is launched. These options allow for the node configuration to be
 * changed without recompiling the software. See \ref command_line_options for
 * a table of the standard command line options.
 *
 * ClientNodeSetup, ServerNodeSetup, and SecureServerNodeSetup use this class to parse
 * the `argc` and `argv` parameters. The RobotRaconteurNodeSetup constructors will accept
 * either `argc` and `argv`, or will accept an initialize CommandLineConfigParser.
 *
 * The CommandLineConfig() constructor takes the "allowed override" flags, and the option prefix.
 * The "allowed override" specifies which options can be overridden using the command line. The
 * prefix option allows the command line flag prefix to be changed. By default it expects
 * all options to begin with `--robotraconteur-` followed by the name of the option. If there are
 * multiple nodes, it is necessary to change the prefix to be unique for each node. For instance,
 * "robotraconteur1-" for the first node and "robotraconteur2-" for the second node.
 *
 * Users may add additional options to the parser. Use AddStringOption(),
 * AddBoolOption(), or AddIntOption() to add additional options.
 *
 * External instances of boost::program_options may be used. Use
 * FillOptionDescription() to fill the options used by the parser,
 * and AcceptParsedResults() to load the parsed results into
 * the parser.
 *
 */
class ROBOTRACONTEUR_CORE_API CommandLineConfigParser
{
    boost::program_options::options_description desc;
    boost::program_options::variables_map vm;
    std::string prefix;

    std::string default_node_name;
    uint16_t default_tcp_port;
    uint32_t default_flags;

  public:
    /**
     * @brief Fill the specified options_description with standard node command line options
     *
     * @param desc The target option_descriptions structure
     * @param allowed_overrides The allowed override flags
     * @param prefix The prefix to use for the options
     */
    static void FillOptionsDescription(boost::program_options::options_description& desc, uint32_t allowed_overrides,
                                       const std::string& prefix = "robotraconteur-");

    /**
     * @brief Construct a new CommandLineConfigParser
     *
     * Must use boost::shared_ptr<CommandLineConfigParser>()
     *
     * @param allowed_overrides The allowed overrides flags
     * @param prefix The prefix to use for the options
     */
    CommandLineConfigParser(uint32_t allowed_overrides, const std::string& prefix = "robotraconteur-");

    /**
     * @brief Set the default NodeName, TCP port, and flags
     *
     * The command line options will be allowed to override the options
     * specified in allowed_overrides passed to CommandLineConfigParser().
     *
     * @param node_name The default NodeName
     * @param tcp_port The default TCP port
     * @param default_flags The default flags
     */
    void SetDefaults(const std::string& node_name, uint16_t tcp_port, uint32_t default_flags);

    /**
     * @brief Add a new string option
     *
     * @param name The name of the option
     * @param descr Description of the option
     */
    void AddStringOption(const std::string& name, const std::string& descr);

    /**
     * @brief Add a new bool option
     *
     * @param name The name of the option
     * @param descr Description of the option
     */
    void AddBoolOption(const std::string& name, const std::string& descr);

    /**
     * @brief Add a new int32_t option
     *
     * @param name The name of the option
     * @param descr Description of the option
     */
    void AddIntOption(const std::string& name, const std::string& descr);

    /**
     * @brief Parse a specified `argc` and `argv`
     *
     * Pass `argc` and `argv` from main()
     *
     * Results are stored in the instance
     *
     * @param argc `argc` from main()
     * @param argv `argv` from main()
     */
    void ParseCommandLine(int argc, char* argv[]);

    /**
     * @brief Parse a specified string vector containing the options
     *
     * Results are stored in the instance
     *
     * @param args The options as a string vector
     */
    void ParseCommandLine(const std::vector<std::string>& args);

    /**
     * @brief Accept options from a boost::program_options::variables_map
     *
     * Accept boost::program_options parse results
     *
     * @param vm The boost::program_options variable map
     */
    void AcceptParsedResult(const boost::program_options::variables_map& vm);

    /**
     * @brief Get the option value as a string
     *
     * Returns empty string if option not specified on command line
     *
     * @param option The name of the option
     * @return std::string The option value, or an empty string
     */
    std::string GetOptionOrDefaultAsString(const std::string& option);

    /**
     * @brief Get the option value as a string
     *
     * Returns default_value if option not specified on command line
     *
     * @param option The name of the option
     * @param default_value The default option value
     * @return std::string The option value, or default_value if not specified on command line
     */
    std::string GetOptionOrDefaultAsString(const std::string& option, const std::string& default_value);

    /**
     * @brief Get the option value as a bool
     *
     * Returns false if option not specified on command line
     *
     * @param option The name of the option
     * @return bool The option value, or false
     */
    bool GetOptionOrDefaultAsBool(const std::string& option);

    /**
     * @brief Get the option value as a bool
     *
     * Returns default_value if option not specified on command line
     *
     * @param option The name of the option
     * @param default_value The default option value
     * @return bool The option value, or default_value if not specified on command line
     */
    bool GetOptionOrDefaultAsBool(const std::string& option, bool default_value);

    /**
     * @brief Get the option value as an int32_t
     *
     * Returns -1 if option not specified on command line
     *
     * @param option The name of the option
     * @return int32_t The option value, or -1
     */
    int32_t GetOptionOrDefaultAsInt(const std::string& option);

    /**
     * @brief Get the option value as an int32_t
     *
     * Returns default_value if option not specified on command line
     *
     * @param option The name of the option
     * @param default_value The default option value
     * @return int32_t The option value, or default_value if not specified on command line
     */
    int32_t GetOptionOrDefaultAsInt(const std::string& option, int32_t default_value);
};

/**
 * @brief Setup a node using specified options and manage node lifecycle
 *
 * RobotRaconteurNodeSetup and its subclasses ClientNodeSetup, ServerNodeSetup,
 * and SecureServerNodeSetup are designed to help configure nodes and manage
 * node lifecycles. The node setup classes use RAII to configure the node
 * on construction, and call RobotRaconteurNode::Shutdown() when the instance
 * is destroyed. The node setup can be created on the stack of main() for fully
 * automatic lifecycle management, or stored
 * as a class instance variable in a shared_ptr to be destroyed when the node is
 * no longer needed.
 *
 * The node setup classes execute the following operations to configure the node:
 * 1. Set log level and tap options from flags, command line options, or environmental variables
 * 2. Register specified service factory types
 * 3. Initialize transports using flags specified in flags or from command line options
 * 4. Configure timeouts
 *
 * See \ref command_line_options for more information on available command line options.
 *
 * Logging level is configured using the environmental variable `ROBOTRACONTEUR_LOG_LEVEL`
 * or the command line option `--robotraconteur-log-level`. See \ref logging.md for more information.
 *
 * See \ref taps.md for more information on using taps.
 *
 * The node setup classes optionally initialize LocalTransport,
 * TcpTransport, HardwareTransport, and/or IntraTransport.
 * \ref transports.md for more information.
 *
 * The LocalTransport::StartServerAsNodeName() or
 * LocalTransport::StartClientAsNodeName() are used to load the NodeID.
 * See LocalTransport for more information on this procedure.
 *
 */
class ROBOTRACONTEUR_CORE_API RobotRaconteurNodeSetup : boost::noncopyable
{
    RR_SHARED_PTR<TcpTransport> tcp_transport;
    RR_SHARED_PTR<LocalTransport> local_transport;
    RR_SHARED_PTR<HardwareTransport> hardware_transport;
    RR_SHARED_PTR<IntraTransport> intra_transport;
    RR_SHARED_PTR<RobotRaconteurNode> node;
    RR_SHARED_PTR<CommandLineConfigParser> config;

    bool release_node;

    void DoSetup(RR_SHARED_PTR<RobotRaconteurNode> node,
                 const std::vector<RR_SHARED_PTR<ServiceFactory> > service_types,
                 RR_SHARED_PTR<CommandLineConfigParser>& config);

  public:
    /**
     * @brief Construct a new RobotRaconteurNodeSetup with node, NodeName TCP port, and flags
     *
     * Construct node setup and configure the specified node. Use this overload if no command line options
     * are provided.
     *
     * service_types may be `ROBOTRACONTEUR_SERVICE_TYPES` if "robotraconteur_generated.h" is included.
     *
     * tcp_port is ignored if RobotRaconteurNodeSetupFlags_TCP_TRANSPORT_START_SERVER is not set in flags
     *
     * @param node The node to configure and manage lifecycle
     * @param service_types The service types to register.
     * @param node_name The NodeName
     * @param tcp_port The port to listen for incoming TCP clients
     * @param flags The configuration flags
     */
    RobotRaconteurNodeSetup(RR_SHARED_PTR<RobotRaconteurNode> node,
                            const std::vector<RR_SHARED_PTR<ServiceFactory> > service_types,
                            const std::string& node_name, uint16_t tcp_port, uint32_t flags);

    /**
     * @brief Construct a new RobotRaconteurNodeSetup with node, NodeName TCP port, flags, and command line options
     *
     * Construct node setup and configure the specified node. Use this overload if command line options
     * are provided.
     *
     * service_types may be `ROBOTRACONTEUR_SERVICE_TYPES` if "robotraconteur_generated.h" is included.
     *
     * tcp_port is ignored if RobotRaconteurNodeSetupFlags_TCP_TRANSPORT_START_SERVER is not set in flags or
     * using command line options
     *
     * @param node The node to configure and manage lifecycle
     * @param service_types The service types to register.
     * @param node_name The NodeName
     * @param tcp_port The port to listen for incoming TCP clients
     * @param flags The configuration flags
     * @param allowed_overrides The allowed override flags
     * @param argc The number of command line arguments in argv vector
     * @param argv The command line argument vector
     */
    RobotRaconteurNodeSetup(RR_SHARED_PTR<RobotRaconteurNode> node,
                            const std::vector<RR_SHARED_PTR<ServiceFactory> > service_types,
                            const std::string& node_name, uint16_t tcp_port, uint32_t flags, uint32_t allowed_overrides,
                            int argc, char* argv[]);

    /**
     * @brief Construct a new RobotRaconteurNodeSetup with node, NodeName TCP port, flags, and command line options
     *
     * Construct node setup and configure the specified node. Use this overload if command line options
     * are provided.
     *
     * service_types may be `ROBOTRACONTEUR_SERVICE_TYPES` if "robotraconteur_generated.h" is included.
     *
     * tcp_port is ignored if RobotRaconteurNodeSetupFlags_TCP_TRANSPORT_START_SERVER is not set in flags or
     * using command line options
     *
     * @param node The node to configure and manage lifecycle
     * @param service_types The service types to register.
     * @param node_name The NodeName
     * @param tcp_port The port to listen for incoming TCP clients
     * @param flags The configuration flags
     * @param allowed_overrides The allowed override flags
     * @param args The command line arguments as an STL vector
     */
    RobotRaconteurNodeSetup(RR_SHARED_PTR<RobotRaconteurNode> node,
                            const std::vector<RR_SHARED_PTR<ServiceFactory> > service_types,
                            const std::string& node_name, uint16_t tcp_port, uint32_t flags, uint32_t allowed_overrides,
                            const std::vector<std::string>& args);

    /**
     * @brief Construct a new RobotRaconteurNodeSetup with node, NodeName TCP port, and command line parser object
     *
     * Construct node setup and configure the specified node. Use this overload if command line options
     * parser object is used.
     *
     * service_types may be `ROBOTRACONTEUR_SERVICE_TYPES` if "robotraconteur_generated.h" is included.
     *
     * tcp_port is ignored if RobotRaconteurNodeSetupFlags_TCP_TRANSPORT_START_SERVER is not set in flags or
     * using command line options
     *
     * CommandLineConfigParser must be fully executed by calling CommandLineConfigParser::ParseCommandLine()
     * or CommandLineConfigParser::AcceptParsedResult()
     *
     * @param node The node to configure and manage lifecycle
     * @param service_types The service types to register.
     * @param node_name The NodeName
     * @param tcp_port The port to listen for incoming TCP clients
     * @param config The command line config parser object
     */
    RobotRaconteurNodeSetup(RR_SHARED_PTR<RobotRaconteurNode> node,
                            const std::vector<RR_SHARED_PTR<ServiceFactory> > service_types,
                            RR_SHARED_PTR<CommandLineConfigParser> config);

    /**
     * @brief Get the LocalTransport
     *
     * Will be null if LocalTransport is not specified in flags
     *
     * @return RR_SHARED_PTR<LocalTransport> The LocalTransport object
     */
    RR_SHARED_PTR<LocalTransport> GetLocalTransport();

    /**
     * @brief Get the TcpTransport
     *
     * Will be null if TcpTransport is not specified in flags
     *
     * @return RR_SHARED_PTR<TcpTransport> The TcpTransport object
     */
    RR_SHARED_PTR<TcpTransport> GetTcpTransport();

    /**
     * @brief Get the HardwareTransport
     *
     * Will be null if HardwareTransport is not specified in flags
     *
     * Note: Hardware transport is not enabled by default
     *
     * @return RR_SHARED_PTR<HardwareTransport> The HardwareTransport object
     */
    RR_SHARED_PTR<HardwareTransport> GetHardwareTransport();

    /**
     * @brief Get the IntraTransport
     *
     * Will be null if IntraTransport is not specified in flags
     *
     * @return RR_SHARED_PTR<IntraTransport> The IntraTransport object
     */
    RR_SHARED_PTR<IntraTransport> GetIntraTransport();

    /**
     * @brief Get the command line config parser object used to configure node
     *
     * @return RR_SHARED_PTR<CommandLineConfigParser> The command line config parser
     */
    RR_SHARED_PTR<CommandLineConfigParser> GetCommandLineConfig();

    /**
     * @brief Release the node from lifecycle management
     *
     * If called, RobotRaconteurNode::Shutdown() will not
     * be called when the node setup instance is destroyed
     *
     */
    void ReleaseNode();

    virtual ~RobotRaconteurNodeSetup();
};

/**
 * @brief Initializes a RobotRaconteurNode instance to default configuration for a client only node
 *
 * ClientNodeSetup is a subclass of RobotRaconteurNodeSetup providing default configuration for a
 * RobotRaconteurNode instance that is used only to create outgoing client connections.
 *
 * See \ref command_line_options for more information on available command line options.
 *
 * Note: String table and HardwareTransport are disabled by default. They can be enabled
 * using command line options.
 *
 * By default, the configuration will do the following:
 *
 * 1. Configure logging level from environmental variable or command line options. Defaults to `INFO` if
 * not specified
 * 2. Configure tap if specified in command line options
 * 3. Register service types passed to service_types
 * 4. Start LocalTransport (default enabled)
 *   1. If `RobotRaconteurNodeSetupFlags_LOCAL_TRANSPORT_START_CLIENT` flag is specified, call
 *      LocalTransport::StartServerAsNodeName() with the specified node_name
 *   2. Start LocalTransport discovery listening if specified in flags or on command line (default enabled)
 *   3. Disable Message Format Version 4 (default enabled) and/or String Table (default disabled) if
 *      specified on command line
 * 5. Start TcpTransport (default enabled)
 *   1. Disable Message Format Version 4 (default enabled) and/or String Table
 *      (default disabled) if specified in flags or command line
 *   2. Start TcpTranport discovery listening (default enabled)
 *   3. Load TLS certificate and set if TLS is specified on command line (default disabled)
 *   4. Process WebSocket origin command line options
 * 6. Start HardwareTransport (default disabled)
 *   1. Disable Message Format Version 4 (default enabled) and/or String Table
 *      (default disabled) if specified in flags or command line
 * 7. Start IntraTransport (default disabled)
 *   1. Disable Message Format Version 4 (default enabled) and/or String Table
 *      (default disabled) if specified in flags or command line
 * 8. Disable timeouts if specified in flags or command line (default timeouts normal)
 *
 * Most users will not need to be concerned with these details, and can simply
 * use the default configuration.
 *
 */
class ROBOTRACONTEUR_CORE_API ClientNodeSetup : public RobotRaconteurNodeSetup
{
  public:
    /**
     * @brief Construct a new ClientNodeSetup and initialize specified RobotRaconteurNode without command line options
     *
     * service_types may be `ROBOTRACONTEUR_SERVICE_TYPES` if "robotraconteur_generated.h" is included.
     *
     * @param node The node to configure and manage lifecycle
     * @param service_types The service types to register
     * @param node_name Node name to use if `RobotRaconteurNodeSetupFlags_LOCAL_TRANSPORT_START_CLIENT` flag is set
     * @param flags The node configuration flags
     */
    ClientNodeSetup(RR_SHARED_PTR<RobotRaconteurNode> node,
                    const std::vector<RR_SHARED_PTR<ServiceFactory> > service_types, const std::string& node_name = "",
                    uint32_t flags = RobotRaconteurNodeSetupFlags_CLIENT_DEFAULT);

    /**
     * @brief Construct a new ClientNodeSetup and initialize the singleton RobotRaconteurNode without command line
     * options
     *
     * Configures RobotRaconteurNode::s() and manages its lifecycle
     *
     * service_types may be `ROBOTRACONTEUR_SERVICE_TYPES` if "robotraconteur_generated.h" is included.
     *
     * @param service_types The service types to register
     * @param node_name Node name to use if `RobotRaconteurNodeSetupFlags_LOCAL_TRANSPORT_START_CLIENT` flag is set
     * @param flags The node configuration flags
     */
    ClientNodeSetup(const std::vector<RR_SHARED_PTR<ServiceFactory> > service_types, const std::string& node_name = "",
                    uint32_t flags = RobotRaconteurNodeSetupFlags_CLIENT_DEFAULT);

    /**
     * @brief Construct a new ClientNodeSetup and initialize specified RobotRaconteurNode with command line options
     *
     * service_types may be `ROBOTRACONTEUR_SERVICE_TYPES` if "robotraconteur_generated.h" is included.
     *
     * See \ref command_line_options for more information on available command line options.
     *
     * @param node The node to configure and manage lifecycle
     * @param service_types The service types to register
     * @param argc The number of command line arguments in argv vector
     * @param argv The command line argument vector
     */
    ClientNodeSetup(RR_SHARED_PTR<RobotRaconteurNode> node,
                    const std::vector<RR_SHARED_PTR<ServiceFactory> > service_types, int argc, char* argv[]);

    /**
     * @brief Construct a new ClientNodeSetup and initialize the singleton RobotRaconteurNode with command line options
     *
     * Configures RobotRaconteurNode::s() and manages its lifecycle
     *
     * service_types may be `ROBOTRACONTEUR_SERVICE_TYPES` if "robotraconteur_generated.h" is included.
     *
     * See \ref command_line_options for more information on available command line options.
     *
     * @param service_types The service types to register
     * @param argc The number of command line arguments in argv vector
     * @param argv The command line argument vector
     */
    ClientNodeSetup(const std::vector<RR_SHARED_PTR<ServiceFactory> > service_types, int argc, char* argv[]);

    /**
     * @brief Construct a new ClientNodeSetup and initialize specified RobotRaconteurNode with command line options
     *
     * service_types may be `ROBOTRACONTEUR_SERVICE_TYPES` if "robotraconteur_generated.h" is included.
     *
     * See \ref command_line_options for more information on available command line options.
     *
     * @param node The node to configure and manage lifecycle
     * @param service_types The service types to register
     * @param args The command line options as a string vector
     */
    ClientNodeSetup(RR_SHARED_PTR<RobotRaconteurNode> node,
                    const std::vector<RR_SHARED_PTR<ServiceFactory> > service_types,
                    const std::vector<std::string>& args);

    /**
     * @brief Construct a new ClientNodeSetup and initialize the singleton RobotRaconteurNode with command line options
     *
     * Configures RobotRaconteurNode::s() and manages its lifecycle
     *
     * service_types may be `ROBOTRACONTEUR_SERVICE_TYPES` if "robotraconteur_generated.h" is included.
     *
     * See \ref command_line_options for more information on available command line options.
     *
     * @param service_types The service types to register
     * @param args The command line options as a string vector
     */
    ClientNodeSetup(const std::vector<RR_SHARED_PTR<ServiceFactory> > service_types,
                    const std::vector<std::string>& args);
};

/**
 * @brief Initializes a RobotRaconteurNode instance to default configuration for a server and client node
 *
 * ServerNodeSetup is a subclass of RobotRaconteurNodeSetup providing default configuration for a
 * RobotRaconteurNode instance that is used as a server to accept incoming client connections
 * and to initiate client connections.
 *
 * ServerNodeSetup requires a NodeName, and a TCP port if LocalTransport and TcpTransport
 * are enabled (default behavior).
 *
 * See \ref command_line_options for more information on available command line options.
 *
 * Note: String table and HardwareTransport are disabled by default. They can be enabled
 * using command line options.
 *
 * By default, the configuration will do the following:
 *
 * 1. Configure logging level from environmental variable or command line options. Defaults to `INFO` if
 * not specified
 * 2. Configure tap if specified in command line options
 * 3. Register service types passed to service_types
 * 4. Start LocalTransport (default enabled)
 *   1. Configure the node to use the specified NodeName, and load the NodeID from the filesystem based
 *      based on the NodeName. NodeID will be automatically generated if not previously used.
 *      1. If "public" option is set, the transport will listen for all local users (default disabled)
 *   2. Start the LocalTransport server to listen for incoming connections with the specified NodeName and NodeID
 *   3. Start LocalTransport discovery announce and listening (default enabled)
 *   4. Disable Message Format Version 4 (default enabled) and/or String Table (default disabled) if
 *      specified on command line
 * 5. Start TcpTransport (default enabled)
 *   1. Start the TcpTransport server to listen for incoming connections on specified port
 *      or using the port sharer (default enabled using specified port)
 *   2. Disable Message Format Version 4 (default enabled) and/or String Table
 *      (default disabled) if specified in flags or command line
 *   3. Start TcpTranport discovery announce and listening (default enabled)
 *   4. Load TLS certificate and set if TLS is specified on command line (default disabled)
 *   5. Process WebSocket origin command line options
 * 6. Start HardwareTransport (default disabled)
 *   1. Disable Message Format Version 4 (default enabled) and/or String Table
 *      (default disabled) if specified in flags or command line
 * 7. Start IntraTransport (default enabled)
 *   1. Enable IntraTransport server to listen for incoming clients (default enabled)
 *   2. Disable Message Format Version 4 (default enabled) and/or String Table
 *      (default disabled) if specified in flags or command line
 * 8. Disable timeouts if specified in flags or command line (default timeouts normal)
 *
 * Most users will not need to be concerned with these details, and can simply
 * use the default configuration.
 *
 */
class ROBOTRACONTEUR_CORE_API ServerNodeSetup : public RobotRaconteurNodeSetup
{
  public:
    /**
     * @brief Construct a new ServerNodeSetup and initialize specified RobotRaconteurNode without command line options
     *
     * service_types may be `ROBOTRACONTEUR_SERVICE_TYPES` if "robotraconteur_generated.h" is included.
     *
     * @param node The node to configure and manage lifecycle
     * @param service_types The service types to register
     * @param node_name The node name for LocalTransport and RobotRaconteurNode
     * @param tcp_port The port to listen on for TcpTransport
     * @param flags The node configuration flags
     */
    ServerNodeSetup(RR_SHARED_PTR<RobotRaconteurNode> node,
                    const std::vector<RR_SHARED_PTR<ServiceFactory> > service_types, const std::string& node_name,
                    uint16_t tcp_port = 0, uint32_t flags = RobotRaconteurNodeSetupFlags_SERVER_DEFAULT);

    /**
     * @brief Construct a new ServerNodeSetup and initialize the singleton RobotRaconteurNode without command line
     * options
     *
     * Configures RobotRaconteurNode::s() and manages its lifecycle
     *
     * service_types may be `ROBOTRACONTEUR_SERVICE_TYPES` if "robotraconteur_generated.h" is included.
     *
     * @param service_types The service types to register
     * @param node_name The node name for LocalTransport and RobotRaconteurNode
     * @param tcp_port The port to listen on for TcpTransport
     * @param flags The node configuration flags
     */
    ServerNodeSetup(std::vector<RR_SHARED_PTR<ServiceFactory> > service_types, const std::string& node_name,
                    uint16_t tcp_port = 0, uint32_t flags = RobotRaconteurNodeSetupFlags_SERVER_DEFAULT);

    /**
     * @brief Construct a new ServerNodeSetup and initialize specified RobotRaconteurNode with command line options
     *
     * service_types may be `ROBOTRACONTEUR_SERVICE_TYPES` if "robotraconteur_generated.h" is included.
     *
     * See \ref command_line_options for more information on available command line options.
     *
     * @param node The node to configure and manage lifecycle
     * @param service_types The service types to register
     * @param node_name The node name for LocalTransport and RobotRaconteurNode
     * @param tcp_port The port to listen on for TcpTransport
     * @param argc The number of command line arguments in argv vector
     * @param argv The command line argument vector
     */
    ServerNodeSetup(RR_SHARED_PTR<RobotRaconteurNode> node,
                    const std::vector<RR_SHARED_PTR<ServiceFactory> > service_types, const std::string& node_name,
                    uint16_t tcp_port, int argc, char* argv[]);

    /**
     * @brief Construct a new ServerNodeSetup and initialize the singleton RobotRaconteurNode with command line options
     *
     * service_types may be `ROBOTRACONTEUR_SERVICE_TYPES` if "robotraconteur_generated.h" is included.
     *
     * See \ref command_line_options for more information on available command line options.
     *
     * @param service_types The service types to register
     * @param node_name The node name for LocalTransport and RobotRaconteurNode
     * @param tcp_port The port to listen on for TcpTransport
     * @param argc The number of command line arguments in argv vector
     * @param argv The command line argument vector
     */
    ServerNodeSetup(std::vector<RR_SHARED_PTR<ServiceFactory> > service_types, const std::string& node_name,
                    uint16_t tcp_port, int argc, char* argv[]);

    /**
     * @brief Construct a new ServerNodeSetup and initialize specified RobotRaconteurNode with command line options
     *
     * service_types may be `ROBOTRACONTEUR_SERVICE_TYPES` if "robotraconteur_generated.h" is included.
     *
     * See \ref command_line_options for more information on available command line options.
     *
     * @param node The node to configure and manage lifecycle
     * @param service_types The service types to register
     * @param node_name The node name for LocalTransport and RobotRaconteurNode
     * @param tcp_port The port to listen on for TcpTransport
     * @param args The command line options as a string vector
     */
    ServerNodeSetup(RR_SHARED_PTR<RobotRaconteurNode> node,
                    const std::vector<RR_SHARED_PTR<ServiceFactory> > service_types, const std::string& node_name,
                    uint16_t tcp_port, const std::vector<std::string>& args);

    /**
     * @brief Construct a new ServerNodeSetup and initialize the singleton RobotRaconteurNode with command line options
     *
     * service_types may be `ROBOTRACONTEUR_SERVICE_TYPES` if "robotraconteur_generated.h" is included.
     *
     * See \ref command_line_options for more information on available command line options.
     *
     * @param service_types The service types to register
     * @param node_name The node name for LocalTransport and RobotRaconteurNode
     * @param tcp_port The port to listen on for TcpTransport
     * @param args The command line options as a string vector
     */
    ServerNodeSetup(std::vector<RR_SHARED_PTR<ServiceFactory> > service_types, const std::string& node_name,
                    uint16_t tcp_port, const std::vector<std::string>& args);
};

/**
 * @brief Initializes a RobotRaconteurNode instance to default configuration for a secure server and client node
 *
 * SecureServerNodeSetup is a subclass of RobotRaconteurNodeSetup providing default configuration for a
 * secure RobotRaconteurNode instance that is used as a server to accept incoming client connections
 * and to initiate client connections. SecureServerNodeSetup is identical to ServerNodeSetup,
 * except that it requires TLS for all network communication.
 *
 * ServerNodeSetup requires a NodeName, and a TCP port if LocalTransport and TcpTransport
 * are enabled (default behavior).
 *
 * See \ref command_line_options for more information on available command line options.
 *
 * Note: String table and HardwareTransport are disabled by default. They can be enabled
 * using command line options.
 *
 * By default, the configuration will do the following:
 *
 * 1. Configure logging level from environmental variable or command line options. Defaults to `INFO` if
 * not specified
 * 2. Configure tap if specified in command line options
 * 3. Register service types passed to service_types
 * 4. Start LocalTransport (default enabled)
 *   1. Configure the node to use the specified NodeName, and load the NodeID from the filesystem based
 *      based on the NodeName. NodeID will be automatically generated if not previously used.
 *      1. If "public" option is set, the transport will listen for all local users (default disabled)
 *   2. Start the LocalTransport server to listen for incoming connections with the specified NodeName and NodeID
 *   3. Start LocalTransport discovery announce and listening (default enabled)
 *   3. Disable Message Format Version 4 (default enabled) and/or String Table (default disabled) if
 *      specified on command line
 * 5. Start TcpTransport (default enabled)
 *   1. Start the TcpTransport server to listen for incoming connections on specified port
 *      or using the port sharer (default enabled using specified port)
 *   2. Disable Message Format Version 4 (default enabled) and/or String Table
 *      (default disabled) if specified in flags or command line
 *   3. Start TcpTranport discovery announce and listening (default enabled)
 *   4. Load TLS certificate and set if TLS is specified on command line (default enabled, required)
 *   5. Process WebSocket origin command line options
 * 6. Start HardwareTransport (default disabled)
 *   1. Disable Message Format Version 4 (default enabled) and/or String Table
 *      (default disabled) if specified in flags or command line
 * 7. Start IntraTransport (default disabled)
 *   1. Enable IntraTransport server to listen for incoming clients (default enabled)
 *   2. Disable Message Format Version 4 (default enabled) and/or String Table
 *      (default disabled) if specified in flags or command line
 * 8. Disable timeouts if specified in flags or command line (default timeouts normal)
 *
 * Most users will not need to be concerned with these details, and can simply
 * use the default configuration.
 *
 */
class ROBOTRACONTEUR_CORE_API SecureServerNodeSetup : public RobotRaconteurNodeSetup
{
  public:
    /**
     * @brief Construct a new SecureServerNodeSetup and initialize specified RobotRaconteurNode without command line
     * options
     *
     * service_types may be `ROBOTRACONTEUR_SERVICE_TYPES` if "robotraconteur_generated.h" is included.
     *
     * @param node The node to configure and manage lifecycle
     * @param service_types The service types to register
     * @param node_name The node name for LocalTransport and RobotRaconteurNode
     * @param tcp_port The port to listen on for TcpTransport
     * @param flags The node configuration flags
     */
    SecureServerNodeSetup(RR_SHARED_PTR<RobotRaconteurNode> node,
                          const std::vector<RR_SHARED_PTR<ServiceFactory> > service_types, const std::string& node_name,
                          uint16_t tcp_port = 0, uint32_t flags = RobotRaconteurNodeSetupFlags_SECURE_SERVER_DEFAULT);

    /**
     * @brief Construct a new SecureServerNodeSetup and initialize the singleton RobotRaconteurNode without command line
     * options
     *
     * Configures RobotRaconteurNode::s() and manages its lifecycle
     *
     * service_types may be `ROBOTRACONTEUR_SERVICE_TYPES` if "robotraconteur_generated.h" is included.
     *
     * @param service_types The service types to register
     * @param node_name The node name for LocalTransport and RobotRaconteurNode
     * @param tcp_port The port to listen on for TcpTransport
     * @param flags The node configuration flags
     */
    SecureServerNodeSetup(std::vector<RR_SHARED_PTR<ServiceFactory> > service_types, const std::string& node_name,
                          uint16_t tcp_port = 0, uint32_t flags = RobotRaconteurNodeSetupFlags_SECURE_SERVER_DEFAULT);

    /**
     * @brief Construct a new SecureServerNodeSetup and initialize specified RobotRaconteurNode with command line
     * options
     *
     * service_types may be `ROBOTRACONTEUR_SERVICE_TYPES` if "robotraconteur_generated.h" is included.
     *
     * See \ref command_line_options for more information on available command line options.
     *
     * @param node The node to configure and manage lifecycle
     * @param service_types The service types to register
     * @param node_name The node name for LocalTransport and RobotRaconteurNode
     * @param tcp_port The port to listen on for TcpTransport
     * @param argc The number of command line arguments in argv vector
     * @param argv The command line argument vector
     */
    SecureServerNodeSetup(RR_SHARED_PTR<RobotRaconteurNode> node,
                          const std::vector<RR_SHARED_PTR<ServiceFactory> > service_types, const std::string& node_name,
                          uint16_t tcp_port, int argc, char* argv[]);

    /**
     * @brief Construct a new SecureServerNodeSetup and initialize the singleton RobotRaconteurNode with command line
     * options
     *
     * service_types may be `ROBOTRACONTEUR_SERVICE_TYPES` if "robotraconteur_generated.h" is included.
     *
     * See \ref command_line_options for more information on available command line options.
     *
     * @param service_types The service types to register
     * @param node_name The node name for LocalTransport and RobotRaconteurNode
     * @param tcp_port The port to listen on for TcpTransport
     * @param argc The number of command line arguments in argv vector
     * @param argv The command line argument vector
     */
    SecureServerNodeSetup(std::vector<RR_SHARED_PTR<ServiceFactory> > service_types, const std::string& node_name,
                          uint16_t tcp_port, int argc, char* argv[]);

    /**
     * @brief Construct a new SecureServerNodeSetup and initialize specified RobotRaconteurNode with command line
     * options
     *
     * service_types may be `ROBOTRACONTEUR_SERVICE_TYPES` if "robotraconteur_generated.h" is included.
     *
     * See \ref command_line_options for more information on available command line options.
     *
     * @param node The node to configure and manage lifecycle
     * @param service_types The service types to register
     * @param node_name The node name for LocalTransport and RobotRaconteurNode
     * @param tcp_port The port to listen on for TcpTransport
     * @param args The command line options as a string vector
     */
    SecureServerNodeSetup(RR_SHARED_PTR<RobotRaconteurNode> node,
                          const std::vector<RR_SHARED_PTR<ServiceFactory> > service_types, const std::string& node_name,
                          uint16_t tcp_port, const std::vector<std::string>& args);

    /**
     * @brief Construct a new SecureServerNodeSetup and initialize the singleton RobotRaconteurNode with command line
     * options
     *
     * service_types may be `ROBOTRACONTEUR_SERVICE_TYPES` if "robotraconteur_generated.h" is included.
     *
     * See \ref command_line_options for more information on available command line options.
     *
     * @param service_types The service types to register
     * @param node_name The node name for LocalTransport and RobotRaconteurNode
     * @param tcp_port The port to listen on for TcpTransport
     * @param args The command line options as a string vector
     */
    SecureServerNodeSetup(std::vector<RR_SHARED_PTR<ServiceFactory> > service_types, const std::string& node_name,
                          uint16_t tcp_port, const std::vector<std::string>& args);
};
#endif

} // namespace RobotRaconteur