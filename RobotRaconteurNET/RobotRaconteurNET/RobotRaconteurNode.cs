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

/// <summary>
/// The central node implementation
/// </summary>
/// <remarks>
/// <para>
///     RobotRaconteurNode implements the current Robot Raconteur instance
///    and acts as the central switchpoint for the instance. The user
///    registers types, connects clients, registers services, and
///    registers transports through this class.
/// </para>
/// <para>
///    If the current program only needs one instance of RobotRaconteurNode,
///    the singleton can be used. The singleton is accessed using:
///</para>
/// <code>
///    RobotRaconteurNode.s
/// </code>
/// <para>
///    The node must be shut down before existing the program,
///    or a memory leak/hard crash will occur. This can either be
///    accomplished manually using the `Shutdown()` function,
///    or automatically by using the ClientNodeSetup or
///    ServerNodeSetup classes.
/// </para>
/// </remarks>
public partial class RobotRaconteurNode
{
    private static RobotRaconteurNode csharp_s = null;
    static private WrappedRobotRaconteurExceptionHelper exhelp;

    /// <summary>
    ///    Singleton accessor
    /// </summary>
    /// <remarks>
    ///    The RobotRaconteurNode singleton can be used when only
    ///    one instance of Robot Raconteur is required in a program.
    ///    The singleton must be shut down when the program exits.
    /// </remarks>
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

    /// <summary>
    /// Get or set the current NodeID
    /// </summary>
    /// <remarks>
    ///    Gets or setthe current NodeID. If one has not been set,
    ///    one will be automatically generated. Cannot be set if a NodeID has been assigned.
    /// </remarks>
    /// <value></value>
    public NodeID NodeID
    {
        get {
            return _NodeID();
        }
        set {
            _SetNodeID(value);
        }
    }

    /// <summary>
    /// Get or set the current NodeName
    /// </summary>
    /// <remarks>
    /// Gets or set the current NodeName. If one has not been set using,
    /// it will be an empty string. Cannot be set if a NodeName has been assigned.
    /// </remarks>
    public string NodeName
    {
        get {
            return _NodeName();
        }
        set {
            _SetNodeName(value);
        }
    }

    /// <summary>
    /// Get or set the number of threads for the node ThreadPool
    /// </summary>
    /// <remarks>
    ///  The ThreadPool will use a maximum of count threads. If
    ///  this number is lower than the current ThreadPool count,
    ///  the ThreadPool will attempt to release threads beyond count
    ///  as they return to idle
    /// </remarks>
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

    /// <summary>
    /// Register a service type
    /// </summary>
    /// <remarks>None</remarks>
    /// <param name="servicetype">The service factory implementing the type to register</param>
    public void RegisterServiceType(ServiceFactory servicetype)
    {
        lock (servicetypes)
        {
            servicetypes.Add(servicetype.GetServiceName(), servicetype);
            _RegisterServiceType(servicetype.ServiceDef());
        }
    }

    /// <summary>
    /// Returns a previously registered service type
    /// </summary>
    /// <remarks>None</remarks>
    /// <param name="servicename">The name of the service type to retrieve</param>
    /// <returns>The service factory</returns>
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

    /// <summary>
    /// Create a client connection to a remote service using a URL
    /// </summary>
    /// <remarks>
    /// <para>
    ///  Synchronously creates a connection to a remote service using a URL. URLs are either provided by
    ///  the service, or are determined using discovery functions such as FindServiceByType().
    ///  This function is the primary way to create client connections.
    /// </para>
    /// <para>
    ///  username and credentials can be used to specify authentication information. Credentials will
    ///  often contain a "password" or token entry.
    /// </para>
    /// <para>
    ///  The listener is a function that is called during various events. See ClientServiceListenerEventType
    ///  for a description of the possible events.
    /// </para>
    /// <para>
    ///  ConnectService will attempt to instantiate a client object reference (proxy) based on the type
    ///  information provided by the service. The type information will contain the type of the object,
    ///  and all the implemented types. The client will normally want a specific one of the implement types.
    ///  Specify this desired type in objecttype to avoid future compatibility issues.
    /// </para>
    /// <para>
    ///  Requires multithreading
    /// </para>
    /// </remarks>
    /// <param name="url">The URL of the service to connect</param>
    /// <param name="username">An optional username for authentication</param>
    /// <param name="credentials">Optional credentials for authentication</param>
    /// <param name="listener">An optional listener callback function</param>
    /// <param name="objecttype">The desired root object proxy type. Optional but highly recommended.</param>
    /// <returns>The root object reference of the connected service</returns>
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
    /// <summary>
    /// Create a client connection to a remote service using a URL
    /// </summary>
    /// <remarks>
    /// <para>
    ///  Synchronously creates a connection to a remote service using a URL. URLs are either provided by
    ///  the service, or are determined using discovery functions such as FindServiceByType().
    ///  This function is the primary way to create client connections.
    /// </para>
    /// <para>
    ///  username and credentials can be used to specify authentication information. Credentials will
    ///  often contain a "password" or token entry.
    /// </para>
    /// <para>
    ///  The listener is a function that is called during various events. See ClientServiceListenerEventType
    ///  for a description of the possible events.
    /// </para>
    /// <para>
    ///  ConnectService will attempt to instantiate a client object reference (proxy) based on the type
    ///  information provided by the service. The type information will contain the type of the object,
    ///  and all the implemented types. The client will normally want a specific one of the implement types.
    ///  Specify this desired type in objecttype to avoid future compatibility issues.
    /// </para>
    /// <para>
    ///  Requires multithreading
    /// </para>
    /// </remarks>
    /// <param name="url">The candidate URLs of the service to connect</param>
    /// <param name="username">An optional username for authentication</param>
    /// <param name="credentials">Optional credentials for authentication</param>
    /// <param name="listener">An optional listener callback function</param>
    /// <param name="objecttype">The desired root object proxy type. Optional but highly recommended.</param>
    /// <returns>The root object reference of the connected service</returns>
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

