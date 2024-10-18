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
/// Interface for service objects to receive service notifications
/// </summary>
/// <remarks>
/// Service objects are passed to the service, either when the service is registered
/// or using objrefs. The service initialized the object by configuring events,
/// pipes, callbacks, and wires for use. The object may implement IRRServiceObject
/// to receive notification of when this process is complete, and to receive
/// a ServerContextPtr and the service path of the object.
/// IRRServiceObject.RRServiceObjectInit() is called after the object has been
/// initialized to provide this information.
/// </remarks>
public interface IRRServiceObject
{
    /// <summary>
    /// Function called after service object has been initialized
    /// </summary>
    /// <remarks>
    /// Override in the service object to receive notification the service object has
    /// been initialized, a ServerContextPtr, and the service path.
    /// </remarks>
    /// <param name="context">The ServerContextPtr owning the object</param>
    /// <param name="service_path">The object service path</param>
    void RRServiceObjectInit(ServerContext context, string service_path);
}

/// <summary>
/// "callback" member type interface
/// </summary>
/// <remarks>
/// <para>
/// The Callback class implements the `callback` member type. Callbacks are declared in service definition
/// files using the `callback` keyword within object declarations. They provide functionality similar to the
/// `function` member, but the direction is reversed, allowing the service to call a function on a specified
/// client. The desired client is specified using the Robot Raconteur endpoint identifier. Clients must
/// configure the callback to use using SetFunction().
/// </para>
/// <para>
///  On the client side,
/// the client specifies a function for the callback using the SetFunction() function.
/// On the service side, the function GetFunction(uint e) is used to retrieve
/// the proxy function to call a client callback.
/// </para>
/// <para>
/// This class is instantiated by the node. It should not be instantiated by the user.
/// </para>
/// </remarks>
/// <typeparam name="T">The type of the callback function. This is determined by the thunk source generator.</typeparam>
public abstract class Callback<T>
{

    protected string m_MemberName;

    public Callback(string name)
    {
        m_MemberName = name;
    }

    /// <summary>
    /// Get or set the currently configured callback function on client side
    /// </summary>
    /// <remarks>
    /// The callback function set will be made available to be called by
    /// the service using a function proxy.
    /// </remarks>
    public abstract T Function { get; set; }

    /// <summary>
    /// Get the proxy function to call the callback for the specified client on
    /// the service side
    /// </summary>
    /// <remarks>
    /// <para>
    ///  This function returns a proxy to the callback on a specified client. The proxy
    /// operates as a reverse function, sending parameters, executing the callback,
    /// and receiving the results.
    /// </para>
    /// <para>
    /// Because services can have multiple clients, it is necessary to specify which client
    /// to call. This is done by passing the endpoint of the client connection to the
    /// endpoint parameter.
    /// </para>
    /// <para>
    /// The endpoint of a client can be determined using the ServerEndpoint.CurrentEndpoint()
    /// function during a `function` or `property` member call. The service can store this
    /// value, and use it to retrieve the callback proxy.
    /// </para>
    /// </remarks>
    /// <param name="e">The endpoint of the client connection</param>
    /// <returns>The callback proxy function</returns>
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

/// <summary>
/// `pipe` member type interface
/// </summary>
/// <remarks>
/// <para>
/// The Pipe class implements the `pipe` member type. Pipes are declared in service definition files
/// using the `pipe` keyword within object declarations. Pipes provide reliable packet streaming between
/// clients and services. They work by creating pipe endpoint pairs (peers), with one endpoint in the client,
/// and one in the service. Packets are transmitted between endpoint pairs. Packets sent by one endpoint are received
/// by the other, where they are placed in a receive queue. Received packets can then be retrieved from the receive
/// queue.
/// </para>
/// <para>
/// Pipe endpoints are created by the client using the Connect() or AsyncConnect() functions. Services receive
/// incoming connection requests through a callback function. This callback is configured using the
/// SetPipeConnectCallback() function. Services may also use the PipeBroadcaster class to automate managing pipe
/// endpoint lifecycles and sending packets to all connected client endpoints. If the SetPipeConnectCallback() function
/// is used, the service is responsible for keeping track of endpoints as the connect and disconnect. See PipeEndpoint
/// for details on sending and receiving packets.
/// </para>
/// <para>
/// Pipe endpoints are *indexed*, meaning that more than one endpoint pair can be created between the client and the
/// service.
/// </para>
/// <para>
///  Pipes may be *unreliable*, meaning that packets may arrive out of order or be dropped. Use IsUnreliable() to check
/// for unreliable pipes. The member modifier `unreliable` is used to specify that a pipe should be unreliable.
/// </para>
/// <para>
/// Pipes may be declared *readonly* or *writeonly*. If neither is specified, the pipe is assumed to be full duplex.
/// *readonly* pipes may only send packets from service to client. *writeonly* pipes may only send packets from client
/// to service. Use Direction() to determine the direction of the pipe.
/// </para>
/// <para>
/// The PipeBroadcaster is often used to simplify the use of Pipes. See PipeBroadcaster for more information.
/// </para>
/// <para>
/// This class is instantiated by the node. It should not be instantiated by the user.
/// </para>
/// </remarks>
/// <typeparam name="T">The packet data type</typeparam>
public class Pipe<T>
{
    internal object innerpipe;

    Func<int, WrappedPipeEndpoint> ConnectFunction;
    Action<int, int, AsyncPipeEndpointReturnDirector, int> AsyncConnectFunction;
    string m_Membername;
    MemberDefinition_Direction direction;

    /// <summary>
    /// The pipe member name
    /// </summary>
    /// <remarks>None</remarks>
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

    /// <summary>
    /// Get the direction of the pipe
    /// </summary>
    /// <remarks>
    /// Pipes may be declared *readonly* or* writeonly* in the service definition file. (If neither
    /// is specified, the pipe is assumed to be full duplex.) *readonly* pipes may only send packets from
    /// service to client. *writeonly* pipes may only send packets from client to service.
    /// </remarks>
    public MemberDefinition_Direction Direction
    {
        get {
            return direction;
        }
    }

    /// <summary>
    /// Connect to any pipe index
    /// </summary>
    /// <remarks>None</remarks>
    public const int ANY_INDEX = -1;

    /// <summary>
    /// Connect a pipe endpoint
    /// </summary>
    /// <remarks>
    /// <para>
    /// Creates a connected pipe endpoint pair, and returns the local endpoint. Use to create the streaming data
    /// connection to the service. Pipe endpoints are indexed, meaning that Connect() may be called multiple
    /// times for the same client connection to create multple pipe endpoint pairs. For most cases Pipe.ANY_INDEX
    /// (-1) can be used to automatically select an available index.
    /// </para>
    /// <para>
    /// Only valid on clients. Will throw InvalidOperationException on the service side.
    /// </para>
    /// </remarks>
    /// <param name="index">The index of the pipe endpoint, or (-1) to automatically select an index</param>
    /// <returns></returns>
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

    /// <summary>
    /// Asynchronously connect a pipe endpoint.
    /// </summary>
    /// <remarks>
    /// <para>
    /// Same as Connect(), but returns async.
    /// </para>
    /// <para>
    /// Only valid on clients. Will throw InvalidOperationException on the service side.
    /// </para>
    /// </remarks>
    /// <param name="timeout">Timeout in milliseconds, or RR_TIMEOUT_INFINITE for no timeout</param>
    /// <returns>Task that upon completion returns connected endpoint</returns>
    public Task<PipeEndpoint> AsyncConnect(int timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE)
    {
        return AsyncConnect(-1, timeout);
    }

