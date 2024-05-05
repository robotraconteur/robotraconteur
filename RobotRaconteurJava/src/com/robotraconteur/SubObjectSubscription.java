package com.robotraconteur;

public class SubObjectSubscription
{
    WrappedSubObjectSubscription _subscription;
    ServiceSubscription _parent;

    public SubObjectSubscription(ServiceSubscription parent, WrappedSubObjectSubscription subscription)
    {
        _subscription = subscription;
        _parent = parent;
    }

    public void close()
    {
        _subscription.close();
    }

    public <T> T getDefaultClient()
    {
        WrappedServiceStub s = _subscription.getDefaultClient();
        return (T)_parent.getClientStub(s);
    }

    public <T> T getDefaultClientWait()
    {
        return getDefaultClientWait(-1);
    }

    public <T> T getDefaultClientWait(int timeout)
    {
        WrappedServiceStub s = _subscription.getDefaultClientWait(timeout);
        return (T)_parent.getClientStub(s);
    }

    public void asyncGetDefaultClient(Action2<Object, RuntimeException> handler, int timeout)
    {
        AsyncStubReturnDirectorImpl<Object> h = new AsyncStubReturnDirectorImpl<Object>(handler);
        int id1 = RRObjectHeap.addObject(h);
        _subscription.asyncGetDefaultClient(timeout, h, id1);
    }

    public void asyncGetDefaultClient(Action2<Object, RuntimeException> handler)
    {
        asyncGetDefaultClient(handler, -1);
    }
}