    /// <summary>
    /// Disconnects a client connection to a service
    /// </summary>
    /// <remarks>
    /// <para>
    ///  Synchronously disconnects a client connection. Client connections
    ///  are automatically closed by Shutdown(), so this function
    ///  is optional.
    /// </para>
    /// <para>
    ///  Requires multithreading
    /// </para>
    /// </remarks>
    /// <param name="obj">The root object of the service to disconnect</param>
    public void DisconnectService(object obj)
    {
        ServiceStub stub = (ServiceStub)obj;
        _DisconnectService(stub.rr_innerstub);
    }

    /// <summary>
    /// Asynchronously create a client connection to a remote service using a URL
    /// </summary>
    /// <remarks>
    /// Same as ConnectService but async. See ConnectService() for more details
    /// on client connections.
    /// </remarks>
    /// <param name="url">The URL of the service to connect</param>
    /// <param name="username">An optional username for authentication</param>
    /// <param name="credentials">Optional credentials for authentication</param>
    /// <param name="listener">An optional listener callback function</param>
    /// <param name="objecttype">The desired root object proxy type. Optional but highly recommended.</param>
    /// <param name="timeout">Timeout is milliseconds, or RR_TIMEOUT_INFINITE for no timeout.</param>
    /// <returns>Task that returns the client object upon completion</returns>
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

    /// <summary>
    /// Asynchronously create a client connection to a remote service using a URL
    /// </summary>
    /// <remarks>
    /// Same as ConnectService but async. See ConnectService() for more details
    /// on client connections.
    /// </remarks>
    /// <param name="url">The candidate URLs of the service to connect</param>
    /// <param name="username">An optional username for authentication</param>
    /// <param name="credentials">Optional credentials for authentication</param>
    /// <param name="listener">An optional listener callback function</param>
    /// <param name="objecttype">The desired root object proxy type. Optional but highly recommended.</param>
    /// <param name="timeout">Timeout is milliseconds, or RR_TIMEOUT_INFINITE for no timeout.</param>
    /// <returns>Task that returns the client object upon completion</returns>
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

    /// <summary>
    /// Asynchronously disconnects a client connection to a service
    /// </summary>
    /// <remarks>
    ///    Same as DisconnectService() but async.
    /// </remarks>
    /// <param name="obj">The root object of the client to disconnect</param>
    public async Task AsyncDisconnectService(object obj)
    {
        ServiceStub stub = (ServiceStub)obj;
        AsyncVoidNoErrReturnDirectorImpl h = new AsyncVoidNoErrReturnDirectorImpl();
        int id = RRObjectHeap.AddObject(h);
        _AsyncDisconnectService(stub.rr_innerstub, h, id);
        await h.Task;
    }

    /// <summary>
    /// Get the service attributes of a client connection
    /// </summary>
    /// <remarks>
    ///  Returns the service attributes of a client connected using
    ///  ConnectService()
    /// </remarks>
    /// <param name="obj">The root object of the client to use to retrieve service attributes</param>
    /// <returns>Dictionary of the service attributes</returns>
    public Dictionary<string, object> GetServiceAttributes(object obj)
    {
        ServiceStub stub = (ServiceStub)obj;
        using (MessageElement me = _GetServiceAttributes(stub.rr_innerstub))
        {
            return (Dictionary<string, object>)UnpackVarType(me);
        }
    }

    /// <summary>
    /// Get the service NodeID of the remote node from a client connection
    /// </summary>
    /// <remarks>
    ///  Returns the NodeID of the remote node that a client is connected
    /// </remarks>
    /// <param name="obj">The root object of the client to use to retrieve service attributes</param>
    /// <returns>The NodeID</returns>
    public NodeID GetServiceNodeID(object obj)
    {
        ServiceStub stub = (ServiceStub)obj;
        return _GetServiceNodeID(stub.rr_innerstub);
    }

    /// <summary>
    /// Get the service NodeName of the remote node from a client connection
    /// </summary>
    /// <remarks>
    ///  Returns the NodeName of the remote node that a client is connected
    /// </remarks>
    /// <param name="obj">The root object of the client to use to retrieve service attributes</param>
    /// <returns>The NodeName</returns>
    public string GetServiceNodeName(object obj)
    {
        ServiceStub stub = (ServiceStub)obj;
        return _GetServiceNodeName(stub.rr_innerstub);
    }

    /// <summary>
    /// Get the name of a service from a client connection
    /// </summary>
    /// <remarks>
    ///  Returns the service name of the remote service that a client is connected
    /// </remarks>
    /// <param name="obj">The root object of the client to use to retrieve service attributes</param>
    /// <returns>The service name</returns>
    public string GetServiceName(object obj)
    {
        ServiceStub stub = (ServiceStub)obj;
        return _GetServiceName(stub.rr_innerstub);
    }

