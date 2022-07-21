set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/Modules)

find_package(PythonInterp3 REQUIRED)
find_package(PythonLibs3 REQUIRED)

set(PYTHON2_SOURCE_DIR "${CMAKE_SOURCE_DIR}/RobotRaconteurPython")

find_path(NUMPY3_INCLUDE_NDARRAYOBJECT_DIR numpy/ndarrayobject.h PATHS ${PYTHON3_INCLUDE_DIRS} NO_DEFAULT_PATH)
if(NUMPY3_INCLUDE_NDARRAYOBJECT_DIR)
    set(NUMPY3_INCLUDE_DIR ${NUMPY3_INCLUDE_NDARRAYOBJECT_DIR})
else()
    execute_process(
        COMMAND ${PYTHON3_EXECUTABLE} -c
                "import numpy; import os; print(os.path.dirname(numpy.__file__) + '/core/include')"
        RESULT_VARIABLE FIND_NUMPY3_RESULT OUTPUT_VARIABLE NUMPY3_INCLUDE_DIR)
    string(STRIP "${NUMPY3_INCLUDE_DIR}" NUMPY3_INCLUDE_DIR)

    if(${FIND_NUMPY3_RESULT})
        message(FATAL_ERROR "Could not determine NumPy include directory")
    endif()
endif()

if(NOT EXISTS "${NUMPY3_INCLUDE_DIR}/numpy/ndarrayobject.h")
    message(FATAL_ERROR "Could not find numpy/ndarrayobject.h include file")
endif()

message(STATUS "NumPy Include Directory: ${NUMPY3_INCLUDE_DIR}")

include(${PYTHON2_SOURCE_DIR}/PythonPackageVersion.cmake)

