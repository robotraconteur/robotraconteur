package com.robotraconteur;

public class ServerNodeSetup extends RobotRaconteurNodeSetup
{
    public ServerNodeSetup(String node_name, int tcp_port, int flags)
    {
        super(node_name, tcp_port, flags);
    }

    public ServerNodeSetup(String node_name, int tcp_port)
    {
        super(node_name, tcp_port,
              RobotRaconteurNodeSetupFlags.RobotRaconteurNodeSetupFlags_SERVER_DEFAULT.swigValue());
    }

    public ServerNodeSetup(String node_name)
    {
        super(node_name, 0, RobotRaconteurNodeSetupFlags.RobotRaconteurNodeSetupFlags_SERVER_DEFAULT.swigValue());
    }

    public ServerNodeSetup(String node_name, int tcp_port, String[] args)
    {
        super(node_name, tcp_port, RobotRaconteurNodeSetupFlags.RobotRaconteurNodeSetupFlags_SERVER_DEFAULT.swigValue(),
              RobotRaconteurNodeSetupFlags.RobotRaconteurNodeSetupFlags_SERVER_DEFAULT_ALLOWED_OVERRIDE.swigValue(),
              args);
    }
}