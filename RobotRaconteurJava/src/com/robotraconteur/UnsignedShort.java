package com.robotraconteur;

public class UnsignedShort
{
    public short value;

    public UnsignedShort()
    {
        this.value = 0;
    }

    public UnsignedShort(short value)
    {
        this.value = value;
    }

    public UnsignedShorts array()
    {
        return new UnsignedShorts(new short[] {value});
    }
}
