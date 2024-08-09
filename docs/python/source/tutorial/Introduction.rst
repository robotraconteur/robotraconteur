Introduction
============

Robot Raconteur is an Apache-2.0 licensed open-source communication
library designed to ease the integration of complex automation systems
that are composed of disparate components that run within different
processes on a computer, are distributed over a network, or are embedded
devices. These components are often produced by different vendors with
completely different interfaces that may not run on the same platforms
and are frequently mutually exclusive in terms of the API provided for
the user of the component. Add in that most modern systems are
distributed over a network and the result is a long, often frustrating
development cycle to produce a front-end that is capable of controlling
all of the elements in a high-level user friendly manner. For modern
laboratory or prototype systems this usually means producing a MATLAB,
Python, or LabView front end that can be scripted. After the prototype
is completed a high-level interface may be developed in a language like
C++. Robot Raconteur is designed specifically to ease this design
process and adds a number of additional capabilities that would
otherwise be time consuming to implement.

Robot Raconteur provides a language, platform, and technology neutral
augmented object-oriented communication system that provides the ability
for a *client* to rapidly access functionality exposed by a *service*
either within the same computer, over a network, or over a connection
technology such as USB. Robot Raconteur is built around the idea of a
*Service* exposing *Objects* to a client. (This tutorial assumes that
you are familiar with basic object-oriented programming used in Python,
C#, or Java. If you are not please review before continuing.) This is
accomplished by registering a *root object* as a service within a Robot
Raconteur *Node* that acts as the server. Object *Members* consist of
the contents of the object, and are typically *functions*, *properties*,
and *events*. (In C++, Python, and Java these are made by using helper
classes like Boost or JavaBeans.) Robot Raconteur uses an *augmented
object-oriented* model that has a number of member types: properties,
functions, events, objrefs, pipes, callbacks, wires, and memories. The
specific function of these members will be discussed later. These
members are mirrored to *Object References* (sometimes called “Proxy
Objects") in a client Robot Raconteur *Node*. These references mirror
the members and allow the client to access the members of the service
objects through a *Transport Connection* between the client and service
node. Figure `1 <#ClientServiceConfiguration>`__ shows a diagram of this
configuration. Multiple clients can access the same service
simultaneously as shown in Figure
`2 <#ClientServiceConfiguration_MultipleClients>`__.

.. figure:: figures/client-service.svg
   :alt: Configuration of Client-Service communication
   :name: ClientServiceConfiguration

   Configuration of Client-Service communication

.. figure:: figures/client-service-multi.svg
   :alt: Configuration of Client-Service communication with multiple
    clients
   :name: ClientServiceConfiguration_MultipleClients

   Configuration of Client-Service communication with multiple clients

A node can expose more than one service. Each service is registered with
a unique name that is used as part of the URL to connect to the service.

The Robot Raconteur library contains almost all of the functionality to
implement the communication between the client and service. It has been
designed to require a minimal amount of “boilerplate" coding instead
uses dynamic meta-programming when possible or code-generation when
necessary to produce the “thunk" code that implements the conversion
between the client member reference and the real member in the service.
To the user the network layer is *almost* invisible. Robot Raconteur
uses plain-text files called *Service Definition* files to define the
objects and composite data types (structures) that are used in a
service. Example
`experimental.create3 <https://github.com/robotraconteur/robotraconteur/blob/master/examples/irobot_create/robdef/experimental.create3.robdef>`_
shows an example service definition, and Section
:doc:`ServiceDefinition` goes into great detail how
these files are used. A service definition is a very simple way to
define the *interface* to the service objects. The service definition
provides a “lowest-common denominator" to all the languages that Robot
Raconteur supports. These service definitions are used as the input to
code generation or dynamic programming (such as in Python) and can
result in tens of thousands of lines of codes in some situations that
would otherwise need to be written manually. A very unique feature of
Robot Raconteur is that it sends these service definition files at
runtime when a client connects. This means that a dynamic language like
Python or MATLAB does not need any a priori information about a service;
these languages can simply connect and provide a fully functional
interface dynamically. This is *extremely* powerful for prototyping and
is the initial motivation for the development of Robot Raconteur.

Beyond the client-service communication, Robot Raconteur has a number of
highly useful support features. Robot Raconteur has auto-discovery,
meaning that nodes can find each other based on the type of the root
object service among other criteria. Authentication is available to
control access to the services on a Node. Multi-hop routing is
implemented meaning that nodes can be used as routers between a client
and service node. This is mainly implemented for access control and
transition between different transport technologies. Finally, Robot
Raconteur provides exclusive object locks that allow clients to request
exclusive use of objects within a service to prevent access collisions.

The core Robot Raconteur library is written in C++ using the
Boost:raw-latex:`\cite{boost}` libraries and is capable of running on a
number of platforms. It has minimal dependencies making it highly
portable. Currently RR has been compiled and tested on Windows (x86 and
x64), Linux (x86, x86_64, ARM hard-float, ARM soft-float, PowerPC,
MIPS), Mac OSX, Android (ARM, x86), and iOS. Language bindings are used
to allow access to the library from other languages. Additional
supported languages include C#, Java, Python, and MATLAB. (The LabView
bindings are available as a commercial product from Wason Technology, LLC)

Two additional implementations are available for use in a web browser
written in JavaScript and for use in an ASP.NET server written in pure
C#. These implementations take advantage of the support for
WebSockets:raw-latex:`\cite{websocket}` over TCP to allow for
compatibility with existing web infrastructure. The HTML5/Javascript
implementation has been tested with Chrome, Firefox, Internet Explorer,
Edge, and Safari. The pure C# implementation (Called
RobotRaconteurWeb) has been tested with IIS and allows RR to run
inside a web server.

Robot Raconteur communication is based on a simple message passing
system with a well defined format that is simple enough that a fully
functional (yet simple) service service has been implemented on an
Arduino UNO (2 kB RAM, 32 kB flash) *with full auto-discovery* using a
customized version of the core Robot Raconteur functionality.The
supported languages and platforms will continue to expand over time so
check back frequently.

The rest of this document will provide a tutorial format to introduce
the plethora of features available. It serves as the primary overview of
Robot Raconteur and as the documentation for the Python library. Other
languages will have shorter documents that describe the specific use of
Robot Raconteur in that language. While Robot Raconteur is feature-rich
and has a learning curve, it is not necessary to use all the features
available in all cases. Once Robot Raconteur is learned, new services
can be developed *very* quickly compared to competing technologies, and
frequently services have been developed for application in less than an
hour that would have otherwise taken days.

Robot Raconteur provides network security through TLS encryption and
certificate based authentication. Details on TLS and certificates are
covered in the document *Robot Raconteur Security using TLS and
Certificates*.

Python examples can be found in the
`examples/ directory <https://github.com/robotraconteur/robotraconteur/tree/master/examples>`_
GitHub repository.

Example Robot
-------------

This tutorial utilizes a iRobot Create that has been augmented with two
webcams, a power converter, and a Raspberry Pi 3 ARM computer. These
robots will be available upon request. Figure `3 <#examplerobot>`__
shows a picture of the robot. Services will be developed in this
tutorial to drive the robot and access the webcams. Clients will use the
services to control the robot and read the sensors.

.. figure:: figures/rrcreate.JPG
   :alt: Photo of the example robot
   :name: examplerobot

   Photo of the example robot