    /// <summary>
    /// Get the Robot Raconteur type of a connected service object
    /// </summary>
    /// <remarks>
    ///  obj must be returned by ConnectService(), AsyncConnectService(),
    ///  or an `objref`
    /// </remarks>
    /// <param name="obj">The object to query</param>
    /// <returns>The Robot Raconteur type of the object</returns>
    public string GetObjectType(object obj)
    {
        ServiceStub stub = (ServiceStub)obj;
        return _GetObjectType(stub.rr_innerstub);
    }

    /// <summary>
    /// Get the service path of a client object
    /// </summary>
    /// <remarks>None</remarks>
    /// <param name="obj">The object to get the service path for</param>
    /// <returns>The object's service path</returns>
    public string GetObjectServicePath(object obj)
    {
        ServiceStub stub = (ServiceStub)obj;
        return _GetObjectServicePath(stub.rr_innerstub);
    }

    /// <summary>
    /// Get a service factory for a C# type
    /// </summary>
    /// <remarks>None</remarks>
    /// <param name="type">The C# type name</param>
    /// <returns>The service factory</returns>
    private ServiceFactory GetServiceFactoryForType(string type)
    {
        string servicename = SplitQualifiedName(type).Item1;
        return GetServiceType(servicename);
    }

    /// <summary>
    /// Get a service factory for a C# type
    /// </summary>
    /// <remarks>None</remarks>
    /// <param name="type">The C# type</param>
    /// <returns>The service factory</returns>
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

    /// <summary>
    /// Request an exclusive access lock to a service object
    /// </summary>
    /// <remarks>
    /// <para>
    /// Called by clients to request an exclusive lock on a service object and
    /// all subobjects (`objrefs`) in the service. The exclusive access lock will
    /// prevent other users ("User" lock) or client connections  ("Session" lock)
    /// from interacting with the objects.
    /// </para>
    /// <para>
    /// Requires multithreading
    /// </para>
    /// </remarks>
    /// <param name="obj">The object to lock. Must be returned by ConnectService or returned by an `objref`</param>
    /// <param name="flags">Select either a "User" or "Session" lock</param>
    /// <returns>"OK" on success</returns>
    public string RequestObjectLock(object obj, RobotRaconteurObjectLockFlags flags)
    {
        if (!(obj is ServiceStub))
            throw new ArgumentException("Can only lock object opened through Robot Raconteur");
        ServiceStub s = (ServiceStub)obj;

        return _RequestObjectLock(s.rr_innerstub, flags);
    }

    /// <summary>
    /// Release an excluse access lock previously locked with RequestObjectLock()
    /// or AsyncRequestObjectLock()
    /// </summary>
    /// <remarks>
    /// <para>
    /// Object must have previously been locked using RequestObjectLock() or
    /// AsyncRequestObjectLock()
    /// </para>
    /// <para>
    /// Requires multithreading
    /// </para>
    /// </remarks>
    /// <param name="obj">The object previously locked</param>
    /// <returns>"OK" on success</returns>
    public string ReleaseObjectLock(object obj)
    {
        if (!(obj is ServiceStub))
            throw new ArgumentException("Can only unlock object opened through Robot Raconteur");
        ServiceStub s = (ServiceStub)obj;

        return _ReleaseObjectLock(s.rr_innerstub);
    }

    /// <summary>
    /// Asynchronously request an exclusive access lock to a service object
    /// </summary>
    /// <remarks>
    /// Same as RequestObjectLock() but async
    /// </remarks>
    /// <param name="obj">The object to lock. Must be returned by ConnectService or returned by an `objref`</param>
    /// <param name="flags">Select either a "User" or "Session" lock</param>
    /// <param name="timeout">Timeout in milliseconds, or RR_TIMEOUT_INFINITE</param>
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

    /// <summary>
    /// Asynchronously release an excluse access lock previously locked
    /// with RequestObjectLock() or AsyncRequestObjectLock()
    /// </summary>
    /// <remarks>
    /// Same as ReleaseObjectLock() but async
    /// </remarks>
    /// <param name="obj">The object previously locked</param>
    /// <param name="timeout">Timeout in milliseconds, or RR_TIMEOUT_INFINITE</param>
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

    /// <summary>
    /// Creates a monitor lock on a specified object
    /// </summary>
    /// <remarks>
    /// <para>
    /// Monitor locks are intendended for short operations that require
    /// guarding to prevent races, corruption, or other concurrency problems.
    /// Monitors emulate a single thread locking the service object.
    /// </para>
    /// <para>
    /// Use of ScopedMonitorLock instead of this function is highly recommended
    /// to take advantage of RAII scoping
    /// </para>
    /// <para>
    /// Monitor locks do not lock any sub-objects (objref)
    /// </para>
    /// <para>
    /// Requires multithreading
    /// </para>
    /// </remarks>
    /// <param name="obj">The object to lock</param>
    /// <param name="timeout">The timeout in milliseconds to acquire the monitor lock, or RR_TIMEOUT_INFINITE</param>
    public void MonitorEnter(object obj, int timeout = -1)
    {
        if (!(obj is ServiceStub))
            throw new ArgumentException("Only service stubs can be monitored by RobotRaconteurNode");
        ServiceStub s = (ServiceStub)obj;

        _MonitorEnter(s.rr_innerstub, timeout);
    }

