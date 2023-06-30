using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using RobotRaconteur;
using System.IO;
using RobotRaconteur;

namespace RobotRaconteurNETTest
{
public class LFSRSeqGen
{

    public uint lfsr;

    public LFSRSeqGen(uint seed, string key = null)
    {
        if (key == null)
        {
            lfsr = seed;
        }
        else
        {
            lfsr = InitKey(seed, key);
        }
    }

    public static uint lfsr_next(uint lfsr)
    {
        unchecked
        {
            uint bit = ((lfsr >> 0) ^ (lfsr >> 1) ^ (lfsr >> 21) ^ (lfsr >> 31)) & 1;
            return ((lfsr >> 1) | (bit << 31));
        }
    }

    public uint lfsr_next()
    {
        uint lfsr2 = lfsr_next(lfsr);
        lfsr = lfsr2;
        return lfsr2;
    }

    public byte[] lfsr_next_bytes()
    {
        return BitConverter.GetBytes(lfsr_next());
    }

    public byte[] lfsr_next_bytes2()
    {
        return BitConverter.GetBytes(lfsr_next2());
    }

    public ulong lfsr_next2()
    {
        ulong lfsr2 = lfsr_next();
        ulong lfsr3 = lfsr_next();
        ulong val_out1 = (lfsr2 << 32) | lfsr3;
        return val_out1;
    }

    private uint InitKey(uint seed, string key)
    {
        // Run the LFSR a few times on the seed to shuffle
        for (int j = 0; j < 128; j++)
        {
            seed = lfsr_next(seed);
        }

        int i = 0;
        uint hash_ = lfsr_next(seed);
        while (i != key.Length)
        {
            hash_ += key[i++];
            hash_ += (hash_ << 10);
            hash_ ^= (hash_ >> 6);
        }

        hash_ += (hash_ << 3);
        hash_ ^= (hash_ >> 11);
        hash_ += (hash_ << 15);

        // Shuffle again
        for (int j = 0; j < 128; j++)
        {
            hash_ = lfsr_next(hash_);
        }

        return lfsr_next(hash_);
    }

    public sbyte NextInt8()
    {
        return unchecked((sbyte)lfsr_next_bytes()[0]);
    }

    public byte NextUInt8()
    {
        return lfsr_next_bytes()[0];
    }

    public short NextInt16()
    {
        return BitConverter.ToInt16(lfsr_next_bytes(), 0);
    }

    public ushort NextUInt16()
    {
        return BitConverter.ToUInt16(lfsr_next_bytes(), 0);
    }

    public int NextInt32()
    {
        return BitConverter.ToInt32(lfsr_next_bytes(), 0);
    }

    public uint NextUInt32()
    {
        return BitConverter.ToUInt32(lfsr_next_bytes(), 0);
    }

    public long NextInt64()
    {
        return BitConverter.ToInt64(lfsr_next_bytes2(), 0);
    }

    public ulong NextUInt64()
    {
        return BitConverter.ToUInt64(lfsr_next_bytes2(), 0);
    }

    public float NextFloat()
    {
        return BitConverter.ToSingle(lfsr_next_bytes(), 0);
    }

    public double NextDouble()
    {
        return BitConverter.ToDouble(lfsr_next_bytes2(), 0);
    }

    public bool NextBool()
    {
        return (lfsr_next_bytes()[0] & 1) != 0;
    }

    public CSingle NextCFloat()
    {
        return new CSingle(NextFloat(), NextFloat());
    }

    public CDouble NextCDouble()
    {
        return new CDouble(NextDouble(), NextDouble());
    }

    public char NextChar()
    {
        const string chars =
            " !\"#$%&\'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";
        long ind = lfsr_next() % chars.Length;
        return chars[(int)ind];
    }

    public uint NextLen(uint max_len)
    {
        uint lfsr2 = lfsr_next();
        if (max_len < 32)
        {
            return lfsr2 % max_len;
        }
        else
        {
            return 8 + (lfsr2 % (max_len - 8));
        }
    }

    public uint NextDist(uint min_, uint max_)
    {
        RRAssert.IsTrue(max_ > min_);
        uint lfsr2 = lfsr_next();
        return (lfsr2 % ((max_ - min_) + 1)) + min_;
    }

    public sbyte[] NextInt8Array(uint len)
    {
        sbyte[] ret = new sbyte[len];
        for (int i = 0; i < len; i++)
        {
            ret[i] = NextInt8();
        }
        return ret;
    }

