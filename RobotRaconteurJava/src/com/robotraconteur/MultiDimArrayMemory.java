package com.robotraconteur;

public class MultiDimArrayMemory<T>
{

    private MultiDimArray multimemory;

    public MultiDimArrayMemory()
    {}

    public MultiDimArrayMemory(MultiDimArray memory)
    {

        multimemory = memory;
    }

    public void attach(MultiDimArray memory)
    {

        this.multimemory = memory;
    }

    public long[] dimensions()
    {
        // C# TO JAVA CONVERTER WARNING: Unsigned integer types have no direct equivalent in Java:
        // ORIGINAL LINE: return multimemory.Dims.Select(x => (ulong)x).ToArray();
        int dimcount = multimemory.dims.length;
        long[] o = new long[dimcount];
        for (int i = 0; i < o.length; i++)
        {
            o[i] = multimemory.dims[i];
        }

        return o;
    }

    public long dimCount()
    {
        // C# TO JAVA CONVERTER WARNING: Unsigned integer types have no direct equivalent in Java:
        // ORIGINAL LINE: return (ulong)multimemory.DimCount;
        return (long)multimemory.dims.length;
    }

    // C# TO JAVA CONVERTER WARNING: Unsigned integer types have no direct equivalent in Java:
    // ORIGINAL LINE: public virtual void Read(ulong[] memorypos, MultiDimArray buffer, ulong[] bufferpos, ulong[]
    // count)
    public void read(long[] memorypos, MultiDimArray buffer, long[] bufferpos, long[] count)
    {
        int dimcount = multimemory.dims.length;
        int[] memorypos2 = new int[dimcount];
        int[] bufferpos2 = new int[dimcount];
        int[] count2 = new int[dimcount];

        for (int i = 0; i < dimcount; i++)
        {
            memorypos2[i] = (int)memorypos[i];
            bufferpos2[i] = (int)bufferpos[i];
            count2[i] = (int)count[i];
        }

        multimemory.retrieveSubArray(memorypos2, buffer, bufferpos2, count2);
    }

    // C# TO JAVA CONVERTER WARNING: Unsigned integer types have no direct equivalent in Java:
    // ORIGINAL LINE: public virtual void Write(ulong[] memorypos, MultiDimArray buffer, ulong[] bufferpos, ulong[]
    // count)
    public void write(long[] memorypos, MultiDimArray buffer, long[] bufferpos, long[] count)
    {

        int dimcount = multimemory.dims.length;
        int[] memorypos2 = new int[dimcount];
        int[] bufferpos2 = new int[dimcount];
        int[] count2 = new int[dimcount];

        for (int i = 0; i < dimcount; i++)
        {
            memorypos2[i] = (int)memorypos[i];
            bufferpos2[i] = (int)bufferpos[i];
            count2[i] = (int)count[i];
        }

        multimemory.assignSubArray(memorypos2, buffer, bufferpos2, count2);
    }
}
