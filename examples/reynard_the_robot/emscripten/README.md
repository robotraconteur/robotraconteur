# Reynard Emscripten Robot Raconteur Client Example

This example demonstrates how to use Robot Raconteur in a web browser using C++ compiled to
WebAssembly (WASM) with [Emscripten](https://emscripten.org/). It is the C++ equivalent of the
[Pyodide example](../pyodide). Inside the browser, Robot Raconteur uses
[HTTP Web Sockets](https://en.wikipedia.org/wiki/WebSocket) to communicate with services through the
`BrowserWebSocketTransport`. Only client connections are supported.

The example page connects to the Reynard the Robot service, displays the robot position, allows
the user to teleport the robot, allows the user to send a message, displays incoming messages,
and displays any errors that might occur.

Note that all Robot Raconteur calls use the "async" versions. The web browser is a single threaded
environment so non-blocking "async" calls must be used for all networking operations. `main()` starts
the connection and returns. The Emscripten runtime stays alive and the async handlers run from the
browser event loop.

## Building the example

Robot Raconteur is not distributed as a prebuilt Emscripten library, so the core library and its
dependencies must be compiled with Emscripten first. Install the
[Emscripten SDK](https://emscripten.org/docs/getting_started/downloads.html) and activate it
with `source ./emsdk_env.sh`. These steps were tested with Emscripten 4.0.10, Boost 1.86.0, and OpenSSL 3.0.15.

All C++ code, including Boost, must be compiled with `-fwasm-exceptions` since Robot Raconteur uses
C++ exceptions.

### Boost

Download and extract the Boost source, then build the required libraries with the Emscripten toolset:

```
./bootstrap.sh
./b2 toolset=emscripten link=static address-model=32 variant=release threading=single \
  cxxflags="-fwasm-exceptions" cflags="-fwasm-exceptions" \
  --with-date_time --with-filesystem --with-regex --with-chrono --with-random --with-program_options --with-system \
  --prefix=$HOME/wasm/boost install
```

### OpenSSL

Download and extract the OpenSSL source, then build the static libraries:

```
emconfigure ./Configure linux-generic32 no-asm no-threads no-shared no-dso no-engine no-tests no-ui-console \
  -DHAVE_FORK=0 -DOPENSSL_NO_SECURE_MEMORY -DNO_SYSLOG --prefix=$HOME/wasm/openssl --libdir=lib
sed -i 's!^CROSS_COMPILE=.*!CROSS_COMPILE=!g' Makefile
emmake make build_generated libcrypto.a libssl.a
emmake make install_dev
```

### RobotRaconteurGen

`RobotRaconteurGen` generates the C++ thunk source for the Reynard service definition. It must run
on the build machine, so it is built natively, not with Emscripten. Either install Robot Raconteur
with a package manager or build only the generator from the root of the Robot Raconteur source:

```
cmake -S . -B build-native -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=OFF
cmake --build build-native --target RobotRaconteurGen
```

### Robot Raconteur core

From the root of the Robot Raconteur source, build and install the core library with Emscripten:

```
emcmake cmake -S . -B build-wasm -DCMAKE_BUILD_TYPE=Release -DBUILD_GEN=OFF -DBUILD_TESTING=OFF \
  -DBoost_ROOT=$HOME/wasm/boost -DOPENSSL_ROOT_DIR=$HOME/wasm/openssl -DOPENSSL_USE_STATIC_LIBS=ON \
  -DCMAKE_FIND_ROOT_PATH="$HOME/wasm/boost;$HOME/wasm/openssl" \
  -DCMAKE_CXX_FLAGS="-fwasm-exceptions -DBOOST_SP_DISABLE_THREADS=1 -DBOOST_BIND_GLOBAL_PLACEHOLDERS=1" \
  -DCMAKE_INSTALL_PREFIX=$HOME/wasm/robotraconteur
cmake --build build-wasm
cmake --install build-wasm
```

### Example

From the `examples/reynard_the_robot/emscripten` directory, build the example. Set
`RobotRaconteurGen_EXECUTABLE` to the native generator built above, or to the installed `RobotRaconteurGen`.

```
emcmake cmake -S . -B build -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_FIND_ROOT_PATH="$HOME/wasm/robotraconteur;$HOME/wasm/boost;$HOME/wasm/openssl" \
  -DBoost_ROOT=$HOME/wasm/boost -DOPENSSL_ROOT_DIR=$HOME/wasm/openssl -DOPENSSL_USE_STATIC_LIBS=ON \
  -DRobotRaconteur_DIR=$HOME/wasm/robotraconteur/lib/cmake/RobotRaconteur \
  -DRobotRaconteurGen_EXECUTABLE=/path/to/robotraconteur/build-native/out/bin/RobotRaconteurGen
cmake --build build
```

The `build` directory will contain `reynard_the_robot_client_emscripten.html`,
`reynard_the_robot_client_emscripten.js`, and `reynard_the_robot_client_emscripten.wasm`.

## Running the example

The Python Reynard the Robot service must be installed and running normally. Because the browser uses web sockets,
the "origin" must be added to allow for access. The "origin" for web sockets is the host URL that initiated
the connection, excluding any file path or query. For this example, the origin is `http://localhost:8000`,
since the page is served by `http`, the hostname is `localhost`, and the port is `8000`. Use the
`--robotraconteur-tcp-ws-add-origins=` command line argument when starting the service:

```
python -m reynard_the_robot --robotraconteur-tcp-ws-add-origins=http://localhost:8000
```

Now open another terminal, and navigate to the `examples/reynard_the_robot/emscripten/build` directory. Run:

```
python -m http.server
```

This command will start a simple HTTP server listening on port 8000. The page must be served
over HTTP. Opening the HTML file directly from disk will not load the WASM file.

Open a browser to http://localhost:8000/reynard_the_robot_client_emscripten.html to use the
Emscripten Reynard the Robot client user interface.
