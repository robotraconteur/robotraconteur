function c=RobotRaconteurWireSubscription(objecttype,stubid)
%RobotRaconteurWireSubscription Subscription for wire members that aggregates the values from client wire connections
%
%    Wire subscriptions are created using the RobotRaconteurServiceSubscription.SubscribeWire() function.
%    This function takes the name of the wire member and an optional service path of the service
%    object that owns the wire member.
%
%    Wire subscriptions aggregate the InValue from all active wire connections. When a client connects,
%    the wire subscriptions will automatically create wire connections to the wire member specified
%    when the WireSubscription was created using RobotRaconteurServiceSubscription.SubscribeWire(). The
%    InValue of all the active wire connections are collected, and the most recent one is used as the current
%    InValue of the wire subscription.
%
%    The lifespan of the InValue can be configured using InValueLifespan. It is recommended that
%    the lifespan be configured, so that the value will expire if the subscription stops receiving
%    fresh in values.
%
%    The wire subscription can also be used to set the OutValue of all active wire connections. This behaves
%    similar to a "reverse broadcaster", sending the same value to all connected services.
%
%    WireSubscription has the following properties:
%
%    wire_sub.InValue - (readonly) The current InValue
%    wire_sub.InValueValid - (readonly) true if there is a valid InValue, false otherwise
%    wire_sub.InValueLifespan - The lifespan of InValue, in seconds
%    wire_sub.IgnoreInValue - Set to true to ignore incoming InValue
%
%    The following functions are available:
%
%    wire_sub.Close()          - Close the wire connection
%    wire_sub.WaitInValueValid(timeout) - Wait for InValue to become valid with timeout in seconds
%    wire_sub.SetOutValueAll(new_value) - Send new OutValue to all connected services
%
%    See also RobotRaconteurServiceSubscription, RobotRaconteurWireConnection

c=struct;
c.rrobjecttype=objecttype;
c.rrstubid=stubid;

c=class(c,'RobotRaconteurWireSubscription');
