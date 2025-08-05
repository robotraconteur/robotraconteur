package com.robotraconteur;

public class PodMultiDimArray
{

    public PodMultiDimArray()
    {
        this.dims = new int[] {0};
    }

    public PodMultiDimArray(int[] dims, Object pod_array)
    {
        this.dims = dims;
        this.pod_array = pod_array;
    }

    public int[] dims;

    public Object pod_array;

    public void retrieveSubArray(int[] memorypos, PodMultiDimArray buffer, int[] bufferpos, int[] count)
    {
        // cSpell: ignore mema, memb
        PodMultiDimArray mema = this;
        PodMultiDimArray memb = buffer;

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
            System.arraycopy(mema.pod_array, (int)indexa[0], memb.pod_array, (int)indexb[0], (int)len[0]);
        }
    }

    public void assignSubArray(int[] memorypos, PodMultiDimArray buffer, int[] bufferpos, int[] count)
    {
        PodMultiDimArray mema = this;
        PodMultiDimArray memb = buffer;

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
            DataTypeUtil.arraycopy(memb.pod_array, (int)indexb[0], mema.pod_array, (int)indexa[0], (int)len[0]);
        }
    }
}
