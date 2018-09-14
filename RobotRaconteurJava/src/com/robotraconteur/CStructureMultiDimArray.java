package com.robotraconteur;


public class CStructureMultiDimArray
{
	
	public CStructureMultiDimArray()
    {
        this.dims = new int[] { 0 };
    }
	
	public CStructureMultiDimArray(int[] dims, Object cstruct_array)
	{		
		this.dims = dims;
		this.cstruct_array = cstruct_array;
	}
	
	public int[] dims;
	
	public Object cstruct_array;
	
	public void retrieveSubArray(int[] memorypos, CStructureMultiDimArray buffer, int[] bufferpos, int[] count)
	{
		CStructureMultiDimArray mema = this;
		CStructureMultiDimArray memb = buffer;
						
		MultiDimArray_CalculateCopyIndicesIter iter=RobotRaconteurJava.multiDimArray_CalculateCopyIndicesBeginIter(mema.dims.length, new vectorint32(mema.dims), new vectorint32(memorypos), memb.dims.length, new vectorint32(memb.dims), new vectorint32(bufferpos), new vectorint32(count));

		int[] len = new int[1];
		int[] indexa=new int[1];
		int[] indexb=new int[1];
				
		while (iter.next(indexa, indexb, len))
		{
			System.arraycopy(mema.cstruct_array, (int)indexa[0], memb.cstruct_array, (int)indexb[0], (int)len[0]);			
		}
	}

	public void assignSubArray(int[] memorypos, CStructureMultiDimArray buffer, int[] bufferpos, int[] count)
	{
		CStructureMultiDimArray mema = this;
		CStructureMultiDimArray memb = buffer;
		
		MultiDimArray_CalculateCopyIndicesIter iter=RobotRaconteurJava.multiDimArray_CalculateCopyIndicesBeginIter(mema.dims.length, new vectorint32(mema.dims), new vectorint32(memorypos), memb.dims.length, new vectorint32(memb.dims), new vectorint32(bufferpos), new vectorint32(count));

		int[] len = new int[1];
		int[] indexa=new int[1];
		int[] indexb=new int[1];
				
		while (iter.next(indexa, indexb, len))
		{
			DataTypeUtil.arraycopy(memb.cstruct_array, (int)indexb[0], mema.cstruct_array, (int)indexa[0], (int)len[0]);			
		}
	}

}