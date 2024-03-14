# Fix for undefined symbols on OSX
# https://stackoverflow.com/questions/25421479/clang-and-undefined-symbols-when-building-a-library
if(APPLE)
    set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -undefined dynamic_lookup")
endif()

if(NOT USE_PREGENERATED_SOURCE)
    if(NOT SWIG_PYTHON_EXTRA_ARGS_OVERRIDE)
        check_type_size("long int" SIZEOF_LONG_INT BUILTIN_TYPES_ONLY)
        if("${SWIG_PYTHON_EXTRA_ARGS}" STREQUAL "" AND "${SIZEOF_LONG_INT}" EQUAL 8 AND CMAKE_COMPILER_IS_GNUCXX)
            set(SWIG_PYTHON_EXTRA_ARGS "-DSWIGWORDSIZE64" CACHE STRING "Swig extra args")
        else()
            set(SWIG_PYTHON_EXTRA_ARGS "" CACHE STRING "Swig extra args")
        endif()
    else()
        set(SWIG_PYTHON_EXTRA_ARGS ${SWIG_PYTHON_EXTRA_ARGS_OVERRIDE})
    endif()

    include_directories(${CMAKE_SOURCE_DIR}/SWIG)

    include(${CMAKE_SOURCE_DIR}/SWIG/RobotRaconteurSWIGSources.cmake)

    set(Python_SWIG_sources
        ${CMAKE_CURRENT_LIST_DIR}/ClientPython.i
        ${CMAKE_CURRENT_LIST_DIR}/DiscoveryPython.i
        ${CMAKE_CURRENT_LIST_DIR}/HardwareTransportPython.i
        ${CMAKE_CURRENT_LIST_DIR}/LocalTransportPython.i
        ${CMAKE_CURRENT_LIST_DIR}/MessagePython.i
        ${CMAKE_CURRENT_LIST_DIR}/NodeIDPython.i
        ${CMAKE_CURRENT_LIST_DIR}/PythonExceptionTypemaps.i
        ${CMAKE_CURRENT_LIST_DIR}/PythonTypemaps.i
        ${CMAKE_CURRENT_LIST_DIR}/PythonTypeSupport.i
        ${CMAKE_CURRENT_LIST_DIR}/RobotRaconteurNodePython.i
        ${CMAKE_CURRENT_LIST_DIR}/ServiceDefinitionPython.i
        ${CMAKE_CURRENT_LIST_DIR}/ServicePython.i
        ${CMAKE_CURRENT_LIST_DIR}/ServiceSecurityPython.i
        ${CMAKE_CURRENT_LIST_DIR}/TcpTransportPython.i
        ${CMAKE_CURRENT_LIST_DIR}/TimerPython.i
        ${CMAKE_CURRENT_LIST_DIR}/TimeSpecPython.i)
    set(Python_Py_sources
        ${CMAKE_CURRENT_LIST_DIR}/__init__.py
        ${CMAKE_CURRENT_LIST_DIR}/classproperty.py
        ${CMAKE_CURRENT_LIST_DIR}/Client.py
        ${CMAKE_CURRENT_LIST_DIR}/NamedClient.py
        ${CMAKE_CURRENT_LIST_DIR}/RobotRaconteurPythonDataTypes.py
        ${CMAKE_CURRENT_BINARY_DIR}/RobotRaconteurPythonError.py
        ${CMAKE_CURRENT_LIST_DIR}/RobotRaconteurPythonUtil.py)

    set(SWIG_DEPENDS ${RobotRaconteur_SWIG_sources} ${Python_SWIG_sources} ${Python_Py_sources})
    set(SWIG_CXX_EXTENSION cxx)

    if(EMSCRIPTEN)
        set(SWIG_PYTHON_EXTRA_ARGS2
            ${SWIG_PYTHON_EXTRA_ARGS} -DROBOTRACONTEUR_EMSCRIPTEN=1 -DROBOTRACONTEUR_NO_TCP_TRANSPORT=1
            -DROBOTRACONTEUR_NO_LOCAL_TRANSPORT=1 -DROBOTRACONTEUR_NO_HARDWARE_TRANSPORT=1
            -DROBOTRACONTEUR_NO_NODE_SETUP=1)
    else()
        set(SWIG_PYTHON_EXTRA_ARGS2 ${SWIG_PYTHON_EXTRA_ARGS} -threads)
    endif()

    set_property(SOURCE ${CMAKE_CURRENT_LIST_DIR}/RobotRaconteurPython.i PROPERTY CPLUSPLUS ON)
    set_property(SOURCE ${CMAKE_CURRENT_LIST_DIR}/RobotRaconteurPython.i PROPERTY SWIG_FLAGS ${SWIG_PYTHON_EXTRA_ARGS2}
                                                                                  -relativeimport)

endif()

if(NOT EMSCRIPTEN)
    set(RR_PY_SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/__init__.py"
        "${CMAKE_CURRENT_LIST_DIR}/classproperty.py"
        "${CMAKE_CURRENT_LIST_DIR}/Client.py"
        "${CMAKE_CURRENT_LIST_DIR}/NamedClient.py"
        "${CMAKE_CURRENT_LIST_DIR}/RobotRaconteurPythonDataTypes.py"
        "${CMAKE_CURRENT_LIST_DIR}/RobotRaconteurPythonUtil.py")
else()
    set(RR_PY_SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/__init__.py" "${CMAKE_CURRENT_LIST_DIR}/classproperty.py"
        "${CMAKE_CURRENT_LIST_DIR}/emscripten/Client.py" "${CMAKE_CURRENT_LIST_DIR}/RobotRaconteurPythonDataTypes.py"
        "${CMAKE_CURRENT_LIST_DIR}/RobotRaconteurPythonUtil.py")
endif()
