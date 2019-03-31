package com.robotraconteur;

import java.lang.reflect.Array;

public class NamedArrayMemory<T> extends ArrayMemory<T> {

	public NamedArrayMemory()
    {
		super();
    }

    public NamedArrayMemory(T memory)
    {
    	 super(memory);
    }
    
    @Override
    public long length()
    {
    	return Array.getLength(memory);
    }
}
