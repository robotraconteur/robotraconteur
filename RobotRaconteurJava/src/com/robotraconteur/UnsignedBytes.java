package com.robotraconteur;

public class UnsignedBytes
{
    public byte[] value;

    public UnsignedBytes(byte[] value)
    {
        this.value = value;
    }

    public UnsignedBytes(int size)
    {
        this.value = new byte[size];
    }

    public UnsignedByte get(int i)
    {
        return new UnsignedByte(this.value[i]);
    }
}