    /// <summary>
    /// Releases a monitor lock
    /// </summary>
    /// <remarks>
    /// <para>
    /// Use of ScopedMonitorLock instead of this function is highly recommended
    /// to take advantage of RAII scoping
    /// </para>
    /// <para>
    /// Requires multithreading
    /// </para>
    /// </remarks>
    /// <param name="obj">The object previously locked by MonitorEnter()</param>
    public void MonitorExit(object obj)
    {
        if (!(obj is ServiceStub))
            throw new ArgumentException("Only service stubs can be monitored by RobotRaconteurNode");
        ServiceStub s = (ServiceStub)obj;

        _MonitorExit(s.rr_innerstub);
    }

    /// <summary>
    /// Wrapper for RobotRaconteurNode.MonitorEnter() and
    /// RobotRaconteurNode.MonitorExit() for use with `using` statements
    /// </summary>
    /// <remarks>None</remarks>
    public class ScopedMonitorLock : IDisposable
    {
        object obj;
        bool locked;

        /// <summary>
        /// Create a monitor lock for the specified object
        /// </summary>
        /// <remarks>
        /// Creates a monitor lock by calling RobotRaconteur.MonitorEnter().
        /// Object will be locked once the object is created.
        /// </remarks>
        /// <param name="obj">The object to monitor lock</param>
        /// <param name="timeout">The timeout in milliseconds to acquire the monitor lock, or
        /// RR_TIMEOUT_INFINITE</param>
        public ScopedMonitorLock(object obj, int timeout = -1)
        {
            RobotRaconteurNode.s.MonitorEnter(obj, timeout);
            locked = true;
        }

        /// <summary>
        /// Relock the object after calling unlock()
        /// </summary>
        /// <remarks>None</remarks>
        /// <param name="timeout">The timeout in milliseconds to acquire the monitor lock, or
        /// RR_TIMEOUT_INFINITE</param>
        public void lock_(Int32 timeout = -1)
        {
            if (obj == null)
                return;
            if (locked)
                throw new Exception("Already locked");
            RobotRaconteurNode.s.MonitorEnter(obj, timeout);
            locked = true;
        }

        /// <summary>
        /// Releases the monitor lock
        /// </summary>
        /// <remarks>
        /// The ScopedMonitorLock destructor will release
        /// the lock automatically, so in most cases it is
        /// not necessary to call this function
        /// </remarks>
        /// <param name="timeout">The timeout in milliseconds to acquire the monitor lock, or
        /// RR_TIMEOUT_INFINITE</param>
        public void unlock(Int32 timeout = -1)
        {
            if (obj == null)
                return;
            if (!locked)
                throw new Exception("Not locked");
            RobotRaconteurNode.s.MonitorExit(timeout);
            locked = false;
        }

        /// <summary>
        /// Release the monitor lock from the class
        /// </summary>
        /// <remarks>
        /// The monitor lock is released from the ScopedMonitorLock
        /// instance. The monitor lock will not be released
        /// by the ScopedMonitorLock destructor.
        /// </remarks>
        public void release()
        {
            obj = null;
        }

        /// <summary>
        /// Unlock if object has not been released
        /// </summary>
        /// <remarks>None</remarks>
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

    /// <summary>
    /// Use discovery to find available services by service type
    /// </summary>
    /// <remarks>
    /// <para>
    /// Uses discovery to find available services based on a service type. This
    /// service type is the type of the root object, ie
    /// `com.robotraconteur.robotics.robot.Robot`. This process will update the detected
    /// node cache.
    /// </para>
    /// <para>
    /// Requires multithreading
    /// </para>
    /// </remarks>
    /// <param name="servicetype">The service type to find, ie `com.robotraconteur.robotics.robot.Robot`</param>
    /// <param name="transportschemes">A list of transport types to search, ie `rr+tcp`, `rr+local`, `rrs+tcp`,
    /// etc</param>
    /// <returns>The detected services</returns>
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

    /// <summary>
    /// Finds nodes on the network with a specified NodeID
    /// </summary>
    /// <remarks>
    /// <para>
    /// Updates the discovery cache and find nodes with the specified NodeID.
    /// This function returns unverified cache information.
    /// </para>
    /// <para>
    /// Requires multithreading
    /// </para>
    /// </remarks>
    /// <param name="id">The NodeID to find</param>
    /// <param name="transportschemes">A list of transport types to search, ie `rr+tcp`, `rr+local`, `rrs+tcp`,
    /// etc</param> <returns>The detected nodes</returns>
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

    /// <summary>
    /// Finds nodes on the network with a specified NodeName
    /// </summary>
    /// <remarks>
    /// <para>
    /// Updates the discovery cache and find nodes with the specified NodeName.
    /// This function returns unverified cache information.
    /// </para>
    /// <para>
    /// Requires multithreading
    /// </para>
    /// </remarks>
    /// <param name="name">The NodeName to find</param>
    /// <param name="transportschemes">A list of transport types to search, ie `rr+tcp`, `rr+local`, `rrs+tcp`,
    /// etc</param> <returns>The detected nodes</returns>
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

    /// <summary>
    /// Disable timeout for asynchronous operations
    /// </summary>
    /// <remarks>None</remarks>
    public const int RR_TIMEOUT_INFINITE = -1;

    /// <summary>
    /// Asynchronously use discovery to find availabe services by service type
    /// </summary>
    /// <remarks>
    ///  Same as FindServiceByType() but async
    /// </remarks>
    /// <param name="servicetype">The service type to find, ie `com.robotraconteur.robotics.robot.Robot`</param>
    /// <param name="transportschemes">A list of transport types to search, ie `rr+tcp`, `rr+local`, `rrs+tcp`,
    /// etc</param>
    /// <param name="timeout">Timeout in milliseconds. Using a timeout greater than 5 seconds is not
    /// recommended.</param>
    /// <returns>A task that returns array of detected services upon completion</returns>
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

