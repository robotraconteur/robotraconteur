package com.robotraconteur;

public class ServiceSubscriptionFilter
{
    public ServiceSubscriptionFilterNode[] Nodes;
    public String[] ServiceNames;
    public String[] TransportSchemes;
    public Func1<ServiceInfo2, Boolean> Predicate;
    public int MaxConnections = 1000000;
}
