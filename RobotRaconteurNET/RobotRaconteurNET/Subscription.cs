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
// Subscriptions

/// <summary>
/// ClientID for use with ServiceSubscription
/// </summary>
/// <remarks>
/// The ServiceSubscriptionClientID stores the NodeID
/// and ServiceName of a connected service.
/// </remarks>
public partial class ServiceSubscriptionClientID
{

    /// <summary>
    /// The NodeID of the connected service
    /// </summary>
    /// <remarks>None</remarks>
    public NodeID NodeID;
    /// <summary>
    /// The ServiceName of the connected service
    /// </summary>
    /// <remarks>None</remarks>
    public string ServiceName;

    /// <summary>
    /// Construct a ServiceSubscriptionClientID
    /// </summary>
    /// <remarks>None</remarks>
    /// <param name="node_id">The NodeID</param>
    /// <param name="service_name">The Service Name</param>
    public ServiceSubscriptionClientID(NodeID node_id, string service_name)
    {
        this.NodeID = new NodeID(node_id);
        this.ServiceName = service_name;
    }

    internal ServiceSubscriptionClientID(WrappedServiceSubscriptionClientID id1)
    {
        this.NodeID = new NodeID(id1.NodeID);
        this.ServiceName = id1.ServiceName;
    }

    /// <summary>
    /// Equality operator
    /// </summary>
    /// <remarks>None</remarks>
    public static bool operator ==(ServiceSubscriptionClientID a, ServiceSubscriptionClientID b)
    {
        if (Object.Equals(a, null) && Object.Equals(b, null))
            return true;
        if (Object.Equals(a, null) || Object.Equals(b, null))
            return false;
        return (a.NodeID == b.NodeID) && (a.ServiceName == b.ServiceName);
    }

    /// <summary>
    /// Inequality operator
    /// </summary>
    /// <remarks>None</remarks>
    public static bool operator !=(ServiceSubscriptionClientID a, ServiceSubscriptionClientID b)
    {
        return !(a == b);
    }

    public override bool Equals(object o)
    {
        if (!(o is ServiceSubscriptionClientID))
            return false;
        return this == ((ServiceSubscriptionClientID)o);
    }

    public override int GetHashCode()
    {
        return (ServiceName?.GetHashCode() ?? 0) + (NodeID?.ToString().GetHashCode() ?? 0);
    }
}

/// <summary>
/// Subscription filter node information
/// </summary>
/// <remarks>
/// Specify a node by NodeID and/or NodeName. Also allows specifying
/// username and password.
///
/// When using username and credentials, secure transports and specified NodeID should
/// be used. Using username and credentials without a transport that verifies the
/// NodeID could result in credentials being leaked.
/// </remarks>
public class ServiceSubscriptionFilterNode
{
    /// <summary>
    /// The NodeID to match. All zero NodeID will match any NodeID.
    /// </summary>
    /// <remarks>None</remarks>
    public NodeID NodeID = RobotRaconteur.NodeID.GetAny();
    /// <summary>
    /// The NodeName to match. Empty or null NodeName will match any NodeName.
    /// </summary>
    /// <remarks>None</remarks>
    public string NodeName;
    /// <summary>
    /// The username to use for authentication. Should only be used with secure transports and verified NodeID
    /// </summary>
    /// <remarks>None</remarks>
    public string Username;
    /// <summary>
    /// The credentials to use for authentication. Should only be used with secure transports and verified NodeID
    /// </summary>
    /// <remarks>None</remarks>
    public Dictionary<string, object> Credentials;
}

/// <summary>
/// Subscription filter
/// </summary>
/// <remarks>
/// The subscription filter is used with RobotRaconteurNode.SubscribeServiceByType() and
/// RobotRaconteurNode::SubscribeServiceInfo2() to decide which services should
/// be connected. Detected services that match the service type are checked against
/// the filter before connecting.
/// </remarks>
public class ServiceSubscriptionFilter
{
    /// <summary>
    /// Vector of nodes that should be connected. Empty means match any node.
    /// </summary>
    /// <remarks>None</remarks>
    public ServiceSubscriptionFilterNode[] Nodes;
    /// <summary>
    /// Vector service names that should be connected. Empty means match any service name.
    /// </summary>
    /// <remarks>None</remarks>
    public string[] ServiceNames;
    /// <summary>
    /// Vector of transport schemes. Empty means match any transport scheme.
    /// </summary>
    /// <remarks>None</remarks>
    public string[] TransportSchemes;
    /// <summary>
    /// Attributes to match
    /// </summary>
    /// <remarks>None</remarks>
    public Dictionary<string, ServiceSubscriptionFilterAttributeGroup> Attributes;
    /// <summary>
    /// Operation to use to match attributes. Defaults to AND
    /// </summary>
    public ServiceSubscriptionFilterAttributeGroupOperation AttributesMatchOperation;

    /// <summary>
    /// A user specified predicate function. If nullptr, the predicate is not checked.
    /// </summary>
    /// <remarks>None</remarks>
    public Func<ServiceInfo2, bool> Predicate;
    /// <summary>
    /// The maximum number of connections the subscription will create. Zero means unlimited connections.
    /// </summary>
    /// <remarks>None</remarks>
    public uint MaxConnections;
}

/// <summary>
/// Subscription for information about detected services
/// </summary>
/// <remarks>
/// <para>
/// Created using RobotRaconteurNode::SubscribeServiceInfo2()
/// </para>
/// <para>
/// The ServiceInfo2Subscription class is used to track services with a specific service type as they are
/// detected on the local network and when they are lost. The currently detected services can also
/// be retrieved. The service information is returned using the ServiceInfo2 structure.
/// </para>
/// </remarks>
public class ServiceInfo2Subscription
{
    class WrappedServiceInfo2SubscriptionDirectorNET : WrappedServiceInfo2SubscriptionDirector
    {
        WeakReference subscription1;

        internal WrappedServiceInfo2SubscriptionDirectorNET(ServiceInfo2Subscription subscription)
        {
            subscription1 = new WeakReference(subscription);
        }

        public override void ServiceDetected(WrappedServiceInfo2Subscription subscription,
                                             WrappedServiceSubscriptionClientID id, ServiceInfo2Wrapped info)
        {
            var s = (ServiceInfo2Subscription)subscription1.Target;
            if (s == null)
                return;
            var info1 = new ServiceInfo2(info);
            var id1 = new ServiceSubscriptionClientID(id);
            if (s.ServiceDetected == null)
                return;
            try
            {
                s.ServiceDetected(s, id1, info1);
            }
            catch
            {}
        }

