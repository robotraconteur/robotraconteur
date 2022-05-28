package com.robotraconteur.javatest;

import com.robotraconteur.*;
import java.io.*;
import java.nio.*;
import java.math.*;

public class LFSRSeqGen
{
    public long lfsr;

    public LFSRSeqGen(long seed)
    {
        lfsr = seed;
    }

    public LFSRSeqGen(long seed, String key)
    {
        if (key == null)
        {
            lfsr = seed;
        }
        else
        {
            lfsr = initKey(seed, key);
        }
    }

    // static BigInteger clip_uint32(BigInteger x)
    // {
    //     return x.and(BigInteger.valueOf(0xFFFFFFFF));
    // }

    static long clip_uint32(long x)
    {
        return x & 0xFFFFFFFFl;
    }

    public static long lfsr_next(long lfsr)
    {
        long bit = ((lfsr >> 0) ^ (lfsr >> 1) ^ (lfsr >> 21) ^ (lfsr >> 31)) & 1;
        return clip_uint32((lfsr >> 1) | (bit << 31));
    }

    public long lfsr_next()
    {
        long lfsr2 = lfsr_next(lfsr);
        lfsr = lfsr2;
        return lfsr2;
    }

    public ByteBuffer lfsr_next_bytes()
    {
        return ByteBuffer.allocate(8).order(ByteOrder.LITTLE_ENDIAN).putLong(lfsr_next());
    }

    public ByteBuffer lfsr_next_bytes2()
    {
        byte[] b = lfsr_next2().toByteArray();
        byte[] b1 = new byte[b.length];
        for (int i = 0; i < b.length; i++)
        {
            b1[i] = b[b.length - i - 1];
        }

        return ByteBuffer.allocate(8).order(ByteOrder.LITTLE_ENDIAN).put(b1, 0, 8);
    }

    public BigInteger lfsr_next2()
    {
        BigInteger lfsr2 = BigInteger.valueOf(lfsr_next());
        BigInteger lfsr3 = BigInteger.valueOf(lfsr_next());
        BigInteger val_out1 = (lfsr2.shiftLeft(32)).or(lfsr3).and(BigInteger.valueOf(0xFFFFFFFF));
        return val_out1;
    }

    private long initKey(long seed, String key)
    {
        // Run the LFSR a few times on the seed to shuffle
        for (int j = 0; j < 128; j++)
        {
            seed = lfsr_next(seed);
        }

        int i = 0;
        long hash_ = lfsr_next(seed);
        while (i != key.length())
        {
            hash_ = clip_uint32(hash_ + (int)key.charAt(i++));
            hash_ = clip_uint32(hash_ + clip_uint32(hash_ << 10));
            hash_ = hash_ ^ (clip_uint32(hash_ >> 6));
        }

        System.out.println(hash_);

        hash_ = clip_uint32(hash_ + (clip_uint32(hash_ << 3)));
        hash_ = hash_ ^ (hash_ >> 11);
        hash_ = clip_uint32(hash_ + clip_uint32(hash_ << 15));

        // Shuffle again
        for (int j = 0; j < 128; j++)
        {
            hash_ = lfsr_next(hash_);
        }

        return lfsr_next(hash_);
    }

    public byte nextInt8()
    {
        return lfsr_next_bytes().get(0);
    }

    public UnsignedByte nextUInt8()
    {
        return new UnsignedByte(lfsr_next_bytes().get(0));
    }

    public short nextInt16()
    {
        return lfsr_next_bytes().getShort(0);
    }

    public UnsignedShort nextUInt16()
    {
        return new UnsignedShort(nextInt16());
    }

    public int nextInt32()
    {
        return lfsr_next_bytes().getInt(0);
    }

    public UnsignedInt nextUInt32()
    {
        return new UnsignedInt(nextInt32());
    }

    public long nextInt64()
    {
        return lfsr_next_bytes2().getLong(0);
    }

    public UnsignedLong nextUInt64()
    {
        return new UnsignedLong(nextInt64());
    }

