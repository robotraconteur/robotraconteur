.. _exceptions:

==========
Exceptions
==========

.. contents:: Table of Contents
   :depth: 1
   :local:
   :backlinks: none

Overview
========

Robot Raconteur uses exceptions to handle errors. Robot Raconteur has a built-in set of exceptions defined
by the library, and also allows custom exceptions to be defined in service definitions.
See the `Framework Exceptions Documentation <https://robotraconteur.github.io/robotraconteur/doc/core/latest/cpp/exceptions.html>`_
for a full list of exceptions, and the documentation for each programming language for how they are used.
All custom exceptions extend ``RobotRaconteurRemoteException``.

Robot Raconteur can transparently pass exceptions from the service to the client (or client to service for callbacks).
The following operations will transparently pass exceptions:
* ``property`` get and set
* ``function`` calls
* ``objref`` get
* ``pipe`` connect and close endpoints (for client)
* ``callback`` calls (passing from client to service)
* ``wire`` connect, close (for clients), peek, and poke
* ``memory`` all operations

The example service definition ``experimental.exception_example`` contains a custom exception and an object
with functions that will throw exceptions as an example. The service definition is shown below:

.. literalinclude:: ../../../examples/features/exceptions/robdef/experimental.exception_example.robdef
    :linenos:

The following example code demonstrates how to catch exceptions thrown by the example service:

.. tabs::

    .. group-tab:: Python

        .. literalinclude:: ../../../examples/features/exceptions/python/exceptions_example.py
            :language: python
            :linenos:

    .. group-tab:: MATLAB

        .. literalinclude:: ../../../examples/features/exceptions/matlab/exceptions_example.m
            :language: matlab
            :linenos:

    .. group-tab:: LabView

        .. raw:: html

            <div style="overflow: auto; width: 100%; height: 400px;">
                <img src="https://raw.githubusercontent.com/robotraconteur/robotraconteur/master/examples/features/exceptions/labview/exceptions_example.png" style="max-width: none;">
            </div>

    .. group-tab:: C\#

        .. literalinclude:: ../../../examples/features/exceptions/cs/exceptions_example.cs
            :language: csharp
            :linenos:

    .. group-tab:: C++

        .. literalinclude:: ../../../examples/features/exceptions/cpp/exceptions_example.cpp
            :language: cpp
            :linenos:
