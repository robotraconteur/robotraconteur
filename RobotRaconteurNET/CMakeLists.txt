if(NOT USE_PREGENERATED_SOURCE)

    include(FindSWIG)
    find_package(SWIG 4.0.2 REQUIRED)
    include(${CMAKE_SOURCE_DIR}/cmake/RRUseSwig.cmake)

    include(NETSwigVars.cmake)
    include_directories(${CMAKE_SOURCE_DIR}/RobotRaconteurCore/include)
    rr_swig_add_module(RobotRaconteurNETNative csharp ${CMAKE_SOURCE_DIR}/SWIG/RobotRaconteurWrapped.cpp
                       ${CMAKE_SOURCE_DIR}/SWIG/RobotRaconteurWrapped.h RobotRaconteurNET.i)

    swig_link_libraries(RobotRaconteurNETNative RobotRaconteurCore)
else()
    set(SWIG_MODULE_RobotRaconteurNETNative_REAL_NAME RobotRaconteurNETNative)

    if("${SIZEOF_LONG_INT}" EQUAL 8 AND CMAKE_COMPILER_IS_GNUCXX)
        set(CMAKE_SWIG_OUTDIR ${PREGENERATED_SOURCE_DIR}/NET/swigwordsize64/)
    else()
        set(CMAKE_SWIG_OUTDIR ${PREGENERATED_SOURCE_DIR}/NET/swigwordsize32/)
    endif()
    include_directories(${CMAKE_SOURCE_DIR}/SWIG ${CMAKE_SWIG_OUTDIR})
    add_library(
        RobotRaconteurNETNative MODULE
        ${CMAKE_SOURCE_DIR}/SWIG/RobotRaconteurWrapped.cpp ${CMAKE_SOURCE_DIR}/SWIG/RobotRaconteurWrapped.h
        ${CMAKE_SWIG_OUTDIR}/RobotRaconteurNETCSHARP_wrap.cxx)
    target_link_libraries(RobotRaconteurNETNative RobotRaconteurCore)
endif()
# if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/RobotRaconteurNET_wrap.cxx")

if(MSVC)
    target_compile_options(${SWIG_MODULE_RobotRaconteurNETNative_REAL_NAME} PRIVATE "/bigobj")
endif()

if(APPLE)
    set_target_properties(${SWIG_MODULE_RobotRaconteurNETNative_REAL_NAME} PROPERTIES SUFFIX ".dylib")
endif()

target_compile_definitions(${SWIG_MODULE_RobotRaconteurNETNative_REAL_NAME} PRIVATE SWIG_TYPE_TABLE=robotraconteur_net)

rrsettargetdirs(${SWIG_MODULE_RobotRaconteurNETNative_REAL_NAME} "NET/Native" "NET/Native")

# get_target_property(outdir RobotRaconteurNETNative LIBRARY_OUTPUT_DIRECTORY) get_target_property(outdir
# RobotRaconteurNETNative RUNTIME_OUTPUT_DIRECTORY_DEBUG)

set(RR_NET_PLATFORM_SUFFIX FALSE CACHE BOOL "")
if(RR_NET_PLATFORM_SUFFIX)
    set_target_properties(${SWIG_MODULE_RobotRaconteurNETNative_REAL_NAME} PROPERTIES OUTPUT_NAME
                                                                                      "RobotRaconteurNETNative")
    if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
        if(CMAKE_SIZEOF_VOID_P EQUAL 8)
            set_target_properties(${SWIG_MODULE_RobotRaconteurNETNative_REAL_NAME}
                                  PROPERTIES OUTPUT_NAME "RobotRaconteurNETNative-linux64")
        else()
            set_target_properties(${SWIG_MODULE_RobotRaconteurNETNative_REAL_NAME}
                                  PROPERTIES OUTPUT_NAME "RobotRaconteurNETNative-linux32")
        endif()
    endif()

    if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

        set_target_properties(${SWIG_MODULE_RobotRaconteurNETNative_REAL_NAME}
                              PROPERTIES OUTPUT_NAME "RobotRaconteurNETNative-darwin")
    endif()
else()
    set_target_properties(${SWIG_MODULE_RobotRaconteurNETNative_REAL_NAME} PROPERTIES PREFIX
                                                                                      "${CMAKE_SHARED_MODULE_PREFIX}")
endif()

set(RR_NET_INSTALL_NATIVE_LIB OFF CACHE BOOL "Install .NET Native lib")
mark_as_advanced(RR_NET_INSTALL_NATIVE_LIB)

if(RR_NET_INSTALL_NATIVE_LIB)
    install(TARGETS ${SWIG_MODULE_RobotRaconteurNETNative_REAL_NAME} DESTINATION ${CMAKE_INSTALL_LIBDIR})
endif()

set(RR_NET_BUILD_NATIVE_ONLY OFF CACHE BOOL "Install .NET Native lib")
mark_as_advanced(RR_NET_BUILD_NATIVE_ONLY)

if(NOT RR_NET_BUILD_NATIVE_ONLY)

    include("${CMAKE_CURRENT_SOURCE_DIR}/NETExceptions.cmake")

    if(WIN32)
        add_custom_target(
            RobotRaconteurNET ALL
            msbuild $<$<CONFIG:debug>:/p:Configuration=Debug>$<$<CONFIG:release>:/p:Configuration=Release> /t:rebuild
            RobotRaconteurNET.csproj WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
    else()
        find_program(XBUILD_EXECUTABLE NAMES xbuild msbuild)
        add_custom_target(
            RobotRaconteurNET ALL
            "${XBUILD_EXECUTABLE}"
            $<$<CONFIG:debug>:/p:Configuration=Debug>$<$<CONFIG:release>:/p:Configuration=Release>
            RobotRaconteurNET.csproj
            WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
    endif()
    add_dependencies(RobotRaconteurNET RobotRaconteurNETNative)

    include(${CMAKE_SOURCE_DIR}/RobotRaconteurGen/cmake/RobotRaconteurGenerateThunk.cmake)

    if(NOT RobotRaconteur_GEN)
        set(RobotRaconteur_GEN RobotRaconteurGen)
    endif()

    file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/test")

    robotraconteur_generate_thunk(
        RR_THUNK_SRCS
        com.robotraconteur.testing.TestService1.robdef
        com.robotraconteur.testing.TestService2.robdef
        com.robotraconteur.testing.TestService3.robdef
        com.robotraconteur.testing.TestService4.robdef
        com.robotraconteur.testing.TestService5.robdef
        LANG
        csharp
        INCLUDE_DIRS
        "${CMAKE_SOURCE_DIR}/test/robdef"
        OUTDIR
        "${CMAKE_CURRENT_BINARY_DIR}/test")

    add_custom_target(RobotRaconteurNETTest_gen DEPENDS ${RR_THUNK_SRCS})
    add_dependencies(RobotRaconteurNETNative RobotRaconteurNETTest_gen)
    if(WIN32)
        add_custom_target(
            RobotRaconteurNETTest ALL
            msbuild $<$<CONFIG:debug>:/p:Configuration=Debug>$<$<CONFIG:release>:/p:Configuration=Release> /t:rebuild
            RobotRaconteurNETTest.csproj WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
    else()
        find_program(XBUILD_EXECUTABLE NAMES xbuild msbuild)
        add_custom_target(
            RobotRaconteurNETTest ALL
            "${XBUILD_EXECUTABLE}"
            $<$<CONFIG:debug>:/p:Configuration=Debug>$<$<CONFIG:release>:/p:Configuration=Release>
            RobotRaconteurNETTest.csproj
            WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
    endif()
    add_dependencies(RobotRaconteurNETTest RobotRaconteurNET)
    # add_custom_target(RobotRaconteurNETTestVB ALL msbuild -p:Configuration=$(Configuration) /t:rebuild
    # RobotRaconteurNETTestVB.vbproj WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
    # add_dependencies(RobotRaconteurNETTestVB RobotRaconteurNET)

    if(WIN32)
        if(CMAKE_SIZEOF_VOID_P EQUAL 8)
            set(RR_PREFER_32BIT "<Prefer32Bit>false</Prefer32Bit>")
        else()
            set(RR_PREFER_32BIT "<Prefer32Bit>true</Prefer32Bit>")
        endif()
    endif()

    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/RobotRaconteurNET/RobotRaconteurNET.csproj.in"
                   "${CMAKE_CURRENT_BINARY_DIR}/RobotRaconteurNET.csproj" @ONLY)
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/RobotRaconteurNET/RobotRaconteurNETTest/RobotRaconteurNETTest.csproj.in"
                   "${CMAKE_CURRENT_BINARY_DIR}/RobotRaconteurNETTest.csproj" @ONLY)

    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/RobotRaconteurNET/netstandard/RobotRaconteurNET.csproj.in"
                   "${CMAKE_CURRENT_BINARY_DIR}/netstandard/Lib/RobotRaconteurNET.csproj" @ONLY)
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/RobotRaconteurNET/netstandard/RobotRaconteurNETTest.csproj.in"
                   "${CMAKE_CURRENT_BINARY_DIR}/netstandard/Test/RobotRaconteurNETTest.csproj" @ONLY)

    if(WIN32 AND (MSVC_VERSION GREATER 1900))
        add_custom_target(
            RobotRaconteurNET_standard ALL
            msbuild /restore /verbosity:quiet
            $<$<CONFIG:debug>:/p:Configuration=Debug>$<$<CONFIG:release>:/p:Configuration=Release> /t:rebuild
            RobotRaconteurNET.csproj
            WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/netstandard/Lib")
        add_custom_target(
            RobotRaconteurNETTest_standard ALL
            msbuild /verbosity:quiet /restore
            $<$<CONFIG:debug>:/p:Configuration=Debug>$<$<CONFIG:release>:/p:Configuration=Release> /t:rebuild
            RobotRaconteurNETTest.csproj
            WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/netstandard/Test")
        add_dependencies(RobotRaconteurNET_standard ${SWIG_MODULE_RobotRaconteurNETNative_REAL_NAME})
        add_dependencies(RobotRaconteurNETTest_standard RobotRaconteurNET_standard)
        add_custom_command(
            TARGET RobotRaconteurNET_standard
            POST_BUILD
            COMMAND
            COMMAND ${CMAKE_COMMAND} -E make_directory
                    ${CMAKE_BINARY_DIR}/$<$<CONFIG:debug>:out_debug>$<$<CONFIG:release>:out>/NET/netstandard
            COMMAND
                ${CMAKE_COMMAND} -E copy
                ${CMAKE_CURRENT_BINARY_DIR}/netstandard/Lib/bin/$<$<CONFIG:debug>:Debug>$<$<CONFIG:release>:Release>/RobotRaconteurNET.dll
                ${CMAKE_BINARY_DIR}/$<$<CONFIG:debug>:out_debug>$<$<CONFIG:release>:out>/NET/netstandard/.)
    endif()

    # configure_file("${CMAKE_CURRENT_SOURCE_DIR}/RobotRaconteurNET/RobotRaconteurNETTestVB/RobotRaconteurNETTestVB.vbproj.in"
    # "${CMAKE_CURRENT_BINARY_DIR}/RobotRaconteurNETTestVB.vbproj" @ONLY)

    string(TIMESTAMP CurrentYear "%Y")

    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/RobotRaconteurNET/Properties/AssemblyInfo.cs.in"
                   "${CMAKE_CURRENT_BINARY_DIR}/AssemblyInfo.cs" @ONLY)

    if(WIN32)
        set(RR_NET_TEST_CMD "\"${CMAKE_COMMAND}\"")
        set(RR_NET_TEST_CMD_ARGS
            "-E env PATH=\"%PATH%\\\;${CMAKE_BINARY_DIR}/@OUT_DIR_NAME@/NET/Native\" ${CMAKE_BINARY_DIR}/@OUT_DIR_NAME@/NET/RobotRaconteurNETTest"
        )
    else()
        find_program(MONO_EXECUTABLE mono)
        set(RR_NET_TEST_CMD "\"${CMAKE_COMMAND}\"")
        set(RR_NET_TEST_CMD_ARGS
            "-E env LD_LIBRARY_PATH=$LD_LIBRARY_PATH:${CMAKE_BINARY_DIR}/@OUT_DIR_NAME@/NET/Native\ ${MONO_EXECUTABLE} ${CMAKE_BINARY_DIR}/@OUT_DIR_NAME@/NET/RobotRaconteurNETTest.exe"
        )
    endif()
    rrconfiguretest(test_prog_net "${RR_NET_TEST_CMD}" "test_prog" NOCTEST ARGS "${RR_NET_TEST_CMD_ARGS}")
    rrconfiguretest(test_net_loopback "${RR_NET_TEST_CMD}" "test_prog" ARGS "${RR_NET_TEST_CMD_ARGS} loopback")
    rrconfiguretest(test_net_loopback2 "${RR_NET_TEST_CMD}" "test_prog" ARGS "${RR_NET_TEST_CMD_ARGS} loopback2")
    rrconfiguretest(test_net_loopback3 "${RR_NET_TEST_CMD}" "test_prog" ARGS "${RR_NET_TEST_CMD_ARGS} loopback3")

endif()
