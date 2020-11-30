function c=RobotRaconteurPipeSubscription(objecttype,stubid)
%RobotRaconteurPipeSubscription Subscription for pipe members that aggregates incoming packets
%    Pipe subscriptions are created using the RobotRaconteurServiceSubscription.SubscribePipe() function. 
%    This function takes the the type of the pipe packets, the name of the pipe member, and an optional 
%    service path of the service object that owns the pipe member.
%
%    Pipe subscriptions collect all incoming packets from connect pipe endpoints. When a client connects,
%    the pipe subscription will automatically connect a pipe endpoint the pipe endpoint specified when
%    the PipeSubscription was created using RobotRaconteurServiceSubscription.SubscribePipe(). The packets 
%    received from each of the collected pipes are collected and placed into a common receive queue. This queue
%    is read using ReceivePacket(). The number of packets available to receive can be checked using Available.
%
%    Pipe subscriptions can also be used to send packets to all connected pipe endpoints. This is done
%    with the SendPacketAll() function. This function behaves somewhat like a "reverse broadcaster",
%    sending the packets to all connected services.
%
%    If the pipe subscription is being used to send packets but not receive them, the IgnoreInValue
%    should be set to true to prevent packets from queueing.
%
%    PipeSubscription has the following read-only properties:
%
%    pipe_sub.Available - The number of packets in the receive queue
%    pipe_sub.IgnoreInValue - Set to true to ignore incoming packets
%    
%    The following functions are available:
%
%    pipe_sub.ReceivePacket() - Receive the next pipe in the receive queue. Throws an error
%                              if receive queue is empty. Use pipe_sub.Available to check
%                              how many packets are available to receive.
%    pipe_sub.ReceivePacketWait(timeout) - Same as pipe_sub.ReceivePacket(), but waits for a packet
%                               instead of throwing an error if the queue is empty. Optional timeout
%                               parameter in seconds.
%
%    pipe_sub.Close()         - Close the pipe endpoint
%    pipe_sub.SendPacketAll(packet) - Send packet to all connected services
%
%    See also RobotRaconteurServiceSubscription

c=struct;
c.rrobjecttype=objecttype;
c.rrstubid=stubid;

c=class(c,'RobotRaconteurPipeSubscription');