    public float nextFloat()
    {
        return lfsr_next_bytes().getFloat(0);
    }

    public double nextDouble()
    {
        return lfsr_next_bytes2().getDouble(0);
    }

    public boolean nextBool()
    {
        return (lfsr_next_bytes().get(0) & 1) != 0;
    }

    public CSingle nextCFloat()
    {
        return new CSingle(nextFloat(), nextFloat());
    }

    public CDouble nextCDouble()
    {
        return new CDouble(nextDouble(), nextDouble());
    }

    public char nextChar()
    {
        String chars =
            " !\"#$%&\'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";
        long ind = lfsr_next() % chars.length();
        return chars.charAt((int)ind);
    }

    public int nextLen(int max_len)
    {
        long lfsr2 = lfsr_next();
        if (max_len < 32)
        {
            return (int)(lfsr2 % max_len);
        }
        else
        {
            return (int)(8 + (lfsr2 % (max_len - 8)));
        }
    }

    public int nextDist(int min_, int max_)
    {
        RRAssert.isTrue(max_ > min_);
        long lfsr2 = lfsr_next();
        return (int)((lfsr2 % ((max_ - min_) + 1)) + min_);
    }

    public byte[] nextInt8Array(int len)
    {
        byte[] ret = new byte[len];
        for (int i = 0; i < len; i++)
        {
            ret[i] = nextInt8();
        }
        return ret;
    }

    public UnsignedBytes nextUInt8Array(int len)
    {
        byte[] ret = new byte[len];
        for (int i = 0; i < len; i++)
        {
            ret[i] = nextInt8();
        }
        return new UnsignedBytes(ret);
    }

    public short[] nextInt16Array(int len)
    {
        short[] ret = new short[len];
        for (int i = 0; i < len; i++)
        {
            ret[i] = nextInt16();
        }
        return ret;
    }

    public UnsignedShorts nextUInt16Array(int len)
    {
        short[] ret = new short[len];
        for (int i = 0; i < len; i++)
        {
            ret[i] = nextInt16();
        }
        return new UnsignedShorts(ret);
    }

    public int[] nextInt32Array(int len)
    {
        int[] ret = new int[len];
        for (int i = 0; i < len; i++)
        {
            ret[i] = nextInt32();
        }
        return ret;
    }

    public UnsignedInts nextUInt32Array(int len)
    {
        int[] ret = new int[len];
        for (int i = 0; i < len; i++)
        {
            ret[i] = nextInt32();
        }
        return new UnsignedInts(ret);
    }

    public long[] nextInt64Array(int len)
    {
        long[] ret = new long[len];
        for (int i = 0; i < len; i++)
        {
            ret[i] = nextInt64();
        }
        return ret;
    }

    public UnsignedLongs nextUInt64Array(int len)
    {
        long[] ret = new long[len];
        for (int i = 0; i < len; i++)
        {
            ret[i] = nextInt64();
        }
        return new UnsignedLongs(ret);
    }

    public float[] nextFloatArray(int len)
    {
        float[] ret = new float[len];
        for (int i = 0; i < len; i++)
        {
            ret[i] = nextFloat();
        }
        return ret;
    }

    public double[] nextDoubleArray(int len)
    {
        double[] ret = new double[len];
        for (int i = 0; i < len; i++)
        {
            ret[i] = nextDouble();
        }
        return ret;
    }

    public boolean[] nextBoolArray(int len)
    {
        boolean[] ret = new boolean[len];
        for (int i = 0; i < len; i++)
        {
            ret[i] = nextBool();
        }
        return ret;
    }

    public CSingle[] nextCFloatArray(int len)
    {
        CSingle[] ret = new CSingle[len];
        for (int i = 0; i < len; i++)
        {
            ret[i] = nextCFloat();
        }
        return ret;
    }

    public CDouble[] nextCDoubleArray(int len)
    {
        CDouble[] ret = new CDouble[len];
        for (int i = 0; i < len; i++)
        {
            ret[i] = nextCDouble();
        }
        return ret;
    }

