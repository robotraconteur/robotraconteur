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
    public NamedArrayElementTypeAndCount(Type element_type, int element_array_count)
    {
        ElementArrayType = element_type;
        ElementArrayCount = element_array_count;
    }

    public Type ElementArrayType { get; }
    public int ElementArrayCount { get; }
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

public interface IRRServiceObject
{
    void RRServiceObjectInit(ServerContext context, string service_path);
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

public class RRObjectHeap
{

    class RRObjectHeap_support : RRNativeObjectHeapSupport
    {
        public override void DeleteObject_i(int id)
        {
            RRObjectHeap.DeleteObject(id);
        }
    }

    static RRObjectHeap_support support;

    static RRObjectHeap()
    {
        support = new RRObjectHeap_support();
        RRNativeObjectHeapSupport.Set_Support(support);
    }

    public static Dictionary<int, object> objects = new Dictionary<int, object>();
    public static int objectcount = 0;

    public static int AddObject(object o)
    {
        lock (objects)
        {
            if (objectcount > int.MaxValue - 10)
                objectcount = 0;
            objectcount++;
            while (objects.ContainsKey(objectcount))
                objectcount++;

            int i = objectcount;
            objects.Add(i, o);
            // Console.WriteLine(objects.Count);
            return i;
        }
    }

    public static void DeleteObject(int id)
    {
        lock (objects)
        {
            try
            {
                objects.Remove(id);
                // Console.WriteLine(objects.Count);
            }
            catch
            {

                // Console.WriteLine(e.ToString());
            }
        }
    }

    public static object GetObject(int id)
    {
        try
        {
            return objects[id];
        }
        catch (KeyNotFoundException)
        {
            throw new ApplicationException("Internal stub error");
        }
    }
}

public class RobotRaconteurServiceObjectInterface : Attribute
{}

public abstract class Callback<T>
{

    protected string m_MemberName;

    public Callback(string name)
    {
        m_MemberName = name;
    }

    public abstract T Function { get; set; }

    public abstract T GetClientFunction(uint e);
}

public class CallbackClient<T> : Callback<T>
{
    public CallbackClient(string name) : base(name)
    {}

    T function = default(T);
    public override T Function
    {
        get {
            return function;
        }
        set {
            function = value;
        }
    }

    public override T GetClientFunction(uint e)
    {
        throw new InvalidOperationException("Invalid for client side of callback");
    }
}

public class CallbackServer<T> : Callback<T>
{
    ServiceSkel skel;

    public CallbackServer(string name, ServiceSkel skel) : base(name)
    {
        this.skel = skel;
    }

    public override T Function
    {
        get {
            throw new InvalidOperationException("Invalid for server side of callback");
        }
        set {
            throw new InvalidOperationException("Invalid for server side of callback");
        }
    }

    public override T GetClientFunction(uint e)
    {
        return (T)skel.GetCallbackFunction(e, m_MemberName);
    }
}

public class Pipe<T>
{
    internal object innerpipe;

    Func<int, WrappedPipeEndpoint> ConnectFunction;
    Action<int, int, AsyncPipeEndpointReturnDirector, int> AsyncConnectFunction;
    string m_Membername;
    MemberDefinition_Direction direction;

    public string MemberName
    {
        get {
            return m_Membername;
        }
    }

    public Pipe(WrappedPipeClient innerpipe)
    {
        ConnectFunction = innerpipe.Connect;
        AsyncConnectFunction = innerpipe.AsyncConnect;
        this.innerpipe = innerpipe;
        m_Membername = innerpipe.GetMemberName();
        direction = innerpipe.Direction();
    }

    public Pipe(WrappedPipeServer innerpipe)
    {
        this.innerpipe = innerpipe;
        m_Membername = innerpipe.GetMemberName();
        direction = innerpipe.Direction();
    }

    ~Pipe()
    {
        if (innerpipe is WrappedPipeServer)
        {
            // ((WrappedPipeServer)innerpipe).SetRRDirector(null);
            // RRObjectHeap.DeleteObject(director_id);
        }
    }

    public MemberDefinition_Direction Direction
    {
        get {
            return direction;
        }
    }

    public PipeEndpoint Connect(int index = -1)
    {

        if (ConnectFunction == null)
            throw new InvalidOperationException("Invalid for server");
        return new PipeEndpoint(ConnectFunction(index));
    }

    private class AsyncConnectDirector : AsyncPipeEndpointReturnDirector
    {
        protected TaskCompletionSource<PipeEndpoint> handler_task =
            new TaskCompletionSource<PipeEndpoint>(TaskContinuationOptions.ExecuteSynchronously);

        public Task<PipeEndpoint> Task
        {
            get {
                return handler_task.Task;
            }
        }

        public AsyncConnectDirector()
        {}