    /// <summary>
    /// Asynchronously finds nodes on the network with the specified NodeID
    /// </summary>
    /// <remarks>
    ///    Same as FindNodeByID() but async
    /// </remarks>
    /// <param name="id">The NodeID to find</param>
    /// <param name="transportschemes">A list of transport types to search, ie `rr+tcp`, `rr+local`, `rrs+tcp`,
    /// etc</param>
    /// <param name="timeout">Timeout in milliseconds. Using a timeout greater than 5 seconds is not
    /// recommended.</param>
    /// <returns>Task that returns array of detected nodes upon completion</returns>
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

    /// <summary>
    /// Asynchronously finds nodes on the network with the specified NodeName
    /// </summary>
    /// <remarks>
    ///    Same as FindNodeByName() but async
    /// </remarks>
    /// <param name="name">The NodeName to find</param>
    /// <param name="transportschemes">A list of transport types to search, ie `rr+tcp`, `rr+local`, `rrs+tcp`,
    /// etc</param>
    /// <param name="timeout">Timeout in milliseconds. Using a timeout greater than 5 seconds is not
    /// recommended.</param>
    /// <returns>Task that returns array of detected nodes upon completion</returns>
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

    /// <summary>
    /// Update the detected nodes cache
    /// </summary>
    /// <remarks>
    /// <para>
    /// The node keeps a cache of detected nodes, but this may become stale
    /// if nodes are rapidly added and removed from the network. Call this
    /// function to update the detected nodes.
    /// </para>
    /// <para>
    /// Requires multithreading
    /// </para>
    /// </remarks>
    /// <param name="schemes">A vector of transport schemes, ie "rr+tcp", "rr+local", etc. to update.</param>
    public void UpdateDetectedNodes(string[] schemes)
    {
        vectorstring schemes1 = new vectorstring();
        foreach (string s in schemes)
            schemes1.Add(s);
        RobotRaconteurNET.WrappedUpdateDetectedNodes(this, schemes1);
    }

    /// <summary>
    /// Asynchronously update the detected nodes cache
    /// </summary>
    /// <remarks>
    /// Same as UpdateDetectedNodes() but async
    /// </remarks>
    /// <param name="schemes">A vector of transport schemes, ie "rr+tcp", "rr+local", etc. to update.</param>
    /// <param name="timeout">The timeout for the operation in milliseconds. This function will often run
    ///   for the full timeout, so values less than 5 seconds are recommended.</param>
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

    /// <summary>
    /// Get the nodes currently detected by Transports
    /// </summary>
    /// <remarks>
    /// <para>
    /// Transports configured to listen for node discovery send detected node
    /// information to the parent node, where it is stored. Normally this information
    /// will expire after one minute, and needs to be constantly refreshed.
    /// </para>
    /// <para>
    /// This node information is not verified. It is the raw discovery
    /// information received by the transports. Verification is done
    /// when the node is interrogated for service information.
    /// </para>
    /// </remarks>
    /// <returns>Array of detected nodes</returns>
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

    /// <summary>
    /// Get cached node discovery information
    /// </summary>
    /// <remarks>
    /// Return current node information from the discovery cache. This
    /// information is unverified and is used for the first step in the
    /// discovery process.
    /// </remarks>
    /// <param name="nodeid">The NodeID of the requested node</param>
    /// <returns>The detected node info</returns>
    public NodeInfo2 GetDetectedNodeCacheInfo(NodeID nodeid)
    {
        return new NodeInfo2(RobotRaconteurNET.WrappedGetDetectedNodeCacheInfo(this, nodeid));
    }

    /// <summary>
    /// Try get cached node discovery information
    /// </summary>
    /// <remarks>
    /// Same as GetDetectedNodeCacheInfo, but returns bool
    /// for success or failure instead of throwing an exception
    /// </remarks>
    /// <param name="nodeid">The NodeID of the requested node</param>
    /// <param name="nodeinfo2">Out parameter for the node info</param>
    /// <returns>true on success, false on failure</returns>
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

    /// <summary>
    /// Registers a service for clients to connect
    /// </summary>
    /// <remarks>
    /// <para>
    /// The supplied object becomes the root object in the service. Other objects may
    /// be accessed by clients using `objref` members. The name of the service must conform
    /// to the naming rules of Robot Raconteur member names. A service is closed using
    /// either CloseService() or when Shutdown() is called.
    /// </para>
    /// <para>
    /// Multiple services can be registered within the same node. Service names
    /// within a single node must be unique.
    /// </para>
    /// </remarks>
    /// <param name="name">The name of the service, must follow member naming rules</param>
    /// <param name="servicetype">The name of the service definition containing the object type.
    ///     Do not include the object type.</param>
    /// <param name="obj">The root object of the service</param>
    /// <param name="policy">An optional security policy for the service to control authentication
    ///     and other security functions</param>
    /// <returns>The instantiated ServerContext. This object is owned
    ///    by the node and the return can be safely ignored.</returns>
    public ServerContext RegisterService(string name, string servicetype, object obj,
                                         ServiceSecurityPolicy policy = null)
    {
        ServiceSkel skel = GetServiceType(servicetype).CreateSkel(obj);

        int id = RRObjectHeap.AddObject(skel);
        skel.innerskelid = id;
        WrappedRRObject o = new WrappedRRObject(skel.RRType, skel, id);

        return _RegisterService(name, servicetype, o, policy);
    }

