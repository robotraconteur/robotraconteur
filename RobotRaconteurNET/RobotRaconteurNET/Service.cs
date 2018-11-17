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

        public abstract IStructureStub FindStructureStub(string objecttype);

        public abstract ICStructureStub FindCStructureStub(string objecttype);

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
        //protected Pipe<T> pipe;
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
            //this.pipe = pipe;
            this.innerpipe = new WrappedPipeBroadcaster();
            this.innerpipe.Init((WrappedPipeServer)pipe.innerpipe, maximum_backlog);
            this.type = ((WrappedPipeServer)pipe.innerpipe).Type;
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

        //protected WeakReference wire;

        protected WrappedWireBroadcaster innerwire;
        protected TypeDefinition type;
        
        public WireBroadcaster(Wire<T> wire)
        {
            this.innerwire = new WrappedWireBroadcaster();
            this.innerwire.Init((WrappedWireServer)wire.innerwire);
            this.type = ((WrappedWireServer)wire.innerwire).Type;
            
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
        protected WrappedWireUnicastReceiver innerwire;
        protected TypeDefinition type;

        public WireUnicastReceiver(Wire<T> wire)
        {
            this.innerwire = new WrappedWireUnicastReceiver();
            this.innerwire.Init((WrappedWireServer)wire.innerwire);
            this.type = ((WrappedWireServer)wire.innerwire).Type;

            var cb = new ValueChangedDirector(this);
            var id=RRObjectHeap.AddObject(cb);
            this.innerwire.AddInValueChangedListener(cb, id);
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

    public class ArrayMemory<T>
    {
        private T[] memory;

        public ArrayMemory()
        {

        }

        public ArrayMemory(T[] memory)
        {
            this.memory = memory;

        }

        public virtual void Attach(T[] memory)
        {
            this.memory = memory;

        }

        public virtual ulong Length
        {
            get
            {
                return (ulong)memory.LongLength;
            }
        }

        public virtual void Read(ulong memorypos, T[] buffer, ulong bufferpos, ulong count)
        {

            Array.Copy(memory, (long)memorypos, buffer, (long)bufferpos, (long)count);
        }
        
        public virtual void Write(ulong memorypos, T[] buffer, ulong bufferpos, ulong count)
        {

            Array.Copy(buffer, (long)bufferpos, memory, (long)memorypos, (long)count);
        }
    }

    public class CStructureArrayMemory<T> : ArrayMemory<T> where T : struct
    {
        public CStructureArrayMemory() : base()
        {
        }

        public CStructureArrayMemory(T[] memory) : base(memory)
        {            
        }
    }

    public class MultiDimArrayMemory<T>
    {

        private MultiDimArray multimemory;
        
        public MultiDimArrayMemory()
        {


        }
        
        public MultiDimArrayMemory(MultiDimArray memory)
        {

            multimemory = memory;
        }
                
        public virtual void Attach(MultiDimArray memory)
        {

            this.multimemory = memory;

        }
        
        public virtual ulong[] Dimensions
        {
            get
            {
                return multimemory.Dims.Select(x => (ulong)x).ToArray();
            }
        }

        public virtual ulong DimCount
        {
            get
            {
                return (ulong)multimemory.DimCount;
            }
        }

        public virtual bool Complex
        {
            get
            {
                return multimemory.Complex;
            }
        }
        
        public virtual void Read(ulong[] memorypos, MultiDimArray buffer, ulong[] bufferpos, ulong[] count)
        {

            multimemory.RetrieveSubArray(memorypos.Select(x => (int)x).ToArray(), buffer, bufferpos.Select(x => (int)x).ToArray(), count.Select(x => (int)x).ToArray());

        }

        
        public virtual void Write(ulong[] memorypos, MultiDimArray buffer, ulong[] bufferpos, ulong[] count)
        {

            multimemory.AssignSubArray(memorypos.Select(x => (int)x).ToArray(), buffer, bufferpos.Select(x => (int)x).ToArray(), count.Select(x => (int)x).ToArray());
        }
    }

    public class CStructureMultiDimArrayMemory<T> where T : struct
    {

        private CStructureMultiDimArray multimemory;

        public CStructureMultiDimArrayMemory()
        {


        }

        public CStructureMultiDimArrayMemory(CStructureMultiDimArray memory)
        {

            multimemory = memory;
        }

        public virtual void Attach(CStructureMultiDimArray memory)
        {

            this.multimemory = memory;

        }

        public virtual ulong[] Dimensions
        {
            get
            {
                return multimemory.Dims.Select(x => (ulong)x).ToArray();
            }
        }

        public virtual ulong DimCount
        {
            get
            {
                return (ulong)multimemory.Dims.Length;
            }
        }
                
        public virtual void Read(ulong[] memorypos, CStructureMultiDimArray buffer, ulong[] bufferpos, ulong[] count)
        {

            multimemory.RetrieveSubArray(memorypos.Select(x => (int)x).ToArray(), buffer, bufferpos.Select(x => (int)x).ToArray(), count.Select(x => (int)x).ToArray());

        }


        public virtual void Write(ulong[] memorypos, CStructureMultiDimArray buffer, ulong[] bufferpos, ulong[] count)
        {

            multimemory.AssignSubArray(memorypos.Select(x => (int)x).ToArray(), buffer, bufferpos.Select(x => (int)x).ToArray(), count.Select(x => (int)x).ToArray());
        }
    }

    public class ArrayMemoryClient<T> : ArrayMemory<T>
    {
        ArrayMemoryBase innerarray;

        public ArrayMemoryClient(ArrayMemoryBase innerarray)
        {
            this.innerarray = innerarray;
        }

        MemberDefinition_Direction Direction
        {
            get
            {
                return WrappedArrayMemoryClientUtil.Direction(innerarray);
            }
        }

        public override ulong Length
        {
            get
            {
                return innerarray.Length();
            }
         }

        public override void Read(ulong memorypos, T[] buffer, ulong bufferpos, ulong count)
        {
            RRBaseArray dat=null;
            try
            {
                dat = WrappedArrayMemoryClientUtil.Read(innerarray, memorypos, count);
                T[] dat2 = (T[])MessageElementDataUtil.RRBaseArrayToArray(dat);
                Array.Copy(dat2, 0, buffer, (long)bufferpos, (long)count);
            }
            finally
            {
                if (dat != null) dat.Dispose();
            }
        }

        public override void Write(ulong memorypos, T[] buffer, ulong bufferpos, ulong count)
        {
            T[] buffer3;
            if ((ulong)buffer.Length == count)
            {
                buffer3 = buffer;
            }
            else
            {
                buffer3 = new T[count];
                Array.Copy(buffer, (long)bufferpos, buffer3, 0, (long)count);
            }
                        
            RRBaseArray buffer2=null;
            try
            {
                buffer2 = MessageElementDataUtil.ArrayToRRBaseArray(buffer3);
                WrappedArrayMemoryClientUtil.Write(innerarray, memorypos, buffer2, 0, count);
            }
            finally
            {
                if (buffer2!=null)
                    buffer2.Dispose();
            }
        }

    }


    public class MultiDimArrayMemoryClient<T> : MultiDimArrayMemory<T>
    {
        MultiDimArrayMemoryBase innermem;
        public MultiDimArrayMemoryClient(MultiDimArrayMemoryBase innermem)
        {
            this.innermem = innermem;
        }

        MemberDefinition_Direction Direction
        {
            get
            {
                return WrappedMultiDimArrayMemoryClientUtil.Direction(innermem);
            }
        }

        public override ulong DimCount
        {
            get
            {
                return innermem.DimCount();
            }
        }

        public override ulong[] Dimensions
        {
            get
            {
                return innermem.Dimensions().Select(x=>(ulong)x).ToArray();
            }
        }

        public override bool Complex
        {
            get
            {
                return innermem.Complex();
            }
        }

        public override void Read(ulong[] memorypos, MultiDimArray buffer, ulong[] bufferpos, ulong[] count)
        {
            RRMultiDimArrayUntyped dat = null;
            RRBaseArray datDims=null;
            RRBaseArray datReal=null;
            RRBaseArray datImag=null;
            try
            {
                vector_uint64_t memorypos2 = new vector_uint64_t();
                foreach (ulong val in memorypos) memorypos2.Add(val);
                vector_uint64_t count2 = new vector_uint64_t();
                foreach (ulong val in count) count2.Add(val);

                dat = WrappedMultiDimArrayMemoryClientUtil.Read(innermem, memorypos2, count2);

                datDims = dat.Dims;
                datReal = dat.Real;
                datImag = dat.Imag;
                int[] dims = (int[])MessageElementDataUtil.RRBaseArrayToArray(datDims);
                T[] real = (T[])MessageElementDataUtil.RRBaseArrayToArray(datReal);
                T[] imag = (T[])MessageElementDataUtil.RRBaseArrayToArray(datImag);

                MultiDimArray dat2 = new MultiDimArray(dims, real, imag);
                buffer.AssignSubArray(bufferpos.Select(x => (int)x).ToArray(), dat2, new int[count.Length], count.Select(x => (int)x).ToArray());
            }
            finally
            {
                if (dat != null)
                {
                    if (datDims != null) datDims.Dispose();
                    if (datReal != null) datReal.Dispose();
                    if (datImag != null) datImag.Dispose();
                    dat.Dispose();
                }
            }
            
            

            
        }

        public override void Write(ulong[] memorypos, MultiDimArray buffer, ulong[] bufferpos, ulong[] count)
        {
            RRMultiDimArrayUntyped dat2 = null;
            RRBaseArray dat2Dims = null;
            RRBaseArray dat2Real = null;
            RRBaseArray dat2Imag = null;
            try
            {
                int elemcount = 1;
                foreach (ulong v in count) elemcount *= (int)v;
                int[] count2 = count.Select(x => (int)x).ToArray();
                T[] real = new T[elemcount];
                T[] imag = null;
                if (Complex) imag = new T[elemcount];

                MultiDimArray writedat1 = new MultiDimArray(count2, real, imag);
                writedat1.AssignSubArray(new int[count.Length], buffer, bufferpos.Select(x => (int)x).ToArray(), count.Select(x => (int)x).ToArray());

                dat2 = new RRMultiDimArrayUntyped();
                dat2.DimCount = count2.Length;
                dat2Dims=MessageElementDataUtil.ArrayToRRBaseArray(count2);
                dat2.Dims = dat2Dims;
                dat2Real = MessageElementDataUtil.ArrayToRRBaseArray(real);
                dat2.Real = dat2Real;
                dat2.Complex = false;
                if (imag != null)
                {
                    dat2.Complex = true;
                    dat2Imag=MessageElementDataUtil.ArrayToRRBaseArray(imag);
                    dat2.Imag = dat2Imag;
                }

                vector_uint64_t memorypos3 = new vector_uint64_t();
                foreach (ulong val in memorypos) memorypos3.Add(val);
                vector_uint64_t count3 = new vector_uint64_t();
                foreach (ulong val in count) count3.Add(val);
                vector_uint64_t bufferpos3 = new vector_uint64_t();
                for (int i = 0; i < count.Length; i++) bufferpos3.Add(0);
                WrappedMultiDimArrayMemoryClientUtil.Write(innermem, memorypos3, dat2, bufferpos3, count3);
            }
            finally
            {
                if (dat2 != null)
                {
                    if (dat2Dims!=null) dat2Dims.Dispose();
                    if (dat2Real!=null) dat2Real.Dispose();
                    if (dat2Imag != null) dat2Imag.Dispose();
                    dat2.Dispose();
                }
            }
        }
    }

    public class CStructureArrayMemoryClient<T> : CStructureArrayMemory<T> where T: struct
    {
        WrappedCStructureArrayMemoryClient innerclient;

        class bufferdirector : WrappedCStructureArrayMemoryClientBuffer
        {
            T[] buffer;

            public bufferdirector(T[] buffer)
            {
                this.buffer = buffer;
            }

            public override void UnpackReadResult(MessageElementCStructureArray res, ulong bufferpos, ulong count)
            {
                T[] res1 = RobotRaconteurNode.s.UnpackCStructureArrayDispose<T>(res);
                Array.Copy(res1, 0, buffer, (long)bufferpos, (long)count);
            }

            public override MessageElementCStructureArray PackWriteRequest(ulong bufferpos, ulong count)
            {
                T[] buffer3;
                if ((ulong)buffer.Length == count)
                {
                    buffer3 = buffer;
                }
                else
                {
                    buffer3 = new T[count];
                    Array.Copy(buffer, (long)bufferpos, buffer3, 0, (long)count);
                }
                return RobotRaconteurNode.s.PackCStructureArray<T>(buffer3);
            }
        }

        public CStructureArrayMemoryClient(WrappedCStructureArrayMemoryClient innerclient)
        {
            this.innerclient = innerclient;
        }

        public override void Attach(T[] memory)
        {
            throw new InvalidOperationException("Invalid for memory client");
        }

        MemberDefinition_Direction Direction
        {
            get
            {
                return innerclient.Direction();
            }
        }

        public override ulong Length
        {
            get
            {
                return innerclient.Length();
            }
        }

        public override void Read(ulong memorypos, T[] buffer, ulong bufferpos, ulong count)
        {
            using (bufferdirector buffer1 = new bufferdirector(buffer))
            {
                innerclient.Read(memorypos, buffer1, bufferpos, count);
            }
        }

        public override void Write(ulong memorypos, T[] buffer, ulong bufferpos, ulong count)
        {
            using (bufferdirector buffer1 = new bufferdirector(buffer))
            {
                innerclient.Write(memorypos, buffer1, bufferpos, count);
            }
        }
    }

    public class CStructureMultiDimArrayMemoryClient<T> : CStructureMultiDimArrayMemory<T> where T : struct
    {
        WrappedCStructureMultiDimArrayMemoryClient innerclient;

        class bufferdirector : WrappedCStructureMultiDimArrayMemoryClientBuffer
        {
            CStructureMultiDimArray buffer;

            public bufferdirector(CStructureMultiDimArray buffer)
            {
                this.buffer = buffer;
            }

            public override void UnpackReadResult(MessageElementCStructureMultiDimArray res, vector_uint64_t bufferpos, vector_uint64_t count)
            {
                using (res)
                using (bufferpos)
                using (count)
                {
                    CStructureMultiDimArray res1 = RobotRaconteurNode.s.UnpackCStructureMultiDimArrayDispose<T>(res);
                    buffer.AssignSubArray(bufferpos.Select(x => (int)x).ToArray(), res1, new int[buffer.Dims.Length], count.Select(x=>(int)x).ToArray());                    
                }
            }

            public override MessageElementCStructureMultiDimArray PackWriteRequest(vector_uint64_t bufferpos, vector_uint64_t count)
            {
                using (bufferpos)
                using (count)
                {
                    CStructureMultiDimArray o = new CStructureMultiDimArray(count.Select(x => (int)x).ToArray(), new T[count.Aggregate(1, (x, y) => (int)x * (int)y)]);
                    buffer.RetrieveSubArray(bufferpos.Select(x => (int)x).ToArray(), o, new int[buffer.Dims.Length], count.Select(x => (int)x).ToArray());
                    return RobotRaconteurNode.s.PackCStructureMultiDimArray<T>(o);
                }
            }
        }

        public CStructureMultiDimArrayMemoryClient(WrappedCStructureMultiDimArrayMemoryClient innerclient)
        {
            this.innerclient = innerclient;
        }

        public virtual MemberDefinition_Direction Direction
        {
            get
            {
                return innerclient.Direction();
            }
        }

        public override void Attach(CStructureMultiDimArray memory)
        {
            throw new InvalidOperationException("Invalid for memory client");
        }

        public override ulong DimCount
        {
            get
            {
                return innerclient.DimCount();
            }
        }

        public override ulong[] Dimensions
        {
            get
            {
                return innerclient.Dimensions().ToArray();
            }
        }



        public override void Read(ulong[] memorypos, CStructureMultiDimArray buffer, ulong[] bufferpos, ulong[] count)
        {
            using (bufferdirector buffer1 = new bufferdirector(buffer))
            using (vector_uint64_t memorypos1 = new vector_uint64_t(memorypos))
            using (vector_uint64_t bufferpos1 = new vector_uint64_t(bufferpos))
            using (vector_uint64_t count1 = new vector_uint64_t(count))
            {
                innerclient.Read(memorypos1, buffer1, bufferpos1, count1);
            }
        }

        public override void Write(ulong[] memorypos, CStructureMultiDimArray buffer, ulong[] bufferpos, ulong[] count)
        {
            using (bufferdirector buffer1 = new bufferdirector(buffer))
            using (vector_uint64_t memorypos1 = new vector_uint64_t(memorypos))
            using (vector_uint64_t bufferpos1 = new vector_uint64_t(bufferpos))
            using (vector_uint64_t count1 = new vector_uint64_t(count))
            {
                innerclient.Write(memorypos1, buffer1, bufferpos1, count1);
            }
        }
    }

    public interface IRobotRaconteurMonitorObject
    {
        void RobotRaconteurMonitorEnter();

        void RobotRaconteurMonitorEnter(int timeout);

        void RobotRaconteurMonitorExit();


    }

    public class WrappedArrayMemoryDirectorNET<T> : WrappedArrayMemoryDirector
    {
        ArrayMemory<T> mem;

        //public int memoryid = 0;

        public WrappedArrayMemoryDirectorNET(ArrayMemory<T> mem)
        {
            this.mem = mem;

            this.objectheapid=RRObjectHeap.AddObject(this);
        }

        public override ulong Length()
        {
            try
            {
                return mem.Length;
            }
            catch (Exception e)
            {
                using (MessageEntry merr = new MessageEntry())
                {
                    RobotRaconteurExceptionUtil.ExceptionToMessageEntry(e, merr);
                    RRDirectorExceptionHelper.SetError(merr);
                    return 0;
                }
                
            }
        }

        public override void Read(ulong memorypos, RRBaseArray buffer, ulong bufferpos, ulong count)
        {
            try
            {
                using (buffer)
                {
                    T[] buffer3 = new T[count];
                    mem.Read(memorypos, buffer3, 0, count);

                    MessageElementDataUtil.ArrayToRRBaseArray(buffer3, buffer);
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

        public override void Write(ulong memorypos, RRBaseArray buffer, ulong bufferpos, ulong count)
        {
            try
            {
                using (buffer)
                {

                    T[] buffer2 = (T[])MessageElementDataUtil.RRBaseArrayToArray(buffer);
                    mem.Write(memorypos, buffer2, bufferpos, count);
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

    public class WrappedMultiDimArrayMemoryDirectorNET<T> : WrappedMultiDimArrayMemoryDirector
    {
        MultiDimArrayMemory<T> mem;

        //public int memoryid = 0;

        public WrappedMultiDimArrayMemoryDirectorNET(MultiDimArrayMemory<T> mem)
        {
            this.mem = mem;
            this.objectheapid = RRObjectHeap.AddObject(this);
        }

        

        public override ulong DimCount()
        {
            try
            {
                return mem.DimCount;
            }
            catch (Exception e)
            {
                using (MessageEntry merr = new MessageEntry())
                {
                    RobotRaconteurExceptionUtil.ExceptionToMessageEntry(e, merr);
                    RRDirectorExceptionHelper.SetError(merr);
                    return 0;
                }
            }
        }

        public override vector_uint64_t Dimensions()
        {
            try
            {
                vector_uint64_t o = new vector_uint64_t();
                foreach (ulong i in mem.Dimensions) o.Add(i);
                return o;
            }
            catch (Exception e)
            {
                using (MessageEntry merr = new MessageEntry())
                {
                    RobotRaconteurExceptionUtil.ExceptionToMessageEntry(e, merr);
                    RRDirectorExceptionHelper.SetError(merr);
                    return new vector_uint64_t();
                }
            }
        }

        public override bool Complex()
        {
            try
            {
                return mem.Complex;
            }
            catch (Exception e)
            {
                using (MessageEntry merr = new MessageEntry())
                {
                    RobotRaconteurExceptionUtil.ExceptionToMessageEntry(e, merr);
                    RRDirectorExceptionHelper.SetError(merr);
                    return false;
                }
            }
        }

        public override void Read(WrappedMultiDimArrayMemoryParams p)
        {
            RRMultiDimArrayUntyped pbuffer=null;
            RRBaseArray pbufferDims = null;
            RRBaseArray pbufferReal = null;
            RRBaseArray pbufferImag = null;

            try
            {
                ulong[] count = p.count.ToArray();
                ulong[] bufferpos = p.bufferpos.ToArray();
                ulong[] memorypos = p.memorypos.ToArray();

                ulong elemcount = 1;
                foreach (ulong e in count) elemcount *= e;

                pbuffer = p.buffer;
                pbufferDims = pbuffer.Dims;
                pbufferReal = pbuffer.Real;
                pbufferImag = pbuffer.Imag;

                if (mem.Complex != pbuffer.Complex) throw new ArgumentException("Complex mismatch");
                T[] real = new T[elemcount];
                T[] imag = null;
                if (mem.Complex) imag = new T[elemcount];

                MultiDimArray m = new MultiDimArray(count.Select(x => (int)x).ToArray(), real, imag);

                mem.Read(memorypos, m, bufferpos, count);
                MessageElementDataUtil.ArrayToRRBaseArray(real, pbufferReal);
                if (imag != null) MessageElementDataUtil.ArrayToRRBaseArray(imag, pbufferImag);
            }
            catch (Exception e)
            {
                using (MessageEntry merr = new MessageEntry())
                {
                    RobotRaconteurExceptionUtil.ExceptionToMessageEntry(e, merr);
                    RRDirectorExceptionHelper.SetError(merr);
                }

            }
            finally
            {
                if (p!=null && pbuffer!=null)
                {
                    if (pbufferDims != null) pbufferDims.Dispose();
                    if (pbufferReal != null) pbufferReal.Dispose();
                    if (pbufferImag != null) pbufferImag.Dispose();
                }
                if (p!=null)
                {
                    p.Dispose();
                }
                
       
            }
        }

        public override void Write(WrappedMultiDimArrayMemoryParams p)
        {
            RRMultiDimArrayUntyped pbuffer = null;
            RRBaseArray pbufferDims = null;
            RRBaseArray pbufferReal = null;
            RRBaseArray pbufferImag = null;
            try
            {
                ulong[] count = p.count.ToArray();
                ulong[] bufferpos = p.bufferpos.ToArray();
                ulong[] memorypos = p.memorypos.ToArray();

                ulong elemcount = 1;
                foreach (ulong e in count) elemcount *= e;

                pbuffer = p.buffer;
                pbufferDims = pbuffer.Dims;
                pbufferReal = pbuffer.Real;
                pbufferImag = pbuffer.Imag;

                if (mem.Complex != pbuffer.Complex) throw new ArgumentException("Complex mismatch");

                int[] dims = (int[])MessageElementDataUtil.RRBaseArrayToArray(pbufferDims);
                T[] real = (T[])MessageElementDataUtil.RRBaseArrayToArray(pbufferReal);
                T[] imag = null;
                if (pbuffer.Complex) imag=(T[])MessageElementDataUtil.RRBaseArrayToArray(pbufferImag);

                MultiDimArray m = new MultiDimArray(count.Select(x => (int)x).ToArray(), real, imag);

                mem.Write(memorypos, m, bufferpos, count);


            }
            catch (Exception e)
            {
                using (MessageEntry merr = new MessageEntry())
                {
                    RobotRaconteurExceptionUtil.ExceptionToMessageEntry(e, merr);
                    RRDirectorExceptionHelper.SetError(merr);
                }

            }
            finally
            {
                if (p != null && pbuffer != null)
                {
                    if (pbufferDims != null) pbufferDims.Dispose();
                    if (pbufferReal != null) pbufferReal.Dispose();
                    if (pbufferImag != null) pbufferImag.Dispose();
                }
                if (p != null)
                {
                    p.Dispose();
                }


            }
        }

    }

    public class WrappedCStructureArrayMemoryDirectorNET<T> : WrappedCStructureArrayMemoryDirector where T : struct
    {
        CStructureArrayMemory<T> mem;

        //public int memoryid = 0;

        public WrappedCStructureArrayMemoryDirectorNET(CStructureArrayMemory<T> mem)
        {
            this.mem = mem;

            this.objectheapid = RRObjectHeap.AddObject(this);
        }

        public override ulong Length()
        {
            try
            {
                return mem.Length;
            }
            catch (Exception e)
            {
                using (MessageEntry merr = new MessageEntry())
                {
                    RobotRaconteurExceptionUtil.ExceptionToMessageEntry(e, merr);
                    RRDirectorExceptionHelper.SetError(merr);
                    return 0;
                }

            }
        }

        public override MessageElementCStructureArray Read(ulong memorypos, ulong bufferpos, ulong count)
        {
            try
            {
                T[] buffer3 = new T[count];
                mem.Read(memorypos, buffer3, 0, count);

                return RobotRaconteurNode.s.PackCStructureArray<T>(buffer3);                
            }
            catch (Exception e)
            {
                using (MessageEntry merr = new MessageEntry())
                {
                    RobotRaconteurExceptionUtil.ExceptionToMessageEntry(e, merr);
                    RRDirectorExceptionHelper.SetError(merr);
                }
                return null;
            }
        }

        public override void Write(ulong memorypos, MessageElementCStructureArray buffer, ulong bufferpos, ulong count)
        {
            try
            {
                using (buffer)
                {

                    T[] buffer2 = (T[])RobotRaconteurNode.s.UnpackCStructureArray<T>(buffer);
                    mem.Write(memorypos, buffer2, bufferpos, count);
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

    public class WrappedCStructureMultiDimArrayMemoryDirectorNET<T> : WrappedCStructureMultiDimArrayMemoryDirector where T : struct
    {
        CStructureMultiDimArrayMemory<T> mem;

        //public int memoryid = 0;

        public WrappedCStructureMultiDimArrayMemoryDirectorNET(CStructureMultiDimArrayMemory<T> mem)
        {
            this.mem = mem;

            this.objectheapid = RRObjectHeap.AddObject(this);
        }

        public override ulong DimCount()
        {
            try
            {
                return mem.DimCount;
            }
            catch (Exception e)
            {
                using (MessageEntry merr = new MessageEntry())
                {
                    RobotRaconteurExceptionUtil.ExceptionToMessageEntry(e, merr);
                    RRDirectorExceptionHelper.SetError(merr);
                    return 0;
                }

            }
        }

        public override vector_uint64_t Dimensions()
        {
            try
            {
                return new vector_uint64_t(mem.Dimensions);
            }
            catch (Exception e)
            {
                using (MessageEntry merr = new MessageEntry())
                {
                    RobotRaconteurExceptionUtil.ExceptionToMessageEntry(e, merr);
                    RRDirectorExceptionHelper.SetError(merr);
                    return new vector_uint64_t();
                }

            }
        }

        public override MessageElementCStructureMultiDimArray Read(vector_uint64_t memorypos, vector_uint64_t bufferpos, vector_uint64_t count)
        {
            try
            {
                using (memorypos)
                using (bufferpos)
                using (count)
                {
                    CStructureMultiDimArray buffer3 = new CStructureMultiDimArray(count.Select(x => (int)x).ToArray(), new T[count.Aggregate(1, (x, y) => (int)x * (int)y)]);
                    mem.Read(memorypos.ToArray(), buffer3, new ulong[count.Count], count.ToArray());
                    return RobotRaconteurNode.s.PackCStructureMultiDimArray<T>(buffer3);
                }
            }
            catch (Exception e)
            {
                using (MessageEntry merr = new MessageEntry())
                {
                    RobotRaconteurExceptionUtil.ExceptionToMessageEntry(e, merr);
                    RRDirectorExceptionHelper.SetError(merr);
                }
                return null;
            }
        }

        public override void Write(vector_uint64_t memorypos, MessageElementCStructureMultiDimArray buffer, vector_uint64_t bufferpos, vector_uint64_t count)
        {
            try
            {
                using (buffer)
                using (memorypos)
                using (bufferpos)
                using (count)
                {
                    CStructureMultiDimArray buffer2 = RobotRaconteurNode.s.UnpackCStructureMultiDimArray<T>(buffer);
                    mem.Write(memorypos.ToArray(), buffer2, bufferpos.ToArray(), count.ToArray());
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

    public interface Generator1<ReturnType, ParamType>
    {
        ReturnType Next(ParamType param);
        void AsyncNext(ParamType param, Action<ReturnType, Exception> handler, int timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE);
        void Abort();
        void AsyncAbort(Action<Exception> handler, int timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE);
        void Close();
        void AsyncClose(Action<Exception> handler, int timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE);
    }

    public interface Generator2<ReturnType>
    {
        ReturnType Next();
        void AsyncNext(Action<ReturnType, Exception> handler, int timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE);
        void Abort();
        void AsyncAbort(Action<Exception> handler, int timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE);
        void Close();
        void AsyncClose(Action<Exception> handler, int timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE);
        ReturnType[] NextAll();
    }

    public interface Generator3<ParamType>
    {
        void Next(ParamType param);
        void AsyncNext(ParamType param, Action<Exception> handler, int timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE);
        void Abort();
        void AsyncAbort(Action<Exception> handler, int timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE);
        void Close();
        void AsyncClose(Action<Exception> handler, int timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE);
    }

    public abstract class SyncGenerator1<ReturnType, ParamType> : Generator1<ReturnType, ParamType>
    {
        public abstract void Abort();
        public abstract void Close();
        public abstract ReturnType Next(ParamType param);

        public void AsyncAbort(Action<Exception> handler, int timeout = -1)
        {
            try
            {
                Abort();
                handler(null);
            }
            catch (Exception e)
            {
                handler(e);
                return;
            }
            return;
        }

        public void AsyncClose(Action<Exception> handler, int timeout = -1)
        {
            try
            {
                Close();
                handler(null);
            }
            catch (Exception e)
            {
                handler(e);
                return;
            }
            return;
        }

        public void AsyncNext(ParamType param, Action<ReturnType, Exception> handler, int timeout = -1)
        {
            try
            {
                ReturnType r = Next(param);
                handler(r, null);
            }
            catch (Exception e)
            {
                handler(default(ReturnType),e);
                return;
            }
            return;
        }
    }

    public abstract class SyncGenerator2<ReturnType> : Generator2<ReturnType>
    {
        public abstract void Abort();
        public abstract void Close();
        public abstract ReturnType Next();

        public void AsyncAbort(Action<Exception> handler, int timeout = -1)
        {
            try
            {
                Abort();
                handler(null);
            }
            catch (Exception e)
            {
                handler(e);
                return;
            }
            return;
        }

        public void AsyncClose(Action<Exception> handler, int timeout = -1)
        {
            try
            {
                Close();
                handler(null);
            }
            catch (Exception e)
            {
                handler(e);
                return;
            }
            return;
        }

        public void AsyncNext(Action<ReturnType, Exception> handler, int timeout = -1)
        {
            try
            {
                ReturnType r = Next();
                handler(r, null);
            }
            catch (Exception e)
            {
                handler(default(ReturnType), e);
                return;
            }
            return;
        }

        public ReturnType[] NextAll()
        {
            List<ReturnType> o = new List<ReturnType>();
            try
            {
                while (true)
                {
                    o.Add(Next());
                }
            }
            catch (StopIterationException) { }
            return o.ToArray();
        }
    }

    public abstract class SyncGenerator3<ParamType> : Generator3<ParamType>
    {
        public abstract void Abort();
        public abstract void Close();
        public abstract void Next(ParamType param);

        public void AsyncAbort(Action<Exception> handler, int timeout = -1)
        {
            try
            {
                Abort();
                handler(null);
            }
            catch (Exception e)
            {
                handler(e);
                return;
            }
            return;
        }

        public void AsyncClose(Action<Exception> handler, int timeout = -1)
        {
            try
            {
                Close();
                handler(null);
            }
            catch (Exception e)
            {
                handler(e);
                return;
            }
            return;
        }

        public void AsyncNext(ParamType param, Action<Exception> handler, int timeout = -1)
        {
            try
            {
                 Next(param);
                handler(null);
            }
            catch (Exception e)
            {
                handler(e);
                return;
            }
            return;
        }
    }

    public class EnumeratorGenerator<T> : SyncGenerator2<T>
    {
        bool aborted = false;
        bool closed = false;
        IEnumerator<T> enumerator;

        public EnumeratorGenerator(IEnumerable<T> enumerable)
            : this(enumerable.GetEnumerator())
        { }

        public EnumeratorGenerator(IEnumerator<T> enumerator)
        {
            this.enumerator = enumerator;
        }

        public override void Abort()
        {
            lock (this)
            {
                aborted = true;
            }
            
        }

        public override void Close()
        {
            lock (this)
            {
                closed = true;
            }

        }

        public override T Next()
        {
            lock (this)
            {
                if (aborted) throw new OperationAbortedException("Generator aborted");
                if (closed) throw new StopIterationException("");
                if (!enumerator.MoveNext()) throw new StopIterationException("");
                return enumerator.Current;
            }
        }
    }

    public class Generator1Client<ReturnType, ParamType> : Generator1<ReturnType, ParamType>
    {
        protected WrappedGeneratorClient inner_gen;

        public Generator1Client(WrappedGeneratorClient inner_gen)
        {
            this.inner_gen = inner_gen;
        }

        public ReturnType Next(ParamType param)
        {
            object param1 = RobotRaconteurNode.s.PackVarType(param);
            try
            {
                using (MessageElement m = new MessageElement("parameter", param1))
                {
                    using (MessageElement m2 = inner_gen.Next(m))
                    {
                        return Wire<ReturnType>.UnpackData(m2);
                    }
                }
            }
            finally
            {
                IDisposable d = param1 as IDisposable;
                if (d != null) d.Dispose();
            }
        }
        public void AsyncNext(ParamType param, Action<ReturnType, Exception> handler, int timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE)
        {
            object param1 = RobotRaconteurNode.s.PackVarType(param);
            try
            {
                using (MessageElement m = new MessageElement("parameter", param1))
                {
                    AsyncRequestDirectorImpl d = new AsyncRequestDirectorImpl(EndAsyncNext, handler);
                    int id = RRObjectHeap.AddObject(d);
                    inner_gen.AsyncNext(m, timeout, d, id);          
                }
            }
            finally
            {
                IDisposable d = param1 as IDisposable;
                if (d != null) d.Dispose();
            }            
        }

        private static void EndAsyncNext(MessageElement m, Exception err, object p)
        {
            Action<ReturnType, Exception> h = (Action<ReturnType, Exception>)p;
            if (err!=null)
            {
                h(default(ReturnType), err);
                return;
            }
            
            h(Wire<ReturnType>.UnpackData(m),null);   
        }

        public void Abort()
        {
            inner_gen.Abort();
        }
        public void AsyncAbort(Action<Exception> handler, int timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE)
        {
            AsyncVoidReturnDirectorImpl h = new AsyncVoidReturnDirectorImpl(delegate (Exception e1, object o1) { handler(e1); }, null);
            int id = RRObjectHeap.AddObject(h);
            inner_gen.AsyncAbort(timeout, h, id);
        }

        public void Close()
        {
            inner_gen.Close();
        }
        public void AsyncClose(Action<Exception> handler, int timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE)
        {
            AsyncVoidReturnDirectorImpl h = new AsyncVoidReturnDirectorImpl(delegate (Exception e1, object o1) { handler(e1); }, null);
            int id = RRObjectHeap.AddObject(h);
            inner_gen.AsyncClose(timeout, h, id);
        }
    }

    public class Generator2Client<ReturnType> : Generator2<ReturnType>
    {
        protected WrappedGeneratorClient inner_gen;

        public Generator2Client(WrappedGeneratorClient inner_gen)
        {
            this.inner_gen = inner_gen;
        }

        public ReturnType Next()
        {            
            using (MessageElement m2 = inner_gen.Next(null))
            {
                return Wire<ReturnType>.UnpackData(m2);
            }            
        }
        public void AsyncNext(Action<ReturnType, Exception> handler, int timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE)
        {            
            AsyncRequestDirectorImpl d = new AsyncRequestDirectorImpl(EndAsyncNext, handler);
            int id = RRObjectHeap.AddObject(d);
            inner_gen.AsyncNext(null, timeout, d, id);                
        }

        private static void EndAsyncNext(MessageElement m, Exception err, object p)
        {
            Action<ReturnType, Exception> h = (Action<ReturnType, Exception>)p;
            if (err != null)
            {
                h(default(ReturnType), err);
                return;
            }

            h(Wire<ReturnType>.UnpackData(m), null);
        }

        public void Abort()
        {
            inner_gen.Abort();
        }
        public void AsyncAbort(Action<Exception> handler, int timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE)
        {
            AsyncVoidReturnDirectorImpl h = new AsyncVoidReturnDirectorImpl(delegate (Exception e1, object o1) { handler(e1); }, null);
            int id = RRObjectHeap.AddObject(h);
            inner_gen.AsyncAbort(timeout, h, id);
        }

        public void Close()
        {
            inner_gen.Close();
        }
        public void AsyncClose(Action<Exception> handler, int timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE)
        {
            AsyncVoidReturnDirectorImpl h = new AsyncVoidReturnDirectorImpl(delegate (Exception e1, object o1) { handler(e1); }, null);
            int id = RRObjectHeap.AddObject(h);
            inner_gen.AsyncClose(timeout, h, id);
        }

        public ReturnType[] NextAll()
        {
            List<ReturnType> o=new List<ReturnType>();
            try
            {
                while (true)
                {
                    o.Add(Next());
                }
            }
            catch (StopIterationException) { }
            return o.ToArray();
        }
    }

    public class Generator3Client<ParamType> : Generator3<ParamType>
    {
        protected WrappedGeneratorClient inner_gen;

        public Generator3Client(WrappedGeneratorClient inner_gen)
        {
            this.inner_gen = inner_gen;
        }

        public void Next(ParamType param)
        {
            object param1 = RobotRaconteurNode.s.PackVarType(param);
            try
            {
                using (MessageElement m = new MessageElement("parameter", param1))
                {
                    inner_gen.Next(m);           
                }
            }
            finally
            {
                IDisposable d = param1 as IDisposable;
                if (d != null) d.Dispose();
            }
        }
        public void AsyncNext(ParamType param, Action<Exception> handler, int timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE)
        {
            object param1 = RobotRaconteurNode.s.PackVarType(param);
            try
            {
                using (MessageElement m = new MessageElement("parameter", param1))
                {
                    AsyncRequestDirectorImpl d = new AsyncRequestDirectorImpl(EndAsyncNext, handler);
                    int id = RRObjectHeap.AddObject(d);
                    inner_gen.AsyncNext(m, timeout, d, id);
                }
            }
            finally
            {
                IDisposable d = param1 as IDisposable;
                if (d != null) d.Dispose();
            }
        }

        private static void EndAsyncNext(MessageElement m, Exception err, object p)
        {
            Action<Exception> h = (Action<Exception>)p;
            h(err);
        }

        public void Abort()
        {
            inner_gen.Abort();
        }
        public void AsyncAbort(Action<Exception> handler, int timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE)
        {
            AsyncVoidReturnDirectorImpl h = new AsyncVoidReturnDirectorImpl(delegate (Exception e1, object o1) { handler(e1); }, null);
            int id = RRObjectHeap.AddObject(h);
            inner_gen.AsyncAbort(timeout, h, id);
        }

        public void Close()
        {
            inner_gen.Close();
        }
        public void AsyncClose(Action<Exception> handler, int timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE)
        {
            AsyncVoidReturnDirectorImpl h = new AsyncVoidReturnDirectorImpl(delegate (Exception e1, object o1) { handler(e1); }, null);
            int id = RRObjectHeap.AddObject(h);
            inner_gen.AsyncClose(timeout, h, id);
        }
    }

    public class WrappedGenerator1ServerDirectorNET<ReturnType,ParamType> : WrappedGeneratorServerDirector
    {
        Generator1<ReturnType, ParamType> generator;
        public WrappedGenerator1ServerDirectorNET(Generator1<ReturnType, ParamType> generator)
        {
            if (generator == null) throw new NullReferenceException("Generator must not be null");
            this.generator = generator;
            this.objectheapid = RRObjectHeap.AddObject(this);
        }
        
        public override MessageElement Next(MessageElement m)
        {
            using (m)
            {
                try
                {
                    ParamType p = Wire<ParamType>.UnpackData(m);
                    ReturnType r = generator.Next(p);
                    object r1 = RobotRaconteurNode.s.PackVarType(r);
                    try
                    {
                        MessageElement m_r = new MessageElement("return", r1);
                        return m_r;
                    }
                    finally
                    {
                        IDisposable d = r1 as IDisposable;
                        if (d != null) d.Dispose();
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
        }

        public override void Abort()
        {
            try
            {
                generator.Abort();
            }
            catch (Exception e)
            {
                using (MessageEntry merr = new MessageEntry())
                {
                    RobotRaconteurExceptionUtil.ExceptionToMessageEntry(e, merr);
                    RRDirectorExceptionHelper.SetError(merr);
                    return;
                }
            }
        }

        public override void Close()
        {
            try
            {
                generator.Close();
            }
            catch (Exception e)
            {
                using (MessageEntry merr = new MessageEntry())
                {
                    RobotRaconteurExceptionUtil.ExceptionToMessageEntry(e, merr);
                    RRDirectorExceptionHelper.SetError(merr);
                    return;
                }
            }
        }
    }

    public class WrappedGenerator2ServerDirectorNET<ReturnType> : WrappedGeneratorServerDirector
    {
        Generator2<ReturnType> generator;
        public WrappedGenerator2ServerDirectorNET(Generator2<ReturnType> generator)
        {
            if (generator == null) throw new NullReferenceException("Generator must not be null");
            this.generator = generator;
            this.objectheapid = RRObjectHeap.AddObject(this);
        }

        public override MessageElement Next(MessageElement m)
        {
            using (m)
            {
                try
                {
                    ReturnType r = generator.Next();
                    object r1 = RobotRaconteurNode.s.PackVarType(r);
                    try
                    {
                        MessageElement m_r = new MessageElement("return", r1);
                        {
                            return m_r;
                        }
                    }
                    finally
                    {
                        IDisposable d = r1 as IDisposable;
                        if (d != null) d.Dispose();
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
        }

        public override void Abort()
        {
            try
            {
                generator.Abort();
            }
            catch (Exception e)
            {
                using (MessageEntry merr = new MessageEntry())
                {
                    RobotRaconteurExceptionUtil.ExceptionToMessageEntry(e, merr);
                    RRDirectorExceptionHelper.SetError(merr);
                    return;
                }
            }
        }

        public override void Close()
        {
            try
            {
                generator.Close();
            }
            catch (Exception e)
            {
                using (MessageEntry merr = new MessageEntry())
                {
                    RobotRaconteurExceptionUtil.ExceptionToMessageEntry(e, merr);
                    RRDirectorExceptionHelper.SetError(merr);
                    return;
                }
            }
        }
    }

    public class WrappedGenerator3ServerDirectorNET<ParamType> : WrappedGeneratorServerDirector
    {
        Generator3<ParamType> generator;
        public WrappedGenerator3ServerDirectorNET(Generator3<ParamType> generator)
        {
            if (generator == null) throw new NullReferenceException("Generator must not be null");
            this.generator = generator;
            this.objectheapid = RRObjectHeap.AddObject(this);
        }

        public override MessageElement Next(MessageElement m)
        {
            using (m)
            {
                try
                {
                    ParamType p = Wire<ParamType>.UnpackData(m);
                    generator.Next(p);
                    return null;
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
        }

        public override void Abort()
        {
            try
            {
                generator.Abort();
            }
            catch (Exception e)
            {
                using (MessageEntry merr = new MessageEntry())
                {
                    RobotRaconteurExceptionUtil.ExceptionToMessageEntry(e, merr);
                    RRDirectorExceptionHelper.SetError(merr);
                    return;
                }
            }
        }

        public override void Close()
        {
            try
            {
                generator.Close();
            }
            catch (Exception e)
            {
                using (MessageEntry merr = new MessageEntry())
                {
                    RobotRaconteurExceptionUtil.ExceptionToMessageEntry(e, merr);
                    RRDirectorExceptionHelper.SetError(merr);
                    return;
                }
            }
        }
    }

    internal class AsyncGeneratorClientReturnDirectorImpl : AsyncGeneratorClientReturnDirector
    {

        protected Action<WrappedGeneratorClient, Exception, object> handler_func;
        protected object param;

        public AsyncGeneratorClientReturnDirectorImpl(Action<WrappedGeneratorClient, Exception, object> handler_func, object param)
        {
            this.handler_func = handler_func;
            this.param = param;
        }

        public override void handler(WrappedGeneratorClient m, uint error_code, string errorname, string errormessage)
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