

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/Modules)

find_package(PythonInterp3 REQUIRED)
find_package(PythonLibs3 REQUIRED)

INCLUDE(FindSWIG)
FIND_PACKAGE(SWIG REQUIRED)
INCLUDE(UseSWIG)

set(PYTHON2_SOURCE_DIR "${CMAKE_SOURCE_DIR}/RobotRaconteurPython")

ADD_DEFINITIONS(-DRR_PYTHON -DNPY_NO_DEPRECATED_API=NPY_1_7_API_VERSION)

IF(WIN32)
IF (RobotRaconteur_USE_SHARED_CORE_LIB)
ADD_DEFINITIONS(-DROBOTRACONTEUR_CORE_IMPORTS)
ENDIF()
ENDIF()

# Fix for undefined symbols on OSX
# https://stackoverflow.com/questions/25421479/clang-and-undefined-symbols-when-building-a-library
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -undefined dynamic_lookup")
endif()

include(CheckTypeSize)
check_type_size("long int" SIZEOF_LONG_INT BUILTIN_TYPES_ONLY)
if ("${SWIG_PYTHON3_EXTRA_ARGS}" STREQUAL "" AND "${SIZEOF_LONG_INT}" EQUAL 8 AND CMAKE_COMPILER_IS_GNUCXX)
set(SWIG_PYTHON3_EXTRA_ARGS "-DSWIGWORDSIZE64" CACHE STRING "Swig extra args")
else()
set(SWIG_PYTHON3_EXTRA_ARGS "" CACHE STRING "Swig extra args")
endif()

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

INCLUDE_DIRECTORIES(${PYTHON3_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/SWIG ${PYTHON2_SOURCE_DIR} ${NUMPY3_INCLUDE_DIR})
GET_FILENAME_COMPONENT(PYTHON3_LINK_DIRS "${PYTHON3_LIBRARIES}" DIRECTORY)
LINK_DIRECTORIES(${PYTHON3_LINK_DIRS})

INCLUDE(${CMAKE_SOURCE_DIR}/SWIG/RobotRaconteurSWIGSources.cmake)

set(Python_SWIG_sources ${PYTHON2_SOURCE_DIR}/ClientPython.i ${PYTHON2_SOURCE_DIR}/DiscoveryPython.i ${PYTHON2_SOURCE_DIR}/HardwareTransportPython.i ${PYTHON2_SOURCE_DIR}/LocalTransportPython.i 
  ${PYTHON2_SOURCE_DIR}/MessagePython.i ${PYTHON2_SOURCE_DIR}/NodeIDPython.i ${PYTHON2_SOURCE_DIR}/PythonExceptionTypemaps.i  ${PYTHON2_SOURCE_DIR}/PythonTypemaps.i ${PYTHON2_SOURCE_DIR}/PythonTypeSupport.i 
  ${PYTHON2_SOURCE_DIR}/RobotRaconteurNodePython.i ${PYTHON2_SOURCE_DIR}/ServiceDefinitionPython.i ${PYTHON2_SOURCE_DIR}/ServicePython.i ${PYTHON2_SOURCE_DIR}/ServiceSecurityPython.i
  ${PYTHON2_SOURCE_DIR}/TcpTransportPython.i ${PYTHON2_SOURCE_DIR}/TimerPython.i ${PYTHON2_SOURCE_DIR}/TimeSpecPython.i)
set(Python_Py_sources  ${PYTHON2_SOURCE_DIR}/__init__.py ${PYTHON2_SOURCE_DIR}/classproperty.py ${PYTHON2_SOURCE_DIR}/Client.py 
  ${PYTHON2_SOURCE_DIR}/NamedClient.py ${PYTHON2_SOURCE_DIR}/RobotRaconteurPythonDataTypes.py ${PYTHON2_SOURCE_DIR}/RobotRaconteurPythonError.py 
  ${PYTHON2_SOURCE_DIR}/RobotRaconteurPythonUtil.py)

set(SWIG_MODULE_RobotRaconteurPython3_EXTRA_DEPS ${RobotRaconteur_SWIG_sources} ${Python_SWIG_sources} ${Python_Py_sources})

set_property(SOURCE ${PYTHON2_SOURCE_DIR}/RobotRaconteurPython.i PROPERTY CPLUSPLUS ON)
set_property(SOURCE ${PYTHON2_SOURCE_DIR}/RobotRaconteurPython.i PROPERTY SWIG_FLAGS ${SWIG_PYTHON3_EXTRA_ARGS} -relativeimport)
set_property(SOURCE ${PYTHON2_SOURCE_DIR}/RobotRaconteurPython.i PROPERTY SWIG_MODULE_NAME RobotRaconteurPython)
SWIG_ADD_MODULE(RobotRaconteurPython3 python  ${PYTHON2_SOURCE_DIR}/RobotRaconteurPython.i ${CMAKE_SOURCE_DIR}/SWIG/RobotRaconteurWrapped.cpp ${CMAKE_SOURCE_DIR}/SWIG/RobotRaconteurWrapped.h ${PYTHON2_SOURCE_DIR}/PythonTypeSupport.cpp ${PYTHON2_SOURCE_DIR}/PythonTypeSupport.h)

SWIG_LINK_LIBRARIES(RobotRaconteurPython3 ${RobotRaconteur_CORE_LIBRARY} ${Boost_LIBRARIES}  ${ROBOTRACONTEUR_EXTRA_LIBS})

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

ADD_CUSTOM_COMMAND(TARGET ${SWIG_MODULE_RobotRaconteurPython3_REAL_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/RobotRaconteurPython.py $<TARGET_FILE_DIR:${SWIG_MODULE_RobotRaconteurPython3_REAL_NAME}>/)
ADD_CUSTOM_COMMAND(TARGET ${SWIG_MODULE_RobotRaconteurPython3_REAL_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/RobotRaconteurPythonError.py $<TARGET_FILE_DIR:${SWIG_MODULE_RobotRaconteurPython3_REAL_NAME}>/)

FILE(GLOB RR_PY_SOURCES ${PYTHON2_SOURCE_DIR}/*.py)
FOREACH (P  ${RR_PY_SOURCES})
ADD_CUSTOM_COMMAND(TARGET ${SWIG_MODULE_RobotRaconteurPython3_REAL_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${P} $<TARGET_FILE_DIR:${SWIG_MODULE_RobotRaconteurPython3_REAL_NAME}>/)
ENDFOREACH(P)

configure_file("${PYTHON2_SOURCE_DIR}/setup.py.in" "${CMAKE_BINARY_DIR}/out/Python3/setup.py" @ONLY)
configure_file("${PYTHON2_SOURCE_DIR}/setup.py.in" "${CMAKE_BINARY_DIR}/out_debug/Python3/setup.py" @ONLY)

include("${PYTHON2_SOURCE_DIR}/PythonExceptions.cmake")