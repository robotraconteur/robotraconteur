package com.robotraconteur;

import java.lang.reflect.Array;
import java.math.BigInteger;

public class PodArrayMemoryClient<T> extends PodArrayMemory<T>
{

    WrappedPodArrayMemoryClient innerclient;

    class bufferdirector extends WrappedPodArrayMemoryClientBuffer
    {
        T buffer;

        public bufferdirector(T buffer)
        {
            this.buffer = buffer;
        }

        @Override
        public void unpackReadResult(MessageElementNestedElementList res, java.math.BigInteger bufferpos,
                                     java.math.BigInteger count)
        {
            T res1 = (T)RobotRaconteurNode.s().unpackStructureDispose(res);
            System.arraycopy(res1, 0, buffer, bufferpos.intValue(), count.intValue());
        }

        @Override
        public MessageElementNestedElementList packWriteRequest(java.math.BigInteger bufferpos,
                                                                java.math.BigInteger count)
        {
            T buffer3;
            if ((long)Array.getLength(buffer) == count.longValue())
            {
                buffer3 = buffer;
            }
            else
            {
                buffer3 = (T)Array.newInstance(buffer.getClass().getComponentType(), count.intValue());
                System.arraycopy(buffer, bufferpos.intValue(), buffer3, 0, count.intValue());
            }
            return (MessageElementNestedElementList)RobotRaconteurNode.s().packStructure(buffer3);
        }
    }

    public PodArrayMemoryClient(WrappedPodArrayMemoryClient innerclient)
    {
        this.innerclient = innerclient;
    }

    @Override public void attach(T memory)
    {
        throw new IllegalStateException("Invalid for memory client");
    }

    MemberDefinition_Direction direction()
    {
        return innerclient.direction();
    }

    @Override public long length()
    {
        return innerclient.length().longValue();
    }

    @Override public void read(long memorypos, T buffer, long bufferpos, long count)
    {
        bufferdirector buffer1 = new bufferdirector(buffer);
        try
        {
            innerclient.read(BigInteger.valueOf(memorypos), buffer1, BigInteger.valueOf(bufferpos),
                             BigInteger.valueOf(count));
        }
        finally
        {
            buffer1.finalize();
        }
    }

    @Override public void write(long memorypos, T buffer, long bufferpos, long count)
    {
        bufferdirector buffer1 = new bufferdirector(buffer);
        try
        {
            innerclient.write(BigInteger.valueOf(memorypos), buffer1, BigInteger.valueOf(bufferpos),
                              BigInteger.valueOf(count));
        }
        finally
        {
            buffer1.finalize();
        }
    }
}
