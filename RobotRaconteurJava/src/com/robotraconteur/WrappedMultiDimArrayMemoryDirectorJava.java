package com.robotraconteur;
import java.math.*;

public class WrappedMultiDimArrayMemoryDirectorJava<T> extends WrappedMultiDimArrayMemoryDirector
{
	private MultiDimArrayMemory<T> mem;

	public int memoryid = 0;

	public WrappedMultiDimArrayMemoryDirectorJava(MultiDimArrayMemory<T> mem)
	{
		this.mem = mem;
		this.setObjectheapid(RRObjectHeap.addObject(this));
	}

	public final void disown()
	{
		swigCMemOwn = false;
	}

	@Override
	public BigInteger dimCount()
	{
		try
		{
			return BigInteger.valueOf(mem.dimCount());
		}
		catch (Exception e)
		{
			MessageEntry merr = new MessageEntry();
			RobotRaconteurExceptionUtil.exceptionToMessageEntry(e, merr);
			RRDirectorExceptionHelper.setError(merr);
			merr.delete();
			return BigInteger.valueOf(0);
		}
	}

	@Override
	public vector_uint64_t dimensions()
	{
		try
		{
			vector_uint64_t o = new vector_uint64_t();
//C# TO JAVA CONVERTER WARNING: Unsigned integer types have no direct equivalent in Java:
//ORIGINAL LINE: foreach (ulong i in mem.Dimensions)
			for (long i : mem.dimensions())
			{
				o.add(BigInteger.valueOf(i));
			}
			return o;
		}
		catch (Exception e)
		{
			MessageEntry merr = new MessageEntry();
			RobotRaconteurExceptionUtil.exceptionToMessageEntry(e, merr);
			RRDirectorExceptionHelper.setError(merr);
			merr.delete();
			return new vector_uint64_t();
		}
	}

	@Override
	public boolean complex()
	{
		try
		{
			return mem.complex();
		}
		catch (Exception e)
		{
			MessageEntry merr = new MessageEntry();
			RobotRaconteurExceptionUtil.exceptionToMessageEntry(e, merr);
			RRDirectorExceptionHelper.setError(merr);
			merr.delete();
			return false;
		}
	}

	@Override
	public void read(WrappedMultiDimArrayMemoryParams p)
	{
		RRMultiDimArrayUntyped pbuffer=null;
        RRBaseArray pbufferDims = null;
        RRBaseArray pbufferReal = null;
        RRBaseArray pbufferImag = null;
		try
		{
			int dimcount=(int)p.getCount().size();

			long[] count = new long[dimcount];
			int[] count2=new int[dimcount];

			long[] bufferpos = new long[dimcount];

			long[] memorypos = new long[dimcount];

			vector_uint64_t pcount=p.getCount();
			vector_uint64_t pbufferpos=p.getBufferpos();
			vector_uint64_t pmemorypos=p.getMemorypos();
			for (int i=0; i<dimcount; i++)
			{
				count[i]=pcount.get(i).longValue();
				bufferpos[i]=pbufferpos.get(i).longValue();
				memorypos[i]=pmemorypos.get(i).longValue();
				count2[i]=(int)count[i];
			}
			

			long elemcount = 1;

			for (long e : count)
			{
				elemcount *= e;
			}
			
			 pbuffer = p.getBuffer();
             pbufferDims = pbuffer.getDims();
             pbufferReal = pbuffer.getReal();
             pbufferImag = pbuffer.getImag();

			if (mem.complex() != p.getBuffer().getComplex())
			{
				throw new UnsupportedOperationException("Complex mismatch");
			}
			T real = (T)DataTypeUtil.arrayFromDataType(pbufferReal.getTypeID(), (int)elemcount);
			T imag = null;
			if (mem.complex())
			{
				imag = (T)DataTypeUtil.arrayFromDataType(pbufferImag.getTypeID(), (int)elemcount);
			}

			MultiDimArray m = new MultiDimArray(count2, real, imag);

			mem.read(memorypos, m, bufferpos, count);
			MessageElementDataUtil.arrayToRRBaseArray(real, pbufferReal);
			if (imag != null)
			{
				MessageElementDataUtil.arrayToRRBaseArray(imag, pbufferImag);
			}
		}
		catch (Exception e)
		{
			MessageEntry merr = new MessageEntry();
			RobotRaconteurExceptionUtil.exceptionToMessageEntry(e, merr);
			RRDirectorExceptionHelper.setError(merr);
			merr.delete();

		}
		finally
		{
			if (p!=null && pbuffer!=null)
            {
                if (pbufferDims != null) pbufferDims.delete();
                if (pbufferReal != null) pbufferReal.delete();
                if (pbufferImag != null) pbufferImag.delete();
            }
            if (p!=null)
            {
                p.delete();
            }
			
		}
	}

	@Override
	public void write(WrappedMultiDimArrayMemoryParams p)
	{
		RRMultiDimArrayUntyped pbuffer=null;
        RRBaseArray pbufferDims = null;
        RRBaseArray pbufferReal = null;
        RRBaseArray pbufferImag = null;
		try
		{
			int dimcount=(int)p.getCount().size();

			long[] count = new long[dimcount];
			int[] count2=new int[dimcount];

			long[] bufferpos = new long[dimcount];

			long[] memorypos = new long[dimcount];

			vector_uint64_t pcount=p.getCount();
			vector_uint64_t pbufferpos=p.getBufferpos();
			vector_uint64_t pmemorypos=p.getMemorypos();
			for (int i=0; i<dimcount; i++)
			{
				count[i]=pcount.get(i).longValue();
				bufferpos[i]=pbufferpos.get(i).longValue();
				memorypos[i]=pmemorypos.get(i).longValue();
				count2[i]=(int)count[i];
			}
			

			long elemcount = 1;
			for (long e : count)
			{
				elemcount *= e;
			}

			pbuffer = p.getBuffer();
            pbufferDims = pbuffer.getDims();
            pbufferReal = pbuffer.getReal();
            pbufferImag = pbuffer.getImag();
			
			if (mem.complex() != pbuffer.getComplex())
			{
				throw new UnsupportedOperationException("Complex mismatch");
			}

			int[] dims = (int[])MessageElementDataUtil.rRBaseArrayToArray(pbufferDims);
			T real = (T)MessageElementDataUtil.rRBaseArrayToArray(pbufferReal);
			T imag = null;
			if (p.getBuffer().getComplex())
			{
				imag = (T)MessageElementDataUtil.rRBaseArrayToArray(pbufferImag);
			}

			MultiDimArray m = new MultiDimArray(count2, real, imag);

			mem.write(memorypos, m, bufferpos, count);


		}
		catch (Exception e)
		{
			MessageEntry merr = new MessageEntry();
			RobotRaconteurExceptionUtil.exceptionToMessageEntry(e, merr);
			RRDirectorExceptionHelper.setError(merr);
			merr.delete();

		}
		finally
		{
			if (p!=null && pbuffer!=null)
            {
                if (pbufferDims != null) pbufferDims.delete();
                if (pbufferReal != null) pbufferReal.delete();
                if (pbufferImag != null) pbufferImag.delete();
            }
            if (p!=null)
            {
                p.delete();
            }
			
		}
	}

	


}