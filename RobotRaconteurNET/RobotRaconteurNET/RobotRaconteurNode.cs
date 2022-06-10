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

public partial class RobotRaconteurNode
{
    private static RobotRaconteurNode csharp_s = null;
    static private WrappedRobotRaconteurExceptionHelper exhelp;
    public static RobotRaconteurNode s
    {
        get {
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
        get {
            return _NodeID();
        }
        set {
            _SetNodeID(value);
        }
    }

    public string NodeName
    {
        get {
            return _NodeName();
        }
        set {
            _SetNodeName(value);
        }
    }

    public int ThreadPoolCount
    {
        get {
            return _GetThreadPoolCount();
        }
        set {
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

    public delegate void ClientServiceListenerDelegate(ServiceStub client, ClientServiceListenerEventType ev,
                                                       object parameter);

    public object ConnectService(string url, string username = null, Dictionary<string, object> credentials = null,
                                 ClientServiceListenerDelegate listener = null, string objecttype = null)
    {

        MessageElementData credentials2 = null;
        try
        {
            if (username == null)
                username = "";
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

            if (objecttype == null)
                objecttype = "";

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
            if (credentials2 != null)
                credentials2.Dispose();
        }
    }

    public object ConnectService(string[] url, string username = null, Dictionary<string, object> credentials = null,
                                 ClientServiceListenerDelegate listener = null, string objecttype = null)
    {
        MessageElementData credentials2 = null;

        try
        {
            if (username == null)
                username = "";
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
            foreach (string ss in url)
                url2.Add(ss);

            if (objecttype == null)
                objecttype = "";

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
            if (credentials2 != null)
                credentials2.Dispose();
        }
    }

    public void DisconnectService(object obj)
    {
        ServiceStub stub = (ServiceStub)obj;
        _DisconnectService(stub.rr_innerstub);
    }

    public async Task<object> AsyncConnectService(string url, string username, Dictionary<string, object> credentials,
                                                  ClientServiceListenerDelegate listener, string objecttype,
                                                  int timeout = RR_TIMEOUT_INFINITE)
    {

        MessageElementData credentials2 = null;
        try
        {
            if (username == null)
                username = "";
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

            if (objecttype == null)
                objecttype = "";

            AsyncStubReturnDirectorImpl<object> h = new AsyncStubReturnDirectorImpl<object>(null);
            int id = RRObjectHeap.AddObject(h);
            _AsyncConnectService(url, username, credentials2, listener2, objecttype, timeout, h, id);
            return await h.Task;
        }
        finally
        {
            if (credentials2 != null)
                credentials2.Dispose();
        }
    }

    public async Task<object> AsyncConnectService(string[] url, string username, Dictionary<string, object> credentials,
                                                  ClientServiceListenerDelegate listener, string objecttype,
                                                  int timeout = RR_TIMEOUT_INFINITE)
    {
        MessageElementData credentials2 = null;

        try
        {
            if (username == null)
                username = "";
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
            foreach (string ss in url)
                url2.Add(ss);

            if (objecttype == null)
                objecttype = "";

            AsyncStubReturnDirectorImpl<object> h = new AsyncStubReturnDirectorImpl<object>();
            int id = RRObjectHeap.AddObject(h);
            _AsyncConnectService(url2, username, credentials2, listener2, objecttype, timeout, h, id);
            return await h.Task;
        }
        finally
        {
            if (credentials2 != null)
                credentials2.Dispose();
        }
    }

    public async Task AsyncDisconnectService(object obj)
    {
        ServiceStub stub = (ServiceStub)obj;
        AsyncVoidNoErrReturnDirectorImpl h = new AsyncVoidNoErrReturnDirectorImpl();
        int id = RRObjectHeap.AddObject(h);
        _AsyncDisconnectService(stub.rr_innerstub, h, id);
        await h.Task;
    }

    public Dictionary<string, object> GetServiceAttributes(object obj)
    {
        ServiceStub stub = (ServiceStub)obj;
        using (MessageElement me = _GetServiceAttributes(stub.rr_innerstub))
        {
            return (Dictionary<string, object>)UnpackVarType(me);
        }
    }

    public NodeID GetServiceNodeID(object obj)
    {
        ServiceStub stub = (ServiceStub)obj;
        return _GetServiceNodeID(stub.rr_innerstub);
    }

    public string GetServiceNodeName(object obj)
    {
        ServiceStub stub = (ServiceStub)obj;
        return _GetServiceNodeName(stub.rr_innerstub);
    }

    public string GetServiceName(object obj)
    {
        ServiceStub stub = (ServiceStub)obj;
        return _GetServiceName(stub.rr_innerstub);
    }

    public string GetObjectType(object obj)
    {
        ServiceStub stub = (ServiceStub)obj;
        return _GetObjectType(stub.rr_innerstub);
    }

    public string GetObjectServicePath(object obj)
    {
        ServiceStub stub = (ServiceStub)obj;
        return _GetObjectServicePath(stub.rr_innerstub);
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
    public MessageElementNestedElementList PackStructure(object s)
    {
        if (s == null)
            return null;
        return GetServiceFactoryForType(s.GetType()).PackStructure(s);
    }

    public T UnpackStructure<T>(MessageElementNestedElementList l)
    {
        if (l == null)
            return default(T);
        return GetServiceFactoryForType(l.TypeName).UnpackStructure<T>(l);
    }

    public T UnpackStructureDispose<T>(MessageElementNestedElementList l)
    {
        using (l)
        {
            return UnpackStructure<T>(l);
        }
    }

    // Pod Packing

    public MessageElementNestedElementList PackPodToArray<T>(ref T s)
        where T : struct
    {
        return GetServiceFactoryForType(s.GetType()).PackPodToArray(ref s);
    }

    public T UnpackPodFromArray<T>(MessageElementNestedElementList l)
        where T : struct
    {
        return GetServiceFactoryForType(l.TypeName).UnpackPodFromArray<T>(l);
    }

    public T UnpackPodFromArrayDispose<T>(MessageElementNestedElementList l)
        where T : struct
    {
        using (l)
        {
            return UnpackPodFromArray<T>(l);
        }
    }

    // Pod Array Packing

    public MessageElementNestedElementList PackPodArray<T>(T[] s)
        where T : struct
    {
        if (s == null)
            return null;
        return GetServiceFactoryForType(s.GetType()).PackPodArray(s);
    }

    public T[] UnpackPodArray<T>(MessageElementNestedElementList l)
        where T : struct
    {
        if (l == null)
            return null;
        return GetServiceFactoryForType(l.TypeName).UnpackPodArray<T>(l);
    }

    public T[] UnpackPodArrayDispose<T>(MessageElementNestedElementList l)
        where T : struct
    {
        using (l)
        {
            return UnpackPodArray<T>(l);
        }
    }

    // Pod MultiDimArray Packing

    public MessageElementNestedElementList PackPodMultiDimArray<T>(PodMultiDimArray s)
        where T : struct
    {
        if (s == null)
            return null;
        return GetServiceFactoryForType(s.pod_array.GetType()).PackPodMultiDimArray<T>(s);
    }

    public PodMultiDimArray UnpackPodMultiDimArray<T>(MessageElementNestedElementList l)
        where T : struct
    {
        if (l == null)
            return null;
        return GetServiceFactoryForType(l.TypeName).UnpackPodMultiDimArray<T>(l);
    }

    public PodMultiDimArray UnpackPodMultiDimArrayDispose<T>(MessageElementNestedElementList l)
        where T : struct
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
            return GetServiceFactoryForType(((NamedMultiDimArray)s).namedarray_array.GetType().GetElementType())
                .PackNamedArray(s);
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

    public MessageElementNestedElementList PackNamedArrayToArray<T>(ref T s)
        where T : struct
    {
        return GetServiceFactoryForType(s.GetType()).PackNamedArrayToArray(ref s);
    }

    public T UnpackNamedArrayFromArray<T>(MessageElementNestedElementList l)
        where T : struct
    {
        return GetServiceFactoryForType(l.TypeName).UnpackNamedArrayFromArray<T>(l);
    }

    public T UnpackNamedArrayFromArrayDispose<T>(MessageElementNestedElementList l)
        where T : struct
    {
        using (l)
        {
            return UnpackNamedArrayFromArray<T>(l);
        }
    }

    // NamedArray Array Packing

    public MessageElementNestedElementList PackNamedArray<T>(T[] s)
        where T : struct
    {
        if (s == null)
            return null;
        return GetServiceFactoryForType(s.GetType()).PackNamedArray(s);
    }

    public T[] UnpackNamedArray<T>(MessageElementNestedElementList l)
        where T : struct
    {
        if (l == null)
            return null;
        return GetServiceFactoryForType(l.TypeName).UnpackNamedArray<T>(l);
    }

    public T[] UnpackNamedArrayDispose<T>(MessageElementNestedElementList l)
        where T : struct
    {
        using (l)
        {
            return UnpackNamedArray<T>(l);
        }
    }

    // Pod MultiDimArray Packing

    public MessageElementNestedElementList PackNamedMultiDimArray<T>(NamedMultiDimArray s)
        where T : struct
    {
        if (s == null)
            return null;
        return GetServiceFactoryForType(s.namedarray_array.GetType()).PackNamedMultiDimArray<T>(s);
    }

    public NamedMultiDimArray UnpackNamedMultiDimArray<T>(MessageElementNestedElementList l)
        where T : struct
    {
        if (l == null)
            return null;
        return GetServiceFactoryForType(l.TypeName).UnpackNamedMultiDimArray<T>(l);
    }

    public NamedMultiDimArray UnpackNamedMultiDimArrayDispose<T>(MessageElementNestedElementList l)
        where T : struct
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
            return GetServiceFactoryForType(((NamedMultiDimArray)s).namedarray_array.GetType().GetElementType())
                .PackNamedArray(s);
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

    public MessageElement PackAnyType<T>(string name, ref T data)
    {
        Type t = typeof(T);

        if (t == typeof(object))
        {
            return MessageElementUtil.NewMessageElement(name, PackVarType((object)data));
        }

        bool is_array = t.IsArray;
        if (!(t.IsValueType || !EqualityComparer<T>.Default.Equals(data, default(T))))
        {
            return MessageElementUtil.NewMessageElement(name, null);
        }

        if (t.IsPrimitive)
        {
            return MessageElementUtil.NewMessageElement(name, new T[] { data });
        }

        if (is_array && t.GetElementType().IsPrimitive)
        {
            return MessageElementUtil.NewMessageElement(name, data);
        }

        if (t == typeof(string))
        {
            return MessageElementUtil.NewMessageElement(name, data);
        }

        if (t == typeof(CDouble) || t == typeof(CSingle))
        {
            return MessageElementUtil.NewMessageElement(name, data);
        }

        if (t.IsEnum)
        {
            return MessageElementUtil.NewMessageElement(name, new int[] { (int)(object)(data) });
        }

        if (is_array)
        {
            var t2 = t.GetElementType();
            if (t2 == typeof(CDouble) || t2 == typeof(CSingle))
            {
                return MessageElementUtil.NewMessageElement(name, data);
            }
        }

        if (t == typeof(MultiDimArray))
        {
            return MessageElementUtil.NewMessageElement(name, PackMultiDimArray((MultiDimArray)(object)data));
        }

        if (t == typeof(PodMultiDimArray))
        {
            return MessageElementUtil.NewMessageElement(name, PackPod((object)data));
        }

        if (t == typeof(NamedMultiDimArray))
        {
            return MessageElementUtil.NewMessageElement(name, PackNamedArray((object)data));
        }

        if (t.IsGenericType)
        {
            if (t.GetGenericTypeDefinition() == typeof(Dictionary<, >))
            {
                var method = typeof(RobotRaconteurNode).GetMethod("PackMapType");
                var dict_params = t.GetGenericArguments();
                var generic = method.MakeGenericMethod(dict_params);
                var packed_map = generic.Invoke(this, new object[] { data });
                return MessageElementUtil.NewMessageElement(name, packed_map);
            }
            if (t.GetGenericTypeDefinition() == typeof(List<>))
            {
                var method = typeof(RobotRaconteurNode).GetMethod("PackListType");
                var list_params = t.GetGenericArguments();
                var generic = method.MakeGenericMethod(list_params);
                var packed_list = generic.Invoke(this, new object[] { data });
                return MessageElementUtil.NewMessageElement(name, packed_list);
            }
            throw new DataTypeException("Invalid Robot Raconteur container value type");
        }

        if (!t.IsValueType && !is_array && t != typeof(PodMultiDimArray) && t != typeof(NamedMultiDimArray))
        {
            return MessageElementUtil.NewMessageElement(name, PackStructure(data));
        }
        else
        {
            Type t2 = t;
            if (t.IsArray)
                t2 = t.GetElementType();
            if (t2.GetCustomAttributes(typeof(NamedArrayElementTypeAndCount), false).Length > 0)
            {
                return MessageElementUtil.NewMessageElement(name, PackNamedArray(data));
            }
            else
            {
                return MessageElementUtil.NewMessageElement(name, PackPod(data));
            }
        }
    }

    private MessageElement PackAnyType<T>(int num, ref T data)
    {
        return PackAnyType(num.ToString(), ref data);
    }

    public T UnpackAnyType<T>(MessageElement e)
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
        case DataTypes.multidimarray_t: {
            MessageElementNestedElementList md = (MessageElementNestedElementList)e.Data;
            return (T)(object)UnpackMultiDimArray(md);
        }
        case DataTypes.structure_t: {
            MessageElementNestedElementList md = (MessageElementNestedElementList)e.Data;
            return UnpackStructure<T>(md);
        }
        /*case DataTypes.pod_t:
            using (MessageElementData md = (MessageElementData)e.Data)
            {
                return (T)UnpackPod(md);
            }*/
        case DataTypes.pod_array_t: {
            MessageElementNestedElementList md = (MessageElementNestedElementList)e.Data;
            if (typeof(T).IsValueType)
            {
                if (md.Elements.Count != 1)
                    throw new DataTypeException("Invalid array size for scalar structure");
                return ((T[])UnpackPod(md))[0];
            }
            else
            {
                return (T)UnpackPod(md);
            }
        }
        case DataTypes.pod_multidimarray_t: {
            return (T)UnpackPod((MessageElementData)e.Data);
        }
        case DataTypes.namedarray_array_t: {
            MessageElementNestedElementList md = (MessageElementNestedElementList)e.Data;
            if (typeof(T).IsValueType)
            {
                if (md.Elements.Count != 1)
                    throw new DataTypeException("Invalid array size for scalar structure");

                return ((T[])UnpackNamedArray(md))[0];
            }
            else
            {
                return (T)UnpackNamedArray(md);
            }
        }
        case DataTypes.namedarray_multidimarray_t: {
            return (T)UnpackNamedArray((MessageElementNestedElementList)e.Data);
        }
        case DataTypes.vector_t:
        case DataTypes.dictionary_t: {
            var t = typeof(T);
            var method = typeof(RobotRaconteurNode).GetMethod("UnpackMapType");
            var dict_params = t.GetGenericArguments();
            var generic = method.MakeGenericMethod(dict_params);
            return (T)generic.Invoke(this, new object[] { e.Data });
        }
        case DataTypes.list_t: {
            var t = typeof(T);
            var method = typeof(RobotRaconteurNode).GetMethod("UnpackListType");
            var list_params = t.GetGenericArguments();
            var generic = method.MakeGenericMethod(list_params);
            return (T)generic.Invoke(this, new object[] { e.Data });
        }
        default:
            throw new DataTypeException("Invalid container data type");
        }
    }

    public T UnpackAnyType<T>(MessageElement e, out string name)
    {
        name = e.ElementName;
        return UnpackAnyType<T>(e);
    }

    public T UnpackAnyType<T>(MessageElement e, out int num)
    {
        num = MessageElementUtil.GetMessageElementNumber(e);
        return UnpackAnyType<T>(e);
    }

    public T UnpackAnyTypeDispose<T>(MessageElement e)
    {
        using (e)
        {
            return UnpackAnyType<T>(e);
        }
    }

    public T UnpackAnyTypeDispose<T>(MessageElement e, out string name)
    {
        using (e)
        {
            return UnpackAnyType<T>(e, out name);
        }
    }

    public T UnpackAnyTypeDispose<T>(MessageElement e, out int num)
    {
        using (e)
        {
            return UnpackAnyType<T>(e, out num);
        }
    }

    // Map type packing

    public object PackMapType<Tkey, Tvalue>(object data)
    {
        if (data == null)
            return null;

        if (typeof(Tkey) == typeof(Int32))
        {
            using (vectorptr_messageelement m = new vectorptr_messageelement())
            {
                Dictionary<Tkey, Tvalue> ddata = (Dictionary<Tkey, Tvalue>)data;

                foreach (KeyValuePair<Tkey, Tvalue> d in ddata)
                {
                    var v = d.Value;
                    MessageElementUtil.AddMessageElementDispose(m, PackAnyType<Tvalue>(Convert.ToInt32(d.Key), ref v));
                }
                return new MessageElementNestedElementList(DataTypes.vector_t, "", m);
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
                    MessageElementUtil.AddMessageElementDispose(m, PackAnyType<Tvalue>(d.Key.ToString(), ref v));
                }
                return new MessageElementNestedElementList(DataTypes.dictionary_t, "", m);
            }
        }

        throw new DataTypeException("Indexed types can only be indexed by int32 and string");
    }

    public object UnpackMapType<Tkey, Tvalue>(object data)
    {
        if (data == null)
            return null;
        var data1 = (MessageElementNestedElementList)data;
        if (data1.Type == DataTypes.vector_t)
        {
            Dictionary<int, Tvalue> o = new Dictionary<int, Tvalue>();

            MessageElementNestedElementList cdata = (MessageElementNestedElementList)data;
            using (vectorptr_messageelement cdataElements = cdata.Elements)
            {
                foreach (MessageElement e in cdataElements)
                {
                    using (e)
                    {
                        int num;
                        var val = UnpackAnyTypeDispose<Tvalue>(e, out num);
                        o.Add(num, val);
                    }
                }
                return o;
            }
        }
        else if (data1.Type == DataTypes.dictionary_t)
        {
            Dictionary<string, Tvalue> o = new Dictionary<string, Tvalue>();

            MessageElementNestedElementList cdata = (MessageElementNestedElementList)data;
            using (vectorptr_messageelement cdataElements = cdata.Elements)
            {
                foreach (MessageElement e in cdataElements)
                {
                    using (e)
                    {
                        string name;
                        var val = UnpackAnyTypeDispose<Tvalue>(e, out name);
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
            if (d != null)
                d.Dispose();
        }
    }

    public object PackListType<Tvalue>(object data)
    {
        if (data == null)
            return null;

        using (vectorptr_messageelement m = new vectorptr_messageelement())
        {
            List<Tvalue> ddata = (List<Tvalue>)data;

            int count = 0;
            foreach (Tvalue d in ddata)
            {
                var v = d;
                MessageElementUtil.AddMessageElementDispose(m, PackAnyType<Tvalue>(count, ref v));
                count++;
            }

            return new MessageElementNestedElementList(DataTypes.list_t, "", m);
        }
    }

    public object UnpackListType<Tvalue>(object data)
    {
        if (data == null)
            return null;
        List<Tvalue> o = new List<Tvalue>();
        int count = 0;
        MessageElementNestedElementList cdata = (MessageElementNestedElementList)data;
        if (cdata.Type != DataTypes.list_t)
            throw new DataTypeException("Expected a list");
        using (vectorptr_messageelement cdataElements = cdata.Elements)
        {
            foreach (MessageElement e in cdataElements)
            {
                using (e)
                {
                    int num;
                    var val = UnpackAnyTypeDispose<Tvalue>(e, out num);
                    if (count != num)
                        throw new DataTypeException("Error in list format");
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
            if (d != null)
                d.Dispose();
        }
    }

    /// <summary>
    /// Packs a varvalue data.  This can handle any type supported by the node
    /// </summary>
    /// <param name="data">The data to be packed</param>
    /// <returns>The packed data for use with MessageElement.Data</returns>
    public object PackVarType(object data)
    {
        if (data == null)
            return null;

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
            if (t.IsArray)
                t2 = t.GetElementType();
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
        if (me == null)
            return null;

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
            using (MessageElementNestedElementList md = (MessageElementNestedElementList)me.Data)
            {
                return UnpackMultiDimArray(md);
            }
        case DataTypes.structure_t:
            using (MessageElementNestedElementList md = (MessageElementNestedElementList)me.Data)
            {
                return UnpackStructure<object>(md);
            }
        // case DataTypes.pod_t:
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
    /// Packs a MultiDimArray into a MessageElementNestedElementList
    /// </summary>
    /// <param name="array">The array to be packed</param>
    /// <returns>A packed array for use with MessageElement.Data</returns>
    public MessageElementNestedElementList PackMultiDimArray(MultiDimArray array)
    {
        if (array == null)
            return null;
        using (vectorptr_messageelement l = new vectorptr_messageelement())
        {
            MessageElementUtil.AddMessageElementDispose(l, "dims", array.Dims);
            MessageElementUtil.AddMessageElementDispose(l, "array", array.Array_);

            return new MessageElementNestedElementList(DataTypes.multidimarray_t, "", l);
        }
    }

    /// <summary>
    /// Unpacks a MessageElementNestedElementList and returns unpacked multidim array
    /// </summary>
    /// <param name="marray">The MessageElementNestedElementList to unpack</param>
    /// <returns>The unpacked multidim array</returns>
    public MultiDimArray UnpackMultiDimArray(MessageElementNestedElementList marray)
    {
        if (marray == null)
            return null;

        MultiDimArray m = new MultiDimArray();
        using (vectorptr_messageelement marrayElements = marray.Elements)
        {
            m.Dims = (MessageElementUtil.FindElementAndCast<uint[]>(marrayElements, "dims"));
            m.Array_ = (MessageElementUtil.FindElementAndCast<Array>(marrayElements, "array"));
            return m;
        }
    }

    public MultiDimArray UnpackMultiDimArrayDispose(MessageElementNestedElementList marray)
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
        if (!(obj is ServiceStub))
            throw new ArgumentException("Can only lock object opened through Robot Raconteur");
        ServiceStub s = (ServiceStub)obj;

        return _RequestObjectLock(s.rr_innerstub, flags);
    }

    public string ReleaseObjectLock(object obj)
    {
        if (!(obj is ServiceStub))
            throw new ArgumentException("Can only unlock object opened through Robot Raconteur");
        ServiceStub s = (ServiceStub)obj;

        return _ReleaseObjectLock(s.rr_innerstub);
    }

    public async Task AsyncRequestObjectLock(object obj, RobotRaconteurObjectLockFlags flags,
                                             int timeout = RR_TIMEOUT_INFINITE)
    {
        if (!(obj is ServiceStub))
            throw new ArgumentException("Can only lock object opened through Robot Raconteur");
        ServiceStub s = (ServiceStub)obj;

        AsyncStringReturnDirectorImpl h = new AsyncStringReturnDirectorImpl();
        int id = RRObjectHeap.AddObject(h);

        _AsyncRequestObjectLock(s.rr_innerstub, flags, timeout, h, id);
        await h.Task;
    }

    public async Task AsyncReleaseObjectLock(object obj, int timeout = RR_TIMEOUT_INFINITE)
    {
        if (!(obj is ServiceStub))
            throw new ArgumentException("Can only unlock object opened through Robot Raconteur");
        ServiceStub s = (ServiceStub)obj;

        AsyncStringReturnDirectorImpl h = new AsyncStringReturnDirectorImpl();
        int id = RRObjectHeap.AddObject(h);
        _AsyncReleaseObjectLock(s.rr_innerstub, timeout, h, id);
        await h.Task;
    }

    public void MonitorEnter(object obj, int timeout = -1)
    {
        if (!(obj is ServiceStub))
            throw new ArgumentException("Only service stubs can be monitored by RobotRaconteurNode");
        ServiceStub s = (ServiceStub)obj;

        _MonitorEnter(s.rr_innerstub, timeout);
    }

    public void MonitorExit(object obj)
    {
        if (!(obj is ServiceStub))
            throw new ArgumentException("Only service stubs can be monitored by RobotRaconteurNode");
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
            if (obj == null)
                return;
            if (locked)
                throw new Exception("Already locked");
            RobotRaconteurNode.s.MonitorEnter(obj, timeout);
            locked = true;
        }

        public void unlock(Int32 timeout = -1)
        {
            if (obj == null)
                return;
            if (!locked)
                throw new Exception("Not locked");
            RobotRaconteurNode.s.MonitorExit(timeout);
            locked = false;
        }

        public void release()
        {
            obj = null;
        }

        public void Dispose()
        {
            if (locked)
                unlock();
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
        foreach (string s2 in transportschemes)
            s.Add(s2);
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
        foreach (string s2 in transportschemes)
            s.Add(s2);
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
        foreach (string s2 in transportschemes)
            s.Add(s2);
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

        protected TaskCompletionSource<ServiceInfo2[]> handler_task =
            new TaskCompletionSource<ServiceInfo2[]>(TaskContinuationOptions.ExecuteSynchronously);

        public Task<ServiceInfo2[]> Task
        {
            get {
                return handler_task.Task;
            }
        }

        public AsyncServiceInfo2DirectorImpl()
        {}

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
                    this.handler_task.SetResult(new ServiceInfo2[0]);
                    return;
                }

                this.handler_task.SetResult(o.ToArray());
            }
            catch (Exception e)
            {
                using (MessageEntry merr = new MessageEntry())
                {
                    RobotRaconteurExceptionUtil.ExceptionToMessageEntry(e, merr);
                    RRDirectorExceptionHelper.SetError(merr, e.ToString());
                }
            }
        }
    }

    private class AsyncNodeInfo2DirectorImpl : AsyncNodeInfo2VectorReturnDirector
    {
        protected TaskCompletionSource<NodeInfo2[]> handler_task =
            new TaskCompletionSource<NodeInfo2[]>(TaskContinuationOptions.ExecuteSynchronously);

        public Task<NodeInfo2[]> Task
        {
            get {
                return handler_task.Task;
            }
        }

        public AsyncNodeInfo2DirectorImpl()
        {}

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
                    handler_task.SetResult(new NodeInfo2[0]);
                    return;
                }

                handler_task.SetResult(o.ToArray());
            }
            catch (Exception e)
            {
                using (MessageEntry merr = new MessageEntry())
                {
                    RobotRaconteurExceptionUtil.ExceptionToMessageEntry(e, merr);
                    RRDirectorExceptionHelper.SetError(merr, e.ToString());
                }
            }
        }
    }

