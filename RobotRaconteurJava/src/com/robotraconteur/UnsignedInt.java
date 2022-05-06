package com.robotraconteur;

public class UnsignedInt
{
    public int value;

    public UnsignedInt()
    {
        this.value = 0;
    }

    public UnsignedInt(int value)
    {
        this.value = value;
    }

    public UnsignedInts array()
    {
        return new UnsignedInts(new int[] {value});
    }
}
