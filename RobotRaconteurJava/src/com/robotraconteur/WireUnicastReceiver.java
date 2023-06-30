package com.robotraconteur;

import java.lang.ref.WeakReference;
import java.util.*;

public class WireUnicastReceiver<T>
{

    protected WrappedWireUnicastReceiver innerwire;
    protected TypeDefinition type;
    protected Wire<T> wire;

    public WireUnicastReceiver(Wire<T> wire)
    {
        this.wire = wire;
        this.innerwire = new WrappedWireUnicastReceiver();
        this.innerwire.init((WrappedWireServer)wire.innerwire);
        this.type = ((WrappedWireServer)wire.innerwire).getType();
        ValueChangedDirector<T> cb = new ValueChangedDirector<T>(this);
        int id = RRObjectHeap.addObject(cb);
        this.innerwire.addInValueChangedListener(cb, id);
    }

    public Wire<T> getWire()
    {
        return wire;
    }

    public T getInValue()
    {
        TimeSpec ts = new TimeSpec();
        long[] ep = new long[1];
        MessageElement m = this.innerwire.getInValue(ts, ep);
        Object data = RobotRaconteurNode.s().unpackVarTypeDispose(m);
        return (T)data;
    }

    private Vector<Action3<T, TimeSpec, Long>> wirelisteners = new Vector<Action3<T, TimeSpec, Long>>();
    public void addInValueListener(Action3<T, TimeSpec, Long> listener)
    {
        synchronized (wirelisteners)
        {
            wirelisteners.add(listener);
        }
    }
    public void removeInValueListener(Action3<T, TimeSpec, Long> listener)
    {
        synchronized (wirelisteners)
        {
            wirelisteners.remove(listener);
        }
    }

    private void fireInValueListener(T value, TimeSpec ts, Long ep)
    {
        synchronized (wirelisteners)
        {
            for (Enumeration<Action3<T, TimeSpec, Long>> e = wirelisteners.elements(); e.hasMoreElements();)
            {
                Action3<T, TimeSpec, Long> ee = e.nextElement();
                ee.action(value, ts, ep);
            }
        }
    }

    public int getInValueLifespan()
    {
        return innerwire.getInValueLifespan();
    }

    public void setInValueLifespan(int millis)
    {
        innerwire.setInValueLifespan(millis);
    }

    static class ValueChangedDirector<T> extends WrappedWireServerPokeValueDirector
    {
        WeakReference<WireUnicastReceiver<T>> cb;

        public ValueChangedDirector(WireUnicastReceiver<T> cb)
        {
            this.cb = new WeakReference<WireUnicastReceiver<T>>(cb);
        }

        @Override public void pokeValue(MessageElement el, TimeSpec ts, long ep)
        {
            try
            {
                WireUnicastReceiver<T> cb1 = this.cb.get();
                if (cb1 == null)
                    return;
                T value = (T)RobotRaconteurNode.s().unpackVarType(el);
                cb1.fireInValueListener(value, ts, ep);
            }
            finally
            {
                if (el != null)
                    el.delete();
            }
        }
    }
}