    public const int RR_TIMEOUT_INFINITE = -1;

    public async Task<ServiceInfo2[]> AsyncFindServiceByType(string servicetype, string[] transportschemes,
                                                             int timeout = 5000)
    {
        vectorstring s = new vectorstring();
        foreach (string s2 in transportschemes)
            s.Add(s2);

        AsyncServiceInfo2DirectorImpl h = new AsyncServiceInfo2DirectorImpl();
        int id = RRObjectHeap.AddObject(h);

        RobotRaconteurNET.AsyncWrappedFindServiceByType(this, servicetype, s, timeout, h, id);
        return await h.Task;
    }

    public async Task<NodeInfo2[]> AsyncFindNodeByID(NodeID id, string[] transportschemes, int timeout = 5000)
    {
        vectorstring s = new vectorstring();
        foreach (string s2 in transportschemes)
            s.Add(s2);

        AsyncNodeInfo2DirectorImpl h = new AsyncNodeInfo2DirectorImpl();
        int id2 = RRObjectHeap.AddObject(h);

        RobotRaconteurNET.AsyncWrappedFindNodeByID(this, id, s, timeout, h, id2);
        return await h.Task;
    }

    public async Task<NodeInfo2[]> AsyncFindNodeByName(string name, string[] transportschemes, int timeout = 5000)
    {
        vectorstring s = new vectorstring();
        foreach (string s2 in transportschemes)
            s.Add(s2);

        AsyncNodeInfo2DirectorImpl h = new AsyncNodeInfo2DirectorImpl();
        int id2 = RRObjectHeap.AddObject(h);

        RobotRaconteurNET.AsyncWrappedFindNodeByName(this, name, s, timeout, h, id2);
        return await h.Task;
    }