include_directories(${PYTHON3_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/SWIG ${PYTHON2_SOURCE_DIR} ${NUMPY3_INCLUDE_DIR})
get_filename_component(PYTHON3_LINK_DIRS "${PYTHON3_LIBRARIES}" DIRECTORY)
link_directories(${PYTHON3_LINK_DIRS})

if(NOT USE_PREGENERATED_SOURCE)
    include(FindSWIG)
    find_package(SWIG 4.0.2 REQUIRED)
    include(${CMAKE_SOURCE_DIR}/cmake/RRUseSwig.cmake)
    include(${PYTHON2_SOURCE_DIR}/PythonSwigVars.cmake)
    set(CMAKE_SWIG_OUTDIR ${CMAKE_CURRENT_BINARY_DIR})
    include_directories(${CMAKE_SOURCE_DIR}/RobotRaconteurCore/include)
    rr_swig_add_module(
        RobotRaconteurPython3
        python
        ${PYTHON2_SOURCE_DIR}/RobotRaconteurPython.i
        ${CMAKE_SOURCE_DIR}/SWIG/RobotRaconteurWrapped.cpp
        ${CMAKE_SOURCE_DIR}/SWIG/RobotRaconteurWrapped.h
        ${PYTHON2_SOURCE_DIR}/PythonTypeSupport.cpp
        ${PYTHON2_SOURCE_DIR}/PythonTypeSupport.h)
    swig_link_libraries(RobotRaconteurPython3 RobotRaconteurCore)
else()
    set(SWIG_MODULE_RobotRaconteurPython3_REAL_NAME _RobotRaconteurPython3)
    if("${SIZEOF_LONG_INT}" EQUAL 8 AND CMAKE_COMPILER_IS_GNUCXX)
        set(CMAKE_SWIG_OUTDIR ${PREGENERATED_SOURCE_DIR}/Python3/swigwordsize64/)
    else()
        set(CMAKE_SWIG_OUTDIR ${PREGENERATED_SOURCE_DIR}/Python3/swigwordsize32/)
    endif()
    include_directories(${CMAKE_SOURCE_DIR}/SWIG ${CMAKE_SWIG_OUTDIR})
    add_library(
        _RobotRaconteurPython3 MODULE
        ${CMAKE_SOURCE_DIR}/SWIG/RobotRaconteurWrapped.cpp ${CMAKE_SOURCE_DIR}/SWIG/RobotRaconteurWrapped.h
        ${PYTHON2_SOURCE_DIR}/PythonTypeSupport.cpp ${PYTHON2_SOURCE_DIR}/PythonTypeSupport.h
        ${CMAKE_SWIG_OUTDIR}/RobotRaconteurPythonPYTHON_wrap.cxx)
    target_link_libraries(_RobotRaconteurPython3 RobotRaconteurCore)
    set_target_properties(_RobotRaconteurPython3 PROPERTIES PREFIX "")
endif()

target_compile_definitions(
    ${SWIG_MODULE_RobotRaconteurPython3_REAL_NAME} PRIVATE RR_PYTHON NPY_NO_DEPRECATED_API=NPY_1_7_API_VERSION
                                                           SWIG_TYPE_TABLE=robotraconteur_python)
target_include_directories(
    ${SWIG_MODULE_RobotRaconteurPython3_REAL_NAME} PRIVATE ${PYTHON3_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/SWIG
                                                           ${CMAKE_CURRENT_SOURCE_DIR} ${NUMPY3_INCLUDE_DIR})

if(MSVC)
    target_compile_options(${SWIG_MODULE_RobotRaconteurPython3_REAL_NAME} PRIVATE "/bigobj")
endif()

rrsettargetdirs(${SWIG_MODULE_RobotRaconteurPython3_REAL_NAME} "Python3/RobotRaconteur" "Python3/RobotRaconteur")

if(WIN32)
    set_target_properties(${SWIG_MODULE_RobotRaconteurPython3_REAL_NAME} PROPERTIES SUFFIX ".pyd")
else()
    set_target_properties(${SWIG_MODULE_RobotRaconteurPython3_REAL_NAME} PROPERTIES SUFFIX ".so")
endif()
set_target_properties(${SWIG_MODULE_RobotRaconteurPython3_REAL_NAME} PROPERTIES OUTPUT_NAME "_RobotRaconteurPython")

if(NOT DEFINED RobotRaconteur_DIR)
    add_dependencies(${SWIG_MODULE_RobotRaconteurPython3_REAL_NAME} RobotRaconteurCore)
endif()

add_custom_command(
    TARGET ${SWIG_MODULE_RobotRaconteurPython3_REAL_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SWIG_OUTDIR}/RobotRaconteurPython.py
            $<TARGET_FILE_DIR:${SWIG_MODULE_RobotRaconteurPython3_REAL_NAME}>/)
add_custom_command(
    TARGET ${SWIG_MODULE_RobotRaconteurPython3_REAL_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/RobotRaconteurPythonError.py
            $<TARGET_FILE_DIR:${SWIG_MODULE_RobotRaconteurPython3_REAL_NAME}>/)

file(GLOB RR_PY_SOURCES ${PYTHON2_SOURCE_DIR}/*.py)
foreach(P ${RR_PY_SOURCES})
    add_custom_command(
        TARGET ${SWIG_MODULE_RobotRaconteurPython3_REAL_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${P} $<TARGET_FILE_DIR:${SWIG_MODULE_RobotRaconteurPython3_REAL_NAME}>/)
endforeach(P)

configure_file("${PYTHON2_SOURCE_DIR}/setup.py.in" "${CMAKE_BINARY_DIR}/out/Python3/setup.py" @ONLY)
configure_file("${PYTHON2_SOURCE_DIR}/setup.py.in" "${CMAKE_BINARY_DIR}/out_debug/Python3/setup.py" @ONLY)
configure_file("${PYTHON2_SOURCE_DIR}/setup.py.in" "${CMAKE_BINARY_DIR}/out_reldebug/Python3/setup.py" @ONLY)

include("${PYTHON2_SOURCE_DIR}/PythonExceptions.cmake")

set(BUILD_PYTHON3_WHEEL OFF CACHE BOOL "Enable building Python 3 wheels")
if(BUILD_PYTHON3_WHEEL)
    add_custom_target(
        RobotRaconteurPython3_Wheel ALL
        ${CMAKE_COMMAND}
        -E
        chdir
        "$<TARGET_FILE_DIR:${SWIG_MODULE_RobotRaconteurPython3_REAL_NAME}>/.."
        "${PYTHON3_EXECUTABLE}"
        setup.py
        bdist_wheel
        DEPENDS ${SWIG_MODULE_RobotRaconteurPython3_REAL_NAME})
    if(WIN32)
        set_target_properties(RobotRaconteurPython3_Wheel PROPERTIES EXCLUDE_FROM_DEFAULT_BUILD_DEBUG TRUE)
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

option(INSTALL_PYTHON3 "Install Python3 library" OFF)
if(ROBOTRACONTEUR_ROS OR ROBOTRACONTEUR_COLCON OR INSTALL_PYTHON3)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/python3_install.cmake.in
                   ${CMAKE_CURRENT_BINARY_DIR}/python3_install.cmake @ONLY)
    install(SCRIPT ${CMAKE_CURRENT_BINARY_DIR}/python3_install.cmake)
endif()

if(ROBOTRACONTEUR_ROS OR ROBOTRACONTEUR_COLCON)
    # Allows Colcon to find Python package
    if(SETUPTOOLS_DEB_LAYOUT)
        set(ROS_PYTHONPATH lib/python3/dist-packages)
    elseif(WIN32)
        set(ROS_PYTHONPATH lib/site-packages)
    else()
        set(ROS_PYTHONPATH lib/python3.${PYTHON_VERSION_MINOR}/dist-packages)
    endif()
    file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/share/robotraconteur/hook/python_path.dsv
         "prepend-non-duplicate;PYTHONPATH;${ROS_PYTHONPATH}")
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/share/robotraconteur/hook/python_path.dsv
            DESTINATION share/robotraconteur/hook)
endif()

if(BUILD_DOCUMENTATION)

    find_program(SPHINX_EXECUTABLE NAMES sphinx-build)

    if(NOT SPHINX_EXECUTABLE)
        message(FATAL_ERROR "Sphinx is required to build Python documentation")
    endif()

    configure_file("${CMAKE_SOURCE_DIR}/docs/python/conf.py.in" "${CMAKE_BINARY_DIR}/out/Python3/sphinx/conf.py" @ONLY)
    configure_file("${CMAKE_SOURCE_DIR}/docs/python/conf.py.in" "${CMAKE_BINARY_DIR}/out_debug/Python3/sphinx/conf.py"
                   @ONLY)
    configure_file("${CMAKE_SOURCE_DIR}/docs/python/conf.py.in"
                   "${CMAKE_BINARY_DIR}/out_reldebug/Python3/sphinx/conf.py" @ONLY)

    file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/docs/python3)

    add_custom_target(
        RobotRaconteurPython3_doc
        COMMAND ${SPHINX_EXECUTABLE} -c $<TARGET_FILE_DIR:${SWIG_MODULE_RobotRaconteurPython3_REAL_NAME}>/../sphinx
                ${CMAKE_SOURCE_DIR}/docs/python/source ${CMAKE_BINARY_DIR}/docs/python3
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating Python documentation with Sphinx"
        VERBATIM
        DEPENDS ${SWIG_MODULE_RobotRaconteurPython3_REAL_NAME})
endif()
