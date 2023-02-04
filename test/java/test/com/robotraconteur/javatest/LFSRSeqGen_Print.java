package com.robotraconteur.javatest;

import com.robotraconteur.*;
import java.util.*;
import java.math.*;

public class LFSRSeqGen_Print
{
    public static void printLFSR()
    {
        printLFSR("Hello World!");
    }

    public static void printLFSR(String key)
    {
        int count = 8;
        long seed = 35913226;

        LFSRSeqGen seqgen = new LFSRSeqGen(seed, key);

        print_array(seqgen.nextInt8Array(count));
        print_array(seqgen.nextUInt8Array(count));
        print_array(seqgen.nextInt16Array(count));
        print_array(seqgen.nextUInt16Array(count));
        print_array(seqgen.nextInt32Array(count));
        print_array(seqgen.nextUInt32Array(count));
        print_array(seqgen.nextInt64Array(count));
        print_array(seqgen.nextUInt64Array(count));
        print_array(seqgen.nextFloatArray(count));
        print_array(seqgen.nextDoubleArray(count));
        System.out.println(seqgen.nextString(count));
        print_array(seqgen.nextBoolArray(count));
        print_array_complex(seqgen.nextCFloatArray(count));
        print_array_complex(seqgen.nextCDoubleArray(count));
        System.out.println();
        print_array(seqgen.nextInt8ArrayVarLen(count));
        print_array(seqgen.nextUInt8ArrayVarLen(count));
        print_array(seqgen.nextInt16ArrayVarLen(count));
        print_array(seqgen.nextUInt16ArrayVarLen(count));
        print_array(seqgen.nextInt32ArrayVarLen(count));
        print_array(seqgen.nextUInt32ArrayVarLen(count));
        print_array(seqgen.nextInt64ArrayVarLen(count));
        print_array(seqgen.nextUInt64ArrayVarLen(count));
        print_array(seqgen.nextFloatArrayVarLen(count));
        print_array(seqgen.nextDoubleArrayVarLen(count));
        System.out.println(seqgen.nextStringVarLen(count));
        print_array(seqgen.nextBoolArrayVarLen(count));
        print_array_complex(seqgen.nextCFloatArrayVarLen(count));
        print_array_complex(seqgen.nextCDoubleArrayVarLen(count));
    }

    static void print_array(byte[] a)
    {
        String[] a1 = new String[a.length];
        for (int i = 0; i < a.length; i++)
        {
            a1[i] = String.valueOf(a[i]);
        }
        System.out.println(String.join(" ", a1));
    }

    static void print_array(UnsignedBytes a)
    {
        String[] a1 = new String[a.value.length];
        for (int i = 0; i < a.value.length; i++)
        {
            long val = a.value[i];
            if (val < 0)
            {
                val = 256 + val;
            }
            a1[i] = String.valueOf(val);
        }
        System.out.println(String.join(" ", a1));
    }

    static void print_array(short[] a)
    {
        String[] a1 = new String[a.length];
        for (int i = 0; i < a.length; i++)
        {
            a1[i] = String.valueOf(a[i]);
        }
        System.out.println(String.join(" ", a1));
    }

    static void print_array(UnsignedShorts a)
    {
        String[] a1 = new String[a.value.length];
        for (int i = 0; i < a.value.length; i++)
        {
            long val = a.value[i];
            if (val < 0)
            {
                val = 32768 + val;
            }
            a1[i] = String.valueOf(val);
        }
        System.out.println(String.join(" ", a1));
    }

    static void print_array(int[] a)
    {
        String[] a1 = new String[a.length];
        for (int i = 0; i < a.length; i++)
        {
            a1[i] = String.valueOf(a[i]);
        }
        System.out.println(String.join(" ", a1));
    }

    static void print_array(UnsignedInts a)
    {
        String[] a1 = new String[a.value.length];
        for (int i = 0; i < a.value.length; i++)
        {
            long val = a.value[i];
            if (val < 0)
            {
                val = 2147483648l + val;
            }
            a1[i] = String.valueOf(val);
        }
        System.out.println(String.join(" ", a1));
    }

    static void print_array(long[] a)
    {
        String[] a1 = new String[a.length];
        for (int i = 0; i < a.length; i++)
        {
            a1[i] = String.valueOf(a[i]);
        }
        System.out.println(String.join(" ", a1));
    }

    static void print_array(UnsignedLongs a)
    {
        String[] a1 = new String[a.value.length];
        for (int i = 0; i < a.value.length; i++)
        {
            BigInteger val = BigInteger.valueOf(a.value[i]);
            if (val.compareTo(BigInteger.valueOf(0)) < 0)
            {
                val = val.add(new BigInteger("18446744073709551616"));
            }
            a1[i] = String.valueOf(val);
        }
        System.out.println(String.join(" ", a1));
    }

    static void print_array(float[] a)
    {
        String[] a1 = new String[a.length];
        for (int i = 0; i < a.length; i++)
        {
            a1[i] = String.valueOf(a[i]);
        }
        System.out.println(String.join(" ", a1));
    }

    static void print_array(double[] a)
    {
        String[] a1 = new String[a.length];
        for (int i = 0; i < a.length; i++)
        {
            a1[i] = String.valueOf(a[i]);
        }
        System.out.println(String.join(" ", a1));
    }

    static void print_array(boolean[] a)
    {
        String[] a1 = new String[a.length];
        for (int i = 0; i < a.length; i++)
        {
            a1[i] = String.valueOf(a[i]);
        }
        System.out.println(String.join(" ", a1));
    }

    static void print_array_complex(CSingle[] a)
    {
        String[] a1 = new String[a.length];
        for (int i = 0; i < a.length; i++)
        {
            a1[i] = String.valueOf(a[i].real) + "+" + String.valueOf(a[i].imag) + "i";
        }
        System.out.println(String.join(" ", a1));
    }

    static void print_array_complex(CDouble[] a)
    {
        String[] a1 = new String[a.length];
        for (int i = 0; i < a.length; i++)
        {
            a1[i] = String.valueOf(a[i].real) + "+" + String.valueOf(a[i].imag) + "i";
        }
        System.out.println(String.join(" ", a1));
    }
}
