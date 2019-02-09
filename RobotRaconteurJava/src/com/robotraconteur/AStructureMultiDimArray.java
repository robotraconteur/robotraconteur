package com.robotraconteur;


public class AStructureMultiDimArray
{
	
	public AStructureMultiDimArray()
    {
        this.dims = new int[] { 0 };
    }
	
	public AStructureMultiDimArray(int[] dims, Object astruct_array)
	{		
		this.dims = dims;
		this.astruct_array = astruct_array;
	}
	
	public int[] dims;
	
	public Object astruct_array;
	
	public void retrieveSubArray(int[] memorypos, AStructureMultiDimArray buffer, int[] bufferpos, int[] count)
	{
		AStructureMultiDimArray mema = this;
		AStructureMultiDimArray memb = buffer;
						
		MultiDimArray_CalculateCopyIndicesIter iter=RobotRaconteurJava.multiDimArray_CalculateCopyIndicesBeginIter(mema.dims.length, new vectorint32(mema.dims), new vectorint32(memorypos), memb.dims.length, new vectorint32(memb.dims), new vectorint32(bufferpos), new vectorint32(count));

		int[] len = new int[1];
		int[] indexa=new int[1];
		int[] indexb=new int[1];
				
		while (iter.next(indexa, indexb, len))
		{
			System.arraycopy(mema.astruct_array, (int)indexa[0], memb.astruct_array, (int)indexb[0], (int)len[0]);			
		}
	}

	public void assignSubArray(int[] memorypos, AStructureMultiDimArray buffer, int[] bufferpos, int[] count)
	{
		AStructureMultiDimArray mema = this;
		AStructureMultiDimArray memb = buffer;
		
		MultiDimArray_CalculateCopyIndicesIter iter=RobotRaconteurJava.multiDimArray_CalculateCopyIndicesBeginIter(mema.dims.length, new vectorint32(mema.dims), new vectorint32(memorypos), memb.dims.length, new vectorint32(memb.dims), new vectorint32(bufferpos), new vectorint32(count));

		int[] len = new int[1];
		int[] indexa=new int[1];
		int[] indexb=new int[1];
				
		while (iter.next(indexa, indexb, len))
		{
			DataTypeUtil.arraycopy(memb.astruct_array, (int)indexb[0], mema.astruct_array, (int)indexa[0], (int)len[0]);			
		}
	}

}