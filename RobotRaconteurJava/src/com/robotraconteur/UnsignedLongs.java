package com.robotraconteur;

public class UnsignedLongs {
public long[] value;
public UnsignedLongs(long[] value)
{
	this.value=value;
	
}

public UnsignedLongs(int size)
{
	this.value=new long[size];
}

public UnsignedLong get(int i)
{
	return new UnsignedLong(this.value[i]);
	
}

}
