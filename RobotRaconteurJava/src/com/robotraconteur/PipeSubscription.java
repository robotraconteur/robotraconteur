package com.robotraconteur;

import java.util.Enumeration;
import java.util.Vector;
import java.lang.ref.WeakReference;
import java.util.HashMap;
import java.util.Map;
import java.util.concurrent.TimeoutException;

public class PipeSubscription<T>
{

    class WrappedPipeSubscriptionDirectorJava extends WrappedPipeSubscriptionDirector
    {
        WeakReference<PipeSubscription<T>> subscription1;

        WrappedPipeSubscriptionDirectorJava(PipeSubscription<T> subscription)
        {
            subscription1 = new WeakReference(subscription);
        }

        @Override public void pipePacketReceived(WrappedPipeSubscription v)
        {
            PipeSubscription<T> s = (PipeSubscription<T>)subscription1.get();
            if (s == null)
                return;

            synchronized (s.packetlisteners)
            {
                for (Enumeration<Action1<PipeSubscription<T>>> e = s.packetlisteners.elements(); e.hasMoreElements();)
                {
                    Action1<PipeSubscription<T>> ee = e.nextElement();
                    ee.action(s);
                }
            }
        }
    }

    WrappedPipeSubscription _subscription;

    PipeSubscription(WrappedPipeSubscription subscription)
    {
        _subscription = subscription;
        WrappedPipeSubscriptionDirectorJava director = new WrappedPipeSubscriptionDirectorJava(this);
        int id = RRObjectHeap.addObject(director);
        subscription.setRRDirector(director, id);
    }

    T unpackValue(MessageElement m)
    {
        Object data = RobotRaconteurNode.s().unpackVarTypeDispose(m);
        return (T)data;
    }

    public T receivePacket()
    {
        WrappedService_typed_packet m = null;
        MessageElement m1 = null;
        try
        {
            m = _subscription.receivePacket();
            m1 = m.getPacket();
            return unpackValue(m1);
        }
        finally
        {
            if (m != null)
                m.finalize();
            if (m1 != null)
                m1.finalize();
        }
    }

    public T receivePacketWait() throws TimeoutException
    {
        return receivePacketWait(-1);
    }

    public T receivePacketWait(int millis) throws TimeoutException
    {
        WrappedService_typed_packet m = null;
        MessageElement m1 = null;
        try
        {
            m = new WrappedService_typed_packet();
            boolean res = _subscription.tryReceivePacketWait(m, millis);
            if (!res)
                throw new TimeoutException("Timed out");
            m1 = m.getPacket();
            return unpackValue(m1);
        }
        finally
        {
            if (m != null)
                m.finalize();
            if (m1 != null)
                m1.finalize();
        }
    }

    public int available()
    {
        return (int)_subscription.available();
    }

    public boolean getIgnoreReceived()
    {
        return _subscription.getIgnoreReceived();
    }

    public void setIgnoreReceived(boolean ignore)
    {
        _subscription.setIgnoreReceived(ignore);
    }

    public void asyncSendPacketAll(T packet)
    {
        WrappedPipeSubscription_send_iterator iter = new WrappedPipeSubscription_send_iterator(_subscription);
        try
        {
            while (iter.next() != null)
            {
                Object dat = null;
                MessageElement m = null;
                try
                {
                    dat = RobotRaconteurNode.s().packVarType(packet);
                    m = new MessageElement("value", dat);
                    iter.asyncSendPacket(m);
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
        }
        finally
        {
            if (iter != null)
            {
                iter.delete();
            }
        }
    }

    public long getActivePipeEndpointCount()
    {
        return _subscription.getActivePipeEndpointCount();
    }

    public void close()
    {
        _subscription.close();
    }

    public void addPipePacketReceivedListener(Action1<PipeSubscription<T>> listener)
    {
        synchronized (packetlisteners)
        {
            packetlisteners.add(listener);
        }
    }
    public void removePipePacketReceivedListener(Action1<PipeSubscription<T>> listener)
    {
        synchronized (packetlisteners)
        {
            packetlisteners.remove(listener);
        }
    }

    Vector<Action1<PipeSubscription<T>>> packetlisteners = new Vector<Action1<PipeSubscription<T>>>();
}
