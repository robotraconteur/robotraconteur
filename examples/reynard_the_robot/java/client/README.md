# Reynard Java Robot Raconteur Client Example

This example demonstrates controlling Reynard using Java as a Robot Raconteur client.

## Setup

The Java OpenJDK 11 or greater is required to compile the example. The Python Reynard the Robot server must be running.
See the [Python example README](../../python/client/README.md) for instructions setting up Reynard the Robot.

This example requires the Robot Raconteur Java files. Download the file `RobotRaconteur-Java-<ver>-Java.zip` and
extract the contents to `C:\ws\robotraconteur_java` on Windows on `$HOME/robotraconteur_java` on Linux/Mac OS.

## Compiling

The thunk source must be generated manually in Java. Execute the following command:

```
RobotRaconteurGen --thunksource --lang=java ..\..\robdef\experimental.reynard_the_robot.robdef
```

Run the following command to build:

```
javac -cp C:\ws\robotraconteur_java\RobotRaconteur.jar reynard_robotraconteur_client.java experimental\reynard_the_robot\*.java
```

## Run Example

Open a command prompt and start the Reynard the Robot server:

```cmd
python -m reynard_the_robot
```

Open another command prompt and run the following command:

```
java -cp C:\ws\robotraconteur_java\RobotRaconteur.jar;. -Djava.library.path=C:\ws\robotraconteur_java reynard_robotraconteur_client
```
