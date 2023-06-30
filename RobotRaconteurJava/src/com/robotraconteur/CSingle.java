package com.robotraconteur;

public class CSingle
{
    public float real;
    public float imag;

    public CSingle(float real, float imag)
    {
        this.real = real;
        this.imag = imag;
    }

    @Override public boolean equals(Object obj)
    {
        if (obj instanceof CSingle)
        {
            CSingle obj1 = (CSingle)obj;
            return this.real == obj1.real && this.imag == obj1.imag;
        }
        return false;
    }
}