    /// <summary>
    /// The current time in UTC time zone
    /// </summary>
    /// <remarks>
    /// Uses the internal node clock to get the current time in UTC.
    ///    While this will normally use the system clock, this may
    ///    use simulation time in certain circumstances
    /// </remarks>
    public DateTime NowUTC
    {
        get {
            return _NowUTC();
        }
    }

    /// <summary>
    /// The current node time
    /// </summary>
    /// <remarks>
    /// UTC time is not monotonic, due to the introduction of leap-seconds, and the possibility
    /// of the system clock being updated by the user. For a real-time systems,
    /// this is unaccetpable and can lead to system instability. The "node time" used by Robot Raconteur
    /// is synchronized to UTC at startup, and is then steadily increasing from that initial time.
    /// It will ignore changes to the system clock, and will also ignore corrections like leap
    /// seconds.
    /// </remarks>
    public DateTime NowNodeTime
    {
        get {
            return _NowNodeTime();
        }
    }

    /// <summary>
    /// The sync time of the node
    /// </summary>
    /// <remarks>
    /// The node synchronizes it's clock with the system time in UTC
    /// when the node is initialized. After this time, a steady
    /// clock is used. This prevents the clock from jumping
    /// forward and back in time. It will no longer be updated
    /// by changes in the system time.
    ///
    /// If an external high precision clock source like PTP is available,
    /// that clock will be used in place of the system and steady clock
    /// </remarks>
    public DateTime NodeSyncTimeUTC
    {
        get {
            return _NodeSyncTimeUTC();
        }
    }

    /// <summary>
    /// The current node time as a TimeSpec
    /// </summary>
    /// <remarks>
    /// The current node time as a TimeSpec. See NowNodeTime()
    /// </remarks>
    public TimeSpec NowTimeSpec
    {
        get {
            return _NowTimeSpec();
        }
    }

    /// <summary>
    /// The sync time of the node as a TimeSpec
    /// </summary>
    /// <remarks>
    /// See NodeSyncTimeUTC()
    /// </remarks>
    public TimeSpec NodeSyncTimeSpec
    {
        get {
            return _NodeSyncTimeSpec();
        }
    }

    /// <summary>
    /// Returns an objref as a specific type
    /// </summary>
    /// <remarks>
    /// <para>
    /// Robot Raconteur service object types are polymorphic using inheritence,
    /// meaning that an object may be represented using multiple object types.
    /// `objref` will attempt to return the relevant type, but it is sometimes
    /// necessary to request a specific type for an objref.
    /// </para>
    /// <para>
    /// This function will return the object from an `objref` as the specified type,
    /// or throw an error if the type is invalid.
    /// </para>
    /// <para>
    /// Requires multithreading
    /// </para>
    /// </remarks>
    /// <param name="obj">The object with the desired `objref`</param>
    /// <param name="objref">The name of the `objref` member</param>
    /// <param name="objecttype">The desired service object type</param>
    /// <returns>The object with the specified interface type. Must be cast to the desired type</returns>
    public object FindObjRefTyped(object obj, string objref, string objecttype)
    {
        if (!(obj is ServiceStub))
            throw new ArgumentException("Only service stubs can have objref");
        ServiceStub s = (ServiceStub)obj;

        return s.FindObjRefTyped(objref, objecttype);
    }

    /// <summary>
    /// Returns an indexed objref as a specified type
    /// </summary>
    /// <remarks>
    /// <para>
    /// Same as FindObjectTyped() but includes an `objref` index
    /// </para>
    /// <para>
    /// Requires multithreading
    /// </para>
    /// </remarks>
    /// <param name="obj">The object with the desired `objref`</param>
    /// <param name="objref">The name of the `objref` member</param>
    /// <param name="index">The index for the `objref`, convert int to string for int32 index type</param>
    /// <param name="objecttype">The desired service object type</param>
    /// <returns>The object with the specified interface type. Must be cast to the desired type</returns>
    public object FindObjRefTyped(object obj, string objref, string index, string objecttype)
    {
        if (!(obj is ServiceStub))
            throw new ArgumentException("Only service stubs can have objref");
        ServiceStub s = (ServiceStub)obj;

        return s.FindObjRefTyped(objref, index, objecttype);
    }

    /// <summary>
    /// Asynchronously returns an objref as a specific type
    /// </summary>
    /// <remarks>
    /// Same as FindObjectTyped() but returns asynchronously
    /// </remarks>
    /// <param name="obj">The object with the desired `objref`</param>
    /// <param name="objref">The name of the `objref` member</param>
    /// <param name="objecttype">The desired service object type</param>
    /// <param name="timeout">Timeout in milliseconds, or RR_TIMEOUT_INFINITE for no timeout</param>
    /// <returns>A task that upon completion returns the object with the specified interface type.
    ///   Must be cast to the desired type</returns>
    public Task<object> AsyncFindObjRefTyped(object obj, string objref, string objecttype,
                                             int timeout = RR_TIMEOUT_INFINITE)
    {
        if (!(obj is ServiceStub))
            throw new ArgumentException("Only service stubs can have objref");
        ServiceStub s = (ServiceStub)obj;

        return s.AsyncFindObjRefTyped<object>(objref, objecttype, timeout);
    }

