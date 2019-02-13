package com.robotraconteur;

import java.lang.reflect.Array;

public class PodArrayMemory<T> extends ArrayMemory<T> {

	public PodArrayMemory()
    {
		super();
    }

    public PodArrayMemory(T memory)
    {
    	 super(memory);
    }
    
    @Override
    public long length()
    {
    	return Array.getLength(memory);
    }
}
