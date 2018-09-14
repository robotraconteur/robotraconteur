package com.robotraconteur;

import java.math.BigInteger;

public class MultiDimArrayMemoryClient<T> extends MultiDimArrayMemory<T>
{
	private MultiDimArrayMemoryBase innermem;
	public MultiDimArrayMemoryClient(MultiDimArrayMemoryBase innermem)
	{
		this.innermem = innermem;
	}

	@Override
	public long dimCount()
	{
		
		return innermem.dimCount().longValue();
	}

	@Override
	public long[] dimensions()
	{
//C# TO JAVA CONVERTER WARNING: Unsigned integer types have no direct equivalent in Java:
//ORIGINAL LINE: return innermem.Dimensions().Select(x=>(ulong)x).ToArray();
		vector_uint64_t i1=innermem.dimensions();
		int dimcount=(int)i1.size();
		long[] o=new long[dimcount];
		for (int i=0; i<o.length; i++)
		{
			o[i]=i1.get(i).longValue();
			
		}
		return o;
	}

	@Override
	public boolean complex()
	{
		return innermem.complex();
	}

//C# TO JAVA CONVERTER WARNING: Unsigned integer types have no direct equivalent in Java:
//ORIGINAL LINE: public override void Read(ulong[] memorypos, MultiDimArray buffer, ulong[] bufferpos, ulong[] count)
	@Override
	public void read(long[] memorypos, MultiDimArray buffer, long[] bufferpos, long[] count)
	{
		RRMultiDimArrayUntyped dat=null;
		RRBaseArray datDims=null;
		RRBaseArray datReal=null;
		RRBaseArray datImag=null;
		try
		{
		vector_uint64_t memorypos2 = new vector_uint64_t();
//C# TO JAVA CONVERTER WARNING: Unsigned integer types have no direct equivalent in Java:
//ORIGINAL LINE: foreach (ulong val in memorypos)
		for (long val : memorypos)
		{
			memorypos2.add(BigInteger.valueOf(val));
		}
		vector_uint64_t count2 = new vector_uint64_t();
//C# TO JAVA CONVERTER WARNING: Unsigned integer types have no direct equivalent in Java:
//ORIGINAL LINE: foreach (ulong val in count)
		for (long val : count)
		{
			count2.add(BigInteger.valueOf(val));
		}

		dat = WrappedMultiDimArrayMemoryClientUtil.read(innermem, memorypos2, count2);
		datDims=dat.getDims();
		int[] dims = (int[])MessageElementDataUtil.rRBaseArrayToArray(datDims);
		datReal=dat.getReal();
		T real = (T)MessageElementDataUtil.rRBaseArrayToArray(datReal);
		datImag=dat.getImag();
		T imag = (T)MessageElementDataUtil.rRBaseArrayToArray(datImag);

		
		int dimcount=bufferpos.length;
		
		int[] bufferpos3=new int[dimcount];
		int[] count3=new int[dimcount];
		
		for (int i=0; i<dimcount; i++)
		{
			
			bufferpos3[i]=(int)bufferpos[i];
			count3[i]=(int)count[i];
		
		}
		
		MultiDimArray dat2 = new MultiDimArray(dims, real, imag);
		buffer.assignSubArray(bufferpos3, dat2, new int[count.length], count3);

		}
		finally
		{
			if (dat != null)
            {
                if (datDims != null) datDims.delete();
                if (datReal != null) datReal.delete();
                if (datImag != null) datImag.delete();
                dat.delete();
            }
			
			
		}


	}



	@Override
	public void write(long[] memorypos, MultiDimArray buffer, long[] bufferpos, long[] count)
	{
		RRMultiDimArrayUntyped dat2=null;
		RRBaseArray dat2Dims=null;
		RRBaseArray dat2Real=null;
		RRBaseArray dat2Imag=null;
		
		try
		{
		int elemcount = 1;


		for (long v : count)
		{
			elemcount *= (int)v;
		}
		
		int dimcount=count.length;
		int[] count2 = new int[dimcount];
		int[] bufferpos2 = new int[dimcount];
		
		for (int i=0; i<dimcount; i++)
		{
			
			bufferpos2[i]=(int)bufferpos[i];
			count2[i]=(int)count[i];
		
		}
		
		T real = (T)DataTypeUtil.arrayFromDataType(innermem.elementTypeID(),elemcount);
		T imag = null;
		if (complex())
		{
			imag = (T)DataTypeUtil.arrayFromDataType(innermem.elementTypeID(),elemcount);
		}

		MultiDimArray writedat1 = new MultiDimArray(count2, real, imag);
		writedat1.assignSubArray(new int[count.length], buffer, bufferpos2, count2);

		dat2 = new RRMultiDimArrayUntyped();
		dat2.setDimCount(count2.length);
		dat2Dims=MessageElementDataUtil.arrayToRRBaseArray(count2);
		dat2.setDims(dat2Dims);
		dat2Real=MessageElementDataUtil.arrayToRRBaseArray(real);
		dat2.setReal(dat2Real);
		dat2.setComplex(false);
		if (imag != null)
		{
			dat2.setComplex(true);
			dat2Imag=MessageElementDataUtil.arrayToRRBaseArray(imag);
			dat2.setImag(dat2Imag);
		}

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
                if (dat2Dims != null) dat2Dims.delete();
                if (dat2Real != null) dat2Real.delete();
                if (dat2Imag != null) dat2Imag.delete();
                dat2.delete();
            }
		
		}
	}



}