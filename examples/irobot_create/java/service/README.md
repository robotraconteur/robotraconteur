```
RobotRaconteurGen --thunksource --lang=java ..\..\robdef\experimental.create3.robdef
```


```
javac -cp C:\ws\robotraconteur_java\RobotRaconteur.jar;C:\ws\robotraconteur_java\jSerialComm.jar *.java experimental\create3\*.java
```


```
java -cp C:\ws\robotraconteur_java\RobotRaconteur.jar;C:\ws\robotraconteur_java\jSerialComm.jar;. -Djava.library.path=C:\ws\robotraconteur_java iRobotCreateService
```
