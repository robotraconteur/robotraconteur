// Copyright 2011-2018 Wason Technology, LLC
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

namespace RobotRaconteur
{

    public struct CDouble
    {
        public double Real;
        public double Imag;

        public CDouble (double real, double imag)
        {
            Real = real;
            Imag = imag;
        }

        public static bool operator== (CDouble a, CDouble b)
        {
            return (a.Real == b.Real) && (a.Imag == b.Imag);
        }

        public static bool operator !=(CDouble a, CDouble b)
        {
            return !((a.Real == b.Real) && (a.Imag == b.Imag));
        }

        public override bool Equals(object obj)
        {
            if (obj == null) return false;
            if (!(obj is CDouble)) return false;
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
            if (obj == null) return false;
            if (!(obj is CSingle)) return false;
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
            //return 0;
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
                case "RobotRaconteur.MessageElementStructure":
                    return DataTypes.structure_t;
                case "RobotRaconteur.MessageElementIndexedSet<int>":
                    return DataTypes.vector_t;
                case "RobotRaconteur.MessageElementIndexedSet<string>":
                    return DataTypes.dictionary_t;
                case "RobotRaconteur.MessageElementMultiDimArray":
                    return DataTypes.multidimarray_t;
                case "System.Object":
                    return DataTypes.varvalue_t;
                case "RobotRaconteur.CDouble":
                    return DataTypes.cdouble_t;
                case "RobotRaconteur.CSingle":
                    return DataTypes.csingle_t;
                case "System.Bool":
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

                case "RobotRaconteur.MessageElementStructure":

                case "RobotRaconteur.MessageElementIndexedSet<int>":

                case "RobotRaconteur.MessageElementIndexedSet<string>":

                case "RobotRaconteur.MessageElementMultiDimArray":

                case "System.Object":

                case "RobotRaconteur.CDouble":
                    
                case "RobotRaconteur.CSingle":
                    
                case "System.Bool":
                    
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

            if (inv is Array) return inv;

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
                case "System.Bool":
                    return new bool[] { (bool)inv };
                default:
                    break;
            }

            throw new DataTypeException("Could not create array for data");
        }

