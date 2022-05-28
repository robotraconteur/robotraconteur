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

public class RobotRaconteurNodeSetup : IDisposable
{
    public TcpTransport TcpTransport { get; }
    public LocalTransport LocalTransport { get; }
    public HardwareTransport HardwareTransport { get; }

    public IntraTransport IntraTransport { get; }

    public CommandLineConfigParser CommandLineConfig { get; }

    protected WrappedRobotRaconteurNodeSetup setup;

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

public class ClientNodeSetup : RobotRaconteurNodeSetup
{
    public ClientNodeSetup(string node_name = null,
                           RobotRaconteurNodeSetupFlags flags = RobotRaconteurNodeSetupFlags.CLIENT_DEFAULT)
        : base(node_name, 0, flags)
    {}

    public ClientNodeSetup(string[] args)
        : base("", 0, RobotRaconteurNodeSetupFlags.CLIENT_DEFAULT,
               RobotRaconteurNodeSetupFlags.CLIENT_DEFAULT_ALLOWED_OVERRIDE, args)
    {}
}

public class ServerNodeSetup : RobotRaconteurNodeSetup
{
    public ServerNodeSetup(string node_name, ushort tcp_port,
                           RobotRaconteurNodeSetupFlags flags = RobotRaconteurNodeSetupFlags.SERVER_DEFAULT)
        : base(node_name, tcp_port, flags)
    {}

    public ServerNodeSetup(string node_name, ushort tcp_port, string[] args)
        : base(node_name, tcp_port, RobotRaconteurNodeSetupFlags.SERVER_DEFAULT,
               RobotRaconteurNodeSetupFlags.SERVER_DEFAULT_ALLOWED_OVERRIDE, args)
    {}
}

public class SecureServerNodeSetup : RobotRaconteurNodeSetup
{
    public SecureServerNodeSetup(
        string node_name, ushort tcp_port,
        RobotRaconteurNodeSetupFlags flags = RobotRaconteurNodeSetupFlags.SECURE_SERVER_DEFAULT)
        : base(node_name, tcp_port, flags)
    {}

    public SecureServerNodeSetup(string node_name, ushort tcp_port, string[] args)
        : base(node_name, tcp_port, RobotRaconteurNodeSetupFlags.SECURE_SERVER_DEFAULT,
               RobotRaconteurNodeSetupFlags.SECURE_SERVER_DEFAULT_ALLOWED_OVERRIDE, args)
    {}
}

}