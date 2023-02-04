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

using System;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.IO;
using System.Reflection;
using System.Linq;
using System.Threading.Tasks;

namespace RobotRaconteur
{

/// <summary>
/// Setup a node using specified options and manage node lifecycle
/// </summary>
/// <remarks>
/// <para>
/// RobotRaconteurNodeSetup and its subclasses ClientNodeSetup, ServerNodeSetup,
/// and SecureServerNodeSetup are designed to help configure nodes and manage
/// node lifecycles. The node setup classes use Dispose() to configure the node
/// on construction, and call RobotRaconteurNode.Shutdown() when the instance
/// is destroyed.
/// </para>
/// <para>
/// The node setup classes execute the following operations to configure the node:
/// </para>
/// <para>
/// 1. Set log level and tap options from flags, command line options, or environmental variables
/// </para>
/// <para>
/// 2. Register specified service factory types
/// </para>
/// <para>
/// 3. Initialize transports using flags specified in flags or from command line options
/// </para>
/// <para>
/// 4. Configure timeouts
/// </para>
/// <para>
/// See command_line_options for more information on available command line options.
/// </para>
/// <para>
/// Logging level is configured using the environmental variable `ROBOTRACONTEUR_LOG_LEVEL`
/// or the command line option `--robotraconteur-log-level`. See logging for more information.
/// </para>
/// <para>
/// See taps for more information on using taps.
/// </para>
/// <para>
/// The node setup classes optionally initialize LocalTransport,
/// TcpTransport, HardwareTransport, and/or IntraTransport.
/// transports for more information.
/// </para>
/// <para>
/// The LocalTransport.StartServerAsNodeName() or
/// LocalTransport.StartClientAsNodeName() are used to load the NodeID.
/// See LocalTransport for more information on this procedure.
/// </para>
/// </remarks>
public class RobotRaconteurNodeSetup : IDisposable
{
    /// <summary>
    /// Get the TcpTransport
    /// </summary>
    /// <remarks>
    /// Will be null if TcpTransport is not specified in flags
    /// </remarks>
    public TcpTransport TcpTransport { get; }
    /// <summary>
    /// Get the LocalTransport
    /// </summary>
    /// <remarks>
    /// Will be null if LocalTransport is not specified in flags
    /// </remarks>
    public LocalTransport LocalTransport { get; }
    /// <summary>
    /// Get the HardwareTransport
    /// </summary>
    /// <remarks>
    /// <para>
    /// Will be null if HardwareTransport is not specified in flags
    /// </para>
    /// <para>
    ///  Note: Hardware transport is not enabled by default
    /// </para>
    /// </remarks>
    public HardwareTransport HardwareTransport { get; }

    /// <summary>
    /// Get the IntraTransport
    /// </summary>
    /// <remarks>
    /// Will be null if IntraTransport is not specified in flags
    /// </remarks>
    public IntraTransport IntraTransport { get; }

    /// <summary>
    /// Get the command line config parser object used to configure node
    /// </summary>
    /// <remarks>None</remarks>
    public CommandLineConfigParser CommandLineConfig { get; }

    protected WrappedRobotRaconteurNodeSetup setup;

    /// <summary>
    /// Construct a new RobotRaconteurNodeSetup with default node, NodeName, TCP port, and flags
    /// </summary>
    /// <remarks>
    /// <para>
    /// Construct node setup and configure the specified node. Use this overload if no command line options
    /// are provided.
    /// </para>
    /// <para>
    /// This constructor will use assembly scanning to register all available service factories.
    /// </para>
    /// </remarks>
    /// <param name="node_name">The NodeName</param>
    /// <param name="tcp_port">The port to listen for incoming TCP clients</param>
    /// <param name="flags">The configuration flags</param>
    public RobotRaconteurNodeSetup(string node_name, ushort tcp_port, RobotRaconteurNodeSetupFlags flags)
    {
        if (node_name == null)
            node_name = "";
        LoadAllServiceTypes(RobotRaconteurNode.s);
        setup = new WrappedRobotRaconteurNodeSetup(RobotRaconteurNode.s, node_name, tcp_port, (uint)flags);
        TcpTransport = setup.GetTcpTransport();
        LocalTransport = setup.GetLocalTransport();
        HardwareTransport = setup.GetHardwareTransport();
        IntraTransport = setup.GetIntraTransport();
        CommandLineConfig = setup.GetCommandLineConfig();
    }

