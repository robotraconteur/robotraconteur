# Reynard Pyodide Robot Raconteur Client Example

This example demonstrates how to use Robot Raconteur in a web browser using [Pyodide](https://pyodide.org/)
Pyodide allows for Python scripts to be executed in a web browser. It is implemented using Web Assembly (WASM).
Robot Raconteur is included as a standard package in Pyodide so there is no installation required. The
Robot Raconteur package uses [HTTP Web Sockets](https://en.wikipedia.org/wiki/WebSocket) to communicate with services.

The example page connects to the Reynard the Robot service, displays the robot position, allows
the user to teleport the robot, allows the user to send a message, displays incoming messages,
and displays any errors that might occur.

Note that all Robot Raconteur calls use the "async" versions. The web browser is a single threaded
environment so non-blocking "async" calls must be used for all networking operations.

## Running the example

The Python pyodide service must be installed and running normally. Because Pyodide uses browser web sockets,
the "origin" must be added to allow for access. The "origin" for web sockets is the host URL that initiated
the connection, excluding any file path or query. For this example, the origin is `http://localhost:8000`,
since the page is served by `http`, the hostname is `localhost`, and the port is `8000`. Use the
`--robotraconteur-tcp-ws-add-origin=` command line argument when starting the service:

```
python -m reynard_the_robot --robotraconteur-tcp-ws-add-origin=http://localhost:8000
```

Now open another terminal, and navigate to the `examples/reynard_the_robot/pydoide` directory. Run:

```
python -m http.server
```

This command will start a simple HTTP server listening on port 8000.

Open a browser to http://localhost:8000/reynard_the_robot_client_pyodide.html to use the
Pyodide Reynard the Robot client user interface.
