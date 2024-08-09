.. _service_definitions:

===================
Service Definitions
===================

.. contents:: Table of Contents
   :depth: 1
   :local:
   :backlinks: none

Overview
========

Robot Raconteur is a Remote Procedure Call (RPC) system that allows for "clients" to interact with "services."
The services and clients need to understand the data types and objects that are made available by the service.
Since Robot Raconteur supports multiple programming languages, the available data types and objects are defined using
Robot Raconteur "Service Definitions," a form of Interface Definition Language (IDL). When stored as
a file, these definitions are typically stored in a file with the extension ``.robdef``. When a client connects,
it downloads the service definition from the service. For dynamic languages like Python and Matlab,
the service definition is used to generate the necessary objects and data types at runtime. For statically typed
languages like C++ and C\#, the service definition is used to generate the necessary objects and data types at compile
time. The generated code is referred to as "thunk" code.

Robot Raconteur uses an advanced "Augmented Object-Oriented" model to define objects. This model extends
the typical members of objects to include additional functionality specific to robotics and automation applications.
See :ref:`objects` for a discussion on the additional functionality provided by the Augmented Object-Oriented model.
See :ref:`value-types` for a discussion on the value types supported by Robot Raconteur.

The following is an example of the service definition used for Reynard the Robot:

.. literalinclude:: ../../../examples/reynard_the_robot/robdef/experimental.reynard_the_robot.robdef
    :linenos:

The service definition begins with the line ``service experimental.reynard_the_robot``. This line defines the name
of the service. In this case, the ``experimental`` namespace is used to indicate that the service is experimental.
The namespace follows "Java" package naming conventions, which is reverse domain names. For instance,
the standard Robot Raconteur types begin with ``com.robotraconteur``. Services by a university might begin with
``edu.universityname``. The next line is ``stdver 0.10``. This line defines the minimum
version of Robot Raconteur required to use the service type. Future revisions to the Robot Raconteur
standard may introduce new features that are not compatible with older versions. Currently, the most recent
revision to the service definition standard is "0.10". This version should be used until a future version
of Robot Raconteur changes the service definition standard.

The service definition defines the structure ``ReynardState``, and the object ``Reynard``. Service definitions
can also define enums, constants, namedarrays, pods, and exceptions. See the following pages for more information
on the contents of service definitions and how to use them:

* :ref:`value_types`
* :ref:`object_types`
* :ref:`constants`
* :ref:`exceptions`

.. note::

    "Values Types" are passed by value, while "Object Types" are passed by reference.

.. note::

    Also see the `Robot Raconteur Framework Service Definition Documentation <https://robotraconteur.github.io/robotraconteur/doc/core/latest/cpp/service_definition.html>`_
    and the `Robot Raconteur Service Definition Standard <https://github.com/robotraconteur/robotraconteur_standards/blob/master/draft/service_definition.md>`_
    for more detailed information on the service definition format.

The following are additional examples of service definitions for the iRobot Create and Simple Webcam examples:

.. literalinclude:: ../../../examples/irobot_create/robdef/experimental.create3.robdef
    :linenos:

.. literalinclude:: ../../../examples/simple_webcam/robdef/experimental.simplewebcam3.robdef
    :linenos:

Standard Service Definitions
============================

Robot Raconteur provides a large number of standard service definitions that are used to define common data types and
objects used by robotics and automation devices.

`Standard Service Robot Raconteur Definitions <https://github.com/robotraconteur/robotraconteur_standard_robdef>`_

`Standard Service Robot Raconteur Definitions Documentation <https://github.com/robotraconteur/robotraconteur_standard_robdef/blob/master/README.md>`_

Frequently used standard service definitions include:

* `com.robotraconteur.geometry <https://github.com/robotraconteur/robotraconteur_standard_robdef/blob/master/group1/com.robotraconteur.geometry.robdef>`_ - Common geometry types including ``Vector3``, ``Quaternion``, and ``Pose``
* `com.robotraconteur.device <https://github.com/robotraconteur/robotraconteur_standard_robdef/blob/master/group1/com.robotraconteur.device.robdef>`_ - Structures and objects for providing common metadata about devices
* `com.robotraconteur.robotics.robot <https://github.com/robotraconteur/robotraconteur_standard_robdef/blob/master/group1/com.robotraconteur.robotics.robot.robdef>`_ - Common objects and structures for robots
* `com.robotraconteur.imaging <https://github.com/robotraconteur/robotraconteur_standard_robdef/blob/master/group1/com.robotraconteur.imaging.robdef>`_ - Common objects and structures for imaging devices

.. note::

    The "Companion" libraries for Robot Raconteur are typically used to simplify the use of standard service definitions.
    For C++ and C\# the companion libraries contain the "thunk" code for the standard service definitions, so
    it is not necessary to generate the code from the service definitions for each project.