        public override void ServiceLost(WrappedServiceInfo2Subscription subscription,
                                         WrappedServiceSubscriptionClientID id, ServiceInfo2Wrapped info)
        {
            var s = (ServiceInfo2Subscription)subscription1.Target;
            if (s == null)
                return;
            var info1 = new ServiceInfo2(info);
            var id1 = new ServiceSubscriptionClientID(id);
            if (s.ServiceLost == null)
                return;
            try
            {
                s.ServiceLost(s, id1, info1);
            }
            catch
            {}
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

    /// <summary>
    /// Close the subscription
    /// </summary>
    /// <remarks>
    /// Closes the subscription. Subscriptions are automatically closed when the node is shut down.
    /// </remarks>
    public void Close()
    {
        _subscription.Close();
    }

    /// <summary>
    /// Returns a dictionary of detected services.
    /// </summary>
    /// <remarks>
    /// The returned dictionary contains the detected nodes as ServiceInfo2. The map
    /// is keyed with ServiceSubscriptionClientID.
    ///
    /// This function does not block.
    /// </remarks>
    /// <returns>The detected services</returns>
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

    /// <summary>
    /// Listener event that is invoked when a service is detected
    /// </summary>
    /// <remarks>None</remarks>
    public event Action<ServiceInfo2Subscription, ServiceSubscriptionClientID, ServiceInfo2> ServiceDetected;
    /// <summary>
    /// Listener event that is invoked when a service is lost
    /// </summary>
    /// <remarks>None</remarks>
    public event Action<ServiceInfo2Subscription, ServiceSubscriptionClientID, ServiceInfo2> ServiceLost;
}

/// <summary>
/// Subscription that automatically connects services and manages lifecycle of connected services
/// </summary>
/// <remarks>
/// <para>
/// Created using RobotRaconteurNode.SubscribeService() or RobotRaconteurNode.SubscribeServiceByType(). The
/// ServiceSubscription class is used to automatically create and manage connections based on connection criteria.
/// RobotRaconteurNode.SubscribeService() is used to create a robust connection to a service with a specific URL.
/// RobotRaconteurNode.SubscribeServiceByType() is used to connect to services with a specified type, filtered with a
/// ServiceSubscriptionFilter. Subscriptions will create connections to matching services, and will retry the connection
/// if it fails or the connection is lost. This behavior allows subscriptions to be used to create robust connections.
/// The retry delay for connections can be modified using ConnectRetryDelay.
/// </para>
/// <para>
/// The currently connected clients can be retrieved using the GetConnectedClients() function. A single "default client"
/// can be retrieved using the GetDefaultClient() function or TryGetDefaultClient() functions. Listeners for client
/// connect and disconnect events can be added  using the AddClientConnectListener() and AddClientDisconnectListener()
/// functions. If the user wants to claim a client, the ClaimClient() and ReleaseClient() functions will be used.
/// Claimed clients will no longer have their lifecycle managed by the subscription.
/// </para>
/// <para>
/// Subscriptions can be used to create `pipe` and `wire` subscriptions. These member subscriptions aggregate
/// the packets and values being received from all services. They can also act as a "reverse broadcaster" to
/// send packets and values to all services that are actively connected. See PipeSubscription and WireSubscription.
/// </para>
/// </remarks>
public class ServiceSubscription
{

    class WrappedServiceSubscriptionDirectorNET : WrappedServiceSubscriptionDirector
    {
        WeakReference subscription1;

        internal WrappedServiceSubscriptionDirectorNET(ServiceSubscription subscription)
        {
            subscription1 = new WeakReference(subscription);
        }

        public override void ClientConnected(WrappedServiceSubscription subscription,
                                             WrappedServiceSubscriptionClientID id, WrappedServiceStub client)
        {
            var s = (ServiceSubscription)subscription1.Target;
            if (s == null)
                return;
            if (s.ClientConnected == null)
                return;

            var client2 = s.GetClientStub(client);
            try
            {
                s.ClientConnected(s, new ServiceSubscriptionClientID(id), client2);
            }
            catch
            {}
        }

        public override void ClientDisconnected(WrappedServiceSubscription subscription,
                                                WrappedServiceSubscriptionClientID id, WrappedServiceStub client)
        {
            var s = (ServiceSubscription)subscription1.Target;
            if (s == null)
                return;
            if (s.ClientDisconnected == null)
                return;

            var client2 = s.DeleteClientStub(client);

            try
            {
                s.ClientDisconnected(s, new ServiceSubscriptionClientID(id), client2);
            }
            catch
            {}
        }

        public override void ClientConnectFailed(WrappedServiceSubscription subscription,
                                                 WrappedServiceSubscriptionClientID id, vectorstring url,
                                                 HandlerErrorInfo err)
        {
            var s = (ServiceSubscription)subscription1.Target;
            if (s == null)
                return;
            if (s.ClientConnectFailed == null)
                return;

            s.ClientConnectFailed(s, new ServiceSubscriptionClientID(id), url.ToArray(),
                                  RobotRaconteurExceptionUtil.ErrorInfoToException(err));
        }
    }

    Dictionary<int, object> client_stubs = new Dictionary<int, object>();

    internal object GetClientStub(WrappedServiceStub innerstub)
    {
        if (innerstub == null)
            return null;

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
                if (id2 == 0)
                    return null;
                client_stubs.Add(id2, stub);
                return stub;
            }
        }
    }

    internal object DeleteClientStub(WrappedServiceStub innerstub)
    {
        if (innerstub == null)
            return null;

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

    /// <summary>
    /// Returns a dictionary of connected clients
    /// </summary>
    /// <remarks>
    /// <para>
    /// The returned dictionary contains the connect clients. The map
    /// is keyed with ServiceSubscriptionClientID.
    /// </para>
    /// <para>
    /// Clients must be cast to a type, similar to the client returned by
    /// RobotRaconteurNode.ConnectService().
    /// </para>
    /// <para>
    /// Clients can be "claimed" using ClaimClient(). Once claimed, the subscription
    /// will stop managing the lifecycle of the client.
    /// </para>
    /// <para>
    /// This function does not block.
    /// </para>
    /// </remarks>
    /// <returns>The detected services.</returns>
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

    /// <summary>
    /// Close the subscription
    /// </summary>
    /// <remarks>
    /// Close the subscription. Subscriptions are automatically closed when the node is shut down.
    /// </remarks>
    public void Close()
    {
        _subscription.Close();
    }

    /// <summary>
    /// Claim a client that was connected by the subscription
    /// </summary>
    /// <remarks>
    /// The subscription class will automatically manage the lifecycle of the connected clients. The clients
    /// will be automatically disconnected and/or reconnected as necessary. If the user wants to disable
    /// this behavior for a specific client connection, the client connection can be "claimed".
    /// </remarks>
    /// <param name="client">The client to be claimed</param>
    public void ClaimClient(object client)
    {
        ServiceStub s = client as ServiceStub;
        if (s == null)
            throw new ArgumentException("Invalid object for ClaimClient");

        _subscription.ClaimClient(s.rr_innerstub);
    }

    /// <summary>
    /// Release a client previously claimed with ClaimClient()
    /// </summary>
    /// <remarks>
    /// Lifecycle management is returned to the subscription
    /// </remarks>
    /// <param name="client">The client to release claim</param>
    public void ReleaseClient(object client)
    {
        ServiceStub s = client as ServiceStub;
        if (s == null)
            throw new ArgumentException("Invalid object for ReleaseClient");

        _subscription.ClaimClient(s.rr_innerstub);
    }

    /// <summary>
    /// Get or set the connect retry delay in milliseconds
    /// </summary>
    /// <remarks>
    /// Default connect retry delay is 2.5 seconds
    /// </remarks>
    /// <value></value>
    public uint ConnectRetryDelay
    {
        get {
            return _subscription.GetConnectRetryDelay();
        }
        set {
            _subscription.SetConnectRetryDelay(value);
        }
    }

