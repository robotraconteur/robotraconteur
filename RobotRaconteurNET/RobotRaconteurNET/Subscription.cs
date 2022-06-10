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
        if (Object.Equals(a, null) && Object.Equals(b, null))
            return true;
        if (Object.Equals(a, null) || Object.Equals(b, null))
            return false;
        return (a.NodeID == b.NodeID) && (a.ServiceName == b.ServiceName);
    }

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
        if (s == null)
            throw new ArgumentException("Invalid object for ClaimClient");

        _subscription.ClaimClient(s.rr_innerstub);
    }

    public void ReleaseClient(object client)
    {
        ServiceStub s = client as ServiceStub;
        if (s == null)
            throw new ArgumentException("Invalid object for ReleaseClient");

        _subscription.ClaimClient(s.rr_innerstub);
    }

    public uint ConnectRetryDelay
    {
        get {
            return _subscription.GetConnectRetryDelay();
        }
        set {
            _subscription.SetConnectRetryDelay(value);
        }
    }

    public event Action<ServiceSubscription, ServiceSubscriptionClientID, object> ClientConnected;
    public event Action<ServiceSubscription, ServiceSubscriptionClientID, object> ClientDisconnected;

    public event Action<ServiceSubscription, ServiceSubscriptionClientID, string[], Exception> ClientConnectFailed;

    public WireSubscription<T> SubscribeWire<T>(string wire_name, string service_path = "")
    {
        var s = _subscription.SubscribeWire(wire_name, service_path);
        return new WireSubscription<T>(s);
    }

    public PipeSubscription<T> SubscribePipe<T>(string pipe_name, string service_path = "", int max_backlog = -1)
    {
        var s = _subscription.SubscribePipe(pipe_name, service_path, max_backlog);
        return new PipeSubscription<T>(s);
    }

    public object GetDefaultClient()
    {
        var s = _subscription.GetDefaultClient();
        return GetClientStub(s);
    }

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

    public object GetDefaultClientWait(int timeout = -1)
    {
        var s = _subscription.GetDefaultClientWait(timeout);
        return GetClientStub(s);
    }

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

    public async Task<object> AsyncGetDefaultClient(int timeout = -1)
    {
        AsyncStubReturnDirectorImpl<object> h = new AsyncStubReturnDirectorImpl<object>(null);
        int id = RRObjectHeap.AddObject(h);
        _subscription.AsyncGetDefaultClient(timeout, h, id);
        return await h.Task;
    }

    public string[] ServiceURL
    {
        get {
            return _subscription.GetServiceURL().ToArray();
        }
    }

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

    public bool WaitInValueValid(int timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE)
    {
        return _subscription.WaitInValueValid(timeout);
    }

    public bool IgnoreInValue
    {
        get {
            return _subscription.GetIgnoreInValue();
        }
        set {
            _subscription.SetIgnoreInValue(value);
        }
    }

    public int InValueLifespan
    {
        get {
            return _subscription.GetInValueLifespan();
        }
        set {
            _subscription.SetInValueLifespan(value);
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

    public uint ActiveWireConnectionCount
    {
        get {
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

    public uint Available
    {
        get {
            return _subscription.Available();
        }
    }

    public bool IgnoreReceived
    {
        get {
            return _subscription.GetIgnoreReceived();
        }
        set {
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

    public uint ActivePipeEndpointCount
    {
        get {
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

    private WrappedServiceSubscriptionFilter SubscribeService_LoadFilter(ServiceSubscriptionFilter filter)
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

    public ServiceSubscription SubscribeServiceByType(string[] service_types, ServiceSubscriptionFilter filter = null)
    {
        var filter2 = SubscribeService_LoadFilter(filter);

        var service_types2 = new vectorstring();
        foreach (string s in service_types)
            service_types2.Add(s);

        var sub1 = RobotRaconteurNET.WrappedSubscribeServiceByType(this, service_types2, filter2);
        return new ServiceSubscription(sub1);
    }

    public ServiceSubscription SubscribeService(string url, string username = null,
                                                Dictionary<string, object> credentials = null, string objecttype = null)
    {
        return SubscribeService(new string[] { url }, username, credentials, objecttype);
    }

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