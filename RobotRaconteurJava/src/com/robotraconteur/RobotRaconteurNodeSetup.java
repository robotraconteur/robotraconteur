package com.robotraconteur;

import java.util.ArrayList;

public class RobotRaconteurNodeSetup {

	WrappedRobotRaconteurNodeSetup setup;
	TcpTransport tcp_transport;
	LocalTransport local_transport;
	HardwareTransport hardware_transport;
	IntraTransport intra_transport;
	CommandLineConfigParser config;
	
	public RobotRaconteurNodeSetup(String node_name, int tcp_port, int flags)
	{
		setup = new WrappedRobotRaconteurNodeSetup(RobotRaconteurNode.s(), node_name, tcp_port, flags);	
		tcp_transport=setup.getTcpTransport();
		local_transport=setup.getLocalTransport();
		hardware_transport=setup.getHardwareTransport();
		intra_transport=setup.getIntraTransport();	
		config=setup.getCommandLineConfig();	
	}
	
	public RobotRaconteurNodeSetup(String node_name, int tcp_port, int flags, int allowed_overrides, String[] args)
	{
		setup = new WrappedRobotRaconteurNodeSetup(RobotRaconteurNode.s(), node_name, tcp_port, flags, allowed_overrides, new vectorstring(args));	
		tcp_transport=setup.getTcpTransport();
		local_transport=setup.getLocalTransport();
		hardware_transport=setup.getHardwareTransport();
		intra_transport=setup.getIntraTransport();	
		config=setup.getCommandLineConfig();	
	}

	public RobotRaconteurNodeSetup(CommandLineConfigParser config)
	{
		setup = new WrappedRobotRaconteurNodeSetup(RobotRaconteurNode.s(), config);	
		tcp_transport=setup.getTcpTransport();
		local_transport=setup.getLocalTransport();
		hardware_transport=setup.getHardwareTransport();
		intra_transport=setup.getIntraTransport();		
		config=setup.getCommandLineConfig();	
	}

	public TcpTransport getTcpTransport()
	{
		return tcp_transport;
	}
	
	public LocalTransport getLocalTransport()
	{
		return local_transport;
	}
	
	public HardwareTransport getHardwareTransport()
	{
		return hardware_transport;
	}

	public CommandLineConfigParser getCommandLineConfig()
	{
		return config;
	}

	public void releaseNode()
	{
		if (setup==null)
			return;
		setup.releaseNode();
	}
		
	@Override
	public void finalize()
	{
		WrappedRobotRaconteurNodeSetup s=null;
		
		synchronized(this)
		{
			if (setup==null)
			{
				return;
			}
			s=setup;
			setup=null;
		}
		
		if (s!=null)
		{
			s.finalize();
		}
	}
			
}
