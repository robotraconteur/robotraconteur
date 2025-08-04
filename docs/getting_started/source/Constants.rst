.. _constants:

=========
Constants
=========

.. contents:: Table of Contents
   :depth: 1
   :local:
   :backlinks: none

Overview
========

Robot Raconteur defines two types of constants: ``enum`` and ``const``. An ``enum`` is a set of named integer
constants, while a ``const`` is a named constant of a specific type.
Constants are made available to clients when they connect to a service, or are generated
as part of "thunk" code for statically typed languages.

Enum Types
==========

Enums are defined using the ``enum`` keyword.

The following is an example of an enum definition from ``com.robotraconteur.robotics.robot`` service definition:

.. code-block::

    enum RobotCommandMode
        invalid_state = -1,
        halt = 0,
        jog,
        trajectory,
        position_command,
        velocity_command,
        homing
    end

Example code excerpts demonstrating the use of enums:

.. tabs::

    .. group-tab:: Python

        .. literalinclude:: ../../../examples/standard_devices/robot/python/robot_client_trajectory.py
            :language: python
            :lines: 21-26

    .. group-tab:: MATLAB

        .. code-block::

            c = RobotRaconteur.ConnectService(url);
            robot_const = RobotRaconteur.GetConstants(c,'com.robotraconteur.robotics.robot');
            robot_command_mode = robot_const.RobotCommandMode;
            halt_mode = robot_command_mode.halt;
            position_command_mode = robot_command_mode.position_command;

    .. group-tab:: LabView

        LabView does not support retrieving constants from service definitions. Use the values directly in your code.

    .. group-tab:: C\#

        .. literalinclude:: ../../../examples/standard_devices/robot/cs/robot_client_trajectory.cs
            :language: csharp
            :lines: 38-41

    .. group-tab:: C++

        .. literalinclude:: ../../../examples/standard_devices/robot/cpp/robot_client_trajectory.cpp
            :language: cpp
            :lines: 35-38

Constants
=========

Constants are declared using the ``const`` keyword. Constants can be numbers, numeric arrays, or strings.
Strings can use JSON encoding to represent special characters. ``const`` can be defined at the service level or within
an object.

Constants are accessed the same way as enums, but there is a single value.

The following is an example of a constant definition from the ``experimental.create3.Create`` object:

.. literalinclude:: ../../../examples/irobot_create/robdef/experimental.create3.robdef
    :lines: 39-42,62

Example code excerpts demonstrating the use of constants:

.. tabs::

    .. group-tab:: Python

        .. code-block:: python

            c = RRN.ConnectService(url)
            create_const = RobotRaconteur.GetConstants('experimental.create3.Create',c)
            drive_straight = create_const['Create']['DRIVE_STRAIGHT']
            spin_clockwise = create_const['Create']['SPIN_CLOCKWISE']
            spin_counterclockwise = create_const['Create']['SPIN_COUNTERCLOCKWISE']

    .. group-tab:: MATLAB

        .. code-block::

            c = RobotRaconteur.ConnectService(url);
            robot_const = RobotRaconteur.GetConstants(c,'com.robotraconteur.robotics.robot');
            drive_straight = robot_const.Create.DRIVE_STRAIGHT;
            spin_clockwise = robot_const.Create.SPIN_CLOCKWISE;
            spin_counterclockwise = robot_const.Create.SPIN_COUNTERCLOCKWISE;

    .. group-tab:: LabView

        LabView does not support retrieving constants from service definitions. Use the values directly in your code.

    .. group-tab:: C\#

        .. code-block:: csharp

            var drive_straight = experimental.create3..Create.experimental__create3Constants.DRIVE_STRAIGHT;
            var spin_clockwise = experimental.create3.Create.experimental__create3Constants.SPIN_CLOCKWISE;
            var spin_counterclockwise = experimental.create3.Create.experimental__create3Constants.SPIN_COUNTERCLOCKWISE;

    .. group-tab:: C++

        .. code-block:: cpp

            auto drive_straight = experimental::create3::experimental__create3Constants::Create::DRIVE_STRAIGHT;
            auto spin_clockwise = experimental::create3::experimental__create3Constants::Create::SPIN_CLOCKWISE;
            auto spin_counterclockwise = experimental::create3::experimental__create3Constants::Create::SPIN_COUNTERCLOCKWISE;
