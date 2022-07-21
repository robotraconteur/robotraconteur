set(Python_ADDITIONAL_VERSIONS 2.7)

find_package(PythonInterp REQUIRED)
find_package(PythonLibs REQUIRED)

find_path(NUMPY_INCLUDE_NDARRAYOBJECT_DIR numpy/ndarrayobject.h PATHS ${PYTHON_INCLUDE_DIRS} NO_DEFAULT_PATH)
if(NUMPY_INCLUDE_NDARRAYOBJECT_DIR)
    set(NUMPY_INCLUDE_DIR ${NUMPY_INCLUDE_NDARRAYOBJECT_DIR})
else()
    execute_process(
        COMMAND ${PYTHON_EXECUTABLE} -c
                "import numpy; import os; print(os.path.dirname(numpy.__file__) + '/core/include')"
        RESULT_VARIABLE FIND_NUMPY_RESULT OUTPUT_VARIABLE NUMPY_INCLUDE_DIR)
    string(STRIP "${NUMPY_INCLUDE_DIR}" NUMPY_INCLUDE_DIR)

    if(${FIND_NUMPY_RESULT})
        message(FATAL_ERROR "Could not determine NumPy include directory")
    endif()
endif()

if(NOT EXISTS "${NUMPY_INCLUDE_DIR}/numpy/ndarrayobject.h")
    message(FATAL_ERROR "Could not find numpy/ndarrayobject.h include file")
endif()

message(STATUS "NumPy Include Directory: ${NUMPY_INCLUDE_DIR}")

include(PythonPackageVersion.cmake)

get_filename_component(PYTHON_LINK_DIRS "${PYTHON_LIBRARIES}" DIRECTORY)
link_directories(${PYTHON_LINK_DIRS})

if(NOT USE_PREGENERATED_SOURCE)
    include(FindSWIG)
    find_package(SWIG 4.0.2 REQUIRED)
    include(${CMAKE_SOURCE_DIR}/cmake/RRUseSwig.cmake)
    include(PythonSwigVars.cmake)
    set(CMAKE_SWIG_OUTDIR ${CMAKE_CURRENT_BINARY_DIR})
    include_directories(${CMAKE_SOURCE_DIR}/RobotRaconteurCore/include)
    rr_swig_add_module(
        RobotRaconteurPython
        python
        RobotRaconteurPython.i
        ${CMAKE_SOURCE_DIR}/SWIG/RobotRaconteurWrapped.cpp
        ${CMAKE_SOURCE_DIR}/SWIG/RobotRaconteurWrapped.h
        PythonTypeSupport.cpp
        PythonTypeSupport.h)
    swig_link_libraries(RobotRaconteurPython RobotRaconteurCore)
else()
    set(SWIG_MODULE_RobotRaconteurPython_REAL_NAME _RobotRaconteurPython)

    if("${SIZEOF_LONG_INT}" EQUAL 8 AND CMAKE_COMPILER_IS_GNUCXX)
        set(CMAKE_SWIG_OUTDIR ${PREGENERATED_SOURCE_DIR}/Python/swigwordsize64/)
    else()
        set(CMAKE_SWIG_OUTDIR ${PREGENERATED_SOURCE_DIR}/Python/swigwordsize32/)
    endif()
    include_directories(${CMAKE_SOURCE_DIR}/SWIG ${CMAKE_SWIG_OUTDIR})
    add_library(
        _RobotRaconteurPython MODULE
        ${CMAKE_SOURCE_DIR}/SWIG/RobotRaconteurWrapped.cpp ${CMAKE_SOURCE_DIR}/SWIG/RobotRaconteurWrapped.h
        PythonTypeSupport.cpp PythonTypeSupport.h ${CMAKE_SWIG_OUTDIR}/RobotRaconteurPythonPYTHON_wrap.cxx)
    target_link_libraries(_RobotRaconteurPython RobotRaconteurCore)
    set_target_properties(_RobotRaconteurPython PROPERTIES PREFIX "")
endif()

target_compile_definitions(
    ${SWIG_MODULE_RobotRaconteurPython_REAL_NAME} PRIVATE RR_PYTHON NPY_NO_DEPRECATED_API=NPY_1_7_API_VERSION
                                                          SWIG_TYPE_TABLE=robotraconteur_python)
