package com.robotraconteur.javatest;

import com.robotraconteur.*;
import java.io.*;
import java.nio.*;
// import java.nio.file.*;

public class MultiDimArrayTest
{
    public static String testdatapath = "";

    public static void Test()
    {
        TestDouble();
        TestByte();
    }

    public static void TestDouble()
    {
        MultiDimArray m1 = LoadDoubleArrayFromFile(testdatapath + "/testmdarray1.bin");
        MultiDimArray m2 = LoadDoubleArrayFromFile(testdatapath + "/testmdarray2.bin");
        MultiDimArray m3 = LoadDoubleArrayFromFile(testdatapath + "/testmdarray3.bin");
        MultiDimArray m4 = LoadDoubleArrayFromFile(testdatapath + "/testmdarray4.bin");
        MultiDimArray m5 = LoadDoubleArrayFromFile(testdatapath + "/testmdarray5.bin");

        m1.assignSubArray(new int[] {2, 2, 3, 3, 4}, m2, new int[] {0, 2, 0, 0, 0}, new int[] {1, 5, 5, 2, 1});

        ca((double[])m1.array, (double[])m3.array);

        MultiDimArray m6 = new MultiDimArray(new int[] {2, 2, 1, 1, 10}, new double[40]);
        m1.retrieveSubArray(new int[] {4, 2, 2, 8, 0}, m6, new int[] {0, 0, 0, 0, 0}, new int[] {2, 2, 1, 1, 10});
        ca((double[])m4.array, (double[])m6.array);

        MultiDimArray m7 = new MultiDimArray(new int[] {4, 4, 4, 4, 10}, new double[2560]);
        m1.retrieveSubArray(new int[] {4, 2, 2, 8, 0}, m7, new int[] {2, 1, 2, 1, 0}, new int[] {2, 2, 1, 1, 10});
        ca((double[])m5.array, (double[])m7.array);
    }

    public static void TestByte()
    {
        MultiDimArray m1 = LoadByteArrayFromFile(testdatapath + "/testmdarray_b1.bin");
        MultiDimArray m2 = LoadByteArrayFromFile(testdatapath + "/testmdarray_b2.bin");
        MultiDimArray m3 = LoadByteArrayFromFile(testdatapath + "/testmdarray_b3.bin");
        MultiDimArray m4 = LoadByteArrayFromFile(testdatapath + "/testmdarray_b4.bin");
        MultiDimArray m5 = LoadByteArrayFromFile(testdatapath + "/testmdarray_b5.bin");

        m1.assignSubArray(new int[] {50, 100}, m2, new int[] {20, 25}, new int[] {200, 200});

        ca(((UnsignedBytes)m1.array).value, ((UnsignedBytes)m3.array).value);

        MultiDimArray m6 = new MultiDimArray(new int[] {200, 200}, new UnsignedBytes(new byte[40000]));
        m1.retrieveSubArray(new int[] {65, 800}, m6, new int[] {0, 0}, new int[] {200, 200});
        ca(((UnsignedBytes)m4.array).value, ((UnsignedBytes)m6.array).value);

        MultiDimArray m7 = new MultiDimArray(new int[] {512, 512}, new UnsignedBytes(new byte[512 * 512]));
        m1.retrieveSubArray(new int[] {65, 800}, m7, new int[] {100, 230}, new int[] {200, 200});
        ca(((UnsignedBytes)m5.array).value, ((UnsignedBytes)m7.array).value);
    }

    public static MultiDimArray LoadDoubleArrayFromFile(String fname)
    {
        try
        {
            FileInputStream f = new FileInputStream(fname);
            MultiDimArray a = LoadDoubleArray(f);
            f.close();
            return a;
        }
        catch (Exception e)
        {
            throw new RuntimeException(e);
        }
    }

    public static int readInt(InputStream s)
    {
        try
        {
            ByteBuffer b = ByteBuffer.allocate(4);
            b.order(ByteOrder.LITTLE_ENDIAN);
            byte[] in = new byte[4];
            s.read(in);
            b.put(in);
            b.rewind();

            return b.getInt();
        }
        catch (Exception e)
        {
            throw new RuntimeException(e);
        }
    }

    public static double readDouble(InputStream s)
    {
        try
        {
            ByteBuffer b = ByteBuffer.allocate(8);
            b.order(ByteOrder.LITTLE_ENDIAN);
            byte[] in = new byte[8];
            s.read(in);
            b.put(in);

            b.rewind();
            return b.getDouble();
        }
        catch (Exception e)
        {
            throw new RuntimeException(e);
        }
    }

    public static MultiDimArray LoadDoubleArray(InputStream s)
    {
        try
        {

            int dimcount = readInt(s);
            int[] dims = new int[dimcount];
            int count = 1;
            for (int i = 0; i < dimcount; i++)
            {
                dims[i] = readInt(s);
                count *= dims[i];
            }

            double[] real = new double[count];

            return new MultiDimArray(dims, real);
        }
        catch (Exception e)
        {
            throw new RuntimeException(e);
        }
    }

    public static MultiDimArray LoadByteArrayFromFile(String fname)
    {
        try
        {
            FileInputStream f = new FileInputStream(fname);
            MultiDimArray a = LoadByteArray(f);
            f.close();
            return a;
        }
        catch (Exception e)
        {
            throw new RuntimeException(e);
        }
    }

    public static MultiDimArray LoadByteArray(InputStream s)
    {
        try
        {

            int dimcount = readInt(s);
            int[] dims = new int[dimcount];
            int count = 1;
            for (int i = 0; i < dimcount; i++)
            {
                dims[i] = readInt(s);
                count *= dims[i];
            }

            byte[] real = new byte[count];

            for (int i = 0; i < count; i++)
            {
                real[i] = (byte)s.read();
            }
            return new MultiDimArray(dims, new UnsignedBytes(real));
        }
        catch (Exception e)
        {
            throw new RuntimeException(e);
        }
    }

    public static final void ca(double[] v1, double[] v2)
    {
        RRAssert.areEqual(v1.length, v2.length);
        for (int i = 0; i < v1.length; i++)
        {
            RRAssert.areEqual(v1[i], v2[i]);
        }
    }

    public static final void ca(float[] v1, float[] v2)
    {
        RRAssert.areEqual(v1.length, v2.length);
        for (int i = 0; i < v1.length; i++)
        {
            RRAssert.areEqual(v1[i], v2[i]);
        }
    }

    public static final void ca(byte[] v1, byte[] v2)
    {
        RRAssert.areEqual(v1.length, v2.length);
        for (int i = 0; i < v1.length; i++)
        {
            RRAssert.areEqual(v1[i], v2[i]);
        }
    }

    public static final void ca(short[] v1, short[] v2)
    {
        RRAssert.areEqual(v1.length, v2.length);
        for (int i = 0; i < v1.length; i++)
        {
            RRAssert.areEqual(v1[i], v2[i]);
        }
    }

    public static final void ca(int[] v1, int[] v2)
    {
        RRAssert.areEqual(v1.length, v2.length);
        for (int i = 0; i < v1.length; i++)
        {
            RRAssert.areEqual(v1[i], v2[i]);
        }
    }

    public static final void ca(long[] v1, long[] v2)
    {
        RRAssert.areEqual(v1.length, v2.length);
        for (int i = 0; i < v1.length; i++)
        {
            RRAssert.areEqual(v1[i], v2[i]);
        }
    }
}