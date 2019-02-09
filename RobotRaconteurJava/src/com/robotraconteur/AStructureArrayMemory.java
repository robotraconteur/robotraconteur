package com.robotraconteur;

import java.lang.reflect.Array;

public class AStructureArrayMemory<T> extends ArrayMemory<T> {

	public AStructureArrayMemory()
    {
		super();
    }

    public AStructureArrayMemory(T memory)
    {
    	 super(memory);
    }
    
    @Override
    public long length()
    {
    	return Array.getLength(memory);
    }
}
