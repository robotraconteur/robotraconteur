package com.robotraconteur;

public class ServerServiceListenerDirectorJava extends ServerServiceListenerDirector
{

    public int listenerid;

    private Action3<ServerContext, ServerServiceListenerEventType, Object> listener;
    private ServerContext context;

    public ServerServiceListenerDirectorJava(ServerContext context,
                                             Action3<ServerContext, ServerServiceListenerEventType, Object> listener)
    {
        this.listener = listener;
        this.context = context;
    }

    // C# TO JAVA CONVERTER WARNING: Unsigned integer types have no direct equivalent in Java:
    // ORIGINAL LINE: public override void Callback(int code, uint endpoint)
    @Override public void callback(int code, long endpoint)
    {
        try
        {
            listener.action(context, ServerServiceListenerEventType.swigToEnum(code), endpoint);
        }
        catch (java.lang.Exception e)
        {}
    }
}