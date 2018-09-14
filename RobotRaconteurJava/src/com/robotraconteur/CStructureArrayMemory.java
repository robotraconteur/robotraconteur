package com.robotraconteur;

import java.lang.reflect.Array;

public class CStructureArrayMemory<T> extends ArrayMemory<T> {

	public CStructureArrayMemory()
    {
		super();
    }

    public CStructureArrayMemory(T memory)
    {
    	 super(memory);
    }
    
    @Override
    public long length()
    {
    	return Array.getLength(memory);
    }
}