    /// <summary>
    /// Asynchronously connect a pipe endpoint.
    /// </summary>
    /// <remarks>
    /// <para>
    /// Same as Connect(), but returns async.
    /// </para>
    /// <para>
    /// Only valid on clients. Will throw InvalidOperationException on the service side.
    /// </para>
    /// </remarks>
    /// <param name="index">The index of the pipe endpoint, or ANY_INDEX to automatically select an index</param>
    /// <param name="timeout">Timeout in milliseconds, or RR_TIMEOUT_INFINITE for no timeout</param>
    /// <returns>Task that upon completion returns connected endpoint</returns>
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

    /// <summary>
    /// Set the pipe endpoint connected callback function
    /// </summary>
    /// <remarks>
    /// <para>
    /// Callback function invoked when a client attempts to connect a pipe endpoint. The callback
    /// will receive the incoming pipe endpoint as a parameter. The service must maintain a reference to the
    /// pipe endpoint, but the pipe will retain ownership of the endpoint until it is closed.
    /// </para>
    /// </remarks>
    /// <para>
    /// The callback may throw an exception to reject incoming connect request.
    /// </para>
    /// <para>
    /// Note: Connect callback is configured automatically by PipeBroadcaster
    /// </para>
    /// <para>
    /// Only valid for services. Will throw InvalidOperationException on the client side.
    /// </para>
    /// <value></value>
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

    /// <summary>
    /// Pipe endpoint used to transmit reliable or unreliable data streams
    /// </summary>
    /// <remarks>
    /// <para>
    /// Pipe endpoints are used to communicate data between connected pipe members.
    /// See Pipe for more information on pipe members.
    /// </para>
    /// <para>
    /// Pipe endpoints are created by clients using the Pipe.Connect() or Pipe.AsyncConnect()
    /// functions. Services receive incoming pipe endpoint connection requests through a
    /// callback function specified using the Pipe.PipeConnectCallback property. Services
    /// may also use the PipeBroadcaster class to automate managing pipe endpoint lifecycles and
    /// sending packets to all connected client endpoints.
    /// </para>
    /// <para>
    /// Pipe endpoints are///indexed*, meaning that more than one pipe endpoint pair can be created
    /// using the same member. This means that multiple data streams can be created independent of
    /// each other between the client and service using the same member.
    /// </para>
    /// <para>
    /// Pipes send reliable packet streams between connected client/service endpoint pairs.
    /// Packets are sent using the SendPacket() or AsyncSendPacket() functions. Packets
    /// are read from the receive queue using the ReceivePacket(), ReceivePacketWait(),
    /// TryReceivePacketWait(), TryReceivePacketWait(), or PeekNextPacket(). The endpoint is closed
    /// using the Close() or AsyncClose() function.
    /// </para>
    /// <para>
    /// This class is instantiated by the Pipe class. It should not be instantiated
    /// by the user.
    /// </para>
    /// </remarks>
    public class PipeEndpoint
    {
        WrappedPipeEndpoint innerpipe;

        /// <summary>
        /// Signal called when a packet has been received
        /// </summary>
        /// <remarks>
        /// Function must accept one argument, receiving the PipeEndpoint that
        /// received a packet
        /// </remarks>
        public event Action<PipeEndpoint> PacketReceivedEvent;

        /// <summary>
        /// Signal called when a packet ack has been received
        /// </summary>
        /// <remarks>
        /// <para>
        /// Packet acks are generated if SetRequestPacketAck() is set to true. The receiving
        /// endpoint generates acks to inform the sender that the packet has been received.
        /// </para>
        /// <para>
        /// Function must accept two arguments, receiving the PipeEndpoint
        /// that received the packet ack and the packet number that is being acked.
        /// </para>
        /// </remarks>
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

        /// <summary>
        /// Get the pipe endpoint index used when endpoint connected
        /// </summary>
        /// <remarks>None</remarks>
        public int Index
        {
            get {
                return innerpipe.GetIndex();
            }
        }

        /// <summary>
        /// Get the Robot Raconteur node Endpoint ID
        /// </summary>
        /// <remarks>
        /// Get the endpoint associated with the ClientContext or ServerEndpoint
        /// associated with the pipe endpoint.
        /// </remarks>
        public uint Endpoint
        {
            get {
                return innerpipe.GetEndpoint();
            }
        }

        /// <summary>
        /// Get or set if pipe endpoint should request packet acks
        /// </summary>
        /// <remarks>
        /// Packet acks are generated by receiving endpoints to inform the sender that
        /// a packet has been received. The ack contains the packet index, the sequence number
        /// of the packet. Packet acks are used for flow control by PipeBroadcaster.
        /// </remarks>
        public bool RequestPacketAck
        {
            get {
                return innerpipe.GetRequestPacketAck();
            }
            set {
                innerpipe.SetRequestPacketAck(value);
            }
        }

        /// <summary>
        /// Get or set whether pipe endpoint should ignore incoming packets
        /// </summary>
        /// <remarks>
        /// Pipe endpoints may optionally desire to ignore incoming data. This is useful if the endpoint
        /// is only being used to send packets, and received packets may create a potential memory leak if they
        /// are not being removed from the queue. If ignore is true, incoming packets will be discarded and
        /// will not be added to the receive queue.
        /// </remarks>
        public bool IgnoreReceived
        {
            get {
                return innerpipe.GetIgnoreReceived();
            }
            set {
                innerpipe.SetIgnoreReceived(value);
            }
        }

        /// <summary>
        /// The direction of the pipe
        /// </summary>
        /// <remarks>
        /// Pipes may be declared*readonly* or*writeonly* in the service definition file. (If neither
        /// is specified, the pipe is assumed to be full duplex.)*readonly* pipes may only send packets from
        /// service to client.*writeonly* pipes may only send packets from client to service.
        /// </remarks>
        public MemberDefinition_Direction Direction
        {
            get {
                return innerpipe.Direction();
            }
        }

        private Action<PipeEndpoint> close_callback;

        /// <summary>
        /// Get or set the endpoint closed callback function
        /// </summary>
        /// <remarks>
        /// <para>
        /// Get or Set a function to invoke when the pipe endpoint has been closed.
        /// </para>
        /// <para>
        /// Callback function must accept one argument, receiving the PipeEndpoint that
        /// was closed.
        /// </para>
        /// </remarks>
        public Action<PipeEndpoint> PipeCloseCallback
        {
            get {
                return close_callback;
            }
            set {
                close_callback = value;
            }
        }

        /// <summary>
        /// Return number of packets in the receive queue
        /// </summary>
        /// <remarks>
        /// Invalid for writeonly pipes.
        /// </remarks>
        public int Available
        {
            get {
                return (int)innerpipe.Available();
            }
        }

        /// <summary>
        /// Get if pipe endpoint is unreliable
        /// </summary>
        /// <remarks>
        /// Pipe members may be declared as*unreliable* using member modifiers in the
        /// service definition. Pipes confirm unreliable operation when pipe endpoints are connected.
        /// </remarks>
        public bool IsUnreliable
        {
            get {
                return innerpipe.IsUnreliable();
            }
        }

        /// <summary>
        /// Peeks the next packet in the receive queue
        /// </summary>
        /// <remarks>
        /// Returns the first packet in the receive queue, but does not remove it from
        /// the queue. Throws an InvalidOperationException if there are no packets in the
        /// receive queue.
        /// </remarks>
        /// <returns>The next packet in the receive queue</returns>
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

