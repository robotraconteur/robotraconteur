package com.robotraconteur;

public class CallbackServer<T> extends Callback<T>
{
    private ServiceSkel skel;

    public CallbackServer(String name, ServiceSkel skel)
    {
        super(name);
        this.skel = skel;
    }

    @Override public T getFunction()
    {
        throw new UnsupportedOperationException("Invalid for server side of callback");
    }
    @Override public void setFunction(T value)
    {
        throw new UnsupportedOperationException("Invalid for server side of callback");
    }

    // C# TO JAVA CONVERTER WARNING: Unsigned integer types have no direct equivalent in Java:
    // ORIGINAL LINE: public override T GetClientFunction(uint e)
    @Override public T getClientFunction(long e)
    {
        return (T)skel.getCallbackFunction(e, m_MemberName);
    }
}
