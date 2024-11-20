===========
Quick Start
===========

.. contents:: Table of Contents
   :depth: 1
   :local:
   :backlinks: none

Introduction
============

The quick start includes short examples to get you up and running with the Robot Raconteur communication
framework, fast!

The quick start examples use the Reynard the Robot package, a simple cartoon robot designed to help learn Robot Raconteur.
The examples include a simple service, and clients in Python, MATLAB, and LabView.

All quick start example files can be found in the ``examples/quickstart`` directory.

Setup
=====

Before running the examples, install Python 3.8 or greater. At the time of this writing, the current Python version
is Python 3.11. Python can be downloaded from the `Python Download Page <https://www.python.org/downloads/>`_,
or installed using a package manager such as ``apt``.

Install the required packages using ``pip``

.. code::

    python -m pip install --user robotraconteur pyserial opencv-contrib-python

.. note::

    On Ubuntu, Debian, and other versions of Linux, replace ``python`` with ``python3`` in all examples. It may
    also be necessary to install the ``python3-pip`` package.

The ``robotraconteur`` Python package is available for most platforms using the ``pip`` installer, but for some
platforms it needs to be installed separately. See the
`Robot Raconteur Install <https://github.com/robotraconteur/robotraconteur/blob/master/docs/common/installation.md>`_
page for more information on these platforms.

Reynard the Robot Quick Start
=============================

The Reynard the Robot quick start creates a simple service, and clients in Python, MATLAB, and LabView. The following
sections describe the examples. Use a web browser pointed to http://localhost:29201 to view the Reynard the Robot
user interface.

Reynard the Robot Quick Start Python Service
--------------------------------------------

The example uses a "service definition" with a single object, ``ReynardQuickstart``, and two functions
``say`` and ``teleport``. The ``say`` function takes the argument ``text`` and displays it in the chat
window on the Reynard page. The ``teleport`` function takes the arguments ``x`` and ``y`` and moves the
Reynard robot base to the new location.

.. code::

    service experimental.reynard_quickstart

    object ReynardQuickstart
        function void say(string text)
        function void teleport(double x, double y)
    end

More information about Service Definitions can be found in the
`Service Definition Framework Documentation <https://robotraconteur.github.io/robotraconteur/doc/core/latest/cpp/service_definition.html>`_
page.

Now, a simple service written in Python can be created:

.. literalinclude:: ../../../examples/quickstart/reynard_quickstart_service.py


Reynard Quick Start Clients
---------------------------

The Reynard Quick Start service can now be called by connecting a client. Because of the magic of Robot Raconteur,
it is only necessary to connect to the service to utilize its members. In Python and MATLAB there is no
boilerplate code, and in the other languages the boilerplate code is generated automatically.

.. literalinclude:: ../../../examples/quickstart/reynard_quickstart_client.py


In MATLAB, this client is even simpler.

.. literalinclude:: ../../../examples/quickstart/reynard_quickstart_client.m



.. |View Robot Raconteur Matlab on File Exchange| image:: https://www.mathworks.com/matlabcentral/images/matlab-file-exchange.svg
   :target: https://www.mathworks.com/matlabcentral/fileexchange/176028-robot-raconteur-matlab

The `Robot Raconteur Matlab <https://www.mathworks.com/matlabcentral/fileexchange/176028-robot-raconteur-matlab>`_
add-on can be installed using "Get Add-Ons" and "Add-On Explorer" in MATLAB.

LabView can also be used to drive the robot:

.. image:: images/labview_reynard_quickstart_client.png
   :alt: LabView Reynard Quick Start client
   :name: LabViewQuickstartClient
   :width: 600

The Robot Raconteur LabView toolbox is a commercial product available from Wason Technology, LLC.

These example clients all use a URL to connect to the robot. "Discovery" and "Subscriptions" can also be used
to connect automatically to devices, even if the URL is not known.

.. literalinclude:: ../../../examples/quickstart/reynard_quickstart_client_sub.py

Subscriptions can use "Filters" to select which service to connect. See the documentation for
`SubscribeServiceByType <https://robotraconteur.github.io/robotraconteur/doc/core/latest/python/api/RobotRaconteurNode.html#RobotRaconteur.RobotRaconteurNode.SubscribeServiceByType>`_
for more information on filters.

Service Browser
---------------

The `Robot Raconteur Service Browser <https://github.com/robotraconteur/RobotRaconteur_ServiceBrowser>`_
is a utility to browse services available on the network and find connection information.

.. image:: images/service_browser.png
   :alt: Robot Raconteur ServiceBrowser
   :name: ServiceBrowser
   :width: 600

The URL can be cut and paste from the information display. Note that this URL uses an IPv6 link-local address
``fe80::d05a:836c:e292:c3c0`` instead of the more common IPv4 address. IPv6 addresses are favored by Robot Raconteur
since the they do not require as much configuration as IPv4 when working on a local network and are more reliable.
