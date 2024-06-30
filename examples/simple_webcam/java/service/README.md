# Simple Webcam Java Robot Raconteur Service Example

This directory contains examples of Robot Raconteur services with webcams. The services use OpenCV to capture
images from the webcam, and uses Robot Raconteur to provide the images to clients. Two services are provided,
a single camera service and a multi-camera service.

The simple webcam example demonstrates using `property`, `function`, `pipe`, `objref`, and `memory` members. It also
demonstrates using constants and structures.

Instructions are provided for Windows and Ubuntu. These examples will work
on other platforms with minor modifications.

## Connection Info

The single and multi camera services by default listen on TCP port 22355. Note that only
one of the services can be used at a time because they listen on the same TCP port. Use the following connection
information to connect to the services:

- Single Camera Service
  - URL: `rr+tcp://localhost:22355?service=webcam`
  - Node Name: `experimental.simplewebcam3`
  - Service Name: `webcam`
  - Root Object Type: `experimental.simplewebcam3.Webcam`
- Multi Camera Service
  - URL: `rr+tcp://localhost:22355?service=multiwebcam`
  - Node Name: `experimental.simplewebcam3_multi`
  - Service Name: `multiwebcam`
  - Root Object Type: `experimental.simplewebcam3.WebcamHost`

## Setup

The Java OpenJDK 11 or greater is required to compile the example.

This example requires the Robot Raconteur Java files. Download the file `RobotRaconteur-Java-<ver>-Java.zip` and
extract the contents to `C:\ws\robotraconteur_java` on Windows on `$HOME/robotraconteur_java` on Linux/Mac OS.

The OpenCV Java library `opencv.jar` is required. Download the OpenCV Java library from Maven Central Repository
and place it in the `C:\ws\robotraconteur_java` directory on Windows or `$HOME/robotraconteur_java` on Linux/Mac OS.

## Compiling

The thunk source must be generated manually in Java. Execute the following command:

```
RobotRaconteurGen --thunksource --lang=java ..\..\robdef\experimental.simplewebcam3.robdef
```

Run the following command to build:

```
javac -cp C:\ws\robotraconteur_java\RobotRaconteur.jar;C:\ws\robotraconteur_java\opencv.jar *.java experimental\simplewebcam3\*.java
```

## Run Example

```
java -cp C:\ws\robotraconteur_java\RobotRaconteur.jar;C:\ws\robotraconteur_java\opencv.jar;. -Djava.library.path=C:\ws\robotraconteur_java SimpleWebcamService
```

Replace `SimpleWebcamService` with `MultiSimpleWebcamService` to run the multi camera service example.
