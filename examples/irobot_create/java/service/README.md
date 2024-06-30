# iRobot Create Java Robot Raconteur Service Example

The iRobot Create service provides a simple example of a Robot Raconteur service. The service uses the
serial port OI interface to communicate with the iRobot Create. The service allows clients to drive the robot,
control the LEDs, and receive state feedback information.

The iRobot Create example demonstrates using `property`, `function`, `event`, `callback`, and `wire` members.
It also demonstrates using constants and structures.

Instructions are provided for Windows and Ubuntu. These examples will work
on other platforms with minor modifications.

## Connection Info

The iRobot Create service by defaults listens on port 22354. Use the following connection information
to connect to the service:

- URL: `rr+tcp://localhost:22354?service=create`
- Node Name: `experimental.create3`
- Service Name: `create`
- Root Object Type: `experimental.create3.Create`

## Setup

The Java OpenJDK 11 or greater is required to compile the example.

This example requires the Robot Raconteur Java files. Download the file `RobotRaconteur-Java-<ver>-Java.zip` and
extract the contents to `C:\ws\robotraconteur_java` on Windows on `$HOME/robotraconteur_java` on Linux/Mac OS.

The OpenCV Java library `jSerialComm.jar` is required. Download the jSerialComm Java library from Maven Central Repository
and place it in the `C:\ws\robotraconteur_java` directory on Windows or `$HOME/robotraconteur_java` on Linux/Mac OS.

## Compiling

The thunk source must be generated manually in Java. Execute the following command:

```
RobotRaconteurGen --thunksource --lang=java ..\..\robdef\experimental.create3.robdef
```

Run the following command to build:

```
javac -cp C:\ws\robotraconteur_java\RobotRaconteur.jar;C:\ws\robotraconteur_java\jSerialComm.jar *.java experimental\create3\*.java
```

## Run Example

```
java -cp C:\ws\robotraconteur_java\RobotRaconteur.jar;C:\ws\robotraconteur_java\jSerialComm.jar;. -Djava.library.path=C:\ws\robotraconteur_java iRobotCreateService COM4
```

Replace `COM4` with the serial port of the iRobot Create. On Linux the serial port may be `/dev/ttyUSB0`.
