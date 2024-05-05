package com.robotraconteur;

import java.util.*;

public class Pipe<T>
{
    Object innerpipe;

    private String m_Membername;
    MemberDefinition_Direction direction;

    public final String getMemberName()
    {
        return m_Membername;
    }

    public final MemberDefinition_Direction getDirection()
    {
        return this.direction;
    }

    public Pipe(WrappedPipeClient innerpipe)
    {

        this.innerpipe = innerpipe;
        m_Membername = innerpipe.getMemberName();
        this.direction = innerpipe.direction();
    }

    public Pipe(WrappedPipeServer innerpipe)
    {
        this.innerpipe = innerpipe;
        m_Membername = innerpipe.getMemberName();
        this.direction = innerpipe.direction();
    }

    protected void finalize() throws Throwable
    {
        if (innerpipe instanceof WrappedPipeServer)
        {
            //((WrappedPipeServer)innerpipe).setRR_Director(null);
        }
    }

    public final Action1<Pipe<T>.PipeEndpoint> getPipeConnectCallback()
    {
        throw new RuntimeException("Read only property");
    }
    public final void setPipeConnectCallback(Action1<Pipe<T>.PipeEndpoint> value)
    {
        connectdirectorclass c = new connectdirectorclass(this, value);
        int id = RRObjectHeap.addObject(c);
        ((WrappedPipeServer)this.innerpipe).setWrappedPipeConnectCallback(c, id);
    }

    public Pipe<T>.PipeEndpoint connect()
    {
        return connect(-1);
    }

    public final Pipe<T>.PipeEndpoint connect(int index)
    {

        return new PipeEndpoint(((WrappedPipeClient)innerpipe).connect(index));
    }

    public final void asyncConnect(Action2<PipeEndpoint, RuntimeException> handler)
    {
        asyncConnect(-1, handler, -1);
    }

    public final void asyncConnect(Action2<PipeEndpoint, RuntimeException> handler, int timeout)
    {
        asyncConnect(-1, handler, timeout);
    }

    public final void asyncConnect(int index, Action2<PipeEndpoint, RuntimeException> handler)
    {
        asyncConnect(index, handler, -1);
    }

    public final void asyncConnect(int index, Action2<PipeEndpoint, RuntimeException> handler, int timeout)
    {
        AsyncConnectDirectorImpl h = new AsyncConnectDirectorImpl(handler);
        int id = RRObjectHeap.addObject(h);
        ((WrappedPipeClient)innerpipe).asyncConnect(index, timeout, h, id);
    }

    /*public static interface PipeConnectCallbackFunction<U>
    {
        void connect(Pipe<U>.PipeEndpoint newpipe);
    }

    public static interface PipeClosedCallbackFunction<U>
    {
        void closed(Pipe<U>.PipeEndpoint closedpipe);
    }


    public static interface PipePacketReceivedListener<U>
    {
        void packetreceived(Pipe<U>.PipeEndpoint e);
    }


    public static interface PipePacketAckReceivedListener<U>
    {
        void packetackreceived(Pipe<U>.PipeEndpoint e, long packetnum);
    }*/

    private class AsyncConnectDirectorImpl extends AsyncPipeEndpointReturnDirector
    {
        Action2<PipeEndpoint, RuntimeException> handler_func = null;

        public AsyncConnectDirectorImpl(Action2<PipeEndpoint, RuntimeException> handler_func)
        {
            this.handler_func = handler_func;
        }

        @Override public void handler(WrappedPipeEndpoint m, HandlerErrorInfo error)
        {

            if (error.getError_code() != 0)
            {

                this.handler_func.action(null, RobotRaconteurExceptionUtil.errorInfoToException(error));

                return;
            }

            PipeEndpoint e = null;

            try
            {
                e = new PipeEndpoint(m);
            }
            catch (RuntimeException ee)
            {
                handler_func.action(null, ee);
                return;
            }

            handler_func.action(e, null);
        }
    }

    private class connectdirectorclass extends WrappedPipeServerConnectDirector
    {
        private Pipe<T> pipe;
        private Action1<Pipe<T>.PipeEndpoint> handler;

        public connectdirectorclass(Pipe<T> pipe, Action1<Pipe<T>.PipeEndpoint> handler)
        {
            this.pipe = pipe;
            this.handler = handler;
        }

