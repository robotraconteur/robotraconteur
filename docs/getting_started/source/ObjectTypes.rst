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
                <img src="../../../examples/reynard_the_robot/labview/client/reynard_robotraconteur_client_vi.png" style="max-width: none;">
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
                <img src="../../../examples/reynard_the_robot/labview/client/reynard_robotraconteur_client_vi.png" style="max-width: none;">
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
                <img src="../../../examples/standard_devices/robot/labview/robot_client_trajectory.png" style="max-width: none;">
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
                <img src="../../../examples/reynard_the_robot/labview/client/reynard_robotraconteur_client_vi.png" style="max-width: none;">
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
                <img src="../../../examples/simple_webcam/labview/client/Simple Webcam Client.png" style="max-width: none;">
            </div>

    .. group-tab:: C\#

        .. literalinclude:: ../../../examples/simple_webcam/cs/client/simple_webcam_client/Program.cs
            :language: csharp
            :lines: 27-28,31-32

    .. group-tab:: C++

        .. literalinclude:: ../../../examples/simple_webcam/cpp/client/simple_webcam_client.cpp
            :language: cpp
            :lines: 40-41,44-45
