package com.robotraconteur;

public class UnsignedShorts {
public short[] value;
public UnsignedShorts(short[] value)
{
	this.value=value;
	
}

public UnsignedShorts(int size)
{
	this.value=new short[size];
}

public UnsignedShort get(int i)
{
	return new UnsignedShort(this.value[i]);
	
}
}
