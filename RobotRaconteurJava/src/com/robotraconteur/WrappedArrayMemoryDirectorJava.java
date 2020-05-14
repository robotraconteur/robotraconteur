package com.robotraconteur;

import java.math.BigInteger;

public class WrappedArrayMemoryDirectorJava<T> extends WrappedArrayMemoryDirector
{
	private ArrayMemory<T> mem;

	public int memoryid = 0;

	public WrappedArrayMemoryDirectorJava(ArrayMemory<T> mem)
	{
		this.mem = mem;
		this.setObjectheapid(RRObjectHeap.addObject(this)); 
	}

	public final void disown()
	{
		swigCMemOwn = false;
	}

	@Override
	public BigInteger length()
	{
		try
		{
			return BigInteger.valueOf(mem.length());
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
	public void read(BigInteger memorypos, RRBaseArray buffer2, BigInteger bufferpos, BigInteger count)
	{
		try
		{
			
			try
			{
			T buffer3 = (T)DataTypeUtil.arrayFromDataType(buffer2.getTypeID(),count.intValue());
			mem.read(memorypos.longValue(), buffer3, 0, count.longValue());
			MessageElementDataUtil.arrayToRRBaseArray(buffer3, buffer2);
			}
			finally
			{
				if (buffer2!=null) buffer2.delete();
				
			}
		}
		catch (Exception e)
		{
			MessageEntry merr = new MessageEntry();
			RobotRaconteurExceptionUtil.exceptionToMessageEntry(e, merr);
			RRDirectorExceptionHelper.setError(merr,RRDirectorExceptionHelper.exceptionToStackTraceString(e));
			merr.delete();

		}
	}

	@Override
	public void write(BigInteger memorypos, RRBaseArray ra, BigInteger bufferpos, BigInteger count)
	{		
		try
		{			
			T buffer2 = (T)MessageElementDataUtil.rRBaseArrayToArray(ra);
			mem.write(memorypos.longValue(), buffer2, bufferpos.longValue(), count.longValue());
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
			if (ra!=null) ra.delete();
		}
	}

	

}