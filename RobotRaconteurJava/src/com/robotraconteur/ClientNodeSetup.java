package com.robotraconteur;

public class ClientNodeSetup extends RobotRaconteurNodeSetup {
	public ClientNodeSetup(String node_name, int flags)
	{
		super(node_name, 0, flags);
	}
	
	public ClientNodeSetup(String node_name)
	{
		super(node_name, 0, RobotRaconteurNodeSetupFlags.RobotRaconteurNodeSetupFlags_CLIENT_DEFAULT.swigValue());
	}

	public ClientNodeSetup()
	{
		super("", 0, RobotRaconteurNodeSetupFlags.RobotRaconteurNodeSetupFlags_CLIENT_DEFAULT.swigValue());
	}

	public ClientNodeSetup(String[] args)
	{
		super("", 0, RobotRaconteurNodeSetupFlags.RobotRaconteurNodeSetupFlags_CLIENT_DEFAULT.swigValue(),
		RobotRaconteurNodeSetupFlags.RobotRaconteurNodeSetupFlags_CLIENT_DEFAULT_ALLOWED_OVERRIDE.swigValue(),
		args);
	}
}
