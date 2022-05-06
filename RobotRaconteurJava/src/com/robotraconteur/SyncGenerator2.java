package com.robotraconteur;

import java.util.ArrayList;
import java.util.List;

public abstract class SyncGenerator2<ReturnType> implements Generator2<ReturnType>
{
    public abstract void abort();
    public abstract ReturnType next();

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

    public void asyncNext(Action2<ReturnType, RuntimeException> handler, int timeout)
    {
        try
        {
            ReturnType r = next();
            handler.action(r, null);
        }
        catch (RuntimeException e)
        {
            handler.action(null, e);
            return;
        }
        return;
    }

    public List<ReturnType> nextAll()
    {
        ArrayList<ReturnType> o = new ArrayList<ReturnType>();
        try
        {
            while (true)
            {
                o.add(next());
            }
        }
        catch (StopIterationException e)
        {}
        return o;
    }
}