    /// <summary>
    /// Construct a new RobotRaconteurNodeSetup with default node, NodeName, TCP port, flags, and command line options
    /// </summary>
    /// <remarks>
    /// <para>
    /// Construct node setup and configure the specified node. Use this overload if command line options
    /// are provided.
    /// </para>
    /// <para>
    /// This constructor will use assembly scanning to register all available service factories.
    /// </para>
    /// </remarks>
    /// <param name="node_name">The NodeName</param>
    /// <param name="tcp_port">The port to listen for incoming TCP clients</param>
    /// <param name="flags">The configuration flags</param>
    /// <param name="allowed_overrides">The allowed override flags</param>
    /// <param name="args">The command line arguments</param>
    public RobotRaconteurNodeSetup(string node_name, ushort tcp_port, RobotRaconteurNodeSetupFlags flags,
                                   RobotRaconteurNodeSetupFlags allowed_overrides, string[] args)
    {
        if (node_name == null)
            node_name = "";
        LoadAllServiceTypes(RobotRaconteurNode.s);
        setup = new WrappedRobotRaconteurNodeSetup(RobotRaconteurNode.s, node_name, tcp_port, (uint)flags,
                                                   (uint)allowed_overrides, new vectorstring(args));
        TcpTransport = setup.GetTcpTransport();
        LocalTransport = setup.GetLocalTransport();
        HardwareTransport = setup.GetHardwareTransport();
        IntraTransport = setup.GetIntraTransport();
        CommandLineConfig = setup.GetCommandLineConfig();
    }

    /// <summary>
    /// Construct a new RobotRaconteurNodeSetup with default node and command line parser object
    /// </summary>
    /// <remarks>
    /// <para>
    /// This constructor will use assembly scanning to register all available service factories.
    /// </para>
    /// </remarks>
    /// <param name="config">The command line config parser object</param>
    public RobotRaconteurNodeSetup(CommandLineConfigParser config)
    {
        LoadAllServiceTypes(RobotRaconteurNode.s);
        setup = new WrappedRobotRaconteurNodeSetup(RobotRaconteurNode.s, config);
        TcpTransport = setup.GetTcpTransport();
        LocalTransport = setup.GetLocalTransport();
        HardwareTransport = setup.GetHardwareTransport();
        IntraTransport = setup.GetIntraTransport();
        CommandLineConfig = setup.GetCommandLineConfig();
    }

    /// <summary>
    /// Release the node from lifecycle management
    /// </summary>
    /// <remarks>
    /// If called, RobotRaconteurNode.Shutdown() will not be called when the node setup instance is destroyed
    /// </remarks>
    public void ReleaseNode()
    {
        setup?.ReleaseNode();
    }

    public void Dispose()
    {
        WrappedRobotRaconteurNodeSetup s;

        lock (this)
        {
            if (setup == null)
            {
                return;
            }
            s = setup;
            setup = null;
        }

        if (s != null)
        {
            s.Dispose();
        }
        RRNativeObjectHeapSupport.Set_Support(null);
    }