        /// <summary>
        /// Receive the next packet in the receive queue
        /// </summary>
        /// <remarks>
        /// Receive the next packet from the receive queue. This function will throw an
        /// InvalidOperationException if there are no packets in the receive queue. Use
        /// ReceivePacketWait() to block until a packet has been received.
        /// </remarks>
        /// <returns>The received packet</returns>
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

        /// <summary>
        /// Peek the next packet in the receive queue, block if queue is empty
        /// </summary>
        /// <remarks>
        /// Same as PeekPacket(), but blocks if queue is empty
        /// </remarks>
        /// <param name="timeout">Timeout in milliseconds to wait for a packet, or RR_TIMEOUT_INFINITE for no
        /// timeout</param>
        /// <returns>The received packet</returns>
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

        /// <summary>
        /// Receive the next packet in the receive queue, block if queue is empty
        /// </summary>
        /// <remarks>
        /// Same as ReceivePacket(), but blocks if queue is empty
        /// </remarks>
        /// <param name="timeout">Timeout in milliseconds to wait for a packet, or RR_TIMEOUT_INFINITE for no
        /// timeout</param>
        /// <returns>The received packet</returns>
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

        /// <summary>
        /// Try receiving a packet, optionally blocking if the queue is empty
        /// </summary>
        /// <remarks>
        /// <para>
        /// Try receiving a packet with various options. Returns true if a packet has been
        /// received, or false if no packet is available instead of throwing an exception on failure.
        /// The timeout and peek parameters can be used to modify behavior to provide functionality
        /// similar to the various Receive and Peek functions.
        /// </para>
        /// </remarks>
        /// <param name="packet">[out] The received packet</param>
        /// <param name="timeout">The timeout in milliseconds. Set to zero for non-blocking operation, an arbitrary
        /// value
        ///     in milliseconds for a finite duration timeout, or RR_TIMEOUT_INFINITE for no timeout</param>
        /// <param name="peek">If true, the packet is not removed from the receive queue</param>
        /// <returns>true if packet was received, otherwise false</returns>
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

        /// <summary>
        /// Sends a packet to the peer endpoint
        /// </summary>
        /// <remarks>
        /// Sends a packet to the peer endpoint. If the pipe is reliable, the packetsare  guaranteed to arrive
        /// in order. If the pipe is set to unreliable, "best effort" is made to deliver packets, and they are not
        /// guaranteed to arrive in order. This function will block until the packet has been transmitted by the
        /// transport. It will return before the peer endpoint has received the packet.
        /// </remarks>
        /// <param name="packet">The packet to send</param>
        /// <returns></returns>
        public uint SendPacket(T packet)
        {

            try
            {
                using (MessageElement m = RobotRaconteurNode.s.PackAnyType<T>("value", ref packet))
                {
                    return innerpipe.SendPacket(m);
                }
            }
            finally
            {}
        }

        /// <summary>
        /// Send a packet to the peer endpoint asynchronously
        /// </summary>
        /// <remarks>
        /// Same as SendPacket(), but async.
        /// </remarks>
        /// <param name="packet">The packet to send</param>
        /// <returns>A task that upon completion returns the sent packet number</returns>
        public async Task<uint> AsyncSendPacket(T packet)
        {

            AsyncUInt32ReturnDirectorImpl h = new AsyncUInt32ReturnDirectorImpl();

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
            return await h.Task;
        }

        /// <summary>
        /// Close the pipe endpoint
        /// </summary>
        /// <remarks>
        ///  Close the pipe endpoint. Blocks until close complete. The peer endpoint is destroyed
        /// automatically.
        /// </remarks>
        public void Close()
        {
            innerpipe.Close();
        }

        /// <summary>
        /// Asynchronously close the pipe endpoint
        /// </summary>
        /// <remarks>
        /// Same as Close() but async
        /// </remarks>
        /// <param name="timeout">Timeout in milliseconds, or RR_TIMEOUT_INFINITE for no timeout</param>
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

/// <summary>
/// Broadcaster to send packets to all connected clients
/// </summary>
/// <remarks>
/// <para>
/// PipeBroadcaster is used by services to send packets to all connected
/// client endpoints. It attaches to the pipe on the service side, and
/// manages the lifecycle of connected endpoints. PipeBroadcaster should
/// only be used with pipes that are declared*readonly*, since it has
/// no provisions for receiving incoming packets from the client.
/// </para>
/// <para>
/// PipeBroadcaster is initialized by the user, or by default implementation
/// classes generated by RobotRaconteurGen (*_default_impl). Default
/// implementation classes will automatically instantiate broadcasters
/// for pipes marked*readonly*. If default implementation classes are
/// not used, the broadcaster must be instantiated manually. It is
/// recommended this be done using the IRRServiceObject interface in
/// the overridden IRRServiceObject.RRServiceObjectInit() function. This
/// function is called after the pipes have been instantiated by the service.
/// </para>
/// <para>
/// Use SendPacket() or AsyncSendPacket() to broadcast packets to all
/// connected clients.
/// </para>
/// <para>
/// PipeBroadcaster provides flow control by optionally tracking how many packets
/// are in flight to each client pipe endpoint. (This is accomplished using packet acks.) If a
/// maximum backlog is specified, pipe endpoints exceeding this count will stop sending packets.
/// Specify the maximum backlog using the Init() function or the SetMaxBacklog() function.
/// </para>
/// <para>
/// The rate that packets are sent can be regulated using a callback function configured
/// with the Predicate property, or using the BroadcastDownsampler class.
/// </para>
/// </remarks>
/// <typeparam name="T">The packet data type</typeparam>
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

    /// <summary>
    /// Construct a new PipeBroadcaster
    /// </summary>
    /// <remarks>None</remarks>
    /// <param name="pipe">The pipe to use for broadcasting. Must be a pipe from a service object.
    /// Specifying a client pipe will result in an exception.</param>
    /// <param name="maximum_backlog">The maximum number of packets in flight, or -1 for unlimited</param>
    public PipeBroadcaster(Pipe<T> pipe, int maximum_backlog = -1)
    {
        this.pipe = pipe;
        this.innerpipe = new WrappedPipeBroadcaster();
        this.innerpipe.Init((WrappedPipeServer)pipe.innerpipe, maximum_backlog);
        this.type = ((WrappedPipeServer)pipe.innerpipe).Type;
    }

    /// <summary>
    /// Get the associated pipe
    /// </summary>
    /// <remarks>None</remarks>
    public Pipe<T> Pipe
    {
        get {
            return pipe;
        }
    }

    /// <summary>
    /// Asynchronously send packet to all connected pipe endpoint clients
    /// </summary>
    /// <remarks>
    /// Async version of SendPacket()
    /// </remarks>
    /// <param name="packet">The packet to send</param>
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

    /// <summary>
    /// Send a packet to all connected pipe endpoint clients
    /// </summary>
    /// <remarks>
    /// Blocks until packet has been sent by all endpoints
    /// </remarks>
    /// <param name="packet">The packet to send</param>
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

    /// <summary>
    /// Get the number of active client endpoints
    /// </summary>
    /// <remarks>None</remarks>
    public uint ActivePipeEndpointCount
    {
        get {
            return innerpipe.GetActivePipeEndpointCount();
        }
    }

