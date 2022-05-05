package com.robotraconteur;

public class NamedMultiDimArray
{

    public NamedMultiDimArray()
    {
        this.dims = new int[] {0};
    }

    public NamedMultiDimArray(int[] dims, Object namedarray_array)
    {
        this.dims = dims;
        this.namedarray_array = namedarray_array;
    }

    public int[] dims;

    public Object namedarray_array;

    public void retrieveSubArray(int[] memorypos, NamedMultiDimArray buffer, int[] bufferpos, int[] count)
    {

        NamedMultiDimArray mema = this;
        NamedMultiDimArray memb = buffer;

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
            System.arraycopy(mema.namedarray_array, (int)indexa[0], memb.namedarray_array, (int)indexb[0], (int)len[0]);
        }
    }

    public void assignSubArray(int[] memorypos, NamedMultiDimArray buffer, int[] bufferpos, int[] count)
    {
        NamedMultiDimArray mema = this;
        NamedMultiDimArray memb = buffer;

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
            DataTypeUtil.arraycopy(memb.namedarray_array, (int)indexb[0], mema.namedarray_array, (int)indexa[0],
                                   (int)len[0]);
        }
    }
}