    /// <summary>
    /// Asynchronously returns an objref as a specific type
    /// </summary>
    /// <remarks>
    /// Same as FindObjectTyped() but returns asynchronously
    /// </remarks>
    /// <param name="obj">The object with the desired `objref`</param>
    /// <param name="objref">The name of the `objref` member</param>
    /// <param name="index">The index for the `objref`, convert int to string for int32 index type</param>
    /// <param name="objecttype">The desired service object type</param>
    /// <param name="timeout">Timeout in milliseconds, or RR_TIMEOUT_INFINITE for no timeout</param>
    /// <returns>A task that upon completion returns the object with the specified interface type.
    ///   Must be cast to the desired type</returns>
    public Task<object> AsyncFindObjRefTyped(object obj, string objref, string index, string objecttype,
                                             int timeout = RR_TIMEOUT_INFINITE)
    {
        if (!(obj is ServiceStub))
            throw new ArgumentException("Only service stubs can have objref");
        ServiceStub s = (ServiceStub)obj;

        return s.AsyncFindObjRefTyped<object>(objref, index, objecttype, timeout);
    }

    /// <summary>
    /// Returns the type of a service object
    /// </summary>
    /// <remarks>
    /// <para>
    /// Returns the fully qualified object type that would be returned by an `objref` member
    /// </para>
    /// <para>
    /// Requires multithreading
    /// </para>
    /// </remarks>
    /// <param name="obj">The object with the desired `objref`</param>
    /// <param name="objref">The name of the `objref` member</param>
    /// <returns>The fully qaulified object type</returns>
    public string FindObjectType(object obj, string objref)
    {
        if (!(obj is ServiceStub))
            throw new ArgumentException("Only service stubs can have objref");
        ServiceStub s = (ServiceStub)obj;

        return _FindObjectType(s.rr_innerstub, objref);
    }

    /// <summary>
    /// Returns the type of a service object
    /// </summary>
    /// <remarks>
    /// <para>
    /// Returns the fully qualified object type that would be returned by an indexed `objref` member
    /// </para>
    /// <para>
    /// Requires multithreading
    /// </para>
    /// </remarks>
    /// <param name="obj">The object with the desired `objref`</param>
    /// <param name="objref">The name of the `objref` member</param>
    /// <param name="index">The index for the `objref`, convert int to string for int32 index type</param>
    /// <returns>The fully qaulified object type</returns>
    public string FindObjectType(object obj, string objref, string index)
    {
        if (!(obj is ServiceStub))
            throw new ArgumentException("Only service stubs can have objref");
        ServiceStub s = (ServiceStub)obj;

        return _FindObjectType(s.rr_innerstub, objref, index);
    }

    /// <summary>
    /// Asynchronously returns the type of a service object
    /// </summary>
    /// <remarks>
    /// Same as FindObjectType() but async
    /// </remarks>
    /// <param name="obj">The object with the desired `objref`</param>
    /// <param name="objref">The name of the `objref` member</param>
    /// <param name="timeout">Timeout in milliseconds, or RR_TIMEOUT_INFINITE for no timeout</param>
    /// <returns>A task that upon completion returns the fully qaulified object type</returns>
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

    /// <summary>
    /// Asynchronously returns the type of a service object
    /// </summary>
    /// <remarks>
    /// Same as FindObjectType() but async
    /// </remarks>
    /// <param name="obj">The object with the desired `objref`</param>
    /// <param name="objref">The name of the `objref` member</param>
    /// <param name="index">The index for the `objref`, convert int to string for int32 index type</param>
    /// <param name="timeout">Timeout in milliseconds, or RR_TIMEOUT_INFINITE for no timeout</param>
    /// <returns>A task that upon completion returns the fully qaulified object type</returns>
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

    /// <summary>
    /// Get or set the timeout for requests in milliseconds
    /// </summary>
    /// <remarks>
    /// Requests are calls to a remote node that expect a response. `function`,
    /// `property`, `callback`, `memory`, and setup calls in `pipe` and `wire`
    /// are all requests. All other Robot Raconteur functions that call the remote
    /// node and expect a response are requests. Default timeout is 15 seconds.
    /// </remarks>
    public uint RequestTimeout
    {
        get {
            return _GetRequestTimeout();
        }
        set {
            _SetRequestTimeout(value);
        }
    }

    /// <summary>
    /// Get or set the timeout for transport activity in milliseconds
    /// </summary>
    /// <remarks>
    /// Sets a timeout for transport inactivity. If no message
    /// is sent or received on the transport for the specified time,
    /// the transport is closed. Default timeout is 10 minutes.
    /// </remarks>
    public uint TransportInactivityTimeout
    {
        get {
            return _GetTransportInactivityTimeout();
        }
        set {
            _SetTransportInactivityTimeout(value);
        }
    }

    /// <summary>
    /// Get or set the timeout for endpoint activity in milliseconds
    /// </summary>
    /// <remarks>
    /// Sets a timeout for endpoint inactivity. If no message
    /// is sent or received by the endpoint for the specified time,
    /// the endpoint is closed. Default timeout is 10 minutes.
    /// </remarks>
    public uint EndpointInactivityTimeout
    {
        get {
            return _GetEndpointInactivityTimeout();
        }
        set {
            _SetEndpointInactivityTimeout(value);
        }
    }

