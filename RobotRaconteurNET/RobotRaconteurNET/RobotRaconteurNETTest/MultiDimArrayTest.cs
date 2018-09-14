using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using RobotRaconteur;
using System.IO;

namespace RobotRaconteurNETTest
{
    class MultiDimArrayTest
    {
        public static string testdatapath="";

        public static void Test()
        {
            TestDouble();
            TestByte();
        }

        public static void TestDouble()
        {
            MultiDimArray m1 = LoadDoubleArrayFromFile(Path.Combine(testdatapath, "testmdarray1.bin"));
            MultiDimArray m2 = LoadDoubleArrayFromFile(Path.Combine(testdatapath, "testmdarray2.bin"));
            MultiDimArray m3 = LoadDoubleArrayFromFile(Path.Combine(testdatapath, "testmdarray3.bin"));
            MultiDimArray m4 = LoadDoubleArrayFromFile(Path.Combine(testdatapath, "testmdarray4.bin"));
            MultiDimArray m5 = LoadDoubleArrayFromFile(Path.Combine(testdatapath, "testmdarray5.bin"));

            m1.AssignSubArray(new int[] { 2, 2, 3, 3, 4 }, m2, new int[] { 0, 2, 0, 0, 0 }, new int[] { 1, 5, 5, 2, 1 });

           

            ca<double>((double[])m1.Real, (double[])m3.Real);
            ca<double>((double[])m1.Imag, (double[])m3.Imag);

            MultiDimArray m6 = new MultiDimArray(new int[] { 2, 2, 1, 1, 10 }, new double[40],new double[40]);
            m1.RetrieveSubArray(new int[] { 4, 2, 2, 8, 0 }, m6, new int[] { 0, 0, 0, 0, 0 }, new int[] { 2, 2, 1, 1, 10 });
            ca<double>((double[])m4.Real, (double[])m6.Real);
            ca<double>((double[])m4.Imag, (double[])m6.Imag);

            MultiDimArray m7 = new MultiDimArray(new int[] { 4, 4, 4, 4, 10 }, new double[2560], new double[2560]);
            m1.RetrieveSubArray(new int[] { 4, 2, 2, 8, 0 }, m7, new int[] { 2, 1,2, 1, 0 }, new int[] { 2, 2, 1, 1, 10 });
            ca<double>((double[])m5.Real, (double[])m7.Real);
            ca<double>((double[])m5.Imag, (double[])m7.Imag);

            



        }

        public static void TestByte()
        {
            MultiDimArray m1 = LoadByteArrayFromFile(Path.Combine(testdatapath, "testmdarray_b1.bin"));
            MultiDimArray m2 = LoadByteArrayFromFile(Path.Combine(testdatapath, "testmdarray_b2.bin"));
            MultiDimArray m3 = LoadByteArrayFromFile(Path.Combine(testdatapath, "testmdarray_b3.bin"));
            MultiDimArray m4 = LoadByteArrayFromFile(Path.Combine(testdatapath, "testmdarray_b4.bin"));
            MultiDimArray m5 = LoadByteArrayFromFile(Path.Combine(testdatapath, "testmdarray_b5.bin"));

            m1.AssignSubArray(new int[] { 50,100 }, m2, new int[] { 20,25}, new int[] { 200,200 });



            ca<byte>((byte[])m1.Real, (byte[])m3.Real);
            

            MultiDimArray m6 = new MultiDimArray(new int[] { 200,200 }, new byte[40000]);
            m1.RetrieveSubArray(new int[] { 65,800 }, m6, new int[] { 0, 0 }, new int[] { 200,200 });
            ca<byte>((byte[])m4.Real, (byte[])m6.Real);
            

            MultiDimArray m7 = new MultiDimArray(new int[] { 512,512 }, new byte[512*512]);
            m1.RetrieveSubArray(new int[] { 65,800 }, m7, new int[] { 100,230}, new int[] { 200,200 });
            ca<byte>((byte[])m5.Real, (byte[])m7.Real);

        }

        public static void ca<T>(T[] v1, T[] v2)
        {
            if (v1.Length != v2.Length) throw new Exception();
            for (int i = 0; i < v1.Length; i++)
            {
                if (!Object.Equals(v1[i], v2[i])) throw new Exception();
            }
        }

        public static MultiDimArray LoadDoubleArrayFromFile(string fname)
        {
            FileStream f = new FileStream(fname, FileMode.Open, FileAccess.Read);
            MultiDimArray a = LoadDoubleArray(f);
            f.Close();
            return a;
        }

        public static MultiDimArray LoadDoubleArray(Stream s)
        {
            BinaryReader r = new BinaryReader(s);
            int dimcount = r.ReadInt32();
            int[] dims = new int[dimcount];
            int count = 1;
            for (int i = 0; i < dimcount; i++)
            {
                dims[i] = r.ReadInt32();
                count *= dims[i];
            }

            double[] real = new double[count];

            for (int i = 0; i < count; i++)
            {
                real[i] = r.ReadDouble();
            }

            if ((r.BaseStream.Length - r.BaseStream.Position) > 0)
            {
                double[] imag = new double[count];

                for (int i = 0; i < count; i++)
                {
                    imag[i] = r.ReadDouble();
                }
                return new MultiDimArray(dims, real,imag);
            }
            else
            {

                return new MultiDimArray(dims, real);
            }


        }

        public static MultiDimArray LoadByteArrayFromFile(string fname)
        {
            FileStream f = new FileStream(fname, FileMode.Open, FileAccess.Read);
            MultiDimArray a = LoadByteArray(f);
            f.Close();
            return a;
        }

        public static MultiDimArray LoadByteArray(Stream s)
        {
            BinaryReader r = new BinaryReader(s);
            int dimcount = r.ReadInt32();
            int[] dims = new int[dimcount];
            int count = 1;
            for (int i = 0; i < dimcount; i++)
            {
                dims[i] = r.ReadInt32();
                count *= dims[i];
            }

            byte[] real = new byte[count];

            for (int i = 0; i < count; i++)
            {
                real[i] = r.ReadByte();
            }

            if ((r.BaseStream.Length - r.BaseStream.Position) > 0)
            {
                byte[] imag = new byte[count];

                for (int i = 0; i < count; i++)
                {
                    imag[i] = r.ReadByte();
                }
                return new MultiDimArray(dims, real, imag);
            }
            else
            {

                return new MultiDimArray(dims, real);
            }


        }
    }
}
