# Reynard Java Robot Raconteur Service Example HTTP REST Backend

This example demonstrates a Robot Raconteur service to control Reynard using Java with HTTP REST communication
with the robot. This example is intended to be representative of the types of drivers that are used with real
industrial robots and devices.

## Setup

The Java OpenJDK 11 or greater is required to compile the example. The Python Reynard the Robot server must be running.
See the [Python example README](../../python/client/README.md) for instructions setting up Reynard the Robot.

This example requires the Robot Raconteur Java files. Download the file `RobotRaconteur-Java-<ver>-Java.zip` and
extract the contents to `C:\ws\robotraconteur_java` on Windows on `$HOME/robotraconteur_java` on Linux/Mac OS.
`gson.jar`, a JSON library, is also required. Download the file `gson-<ver>.jar` and place it in the same directory as
`RobotRaconteur.jar`. See [gson](https://github.com/google/gson).

## Compiling

The thunk source must be generated manually in Java. Execute the following command:

```cmd
RobotRaconteurGen --thunksource --lang=java ..\..\..\robdef\experimental.reynard_the_robot.robdef
```

Run the following command to build:

```cmd
javac -cp C:\ws\robotraconteur_java\RobotRaconteur.jar;C:\ws\robotraconteur_java\gson.jar;. reynard_robotraconteur_service_http_rest.java experimental\reynard_the_robot\*.java
```

## Run Example

Start the Reynard the Robot server:

```cmd
python -m reynard_the_robot --disable-robotraconteur
```

On Linux, `python3` may need to be used instead of `python`.

Run the following command in a separate command prompt:

```cmd
java -cp C:\ws\robotraconteur_java\RobotRaconteur.jar;C:\ws\robotraconteur_java\gson.jar;. -Djava.library.path=C:\ws\robotraconteur_java reynard_robotraconteur_service_http_rest
```

The example Robot Raconteur service listens on port 59201. Python can be used to connect to the service:

```python
from RobotRaconteur.Client import *
c = RRN.ConnectService('rr+tcp://localhost:59201?service=reynard')
c.say('Hello World!')
```