    private void LoadAllServiceTypes(RobotRaconteurNode node)
    {
        // https://stackoverflow.com/questions/13493416/scan-assembly-for-classes-that-implement-certain-interface-and-add-them-to-a-con
        var assignableType = typeof(ServiceFactory);

        var scanners = AppDomain.CurrentDomain.GetAssemblies()
                           .ToList()
                           .SelectMany(x => x.GetTypes())
                           .Where(t => assignableType.IsAssignableFrom(t) && t.IsClass && !t.IsAbstract)
                           .ToList();

        foreach (Type type in scanners)
        {
            var service_factory = Activator.CreateInstance(type) as ServiceFactory;
            if (service_factory != null)
            {
                node.RegisterServiceType(service_factory);
            }
        }
    }
}

/// <summary>
/// Initializes a RobotRaconteurNode instance to default configuration for a client only node
/// </summary>
/// <remarks>
/// <para>
/// ClientNodeSetup is a subclass of RobotRaconteurNodeSetup providing default configuration for a
/// RobotRaconteurNode instance that is used only to create outgoing client connections.
/// </para>
/// <para>
/// See command_line_options for more information on available command line options.
/// </para>
/// <para>
/// Note: String table and HardwareTransport are disabled by default. They can be enabled
/// using command line options.
/// </para>
/// <para>
/// By default, the configuration will do the following:
/// </para>
/// <para>
/// 1. Configure logging level from environmental variable or command line options. Defaults to `INFO` if
/// not specified
/// </para>
/// <para>
/// 2. Configure tap if specified in command line options
/// </para>
/// <para>
/// 3. Register service types passed to service_types
/// </para>
/// <para>
/// 4. Start LocalTransport (default enabled)
///   1. If `RobotRaconteurNodeSetupFlags_LOCAL_TRANSPORT_START_CLIENT` flag is specified, call
///      LocalTransport::StartServerAsNodeName() with the specified node_name
///   2. Start LocalTransport discovery listening if specified in flags or on command line (default enabled)
///   3. Disable Message Format Version 4 (default enabled) and/or String Table (default disabled) if
///      specified on command line
/// </para>
/// <para>
/// 5. Start TcpTransport (default enabled)
///   1. Disable Message Format Version 4 (default enabled) and/or String Table
///      (default disabled) if specified in flags or command line
///   2. Start TcpTranport discovery listening (default enabled)
///   3. Load TLS certificate and set if TLS is specified on command line (default disabled)
///   4. Process WebSocket origin command line options
/// </para>
/// <para>
/// 6. Start HardwareTransport (default disabled)
///   1. Disable Message Format Version 4 (default enabled) and/or String Table
///      (default disabled) if specified in flags or command line
/// </para>
/// <para>
/// 7. Start IntraTransport (default disabled)
///   1. Disable Message Format Version 4 (default enabled) and/or String Table
///      (default disabled) if specified in flags or command line
/// </para>
/// <para>
/// 8. Disable timeouts if specified in flags or command line (default timeouts normal)
/// </para>
/// <para>
/// Most users will not need to be concerned with these details, and can simply
/// use the default configuration
/// </para>
/// </remarks>
public class ClientNodeSetup : RobotRaconteurNodeSetup
{
    /// <summary>
    /// Construct a new ClientNodeSetup and initialize specified RobotRaconteurNode without command line options
    /// </summary>
    /// <remarks>
    /// This constructor will use assembly scanning to register all available service factories.
    /// </remarks>
    /// <param name="node_name">Node name to use if `RobotRaconteurNodeSetupFlags.LOCAL_TRANSPORT_START_CLIENT` flag is
    /// set</param>
    /// <param name="flags">The node configuration flags</param>
    public ClientNodeSetup(string node_name = null,
                           RobotRaconteurNodeSetupFlags flags = RobotRaconteurNodeSetupFlags.CLIENT_DEFAULT)
        : base(node_name, 0, flags)
    {}