    /// <summary>
    /// Event listener for when a client connects
    /// </summary>
    /// <remarks>None</remarks>
    public event Action<ServiceSubscription, ServiceSubscriptionClientID, object> ClientConnected;
    /// <summary>
    /// Event listener for when a client disconnects
    /// </summary>
    /// <remarks>None</remarks>
    public event Action<ServiceSubscription, ServiceSubscriptionClientID, object> ClientDisconnected;

    /// <summary>
    /// Event listener for when a client connection attempt fails. Use to diagnose connection problems
    /// </summary>
    /// <remarks>None</remarks>
    public event Action<ServiceSubscription, ServiceSubscriptionClientID, string[], Exception> ClientConnectFailed;

    /// <summary>
    /// Creates a wire subscription
    /// </summary>
    /// <remarks>
    /// <para>
    /// Wire subscriptions aggregate the value received from the connected services. It can also act as a
    /// "reverse broadcaster" to send values to clients. See WireSubscription.
    /// </para>
    /// <para>
    /// The optional service path may be an empty string to use the root object in the service. The first level of the
    /// service path may be "*" to match any service name. For instance, the service path "*.sub_obj" will match
    /// any service name, and use the "sub_obj" objref.
    /// </para>
    /// </remarks>
    /// <param name="wire_name">The member name of the wire</param>
    /// <param name="service_path">The service path of the object owning the wire member.
    ///     Leave as empty string for root object</param>
    /// <typeparam name="T">The type of the wire value. This must be specified since the subscription doesn't
    /// know the wire value type</typeparam>
    /// <returns>The wire subscription</returns>
    public WireSubscription<T> SubscribeWire<T>(string wire_name, string service_path = "")
    {
        var s = _subscription.SubscribeWire(wire_name, service_path);
        return new WireSubscription<T>(s);
    }

    /// <summary>
    /// Creates a pipe subscription
    /// </summary>
    /// <remarks>
    /// <para>
    /// Pipe subscriptions aggregate the packets received from the connected services. It can also act as a
    /// "reverse broadcaster" to send packets to clients. See PipeSubscription.
    /// </para>
    /// <para>
    /// The optional service path may be an empty string to use the root object in the service. The first level of the
    /// service path may be "*" to match any service name. For instance, the service path "*.sub_obj" will match
    /// any service name, and use the "sub_obj" objref.
    /// </para>
    /// </remarks>
    /// <param name="pipe_name">The member name of the pipe</param>
    /// <param name="service_path">The service path of the object owning the pipe member.
    ///  Leave as empty string for root object</param>
    /// <param name="max_backlog">The maximum number of packets to store in receive queue</param>
    /// <typeparam name="T">The type of the pipe packets. This must be specified since the subscription does not
    /// know the pipe packet type</typeparam>
    /// <returns>The pipe subscription</returns>
    public PipeSubscription<T> SubscribePipe<T>(string pipe_name, string service_path = "", int max_backlog = -1)
    {
        var s = _subscription.SubscribePipe(pipe_name, service_path, max_backlog);
        return new PipeSubscription<T>(s);
    }

    /// <summary>
    /// Creates a sub object subscription
    /// </summary>
    /// <remarks>
    /// <para>
    /// Sub objects are objects within a service that are not the root object. Sub objects are typically
    /// referenced using objref members, however they can also be referenced using a service path.
    /// The SubObjectSubscription class is used to automatically access sub objects of the default client.
    /// </para>
    /// <para>
    /// The service path is broken up into segments using periods. See the Robot Raconter
    /// documentation for more information. The BuildServicePath() function can be used to assist
    /// building service paths. The first level of the* service path may be "*" to match any service name.
    /// For instance, the service path "*.sub_obj" will match any service name, and use the "sub_obj" objref
    /// </para>
    /// </remarks>
    /// <param name="servicepath">The service path of the object</param>
    /// <param name="objecttype">Optional object type to use for the sub object</param>
    /// <returns>The sub object subscription</returns>
    public SubObjectSubscription SubscribeSubObject(string servicepath, string objecttype = "")
    {
        var s = _subscription.SubscribeSubObject(servicepath, objecttype);
        return new SubObjectSubscription(this, s);
    }

    /// <summary>
    /// Get the "default client" connection
    /// </summary>
    /// <remarks>
    /// <para>
    /// The "default client" is the "first" client returned from the connected clients map. This is effectively
    /// default, and is only useful if only a single client connection is expected. This is normally true
    /// for RobotRaconteurNode.SubscribeService()
    /// </para>
    /// <para>
    /// Clients using GetDefaultClient() should not store a reference to the client. It should instead
    /// call GetDefaultClient() right before using the client to make sure the most recenty connection
    /// is being used. If possible, SubscribePipe() or SubscribeWire() should be used so the lifecycle
    /// of pipes and wires can be managed automatically.
    /// </para>
    /// </remarks>
    /// <returns>The client connection. Cast to expected object type</returns>
    public object GetDefaultClient()
    {
        var s = _subscription.GetDefaultClient();
        return GetClientStub(s);
    }

    /// <summary>
    /// Try getting the "default client" connection
    /// </summary>
    /// <remarks>
    /// Same as GetDefaultClient(), but returns a bool success instead of throwing
    /// exceptions on failure.
    /// </remarks>
    /// <param name="obj">[out] The client connection</param>
    /// <returns>true if client object is valid, false otherwise</returns>
    public bool TryGetDefaultClient(out object obj)
    {
        var res = _subscription.TryGetDefaultClient();
        if (!res.res)
        {
            obj = null;
            return false;
        }

        var s = res.client;
        obj = GetClientStub(s);
        return true;
    }

    /// <summary>
    /// Get the "default client" connection, waiting with timeout if not connected
    /// </summary>
    /// <remarks>
    /// <para>
    /// The "default client" is the "first" client returned from the connected clients map. This is effectively
    /// default, and is only useful if only a single client connection is expected. This is normally true
    /// for RobotRaconteurNode.SubscribeService()
    /// </para>
    /// <para>
    /// Clients using GetDefaultClient() should not store a reference to the client. It should instead
    /// call GetDefaultClient() right before using the client to make sure the most recently connection
    /// is being used. If possible, SubscribePipe() or SubscribeWire() should be used so the lifecycle
    /// of pipes and wires can be managed automatically.
    /// </para>
    /// </remarks>
    /// <param name="timeout">Timeout in milliseconds, or RR_TIMEOUT_INFINITE for no timeout</param>
    /// <returns>The client connection. Cast to expected object type</returns>
    public object GetDefaultClientWait(int timeout = -1)
    {
        var s = _subscription.GetDefaultClientWait(timeout);
        return GetClientStub(s);
    }

