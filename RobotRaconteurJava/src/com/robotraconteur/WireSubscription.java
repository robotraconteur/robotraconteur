package com.robotraconteur;

import java.util.Enumeration;
import java.util.Vector;
import java.lang.ref.WeakReference;
import java.util.HashMap;
import java.util.Map;

public class WireSubscription<T>
{

    class WrappedWireSubscriptionDirectorJava extends WrappedWireSubscriptionDirector
    {
        WeakReference<WireSubscription<T>> subscription1;

        WrappedWireSubscriptionDirectorJava(WireSubscription<T> subscription)
        {
            subscription1 = new WeakReference(subscription);
        }

        @Override
        public void wireValueChanged(WrappedWireSubscription v, WrappedService_typed_packet value, TimeSpec time)
        {
            WireSubscription<T> s = (WireSubscription<T>)subscription1.get();
            if (s == null)
                return;

            T v1;

            MessageElement packet = value.getPacket();
            try
            {
                v1 = s.unpackValue(packet);
            }
            finally
            {
                if (packet != null)
                    packet.finalize();
                if (value != null)
                    value.finalize();
            }

            synchronized (s.wirelisteners)
            {
                for (Enumeration<Action3<WireSubscription<T>, T, TimeSpec>> e = s.wirelisteners.elements();
                     e.hasMoreElements();)
                {
                    Action3<WireSubscription<T>, T, TimeSpec> ee = e.nextElement();
                    ee.action(s, (T)v1, time);
                }
            }
        }
    }

    WrappedWireSubscription _subscription;

    WireSubscription(WrappedWireSubscription subscription)
    {
        _subscription = subscription;
        WrappedWireSubscriptionDirectorJava director = new WrappedWireSubscriptionDirectorJava(this);
        int id = RRObjectHeap.addObject(director);
        subscription.setRRDirector(director, id);
    }

    T unpackValue(MessageElement m)
    {
        Object data = RobotRaconteurNode.s().unpackVarTypeDispose(m);
        return (T)data;
    }

    public T getInValue()
    {
        WrappedService_typed_packet m = null;
        MessageElement m1 = null;
        try
        {
            m = _subscription.getInValue();
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

    public boolean waitInValueValid()
    {
        return waitInValueValid(-1);
    }

    public boolean waitInValueValid(int timeout)
    {
        return _subscription.waitInValueValid();
    }

    public boolean getIgnoreInValue()
    {
        return _subscription.getIgnoreInValue();
    }

    public void setIgnoreInValue(boolean ignore)
    {
        _subscription.setIgnoreInValue(ignore);
    }

    public int getInValueLifespan()
    {
        return _subscription.getInValueLifespan();
    }

    public void setInValueLifespan(int millis)
    {
        _subscription.setInValueLifespan(millis);
    }

    public void setOutValueAll(T value)
    {
        WrappedWireSubscription_send_iterator iter = new WrappedWireSubscription_send_iterator(_subscription);
        while (iter.next() != null)
        {
            Object dat = null;
            MessageElement m = null;
            try
            {
                dat = RobotRaconteurNode.s().packVarType(value);
                m = new MessageElement("value", dat);
                iter.setOutValue(m);
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

    public long getActiveWireConnectionCount()
    {
        return _subscription.getActiveWireConnectionCount();
    }

    public void close()
    {
        _subscription.close();
    }

    Vector<Action3<WireSubscription<T>, T, TimeSpec>> wirelisteners =
        new Vector<Action3<WireSubscription<T>, T, TimeSpec>>();

    public void addWireValueChangedListener(Action3<WireSubscription<T>, T, TimeSpec> listener)
    {
        synchronized (wirelisteners)
        {
            wirelisteners.add(listener);
        }
    }
    public void removeWireValueChangedListener(Action3<WireSubscription<T>, T, TimeSpec> listener)
    {
        synchronized (wirelisteners)
        {
            wirelisteners.remove(listener);
        }
    }
}