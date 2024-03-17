package com.robotraconteur;

public class ClientServiceListenerDirectorJava extends ClientServiceListenerDirector
{

    public int listenerid;

    private Action3<ServiceStub, ClientServiceListenerEventType, Object> listener;
    public ServiceStub stub;

    public ClientServiceListenerDirectorJava(Action3<ServiceStub, ClientServiceListenerEventType, Object> listener)
    {

        this.listener = listener;
    }

    @Override public void callback(int code)
    {
        listener.action(stub, ClientServiceListenerEventType.swigToEnum(code), null);
    }

    @Override public void callback2(int code, String p)
    {
        listener.action(stub, ClientServiceListenerEventType.swigToEnum(code), p);
    }
}