    public void UpdateDetectedNodes(string[] schemes)
    {
        vectorstring schemes1 = new vectorstring();
        foreach (string s in schemes)
            schemes1.Add(s);
        RobotRaconteurNET.WrappedUpdateDetectedNodes(this, schemes1);
    }

    public async Task AsyncUpdateDetectedNodes(string[] schemes, int timeout = 5000)
    {
        vectorstring schemes1 = new vectorstring();
        foreach (string s in schemes)
            schemes1.Add(s);
        AsyncVoidNoErrReturnDirectorImpl h = new AsyncVoidNoErrReturnDirectorImpl();
        int id2 = RRObjectHeap.AddObject(h);
        RobotRaconteurNET.AsyncWrappedUpdateDetectedNodes(this, schemes1, timeout, h, id2);
        await h.Task;
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

    public NodeInfo2 GetDetectedNodeCacheInfo(NodeID nodeid)
    {
        return new NodeInfo2(RobotRaconteurNET.WrappedGetDetectedNodeCacheInfo(this, nodeid));
    }

    public bool TryGetDetectedNodeCacheInfo(NodeID nodeid, out NodeInfo2 nodeinfo2)
    {
        WrappedNodeInfo2 ret = new WrappedNodeInfo2();
        bool res = RobotRaconteurNET.WrappedTryGetDetectedNodeCacheInfo(this, nodeid, ret);
        if (!res)
        {
            nodeinfo2 = null;
            return false;
        }
        nodeinfo2 = new NodeInfo2(ret);
        return true;
    }

    public ServerContext RegisterService(string name, string servicetype, object obj,
                                         ServiceSecurityPolicy policy = null)
    {
        ServiceSkel skel = GetServiceType(servicetype).CreateSkel(obj);

        int id = RRObjectHeap.AddObject(skel);
        skel.innerskelid = id;
        WrappedRRObject o = new WrappedRRObject(skel.RRType, skel, id);

        return _RegisterService(name, servicetype, o, policy);
    }

    public DateTime NowUTC
    {
        get {
            return _NowUTC();
        }
    }

    public DateTime NowNodeTime
    {
        get {
            return _NowNodeTime();
        }
    }

    public DateTime NodeSyncTimeUTC
    {
        get {
            return _NodeSyncTimeUTC();
        }
    }

    public TimeSpec NowTimeSpec
    {
        get {
            return _NowTimeSpec();
        }
    }

    public TimeSpec NodeSyncTimeSpec
    {
        get {
            return _NodeSyncTimeSpec();
        }
    }

    public object FindObjRefTyped(object obj, string objref, string objecttype)
    {
        if (!(obj is ServiceStub))
            throw new ArgumentException("Only service stubs can have objref");
        ServiceStub s = (ServiceStub)obj;

        return s.FindObjRefTyped(objref, objecttype);
    }

    public object FindObjRefTyped(object obj, string objref, string index, string objecttype)
    {
        if (!(obj is ServiceStub))
            throw new ArgumentException("Only service stubs can have objref");
        ServiceStub s = (ServiceStub)obj;

        return s.FindObjRefTyped(objref, index, objecttype);
    }

    public Task<object> AsyncFindObjRefTyped(object obj, string objref, string objecttype,
                                             int timeout = RR_TIMEOUT_INFINITE)
    {
        if (!(obj is ServiceStub))
            throw new ArgumentException("Only service stubs can have objref");
        ServiceStub s = (ServiceStub)obj;

        return s.AsyncFindObjRefTyped<object>(objref, objecttype, timeout);
    }

    public Task<object> AsyncFindObjRefTyped(object obj, string objref, string index, string objecttype,
                                             int timeout = RR_TIMEOUT_INFINITE)
    {
        if (!(obj is ServiceStub))
            throw new ArgumentException("Only service stubs can have objref");
        ServiceStub s = (ServiceStub)obj;

        return s.AsyncFindObjRefTyped<object>(objref, index, objecttype, timeout);
    }

    public string FindObjectType(object obj, string objref)
    {
        if (!(obj is ServiceStub))
            throw new ArgumentException("Only service stubs can have objref");
        ServiceStub s = (ServiceStub)obj;

        return _FindObjectType(s.rr_innerstub, objref);
    }

    public string FindObjectType(object obj, string objref, string index)
    {
        if (!(obj is ServiceStub))
            throw new ArgumentException("Only service stubs can have objref");
        ServiceStub s = (ServiceStub)obj;

        return _FindObjectType(s.rr_innerstub, objref, index);
    }

    public async Task<string> AsyncFindObjectType(object obj, string objref, int timeout = RR_TIMEOUT_INFINITE)
    {
        if (!(obj is ServiceStub))
            throw new ArgumentException("Only service stubs can have objref");
        ServiceStub s = (ServiceStub)obj;

        AsyncStringReturnDirectorImpl h = new AsyncStringReturnDirectorImpl();
        int id2 = RRObjectHeap.AddObject(h);
        _AsyncFindObjectType(s.rr_innerstub, objref, timeout, h, id2);
        return await h.Task;
    }

    public async Task<string> AsyncFindObjectType(object obj, string objref, string index,
                                                  int timeout = RR_TIMEOUT_INFINITE)
    {
        if (!(obj is ServiceStub))
            throw new ArgumentException("Only service stubs can have objref");
        ServiceStub s = (ServiceStub)obj;

        AsyncStringReturnDirectorImpl h = new AsyncStringReturnDirectorImpl();
        int id2 = RRObjectHeap.AddObject(h);
        _AsyncFindObjectType(s.rr_innerstub, objref, index, timeout, h, id2);
        return await h.Task;
    }

    public uint RequestTimeout
    {
        get {
            return _GetRequestTimeout();
        }
        set {
            _SetRequestTimeout(value);
        }
    }

    public uint TransportInactivityTimeout
    {
        get {
            return _GetTransportInactivityTimeout();
        }
        set {
            _SetTransportInactivityTimeout(value);
        }
    }

    public uint EndpointInactivityTimeout
    {
        get {
            return _GetEndpointInactivityTimeout();
        }
        set {
            _SetEndpointInactivityTimeout(value);
        }
    }

    public uint MemoryMaxTransferSize
    {
        get {
            return _GetMemoryMaxTransferSize();
        }
        set {
            _SetMemoryMaxTransferSize(value);
        }
    }

    public uint NodeDiscoveryMaxCacheCount
    {
        get {
            return _GetNodeDiscoveryMaxCacheCount();
        }
        set {
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
        if (pos == -1)
            throw new ArgumentException("Name is not qualified");
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
        if (exp == null)
            return exp;
        string type = exp.Error;
        if (!type.Contains("."))
            return exp;
        var stype = RobotRaconteurNode.SplitQualifiedName(type);
        if (!IsServiceTypeRegistered(stype.Item1))
            return exp;
        return GetServiceType(stype.Item1).DownCastException(exp);
    }

    public void PostToThreadPool(Action target)
    {
        AsyncVoidNoErrReturnDirectorImpl h = new AsyncVoidNoErrReturnDirectorImpl();
        int id = RRObjectHeap.AddObject(h);
        _PostToThreadPool(h, id);
        h.Task.ContinueWith(
            _ => target());
    }

    public string RobotRaconteurVersion
    {
        get {
            return this._GetRobotRaconteurVersion();
        }
    }
}

}