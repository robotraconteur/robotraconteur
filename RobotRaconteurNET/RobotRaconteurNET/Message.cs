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
            if (val == null)
                return null;
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
                if (ad != null)
                    ad.Dispose();
                throw new DataTypeException("Unknown RRArray type");
            }
            a = MessageElementDataUtil.ToMessageElementNestedElementList(val);
            if (a != null)
                return a;
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
        if (elementtype != DataTypes.void_t && (elementtype < DataTypes.uint64_t || elementtype == DataTypes.bool_t) &&
            !(dat is Array))
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
        get {
            return GetData();
        }
        set {
            SetData(value);
        }
    }

    public T CastData<T>()
    {
        if (Data == null)
            return default(T);
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
            if (m != null)
                m.Dispose();
            throw;
        }
        finally
        {
            IDisposable d = data as IDisposable;
            if (d != null)
                d.Dispose();
        }
    }

    public static MessageElement NewMessageElement(string name, object data)
    {
        MessageElement m = null;

        {
            m = new MessageElement();
            m.ElementName = name;
            m.Data = data;
            return m;
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
            if (m != null)
                m.Dispose();
            throw;
        }
        finally
        {
            IDisposable d = data as IDisposable;
            if (d != null)
                d.Dispose();
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
            if (m != null)
                m.Dispose();
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
            if (dat2 != null)
                dat2.Dispose();
            throw;
        }
        finally
        {
            if (m != null)
                m.Dispose();
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

    public static MessageElement PackScalar<T>(string name, T val)
        where T : struct
    {
        return NewMessageElementDispose(name, new T[] { val });
    }

    public static MessageElement PackArray<T>(string name, T[] val)
        where T : struct
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

    public static MessageElement PackPodToArray<T>(string name, ref T val)
        where T : struct
    {
        return NewMessageElementDispose(name, RobotRaconteurNode.s.PackPodToArray<T>(ref val));
    }

    public static MessageElement PackPodArray<T>(string name, T[] val)
        where T : struct
    {
        return NewMessageElementDispose(name, RobotRaconteurNode.s.PackPodArray<T>(val));
    }

    public static MessageElement PackPodMultiDimArray<T>(string name, PodMultiDimArray val)
        where T : struct
    {
        return NewMessageElementDispose(name, RobotRaconteurNode.s.PackPodMultiDimArray<T>(val));
    }

    public static MessageElement PackNamedArrayToArray<T>(string name, ref T val)
        where T : struct
    {
        return NewMessageElementDispose(name, RobotRaconteurNode.s.PackNamedArrayToArray<T>(ref val));
    }

    public static MessageElement PackNamedArray<T>(string name, T[] val)
        where T : struct
    {
        return NewMessageElementDispose(name, RobotRaconteurNode.s.PackNamedArray<T>(val));
    }

    public static MessageElement PackNamedMultiDimArray<T>(string name, NamedMultiDimArray val)
        where T : struct
    {
        return NewMessageElementDispose(name, RobotRaconteurNode.s.PackNamedMultiDimArray<T>(val));
    }

    public static T UnpackScalar<T>(MessageElement m)
        where T : struct
    {
        T[] a = CastDataAndDispose<T[]>(m);
        if (a.Length != 1)
            throw new DataTypeException("Invalid scalar");
        return a[0];
    }

    public static T[] UnpackArray<T>(MessageElement m)
        where T : struct
    {
        T[] a = CastDataAndDispose<T[]>(m);
        if (a == null)
            throw new NullReferenceException();
        return a;
    }

    public static MultiDimArray UnpackMultiDimArray(MessageElement m)
    {
        MultiDimArray a = RobotRaconteurNode.s.UnpackMultiDimArrayDispose(
            MessageElementUtil.CastDataAndDispose<MessageElementNestedElementList>(m));
        if (a == null)
            throw new NullReferenceException();
        return a;
    }

    public static string UnpackString(MessageElement m)
    {
        string s = MessageElementUtil.CastDataAndDispose<string>(m);
        if (s == null)
            throw new NullReferenceException();
        return s;
    }

    public static T UnpackStructure<T>(MessageElement m)
    {
        return RobotRaconteurNode.s.UnpackStructureDispose<T>(
            MessageElementUtil.CastDataAndDispose<MessageElementNestedElementList>(m));
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
        if (a.Length != 1)
            throw new DataTypeException("Invalid enum");
        return (T)(object)a[0];
    }

    public static T UnpackPodFromArray<T>(MessageElement m)
        where T : struct
    {
        return RobotRaconteurNode.s.UnpackPodFromArrayDispose<T>(
            CastDataAndDispose<MessageElementNestedElementList>(m));
    }

    public static T[] UnpackPodArray<T>(MessageElement m)
        where T : struct
    {
        return RobotRaconteurNode.s.UnpackPodArrayDispose<T>(CastDataAndDispose<MessageElementNestedElementList>(m));
    }

    public static PodMultiDimArray UnpackPodMultiDimArray<T>(MessageElement m)
        where T : struct
    {
        return RobotRaconteurNode.s.UnpackPodMultiDimArrayDispose<T>(
            MessageElementUtil.CastDataAndDispose<MessageElementNestedElementList>(m));
    }

    public static T UnpackNamedArrayFromArray<T>(MessageElement m)
        where T : struct
    {
        return RobotRaconteurNode.s.UnpackNamedArrayFromArrayDispose<T>(
            CastDataAndDispose<MessageElementNestedElementList>(m));
    }

    public static T[] UnpackNamedArray<T>(MessageElement m)
        where T : struct
    {
        return RobotRaconteurNode.s.UnpackNamedArrayDispose<T>(CastDataAndDispose<MessageElementNestedElementList>(m));
    }

    public static NamedMultiDimArray UnpackNamedMultiDimArray<T>(MessageElement m)
        where T : struct
    {
        return RobotRaconteurNode.s.UnpackNamedMultiDimArrayDispose<T>(
            MessageElementUtil.CastDataAndDispose<MessageElementNestedElementList>(m));
    }
}

public partial class MessageElementDataUtil
{
    public static Array RRBaseArrayToArray(RRBaseArray i)
    {
        if (i == null)
            return null;
        DataTypes type = i.GetTypeID();
        Array o = null;
        switch (type)
        {
        case DataTypes.double_t: {
            var o2 = new double[i.size()];
            MessageElementDataUtil.RRBaseArrayToDoubles(i, o2, o2.Length);
            return o2;
        }
        case DataTypes.single_t: {
            var o2 = new float[i.size()];
            MessageElementDataUtil.RRBaseArrayToFloats(i, o2, o2.Length);
            return o2;
        }
        case DataTypes.int8_t: {
            var o2 = new sbyte[i.size()];
            MessageElementDataUtil.RRBaseArrayToBytes(i, o2, o2.Length);
            return o2;
        }
        case DataTypes.uint8_t: {
            var o2 = new byte[i.size()];
            MessageElementDataUtil.RRBaseArrayToBytes(i, o2, o2.Length);
            return o2;
        }
        case DataTypes.int16_t: {
            var o2 = new short[i.size()];
            MessageElementDataUtil.RRBaseArrayToShorts(i, o2, o2.Length);
            return o2;
        }
        case DataTypes.uint16_t: {
            var o2 = new ushort[i.size()];
            MessageElementDataUtil.RRBaseArrayToShorts(i, o2, o2.Length);
            return o2;
        }
        case DataTypes.int32_t: {
            var o2 = new int[i.size()];
            MessageElementDataUtil.RRBaseArrayToInts(i, o2, o2.Length);
            return o2;
        }
        case DataTypes.uint32_t: {
            var o2 = new uint[i.size()];
            MessageElementDataUtil.RRBaseArrayToInts(i, o2, o2.Length);
            return o2;
        }
        case DataTypes.int64_t: {
            var o2 = new long[i.size()];
            MessageElementDataUtil.RRBaseArrayToLongs(i, o2, o2.Length);
            return o2;
        }
        case DataTypes.uint64_t: {
            var o2 = new ulong[i.size()];
            MessageElementDataUtil.RRBaseArrayToLongs(i, o2, o2.Length);
            return o2;
        }
        case DataTypes.cdouble_t: {
            var o2 = new double[i.size() * 2];
            MessageElementDataUtil.RRBaseArrayComplexToDoubles(i, o2, o2.Length);
            var o3 = new CDouble[i.size()];
            for (int j = 0; j < o3.Length; j++)
            {
                o3[j] = new CDouble(o2[j * 2], o2[j * 2 + 1]);
            }
            return o3;
        }
        case DataTypes.csingle_t: {
            var o2 = new float[i.size() * 2];
            MessageElementDataUtil.RRBaseArrayComplexToFloats(i, o2, o2.Length);
            var o3 = new CSingle[i.size()];
            for (int j = 0; j < o3.Length; j++)
            {
                o3[j] = new CSingle(o2[j * 2], o2[j * 2 + 1]);
            }
            return o3;
        }
        case DataTypes.bool_t: {
            var o2 = new byte[i.size()];
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
        if (i == null)
            return null;
        DataTypes type = DataTypeUtil.TypeIDFromString(RobotRaconteurNode.GetTypeString(i.GetType().GetElementType()));

        /*GCHandle h = GCHandle.Alloc(i, GCHandleType.Pinned);
        rr_memcpy.memcpy(o.void_ptr(), h.AddrOfPinnedObject(), i.Length * o.ElementSize());
        h.Free();*/

        switch (type)
        {
        case DataTypes.double_t: {
            var a = (double[])i;
            return MessageElementDataUtil.DoublesToRRBaseArray(a, a.Length);
        }
        case DataTypes.single_t: {
            var a = (float[])i;
            return MessageElementDataUtil.FloatsToRRBaseArray(a, a.Length);
        }
        case DataTypes.int8_t: {
            var a = (sbyte[])i;
            return MessageElementDataUtil.BytesToRRBaseArray(a, a.Length, DataTypes.int8_t);
        }
        case DataTypes.uint8_t: {
            var a = (byte[])i;
            return MessageElementDataUtil.BytesToRRBaseArray(a, a.Length);
        }
        case DataTypes.int16_t: {
            var a = (short[])i;
            return MessageElementDataUtil.ShortsToRRBaseArray(a, a.Length, DataTypes.int16_t);
        }
        case DataTypes.uint16_t: {
            var a = (ushort[])i;
            return MessageElementDataUtil.ShortsToRRBaseArray(a, a.Length);
        }
        case DataTypes.int32_t: {
            var a = (int[])i;
            return MessageElementDataUtil.IntsToRRBaseArray(a, a.Length, DataTypes.int32_t);
        }
        case DataTypes.uint32_t: {
            var a = (uint[])i;
            return MessageElementDataUtil.IntsToRRBaseArray(a, a.Length);
        }
        case DataTypes.int64_t: {
            var a = (long[])i;
            return MessageElementDataUtil.LongsToRRBaseArray(a, a.Length, DataTypes.int64_t);
        }
        case DataTypes.uint64_t: {
            var a = (ulong[])i;
            return MessageElementDataUtil.LongsToRRBaseArray(a, a.Length);
        }
        case DataTypes.cdouble_t: {
            var a = (CDouble[])i;
            var b = new double[a.Length * 2];
            for (int j = 0; j < a.Length; j++)
            {
                b[j * 2] = a[j].Real;
                b[j * 2 + 1] = a[j].Imag;
            }
            return MessageElementDataUtil.DoublesToComplexRRBaseArray(b, b.Length);
        }
        case DataTypes.csingle_t: {
            var a = (CSingle[])i;
            var b = new float[a.Length * 2];
            for (int j = 0; j < a.Length; j++)
            {
                b[j * 2] = a[j].Real;
                b[j * 2 + 1] = a[j].Imag;
            }
            return MessageElementDataUtil.FloatsToComplexRRBaseArray(b, b.Length);
        }
        case DataTypes.bool_t: {
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
        if (i == null)
            return null;
        DataTypes type = DataTypeUtil.TypeIDFromString(RobotRaconteurNode.GetTypeString(i.GetType().GetElementType()));
        if (i.Length != o.size())
            throw new Exception("Length mismatch");
        if (o.GetTypeID() != type)
            throw new Exception("Type mismatch");
        /*GCHandle h = GCHandle.Alloc(i, GCHandleType.Pinned);
        rr_memcpy.memcpy(o.void_ptr(), h.AddrOfPinnedObject(), i.Length * o.ElementSize());
        h.Free();*/

        switch (type)
        {
        case DataTypes.double_t: {
            var a = (double[])i;
            MessageElementDataUtil.DoublesToRRBaseArray(a, a.Length, o);
            return o;
        }
        case DataTypes.single_t: {
            var a = (float[])i;
            MessageElementDataUtil.FloatsToRRBaseArray(a, a.Length, o);
            return o;
        }
        case DataTypes.int8_t: {
            var a = (sbyte[])i;
            MessageElementDataUtil.BytesToRRBaseArray(a, a.Length, o);
            return o;
        }
        case DataTypes.uint8_t: {
            var a = (byte[])i;
            MessageElementDataUtil.BytesToRRBaseArray(a, a.Length, o);
            return o;
        }
        case DataTypes.int16_t: {
            var a = (short[])i;
            MessageElementDataUtil.ShortsToRRBaseArray(a, a.Length, o);
            return o;
        }
        case DataTypes.uint16_t: {
            var a = (ushort[])i;
            MessageElementDataUtil.ShortsToRRBaseArray(a, a.Length, o);
            return o;
        }
        case DataTypes.int32_t: {
            var a = (int[])i;
            MessageElementDataUtil.IntsToRRBaseArray(a, a.Length, o);
            return o;
        }
        case DataTypes.uint32_t: {
            var a = (uint[])i;
            MessageElementDataUtil.IntsToRRBaseArray(a, a.Length, o);
            return o;
        }
        case DataTypes.int64_t: {
            var a = (long[])i;
            MessageElementDataUtil.LongsToRRBaseArray(a, a.Length, o);
            return o;
        }
        case DataTypes.uint64_t: {
            var a = (ulong[])i;
            MessageElementDataUtil.LongsToRRBaseArray(a, a.Length, o);
            return o;
        }
        case DataTypes.cdouble_t: {
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
        case DataTypes.csingle_t: {
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
        case DataTypes.bool_t: {
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

public class vectorptr_messageelement_util
{
    public static MessageElement FindElement(vectorptr_messageelement m, string name)
    {
        foreach (MessageElement m2 in m)
        {
            if (m2.ElementName == name)
                return m2;
            else
                m2.Dispose();
        }
        throw new MessageElementNotFoundException("Message element " + name + " not found");
    }
}

}
