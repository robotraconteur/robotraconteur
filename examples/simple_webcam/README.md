# Simple Webcam Robot Raconteur Examples

This directory contains examples of using Robot Raconteur clients with webcams. The services use OpenCV to capture
images from the webcam, and uses Robot Raconteur to provide the images to clients.
Examples are provided for a single and multi-camera system. Client examples demonstrate
capturing single images, streaming images, and using memory members.

Examples are broken down by language:

- [robdef](robdef) - The Robot Raconteur definition files for simple webcam used by all languages.
- [Python](python) - Python examples
- [C++](cpp) - C++ examples
- [C#](cs) - C# examples
- [Java](java) - Java examples
- [Matlab](matlab) - Matlab examples
- [LabView](labview) - NI LabView examples

Practical systems should use the standard camera driver [robotraconteur_camera_driver](https://github.com/robotraconteur-contrib/robotraconteur_camera_driver) which uses
the standard  [service definition
`com.robotraconteur.imaging`](https://github.com/robotraconteur/robotraconteur_standard_robdef/blob/master/group1/com.robotraconteur.imaging.robdef).

Instructions for atomically running the service on a Raspberry Pi are provided in the [C++ Service Readme](cpp/service/README.md).
