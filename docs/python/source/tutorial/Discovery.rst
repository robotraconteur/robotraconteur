.. _autodiscovery:

Service Discovery
======================

A powerful feature of Robot Raconteur is the ability to detect services automatically. Each transport has a method to
broadcast what services are available. For the ``TcpChannel`` this is accomplished through broadcast UDP packets. Other
transports will use the method most appropriate for the transport technology.

The first step in the auto-discovery process for TCP is for the node containing the service to broadcast an announcement
that the node is available. Version 0.9 and up use a request-response method for autodiscovery. Nodes send a broadcast
UDP packet every 55 seconds to announce that they are available and still listening. This low frequency broadcast is not
frequent enough for nodes searching for services, so broadcasting nodes will also wait for request broadcast packets.
Clients will send requests packets when they begin searching for services. Service nodes will then respond with their
connection information, after a random backoff period to prevent congestion. By default, the TCP discovery is only
enabled for IPv6 to reduce network traffic. IPv4 can be enabled by specifying different flags to the ``TcpTransport`` or
``ServerNodeSetup``.

For the ``LocalTransport``, the discovery information is stored on the filesystem.

The packet sent by the service nodes contains the ``NodeName``, ``NodeID``, and a URL to connect to the “Service Index",
which is a special service that lists the services registered in the node. The client will interrogate the service nodes
it has discovered to determine the available services. The “Service Index" is registered automatically
by the node and does not require any extra configuration by the user.

The auto-discovery functionality is automatically enabled by ``ServerNodeSetup`` and the
``from RobotRaconteur.Client import *`` functions. To manually enable auto-discovery on transports, use:

.. code:: python

  t=RR.TcpTransport()
  t.EnableNodeDiscoveryListening()

For the service, use:

.. code:: python

  t=RR.TcpTransport()
  t.EnableNodeAnnounce()

To find a service, use the command:

.. code:: python

  res=RRN.FindServiceByType("experimental.create.Create",["rr+local","rr+tcp","rrs+tcp"])

where “experimental.create2.Create" is replaced with the fully qualified type being searched for and the second
parameter is a list of the transport types to search. ``res`` is a list of ``ServiceInfo2`` structures that contains the
``NodeID``, ``NodeName``, ``Name``, ``RootObjectType``, ``RootObjectImplements``, ``ConnectionURL`` (list), and the
``Attributes``. The attributes entry is type ``varvalue{string}`` but should only contain type ``string`` and numeric
entries. This is used to help identify the correct service to connect to. Service attributes are set through the
``ServerContext`` object that is returned when a service is registered. A short example:

.. code:: python
  context=RRN.RegisterService ("Create","experimental.create.Create",obj)
  attributes={"RobotName" : RR.RobotRaconteurVarValue("Create1","string")}
  context.SetServiceAttributes(attributes)

Nodes can also be searched for by “NodeID” and “NodeName” separate from services. Use ``FindNodeByID`` and
``FindNodeByName`` in ``RobotRaconteurNode``. These will return the “NodeID”, “NodeName”, and the possible
“ConnectionURLs” without the query portion.

``ServiceInfo2Subscription`` continuosly tracks available services, and generally provides better results than
``FindServiceByType`` since it will detect services as they become available.