    /// <summary>
    /// Try getting the "default client" connection, waiting with timeout if not connected
    /// </summary>
    /// <remarks>
    /// Same as GetDefaultClientWait(), but returns a bool success instead of throwing
    /// exceptions on failure.
    /// </remarks>
    /// <param name="obj">[out] The client connection</param>
    /// <param name="timeout">Timeout in milliseconds, or RR_TIMEOUT_INFINITE for no timeout</param>
    /// <returns>true if client object is valid, false otherwise</returns>
    public bool TryGetDefaultClientWait(out object obj, int timeout = -1)
    {
        var res = _subscription.TryGetDefaultClientWait(timeout);
        if (!res.res)
        {
            obj = null;
            return false;
        }

        var s = res.client;
        obj = GetClientStub(s);
        return true;
    }

    /// <summary>
    /// Asynchronously get the default client, with optional timeout
    /// </summary>
    /// <remarks>
    /// Same as GetDefaultClientWait(), but async.
    /// </remarks>
    /// <param name="timeout">Timeout in milliseconds, or RR_TIMEOUT_INFINITE for no timeout</param>
    /// <returns>Task that upon completion returns the client object</returns>
    public async Task<object> AsyncGetDefaultClient(int timeout = -1)
    {
        AsyncStubReturnDirectorImpl<object> h = new AsyncStubReturnDirectorImpl<object>(null);
        int id = RRObjectHeap.AddObject(h);
        _subscription.AsyncGetDefaultClient(timeout, h, id);
        return await h.Task;
    }

    /// <summary>
    /// Get the service connection URL
    /// </summary>
    /// <remarks>
    /// Returns the service connection URL. Only valid when subscription was created using
    /// RobotRaconteurNode.SubscribeService(). Will throw an exception if subscription
    /// was opened using RobotRaconteurNode.SubscribeServiceByType()
    /// </remarks>
    public string[] ServiceURL
    {
        get {
            return _subscription.GetServiceURL().ToArray();
        }
    }

    /// <summary>
    /// Update the service connection URL
    /// </summary>
    /// <remarks>
    /// Updates the URL used to connect to the service. If close_connected is true,
    /// existing connections will be closed. If false,
    /// existing connections will not be closed.
    /// </remarks>
    /// <param name="url">The new URL to use to connect to service</param>
    /// <param name="username">(Optional) The new username</param>
    /// <param name="credentials">(Optional) The new credentials</param>
    /// <param name="objecttype">(Optional) The desired root object proxy type. Optional but highly recommended.</param>
    /// <param name="close_connected">(Optional, default false) Close existing connections</param>
    public void UpdateServiceURL(string[] url, string username = null, Dictionary<string, object> credentials = null,
                                 string objecttype = null, bool close_connected = false)
    {
        MessageElementData credentials2 = null;
        try
        {
            if (username == null)
                username = "";
            if (credentials != null)
            {
                credentials2 = (MessageElementData)RobotRaconteurNode.s.PackVarType(credentials);
            }

            if (objecttype == null)
                objecttype = "";
            vectorstring url2 = new vectorstring();
            foreach (string ss in url)
                url2.Add(ss);

            _subscription.UpdateServiceURL(url2, username, credentials2, objecttype, close_connected);
        }
        finally
        {
            if (credentials2 != null)
                credentials2.Dispose();
        }
    }

    public void UpdateServiceByType(string[] service_types, ServiceSubscriptionFilter filter = null)
    {
        var filter2 = RobotRaconteurNode.s.SubscribeService_LoadFilter(filter);

        var service_types2 = new vectorstring();
        foreach (string s in service_types)
            service_types2.Add(s);

        this._subscription.UpdateServiceByType(service_types2, filter2);
    }

    /// <summary>
    /// Update the service connection URL
    /// </summary>
    /// <remarks>
    /// Updates the URL used to connect to the service. If close_connected is true,
    /// existing connections will be closed. If false,
    /// existing connections will not be closed.
    /// </remarks>
    /// <param name="url">The new URL to use to connect to service</param>
    /// <param name="username">(Optional) The new username</param>
    /// <param name="credentials">(Optional) The new credentials</param>
    /// <param name="objecttype">(Optional) The desired root object proxy type. Optional but highly recommended.</param>
    /// <param name="close_connected">(Optional, default false) Close existing connections</param>
    public void UpdateServiceURL(string url, string username = null, Dictionary<string, object> credentials = null,
                                 string objecttype = null, bool close_connected = false)
    {
        MessageElementData credentials2 = null;
        try
        {
            if (username == null)
                username = "";
            if (credentials != null)
            {
                credentials2 = (MessageElementData)RobotRaconteurNode.s.PackVarType(credentials);
            }

            if (objecttype == null)
                objecttype = "";

            _subscription.UpdateServiceURL(url, username, credentials2, objecttype, close_connected);
        }
        finally
        {
            if (credentials2 != null)
                credentials2.Dispose();
        }
    }
}

/// <summary>
/// Subscription for wire members that aggregates the values from client wire connections
/// </summary>
/// <remarks>
/// <para>
/// Wire subscriptions are created using the ServiceSubscription.SubscribeWire() function. This function takes the
/// type of the wire value, the name of the wire member, and an optional service path of the service
/// object that owns the wire member.
/// </para>
/// <para>
/// Wire subscriptions aggregate the InValue from all active wire connections. When a client connects,
/// the wire subscriptions will automatically create wire connections to the wire member specified
/// when the WireSubscription was created using ServiceSubscription::SubscribeWire(). The InValue of
/// all the active wire connections are collected, and the most recent one is used as the current InValue
/// of the wire subscription. The current value, the timespec, and the wire connection can be accessed
/// using GetInValue() or TryGetInValue().
/// </para>
/// <para>
/// The lifespan of the InValue can be configured using SetInValueLifespan(). It is recommended that
/// the lifespan be configured, so that the value will expire if the subscription stops receiving
/// fresh in values.
/// </para>
/// <para>
/// The wire subscription can also be used to set the OutValue of all active wire connections. This behaves
/// similar to a "reverse broadcaster", sending the same value to all connected services.
/// </para>
/// </remarks>
/// <typeparam name="T">The value type used by the wire</typeparam>
public class WireSubscription<T>
{
    class WrappedWireSubscriptionDirectorNET : WrappedWireSubscriptionDirector
    {
        WeakReference subscription1;

        internal WrappedWireSubscriptionDirectorNET(WireSubscription<T> subscription)
        {
            subscription1 = new WeakReference(subscription);
        }

        public override void WireValueChanged(WrappedWireSubscription subscription, WrappedService_typed_packet value,
                                              TimeSpec time)
        {
            using (value)
            {
                var s = (WireSubscription<T>)subscription1.Target;
                if (s == null)
                    return;
                if (s.WireValueChanged == null)
                    return;
                try
                {
                    using (var m1 = value.packet)
                    {
                        var v = RobotRaconteurNode.s.UnpackAnyType<T>(m1);

                        s.WireValueChanged(s, v, time);
                    }
                }
                catch
                {}
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

    /// <summary>
    /// Get the current InValue
    /// </summary>
    /// <remarks>
    /// Throws ValueNotSetException if no valid value is available
    /// </remarks>
    public T InValue
    {
        get {
            var m = _subscription.GetInValue();
            using (m)
            {
                var m1 = m.packet;
                using (m1)
                {
                    return RobotRaconteurNode.s.UnpackAnyType<T>(m1);
                }
            }
        }
    }

    /// <summary>
    /// Get the current InValue and metadata
    /// </summary>
    /// <remarks>
    /// Throws ValueNotSetException if no valid value is available
    /// </remarks>
    /// <param name="time">[out] the LastValueReceivedTime of the InValue</param>
    /// <returns>The current InValue</returns>
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
                return RobotRaconteurNode.s.UnpackAnyType<T>(m1);
            }
        }
    }

