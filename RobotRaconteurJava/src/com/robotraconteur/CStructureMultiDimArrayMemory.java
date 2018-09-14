package com.robotraconteur;

public class CStructureMultiDimArrayMemory<T>  {

	private CStructureMultiDimArray multimemory;

    public CStructureMultiDimArrayMemory()
    {


    }

    public CStructureMultiDimArrayMemory(CStructureMultiDimArray memory)
    {

        multimemory = memory;
    }

    
    public void attach(CStructureMultiDimArray memory)
    {

        this.multimemory = memory;

    }

    public long[] dimensions()
    {
    	int[] dims1 = multimemory.dims;
    	long[] dims2 = new long[dims1.length];
    	System.arraycopy(dims1, 0, dims2, 0, dims1.length);
    	return dims2;
    }

    public long dimCount()
    {
    	return (long)multimemory.dims.length;        
    }
            
    
    private int[] long_to_int(long[] v)
    {
    	int[] o = new int[v.length];
    	for (int i=0; i<v.length; i++)
    	{
    		o[i] = (int)v[i];
    	}
    	return o;
    	
    }
    
    public void read(long[] memorypos, CStructureMultiDimArray buffer, long[] bufferpos, long[] count)
    {    	
        multimemory.retrieveSubArray(long_to_int(memorypos), buffer, long_to_int(bufferpos), long_to_int(count));
    }


    public void write(long[] memorypos, CStructureMultiDimArray buffer, long[] bufferpos, long[] count)
    {    	
        multimemory.assignSubArray(long_to_int(memorypos), buffer, long_to_int(bufferpos), long_to_int(count));
    }
}
