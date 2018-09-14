package com.robotraconteur;

public class UnsignedLong {
public long value;
public UnsignedLong()
{
	this.value=0;
}

public UnsignedLong(long value)
{
	this.value=value;		
}

public UnsignedLongs array()
{
	return new UnsignedLongs(new long[] {value});
	

}
}