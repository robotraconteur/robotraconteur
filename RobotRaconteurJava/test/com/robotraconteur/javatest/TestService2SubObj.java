package com.robotraconteur.javatest;

import com.robotraconteur.*;
import com.robotraconteur.testing.TestService1.*;
import com.robotraconteur.testing.TestService2.*;

public class TestService2SubObj implements subobj
{
    public final double add_val(double v)
    {
        return v + 1;
    }
}