    /// <summary>
    /// Set the predicate callback function
    /// </summary>
    /// <remarks>
    /// <para>
    /// A predicate is optionally used to regulate when packets are sent to clients. This is used by the
    /// BroadcastDownsampler to regulate update rates of packets sent to clients.
    /// </para>
    /// <para>
    /// The predicate callback is invoked before the broadcaster sends a packet to an endpoint. If the predicate returns
    /// true, the packet will be sent. If it is false, the packet will not be sent to that endpoint.
    /// </para>
    /// <para>
    /// It receives the broadcaster, the client endpoint ID, and the pipe endpoint index. It returns true to send the
    /// packet, or false to not send the packet.
    /// </para>
    /// </remarks>
    /// <value></value>
    public Func<uint, int, bool> Predicate
    {
        set {
            var p = new WrappedPipeBroadcasterPredicateDirectorNET(value);
            int id = RRObjectHeap.AddObject(p);
            innerpipe.SetPredicateDirector(p, id);
        }
    }

    /// <summary>
    /// Get or set the maximum backlog
    /// </summary>
    /// <remarks>
    /// PipeBroadcaster provides flow control by optionally tracking how many packets
    /// are in flight to each client pipe endpoint. (This is accomplished using packet acks.) If a
    /// maximum backlog is specified, pipe endpoints exceeding this count will stop sending packets.
    /// Use -1 for infinite packets in flight.
    /// </remarks>
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

/// <summary>
/// `wire` member type interface
/// </summary>
/// <remarks>
/// <para>
/// The Wire class implements the `wire` member type. Wires are declared in service definition files
/// using the `wire` keyword within object declarations. Wires provide "most recent" value streaming
/// between clients and services. They work by creating "connection" pairs between the client and service.
/// The wire streams the current value between the wire connection pairs using packets. Wires
/// are unreliable; only the most recent value is of interest, and any older values
/// will be dropped. Wire connections have an InValue and an OutValue. Users set the OutValue on the
/// connection. The new OutValue is transmitted to the peer wire connection, and becomes the peer's
/// InValue. The peer can then read the InValue. The client and service have their own InValue
/// and OutValue, meaning that each direction, client to service or service to client, has its own
/// value.
/// </para>
/// <para>
/// Wire connections are created using the Connect() or AsyncConnect() functions. Services receive
/// incoming connection requests through a callback function. Thes callback is configured using
/// the SetWireConnectCallback() function. Services may also use the WireBroadcaster class
/// or WireUnicastReceiver class to automate managing wire connection lifecycles. WireBroadcaster
/// is used to send values to all connected clients. WireUnicastReceiver is used to receive the
/// value from the most recent wire connection. See WireConnection for details on sending
/// and receiving streaming values.
/// </para>
/// <para>
/// Wire clients may also optionally "peek" and "poke" the wire without forming a streaming
/// connection. This is useful if the client needs to read the InValue or set the OutValue
/// instantaniously, but does not need continuous updating. PeekInValue() or
/// AsyncPeekInValue() will retrieve the client's current InValue. PokeOutValue() or
/// AsyncPokeOutValue() will send a new client OutValue to the service.
/// PeekOutValue() or AsyncPeekOutValue() will retrieve the last client OutValue received by
/// the service.
/// </para>
/// <para>
/// "Peek" and "poke" operations initiated by the client are received on the service using
/// callbacks. Use SetPeekInValueCallback(), SetPeekOutValueCallback(),
/// and SetPokeOutValueCallback() to configure the callbacks to handle these requests.
/// WireBroadcaster and WireUnicastReceiver configure these callbacks automatically, so
/// the user does not need to configure the callbacks when these classes are used.
/// </para>
/// <para>
/// Wires can be declared*readonly* or*writeonly*. If neither is specified, the wire is assumed
/// to be full duplex.*readonly* pipes may only send values from service to client, ie OutValue
/// on service side and InValue on client side.*writeonly* pipes may only send values from
/// client to service, ie OutValue on client side and InValue on service side. Use Direction()
/// to determine the direction of the wire.
/// </para>
/// <para>
/// Unlike pipes, wire connections are not indexed, so only one connection pair can be
/// created per client connection.
/// </para>
/// <para>
/// WireBroadcaster or WireUnicastReceiver are typically used to simplify using wires.
/// See WireBroadcaster and WireUnicastReceiver for more information.
/// </para>
/// <para>
/// This class is instantiated by the node. It should not be instantiated by the user.
/// </para>
/// </remarks>
/// <typeparam name="T">The value data type</typeparam>
public class Wire<T>
{
    internal object innerwire;

    string m_Membername;
    MemberDefinition_Direction direction;

    /// <summary>
    /// Get the member name of the wire
    /// </summary>
    /// <remarks>None</remarks>
    public string MemberName
    {
        get {
            return this.m_Membername;
        }
    }

    /// <summary>
    /// Get the direction of the wire
    /// </summary>
    /// <remarks>
    /// Wires may be declared*readonly* or*writeonly* in the service definition file. (If neither
    /// is specified, the wire is assumed to be full duplex.)*readonly* wire may only send out values from
    /// service to client.*writeonly* wires may only send out values from client to service.
    /// </remarks>
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

    /// <summary>
    /// Connect the wire
    /// </summary>
    /// <remarks>
    /// <para>
    /// Creates a connection between the wire, returning the client connection. Used to create
    /// a "most recent" value streaming connection to the service.
    /// </para>
    /// <para>
    /// Only valid on clients. Will throw InvalidOperationException on the service side.
    /// </para>
    /// <para>
    /// Note: If a streaming connection is not required, use PeekInValue(), PeekOutValue(),
    /// or PokeOutValue() instead of creating a connection.
    /// </para>
    /// </remarks>
    /// <returns>The wire connection</returns>
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

    /// <summary>
    /// Asynchronously connect the wire
    /// </summary>
    /// <remarks>
    /// <para>
    /// Same as Connect(), but async
    /// </para>
    /// <para>
    /// Only valid on clients. Will throw InvalidOperationException on the service side.
    /// </para>
    /// </remarks>
    /// <param name="timeout">Timeout in milliseconds, or RR_TIMEOUT_INFINITE for no timeout</param>
    /// <returns>Task that upon completion returns the connected WireConnection</returns>
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

    /// <summary>
    /// Set wire connected callback function
    /// </summary>
    /// <remarks>
    /// <para>
    /// Callback function invoked when a client attempts to connect a the wire. The callback
    /// will receive the incoming wire connection as a parameter. The service must maintain a
    /// reference to the wire connection, but the wire will retain ownership of the wire connection
    /// until it is closed. Using  boost::weak_ptr to store the reference to the connection
    /// is recommended.
    /// </para>
    /// <para>
    /// The callback may throw an exception to reject incoming connect request.
    /// </para>
    /// <para>
    /// Note: Connect callback is configured automatically by WireBroadcaster or
    /// WireUnicastReceiver
    /// </para>
    /// <para>
    /// Only valid for services. Will throw InvalidOperationException on the client side.
    /// </para>
    /// </remarks>
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

    /// <summary>
    /// Peek the current InValue
    /// </summary>
    /// <remarks>
    /// <para>
    /// Peeks the current InValue using a "request" instead of a streaming value. Use
    /// if only the instantanouse value is required.
    /// </para>
    /// <para>
    /// Peek and poke are similar to `property` members. Unlike streaming,
    /// peek and poke are reliable operations.
    /// </para>
    /// <para>
    /// Throws ValueNotSetException if InValue is not valid.
    /// </para>
    /// <para>
    /// Only valid on clients. Will throw InvalidOperationException on the service side.
    /// </para>
    /// </remarks>
    /// <param name="ts">[out] The timestamp of the current InValue</param>
    /// <returns>The current InValue</returns>
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

    /// <summary>
    /// Peek the current OutValue
    /// </summary>
    /// <remarks>
    /// <para>
    /// Peeks the current OutValue using a "request" instead of a streaming value. Use
    /// if only the instantanouse value is required.
    /// </para>
    /// <para>
    /// Peek and poke are similar to `property` members. Unlike streaming,
    /// peek and poke are reliable operations.
    /// </para>
    /// <para>
    /// Throws ValueNotSetException if OutValue is not valid.
    /// </para>
    /// <para>
    /// Only valid on clients. Will throw InvalidOperationException on the service side.
    /// </para>
    /// </remarks>
    /// <param name="ts">[out] The timestamp of the current OutValue</param>
    /// <returns>The current OutValue</returns>
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

    /// <summary>
    /// Poke the OutValue
    /// </summary>
    /// <remarks>
    /// <para>
    /// Pokes the OutValue using a "request" instead of a streaming value. Use
    /// to update the OutValue if the value is updated infrequently.
    /// </para>
    /// <para>
    /// Peek and poke are similar to `property` members. Unlike streaming,
    /// peek and poke are reliable operations.
    /// </para>
    /// <para>
    /// Only valid on clients. Will throw InvalidOperationException on the service side.
    /// </para>
    /// </remarks>
    /// <param name="value">The new OutValue</param>
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

    /// <summary>
    /// Asynchronously peek the current InValue
    /// </summary>
    /// <remarks>
    /// <para>
    /// Same as PeekInValue(), but returns asynchronously.
    /// </para>
    /// <para>
    /// Only valid on clients. Will throw InvalidOperationException on the service side.
    /// </para>
    /// </remarks>
    /// <param name="timeout">Timeout in milliseconds, or RR_TIMEOUT_INFINITE for no timeout</param>
    /// <returns>Task that upon completion returns the packet and timespec</returns>
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

    /// <summary>
    /// Asynchronously peek the current OutValue
    /// </summary>
    /// <remarks>
    /// <para>
    /// Same as PeekOutValue(), but async.
    /// </para>
    /// <para>
    /// Only valid on clients. Will throw InvalidOperationException on the service side.
    /// </para>
    /// </remarks>
    /// <param name="timeout">Timeout in milliseconds, or RR_TIMEOUT_INFINITE for no timeout</param>
    /// <returns>Task that upon completion returns the packet and timespec</returns>
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

    /// <summary>
    /// Asynchronously poke the OutValue
    /// </summary>
    /// <remarks>
    /// <para>
    /// Same as PokeOutValue(), but async
    /// </para>
    /// <para>
    /// Only valid on clients. Will throw InvalidOperationException on the service side.
    /// </para>
    /// </remarks>
    /// <param name="value">The new OutValue</param>
    /// <param name="timeout">Timeout in milliseconds, or RR_TIMEOUT_INFINITE for no timeout</param>
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

    /// <summary>
    /// Set the PeekInValue callback function
    /// </summary>
    /// <remarks>
    /// <para>
    /// Peek and poke operations are used when a streaming connection of the most recent value
    /// is not required. Clients initiate peek and poke operations using PeekInValue(), PeekOutValue(),
    /// PokeOutValue(), or their asynchronous equivalents. Services receive the peek and poke
    /// requests through callbacks.
    /// </para>
    /// <para>
    /// PeekInValueCallback configures the service callback for PeekInValue() requests.
    /// </para>
    /// <para>
    /// The specified callback function should have the following signature:
    /// </para>
    /// <para>
    /// T peek_invalue_callback(uint client_endpoint);
    /// </para>
    /// <para>
    /// The function receives the client endpoint ID, and returns the current InValue.
    /// </para>
    /// <para>
    /// Note: Callback is configured automatically by WireBroadcaster or
    /// WireUnicastReceiver
    /// </para>
    /// <para>
    /// Only valid for services. Will throw InvalidOperationException on the client side.
    /// </para>
    /// </remarks>
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

    /// <summary>
    /// Set the PeekOutValue callback function
    /// </summary>
    /// <remarks>
    /// <para>
    /// Peek and poke operations are used when a streaming connection of the most recent value
    /// is not required. Clients initiate peek and poke operations using PeekInValue(), PeekOutValue(),
    /// PokeOutValue(), or their asynchronous equivalents. Services receive the peek and poke
    /// requests through callbacks.
    /// </para>
    /// <para>
    /// PeekOutValueCallback configures the service callback for PeekOutValue() requests.
    /// </para>
    /// <para>
    /// The specified callback function should have the following signature:
    /// </para>
    /// <para>
    ///     T peek_outvalue_callback(uint client_endpoint);
    /// </para>
    /// <para>
    /// The function receives the client endpoint ID, and returns the current OutValue.
    /// </para>
    /// <para>
    /// Note: Callback is configured automatically by WireBroadcaster or
    /// WireUnicastReceiver
    /// </para>
    /// <para>
    /// Only valid for services. Will throw InvalidOperationException on the client side.
    /// </para>
    /// </remarks>
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

    /// <summary>
    /// Set the PokeOutValue callback function
    /// </summary>
    /// <remarks>
    /// <para>
    /// Peek and poke operations are used when a streaming connection of the most recent value
    /// is not required. Clients initiate peek and poke operations using PeekInValue(), PeekOutValue(),
    /// PokeOutValue(), or their asynchronous equivalents. Services receive the peek and poke
    /// requests through callbacks.
    /// </para>
    /// <para>
    /// PokeOutValueCallback configures the service callback for PokeOutValue() requests.
    /// </para>
    /// <para>
    /// The specified callback function should have the following signature:
    /// </para>
    /// <para>
    ///     void poke_outvalue_callback( T, TimeSpec timestamp, uint client_endpoint);
    /// </para>
    /// <para>
    /// The function receives the new out value, the new out value timestamp in the client's clock,
    /// and the client endpoint ID.
    /// </para>
    /// <para>
    /// Note: Callback is configured automatically by WireBroadcaster or
    /// WireUnicastReceiver
    /// </para>
    /// <para>
    /// Only valid for services. Will throw InvalidOperationException on the client side.
    /// </para>
    /// </remarks>
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

    /// <summary>
    /// Wire connection used to transmit "most recent" values
    /// </summary>
    /// <remarks>
    /// <para>
    /// Wire connections are used to transmit "most recent" values between connected
    /// wire members. See Wire for more information on wire members.
    /// </para>
    /// <para>
    /// Wire connections are created by clients using the Wire::Connect() or Wire::AsyncConnect()
    /// functions. Services receive incoming wire connection requests through a
    /// callback function specified using the Wire::SetWireConnectCallback() function. Services
    /// may also use the WireBroadcaster class to automate managing wire connection lifecycles and
    /// sending values to all connected clients, or use WireUnicastReceiver to receive an incoming
    /// value from the most recently connected client.
    /// </para>
    /// <para>
    /// Wire connections are used to transmit "most recent" values between clients and services. Connection
    /// the wire creates a connection pair, one in the client, and one in the service. Each wire connection
    /// object has an InValue and an OutValue. Setting the OutValue of one will cause the specified value to
    /// be transmitted to the InValue of the peer. See Wire for more information.
    /// </para>
    /// <para>
    /// Values can optionally be specified to have a finite lifespan using SetInValueLifespan() and
    /// OutValueLifespan. Lifespans can be used to prevent using old values that have
    /// not been recently updated.
    /// </para>
    /// <para>
    /// This class is instantiated by the Wire class. It should not be instantiated
    /// by the user.
    /// </para>
    /// </remarks>
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

        /// <summary>
        /// The direction of the wire
        /// </summary>
        /// <remarks>
        /// Wires may be declared*readonly* or*writeonly* in the service definition file. (If neither
        /// is specified, the wire is assumed to be full duplex.)*readonly* wire may only send out values from
        /// service to client.*writeonly* wires may only send out values from client to service.
        /// </remarks>
        public MemberDefinition_Direction Direction
        {
            get {
                return innerwire.Direction();
            }
        }

        /// <summary>
        /// Get the Robot Raconteur node Endpoint ID
        /// </summary>
        /// <remarks>
        /// Returns the endpoint associated with the ClientContext or ServerEndpoint
        /// associated with the wire connection.
        /// </remarks>
        public uint Endpoint
        {
            get {
                return innerwire.GetEndpoint();
            }
        }

        /// <summary>
        /// Get the current InValue
        /// </summary>
        /// <remarks>
        /// Gets the current InValue that was transmitted from the peer. Throws
        /// ValueNotSetException if no value has been received, or the most
        /// recent value lifespan has expired.
        /// </remarks>
        public T InValue
        {
            get {
                using (MessageElement m = innerwire.GetInValue())
                {
                    return RobotRaconteurNode.s.UnpackAnyType<T>(m);
                }
            }
        }

        /// <summary>
        /// Get or set the current OutValue
        /// </summary>
        /// <remarks>
        /// <para>
        /// Gets the current OutValue that was transmitted to the peer. Throws
        /// ValueNotSetException if no value has been received, or the most
        /// recent value lifespan has expired.
        /// </para>
        /// <para>
        /// Setting the OutValue for the wire connection. The specified value will be
        /// transmitted to the peer, and will become the peers InValue. The transmission
        /// is unreliable, meaning that values may be dropped if newer values arrive.
        /// </para>
        /// </remarks>
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

        /// <summary>
        /// Get the timestamp of the last received value
        /// </summary>
        /// <remarks>
        /// Returns the timestamp of the value in the///senders* clock
        /// </remarks>
        public TimeSpec LastValueReceivedTime
        {
            get {
                return innerwire.GetLastValueReceivedTime();
            }
        }

        /// <summary>
        /// Get the timestamp of the last sent value
        /// </summary>
        /// <remarks>
        /// Returns the timestamp of the last sent value in the///local* clock
        /// </remarks>
        public TimeSpec LastValueSentTime
        {
            get {
                return innerwire.GetLastValueSentTime();
            }
        }

        /// <summary>
        /// Try getting the InValue, returning true on success or false on failure
        /// </summary>
        /// <remarks>
        /// Get the current InValue and InValue timestamp. Return true or false on
        /// success or failure instead of throwing exception.
        /// </remarks>
        /// <param name="value">[out] The current InValue</param>
        /// <param name="ts">[out] The current InValue timestamp in the senders clock</param>
        /// <returns>true if the value is valid, otherwise false</returns>
        public bool TryGetInValue(out T value, out TimeSpec ts)
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

        /// <summary>
        /// Try getting the OutValue, returning true on success or false on failure
        /// </summary>
        /// <remarks>
        /// Get the current OutValue and OutValue timestamp. Return true or false on
        /// success and failure instead of throwing exception.
        /// </remarks>
        /// <param name="value">[out] The current OutValue</param>
        /// <param name="ts">[out] The current OutValue timestamp in the local clock</param>
        /// <returns>true if the value is valid, otherwise false</returns>
        public bool TryGetOutValue(out T value, out TimeSpec ts)
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

        /// <summary>
        /// Close the wire connection
        /// </summary>
        /// <remarks>
        /// Close the wire connection. Blocks until close complete. The peer wire connection
        /// is destroyed automatically.
        /// </remarks>
        public void Close()
        {
            innerwire.Close();
        }

        /// <summary>
        /// Asynchronously close the wire connection
        /// </summary>
        /// <remarks>
        /// Same as Close() but async
        /// </remarks>
        /// <param name="timeout">Timeout in milliseconds, or RR_TIMEOUT_INFINITE for no timeout</param>
        public async Task AsyncClose(int timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE)
        {
            AsyncVoidReturnDirectorImpl h = new AsyncVoidReturnDirectorImpl();
            int id = RRObjectHeap.AddObject(h);
            innerwire.AsyncClose(timeout, h, id);
            await h.Task;
        }

        /// <summary>
        /// Event invoked when the InValue is changed
        /// </summary>
        /// <remarks>
        /// Callback function must accept three arguments, receiving the WireConnection that
        /// received a packet, the new value, and the value's TimeSpec timestamp
        /// </remarks>
        public event Action<Wire<T>.WireConnection, T, TimeSpec> WireValueChanged;

        private Action<WireConnection> close_callback;

        /// <summary>
        /// Get or set the connection closed callback function
        /// </summary>
        /// <remarks>
        /// <para>
        /// Sets a function to invoke when the wire connection has been closed.
        /// </para>
        /// <para>
        /// Callback function must accept one argument, receiving the WireConnection that
        /// was closed.
        /// </para>
        /// </remarks>
        public Action<WireConnection> WireCloseCallback
        {
            get {
                return close_callback;
            }
            set {
                close_callback = value;
            }
        }

        /// <summary>
        /// Get if the InValue is valid
        /// </summary>
        /// <remarks>
        /// The InValue is valid if a value has been received and
        /// the value has not expired
        /// </remarks>
        public bool InValueValid
        {
            get {
                return innerwire.GetInValueValid();
            }
        }

        /// <summary>
        /// Get if the OutValue is valid
        /// </summary>
        /// <remarks>
        /// The OutValue is valid if a value has been
        /// set using OutValue
        /// </remarks>
        public bool OutValueValid
        {
            get {
                return innerwire.GetOutValueValid();
            }
        }

        /// <summary>
        /// Get or set whether wire connection should ignore incoming values
        /// </summary>
        /// <remarks>
        /// Wire connections may optionally desire to ignore incoming values. This is useful if the connection
        /// is only being used to send out values, and received values may create a potential memory . If ignore is
        /// true, incoming values will be discarded.
        /// </remarks>
        public bool IgnoreInValue
        {
            get {
                return innerwire.GetIgnoreInValue();
            }
            set {
                innerwire.SetIgnoreInValue(value);
            }
        }

        /// <summary>
        /// Get or set the lifespan of InValue
        /// </summary>
        /// <remarks>
        /// <para>
        /// InValue may optionally have a finite lifespan specified in milliseconds. Once
        /// the lifespan after reception has expired, the InValue is cleared and becomes invalid.
        /// Attempts to access InValue will result in ValueNotSetException.
        /// </para>
        /// <para>
        /// InValue lifespans may be used to avoid using a stale value received by the wire. If
        /// the lifespan is not set, the wire will continue to return the last received value, even
        /// if the value is old.
        /// </para>
        /// <para>
        /// The lifespan in millisecond, or RR_VALUE_LIFESPAN_INFINITE for infinite lifespan
        /// </para>
        /// </remarks>
        public int InValueLifespan
        {
            get {
                return innerwire.GetInValueLifespan();
            }
            set {
                innerwire.SetInValueLifespan(value);
            }
        }

        /// <summary>
        /// Get or set the lifespan of OutValue
        /// </summary>
        /// <remarks>
        /// <para>
        /// OutValue may optionally have a finite lifespan specified in milliseconds. Once
        /// the lifespan after sending has expired, the OutValue is cleared and becomes invalid.
        /// Attempts to access OutValue will result in ValueNotSetException.
        /// </para>
        /// <para>
        /// OutValue lifespans may be used to avoid using a stale value sent by the wire. If
        /// the lifespan is not set, the wire will continue to return the last sent value, even
        /// if the value is old.
        /// </para>
        /// <para>
        /// The lifespan in millisecond, or RR_VALUE_LIFESPAN_INFINITE for infinite lifespan
        /// </para>
        /// </remarks>
        public int OutValueLifespan
        {
            get {
                return innerwire.GetOutValueLifespan();
            }
            set {
                innerwire.SetOutValueLifespan(value);
            }
        }

        /// <summary>
        /// Waits for InValue to be valid
        /// </summary>
        /// <remarks>
        /// Blocks the current thread until InValue is valid,
        /// with an optional timeout. Returns true if InValue is valid,
        /// or false if timeout occurred.
        /// </remarks>
        /// <param name="timeout">Timeout in milliseconds, or RR_TIMEOUT_INFINITE for no timeout</param>
        /// <returns>true if InValue is valid, otherwise false</returns>
        public bool WaitInValueValid(int timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE)
        {
            return innerwire.WaitInValueValid(timeout);
        }

        /// <summary>
        /// Waits for OutValue to be valid
        /// </summary>
        /// <remarks>
        /// Blocks the current thread until OutValue is valid,
        /// with an optional timeout. Returns true if OutValue is valid,
        /// or false if timeout occurred.
        /// </remarks>
        /// <param name="timeout">Timeout in milliseconds, or RR_TIMEOUT_INFINITE for no timeout</param>
        /// <returns>true if InValue is valid, otherwise false</returns>
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

/// <summary>
/// Broadcaster to send values to all connected clients
/// </summary>
/// <remarks>
/// <para>
/// WireBroadcaster is used by services to send values to all
/// connected client endpoints. It attaches to the wire on the service
/// side, and manages the lifecycle of connections. WireBroadcaster
/// should only we used with wires that are declared*readonly*, since
/// it has no provisions for receiving incoming values from clients.
/// </para>
/// <para>
/// WireBroadcaster is initialized by the user, or by default implementation
/// classes generated by RobotRaconteurGen (*_default_impl). Default
/// implementation classes will automatically instantiate broadcasters for
/// wires marked*readonly*. If default implementation classes are
/// not used, the broadcaster must be instantiated manually. It is recommended this
/// be done using the IRRServiceObject interface in the overridden
/// IRRServiceObject::RRServiceObjectInit() function. This function is called after
/// the wires have been instantiated by the service.
/// </para>
/// <para>
/// Set the OutValue property to broadcast values to all connected clients.
/// </para>
/// <para>
/// The rate that packets are sent can be regulated using a callback function configured
/// with the Predicate property, or using the BroadcastDownsampler class.
/// </para>
/// </remarks>
/// <typeparam name="T">The value data type</typeparam>
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

    /// <summary>
    /// Construct a new WireBroadcaster
    /// </summary>
    /// <remarks>None</remarks>
    /// <param name="wire">The wire to use for broadcasting. Must be a wire from a service object.
    /// Specifying a client wire will result in an exception.</param>
    public WireBroadcaster(Wire<T> wire)
    {
        this.wire = wire;
        this.innerwire = new WrappedWireBroadcaster();
        this.innerwire.Init((WrappedWireServer)wire.innerwire);
        this.type = ((WrappedWireServer)wire.innerwire).Type;
    }

    /// <summary>
    /// Get the assosciated wire
    /// </summary>
    /// <remarks>None</remarks>
    public Wire<T> Wire
    {
        get {
            return wire;
        }
    }

    /// <summary>
    /// Set the OutValue for all connections
    /// </summary>
    /// <remarks>
    /// <para>
    /// Sets the OutValue for all connections. This will transmit the value
    /// to all connected clients using packets. The value will become the clients'
    /// InValue.
    /// </para>
    /// <para>
    /// The value will be returned when clients call Wire.PeekInValue() or
    /// Wire.AsyncPeekInValue()
    /// </para>
    /// </remarks>
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

    /// <summary>
    /// Get the current number of active wire connections
    /// </summary>
    /// <remarks>None</remarks>
    public uint ActiveWireConnectionCount
    {
        get {
            return innerwire.GetActiveWireConnectionCount();
        }
    }

    /// <summary>
    /// Set the predicate callback function
    /// </summary>
    /// <remarks>
    /// <para>
    /// A predicate is optionally used to regulate when values are sent to clients. This is used by the
    /// BroadcastDownsampler to regulate update rates of values sent to clients.
    /// </para>
    /// <para>
    /// The predicate callback is invoked before the broadcaster sets the OutValue of a connection. If the predicate
    /// returns true, the OutValue packet will be sent. If it is false, the OutValue packet will not be sent to that
    /// endpoint. The predicate callback must have the following signature:
    /// </para>
    /// <para>
    ///     bool broadcaster_predicate(WireBroadcaster broadcaster, uint client_endpoint);
    /// </para>
    /// <para>
    /// It receives the broadcaster and the client endpoint ID. It returns true to send the OutValue packet,
    /// or false to not send the OutValue packet.
    /// </para>
    /// </remarks>
    public Func<uint, bool> Predicate
    {
        set {
            var p = new WrappedWireBroadcasterPredicateDirectorNET(value);
            int id = RRObjectHeap.AddObject(p);
            innerwire.SetPredicateDirector(p, id);
        }
    }

    /// <summary>
    /// Get or set the lifespan of OutValue
    /// </summary>
    /// <remarks>
    /// <para>
    /// OutValue may optionally have a finite lifespan specified in milliseconds. Once
    /// the lifespan after sending has expired, the OutValue is cleared and becomes invalid.
    /// Attempts to access OutValue will result in ValueNotSetException.
    /// </para>
    /// <para>
    /// OutValue lifespans may be used to avoid using a stale value sent by the wire. If
    /// the lifespan is not set, the wire will continue to return the last sent value, even
    /// if the value is old.
    /// </para>
    /// <para>
    /// The lifespan in millisecond, or RR_VALUE_LIFESPAN_INFINITE for infinite lifespan
    /// </para>
    /// </remarks>
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

/// <summary>
/// Receive the InValue from the most recent connection
/// </summary>
/// <remarks>
/// <para>
/// WireUnicastReceiver is used by services to receive a value from a single client.
/// When a client sets its OutValue, this value is transmitted to the service using
/// packets, and becomes the service's InValue for that connection. Service wires
/// can have multiple active clients, so the service needs to choose which connection
/// is "active". The WireUnicastReceiver selects the "most recent" connection, and
/// returns that connection's InValue. Any existing connections are closed.
/// WireUnicastReceiver should only be used with wires that are declared*writeonly*.
/// It is recommended that object locks be used to protect from concurrent
/// access when unicast receivers are used.
/// </para>
/// <para>
/// WireUnicastReceiver is initialized by the user, or by default implementation
/// classes generated by RobotRaconteurGen (*_default_impl). Default
/// implementation classes will automatically instantiate unicast receivers for
/// wires marked*writeonly*. If default implementation classes are
/// not used, the unicast receiver must be instantiated manually. It is recommended this
/// be done using the IRRServiceObject interface in the overridden
/// IRRServiceObject::RRServiceObjectInit() function. This function is called after
/// the wires have been instantiated by the service.
/// </para>
/// <para>
/// The current InValue is received using GetInValue() or TryGetInValue(). The
/// InValueChanged signal can be used to monitor for changes to the InValue.
/// </para>
/// <para>
/// Clients may also use PokeOutValue() or AsyncPokeOutValue() to update the
/// unicast receiver's value.
/// </para>
/// </remarks>
/// <typeparam name="T">The value type</typeparam>
public class WireUnicastReceiver<T>
{
    protected Wire<T> wire;
    protected WrappedWireUnicastReceiver innerwire;
    protected TypeDefinition type;

