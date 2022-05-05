package com.robotraconteur.javatest;

import com.robotraconteur.*;
import com.robotraconteur.testing.TestService1.*;
import com.robotraconteur.testing.TestService2.*;

public class sub2_impl implements sub2
{
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

    private String m_data;
    public final String get_data()
    {
        return m_data;
    }
    public final void set_data(String value)
    {
        m_data = value;
    }

    private java.util.HashMap<String, sub3_impl> o3_1 = new java.util.HashMap<String, sub3_impl>();

    public final sub3 get_o3_1(String ind)
    {
        if (o3_1.containsKey(ind))
        {
            return o3_1.get(ind);
        }
        sub3_impl o = new sub3_impl();
        o.m_ind = ind;
        o3_1.put(ind, o);
        return o;
    }
}