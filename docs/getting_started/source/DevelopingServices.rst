.. _developing_services:

===============================
Developing Drivers and Services
===============================

Developing services is typically done experienced developers, and is not covered by the getting started
guide except for some general guidelines.

The "Reynard the Robot", "Simple Webcam", and "iRobot Create" examples contained in the
`robotraconteur Repository <https://github.com/robotraconteur/robotraconteur>`_ all contain examples of
services that can be used as reference when developing your own services. These services
are not as advanced as full device drivers but are good examples of how to develop services.

The device drivers listed in the :ref:`common_devices` section are examples of device drivers that can be
used as reference when developing your own device drivers. These drivers are fully functional
and can be used as a reference when developing your own device drivers.

Some basic guidelines for developing device drivers and services are:

* Services are typically developed in Python, C++, C\#, LabView, or Java.
* Use the `Standard Service Definitions <https://github.com/robotraconteur/robotraconteur_standard_robdef>`_
  whenever possible. This will make your service more compatible with other
  services and clients. Even if a new service definition is designed, use the standard data types
  whenever possible.
* Use ``ServerNodeSetup`` to initialize the node.
* Use the companion library for your language to simplify the development of services.
* Whenever possible implement the ``com.robotraconteur.device.Device`` interface for your service to allow
  it to be discovered by clients as a device. Make sure to also populate the service attributes
  so that the device will be correctly discovered. See the examples in the :ref:`common_devices` section
  to see how the device information is populated in the service.
* When designing service definitions, follow these conventions:

  * Service names should use Java style package names, using reverse domain name order. All letters should be lowercase. `experimental` or `communication` can be used for the TLD if no domain is available.
  * Enumeration, structure, pod, namedarray, and object names should be nouns with each internal word capitalized (UpperCamelCase)
  * All letters in constant names should be capitalized with internal words separated with underscores (ALL_CAPS)
  * Field names, member names, parameter names, modifier names, and enumeration values should use lowercase for letters, and separate each internal word with underscores (snake_case)
  * Service definition scope declaration should not be indented. Fields, members, and enum values should be indented four spaces. Tabs should not be used for indentation.
  * It is suggested that lines be split after 79 characters using the line continuation character `\\`
  * Line continuations should be indented four spaces more than the line before the continuation. Additional line continuations should match the indentation of the first continuation.
  * Comments should match the indentation of the relevant declaration
