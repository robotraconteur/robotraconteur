

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/Modules)

find_package(PythonInterp3 REQUIRED)
find_package(PythonLibs3 REQUIRED)

set(PYTHON2_SOURCE_DIR "${CMAKE_SOURCE_DIR}/RobotRaconteurPython")

find_path(NUMPY3_INCLUDE_NDARRAYOBJECT_DIR numpy/ndarrayobject.h PATHS ${PYTHON3_INCLUDE_DIRS} NO_DEFAULT_PATH)
if (NUMPY3_INCLUDE_NDARRAYOBJECT_DIR)
set(NUMPY3_INCLUDE_DIR ${NUMPY3_INCLUDE_NDARRAYOBJECT_DIR})
else()
execute_process(COMMAND ${PYTHON3_EXECUTABLE} -c "import numpy; import os; print(os.path.dirname(numpy.__file__) + '/core/include')" RESULT_VARIABLE FIND_NUMPY3_RESULT OUTPUT_VARIABLE NUMPY3_INCLUDE_DIR )
string(STRIP "${NUMPY3_INCLUDE_DIR}" NUMPY3_INCLUDE_DIR)

if(${FIND_NUMPY3_RESULT})
message(FATAL_ERROR "Could not determine NumPy include directory")
endif()
endif()

if (NOT EXISTS "${NUMPY3_INCLUDE_DIR}/numpy/ndarrayobject.h")
message(FATAL_ERROR "Could not find numpy/ndarrayobject.h include file")
endif()

message(STATUS "NumPy Include Directory: ${NUMPY3_INCLUDE_DIR}")

include(${PYTHON2_SOURCE_DIR}/PythonPackageVersion.cmake)

