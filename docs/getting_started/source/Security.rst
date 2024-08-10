.. _security:

========
Security
========

.. contents:: Table of Contents
   :depth: 1
   :local:
   :backlinks: none

Overview
========

Robot Raconteur supports using credentials to authenticate clients, and supports encrypted communication using
TLS. The security features of Robot Raconteur are designed to be flexible and easy to use.

Authentication
==============

Robot Raconteur supports using credentials to authenticate clients. The server can require clients to provide
a username and credentials, an optionally a TLS client certificate. The credentials are typically
a password, but can be any Robot Raconteur Map containing other information such as a token or one-time password.

The following examples demonstrate connecting to a service that requires a username and password:

.. tabs::

    .. group-tab:: Python

        .. literalinclude:: ../../../examples/features/authentication/python/authentication_example.py
            :language: python
            :linenos:

    .. group-tab:: MATLAB

        .. literalinclude:: ../../../examples/features/authentication/matlab/authentication_example.m
            :language: matlab
            :linenos:

    .. group-tab:: LabView

        .. raw:: html

            <div style="overflow: auto; width: 100%; height: 400px;">
                <img src="https://raw.githubusercontent.com/robotraconteur/robotraconteur/master/examples/features/authentication/labview/authentication_example.png" style="max-width: none;">
            </div>

    .. group-tab:: C\#

        .. literalinclude:: ../../../examples/features/authentication/cs/authentication_example.cs
            :language: csharp
            :linenos:

    .. group-tab:: C++

        .. literalinclude:: ../../../examples/features/authentication/cpp/authentication_example.cpp
            :language: cpp
            :linenos:

TLS Encryption
==============

Robot Raconteur supports encrypted communication using TLS. Wason Technology provides a certificate authority
and sells certificates for use with Robot Raconteur. All production devices using Robot Raconteur are
expected to use certificates provided by Wason Technology.

On the server side, the server should use ``SecureServerNodeSetup`` to set up the service node with the appropriate
configuration. There must be a certificate and private key file available that matches the NodeID uuid
of the service node. See the framework documentation for more information.

On the client side, simply replace the ``rr+tcp://`` or ``rr+ws://`` URL scheme with ``rr+tcps://`` or ``rrs+ws://``.
For instance, to connect to a service using TLS, use the URL ``rrs+tcp://192.168.1.10:1234?service=my_service`` instead of
``rr+tcp://192.168.1.10:1234?service=my_service``. Otherwise no other changes are necessary for the client, although it is highly
recommended to include the `?nodeid=xxx` parameter in the URL to prevent sending credentials to the wrong device.
For example, ``rrs+tcp://192.168.1.10:1234?nodeid=7932d2a6-b7e1-4068-82d9-16f93e868ed9&service=my_service``.

When connecting to web servers, the URL scheme should be ``rr+wss://`` should be used. The ``rr+wss://`` scheme
uses standard DNS based certificate validation, and the certificate must be signed by a standard certificate authority.
This is in contrast to the ``rrs+tcp://`` scheme, which uses the Robot Raconteur certificate authority. For example,
the URL used in the web server example ``rr+wss://wstest2.wasontech.com/robotraconteur?service=testobj`` will
use standard certificate validation.
