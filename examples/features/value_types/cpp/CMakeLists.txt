cmake_minimum_required(VERSION 3.3)
project(value_types)

set(CMAKE_CXX_STANDARD 11)

if(NOT TARGET RobotRaconteurCore)
    find_package(RobotRaconteur REQUIRED)
endif()

robotraconteur_generate_thunk(
    RR_THUNK_SRCS
    RR_THUNK_HDRS
    experimental.value_types.robdef
    MASTER_HEADER
    INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/../robdef
    AUTO_IMPORT)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_executable(numeric_value_types numeric_value_types.cpp ${RR_THUNK_SRCS} ${RR_THUNK_HDRS})
target_link_libraries(numeric_value_types RobotRaconteurCore)

add_executable(string_value_type string_value_type.cpp ${RR_THUNK_SRCS} ${RR_THUNK_HDRS})
target_link_libraries(string_value_type RobotRaconteurCore)

add_executable(struct_value_types struct_value_types.cpp ${RR_THUNK_SRCS} ${RR_THUNK_HDRS})
target_link_libraries(struct_value_types RobotRaconteurCore)

add_executable(namedarray_value_types namedarray_value_types.cpp ${RR_THUNK_SRCS} ${RR_THUNK_HDRS})
target_link_libraries(namedarray_value_types RobotRaconteurCore)

add_executable(pod_value_types pod_value_types.cpp ${RR_THUNK_SRCS} ${RR_THUNK_HDRS})
target_link_libraries(pod_value_types RobotRaconteurCore)

add_executable(container_value_types container_value_types.cpp ${RR_THUNK_SRCS} ${RR_THUNK_HDRS})
target_link_libraries(container_value_types RobotRaconteurCore)

add_executable(varvalue_value_types varvalue_value_types.cpp ${RR_THUNK_SRCS} ${RR_THUNK_HDRS})
target_link_libraries(varvalue_value_types RobotRaconteurCore)

add_executable(null_value_types varvalue_value_types.cpp ${RR_THUNK_SRCS} ${RR_THUNK_HDRS})
target_link_libraries(null_value_types RobotRaconteurCore)
