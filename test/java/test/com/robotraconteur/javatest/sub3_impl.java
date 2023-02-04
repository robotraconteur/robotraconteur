package com.robotraconteur.javatest;

import com.robotraconteur.*;
import com.robotraconteur.testing.TestService1.*;
import com.robotraconteur.testing.TestService2.*;

public class sub3_impl implements sub3
{
    private String m_data2;
    public final String get_data2()
    {
        return m_data2;
    }
    public final void set_data2(String value)
    {
        m_data2 = value;
    }

    private double m_data3;
    public final double get_data3()
    {
        return m_data3;
    }
    public final void set_data3(double value)
    {
        m_data3 = value;
    }

    public final double add(double d)
    {
        return d + 42;
    }

    public String m_ind;
    public final String get_ind()
    {
        return m_ind;
    }
    public final void set_ind(String value)
    {}
}