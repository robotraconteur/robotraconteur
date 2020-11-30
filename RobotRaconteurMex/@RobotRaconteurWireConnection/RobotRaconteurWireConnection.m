function c=RobotRaconteurWireConnection(objecttype,stubid)
%RobotRaconteurWireConnection Wire connection used to transmit "most recent" values
%    Wire connections are used to transmit "most recent" values between connected
%    wire members. Wire connectios are created using a client connection returned
%    from RobotRaconteur.ConnectService() using the wire's Connect() function.
%    The following example connects to the "robot_state" wire of a robot:
%
%        wire_con = robot_client.robot_state.Connect()
%
%    (Wire connections are not indexed, so there is no argument to Connect())
%
%    Wire connections are used to transmit "most recent" values between clients and services. Connecting
%    the wire creates a connection pair, one in the client, and one in the service. Each wire connection 
%    object has an InValue and an OutValue. Setting the OutValue of one will cause the specified value to
%    be transmitted to the InValue of the peer.
%
%    Values can optionally be specified to have a finite lifespan using InValueLifespan and
%    OutValueLifespan. Lifespans can be used to prevent using old values that have
%    not been recently updated.
%
%    Wire clients may also optionally "peek" and "poke" the wire without forming a streaming
%    connection. This is useful if the client needs to read the InValue or set the OutValue
%    instantaniously, but does not need continuous updating. PeekInValue() will retrieve the client's 
%    current InValue. PokeOutValue() will send a new client OutValue to the service. PeekOutValue() will 
%    retrieve the last client OutValue received by the service. Peek and poke operations are done using
%    the wire directly instead of connecting a wire connection. See below for more information.
%
%    WireConnection has the following properties:
%
%    wire_con.InValue - (readonly) The current InValue 
%    wire_con.OutValue - The current OutValue. Set to new value to transmit value to service
%    wire_con.LastValueReceivedTime - (readonly) The last value received time in the senders clock 
%    wire_con.LastValueSentTime - (readonly) The last value sent time in local node clock
%    wire_con.InValueValid - (readonly) true if there is a valid InValue, false otherwise
%    wire_con.OutValueValid - (readonly) true if there is a valid OutValue, false otherwise
%    wire_con.Direction - (readonly) The direction of the wire (0=both, 1=readonly, 2=writeonly)
%    wire_con.InValueLifespan - The lifespan of InValue, in seconds
%    wire_con.OutValueLifespan - The lifespan of OutValue, in seconds
%    
%    The following functions are available:
%
%    wire_con.Close()          - Close the wire connection
%    wire_con.WaitInValueValid(timeout) - Wait for InValue to become valid with timeout in seconds
%    wire_con.WaitOutValueValid(timeout) - Wait for OutValue to become valid with timeout in seconds
%
%    Peek and Poke functions using wire (not wire connection), while "client" and "wire_name" are the client
%    connection returned by RobotRaconteur.ConnectService(), and wire_name is the name of the wire member:
%
%    client.wire_name.PeekInValue() - Peek the current InValue synchronously from the service
%    client.wire_name.PeekOutValue() - Peek the current OutValue synchronously from the service
%    client.wire_name.PokeOutValue(new_value) - Poke the OutValue synchronously to the service
%

c=struct;
c.rrobjecttype=objecttype;
c.rrstubid=stubid;

c=class(c,'RobotRaconteurWireConnection');
