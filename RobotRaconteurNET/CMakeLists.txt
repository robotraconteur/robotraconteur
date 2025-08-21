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

option(ROBOTRACONTEUR_SKIP_RPATH "Skip RPATH for shared libraries" OFF)
if(ROBOTRACONTEUR_SKIP_RPATH)
    set_target_properties(${SWIG_MODULE_RobotRaconteurNETNative_REAL_NAME} PROPERTIES SKIP_BUILD_RPATH TRUE)
endif()

set(RR_NET_INSTALL_NATIVE_LIB OFF CACHE BOOL "Install .NET Native lib")
mark_as_advanced(RR_NET_INSTALL_NATIVE_LIB)

if(RR_NET_INSTALL_NATIVE_LIB)
    install(TARGETS ${SWIG_MODULE_RobotRaconteurNETNative_REAL_NAME} DESTINATION ${CMAKE_INSTALL_LIBDIR})
endif()

set(RR_NET_BUILD_NATIVE_ONLY OFF CACHE BOOL "Install .NET Native lib")
mark_as_advanced(RR_NET_BUILD_NATIVE_ONLY)

include("${CMAKE_CURRENT_SOURCE_DIR}/NETExceptions.cmake")
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/RobotRaconteurNET/Properties/AssemblyInfo.cs.in"
               "${CMAKE_CURRENT_BINARY_DIR}/AssemblyInfo.cs" @ONLY)

