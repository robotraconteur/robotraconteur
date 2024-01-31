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
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace RobotRaconteur
{

/// <summary>
/// Base class for service factories
/// </summary>
public abstract class ServiceFactory
{
    private ServiceDefinition sdef = null;

    public ServiceDefinition ServiceDef()
    {
        if (sdef == null)
        {
            sdef = new ServiceDefinition();
            sdef.FromString(DefString());
        }
        return sdef;
    }

    public string RemovePath(string path)
    {
        return RobotRaconteurNode.SplitQualifiedName(path).Item2;
    }

    public bool CompareNamespace(string qualified_typename, out string typename)
    {
        var s = RobotRaconteurNode.SplitQualifiedName(qualified_typename);
        typename = s.Item2;
        return s.Item1 == GetServiceName();
    }

    public abstract string DefString();

    public MessageElementNestedElementList PackStructure(object s)
    {
        string typename;
        if (CompareNamespace(RobotRaconteurNode.GetTypeString(s.GetType()), out typename))
        {
            return FindStructureStub(typename).PackStructure(s);
        }
        else
        {
            return RobotRaconteurNode.s.PackStructure(s);
        }
    }

    public T UnpackStructure<T>(MessageElementNestedElementList l)
    {
        string typename;
        if (CompareNamespace(l.GetTypeString(), out typename))
        {
            return (T)FindStructureStub(typename).UnpackStructure<T>(l);
        }
        else
        {
            return RobotRaconteurNode.s.UnpackStructure<T>(l);
        }
    }

    public MessageElementNestedElementList PackPodToArray<T>(ref T s)
        where T : struct
    {
        string typename;
        if (CompareNamespace(RobotRaconteurNode.GetTypeString(s.GetType()), out typename))
        {
            return ((PodStub<T>)FindPodStub(typename)).PackPodToArray(ref s);
        }
        else
        {
            return RobotRaconteurNode.s.PackPodToArray(ref s);
        }
    }

    public T UnpackPodFromArray<T>(MessageElementNestedElementList l)
        where T : struct
    {
        string typename;
        if (CompareNamespace(l.GetTypeString(), out typename))
        {
            return ((PodStub<T>)FindPodStub(typename)).UnpackPodFromArray(l);
        }
        else
        {
            return RobotRaconteurNode.s.UnpackPodFromArray<T>(l);
        }
    }

    public MessageElementNestedElementList PackPodArray<T>(T[] s)
        where T : struct
    {
        string typename;
        if (CompareNamespace(RobotRaconteurNode.GetTypeString(s.GetType().GetElementType()), out typename))
        {
            return ((PodStub<T>)FindPodStub(typename)).PackPodArray(s);
        }
        else
        {
            return RobotRaconteurNode.s.PackPodArray(s);
        }
    }

    public T[] UnpackPodArray<T>(MessageElementNestedElementList l)
        where T : struct
    {
        string typename;
        if (CompareNamespace(l.GetTypeString(), out typename))
        {
            return ((PodStub<T>)FindPodStub(typename)).UnpackPodArray(l);
        }
        else
        {
            return RobotRaconteurNode.s.UnpackPodArray<T>(l);
        }
    }

    public MessageElementNestedElementList PackPodMultiDimArray<T>(PodMultiDimArray s)
        where T : struct
    {
        string typename;
        if (CompareNamespace(RobotRaconteurNode.GetTypeString(s.pod_array.GetType().GetElementType()), out typename))
        {
            return ((PodStub<T>)FindPodStub(typename)).PackPodMultiDimArray(s);
        }
        else
        {
            return RobotRaconteurNode.s.PackPodMultiDimArray<T>(s);
        }
    }

    public PodMultiDimArray UnpackPodMultiDimArray<T>(MessageElementNestedElementList l)
        where T : struct
    {
        string typename;
        if (CompareNamespace(l.GetTypeString(), out typename))
        {
            return ((PodStub<T>)FindPodStub(typename)).UnpackPodMultiDimArray(l);
        }
        else
        {
            return RobotRaconteurNode.s.UnpackPodMultiDimArray<T>(l);
        }
    }

    public MessageElementData PackPod(object s)
    {
        Type t;

        var s1 = s as PodMultiDimArray;
        if (s1 != null)
        {
            t = s1.pod_array.GetType().GetElementType();
        }
        else
        {
            var s2 = s as Array;
            if (s2 != null)
            {
                t = s2.GetType().GetElementType();
            }
            else
            {
                t = s.GetType();
            }
        }

        string typename;
        if (CompareNamespace(RobotRaconteurNode.GetTypeString(t), out typename))
        {
            return FindPodStub(typename).PackPod(s);
        }
        else
        {
            return RobotRaconteurNode.s.PackPod(s);
        }
    }

    public object UnpackPod(MessageElementData m)
    {
        string typename;
        if (CompareNamespace(m.GetTypeString(), out typename))
        {
            return FindPodStub(typename).UnpackPod(m);
        }
        else
        {
            return RobotRaconteurNode.s.UnpackPod(m);
        }
    }

    // namedarray

    public MessageElementNestedElementList PackNamedArrayToArray<T>(ref T s)
        where T : struct
    {
        string typename;
        if (CompareNamespace(RobotRaconteurNode.GetTypeString(s.GetType()), out typename))
        {
            return ((INamedArrayStub<T>)FindNamedArrayStub(typename)).PackNamedArrayStructToArray(ref s);
        }
        else
        {
            return RobotRaconteurNode.s.PackNamedArrayToArray(ref s);
        }
    }

    public T UnpackNamedArrayFromArray<T>(MessageElementNestedElementList l)
        where T : struct
    {
        string typename;
        if (CompareNamespace(l.GetTypeString(), out typename))
        {
            return ((INamedArrayStub<T>)FindNamedArrayStub(typename)).UnpackNamedArrayStructFromArray(l);
        }
        else
        {
            return RobotRaconteurNode.s.UnpackNamedArrayFromArray<T>(l);
        }
    }

    public MessageElementNestedElementList PackNamedArray<T>(T[] s)
        where T : struct
    {
        string typename;
        if (CompareNamespace(RobotRaconteurNode.GetTypeString(s.GetType().GetElementType()), out typename))
        {
            return ((INamedArrayStub<T>)FindNamedArrayStub(typename)).PackNamedArray(s);
        }
        else
        {
            return RobotRaconteurNode.s.PackNamedArray(s);
        }
    }

    public T[] UnpackNamedArray<T>(MessageElementNestedElementList l)
        where T : struct
    {
        string typename;
        if (CompareNamespace(l.GetTypeString(), out typename))
        {
            return ((INamedArrayStub<T>)FindNamedArrayStub(typename)).UnpackNamedArray(l);
        }
        else
        {
            return RobotRaconteurNode.s.UnpackNamedArray<T>(l);
        }
    }

    public MessageElementNestedElementList PackNamedMultiDimArray<T>(NamedMultiDimArray s)
        where T : struct
    {
        string typename;
        if (CompareNamespace(RobotRaconteurNode.GetTypeString(s.namedarray_array.GetType().GetElementType()),
                             out typename))
        {
            return ((INamedArrayStub<T>)FindNamedArrayStub(typename)).PackNamedMultiDimArray(s);
        }
        else
        {
            return RobotRaconteurNode.s.PackNamedMultiDimArray<T>(s);
        }
    }

    public NamedMultiDimArray UnpackNamedMultiDimArray<T>(MessageElementNestedElementList l)
        where T : struct
    {
        string typename;
        if (CompareNamespace(l.GetTypeString(), out typename))
        {
            return ((INamedArrayStub<T>)FindNamedArrayStub(typename)).UnpackNamedMultiDimArray(l);
        }
        else
        {
            return RobotRaconteurNode.s.UnpackNamedMultiDimArray<T>(l);
        }
    }

    public MessageElementData PackNamedArray(object s)
    {
        Type t;

        var s1 = s as NamedMultiDimArray;
        if (s1 != null)
        {
            t = s1.namedarray_array.GetType().GetElementType();
        }
        else
        {
            var s2 = s as Array;
            if (s2 != null)
            {
                t = s2.GetType().GetElementType();
            }
            else
            {
                t = s.GetType();
            }
        }

        string typename;
        if (CompareNamespace(RobotRaconteurNode.GetTypeString(t), out typename))
        {
            return FindNamedArrayStub(typename).PackNamedArray(s);
        }
        else
        {
            return RobotRaconteurNode.s.PackNamedArray(s);
        }
    }

    public object UnpackNamedArray(MessageElementData m)
    {
        string typename;
        if (CompareNamespace(m.GetTypeString(), out typename))
        {
            return FindNamedArrayStub(typename).UnpackNamedArray(m);
        }
        else
        {
            return RobotRaconteurNode.s.UnpackNamedArray(m);
        }
    }

    public abstract IStructureStub FindStructureStub(string objecttype);

    public abstract IPodStub FindPodStub(string objecttype);

    public abstract INamedArrayStub FindNamedArrayStub(string objecttype);

    public abstract ServiceStub CreateStub(WrappedServiceStub innerstub);

    public abstract ServiceSkel CreateSkel(object innerskel);

    public abstract string GetServiceName();

    public abstract RobotRaconteurException DownCastException(RobotRaconteurException exp);
}

public interface IStructureStub
{
    MessageElementNestedElementList PackStructure(Object s);

    T UnpackStructure<T>(MessageElementNestedElementList m);
}

public interface IPodStub
{
    MessageElementData PackPod(object s);

    object UnpackPod(MessageElementData m);
}

public abstract class PodStub<T> : IPodStub
    where T : struct
{
    public abstract MessageElementNestedElementList PackPod(ref T s);

    public abstract T UnpackPod(MessageElementNestedElementList m);

    public virtual MessageElementNestedElementList PackPodToArray(ref T s2)
    {
        using (var mm = new vectorptr_messageelement())
        {
            MessageElementUtil.AddMessageElementDispose(
                mm, MessageElementUtil.NewMessageElementDispose(0, PackPod(ref s2)));

            return new MessageElementNestedElementList(DataTypes.pod_array_t, TypeName, mm);
        }
    }

    public virtual MessageElementNestedElementList PackPodArray(T[] s2)
    {
        if (s2 == null)
            return null;

        using (var mm = new vectorptr_messageelement())
        {
            for (int i = 0; i < s2.Length; i++)
            {
                MessageElementUtil.AddMessageElementDispose(
                    mm, MessageElementUtil.NewMessageElementDispose(i, PackPod(ref s2[i])));
            }
            return new MessageElementNestedElementList(DataTypes.pod_array_t, TypeName, mm);
        }
    }

    public virtual MessageElementNestedElementList PackPodMultiDimArray(PodMultiDimArray s3)
    {
        if (s3 == null)
            return null;
        using (vectorptr_messageelement l = new vectorptr_messageelement())
        {
            MessageElementUtil.AddMessageElementDispose(l, "dims", s3.Dims);
            using (var s4 = PackPodArray((T[])s3.pod_array))
            {
                MessageElementUtil.AddMessageElementDispose(l, "array", s4);
                return new MessageElementNestedElementList(DataTypes.pod_multidimarray_t, TypeName, l);
            }
        }
    }

    public virtual T UnpackPodFromArray(MessageElementNestedElementList s2)
    {
        if (s2.TypeName != TypeName)
            throw new DataTypeException("pod type mismatch");
        using (vectorptr_messageelement cdataElements = s2.Elements)
        {
            if (cdataElements.Count != 1)
                throw new DataTypeException("pod type mismatch");

            var e = cdataElements[0];
            using (e)
            {
                if (0 != MessageElementUtil.GetMessageElementNumber(e))
                    throw new DataTypeException("Error in list format");
                using (MessageElementNestedElementList md = (MessageElementNestedElementList)e.Data)
                {
                    return UnpackPod(md);
                }
            }
        }
    }

    public virtual T[] UnpackPodArray(MessageElementNestedElementList s2)
    {
        if (s2.TypeName != TypeName)
            throw new DataTypeException("pod type mismatch");
        int count = 0;
        using (vectorptr_messageelement cdataElements = s2.Elements)
        {
            T[] o = new T[cdataElements.Count];
            foreach (MessageElement e in cdataElements)
            {
                using (e)
                {
                    if (count != MessageElementUtil.GetMessageElementNumber(e))
                        throw new DataTypeException("Error in list format");
                    using (MessageElementNestedElementList md = (MessageElementNestedElementList)e.Data)
                    {
                        o[count] = UnpackPod(md);
                    }
                    count++;
                }
            }
            return o;
        }
    }

    public virtual PodMultiDimArray UnpackPodMultiDimArray(MessageElementNestedElementList s3)
    {
        if (s3.TypeName != TypeName)
            throw new DataTypeException("pod type mismatch");
        var o = new PodMultiDimArray();
        using (vectorptr_messageelement marrayElements = s3.Elements)
        {
            o.Dims = (MessageElementUtil.FindElementAndCast<uint[]>(marrayElements, "dims"));
            using (var s2 = (MessageElementUtil.FindElementAndCast<MessageElementNestedElementList>(marrayElements,
                                                                                                    "array")))
            {
                o.pod_array = UnpackPodArray(s2);
            }
        }
        return o;
    }

    public virtual MessageElementData PackPod(object s)
    {
        if (s is T)
        {
            T s2 = (T)s;
            return PackPodToArray(ref s2);
        }

        var s3 = s as T[];
        if (s3 != null)
        {
            return PackPodArray(s3);
        }

        var s4 = s as PodMultiDimArray;
        if (s4 != null)
        {
            return PackPodMultiDimArray(s4);
        }

        throw new DataTypeException("Unexpected message element type for PackPod");
    }
    public virtual object UnpackPod(MessageElementData m)
    {
        /*var m2 = m as MessageElementNestedElementList;
        if (m2 != null)
        {

            return UnpackPod(m2);
        }*/

        var m3 = m as MessageElementNestedElementList;
        if (m3 != null)
        {
            if (m3.Type == DataTypes.pod_array_t)
            {
                return UnpackPodArray(m3);
            }
            if (m3.Type == DataTypes.pod_multidimarray_t)
            {
                return UnpackPodMultiDimArray(m3);
            }
        }

        var m4 = m as MessageElementNestedElementList;
        if (m4 != null)
        {}

        throw new DataTypeException("Unexpected message element type for UnpackPod");
    }

    public abstract string TypeName { get; }
}

public class NamedArrayElementTypeAndCount : System.Attribute
{
    private readonly Type _elementArrayType;
    private readonly int _elementArrayCount;

    public NamedArrayElementTypeAndCount(Type element_type, int element_array_count)
    {
        _elementArrayType = element_type;
        _elementArrayCount = element_array_count;
    }

    public Type ElementArrayType
    {
        get {
            return _elementArrayType;
        }
    }
    public int ElementArrayCount
    {
        get {
            return _elementArrayCount;
        }
    }
}

public interface INamedArrayStub
{
    MessageElementData PackNamedArray(object s);

    object UnpackNamedArray(MessageElementData m);
}

public interface INamedArrayStub<T> : INamedArrayStub
{
    MessageElementNestedElementList PackNamedArrayStructToArray(ref T s2);

    MessageElementNestedElementList PackNamedArray(T[] s2);

    MessageElementNestedElementList PackNamedMultiDimArray(NamedMultiDimArray s3);

    T UnpackNamedArrayStructFromArray(MessageElementNestedElementList s2);

    T[] UnpackNamedArray(MessageElementNestedElementList s2);

    NamedMultiDimArray UnpackNamedMultiDimArray(MessageElementNestedElementList s3);
}

public abstract class NamedArrayStub<T, U> : INamedArrayStub<T>
    where T : struct
{
    public abstract U[] GetNumericArrayFromNamedArrayStruct(ref T s);

    public abstract T GetNamedArrayStructFromNumericArray(U[] m);

    public abstract U[] GetNumericArrayFromNamedArray(T[] s);

    public abstract T[] GetNamedArrayFromNumericArray(U[] m);

    public virtual MessageElementNestedElementList PackNamedArrayStructToArray(ref T s2)
    {
        using (var mm = new vectorptr_messageelement())
        {
            MessageElementUtil.AddMessageElementDispose(
                mm, MessageElementUtil.NewMessageElementDispose("array", GetNumericArrayFromNamedArrayStruct(ref s2)));

            return new MessageElementNestedElementList(DataTypes.namedarray_array_t, TypeName, mm);
        }
    }

    public virtual MessageElementNestedElementList PackNamedArray(T[] s2)
    {
        if (s2 == null)
            return null;

        using (var mm = new vectorptr_messageelement())
        {

            MessageElementUtil.AddMessageElementDispose(
                mm, MessageElementUtil.NewMessageElementDispose("array", GetNumericArrayFromNamedArray(s2)));

            return new MessageElementNestedElementList(DataTypes.namedarray_array_t, TypeName, mm);
        }
    }

    public virtual MessageElementNestedElementList PackNamedMultiDimArray(NamedMultiDimArray s3)
    {
        if (s3 == null)
            return null;
        using (vectorptr_messageelement l = new vectorptr_messageelement())
        {
            MessageElementUtil.AddMessageElementDispose(l, "dims", s3.Dims);
            MessageElementUtil.AddMessageElementDispose(l, "array", PackNamedArray((T[])s3.namedarray_array));
            return new MessageElementNestedElementList(DataTypes.namedarray_multidimarray_t, TypeName, l);
        }
    }

    public virtual T UnpackNamedArrayStructFromArray(MessageElementNestedElementList s2)
    {
        if (s2.TypeName != TypeName)
            throw new DataTypeException("namedarray type mismatch");
        using (vectorptr_messageelement cdataElements = s2.Elements)
        {
            if (cdataElements.Count != 1)
                throw new DataTypeException("pod type mismatch");

            var a = MessageElementUtil.FindElementAndCast<U[]>(cdataElements, "array");

            return GetNamedArrayStructFromNumericArray(a);
        }
    }

    public virtual T[] UnpackNamedArray(MessageElementNestedElementList s2)
    {
        if (s2.TypeName != TypeName)
            throw new DataTypeException("namedarray type mismatch");
        using (vectorptr_messageelement cdataElements = s2.Elements)
        {
            if (cdataElements.Count != 1)
                throw new DataTypeException("pod type mismatch");

            var a = MessageElementUtil.FindElementAndCast<U[]>(cdataElements, "array");

            return GetNamedArrayFromNumericArray(a);
        }
    }

    public virtual NamedMultiDimArray UnpackNamedMultiDimArray(MessageElementNestedElementList s3)
    {
        if (s3.TypeName != TypeName)
            throw new DataTypeException("pod type mismatch");
        var o = new NamedMultiDimArray();
        using (vectorptr_messageelement marrayElements = s3.Elements)
        {
            o.Dims = (MessageElementUtil.FindElementAndCast<uint[]>(marrayElements, "dims"));
            using (var s2 = (MessageElementUtil.FindElementAndCast<MessageElementNestedElementList>(marrayElements,
                                                                                                    "array")))
            {
                o.namedarray_array = UnpackNamedArray(s2);
            }
        }
        return o;
    }

    public virtual MessageElementData PackNamedArray(object s)
    {
        if (s is T)
        {
            T s2 = (T)s;
            return PackNamedArrayStructToArray(ref s2);
        }

        var s3 = s as T[];
        if (s3 != null)
        {
            return PackNamedArray(s3);
        }

        var s4 = s as NamedMultiDimArray;
        if (s4 != null)
        {
            return PackNamedMultiDimArray(s4);
        }

        throw new DataTypeException("Unexpected message element type for PackNamedArray");
    }
    public virtual object UnpackNamedArray(MessageElementData m)
    {
        /*var m2 = m as MessageElementNestedElementList;
        if (m2 != null)
        {

            return UnpackPod(m2);
        }*/

        var m3 = m as MessageElementNestedElementList;
        if (m3 != null)
        {
            if (m3.Type == DataTypes.namedarray_array_t)
            {
                return UnpackNamedArray(m3);
            }
            if (m3.Type == DataTypes.namedarray_multidimarray_t)
            {
                return UnpackNamedMultiDimArray(m3);
            }
        }
        throw new DataTypeException("Unexpected message element type for UnpackNamedArray");
    }

    public abstract string TypeName { get; }
}

public abstract class ServiceStub
{

    protected async Task<MessageElement> rr_async_PropertyGet(string name, int timeout)
    {
        AsyncRequestDirectorImpl d = new AsyncRequestDirectorImpl();
        int id = RRObjectHeap.AddObject(d);

        rr_innerstub.async_PropertyGet(name, timeout, d, id);
        return await d.Task;
    }

    protected async Task rr_async_PropertySet(string name, MessageElement value, int timeout)
    {
        AsyncRequestDirectorImpl d = new AsyncRequestDirectorImpl();
        int id = RRObjectHeap.AddObject(d);

        rr_innerstub.async_PropertySet(name, value, timeout, d, id);
        await d.Task;
    }

    protected async Task<MessageElement> rr_async_FunctionCall(string name, vectorptr_messageelement p, int timeout)
    {
        AsyncRequestDirectorImpl d = new AsyncRequestDirectorImpl();
        int id = RRObjectHeap.AddObject(d);

        rr_innerstub.async_FunctionCall(name, p, timeout, d, id);
        return await d.Task;
    }

    protected async Task<WrappedGeneratorClient> rr_async_GeneratorFunctionCall(string name, vectorptr_messageelement p,
                                                                                int timeout)
    {
        AsyncGeneratorClientReturnDirectorImpl d = new AsyncGeneratorClientReturnDirectorImpl();
        int id = RRObjectHeap.AddObject(d);

        rr_innerstub.async_GeneratorFunctionCall(name, p, timeout, d, id);
        return await d.Task;
    }

    public WrappedServiceStub rr_innerstub;
    private WrappedServiceStubDirectorNET director;

    public ServiceStub(WrappedServiceStub innerstub)
    {
        director = new WrappedServiceStubDirectorNET(this);

        rr_innerstub = innerstub;
        int id = RRObjectHeap.AddObject(this);
        innerstub.SetRRDirector(director, id);
    }

    ~ServiceStub()
    {
        // rr_innerstub.RRClose();
    }

    public abstract void DispatchEvent(string membername, vectorptr_messageelement m);

    object objref_lock = new object();

    public object FindObjRef(string n)
    {

        WrappedServiceStub s = rr_innerstub.FindObjRef(n);
        lock (objref_lock)
        {
            if (s.GetObjectHeapID() != 0)
            {
                return RRObjectHeap.GetObject(s.GetObjectHeapID());
            }
            ServiceFactory f = RobotRaconteurNode.s.GetServiceType(s.RR_objecttype.GetServiceDefinition().Name);
            return f.CreateStub(s);
        }
    }

    public object FindObjRef(string n, string i)
    {

        WrappedServiceStub s = rr_innerstub.FindObjRef(n, i);
        lock (objref_lock)
        {
            if (s.GetObjectHeapID() != 0)
            {
                return RRObjectHeap.GetObject(s.GetObjectHeapID());
            }
            ServiceFactory f = RobotRaconteurNode.s.GetServiceType(s.RR_objecttype.GetServiceDefinition().Name);
            return f.CreateStub(s);
        }
    }

    public object FindObjRefTyped(string n, string objecttype)
    {
        WrappedServiceStub s = rr_innerstub.FindObjRefTyped(n, objecttype);
        lock (objref_lock)
        {
            if (s.GetObjectHeapID() != 0)
            {
                return RRObjectHeap.GetObject(s.GetObjectHeapID());
            }
            ServiceFactory f = RobotRaconteurNode.s.GetServiceType(s.RR_objecttype.GetServiceDefinition().Name);
            return f.CreateStub(s);
        }
    }

    public object FindObjRefTyped(string n, string i, string objecttype)
    {
        WrappedServiceStub s = rr_innerstub.FindObjRefTyped(n, i, objecttype);
        lock (objref_lock)
        {
            if (s.GetObjectHeapID() != 0)
            {
                return RRObjectHeap.GetObject(s.GetObjectHeapID());
            }
            ServiceFactory f = RobotRaconteurNode.s.GetServiceType(s.RR_objecttype.GetServiceDefinition().Name);
            return f.CreateStub(s);
        }
    }

    public async Task<object> AsyncFindObjRef(string n, int timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE)
    {
        AsyncStubReturnDirectorImpl<object> d = new AsyncStubReturnDirectorImpl<object>(null);
        int id = RRObjectHeap.AddObject(d);
        rr_innerstub.async_FindObjRef(n, timeout, d, id);
        return await d.Task;
    }

    public async Task<object> AsyncFindObjRef(string n, string i, int timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE)
    {
        AsyncStubReturnDirectorImpl<object> d = new AsyncStubReturnDirectorImpl<object>(null);
        int id = RRObjectHeap.AddObject(d);
        rr_innerstub.async_FindObjRef(n, i, timeout, d, id);
        return await d.Task;
    }

    public async Task<T> AsyncFindObjRefTyped<T>(string n, string type,
                                                 int timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE)
    {
        AsyncStubReturnDirectorImpl<T> d = new AsyncStubReturnDirectorImpl<T>(null);
        int id = RRObjectHeap.AddObject(d);
        rr_innerstub.async_FindObjRefTyped(n, type, timeout, d, id);
        return await d.Task;
    }

    public async Task<T> AsyncFindObjRefTyped<T>(string n, string i, string type,
                                                 int timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE)
    {
        AsyncStubReturnDirectorImpl<T> d = new AsyncStubReturnDirectorImpl<T>(null);
        int id = RRObjectHeap.AddObject(d);
        rr_innerstub.async_FindObjRefTyped(n, i, type, timeout, d, id);
        return await d.Task;
    }

    public virtual MessageElement CallbackCall(string membername, vectorptr_messageelement m)
    {
        throw new MemberNotFoundException("Member not found");
    }
}

public class WrappedServiceStubDirectorNET : WrappedServiceStubDirector
{
    WeakReference stub;

    public WrappedServiceStubDirectorNET(ServiceStub stub)
    {
        this.stub = new WeakReference(stub);
    }

    public override MessageElement CallbackCall(string CallbackName, vectorptr_messageelement args)
    {

        try
        {
            using (args)
            {
                return ((ServiceStub)stub.Target).CallbackCall(CallbackName, args);
            }
        }
        catch (Exception e)
        {
            using (MessageEntry merr = new MessageEntry())
            {
                RobotRaconteurExceptionUtil.ExceptionToMessageEntry(e, merr);
                RRDirectorExceptionHelper.SetError(merr, e.ToString());
                return null;
            }
        }
    }

    public override void DispatchEvent(string EventName, vectorptr_messageelement args)
    {
        try
        {
            using (args)
            {
                ((ServiceStub)stub.Target).DispatchEvent(EventName, args);
            }
        }
        catch
        {}
    }
}

public abstract class ServiceSkel : WrappedServiceSkelDirector
{

    protected WrappedServiceSkel innerskel;

    public ServiceSkel(object o)
    {
        uncastobj = o;
        if (o == null)
            throw new NullReferenceException();
    }

    public virtual void InitCallbackServers(object o)
    {}

    public string ServicePath
    {
        get {
            return "";
        }
    }

    protected internal object uncastobj;

    public object UncastObject
    {
        get {
            return uncastobj;
        }
    }

    public abstract MessageElement CallGetProperty(string name, WrappedServiceSkelAsyncAdapter async_adapter);

    public abstract void CallSetProperty(string name, MessageElement m, WrappedServiceSkelAsyncAdapter async_adapter);

    public abstract MessageElement CallFunction(string name, vectorptr_messageelement m,
                                                WrappedServiceSkelAsyncAdapter async_adapter);

    public abstract object GetSubObj(string name, string ind);

    public virtual void RegisterEvents(object obj1)
    {}

    public virtual void UnregisterEvents(object obj1)
    {}

    public virtual void InitPipeServers(object obj1)
    {}
    public virtual void InitWireServers(object obj1)
    {}

    public override void ReleaseCastObject()
    {
        // RRObjectHeap.DeleteObject(innerskelid);
        innerskel = null;
    }

    public virtual object GetCallbackFunction(uint endpoint, string membername)
    {
        throw new MemberNotFoundException("Callback " + membername + " not found");
    }

    // public virtual MessageEntry CallMemoryFunction(MessageEntry m, Endpoint e) { throw new
    // MemberNotFoundException("Memory " + m.MemberName + " not found"); }

    internal int innerskelid;

    public override void Init(WrappedServiceSkel skel)
    {
        innerskel = skel;

        RegisterEvents(uncastobj);
        InitPipeServers(uncastobj);
        InitCallbackServers(uncastobj);
        InitWireServers(uncastobj);

        var init_object = uncastobj as IRRServiceObject;
        if (init_object != null)
        {
            init_object.RRServiceObjectInit(skel.GetContext(), skel.GetServicePath());
        }
    }
    public override MessageElement _CallFunction(string name, vectorptr_messageelement args,
                                                 WrappedServiceSkelAsyncAdapter async_adapter)
    {
        try
        {
            using (args)
            {
                return CallFunction(name, args, async_adapter);
            }
        }
        catch (Exception e)
        {
            using (MessageEntry merr = new MessageEntry())
            {
                RobotRaconteurExceptionUtil.ExceptionToMessageEntry(e, merr);
                RRDirectorExceptionHelper.SetError(merr, e.ToString());
                return null;
            }
        }
    }

    public override MessageElement _CallGetProperty(string name, WrappedServiceSkelAsyncAdapter async_adapter)
    {
        try
        {
            return CallGetProperty(name, async_adapter);
        }
        catch (Exception e)
        {
            using (MessageEntry merr = new MessageEntry())
            {
                RobotRaconteurExceptionUtil.ExceptionToMessageEntry(e, merr);
                RRDirectorExceptionHelper.SetError(merr, e.ToString());
                return null;
            }
        }
    }

    public override void _CallSetProperty(string name, MessageElement m, WrappedServiceSkelAsyncAdapter async_adapter)
    {
        try
        {
            using (m)
            {
                CallSetProperty(name, m, async_adapter);
            }
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

    public abstract string RRType { get; }

    public override WrappedRRObject _GetSubObj(string name, string index)
    {
        try
        {
            object newobj = GetSubObj(name, index);
            if (newobj == null)
                return null;
            ServiceSkel skel = RobotRaconteurNode.s.GetServiceType(RobotRaconteurNode.SplitQualifiedName(RRType).Item1)
                                   .CreateSkel(newobj);

            int id = RRObjectHeap.AddObject(skel);
            skel.innerskelid = id;
            return new WrappedRRObject(skel.RRType, skel, id);
        }
        catch (Exception e)
        {
            using (MessageEntry merr = new MessageEntry())
            {
                RobotRaconteurExceptionUtil.ExceptionToMessageEntry(e, merr);
                RRDirectorExceptionHelper.SetError(merr, e.ToString());
                return null;
            }
        }
    }

    public override void MonitorEnter(int timeout)
    {
        try
        {

            if (!(uncastobj is IRobotRaconteurMonitorObject))
            {
                throw new InvalidOperationException("Object is not monitor lockable");
            }

            ((IRobotRaconteurMonitorObject)uncastobj).RobotRaconteurMonitorEnter(timeout);
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

    public override void MonitorExit()
    {
        try
        {
            if (!(uncastobj is IRobotRaconteurMonitorObject))
            {
                throw new InvalidOperationException("Object is not monitor lockable");
            }

            ((IRobotRaconteurMonitorObject)uncastobj).RobotRaconteurMonitorExit();
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

    public override WrappedArrayMemoryDirector _GetArrayMemory(string name)
    {
        try
        {
            return GetArrayMemory(name);
        }
        catch (Exception e)
        {
            using (MessageEntry merr = new MessageEntry())
            {
                RobotRaconteurExceptionUtil.ExceptionToMessageEntry(e, merr);
                RRDirectorExceptionHelper.SetError(merr, e.ToString());
                return null;
            }
        }
    }

    public override WrappedMultiDimArrayMemoryDirector _GetMultiDimArrayMemory(string name)
    {
        try
        {
            return GetMultiDimArrayMemory(name);
        }
        catch (Exception e)
        {
            using (MessageEntry merr = new MessageEntry())
            {
                RobotRaconteurExceptionUtil.ExceptionToMessageEntry(e, merr);
                RRDirectorExceptionHelper.SetError(merr, e.ToString());
                return null;
            }
        }
    }

    public virtual WrappedArrayMemoryDirector GetArrayMemory(string name)
    {
        throw new MemberNotFoundException("Member not found");
    }

    public virtual WrappedMultiDimArrayMemoryDirector GetMultiDimArrayMemory(string name)
    {
        throw new MemberNotFoundException("Member not found");
    }

    public override WrappedPodArrayMemoryDirector _GetPodArrayMemory(string name)
    {
        try
        {
            return GetPodArrayMemory(name);
        }
        catch (Exception e)
        {
            using (MessageEntry merr = new MessageEntry())
            {
                RobotRaconteurExceptionUtil.ExceptionToMessageEntry(e, merr);
                RRDirectorExceptionHelper.SetError(merr, e.ToString());
                return null;
            }
        }
    }

    public override WrappedPodMultiDimArrayMemoryDirector _GetPodMultiDimArrayMemory(string name)
    {
        try
        {
            return GetPodMultiDimArrayMemory(name);
        }
        catch (Exception e)
        {
            using (MessageEntry merr = new MessageEntry())
            {
                RobotRaconteurExceptionUtil.ExceptionToMessageEntry(e, merr);
                RRDirectorExceptionHelper.SetError(merr, e.ToString());
                return null;
            }
        }
    }

    public virtual WrappedPodArrayMemoryDirector GetPodArrayMemory(string name)
    {
        throw new MemberNotFoundException("Member not found");
    }

    public virtual WrappedPodMultiDimArrayMemoryDirector GetPodMultiDimArrayMemory(string name)
    {
        throw new MemberNotFoundException("Member not found");
    }

    public override WrappedNamedArrayMemoryDirector _GetNamedArrayMemory(string name)
    {
        try
        {
            return GetNamedArrayMemory(name);
        }
        catch (Exception e)
        {
            using (MessageEntry merr = new MessageEntry())
            {
                RobotRaconteurExceptionUtil.ExceptionToMessageEntry(e, merr);
                RRDirectorExceptionHelper.SetError(merr, e.ToString());
                return null;
            }
        }
    }

    public override WrappedNamedMultiDimArrayMemoryDirector _GetNamedMultiDimArrayMemory(string name)
    {
        try
        {
            return GetNamedMultiDimArrayMemory(name);
        }
        catch (Exception e)
        {
            using (MessageEntry merr = new MessageEntry())
            {
                RobotRaconteurExceptionUtil.ExceptionToMessageEntry(e, merr);
                RRDirectorExceptionHelper.SetError(merr, e.ToString());
                return null;
            }
        }
    }

    public virtual WrappedNamedArrayMemoryDirector GetNamedArrayMemory(string name)
    {
        throw new MemberNotFoundException("Member not found");
    }

    public virtual WrappedNamedMultiDimArrayMemoryDirector GetNamedMultiDimArrayMemory(string name)
    {
        throw new MemberNotFoundException("Member not found");
    }
}

public class ServiceSkelUtil
{
    public static Type FindParentInterface(Type objtype)
    {

        List<Type> interfaces = new List<Type>(objtype.GetInterfaces());
        interfaces.RemoveAll(
            x => (x.GetCustomAttributes(typeof(RobotRaconteurServiceObjectInterface), true).Length == 0));

        if (interfaces.Count == 0)
            throw new DataTypeException("Object not a Robot Raconteur type");
        if (interfaces.Count == 1)
            return interfaces[0];

        List<Type> parentinterfaces = new List<Type>();

        for (int i = 0; i < interfaces.Count; i++)
        {
            bool parent = true;
            for (int j = 0; j < interfaces.Count; j++)
            {
                if (i != j)
                    if (interfaces[j].GetInterface(interfaces[i].ToString()) != null)
                        parent = false;
            }

            if (parent)
                parentinterfaces.Add(interfaces[i]);
        }

        if (parentinterfaces.Count != 1)
            throw new DataTypeException(
                "Robot Raconteur types can only directly inheret one Robot Raconteur interface type");

        return parentinterfaces[0];
    }
}

public partial class WrappedServiceSkelAsyncAdapter
{
    public void EndTask<T>(Task<T> task, Func<T, MessageElement> packer)
    {
        if (task.IsCanceled)
        {
            var exp = new RobotRaconteur.OperationCancelledException("Async operation was cancelled");
            var error_info = RobotRaconteurExceptionUtil.ExceptionToErrorInfo(exp);
            End(error_info);
            return;
        }

        if (task.IsFaulted)
        {
            var exp = task.Exception;
            var error_info = RobotRaconteurExceptionUtil.ExceptionToErrorInfo(exp.InnerException);
            End(error_info);
            return;
        }

        MessageElement res;
        try
        {
            res = packer(task.Result);
        }
        catch (Exception exp)
        {
            var error_info = RobotRaconteurExceptionUtil.ExceptionToErrorInfo(exp);
            End(error_info);
            return;
        }

        End(res, new HandlerErrorInfo());
    }

    public void EndTask(Task task)
    {
        if (task.IsCanceled)
        {
            var exp = new RobotRaconteur.OperationCancelledException("Async operation was cancelled");
            var error_info = RobotRaconteurExceptionUtil.ExceptionToErrorInfo(exp);
            End(error_info);
            return;
        }

        if (task.IsFaulted)
        {
            var exp = task.Exception;
            var error_info = RobotRaconteurExceptionUtil.ExceptionToErrorInfo(exp.InnerException);
            End(error_info);
            return;
        }

        End(new HandlerErrorInfo());
    }

    public void EndTask(Task task, MessageElement res)
    {
        if (task.IsCanceled)
        {
            var exp = new RobotRaconteur.OperationCancelledException("Async operation was cancelled");
            var error_info = RobotRaconteurExceptionUtil.ExceptionToErrorInfo(exp);
            End(error_info);
            return;
        }

        if (task.IsFaulted)
        {
            var exp = task.Exception;
            var error_info = RobotRaconteurExceptionUtil.ExceptionToErrorInfo(exp.InnerException);
            End(error_info);
            return;
        }

        End(res, new HandlerErrorInfo());
    }
}

public class RobotRaconteurServiceObjectInterface : Attribute
{}

}