# iRobot Create Java Robot Raconteur Client Examples

The iRobot Create Client example demonstrates how to use the iRobot Create service with Robot Raconteur.
The client drives the robot for a short distance, reads the states, and provides a callback for playing
a short song. (In most cases a callback would not be used for this function, but it is a good example of how to use callbacks.)

Press the "Play" button on the Create to play the song. The callback will be called when the song is played.

This example assumes that the iRobot Create Service is running. Any implementation of the service from the examples
can be used. Pass the correct URL as the first argument on the
command line of a different URL is required for the connection, for example if the service is running on a different
machine.

The iRobot Create example demonstrates using `property`, `function`, `event`, `callback`, and `wire` members.
It also demonstrates using constants and structures.

Instructions are provided for Windows and Ubuntu. These examples will work
on other platforms with minor modifications.

## Setup

The Java OpenJDK 11 or greater is required to compile the example.

This example requires the Robot Raconteur Java files. Download the file `RobotRaconteur-Java-<ver>-Java.zip` and
extract the contents to `C:\ws\robotraconteur_java` on Windows on `$HOME/robotraconteur_java` on Linux/Mac OS.

## Compiling

The thunk source must be generated manually in Java. Execute the following command:

```
RobotRaconteurGen --thunksource --lang=java ..\..\robdef\experimental.create3.robdef
```

Run the following command to build:

```
javac -cp C:\ws\robotraconteur_java\RobotRaconteur.jar *.java experimental\create3\*.java
```

## Run Example

Start one of the iRobot Create service examples and run:

```
java -cp C:\ws\robotraconteur_java\RobotRaconteur.jar;. -Djava.library.path=C:\ws\robotraconteur_java iRobotCreateClient
```
