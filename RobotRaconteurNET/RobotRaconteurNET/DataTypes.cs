// Copyright 2011-2020 Wason Technology, LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

using System;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.IO;
using System.Reflection;
using System.Linq;
using System.Threading.Tasks;

namespace RobotRaconteur
{

public struct CDouble
{
    public double Real;
    public double Imag;

    public CDouble(double real, double imag)
    {
        Real = real;
        Imag = imag;
    }

    public static bool operator ==(CDouble a, CDouble b)
    {
        return (a.Real == b.Real) && (a.Imag == b.Imag);
    }

    public static bool operator !=(CDouble a, CDouble b)
    {
        return !((a.Real == b.Real) && (a.Imag == b.Imag));
    }

    public override bool Equals(object obj)
    {
        if (obj == null)
            return false;
        if (!(obj is CDouble))
            return false;
        return ((CDouble)obj) == this;
    }

    public override int GetHashCode()
    {
        return (int)(Real % 1e7 + Imag % 1e7);
    }
}

public struct CSingle
{
    public float Real;
    public float Imag;

    public CSingle(float real, float imag)
    {
        Real = real;
        Imag = imag;
    }

    public static bool operator ==(CSingle a, CSingle b)
    {
        return (a.Real == b.Real) && (a.Imag == b.Imag);
    }

    public static bool operator !=(CSingle a, CSingle b)
    {
        return !((a.Real == b.Real) && (a.Imag == b.Imag));
    }

    public override bool Equals(object obj)
    {
        if (obj == null)
            return false;
        if (!(obj is CSingle))
            return false;
        return ((CSingle)obj) == this;
    }

    public override int GetHashCode()
    {
        return (int)(Real % 1e7 + Imag % 1e7);
    }
}

public class DataTypeUtil
{

    public static uint size(DataTypes type)
    {
        switch (type)
        {
        case DataTypes.double_t:
            return 8;

        case DataTypes.single_t:
            return 4;
        case DataTypes.int8_t:
        case DataTypes.uint8_t:
            return 1;
        case DataTypes.int16_t:
        case DataTypes.uint16_t:
            return 2;
        case DataTypes.int32_t:
        case DataTypes.uint32_t:
            return 4;
        case DataTypes.int64_t:
        case DataTypes.uint64_t:
            return 8;
        case DataTypes.string_t:
            return 1;
        case DataTypes.cdouble_t:
            return 16;
        case DataTypes.csingle_t:
            return 8;
        case DataTypes.bool_t:
            return 1;
        }
        throw new ArgumentException();
        // return 0;
    }

    public static DataTypes TypeIDFromString(string stype)
    {
        switch (stype)
        {
        case "null":
            return DataTypes.void_t;
        case "System.Double":
            return DataTypes.double_t;
        case "System.Single":
            return DataTypes.single_t;
        case "System.SByte":
            return DataTypes.int8_t;
        case "System.Byte":
            return DataTypes.uint8_t;
        case "System.Int16":
            return DataTypes.int16_t;
        case "System.UInt16":
            return DataTypes.uint16_t;
        case "System.Int32":
            return DataTypes.int32_t;
        case "System.UInt32":
            return DataTypes.uint32_t;
        case "System.Int64":
            return DataTypes.int64_t;
        case "System.UInt64":
            return DataTypes.uint64_t;
        case "System.String":
            return DataTypes.string_t;
        case "RobotRaconteur.MessageElementNestedElementList":
            return DataTypes.namedtype_t;
        case "System.Object":
            return DataTypes.varvalue_t;
        case "RobotRaconteur.CDouble":
            return DataTypes.cdouble_t;
        case "RobotRaconteur.CSingle":
            return DataTypes.csingle_t;
        case "System.Boolean":
            return DataTypes.bool_t;
        default:
            break;
        }

        throw new DataTypeException("Unknown data type");
    }

    public static bool TypeIDFromString_known(string stype)
    {
        switch (stype)
        {
        case "null":

        case "System.Double":

        case "System.Single":

        case "System.SByte":

        case "System.Byte":

        case "System.Int16":

        case "System.UInt16":

        case "System.Int32":

        case "System.UInt32":

        case "System.Int64":

        case "System.UInt64":

        case "System.String":

        case "RobotRaconteur.MessageElementNestedElementList":

        case "System.Object":

        case "RobotRaconteur.CDouble":

        case "RobotRaconteur.CSingle":

        case "System.Boolean":

            return true;
        }

        return false;
    }