    public byte[] NextUInt8Array(uint len)
    {
        byte[] ret = new byte[len];
        for (int i = 0; i < len; i++)
        {
            ret[i] = NextUInt8();
        }
        return ret;
    }

    public short[] NextInt16Array(uint len)
    {
        short[] ret = new short[len];
        for (int i = 0; i < len; i++)
        {
            ret[i] = NextInt16();
        }
        return ret;
    }

    public ushort[] NextUInt16Array(uint len)
    {
        ushort[] ret = new ushort[len];
        for (int i = 0; i < len; i++)
        {
            ret[i] = NextUInt16();
        }
        return ret;
    }

    public int[] NextInt32Array(uint len)
    {
        int[] ret = new int[len];
        for (int i = 0; i < len; i++)
        {
            ret[i] = NextInt32();
        }
        return ret;
    }

    public uint[] NextUInt32Array(uint len)
    {
        uint[] ret = new uint[len];
        for (int i = 0; i < len; i++)
        {
            ret[i] = NextUInt32();
        }
        return ret;
    }

    public long[] NextInt64Array(uint len)
    {
        long[] ret = new long[len];
        for (int i = 0; i < len; i++)
        {
            ret[i] = NextInt64();
        }
        return ret;
    }

    public ulong[] NextUInt64Array(uint len)
    {
        ulong[] ret = new ulong[len];
        for (int i = 0; i < len; i++)
        {
            ret[i] = NextUInt64();
        }
        return ret;
    }

    public float[] NextFloatArray(uint len)
    {
        float[] ret = new float[len];
        for (int i = 0; i < len; i++)
        {
            ret[i] = NextFloat();
        }
        return ret;
    }

    public double[] NextDoubleArray(uint len)
    {
        double[] ret = new double[len];
        for (int i = 0; i < len; i++)
        {
            ret[i] = NextDouble();
        }
        return ret;
    }

    public bool[] NextBoolArray(uint len)
    {
        bool[] ret = new bool[len];
        for (int i = 0; i < len; i++)
        {
            ret[i] = NextBool();
        }
        return ret;
    }

    public CSingle[] NextCFloatArray(uint len)
    {
        CSingle[] ret = new CSingle[len];
        for (int i = 0; i < len; i++)
        {
            ret[i] = NextCFloat();
        }
        return ret;
    }

    public CDouble[] NextCDoubleArray(uint len)
    {
        CDouble[] ret = new CDouble[len];
        for (int i = 0; i < len; i++)
        {
            ret[i] = NextCDouble();
        }
        return ret;
    }

    public string NextString(uint len)
    {
        char[] ret1 = new char[len];
        for (int i = 0; i < len; i++)
        {
            ret1[i] = NextChar();
        }
        return new string(ret1);
    }

    public sbyte[] NextInt8ArrayVarLen(uint max_len)
    {
        uint len = NextLen(max_len);
        sbyte[] ret = new sbyte[len];
        for (int i = 0; i < len; i++)
        {
            ret[i] = NextInt8();
        }
        return ret;
    }

    public byte[] NextUInt8ArrayVarLen(uint max_len)
    {
        uint len = NextLen(max_len);
        byte[] ret = new byte[len];
        for (int i = 0; i < len; i++)
        {
            ret[i] = NextUInt8();
        }
        return ret;
    }

    public short[] NextInt16ArrayVarLen(uint max_len)
    {
        uint len = NextLen(max_len);
        short[] ret = new short[len];
        for (int i = 0; i < len; i++)
        {
            ret[i] = NextInt16();
        }
        return ret;
    }

    public ushort[] NextUInt16ArrayVarLen(uint max_len)
    {
        uint len = NextLen(max_len);
        ushort[] ret = new ushort[len];
        for (int i = 0; i < len; i++)
        {
            ret[i] = NextUInt16();
        }
        return ret;
    }

    public int[] NextInt32ArrayVarLen(uint max_len)
    {
        uint len = NextLen(max_len);
        int[] ret = new int[len];
        for (int i = 0; i < len; i++)
        {
            ret[i] = NextInt32();
        }
        return ret;
    }

    public uint[] NextUInt32ArrayVarLen(uint max_len)
    {
        uint len = NextLen(max_len);
        uint[] ret = new uint[len];
        for (int i = 0; i < len; i++)
        {
            ret[i] = NextUInt32();
        }
        return ret;
    }

    public long[] NextInt64ArrayVarLen(uint max_len)
    {
        uint len = NextLen(max_len);
        long[] ret = new long[len];
        for (int i = 0; i < len; i++)
        {
            ret[i] = NextInt64();
        }
        return ret;
    }