set(RR_NET_FRAMEWORK_VERSION "v4.5.2" CACHE STRING ".NET Framework version for library")
if(NOT RR_NET_BUILD_NATIVE_ONLY)

    if(WIN32)
        add_custom_target(
            RobotRaconteurNET ALL
            msbuild $<$<CONFIG:debug>:/p:Configuration=Debug>$<$<CONFIG:release>:/p:Configuration=Release> /t:rebuild
            /p:Platform="Any CPU" /nowarn:CS1591 RobotRaconteurNET.csproj
            WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
    else()
        find_program(XBUILD_EXECUTABLE NAMES xbuild msbuild)
        add_custom_target(
            RobotRaconteurNET ALL
            "${XBUILD_EXECUTABLE}"
            $<$<CONFIG:debug>:/p:Configuration=Debug>$<$<CONFIG:release>:/p:Configuration=Release> /p:Platform="Any CPU"
            RobotRaconteurNET.csproj
            WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")

        execute_process(
            COMMAND "${XBUILD_EXECUTABLE}" /version
            OUTPUT_VARIABLE XBUILD_VERSION_OUTPUT
            ERROR_VARIABLE XBUILD_VERSION_ERROR
            RESULT_VARIABLE XBUILD_VERSION_RESULT
            OUTPUT_STRIP_TRAILING_WHITESPACE)
        # Extract version number like "12.0" from "XBuild Engine Version 12.0"
        string(REGEX MATCH "XBuild Engine Version ([0-9]+\\.[0-9]+)" _match "${XBUILD_VERSION_OUTPUT}")
        set(XBUILD_VERSION "")
        if(_match)
            string(REGEX REPLACE "XBuild Engine Version ([0-9]+\\.[0-9]+)" "\\1" XBUILD_VERSION "${_match}")
        endif()
        if("${XBUILD_VERSION}" AND "${XBUILD_VERSION}" VERSION_LESS "14.0")
            set(RR_NET_DISABLED_WARNINGS "")
        else()
            set(RR_NET_DISABLED_WARNINGS "CS1591;CS1573")
        endif()
    endif()
    add_dependencies(RobotRaconteurNET RobotRaconteurNETNative)

    add_custom_command(
        TARGET RobotRaconteurNET POST_BUILD
        COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/xml_doc/merge_doc_xml.cmake
                $<$<CONFIG:debug>:Debug>$<$<CONFIG:release>:Release>)

    set(RR_NET_BUILT_DOC_XML "${CMAKE_CURRENT_BINARY_DIR}/xml_doc/RobotRaconteurNET_built_doc.xml ")
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/merge_doc_xml.cmake.in
                   ${CMAKE_CURRENT_BINARY_DIR}/xml_doc/merge_doc_xml.cmake @ONLY)
    set(RR_NET_DLL_DIR "${CMAKE_BINARY_DIR}/out/NET")
    configure_file(${CMAKE_SOURCE_DIR}/docs/net/docfx.json.in ${CMAKE_BINARY_DIR}/out/NET/docs/docfx.json @ONLY)
    configure_file(${CMAKE_SOURCE_DIR}/docs/net/generate_docfx_filter_config.cmake.in
                   ${CMAKE_BINARY_DIR}/out/NET/docs/generate_docfx_filter_config.cmake @ONLY)
    set(RR_NET_DLL_DIR "${CMAKE_BINARY_DIR}/out_debug/NET")
    configure_file(${CMAKE_SOURCE_DIR}/docs/net/docfx.json.in ${CMAKE_BINARY_DIR}/out_debug/NET/docs/docfx.json @ONLY)
    configure_file(${CMAKE_SOURCE_DIR}/docs/net/generate_docfx_filter_config.cmake.in
                   ${CMAKE_BINARY_DIR}/out_debug/NET/docs/generate_docfx_filter_config.cmake @ONLY)

    if(BUILD_DOCUMENTATION)
        find_program(DOCFX_EXECUTABLE NAMES docfx)

        if(NOT DOCFX_EXECUTABLE)
            message(FATAL_ERROR "docfx is required to build csharp documentation")
        endif()

        file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/docs/net)

        add_custom_target(
            RobotRaconteurNET_doc
            COMMAND ${CMAKE_COMMAND} -P generate_docfx_filter_config.cmake
            COMMAND
                ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/docs/net/source/index.md
                ${CMAKE_SOURCE_DIR}/docs/net/source/toc.yml
                $<TARGET_FILE_DIR:${SWIG_MODULE_RobotRaconteurNETNative_REAL_NAME}>/../docs
            COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/docs/net/source/articles
                    $<TARGET_FILE_DIR:${SWIG_MODULE_RobotRaconteurNETNative_REAL_NAME}>/../docs/articles
            COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/docs/net/source/images
                    $<TARGET_FILE_DIR:${SWIG_MODULE_RobotRaconteurNETNative_REAL_NAME}>/../docs/images
            COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/docs/net/source/api
                    $<TARGET_FILE_DIR:${SWIG_MODULE_RobotRaconteurNETNative_REAL_NAME}>/../docs/api
            COMMAND ${DOCFX_EXECUTABLE} metadata
            COMMAND ${DOCFX_EXECUTABLE} build --output ${CMAKE_BINARY_DIR}/docs
            WORKING_DIRECTORY $<TARGET_FILE_DIR:${SWIG_MODULE_RobotRaconteurNETNative_REAL_NAME}>/../docs
            COMMENT "Generating csharp documentation yaml"
            DEPENDS RobotRaconteurNET)

    endif()

    if(WIN32)
        if(CMAKE_SIZEOF_VOID_P EQUAL 8)
            set(RR_PREFER_32BIT "<Prefer32Bit>false</Prefer32Bit>")
        else()
            set(RR_PREFER_32BIT "<Prefer32Bit>true</Prefer32Bit>")
        endif()
    endif()

    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/RobotRaconteurNET/RobotRaconteurNET.csproj.in"
                   "${CMAKE_CURRENT_BINARY_DIR}/RobotRaconteurNET.csproj" @ONLY)

    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/RobotRaconteurNET/netstandard/RobotRaconteurNET.csproj.in"
                   "${CMAKE_CURRENT_BINARY_DIR}/netstandard/Lib/RobotRaconteurNET.csproj" @ONLY)

    if(WIN32 AND (MSVC_VERSION GREATER 1900))
        add_custom_target(
            RobotRaconteurNET_standard ALL
            msbuild
            /restore
            /verbosity:quiet
            $<$<CONFIG:debug>:/p:Configuration=Debug>$<$<CONFIG:release>:/p:Configuration=Release>
            /t:rebuild
            /p:Platform="Any CPU"
            RobotRaconteurNET.csproj
            WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/netstandard/Lib")
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
        add_dependencies(RobotRaconteurNET_standard RobotRaconteurNETNative)

        add_custom_command(
            TARGET RobotRaconteurNET_standard POST_BUILD
            COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/xml_doc/netstandard/merge_doc_xml.cmake
                    $<$<CONFIG:debug>:Debug>$<$<CONFIG:release>:Release> netstandard)

        set(RR_NET_BUILT_DOC_XML "${CMAKE_CURRENT_BINARY_DIR}/xml_doc/netstandard/RobotRaconteurNET_built_doc.xml ")
        configure_file(${CMAKE_CURRENT_SOURCE_DIR}/merge_doc_xml.cmake.in
                       ${CMAKE_CURRENT_BINARY_DIR}/xml_doc/netstandard/merge_doc_xml.cmake @ONLY)
    endif()

    string(TIMESTAMP CurrentYear "%Y")

endif()
