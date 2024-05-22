```cmd
RobotRaconteurGen --thunksource --lang=java ..\..\..\robdef\experimental.reynard_the_robot.robdef
```

```cmd
javac -cp C:\ws\robotraconteur_java\RobotRaconteur.jar;C:\ws\robotraconteur_java\gson.jar;. reynard_robotraconteur_service_http_rest.java experimental\reynard_the_robot\*.java
```

```cmd
java -cp C:\ws\robotraconteur_java\RobotRaconteur.jar;C:\ws\robotraconteur_java\gson.jar;. -Djava.library.path=C:\ws\robotraconteur_java reynard_robotraconteur_service_http_rest
```
