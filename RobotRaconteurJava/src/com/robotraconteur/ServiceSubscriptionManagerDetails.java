package com.robotraconteur;

public class ServiceSubscriptionManagerDetails
{
    public String Name;
    public ServiceSubscriptionManager_CONNECTION_METHOD ConnectionMethod =
        ServiceSubscriptionManager_CONNECTION_METHOD.ServiceSubscriptionManager_CONNECTION_METHOD_DEFAULT;
    public String[] Urls;
    public String UrlUsername;
    public java.util.Map<String, Object> UrlCredentials;
    public String[] ServiceTypes;
    public ServiceSubscriptionFilter Filter;
    public boolean Enabled = true;
}
