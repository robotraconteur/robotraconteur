if(NOT RR_NET_BUILD_NATIVE_ONLY)

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
    # add_dependencies(RobotRaconteurNETNative RobotRaconteurNETTest_gen)

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
    add_dependencies(RobotRaconteurNETTest RobotRaconteurNET RobotRaconteurNETTest_gen)

    if(WIN32)
        if(CMAKE_SIZEOF_VOID_P EQUAL 8)
            set(RR_PREFER_32BIT "<Prefer32Bit>false</Prefer32Bit>")
        else()
            set(RR_PREFER_32BIT "<Prefer32Bit>true</Prefer32Bit>")
        endif()
    endif()

    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/RobotRaconteurNETTest/RobotRaconteurNETTest.csproj.in"
                   "${CMAKE_CURRENT_BINARY_DIR}/RobotRaconteurNETTest.csproj" @ONLY)

    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/netstandard/RobotRaconteurNETTest.csproj.in"
                   "${CMAKE_CURRENT_BINARY_DIR}/netstandard/Test/RobotRaconteurNETTest.csproj" @ONLY)

    if(WIN32)
        set(RR_NET_TEST_CMD "\"${CMAKE_COMMAND}\"")
        set(RR_NET_TEST_CMD_ARGS
            "-E env PATH=\"%PATH%\\\;${CMAKE_BINARY_DIR}/@OUT_DIR_NAME@/NET/Native\" ${CMAKE_BINARY_DIR}/test/@OUT_DIR_NAME@/NET/RobotRaconteurNETTest"
        )
    else()
        find_program(MONO_EXECUTABLE mono)
        set(RR_NET_TEST_CMD "\"${CMAKE_COMMAND}\"")
        set(RR_NET_TEST_CMD_ARGS
            "-E env LD_LIBRARY_PATH=$LD_LIBRARY_PATH:${CMAKE_BINARY_DIR}/@OUT_DIR_NAME@/NET/Native\ ${MONO_EXECUTABLE} ${CMAKE_BINARY_DIR}/test/@OUT_DIR_NAME@/NET/RobotRaconteurNETTest.exe"
        )
    endif()
    rrconfiguretest(test_prog_net "${RR_NET_TEST_CMD}" "test_prog" NOCTEST ARGS "${RR_NET_TEST_CMD_ARGS}")
    rrconfiguretest(test_net_loopback "${RR_NET_TEST_CMD}" "test_prog" ARGS "${RR_NET_TEST_CMD_ARGS} loopback")
    rrconfiguretest(test_net_loopback2 "${RR_NET_TEST_CMD}" "test_prog" ARGS "${RR_NET_TEST_CMD_ARGS} loopback2")
    rrconfiguretest(test_net_loopback3 "${RR_NET_TEST_CMD}" "test_prog" ARGS "${RR_NET_TEST_CMD_ARGS} loopback3")

    if(WIN32 AND (MSVC_VERSION GREATER 1900))
        add_custom_target(
            RobotRaconteurNETTest_standard ALL
            msbuild /verbosity:quiet /restore
            $<$<CONFIG:debug>:/p:Configuration=Debug>$<$<CONFIG:release>:/p:Configuration=Release> /t:rebuild
            RobotRaconteurNETTest.csproj
            WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/netstandard/Test")
        add_dependencies(RobotRaconteurNETTest_standard RobotRaconteurNET_standard RobotRaconteurNETTest_gen)

        set(RR_NET_STD_TEST_CMD "\"${CMAKE_COMMAND}\"")
        set(RR_NET_STD_TEST_CMD_ARGS
            "-E env PATH=\"%PATH%\\\;${CMAKE_BINARY_DIR}/@OUT_DIR_NAME@/NET/Native\" dotnet ${CMAKE_BINARY_DIR}/test/@OUT_DIR_NAME@/NET/RobotRaconteurNETTest"
        )
    endif()

endif()
