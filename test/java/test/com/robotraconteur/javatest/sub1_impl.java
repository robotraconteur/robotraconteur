package com.robotraconteur.javatest;

import com.robotraconteur.*;
import com.robotraconteur.testing.TestService1.*;
import com.robotraconteur.testing.TestService2.*;

import java.util.concurrent.TimeUnit;
import java.util.concurrent.locks.*;

public class sub1_impl implements sub1, IRobotRaconteurMonitorObject
{
    private double[] m_d1;
    public final double[] get_d1()
    {
        return m_d1;
    }
    public final void set_d1(double[] value)
    {
        m_d1 = value;
    }

    private MultiDimArray m_d2;
    public final MultiDimArray get_d2()
    {
        return m_d2;
    }
    public final void set_d2(MultiDimArray value)
    {
        m_d2 = value;
    }

    public String m_s_ind;

    public final String get_s_ind()
    {
        return m_s_ind;
    }
    public final void set_s_ind(String value)
    {}

    public int m_i_ind;
    public final int get_i_ind()
    {
        return m_i_ind;
    }
    public final void set_i_ind(int value)
    {}

    private sub2_impl o2_1 = new sub2_impl();

    public final sub2 get_o2_1()
    {
        return o2_1;
    }

    private java.util.HashMap<Integer, sub2_impl> o2_2 = new java.util.HashMap<Integer, sub2_impl>();

    public final sub2 get_o2_2(int ind)
    {
        if (o2_2.containsKey(ind))
        {
            return o2_2.get(ind);
        }
        sub2_impl o = new sub2_impl();
        o.set_i_ind(ind);
        o2_2.put(ind, o);
        return o;
    }

    private java.util.HashMap<String, sub2_impl> o2_3 = new java.util.HashMap<String, sub2_impl>();

    public final sub2 get_o2_3(String ind)
    {
        if (o2_3.containsKey(ind))
        {
            return o2_3.get(ind);
        }
        sub2_impl o = new sub2_impl();
        o.set_s_ind(ind);
        o2_3.put(ind, o);
        return o;
    }

    RRMonitor lock = new RRMonitor();

    public final void robotRaconteurMonitorEnter()
    {
        try
        {
            lock.lock();
        }
        catch (Exception e)
        {
            e.printStackTrace(System.out);
        }
    }

    public final void robotRaconteurMonitorEnter(int timeout)
    {
        try
        {

            RRAssert.isTrue(lock.tryLock(timeout));
        }
        catch (Exception e)
        {
            e.printStackTrace(System.out);
            RRAssert.fail();
        }
    }

    public final void robotRaconteurMonitorExit()
    {
        try
        {
            lock.unlock();
        }
        catch (Exception e)
        {
            e.printStackTrace(System.out);
        }
    }
}