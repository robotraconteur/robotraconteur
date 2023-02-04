package com.robotraconteur;

public class UnsignedInts
{
    public int[] value;
    public UnsignedInts(int[] value)
    {
        this.value = value;
    }

    public UnsignedInts(int size)
    {
        this.value = new int[size];
    }

    public UnsignedInt get(int i)
    {
        return new UnsignedInt(this.value[i]);
    }
}
