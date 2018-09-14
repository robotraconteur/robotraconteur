package com.robotraconteur;

public class ArrayMemory<T>
{

	protected T memory;

	public ArrayMemory()
	{

	}

	public ArrayMemory(T memory)
	{
		this.memory = memory;

	}



	public void attach(T memory)
	{
		this.memory = memory;

	}

	public long length()
	{
		return DataTypeUtil.getArrayLength(this.memory);
		
	}

	public void read(long memorypos, T buffer, long bufferpos, long count)
	{

		DataTypeUtil.arraycopy((Object)memory, (int)memorypos, (Object)buffer, (int)bufferpos, (int)count);
	}

	public void write(long memorypos, T buffer, long bufferpos, long count)
	{

		DataTypeUtil.arraycopy(buffer, (int)bufferpos, memory, (int)memorypos, (int)count);
	}


}