    public static bool IsNumber(DataTypes type)
    {
        switch (type)
        {
        case DataTypes.double_t:
        case DataTypes.single_t:
        case DataTypes.int8_t:
        case DataTypes.uint8_t:
        case DataTypes.int16_t:
        case DataTypes.uint16_t:
        case DataTypes.int32_t:
        case DataTypes.uint32_t:
        case DataTypes.int64_t:
        case DataTypes.uint64_t:
        case DataTypes.cdouble_t:
        case DataTypes.csingle_t:
        case DataTypes.bool_t:
            return true;
        default:
            return false;
        }
    }

    public static Array ArrayFromDataType(DataTypes t, uint length)
    {
        switch (t)
        {

        case DataTypes.double_t:
            return new double[length];
        case DataTypes.single_t:
            return new float[length];
        case DataTypes.int8_t:
            return new sbyte[length];
        case DataTypes.uint8_t:
            return new byte[length];
        case DataTypes.int16_t:
            return new short[length];
        case DataTypes.uint16_t:
            return new ushort[length];
        case DataTypes.int32_t:
            return new int[length];
        case DataTypes.uint32_t:
            return new uint[length];
        case DataTypes.int64_t:
            return new long[length];
        case DataTypes.uint64_t:
            return new ulong[length];
        case DataTypes.string_t:
            return null;
        case DataTypes.structure_t:
            return null;
        case DataTypes.cdouble_t:
            return new CDouble[length];
        case DataTypes.csingle_t:
            return new CSingle[length];
        case DataTypes.bool_t:
            return new bool[length];
        default:
            break;
        }

        throw new DataTypeException("Could not create array for data type");
    }

    public static object ArrayFromScalar(object inv)
    {

        if (inv is Array)
            return inv;

        string stype = inv.GetType().ToString();
        switch (stype)
        {
        case "System.Double":
            return new double[] { ((double)inv) };
        case "System.Single":
            return new float[] { ((float)inv) };
        case "System.SByte":
            return new sbyte[] { ((sbyte)inv) };
        case "System.Byte":
            return new byte[] { ((byte)inv) };
        case "System.Int16":
            return new short[] { ((short)inv) };
        case "System.UInt16":
            return new ushort[] { ((ushort)inv) };
        case "System.Int32":
            return new int[] { ((int)inv) };
        case "System.UInt32":
            return new uint[] { ((uint)inv) };
        case "System.Int64":
            return new long[] { ((long)inv) };
        case "System.UInt64":
            return new ulong[] { ((ulong)inv) };
        case "RobotRaconteur.CDouble":
            return new CDouble[] { (CDouble)inv };
        case "RobotRaconteur.CSingle":
            return new CSingle[] { (CSingle)inv };
        case "System.Boolean":
            return new bool[] { (bool)inv };
        default:
            break;
        }

        throw new DataTypeException("Could not create array for data");
    }

    public static T[] VerifyArrayLength<T>(T[] a, int len, bool varlength)
        where T : struct
    {
        if (a == null)
            throw new NullReferenceException();
        if (len != 0)
        {
            if (varlength && a.Length > len)
            {
                throw new DataTypeException("Array dimension mismatch");
            }
            if (!varlength && a.Length != len)
            {
                throw new DataTypeException("Array dimension mismatch");
            }
        }
        return a;
    }

    public static MultiDimArray VerifyArrayLength(MultiDimArray a, int n_elems, uint[] len)
    {
        if (a.Dims.Length != len.Length)
        {
            throw new DataTypeException("Array dimension mismatch");
        }

        for (int i = 0; i < len.Length; i++)
        {
            if (a.Dims[i] != len[i])
            {
                throw new DataTypeException("Array dimension mismatch");
            }
        }
        return a;
    }

    public static PodMultiDimArray VerifyArrayLength(PodMultiDimArray a, int n_elems, uint[] len)
    {
        if (a.Dims.Length != len.Length)
        {
            throw new DataTypeException("Array dimension mismatch");
        }

        for (int i = 0; i < len.Length; i++)
        {
            if (a.Dims[i] != len[i])
            {
                throw new DataTypeException("Array dimension mismatch");
            }
        }
        return a;
    }

    public static NamedMultiDimArray VerifyArrayLength(NamedMultiDimArray a, int n_elems, uint[] len)
    {
        if (a.Dims.Length != len.Length)
        {
            throw new DataTypeException("Array dimension mismatch");
        }

        for (int i = 0; i < len.Length; i++)
        {
            if (a.Dims[i] != len[i])
            {
                throw new DataTypeException("Array dimension mismatch");
            }
        }
        return a;
    }

    public static List<T[]> VerifyArrayLength<T>(List<T[]> a, int len, bool varlength)
        where T : struct
    {
        if (a == null)
            return a;
        foreach (T[] aa in a)
        {
            VerifyArrayLength(aa, len, varlength);
        }

        return a;
    }

