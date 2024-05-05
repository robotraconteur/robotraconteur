# Robot Raconteur Ecosystem {#ecosystem}

Robot Raconteur is utilized to design and build many devices, device drivers, utilities,
clients, controllers, applications, etc. These different use cases are all able
to communicate using the Robot Raconteur framework. They form an "ecosystem"
around Robot Raconteur. The Robot Raconteur project aims to develop this ecosystem
into a wide ranging set of interoperable software and devices.

## Standard Service Definitions {#stdrobdef}

The Robot Raconteur project has defined a number of standard service definitions that contain
numerous structure, pod, namedarray, and object types. These types cover a range of common
data types, and provide standardized interfaces to devices. These types should be used whenever
possible so that services will be interoperable. The standard service types are available in the
[robotraconteur_standard_robdef](https://github.com/robotraconteur/robotraconteur_standard_robdef)
GitHub repository.

## Robot Raconteur Companion Libraries {#companion}

The Robot Raconteur Companion libraries are provided to assist in using the standard service types, along with other generic utility functions. Currently, the companion libraries contain the standard service types, info file loaders, and general utility functions. The following libraries are available:

Python: [robotraconteur_companion_python](https://github.com/robotraconteur/robotraconteur_companion_python)

C++: [robotraconteur_companion](https://github.com/robotraconteur/robotraconteur_companion)

C#: [RobotRaconteurNET.Companion](https://github.com/robotraconteur/RobotRaconteurNET.Companion)

The Python companion library can also be installed using `pip install RobotRaconteurCompanion`

See [robotraconteur_camera_driver](https://github.com/robotraconteur-contrib/robotraconteur_camera_driver/blob/master/robotraconteur_camera_driver.py) for an example utilizing standard types and the companion

## Robot Raconteur Directory {#directory}

The Robot Raconteur project maintains a list of available drivers. The directory can be found here:

https://github.com/robotraconteur/robotraconteur-directory
