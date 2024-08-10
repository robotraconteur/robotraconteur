.. _web:

========================
Web Browsers and Servers
========================

.. contents:: Table of Contents
   :depth: 1
   :local:
   :backlinks: none

Overview
========

Robot Raconteur supports using `Web Sockets <https://en.wikipedia.org/wiki/WebSocket>`_ to communicate with
services. The Web Socket protocol is an extension to HTTP that allows for full-duplex communication between
a client and a server. The Robot Raconteur TCP transport is capable of using Web Sockets for both client
and server communication. This allows for services to be accessed from web browsers, and for services to
be hosted on web servers. Robot Raconteur supports using standard HTTPS encryption and certificate chains
for secure communication.

Allowing Web Socket Connections
===============================

Web sockets use "origin" based security. The origin is the domain name of the web page that is making the
connection. The server can allow or deny connections based on the origin. Robot Raconteur uses origin
control to prevent `cross-site scripting attacks <https://en.wikipedia.org/wiki/Cross-site_scripting>`_. where
random web pages attempt to connect to a service.

By default, the following origins are allowed:

* ``null``
* ``file://``
* ``chrome-extension://``
* ``http://robotraconteur.com``
* ``http://robotraconteur.com:80``
* ``http://*.robotraconteur.com``
* ``http://*.robotraconteur.com:80``
* ``https://robotraconteur.com``
* ``https://robotraconteur.com:443``
* ``https://*.robotraconteur.com``
* ``https://*.robotraconteur.com:443``

For pages loaded off the filesystem, the origin is either ``null`` or ``file://``. For pages loaded from a web server,
the origin is the domain name of the server. The port number is also sometimes included in the origin.

Any other origin, including ``localhost``, will be denied by default. To allow additional origins, the command
line argument ``--robotraconteur-tcp-ws-add-origin=`` or the
``TcpTransport::RegisterWebSocketAllowedOrigin()`` API function can be used. The command line arguments
are process by the ``ServerNodeSetup`` instance that is used to initialize the node. The
origin control is configured on the service side.

For example, to start Reynard the Robot and allow connections from ``http://localhost:8000``, run the following:

.. code-block:: bash

    python -m reynard_the_robot --robotraconteur-tcp-ws-add-origin=http://localhost:8000

Robot Raconteur Web Browser Clients using Pyodide
=================================================

`Pyodide <https://pyodide.org/>`_ is a Python runtime for the web.
Pyodide allows for Python scripts to be executed in a web browser. It is implemented using Web Assembly (WASM).
Robot Raconteur is included as a standard package in Pyodide so there is no installation required. The
Robot Raconteur package uses `HTTP Web Sockets <https://en.wikipedia.org/wiki/WebSocket>`_ to communicate with services.

The following is an example of a simple web page that uses Pyodide to connect to a Reynard the Robot
and provides a simple user interface:

.. collapse:: Click to view the Pyodide example code

    .. literalinclude:: ../../../examples/reynard_the_robot/pyodide/reynard_the_robot_client_pyodide.py
        :language: python
        :linenos:

    .. literalinclude:: ../../../examples/reynard_the_robot/pyodide/reynard_the_robot_client_pyodide.html
        :language: html
        :linenos:

|
Robot Raconteur Web Browser Clients using Robot Raconteur Web
=============================================================

Robot Raconteur clients can be implemented in Java Script using Robot Raconteur Web and the H5 C\# to
JavaScript compiler. See the `Robot Raconteur Web <https://github.com/robotraconteur/robotraconteurweb>`_
repository for more information.

Robot Raconteur Web Servers
===========================

Robot Raconteur Web is a pure C\# implementation of the Robot Raconteur transport system that can be used
inside an ASP.NET Core web server. This allows for Robot Raconteur clients to connect to standard
web servers without any specialized infrastructure. See the
`Robot Raconteur Web <https://github.com/robotraconteur/robotraconteurweb>`_
repository for more information.

The ``rr+wss://wstest2.wasontech.com/``
server is a minimal example of a Robot Raconteur running inside a standard web server, and is
used for testing and demonstration purposes.

The following examples demonstrate connecting to the ``rr+wss://wstest2.wasontech.com/`` server:

.. tabs::

    .. group-tab:: Python

        .. literalinclude:: ../../../examples/features/web_https_client/python/web_https_client.py
            :language: python
            :linenos:

    .. group-tab:: MATLAB

        .. literalinclude:: ../../../examples/features/web_https_client/matlab/web_https_client.m
            :language: matlab
            :linenos:

    .. group-tab:: LabView

        .. raw:: html

            <div style="overflow: auto; width: 100%; height: 400px;">
                <img src="https://raw.githubusercontent.com/robotraconteur/robotraconteur/master/examples/features/web_https_client/labview/web_https_client.png" style="max-width: none;">
            </div>

    .. group-tab:: C\#

        .. literalinclude:: ../../../examples/features/web_https_client/cs/web_https_client.cs
            :language: csharp
            :linenos:

    .. group-tab:: C++

        .. literalinclude:: ../../../examples/features/web_https_client/cpp/web_https_client.cpp
            :language: cpp
            :linenos:
