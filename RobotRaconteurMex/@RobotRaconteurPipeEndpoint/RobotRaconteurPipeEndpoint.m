function c=RobotRaconteurPipeEndpoint(objecttype,stubid)
%RobotRaconteurPipeEndpoint Pipe endpoint used to transmit reliable or unreliable data streams
%    Pipe endpoints are used to communicate data between connected pipe members. Pipe endpoints
%    are created using a client connection returned from RobotRaconteur.ConnectService() using
%    the pipe's Connect() function. The following example connects to the "frame_stream"
%    pipe of a camera:
%
%        pipe_ep = camera_client.frame_stream.Connect(-1)
%
%    Pipe endpoints are *indexed*, meaning that more than one pipe endpoint pair can be created
%    using the same member. This means that multiple data streams can be created independent of
%    each other between the client and service using the same member. The parameter -1 in the 
%    Connect() example above is the pipe endpoint index. -1 allows the service to select the index,
%    and is used in most cases.
%
%    Pipes send reliable packet streams between connected client/service endpoint pairs.
%    Packets are sent using the SendPacket() functions. Packets
%    are read from the receive queue using the ReceivePacket(), ReceivePacketWait(), 
%    TryReceivePacketWait(), TryReceivePacketWait(), or PeekNextPacket(). The endpoint is closed
%    using the Close() function.
%
%    PipeEndpoint has the following read-only properties:
%
%    pipe_ep.Available - The number of packets in the receive queue
%    pipe_ep.IsUnreliable - 1 if the pipe is configured as unreliable, otherwise 0
%    pipe_ep.Direction - The direction of the pipe (0=both, 1=readonly, 2=writeonly)
%    
%    The following functions are available:
%
%    pipe_ep.ReceivePacket() - Receive the next pipe in the receive queue. Throws an error
%                              if receive queue is empty. Use pipe_ep.Available to check
%                              how many packets are available to receive.
%    pipe_ep.PeekNextPacket() - Same as pipe_ep.ReceivePacket(), but leaves the packet in
%                               the receive queue.
%    pipe_ep.ReceivePacketWait(timeout) - Same as pipe_ep.ReceivePacket(), but waits for a packet
%                               instead of throwing an error if the queue is empty. Optional timeout
%                               parameter in seconds.
%    pipe_ep.PeekNextPacketWait(timeout) - Same as pipe_ep.ReceivePacketWait(), but does not remove
%                               the packet from the receive queue. timeout parameter is optional.
%    pipe_ep.TryReceivePacketWait(timeout,peek) - Try receiving a packet, waiting for return. Returns
%                               a cell array with {success,packet}, where success will be false if the
%                               operation fails instead of throwing an error. Optional timeout parameter in 
%                               seconds. Optional peek, if true the packet will not be removed from
%                               receive queue.
%    pipe_ep.SendPacket(packet) - Send a packet to the service
%    pipe_ep.Close()          - Close the pipe endpoint 

c=struct;
c.rrobjecttype=objecttype;
c.rrstubid=stubid;

c=class(c,'RobotRaconteurPipeEndpoint');
