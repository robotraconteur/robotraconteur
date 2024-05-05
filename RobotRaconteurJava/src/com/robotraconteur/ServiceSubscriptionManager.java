package com.robotraconteur;

public class ServiceSubscriptionManager
{
    WrappedServiceSubscriptionManager _subscription_manager;

    java.util.HashMap<String, ServiceSubscription> _subscriptions =
        new java.util.HashMap<String, ServiceSubscription>();

    WrappedServiceSubscriptionManagerDetails _ServiceSubscriptionManager_LoadDetails(
        ServiceSubscriptionManagerDetails details)
    {
        WrappedServiceSubscriptionManagerDetails details2 = new WrappedServiceSubscriptionManagerDetails();
        details2.setName(details.Name);
        details2.setConnectionMethod((ServiceSubscriptionManager_CONNECTION_METHOD)details.ConnectionMethod);
        if (details.Urls != null)
        {
            for (String s : details.Urls)
            {
                details2.getUrls().add(s);
            }
        }
        details2.setUrlUsername(details.UrlUsername);
        if (details.UrlCredentials != null)
        {
            details2.setUrlCredentials((MessageElementData)RobotRaconteurNode.s().packMapType(
                details.UrlCredentials, String.class, Object.class));
        }
        if (details.ServiceTypes != null)
        {
            for (String s : details.ServiceTypes)
            {
                details2.getServiceTypes().add(s);
            }
        }
        if (details.Filter != null)
        {
            details2.setFilter(RobotRaconteurNode.s().subscribeService_LoadFilter(details.Filter));
        }
        details2.setEnabled(details.Enabled);
        return details2;
    }

    public ServiceSubscriptionManager()
    {
        _subscription_manager = new WrappedServiceSubscriptionManager();
    }

    public ServiceSubscriptionManager(ServiceSubscriptionManagerDetails[] details)
    {
        vector_wrappedservicesubscriptionmanagerdetails details2 =
            new vector_wrappedservicesubscriptionmanagerdetails();
        if (details != null)
        {
            for (ServiceSubscriptionManagerDetails d : details)
            {
                details2.add(_ServiceSubscriptionManager_LoadDetails(d));
            }
        }

        _subscription_manager = new WrappedServiceSubscriptionManager(details2);
    }

    public void addSubscription(ServiceSubscriptionManagerDetails details)
    {
        _subscription_manager.addSubscription(_ServiceSubscriptionManager_LoadDetails(details));
    }

    public void removeSubscirption(String name)
    {
        removeSubscription(name, true);
    }

    public void removeSubscription(String name, boolean close)
    {
        synchronized (this)
        {
            _subscription_manager.removeSubscription(name, close);
            if (close)
            {
                _subscriptions.remove(name);
            }
        }
    }

    public void enableSubscription(String name)
    {
        _subscription_manager.enableSubscription(name);
    }

    public void disableSubscription(String name)
    {
        disableSubscription(name, true);
    }

    public void disableSubscription(String name, boolean close)
    {
        _subscription_manager.disableSubscription(name, close);
    }

    public ServiceSubscription getSubscription(String name)
    {
        return getSubscription(name, false);
    }

    public ServiceSubscription getSubscription(String name, boolean force_create)
    {
        synchronized (this)
        {
            if (_subscriptions.containsKey(name))
            {
                return _subscriptions.get(name);
            }

            WrappedServiceSubscription s = _subscription_manager.getSubscription(name, force_create);
            if (s == null)
            {
                return null;
            }

            ServiceSubscription s2 = new ServiceSubscription(s);
            _subscriptions.put(name, s2);
            return s2;
        }
    }

    public boolean isConnected(String name)
    {
        return _subscription_manager.isConnected(name);
    }

    public boolean isEnabled(String name)
    {
        return _subscription_manager.isEnabled(name);
    }

    public void close()
    {
        close(true);
    }

    public void close(boolean close_subscriptions)
    {
        _subscription_manager.close(close_subscriptions);
        synchronized (this)
        {
            _subscriptions.clear();
        }
    }

    public String[] getSubscriptionNames()
    {
        vectorstring s = _subscription_manager.getSubscriptionNames();
        String[] s2 = new String[s.size()];
        for (int i = 0; i < s.size(); i++)
        {
            s2[i] = s.get(i);
        }
        return s2;
    }

    public ServiceSubscriptionManagerDetails[] getSubscriptionDetails()
    {
        vector_wrappedservicesubscriptionmanagerdetails details = _subscription_manager.getSubscriptionDetails();
        ServiceSubscriptionManagerDetails[] details2 = new ServiceSubscriptionManagerDetails[details.size()];
        for (int i = 0; i < details.size(); i++)
        {
            details2[i] = new ServiceSubscriptionManagerDetails();
            WrappedServiceSubscriptionManagerDetails details3 = details.get(i);
            details2[i].Name = details3.getName();
            details2[i].ConnectionMethod = details3.getConnectionMethod();
            if (details3.getUrls() != null)
            {
                details2[i].Urls = details3.getUrls().toArray(new String[0]);
            }
            if (details3.getServiceTypes() != null)
            {
                details2[i].ServiceTypes = details3.getServiceTypes().toArray(new String[0]);
            }
            details2[i].Enabled = details3.getEnabled();
        }
        return details2;
    }
}
