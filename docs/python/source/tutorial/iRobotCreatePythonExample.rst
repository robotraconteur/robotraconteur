iRobot Create Python Example
============================

Currently Robot Raconteur is not natively supported by commercial hardware so it is necessary to “wrap” the provided
APIs with a Robot Raconteur service. For this example, we are going to wrap the serial Create Open Interface (OI) with a
service. The sample code is by no means exhaustive of all the capabilities the robot has to offer; it is intended to be
instructive on the use of Robot Raconteur. The user is encouraged to fill out the functionality by adding more members
to the service definition!

Simple service
--------------

The first step in using Robot Raconteur is to develop an object that implements the service definition. The following
example shows a non-Robot Raconteur program that contains a class ``Create_impl`` that
implements the service definition "experimental.create3" presented in :doc:`ServiceDefinition`.
Table `Members <#createmembers>`__
lists the members and the functionality that will be implemented.

.. code:: python

   import serial
   import struct
   import time
   import RobotRaconteur as RR
   #Convenience shorthand to the default node.
   #RRN is equivalent to RR.RobotRaconteurNode.s
   RRN=RR.RobotRaconteurNode.s
   import thread
   import threading

   serial_port_name="/dev/ttyUSB0"

   class Create_impl(object):
       def __init__(self):
           self.Bump=RR.EventHook()
           self._lock=threading.RLock()
           self._packets=None
           self._play_callback=None

       def drive(self, velocity, radius):
           with self._lock:
               dat=struct.pack(">B2h",137,int(velocity*1e3),int(radius*1e3))
               self._serial.write(dat)


       @property
       def distance_traveled(self):
           return 0

       @property
       def angle_traveled(self):
           return 0

       @property
       def bumpers(self):
           return 0

       @property
       def play_callback(self):
           return self._play_callback
       @play_callback.setter
       def play_callback(self,value):
           self._play_callback=value

       def init(self,port):
           self._serial=serial.Serial(port="/dev/ttyUSB0",baudrate=57600)
           dat=struct.pack(">2B",128,131)
           self._serial.write(dat)

       def shutdown(self):
           self._serial.close()

   def main():

       #Initialize the object in the service
       obj=Create_impl()
       obj.Init(serial_port_name)

       #Drive a bit to show that it works
       obj.drive(200,1000)
       time.sleep(1)
       obj.drive(0,0)

       #Shutdown
       obj.Shutdown()



   if __name__ == '__main__':
       main()

