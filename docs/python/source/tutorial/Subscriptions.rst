Subscriptions
=============

Subscriptions are an extension of the node auto-discovery that automatically forms connections to detected nodes. A
subscription is created using the ``SubscribeServiceByType`` function in ``RobotRaconteurNode``. This function takes two
arguments: a list of service types, and an optional ``ServiceSubscriptionFilter``. The returned object is a
``ServiceSubscription``, which can be used to retrieve the connected services. The ``GetConnectedClients`` function
returns a ``dict`` of all the connected clients. The key contains the “NodeID” and service name of the connection. The
value is the connected client object reference, which can be used directly. The ``ClientConnected`` and
``ClientDisconnected`` events in ``ServiceSubscription`` can be used to detect when clients are connected or the
connection is lost.

Wires and Pipes can be subscribed once a ``ServiceSubscription`` is created. The ``SubscribeWire`` function takes the
name of the member and returns a ``WireSubscription`` that can be used to interact with all connected wires. The
``InValue`` property will return the “most recent” value received from all wires. The ``OutValue`` property will set all
wires to the specified value. For pipes, the ``SubscribePipe`` is used, and retuns a ``PipeSubscription``. This function
can receive packets using the ``ReceivePacket`` or ``TryReceivePacket`` functions. The received packets are not sorted
and all received packets are retrieved in the order they arrived. The ``SendPacket`` function can be used to send
packets to all connected services. Note that currently the Wire and Pipe subscriptions can only be made to the root
object. Wires and Pipes for objects retrieved using ObjRefs cannot be used.

Authentication information can be sent as part of ``ServiceSubscriptionFilter``.

The discovery information can be subscribed without creating connections to clients using the ``SubscribeServiceInfo2``
function in ``RobotRaconteurNode``. This is essentially a constantly running version of ``FindServiceByType``.