        public override void handler(WrappedPipeEndpoint m, HandlerErrorInfo error)
        {
            if (error.error_code != 0)
            {
                using (MessageEntry merr = new MessageEntry())
                {
                    this.handler_task.SetException(RobotRaconteurExceptionUtil.ErrorInfoToException(error));
                    return;
                }
            }

            PipeEndpoint e1;

            try
            {
                e1 = new PipeEndpoint(m);
            }
            catch (Exception e)
            {
                handler_task.SetException(e);
                return;
            }

            handler_task.SetResult(e1);
        }
    }

    public Task<PipeEndpoint> AsyncConnect(int timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE)
    {
        return AsyncConnect(-1, timeout);
    }

    public async Task<PipeEndpoint> AsyncConnect(int index, int timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE)
    {

        if (AsyncConnectFunction == null)
            throw new InvalidOperationException("Invalid for server");
        AsyncConnectDirector h = new AsyncConnectDirector();
        int id = RRObjectHeap.AddObject(h);
        AsyncConnectFunction(index, timeout, h, id);
        return await h.Task;
    }

    // public delegate void PipeConnectCallbackFunction(PipeEndpoint newpipe);

    // public delegate void PipeDisconnectCallbackFunction(PipeEndpoint closedpipe);

    // public delegate void PipePacketReceivedCallbackFunction(PipeEndpoint e);

    // public delegate void PipePacketAckReceivedCallbackFunction(PipeEndpoint e, uint packetnum);

    public Action<PipeEndpoint> PipeConnectCallback
    {
        get {
            throw new InvalidOperationException("Read only property");
        }
        set {
            connectdirectorclass c = new connectdirectorclass(this, value);
            int id = RRObjectHeap.AddObject(c);
            ((WrappedPipeServer)innerpipe).SetWrappedPipeConnectCallback(c, id);
        }
    }

    class connectdirectorclass : WrappedPipeServerConnectDirector
    {
        Pipe<T> pipe;
        Action<PipeEndpoint> handler;

        public connectdirectorclass(Pipe<T> pipe, Action<PipeEndpoint> handler)
        {
            this.pipe = pipe;
            this.handler = handler;
        }

