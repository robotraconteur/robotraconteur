cmake_minimum_required(VERSION 3.12...3.99)
project(reynard_the_robot_client_emscripten)

if(NOT EMSCRIPTEN)
    message(FATAL_ERROR "This example must be configured with emcmake")
endif()

set(CMAKE_CXX_STANDARD 11)

# Robot Raconteur core compiled with Emscripten, installed with BUILD_GEN=OFF. Set RobotRaconteurGen_EXECUTABLE to a
# native RobotRaconteurGen, since the generator must run on the build machine.
find_package(RobotRaconteur REQUIRED)

robotraconteur_generate_thunk(
    RR_THUNK_SRCS
    RR_THUNK_HDRS
    experimental.reynard_the_robot.robdef
    MASTER_HEADER
    INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/../robdef/
    AUTO_IMPORT)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_executable(reynard_the_robot_client_emscripten reynard_the_robot_client_emscripten.cpp ${RR_THUNK_SRCS}
                                                   ${RR_THUNK_HDRS})

# Robot Raconteur uses C++ exceptions. WebAssembly exceptions must match the flags used to build the core and Boost.
target_compile_options(reynard_the_robot_client_emscripten PRIVATE -fwasm-exceptions)
target_compile_definitions(reynard_the_robot_client_emscripten PRIVATE BOOST_SP_DISABLE_THREADS=1
                                                                       BOOST_BIND_GLOBAL_PLACEHOLDERS=1)

target_link_libraries(reynard_the_robot_client_emscripten RobotRaconteurCore)

# lwebsocket.js provides the browser WebSocket API used by BrowserWebSocketTransport. lembind provides emscripten::val
# used to access the page. ALLOW_MEMORY_GROWTH lets the heap grow for large messages.
target_link_options(reynard_the_robot_client_emscripten PRIVATE -fwasm-exceptions -lwebsocket.js -lembind
                    -sALLOW_MEMORY_GROWTH=1)

# Copy the HTML page next to the generated reynard_the_robot_client_emscripten.js and .wasm files
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/reynard_the_robot_client_emscripten.html
               ${CMAKE_CURRENT_BINARY_DIR}/reynard_the_robot_client_emscripten.html COPYONLY)