INCLUDE_DIRECTORIES(${PYTHON3_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/SWIG ${PYTHON2_SOURCE_DIR} ${NUMPY3_INCLUDE_DIR})
GET_FILENAME_COMPONENT(PYTHON3_LINK_DIRS "${PYTHON3_LIBRARIES}" DIRECTORY)
LINK_DIRECTORIES(${PYTHON3_LINK_DIRS})

if (NOT USE_PREGENERATED_SOURCE)
INCLUDE(FindSWIG)
FIND_PACKAGE(SWIG 4.0.2 REQUIRED)
include(${CMAKE_SOURCE_DIR}/cmake/RRUseSwig.cmake)
include(${PYTHON2_SOURCE_DIR}/PythonSwigVars.cmake)
set(CMAKE_SWIG_OUTDIR ${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_SOURCE_DIR}/RobotRaconteurCore/include)
RR_SWIG_ADD_MODULE(RobotRaconteurPython3 python ${PYTHON2_SOURCE_DIR}/RobotRaconteurPython.i ${CMAKE_SOURCE_DIR}/SWIG/RobotRaconteurWrapped.cpp ${CMAKE_SOURCE_DIR}/SWIG/RobotRaconteurWrapped.h ${PYTHON2_SOURCE_DIR}/PythonTypeSupport.cpp ${PYTHON2_SOURCE_DIR}/PythonTypeSupport.h)
SWIG_LINK_LIBRARIES(RobotRaconteurPython3 RobotRaconteurCore)
else()
set(SWIG_MODULE_RobotRaconteurPython3_REAL_NAME _RobotRaconteurPython3)
if ("${SIZEOF_LONG_INT}" EQUAL 8 AND CMAKE_COMPILER_IS_GNUCXX )
set(CMAKE_SWIG_OUTDIR ${PREGENERATED_SOURCE_DIR}/Python3/swigwordsize64/)
else()
set(CMAKE_SWIG_OUTDIR ${PREGENERATED_SOURCE_DIR}/Python3/swigwordsize32/)
endif()
include_directories(${CMAKE_SOURCE_DIR}/SWIG ${CMAKE_SWIG_OUTDIR})
add_library(_RobotRaconteurPython3 MODULE ${CMAKE_SOURCE_DIR}/SWIG/RobotRaconteurWrapped.cpp ${CMAKE_SOURCE_DIR}/SWIG/RobotRaconteurWrapped.h ${PYTHON2_SOURCE_DIR}/PythonTypeSupport.cpp ${PYTHON2_SOURCE_DIR}/PythonTypeSupport.h ${CMAKE_SWIG_OUTDIR}/RobotRaconteurPythonPYTHON_wrap.cxx)
target_link_libraries(_RobotRaconteurPython3 RobotRaconteurCore)
set_target_properties(_RobotRaconteurPython3 PROPERTIES PREFIX "")
endif()

target_compile_definitions(${SWIG_MODULE_RobotRaconteurPython3_REAL_NAME} PRIVATE RR_PYTHON NPY_NO_DEPRECATED_API=NPY_1_7_API_VERSION SWIG_TYPE_TABLE=robotraconteur_python)
target_include_directories(${SWIG_MODULE_RobotRaconteurPython3_REAL_NAME} PRIVATE ${PYTHON3_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/SWIG ${CMAKE_CURRENT_SOURCE_DIR} ${NUMPY3_INCLUDE_DIR})

IF (MSVC)
target_compile_options(${SWIG_MODULE_RobotRaconteurPython3_REAL_NAME} PRIVATE "/bigobj")
ENDIF()

RRSetTargetDirs(${SWIG_MODULE_RobotRaconteurPython3_REAL_NAME} "Python3/RobotRaconteur" "Python3/RobotRaconteur")

if (WIN32)
set_target_properties(${SWIG_MODULE_RobotRaconteurPython3_REAL_NAME} PROPERTIES SUFFIX ".pyd")
else()
set_target_properties(${SWIG_MODULE_RobotRaconteurPython3_REAL_NAME} PROPERTIES SUFFIX ".so")
endif()
set_target_properties(${SWIG_MODULE_RobotRaconteurPython3_REAL_NAME} PROPERTIES OUTPUT_NAME "_RobotRaconteurPython")

if (NOT DEFINED RobotRaconteur_DIR)
add_dependencies(${SWIG_MODULE_RobotRaconteurPython3_REAL_NAME} RobotRaconteurCore)
endif()

ADD_CUSTOM_COMMAND(TARGET ${SWIG_MODULE_RobotRaconteurPython3_REAL_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SWIG_OUTDIR}/RobotRaconteurPython.py $<TARGET_FILE_DIR:${SWIG_MODULE_RobotRaconteurPython3_REAL_NAME}>/)
ADD_CUSTOM_COMMAND(TARGET ${SWIG_MODULE_RobotRaconteurPython3_REAL_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/RobotRaconteurPythonError.py $<TARGET_FILE_DIR:${SWIG_MODULE_RobotRaconteurPython3_REAL_NAME}>/)

FILE(GLOB RR_PY_SOURCES ${PYTHON2_SOURCE_DIR}/*.py)
FOREACH (P  ${RR_PY_SOURCES})
ADD_CUSTOM_COMMAND(TARGET ${SWIG_MODULE_RobotRaconteurPython3_REAL_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${P} $<TARGET_FILE_DIR:${SWIG_MODULE_RobotRaconteurPython3_REAL_NAME}>/)
ENDFOREACH(P)

configure_file("${PYTHON2_SOURCE_DIR}/setup.py.in" "${CMAKE_BINARY_DIR}/out/Python3/setup.py" @ONLY)
configure_file("${PYTHON2_SOURCE_DIR}/setup.py.in" "${CMAKE_BINARY_DIR}/out_debug/Python3/setup.py" @ONLY)
configure_file("${PYTHON2_SOURCE_DIR}/setup.py.in" "${CMAKE_BINARY_DIR}/out_reldebug/Python3/setup.py" @ONLY)

include("${PYTHON2_SOURCE_DIR}/PythonExceptions.cmake")

set(BUILD_PYTHON3_WHEEL OFF CACHE BOOL "Enable building Python 3 wheels")
if(BUILD_PYTHON3_WHEEL)
add_custom_target(RobotRaconteurPython3_Wheel ALL ${CMAKE_COMMAND} -E chdir "$<TARGET_FILE_DIR:${SWIG_MODULE_RobotRaconteurPython3_REAL_NAME}>/.." "${PYTHON3_EXECUTABLE}" setup.py bdist_wheel
	DEPENDS ${SWIG_MODULE_RobotRaconteurPython3_REAL_NAME})
if (WIN32)
set_target_properties(RobotRaconteurPython3_Wheel PROPERTIES EXCLUDE_FROM_DEFAULT_BUILD_DEBUG TRUE)
endif()
endif()

set(RR_PYTHON_TEST_CMD "\"${CMAKE_COMMAND}\"")
set(RR_PYTHON_TEST_CMD_ARGS "-E env PYTHONPATH=${CMAKE_BINARY_DIR}/@OUT_DIR_NAME@/Python3 ROBOTRACONTEUR_ROBDEF_PATH=${CMAKE_SOURCE_DIR}/testing/RobotRaconteurTest \"${PYTHON3_EXECUTABLE}\" ${CMAKE_SOURCE_DIR}/RobotRaconteurPython/test/RobotRaconteurPythonTest.py")
RRConfigureTest(test_prog_python3 "${RR_PYTHON_TEST_CMD}" "test_prog" NOCTEST NODEBUG ARGS "${RR_PYTHON_TEST_CMD_ARGS}")
RRConfigureTest(test_python3_loopback "${RR_PYTHON_TEST_CMD}" "test_prog" NODEBUG ARGS "${RR_PYTHON_TEST_CMD_ARGS} loopback")
RRConfigureTest(test_python3_loopback2 "${RR_PYTHON_TEST_CMD}" "test_prog" NODEBUG ARGS "${RR_PYTHON_TEST_CMD_ARGS} loopback2")
RRConfigureTest(test_python3_loopback3 "${RR_PYTHON_TEST_CMD}" "test_prog" NODEBUG ARGS "${RR_PYTHON_TEST_CMD_ARGS} loopback3")

if(ROBOTRACONTEUR_ROS)

# This mess is taken from github.com/ros/catkin/cmake/python.cmake...
set(enable_setuptools_deb_layout OFF)
if(EXISTS "/etc/debian_version")
  set(enable_setuptools_deb_layout ON)
endif()
option(SETUPTOOLS_DEB_LAYOUT "Enable debian style python package layout" ${enable_setuptools_deb_layout})
if(SETUPTOOLS_DEB_LAYOUT)
  message(STATUS "Using Debian Python package layout")
  set(SETUPTOOLS_ARG_EXTRA "--install-layout=deb")
endif()

install(CODE "if (\"$\{CMAKE_INSTALL_CONFIG_NAME\}\" STREQUAL \"Release\" OR \"$\{CMAKE_INSTALL_CONFIG_NAME\}\" STREQUAL \"\")
set(RR_BUILD_CONFIG_DIR \"out\")
elseif (\"$\{CMAKE_INSTALL_CONFIG_NAME\}\" STREQUAL \"RelWithDebInfo\")
set(RR_BUILD_CONFIG_DIR \"out_reldebug\")
elseif (\"$\{CMAKE_INSTALL_CONFIG_NAME\}\" STREQUAL \"Debug\")
set(RR_BUILD_CONFIG_DIR \"out_debug\")
else()
set(RR_BUILD_CONFIG_DIR \"out\")
endif()
message(STATUS \"Running setup.py in ${CMAKE_BINARY_DIR}/$\{RR_BUILD_CONFIG_DIR\}/Python3\")
execute_process(COMMAND ${PYTHON3_EXECUTABLE} setup.py install -f
--root=/ --prefix=${CMAKE_INSTALL_PREFIX} ${SETUPTOOLS_ARG_EXTRA} --single-version-externally-managed WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/$\{RR_BUILD_CONFIG_DIR\}/Python3)")
endif()


if(BUILD_DOCUMENTATION)
  
  find_program(SPHINX_EXECUTABLE NAMES sphinx-build)

  if(NOT SPHINX_EXECUTABLE)
    message(FATAL_ERROR "Sphinx is required to build Python documentation")
  endif()

  configure_file("${CMAKE_SOURCE_DIR}/docs/python/conf.py.in" "${CMAKE_BINARY_DIR}/out/Python3/sphinx/conf.py" @ONLY)
  configure_file("${CMAKE_SOURCE_DIR}/docs/python/conf.py.in" "${CMAKE_BINARY_DIR}/out_debug/Python3/sphinx/conf.py" @ONLY)
  configure_file("${CMAKE_SOURCE_DIR}/docs/python/conf.py.in" "${CMAKE_BINARY_DIR}/out_reldebug/Python3/sphinx/conf.py" @ONLY)

  file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/docs/python3)
  
  
  add_custom_target(RobotRaconteurPython3_doc
        COMMAND ${SPHINX_EXECUTABLE} -c $<TARGET_FILE_DIR:${SWIG_MODULE_RobotRaconteurPython3_REAL_NAME}>/../sphinx
        ${CMAKE_SOURCE_DIR}/docs/python/source ${CMAKE_BINARY_DIR}/docs/python3
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating Python documentation with Sphinx"
        VERBATIM DEPENDS ${SWIG_MODULE_RobotRaconteurPython3_REAL_NAME})
endif()