# Reynard Java Robot Raconteur Client Example

This example demonstrates controlling Reynard using Java as a Robot Raconteur client.

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

Run the following command:

```
java -cp C:\ws\robotraconteur_java\RobotRaconteur.jar;. -Djava.library.path=C:\ws\robotraconteur_java reynard_robotraconteur_client
```