        @Override public void pipeConnectCallback(WrappedPipeEndpoint pipeendpoint)
        {
            try
            {
                handler.action(new PipeEndpoint(pipeendpoint));
            }
            catch (Exception e)
            {
                MessageEntry merr = new MessageEntry();
                RobotRaconteurExceptionUtil.exceptionToMessageEntry(e, merr);
                RRDirectorExceptionHelper.setError(merr, RRDirectorExceptionHelper.exceptionToStackTraceString(e));
                merr.delete();
            }
        }
    }

    public class PipeEndpoint
    {
        private WrappedPipeEndpoint innerpipe;

        private directorclass director;

        public PipeEndpoint(WrappedPipeEndpoint innerpipe)
        {
            this.innerpipe = innerpipe;
            director = new directorclass(this);

            innerpipe.setRRDirector(director, RRObjectHeap.addObject(this));
        }

        protected void finalize() throws Throwable
        {
            // innerpipe.setRR_Director(null);
        }

        public final int getIndex()
        {
            return innerpipe.getIndex();
        }

        public final long getEndpoint()
        {
            return innerpipe.getEndpoint();
        }

        public final boolean getRequestPacketAck()
        {
            return innerpipe.getRequestPacketAck();
        }
        public final void setRequestPacketAck(boolean value)
        {
            innerpipe.setRequestPacketAck(value);
        }

        public final boolean getIgnoreReceived()
        {
            return innerpipe.getIgnoreReceived();
        }
        public final void setIgnoreReceived(boolean value)
        {
            innerpipe.setIgnoreReceived(value);
        }

        private Action1<Pipe<T>.PipeEndpoint> close_callback;

        public final Action1<Pipe<T>.PipeEndpoint> getPipeCloseCallback()
        {
            return close_callback;
        }
        public final void setPipeCloseCallback(Action1<Pipe<T>.PipeEndpoint> value)
        {
            close_callback = value;
        }

        public final int available()
        {
            return (int)innerpipe.available();
        }

        public final boolean isUnreliable()
        {
            return innerpipe.isUnreliable();
        }

        public final MemberDefinition_Direction getDirection()
        {
            return innerpipe.direction();
        }

        public final T peekNextPacket()
        {
            MessageElement m = null;
            try
            {
                m = innerpipe.peekNextPacket();
                Object data = RobotRaconteurNode.s().unpackVarType(m);

                return (T)data;
            }
            finally
            {
                if (m != null)
                    m.delete();
            }
        }

        public final T receivePacket()
        {
            MessageElement m = null;
            try
            {
                m = innerpipe.receivePacket();
                Object data = RobotRaconteurNode.s().unpackVarType(m);

                return (T)data;
            }
            finally
            {
                if (m != null)
                    m.delete();
            }
        }

        public final T peekNextPacketWait()
        {
            return peekNextPacketWait(-1);
        }

        public final T peekNextPacketWait(int timeout)
        {
            MessageElement m = null;
            try
            {
                m = innerpipe.peekNextPacketWait(timeout);
                Object data = RobotRaconteurNode.s().unpackVarType(m);

                return (T)data;
            }
            finally
            {
                if (m != null)
                    m.delete();
            }
        }

        public final T receivePacketWait()
        {
            return receivePacketWait(-1);
        }

        public final T receivePacketWait(int timeout)
        {
            MessageElement m = null;
            try
            {
                m = innerpipe.receivePacketWait(timeout);
                Object data = RobotRaconteurNode.s().unpackVarType(m);

                return (T)data;
            }
            finally
            {
                if (m != null)
                    m.delete();
            }
        }

        public class TryReceivePacketWaitResult
        {
            public boolean result = false;
            public T packet;
        }

        public final TryReceivePacketWaitResult TryReceivePacketWait()
        {
            return TryReceivePacketWait(-1, false);
        }

        public final TryReceivePacketWaitResult TryReceivePacketWait(int timeout)
        {
            return TryReceivePacketWait(timeout, false);
        }

        public final TryReceivePacketWaitResult TryReceivePacketWait(int timeout, boolean peek)
        {
            TryReceivePacketWaitResult o = new TryReceivePacketWaitResult();
            o.result = false;

            WrappedTryReceivePacketWaitResult res = innerpipe.tryReceivePacketWait(timeout, peek);
            try
            {
                if (!res.getRes())
                    return o;
                Object data = RobotRaconteurNode.s().unpackVarType(res.getPacket());
                o.result = true;
                o.packet = (T)data;
                return o;
            }
            finally
            {
                if (res != null)
                    res.delete();
            }
        }

