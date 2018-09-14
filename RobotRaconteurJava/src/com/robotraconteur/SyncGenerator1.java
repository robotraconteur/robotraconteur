package com.robotraconteur;

public abstract class SyncGenerator1<ReturnType, ParamType> implements Generator1<ReturnType, ParamType>
{
    public abstract void abort();
    public abstract ReturnType next(ParamType param);

    public void asyncAbort(Action1<RuntimeException> handler, int timeout)
    {
        try
        {
            abort();
            handler.action(null);
        }
        catch (RuntimeException e)
        {
            handler.action(e);
            return;
        }
        return;
    }

    public void asyncClose(Action1<RuntimeException> handler, int timeout)
    {
        try
        {
            close();
            handler.action(null);
        }
        catch (RuntimeException e)
        {
            handler.action(e);
            return;
        }
        return;
    }
    
    public void asyncNext(ParamType param, Action2<ReturnType, RuntimeException> handler, int timeout)
    {
        try
        {
            ReturnType r = next(param);
            handler.action(r, null);
        }
        catch (RuntimeException e)
        {
            handler.action(null,e);
            return;
        }
        return;
    }
    
}