        public override void PipeConnectCallback(WrappedPipeEndpoint pipeendpoint)
        {
            try
            {
                handler(new PipeEndpoint(pipeendpoint));
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

    public class PipeEndpoint
    {
        WrappedPipeEndpoint innerpipe;

        public event Action<PipeEndpoint> PacketReceivedEvent;
        public event Action<PipeEndpoint, uint> PacketAckReceivedEvent;

        directorclass director;
        // int director_id;

        public PipeEndpoint(WrappedPipeEndpoint innerpipe)
        {
            this.innerpipe = innerpipe;
            director = new directorclass(this);

            innerpipe.SetRRDirector(director, RRObjectHeap.AddObject(this));
            // director_id = RRObjectHeap.AddObject(innerpipe);
        }

        ~PipeEndpoint()
        {
            // innerpipe.SetRRDirector(null);
            // RRObjectHeap.DeleteObject(director_id);
        }

        public int Index
        {
            get {
                return innerpipe.GetIndex();
            }
        }

        public uint Endpoint
        {
            get {
                return innerpipe.GetEndpoint();
            }
        }

        public bool RequestPacketAck
        {
            get {
                return innerpipe.GetRequestPacketAck();
            }
            set {
                innerpipe.SetRequestPacketAck(value);
            }
        }

        public bool IgnoreReceived
        {
            get {
                return innerpipe.GetIgnoreReceived();
            }
            set {
                innerpipe.SetIgnoreReceived(value);
            }
        }

        public MemberDefinition_Direction Direction
        {
            get {
                return innerpipe.Direction();
            }
        }

        private Action<PipeEndpoint> close_callback;

        public Action<PipeEndpoint> PipeCloseCallback
        {
            get {
                return close_callback;
            }
            set {
                close_callback = value;
            }
        }

        public int Available
        {
            get {
                return (int)innerpipe.Available();
            }
        }

        public bool IsUnreliable
        {
            get {
                return innerpipe.IsUnreliable();
            }
        }

        public T PeekNextPacket()
        {
            using (MessageElement m = innerpipe.PeekNextPacket())
            {
                object data = RobotRaconteurNode.s.UnpackAnyType<T>(m);
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
        }

        public T ReceivePacket()
        {
            using (MessageElement m = innerpipe.ReceivePacket())
            {
                object data = RobotRaconteurNode.s.UnpackAnyType<T>(m);
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
        }

        public T PeekNextPacketWait(int timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE)
        {
            using (MessageElement m = innerpipe.PeekNextPacketWait(timeout))
            {
                object data = RobotRaconteurNode.s.UnpackAnyType<T>(m);
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
        }

        public T ReceivePacketWait(int timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE)
        {
            using (MessageElement m = innerpipe.ReceivePacketWait(timeout))
            {
                object data = RobotRaconteurNode.s.UnpackAnyType<T>(m);
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
        }

        public bool TryReceivePacketWait(out T packet, int timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE,
                                         bool peek = false)
        {
            packet = default(T);
            WrappedTryReceivePacketWaitResult ret = innerpipe.TryReceivePacketWait(timeout, peek);
            using (ret) using (MessageElement m = ret.packet)
            {
                if (ret.res == false)
                    return false;
                object data = RobotRaconteurNode.s.UnpackAnyType<T>(m);
                if (data is Array)
                {
                    if (typeof(T).IsArray)
                        packet = (T)data;
                    else
                        packet = ((T[])data)[0];
                }
                else
                {
                    packet = (T)data;
                }

                return true;
            }
        }

        public uint SendPacket(T data)
        {

            try
            {
                using (MessageElement m = RobotRaconteurNode.s.PackAnyType<T>("value", ref data))
                {
                    return innerpipe.SendPacket(m);
                }
            }
            finally
            {}
        }

        public async Task<uint> AsyncSendPacket(T data)
        {

            AsyncUInt32ReturnDirectorImpl h = new AsyncUInt32ReturnDirectorImpl();

            try
            {
                using (MessageElement m = RobotRaconteurNode.s.PackAnyType<T>("value", ref data))
                {
                    int id = RRObjectHeap.AddObject(h);
                    innerpipe.AsyncSendPacket(m, h, id);
                }
            }
            finally
            {}
            return await h.Task;
        }

        public void Close()
        {
            innerpipe.Close();
        }

        public async Task AsyncClose(int timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE)
        {
            AsyncVoidReturnDirectorImpl h = new AsyncVoidReturnDirectorImpl();
            int id = RRObjectHeap.AddObject(h);
            innerpipe.AsyncClose(timeout, h, id);
            await h.Task;
        }

        class directorclass : WrappedPipeEndpointDirector
        {
            PipeEndpoint pipe;
            public directorclass(PipeEndpoint pipe)
            {
                this.pipe = pipe;
            }

            public override void PacketAckReceivedEvent(uint packetnum)
            {
                try
                {

                    if (pipe.PacketAckReceivedEvent == null)
                        return;
                    pipe.PacketAckReceivedEvent(pipe, packetnum);
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

            public override void PacketReceivedEvent()
            {
                try
                {
                    if (pipe.PacketReceivedEvent == null)
                        return;
                    pipe.PacketReceivedEvent(pipe);
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

            public override void PipeEndpointClosedCallback()
            {
                try
                {
                    if (pipe.PipeCloseCallback == null)
                        return;
                    pipe.PipeCloseCallback(pipe);
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
    }
}

public class PipeBroadcaster<T>
{
    protected Pipe<T> pipe;
    protected internal WrappedPipeBroadcaster innerpipe;
    protected TypeDefinition type;

    class WrappedPipeBroadcasterPredicateDirectorNET : WrappedPipeBroadcasterPredicateDirector
    {
        Func<uint, int, bool> f;

        public WrappedPipeBroadcasterPredicateDirectorNET(Func<uint, int, bool> f)
        {
            this.f = f;
        }

        public override bool Predicate(uint client_endpoint, int index)
        {
            return f(client_endpoint, index);
        }
    }

    public PipeBroadcaster(Pipe<T> pipe, int maximum_backlog = -1)
    {
        this.pipe = pipe;
        this.innerpipe = new WrappedPipeBroadcaster();
        this.innerpipe.Init((WrappedPipeServer)pipe.innerpipe, maximum_backlog);
        this.type = ((WrappedPipeServer)pipe.innerpipe).Type;
    }

    public Pipe<T> Pipe
    {
        get {
            return pipe;
        }
    }

    public async Task AsyncSendPacket(T packet)
    {
        AsyncVoidNoErrReturnDirectorImpl h = new AsyncVoidNoErrReturnDirectorImpl();

        try
        {
            using (MessageElement m = RobotRaconteurNode.s.PackAnyType<T>("value", ref packet))
            {
                int id = RRObjectHeap.AddObject(h);
                innerpipe.AsyncSendPacket(m, h, id);
            }
        }
        finally
        {}

        await h.Task;
    }

    public void SendPacket(T packet)
    {
        try
        {
            using (MessageElement m = RobotRaconteurNode.s.PackAnyType<T>("value", ref packet))
            {
                innerpipe.SendPacket(m);
                return;
            }
        }
        finally
        {}
    }

    public uint ActivePipeEndpointCount
    {
        get {
            return innerpipe.GetActivePipeEndpointCount();
        }
    }

    public Func<uint, int, bool> Predicate
    {
        set {
            var p = new WrappedPipeBroadcasterPredicateDirectorNET(value);
            int id = RRObjectHeap.AddObject(p);
            innerpipe.SetPredicateDirector(p, id);
        }
    }

    public int MaxBacklog
    {
        get {
            return innerpipe.GetMaxBacklog();
        }
        set {
            innerpipe.SetMaxBacklog(value);
        }
    }
}

public class Wire<T>
{
    internal object innerwire;

    string m_Membername;
    MemberDefinition_Direction direction;

    public string MemberName
    {
        get {
            return this.m_Membername;
        }
    }

    public MemberDefinition_Direction Direction
    {
        get {
            return direction;
        }
    }

    public Wire(WrappedWireClient innerwire)
    {
        this.innerwire = innerwire;
        m_Membername = innerwire.GetMemberName();
        direction = innerwire.Direction();
    }

    public Wire(WrappedWireServer innerwire)
    {
        this.innerwire = innerwire;
        m_Membername = innerwire.GetMemberName();
        // director_id = RRObjectHeap.AddObject(director);
        direction = innerwire.Direction();
    }

    ~Wire()
    {
        if (innerwire is WrappedWireServer)
        {
            // ((WrappedWireServer)innerwire).SetRRDirector(null);
            // RRObjectHeap.DeleteObject(director_id);
        }
    }

    public WireConnection Connect()
    {
        WrappedWireClient c = innerwire as WrappedWireClient;
        if (c == null)
            throw new InvalidOperationException("Invalid for server");
        return new WireConnection(c.Connect());
    }

    private class AsyncConnectDirector : AsyncWireConnectionReturnDirector
    {
        protected TaskCompletionSource<WireConnection> handler_task =
            new TaskCompletionSource<WireConnection>(TaskContinuationOptions.ExecuteSynchronously);

        public Task<WireConnection> Task
        {
            get {
                return handler_task.Task;
            }
        }

        public AsyncConnectDirector()
        {}

        public override void handler(WrappedWireConnection m, HandlerErrorInfo error)
        {
            if (error.error_code != 0)
            {
                using (MessageEntry merr = new MessageEntry())
                {
                    this.handler_task.SetException(RobotRaconteurExceptionUtil.ErrorInfoToException(error));
                    return;
                }
            }

            WireConnection e1;

            try
            {
                e1 = new WireConnection(m);
            }
            catch (Exception e)
            {
                handler_task.SetException(e);
                return;
            }

            handler_task.SetResult(e1);
        }
    }

    public async Task<WireConnection> AsyncConnect(int timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE)
    {
        WrappedWireClient c = innerwire as WrappedWireClient;
        if (c == null)
            throw new InvalidOperationException("Invalid for server");
        AsyncConnectDirector h = new AsyncConnectDirector();
        int id = RRObjectHeap.AddObject(h);
        c.AsyncConnect(timeout, h, id);
        return await h.Task;
    }

    public Action<Wire<T>, WireConnection> WireConnectCallback
    {
        get {
            throw new InvalidOperationException("Read only property");
        }
        set {
            connectdirectorclass c = new connectdirectorclass(this, value);
            int id = RRObjectHeap.AddObject(c);
            ((WrappedWireServer)innerwire).SetWrappedWireConnectCallback(c, id);
        }
    }

    // public delegate void WireConnectCallbackFunction(Wire<T> wire, WireConnection connection);

    // public delegate void WireDisconnectCallbackFunction(WireConnection wire);

    // public delegate void WireValueChangedFunction(WireConnection connection, T value, TimeSpec time);

    class connectdirectorclass : WrappedWireServerConnectDirector
    {
        Wire<T> wire;
        Action<Wire<T>, WireConnection> handler;

        public connectdirectorclass(Wire<T> wire, Action<Wire<T>, WireConnection> handler)
        {
            this.wire = wire;
            this.handler = handler;
        }

        public override void WireConnectCallback(WrappedWireConnection pipeendpoint)
        {
            try
            {
                handler(wire, new WireConnection(pipeendpoint));
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

    class peekdirectorclass : AsyncWirePeekReturnDirector
    {
        protected TaskCompletionSource<Tuple<T, TimeSpec>> handler_task =
            new TaskCompletionSource<Tuple<T, TimeSpec>>(TaskContinuationOptions.ExecuteSynchronously);

        public Task<Tuple<T, TimeSpec>> Task
        {
            get {
                return handler_task.Task;
            }
        }

        internal peekdirectorclass()
        {}

        public override void handler(MessageElement m, TimeSpec ts, HandlerErrorInfo error)
        {
            using (m)
            {
                try
                {
                    this.Dispose();

                    if (error.error_code != 0)
                    {
                        using (MessageEntry merr = new MessageEntry())
                        {

                            this.handler_task.SetException(RobotRaconteurExceptionUtil.ErrorInfoToException(error));
                            return;
                        }
                    }

                    this.handler_task.SetResult(Tuple.Create(RobotRaconteurNode.s.UnpackAnyType<T>(m), ts));
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
    }

    public T PeekInValue(out TimeSpec ts)
    {
        WrappedWireClient c = innerwire as WrappedWireClient;
        if (c == null)
            throw new InvalidOperationException("Invalid for server");
        ts = new TimeSpec();
        using (MessageElement m = c.PeekInValue(ts))
        {
            return RobotRaconteurNode.s.UnpackAnyType<T>(m);
        }
    }

    public T PeekOutValue(out TimeSpec ts)
    {
        WrappedWireClient c = innerwire as WrappedWireClient;
        if (c == null)
            throw new InvalidOperationException("Invalid for server");
        ts = new TimeSpec();
        using (MessageElement m = c.PeekOutValue(ts))
        {
            return RobotRaconteurNode.s.UnpackAnyType<T>(m);
        }
    }

    public void PokeOutValue(T value)
    {
        WrappedWireClient c = innerwire as WrappedWireClient;
        if (c == null)
            throw new InvalidOperationException("Invalid for server");

        try
        {

            using (MessageElement m = RobotRaconteurNode.s.PackAnyType<T>("value", ref value))
            {
                c.PokeOutValue(m);
            }
        }
        finally
        {}
    }

    public async Task<Tuple<T, TimeSpec>> AsyncPeekInValue(int timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE)
    {
        WrappedWireClient c = innerwire as WrappedWireClient;
        if (c == null)
            throw new InvalidOperationException("Invalid for server");

        peekdirectorclass h = new peekdirectorclass();
        int id = RRObjectHeap.AddObject(h);
        c.AsyncPeekInValue(timeout, h, id);
        return await h.Task;
    }

    public async Task<Tuple<T, TimeSpec>> AsyncPeekOutValue(int timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE)
    {
        WrappedWireClient c = innerwire as WrappedWireClient;
        if (c == null)
            throw new InvalidOperationException("Invalid for server");

        peekdirectorclass h = new peekdirectorclass();
        int id = RRObjectHeap.AddObject(h);
        c.AsyncPeekOutValue(timeout, h, id);
        return await h.Task;
    }

    public async Task AsyncPokeOutValue(T value, int timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE)
    {
        WrappedWireClient c = innerwire as WrappedWireClient;
        if (c == null)
            throw new InvalidOperationException("Invalid for server");
        try
        {
            using (MessageElement m = RobotRaconteurNode.s.PackAnyType<T>("value", ref value))
            {
                AsyncVoidReturnDirectorImpl h = new AsyncVoidReturnDirectorImpl();
                int id = RRObjectHeap.AddObject(h);
                c.AsyncPokeOutValue(m, timeout, h, id);
                await h.Task;
            }
        }
        finally
        {}
    }

    class peekcallbackclass : WrappedWireServerPeekValueDirector
    {
        Func<uint, T> cb;

        public peekcallbackclass(Func<uint, T> cb)
        {
            this.cb = cb;
        }

        public override MessageElement PeekValue(uint ep)
        {
            var value = cb(ep);

            MessageElement m = RobotRaconteurNode.s.PackAnyType<T>("value", ref value);
            return m;
        }
    }

    class pokecallbackclass : WrappedWireServerPokeValueDirector
    {
        Action<T, TimeSpec, uint> cb;

        public pokecallbackclass(Action<T, TimeSpec, uint> cb)
        {
            this.cb = cb;
        }

        public override void PokeValue(MessageElement el, TimeSpec ts, uint ep)
        {
            T value = RobotRaconteurNode.s.UnpackAnyType<T>(el);
            cb(value, ts, ep);
        }
    }

    public Func<uint, T> PeekInValueCallback
    {
        get {
            throw new InvalidOperationException("Read only property");
        }
        set {
            peekcallbackclass c = new peekcallbackclass(value);
            int id = RRObjectHeap.AddObject(c);
            ((WrappedWireServer)innerwire).SetPeekInValueCallback(c, id);
        }
    }

    public Func<uint, T> PeekOutValueCallback
    {
        get {
            throw new InvalidOperationException("Read only property");
        }
        set {
            peekcallbackclass c = new peekcallbackclass(value);
            int id = RRObjectHeap.AddObject(c);
            ((WrappedWireServer)innerwire).SetPeekOutValueCallback(c, id);
        }
    }

    public Action<T, TimeSpec, uint> PokeOutValueCallback
    {
        get {
            throw new InvalidOperationException("Read only property");
        }
        set {
            pokecallbackclass c = new pokecallbackclass(value);
            int id = RRObjectHeap.AddObject(c);
            ((WrappedWireServer)innerwire).SetPokeOutValueCallback(c, id);
        }
    }

    public class WireConnection
    {
        WrappedWireConnection innerwire;

        directorclass director;
        // int director_id = 0;

        public WireConnection(WrappedWireConnection innerwire)
        {
            this.innerwire = innerwire;
            director = new directorclass(this);
            // director_id = RRObjectHeap.AddObject(director);
            // RRObjectHeap.AddObject(innerwire);

            innerwire.SetRRDirector(director, RRObjectHeap.AddObject(this));
        }

        ~WireConnection()
        {
            /// innerwire.SetRRDirector(null);
            // RRObjectHeap.DeleteObject(director_id);
            try
            {

                // innerwire.Close();
            }
            catch
            {}
        }

        public MemberDefinition_Direction Direction
        {
            get {
                return innerwire.Direction();
            }
        }

        public uint Endpoint
        {
            get {
                return innerwire.GetEndpoint();
            }
        }

        public T InValue
        {
            get {
                using (MessageElement m = innerwire.GetInValue())
                {
                    return RobotRaconteurNode.s.UnpackAnyType<T>(m);
                }
            }
        }

        public T OutValue
        {
            get {
                using (MessageElement m = innerwire.GetOutValue())
                {
                    return RobotRaconteurNode.s.UnpackAnyType<T>(m);
                }
            }

            set {

                try
                {
                    using (MessageElement m = RobotRaconteurNode.s.PackAnyType<T>("value", ref value))
                    {
                        innerwire.SetOutValue(m);
                    }
                }
                finally
                {}
            }
        }

        public TimeSpec LastValueReceivedTime
        {
            get {
                return innerwire.GetLastValueReceivedTime();
            }
        }

        public TimeSpec LastValueSentTime
        {
            get {
                return innerwire.GetLastValueSentTime();
            }
        }

        bool TryGetInValue(out T value, out TimeSpec ts)
        {
            value = default(T);
            ts = default(TimeSpec);
            TryGetValueResult ret = innerwire.TryGetInValue();
            using (ret) using (MessageElement m = ret.value)
            {
                if (ret.res == false)
                    return false;
                ts = ret.ts;
                value = RobotRaconteurNode.s.UnpackAnyType<T>(m);
                return true;
            }
        }

        bool TryGetOutValue(out T value, out TimeSpec ts)
        {
            value = default(T);
            ts = default(TimeSpec);
            TryGetValueResult ret = innerwire.TryGetOutValue();
            using (ret) using (MessageElement m = ret.value)
            {
                if (ret.res == false)
                    return false;
                ts = ret.ts;
                value = RobotRaconteurNode.s.UnpackAnyType<T>(m);
                return true;
            }
        }

        public void Close()
        {
            innerwire.Close();
        }

        public async Task AsyncClose(int timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE)
        {
            AsyncVoidReturnDirectorImpl h = new AsyncVoidReturnDirectorImpl();
            int id = RRObjectHeap.AddObject(h);
            innerwire.AsyncClose(timeout, h, id);
            await h.Task;
        }

        public event Action<Wire<T>.WireConnection, T, TimeSpec> WireValueChanged;

        private Action<WireConnection> close_callback;

        public Action<WireConnection> WireCloseCallback
        {
            get {
                return close_callback;
            }
            set {
                close_callback = value;
            }
        }

        public bool InValueValid
        {
            get {
                return innerwire.GetInValueValid();
            }
        }

        public bool OutValueValid
        {
            get {
                return innerwire.GetOutValueValid();
            }
        }

        public bool IgnoreInValue
        {
            get {
                return innerwire.GetIgnoreInValue();
            }
            set {
                innerwire.SetIgnoreInValue(value);
            }
        }

        public int InValueLifespan
        {
            get {
                return innerwire.GetInValueLifespan();
            }
            set {
                innerwire.SetInValueLifespan(value);
            }
        }

        public int OutValueLifespan
        {
            get {
                return innerwire.GetOutValueLifespan();
            }
            set {
                innerwire.SetOutValueLifespan(value);
            }
        }

        public bool WaitInValueValid(int timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE)
        {
            return innerwire.WaitInValueValid(timeout);
        }

        public bool WaitOutValueValid(int timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE)
        {
            return innerwire.WaitOutValueValid(timeout);
        }

        class directorclass : WrappedWireConnectionDirector
        {

            WireConnection wire;

            public directorclass(WireConnection wire)
            {
                this.wire = wire;
            }

            public override void WireConnectionClosedCallback()
            {
                try
                {
                    if (wire.WireCloseCallback == null)
                        return;
                    wire.WireCloseCallback(wire);
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

            public override void WireValueChanged(MessageElement value, TimeSpec time)
            {
                try
                {
                    using (value)
                    {
                        if (wire.WireValueChanged == null)
                            return;

                        object data = RobotRaconteurNode.s.UnpackAnyType<T>(value);
                        T data2;
                        if (data is Array)
                        {
                            if (typeof(T).IsArray)
                                data2 = (T)data;
                            else
                                data2 = ((T[])data)[0];
                        }
                        else
                        {
                            data2 = (T)data;
                        }

                        wire.WireValueChanged(wire, data2, time);
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
        }
    }
}

public class WireBroadcaster<T>
{
    class WrappedWireBroadcasterPredicateDirectorNET : WrappedWireBroadcasterPredicateDirector
    {
        Func<uint, bool> f;

        public WrappedWireBroadcasterPredicateDirectorNET(Func<uint, bool> f)
        {
            this.f = f;
        }

        public override bool Predicate(uint client_endpoint)
        {
            return f(client_endpoint);
        }
    }

    class peekcallbackclass : WrappedWireServerPeekValueDirector
    {
        Func<uint, T> cb;

        public peekcallbackclass(Func<uint, T> cb)
        {
            this.cb = cb;
        }

        public override MessageElement PeekValue(uint ep)
        {
            var value = cb(ep);

            MessageElement m = RobotRaconteurNode.s.PackAnyType<T>("value", ref value);
            return m;
        }
    }

    protected Wire<T> wire;

    protected internal WrappedWireBroadcaster innerwire;
    protected TypeDefinition type;

    public WireBroadcaster(Wire<T> wire)
    {
        this.wire = wire;
        this.innerwire = new WrappedWireBroadcaster();
        this.innerwire.Init((WrappedWireServer)wire.innerwire);
        this.type = ((WrappedWireServer)wire.innerwire).Type;
    }

    public Wire<T> Wire
    {
        get {
            return wire;
        }
    }

    public T OutValue
    {
        set {
            try
            {
                using (MessageElement m = RobotRaconteurNode.s.PackAnyType<T>("value", ref value))
                {
                    innerwire.SetOutValue(m);
                }
            }
            finally
            {}
        }
    }

    public uint ActiveWireConnectionCount
    {
        get {
            return innerwire.GetActiveWireConnectionCount();
        }
    }

    public Func<uint, bool> Predicate
    {
        set {
            var p = new WrappedWireBroadcasterPredicateDirectorNET(value);
            int id = RRObjectHeap.AddObject(p);
            innerwire.SetPredicateDirector(p, id);
        }
    }

    public int OutValueLifespan
    {
        get {
            return innerwire.GetOutValueLifespan();
        }
        set {
            innerwire.SetOutValueLifespan(value);
        }
    }

    public Func<uint, T> PeekInValueCallback
    {
        get {
            throw new InvalidOperationException("Read only property");
        }
        set {
            peekcallbackclass c = new peekcallbackclass(value);
            int id = RRObjectHeap.AddObject(c);
            innerwire.SetPeekInValueCallback(c, id);
        }
    }
}

public class WireUnicastReceiver<T>
{
    protected Wire<T> wire;
    protected WrappedWireUnicastReceiver innerwire;
    protected TypeDefinition type;

    public WireUnicastReceiver(Wire<T> wire)
    {
        this.wire = wire;
        this.innerwire = new WrappedWireUnicastReceiver();
        this.innerwire.Init((WrappedWireServer)wire.innerwire);
        this.type = ((WrappedWireServer)wire.innerwire).Type;

        var cb = new ValueChangedDirector(this);
        var id = RRObjectHeap.AddObject(cb);
        this.innerwire.AddInValueChangedListener(cb, id);
    }

    public Wire<T> Wire
    {
        get {
            return wire;
        }
    }

    public T GetInValue(out TimeSpec ts, out uint ep)
    {
        ts = new TimeSpec();
        var m = innerwire.GetInValue(ts, out ep);
        return RobotRaconteurNode.s.UnpackAnyType<T>(m);
    }

    public bool TryGetInValue(out T value, out TimeSpec time, out uint client)
    {
        var m = new WrappedService_typed_packet();
        var t = new TimeSpec();
        using (m)
        {
            if (innerwire.TryGetInValue(m, t))
            {
                time = t;
                client = m.client;
                using (var m1 = m.packet)
                {
                    value = RobotRaconteurNode.s.UnpackAnyType<T>(m1);
                    return true;
                }
            }
            else
            {
                time = null;
                value = default(T);
                client = 0;
                return false;
            }
        }
    }

    class ValueChangedDirector : WrappedWireServerPokeValueDirector
    {
        WeakReference cb;

        public ValueChangedDirector(WireUnicastReceiver<T> cb)
        {
            this.cb = new WeakReference(cb);
        }

        public override void PokeValue(MessageElement el, TimeSpec ts, uint ep)
        {
            WireUnicastReceiver<T> cb1 = cb.Target as WireUnicastReceiver<T>;
            if (cb1 == null)
                return;

            T value = RobotRaconteurNode.s.UnpackAnyType<T>(el);
            cb1.ValueChanged(value, ts, ep);
        }
    }

    internal void ValueChanged(T value, TimeSpec ts, uint ep)
    {
        if (InValueChanged != null)
        {
            InValueChanged(value, ts, ep);
        }
    }

    public event Action<T, TimeSpec, uint> InValueChanged;

    public int InValueLifespan
    {
        get {
            return innerwire.GetInValueLifespan();
        }
        set {
            innerwire.SetInValueLifespan(value);
        }
    }
}

public interface IRobotRaconteurMonitorObject
{
    void RobotRaconteurMonitorEnter();

    void RobotRaconteurMonitorEnter(int timeout);

    void RobotRaconteurMonitorExit();
}

public class ClientServiceListenerDirectorNET : ClientServiceListenerDirector
{

    public int listenerid;

    RobotRaconteurNode.ClientServiceListenerDelegate listener;
    internal ServiceStub stub;

    public ClientServiceListenerDirectorNET(RobotRaconteurNode.ClientServiceListenerDelegate listener)
    {
        this.listener = listener;
    }

    public override void Callback(int code)
    {
        listener(stub, (ClientServiceListenerEventType)code, null);
    }

    public override void Callback2(int code, string p)
    {
        listener(stub, (ClientServiceListenerEventType)code, p);
    }
}

public class ServerServiceListenerDirectorNET : ServerServiceListenerDirector
{

    public int listenerid;

    ServerContext.ServerServiceListenerDelegate listener;
    WeakReference context = new WeakReference(null);

    public ServerServiceListenerDirectorNET(ServerContext context, ServerContext.ServerServiceListenerDelegate listener)
    {
        this.listener = listener;
        this.context.Target = context;
    }

    public override void Callback(int code, uint endpoint)
    {
        try
        {
            listener((ServerContext)context.Target, (ServerServiceListenerEventType)code, endpoint);
        }
        catch
        {}
    }
}

public partial class ServiceSecurityPolicy
{
    private class WrappedUserAuthenticatorDirectorNET : WrappedUserAuthenticatorDirector
    {
        IUserAuthenticator target;

        public WrappedUserAuthenticatorDirectorNET(IUserAuthenticator target)
        {
            this.target = target;
        }

        protected override AuthenticatedUser AuthenticateUser(string username, MessageElement credentials,
                                                              ServerContext context)
        {
            using (credentials)
            {
                IUserAuthenticator t = target;
                if (t == null)
                    throw new AuthenticationException("Authenticator internal error");

                Dictionary<string, object> c2 =
                    (Dictionary<string, object>)RobotRaconteurNode.s.UnpackMapType<string, object>(credentials);

                AuthenticatedUser ret = t.AuthenticateUser(username, c2, context);
                return ret;
            }
        }
    }

    public ServiceSecurityPolicy(IUserAuthenticator authenticator, Dictionary<string, string> policies)
        : this(init_authenticator(authenticator), init_policies(policies))
    {}

    static private NativeUserAuthenticator init_authenticator(IUserAuthenticator authenticator)
    {
        if (authenticator is NativeUserAuthenticator)
        {
            return (NativeUserAuthenticator)authenticator;
        }

        WrappedUserAuthenticatorDirectorNET n = new WrappedUserAuthenticatorDirectorNET(authenticator);
        int id = RRObjectHeap.AddObject(n);
        WrappedUserAuthenticator a2 = new WrappedUserAuthenticator();
        a2.SetRRDirector(n, id);
        return a2;
    }

    static private map_strstr init_policies(Dictionary<string, string> policies)
    {
        map_strstr s = new map_strstr();
        foreach (KeyValuePair<string, string> v in policies)
        {
            s.Add(v.Key, v.Value);
        }
        return s;
    }
}

public interface IUserAuthenticator
{
    AuthenticatedUser AuthenticateUser(string username, Dictionary<string, object> credentials, ServerContext context);
}

public partial class NativeUserAuthenticator
{

    public virtual AuthenticatedUser AuthenticateUser(string username, Dictionary<string, object> credentials,
                                                      ServerContext context)
    {
        MessageElement c = (MessageElement)RobotRaconteurNode.s.PackMapType<string, object>(credentials);
        return _AuthenticateUser(username, c, context);
    }
}

public partial class AuthenticatedUser
{
    public string Username
    {
        get {
            return _GetUsername();
        }
    }

    public string[] Privileges
    {
        get {
            vectorstring s = _GetPrivileges();
            string[] o = new string[s.Count];
            s.CopyTo(o);
            return o;
        }
    }

    public string[] Properties
    {
        get {
            vectorstring s = _GetProperties();
            string[] o = new string[s.Count];
            s.CopyTo(o);
            return o;
        }
    }

    public DateTime LoginTime
    {
        get {
            return _GetLoginTime();
        }
    }

    public DateTime LastAccessTime
    {
        get {
            return _GetLastAccessTime();
        }
    }
}

}