    /// <summary>
    /// Construct a new ClientNodeSetup and initialize specified RobotRaconteurNode with command line options
    /// </summary>
    /// <remarks>
    /// <para>
    /// This constructor will use assembly scanning to register all available service factories.
    /// </para>
    /// <para>
    /// See command_line_options for more information on available command line options.
    /// </para>
    /// </remarks>
    /// <param name="args">The command line options</param>
    public ClientNodeSetup(string[] args)
        : base("", 0, RobotRaconteurNodeSetupFlags.CLIENT_DEFAULT,
               RobotRaconteurNodeSetupFlags.CLIENT_DEFAULT_ALLOWED_OVERRIDE, args)
    {}
}

/// <summary>
/// Initializes a RobotRaconteurNode instance to default configuration for a server and client node
/// </summary>
/// <remarks>
/// <para>
/// ServerNodeSetup is a subclass of RobotRaconteurNodeSetup providing default configuration for a
/// RobotRaconteurNode instance that is used as a server to accept incoming client connections
/// and to initiate client connections.
/// </para>
/// <para>
/// ServerNodeSetup requires a NodeName, and a TCP port if LocalTransport and TcpTransport
/// are enabled (default behavior).
/// </para>
/// <para>
/// See command_line_options for more information on available command line options.
/// </para>
/// <para>
/// Note: String table and HardwareTransport are disabled by default. They can be enabled
/// using command line options.
/// </para>
/// <para>
/// By default, the configuration will do the following:
/// </para>
/// <para>
/// 1. Configure logging level from environmental variable or command line options. Defaults to `INFO` if
/// not specified
/// </para>
/// <para>
/// 2. Configure tap if specified in command line options
/// </para>
/// <para>
/// 3. Register service types passed to service_types
/// </para>
/// <para>
/// 4. Start LocalTransport (default enabled)
///   1. Configure the node to use the specified NodeName, and load the NodeID from the filesystem based
///      based on the NodeName. NodeID will be automatically generated if not previously used.
///      1. If "public" option is set, the transport will listen for all local users (default disabled)
///   2. Start the LocalTransport server to listen for incoming connections with the specified NodeName and NodeID
///   3. Start LocalTransport discovery announce and listening (default enabled)
///   4. Disable Message Format Version 4 (default enabled) and/or String Table (default disabled) if
///      specified on command line
/// </para>
/// <para>
/// 5. Start TcpTransport (default enabled)
///   1. Start the TcpTransport server to listen for incoming connections on specified port
///      or using the port sharer (default enabled using specified port)
///   2. Disable Message Format Version 4 (default enabled) and/or String Table
///      (default disabled) if specified in flags or command line
///   3. Start TcpTranport discovery announce and listening (default enabled)
///   4. Load TLS certificate and set if TLS is specified on command line (default disabled)
///   5. Process WebSocket origin command line options
/// </para>
/// <para>
/// 6. Start HardwareTransport (default disabled)
///   1. Disable Message Format Version 4 (default enabled) and/or String Table
///      (default disabled) if specified in flags or command line
/// </para>
/// <para>
/// 7. Start IntraTransport (default enabled)
///   1. Enable IntraTransport server to listen for incoming clients (default enabled)
///   2. Disable Message Format Version 4 (default enabled) and/or String Table
///      (default disabled) if specified in flags or command line
/// </para>
/// <para>
/// 8. Disable timeouts if specified in flags or command line (default timeouts normal)
/// </para>
/// <para>
/// Most users will not need to be concerned with these details, and can simply
/// use the default configuration.
/// </para>
/// </remarks>
public class ServerNodeSetup : RobotRaconteurNodeSetup
{
    /// <summary>
    /// Construct a new ServerNodeSetup and initialize specified RobotRaconteurNode without command line options
    /// </summary>
    /// <remarks>
    /// This constructor will use assembly scanning to register all available service factories.
    /// </remarks>
    /// <param name="node_name">The node name for LocalTransport and RobotRaconteurNode</param>
    /// <param name="tcp_port">The port to listen on for TcpTransport</param>
    /// <param name="flags">The node configuration flags</param>
    public ServerNodeSetup(string node_name, ushort tcp_port,
                           RobotRaconteurNodeSetupFlags flags = RobotRaconteurNodeSetupFlags.SERVER_DEFAULT)
        : base(node_name, tcp_port, flags)
    {}

