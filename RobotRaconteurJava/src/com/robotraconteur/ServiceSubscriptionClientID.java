package com.robotraconteur;

public class ServiceSubscriptionClientID
{
	public NodeID NodeID;
	public String ServiceName;

	public ServiceSubscriptionClientID(NodeID node_id, String service_name)
	{
		this.NodeID = node_id;
		this.ServiceName = service_name;
	}

	ServiceSubscriptionClientID(WrappedServiceSubscriptionClientID id1)
	{
		this.NodeID = id1.getNodeID();
		this.ServiceName = id1.getServiceName();
	}

	@Override
	public boolean equals(Object id2)
	{
		ServiceSubscriptionClientID id3 = (ServiceSubscriptionClientID)id2;
		return (this.NodeID.equals(id3.NodeID)) && (this.ServiceName.equals(id3.ServiceName));
	}

	@Override
	public int hashCode()
	{
		return this.NodeID.toString().hashCode() + this.ServiceName.hashCode();
	}

}