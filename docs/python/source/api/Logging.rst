================
Logging and Taps
================

Logging is configured using environmental variblase or command line option 
``--robotraconteur-log-level=`` with the node setup classes, or using the 
RobotRaconteur.SetLogLevel() with the RobotRaconteur.LogLevel_* enum values.

Log output target can be configured using RobotRaconter.RobotRaconturNode.SetLogRecordHandler(). 
By default the output is sent to ``stderr``. The 
RobotRaconteur.RobotRaconteurNode.FileLogRecordHandler can be used to save the log to a file.

Taps are used to record log records and message traffic. The "local tap"
creates a local socket that a recording program can connect to record the traffic.
This can be enabled using the ``--robotraconteur-local-tap-enable=true`` and 
``--robotraconteur-local-tap-name=`` command line options with the node setup classes.

RRLogRecord Class
=================

.. autoclass:: RobotRaconteur.RRLogRecord
   :members:

LogRecordHandler Class
======================

.. autoclass:: RobotRaconteur.LogRecordHandler
   :members:

FileLogRecordHandler Class
==========================

.. autoclass:: RobotRaconteur.FileLogRecordHandler
   :members:

LogLevel Enum
=============

.. data:: RobotRaconteur.LogLevel_Trace
   :value: 0

   `trace` log level

.. data:: RobotRaconteur.LogLevel_Debug
   :value: 1

   `debug` log level

.. data:: RobotRaconteur.LogLevel_Info
   :value: 2

   `info` log level

.. data:: RobotRaconteur.LogLevel_Warning
   :value: 3

   `warning` log level

.. data:: RobotRaconteur.LogLevel_Error
   :value: 4

   `error` log level

.. data:: RobotRaconteur.LogLevel_Fatal
   :value: 5

   `fatal` log level

.. data:: RobotRaconteur.LogLevel_Disable
   :value: 1000

   `disabled` log level

LogComponent Enum
=================

.. data:: RobotRaconteur.LogComponent_Default
   :value: 0

   default component

.. data:: RobotRaconteur.LogComponent_Node
   :value: 1

   Robot Raconteur Node component

.. data:: RobotRaconteur.LogComponent_Transport
   :value: 2

   tranport component

.. data:: RobotRaconteur.LogComponent_Message
   :value: 3

   message or message serialization component

.. data:: RobotRaconteur.LogComponent_Client
   :value: 4

   client component

.. data:: RobotRaconteur.LogComponent_Service
   :value: 5

   service component

.. data:: RobotRaconteur.LogComponent_Member
   :value: 6

   member component

.. data:: RobotRaconteur.LogComponent_Pack
   :value: 7

   data message packing component

.. data:: RobotRaconteur.LogComponent_Unpack
   :value: 8

   data message unpacknig component

.. data:: RobotRaconteur.LogComponent_ServiceDefinition
   :value: 9

   service definition parser component

.. data:: RobotRaconteur.LogComponent_Discovery
   :value: 10

   node/service discovery component

.. data:: RobotRaconteur.LogComponent_Subscription
   :value: 11

   subscription component

.. data:: RobotRaconteur.LogComponent_NodeSetup
   :value: 12

   node setup component

.. data:: RobotRaconteur.LogComponent_Utility
   :value: 13

   utility component

.. data:: RobotRaconteur.LogComponent_RobDefLib
   :value: 14

   service definition standard library component (external)

.. data:: RobotRaconteur.LogComponent_User
   :value: 15

   user component (external)

.. data:: RobotRaconteur.LogComponent_UserClient
   :value: 16

   user client component (external)

.. data:: RobotRaconteur.LogComponent_UserService
   :value: 17

   user service component (external)

.. data:: RobotRaconteur.LogComponent_ThirdParty
   :value: 18

   third party library component (external)