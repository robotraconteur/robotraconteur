package com.robotraconteur.javatest;

import com.robotraconteur.testing.TestService3.*;
import com.robotraconteur.*;

public class obj5_impl extends obj5_default_impl implements obj5
{

    ArrayMemory<int[]> _m1 = new ArrayMemory<int[]>(new int[100]);
    ArrayMemory<int[]> _m2 = new ArrayMemory<int[]>(new int[100]);
    ArrayMemory<int[]> _m3 = new ArrayMemory<int[]>(new int[100]);

    @Override public int f1()
    {
        return 10;
    }

    @Override public int f2()
    {
        return 11;
    }

    @Override public ArrayMemory<int[]> get_m1()
    {
        return _m1;
    }

    @Override public ArrayMemory<int[]> get_m2()
    {
        return _m2;
    }

    @Override public ArrayMemory<int[]> get_m3()
    {
        return _m3;
    }
}
