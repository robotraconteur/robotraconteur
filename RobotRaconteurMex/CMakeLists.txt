set(BUILD_MATLAB_MEX OFF CACHE BOOL OFF)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj")
endif()

if(APPLE)
    # string(REPLACE "i386" "" CMAKE_OSX_ARCHITECTURES ${CMAKE_OSX_ARCHITECTURES})
    set(CMAKE_SHARED_LINKER_FLAGS ${CMAKE_SHARED_LINKER_FLAGS}
                                  -Wl,-exported_symbols_list,${CMAKE_CURRENT_SOURCE_DIR}/osx_mex_exports.txt)
endif()

if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_SHARED_LINKER_FLAGS ${CMAKE_SHARED_LINKER_FLAGS} -Wl,--exclude-libs,ALL)
endif()

# include(FindMatlab)
find_package(Matlab COMPONENTS MX_LIBRARY REQUIRED)

add_definitions(-DMATLAB_MEX_FILE -DMATLAB_DEFAULT_RELEASE=R2018a)

# INCLUDE(BuildMex.cmake)

include_directories(${Matlab_INCLUDE_DIRS})

matlab_add_mex(
    NAME RobotRaconteurMex
    SRC RobotRaconteurMex.cpp RobotRaconteurMex.h
    OUTPUT_NAME RobotRaconteurMex
    LINK_TO RobotRaconteurCore)

file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/out/Matlab")
file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/out_debug/Matlab")
file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/out_reldebug/Matlab")

rrsettargetdirs(RobotRaconteurMex "Matlab" "Matlab")

add_custom_command(TARGET RobotRaconteurMex PRE_BUILD COMMAND "${CMAKE_COMMAND}" -E make_directory
                                                              $<TARGET_FILE_DIR:RobotRaconteurMex>)

set(RR_MEX_M_DIRS
    @RobotRaconteur
    @RobotRaconteurObject
    @RobotRaconteurEventListener
    @RobotRaconteurMemoryClient
    @RobotRaconteurRate
    @RobotRaconteurEventData
    @RobotRaconteurGenerator
    @RobotRaconteurPipeEndpoint
    @RobotRaconteurWireConnection
    @RobotRaconteurWireSubscription
    @RobotRaconteurPipeSubscription
    @RobotRaconteurSubObjectSubscription
    @RobotRaconteurServiceSubscription
    @RobotRaconteurServiceInfo2Subscription
    @RobotRaconteurVarValue)

set(RR_MEX_M_FILES RobotRaconteurObjectServerConnectEvent.m RobotRaconteurObjectServerDispatchEvent.m
                   RobotRaconteurAsyncOps.m RobotRaconteurFramework.m RobotRaconteurValueTypes.m)

foreach(_M_DIR ${RR_MEX_M_DIRS})
    add_custom_command(
        TARGET RobotRaconteurMex POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/${_M_DIR}
                $<TARGET_FILE_DIR:RobotRaconteurMex>/${_M_DIR})
endforeach()

foreach(_M_FILE ${RR_MEX_M_FILES})
    add_custom_command(
        TARGET RobotRaconteurMex POST_BUILD COMMAND "${CMAKE_COMMAND}" -E copy ${CMAKE_CURRENT_SOURCE_DIR}/${_M_FILE}
                                                    $<TARGET_FILE_DIR:RobotRaconteurMex>/${_M_FILE})
endforeach()