        public final long sendPacket(T data)
        {
            Object dat = null;
            MessageElement m = null;
            try
            {
                dat = RobotRaconteurNode.s().packVarType(data);
                m = new MessageElement("value", dat);
                return innerpipe.sendPacket(m);
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

        public final void asyncSendPacket(T data, Action2<Long, RuntimeException> handler)
        {
            Object dat = null;
            MessageElement m = null;
            AsyncUInt32ReturnDirectorImpl h = new AsyncUInt32ReturnDirectorImpl(handler);

            try
            {
                dat = RobotRaconteurNode.s().packVarType(data);
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

        public final void close()
        {
            innerpipe.close();
        }

        public final void asyncClose(Action1<RuntimeException> handler)
        {
            asyncClose(handler, -1);
        }

        public final void asyncClose(Action1<RuntimeException> handler, int timeout)
        {
            AsyncVoidReturnDirectorImpl h = new AsyncVoidReturnDirectorImpl(handler);
            int id1 = RRObjectHeap.addObject(h);
            innerpipe.asyncClose(timeout, h, id1);
        }

        private Vector<Action1<Pipe<T>.PipeEndpoint>> packetlisteners = new Vector<Action1<Pipe<T>.PipeEndpoint>>();
        public void addPacketReceivedListener(Action1<Pipe<T>.PipeEndpoint> listener)
        {
            synchronized (packetlisteners)
            {
                packetlisteners.add(listener);
            }
        }
        public void removePacketReceivedListener(Action1<Pipe<T>.PipeEndpoint> listener)
        {
            synchronized (packetlisteners)
            {
                packetlisteners.remove(listener);
            }
        }

        private Vector<Action2<Pipe<T>.PipeEndpoint, Long>> packetacklisteners =
            new Vector<Action2<Pipe<T>.PipeEndpoint, Long>>();
        public void addPacketAckReceivedListener(Action2<Pipe<T>.PipeEndpoint, Long> listener)
        {
            synchronized (packetacklisteners)
            {
                packetacklisteners.add(listener);
            }
        }
        public void removePacketAckReceivedListener(Action2<Pipe<T>.PipeEndpoint, Long> listener)
        {
            synchronized (packetlisteners)
            {
                packetacklisteners.remove(listener);
            }
        }

        private class directorclass extends WrappedPipeEndpointDirector
        {
            private PipeEndpoint pipe;
            public directorclass(PipeEndpoint pipe)
            {
                this.pipe = pipe;
            }

            @Override public void packetAckReceivedEvent(long packetnum)
            {
                try
                {
                    synchronized (pipe.packetacklisteners)
                    {
                        for (Enumeration<Action2<Pipe<T>.PipeEndpoint, Long>> e = pipe.packetacklisteners.elements();
                             e.hasMoreElements();)
                        {
                            Action2<Pipe<T>.PipeEndpoint, Long> ee = e.nextElement();
                            ee.action(pipe, (long)packetnum);
                        }
                    }
                }
                catch (Exception e)
                {
                    MessageEntry merr = new MessageEntry();
                    RobotRaconteurExceptionUtil.exceptionToMessageEntry(e, merr);
                    RRDirectorExceptionHelper.setError(merr, RRDirectorExceptionHelper.exceptionToStackTraceString(e));
                    merr.delete();
                }
            }

            @Override public void packetReceivedEvent()
            {
                try
                {
                    synchronized (pipe.packetlisteners)
                    {
                        for (Enumeration<Action1<Pipe<T>.PipeEndpoint>> e = pipe.packetlisteners.elements();
                             e.hasMoreElements();)
                        {
                            Action1<Pipe<T>.PipeEndpoint> ee = e.nextElement();
                            ee.action(pipe);
                        }
                    }
                }
                catch (Exception e)
                {
                    MessageEntry merr = new MessageEntry();
                    RobotRaconteurExceptionUtil.exceptionToMessageEntry(e, merr);
                    RRDirectorExceptionHelper.setError(merr, RRDirectorExceptionHelper.exceptionToStackTraceString(e));
                    merr.delete();
                }
            }

            @Override public void pipeEndpointClosedCallback()
            {
                try
                {
                    if (pipe.getPipeCloseCallback() == null)
                    {
                        return;
                    }
                    pipe.getPipeCloseCallback().action(pipe);
                }
                catch (Exception e)
                {
                    MessageEntry merr = new MessageEntry();
                    RobotRaconteurExceptionUtil.exceptionToMessageEntry(e, merr);
                    RRDirectorExceptionHelper.setError(merr, RRDirectorExceptionHelper.exceptionToStackTraceString(e));
                    merr.delete();
                }
            }
        }
    }
}