    /// <summary>
    /// Get or set the maximum chunk size for memory transfers in bytes
    /// </summary>
    /// <remarks>
    /// `memory` members break up large transfers into chunks to avoid
    /// sending messages larger than the transport maximum, which is normally
    /// approximately 10 MB. The memory max transfer size is the largest
    /// data chunk the memory will send, in bytes. Default is 100 kB.
    /// </remarks>
    public uint MemoryMaxTransferSize
    {
        get {
            return _GetMemoryMaxTransferSize();
        }
        set {
            _SetMemoryMaxTransferSize(value);
        }
    }

    /// <summary>
    /// Get or set the maximum number of detected nodes that will be cached
    /// </summary>
    /// <remarks>
    /// The node keeps a cache of detected nodes. The NodeDiscoveryMaxCacheCount
    /// sets an upper limit to how many detected nodes are cached. By default set to 4096
    /// </remarks>
    public uint NodeDiscoveryMaxCacheCount
    {
        get {
            return _GetNodeDiscoveryMaxCacheCount();
        }
        set {
            _SetNodeDiscoveryMaxCacheCount(value);
        }
    }

    /// <summary>
    /// Shuts down the node. Called automatically by ClientNodeSetup and ServerNodeSetup
    /// </summary>
    /// <remarks>
    /// <para>
    /// Shutdown must be called before program exit to avoid segfaults and other undefined
    /// behavior. The use of ClientNodeSetup and ServerNodeSetup is recommended to automate
    /// the node lifecycle. Calling this function does the following:
    /// </para>
    /// <list type="number">
    /// <item>1. Closes all services and releases all service objects</item>
    /// <item>2. Closes all client connections</item>
    /// <item>3. Shuts down discovery</item>
    /// <item>4. Shuts down all transports</item>
    /// <item>5. Notifies all shutdown listeners</item>
    /// <item>6. Releases all periodic cleanup task listeners</item>
    /// <item>7. Shuts down and releases the thread pool</item>
    /// </list>
    /// <para>
    /// Requires Multithreading
    /// </para>
    /// </remarks>
    public void Shutdown()
    {

        this._Shutdown();
        RRNativeObjectHeapSupport.Set_Support(null);
    }

    /// <summary>
    /// Split a qualified name into its service definition name and unqualified name parts
    /// </summary>
    /// <remarks>None</remarks>
    /// <param name="name">Name to split</param>
    /// <returns>Tulpe containing service definition name and unqualified name</returns>
    public static Tuple<string, string> SplitQualifiedName(string name)
    {
        int pos = name.LastIndexOf('.');
        if (pos == -1)
            throw new ArgumentException("Name is not qualified");
        string[] o = new string[2];
        return Tuple.Create(name.Substring(0, pos), name.Substring(pos + 1, name.Length - pos - 1));
    }

    /// <summary>
    /// Get a service factory for a C# type
    /// </summary>
    /// <remarks>None</remarks>
    /// <param name="type">The C# type</param>
    /// <returns>The service factory</returns>
    public static string GetTypeString(Type type)
    {
        return type.ToString().Replace("_.", ".").TrimEnd(new char[] { '_' });
    }

    /// <summary>
    /// Set an exception handler function
    /// </summary>
    /// <remarks>
    /// The ThreadPool will catch exceptions that are uncaught
    /// by other logic and pass the exception to the specified
    /// exception handler. User handler functions that throw exceptions
    /// will also be caught and passed to the specified handler
    /// function
    /// </remarks>
    /// <param name="handler">The handler function for uncaught exceptions</param>
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

    /// <summary>
    /// Create a Timer object
    /// </summary>
    /// <remarks>
    /// <para>
    /// This function will normally return a WallTimer instance
    /// </para>
    /// <para>
    /// Start() must be called after timer creation
    /// </para>
    /// </remarks>
    /// <param name="period">The period of the timer in milliseconds</param>
    /// <param name="handler">The handler function to call when timer times out</param>
    /// <param name="oneshot">True if timer is a one-shot timer, false for repeated timer</param>
    /// <returns>The new Timer object. Must call Start()</returns>
    public Timer CreateTimer(int period, Action<TimerEvent> handler, bool oneshot = false)
    {
        AsyncTimerEventReturnDirectorImpl t = new AsyncTimerEventReturnDirectorImpl(handler);
        int id = RRObjectHeap.AddObject(t);
        return _CreateTimer(period, oneshot, t, id);
    }

    /// <summary>
    /// Downcasts a RobotRaconteurException
    /// </summary>
    /// <remarks>
    /// Serialized RobotRaconteurException may not be correctly downcast when
    /// deserialized. DownCastException will find the correct type,
    /// downcast the exception, and return the correctly typed exception
    /// </remarks>
    /// <param name="exp">The RobotRaconteurException to downcast</param>
    /// <returns>The downcasted exception</returns>
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

    /// <summary>
    /// Post an Action to be executed by the thread pool
    /// </summary>
    /// <remarks>None</remarks>
    /// <param name="target">The Action to execute</param>
    public void PostToThreadPool(Action target)
    {
        AsyncVoidNoErrReturnDirectorImpl h = new AsyncVoidNoErrReturnDirectorImpl();
        int id = RRObjectHeap.AddObject(h);
        _PostToThreadPool(h, id);
        h.Task.ContinueWith(
            _ => target());
    }

    /// <summary>
    /// Get the current RobotRaconteurVersion as a string
    /// </summary>
    /// <remarks>
    /// Version is three numbers separated by dots, ie "0.9.2"
    /// </remarks>
    public string RobotRaconteurVersion
    {
        get {
            return this._GetRobotRaconteurVersion();
        }
    }
}

}