    /// <summary>
    /// Try getting the current InValue
    /// </summary>
    /// <remarks>
    /// Same as InValue, but returns a bool for success or failure instead of throwing
    /// an exception.
    /// </remarks>
    /// <param name="value">[out] the current InValue</param>
    /// <returns>true if value is valid, otherwise false</returns>
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
                    value = RobotRaconteurNode.s.UnpackAnyType<T>(m1);
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

    /// <summary>
    /// Try getting the current InValue and metadata
    /// </summary>
    /// <remarks>
    /// Same as GetInValue(), but returns a bool for success or failure instead of throwing
    /// an exception.
    /// </remarks>
    /// <param name="value">[out] the current InValue</param>
    /// <param name="time">[out] the LastValueReceivedTime of the InValue</param>
    /// <returns>true if value is valid, otherwise false</returns>
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
                    value = RobotRaconteurNode.s.UnpackAnyType<T>(m1);
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

    /// <summary>
    /// Wait for a valid InValue to be received from a client
    /// </summary>
    /// <remarks>
    /// Blocks the current thread until value is received or timeout
    /// </remarks>
    /// <param name="timeout">The timeout in milliseconds</param>
    /// <returns>true if value is valid, otherwise false</returns>
    public bool WaitInValueValid(int timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE)
    {
        return _subscription.WaitInValueValid(timeout);
    }

    /// <summary>
    /// Get or Set if InValue is ignored
    /// </summary>
    /// <remarks></remarks>
    /// <value></value>
    public bool IgnoreInValue
    {
        get {
            return _subscription.GetIgnoreInValue();
        }
        set {
            _subscription.SetIgnoreInValue(value);
        }
    }

    /// <summary>
    /// Get or Set the InValue lifespan in milliseconds
    /// </summary>
    /// <remarks>
    /// Get the lifespan of InValue in milliseconds. The value will expire after the specified
    /// lifespan, becoming invalid. Use -1 for infinite lifespan.
    /// </remarks>
    public int InValueLifespan
    {
        get {
            return _subscription.GetInValueLifespan();
        }
        set {
            _subscription.SetInValueLifespan(value);
        }
    }

    /// <summary>
    /// Set the OutValue for all active wire connections
    /// </summary>
    /// <remarks>
    /// Behaves like a "reverse broadcaster". Calls WireConnection.SetOutValue()
    /// for all connected wire connections.
    /// </remarks>
    /// <param name="value">The new OutValue</param>
    public void SetOutValueAll(T value)
    {
        using (var iter = new WrappedWireSubscription_send_iterator(_subscription))
        {

            while (iter.Next() != null)
            {
                object dat = null;
                try
                {
                    using (MessageElement m = RobotRaconteurNode.s.PackAnyType<T>("value", ref value))
                    {
                        iter.SetOutValue(m);
                    }
                }
                finally
                {
                    IDisposable d = dat as IDisposable;
                    if (d != null)
                        d.Dispose();
                }
            }
        }
    }

    /// <summary>
    /// Get the number of wire connections currently connected
    /// </summary>
    /// <remarks>None</remarks>
    public uint ActiveWireConnectionCount
    {
        get {
            return (uint)_subscription.GetActiveWireConnectionCount();
        }
    }

    /// <summary>
    /// Closes the wire subscription
    /// </summary>
    /// <remarks>
    /// Wire subscriptions are automatically closed when the parent ServiceSubscription is closed
    /// or when the node is shut down.
    /// </remarks>
    public void Close()
    {
        _subscription.Close();
    }

    public event Action<WireSubscription<T>, T, TimeSpec> WireValueChanged;
}

/// <summary>
/// Subscription for pipe members that aggregates incoming packets from client pipe endpoints
/// </summary>
/// <remarks>
/// <para>
/// Pipe subscriptions are created using the ServiceSubscription.SubscribePipe() function. This function takes the
/// the type of the pipe packets, the name of the pipe member, and an optional service path of the service
/// object that owns the pipe member.
/// </para>
/// <para>
/// Pipe subscriptions collect all incoming packets from connect pipe endpoints. When a client connects,
/// the pipe subscription will automatically connect a pipe endpoint the pipe endpoint specified when
/// the PipeSubscription was created using ServiceSubscription.SubscribePipe(). The packets received
/// from each of the collected pipes are collected and placed into a common receive queue. This queue
/// is read using ReceivePacket(), TryReceivePacket(), or TryReceivePacketWait(). The number of packets
/// available to receive can be checked using Available().
/// </para>
/// <para>
/// Pipe subscriptions can also be used to send packets to all connected pipe endpoints. This is done
/// with the AsyncSendPacketAll() function. This function behaves somewhat like a "reverse broadcaster",
/// sending the packets to all connected services.
/// </para>
/// <para>
/// If the pipe subscription is being used to send packets but not receive them, the SetIgnoreInValue()
/// should be set to true to prevent packets from queueing.
/// </para>
/// </remarks>
/// <typeparam name="T">The type of the pipe packets</typeparam>
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
            if (s == null)
                return;
            if (s.PipePacketReceived == null)
                return;
            try
            {
                s.PipePacketReceived(s);
            }
            catch
            {}
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

    /// <summary>
    /// Dequeue a packet from the receive queue
    /// </summary>
    /// <remarks>
    /// If the receive queue is empty, an InvalidOperationException() is thrown
    /// </remarks>
    /// <returns>The dequeued packet</returns>
    public T ReceivePacket()
    {
        using (var m = _subscription.ReceivePacket())
        {
            using (var m1 = m.packet)
            {
                return RobotRaconteurNode.s.UnpackAnyType<T>(m1);
            }
        }
    }