.. container::
   :name: createmembers

   .. table:: Selected Members of Create object

      +---------------------------------------------------------------------------------+---------------------------------------------------------------------------------------------------------------------------------------+
      | Member                                                                          | Description                                                                                                                           |
      +=================================================================================+=======================================================================================================================================+
      | ``function void drive(double velocity, double radius)``                           | Drives the create at ``velocity`` (m/s) with ``radius`` (m)                                                                         |
      +---------------------------------------------------------------------------------+---------------------------------------------------------------------------------------------------------------------------------------+
      | ``property int32 distance_traveled``                                             | Total distance traveled (doesn't seem to be accurate...)                                                                              |
      +---------------------------------------------------------------------------------+---------------------------------------------------------------------------------------------------------------------------------------+
      | ``property int32 angle_traveled``                                                | Total angle traveled (doesn't seem to be accurate...)                                                                                 |
      +---------------------------------------------------------------------------------+---------------------------------------------------------------------------------------------------------------------------------------+
      | ``property uint8 bumpers``                                                      | Returns the byte with flags about the state of the bumper and wheel drops (See OI manual sensor packet id 7)                          |
      +---------------------------------------------------------------------------------+---------------------------------------------------------------------------------------------------------------------------------------+
      | ``event bump()``                                                                | Event fired when the bumper goes from no contact to contact                                                                           |
      +---------------------------------------------------------------------------------+---------------------------------------------------------------------------------------------------------------------------------------+
      | ``wire CreateState create_state``                                               | Streaming state from the Create robot                                                                                                 |
      |                                                                                 |                                                                                                                                       |
      +---------------------------------------------------------------------------------+---------------------------------------------------------------------------------------------------------------------------------------+
      | ``callback uint8[] play_callback(double distance_traveled, double angle_traveled)`` | A callback that is called when the “Play" button is pressed and returns notes to play on the Create.                                  |
      +---------------------------------------------------------------------------------+---------------------------------------------------------------------------------------------------------------------------------------+


See the completed service implementation in `irobot_create_service.py <https://github.com/robotraconteur/robotraconteur/blob/master/examples/irobot_create/python/service/irobot_create_service.py>`_.

The above example shows the members implemented, but not yet exposed as a service. Properties and functions are simply
properties and functions in Python, events are implemented through the ``EventHook`` class that must be present as a
variable in the class. The ``Wire`` and ``Callback`` objects are implemented as properties and initialized to ``None``
and will be set by the Robot Raconteur node when the object is exposed as a service. The main function in this example
will drive the robot a few feet to demonstrate that the service works. Replace “/dev/ttyUSB0" with the appropriate
device (COM1, COM2, etc on Windows). The class shown above is mostly a skeleton class that needs to be filled in further
to have functionality beyond simply driving.

The function "Drive" has a ``with self._lock`` block protecting the code within the function. Robot Raconteur is
multi-threaded, meaning that all members including functions can be called *concurrently*. If there is an
operation or data structure that can be corrupted by simultaneous access, it is necessary to use a *thread lock*,
also-known-as a *mutex*. In the ``__init__`` for class ``Create_impl``, the ``self._lock`` variable is set to a new
instance of ``threading.RLock()``. When used with the ``with`` statement, it will lock itself so only one block can
execute at a time with one thread. If all the functions in the class use the same “with lock", only one thread at a time
will be able to access the class. If you are not familiar with multi-threading, it is best to have one global lock for
all your functions to prevent collisions.

Now that there is a basic object implemented, it is time to expose it as a Robot Raconteur service. The
following example shows a replacement for the ``main`` function that instead of simply driving the
robot, exposes the service.

.. code:: python

   def main():

       obj=Create_impl()
       comm_port=sys.argv[1]
       obj.Init(comm_port)

       with RR.ServerNodeSetup("experimental.create3.Create",2354):

           RRN.RegisterServiceTypeFromFile("experimental.create3.robdef")
           RRN.RegisterService("Create","experimental.create3.Create",obj)

           raw_input("Server started, press enter to quit...")

           obj.Shutdown()

   if __name__ == '__main__':
       main()

A Robot Raconteur node requires a few steps to initialize the service:

#. Use ServerNodeSetup to initialize the node. Use the `with` statement so it
   will also automatically shut down the node. The setup classes will also
   automatically check command line arguments to modify options for the node.

#. Register the relevant service types (robdef)

#. Register the root object for the service

Each Robot Raconteur node is uniquely identified by a 128-bit UUID “NodeID”. UUIDs are a widely used concept, and are
statistically guaranteed to be unique when randomly generated
(See `Wikipedia UUID <https://en.wikipedia.org/wiki/Universally_unique_identifier>`_ for more information on UUIDs.)
A node also has a name, the “NodeName”. A “NodeName”
is intended to help clients find relevant services, and is not guaranteed to be unique. For client nodes, the “NodeID”
is typically allowed to be automatically generated when needed, with the “NodeName” left emtpy. For a server node, the
“NodeName” is normally specified, with the “NodeID” retrieved from a local cache based on the “NodeName”. The “NodeID”
is randomly generated the first time the “NodeName” is used, and is retrieved from the cache subsequently. TLS
certificates for Robot Raconteur are assigned to the “NodeID”, and guarantee the identify of the node based on its
"NodeID".

“Transports" are used to communicate between nodes. The currently available transports are ``TcpTransport`` for
communication over a TCP/IP network, ``LocalTransport`` for communication between nodes running on the same computer,
and ``HardwareTransport`` for communication over USB, Bluetooth, and PCIe, and ``IntraTransport`` for communication
within the same process. For most server nodes, the ``TcpTransport``
and ``LocalTransport`` are configured to listen for incoming clients. The ``TcpTransport`` will listen for
connections on a TCP port, while the ``LocalTransport`` listens for connections on a file handle that is identified
by the “NodeName” or “NodeID” of the server node. If a TLS certificate is available, it can be loaded into the TCP
transport. This is done using command line arguments to the node, or using ``SecureServerNodeSetup``.

For most use cases, the Python class ``ServerNodeSetup`` can be used to initialize the server node. The
``ServerNodeSetup`` takes the “NodeName”, the TCP listen port, and an optional set of flags as parameters. In Python,
the ``ServerNodeSetup`` is used with the ``with`` statement. When the ``with`` statement scope is exited, the node is
shut down.

Service types (stored in robdef files) can either be included in the Python source file as strings, or can be loaded
from file. In this example, the service definition is loaded from a file using the
``RRN.RegisterServiceDefinitionFromFile`` function.

Once the identification and transports have been initialized, the object is registered for use. The first parameter in
``RRN.RegisterService`` is the name of the service, the second parameter is the fully qualified Robot Raconteur type of
the object, and the last parameter is the object to expose as a service. (Note that a node can have multiple services
registered as long as they have different names).

After initialization, the program waits for the user to press “Enter" to stop the server. The service is now available
for use by a client!

Simple client
-------------

While there are several steps to starting a service, connecting as a client is very simple. The following
is an example of driving the robot over a network using the service example above:

::

   from RobotRaconteur.Client import *

   #Connect to the service
   obj=RRN.ConnectService("rr+tcp://101.2.2.2?service=Create")

   #Drive a bit
   obj.drive(200,1000)
   time.sleep(1)
   obj.drive(0,0)

The example registers uses the ``RobotRaconteur.Client`` convenience module to configure for the most common client
operations. This module creates a variable “RR" that contains the Robot Raconteur module, and “RRN" that is the default
node. It also registers the transports ``TcpTransport``, ``LocalTransport``, ``HardwareTransport``, and
``IntraTransport``.

Robot Raconteur uses URLs to connect to services. The most common URLs are for local and TCP cases.

The url format for the ``LocalTransport`` is:

``rr+local:///?nodename=TargetNodeName&service=ServiceName``

and the url format for the ``TcpTransport`` is:

``rr+tcp://hostname:port?service=ServiceName``

The standard URL format is used, and the target service is passed as part of the “query" portion of the URL. Often it is
necessary to specify the node to connect. For instance, the local transport requires the “nodename" to be specified
because there can be multiple nodes running locally. The target node can be identified by NodeName, by NodeID, or
by both. The NodeID should be the UUID of the node without curly braces. This is due to the limitations of URL syntax.

For instance, these are all valid URLs for the local transport to connect to the CreateService (replace the UUID with
the one generated for your service):

``rr+local:///?nodename=experimental.create.Create&service=Create``

``rr+local:///?nodeid=6f6706c9-91cc-d448-ae8c-c5a2acac198c&service=Create``

``rr+local:///?nodeid=6f6706c9-91cc-d448-ae8c-c5a2acac198c&nodename=experimental.create.Create&service=Create``

The following are valid URLs to connect to the CreateServer using tcp:

``rr+tcp://localhost:2354/?service=Create``

``rr+tcp://localhost:2354/?nodename=experimental.create.Create&service=Create``

``rr+tcp://localhost:2354/?nodeid=6f6706c9-91cc-d448-ae8c-c5a2acac198c&service=Create``

``rr+tcp://localhost:2354/?nodeid=6f6706c9-91cc-d448-ae8c-c5a2acac198c&nodename=experimental.create.Create&service=Create``

*Replace “localhost" with the IP address or hostname of a foreign computer if accessing over a network.*

Note that for the TCP connections, the “rr+tcp" can be connected to “rrs+tcp" to enable TLS to encrypt the
communication. See the *Robot Raconteur Security using TLS and Certificates* manual for details on using TLS.

See `Robot Raconteur URLs <https://robotraconteur.github.io/robotraconteur/doc/core/latest/cpp/nodes_and_communication.html#urls>`_ for details on how to use URLs for more advanced cases.

A reference to the service object is returned, and it can now be used to access the members. In this example, the robot
is driven a bit to demonstrate how to use a function.

iRobot Create Service
---------------------

The initial service shown above only fills in the ``drive`` member. The example
`irobot_create_service.py <https://github.com/robotraconteur/robotraconteur/blob/master/examples/irobot_create/python/service/irobot_create_service.py>`_
on GitHub shows a complete service that fills in all of the members. This is not intended to
be exhaustive for the full features of the iRobot Create; it is instead intended to be used to demonstrate features of
Robot Raconteur. Because of the length of the code it is printed in the appendix and will be referred to throughout this
section.

The background thread receives data from the serial port
and transmits the data to the ``bump`` event, the ``create_state`` wire, or the ``play_callback`` where appropriate.
 The function ``_receive_sensor_packets`` implements the ability to receive and parse the packets. This function is dedicated
to handling the serial data from the robot and calls the ``_fire_bump`` function to fire the ``bump`` event, the
``_parse_sensor_packets`` function to parse the new value of the ``create_state`` wire, or the ``_play`` function to handle when
the Play button is pressed on the robot. It also keeps a running tally of distance and angle traveled in the
``_distance_traveled`` and ``_angle_traveled`` variables. The rest of this section will discuss the implementation of the
different members. It stores the Bump data in the ``_bumpers`` variable.

The ``bumpers``, ``distance_traveled``, and ``angle_traveled`` properties are implemented as standard Python properties
using the ``@property`` decorator. Because these are read only, the setters throw an exception. Properties transparently
transmit exceptions back to the client. Functions also transparently transmit exceptions to the client. All Robot
Raconteur calls should be surrounded with try/except blocks that catch ``Exception`` meaning it will catch and process
any thrown exception.

| Events in Python are implemented using the ``EventHook()`` class. The ``__init__`` function of
| ``Create_impl`` sets:

``self.Bump==RR.EventHook()``

This line creates the ``EventHook`` object that is used to connect events. The ``fire_Bump`` function then fires this
event. The Robot Raconteur node will transmit this event to all connected clients. Note that the ``fire`` command of
``EventHook`` may contain parameters if the event has parameters.

The ``packets`` wire is implemented by the node when the object is registered as a service. Because the wire is marked
``readonly`` using a member modifier and the ``create_state`` object attribute is not set, the node will assume that we want
a ``WireBroadcaster``. The node will create the attribute and assign a ``WireBroadcaster``. The ``WireBroadcaster``
class is designed to send the same value to all connected clients. If the wire is marked ``writeonly``, the node will
provide a ``WireUnicastReceiver`` object. If the wire does not specify a direction, A ``WireServer`` is passed to the
object through a property, which must be implemented by the object to receive the ``WireServer``.

The ``_SendSensorPackets`` function is called by the serial receive thread when there is a new data packet. The
``_SendSensorPackets`` uses the ``OutValue`` of the ``WireBroadcaster`` to send the new value to all connected clients.
The packet data is stored in a ``experimental.create.SensorPacket`` structure that is defined in the service definition.
The ``RRN.NewStructure`` command is used to initialize a new Robot Raconteur structure in Python. If there is an error,
assume that the wire has been closed and delete it from the dictionary.

Wires use the ``InValue`` and ``OutValue`` in ``WireConnection`` to send and receive values. For a ``readonly`` wire,
the client will use the ``InValue`` while the service will use the ``OutValue`` property. Fir a ``writeonly`` wire,
these roles are reversed and the client will use the ``OutValue`` property while the service will use the ``InValue``
property. If the wire does not specify the direction, both the client and service can use ``InValue`` and ``OutValue``.

As of Version 0.9, wire clients can also “peek” and “poke” values. The peek and poke read the value synchronously
without creating a streaming connection. (The behavior of “peek” and “poke” is similar to the behavior of properties.)
``PeekInValue`` is used to read the in value, while ``PeekOutValue`` and ``PokeOutValue`` are used to read and write the
out value. (The “in” and “out” directions in the peek/poke functions are relative to the client.)

``WireConnection`` also has the ``LastValueReceivedTime`` and ``LastValueSentTime`` to determine the last time that
values were updated. These are relative to ``InValue`` and ``OutValue`` when using streaming data, and are received from
the peek and poke functions as part of the return from the functions.

The ``play_callback`` member is assigned to the texttt\_play_callback attribute of the ``Create_impl`` object by the
node when the object is registered as a service. The ``_play`` function demonstrates how to use the callback. The
``claim_play_callback()`` function contains the following line:

``self._ep=RR.ServerEndpoint.GetCurrentEndpoint()``

This line is used to determine the “endpoint" of the current client that is calling the function. The endpoint is used
to uniquely identify the client. When a callback is used, it is necessary to specify which client to call because there
may be multiple connected clients. The client is identified using the endpoint. The ``_play`` function contains the
following lines, which executes the callback on the client:

``cb_func=self.play_callback.GetClientFunction(self._ep)``

``notes=cb_func(self._distanceTraveled, self._angleTraveled)``

The first line retrieves the a function handle to call the client based on the stored endpoint. The second line executes
this function, which is actually implemented by calling the client with the supplied parameters and then returning the
result. Note that exceptions are also transmitted transparently by callbacks from the client to the service.

The ``ServerNodeSetup`` class by default will call ``EnableNodeAnnounce``. This initializes the auto-discovery system to
send out beacon packets so that client nodes can find the service.

iRobot Create Client
--------------------

An example client `irobot_create_client.py <https://github.com/robotraconteur/robotraconteur/blob/master/examples/irobot_create/python/client/irobot_create_client.py>`_
on GitHub utilizes the service.
The client is similar to the previous example client, however it adds functionality using the ``pump``, ``create_state``, and
``play_callback`` member. The line:

``c.bump += bumped``

adds the function ``bumped`` as a handler when the event is fired. The line:

``wire=c.create_state.Connect()``

connects to the ``packets`` wire and returns a WireConnection object that is stored in the ``wire`` variable. This
WireConnection has the same functionality as the one provided to the service object in the previous section. In this
example, the ``WireValueChange`` event is used. The line:

``wire.WireValueChanged+=wire_changed``

adds the ``wire_changed`` function as a handler and is called when the service provides a new value for the wire. This
event is also available on the service however in this application it is not needed. The final step in the configuration
is to set the function ``play_callback`` as the callback function for the ``play_callback`` member through the following
line:

``c.play_callback.Function=play_callback``

This function will now be called by the service when the service calls this client's callback.

After the setup the robot is driven a bit and then pauses to allow the user to try out the functionality. The
``RobotRaconteurNode`` is shutdown automatically when the program exits.
