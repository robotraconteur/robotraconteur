.. _object_types:

============
Object Types
============

.. contents:: Table of Contents
   :depth: 1
   :local:
   :backlinks: none

Overview
========

Robot Raconteur uses an "augmented object-oriented" model. Robot Raconteur objects may contain
eight different member types: ``property``, ``function``, ``event``, ``objref``,
``pipe``, ``callback``, ``wire``, and ``memory``. Objects are owned by services, and passed by reference
to clients. Value types are passed between clients and services using the object members. Services
may contain multiple objects. One object is the "root" object, and is returned when the client connects.
Other objects in the service are accessed using ``objref`` members.

Objects are defined using the ``object`` keyword in service definitions. See the :ref:`service_definitions` page
and the `Service Definition Framework page <https://robotraconteur.github.io/robotraconteur/doc/core/latest/cpp/service_definition.html>`_
for more information on defining objects.

Objects can ``implement`` other objects. This is used to allow for polymorphism in the object model. The
``implements`` keyword is used to specify the object that is being implemented. Inheritance works similarly
to C\# and Java interface inheritance.

property Member
===============

The ``property`` member is used to define a member that can be read and written by clients. Any valid
Robot Raconteur type can be used as a property type. The property can also be modified to be ``[readonly]`` or
``[writeonly]``.

Example properties from the ``experimental.reynard_the_robot.Reynard`` object:

.. literalinclude:: ../../../examples/reynard_the_robot/robdef/experimental.reynard_the_robot.robdef
    :lines: 21,29

Example code excerpts demonstrating the use of properties:

.. tabs::

    .. group-tab:: Python

        .. literalinclude:: ../../../examples/reynard_the_robot/python/client/reynard_robotraconteur_client.py
            :language: python
            :lines: 38-42

    .. group-tab:: MATLAB

        .. literalinclude:: ../../../examples/reynard_the_robot/matlab/client/reynard_robotraconteur_client.m
            :language: matlab
            :lines: 34-39

    .. group-tab:: LabView

        .. raw:: html

            <div style="overflow: auto; width: 100%; height: 400px;">
                <img src="https://raw.githubusercontent.com/robotraconteur/robotraconteur/master/examples/reynard_the_robot/labview/client/reynard_robotraconteur_client_vi.png" style="max-width: none;">
            </div>

    .. group-tab:: C\#

        .. literalinclude:: ../../../examples/reynard_the_robot/cs/client/reynard_robotraconteur_client.cs
            :language: csharp
            :lines: 41-46

    .. group-tab:: C++

        .. literalinclude:: ../../../examples/reynard_the_robot/cpp/client/reynard_robotraconteur_client.cpp
            :language: cpp
            :lines: 43-50

function Member
===============

The ``function`` member is used to define a member that can be called by clients. The function can have zero,
one, or more parameters, and can return zero or one value. (A void function has no return value.)

Example functions from the ``experimental.reynard_the_robot.Reynard`` object:

.. literalinclude:: ../../../examples/reynard_the_robot/robdef/experimental.reynard_the_robot.robdef
    :lines: 15,17,19,23,25,27

.. note::

    Member names beginning with ``set`` and ``get`` prefixes are reserved, so functions should use
    ``getf_`` and ``setf_`` instead to stand for "get function" and "set function".

Example code excerpts demonstrating the use of functions:

.. tabs::

    .. group-tab:: Python

        .. literalinclude:: ../../../examples/reynard_the_robot/python/client/reynard_robotraconteur_client.py
            :language: python
            :lines: 20-21,23-24,32-33,35-36,49-50

    .. group-tab:: MATLAB

        .. literalinclude:: ../../../examples/reynard_the_robot/matlab/client/reynard_robotraconteur_client.m
            :language: matlab
            :lines: 16-17,19-20,25-26,28-29,46-47

    .. group-tab:: LabView

        .. raw:: html

            <div style="overflow: auto; width: 100%; height: 400px;">
                <img src="https://raw.githubusercontent.com/robotraconteur/robotraconteur/master/examples/reynard_the_robot/labview/client/reynard_robotraconteur_client_vi.png" style="max-width: none;">
            </div>

    .. group-tab:: C\#

        .. literalinclude:: ../../../examples/reynard_the_robot/cs/client/reynard_robotraconteur_client.cs
            :language: csharp
            :lines: 23-24,26-27,35-36,38-39,53-54

    .. group-tab:: C++

        .. literalinclude:: ../../../examples/reynard_the_robot/cpp/client/reynard_robotraconteur_client.cpp
            :language: cpp
            :lines: 25-26,28-29,34-35,37-38,40-41,59-60