    public static Dictionary<K, T[]> VerifyArrayLength<K, T>(Dictionary<K, T[]> a, int len, bool varlength)
        where T : struct
    {
        if (a == null)
            return a;
        foreach (T[] aa in a.Values)
        {
            VerifyArrayLength(aa, len, varlength);
        }

        return a;
    }

    public static List<MultiDimArray> VerifyArrayLength(List<MultiDimArray> a, int n_elems, uint[] len)
    {
        if (a == null)
            return a;
        foreach (MultiDimArray aa in a)
        {
            VerifyArrayLength(aa, n_elems, len);
        }

        return a;
    }

    public static Dictionary<K, MultiDimArray> VerifyArrayLength<K>(Dictionary<K, MultiDimArray> a, int n_elems,
                                                                    uint[] len)
    {
        if (a == null)
            return a;
        foreach (MultiDimArray aa in a.Values)
        {
            VerifyArrayLength(aa, n_elems, len);
        }

        return a;
    }

    public static List<PodMultiDimArray> VerifyArrayLength(List<PodMultiDimArray> a, int n_elems, uint[] len)
    {
        if (a == null)
            return a;
        foreach (PodMultiDimArray aa in a)
        {
            VerifyArrayLength(aa, n_elems, len);
        }

        return a;
    }

    public static Dictionary<K, PodMultiDimArray> VerifyArrayLength<K>(Dictionary<K, PodMultiDimArray> a, int n_elems,
                                                                       uint[] len)
    {
        if (a == null)
            return a;
        foreach (PodMultiDimArray aa in a.Values)
        {
            VerifyArrayLength(aa, n_elems, len);
        }

        return a;
    }

    public static List<NamedMultiDimArray> VerifyArrayLength(List<NamedMultiDimArray> a, int n_elems, uint[] len)
    {
        if (a == null)
            return a;
        foreach (NamedMultiDimArray aa in a)
        {
            VerifyArrayLength(aa, n_elems, len);
        }

        return a;
    }

    public static Dictionary<K, NamedMultiDimArray> VerifyArrayLength<K>(Dictionary<K, NamedMultiDimArray> a,
                                                                         int n_elems, uint[] len)
    {
        if (a == null)
            return a;
        foreach (NamedMultiDimArray aa in a.Values)
        {
            VerifyArrayLength(aa, n_elems, len);
        }

        return a;
    }
}

public partial class RobotRaconteurVersion
{
    public override bool Equals(object v2)
    {
        RobotRaconteurVersion v2_1 = v2 as RobotRaconteurVersion;

        if (v2_1 == null)
            return false;

        return RobotRaconteurNET.RobotRaconteurVersion_eq(this, v2_1);
    }

    public override int GetHashCode()
    {
        var v1 = Tuple.Create(major, minor, patch, tweak);
        return v1.GetHashCode();
    }

    public static bool operator ==(RobotRaconteurVersion v1, RobotRaconteurVersion v2)
    {
        return RobotRaconteurNET.RobotRaconteurVersion_eq(v1, v2);
    }

    public static bool operator !=(RobotRaconteurVersion v1, RobotRaconteurVersion v2)
    {
        return RobotRaconteurNET.RobotRaconteurVersion_ne(v1, v2);
    }
}

public partial class map_strstr
{
    public static implicit operator map_strstr(System.Collections.Generic.Dictionary<string, string> i)
    {
        map_strstr o = new map_strstr();
        foreach (System.Collections.Generic.KeyValuePair<string, string> i2 in i)
            o.Add(i2.Key, i2.Value);

        return o;
    }
}

public partial class TimeSpec
{
    public static bool operator ==(TimeSpec a, TimeSpec b)
    {
        if (Object.Equals(a, null) && Object.Equals(b, null))
            return true;
        if (Object.Equals(a, null) || Object.Equals(b, null))
            return false;
        return a.eq(b);
    }

    public override bool Equals(Object o)
    {
        if (!(o is TimeSpec))
            return false;
        return this == ((TimeSpec)o);
    }

    public override int GetHashCode()
    {
        return (int)((seconds + nanoseconds) % int.MaxValue);
    }

    public static bool operator !=(TimeSpec a, TimeSpec b)
    {
        return !(a == b);
    }

    public static TimeSpec operator +(TimeSpec a, TimeSpec b)
    {
        return a.add(b);
    }

    public static TimeSpec operator -(TimeSpec a, TimeSpec b)
    {
        return a.sub(b);
    }

    public static bool operator>(TimeSpec a, TimeSpec b)
    {
        return a.gt(b);
    }

    public static bool operator<(TimeSpec a, TimeSpec b)
    {
        return a.lt(b);
    }

    public static bool operator >=(TimeSpec a, TimeSpec b)
    {
        return a.ge(b);
    }

    public static bool operator <=(TimeSpec a, TimeSpec b)
    {
        return a.le(b);
    }
}

}