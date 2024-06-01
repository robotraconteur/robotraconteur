
```
RobotRaconteurGen --thunksource --lang=java ..\..\robdef\experimental.simplewebcam3.robdef
```


```
javac -cp C:\ws\robotraconteur_java\RobotRaconteur.jar;C:\ws\robotraconteur_java\opencv.jar *.java experimental\simplewebcam3\*.java
```


```
java -cp C:\ws\robotraconteur_java\RobotRaconteur.jar;C:\ws\robotraconteur_java\opencv.jar;. -Djava.library.path=C:\ws\robotraconteur_java SimpleWebcamService
```