``function`` members can also return "generators", which are a simple co-routine mechanism. The generator
function returns a generator object that has three functions: ``Next()``, ``Abort()``, and ``Close()``. The
``Next()`` function is called to get the next value from the generator. The ``Abort()`` function is called
to abort the generator. The ``Close()`` function is called to close the generator cleanly. A ``StopIterationException``
will be thrown when the generator has no more values to return. Optionally use ``TryNext()`` which will return
a boolean indicating if the generator has more values. Function generators are declared by adding the
``{generator}`` container to the return type, the last parameter, or both. The ``Next()`` function
can return a value, receive a value, or both.

Generators can be used to transfer large amounts of data in a streaming fashion. Robot Raconteur typically
has around a 10 MB limit on the size of a single message, so generators are used to transfer large data sets.

An example of generator functions for transferring large data from the ``com.robotraconteur.resource.ResourceStorage``
object:

.. code-block::

    function ResourcePart{generator} resource_get(ResourceIdentifier identifier)
    function ResourcePart{generator} resource_get_all(Identifier bucket_identifier)
    function void resource_set(ResourceIdentifier identifier, bool overwrite, ResourcePart{generator} resource)

Function generators can also be used for long-running operations. Robot Raconteur functions typically
have a timeout of around 10 seconds, so long-running operations should use a generator to return partial
results to the client. ``Next()`` is called repeatedly by the client to get the next result until the operation
is complete. This functionality is similar to
`ROS Actions <https://docs.ros.org/en/foxy/Tutorials/Beginner-CLI-Tools/Understanding-ROS2-Actions/Understanding-ROS2-Actions.html>`_.

An example of a generator function for executing a trajectory from the ``com.robotraconteur.robotics.robot.Robot``
object:

.. code-block::

    function TrajectoryStatus{generator} execute_trajectory(JointTrajectory trajectory)

Example code excerpts demonstrating the use of function generators:

.. tabs::

    .. group-tab:: Python

        .. literalinclude:: ../../../examples/standard_devices/robot/python/robot_client_trajectory.py
            :language: python
            :lines: 75-89

    .. group-tab:: MATLAB

        .. literalinclude:: ../../../examples/standard_devices/robot/matlab/robot_client_trajectory.m
            :language: matlab
            :lines: 63-79

    .. group-tab:: LabView

        .. raw:: html

            <div style="overflow: auto; width: 100%; height: 400px;">
                <img src="https://raw.githubusercontent.com/robotraconteur/robotraconteur/master/examples/standard_devices/robot/labview/robot_client_trajectory.png" style="max-width: none;">
            </div>

    .. group-tab:: C\#

        .. literalinclude:: ../../../examples/standard_devices/robot/cs/robot_client_trajectory.cs
            :language: csharp
            :lines: 86-94

    .. group-tab:: C++

        .. literalinclude:: ../../../examples/standard_devices/robot/cpp/robot_client_trajectory.cpp
            :language: cpp
            :lines: 102-111

event Member
============

The ``event`` member is used to define a member that sends a notification to clients. The event can have zero
or more parameters. Clients assign a callback function to the event to receive notifications. Each programming
language has a different way of handling events.

.. note::

    Events are sent to every client every time, so they should be used sparingly. High update rate
    data should use ``wire`` or ``pipe`` members instead of events.

Example events from the ``experimental.reynard_the_robot.Reynard`` object:

.. literalinclude:: ../../../examples/reynard_the_robot/robdef/experimental.reynard_the_robot.robdef
    :lines: 33

Example code excerpts demonstrating the use of events:

.. tabs::

    .. group-tab:: Python

        .. literalinclude:: ../../../examples/reynard_the_robot/python/client/reynard_robotraconteur_client.py
            :language: python
            :lines: 9-10,13-14

    .. group-tab:: MATLAB

        .. literalinclude:: ../../../examples/reynard_the_robot/matlab/client/reynard_robotraconteur_client.m
            :language: matlab
            :lines: 6-7,9-10,57-60

    .. group-tab:: LabView

        .. raw:: html

            <div style="overflow: auto; width: 100%; height: 400px;">
                <img src="https://raw.githubusercontent.com/robotraconteur/robotraconteur/master/examples/reynard_the_robot/labview/client/reynard_robotraconteur_client_vi.png" style="max-width: none;">
            </div>

    .. group-tab:: C\#

        .. literalinclude:: ../../../examples/reynard_the_robot/cs/client/reynard_robotraconteur_client.cs
            :language: csharp
            :lines: 14-16

    .. group-tab:: C++

        .. literalinclude:: ../../../examples/reynard_the_robot/cpp/client/reynard_robotraconteur_client.cpp
            :language: cpp
            :lines: 21-24

objref Member
=============

The ``objref`` member is used to access other objects in the service. The objref can optionally have
an ``int32`` or ``string`` index.

Example objref from the ``experimental.simplewebcam3.WebcamHost`` object:

.. literalinclude:: ../../../examples/simple_webcam/robdef/experimental.simplewebcam3.robdef
    :lines: 38

``objref`` members can also be declared ``varobject`` to return any object type.

Example code excerpts demonstrating the use of objrefs:

.. tabs::

    .. group-tab:: Python

        .. literalinclude:: ../../../examples/simple_webcam/python/client/simple_webcam_client.py
            :language: python
            :lines: 30-32,36

    .. group-tab:: MATLAB

        .. literalinclude:: ../../../examples/simple_webcam/matlab/client/simple_webcam_client.m
            :language: matlab
            :lines: 8-10,13-14

    .. group-tab:: LabView

        .. raw:: html

            <div style="overflow: auto; width: 100%; height: 400px;">
                <img src="https://raw.githubusercontent.com/robotraconteur/robotraconteur/master/examples/simple_webcam/labview/client/Simple Webcam Client.png" style="max-width: none;">
            </div>

    .. group-tab:: C\#

        .. literalinclude:: ../../../examples/simple_webcam/cs/client/simple_webcam_client/Program.cs
            :language: csharp
            :lines: 27-28,31-32

    .. group-tab:: C++

        .. literalinclude:: ../../../examples/simple_webcam/cpp/client/simple_webcam_client.cpp
            :language: cpp
            :lines: 40-41,44-45

pipe Member
===========

The ``pipe`` member provides reliable in-order streaming data transfer between the client and service. Pipes
are full duplex, so data can be sent in both directions. Pipes are "connected" to create connections
between clients and services. A single pipe
member can have multiple connections between the same client and service. A pipe can also be modified to be
``[readonly]`` or ``[writeonly]``. Pipes can also be modified to be ``[unreliable]`` to provide faster
data transfer with no guarantees of delivery, where packets may be dropped or arrive out of order.

Example pipes from the ``experimental.simplewebcam3.Webcam`` object:

.. literalinclude:: ../../../examples/simple_webcam/robdef/experimental.simplewebcam3.robdef
    :lines: 28

Example code excerpts demonstrating the use of pipes follow. These examples use ``PipeEndpoint`` with a callback
when new frames arrive. ``PipeEndpoint`` also has a ``TryReceivePacketWait()`` function to receive a packet
synchronously with a timeout without using a callback.

