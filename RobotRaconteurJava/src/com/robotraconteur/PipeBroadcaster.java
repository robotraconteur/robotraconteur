package com.robotraconteur;

import java.util.*;
import java.lang.ref.*;
import java.util.concurrent.*;

public class PipeBroadcaster<T>
{
    protected WrappedPipeBroadcaster innerpipe;
    protected TypeDefinition type;
    protected Pipe<T> pipe;

    static class WrappedPipeBroadcasterPredicateDirectorJava extends WrappedPipeBroadcasterPredicateDirector
    {
        Func2<Long, Integer, Boolean> f;

        public WrappedPipeBroadcasterPredicateDirectorJava(Func2<Long, Integer, Boolean> f)
        {
            this.f = f;
        }

        @Override public boolean predicate(long client_endpoint, int index)
        {
            return f.func(client_endpoint, index);
        }
    }

    public PipeBroadcaster(Pipe<T> pipe)
    {
        this(pipe, -1);
    }

    public PipeBroadcaster(Pipe<T> pipe, int maximum_backlog)
    {
        this.pipe = pipe;
        this.innerpipe = new WrappedPipeBroadcaster();
        this.innerpipe.init((WrappedPipeServer)pipe.innerpipe, maximum_backlog);
        this.type = ((WrappedPipeServer)pipe.innerpipe).getType();
    }

    public Pipe<T> getPipe()
    {
        return pipe;
    }

    public void asyncSendPacket(T packet, Action handler)
    {
        Object dat = null;
        MessageElement m = null;
        AsyncVoidNoErrReturnDirectorImpl h = new AsyncVoidNoErrReturnDirectorImpl(handler);

        try
        {
            dat = RobotRaconteurNode.s().packVarType(packet);
            m = new MessageElement("value", dat);
            int id1 = RRObjectHeap.addObject(h);
            innerpipe.asyncSendPacket(m, h, id1);
        }
        finally
        {
            if (m != null)
                m.delete();
            if (dat != null)
            {
                if (dat instanceof MessageElementData)
                {
                    ((MessageElementData)dat).delete();
                }
            }
        }
    }

    public void sendPacket(T packet)
    {
        Object dat = null;
        MessageElement m = null;
        try
        {
            dat = RobotRaconteurNode.s().packVarType(packet);
            m = new MessageElement("value", dat);
            innerpipe.sendPacket(m);
        }
        finally
        {
            if (m != null)
                m.delete();
            if (dat != null)
            {
                if (dat instanceof MessageElementData)
                {
                    ((MessageElementData)dat).delete();
                }
            }
        }
    }

    public long getActivePipeEndpointCount()
    {
        return innerpipe.getActivePipeEndpointCount();
    }

    public void setPredicate(Func2<Long, Integer, Boolean> f)
    {
        WrappedPipeBroadcasterPredicateDirectorJava p = new WrappedPipeBroadcasterPredicateDirectorJava(f);
        int id = RRObjectHeap.addObject(p);
        innerpipe.setPredicateDirector(p, id);
    }

    public int getMaxBacklog()
    {
        return innerpipe.getMaxBacklog();
    }

    public void setMaxBacklog(int maximum_backlog)
    {
        innerpipe.setMaxBacklog(maximum_backlog);
    }
}
