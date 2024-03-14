==========
Transports
==========

TcpTransport Class
==================

.. autoclass:: RobotRaconteur.TcpTransport(Node=none)
   :members:

LocalTransport Class
====================

.. autoclass:: RobotRaconteur.LocalTransport(Node=none)
   :members:

IntraTransport Class
====================

.. autoclass:: RobotRaconteur.IntraTransport(Node=none)
   :members:

HardwareTransport Class
=======================

.. autoclass:: RobotRaconteur.HardwareTransport(Node=none)
   :members:

BrowserWebSocketTransport Class
===============================

.. class:: RobotRaconteur.BrowserWebSocketTransport(Node=none)

   Transport for creating client connections inside a web browser using WebSockets
   
   Robot Raconteur can be compiled to run inside a web browser using Emscripten and WebAssembly (WASM).
   While inside a web browser, the only connection method currently available to connection to
   a Robot Raconteur service is using WebSockets. The BrowserWebSocketTransport class implements
   the WebSocket transport for the web browser. Currently only the client side is implemented.
   
   See \\ref robotraconteur_url for more information on URLs.
   
   Currently the url connections schemes ``rr+ws``, ``rr+wss`` and ``rr+tcp`` are supported.
   ``rr+tcp`` are treated as ``rr+ws`` connections.
    
   The BrowserWebSocketTransport is automatically registered when the ``RobotRaconteur.Client``
   module is used. If the ``RobotRaconteur.Client`` module is not used, the
   BrowserWebSocketTransport must be manually registered with the node using
   RobotRaconteurNode.RegisterTransport(). NodeSetup is not currently
   available in the web browser.
    
   Note that for services to accept a WebSocket connection, the service must
   have the WebSocket \"origin\" configured correctly. The origin is the base
   URL of the web page that is hosting the web page that is connecting to the
   service. For example, if the web page is hosted at ``https://example.com/application/index.html``,
   the origin would be ``https://example.com``. For localhost, the origin is ``http://localhost:8080``,
   where 8080 is the port the web page is hosted on. The origin can be configured
   using the function TcpTransport.AddWebSocketAllowedOrigin(), or using
   the ``--robotraconteur-tcp-ws-add-origin`` command line option if a node setup class is used.
   If a local file is used to host the web page, the origin is ``null`` and no origin
   checking is performed.
    
   See TcpTransport.AddWebSocketAllowedOrigin() for more information on configuring
   the WebSocket origin and the default origins that are automatically configured.
   
   :param node: (optional) The node that will use the transport. Default is the singleton node
   :type node: RobotRaconteur.RobotRaconteurNode

   .. property:: DefaultReceiveTimeout

      Set the default receive timeout in seconds

		If no messages are received within the timeout, the connection
		is assumed to be lost.

		Default: 15 seconds

		:rtype: float

   .. property:: DefaultConnectTimeout

      Set the default connect timeout in seconds

		If the connection is not completed within the timeout, the connection
		attempt will be aborted.

		Default: 5 seconds

		:rtype: float

   .. property:: DefaultHeartbeatPeriod

      Set the default heartbeat period in seconds

		The heartbeat is used to keep the connection alive
		if no communication is occuring between nodes.

		Default: 5 seconds

		:rtype: float

   .. property:: MaxMessageSize

      Set the maximum serialized message size

		Default: 10 MB

		:rtype: int

   .. property:: DisableMessage4

      Set disable Message Format Version 4

		Message Format Version 2 will be used

		Default: Message V4 is enabled

		:rtype: bool

   .. property:: DisableStringTable

      Set disable string table

		Default: false

		RobotRaconteurNodeSetup and its subclasses
		will disable the string table by default

		:rtype: bool

   .. property:: DisableAsyncMessageIO

      Set if async message io is disabled

		Async message io has better memory handling, at the
		expense of slightly higher latency.

      Async message io is not well supported 
      by this transport. Enabling async message io
      is not recommended.

		Default: Async io disabled

		:rtype: bool

       