.. tabs::

    .. group-tab:: Python

        .. literalinclude:: ../../../examples/simple_webcam/python/client/simple_webcam_client_streaming.py
            :language: python
            :lines: 34-41,62-70

    .. group-tab:: MATLAB

        .. literalinclude:: ../../../examples/simple_webcam/matlab/client/simple_webcam_client_stream.m
            :language: matlab
            :lines: 8-9,23-27

    .. group-tab:: LabView

        .. raw:: html

            <div style="overflow: auto; width: 100%; height: 400px;">
                <img src="https://raw.githubusercontent.com/robotraconteur/robotraconteur/master/examples/simple_webcam/labview/client/Simple Webcam Client.png" style="max-width: none;">
            </div>

    .. group-tab:: C\#

        .. literalinclude:: ../../../examples/simple_webcam/cs/client/simple_webcam_client_streaming/Program.cs
            :language: csharp
            :lines: 26-31,88-97

    .. group-tab:: C++

        .. literalinclude:: ../../../examples/simple_webcam/cpp/client/simple_webcam_client_streaming.cpp
            :language: cpp
            :lines: 28-38,57-62

callback Member
===============

The ``callback`` member is a "reverse function" that allows the service to call a function in the client. The
service must specify which client to call using the "Local Endpoint" of the client. The service determines
the client's local endpoint during a function call. In the example, a specific function is used
to claim the callback, but the service has flexibility in how it assigns the callback.

Example callback from the ``experimental.create3.Create`` object:

.. literalinclude:: ../../../examples/irobot_create/robdef/experimental.create3.robdef
    :lines: 61

Example code excerpts demonstrating the use of callbacks:

.. tabs::

    .. group-tab:: Python

        .. literalinclude:: ../../../examples/irobot_create/python/client/irobot_create_client.py
            :language: python
            :lines: 37-39,59-60

    .. group-tab:: MATLAB

        .. literalinclude:: ../../../examples/irobot_create/matlab/client/irobot_create_client.m
            :language: matlab
            :lines: 22-24,73-80

    .. group-tab:: LabView

        .. raw:: html

            <div style="overflow: auto; width: 100%; height: 400px;">
                <img src="https://raw.githubusercontent.com/robotraconteur/robotraconteur/master/examples/irobot_create/labview/client/iRobot Create Client.png" style="max-width: none;">
            </div>

    .. group-tab:: C\#

        .. literalinclude:: ../../../examples/irobot_create/cs/client/irobot_create_client.cs
            :language: csharp
            :lines: 24-28,53-58

    .. group-tab:: C++

        .. literalinclude:: ../../../examples/irobot_create/cpp/client/irobot_create_client.cpp
            :language: cpp
            :lines: 41-48

wire Member
===========

The ``wire`` member is used to communicate a "most recent" value between the client and service. The wire
is "connected" to create connections between clients and services. A client may only create one client
connection to a wire. Wires can also be modified to be ``[readonly]`` or ``[writeonly]``.

Wires are normally "connected" to create a real-time streaming connection between the client and service. Wires
can also be "Peeked" and "Poked" using the ``PeekInValue()``, ``PeekOutValue()`` and ``PokeOutValue()``
functions. These functions work like properties where a request is sent to the service to get or set the value rather
than using a streaming connection.

Example wire from the ``experimental.reynard_the_robot.Reynard`` object:

.. literalinclude:: ../../../examples/reynard_the_robot/robdef/experimental.reynard_the_robot.robdef
    :lines: 31

Example code excerpts demonstrating the use of wires with ``PeekInValue()``:

.. tabs::

    .. group-tab:: Python

        .. literalinclude:: ../../../examples/reynard_the_robot/python/client/reynard_robotraconteur_client.py
            :language: python
            :lines: 16-18

    .. group-tab:: MATLAB

        .. literalinclude:: ../../../examples/reynard_the_robot/matlab/client/reynard_robotraconteur_client.m
            :language: matlab
            :lines: 12-14

    .. group-tab:: LabView

        .. raw:: html

            <div style="overflow: auto; width: 100%; height: 400px;">
                <img src="https://raw.githubusercontent.com/robotraconteur/robotraconteur/master/examples/reynard_the_robot/labview/client/reynard_robotraconteur_client_vi.png" style="max-width: none;">
            </div>

    .. group-tab:: C\#

        .. literalinclude:: ../../../examples/reynard_the_robot/cs/client/reynard_robotraconteur_client.cs
            :language: csharp
            :lines: 18-21

    .. group-tab:: C++

        .. literalinclude:: ../../../examples/reynard_the_robot/cpp/client/reynard_robotraconteur_client.cpp
            :language: cpp
            :lines: 62-65

