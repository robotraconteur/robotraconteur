# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying file Copyright.txt or
# https://cmake.org/licensing for details.

# .rst: FindPythonInterp
# ----------------
#
# Find python interpreter
#
# This module finds if Python interpreter is installed and determines where the executables are.  This code sets the
# following variables:
#
# ::
#
# PYTHONINTERP3_FOUND         - Was the Python executable found PYTHON3_EXECUTABLE          - path to the Python
# interpreter
#
# ::
#
# PYTHON3_VERSION_STRING      - Python version found e.g. 2.5.2 PYTHON3_VERSION_MAJOR       - Python major version found
# e.g. 2 PYTHON3_VERSION_MINOR       - Python minor version found e.g. 5 PYTHON3_VERSION_PATCH       - Python patch
# version found e.g. 2
#
# The Python3_ADDITIONAL_VERSIONS variable can be used to specify a list of version numbers that should be taken into
# account when searching for Python.  You need to set this variable before calling find_package(PythonInterp).
#
# If calling both ``find_package(PythonInterp)`` and ``find_package(PythonLibs)``, call ``find_package(PythonInterp)``
# first to get the currently active Python version by default with a consistent version of PYTHON3_LIBRARIES.

unset(_Python3_NAMES)

set(_PYTHON3_VERSIONS
    3.12
    3.11
    3.10
    3.9
    3.8
    3.7
    3.6
    3.5
    3.4
    3.3
    3.2
    3.1
    3.0)

set(_PYTHON3_FIND_OTHER_VERSIONS ${_PYTHON3_VERSIONS})

# find_program(PYTHON3_EXECUTABLE NAMES ${_Python3_NAMES})

# Set up the versions we know about, in the order we will search. Always add the user supplied additional versions to
# the front.
set(_Python3_VERSIONS ${Python3_ADDITIONAL_VERSIONS})
# If FindPythonInterp has already found the major and minor version, insert that version next to get consistent versions
# of the interpreter and library.
if(DEFINED PYTHONLIBS3_VERSION_STRING)
    string(REPLACE "." ";" _PYTHONLIBS3_VERSION "${PYTHONLIBS3_VERSION_STRING}")
    list(GET _PYTHONLIBS3_VERSION 0 _PYTHONLIBS3_VERSION_MAJOR)
    list(GET _PYTHONLIBS3_VERSION 1 _PYTHONLIBS3_VERSION_MINOR)
    list(APPEND _Python3_VERSIONS ${_PYTHONLIBS3_VERSION_MAJOR}.${_PYTHONLIBS3_VERSION_MINOR})
endif()
# Search for the current active python version first list(APPEND _Python3_VERSIONS ";")
list(APPEND _Python3_VERSIONS ${_PYTHON3_FIND_OTHER_VERSIONS})

unset(_PYTHON3_FIND_OTHER_VERSIONS)
unset(_PYTHON1_VERSIONS)
unset(_PYTHON2_VERSIONS)
unset(_PYTHON3_VERSIONS)

# Search for newest python version if python executable isn't found
if(NOT PYTHON3_EXECUTABLE)
    foreach(_CURRENT_VERSION IN LISTS _Python3_VERSIONS)
        set(_Python3_NAMES python${_CURRENT_VERSION})
        if(CMAKE_HOST_WIN32)
            list(APPEND _Python3_NAMES python)
        endif()
        find_program(
            PYTHON3_EXECUTABLE
            NAMES ${_Python3_NAMES}
            PATHS [HKEY_LOCAL_MACHINE\\SOFTWARE\\Python\\PythonCore\\${_CURRENT_VERSION}\\InstallPath]
                  [HKEY_LOCAL_MACHINE\\SOFTWARE\\Python\\PythonCore\\${_CURRENT_VERSION}-32\\InstallPath]
                  [HKEY_LOCAL_MACHINE\\SOFTWARE\\Python\\PythonCore\\${_CURRENT_VERSION}-64\\InstallPath])
    endforeach()
endif()

# determine python version string
if(PYTHON3_EXECUTABLE)
    execute_process(
        COMMAND "${PYTHON3_EXECUTABLE}" -c
                "import sys; sys.stdout.write(';'.join([str(x) for x in sys.version_info[:3]]))"
        OUTPUT_VARIABLE _VERSION
        RESULT_VARIABLE _PYTHON3_VERSION_RESULT
        ERROR_QUIET)
    if(NOT _PYTHON3_VERSION_RESULT)
        string(REPLACE ";" "." PYTHON3_VERSION_STRING "${_VERSION}")
        list(GET _VERSION 0 PYTHON3_VERSION_MAJOR)
        list(GET _VERSION 1 PYTHON3_VERSION_MINOR)
        list(GET _VERSION 2 PYTHON3_VERSION_PATCH)
        if(PYTHON3_VERSION_PATCH EQUAL 0)
            # it's called "Python 2.7", not "2.7.0"
            string(REGEX REPLACE "\\.0$" "" PYTHON3_VERSION_STRING "${PYTHON3_VERSION_STRING}")
        endif()
    else()
        # sys.version predates sys.version_info, so use that
        execute_process(
            COMMAND "${PYTHON3_EXECUTABLE}" -c "import sys; sys.stdout.write(sys.version)"
            OUTPUT_VARIABLE _VERSION
            RESULT_VARIABLE _PYTHON3_VERSION_RESULT
            ERROR_QUIET)
        if(NOT _PYTHON3_VERSION_RESULT)
            string(REGEX REPLACE " .*" "" PYTHON3_VERSION_STRING "${_VERSION}")
            string(REGEX REPLACE "^([0-9]+)\\.[0-9]+.*" "\\1" PYTHON3_VERSION_MAJOR "${PYTHON3_VERSION_STRING}")
            string(REGEX REPLACE "^[0-9]+\\.([0-9])+.*" "\\1" PYTHON3_VERSION_MINOR "${PYTHON3_VERSION_STRING}")
            if(PYTHON3_VERSION_STRING MATCHES "^[0-9]+\\.[0-9]+\\.([0-9]+)")
                set(PYTHON3_VERSION_PATCH "${CMAKE_MATCH_1}")
            else()
                set(PYTHON3_VERSION_PATCH "0")
            endif()
        else()
            # sys.version was first documented for Python 1.5, so assume this is older.
            set(PYTHON3_VERSION_STRING "1.4")
            set(PYTHON3_VERSION_MAJOR "1")
            set(PYTHON3_VERSION_MINOR "4")
            set(PYTHON3_VERSION_PATCH "0")
        endif()
    endif()
    unset(_PYTHON3_VERSION_RESULT)
    unset(_VERSION)
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(PythonInterp3 REQUIRED_VARS PYTHON3_EXECUTABLE VERSION_VAR PYTHON3_VERSION_STRING)

mark_as_advanced(PYTHON3_EXECUTABLE)