    public String nextString(int len)
    {
        char[] ret1 = new char[len];
        for (int i = 0; i < len; i++)
        {
            ret1[i] = nextChar();
        }
        return new String(ret1);
    }

    public byte[] nextInt8ArrayVarLen(int max_len)
    {
        int len = nextLen(max_len);
        byte[] ret = new byte[len];
        for (int i = 0; i < len; i++)
        {
            ret[i] = nextInt8();
        }
        return ret;
    }

    public UnsignedBytes nextUInt8ArrayVarLen(int max_len)
    {
        int len = nextLen(max_len);
        byte[] ret = new byte[len];
        for (int i = 0; i < len; i++)
        {
            ret[i] = nextInt8();
        }
        return new UnsignedBytes(ret);
    }

    public short[] nextInt16ArrayVarLen(int max_len)
    {
        int len = nextLen(max_len);
        short[] ret = new short[len];
        for (int i = 0; i < len; i++)
        {
            ret[i] = nextInt16();
        }
        return ret;
    }

    public UnsignedShorts nextUInt16ArrayVarLen(int max_len)
    {
        int len = nextLen(max_len);
        short[] ret = new short[len];
        for (int i = 0; i < len; i++)
        {
            ret[i] = nextInt16();
        }
        return new UnsignedShorts(ret);
    }

    public int[] nextInt32ArrayVarLen(int max_len)
    {
        int len = nextLen(max_len);
        int[] ret = new int[len];
        for (int i = 0; i < len; i++)
        {
            ret[i] = nextInt32();
        }
        return ret;
    }

    public UnsignedInts nextUInt32ArrayVarLen(int max_len)
    {
        int len = nextLen(max_len);
        int[] ret = new int[len];
        for (int i = 0; i < len; i++)
        {
            ret[i] = nextInt32();
        }
        return new UnsignedInts(ret);
    }

    public long[] nextInt64ArrayVarLen(int max_len)
    {
        int len = nextLen(max_len);
        long[] ret = new long[len];
        for (int i = 0; i < len; i++)
        {
            ret[i] = nextInt64();
        }
        return ret;
    }

    public UnsignedLongs nextUInt64ArrayVarLen(int max_len)
    {
        int len = nextLen(max_len);
        long[] ret = new long[len];
        for (int i = 0; i < len; i++)
        {
            ret[i] = nextInt64();
        }
        return new UnsignedLongs(ret);
    }

    public float[] nextFloatArrayVarLen(int max_len)
    {
        int len = nextLen(max_len);
        float[] ret = new float[len];
        for (int i = 0; i < len; i++)
        {
            ret[i] = nextFloat();
        }
        return ret;
    }

    public double[] nextDoubleArrayVarLen(int max_len)
    {
        int len = nextLen(max_len);
        double[] ret = new double[len];
        for (int i = 0; i < len; i++)
        {
            ret[i] = nextDouble();
        }
        return ret;
    }

    public boolean[] nextBoolArrayVarLen(int max_len)
    {
        int len = nextLen(max_len);
        boolean[] ret = new boolean[len];
        for (int i = 0; i < len; i++)
        {
            ret[i] = nextBool();
        }
        return ret;
    }

    public CSingle[] nextCFloatArrayVarLen(int max_len)
    {
        int len = nextLen(max_len);
        CSingle[] ret = new CSingle[len];
        for (int i = 0; i < len; i++)
        {
            ret[i] = nextCFloat();
        }
        return ret;
    }

    public CDouble[] nextCDoubleArrayVarLen(int max_len)
    {
        int len = nextLen(max_len);
        CDouble[] ret = new CDouble[len];
        for (int i = 0; i < len; i++)
        {
            ret[i] = nextCDouble();
        }
        return ret;
    }

    public String nextStringVarLen(int max_len)
    {
        int len = nextLen(max_len);
        char[] ret1 = new char[len];
        for (int i = 0; i < len; i++)
        {
            ret1[i] = nextChar();
        }
        return new String(ret1);
    }
}