The standard robot streaming position command demonstrates using read and write wires to command
a robot in (soft) real-time using the ``com.robotraconteur.robotics.robot.Robot`` object. Wire
connections are established to the ``robot_state`` and ``position_command`` wires for real-time
streaming of the robot state and position command.

Example wires from the ``com.robotraconteur.robotics.robot.Robot`` object:

.. code-block::

    wire RobotState robot_state [readonly,nolock]
    wire RobotJointCommand position_command [writeonly]

Example code excerpts demonstrating the use of wires for real-time streaming:

.. tabs::

    .. group-tab:: Python

        .. literalinclude:: ../../../examples/standard_devices/robot/python/robot_client_position_command.py
            :language: python
            :lines: 37-45,55-82

    .. group-tab:: MATLAB

        .. literalinclude:: ../../../examples/standard_devices/robot/matlab/robot_client_position_command.m
            :language: matlab
            :lines: 19-27,38-68

    .. group-tab:: LabView

        .. raw:: html

            <div style="overflow: auto; width: 100%; height: 400px;">
                <img src="https://raw.githubusercontent.com/robotraconteur/robotraconteur/master/examples/standard_devices/robot/labview/robot_client_position_command.png" style="max-width: none;">
            </div>

    .. group-tab:: C\#

        .. literalinclude:: ../../../examples/standard_devices/robot/cs/robot_client_position_command.cs
            :language: csharp
            :lines: 33-41,54-93

    .. group-tab:: C++

        .. literalinclude:: ../../../examples/standard_devices/robot/cpp/robot_client_position_command.cpp
            :language: cpp
            :lines: 41-49,59-96

memory Member
=============

Memories represent a block of memory that can be read and written by the client. Memories can be single-dimensional
or multi-dimensional. Memory members can be accessed like an array in the client. Each programming
language has a different way of handling memory members. Memory members can be modified to be ``[readonly]``
or ``[writeonly]``. Memory members are also intended to represent a memory-mapped region of memory
such as a register space or hardware memory buffer,
but currently this functionality is not used by any client libraries.

Example memory members from the ``experimental.simplewebcam3.Webcam`` object:

.. literalinclude:: ../../../examples/simple_webcam/robdef/experimental.simplewebcam3.robdef
    :lines: 31-32

Example code excerpts demonstrating the use of memory members:

.. tabs::

    .. group-tab:: Python

        .. literalinclude:: ../../../examples/simple_webcam/python/client/simple_webcam_client_memory.py
            :language: python
            :lines: 26-42

    .. group-tab:: MATLAB

        .. literalinclude:: ../../../examples/simple_webcam/matlab/client/simple_webcam_client_memory.m
            :language: matlab
            :lines: 11-27

    .. group-tab:: LabView

        .. raw:: html

            <div style="overflow: auto; width: 100%; height: 400px;">
                <img src="https://raw.githubusercontent.com/robotraconteur/robotraconteur/master/examples/simple_webcam/labview/client/Simple Webcam Client Memory.png" style="max-width: none;">
            </div>

    .. group-tab:: C\#

        .. literalinclude:: ../../../examples/simple_webcam/cs/client/simple_webcam_client_memory/Program.cs
            :language: csharp
            :lines: 30-33,45-52

    .. group-tab:: C++

        .. literalinclude:: ../../../examples/simple_webcam/cpp/client/simple_webcam_client_memory.cpp
            :language: cpp
            :lines: 36-39,49-52
