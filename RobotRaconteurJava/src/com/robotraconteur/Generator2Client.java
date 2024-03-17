package com.robotraconteur;

import java.util.ArrayList;
import java.util.List;

import com.robotraconteur.ServiceStub.AsyncRequestDirectorImpl;

public class Generator2Client<ReturnType> implements Generator2<ReturnType>
{
    protected WrappedGeneratorClient inner_gen;

    public Generator2Client(WrappedGeneratorClient inner_gen)
    {
        this.inner_gen = inner_gen;
    }

    public ReturnType next()
    {

        MessageElement m2 = null;
        try
        {
            m2 = inner_gen.next(null);
            return (ReturnType)RobotRaconteurNode.s().unpackVarType(m2);
        }
        finally
        {
            if (m2 != null)
                m2.finalize();
        }
    }

    public void asyncNext(Action2<ReturnType, RuntimeException> handler, int timeout)
    {
        AsyncRequestDirectorImpl d = new AsyncRequestDirectorImpl(new endAsyncNext(), handler);
        int id = RRObjectHeap.addObject(d);
        inner_gen.asyncNext(null, timeout, d, id);
    }

    protected class endAsyncNext implements Action3<MessageElement, RuntimeException, Object>
    {
        public void action(MessageElement m, RuntimeException err, Object p)
        {
            Action2<ReturnType, RuntimeException> h = (Action2<ReturnType, RuntimeException>)p;
            if (err != null)
            {
                h.action(null, err);
                return;
            }

            h.action((ReturnType)RobotRaconteurNode.s().unpackVarType(m), null);
        }
    }

    public void abort()
    {
        inner_gen.abort();
    }
    public void asyncAbort(Action1<RuntimeException> handler, int timeout)
    {
        AsyncVoidReturnDirectorImpl h = new AsyncVoidReturnDirectorImpl(handler);
        int id = RRObjectHeap.addObject(h);
        inner_gen.asyncAbort(timeout, h, id);
    }

    public void close()
    {
        inner_gen.close();
    }
    public void asyncClose(Action1<RuntimeException> handler, int timeout)
    {
        AsyncVoidReturnDirectorImpl h = new AsyncVoidReturnDirectorImpl(handler);
        int id = RRObjectHeap.addObject(h);
        inner_gen.asyncClose(timeout, h, id);
    }

    @Override public List<ReturnType> nextAll()
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
