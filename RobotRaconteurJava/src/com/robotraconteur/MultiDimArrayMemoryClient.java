package com.robotraconteur;

import java.math.BigInteger;

public class MultiDimArrayMemoryClient<T> extends MultiDimArrayMemory<T>
{
    private MultiDimArrayMemoryBase innermem;
    public MultiDimArrayMemoryClient(MultiDimArrayMemoryBase innermem)
    {
        this.innermem = innermem;
    }

    @Override public long dimCount()
    {

        return innermem.dimCount().longValue();
    }

    @Override public long[] dimensions()
    {
        vector_uint64_t i1 = innermem.dimensions();
        int dimcount = (int)i1.size();
        long[] o = new long[dimcount];
        for (int i = 0; i < o.length; i++)
        {
            o[i] = i1.get(i).longValue();
        }
        return o;
    }

    @Override public void read(long[] memorypos, MultiDimArray buffer, long[] bufferpos, long[] count)
    {
        RRMultiDimArrayUntyped dat = null;
        RRBaseArray datDims = null;
        RRBaseArray datArray = null;

        try
        {
            vector_uint64_t memorypos2 = new vector_uint64_t();

            for (long val : memorypos)
            {
                memorypos2.add(BigInteger.valueOf(val));
            }
            vector_uint64_t count2 = new vector_uint64_t();

            for (long val : count)
            {
                count2.add(BigInteger.valueOf(val));
            }

            dat = WrappedMultiDimArrayMemoryClientUtil.read(innermem, memorypos2, count2);
            datDims = dat.getDims();
            int[] dims = ((UnsignedInts)MessageElementDataUtil.rRBaseArrayToArray(datDims)).value;
            datArray = dat.getArray();
            T array = (T)MessageElementDataUtil.rRBaseArrayToArray(datArray);

            int dimcount = bufferpos.length;

            int[] bufferpos3 = new int[dimcount];
            int[] count3 = new int[dimcount];

            for (int i = 0; i < dimcount; i++)
            {

                bufferpos3[i] = (int)bufferpos[i];
                count3[i] = (int)count[i];
            }

            MultiDimArray dat2 = new MultiDimArray(dims, array);
            buffer.assignSubArray(bufferpos3, dat2, new int[count.length], count3);
        }
        finally
        {
            if (dat != null)
            {
                if (datDims != null)
                    datDims.delete();
                if (datArray != null)
                    datArray.delete();
                dat.delete();
            }
        }
    }

    @Override public void write(long[] memorypos, MultiDimArray buffer, long[] bufferpos, long[] count)
    {
        RRMultiDimArrayUntyped dat2 = null;
        RRBaseArray dat2Dims = null;
        RRBaseArray dat2Array = null;

        try
        {
            int elemcount = 1;

            for (long v : count)
            {
                elemcount *= (int)v;
            }

            int dimcount = count.length;
            int[] count2 = new int[dimcount];
            int[] bufferpos2 = new int[dimcount];

            for (int i = 0; i < dimcount; i++)
            {

                bufferpos2[i] = (int)bufferpos[i];
                count2[i] = (int)count[i];
            }

            T array = (T)DataTypeUtil.arrayFromDataType(innermem.elementTypeID(), elemcount);
            T imag = null;

            MultiDimArray writedat1 = new MultiDimArray(count2, array);
            writedat1.assignSubArray(new int[count.length], buffer, bufferpos2, count2);

            dat2 = new RRMultiDimArrayUntyped();
            dat2Dims = MessageElementDataUtil.arrayToRRBaseArray(new UnsignedInts(count2));
            dat2.setDims(dat2Dims);
            dat2Array = MessageElementDataUtil.arrayToRRBaseArray(array);
            dat2.setArray(dat2Array);

            vector_uint64_t memorypos3 = new vector_uint64_t();

            for (long val : memorypos)
            {
                memorypos3.add(BigInteger.valueOf(val));
            }
            vector_uint64_t count3 = new vector_uint64_t();

            for (long val : count)
            {
                count3.add(BigInteger.valueOf(val));
            }
            vector_uint64_t bufferpos3 = new vector_uint64_t();
            for (int i = 0; i < count.length; i++)
            {
                bufferpos3.add(BigInteger.valueOf(0));
            }

            WrappedMultiDimArrayMemoryClientUtil.write(innermem, memorypos3, dat2, bufferpos3, count3);
        }
        finally
        {
            if (dat2 != null)
            {
                if (dat2Dims != null)
                    dat2Dims.delete();
                if (dat2Array != null)
                    dat2Array.delete();
                dat2.delete();
            }
        }
    }
}