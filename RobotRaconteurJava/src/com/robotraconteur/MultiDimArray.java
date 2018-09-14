package com.robotraconteur;






/** 
 Represents an unpacked MultiDimArray.  This class stores
 the real and complex data as column vectors that is 
 reshaped based on Dims.
*/
public class MultiDimArray
{

	public MultiDimArray()
	{
	}


	public MultiDimArray(int[] Dims, Object Real)
	{
		this(Dims, Real, null);
	}

	public MultiDimArray(int[] Dims, Object Real, Object Imag)
	{
		this.dimCount = Dims.length;
		this.dims = Dims;
		this.real = Real;

		if (Imag != null)
		{
			this.complex = true;
			this.imag = Imag;
		}

	}

	/** 
	 The number of array dimensions
	*/
	public int dimCount;
	/** 
	 The dimensions of the array
	*/
	public int[] dims;
	/** 
	 The column vector representation of real values
	*/
	public Object real;

	/** 
	 True if this array is complex, false if it is real
	*/
	public boolean complex = false;
	/** 
	 The column vector representation of the imaginary values
	*/
	public Object imag;

	
	public void retrieveSubArray(int[] memorypos, MultiDimArray buffer, int[] bufferpos, int[] count)
	{

		MultiDimArray mema = this;
		MultiDimArray memb = buffer;

		if (mema.complex != memb.complex)
		{
			throw new UnsupportedOperationException("Complex mismatch");
		}
				
		MultiDimArray_CalculateCopyIndicesIter iter=RobotRaconteurJava.multiDimArray_CalculateCopyIndicesBeginIter(mema.dimCount, new vectorint32(mema.dims), new vectorint32(memorypos), memb.dimCount, new vectorint32(memb.dims), new vectorint32(bufferpos), new vectorint32(count));

		int[] len = new int[1];
		int[] indexa=new int[1];
		int[] indexb=new int[1];
				
		while (iter.next(indexa, indexb, len))
		{
			DataTypeUtil.arraycopy(mema.real, (int)indexa[0], memb.real, (int)indexb[0], (int)len[0]);
			if (mema.complex)
			{
				DataTypeUtil.arraycopy(mema.imag, (int)indexa[0], memb.imag, (int)indexb[0], (int)len[0]);
			}
		}
	}

	public void assignSubArray(int[] memorypos, MultiDimArray buffer, int[] bufferpos, int[] count)
	{


		MultiDimArray mema = this;
		MultiDimArray memb = buffer;

		if (mema.complex != memb.complex)
		{
			throw new UnsupportedOperationException("Complex mismatch");
		}

		MultiDimArray_CalculateCopyIndicesIter iter=RobotRaconteurJava.multiDimArray_CalculateCopyIndicesBeginIter(mema.dimCount, new vectorint32(mema.dims), new vectorint32(memorypos), memb.dimCount, new vectorint32(memb.dims), new vectorint32(bufferpos), new vectorint32(count));

		int[] len = new int[1];
		int[] indexa=new int[1];
		int[] indexb=new int[1];
				
		while (iter.next(indexa, indexb, len))
		{
			DataTypeUtil.arraycopy(memb.real, (int)indexb[0], mema.real, (int)indexa[0], (int)len[0]);
			if (mema.complex)
			{
				DataTypeUtil.arraycopy(memb.imag, (int)indexb[0], mema.imag, (int)indexa[0], (int)len[0]);
			}
		}
	}

}