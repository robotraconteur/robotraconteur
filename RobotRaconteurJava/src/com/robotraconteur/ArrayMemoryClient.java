package com.robotraconteur;
import java.lang.*;
import java.math.*;

public class ArrayMemoryClient<T> extends ArrayMemory<T>
{
    private ArrayMemoryBase innerarray;

    public ArrayMemoryClient(ArrayMemoryBase innerarray)
    {
        this.innerarray = innerarray;
    }

    @Override public long length()
    {
        return innerarray.length().longValue();
    }

    // C# TO JAVA CONVERTER WARNING: Unsigned integer types have no direct equivalent in Java:
    // ORIGINAL LINE: public override void Read(ulong memorypos, T[] buffer, ulong bufferpos, ulong count)
    @Override public void read(long memorypos, T buffer, long bufferpos, long count)
    {
        RRBaseArray dat = null;
        try
        {
            dat =
                WrappedArrayMemoryClientUtil.read(innerarray, BigInteger.valueOf(memorypos), BigInteger.valueOf(count));
            T dat2 = (T)MessageElementDataUtil.rRBaseArrayToArray(dat);
            DataTypeUtil.arraycopy(dat2, 0, buffer, (int)bufferpos, (int)count);
        }
        finally
        {
            if (dat != null)
                dat.delete();
        }
    }

    // C# TO JAVA CONVERTER WARNING: Unsigned integer types have no direct equivalent in Java:
    // ORIGINAL LINE: public override void Write(ulong memorypos, T[] buffer, ulong bufferpos, ulong count)
    @Override public void write(long memorypos, T buffer, long bufferpos, long count)
    {
        T buffer3;
        // C# TO JAVA CONVERTER WARNING: Unsigned integer types have no direct equivalent in Java:
        // ORIGINAL LINE: if ((ulong)buffer.Length == count)
        if ((long)DataTypeUtil.getArrayLength(buffer) == count)
        {
            buffer3 = buffer;
        }
        else
        {

            buffer3 = (T)DataTypeUtil.arrayFromDataType(innerarray.elementTypeID(), (int)count);
            DataTypeUtil.arraycopy(buffer, (int)bufferpos, buffer3, 0, (int)count);
        }

        RRBaseArray buffer2 = null;
        try
        {
            buffer2 = MessageElementDataUtil.arrayToRRBaseArray(buffer3);
            WrappedArrayMemoryClientUtil.write(innerarray, BigInteger.valueOf(memorypos), buffer2,
                                               BigInteger.valueOf(0), BigInteger.valueOf(count));
        }
        finally
        {
            if (buffer2 != null)
                buffer2.delete();
        }
    }
}