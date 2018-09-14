package com.robotraconteur;

public class ServiceInfo2
{
	public String Name;
	public String RootObjectType;
	public String[] RootObjectImplements;
	public String[] ConnectionURL;
	public java.util.HashMap<String, Object> Attributes;
	public NodeID NodeID;
	public String NodeName;

	public ServiceInfo2()
	{
	}
	
	public ServiceInfo2(ServiceInfo2Wrapped i2)
	{
		Name = i2.getName();
		NodeID = new NodeID(i2.getNodeID().toString());
		NodeName = i2.getNodeName();
		RootObjectType = i2.getRootObjectType();
		java.util.ArrayList<String> impl = new java.util.ArrayList<String>();
		for (int j2=0; j2<i2.getRootObjectImplements().size(); j2++ )
		{
			String impl2=i2.getRootObjectImplements().get(j2);
			impl.add(impl2);
		}
		RootObjectImplements = impl.toArray(new String[0]);
		Attributes = (java.util.HashMap<String, Object>)RobotRaconteurNode.s().unpackVarType(i2.getAttributes());
		if (i2.getAttributes()!=null) i2.getAttributes().delete();
		java.util.ArrayList<String> con = new java.util.ArrayList<String>();
		for (int j3=0; j3<i2.getConnectionURL().size(); j3++)
		{
			String con2=i2.getConnectionURL().get(j3);
			con.add(con2);
		}
		ConnectionURL = con.toArray(new String[0]);
		
	}

}