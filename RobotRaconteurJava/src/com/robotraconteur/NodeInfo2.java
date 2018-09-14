package com.robotraconteur;

public class NodeInfo2 {
		
	public NodeID NodeID;
	public String NodeName;
	public String[] ConnectionURL;
	
	public NodeInfo2() {}
	
	public NodeInfo2(WrappedNodeInfo2 i)
	{
		java.util.ArrayList<String> con = new java.util.ArrayList<String>();
		for (int j3=0; j3<i.getConnectionURL().size(); j3++)
		{
			String con2=i.getConnectionURL().get(j3);
			con.add(con2);
		}
		ConnectionURL = con.toArray(new String[0]);
		
		this.NodeID=i.getNodeID();
		this.NodeName=i.getNodeName();
		
	}
	
}
