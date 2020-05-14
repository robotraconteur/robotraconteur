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
			RRDirectorExceptionHelper.setError(merr,RRDirectorExceptionHelper.exceptionToStackTraceString(e));
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
			RRDirectorExceptionHelper.setError(merr,RRDirectorExceptionHelper.exceptionToStackTraceString(e));
			merr.delete();
			return new vector_uint64_t();
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
             pbufferReal = pbuffer.getArray();
			
			T real = (T)DataTypeUtil.arrayFromDataType(pbufferReal.getTypeID(), (int)elemcount);
			

			MultiDimArray m = new MultiDimArray(count2, real);

			mem.read(memorypos, m, bufferpos, count);
			MessageElementDataUtil.arrayToRRBaseArray(real, pbufferReal);			
		}
		catch (Exception e)
		{
			MessageEntry merr = new MessageEntry();
			RobotRaconteurExceptionUtil.exceptionToMessageEntry(e, merr);
			RRDirectorExceptionHelper.setError(merr,RRDirectorExceptionHelper.exceptionToStackTraceString(e));
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
            pbufferReal = pbuffer.getArray();            
			
			
			int[] dims = ((UnsignedInts)MessageElementDataUtil.rRBaseArrayToArray(pbufferDims)).value;
			T real = (T)MessageElementDataUtil.rRBaseArrayToArray(pbufferReal);
			T imag = null;
			
			MultiDimArray m = new MultiDimArray(count2, real);

			mem.write(memorypos, m, bufferpos, count);


		}
		catch (Exception e)
		{
			MessageEntry merr = new MessageEntry();
			RobotRaconteurExceptionUtil.exceptionToMessageEntry(e, merr);
			RRDirectorExceptionHelper.setError(merr,RRDirectorExceptionHelper.exceptionToStackTraceString(e));
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