target_include_directories(
    ${SWIG_MODULE_RobotRaconteurPython_REAL_NAME} PRIVATE ${PYTHON_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/SWIG
                                                          ${CMAKE_CURRENT_SOURCE_DIR} ${NUMPY_INCLUDE_DIR})

if(MSVC)
    target_compile_options(${SWIG_MODULE_RobotRaconteurPython_REAL_NAME} PRIVATE "/bigobj")
endif()

rrsettargetdirs(${SWIG_MODULE_RobotRaconteurPython_REAL_NAME} "Python/RobotRaconteur" "Python/RobotRaconteur")

if(WIN32)
    set_target_properties(${SWIG_MODULE_RobotRaconteurPython_REAL_NAME} PROPERTIES SUFFIX ".pyd")
    set_target_properties(${SWIG_MODULE_RobotRaconteurPython_REAL_NAME} PROPERTIES EXCLUDE_FROM_DEFAULT_BUILD_DEBUG
                                                                                   TRUE)
else()
    set_target_properties(${SWIG_MODULE_RobotRaconteurPython_REAL_NAME} PROPERTIES SUFFIX ".so")
endif()

if(NOT DEFINED RobotRaconteur_DIR)
    add_dependencies(${SWIG_MODULE_RobotRaconteurPython_REAL_NAME} RobotRaconteurCore)
endif()

add_custom_command(
    TARGET ${SWIG_MODULE_RobotRaconteurPython_REAL_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SWIG_OUTDIR}/RobotRaconteurPython.py
            $<TARGET_FILE_DIR:${SWIG_MODULE_RobotRaconteurPython_REAL_NAME}>/)
add_custom_command(
    TARGET ${SWIG_MODULE_RobotRaconteurPython_REAL_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/RobotRaconteurPythonError.py
            $<TARGET_FILE_DIR:${SWIG_MODULE_RobotRaconteurPython_REAL_NAME}>/)

file(GLOB RR_PY_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.py)
foreach(P ${RR_PY_SOURCES})
    get_filename_component(P_NAME ${P} NAME)
    set(P2 ${P})
    set(P3 $<SHELL_PATH:$<TARGET_FILE_DIR:${SWIG_MODULE_RobotRaconteurPython_REAL_NAME}>>/${P_NAME})
    set(PYTHON_COPY_COMMAND ${CMAKE_COMMAND} -E copy)
    if(MSVC)
        set(PYTHON_USE_HARD_LINKS "" CACHE BOOL "Use hard links for Python copy")
        if(PYTHON_USE_HARD_LINKS)
            set(PYTHON_COPY_COMMAND mklink /h)
            set(P2 ${P3})
            set(P3 ${P})
            string(REPLACE "/" "\\" P2 ${P2})
            string(REPLACE "/" "\\" P3 ${P3})
            add_custom_command(TARGET ${SWIG_MODULE_RobotRaconteurPython_REAL_NAME} POST_BUILD COMMAND del ${P2})
        endif()
    endif()
    add_custom_command(TARGET ${SWIG_MODULE_RobotRaconteurPython_REAL_NAME} POST_BUILD COMMAND ${PYTHON_COPY_COMMAND}
                                                                                               ${P2} ${P3})
endforeach(P)

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/setup.py.in" "${CMAKE_BINARY_DIR}/out/Python/setup.py" @ONLY)
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/setup.py.in" "${CMAKE_BINARY_DIR}/out_debug/Python/setup.py" @ONLY)
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/setup.py.in" "${CMAKE_BINARY_DIR}/out_reldebug/Python/setup.py" @ONLY)

set(PYTHON2_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
include("${CMAKE_CURRENT_SOURCE_DIR}/PythonExceptions.cmake")

set(BUILD_PYTHON_WHEEL OFF CACHE BOOL "Enable building Python 2 wheels")
if(BUILD_PYTHON_WHEEL)
    add_custom_target(
        RobotRaconteurPython_Wheel ALL
        ${CMAKE_COMMAND}
        -E
        chdir
        "$<TARGET_FILE_DIR:${SWIG_MODULE_RobotRaconteurPython_REAL_NAME}>/.."
        "${PYTHON_EXECUTABLE}"
        setup.py
        bdist_wheel
        DEPENDS ${SWIG_MODULE_RobotRaconteurPython_REAL_NAME})
    if(WIN32)
        set_target_properties(RobotRaconteurPython_Wheel PROPERTIES EXCLUDE_FROM_DEFAULT_BUILD_DEBUG TRUE)
    endif()
endif()

# This mess is taken from github.com/ros/catkin/cmake/python.cmake...
set(enable_setuptools_deb_layout OFF)
if(ROBOTRACONTEUR_ROS)
    if(EXISTS "/etc/debian_version")
        set(enable_setuptools_deb_layout ON)
    endif()
endif()

option(SETUPTOOLS_DEB_LAYOUT "Enable debian style python package layout" ${enable_setuptools_deb_layout})
if(SETUPTOOLS_DEB_LAYOUT)
    message(STATUS "Using Debian Python package layout")
    set(SETUPTOOLS_ARG_EXTRA "--install-layout=deb")
endif()

option(INSTALL_PYTHON "Install Python library" OFF)
if(ROBOTRACONTEUR_ROS OR ROBOTRACONTEUR_COLCON OR INSTALL_PYTHON)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/python_install.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/python_install.cmake
                   @ONLY)
    install(SCRIPT ${CMAKE_CURRENT_BINARY_DIR}/python_install.cmake)
endif()