    /// <summary>
    /// Try dequeuing a packet from the receive queue
    /// </summary>
    /// <remarks>
    /// Same as ReceivePacket(), but returns a bool for success or failure instead of throwing
    /// an exception
    /// </remarks>
    /// <param name="packet">[out] the dequeued packet</param>
    /// <returns>true if packet dequeued successfully, otherwise false if queue is empty</returns>
    public bool TryReceivePacket(out T packet)
    {
        using (var m = new WrappedService_typed_packet())
        {

            if (_subscription.TryReceivePacket(m))
            {
                using (var m1 = m.packet)
                {

                    packet = RobotRaconteurNode.s.UnpackAnyType<T>(m1);
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

    /// <summary>
    /// Try dequeuing a packet from the receive queue, optionally waiting or peeking the packet
    /// </summary>
    /// <remarks>None</remarks>
    /// <param name="packet">[out] the dequeued packet</param>
    /// <param name="timeout">The time to wait for a packet to be received in milliseconds if the queue is empty, or
    /// RR_TIMEOUT_INFINITE to wait forever</param>
    /// <param name="peek">If true, the packet is returned, but not dequeued. If false, the packet is dequeued</param>
    /// <returns>true if packet dequeued successfully, otherwise false if queue is empty or timed out</returns>
    public bool TryReceivePacketWait(out T packet, int timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE,
                                     bool peek = false)
    {
        using (var m = new WrappedService_typed_packet())
        {
            if (_subscription.TryReceivePacketWait(m, timeout, peek))
            {
                using (var m1 = m.packet)
                {

                    packet = RobotRaconteurNode.s.UnpackAnyType<T>(m1);
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

    /// <summary>
    /// Get the number of packets available to receive
    /// </summary>
    /// <remarks>
    /// Use ReceivePacket(), TryReceivePacket(), or TryReceivePacketWait() to receive the packet
    /// </remarks>
    public uint Available
    {
        get {
            return _subscription.Available();
        }
    }

    /// <summary>
    /// Get or set if incoming packets are ignored
    /// </summary>
    /// <remarks>None</remarks>
    public bool IgnoreReceived
    {
        get {
            return _subscription.GetIgnoreReceived();
        }
        set {
            _subscription.SetIgnoreReceived(value);
        }
    }

    /// <summary>
    /// Sends a packet to all connected pipe endpoints
    /// </summary>
    /// <remarks>
    /// Calls AsyncSendPacket() on all connected pipe endpoints with the specified value.
    /// Returns immediately, not waiting for transmission to complete.
    /// </remarks>
    /// <param name="value">The packet to send</param>
    public void AsyncSendPacketAll(T value)
    {
        using (var iter = new WrappedPipeSubscription_send_iterator(_subscription))
        {

            while (iter.Next() != null)
            {
                object dat = null;
                try
                {
                    using (MessageElement m = RobotRaconteurNode.s.PackAnyType<T>("value", ref value))
                    {
                        iter.AsyncSendPacket(m);
                    }
                }
                finally
                {
                    IDisposable d = dat as IDisposable;
                    if (d != null)
                        d.Dispose();
                }
            }
        }
    }

    /// <summary>
    /// Get the number of pipe endpoints currently connected
    /// </summary>
    /// <remarks>None</remarks>
    public uint ActivePipeEndpointCount
    {
        get {
            return _subscription.GetActivePipeEndpointCount();
        }
    }

    /// <summary>
    /// Closes the pipe subscription
    /// </summary>
    /// <remarks>
    /// Pipe subscriptions are automatically closed when the parent ServiceSubscription is closed
    /// or when the node is shut down.
    /// </remarks>
    public void Close()
    {
        _subscription.Close();
    }
    /// <summary>
    /// Listener event for when a pipe packet is received
    /// </summary>
    /// <remarks>None</remarks>
    public event Action<PipeSubscription<T>> PipePacketReceived;
}

/// <summary>
/// Subscription for sub objects of the default client.
/// </summary>
/// <remarks>
/// <para>SubObjectSubscription is used to access sub objects of the default client. Sub objects are objects within a
/// service that are not the root object. Sub objects are typically referenced using objref members, however they can
/// also be referenced using a service path. The SubObjectSubscription class is used to automatically access sub objects
/// of the default client.</para>
///
/// <para>Use ServiceSubscription.SubscribeSubObject() to create a SubObjectSubscription.</para>
///
/// <para>This class should not be used to access Pipe or Wire members. Use the ServiceSubscription.SubscribePipe() and
/// ServiceSubscription.SubscribeWire() functions to access Pipe and Wire members.</para>
/// </remarks>
public class SubObjectSubscription
{
    WrappedSubObjectSubscription _subscription;
    ServiceSubscription _parent;

    internal SubObjectSubscription(ServiceSubscription parent, WrappedSubObjectSubscription subscription)
    {
        _subscription = subscription;
        _parent = parent;
    }

    /// <summary>
    /// Closes the sub object subscription.
    /// </summary>
    /// <remarks>
    /// <para>Sub object subscriptions are automatically closed when the parent ServiceSubscription is closed
    /// or when the node is shut down.</para>
    /// </remarks>
    public void Close()
    {
        _subscription.Close();
    }
    /// <summary>
    /// Get the "default client" sub object.
    /// </summary>
    /// <remarks>
    /// <para>The sub object is retrieved from the default client. The default client is the first client
    /// that connected to the service. If no clients are currently connected, an exception is thrown.</para>
    ///
    /// <para>Clients using GetDefaultClient() should not store a reference to the client. Call GetDefaultClient()
    /// each time the client is needed.</para>
    /// </remarks>
    /// <returns>The sub object.</returns>
    public object GetDefaultClient()
    {
        var s = _subscription.GetDefaultClient();
        return _parent.GetClientStub(s);
    }
    /// <summary>
    /// Try getting the "default client" sub object.
    /// </summary>
    /// <remarks>
    /// <para>Same as GetDefaultClient(), but returns a bool for success or failure instead of throwing
    /// an exception on failure.</para>
    /// </remarks>
    /// <param name="obj">The sub object.</param>
    /// <returns>true if the sub object was retrieved successfully; otherwise, false.</returns>
    public bool TryGetDefaultClient(out object obj)
    {
        var res = _subscription.TryGetDefaultClient();
        if (!res.res)
        {
            obj = null;
            return false;
        }

        var s = res.client;
        obj = _parent.GetClientStub(s);
        return true;
    }

    /// <summary>
    /// Get the "default client" sub object, waiting for a specified timeout.
    /// </summary>
    /// <remarks>
    /// <para>The sub object is retrieved from the default client. The default client is the first client
    /// that connected to the service. If no clients are currently connected, an exception is thrown.</para>
    ///
    /// <para>Clients using GetDefaultClient() should not store a reference to the client. Call GetDefaultClient()
    /// each time the client is needed.</para>
    ///
    /// <para>This function blocks the current thread until the client is retrieved or the timeout is reached.</para>
    /// </remarks>
    /// <param name="timeout">The timeout in milliseconds.</param>
    /// <returns>The sub object.</returns>
    public object GetDefaultClientWait(int timeout = -1)
    {
        var s = _subscription.GetDefaultClientWait(timeout);
        return _parent.GetClientStub(s);
    }

    /// <summary>
    /// Try getting the "default client" sub object, waiting for a specified timeout.
    /// </summary>
    /// <remarks>
    /// <para>Same as GetDefaultClientWait(), but returns a bool for success or failure instead of throwing
    /// an exception on failure.</para>
    /// </remarks>
    /// <param name="obj">The sub object.</param>
    /// <param name="timeout">The timeout in milliseconds.</param>
    /// <returns>true if the sub object was retrieved successfully; otherwise, false.</returns>
    public bool TryGetDefaultClientWait(out object obj, int timeout = -1)
    {
        var res = _subscription.TryGetDefaultClientWait(timeout);
        if (!res.res)
        {
            obj = null;
            return false;
        }

        var s = res.client;
        obj = _parent.GetClientStub(s);
        return true;
    }

    /// <summary>
    /// Asynchronously get the "default client" sub object.
    /// </summary>
    /// <remarks>
    /// <para>Asynchronous version of GetDefaultClient(). The task completes when the
    /// client is retrieved or an error occurs.</para>
    /// </remarks>
    /// <param name="timeout">The timeout in milliseconds.</param>
    public async Task<object> AsyncGetDefaultClient(int timeout = -1)
    {
        AsyncStubReturnDirectorImpl<object> h = new AsyncStubReturnDirectorImpl<object>(null);
        int id = RRObjectHeap.AddObject(h);
        _subscription.AsyncGetDefaultClient(timeout, h, id);
        return await h.Task;
    }
}
/// <summary>
/// ServiceSubscriptionManager subscription connection information
/// </summary>
/// <remarks>
/// Contains the connection information for a ServiceSubscriptionManager subscription
/// and the local name of the subscription
/// </remarks>
public class ServiceSubscriptionManagerDetails
{
    /// <summary>
    /// The local name of the subscription
    /// </summary>
    /// <remarks>None</remarks>
    public string Name;
    /// <summary>
    /// The connection method to use, URL or service type
    /// </summary>
    /// <remarks>None</remarks>
    public ServiceSubscriptionManager_CONNECTION_METHOD ConnectionMethod =
        ServiceSubscriptionManager_CONNECTION_METHOD.CONNECTION_METHOD_DEFAULT;
    /// <summary>
    /// The URLs to use for subscription
    /// </summary>
    /// <remarks>None</remarks>
    public string[] Urls;
    /// <summary>
    /// The username to use for URLs (optional)
    /// </summary>
    /// <remarks>None</remarks>
    public string UrlUsername;
    /// <summary>
    /// The credentials to use for URLs (optional)
    /// </summary>
    /// <remarks>None</remarks>
    public Dictionary<string, object> UrlCredentials;
    /// <summary>
    /// The service types to use for subscription
    /// </summary>
    /// <remarks>None</remarks>
    public string[] ServiceTypes;
    /// <summary>
    /// The filter to use for subscription when service type is used (optional)
    /// </summary>
    /// <remarks>None</remarks>
    public ServiceSubscriptionFilter Filter;
    /// <summary>
    /// If the subscription is enabled
    /// </summary>
    /// <remarks>None</remarks>
    public bool Enabled = true;
}

/// <summary>
/// Class to manage multiple subscriptions to services
/// </summary>
/// <remarks>
/// <para> ServiceSubscriptionManager is used to manage multiple subscriptions to services. Subscriptions
/// are created using information contained in ServiceSubscriptionManagerDetails structures. The subscriptions
/// can connect using URLs or service types. The subscriptions can be enabled or disabled, and can be
/// closed.
/// </para>
/// </remarks>
public class ServiceSubscriptionManager
{
    internal WrappedServiceSubscriptionManager _subscription_manager;

    internal Dictionary<string, ServiceSubscription> _subscriptions = new Dictionary<string, ServiceSubscription>();

    internal WrappedServiceSubscriptionManagerDetails
    _ServiceSubscriptionManager_LoadDetails(ServiceSubscriptionManagerDetails details)
    {
        var details2 = new WrappedServiceSubscriptionManagerDetails();
        details2.Name = details.Name;
        details2.ConnectionMethod = (ServiceSubscriptionManager_CONNECTION_METHOD)details.ConnectionMethod;
        if (details.Urls != null)
        {
            foreach (var s in details.Urls)
            {
                details2.Urls.Add(s);
            }
        }
        details2.UrlUsername = details.UrlUsername;
        if (details.UrlCredentials != null)
        {
            details2.UrlCredentials =
                (MessageElementData)RobotRaconteurNode.s.PackMapType<string, object>(details.UrlCredentials);
        }
        if (details.ServiceTypes != null)
        {
            foreach (var s in details.ServiceTypes)
            {
                details2.ServiceTypes.Add(s);
            }
        }
        if (details.Filter != null)
        {
            details2.Filter = RobotRaconteurNode.s.SubscribeService_LoadFilter(details.Filter);
        }
        details2.Enabled = details.Enabled;
        return details2;
    }

    /// <summary>
    /// Construct a new ServiceSubscriptionManager object
    /// </summary>
    /// <remarks>None</remarks>
    /// <param name="details">(optional) List of subscriptions to initialize</param>
    public ServiceSubscriptionManager(ServiceSubscriptionManagerDetails[] details = null)
    {
        var details2 = new vector_wrappedservicesubscriptionmanagerdetails();
        if (details != null)
        {
            foreach (var d in details)
            {
                details2.Add(_ServiceSubscriptionManager_LoadDetails(d));
            }
        }

        _subscription_manager = new WrappedServiceSubscriptionManager(details2);
    }

    /// <summary>
    /// Add a subscription to the manager
    /// </summary>
    /// <remarks>None</remarks>
    /// <param name="details">The subscription details</param>
    public void AddSubscription(ServiceSubscriptionManagerDetails details)
    {
        _subscription_manager.AddSubscription(_ServiceSubscriptionManager_LoadDetails(details));
    }

    /// <summary>
    /// Remove a subscription from the manager
    /// </summary>
    /// <remarks>None</remarks>
    /// <param name="name">The local name of the subscription to remove</param>
    /// <param name="close">(optional) If true, close the subscription</param>
    public void RemoveSubscription(string name, bool close = true)
    {
        lock (this)
        {
            _subscription_manager.RemoveSubscription(name, close);
            if (close)
            {
                _subscriptions.Remove(name);
            }
        }
    }

    /// <summary>
    /// Enable a subscription
    /// </summary>
    /// <remarks>None</remarks>
    /// <param name="name">The name of the subscription to enable</param>
    public void EnableSubscription(string name)
    {
        _subscription_manager.EnableSubscription(name);
    }

    /// <summary>
    /// Disable a subscription
    /// </summary>
    /// <remarks>None</remarks>
    /// <param name="name">The name of the subscription to disable</param>
    /// <param name="close">(optional) If true, close the subscription</param>
    public void DisableSubscription(string name, bool close = true)
    {
        _subscription_manager.DisableSubscription(name, close);
    }

    /// <summary>
    /// Get a subscription by name
    /// </summary>
    /// <param name="name">The local name of the subscription</param>
    /// <param name="force_create">(optional) If true, create the subscription if it does not exist</param>
    /// <remarks>None</remarks>
    /// <returns>The subscription</returns>
    public ServiceSubscription GetSubscription(string name, bool force_create = false)
    {
        lock (this)
        {
            if (_subscriptions.ContainsKey(name))
            {
                return _subscriptions[name];
            }

            var s = _subscription_manager.GetSubscription(name, force_create);
            if (s == null)
            {
                return null;
            }

            var s2 = new ServiceSubscription(s);
            _subscriptions[name] = s2;
            return s2;
        }
    }

    /// <summary>
    /// Get if a subscription is connected
    /// </summary>
    /// <remarks>None</remarks>
    /// <param name="name">Teh local name of the subscription</param>
    /// <returns>True if the subscription is connected</returns>
    public bool IsConnected(string name)
    {
        return _subscription_manager.IsConnected(name);
    }

    /// <summary>
    /// Get if a subscription is enabled
    /// </summary>
    /// <remarks>None</remarks>
    /// <param name="name">The local name of the subscription</param>
    /// <returns>True if the subscription is enabled</returns>
    public bool IsEnabled(string name)
    {
        return _subscription_manager.IsEnabled(name);
    }

    /// <summary>
    /// Close the subscription manager
    /// </summary>
    /// <remarks>None</remarks>
    /// <param name="close_subscriptions">(optional) If true, close all subscriptions</param>
    public void Close(bool close_subscriptions = true)
    {
        _subscription_manager.Close(close_subscriptions);
        lock (this)
        {
            _subscriptions.Clear();
        }
    }

    /// <summary>
    /// Get the local names of all subscriptions
    /// </summary>
    /// <remarks>None</remarks>
    public string[] SubscriptionNames
    {
        get {
            return _subscription_manager.GetSubscriptionNames().ToArray();
        }
    }

    /// <summary>
    /// Get the details of all subscriptions
    /// </summary>
    /// <remarks>None</remarks>
    public ServiceSubscriptionManagerDetails[] SubscriptionDetails
    {
        get {
            var details = new List<ServiceSubscriptionManagerDetails>();
            var details2 = _subscription_manager.GetSubscriptionDetails();
            foreach (var d in details2)
            {
                var d2 = new ServiceSubscriptionManagerDetails();
                d2.Name = d.Name;
                d2.ConnectionMethod = (ServiceSubscriptionManager_CONNECTION_METHOD)d.ConnectionMethod;
                d2.Urls = d.Urls.ToArray();
                d2.UrlUsername = d.UrlUsername;
                d2.ServiceTypes = d.ServiceTypes.ToArray();
                d2.Enabled = d.Enabled;
                details.Add(d2);
            }
            return details.ToArray();
        }
    }
}

public partial class RobotRaconteurNode
{

    private class WrappedServiceSubscriptionFilterPredicateDirectorNET
        : WrappedServiceSubscriptionFilterPredicateDirector
    {
        Func<ServiceInfo2, bool> _f;

        public WrappedServiceSubscriptionFilterPredicateDirectorNET(Func<ServiceInfo2, bool> f)
        {
            if (f == null)
                throw new NullReferenceException();
            _f = f;
        }

        public override bool Predicate(ServiceInfo2Wrapped info)
        {
            var info2 = new ServiceInfo2(info);
            return _f(info2);
        }
    }

    internal WrappedServiceSubscriptionFilter SubscribeService_LoadFilter(ServiceSubscriptionFilter filter)
    {
        WrappedServiceSubscriptionFilter filter2 = null;
        if (filter != null)
        {
            filter2 = new WrappedServiceSubscriptionFilter();
            if (filter.ServiceNames != null)
                foreach (string s in filter.ServiceNames)
                    filter2.ServiceNames.Add(s);
            if (filter.TransportSchemes != null)
                foreach (string s in filter.TransportSchemes)
                    filter2.TransportSchemes.Add(s);
            if (filter.Attributes != null)
            {
                foreach (var a in filter.Attributes)
                {
                    if (a.Value == null)
                        continue;
                    filter2.Attributes[a.Key] = a.Value;
                }
            }
            filter2.AttributesMatchOperation = filter.AttributesMatchOperation;
            filter2.MaxConnections = filter.MaxConnections;
            if (filter.Nodes != null)
            {
                var nodes2 = new vectorptr_wrappedservicesubscriptionnode();
                foreach (var n1 in filter.Nodes)
                {
                    if (n1 == null)
                        continue;
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

    /// <summary>
    /// Subscribe to listen for available services information
    /// </summary>
    /// <remarks>
    /// A ServiceInfo2Subscription will track the availability of service types and
    /// inform when services become available or are lost. If connections to
    /// available services are also required, ServiceSubscription should be used.
    /// </remarks>
    /// <param name="service_types">An array of service types to listen for, ie
    ///     `com.robotraconteur.robotics.robot.Robot`</param>
    /// <param name="filter">A filter to select individual services based on specified criteria</param>
    /// <returns>The active subscription</returns>
    public ServiceInfo2Subscription SubscribeServiceInfo2(string[] service_types,
                                                          ServiceSubscriptionFilter filter = null)
    {
        var filter2 = SubscribeService_LoadFilter(filter);

        var service_types2 = new vectorstring();
        foreach (string s in service_types)
            service_types2.Add(s);

        var sub1 = RobotRaconteurNET.WrappedSubscribeServiceInfo2(this, service_types2, filter2);
        return new ServiceInfo2Subscription(sub1);
    }

    /// <summary>
    /// Subscribe to listen for available services and automatically connect
    /// </summary>
    /// <remarks>
    /// A ServiceSubscription will track the availability of service types and
    /// create connections when available.
    /// </remarks>
    /// <param name="service_types">An arrayof service types to listen for, ie
    ///     `com.robotraconteur.robotics.robot.Robot`</param>
    /// <param name="filter">A filter to select individual services based on specified criteria</param>
    /// <returns>The active subscription</returns>
    public ServiceSubscription SubscribeServiceByType(string[] service_types, ServiceSubscriptionFilter filter = null)
    {
        var filter2 = SubscribeService_LoadFilter(filter);

        var service_types2 = new vectorstring();
        foreach (string s in service_types)
            service_types2.Add(s);

        var sub1 = RobotRaconteurNET.WrappedSubscribeServiceByType(this, service_types2, filter2);
        return new ServiceSubscription(sub1);
    }

    /// <summary>
    /// Subscribe to a service using one a URL. Used to create robust connections to services
    /// </summary>
    /// <remarks>
    /// Creates a ServiceSubscription assigned to a service with one or more candidate connection URLs. The
    /// subscription will attempt to maintain a persistent connection, reconnecting if the connection is lost.
    /// </remarks>
    /// <param name="url">One or more candidate connection urls</param>
    /// <param name="username">An optional username for authentication</param>
    /// <param name="credentials">Optional credentials for authentication</param>
    /// <param name="objecttype">The desired root object proxy type. Optional but highly recommended.</param>
    /// <returns>The active subscription</returns>
    public ServiceSubscription SubscribeService(string url, string username = null,
                                                Dictionary<string, object> credentials = null, string objecttype = null)
    {
        return SubscribeService(new string[] { url }, username, credentials, objecttype);
    }

    /// <summary>
    /// Subscribe to a service using one or more URL. Used to create robust connections to services
    /// </summary>
    /// <remarks>
    /// Creates a ServiceSubscription assigned to a service with one or more candidate connection URLs. The
    /// subscription will attempt to maintain a persistent connection, reconnecting if the connection is lost.
    /// </remarks>
    /// <param name="url">One or more candidate connection urls</param>
    /// <param name="username">An optional username for authentication</param>
    /// <param name="credentials">Optional credentials for authentication</param>
    /// <param name="objecttype">The desired root object proxy type. Optional but highly recommended.</param>
    /// <returns>The active subscription</returns>
    public ServiceSubscription SubscribeService(string[] url, string username = null,
                                                Dictionary<string, object> credentials = null, string objecttype = null)
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

            if (objecttype == null)
                objecttype = "";
            vectorstring url2 = new vectorstring();
            foreach (string ss in url)
                url2.Add(ss);

            var sub1 = RobotRaconteurNET.WrappedSubscribeService(this, url2, username, credentials2, objecttype);
            return new ServiceSubscription(sub1);
        }
        finally
        {
            if (credentials2 != null)
                credentials2.Dispose();
        }
    }
}

}