    public ulong[] NextUInt64ArrayVarLen(uint max_len)
    {
        uint len = NextLen(max_len);
        ulong[] ret = new ulong[len];
        for (int i = 0; i < len; i++)
        {
            ret[i] = NextUInt64();
        }
        return ret;
    }

    public float[] NextFloatArrayVarLen(uint max_len)
    {
        uint len = NextLen(max_len);
        float[] ret = new float[len];
        for (int i = 0; i < len; i++)
        {
            ret[i] = NextFloat();
        }
        return ret;
    }

    public double[] NextDoubleArrayVarLen(uint max_len)
    {
        uint len = NextLen(max_len);
        double[] ret = new double[len];
        for (int i = 0; i < len; i++)
        {
            ret[i] = NextDouble();
        }
        return ret;
    }

    public bool[] NextBoolArrayVarLen(uint max_len)
    {
        uint len = NextLen(max_len);
        bool[] ret = new bool[len];
        for (int i = 0; i < len; i++)
        {
            ret[i] = NextBool();
        }
        return ret;
    }

    public CSingle[] NextCFloatArrayVarLen(uint max_len)
    {
        uint len = NextLen(max_len);
        CSingle[] ret = new CSingle[len];
        for (int i = 0; i < len; i++)
        {
            ret[i] = NextCFloat();
        }
        return ret;
    }

    public CDouble[] NextCDoubleArrayVarLen(uint max_len)
    {
        uint len = NextLen(max_len);
        CDouble[] ret = new CDouble[len];
        for (int i = 0; i < len; i++)
        {
            ret[i] = NextCDouble();
        }
        return ret;
    }

    public string NextStringVarLen(uint max_len)
    {
        uint len = NextLen(max_len);
        char[] ret1 = new char[len];
        for (int i = 0; i < len; i++)
        {
            ret1[i] = NextChar();
        }
        return new string(ret1);
    }
}

public static class LFSRSeqGen_Print
{
    public static void PrintLFSR(string key = "Hello World!")
    {
        uint count = 8;
        uint seed = 35913226;

        LFSRSeqGen seqgen = new LFSRSeqGen(seed, key);

        print_array(seqgen.NextInt8Array(count));
        print_array(seqgen.NextUInt8Array(count));
        print_array(seqgen.NextInt16Array(count));
        print_array(seqgen.NextUInt16Array(count));
        print_array(seqgen.NextInt32Array(count));
        print_array(seqgen.NextUInt32Array(count));
        print_array(seqgen.NextInt64Array(count));
        print_array(seqgen.NextUInt64Array(count));
        print_array(seqgen.NextFloatArray(count));
        print_array(seqgen.NextDoubleArray(count));
        Console.WriteLine(seqgen.NextString(count));
        print_array(seqgen.NextBoolArray(count));
        print_array_complex(seqgen.NextCFloatArray(count));
        print_array_complex(seqgen.NextCDoubleArray(count));
        Console.WriteLine();
        print_array(seqgen.NextInt8ArrayVarLen(count));
        print_array(seqgen.NextUInt8ArrayVarLen(count));
        print_array(seqgen.NextInt16ArrayVarLen(count));
        print_array(seqgen.NextUInt16ArrayVarLen(count));
        print_array(seqgen.NextInt32ArrayVarLen(count));
        print_array(seqgen.NextUInt32ArrayVarLen(count));
        print_array(seqgen.NextInt64ArrayVarLen(count));
        print_array(seqgen.NextUInt64ArrayVarLen(count));
        print_array(seqgen.NextFloatArrayVarLen(count));
        print_array(seqgen.NextDoubleArrayVarLen(count));
        Console.WriteLine(seqgen.NextStringVarLen(count));
        print_array(seqgen.NextBoolArrayVarLen(count));
        print_array_complex(seqgen.NextCFloatArrayVarLen(count));
        print_array_complex(seqgen.NextCDoubleArrayVarLen(count));
    }

    static void print_array<T>(T[] a)
    {
        Console.WriteLine(string.Join(" ", a.Select(x => x.ToString())));
    }

    static void print_array_complex(CSingle[] a)
    {
        Console.WriteLine(string.Join(" ", a.Select(x => x.Real.ToString() + "+" + x.Real.ToString() + "i")));
    }

    static void print_array_complex(CDouble[] a)
    {
        Console.WriteLine(string.Join(" ", a.Select(x => x.Real.ToString() + "+" + x.Real.ToString() + "i")));
    }
}

}