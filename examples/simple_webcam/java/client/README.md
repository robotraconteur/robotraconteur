# Simple Webcam Java Robot Raconteur Client Examples

Several examples are provided for clients communicating with the simple webcam service. These examples expect
either the `SimpleWebcamService` or `MultiSimpleWebcamService` services to be running depending on the example.

- `SimpleWebcamClient` - (`MultiSimpleWebcamService`) Connects to the multi camera service and captures a single frame
   from one or two cameras using the `capture_frame()` function member.
   The captured images are displayed for the user. Select one of the windows and press enter
   to close the window and exit the program.
- `SimpleWebcamClient_streaming` - (`SimpleWebcamService`) Connects to the single camera service and streams images
   from the camera using the `frame_stream` pipe member.
   The images are displayed for the user. Press escape to close the window and exit the program.
- `SimpleWebcamClient_memory` - (`SimpleWebcamService`) Connects to the single camera service and captures
   a frame using the `capture_frame_to_buffer()` function member. This function captures a frame and stores it to
   the buffers used by the `buffer` and `multidimbuffer` memory members. Segments of the memory members are displayed.
   This example is useful for demonstrating how to use memory members in Robot Raconteur.

These examples assume that the corresponding service is running. Any of the simple webcam service
examples should work regardless of language. Pass the correct URL as the first argument on the
command line of a different URL is required for the connection, for example if the service is running on a different
machine.

The simple webcam example demonstrates using `property`, `function`, `pipe`, `objref`, and `memory` members. It also
demonstrates using constants and structures.

Instructions are provided for Windows and Ubuntu. These examples will work
on other platforms with minor modifications.

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

Start the appropriate service from any of the simple webcam service examples and run:

```
java -cp C:\ws\robotraconteur_java\RobotRaconteur.jar;C:\ws\robotraconteur_java\opencv.jar;. -Djava.library.path=C:\ws\robotraconteur_java SimpleWebcamClient
```

Replace `SimpleWebcamClient` with `SimpleWebcamClient_streaming` or `SimpleWebcamClient_memory` to run the other examples.
