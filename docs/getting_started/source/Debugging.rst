.. _debugging:

=========
Debugging
=========

.. contents:: Table of Contents
   :depth: 1
   :local:
   :backlinks: none

Logging
=======

Robot Raconteur provides powerful logging capabilities to help debug and diagnose issues.

Logging can be enabled using an environmental variable, a command line argument, or programmatically using the API.

The following log levels are supported:

* ``TRACE``: Very detailed logs. Only available in debug builds.
* ``DEBUG``: Detailed logs for debugging with information about caught exceptions.
* ``INFO``: Informational logs.
* ``WARNING``: Warnings that may indicate a problem.
* ``ERROR``: Errors that may indicate a problem.
* ``FATAL``: Errors that will cause the program to exit.

By default, the node is in ``WARNING`` log level.

To set the log level using an environmental variable:

Windows:

.. code-block:: bash

    set ROBOTRACONTEUR_LOG_LEVEL=DEBUG

Linux or MacOS:

.. code-block:: bash

    export ROBOTRACONTEUR_LOG_LEVEL=DEBUG

To set the log level using a command line argument:

.. code-block:: bash

    python -m reynard_the_robot --robotraconteur-log-level=DEBUG

The command line arguments are configured by the ``ClientNodeSetup`` instance that is used to initialize the node.
(In Python, the ``from RobotRaconteur.Client import *`` statement automatically creates a ``ClientNodeSetup`` instance.)

To set programmatically, use the one of the ``SetLogLevel`` functions in ``RobotRaconteurNode``. See the
API documentation for each programming language for more information.

In MATLAB, it is necessary to use the API to enable logging and send the log output to a file:

.. code-block:: matlab

    RobotRaconteur.SetLogLevel('DEBUG');
    RobotRaconteur.OpenLogFile('log.txt');

Command Line Options
======================

Robot Raconteur supports a number of command line options to help with debugging and diagnostics. See
the `Command Line Arguments Wiki Page <https://github.com/robotraconteur/robotraconteur/wiki/Command-Line-Options>`_
for a full list of available command line options.

Python Debugging
================

Python uses two additional environmental variables to enable additional debugging capabilities:

* ``ROBOTRACONTEUR_PYTHON_TRACEBACK_PRINT_EXC``: Enables detailed tracebacks for exceptions caught by Robot Raconteur.
* ``ROBOTRACONTEUR_PYTHON_ENABLE_DEBUGPY``: Enable support for the Visual Studio Code debugger so that breakpoints
  will work with the Robot Raconteur thread pool threads.

The following is a normal configuration for debugging in Python:

Windows:

.. code-block:: bash

    set ROBOTRACONTEUR_LOG_LEVEL=DEBUG
    set ROBOTRACONTEUR_PYTHON_TRACEBACK_PRINT_EXC=1
    set ROBOTRACONTEUR_PYTHON_ENABLE_DEBUGPY=TRUE

Linux or MacOS:

.. code-block:: bash

    export ROBOTRACONTEUR_LOG_LEVEL=DEBUG
    export ROBOTRACONTEUR_PYTHON_TRACEBACK_PRINT_EXC=1
    export ROBOTRACONTEUR_PYTHON_ENABLE_DEBUGPY=TRUE

A sample Visual Studio Code launch configuration is shown below:

.. code-block:: json

    {
        "name": "Python Debugger: Current File",
        "type": "debugpy",
        "request": "launch",
        "program": "${file}",
        "console": "integratedTerminal",
        "env": {
            "ROBOTRACONTEUR_PYTHON_TRACEBACK_PRINT_EXC": "1",
            "ROBOTRACONTEUR_PYTHON_ENABLE_DEBUGPY": "TRUE",
            "ROBOTRACONTEUR_LOG_LEVEL": "WARNING"
        }
    },

Taps
====

Robot Raconteur supports the use of taps to monitor and debug communication between clients and services.
Taps send all log messages and message traffic to a local socket. A log recorder program
can be used to record the log messages and message traffic to a file for later analysis.

See the `Taps Wiki Page <https://github.com/robotraconteur/robotraconteur/wiki/Taps>`_ for more information on
taps.
