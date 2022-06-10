set(RobotRaconteurGen_src
    CPPServiceLangGen.cpp CSharpServiceLangGen.cpp
    # VBNETServiceLangGen.cpp
    JavaServiceLangGen.cpp RobotRaconteurGen.cpp StringTableGen.cpp)

set(RobotRaconteurGen_header
    CPPServiceLangGen.h CSharpServiceLangGen.h
    # VBNETServiceLangGen.h
    JavaServiceLangGen.h StringTableGen.h)

add_executable(RobotRaconteurGen ${RobotRaconteurGen_src} ${RobotRaconteurGen_header})

if(${CMAKE_VERSION} VERSION_GREATER "3.16.0")
    target_precompile_headers(RobotRaconteurGen PRIVATE ${RobotRaconteurGen_header})
endif()

if(ROBOTRACONTEUR_ROS)
    find_package(roslib REQUIRED)
    target_link_libraries(RobotRaconteurGen PRIVATE RobotRaconteurCore ${roslib_LIBRARIES})
    target_include_directories(RobotRaconteurGen PRIVATE ${roslib_INCLUDE_DIRS})
    target_compile_definitions(RobotRaconteurGen PRIVATE ROBOTRACONTEUR_ROS)
else()
    target_link_libraries(RobotRaconteurGen RobotRaconteurCore)
endif()

rrsettargetdirs(RobotRaconteurGen "bin" "lib")

rrconfiguretest(gen_prog "${CMAKE_BINARY_DIR}/@OUT_DIR_NAME@/bin/RobotRaconteurGen" "test_prog" NOCTEST)
# RRConfigureTest(test_cpp_loopback "${CMAKE_BINARY_DIR}/@OUT_DIR_NAME@/bin/RobotRaconteurTest" "test_prog" ARGS
# "loopback")

set(RR_TEST_ROBDEFS com.robotraconteur.testing.TestService1.robdef com.robotraconteur.testing.TestService2.robdef
                    com.robotraconteur.testing.TestService3.robdef)

function(RR_TEST_GEN lang)
    string(REPLACE ";" " " RR_TEST_ROBDEFS_1 "${RR_TEST_ROBDEFS}")
    set(RR_TEST_GEN_CMD "${CMAKE_BINARY_DIR}/@OUT_DIR_NAME@/bin/RobotRaconteurGen")
    set(RR_TEST_GEN_CMD_ARGS
        "--thunksource --lang=${lang} -I ${CMAKE_SOURCE_DIR}/test/robdef --output-dir ${CMAKE_CURRENT_BINARY_DIR}/gen/@OUT_DIR_NAME@/${lang} ${RR_TEST_ROBDEFS_1}"
    )

    file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/gen/out/${lang}")
    file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/gen/out_debug/${lang}")
    file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/gen/out_reldebug/${lang}")

    set(GEN_FILE_LIST "")

    file(GLOB_RECURSE ROBOTRACONTEUR_GEN_TEST_FILES RELATIVE "${CMAKE_SOURCE_DIR}/test/gen/${lang}"
         "${CMAKE_SOURCE_DIR}/test/gen/${lang}/*")
    foreach(F ${ROBOTRACONTEUR_GEN_TEST_FILES})
        if(WIN32)
            configure_file("${CMAKE_SOURCE_DIR}/test/gen/${lang}/${F}"
                           "${CMAKE_CURRENT_BINARY_DIR}/gen/test/${lang}/${F}" NEWLINE_STYLE CRLF)
        else()
            configure_file("${CMAKE_SOURCE_DIR}/test/gen/${lang}/${F}"
                           "${CMAKE_CURRENT_BINARY_DIR}/gen/test/${lang}/${F}" NEWLINE_STYLE LF)
        endif()
        set(GEN_FILE_LIST
            "${GEN_FILE_LIST}\"${CMAKE_CURRENT_BINARY_DIR}/gen/test/${lang}/${F};${CMAKE_CURRENT_BINARY_DIR}/gen/@OUT_DIR_NAME@/${lang}/${F}\","
        )
    endforeach()

    rrconfiguretest(
        "test_gen_${lang}"
        "${RR_TEST_GEN_CMD}"
        "test_gen"
        ARGS
        "${RR_TEST_GEN_CMD_ARGS}"
        FILELIST
        GEN_FILE_LIST)
endfunction()

rr_test_gen(cpp)
rr_test_gen(csharp)
rr_test_gen(java)

export(TARGETS RobotRaconteurGen FILE ${CMAKE_BINARY_DIR}/RobotRaconteurGenTargets.cmake)
install(TARGETS RobotRaconteurGen EXPORT RobotRaconteurGenTargets DESTINATION ${CMAKE_INSTALL_BINDIR})
install(EXPORT RobotRaconteurGenTargets DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${CMAKE_PROJECT_NAME})
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/RobotRaconteurGenerateThunk.cmake
               ${CMAKE_BINARY_DIR}/RobotRaconteurGenerateThunk.cmake @ONLY)
install(FILES ${CMAKE_BINARY_DIR}/RobotRaconteurGenerateThunk.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${CMAKE_PROJECT_NAME})
