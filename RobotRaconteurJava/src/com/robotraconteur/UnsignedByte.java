package com.robotraconteur;

public class UnsignedByte {
	public byte value;
	
	public UnsignedByte()
	{
		this.value=0;
	}
	
	public UnsignedByte(byte value)
	{
		this.value=value;		
	}
	
	public UnsignedBytes array()
	{
		return new UnsignedBytes(new byte[] {value});
		
	}
}