    /// <summary>
    /// Construct a new ServerNodeSetup and initialize specified RobotRaconteurNode with command line options
    /// </summary>
    /// <remarks>
    /// <para>
    /// This constructor will use assembly scanning to register all available service factories.
    /// </para>
    /// <para>
    /// See command_line_options for more information on available command line options.
    /// </para>
    /// </remarks>
    /// <param name="node_name">The node name for LocalTransport and RobotRaconteurNode</param>
    /// <param name="tcp_port">The port to listen on for TcpTransport</param>
    /// <param name="args">The command line arguments</param>
    public ServerNodeSetup(string node_name, ushort tcp_port, string[] args)
        : base(node_name, tcp_port, RobotRaconteurNodeSetupFlags.SERVER_DEFAULT,
               RobotRaconteurNodeSetupFlags.SERVER_DEFAULT_ALLOWED_OVERRIDE, args)
    {}
}

/// <summary>
/// Initializes a RobotRaconteurNode instance to default configuration for a secure server and client node
/// </summary>
/// <remarks>
/// <para>
/// SecureServerNodeSetup is a subclass of RobotRaconteurNodeSetup providing default configuration for a
/// secure RobotRaconteurNode instance that is used as a server to accept incoming client connections
/// and to initiate client connections. SecureServerNodeSetup is identical to ServerNodeSetup,
/// except that it requires TLS for all network communication.
/// </para>
/// <para>
/// ServerNodeSetup requires a NodeName, and a TCP port if LocalTransport and TcpTransport
/// are enabled (default behavior).
/// </para>
/// <para>
/// See command_line_options for more information on available command line options.
/// </para>
/// <para>
/// Note: String table and HardwareTransport are disabled by default. They can be enabled
/// using command line options.
/// </para>
/// <para>
/// By default, the configuration will do the following:
/// </para>
/// <para>
/// 1. Configure logging level from environmental variable or command line options. Defaults to `INFO` if
/// not specified
/// </para>
/// <para>
/// 2. Configure tap if specified in command line options
/// </para>
/// <para>
/// 3. Register service types passed to service_types
/// </para>
/// <para>
/// 4. Start LocalTransport (default enabled)
///   1. Configure the node to use the specified NodeName, and load the NodeID from the filesystem based
///      based on the NodeName. NodeID will be automatically generated if not previously used.
///      1. If "public" option is set, the transport will listen for all local users (default disabled)
///   2. Start the LocalTransport server to listen for incoming connections with the specified NodeName and NodeID
///   3. Start LocalTransport discovery announce and listening (default enabled)
///   3. Disable Message Format Version 4 (default enabled) and/or String Table (default disabled) if
///      specified on command line
/// </para>
/// <para>
/// 5. Start TcpTransport (default enabled)
///   1. Start the TcpTransport server to listen for incoming connections on specified port
///      or using the port sharer (default enabled using specified port)
///   2. Disable Message Format Version 4 (default enabled) and/or String Table
///      (default disabled) if specified in flags or command line
///   3. Start TcpTranport discovery announce and listening (default enabled)
///   4. Load TLS certificate and set if TLS is specified on command line (default enabled, required)
///   5. Process WebSocket origin command line options
/// </para>
/// <para>
/// 6. Start HardwareTransport (default disabled)
///   1. Disable Message Format Version 4 (default enabled) and/or String Table
///      (default disabled) if specified in flags or command line
/// </para>
/// <para>
/// 7. Start IntraTransport (default disabled)
///   1. Enable IntraTransport server to listen for incoming clients (default enabled)
///   2. Disable Message Format Version 4 (default enabled) and/or String Table
///      (default disabled) if specified in flags or command line
/// </para>
/// <para>
/// 8. Disable timeouts if specified in flags or command line (default timeouts normal)
/// </para>
/// <para>
/// Most users will not need to be concerned with these details, and can simply
/// use the default configuration.
/// </para>
/// </remarks>
public class SecureServerNodeSetup : RobotRaconteurNodeSetup
{
    /// <summary>
    /// Construct a new SecureServerNodeSetup and initialize specified RobotRaconteurNode without command line options
    /// </summary>
    /// <remarks>
    /// This constructor will use assembly scanning to register all available service factories.
    /// </remarks>
    /// <param name="node_name">The node name for LocalTransport and RobotRaconteurNode</param>
    /// <param name="tcp_port">The port to listen on for TcpTransport</param>
    /// <param name="flags">The node configuration flags</param>
    public SecureServerNodeSetup(
        string node_name, ushort tcp_port,
        RobotRaconteurNodeSetupFlags flags = RobotRaconteurNodeSetupFlags.SECURE_SERVER_DEFAULT)
        : base(node_name, tcp_port, flags)
    {}

    /// <summary>
    /// Construct a new SecureServerNodeSetup and initialize specified RobotRaconteurNode with command line options
    /// </summary>
    /// <remarks>
    /// <para>
    /// This constructor will use assembly scanning to register all available service factories.
    /// </para>
    /// <para>
    /// See command_line_options for more information on available command line options.
    /// </para>
    /// </remarks>
    /// <param name="node_name">The node name for LocalTransport and RobotRaconteurNode</param>
    /// <param name="tcp_port">The port to listen on for TcpTransport</param>
    /// <param name="args">The command line arguments</param>
    public SecureServerNodeSetup(string node_name, ushort tcp_port, string[] args)
        : base(node_name, tcp_port, RobotRaconteurNodeSetupFlags.SECURE_SERVER_DEFAULT,
               RobotRaconteurNodeSetupFlags.SECURE_SERVER_DEFAULT_ALLOWED_OVERRIDE, args)
    {}
}

}