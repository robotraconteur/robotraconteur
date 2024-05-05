Standard Service Definitions and Companion
==========================================

The Robot Raconteur project has defined a number of standard service definitions that contain
numerous structure, pod, namedarray, and object types. These types cover a range
of common data types, and provide standardized interfaces to devices. These types
should be used whenever possible so that services will be interoperable. The
standard service types are available in the
`robotraconteur_standard_robdef <https://github.com/robotraconteur/robotraconteur_standard_robdef>`_
GitHub repository.

The Robot Raconteur Companion libraries are provided to assist in using the standard
service types, along with other generic utility functions. Currently, the
companion libraries contain the standard service types, info file loaders,
and general utility functions. The following libraries are available:

 - Python: `robotraconteur_companion_python <https://github.com/robotraconteur/robotraconteur_companion_python>`_
 - C++: `robotraconteur_companion <https://github.com/robotraconteur/robotraconteur_companion>`_
 - C#: `RobotRaconteurNET.Companion <https://github.com/robotraconteur/RobotRaconteurNET.Companion>`_

The Python companion library can also be installed using ``pip install RobotRaconteurCompanion``

See `robotraconteur_camera_driver <https://github.com/robotraconteur-contrib/robotraconteur_camera_driver/blob/master/robotraconteur_camera_driver.py>`_
for an example utilizing standard types and the companion library.
