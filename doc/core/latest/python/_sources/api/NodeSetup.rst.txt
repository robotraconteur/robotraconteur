==========
Node Setup
==========

RobotRaconteurNodeSetup Class
=============================

.. autoclass:: RobotRaconteur.RobotRaconteurNodeSetup
    :members:

ClientNodeSetup Class
=====================

.. autoclass:: RobotRaconteur.ClientNodeSetup
    :members:

ServerNodeSetup Class
=====================

.. autoclass:: RobotRaconteur.ServerNodeSetup
    :members:
    :inherited-members:

SecureServerNodeSetup Class
===========================

.. autoclass:: RobotRaconteur.SecureServerNodeSetup
    :members:
    :inherited-members:

CommandLineConfigParser Class
=============================

.. autoclass:: RobotRaconteur.CommandLineConfigParser
    :members:
    :inherited-members:


Node Setup Flags
================

Setup option flags

Setup option flags passed to node setup classes to select options to enable
and disable. Flags are used to configure the following types of options:

1. Enable and disable transport types
2. Modify transport options including discovery, security requirements, and connection listening
3. Configure TLS behavior
4. Enable local tap for logging

Node setup classes also allow options and flags to be "overridden" using
command line options. Use the `*_ALLOW_OVERRIDE` options to configure
when these overrides are allowed.

The ClientNodeSetup, ServerNodeSetup, and SecureServerNodeSetup
are convenience classes for the most commonly used options.

.. data:: RobotRaconteur.RobotRaconteurNodeSetupFlags_NONE
   :value: 0x0

   No options enabled

.. data:: RobotRaconteur.RobotRaconteurNodeSetupFlags_ENABLE_NODE_DISCOVERY_LISTENING
   :value: 0x1

   Enable node discovery listening on all transports

.. data:: RobotRaconteur.RobotRaconteurNodeSetupFlags_ENABLE_NODE_ANNOUNCE
   :value: 0x2

   Enable node announce on all transports

.. data:: RobotRaconteur.RobotRaconteurNodeSetupFlags_ENABLE_LOCAL_TRANSPORT
   :value: 0x4

   Enable LocalTransport

.. data:: RobotRaconteur.RobotRaconteurNodeSetupFlags_ENABLE_TCP_TRANSPORT
   :value: 0x8

   Enable TcpTransport

.. data:: RobotRaconteur.RobotRaconteurNodeSetupFlags_ENABLE_HARDWARE_TRANSPORT
   :value: 0x10

   Enable HardwareTransport

.. data:: RobotRaconteur.RobotRaconteurNodeSetupFlags_LOCAL_TRANSPORT_START_SERVER
   :value: 0x20

   Start the LocalTransport server to listen for incoming clients

.. data:: RobotRaconteur.RobotRaconteurNodeSetupFlags_LOCAL_TRANSPORT_START_CLIENT
   :value: 0x40

   Start the LocalTransport client with specified node name

.. data:: RobotRaconteur.RobotRaconteurNodeSetupFlags_TCP_TRANSPORT_START_SERVER
   :value: 0x80

   Start the TcpTransport server to listen for incoming clients on the specified port

.. data:: RobotRaconteur.RobotRaconteurNodeSetupFlags_TCP_TRANSPORT_START_SERVER_PORT_SHARER
   :value: 0x100

   Start the TcpTransport server to incoming for incoming clients using the port sharer

.. data:: RobotRaconteur.RobotRaconteurNodeSetupFlags_DISABLE_MESSAGE4
   :value: 0x200

   Disable Message Format Version 4 on all transports

.. data:: RobotRaconteur.RobotRaconteurNodeSetupFlags_DISABLE_STRINGTABLE
   :value: 0x400

   Disable Message Format Version 4 string table on all transports

.. data:: RobotRaconteur.RobotRaconteurNodeSetupFlags_DISABLE_TIMEOUTS
   :value: 0x800

   Disable all timeouts (useful for debugging)

.. data:: RobotRaconteur.RobotRaconteurNodeSetupFlags_LOAD_TLS_CERT
   :value: 0x1000

   Load the TLS certificate for TcpTransport

.. data:: RobotRaconteur.RobotRaconteurNodeSetupFlags_REQUIRE_TLS
   :value: 0x2000

   Require TLS for all clients on TcpTransport

.. data:: RobotRaconteur.RobotRaconteurNodeSetupFlags_LOCAL_TRANSPORT_SERVER_PUBLIC
   :value: 0x4000

   Make LocalTransport server listen for incoming clients from all users

.. data:: RobotRaconteur.RobotRaconteurNodeSetupFlags_NODENAME_OVERRIDE
   :value: 0x10000

   Allow NodeName to be configured using command line options

.. data:: RobotRaconteur.RobotRaconteurNodeSetupFlags_NODEID_OVERRIDE
   :value: 0x20000

   Allow NodeID to be configured using command line options

.. data:: RobotRaconteur.RobotRaconteurNodeSetupFlags_TCP_PORT_OVERRIDE
   :value: 0x40000

   Allow TCP port to be configured using command line options

.. data:: RobotRaconteur.RobotRaconteurNodeSetupFlags_TCP_WEBSOCKET_ORIGIN_OVERRIDE
   :value: 0x80000

   Allow TCP WebSocket origin control to be configured using command line options

.. data:: RobotRaconteur.RobotRaconteurNodeSetupFlags_ENABLE_INTRA_TRANSPORT
   :value: 0x100000

   Enable IntraTransport

.. data:: RobotRaconteur.RobotRaconteurNodeSetupFlags_INTRA_TRANSPORT_START_SERVER
   :value: 0x200000

   Start the IntraTransport server to listen for incoming clients

.. data:: RobotRaconteur.RobotRaconteurNodeSetupFlags_LOCAL_TAP_ENABLE
   :value: 0x1000000

   Enable the LocalTap debug logging system

.. data:: RobotRaconteur.RobotRaconteurNodeSetupFlags_LOCAL_TAP_NAME
   :value: 0x2000000

   Allow the user to set the LocalTap name

.. data:: RobotRaconteur.RobotRaconteurNodeSetupFlags_ENABLE_ALL_TRANSPORTS
   :value: 0x10001c

   Convenience flag to enable all transports

.. data:: RobotRaconteur.RobotRaconteurNodeSetupFlags_CLIENT_DEFAULT
   :value: 0x10004d

   Default configuration for client nodes (See ClientNodeSetup)

.. data:: RobotRaconteur.RobotRaconteurNodeSetupFlags_CLIENT_DEFAULT_ALLOWED_OVERRIDE
   :value: 0x3133e5d

   Default allowed overrides for client nodes (See ClientNodeSetup)

.. data:: RobotRaconteur.RobotRaconteurNodeSetupFlags_SERVER_DEFAULT
   :value: 0x3004af

   Default configuration for server nodes

.. data:: RobotRaconteur.RobotRaconteurNodeSetupFlags_SERVER_DEFAULT_ALLOWED_OVERRIDE
   :value: 0x33f7fff

   Default allowed overrides for server nodes

.. data:: RobotRaconteur.RobotRaconteurNodeSetupFlags_SECURE_SERVER_DEFAULT
   :value: 0x3034af

   Default configuration for server nodes requiring TLS network transports

.. data:: RobotRaconteur.RobotRaconteurNodeSetupFlags_SECURE_SERVER_DEFAULT_ALLOWED_OVERRIDE
   :value: 0x33f4fff

   Default allowed overrides for server nodes requiring TLS network transports