        public static T[] VerifyArrayLength<T>(T[] a, int len, bool varlength) where T : struct
        {
            if (a == null) throw new NullReferenceException();
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

        public static List<T[]> VerifyArrayLength<T>(List<T[]> a, int len, bool varlength) where T : struct
        {
            if (a == null) return a;
            foreach (T[] aa in a)
            {
                VerifyArrayLength(aa, len, varlength);
            }

            return a;
        }

        public static Dictionary<K, T[]> VerifyArrayLength<K, T>(Dictionary<K, T[]> a, int len, bool varlength) where T : struct
        {
            if (a == null) return a;
            foreach (T[] aa in a.Values)
            {
                VerifyArrayLength(aa, len, varlength);
            }

            return a;
        }

        public static List<MultiDimArray> VerifyArrayLength(List<MultiDimArray> a, int n_elems, uint[] len)
        {
            if (a == null) return a;
            foreach (MultiDimArray aa in a)
            {
                VerifyArrayLength(aa, n_elems, len);
            }

            return a;
        }

        public static Dictionary<K, MultiDimArray> VerifyArrayLength<K>(Dictionary<K, MultiDimArray> a, int n_elems, uint[] len)
        {
            if (a == null) return a;
            foreach (MultiDimArray aa in a.Values)
            {
                VerifyArrayLength(aa, n_elems, len);
            }

            return a;
        }

        public static List<PodMultiDimArray> VerifyArrayLength(List<PodMultiDimArray> a, int n_elems, uint[] len)
        {
            if (a == null) return a;
            foreach (PodMultiDimArray aa in a)
            {
                VerifyArrayLength(aa, n_elems, len);
            }

            return a;
        }

        public static Dictionary<K, PodMultiDimArray> VerifyArrayLength<K>(Dictionary<K, PodMultiDimArray> a, int n_elems, uint[] len)
        {
            if (a == null) return a;
            foreach (PodMultiDimArray aa in a.Values)
            {
                VerifyArrayLength(aa, n_elems, len);
            }

            return a;
        }

        public static List<NamedMultiDimArray> VerifyArrayLength(List<NamedMultiDimArray> a, int n_elems, uint[] len)
        {
            if (a == null) return a;
            foreach (NamedMultiDimArray aa in a)
            {
                VerifyArrayLength(aa, n_elems, len);
            }

            return a;
        }

        public static Dictionary<K, NamedMultiDimArray> VerifyArrayLength<K>(Dictionary<K, NamedMultiDimArray> a, int n_elems, uint[] len)
        {
            if (a == null) return a;
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

            if (v2_1 == null) return false;

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

    public partial class vectorptr_memberdefinition
    {
        private MemberDefinition getitem(int index)
        {
            return MemberDefinitionUtil.SwigCast(_getitem(index));

        }

        private MemberDefinition getitemcopy(int index)
        {
            return MemberDefinitionUtil.SwigCast(_getitemcopy(index));
        }

    }

    public partial class MessageEntry
    {
        public MessageElement AddElement(string name, object data)
        {
            using (MessageElement m = new MessageElement(name, data))
            {
                elements.Add(m);
                return m;
            }

        }
    }

    public partial class MessageElement
    {
        public MessageElement(string name, object data) : this()
        {
            ElementName = name;
            SetData(data);
        }

        private object GetData()
        {

            using (MessageElementData val = _GetData())
            {
                if (val == null) return null;
                object a = null;
                a = MessageElementDataUtil.ToRRBaseArray(val);
                if (a != null)
                {
                    if (DataTypeUtil.IsNumber(val.GetTypeID()))
                    {
                        RRBaseArray ra = (RRBaseArray)a;
                        try
                        {
                            return MessageElementDataUtil.RRBaseArrayToArray(ra);
                        }
                        finally
                        {
                            ra.Dispose();

                        }
                    }
                    if (val.GetTypeID() == DataTypes.string_t)
                    {
                        RRBaseArray ra = (RRBaseArray)a;
                        try
                        {
                            return MessageElementDataUtil.RRBaseArrayToString(ra);
                        }
                        finally
                        {
                            ra.Dispose();

                        }
                    }
                    IDisposable ad = a as IDisposable;
                    if (ad != null) ad.Dispose();
                    throw new DataTypeException("Unknown RRArray type");

                }
                a = MessageElementDataUtil.ToMessageElementStructure(val);
                if (a != null) return a;
                a = MessageElementDataUtil.ToMessageElementMap_int32_t(val);
                if (a != null) return a;
                a = MessageElementDataUtil.ToMessageElementMap_string(val);
                if (a != null) return a;
                a = MessageElementDataUtil.ToMessageElementList(val);
                if (a != null) return a;
                a = MessageElementDataUtil.ToMessageElementMultiDimArray(val);
                if (a != null) return a;
                a = MessageElementDataUtil.ToMessageElementPod(val);
                if (a != null) return a;
                a = MessageElementDataUtil.ToMessageElementPodArray(val);
                if (a != null) return a;
                a = MessageElementDataUtil.ToMessageElementPodMultiDimArray(val);
                if (a != null) return a;
                a = MessageElementDataUtil.ToMessageElementNamedArray(val);
                if (a != null) return a;
                a = MessageElementDataUtil.ToMessageElementNamedMultiDimArray(val);
                if (a != null) return a;
                throw new ApplicationException("Unknown data type");

            }

            throw new ApplicationException("Unknown data type");

        }

        private void SetData(object dat)
        {
            if (dat == null)
            {
                _SetData(null);
                return;
            }
            if (dat is Array)
            {

                using (RRBaseArray rb = MessageElementDataUtil.ArrayToRRBaseArray((Array)dat))
                {
                    _SetData(rb);
                }
                return;
            }

            if (dat is string)
            {


                using (RRBaseArray rb = MessageElementDataUtil.stringToRRBaseArray((string)dat))
                {
                    _SetData(rb);
                }

                return;
            }

            if (dat is MessageElementData)
            {
                _SetData((MessageElementData)dat);
                return;
            }

            string datatype = datatype = RobotRaconteurNode.GetTypeString(dat.GetType());
            DataTypes elementtype = DataTypeUtil.TypeIDFromString(datatype);
            if (elementtype != DataTypes.void_t && elementtype < DataTypes.uint64_t && !(dat is Array))
            {
                object dat2 = DataTypeUtil.ArrayFromScalar(dat);

                using (RRBaseArray rb = MessageElementDataUtil.ArrayToRRBaseArray((Array)dat2))
                {
                    _SetData(rb);
                }

                return;
            }

            throw new DataTypeException("Invalid MessageElement data type");

        }

        public object Data
        {
            get
            {
                return GetData();
            }
            set
            {
                SetData(value);
            }

        }

        public T CastData<T>()
        {
            if (Data == null) return default(T);
            if (Data is T)
                return (T)Data;
            throw new DataTypeException("Could not cast data to type " + typeof(T).ToString());
        }
    }


    public class MessageElementUtil
    {
        public static MessageElement NewMessageElementDispose(string name, object data)
        {
            MessageElement m = null;
            try
            {
                m = new MessageElement();
                m.ElementName = name;
                m.Data = data;
                return m;
            }
            catch
            {
                if (m != null) m.Dispose();
                throw;
            }
            finally
            {
                IDisposable d = data as IDisposable;
                if (d != null) d.Dispose();
            }

        }

        public static void AddMessageElementDispose(vectorptr_messageelement vct, string name, object data)
        {
            using (MessageElement m = NewMessageElementDispose(name, data))
            {
                vct.Add(m);
            }
        }

        public static MessageElement NewMessageElementDispose(int i, object data)
        {
            MessageElement m = null;
            try
            {
                m = new MessageElement();
                m.ElementNumber = i;
                m.ElementFlags &= ((byte)~RobotRaconteurNET.MessageElementFlags_ELEMENT_NAME_STR);
                m.ElementFlags |= ((byte)RobotRaconteurNET.MessageElementFlags_ELEMENT_NUMBER);
                m.Data = data;
                return m;
            }
            catch
            {
                if (m != null) m.Dispose();
                throw;
            }
            finally
            {
                IDisposable d = data as IDisposable;
                if (d != null) d.Dispose();
            }

        }

        public static void AddMessageElementDispose(vectorptr_messageelement vct, int i, object data)
        {
            using (MessageElement m = NewMessageElementDispose(i, data))
            {
                vct.Add(m);
            }
        }

        public static void AddMessageElementDispose(vectorptr_messageelement vct, MessageElement m)
        {
            try
            {
                vct.Add(m);
            }
            finally
            {
                if (m != null) m.Dispose();
            }

        }

        public static T FindElementAndCast<T>(vectorptr_messageelement elems, string name)
        {
            using (MessageElement e = MessageElement.FindElement(elems, name))
            {
                return e.CastData<T>();
            }
        }

        public static T CastDataAndDispose<T>(MessageElement m)
        {
            object dat = null;
            try
            {
                dat = m.Data;
                return (T)dat;
            }
            catch
            {
                IDisposable dat2 = dat as IDisposable;
                if (dat2 != null) dat2.Dispose();
                throw;
            }
            finally
            {
                if (m != null) m.Dispose();
            }
        }

        public static int GetMessageElementNumber(MessageElement e)
        {
            if ((e.ElementFlags & RobotRaconteurNET.MessageElementFlags_ELEMENT_NUMBER) != 0)
            {
                return e.ElementNumber;
            }
            else if ((e.ElementFlags & RobotRaconteurNET.MessageElementFlags_ELEMENT_NAME_STR) != 0)
            {
                return Int32.Parse(e.ElementName);
            }
            else
            {
                throw new InvalidDataException("Could not determine Element Number");
            }
        }

        public static MessageElement PackScalar<T>(string name, T val) where T : struct
        {
            return NewMessageElementDispose(name, new T[] { val });
        }

        public static MessageElement PackArray<T>(string name, T[] val) where T : struct
        {
            if (val == null)
            {
                throw new NullReferenceException();
            }
            return NewMessageElementDispose(name, val);
        }

        public static MessageElement PackMultiDimArray(string name, MultiDimArray val)
        {
            if (val == null)
            {
                throw new NullReferenceException();
            }

            return NewMessageElementDispose(name, RobotRaconteurNode.s.PackMultiDimArray(val));
        }

        public static MessageElement PackString(string name, string val)
        {
            if (val == null)
            {
                throw new NullReferenceException();
            }
            return NewMessageElementDispose(name, val);
        }

        public static MessageElement PackStructure(string name, object val)
        {
            return NewMessageElementDispose(name, RobotRaconteurNode.s.PackStructure(val));
        }

        public static MessageElement PackVarType(string name, object val)
        {
            return NewMessageElementDispose(name, RobotRaconteurNode.s.PackVarType(val));
        }

        public static MessageElement PackMapType<K, T>(string name, Dictionary<K, T> val)
        {
            return NewMessageElementDispose(name, RobotRaconteurNode.s.PackMapType<K, T>(val));
        }

        public static MessageElement PackListType<T>(string name, List<T> val)
        {
            return NewMessageElementDispose(name, RobotRaconteurNode.s.PackListType<T>(val));
        }

        public static MessageElement PackEnum<T>(string name, T val)
        {
            return NewMessageElementDispose(name, new int[] { (int)(object)val });
        }

        public static MessageElement PackPodToArray<T>(string name, ref T val) where T : struct
        {
            return NewMessageElementDispose(name, RobotRaconteurNode.s.PackPodToArray<T>(ref val));
        }

        public static MessageElement PackPodArray<T>(string name, T[] val) where T : struct
        {
            return NewMessageElementDispose(name, RobotRaconteurNode.s.PackPodArray<T>(val));
        }

        public static MessageElement PackPodMultiDimArray<T>(string name, PodMultiDimArray val) where T : struct
        {
            return NewMessageElementDispose(name, RobotRaconteurNode.s.PackPodMultiDimArray<T>(val));
        }

        public static MessageElement PackNamedArrayToArray<T>(string name, ref T val) where T : struct
        {
            return NewMessageElementDispose(name, RobotRaconteurNode.s.PackNamedArrayToArray<T>(ref val));
        }

        public static MessageElement PackNamedArray<T>(string name, T[] val) where T : struct
        {
            return NewMessageElementDispose(name, RobotRaconteurNode.s.PackNamedArray<T>(val));
        }

        public static MessageElement PackNamedMultiDimArray<T>(string name, NamedMultiDimArray val) where T : struct
        {
            return NewMessageElementDispose(name, RobotRaconteurNode.s.PackNamedMultiDimArray<T>(val));
        }

        public static T UnpackScalar<T>(MessageElement m) where T : struct
        {
            T[] a = CastDataAndDispose<T[]>(m);
            if (a.Length != 1) throw new DataTypeException("Invalid scalar");
            return a[0];
        }

        public static T[] UnpackArray<T>(MessageElement m) where T : struct
        {
            T[] a = CastDataAndDispose<T[]>(m);
            if (a == null) throw new NullReferenceException();
            return a;
        }

        public static MultiDimArray UnpackMultiDimArray(MessageElement m)
        {
            MultiDimArray a = RobotRaconteurNode.s.UnpackMultiDimArrayDispose(MessageElementUtil.CastDataAndDispose<MessageElementMultiDimArray>(m));
            if (a == null) throw new NullReferenceException();
            return a;
        }

        public static string UnpackString(MessageElement m)
        {
            string s = MessageElementUtil.CastDataAndDispose<string>(m);
            if (s == null) throw new NullReferenceException();
            return s;
        }

        public static T UnpackStructure<T>(MessageElement m)
        {
            return RobotRaconteurNode.s.UnpackStructureDispose<T>(MessageElementUtil.CastDataAndDispose<MessageElementStructure>(m));
        }

        public static object UnpackVarType(MessageElement m)
        {
            return RobotRaconteurNode.s.UnpackVarTypeDispose(m);
        }

        public static Dictionary<K, T> UnpackMap<K, T>(MessageElement m)
        {
            return (Dictionary<K, T>)RobotRaconteurNode.s.UnpackMapTypeDispose<K, T>(m.Data);
        }

        public static List<T> UnpackList<T>(MessageElement m)
        {
            return (List<T>)RobotRaconteurNode.s.UnpackListTypeDispose<T>(m.Data);
        }

        public static T UnpackEnum<T>(MessageElement m)
        {
            int[] a = CastDataAndDispose<int[]>(m);
            if (a.Length != 1) throw new DataTypeException("Invalid enum");
            return (T)(object)a[0];
        }

        public static T UnpackPodFromArray<T>(MessageElement m) where T : struct
        {
            return RobotRaconteurNode.s.UnpackPodFromArrayDispose<T>(CastDataAndDispose<MessageElementPodArray>(m));
        }

        public static T[] UnpackPodArray<T>(MessageElement m) where T : struct
        {
            return RobotRaconteurNode.s.UnpackPodArrayDispose<T>(CastDataAndDispose<MessageElementPodArray>(m));
        }

        public static PodMultiDimArray UnpackPodMultiDimArray<T>(MessageElement m) where T : struct
        {
            return RobotRaconteurNode.s.UnpackPodMultiDimArrayDispose<T>(MessageElementUtil.CastDataAndDispose<MessageElementPodMultiDimArray>(m));
        }

        public static T UnpackNamedArrayFromArray<T>(MessageElement m) where T : struct
        {
            return RobotRaconteurNode.s.UnpackNamedArrayFromArrayDispose<T>(CastDataAndDispose<MessageElementNamedArray>(m));
        }

        public static T[] UnpackNamedArray<T>(MessageElement m) where T : struct
        {
            return RobotRaconteurNode.s.UnpackNamedArrayDispose<T>(CastDataAndDispose<MessageElementNamedArray>(m));
        }

        public static NamedMultiDimArray UnpackNamedMultiDimArray<T>(MessageElement m) where T : struct
        {
            return RobotRaconteurNode.s.UnpackNamedMultiDimArrayDispose<T>(MessageElementUtil.CastDataAndDispose<MessageElementNamedMultiDimArray>(m));
        }
    }


    public partial class MessageElementDataUtil
    {
        public static Array RRBaseArrayToArray(RRBaseArray i)
        {
            if (i == null) return null;
            DataTypes type = i.GetTypeID();
            Array o = null;
            switch (type)
            {
                case DataTypes.double_t:
                    {
                        var o2 = new double[i.Length()];
                        MessageElementDataUtil.RRBaseArrayToDoubles(i, o2, o2.Length);
                        return o2;
                    }
                case DataTypes.single_t:
                    {
                        var o2 = new float[i.Length()];
                        MessageElementDataUtil.RRBaseArrayToFloats(i, o2, o2.Length);
                        return o2;
                    }                    
                case DataTypes.int8_t:
                    {
                        var o2 = new sbyte[i.Length()];
                        MessageElementDataUtil.RRBaseArrayToBytes(i, o2, o2.Length);
                        return o2;
                    }
                case DataTypes.uint8_t:
                    {
                        var o2 = new byte[i.Length()];
                        MessageElementDataUtil.RRBaseArrayToBytes(i, o2, o2.Length);
                        return o2;
                    }
                case DataTypes.int16_t:
                    {
                        var o2 = new short[i.Length()];
                        MessageElementDataUtil.RRBaseArrayToShorts(i, o2, o2.Length);
                        return o2;
                    }
                case DataTypes.uint16_t:
                    {
                        var o2 = new ushort[i.Length()];
                        MessageElementDataUtil.RRBaseArrayToShorts(i, o2, o2.Length);
                        return o2;
                    }
                case DataTypes.int32_t:
                    {
                        var o2 = new int[i.Length()];
                        MessageElementDataUtil.RRBaseArrayToInts(i, o2, o2.Length);
                        return o2;
                    }
                case DataTypes.uint32_t:
                    {
                        var o2 = new uint[i.Length()];
                        MessageElementDataUtil.RRBaseArrayToInts(i, o2, o2.Length);
                        return o2;
                    }
                case DataTypes.int64_t:
                    {
                        var o2 = new long[i.Length()];
                        MessageElementDataUtil.RRBaseArrayToLongs(i, o2, o2.Length);
                        return o2;
                    }
                case DataTypes.uint64_t:
                    {
                        var o2 = new ulong[i.Length()];
                        MessageElementDataUtil.RRBaseArrayToLongs(i, o2, o2.Length);
                        return o2;
                    }
                case DataTypes.cdouble_t:
                    {
                        var o2 = new double[i.Length()*2];
                        MessageElementDataUtil.RRBaseArrayComplexToDoubles(i, o2, o2.Length);
                        var o3 = new CDouble[i.Length()];
                        for (int j=0; j < o3.Length; j++)
                        {
                            o3[j] = new CDouble(o2[j * 2], o2[j * 2 + 1]);
                        }
                        return o3;
                    }
                case DataTypes.csingle_t:
                    {
                        var o2 = new float[i.Length() * 2];
                        MessageElementDataUtil.RRBaseArrayComplexToFloats(i, o2, o2.Length);
                        var o3 = new CSingle[i.Length()];
                        for (int j = 0; j < o3.Length; j++)
                        {
                            o3[j] = new CSingle(o2[j * 2], o2[j * 2 + 1]);
                        }
                        return o3;
                    }
                case DataTypes.bool_t:
                    {
                        var o2 = new byte[i.Length()];
                        MessageElementDataUtil.RRBaseArrayBoolToBytes(i, o2, o2.Length);
                        var o3 = new bool[o2.Length];
                        for (int j = 0; j < o3.Length; j++)
                        {
                            o3[j] = o2[j] != 0;
                        }
                        return o3;
                    }              
                default:
                    throw new DataTypeException("Invalid RRBaseArray type");
                }            
        }

        public static RRBaseArray ArrayToRRBaseArray(Array i)
        {
            if (i == null) return null;
            DataTypes type = DataTypeUtil.TypeIDFromString(RobotRaconteurNode.GetTypeString(i.GetType().GetElementType()));
            
            /*GCHandle h = GCHandle.Alloc(i, GCHandleType.Pinned);
            rr_memcpy.memcpy(o.void_ptr(), h.AddrOfPinnedObject(), i.Length * o.ElementSize());
            h.Free();*/

            switch (type)
            {
                case DataTypes.double_t:
                    {
                        var a = (double[])i;
                        return MessageElementDataUtil.DoublesToRRBaseArray(a, a.Length);
                    }
                case DataTypes.single_t:
                    {
                        var a = (float[])i;
                        return MessageElementDataUtil.FloatsToRRBaseArray(a, a.Length);
                    }
                case DataTypes.int8_t:
                    {
                        var a = (sbyte[])i;
                        return MessageElementDataUtil.BytesToRRBaseArray(a, a.Length, DataTypes.int8_t);
                    }
                case DataTypes.uint8_t:
                    {
                        var a = (byte[])i;
                        return MessageElementDataUtil.BytesToRRBaseArray(a, a.Length);
                    }
                case DataTypes.int16_t:
                    {
                        var a = (short[])i;
                        return MessageElementDataUtil.ShortsToRRBaseArray(a, a.Length, DataTypes.int16_t);
                    }
                case DataTypes.uint16_t:
                    {
                        var a = (ushort[])i;
                        return MessageElementDataUtil.ShortsToRRBaseArray(a, a.Length);
                    }
                case DataTypes.int32_t:
                    {
                        var a = (int[])i;
                        return MessageElementDataUtil.IntsToRRBaseArray(a, a.Length, DataTypes.int32_t);
                    }
                case DataTypes.uint32_t:
                    {
                        var a = (uint[])i;
                        return MessageElementDataUtil.IntsToRRBaseArray(a, a.Length);
                    }
                case DataTypes.int64_t:
                    {
                        var a = (long[])i;
                        return MessageElementDataUtil.LongsToRRBaseArray(a, a.Length, DataTypes.int64_t);
                    }
                case DataTypes.uint64_t:
                    {
                        var a = (ulong[])i;
                        return MessageElementDataUtil.LongsToRRBaseArray(a, a.Length);
                    }
                case DataTypes.cdouble_t:
                    {
                        var a = (CDouble[])i;
                        var b = new double[a.Length * 2];
                        for (int j = 0; j<a.Length; j++)
                        {
                            b[j * 2] = a[j].Real;
                            b[j * 2 + 1] = a[j].Imag;
                        }
                        return MessageElementDataUtil.DoublesToComplexRRBaseArray(b, b.Length);
                    }
                case DataTypes.csingle_t:
                    {
                        var a = (CSingle[])i;
                        var b = new float[a.Length * 2];
                        for (int j = 0; j < a.Length; j++)
                        {
                            b[j * 2] = a[j].Real;
                            b[j * 2 + 1] = a[j].Imag;
                        }
                        return MessageElementDataUtil.FloatsToComplexRRBaseArray(b, b.Length);
                    }
                case DataTypes.bool_t:
                    {
                        var a = (bool[])i;
                        var b = new byte[a.Length];
                        for (int j = 0; j < a.Length; j++)
                        {
                            b[j] = a[j] ? (byte)1 : (byte)0;                    
                        }
                        return MessageElementDataUtil.BytesToBoolRRBaseArray(b, b.Length);
                    }
                default:                
                    throw new DataTypeException("Invalid RRBaseArray type");
            }           
        }

        public static RRBaseArray ArrayToRRBaseArray(Array i, RRBaseArray o)
        {
            if (i == null) return null;
            DataTypes type = DataTypeUtil.TypeIDFromString(RobotRaconteurNode.GetTypeString(i.GetType().GetElementType()));
            if (i.Length != o.Length()) throw new Exception("Length mismatch");
            if (o.GetTypeID() != type) throw new Exception("Type mismatch");
            /*GCHandle h = GCHandle.Alloc(i, GCHandleType.Pinned);
            rr_memcpy.memcpy(o.void_ptr(), h.AddrOfPinnedObject(), i.Length * o.ElementSize());
            h.Free();*/

            switch (type)
            {
                case DataTypes.double_t:
                    {
                        var a = (double[])i;
                        MessageElementDataUtil.DoublesToRRBaseArray(a, a.Length, o);
                        return o;
                    }                    
                case DataTypes.single_t:
                    {
                        var a = (float[])i;
                        MessageElementDataUtil.FloatsToRRBaseArray(a, a.Length, o);
                        return o;
                    }
                case DataTypes.int8_t:
                    {
                        var a = (sbyte[])i;
                        MessageElementDataUtil.BytesToRRBaseArray(a, a.Length, o);
                        return o;
                    }
                case DataTypes.uint8_t:
                    {
                        var a = (byte[])i;
                        MessageElementDataUtil.BytesToRRBaseArray(a, a.Length, o);
                        return o;
                    }
                case DataTypes.int16_t:
                    {
                        var a = (short[])i;
                        MessageElementDataUtil.ShortsToRRBaseArray(a, a.Length, o);
                        return o;
                    }
                case DataTypes.uint16_t:
                    {
                        var a = (ushort[])i;
                        MessageElementDataUtil.ShortsToRRBaseArray(a, a.Length, o);
                        return o;
                    }
                case DataTypes.int32_t:
                    {
                        var a = (int[])i;
                        MessageElementDataUtil.IntsToRRBaseArray(a, a.Length, o);
                        return o;
                    }
                case DataTypes.uint32_t:
                    {
                        var a = (uint[])i;
                        MessageElementDataUtil.IntsToRRBaseArray(a, a.Length, o);
                        return o;
                    }
                case DataTypes.int64_t:
                    {
                        var a = (long[])i;
                        MessageElementDataUtil.LongsToRRBaseArray(a, a.Length, o);
                        return o;
                    }
                case DataTypes.uint64_t:
                    {
                        var a = (ulong[])i;
                        MessageElementDataUtil.LongsToRRBaseArray(a, a.Length, o);
                        return o;
                    }
                case DataTypes.cdouble_t:
                    {
                        var a = (CDouble[])i;
                        var b = new double[a.Length * 2];
                        for (int j = 0; j < a.Length; j++)
                        {
                            b[j * 2] = a[j].Real;
                            b[j * 2 + 1] = a[j].Imag;
                        }
                        MessageElementDataUtil.DoublesToComplexRRBaseArray(b, b.Length, o);
                        return o;
                    }
                case DataTypes.csingle_t:
                    {
                        var a = (CSingle[])i;
                        var b = new float[a.Length * 2];
                        for (int j = 0; j < a.Length; j++)
                        {
                            b[j * 2] = a[j].Real;
                            b[j * 2 + 1] = a[j].Imag;
                        }
                        MessageElementDataUtil.FloatsToComplexRRBaseArray(b, b.Length, o);
                        return o;
                    }
                case DataTypes.bool_t:
                    {
                        var a = (bool[])i;
                        var b = new byte[a.Length];
                        for (int j = 0; j < a.Length; j++)
                        {
                            b[j] = a[j] ? (byte)1 : (byte)0;
                        }
                        MessageElementDataUtil.BytesToBoolRRBaseArray(b, b.Length, o);
                        return o;
                    }
                default:
                    throw new DataTypeException("Invalid RRBaseArray type");
            }

            return o;
        }

        /*public static string RRBaseArrayToString(RRBaseArray i)
        {
            DataTypes type = i.GetTypeID();
            byte[] o1 = new byte[i.Length()];

            /*GCHandle h = GCHandle.Alloc(o1, GCHandleType.Pinned);
            rr_memcpy.memcpy(h.AddrOfPinnedObject(), i.void_ptr(), i.Length() * i.ElementSize());
            h.Free();*/
            /*Marshal.Copy(i.void_ptr(), o1, 0, o1.Length);
            return System.Text.UTF8Encoding.UTF8.GetString(o1);
        }*/

        /*public static RRBaseArray stringToRRBaseArray(string i)
        {
            byte[] i1 = System.Text.UTF8Encoding.UTF8.GetBytes(i);
            DataTypes type = DataTypes.string_t;
            RRBaseArray o = RobotRaconteurNET.AllocateRRArrayByType(type, (uint)i1.Length);
            /*GCHandle h = GCHandle.Alloc(i1, GCHandleType.Pinned);
            rr_memcpy.memcpy(o.void_ptr(), h.AddrOfPinnedObject(), i1.Length * o.ElementSize());
            h.Free();*/
            /*Marshal.Copy(i1, 0, o.void_ptr(), i1.Length);
            return o;
        }*/


    }


    class WrappedRobotRaconteurExceptionHelper : RobotRaconteurExceptionHelper
    {

        public WrappedRobotRaconteurExceptionHelper() : base()
        {

        }

        public override void SetRobotRaconteurException(MessageErrorType errorcode, string errorname, string errorstring)
        {
            using (MessageEntry e = new MessageEntry())
            {
                Exception ex = RobotRaconteurExceptionUtil.ErrorCodeToException(errorcode, errorname, errorstring);
                RobotRaconteurNETPINVOKE.SWIGPendingException.Set(ex);
            }
        }

    }

    public partial class RobotRaconteurNode
    {
        private static RobotRaconteurNode csharp_s = null;
        static private WrappedRobotRaconteurExceptionHelper exhelp;
        public static RobotRaconteurNode s
        {
            get
            {
                if (csharp_s == null)
                {
                    csharp_s = _get_s();
                    exhelp = new WrappedRobotRaconteurExceptionHelper();
                    RobotRaconteurNET.robotRaconteurExceptionHelper = exhelp;
                }

                return csharp_s;
            }
        }

        public NodeID NodeID
        {
            get
            {
                return _NodeID();
            }
            set
            {
                _SetNodeID(value);
            }
        }

        public string NodeName
        {
            get
            {
                return _NodeName();
            }
            set
            {
                _SetNodeName(value);
            }
        }

        public int ThreadPoolCount
        {
            get
            {
                return _GetThreadPoolCount();
            }
            set
            {
                _SetThreadPoolCount(value);
            }
        }

        private Dictionary<string, ServiceFactory> servicetypes = new Dictionary<string, ServiceFactory>();

        public void RegisterServiceType(ServiceFactory servicetype)
        {
            lock (servicetypes)
            {
                servicetypes.Add(servicetype.GetServiceName(), servicetype);
                _RegisterServiceType(servicetype.ServiceDef());
            }
        }

        public ServiceFactory GetServiceType(string servicename)
        {
            lock (servicetypes)
            {
                /*try
                {
                    if (!servicetypes.ContainsKey(servicename))
                    {
                        string factoryname = servicename + "." + servicename.Replace(".", "__") + "Factory";
                        Type factorytype = Type.GetType(factoryname);
                        if (factorytype != null)
                        {
                            ServiceFactory f = (ServiceFactory)Activator.CreateInstance(factorytype);
                            if (f.GetServiceName() == servicename)
                            {
                                servicetypes.Add(f.GetServiceName(), f);
                            }
                        }
                    }
                }
                catch { }*/


                try
                {
                    return servicetypes[servicename];
                }
                catch (KeyNotFoundException)
                {
                    throw new ApplicationException("Service type " + servicename + " not found");
                }

            }
        }

        public ServiceFactory GetServiceFactory(string servicename)
        {
            return GetServiceType(servicename);
        }




        public delegate void ClientServiceListenerDelegate(ServiceStub client, ClientServiceListenerEventType ev, object parameter);

        public object ConnectService(string url, string username = null, Dictionary<string, object> credentials = null, ClientServiceListenerDelegate listener = null, string objecttype = null)
        {

            MessageElementData credentials2 = null;
            try
            {
                if (username == null) username = "";
                if (credentials != null)
                {
                    credentials2 = (MessageElementData)PackVarType(credentials);
                }

                ClientServiceListenerDirectorNET listener2 = null;
                if (listener != null)
                {
                    listener2 = new ClientServiceListenerDirectorNET(listener);
                    listener2.objectheapid = RRObjectHeap.AddObject(listener2);
                }

                if (objecttype == null) objecttype = "";

                WrappedServiceStub s = _ConnectService(url, username, credentials2, listener2, objecttype);
                ServiceStub s2 = GetServiceType(s.RR_objecttype.GetServiceDefinition().Name).CreateStub(s);
                if (listener2 != null)
                {
                    listener2.stub = s2;



                }
                return s2;
            }
            finally
            {
                if (credentials2 != null) credentials2.Dispose();
            }

        }

        public object ConnectService(string[] url, string username = null, Dictionary<string, object> credentials = null, ClientServiceListenerDelegate listener = null, string objecttype = null)
        {
            MessageElementData credentials2 = null;

            try
            {
                if (username == null) username = "";
                if (credentials != null)
                {
                    credentials2 = (MessageElementData)PackVarType(credentials);
                }

                ClientServiceListenerDirectorNET listener2 = null;
                if (listener != null)
                {
                    listener2 = new ClientServiceListenerDirectorNET(listener);
                    listener2.objectheapid = RRObjectHeap.AddObject(listener2);
                }

                vectorstring url2 = new vectorstring();
                foreach (string ss in url) url2.Add(ss);

                if (objecttype == null) objecttype = "";

                WrappedServiceStub s = _ConnectService(url2, username, credentials2, listener2, objecttype);
                ServiceStub s2 = GetServiceType(s.RR_objecttype.GetServiceDefinition().Name).CreateStub(s);
                if (listener2 != null)
                {
                    listener2.stub = s2;


                }
                return s2;
            }
            finally
            {
                if (credentials2 != null) credentials2.Dispose();
            }

        }

        public void DisconnectService(object obj)
        {
            ServiceStub stub = (ServiceStub)obj;
            _DisconnectService(stub.rr_innerstub);
        }

        public void AsyncConnectService(string url, string username, Dictionary<string, object> credentials, ClientServiceListenerDelegate listener, string objecttype, Action<object, Exception> handler, int timeout = RR_TIMEOUT_INFINITE)
        {

            MessageElementData credentials2 = null;
            try
            {
                if (username == null) username = "";
                if (credentials != null)
                {
                    credentials2 = (MessageElementData)PackVarType(credentials);
                }

                ClientServiceListenerDirectorNET listener2 = null;
                if (listener != null)
                {
                    listener2 = new ClientServiceListenerDirectorNET(listener);
                    listener2.objectheapid = RRObjectHeap.AddObject(listener2);
                }

                if (objecttype == null) objecttype = "";

                AsyncStubReturnDirectorImpl<object> h = new AsyncStubReturnDirectorImpl<object>(delegate (object o1, Exception e1, object o2) { handler(o1, e1); }, null);
                int id = RRObjectHeap.AddObject(h);
                _AsyncConnectService(url, username, credentials2, listener2, objecttype, timeout, h, id);

            }
            finally
            {
                if (credentials2 != null) credentials2.Dispose();
            }

        }

        public void AsyncConnectService(string[] url, string username, Dictionary<string, object> credentials, ClientServiceListenerDelegate listener, string objecttype, Action<object, Exception> handler, int timeout = RR_TIMEOUT_INFINITE)
        {
            MessageElementData credentials2 = null;

            try
            {
                if (username == null) username = "";
                if (credentials != null)
                {
                    credentials2 = (MessageElementData)PackVarType(credentials);
                }

                ClientServiceListenerDirectorNET listener2 = null;
                if (listener != null)
                {
                    listener2 = new ClientServiceListenerDirectorNET(listener);
                    listener2.objectheapid = RRObjectHeap.AddObject(listener2);
                }

                vectorstring url2 = new vectorstring();
                foreach (string ss in url) url2.Add(ss);

                if (objecttype == null) objecttype = "";

                AsyncStubReturnDirectorImpl<object> h = new AsyncStubReturnDirectorImpl<object>(delegate (object o1, Exception e1, object o2) { handler(o1, e1); }, null);
                int id = RRObjectHeap.AddObject(h);
                _AsyncConnectService(url2, username, credentials2, listener2, objecttype, timeout, h, id);
            }
            finally
            {
                if (credentials2 != null) credentials2.Dispose();
            }

        }

        public void AsyncDisconnectService(object obj, Action handler)
        {
            ServiceStub stub = (ServiceStub)obj;
            AsyncVoidNoErrReturnDirectorImpl h = new AsyncVoidNoErrReturnDirectorImpl(delegate (object o) { handler(); }, null);
            int id = RRObjectHeap.AddObject(h);
            _AsyncDisconnectService(stub.rr_innerstub, h, id);
        }

        public Dictionary<string, object> GetServiceAttributes(object obj)
        {
            ServiceStub stub = (ServiceStub)obj;
            using (MessageElement me = _GetServiceAttributes(stub.rr_innerstub))
            {
                return (Dictionary<string, object>)UnpackVarType(me);
            }
        }

        private ServiceFactory GetServiceFactoryForType(string type)
        {
            string servicename = SplitQualifiedName(type).Item1;
            return GetServiceType(servicename);
        }

        private ServiceFactory GetServiceFactoryForType(Type type)
        {
            string servicename = SplitQualifiedName(GetTypeString(type)).Item1;
            return GetServiceType(servicename);
        }

        // Structure Packing
        public MessageElementStructure PackStructure(object s)
        {
            if (s == null) return null;
            return GetServiceFactoryForType(s.GetType()).PackStructure(s);
        }

        public T UnpackStructure<T>(MessageElementStructure l)
        {
            if (l == null) return default(T);
            return GetServiceFactoryForType(l.Type).UnpackStructure<T>(l);
        }

        public T UnpackStructureDispose<T>(MessageElementStructure l)
        {
            using (l)
            {
                return UnpackStructure<T>(l);
            }
        }

        // Pod Packing

        public MessageElementPodArray PackPodToArray<T>(ref T s) where T : struct
        {
            return GetServiceFactoryForType(s.GetType()).PackPodToArray(ref s);
        }

        public T UnpackPodFromArray<T>(MessageElementPodArray l) where T : struct
        {
            return GetServiceFactoryForType(l.Type).UnpackPodFromArray<T>(l);
        }

        public T UnpackPodFromArrayDispose<T>(MessageElementPodArray l) where T : struct
        {
            using (l)
            {
                return UnpackPodFromArray<T>(l);
            }
        }

        // Pod Array Packing

        public MessageElementPodArray PackPodArray<T>(T[] s) where T : struct
        {
            if (s == null) return null;
            return GetServiceFactoryForType(s.GetType()).PackPodArray(s);
        }

        public T[] UnpackPodArray<T>(MessageElementPodArray l) where T : struct
        {
            if (l == null) return null;
            return GetServiceFactoryForType(l.Type).UnpackPodArray<T>(l);
        }

        public T[] UnpackPodArrayDispose<T>(MessageElementPodArray l) where T : struct
        {
            using (l)
            {
                return UnpackPodArray<T>(l);
            }
        }

        // Pod MultiDimArray Packing

        public MessageElementPodMultiDimArray PackPodMultiDimArray<T>(PodMultiDimArray s) where T : struct
        {
            if (s == null) return null;
            return GetServiceFactoryForType(s.pod_array.GetType()).PackPodMultiDimArray<T>(s);
        }

        public PodMultiDimArray UnpackPodMultiDimArray<T>(MessageElementPodMultiDimArray l) where T : struct
        {
            if (l == null) return null;
            return GetServiceFactoryForType(l.Type).UnpackPodMultiDimArray<T>(l);
        }

        public PodMultiDimArray UnpackPodMultiDimArrayDispose<T>(MessageElementPodMultiDimArray l) where T : struct
        {
            using (l)
            {
                return UnpackPodMultiDimArray<T>(l);
            }
        }

        // Pod boxed packing

        public MessageElementData PackPod(object s)
        {
            var t = s.GetType();

            if (t.IsValueType)
            {
                return GetServiceFactoryForType(t).PackPod(s);
            }
            else if (t.IsArray)
            {
                return GetServiceFactoryForType(t.GetElementType()).PackPod(s);
            }
            else if (t == typeof(PodMultiDimArray))
            {
                return GetServiceFactoryForType(((PodMultiDimArray)s).pod_array.GetType().GetElementType()).PackPod(s);
            }
            else if (t == typeof(NamedMultiDimArray))
            {
                return GetServiceFactoryForType(((NamedMultiDimArray)s).namedarray_array.GetType().GetElementType()).PackNamedArray(s);
            }
            throw new DataTypeException("Invalid pod object");
        }

        public object UnpackPod(MessageElementData l)
        {
            return GetServiceFactoryForType(l.GetTypeString()).UnpackPod(l);
        }

        public object UnpackPodDispose(MessageElementData l)
        {
            using (l)
            {
                return UnpackPod(l);
            }
        }

        // NamedArray Packing

        public MessageElementNamedArray PackNamedArrayToArray<T>(ref T s) where T : struct
        {
            return GetServiceFactoryForType(s.GetType()).PackNamedArrayToArray(ref s);
        }

        public T UnpackNamedArrayFromArray<T>(MessageElementNamedArray l) where T : struct
        {
            return GetServiceFactoryForType(l.Type).UnpackNamedArrayFromArray<T>(l);
        }

        public T UnpackNamedArrayFromArrayDispose<T>(MessageElementNamedArray l) where T : struct
        {
            using (l)
            {
                return UnpackNamedArrayFromArray<T>(l);
            }
        }

        // NamedArray Array Packing

        public MessageElementNamedArray PackNamedArray<T>(T[] s) where T : struct
        {
            if (s == null) return null;
            return GetServiceFactoryForType(s.GetType()).PackNamedArray(s);
        }

        public T[] UnpackNamedArray<T>(MessageElementNamedArray l) where T : struct
        {
            if (l == null) return null;
            return GetServiceFactoryForType(l.Type).UnpackNamedArray<T>(l);
        }

        public T[] UnpackNamedArrayDispose<T>(MessageElementNamedArray l) where T : struct
        {
            using (l)
            {
                return UnpackNamedArray<T>(l);
            }
        }

        // Pod MultiDimArray Packing

        public MessageElementNamedMultiDimArray PackNamedMultiDimArray<T>(NamedMultiDimArray s) where T : struct
        {
            if (s == null) return null;
            return GetServiceFactoryForType(s.namedarray_array.GetType()).PackNamedMultiDimArray<T>(s);
        }

        public NamedMultiDimArray UnpackNamedMultiDimArray<T>(MessageElementNamedMultiDimArray l) where T : struct
        {
            if (l == null) return null;
            return GetServiceFactoryForType(l.Type).UnpackNamedMultiDimArray<T>(l);
        }

        public NamedMultiDimArray UnpackNamedMultiDimArrayDispose<T>(MessageElementNamedMultiDimArray l) where T : struct
        {
            using (l)
            {
                return UnpackNamedMultiDimArray<T>(l);
            }
        }

        // Pod boxed packing

        public MessageElementData PackNamedArray(object s)
        {
            var t = s.GetType();

            if (t.IsValueType)
            {
                return GetServiceFactoryForType(t).PackNamedArray(s);
            }
            else if (t.IsArray)
            {
                return GetServiceFactoryForType(t.GetElementType()).PackNamedArray(s);
            }
            else if (t == typeof(NamedMultiDimArray))
            {
                return GetServiceFactoryForType(((NamedMultiDimArray)s).namedarray_array.GetType().GetElementType()).PackNamedArray(s);
            }
            throw new DataTypeException("Invalid pod object");
        }

        public object UnpackNamedArray(MessageElementData l)
        {
            return GetServiceFactoryForType(l.GetTypeString()).UnpackNamedArray(l);
        }

        public object UnpackNamedArrayDispose(MessageElementData l)
        {
            using (l)
            {
                return UnpackNamedArray(l);
            }
        }

        private MessageElement PackContainerValue<T>(string name, ref T data)
        {
            Type t = typeof(T);

            if (t == typeof(object))
            {
                return MessageElementUtil.NewMessageElementDispose(name, PackVarType((object)data));
            }

            bool is_array = t.IsArray;
            if (!(t.IsValueType || !EqualityComparer<T>.Default.Equals(data, default(T))))
            {
                return MessageElementUtil.NewMessageElementDispose(name, null);
            }

            if (t.IsPrimitive || (is_array && t.GetElementType().IsPrimitive))
            {
                return MessageElementUtil.NewMessageElementDispose(name, data);
            }

            if (t == typeof(string))
            {
                return MessageElementUtil.NewMessageElementDispose(name, data);
            }

            if (t == typeof(CDouble) || t == typeof(CSingle))
            {
                return MessageElementUtil.NewMessageElementDispose(name, data);
            }

            if (is_array)
            {
                var t2 = t.GetElementType();
                if (t2 == typeof(CDouble) || t2 == typeof(CSingle))
                {
                    return MessageElementUtil.NewMessageElementDispose(name, data);
                }
            }

            if (t == typeof(MultiDimArray))
            {
                return MessageElementUtil.NewMessageElementDispose(name, PackMultiDimArray((MultiDimArray)(object)data));
            }

            if (t == typeof(PodMultiDimArray))
            {
                return MessageElementUtil.NewMessageElementDispose(name, PackPod((object)data));
            }

            if (t == typeof(NamedMultiDimArray))
            {
                return MessageElementUtil.NewMessageElementDispose(name, PackNamedArray((object)data));
            }

            if (t.IsGenericType)
            {
                throw new DataTypeException("Invalid Robot Raconteur container value type");
            }

            if (!t.IsValueType && !is_array && t != typeof(PodMultiDimArray) && t != typeof(NamedMultiDimArray))
            {
                return MessageElementUtil.NewMessageElementDispose(name, PackStructure(data));
            }
            else
            {
                Type t2 = t;
                if (t.IsArray) t2 = t.GetElementType();
                if (Attribute.GetCustomAttribute(t2, typeof(NamedArrayElementTypeAndCount), false) != null)
                {
                    return MessageElementUtil.NewMessageElementDispose(name, PackNamedArray(data));
                }
                else
                {
                    return MessageElementUtil.NewMessageElementDispose(name, PackPod(data));
                }
            }
        }

        private MessageElement PackContainerValue<T>(int num, ref T data)
        {
            var m = PackContainerValue("", ref data);
            m.ElementNumber = num;
            m.ElementFlags &= ((byte)~RobotRaconteurNET.MessageElementFlags_ELEMENT_NAME_STR);
            m.ElementFlags |= ((byte)RobotRaconteurNET.MessageElementFlags_ELEMENT_NUMBER);
            return m;
        }

        private T UnpackContainerValue<T>(MessageElement e)
        {
            switch (e.ElementType)
            {
                case DataTypes.void_t:
                    if (typeof(T).IsValueType)
                        throw new DataTypeException("Primitive types may not be null");
                    return default(T);
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
                    if (typeof(T).IsArray)
                    {
                        return (T)e.Data;
                    }
                    else
                    {
                        return (typeof(T) == typeof(object)) ? (T)e.Data : ((T[])e.Data)[0];
                    }
                case DataTypes.string_t:
                    return (T)e.Data;
                case DataTypes.multidimarray_t:
                    using (MessageElementMultiDimArray md = (MessageElementMultiDimArray)e.Data)
                    {
                        return (T)(object)UnpackMultiDimArray(md);
                    }
                case DataTypes.structure_t:
                    using (MessageElementStructure md = (MessageElementStructure)e.Data)
                    {
                        return UnpackStructure<T>(md);
                    }
                /*case DataTypes.pod_t:
                    using (MessageElementData md = (MessageElementData)e.Data)
                    {
                        return (T)UnpackPod(md);
                    }*/
                case DataTypes.pod_array_t:
                    using (MessageElementPodArray md = (MessageElementPodArray)e.Data)
                    {
                        if (typeof(T).IsValueType)
                        {
                            if (md.Elements.Count != 1) throw new DataTypeException("Invalid array size for scalar structure");

                            return ((T[])UnpackPod(md))[0];

                        }
                        else
                        {
                            return (T)UnpackPod(md);
                        }
                    }
                case DataTypes.pod_multidimarray_t:
                    using (MessageElementData md = (MessageElementData)e.Data)
                    {
                        return (T)UnpackPod(md);
                    }
                case DataTypes.namedarray_array_t:
                    using (MessageElementNamedArray md = (MessageElementNamedArray)e.Data)
                    {
                        if (typeof(T).IsValueType)
                        {
                            if (md.Elements.Count != 1) throw new DataTypeException("Invalid array size for scalar structure");

                            return ((T[])UnpackNamedArray(md))[0];

                        }
                        else
                        {
                            return (T)UnpackNamedArray(md);
                        }
                    }
                case DataTypes.namedarray_multidimarray_t:
                    using (MessageElementData md = (MessageElementData)e.Data)
                    {
                        return (T)UnpackNamedArray(md);
                    }
                default:
                    throw new DataTypeException("Invalid container data type");
            }
        }

        private T UnpackContainerValue<T>(MessageElement e, out string name)
        {
            name = e.ElementName;
            return UnpackContainerValue<T>(e);
        }

        private T UnpackContainerValue<T>(MessageElement e, out int num)
        {
            num = MessageElementUtil.GetMessageElementNumber(e);
            return UnpackContainerValue<T>(e);
        }

        //Map type packing

        public object PackMapType<Tkey, Tvalue>(object data)
        {
            if (data == null) return null;

            if (typeof(Tkey) == typeof(Int32))
            {
                using (vectorptr_messageelement m = new vectorptr_messageelement())
                {
                    Dictionary<Tkey, Tvalue> ddata = (Dictionary<Tkey, Tvalue>)data;

                    foreach (KeyValuePair<Tkey, Tvalue> d in ddata)
                    {
                        var v = d.Value;
                        MessageElementUtil.AddMessageElementDispose(m, PackContainerValue(Convert.ToInt32(d.Key), ref v));
                    }
                    return new MessageElementMap_int32_t(m);
                }

            }

            if (typeof(Tkey) == typeof(String))
            {
                using (vectorptr_messageelement m = new vectorptr_messageelement())
                {
                    Dictionary<Tkey, Tvalue> ddata = (Dictionary<Tkey, Tvalue>)data;

                    foreach (KeyValuePair<Tkey, Tvalue> d in ddata)
                    {
                        var v = d.Value;
                        MessageElementUtil.AddMessageElementDispose(m, PackContainerValue(d.Key.ToString(), ref v));
                    }
                    return new MessageElementMap_string(m);
                }
            }

            throw new DataTypeException("Indexed types can only be indexed by int32 and string");

        }


        public object UnpackMapType<Tkey, Tvalue>(object data)
        {
            if (data == null) return null;

            if (data is MessageElementMap_int32_t)
            {
                Dictionary<int, Tvalue> o = new Dictionary<int, Tvalue>();

                MessageElementMap_int32_t cdata = (MessageElementMap_int32_t)data;
                using (vectorptr_messageelement cdataElements = cdata.Elements)
                {
                    foreach (MessageElement e in cdataElements)
                    {
                        using (e)
                        {
                            int num;
                            var val = UnpackContainerValue<Tvalue>(e, out num);
                            o.Add(num, val);
                        }
                    }
                    return o;
                }
            }
            else if (data is MessageElementMap_string)
            {
                Dictionary<string, Tvalue> o = new Dictionary<string, Tvalue>();

                MessageElementMap_string cdata = (MessageElementMap_string)data;
                using (vectorptr_messageelement cdataElements = cdata.Elements)
                {
                    foreach (MessageElement e in cdataElements)
                    {
                        using (e)
                        {
                            string name;
                            var val = UnpackContainerValue<Tvalue>(e, out name);
                            o.Add(name, val);
                        }
                    }
                    return o;
                }
            }
            else
            {
                throw new DataTypeException("Indexed types can only be indexed by int32 and string");
            }

        }

        public object UnpackMapTypeDispose<Tkey, Tvalue>(object data)
        {
            try
            {
                return UnpackMapType<Tkey, Tvalue>(data);
            }
            finally
            {
                IDisposable d = data as IDisposable;
                if (d != null) d.Dispose();
            }

        }

        public object PackListType<Tvalue>(object data)
        {
            if (data == null) return null;
           
            using (vectorptr_messageelement m = new vectorptr_messageelement())
            {
                List<Tvalue> ddata = (List<Tvalue>)data;

                int count = 0;
                foreach (Tvalue d in ddata)
                {
                    var v = d;
                    MessageElementUtil.AddMessageElementDispose(m, PackContainerValue(count, ref v));
                    count++;
                }

                return new MessageElementList(m);
            }
        }

        public object UnpackListType<Tvalue>(object data)
        {
            if (data == null) return null;
            List<Tvalue> o = new List<Tvalue>();
            int count = 0;
            MessageElementList cdata = (MessageElementList)data;
            using (vectorptr_messageelement cdataElements = cdata.Elements)
            {
                foreach (MessageElement e in cdataElements)
                {
                    using (e)
                    {
                        int num;
                        var val = UnpackContainerValue<Tvalue>(e, out num);
                        if (count != num) throw new DataTypeException("Error in list format");
                        o.Add(val);
                        count++;
                    }
                }
                return o;
            }
        }

        public object UnpackListTypeDispose<Tvalue>(object data)
        {
            try
            {
                return UnpackListType<Tvalue>(data);
            }
            finally
            {
                IDisposable d = data as IDisposable;
                if (d != null) d.Dispose();
            }

        }

        /// <summary>
        /// Packs a varvalue data.  This can handle any type supported by the node
        /// </summary>
        /// <param name="data">The data to be packed</param>
        /// <returns>The packed data for use with MessageElement.Data</returns>
        public object PackVarType(object data)
        {
            if (data == null) return null;

            Type t = data.GetType();

            if (t == typeof(Dictionary<int, object>))
            {
                return PackMapType<int, object>(data);

            }
            else if (t == typeof(Dictionary<string, object>))
            {
                return PackMapType<string, object>(data);

            }
            else if (t == typeof(List<object>))
            {
                return PackListType<object>(data);

            }

            bool is_array = t.IsArray;

            if (t.IsPrimitive || (is_array && t.GetElementType().IsPrimitive))
            {
                return data;
            }

            if (t == typeof(string))
            {
                return data;
            }

            if (t == typeof(MultiDimArray))
            {
                return PackMultiDimArray((MultiDimArray)data);
            }

            if (t == typeof(PodMultiDimArray))
            {
                return PackPod(data);
            }

            if (t.IsGenericType)
            {
                throw new DataTypeException("Invalid Robot Raconteur varvalue type");
            }

            if (!t.IsValueType && !is_array && t != typeof(PodMultiDimArray) && t != typeof(NamedMultiDimArray))
            {
                return PackStructure(data);
            }
            else
            {
                Type t2 = t;
                if (t.IsArray) t2 = t.GetElementType();
                if (Attribute.GetCustomAttribute(t2, typeof(NamedArrayElementTypeAndCount), false) != null)
                {
                    return PackNamedArray(data);
                }
                else
                {
                    return PackPod(data);
                }
            }

        }

        /// <summary>
        /// Unpacks a varvalue from a MessageElement.  This can unpack any type supported by the node 
        /// </summary>
        /// <param name="me">The message element containing the data</param>
        /// <returns>The unpacked data</returns>
        public object UnpackVarType(MessageElement me)
        {
            if (me == null) return null;

            switch (me.ElementType)
            {
                case DataTypes.void_t:
                    return null;
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
                case DataTypes.string_t:
                    return me.Data;
                case DataTypes.multidimarray_t:
                    using (MessageElementMultiDimArray md = (MessageElementMultiDimArray)me.Data)
                    {
                        return UnpackMultiDimArray(md);
                    }
                case DataTypes.structure_t:
                    using (MessageElementStructure md = (MessageElementStructure)me.Data)
                    {
                        return UnpackStructure<object>(md);
                    }
                //case DataTypes.pod_t:
                case DataTypes.pod_array_t:
                case DataTypes.pod_multidimarray_t:
                    using (MessageElementData md = (MessageElementData)me.Data)
                    {
                        return UnpackPod(md);
                    }
                case DataTypes.namedarray_array_t:
                case DataTypes.namedarray_multidimarray_t:
                    using (MessageElementData md = (MessageElementData)me.Data)
                    {
                        return UnpackNamedArray(md);
                    }
                case DataTypes.vector_t:
                    using (MessageElementData md = (MessageElementData)me.Data)
                    {
                        return UnpackMapType<int, object>(md);
                    }
                case DataTypes.dictionary_t:
                    using (MessageElementData md = (MessageElementData)me.Data)
                    {
                        return UnpackMapType<string, object>(md);
                    }
                case DataTypes.list_t:
                    using (MessageElementData md = (MessageElementData)me.Data)
                    {
                        return UnpackListType<object>(md);
                    }
                default:
                    throw new DataTypeException("Invalid varvalue data type");
            }
        }

        public object UnpackVarTypeDispose(MessageElement me)
        {
            try
            {
                return UnpackVarType(me);
            }
            finally
            {
                if (me != null)
                {
                    me.Dispose();
                }
            }
        }

        /// <summary>
        /// Packs a MultiDimArray into a MessageElementMultiDimArray
        /// </summary>
        /// <param name="array">The array to be packed</param>
        /// <returns>A packed array for use with MessageElement.Data</returns>
        public MessageElementMultiDimArray PackMultiDimArray(MultiDimArray array)
        {
            if (array == null) return null;
            using (vectorptr_messageelement l = new vectorptr_messageelement())
            {
                MessageElementUtil.AddMessageElementDispose(l, "dims", array.Dims);                
                MessageElementUtil.AddMessageElementDispose(l, "array", array.Array_);
                
                return new MessageElementMultiDimArray(l);
            }


        }

        /// <summary>
        /// Unpacks a MessageElementMultiDimArray and returns unpacked multidim array
        /// </summary>
        /// <param name="marray">The MessageElementMultiDimArray to unpack</param>
        /// <returns>The unpacked multidim array</returns>
        public MultiDimArray UnpackMultiDimArray(MessageElementMultiDimArray marray)
        {
            if (marray == null) return null;

            MultiDimArray m = new MultiDimArray();
            using (vectorptr_messageelement marrayElements = marray.Elements)
            {                
                m.Dims = (MessageElementUtil.FindElementAndCast<uint[]>(marrayElements, "dims"));
                m.Array_ = (MessageElementUtil.FindElementAndCast<Array>(marrayElements, "array"));
                return m;
            }


        }

        public MultiDimArray UnpackMultiDimArrayDispose(MessageElementMultiDimArray marray)
        {
            try
            {
                return UnpackMultiDimArray(marray);
            }
            finally
            {
                if (marray != null)
                    marray.Dispose();
            }


        }

        public string RequestObjectLock(object obj, RobotRaconteurObjectLockFlags flags)
        {
            if (!(obj is ServiceStub)) throw new ArgumentException("Can only lock object opened through Robot Raconteur");
            ServiceStub s = (ServiceStub)obj;

            return _RequestObjectLock(s.rr_innerstub, flags);


        }



        public string ReleaseObjectLock(object obj)
        {
            if (!(obj is ServiceStub)) throw new ArgumentException("Can only unlock object opened through Robot Raconteur");
            ServiceStub s = (ServiceStub)obj;

            return _ReleaseObjectLock(s.rr_innerstub);


        }

        public void AsyncRequestObjectLock(object obj, RobotRaconteurObjectLockFlags flags, Action<string, Exception> handler, int timeout = RR_TIMEOUT_INFINITE)
        {
            if (!(obj is ServiceStub)) throw new ArgumentException("Can only lock object opened through Robot Raconteur");
            ServiceStub s = (ServiceStub)obj;

            AsyncStringReturnDirectorImpl h = new AsyncStringReturnDirectorImpl(delegate (string s1, Exception e1, object o1) { handler(s1, e1); }, null);
            int id = RRObjectHeap.AddObject(h);

            _AsyncRequestObjectLock(s.rr_innerstub, flags, timeout, h, id);


        }

        public void AsyncReleaseObjectLock(object obj, Action<string, Exception> handler, int timeout = RR_TIMEOUT_INFINITE)
        {
            if (!(obj is ServiceStub)) throw new ArgumentException("Can only unlock object opened through Robot Raconteur");
            ServiceStub s = (ServiceStub)obj;

            AsyncStringReturnDirectorImpl h = new AsyncStringReturnDirectorImpl(delegate (string s1, Exception e1, object o1) { handler(s1, e1); }, null);
            int id = RRObjectHeap.AddObject(h);
            _AsyncReleaseObjectLock(s.rr_innerstub, timeout, h, id);


        }

        public void MonitorEnter(object obj, int timeout = -1)
        {
            if (!(obj is ServiceStub)) throw new ArgumentException("Only service stubs can be monitored by RobotRaconteurNode");
            ServiceStub s = (ServiceStub)obj;

            _MonitorEnter(s.rr_innerstub, timeout);
        }

        public void MonitorExit(object obj)
        {
            if (!(obj is ServiceStub)) throw new ArgumentException("Only service stubs can be monitored by RobotRaconteurNode");
            ServiceStub s = (ServiceStub)obj;

            _MonitorExit(s.rr_innerstub);
        }

        public class ScopedMonitorLock : IDisposable
        {
            object obj;
            bool locked;

            public ScopedMonitorLock(object obj, int timeout = -1)
            {
                RobotRaconteurNode.s.MonitorEnter(obj, timeout);
                locked = true;
            }

            public void lock_(Int32 timeout = -1)
            {
                if (obj == null) return;
                if (locked) throw new Exception("Already locked");
                RobotRaconteurNode.s.MonitorEnter(obj, timeout);
                locked = true;
            }

            public void unlock(Int32 timeout = -1)
            {
                if (obj == null) return;
                if (!locked) throw new Exception("Not locked");
                RobotRaconteurNode.s.MonitorExit(timeout);
                locked = false;
            }

            public void release()
            {
                obj = null;
            }

            public void Dispose()
            {
                if (locked) unlock();
                obj = null;
            }

            ~ScopedMonitorLock()
            {
                Dispose();
            }
        }

        public ServiceInfo2[] FindServiceByType(string servicetype, string[] transportschemes)
        {
            vectorstring s = new vectorstring();
            foreach (string s2 in transportschemes) s.Add(s2);
            vectorserviceinfo2wrapped i = RobotRaconteurNET.WrappedFindServiceByType(this, servicetype, s);

            List<ServiceInfo2> o = new List<ServiceInfo2>();
            foreach (ServiceInfo2Wrapped i2 in i)
            {

                o.Add(new ServiceInfo2(i2));
            }

            return o.ToArray();
        }

        public NodeInfo2[] FindNodeByID(NodeID id, string[] transportschemes)
        {
            vectorstring s = new vectorstring();
            foreach (string s2 in transportschemes) s.Add(s2);
            vectornodeinfo2 i = RobotRaconteurNET.WrappedFindNodeByID(this, id, s);

            List<NodeInfo2> o = new List<NodeInfo2>();
            foreach (WrappedNodeInfo2 i2 in i)
            {
                o.Add(new NodeInfo2(i2));
            }

            return o.ToArray();
        }

        public NodeInfo2[] FindNodeByName(string name, string[] transportschemes)
        {
            vectorstring s = new vectorstring();
            foreach (string s2 in transportschemes) s.Add(s2);
            vectornodeinfo2 i = RobotRaconteurNET.WrappedFindNodeByName(this, name, s);

            List<NodeInfo2> o = new List<NodeInfo2>();
            foreach (WrappedNodeInfo2 i2 in i)
            {
                o.Add(new NodeInfo2(i2));
            }

            return o.ToArray();
        }


        private class AsyncServiceInfo2DirectorImpl : AsyncServiceInfo2VectorReturnDirector
        {
            protected Action<ServiceInfo2[]> handler_func;

            public AsyncServiceInfo2DirectorImpl(Action<ServiceInfo2[]> handler_func)
            {
                this.handler_func = handler_func;
            }

            public override void handler(vectorserviceinfo2wrapped i)
            {
                try
                {
                    List<ServiceInfo2> o = new List<ServiceInfo2>();
                    try
                    {
                        using (i)
                        {

                            foreach (ServiceInfo2Wrapped i2 in i)
                            {

                                o.Add(new ServiceInfo2(i2));
                            }

                        }
                    }
                    catch (Exception)
                    {
                        handler_func(new ServiceInfo2[0]);
                        return;
                    }

                    handler_func(o.ToArray());
                }
                catch (Exception e)
                {
                    using (MessageEntry merr = new MessageEntry())
                    {
                        RobotRaconteurExceptionUtil.ExceptionToMessageEntry(e, merr);
                        RRDirectorExceptionHelper.SetError(merr);
                    }
                }
            }
        }

        private class AsyncNodeInfo2DirectorImpl : AsyncNodeInfo2VectorReturnDirector
        {
            protected Action<NodeInfo2[]> handler_func;

            public AsyncNodeInfo2DirectorImpl(Action<NodeInfo2[]> handler_func)
            {
                this.handler_func = handler_func;
            }

            public override void handler(vectornodeinfo2 i)
            {
                try
                {
                    List<NodeInfo2> o = new List<NodeInfo2>();
                    try
                    {
                        using (i)
                        {

                            foreach (WrappedNodeInfo2 i2 in i)
                            {

                                o.Add(new NodeInfo2(i2));
                            }
                        }
                    }
                    catch (Exception)
                    {
                        handler_func(new NodeInfo2[0]);
                        return;
                    }

                    handler_func(o.ToArray());
                }
                catch (Exception e)
                {
                    using (MessageEntry merr = new MessageEntry())
                    {
                        RobotRaconteurExceptionUtil.ExceptionToMessageEntry(e, merr);
                        RRDirectorExceptionHelper.SetError(merr);
                    }
                }
            }
        }

        public const int RR_TIMEOUT_INFINITE = -1;

        public void AsyncFindServiceByType(string servicetype, string[] transportschemes, Action<ServiceInfo2[]> handler, int timeout = 5000)
        {
            vectorstring s = new vectorstring();
            foreach (string s2 in transportschemes) s.Add(s2);

            AsyncServiceInfo2DirectorImpl h = new AsyncServiceInfo2DirectorImpl(handler);
            int id = RRObjectHeap.AddObject(h);

            RobotRaconteurNET.AsyncWrappedFindServiceByType(this, servicetype, s, timeout, h, id);

        }

        public void AsyncFindNodeByID(NodeID id, string[] transportschemes, Action<NodeInfo2[]> handler, int timeout = 5000)
        {
            vectorstring s = new vectorstring();
            foreach (string s2 in transportschemes) s.Add(s2);

            AsyncNodeInfo2DirectorImpl h = new AsyncNodeInfo2DirectorImpl(handler);
            int id2 = RRObjectHeap.AddObject(h);

            RobotRaconteurNET.AsyncWrappedFindNodeByID(this, id, s, timeout, h, id2);

        }

        public void AsyncFindNodeByName(string name, string[] transportschemes, Action<NodeInfo2[]> handler, int timeout = 5000)
        {
            vectorstring s = new vectorstring();
            foreach (string s2 in transportschemes) s.Add(s2);

            AsyncNodeInfo2DirectorImpl h = new AsyncNodeInfo2DirectorImpl(handler);
            int id2 = RRObjectHeap.AddObject(h);

            RobotRaconteurNET.AsyncWrappedFindNodeByName(this, name, s, timeout, h, id2);

        }

        public void UpdateDetectedNodes(string[] schemes)
        {
            vectorstring schemes1 = new vectorstring();
            foreach (string s in schemes) schemes1.Add(s);
            RobotRaconteurNET.WrappedUpdateDetectedNodes(this, schemes1);
        }

        public void AsyncUpdateDetectedNodes(string[] schemes, Action handler, int timeout = 5000)
        {
            vectorstring schemes1 = new vectorstring();
            foreach (string s in schemes) schemes1.Add(s);
            AsyncVoidNoErrReturnDirectorImpl h = new AsyncVoidNoErrReturnDirectorImpl(delegate (object o) { handler(); }, null);
            int id2 = RRObjectHeap.AddObject(h);
            RobotRaconteurNET.AsyncWrappedUpdateDetectedNodes(this, schemes1, timeout, h, id2);
        }

        public NodeID[] GetDetectedNodes()
        {
            vectorstring o1 = RobotRaconteurNET.WrappedGetDetectedNodes(this);
            NodeID[] o = new NodeID[o1.Count];
            for (int i = 0; i < o.Length; i++)
            {
                o[i] = new NodeID(o1[i]);
            }
            return o;
        }

        public ServerContext RegisterService(string name, string servicetype, object obj, ServiceSecurityPolicy policy = null)
        {
            ServiceSkel skel = GetServiceType(servicetype).CreateSkel(obj);

            int id = RRObjectHeap.AddObject(skel);
            skel.innerskelid = id;
            WrappedRRObject o = new WrappedRRObject(skel.RRType, skel, id);


            return _RegisterService(name, servicetype, o, policy);
        }

        public DateTime NowUTC
        {
            get
            {
                return _NowUTC();
            }
        }

        public object FindObjRefTyped(object obj, string objref, string objecttype)
        {
            if (!(obj is ServiceStub)) throw new ArgumentException("Only service stubs can have objref");
            ServiceStub s = (ServiceStub)obj;

            return s.FindObjRefTyped(objref, objecttype);
        }

        public object FindObjRefTyped(object obj, string objref, string index, string objecttype)
        {
            if (!(obj is ServiceStub)) throw new ArgumentException("Only service stubs can have objref");
            ServiceStub s = (ServiceStub)obj;

            return s.FindObjRefTyped(objref, index, objecttype);
        }

        public void AsyncFindObjRefTyped(object obj, string objref, string objecttype, Action<object, Exception> handler, int timeout = RR_TIMEOUT_INFINITE)
        {
            if (!(obj is ServiceStub)) throw new ArgumentException("Only service stubs can have objref");
            ServiceStub s = (ServiceStub)obj;

            s.AsyncFindObjRefTyped<object>(objref, objecttype, handler, timeout);
        }

        public void AsyncFindObjRefTyped(object obj, string objref, string index, string objecttype, Action<object, Exception> handler, int timeout = RR_TIMEOUT_INFINITE)
        {
            if (!(obj is ServiceStub)) throw new ArgumentException("Only service stubs can have objref");
            ServiceStub s = (ServiceStub)obj;

            s.AsyncFindObjRefTyped<object>(objref, index, objecttype, handler, timeout);
        }

        public string FindObjectType(object obj, string objref)
        {
            if (!(obj is ServiceStub)) throw new ArgumentException("Only service stubs can have objref");
            ServiceStub s = (ServiceStub)obj;

            return _FindObjectType(s.rr_innerstub, objref);
        }

        public string FindObjectType(object obj, string objref, string index)
        {
            if (!(obj is ServiceStub)) throw new ArgumentException("Only service stubs can have objref");
            ServiceStub s = (ServiceStub)obj;

            return _FindObjectType(s.rr_innerstub, objref, index);
        }

        public void AsyncFindObjectType(object obj, string objref, Action<string, Exception> handler, int timeout = RR_TIMEOUT_INFINITE)
        {
            if (!(obj is ServiceStub)) throw new ArgumentException("Only service stubs can have objref");
            ServiceStub s = (ServiceStub)obj;

            Action<string, Exception, object> h2 = delegate (string s2, Exception e, object o) { handler(s2, e); };
            AsyncStringReturnDirectorImpl h = new AsyncStringReturnDirectorImpl(h2, null);
            int id2 = RRObjectHeap.AddObject(h);
            _AsyncFindObjectType(s.rr_innerstub, objref, timeout, h, id2);
        }

        public void AsyncFindObjectType(object obj, string objref, string index, Action<string, Exception> handler, int timeout = RR_TIMEOUT_INFINITE)
        {
            if (!(obj is ServiceStub)) throw new ArgumentException("Only service stubs can have objref");
            ServiceStub s = (ServiceStub)obj;

            Action<string, Exception, object> h2 = delegate (string s2, Exception e, object o) { handler(s2, e); };
            AsyncStringReturnDirectorImpl h = new AsyncStringReturnDirectorImpl(h2, null);
            int id2 = RRObjectHeap.AddObject(h);
            _AsyncFindObjectType(s.rr_innerstub, objref, index, timeout, h, id2);
        }

        public uint RequestTimeout
        {
            get
            {
                return _GetRequestTimeout();
            }
            set
            {
                _SetRequestTimeout(value);
            }
        }

        public uint TransportInactivityTimeout
        {
            get
            {
                return _GetTransportInactivityTimeout();
            }
            set
            {
                _SetTransportInactivityTimeout(value);
            }
        }

        public uint EndpointInactivityTimeout
        {
            get
            {
                return _GetEndpointInactivityTimeout();
            }
            set
            {
                _SetEndpointInactivityTimeout(value);
            }
        }

        public uint MemoryMaxTransferSize
        {
            get
            {
                return _GetMemoryMaxTransferSize();
            }
            set
            {
                _SetMemoryMaxTransferSize(value);
            }
        }

        public uint NodeDiscoveryMaxCacheCount
        {
            get
            {
                return _GetNodeDiscoveryMaxCacheCount();
            }
            set
            {
                _SetNodeDiscoveryMaxCacheCount(value);
            }
        }

        public void Shutdown()
        {

            this._Shutdown();
            RRNativeObjectHeapSupport.Set_Support(null);
        }

        public static Tuple<string, string> SplitQualifiedName(string name)
        {
            int pos = name.LastIndexOf('.');
            if (pos == -1) throw new ArgumentException("Name is not qualified");
            string[] o = new string[2];
            return Tuple.Create(name.Substring(0, pos), name.Substring(pos + 1, name.Length - pos - 1));
        }

        public static string GetTypeString(Type type)
        {
            return type.ToString().Replace("_.", ".").TrimEnd(new char[] { '_' });
        }

        public void SetExceptionHandler(Action<Exception> handler)
        {
            if (handler == null)
            {
                _ClearExceptionHandler();
                return;
            }
            AsyncExceptionDirectorImpl h = new AsyncExceptionDirectorImpl(handler);
            int id1 = RRObjectHeap.AddObject(h);
            _SetExceptionHandler(h, id1);
        }

        public Timer CreateTimer(int period, Action<TimerEvent> handler, bool oneshot = false)
        {
            AsyncTimerEventReturnDirectorImpl t = new AsyncTimerEventReturnDirectorImpl(handler);
            int id = RRObjectHeap.AddObject(t);
            return _CreateTimer(period, oneshot, t, id);
        }

        public RobotRaconteurException DownCastException(RobotRaconteurException exp)
        {
            if (exp == null) return exp;
            string type = exp.Error;
            if (!type.Contains(".")) return exp;
            var stype = RobotRaconteurNode.SplitQualifiedName(type);
            if (!IsServiceTypeRegistered(stype.Item1)) return exp;
            return GetServiceType(stype.Item1).DownCastException(exp);

        }

        public void PostToThreadPool(Action target)
        {
            AsyncVoidNoErrReturnDirectorImpl h = new AsyncVoidNoErrReturnDirectorImpl(delegate (object param) { target(); }, null);
            int id = RRObjectHeap.AddObject(h);
            _PostToThreadPool(h, id);
        }

        public string RobotRaconteurVersion
        {
            get
            {
                return this._GetRobotRaconteurVersion();
            }
        }
    }

    public class vectorptr_messageelement_util
    {
        public static MessageElement FindElement(vectorptr_messageelement m, string name)
        {
            foreach (MessageElement m2 in m)
            {
                if (m2.ElementName == name) return m2;
                else m2.Dispose();
            }
            throw new MessageElementNotFoundException("Message element " + name + " not found");
        }

    }

    public class ServiceInfo2
    {
        public string Name;
        public string RootObjectType;
        public string[] RootObjectImplements;
        public string[] ConnectionURL;
        public Dictionary<string, object> Attributes;
        public NodeID NodeID;
        public string NodeName;

        public ServiceInfo2() { }

        public ServiceInfo2(ServiceInfo2Wrapped s)
        {
            this.Name = s.Name;
            this.NodeID = new NodeID(s.NodeID.ToString());
            this.NodeName = s.NodeName;
            this.RootObjectType = s.RootObjectType;
            List<string> impl = new List<string>();
            foreach (string impl2 in s.RootObjectImplements) impl.Add(impl2);
            this.RootObjectImplements = impl.ToArray();
            this.Attributes = (Dictionary<string, object>)RobotRaconteurNode.s.UnpackVarType(s.Attributes);
            if (s.Attributes != null) s.Attributes.Dispose();
            List<string> con = new List<string>();
            foreach (string con2 in s.ConnectionURL) con.Add(con2);
            this.ConnectionURL = con.ToArray();
        }

    }

    public class NodeInfo2
    {
        public NodeID NodeID;
        public string NodeName;
        public string[] ConnectionURL;

        public NodeInfo2() { }

        public NodeInfo2(WrappedNodeInfo2 n)
        {
            this.NodeID = new NodeID(n.NodeID.ToString());
            this.NodeName = n.NodeName;
            List<string> con = new List<string>();
            foreach (string con2 in n.ConnectionURL) con.Add(con2);
            this.ConnectionURL = con.ToArray();
        }

    }

    public partial class map_strstr
    {
        public static implicit operator map_strstr(System.Collections.Generic.Dictionary<string, string> i)
        {
            map_strstr o = new map_strstr();
            foreach (System.Collections.Generic.KeyValuePair<string, string> i2 in i) o.Add(i2.Key, i2.Value);

            return o;
        }

    }

    public class RobotRaconteurNativeLoader
    {
        private static bool loaded = false;
        public static void Load()
        {

            if (loaded) return;
            if (Environment.OSVersion.Platform == PlatformID.Win32NT)
            {


                Stream s = null;

                try
                {
                    if (IntPtr.Size == 4)
                    {
                        s = typeof(RobotRaconteurNativeLoader).Assembly.GetManifestResourceStream("RobotRaconteurNETNative_win32");

                    }
                    else
                    {
                        s = typeof(RobotRaconteurNativeLoader).Assembly.GetManifestResourceStream("RobotRaconteurNETNative_win64");
                    }

                    int count = 0;
                    bool ready = false;
                    string path = "";
                    while (!ready)
                    {
                        s.Seek(0, SeekOrigin.Begin);
                        path = Path.Combine(Path.Combine(Path.GetTempPath(), typeof(RobotRaconteurNativeLoader).Assembly.FullName + "-" + IntPtr.Size + "-" + count), "RobotRaconteurNETNative.dll");

                        if (!Directory.Exists(Path.GetDirectoryName(path)))
                        {
                            Directory.CreateDirectory(Path.GetDirectoryName(path));
                        }

                        if (File.Exists(path))
                        {
                            using (Stream file = File.OpenRead(path))
                            {
                                bool same = true;

                                if (file.Length != s.Length) same = false;

                                byte[] buffer1 = new byte[4096];
                                byte[] buffer2 = new byte[4096];
                                int len1 = 0;
                                int len2 = 0;
                                while (((len1 = s.Read(buffer1, 0, buffer1.Length)) > 0) && same)
                                {
                                    len2 = file.Read(buffer2, 0, len1);
                                    if (len1 != len2)
                                    {
                                        same = false;
                                        continue;
                                    }

                                    for (int i = 0; (i < len1 && same); i++)
                                    {
                                        if (buffer1[i] != buffer2[i])
                                        {
                                            same = false;
                                            continue;
                                        }
                                    }

                                }

                                if (same)
                                {
                                    ready = true;
                                }
                                else
                                {
                                    count++;
                                }

                            }




                        }
                        else
                        {

                            using (Stream file = File.OpenWrite(path))
                            {

                                byte[] buffer = new byte[4096];
                                int len = 0;
                                while ((len = s.Read(buffer, 0, buffer.Length)) > 0)
                                {
                                    file.Write(buffer, 0, len);
                                }
                                ready = true;
                            }
                        }
                    }

                    Environment.SetEnvironmentVariable("PATH", Path.GetDirectoryName(path) + ";" + Environment.GetEnvironmentVariable("PATH"));
                    // Console.WriteLine(Environment.GetEnvironmentVariable("PATH"));
                    loaded = true;

                }
                catch { }
                finally
                {
                    if (s != null) s.Close();
                }



            }


        }



    }

    public partial class TimeSpec
    {
        public static bool operator ==(TimeSpec a, TimeSpec b)
        {
            if (Object.Equals(a, null) && Object.Equals(b, null)) return true;
            if (Object.Equals(a, null) || Object.Equals(b, null)) return false;
            return a.eq(b);
        }

        public override bool Equals(Object o)
        {
            if (!(o is TimeSpec)) return false;
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

        public static bool operator >(TimeSpec a, TimeSpec b)
        {
            return a.gt(b);
        }

        public static bool operator <(TimeSpec a, TimeSpec b)
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

    public partial class AuthenticatedUser
    {
        public string Username
        {
            get
            {
                return _GetUsername();
            }
        }

        public string[] Privileges
        {
            get
            {
                vectorstring s = _GetPrivileges();
                string[] o = new string[s.Count];
                s.CopyTo(o);
                return o;
            }
        }

        public string[] Properties
        {
            get
            {
                vectorstring s = _GetProperties();
                string[] o = new string[s.Count];
                s.CopyTo(o);
                return o;
            }
        }

        public DateTime LoginTime
        {
            get
            {
                return _GetLoginTime();

            }
        }

        public DateTime LastAccessTime
        {
            get
            {
                return _GetLastAccessTime();

            }
        }

    }

    internal class AsyncRequestDirectorImpl : AsyncRequestDirector
    {

        protected Action<MessageElement, Exception, object> handler_func;
        protected object param;

        public AsyncRequestDirectorImpl(Action<MessageElement, Exception, object> handler_func, object param)
        {
            this.handler_func = handler_func;
            this.param = param;
        }

        public override void handler(MessageElement m, uint error_code, string errorname, string errormessage)
        {
            using (m)
            {
                try
                {
                    this.Dispose();

                    if (error_code != 0)
                    {
                        using (MessageEntry merr = new MessageEntry())
                        {

                            this.handler_func(null, RobotRaconteurExceptionUtil.ErrorCodeToException((RobotRaconteur.MessageErrorType)error_code, errorname, errormessage), param);
                            return;
                        }
                    }

                    handler_func(m, null, param);

                }
                catch (Exception e)
                {
                    using (MessageEntry merr = new MessageEntry())
                    {
                        RobotRaconteurExceptionUtil.ExceptionToMessageEntry(e, merr);
                        RRDirectorExceptionHelper.SetError(merr);
                    }
                }
            }

        }
    }

    internal class AsyncStubReturnDirectorImpl<T> : AsyncStubReturnDirector
    {

        protected Action<T, Exception, object> handler_func;
        protected object param;
        protected ServiceFactory factory;

        public AsyncStubReturnDirectorImpl(Action<T, Exception, object> handler_func, object param, ServiceFactory f = null)
        {
            this.handler_func = handler_func;
            this.param = param;
            this.factory = f;
        }

        public override void handler(WrappedServiceStub innerstub, uint error_code, string errorname, string errormessage)
        {
            try
            {
                if (error_code != 0)
                {
                    using (MessageEntry merr = new MessageEntry())
                    {

                        this.handler_func(default(T), RobotRaconteurExceptionUtil.ErrorCodeToException((RobotRaconteur.MessageErrorType)error_code, errorname, errormessage), param);
                        return;
                    }
                }

                object s = null;
                T s1 = default(T);
                try
                {
                    int id = innerstub.GetObjectHeapID();
                    if (id != 0)
                    {
                        handler_func((T)RRObjectHeap.GetObject(id), null, param);
                        return;
                    }

                    ServiceFactory f;
                    if (factory == null)
                    {
                        f = RobotRaconteurNode.s.GetServiceType(innerstub.RR_objecttype.GetServiceDefinition().Name);
                    }
                    else
                    {
                        f = factory;
                    }
                    s = f.CreateStub(innerstub);
                    s1 = (T)s;
                }
                catch (Exception e)
                {
                    handler_func(default(T), e, param);
                    return;
                }


                handler_func(s1, null, param);
            }
            catch (Exception e)
            {
                using (MessageEntry merr = new MessageEntry())
                {
                    RobotRaconteurExceptionUtil.ExceptionToMessageEntry(e, merr);
                    RRDirectorExceptionHelper.SetError(merr);
                }
            }
        }


    }

    internal class AsyncVoidReturnDirectorImpl : AsyncVoidReturnDirector
    {
        protected Action<Exception, object> handler_func;
        protected object param;

        public AsyncVoidReturnDirectorImpl(Action<Exception, object> handler_func, object param)
        {
            this.handler_func = handler_func;
            this.param = param;
        }

        public override void handler(uint error_code, string errorname, string errormessage)
        {
            try
            {
                if (error_code != 0)
                {
                    using (MessageEntry merr = new MessageEntry())
                    {

                        this.handler_func(RobotRaconteurExceptionUtil.ErrorCodeToException((RobotRaconteur.MessageErrorType)error_code, errorname, errormessage), param);
                        return;
                    }
                }

                handler_func(null, param);
            }
            catch (Exception e)
            {
                using (MessageEntry merr = new MessageEntry())
                {
                    RobotRaconteurExceptionUtil.ExceptionToMessageEntry(e, merr);
                    RRDirectorExceptionHelper.SetError(merr);
                }
            }

        }

    }


    internal class AsyncVoidNoErrReturnDirectorImpl : AsyncVoidNoErrReturnDirector
    {
        protected Action<object> handler_func;
        protected object param;

        public AsyncVoidNoErrReturnDirectorImpl(Action<object> handler_func, object param)
        {
            this.handler_func = handler_func;
            this.param = param;
        }

        public override void handler()
        {
            try
            {
                handler_func(param);
            }
            catch (Exception e)
            {
                using (MessageEntry merr = new MessageEntry())
                {
                    RobotRaconteurExceptionUtil.ExceptionToMessageEntry(e, merr);
                    RRDirectorExceptionHelper.SetError(merr);
                }
            }

        }

    }

    internal class AsyncStringReturnDirectorImpl : AsyncStringReturnDirector
    {

        protected Action<string, Exception, object> handler_func;
        protected object param;

        public AsyncStringReturnDirectorImpl(Action<string, Exception, object> handler_func, object param)
        {
            this.handler_func = handler_func;
            this.param = param;
        }

        public override void handler(string s, uint error_code, string errorname, string errormessage)
        {
            try
            {
                if (error_code != 0)
                {
                    using (MessageEntry merr = new MessageEntry())
                    {

                        this.handler_func(null, RobotRaconteurExceptionUtil.ErrorCodeToException((RobotRaconteur.MessageErrorType)error_code, errorname, errormessage), param);
                        return;
                    }
                }

                handler_func(s, null, param);
            }
            catch (Exception e)
            {
                using (MessageEntry merr = new MessageEntry())
                {
                    RobotRaconteurExceptionUtil.ExceptionToMessageEntry(e, merr);
                    RRDirectorExceptionHelper.SetError(merr);
                }
            }

        }



    }

    internal class AsyncUInt32ReturnDirectorImpl : AsyncUInt32ReturnDirector
    {

        protected Action<uint, Exception, object> handler_func;
        protected object param;

        public AsyncUInt32ReturnDirectorImpl(Action<uint, Exception, object> handler_func, object param)
        {
            this.handler_func = handler_func;
            this.param = param;
        }

        public override void handler(uint v, uint error_code, string errorname, string errormessage)
        {

            try
            {
                if (error_code != 0)
                {
                    using (MessageEntry merr = new MessageEntry())
                    {
                        this.handler_func(0, RobotRaconteurExceptionUtil.ErrorCodeToException((RobotRaconteur.MessageErrorType)error_code, errorname, errormessage), param);
                        return;
                    }
                }


                handler_func(v, null, param);
            }
            catch (Exception e)
            {
                using (MessageEntry merr = new MessageEntry())
                {
                    RobotRaconteurExceptionUtil.ExceptionToMessageEntry(e, merr);
                    RRDirectorExceptionHelper.SetError(merr);
                }
            }

        }

    }

    internal class AsyncExceptionDirectorImpl : AsyncVoidReturnDirector
    {

        protected Action<Exception> handler_func;

        public AsyncExceptionDirectorImpl(Action<Exception> handler_func)
        {
            this.handler_func = handler_func;

        }

        public override void handler(uint error_code, string errorname, string errormessage)
        {
            try
            {
                if (error_code != 0)
                {
                    using (MessageEntry merr = new MessageEntry())
                    {

                        this.handler_func(RobotRaconteurExceptionUtil.ErrorCodeToException((RobotRaconteur.MessageErrorType)error_code, errorname, errormessage));
                        return;
                    }
                }

                handler_func(new Exception("Unknown exception"));
            }
            catch (Exception e)
            {
                using (MessageEntry merr = new MessageEntry())
                {
                    RobotRaconteurExceptionUtil.ExceptionToMessageEntry(e, merr);
                    RRDirectorExceptionHelper.SetError(merr);
                }
            }

        }

    }

    internal class AsyncTimerEventReturnDirectorImpl : AsyncTimerEventReturnDirector
    {
        protected Action<TimerEvent> handler_func;

        public AsyncTimerEventReturnDirectorImpl(Action<TimerEvent> handler_func)
        {
            this.handler_func = handler_func;
        }

        public override void handler(TimerEvent ev, uint error_code, string errorname, string errormessage)
        {
            try
            {
                handler_func(ev);
            }
            catch (Exception e)
            {
                using (MessageEntry merr = new MessageEntry())
                {
                    RobotRaconteurExceptionUtil.ExceptionToMessageEntry(e, merr);
                    RRDirectorExceptionHelper.SetError(merr);
                }
            }

        }

    }

    public partial class WallTimer
    {



        public WallTimer(int period, Action<TimerEvent> handler, bool oneshot, RobotRaconteurNode node = null)
            : this(period, oneshot, node, AttachHandler(handler))
        {
            if (node == null) node = RobotRaconteurNode.s;

            if (RobotRaconteurNETPINVOKE.SWIGPendingException.Pending) throw RobotRaconteurNETPINVOKE.SWIGPendingException.Retrieve();
        }

        private static WallTimer_initstruct AttachHandler(Action<TimerEvent> handler)
        {
            WallTimer_initstruct s = new WallTimer_initstruct();
            AsyncTimerEventReturnDirectorImpl h = new AsyncTimerEventReturnDirectorImpl(handler);
            s.handler = h;
            s.id = RRObjectHeap.AddObject(h);
            return s;
        }

    }

    // Subscriptions

    public partial class ServiceSubscriptionClientID
    {

        public NodeID NodeID;
        public string ServiceName;

        public ServiceSubscriptionClientID(NodeID node_id, string service_name)
        {
            this.NodeID = node_id;
            this.ServiceName = service_name;
        }

        internal ServiceSubscriptionClientID(WrappedServiceSubscriptionClientID id1)
        {
            this.NodeID = id1.NodeID;
            this.ServiceName = id1.ServiceName;
        }

        public static bool operator ==(ServiceSubscriptionClientID a, ServiceSubscriptionClientID b)
        {
            if (Object.Equals(a, null) && Object.Equals(b, null)) return true;
            if (Object.Equals(a, null) || Object.Equals(b, null)) return false;
            return (a.NodeID == b.NodeID) && (a.ServiceName == b.ServiceName);
        }

        public static bool operator !=(ServiceSubscriptionClientID a, ServiceSubscriptionClientID b)
        {
            return !(a == b);
        }

        public override bool Equals(object o)
        {
            if (!(o is ServiceSubscriptionClientID)) return false;
            return this == ((ServiceSubscriptionClientID)o);
        }

        public override int GetHashCode()
        {
            return (ServiceName?.GetHashCode() ?? 0) + (NodeID?.ToString().GetHashCode() ?? 0);
        }
    }

    public class ServiceSubscriptionFilterNode
    {
        public NodeID NodeID = RobotRaconteur.NodeID.GetAny();
        public string NodeName;
        public string Username;
        public Dictionary<string, object> Credentials;
    }

    public class ServiceSubscriptionFilter
    {
        public ServiceSubscriptionFilterNode[] Nodes;
        public string[] ServiceNames;
        public string[] TransportSchemes;
        public Func<ServiceInfo2, bool> Predicate;
        public uint MaxConnections;
    }

    public class ServiceInfo2Subscription
    {
        class WrappedServiceInfo2SubscriptionDirectorNET : WrappedServiceInfo2SubscriptionDirector
        {
            WeakReference subscription1;

            internal WrappedServiceInfo2SubscriptionDirectorNET(ServiceInfo2Subscription subscription)
            {
                subscription1 = new WeakReference(subscription);
            }

            public override void ServiceDetected(WrappedServiceInfo2Subscription subscription, WrappedServiceSubscriptionClientID id, ServiceInfo2Wrapped info)
            {
                var s = (ServiceInfo2Subscription)subscription1.Target;
                if (s == null) return;
                var info1 = new ServiceInfo2(info);
                var id1 = new ServiceSubscriptionClientID(id);
                if (s.ServiceDetected == null) return;
                try
                {
                    s.ServiceDetected(s, id1, info1);
                }
                catch { }
            }

            public override void ServiceLost(WrappedServiceInfo2Subscription subscription, WrappedServiceSubscriptionClientID id, ServiceInfo2Wrapped info)
            {
                var s = (ServiceInfo2Subscription)subscription1.Target;
                if (s == null) return;
                var info1 = new ServiceInfo2(info);
                var id1 = new ServiceSubscriptionClientID(id);
                if (s.ServiceLost == null) return;
                try
                {
                    s.ServiceLost(s, id1, info1);
                }
                catch { }
            }
        }


        protected WrappedServiceInfo2Subscription _subscription;

        internal ServiceInfo2Subscription(WrappedServiceInfo2Subscription subscription)
        {
            _subscription = subscription;
            var director = new WrappedServiceInfo2SubscriptionDirectorNET(this);
            int id = RRObjectHeap.AddObject(director);
            subscription.SetRRDirector(director, id);
        }

        public void Close()
        {
            _subscription.Close();
        }

        public Dictionary<ServiceSubscriptionClientID, ServiceInfo2> GetDetectedServiceInfo2()
        {
            var o = new Dictionary<ServiceSubscriptionClientID, ServiceInfo2>();

            var c1 = _subscription.GetDetectedServiceInfo2();
            foreach (var c2 in c1)
            {
                var id = new ServiceSubscriptionClientID(c2.Key);
                var stub = new ServiceInfo2(c2.Value);
                o.Add(id, stub);
            }

            return o;
        }

        public event Action<ServiceInfo2Subscription, ServiceSubscriptionClientID, ServiceInfo2> ServiceDetected;
        public event Action<ServiceInfo2Subscription, ServiceSubscriptionClientID, ServiceInfo2> ServiceLost;
    }

    public class ServiceSubscription
    {

        class WrappedServiceSubscriptionDirectorNET : WrappedServiceSubscriptionDirector
        {
            WeakReference subscription1;

            internal WrappedServiceSubscriptionDirectorNET(ServiceSubscription subscription)
            {
                subscription1 = new WeakReference(subscription);
            }

            public override void ClientConnected(WrappedServiceSubscription subscription, WrappedServiceSubscriptionClientID id, WrappedServiceStub client)
            {
                var s = (ServiceSubscription)subscription1.Target;
                if (s == null) return;
                if (s.ClientConnected == null) return;

                var client2 = s.GetClientStub(client);
                try
                {
                    s.ClientConnected(s, new ServiceSubscriptionClientID(id), client2);
                }
                catch { }
            }

            public override void ClientDisconnected(WrappedServiceSubscription subscription, WrappedServiceSubscriptionClientID id, WrappedServiceStub client)
            {
                var s = (ServiceSubscription)subscription1.Target;
                if (s == null) return;
                if (s.ClientDisconnected == null) return;

                var client2 = s.DeleteClientStub(client);

                try
                {
                    s.ClientDisconnected(s, new ServiceSubscriptionClientID(id), client2);
                }
                catch { }
            }
        }

        Dictionary<int, object> client_stubs = new Dictionary<int, object>();

        internal object GetClientStub(WrappedServiceStub innerstub)
        {
            if (innerstub == null) return null;

            lock (this)
            {
                int id = innerstub.GetObjectHeapID();
                object stub;
                if (id != 0 && client_stubs.TryGetValue(id, out stub))
                {
                    return stub;
                }
                else
                {
                    ServiceFactory f;

                    f = RobotRaconteurNode.s.GetServiceType(innerstub.RR_objecttype.GetServiceDefinition().Name);

                    stub = f.CreateStub(innerstub);
                    int id2 = innerstub.GetObjectHeapID();
                    if (id2 == 0) return null;
                    client_stubs.Add(id2, stub);
                    return stub;
                }
            }
        }

        internal object DeleteClientStub(WrappedServiceStub innerstub)
        {
            if (innerstub == null) return null;

            lock (this)
            {
                int id = innerstub.GetObjectHeapID();

                if (id != 0)
                {
                    object stub;
                    if (client_stubs.TryGetValue(id, out stub))
                    {
                        client_stubs.Remove(id);
                        return stub;
                    }
                }
                return null;
            }
        }

        protected WrappedServiceSubscription _subscription;

        internal ServiceSubscription(WrappedServiceSubscription subscription)
        {
            _subscription = subscription;
            var director = new WrappedServiceSubscriptionDirectorNET(this);
            int id = RRObjectHeap.AddObject(director);
            subscription.SetRRDirector(director, id);
        }

        public Dictionary<ServiceSubscriptionClientID, object> GetConnectedClients()
        {
            var o = new Dictionary<ServiceSubscriptionClientID, object>();

            var c1 = _subscription.GetConnectedClients();
            foreach (var c2 in c1)
            {
                var id = new ServiceSubscriptionClientID(c2.Key);
                var stub = GetClientStub(c2.Value);
                o.Add(id, stub);
            }

            return o;
        }

        public void Close()
        {
            _subscription.Close();
        }

        public void ClaimClient(object client)
        {
            ServiceStub s = client as ServiceStub;
            if (s == null) throw new ArgumentException("Invalid object for ClaimClient");

            _subscription.ClaimClient(s.rr_innerstub);
        }

        public void ReleaseClient(object client)
        {
            ServiceStub s = client as ServiceStub;
            if (s == null) throw new ArgumentException("Invalid object for ReleaseClient");

            _subscription.ClaimClient(s.rr_innerstub);
        }

        public uint ConnectRetryDelay
        {
            get
            {
                return _subscription.GetConnectRetryDelay();
            }
            set
            {
                _subscription.SetConnectRetryDelay(value);
            }
        }

        public event Action<ServiceSubscription, ServiceSubscriptionClientID, object> ClientConnected;
        public event Action<ServiceSubscription, ServiceSubscriptionClientID, object> ClientDisconnected;

        public WireSubscription<T> SubscribeWire<T>(string wire_name)
        {
            var s = _subscription.SubscribeWire(wire_name);
            return new WireSubscription<T>(s);
        }

        public PipeSubscription<T> SubscribePipe<T>(string pipe_name, int max_backlog = -1)
        {
            var s = _subscription.SubscribePipe(pipe_name);
            return new PipeSubscription<T>(s);
        }
    }

    public class WireSubscription<T>
    {
        class WrappedWireSubscriptionDirectorNET : WrappedWireSubscriptionDirector
        {
            WeakReference subscription1;

            internal WrappedWireSubscriptionDirectorNET(WireSubscription<T> subscription)
            {
                subscription1 = new WeakReference(subscription);
            }

            public override void WireValueChanged(WrappedWireSubscription subscription, WrappedService_typed_packet value, TimeSpec time)
            {
                using (value)
                {
                    var s = (WireSubscription<T>)subscription1.Target;
                    if (s == null) return;
                    if (s.WireValueChanged == null) return;
                    try
                    {
                        using (var m1 = value.packet)
                        {
                            var v = s.UnpackValue(m1);

                            s.WireValueChanged(s, v, time);
                        }
                    }
                    catch { }
                }
            }

        }


        WrappedWireSubscription _subscription;

        internal WireSubscription(WrappedWireSubscription subscription)
        {
            _subscription = subscription;
            var director = new WrappedWireSubscriptionDirectorNET(this);
            int id = RRObjectHeap.AddObject(director);
            subscription.SetRRDirector(director, id);
        }

        internal T UnpackValue(MessageElement m)
        {
            object data = RobotRaconteurNode.s.UnpackVarType(m);
            if (data is Array)
            {
                if (typeof(T).IsArray)
                    return (T)data;
                else
                    return ((T[])data)[0];
            }
            else
            {
                return (T)data;
            }
        }

        public T InValue
        {
            get
            {
                var m = _subscription.GetInValue();
                using (m)
                {
                    var m1 = m.packet;
                    using (m1)
                    {
                        return UnpackValue(m1);
                    }
                }
            }
        }

        public T GetInValue(out TimeSpec time)
        {
            TimeSpec t = new TimeSpec();
            var m = _subscription.GetInValue(t);
            using (m)
            {
                var m1 = m.packet;
                using (m.packet)
                {
                    time = t;
                    return UnpackValue(m1);
                }
            }
        }

        public bool TryGetInValue(out T value)
        {
            var m = new WrappedService_typed_packet();
            using (m)
            {
                if (_subscription.TryGetInValue(m))
                {
                    var m1 = m.packet;
                    using (m1)
                    {
                        value = UnpackValue(m1);
                    }
                    return true;
                }
                else
                {
                    value = default(T);
                    return false;
                }
            }
        }

        public bool TryGetInValue(out T value, out TimeSpec time)
        {
            var m = new WrappedService_typed_packet();
            var t = new TimeSpec();
            using (m)
            {
                if (_subscription.TryGetInValue(m, t))
                {
                    time = t;
                    using (var m1 = m.packet)
                    {
                        value = UnpackValue(m1);
                        return true;
                    }
                }
                else
                {
                    time = null;
                    value = default(T);
                    return false;
                }
            }
        }

        public bool WaitInValueValid(int timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE)
        {
            return _subscription.WaitInValueValid(timeout);
        }

        public bool IgnoreInValue
        {
            get
            {
                return _subscription.GetIgnoreInValue();
            }
            set
            {
                _subscription.SetIgnoreInValue(value);
            }
        }

        public void SetOutValueAll(T value)
        {
            var iter = new WrappedWireSubscription_send_iterator(_subscription);

            while (iter.Next() != null)
            {
                object dat = null;
                try
                {
                    dat = RobotRaconteurNode.s.PackVarType(value);
                    using (MessageElement m = new MessageElement("value", dat))
                    {
                        iter.SetOutValue(m);
                    }
                }
                finally
                {
                    IDisposable d = dat as IDisposable;
                    if (d != null) d.Dispose();
                }
            }
        }

        public uint ActiveWireConnectionCount
        {
            get
            {
                return (uint)_subscription.GetActiveWireConnectionCount();
            }
        }

        public void Close()
        {
            _subscription.Close();
        }

        public event Action<WireSubscription<T>, T, TimeSpec> WireValueChanged;
    }

    public partial class PipeSubscription<T>
    {
        class WrappedPipeSubscriptionDirectorNET : WrappedPipeSubscriptionDirector
        {
            WeakReference subscription1;

            internal WrappedPipeSubscriptionDirectorNET(PipeSubscription<T> subscription)
            {
                subscription1 = new WeakReference(subscription);
            }

            public override void PipePacketReceived(WrappedPipeSubscription subscription)
            {
                var s = (PipeSubscription<T>)subscription1.Target;
                if (s == null) return;
                if (s.PipePacketReceived == null) return;
                try
                {
                    s.PipePacketReceived(s);
                }
                catch { }
            }

        }


        WrappedPipeSubscription _subscription;

        internal PipeSubscription(WrappedPipeSubscription subscription)
        {
            _subscription = subscription;
            var director = new WrappedPipeSubscriptionDirectorNET(this);
            int id = RRObjectHeap.AddObject(director);
            subscription.SetRRDirector(director, id);
        }

        internal T UnpackValue(MessageElement m)
        {
            object data = RobotRaconteurNode.s.UnpackVarType(m);
            if (data is Array)
            {
                if (typeof(T).IsArray)
                    return (T)data;
                else
                    return ((T[])data)[0];
            }
            else
            {
                return (T)data;
            }
        }

        public T ReceivePacket()
        {
            using (var m = _subscription.ReceivePacket())
            {
                using (var m1 = m.packet)
                {
                    return UnpackValue(m1);
                }
            }
        }

        public bool TryReceivePacket(out T packet)
        {
            using (var m = new WrappedService_typed_packet())
            {

                if (_subscription.TryReceivePacket(m))
                {
                    using (var m1 = m.packet)
                    {

                        packet = UnpackValue(m1);
                        return true;
                    }
                }
                else
                {
                    packet = default(T);
                    return false;
                }
            }
        }

        public bool TryReceivePacketWait(out T packet, int timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE, bool peek = false)
        {
            using (var m = new WrappedService_typed_packet())
            {
                if (_subscription.TryReceivePacketWait(m, timeout, peek))
                {
                    using (var m1 = m.packet)
                    {

                        packet = UnpackValue(m1);
                        return true;
                    }
                }
                else
                {
                    packet = default(T);
                    return false;
                }
            }
        }

        public uint Available
        {
            get
            {
                return _subscription.Available();
            }
        }

        public bool IgnoreReceived
        {
            get
            {
                return _subscription.GetIgnoreReceived();
            }
            set
            {
                _subscription.SetIgnoreReceived(value);
            }
        }

        public void AsyncSendPacketAll(T value)
        {
            var iter = new WrappedPipeSubscription_send_iterator(_subscription);

            while (iter.Next() != null)
            {
                object dat = null;
                try
                {
                    dat = RobotRaconteurNode.s.PackVarType(value);
                    using (MessageElement m = new MessageElement("value", dat))
                    {
                        iter.AsyncSendPacket(m);
                    }
                }
                finally
                {
                    IDisposable d = dat as IDisposable;
                    if (d != null) d.Dispose();
                }
            }
        }

        public uint ActivePipeEndpointCount
        {
            get
            {
                return _subscription.GetActivePipeEndpointCount();
            }
        }

        public void Close()
        {
            _subscription.Close();
        }

        public event Action<PipeSubscription<T>> PipePacketReceived;

    }

    public partial class RobotRaconteurNode
    {

        private class WrappedServiceSubscriptionFilterPredicateDirectorNET : WrappedServiceSubscriptionFilterPredicateDirector
        {
            Func<ServiceInfo2, bool> _f;

            public WrappedServiceSubscriptionFilterPredicateDirectorNET(Func<ServiceInfo2, bool> f)
            {
                if (f == null) throw new NullReferenceException();
                _f = f;
            }

            public override bool Predicate(ServiceInfo2Wrapped info)
            {
                var info2 = new ServiceInfo2(info);
                return _f(info2);
            }

        }

        private WrappedServiceSubscriptionFilter SubscribeService_LoadFilter(ServiceSubscriptionFilter filter)
        {
            WrappedServiceSubscriptionFilter filter2 = null;
            if (filter != null)
            {
                filter2 = new WrappedServiceSubscriptionFilter();
                if (filter.ServiceNames != null) foreach (string s in filter.ServiceNames) filter2.ServiceNames.Add(s);
                if (filter.TransportSchemes != null) foreach (string s in filter.TransportSchemes) filter2.TransportSchemes.Add(s);
                filter2.MaxConnections = filter.MaxConnections;
                if (filter.Nodes != null)
                {
                    var nodes2 = new vectorptr_wrappedservicesubscriptionnode();
                    foreach (var n1 in filter.Nodes)
                    {
                        if (n1 == null) continue;
                        var n2 = new WrappedServiceSubscriptionFilterNode();

                        n2.NodeID = n1.NodeID ?? NodeID.GetAny();
                        n2.NodeName = n1.NodeName ?? "";

                        n2.Username = n1.Username ?? "";
                        if (n1.Credentials != null)
                        {
                            n2.Credentials = (MessageElementData)this.PackMapType<string, object>(n1.Credentials);
                        }

                        nodes2.Add(n2);
                    }

                    filter2.Nodes = nodes2;
                }

                if (filter.Predicate != null)
                {
                    var director = new WrappedServiceSubscriptionFilterPredicateDirectorNET(filter.Predicate);
                    int id = RRObjectHeap.AddObject(director);
                    filter2.SetRRPredicateDirector(director, id);
                }

            }
            return filter2;

        }

        public ServiceInfo2Subscription SubscribeServiceInfo2(string[] service_types, ServiceSubscriptionFilter filter = null)
        {
            var filter2 = SubscribeService_LoadFilter(filter);

            var service_types2 = new vectorstring();
            foreach (string s in service_types) service_types2.Add(s);

            var sub1 = RobotRaconteurNET.WrappedSubscribeServiceInfo2(this, service_types2, filter2);
            return new ServiceInfo2Subscription(sub1);
        }

        public ServiceSubscription SubscribeService(string[] service_types, ServiceSubscriptionFilter filter = null)
        {
            var filter2 = SubscribeService_LoadFilter(filter);

            var service_types2 = new vectorstring();
            foreach (string s in service_types) service_types2.Add(s);

            var sub1 = RobotRaconteurNET.WrappedSubscribeService(this, service_types2, filter2);
            return new ServiceSubscription(sub1);
        }


    }

    public class RobotRaconteurNodeSetup : IDisposable
    {
        public TcpTransport TcpTransport { get; }
        public LocalTransport LocalTransport { get; }
        public HardwareTransport HardwareTransport { get; }

        protected WrappedRobotRaconteurNodeSetup setup;

        public RobotRaconteurNodeSetup(string node_name, ushort tcp_port, RobotRaconteurNodeSetupFlags flags)
        {
            if (node_name == null) node_name = "";
            setup = new WrappedRobotRaconteurNodeSetup(RobotRaconteurNode.s, node_name, tcp_port, (uint)flags);
            TcpTransport = setup.GetTcpTransport();
            LocalTransport = setup.GetLocalTransport();
            HardwareTransport = setup.GetHardwareTransport();

            LoadAllServiceTypes(RobotRaconteurNode.s);
        }

        public void Dispose()
        {
            WrappedRobotRaconteurNodeSetup s;

            lock (this)
            {
                if (setup == null)
                {
                    return;
                }
                s = setup;
                setup = null;
            }

            if (s!=null)
            {
                s.Dispose();
            }
        }

        private void LoadAllServiceTypes(RobotRaconteurNode node)
        {
            // https://stackoverflow.com/questions/13493416/scan-assembly-for-classes-that-implement-certain-interface-and-add-them-to-a-con
            var assignableType = typeof(ServiceFactory);

            var scanners = AppDomain.CurrentDomain.GetAssemblies().ToList()
                .SelectMany(x => x.GetTypes())
                .Where(t => assignableType.IsAssignableFrom(t) && t.IsClass && !t.IsAbstract).ToList();

            foreach (Type type in scanners)
            {
                var service_factory = Activator.CreateInstance(type) as ServiceFactory;
                if (service_factory!=null)
                {
                    node.RegisterServiceType(service_factory);
                }
            }
        }

    }

    public class ClientNodeSetup : RobotRaconteurNodeSetup
    {
        public ClientNodeSetup(string node_name = null, RobotRaconteurNodeSetupFlags flags = RobotRaconteurNodeSetupFlags.CLIENT_DEFAULT)            
            : base(node_name, 0, flags)
        { }
    }

    public class ServerNodeSetup : RobotRaconteurNodeSetup
    {
        public ServerNodeSetup(string node_name, ushort tcp_port, RobotRaconteurNodeSetupFlags flags = RobotRaconteurNodeSetupFlags.SERVER_DEFAULT)
            : base(node_name, tcp_port, flags)
        { }
    }

    public class SecureServerNodeSetup : RobotRaconteurNodeSetup
    {
        public SecureServerNodeSetup(string node_name, ushort tcp_port, RobotRaconteurNodeSetupFlags flags = RobotRaconteurNodeSetupFlags.SECURE_SERVER_DEFAULT)
            : base(node_name, tcp_port, flags)
        { }
    }
}