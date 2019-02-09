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
using System.Collections.Generic;
using System.Linq;

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

        public MessageElementStructure PackStructure(object s)
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

        public T UnpackStructure<T>(MessageElementStructure l)
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

        public MessageElementCStructureArray PackCStructureToArray<T>(ref T s) where T : struct
        {
            string typename;
            if (CompareNamespace(RobotRaconteurNode.GetTypeString(s.GetType()), out typename))
            {
                return ((CStructureStub<T>)FindCStructureStub(typename)).PackCStructureToArray(ref s);
            }
            else
            {
                return RobotRaconteurNode.s.PackCStructureToArray(ref s);
            }
        }

        public T UnpackCStructureFromArray<T>(MessageElementCStructureArray l) where T : struct
        {
            string typename;
            if (CompareNamespace(l.GetTypeString(), out typename))
            {
                return ((CStructureStub<T>)FindCStructureStub(typename)).UnpackCStructureFromArray(l);
            }
            else
            {
                return RobotRaconteurNode.s.UnpackCStructureFromArray<T>(l);
            }
        }

        public MessageElementCStructureArray PackCStructureArray<T>(T[] s) where T : struct
        {
            string typename;
            if (CompareNamespace(RobotRaconteurNode.GetTypeString(s.GetType().GetElementType()), out typename))
            {
                return ((CStructureStub<T>)FindCStructureStub(typename)).PackCStructureArray(s);
            }
            else
            {
                return RobotRaconteurNode.s.PackCStructureArray(s);
            }
        }

        public T[] UnpackCStructureArray<T>(MessageElementCStructureArray l) where T : struct
        {
            string typename;
            if (CompareNamespace(l.GetTypeString(), out typename))
            {
                return ((CStructureStub<T>)FindCStructureStub(typename)).UnpackCStructureArray(l);
            }
            else
            {
                return RobotRaconteurNode.s.UnpackCStructureArray<T>(l);
            }
        }

        public MessageElementCStructureMultiDimArray PackCStructureMultiDimArray<T>(CStructureMultiDimArray s) where T: struct
        {
            string typename;
            if (CompareNamespace(RobotRaconteurNode.GetTypeString(s.cstruct_array.GetType().GetElementType()), out typename))
            {
                return ((CStructureStub<T>)FindCStructureStub(typename)).PackCStructureMultiDimArray(s);
            }
            else
            {
                return RobotRaconteurNode.s.PackCStructureMultiDimArray<T>(s);
            }
        }

        public CStructureMultiDimArray UnpackCStructureMultiDimArray<T>(MessageElementCStructureMultiDimArray l) where T: struct
        {
            string typename;
            if (CompareNamespace(l.GetTypeString(), out typename))
            {
                return ((CStructureStub<T>)FindCStructureStub(typename)).UnpackCStructureMultiDimArray(l);
            }
            else
            {
                return RobotRaconteurNode.s.UnpackCStructureMultiDimArray<T>(l);
            }
        }

        public MessageElementData PackCStructure(object s)
        {
            Type t;

            var s1 = s as CStructureMultiDimArray;
            if (s1 != null)
            {
                t = s1.cstruct_array.GetType().GetElementType();
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
                return FindCStructureStub(typename).PackCStructure(s);
            }
            else
            {
                return RobotRaconteurNode.s.PackCStructure(s);
            }
        }

        public object UnpackCStructure(MessageElementData m)
        {
            string typename;
            if (CompareNamespace(m.GetTypeString(), out typename))
            {
                return FindCStructureStub(typename).UnpackCStructure(m);
            }
            else
            {
                return RobotRaconteurNode.s.UnpackCStructure(m);
            }
        }

        //astruct

        public MessageElementAStructureArray PackAStructureToArray<T>(ref T s) where T : struct
        {
            string typename;
            if (CompareNamespace(RobotRaconteurNode.GetTypeString(s.GetType()), out typename))
            {
                return ((IAStructureStub<T>)FindAStructureStub(typename)).PackAStructureToArray(ref s);
            }
            else
            {
                return RobotRaconteurNode.s.PackAStructureToArray(ref s);
            }
        }

        public T UnpackAStructureFromArray<T>(MessageElementAStructureArray l) where T : struct
        {
            string typename;
            if (CompareNamespace(l.GetTypeString(), out typename))
            {
                return ((IAStructureStub<T>)FindAStructureStub(typename)).UnpackAStructureFromArray(l);
            }
            else
            {
                return RobotRaconteurNode.s.UnpackAStructureFromArray<T>(l);
            }
        }

        public MessageElementAStructureArray PackAStructureArray<T>(T[] s) where T : struct
        {
            string typename;
            if (CompareNamespace(RobotRaconteurNode.GetTypeString(s.GetType().GetElementType()), out typename))
            {
                return ((IAStructureStub<T>)FindAStructureStub(typename)).PackAStructureArray(s);
            }
            else
            {
                return RobotRaconteurNode.s.PackAStructureArray(s);
            }
        }

        public T[] UnpackAStructureArray<T>(MessageElementAStructureArray l) where T : struct
        {
            string typename;
            if (CompareNamespace(l.GetTypeString(), out typename))
            {
                return ((IAStructureStub<T>)FindAStructureStub(typename)).UnpackAStructureArray(l);
            }
            else
            {
                return RobotRaconteurNode.s.UnpackAStructureArray<T>(l);
            }
        }

        public MessageElementAStructureMultiDimArray PackAStructureMultiDimArray<T>(AStructureMultiDimArray s) where T : struct
        {
            string typename;
            if (CompareNamespace(RobotRaconteurNode.GetTypeString(s.astruct_array.GetType().GetElementType()), out typename))
            {
                return ((IAStructureStub<T>)FindAStructureStub(typename)).PackAStructureMultiDimArray(s);
            }
            else
            {
                return RobotRaconteurNode.s.PackAStructureMultiDimArray<T>(s);
            }
        }

        public AStructureMultiDimArray UnpackAStructureMultiDimArray<T>(MessageElementAStructureMultiDimArray l) where T : struct
        {
            string typename;
            if (CompareNamespace(l.GetTypeString(), out typename))
            {
                return ((IAStructureStub<T>)FindAStructureStub(typename)).UnpackAStructureMultiDimArray(l);
            }
            else
            {
                return RobotRaconteurNode.s.UnpackAStructureMultiDimArray<T>(l);
            }
        }

        public MessageElementData PackAStructure(object s)
        {
            Type t;

            var s1 = s as AStructureMultiDimArray;
            if (s1 != null)
            {
                t = s1.astruct_array.GetType().GetElementType();
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
                return FindAStructureStub(typename).PackAStructure(s);
            }
            else
            {
                return RobotRaconteurNode.s.PackAStructure(s);
            }
        }

        public object UnpackAStructure(MessageElementData m)
        {
            string typename;
            if (CompareNamespace(m.GetTypeString(), out typename))
            {
                return FindAStructureStub(typename).UnpackAStructure(m);
            }
            else
            {
                return RobotRaconteurNode.s.UnpackAStructure(m);
            }
        }

        public abstract IStructureStub FindStructureStub(string objecttype);

        public abstract ICStructureStub FindCStructureStub(string objecttype);

        public abstract IAStructureStub FindAStructureStub(string objecttype);

        public abstract ServiceStub CreateStub(WrappedServiceStub innerstub);

        public abstract ServiceSkel CreateSkel(object innerskel);

        public abstract string GetServiceName();

        public abstract RobotRaconteurException DownCastException(RobotRaconteurException exp);
    }

    public interface IStructureStub
    {
        MessageElementStructure PackStructure(Object s);

        T UnpackStructure<T>(MessageElementStructure m);
    }

    public interface ICStructureStub
    {
        MessageElementData PackCStructure(object s);

        object UnpackCStructure(MessageElementData m);
    }

    public abstract class CStructureStub<T> : ICStructureStub where T :struct
    {
        public abstract MessageElementCStructure PackCStructure(ref T s);
        
        public abstract T UnpackCStructure(MessageElementCStructure m);


        public virtual MessageElementCStructureArray PackCStructureToArray(ref T s2)
        {
            using (var mm = new vectorptr_messageelement())
            {                
                MessageElementUtil.AddMessageElementDispose(mm,
                    MessageElementUtil.NewMessageElementDispose(0, PackCStructure(ref s2))
                    );
                
                return new MessageElementCStructureArray(TypeName, mm);
            }
        }

        public virtual MessageElementCStructureArray PackCStructureArray(T[] s2)
        {
            if (s2 == null) return null;

            using (var mm = new vectorptr_messageelement())
            {
                for (int i = 0; i < s2.Length; i++)
                {
                    MessageElementUtil.AddMessageElementDispose(mm,
                        MessageElementUtil.NewMessageElementDispose(i, PackCStructure(ref s2[i]))
                        );
                }
                return new MessageElementCStructureArray(TypeName, mm);
            }
        }

        public virtual MessageElementCStructureMultiDimArray PackCStructureMultiDimArray(CStructureMultiDimArray s3)
        {
            if (s3 == null) return null;
            using (vectorptr_messageelement l = new vectorptr_messageelement())
            {
                MessageElementUtil.AddMessageElementDispose(l, "dims", s3.Dims);
                using (var s4 = PackCStructureArray((T[])s3.cstruct_array))
                {
                    MessageElementUtil.AddMessageElementDispose(l, "array", s4);
                    return new MessageElementCStructureMultiDimArray(TypeName, l);
                }
            }
        }

        public virtual T UnpackCStructureFromArray(MessageElementCStructureArray s2)
        {
            if (s2.Type != TypeName) throw new DataTypeException("cstructure type mismatch");            
            using (vectorptr_messageelement cdataElements = s2.Elements)
            {
                if (cdataElements.Count != 1) throw new DataTypeException("cstructure type mismatch");

                var e = cdataElements[0];                
                using (e)
                {
                    if (0 != MessageElementUtil.GetMessageElementNumber(e)) throw new DataTypeException("Error in list format");
                    using (MessageElementCStructure md = (MessageElementCStructure)e.Data)
                    {
                        return UnpackCStructure(md);
                    }                        
                }               
            }
        }

        public virtual T[] UnpackCStructureArray(MessageElementCStructureArray s2)
        {
            if (s2.Type != TypeName) throw new DataTypeException("cstructure type mismatch");
            int count = 0;
            using (vectorptr_messageelement cdataElements = s2.Elements)
            {
                T[] o = new T[cdataElements.Count];
                foreach (MessageElement e in cdataElements)
                {
                    using (e)
                    {
                        if (count != MessageElementUtil.GetMessageElementNumber(e)) throw new DataTypeException("Error in list format");
                        using (MessageElementCStructure md = (MessageElementCStructure)e.Data)
                        {
                            o[count] = UnpackCStructure(md);                            
                        }                        
                        count++;
                    }
                }
                return o;
            }
        }

        public virtual CStructureMultiDimArray UnpackCStructureMultiDimArray(MessageElementCStructureMultiDimArray s3)
        {
            if (s3.Type != TypeName) throw new DataTypeException("cstructure type mismatch");
            var o = new CStructureMultiDimArray();
            using (vectorptr_messageelement marrayElements = s3.Elements)
            {
                o.Dims = (MessageElementUtil.FindElementAndCast<int[]>(marrayElements, "dims"));
                using (var s2 = (MessageElementUtil.FindElementAndCast<MessageElementCStructureArray>(marrayElements, "array")))
                {
                    o.cstruct_array = UnpackCStructureArray(s2);
                }
            }
            return o;
        }

        public virtual MessageElementData PackCStructure(object s)
        {
            if (s is T)
            {
                T s2 = (T)s;
                return PackCStructureToArray(ref s2);
            }

            var s3 = s as T[];
            if (s3 != null)
            {
                return PackCStructureArray(s3);
            }

            var s4 = s as CStructureMultiDimArray;
            if (s4 != null)
            {
                return PackCStructureMultiDimArray(s4);
            }

            throw new DataTypeException("Unexpected message element type for PackCStructure");
        }
        public virtual object UnpackCStructure(MessageElementData m)
        {
            /*var m2 = m as MessageElementCStructure;
            if (m2 != null)
            {
                
                return UnpackCStructure(m2);           
            }*/

            var m3 = m as MessageElementCStructureArray;
            if (m3 != null)
            {
                return UnpackCStructureArray(m3);
            }

            var m4 = m as MessageElementCStructureMultiDimArray;
            if (m4 != null)
            {
                return UnpackCStructureMultiDimArray(m4);
            }

            throw new DataTypeException("Unexpected message element type for UnpackCStructure");
        }

        public abstract string TypeName { get; }
    }
            
    public class AStructureElementTypeAndCount : System.Attribute
    {
        public AStructureElementTypeAndCount(Type element_type, int element_array_count)
        {
            ElementArrayType = element_type;
            ElementArrayCount = element_array_count;
        }

        public Type ElementArrayType { get; }
        public int ElementArrayCount { get; }
        
    }

    public interface IAStructureStub
    {
        MessageElementData PackAStructure(object s);

        object UnpackAStructure(MessageElementData m);
    }

    public interface IAStructureStub<T> : IAStructureStub
    {
        MessageElementAStructureArray PackAStructureToArray(ref T s2);

        MessageElementAStructureArray PackAStructureArray(T[] s2);

        MessageElementAStructureMultiDimArray PackAStructureMultiDimArray(AStructureMultiDimArray s3);

        T UnpackAStructureFromArray(MessageElementAStructureArray s2);

        T[] UnpackAStructureArray(MessageElementAStructureArray s2);

        AStructureMultiDimArray UnpackAStructureMultiDimArray(MessageElementAStructureMultiDimArray s3);
    }

    public abstract class AStructureStub<T,U> : IAStructureStub<T> where T : struct
    {
        public abstract U[] GetNumericArrayFromAStructure(ref T s);

        public abstract T GetAStructureFromNumericArray(U[] m);

        public abstract U[] GetNumericArrayFromAStructureArray(T[] s);

        public abstract T[] GetAStructureArrayFromNumericArray(U[] m);


        public virtual MessageElementAStructureArray PackAStructureToArray(ref T s2)
        {
            using (var mm = new vectorptr_messageelement())
            {
                MessageElementUtil.AddMessageElementDispose(mm,
                    MessageElementUtil.NewMessageElementDispose("array", GetNumericArrayFromAStructure(ref s2))
                    );

                return new MessageElementAStructureArray(TypeName, mm);
            }
        }

        public virtual MessageElementAStructureArray PackAStructureArray(T[] s2)
        {
            if (s2 == null) return null;

            using (var mm = new vectorptr_messageelement())
            {
                
                MessageElementUtil.AddMessageElementDispose(mm,
                    MessageElementUtil.NewMessageElementDispose("array", GetNumericArrayFromAStructureArray(s2))
                    );
                
                return new MessageElementAStructureArray(TypeName, mm);
            }
        }

        public virtual MessageElementAStructureMultiDimArray PackAStructureMultiDimArray(AStructureMultiDimArray s3)
        {
            if (s3 == null) return null;
            using (vectorptr_messageelement l = new vectorptr_messageelement())
            {
                MessageElementUtil.AddMessageElementDispose(l, "dims", s3.Dims);
                MessageElementUtil.AddMessageElementDispose(l, "array", PackAStructureArray((T[])s3.astruct_array));
                return new MessageElementAStructureMultiDimArray(TypeName, l);
                
            }
        }

        public virtual T UnpackAStructureFromArray(MessageElementAStructureArray s2)
        {
            if (s2.Type != TypeName) throw new DataTypeException("astructure type mismatch");
            using (vectorptr_messageelement cdataElements = s2.Elements)
            {
                if (cdataElements.Count != 1) throw new DataTypeException("cstructure type mismatch");

                var a = MessageElementUtil.FindElementAndCast<U[]>(cdataElements, "array");

                return GetAStructureFromNumericArray(a);                             
            }
        }

        public virtual T[] UnpackAStructureArray(MessageElementAStructureArray s2)
        {
            if (s2.Type != TypeName) throw new DataTypeException("astructure type mismatch");
            using (vectorptr_messageelement cdataElements = s2.Elements)
            {
                if (cdataElements.Count != 1) throw new DataTypeException("cstructure type mismatch");

                var a = MessageElementUtil.FindElementAndCast<U[]>(cdataElements, "array");

                return GetAStructureArrayFromNumericArray(a);
            }
        }

        public virtual AStructureMultiDimArray UnpackAStructureMultiDimArray(MessageElementAStructureMultiDimArray s3)
        {
            if (s3.Type != TypeName) throw new DataTypeException("cstructure type mismatch");
            var o = new AStructureMultiDimArray();
            using (vectorptr_messageelement marrayElements = s3.Elements)
            {
                o.Dims = (MessageElementUtil.FindElementAndCast<int[]>(marrayElements, "dims"));
                using (var s2 = (MessageElementUtil.FindElementAndCast<MessageElementAStructureArray>(marrayElements, "array")))
                {
                    o.astruct_array = UnpackAStructureArray(s2);
                }
            }
            return o;
        }

        public virtual MessageElementData PackAStructure(object s)
        {
            if (s is T)
            {
                T s2 = (T)s;
                return PackAStructureToArray(ref s2);
            }

            var s3 = s as T[];
            if (s3 != null)
            {
                return PackAStructureArray(s3);
            }

            var s4 = s as AStructureMultiDimArray;
            if (s4 != null)
            {
                return PackAStructureMultiDimArray(s4);
            }

            throw new DataTypeException("Unexpected message element type for PackAStructure");
        }
        public virtual object UnpackAStructure(MessageElementData m)
        {
            /*var m2 = m as MessageElementCStructure;
            if (m2 != null)
            {
                
                return UnpackCStructure(m2);           
            }*/

            var m3 = m as MessageElementAStructureArray;
            if (m3 != null)
            {
                return UnpackAStructureArray(m3);
            }

            var m4 = m as MessageElementAStructureMultiDimArray;
            if (m4 != null)
            {
                return UnpackAStructureMultiDimArray(m4);
            }

            throw new DataTypeException("Unexpected message element type for UnpackAStructure");
        }

        public abstract string TypeName { get; }
    }


    public abstract class ServiceStub
    {
       

        protected void rr_async_PropertyGet(string name, Action<MessageElement, Exception, object> handler, object param, int timeout)
        {
            AsyncRequestDirectorImpl d = new AsyncRequestDirectorImpl(handler, param);
            int id=RRObjectHeap.AddObject(d);

            rr_innerstub.async_PropertyGet(name, timeout, d,id);

        }

        protected void rr_async_PropertySet(string name, MessageElement value, Action<MessageElement, Exception, object> handler, object param, int timeout)
        {
            AsyncRequestDirectorImpl d = new AsyncRequestDirectorImpl(handler, param);
            int id = RRObjectHeap.AddObject(d);

            rr_innerstub.async_PropertySet(name, value, timeout, d, id);

        }

        protected void rr_async_FunctionCall(string name, vectorptr_messageelement p, Action<MessageElement, Exception, object> handler, object param, int timeout)
        {
            AsyncRequestDirectorImpl d = new AsyncRequestDirectorImpl(handler, param);
            int id = RRObjectHeap.AddObject(d);

            rr_innerstub.async_FunctionCall(name, p, timeout, d, id);
        }

        protected void rr_async_GeneratorFunctionCall(string name, vectorptr_messageelement p, Action<WrappedGeneratorClient, Exception, object> handler, object param, int timeout)
        {
            AsyncGeneratorClientReturnDirectorImpl d = new AsyncGeneratorClientReturnDirectorImpl(handler, param);
            int id = RRObjectHeap.AddObject(d);

            rr_innerstub.async_GeneratorFunctionCall(name, p, timeout, d, id);
        }

        public WrappedServiceStub rr_innerstub;
        private WrappedServiceStubDirectorNET director;

        public ServiceStub(WrappedServiceStub innerstub)
        {
            director = new WrappedServiceStubDirectorNET(this);
            
            rr_innerstub = innerstub;
            int id = RRObjectHeap.AddObject(this);
            innerstub.SetRRDirector(director,id);

            
            
        }

        ~ServiceStub()
        {
            //rr_innerstub.RRClose();
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
            WrappedServiceStub s = rr_innerstub.FindObjRefTyped(n,objecttype);
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

        public void AsyncFindObjRef(string n, Action<object, Exception> handler, int timeout=RobotRaconteurNode.RR_TIMEOUT_INFINITE)
        {
            AsyncStubReturnDirectorImpl<object> d = new AsyncStubReturnDirectorImpl<object>(delegate(object o1, Exception e1, object o2) { handler(o1, e1); }, null);
            int id=RRObjectHeap.AddObject(d);
            rr_innerstub.async_FindObjRef(n, timeout, d, id);
        }

        public void AsyncFindObjRef(string n, string i, Action<object, Exception> handler, int timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE)
        {
            AsyncStubReturnDirectorImpl<object> d = new AsyncStubReturnDirectorImpl<object>(delegate(object o1, Exception e1, object o2) { handler(o1, e1); }, null);
            int id = RRObjectHeap.AddObject(d);
            rr_innerstub.async_FindObjRef(n, i, timeout, d, id);
        }

        public void AsyncFindObjRefTyped<T>(string n, string type, Action<T, Exception> handler, int timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE)
        {
            AsyncStubReturnDirectorImpl<T> d = new AsyncStubReturnDirectorImpl<T>(delegate(T o1, Exception e1, object o2) { handler(o1, e1); }, null);
            int id = RRObjectHeap.AddObject(d);
            rr_innerstub.async_FindObjRefTyped(n, type, timeout, d, id);
        }

        public void AsyncFindObjRefTyped<T>(string n, string i, string type, Action<T, Exception> handler, int timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE)
        {
            AsyncStubReturnDirectorImpl<T> d = new AsyncStubReturnDirectorImpl<T>(delegate(T o1, Exception e1, object o2) { handler(o1, e1); }, null);
            int id = RRObjectHeap.AddObject(d);
            rr_innerstub.async_FindObjRefTyped(n, i, type, timeout, d, id);
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
                    RRDirectorExceptionHelper.SetError(merr);
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
            catch { }                     
        }        
    }


    public abstract class ServiceSkel : WrappedServiceSkelDirector
    {

        protected WrappedServiceSkel innerskel;

        public ServiceSkel(object o)
        {
            uncastobj = o;
            if (o == null) throw new NullReferenceException();




        }

        public virtual void InitCallbackServers(object o)
        { }


        public string ServicePath { get { return ""; } }

        protected internal object uncastobj;

        public object UncastObject { get { return uncastobj; } }

        public abstract MessageElement CallGetProperty(string name);

        public abstract void CallSetProperty(string name, MessageElement m);

        public abstract MessageElement CallFunction(string name, vectorptr_messageelement m);

        public abstract object GetSubObj(string name, string ind);


        public virtual void RegisterEvents(object obj1)
        {

        }

        public virtual void UnregisterEvents(object obj1)
        {

        }

        public virtual void InitPipeServers(object obj1) { }
        public virtual void InitWireServers(object obj1) { }


        public override void ReleaseCastObject() {
            //RRObjectHeap.DeleteObject(innerskelid);
            innerskel = null;

        }

        public virtual object GetCallbackFunction(uint endpoint, string membername) { throw new MemberNotFoundException("Callback " + membername + " not found"); }

        //public virtual MessageEntry CallMemoryFunction(MessageEntry m, Endpoint e) { throw new MemberNotFoundException("Memory " + m.MemberName + " not found"); }

        internal int innerskelid;

        public override void Init(WrappedServiceSkel skel)
        {
            innerskel = skel;

            RegisterEvents(uncastobj);
            InitPipeServers(uncastobj);
            InitCallbackServers(uncastobj);
            InitWireServers(uncastobj);
        }
        public override MessageElement _CallFunction(string name, vectorptr_messageelement args)
        {
            try
            {
                using (args)
                {
                    return CallFunction(name, args);                    
                }
            }
            catch (Exception e)
            {
                using (MessageEntry merr = new MessageEntry())
                {
                    RobotRaconteurExceptionUtil.ExceptionToMessageEntry(e, merr);
                    RRDirectorExceptionHelper.SetError(merr);
                    return null;
                }
            }
        }

        public override MessageElement _CallGetProperty(string name)
        {             
            try
            {
                return CallGetProperty(name);                
            }
            catch (Exception e)
            {
                using (MessageEntry merr = new MessageEntry())
                {
                    RobotRaconteurExceptionUtil.ExceptionToMessageEntry(e, merr);
                    RRDirectorExceptionHelper.SetError(merr);
                    return null;
                }
            }           
            
        }

        public override void _CallSetProperty(string name, MessageElement m)
        {
            try
            {
                using (m)
                {
                    CallSetProperty(name, m);
                }
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

        public abstract string RRType { get; }

        public override WrappedRRObject _GetSubObj(string name, string index)
        {
            try
            {
                object newobj = GetSubObj(name, index);
                if (newobj == null) return null;
                ServiceSkel skel = RobotRaconteurNode.s.GetServiceType(RobotRaconteurNode.SplitQualifiedName(RRType).Item1).CreateSkel(newobj);

                int id = RRObjectHeap.AddObject(skel);
                skel.innerskelid = id;
                return new WrappedRRObject(skel.RRType, skel, id);                
            }
            catch (Exception e)
            {
                using (MessageEntry merr = new MessageEntry())
                {
                    RobotRaconteurExceptionUtil.ExceptionToMessageEntry(e, merr);
                    RRDirectorExceptionHelper.SetError(merr);
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
                    RRDirectorExceptionHelper.SetError(merr);
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
                    RRDirectorExceptionHelper.SetError(merr);
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
                    RRDirectorExceptionHelper.SetError(merr);
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
                    RRDirectorExceptionHelper.SetError(merr);
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

        public override WrappedCStructureArrayMemoryDirector _GetCStructureArrayMemory(string name)
        {
            try
            {
                return GetCStructureArrayMemory(name);
            }
            catch (Exception e)
            {
                using (MessageEntry merr = new MessageEntry())
                {
                    RobotRaconteurExceptionUtil.ExceptionToMessageEntry(e, merr);
                    RRDirectorExceptionHelper.SetError(merr);
                    return null;
                }
            }
        }

        public override WrappedCStructureMultiDimArrayMemoryDirector _GetCStructureMultiDimArrayMemory(string name)
        {
            try
            {
                return GetCStructureMultiDimArrayMemory(name);
            }
            catch (Exception e)
            {
                using (MessageEntry merr = new MessageEntry())
                {
                    RobotRaconteurExceptionUtil.ExceptionToMessageEntry(e, merr);
                    RRDirectorExceptionHelper.SetError(merr);
                    return null;
                }
            }
        }

        public virtual WrappedCStructureArrayMemoryDirector GetCStructureArrayMemory(string name)
        {
            throw new MemberNotFoundException("Member not found");
        }

        public virtual WrappedCStructureMultiDimArrayMemoryDirector GetCStructureMultiDimArrayMemory(string name)
        {
            throw new MemberNotFoundException("Member not found");
        }

        public override WrappedAStructureArrayMemoryDirector _GetAStructureArrayMemory(string name)
        {
            try
            {
                return GetAStructureArrayMemory(name);
            }
            catch (Exception e)
            {
                using (MessageEntry merr = new MessageEntry())
                {
                    RobotRaconteurExceptionUtil.ExceptionToMessageEntry(e, merr);
                    RRDirectorExceptionHelper.SetError(merr);
                    return null;
                }
            }
        }

        public override WrappedAStructureMultiDimArrayMemoryDirector _GetAStructureMultiDimArrayMemory(string name)
        {
            try
            {
                return GetAStructureMultiDimArrayMemory(name);
            }
            catch (Exception e)
            {
                using (MessageEntry merr = new MessageEntry())
                {
                    RobotRaconteurExceptionUtil.ExceptionToMessageEntry(e, merr);
                    RRDirectorExceptionHelper.SetError(merr);
                    return null;
                }
            }
        }

        public virtual WrappedAStructureArrayMemoryDirector GetAStructureArrayMemory(string name)
        {
            throw new MemberNotFoundException("Member not found");
        }

        public virtual WrappedAStructureMultiDimArrayMemoryDirector GetAStructureMultiDimArrayMemory(string name)
        {
            throw new MemberNotFoundException("Member not found");
        }
    }


    public class ServiceSkelUtil
    {
        public static Type FindParentInterface(Type objtype)
        {


            List<Type> interfaces = new List<Type>(objtype.GetInterfaces());
            interfaces.RemoveAll(x => (x.GetCustomAttributes(typeof(RobotRaconteurServiceObjectInterface), true).Length == 0));

            if (interfaces.Count == 0) throw new DataTypeException("Object not a Robot Raconteur type");
            if (interfaces.Count == 1) return interfaces[0];

            List<Type> parentinterfaces = new List<Type>();

            for (int i = 0; i < interfaces.Count; i++)
            {
                bool parent = true;
                for (int j = 0; j < interfaces.Count; j++)
                {
                    if (i != j)
                        if (interfaces[j].GetInterface(interfaces[i].ToString()) != null) parent = false;
                }

                if (parent)
                    parentinterfaces.Add(interfaces[i]);

            }

            if (parentinterfaces.Count != 1)
                throw new DataTypeException("Robot Raconteur types can only directly inheret one Robot Raconteur interface type");

           return parentinterfaces[0];

            


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
                if (objectcount > int.MaxValue - 10) objectcount = 0;
                objectcount++;
                while (objects.ContainsKey(objectcount)) objectcount++;

                int i=objectcount;
                objects.Add(i, o);
                //Console.WriteLine(objects.Count);
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
                catch {

                    //Console.WriteLine(e.ToString());
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
    {

    }


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
        public CallbackClient(string name)
            : base(name)
        {
        }

        T function = default(T);
        public override T Function
        {
            get
            {
                return function;
            }
            set
            {
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

        public CallbackServer(string name, ServiceSkel skel)
            : base(name)
        {
            this.skel = skel;
        }

        public override T Function
        {
            get
            {
                throw new InvalidOperationException("Invalid for server side of callback");
            }
            set
            {
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
            get
            {
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
                //RRObjectHeap.DeleteObject(director_id);
            }
        }
        
        public MemberDefinition_Direction Direction
        {
            get
            {
                return direction;
            }
        }

        public PipeEndpoint Connect(int index = -1)
        {

            if (ConnectFunction == null) throw new InvalidOperationException("Invalid for server");
            return new PipeEndpoint(ConnectFunction(index));
        }

        private class AsyncConnectDirector : AsyncPipeEndpointReturnDirector
        {
            Action<PipeEndpoint, Exception> handler_func;

            public AsyncConnectDirector(Action<PipeEndpoint, Exception> handler_func)
            {
                this.handler_func = handler_func;
            }

            public override void handler(WrappedPipeEndpoint m, uint error_code, string errorname, string errormessage)
            {
                if (error_code != 0)
                {
                    using (MessageEntry merr = new MessageEntry())
                    {
                        this.handler_func(null, RobotRaconteurExceptionUtil.ErrorCodeToException((RobotRaconteur.MessageErrorType)error_code,errorname,errormessage));
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
                    handler_func(null, e);
                    return;
                }


                handler_func(e1, null);
            }

        }

        public void AsyncConnect(Action<PipeEndpoint, Exception> handler, int timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE)
        {
            AsyncConnect(-1, handler);
        }

        public void AsyncConnect(int index, Action<PipeEndpoint, Exception> handler, int timeout=RobotRaconteurNode.RR_TIMEOUT_INFINITE )
        {

            if (AsyncConnectFunction == null) throw new InvalidOperationException("Invalid for server");
            AsyncConnectDirector h = new AsyncConnectDirector(handler);
            int id = RRObjectHeap.AddObject(h);
            AsyncConnectFunction(index,timeout,h,id);
        }

        //public delegate void PipeConnectCallbackFunction(PipeEndpoint newpipe);

        //public delegate void PipeDisconnectCallbackFunction(PipeEndpoint closedpipe);


        //public delegate void PipePacketReceivedCallbackFunction(PipeEndpoint e);


        //public delegate void PipePacketAckReceivedCallbackFunction(PipeEndpoint e, uint packetnum);


        public Action<PipeEndpoint> PipeConnectCallback
        {
            get { throw new InvalidOperationException("Read only property"); }
            set {
                connectdirectorclass c = new connectdirectorclass(this, value);
                int id = RRObjectHeap.AddObject(c);
                ((WrappedPipeServer)innerpipe).SetWrappedPipeConnectCallback(c,id);
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
                        RRDirectorExceptionHelper.SetError(merr);
                    }
                    
                }
            }
        }

        public class PipeEndpoint
        {
            WrappedPipeEndpoint innerpipe;

            public event Action<PipeEndpoint> PacketReceivedEvent;
            public event Action<PipeEndpoint,uint> PacketAckReceivedEvent;

            directorclass director;
            //int director_id;

            public PipeEndpoint(WrappedPipeEndpoint innerpipe)
            {
                this.innerpipe = innerpipe;
                director = new directorclass(this);
                
                innerpipe.SetRRDirector(director,RRObjectHeap.AddObject(this));
                //director_id = RRObjectHeap.AddObject(innerpipe);
                
            }

            ~PipeEndpoint()
            {
               // innerpipe.SetRRDirector(null);
                //RRObjectHeap.DeleteObject(director_id);
               
            }

            public int Index
            {
                get
                {
                    return innerpipe.GetIndex();
                }
            }

            public uint Endpoint
            {
                get
                {
                    return innerpipe.GetEndpoint();
                }
            }

            public bool RequestPacketAck
            {
                get
                {
                    return innerpipe.GetRequestPacketAck();
                }
                set
                {
                    innerpipe.SetRequestPacketAck(value);
                }

            }

            public bool IgnoreReceived
            {
                get
                {
                    return innerpipe.GetIgnoreReceived();
                }
                set
                {
                    innerpipe.SetIgnoreReceived(value);
                }

            }

            public MemberDefinition_Direction Direction
            {
                get
                {
                    return innerpipe.Direction();
                }
            }

            private Action<PipeEndpoint> close_callback;

            public Action<PipeEndpoint> PipeCloseCallback
            {
                get { return close_callback; }
                set { close_callback = value; }
            }

            public int Available
            {
                get
                {
                    return (int)innerpipe.Available();
                }
            }

            public bool IsUnreliable
            {
                get
                {
                    return innerpipe.IsUnreliable();
                }
            }

            public T PeekNextPacket()
            {
                using (MessageElement m = innerpipe.PeekNextPacket())
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
            }

            public T ReceivePacket()
            {
                using (MessageElement m = innerpipe.ReceivePacket())
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
            }

            public T PeekNextPacketWait(int timeout= RobotRaconteurNode.RR_TIMEOUT_INFINITE)
            {
                using (MessageElement m = innerpipe.PeekNextPacketWait(timeout))
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
            }

            public T ReceivePacketWait(int timeout=RobotRaconteurNode.RR_TIMEOUT_INFINITE)
            {
                using (MessageElement m = innerpipe.ReceivePacketWait(timeout))
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
            }

            public bool TryReceivePacketWait(out T packet, int timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE, bool peek = false)
            {
                packet = default(T);
                using (MessageElement m = new MessageElement())
                {
                    bool ret = innerpipe.TryReceivePacketWait(m, timeout, peek);
                    if (ret == false) return false;
                    object data = RobotRaconteurNode.s.UnpackVarType(m);
                    if (data is Array)
                    {
                        if (typeof(T).IsArray)
                            packet= (T)data;
                        else
                            packet= ((T[])data)[0];
                    }
                    else
                    {
                        packet= (T)data;
                    }

                    return true;
                }
            }


            public uint SendPacket(T data)
            {
                object dat = null;
                try
                {
                    dat = RobotRaconteurNode.s.PackVarType(data);
                    using (MessageElement m = new MessageElement("value", dat))
                    {
                        return innerpipe.SendPacket(m);
                    }
                }
                finally
                {
                    IDisposable d = dat as IDisposable;
                    if (d != null) d.Dispose();
                }
            }

            public void AsyncSendPacket(T data, Action<uint,Exception> handler)
            {
                object dat = null;

                AsyncUInt32ReturnDirectorImpl h = new AsyncUInt32ReturnDirectorImpl(delegate(uint i1, Exception e1, object o1) { handler(i1, e1); }, null);
                

                try
                {
                    dat = RobotRaconteurNode.s.PackVarType(data);
                    using (MessageElement m = new MessageElement("value", dat))
                    {
                        int id = RRObjectHeap.AddObject(h);
                        innerpipe.AsyncSendPacket(m,h,id);
                    }
                }
                finally
                {
                    IDisposable d = dat as IDisposable;
                    if (d != null) d.Dispose();
                }
            }


            public void Close()
            {
                innerpipe.Close();
            }

            public void AsyncClose(Action<Exception> handler, int timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE)
            {
                AsyncVoidReturnDirectorImpl h = new AsyncVoidReturnDirectorImpl(delegate(Exception e1, object o1) { handler(e1); }, null);
                int id = RRObjectHeap.AddObject(h);
                innerpipe.AsyncClose(timeout, h, id);
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

                        if (pipe.PacketAckReceivedEvent == null) return;
                        pipe.PacketAckReceivedEvent(pipe, packetnum);
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

                public override void PacketReceivedEvent()
                {
                    try
                    {
                        if (pipe.PacketReceivedEvent == null) return;
                        pipe.PacketReceivedEvent(pipe);
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

                public override void PipeEndpointClosedCallback()
                {
                    try
                    {
                        if (pipe.PipeCloseCallback == null) return;
                        pipe.PipeCloseCallback(pipe);
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

    }

    public class PipeBroadcaster<T>
    {
        protected Pipe<T> pipe;
        protected WrappedPipeBroadcaster innerpipe;
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


        public PipeBroadcaster(Pipe<T> pipe, int maximum_backlog=-1)
        {
            this.pipe = pipe;
            this.innerpipe = new WrappedPipeBroadcaster();
            this.innerpipe.Init((WrappedPipeServer)pipe.innerpipe, maximum_backlog);
            this.type = ((WrappedPipeServer)pipe.innerpipe).Type;
        }        

        public Pipe<T> Pipe
        {
            get { return pipe;  }
        }
        
        public void AsyncSendPacket(T packet, Action handler)
        {
            object dat = null;

            AsyncVoidNoErrReturnDirectorImpl h = new AsyncVoidNoErrReturnDirectorImpl(delegate(object o) { handler(); },null);
            
            try
            {
                dat = RobotRaconteurNode.s.PackVarType(packet);
                using (MessageElement m = new MessageElement("value", dat))
                {
                    int id = RRObjectHeap.AddObject(h);
                    innerpipe.AsyncSendPacket(m, h, id);
                }
            }
            finally
            {
                IDisposable d = dat as IDisposable;
                if (d != null) d.Dispose();
            }
        }

        public void SendPacket(T packet)
        {
            object dat = null;
            try
            {
                dat = RobotRaconteurNode.s.PackVarType(packet);
                using (MessageElement m = new MessageElement("value", dat))
                {
                    innerpipe.SendPacket(m);
                    return;
                }
            }
            finally
            {
                IDisposable d = dat as IDisposable;
                if (d != null) d.Dispose();
            }
        }

        public uint ActivePipeEndpointCount
        {
            get
            {                
                return innerpipe.GetActivePipeEndpointCount();                
            }
        }

        public Func<uint,int,bool> Predicate
        {                       
            set
            {
                var p = new WrappedPipeBroadcasterPredicateDirectorNET(value);
                int id = RRObjectHeap.AddObject(p);
                innerpipe.SetPredicateDirector(p, id);
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
            get
            {
                return this.m_Membername;
            }
        }

        public MemberDefinition_Direction Direction
        {
            get
            {
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
            //director_id = RRObjectHeap.AddObject(director);         
            direction = innerwire.Direction();
        }

        ~Wire()
        {
            if (innerwire is WrappedWireServer)
            {
               // ((WrappedWireServer)innerwire).SetRRDirector(null);
                //RRObjectHeap.DeleteObject(director_id);
            }
        }

        public WireConnection Connect()
        {
            WrappedWireClient c = innerwire as WrappedWireClient;
            if (c == null) throw new InvalidOperationException("Invalid for server");
            return new WireConnection(c.Connect());
        }

        private class AsyncConnectDirector : AsyncWireConnectionReturnDirector
        {
            Action<WireConnection, Exception> handler_func;

            public AsyncConnectDirector(Action<WireConnection, Exception> handler_func)
            {
                this.handler_func = handler_func;
            }

            public override void handler(WrappedWireConnection m, uint error_code, string errorname, string errormessage)
            {
                if (error_code != 0)
                {
                    using (MessageEntry merr = new MessageEntry())
                    {
                        this.handler_func(null, RobotRaconteurExceptionUtil.ErrorCodeToException((RobotRaconteur.MessageErrorType)error_code, errorname, errormessage));
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
                    handler_func(null, e);
                    return;
                }


                handler_func(e1, null);
            }

        }

        public void AsyncConnect(Action<WireConnection, Exception> handler, int timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE)
        {
            WrappedWireClient c = innerwire as WrappedWireClient;
            if (c == null) throw new InvalidOperationException("Invalid for server");
            AsyncConnectDirector h = new AsyncConnectDirector(handler);
            int id = RRObjectHeap.AddObject(h);
            c.AsyncConnect(timeout, h, id);
        }

        public Action<Wire<T>, WireConnection> WireConnectCallback
        {
            get { throw new InvalidOperationException("Read only property"); }
            set {
                connectdirectorclass c = new connectdirectorclass(this, value);
                int id=RRObjectHeap.AddObject(c);
                ((WrappedWireServer)innerwire).SetWrappedWireConnectCallback(c, id);
            }
        }

        //public delegate void WireConnectCallbackFunction(Wire<T> wire, WireConnection connection);

        //public delegate void WireDisconnectCallbackFunction(WireConnection wire);

        

        //public delegate void WireValueChangedFunction(WireConnection connection, T value, TimeSpec time);

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
                        RRDirectorExceptionHelper.SetError(merr);
                    }
                    
                }
            }           

        }

        class peekdirectorclass : AsyncWirePeekReturnDirector
        {
            Action<T, TimeSpec, Exception> handler_func;

            internal peekdirectorclass(Action<T, TimeSpec, Exception> handler)
            {
                this.handler_func = handler;
            }

            public override void handler(MessageElement m, TimeSpec ts, uint error_code, string errorname, string errormessage)
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

                                this.handler_func(default(T), null, RobotRaconteurExceptionUtil.ErrorCodeToException((RobotRaconteur.MessageErrorType)error_code, errorname, errormessage));
                                return;
                            }
                        }

                        handler_func(Wire<T>.UnpackData(m), ts, null);

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

        public T PeekInValue(out TimeSpec ts)
        {
            WrappedWireClient c = innerwire as WrappedWireClient;
            if (c == null) throw new InvalidOperationException("Invalid for server");
            ts = new TimeSpec();
            using (MessageElement m = c.PeekInValue(ts))
            {
                return UnpackData(m);
            }
        }

        public T PeekOutValue(out TimeSpec ts)
        {
            WrappedWireClient c = innerwire as WrappedWireClient;
            if (c == null) throw new InvalidOperationException("Invalid for server");
            ts = new TimeSpec();
            using (MessageElement m = c.PeekOutValue(ts))
            {
                return UnpackData(m);
            }
        }

        public void PokeOutValue(T value)
        {
            WrappedWireClient c = innerwire as WrappedWireClient;
            if (c == null) throw new InvalidOperationException("Invalid for server");
            object dat = null;
            try
            {
                dat = RobotRaconteurNode.s.PackVarType(value);
                using (MessageElement m = new MessageElement("value", dat))
                {
                    c.PokeOutValue(m);
                }
            }
            finally
            {
                IDisposable d = dat as IDisposable;
                if (d != null) d.Dispose();
            }
        }

        public void AsyncPeekInValue(Action<T,TimeSpec,Exception> handler, int timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE)
        {
            WrappedWireClient c = innerwire as WrappedWireClient;
            if (c == null) throw new InvalidOperationException("Invalid for server");

            peekdirectorclass h = new peekdirectorclass(handler);
            int id = RRObjectHeap.AddObject(h);
            c.AsyncPeekInValue(timeout, h, id);           
        }

        public void AsyncPeekOutValue(Action<T, TimeSpec, Exception> handler, int timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE)
        {
            WrappedWireClient c = innerwire as WrappedWireClient;
            if (c == null) throw new InvalidOperationException("Invalid for server");

            peekdirectorclass h = new peekdirectorclass(handler);
            int id = RRObjectHeap.AddObject(h);
            c.AsyncPeekOutValue(timeout, h, id);
        }

        public void AsyncPokeOutValue(T value, Action<Exception> handler, int timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE)
        {
            WrappedWireClient c = innerwire as WrappedWireClient;
            if (c == null) throw new InvalidOperationException("Invalid for server");
            object dat = null;
            try
            {
                dat = RobotRaconteurNode.s.PackVarType(value);
                using (MessageElement m = new MessageElement("value", dat))
                {
                    AsyncVoidReturnDirectorImpl h = new AsyncVoidReturnDirectorImpl(delegate (Exception e1, object o1) { handler(e1); }, null);
                    int id = RRObjectHeap.AddObject(h);
                    c.AsyncPokeOutValue(m, timeout, h, id);
                }
            }
            finally
            {
                IDisposable d = dat as IDisposable;
                if (d != null) d.Dispose();
            }
        }

        internal static T UnpackData(MessageElement m)
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

        class peekcallbackclass : WrappedWireServerPeekValueDirector
        {
            Func<uint,T> cb;

            public peekcallbackclass(Func<uint,T> cb)
            {
                this.cb = cb;
            }

            public override MessageElement PeekValue(uint ep)
            {
                var value = cb(ep);
               
                var dat = RobotRaconteurNode.s.PackVarType(value);
                MessageElement m = new MessageElement("value", dat);
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
                T value = Wire<T>.UnpackData(el);
                cb(value, ts, ep);               
            }
        }

        public Func<uint,T> PeekInValueCallback
        {
            get { throw new InvalidOperationException("Read only property"); }
            set
            {
                peekcallbackclass c = new peekcallbackclass(value);
                int id = RRObjectHeap.AddObject(c);
                ((WrappedWireServer)innerwire).SetPeekInValueCallback(c, id);
            }
        }

        public Func<uint,T> PeekOutValueCallback
        {
            get { throw new InvalidOperationException("Read only property"); }
            set
            {
                peekcallbackclass c = new peekcallbackclass(value);
                int id = RRObjectHeap.AddObject(c);
                ((WrappedWireServer)innerwire).SetPeekOutValueCallback(c, id);
            }
        }

        public Action<T, TimeSpec, uint> PokeOutValueCallback
        {
            get { throw new InvalidOperationException("Read only property"); }
            set
            {
                pokecallbackclass c = new pokecallbackclass(value);
                int id = RRObjectHeap.AddObject(c);
                ((WrappedWireServer)innerwire).SetPokeOutValueCallback(c, id);
            }
        }

        public class WireConnection
        {
            WrappedWireConnection innerwire;

            directorclass director;
            //int director_id = 0;

            public WireConnection(WrappedWireConnection innerwire)
            {
                this.innerwire = innerwire;
                director = new directorclass(this);
                //director_id = RRObjectHeap.AddObject(director);
                //RRObjectHeap.AddObject(innerwire);
                
                innerwire.SetRRDirector(director,RRObjectHeap.AddObject(this));
            }

            ~WireConnection()
            {
                ///innerwire.SetRRDirector(null);
                //RRObjectHeap.DeleteObject(director_id);
                try
                {

                   // innerwire.Close();
                }
                catch 
                {
                }
            }

            public MemberDefinition_Direction Direction
            {
                get
                {
                    return innerwire.Direction();
                }
            }

            public uint Endpoint
            {
                get
                {
                    return innerwire.GetEndpoint();
                }
            }

            public T InValue
            {
                get
                {
                    using (MessageElement m = innerwire.GetInValue())
                    {
                        return Wire<T>.UnpackData(m);
                    }
                }

            }

            public T OutValue
            {
                get
                {
                    using (MessageElement m = innerwire.GetOutValue())
                    {
                        return Wire<T>.UnpackData(m);
                    }
                }

                set
                {
                    object dat=null;
                    try
                    {
                        dat = RobotRaconteurNode.s.PackVarType(value);
                        using (MessageElement m = new MessageElement("value", dat))
                        {
                            innerwire.SetOutValue(m);
                        }
                    }
                    finally
                    {
                        IDisposable d = dat as IDisposable;
                        if (d != null) d.Dispose();
                    }

                }
                                
            }

            public TimeSpec LastValueReceivedTime
            {
                get
                {
                    return innerwire.GetLastValueReceivedTime();
                }

            }

            public TimeSpec LastValueSentTime
            {
                get
                {
                    return innerwire.GetLastValueSentTime();
                }

            }

            bool TryGetInValue(out T value, out TimeSpec ts)
            {
                value = default(T);
                ts = new TimeSpec();
                using (MessageElement m = new MessageElement())
                {
                    bool ret = innerwire.TryGetInValue(m, ts);
                    if (ret == false) return false;
                    value = Wire<T>.UnpackData(m);
                    return true;
                }
            }

            bool TryGetOutValue(out T value, out TimeSpec ts)
            {
                value = default(T);
                ts = new TimeSpec();
                using (MessageElement m = new MessageElement())
                {
                    bool ret = innerwire.TryGetOutValue(m, ts);
                    if (ret == false) return false;
                    value = Wire<T>.UnpackData(m);
                    return true;
                }
            }

            public void Close()
            {
                innerwire.Close();
            }

            public void AsyncClose(Action<Exception> handler, int timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE)
            {
                AsyncVoidReturnDirectorImpl h = new AsyncVoidReturnDirectorImpl(delegate(Exception e1, object o1) { handler(e1); }, null);
                int id = RRObjectHeap.AddObject(h);
                innerwire.AsyncClose(timeout, h, id);
            }

            public event Action<Wire<T>.WireConnection,T,TimeSpec> WireValueChanged;

            private Action<WireConnection> close_callback;

            public Action<WireConnection> WireCloseCallback
            {
                get { return close_callback; }
                set { close_callback = value; }
            }

            public bool InValueValid
            {
                get
                {
                    return innerwire.GetInValueValid();
                }
            }

            public bool OutValueValid
            {
                get
                {
                    return innerwire.GetOutValueValid();
                }
            }

            public bool IgnoreInValue
            {
                get
                {
                    return innerwire.GetIgnoreInValue();
                }
                set
                {
                    innerwire.SetIgnoreInValue(value);
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
                        if (wire.WireCloseCallback == null) return;
                        wire.WireCloseCallback(wire);
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

                public override void WireValueChanged(MessageElement value, TimeSpec time)
                {
                    try
                    {
                        using (value)
                        { 
                            if (wire.WireValueChanged == null) return;
                                                
                            object data = RobotRaconteurNode.s.UnpackVarType(value);
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
                            RRDirectorExceptionHelper.SetError(merr);
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

        protected Wire<T> wire;

        protected WrappedWireBroadcaster innerwire;
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
            get { return wire; }
        }

        public T OutValue
        {
            set
            {
                object dat = null;
                try
                {
                    dat = RobotRaconteurNode.s.PackVarType(value);
                    using (MessageElement m = new MessageElement("value", dat))
                    {
                        innerwire.SetOutValue(m);
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
                return innerwire.GetActiveWireConnectionCount();
            }
        }

        public Func<uint, bool> Predicate
        {
            set
            {
                var p = new WrappedWireBroadcasterPredicateDirectorNET(value);
                int id = RRObjectHeap.AddObject(p);
                innerwire.SetPredicateDirector(p, id);
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
            var id=RRObjectHeap.AddObject(cb);
            this.innerwire.AddInValueChangedListener(cb, id);
        }

        public Wire<T> Wire
        {
            get { return wire; }
        }

        public T GetInValue(out TimeSpec ts, out uint ep)
        {
            ts = new TimeSpec();
            var m=innerwire.GetInValue(ts, out ep);
            return Wire<T>.UnpackData(m);
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
                if (cb1 == null) return;

                T value = Wire<T>.UnpackData(el);
                cb1.ValueChanged(value, ts, ep);
            }
        }

        internal void ValueChanged(T value, TimeSpec ts, uint ep)
        {
            if(InValueChanged!=null)
            {
                InValueChanged(value, ts, ep);
            }
        }

        public event Action<T, TimeSpec, uint> InValueChanged;

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
            this.listener=listener;
            

        }

        public override void Callback(int code)
        {
            try
            {
                listener(stub, (ClientServiceListenerEventType)code, null);
            }
            catch
            {

            }
        }

        

        

    }


    public class ServerServiceListenerDirectorNET : ServerServiceListenerDirector
    {

        public int listenerid;

        ServerContext.ServerServiceListenerDelegate listener;
        WeakReference context=new WeakReference(null);

        public ServerServiceListenerDirectorNET(ServerContext context, ServerContext.ServerServiceListenerDelegate listener)
        {
            this.listener=listener;
            this.context.Target=context;

        }

        public override void  Callback(int code, uint endpoint)
        {
            try
            {
                listener((ServerContext)context.Target, (ServerServiceListenerEventType)code, endpoint);
            }
            catch
            {

            }
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


            protected override AuthenticatedUser AuthenticateUser(string username, MessageElement credentials, ServerContext context)
            {
                using (credentials)
                {
                    IUserAuthenticator t = target;
                    if (t == null) throw new AuthenticationException("Authenticator internal error");


                    Dictionary<string, object> c2 = (Dictionary<string, object>)RobotRaconteurNode.s.UnpackMapType<string, object>(credentials);

                    AuthenticatedUser ret = t.AuthenticateUser(username, c2, context);
                    return ret;
                }

            }

        }

        public ServiceSecurityPolicy(IUserAuthenticator authenticator, Dictionary<string, string> policies) : this(init_authenticator(authenticator),init_policies(policies))
        {

        }

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

        public virtual AuthenticatedUser AuthenticateUser(string username, Dictionary<string, object> credentials, ServerContext context)
        {
            MessageElement c =(MessageElement) RobotRaconteurNode.s.PackMapType<string,object>(credentials);
            return _AuthenticateUser(username, c, context);
        }


    }
    

}