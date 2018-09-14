package com.robotraconteur;

public abstract class SyncGenerator3<ParamType> implements Generator3<ParamType>
{
    public abstract void abort();
    public abstract void next(ParamType param);

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
    
    public void asyncNext(ParamType param, Action1<RuntimeException> handler, int timeout)
    {
        try
        {
            next(param);
            handler.action(null);
        }
        catch (RuntimeException e)
        {
            handler.action(e);
            return;
        }
        return;
    }
}