    /// <summary>
    /// Construct a new WireUnicastReceiverBase
    /// </summary>
    /// <remarks>None</remarks>
    /// <param name="wire">The wire to use for broadcasting. Must be a wire from a service object.
    /// Specifying a client wire will result in an exception.</param>
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

    /// <summary>
    /// Get the associated wire
    /// </summary>
    /// <remarks>None</remarks>
    public Wire<T> Wire
    {
        get {
            return wire;
        }
    }

    /// <summary>
    /// Get the current InValue
    /// </summary>
    /// <remarks>
    /// Gets the current InValue that was received from the active connection.
    /// Throws ValueNotSetException if no value has been received, or
    /// the most recent value lifespan has expired.
    /// </remarks>
    /// <param name="ts">[out] The current InValue timestamp</param>
    /// <param name="ep">[out] The client endpoint ID of the InValue</param>
    /// <returns>The current InValue</returns>
    public T GetInValue(out TimeSpec ts, out uint ep)
    {
        ts = new TimeSpec();
        var m = innerwire.GetInValue(ts, out ep);
        return RobotRaconteurNode.s.UnpackAnyType<T>(m);
    }

    /// <summary>
    /// Try getting the current InValue, returning true on success or false on failure
    /// </summary>
    /// <remarks>
    /// Gets the current InValue, its timestamp, and the client endpoint ID. Returns true if
    /// value is valid, or false if value is invalid. Value will be invalid if no value has
    /// been received, or the value lifespan has expired.
    /// </remarks>
    /// <param name="value">[out] The current InValue</param>
    /// <param name="time">[out] The current InValue timestamp</param>
    /// <param name="client">[out] The client endpoint ID of the InValue</param>
    /// <returns>true if value is valid, otherwise false</returns>
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

    /// <summary>
    /// Event fired when InValue has changed.
    /// </summary>
    /// <remarks>
    /// Callback function must accept three arguments, receiving the new value,
    /// value's TimeSpec timestamp, and the client endpoint ID.
    /// </remarks>
    public event Action<T, TimeSpec, uint> InValueChanged;

    /// <summary>
    /// Get or set the lifespan of InValue
    /// </summary>
    /// <remarks>
    /// <para>
    /// InValue may optionally have a finite lifespan specified in milliseconds. Once
    /// the lifespan after reception has expired, the InValue is cleared and becomes invalid.
    /// Attempts to access InValue will result in ValueNotSetException.
    /// </para>
    /// <para>
    /// InValue lifespans may be used to avoid using a stale value received by the wire. If
    /// the lifespan is not set, the wire will continue to return the last received value, even
    /// if the value is old.
    /// </para>
    /// <para>
    /// The lifespan in millisecond, or RR_VALUE_LIFESPAN_INFINITE for infinite lifespan
    /// </para>
    /// </remarks>
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

/// <summary>
/// Service object monitor lock notification
/// </summary>
/// <remarks>
/// Service objects must implement IRobotRaconteurMonitorObject for
/// monitor locking to function. Services call RobotRaconteurMonitorEnter()
/// with an optional timeout to request the lock, and call RobotRaconteurMonitorExit()
/// to release the monitor lock. RobotRaconteurMonitorEnter() should block
/// until a thread-exclusive lock can be established.
/// </remarks>
public interface IRobotRaconteurMonitorObject
{
    /// <summary>
    /// Request a thread-exclusive lock without timeout. May block until lock can be established
    /// </summary>
    /// <remarks>None</remarks>
    void RobotRaconteurMonitorEnter();

    /// <summary>
    /// Request a thread-exclusive lock with timeout. May block until lock can be established,
    /// up to the specified timeout.
    /// </summary>
    /// <remarks>None</remarks>
    /// <param name="timeout">Lock request timeout in milliseconds</param>
    void RobotRaconteurMonitorEnter(int timeout);

    /// <summary>
    /// Release the thread-exclusive monitor lock
    /// </summary>
    /// <remarks>None</remarks>
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

/// <summary>
/// Interface for custom user authenticators
/// </summary>
/// <remarks>None</remarks>
public interface IUserAuthenticator
{
    /// <summary>
    /// Authenticate a user using username and credentials
    /// </summary>
    /// <remarks>
    /// The service will call AuthenticateUser when clients attempt to authenticate. The
    /// username and credentials are passed to RobotRaconteurNode.ConnectService() or
    /// RobotRaconteurNode.AsyncConnectService(). The authenticator must analyze these
    /// values to decide how to authenticate the user.
    ///
    /// On successful authentication, return a populated AuthenticatedUser object. On
    /// failure, throw an exception.
    ///
    /// See security for more information.
    /// </remarks>
    /// <param name="username">The username provided by the client</param>
    /// <param name="credentials">The credentials provided by the client</param>
    /// <param name="context">The context of the service requesting authentication</param>
    /// <returns>An authenticated user object</returns>
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
    /// <summary>
    /// The authenticated username
    /// </summary>
    /// <remarks>None</remarks>
    public string Username
    {
        get {
            return _GetUsername();
        }
    }

    /// <summary>
    /// The user privileges
    /// </summary>
    /// <remarks>None</remarks>
    public string[] Privileges
    {
        get {
            vectorstring s = _GetPrivileges();
            string[] o = new string[s.Count];
            s.CopyTo(o);
            return o;
        }
    }

    /// <summary>
    /// The user properties
    /// </summary>
    /// <remarks>None</remarks>
    public string[] Properties
    {
        get {
            vectorstring s = _GetProperties();
            string[] o = new string[s.Count];
            s.CopyTo(o);
            return o;
        }
    }

    /// <summary>
    /// The user login time
    /// </summary>
    /// <remarks>None</remarks>
    public DateTime LoginTime
    {
        get {
            return _GetLoginTime();
        }
    }

    /// <summary>
    /// The user last access time
    /// </summary>
    /// <remarks>None</remarks>
    public DateTime LastAccessTime
    {
        get {
            return _GetLastAccessTime();
        }
    }
}

/// <summary>
///  Class for automatic broadcast downsampler stepping
/// </summary>
/// <remarks>
/// Helper class to automate BroadcastDownsampler stepping.
/// Calls BroadcastDownsampler.BeginStep() on construction,
/// and BroadcastDownsampler.EndStep() when disposed.
/// </remarks>
public class BroadcastDownsamplerStep : IDisposable
{
    protected BroadcastDownsampler downsampler;

    /// <summary>
    /// Construct a BroadcastDownsampler
    /// </summary>
    /// <remarks>
    /// Calls BroadcastDownsampler.BeginStep() on downsampler.
    /// Calls BroadcastDownsampler.EndStep() on downsampler
    /// when destroyed.
    /// </remarks>
    /// <param name="downsampler">The downsampler to step</param>
    public BroadcastDownsamplerStep(BroadcastDownsampler downsampler)
    {
        this.downsampler = downsampler;
        downsampler.BeginStep();
    }

    public void Dispose()
    {
        downsampler.EndStep();
    }
}

}
