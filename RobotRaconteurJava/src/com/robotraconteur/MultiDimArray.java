package com.robotraconteur;

/**
 Represents an unpacked MultiDimArray.  This class stores
 the real and complex data as column vectors that is
 reshaped based on Dims.
*/
public class MultiDimArray
{

    public MultiDimArray()
    {}

    public MultiDimArray(int[] dims, Object array)
    {
        this.dims = dims;
        this.array = array;
    }

    public int[] dims;
    public Object array;

    public void retrieveSubArray(int[] memorypos, MultiDimArray buffer, int[] bufferpos, int[] count)
    {

        MultiDimArray mema = this;
        MultiDimArray memb = buffer;

        vectoruint32 mema_dims1 = new vectoruint32();
        for (int i = 0; i < mema.dims.length; i++)
            mema_dims1.add((long)mema.dims[i]);
        vectoruint32 memorypos1 = new vectoruint32();
        for (int i = 0; i < memorypos.length; i++)
            memorypos1.add((long)memorypos[i]);
        vectoruint32 memb_dims1 = new vectoruint32();
        for (int i = 0; i < memb.dims.length; i++)
            memb_dims1.add((long)memb.dims[i]);
        vectoruint32 bufferpos1 = new vectoruint32();
        for (int i = 0; i < bufferpos.length; i++)
            bufferpos1.add((long)bufferpos[i]);
        vectoruint32 count1 = new vectoruint32();
        for (int i = 0; i < count.length; i++)
            count1.add((long)count[i]);

        MultiDimArray_CalculateCopyIndicesIter iter = RobotRaconteurJava.multiDimArray_CalculateCopyIndicesBeginIter(
            mema_dims1, memorypos1, memb_dims1, bufferpos1, count1);

        long[] len = new long[1];
        long[] indexa = new long[1];
        long[] indexb = new long[1];

        while (iter.next(indexa, indexb, len))
        {
            DataTypeUtil.arraycopy(mema.array, (int)indexa[0], memb.array, (int)indexb[0], (int)len[0]);
        }
    }

    public void assignSubArray(int[] memorypos, MultiDimArray buffer, int[] bufferpos, int[] count)
    {

        MultiDimArray mema = this;
        MultiDimArray memb = buffer;

        vectoruint32 mema_dims1 = new vectoruint32();
        for (int i = 0; i < mema.dims.length; i++)
            mema_dims1.add((long)mema.dims[i]);
        vectoruint32 memorypos1 = new vectoruint32();
        for (int i = 0; i < memorypos.length; i++)
            memorypos1.add((long)memorypos[i]);
        vectoruint32 memb_dims1 = new vectoruint32();
        for (int i = 0; i < memb.dims.length; i++)
            memb_dims1.add((long)memb.dims[i]);
        vectoruint32 bufferpos1 = new vectoruint32();
        for (int i = 0; i < bufferpos.length; i++)
            bufferpos1.add((long)bufferpos[i]);
        vectoruint32 count1 = new vectoruint32();
        for (int i = 0; i < count.length; i++)
            count1.add((long)count[i]);

        MultiDimArray_CalculateCopyIndicesIter iter = RobotRaconteurJava.multiDimArray_CalculateCopyIndicesBeginIter(
            mema_dims1, memorypos1, memb_dims1, bufferpos1, count1);

        long[] len = new long[1];
        long[] indexa = new long[1];
        long[] indexb = new long[1];

        while (iter.next(indexa, indexb, len))
        {
            DataTypeUtil.arraycopy(memb.array, (int)indexb[0], mema.array, (int)indexa[0], (int)len[0]);
        }
    }
}
