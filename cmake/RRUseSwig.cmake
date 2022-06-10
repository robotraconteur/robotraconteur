if(${CMAKE_VERSION} VERSION_GREATER "3.13.0" OR ${CMAKE_VERSION} VERSION_EQUAL "3.13.0")
    cmake_policy(SET CMP0078 OLD)
endif()

if(${CMAKE_VERSION} VERSION_GREATER "3.14.0" OR ${CMAKE_VERSION} VERSION_EQUAL "3.14.0")
    cmake_policy(SET CMP0086 OLD)
endif()

if(${CMAKE_VERSION} VERSION_GREATER "3.21.0" OR ${CMAKE_VERSION} VERSION_EQUAL "3.14.0")
    cmake_policy(SET CMP0122 OLD)
endif()

include(UseSWIG)

if(${CMAKE_VERSION} VERSION_GREATER "3.8.0" OR ${CMAKE_VERSION} VERSION_EQUAL "3.8.0")
    macro(RR_SWIG_ADD_MODULE name language)
        swig_add_library(
            ${name}
            LANGUAGE ${language}
            TYPE MODULE
            SOURCES ${ARGN})
    endmacro()
else()

    macro(RR_SWIG_ADD_MODULE name language)
        swig_add_module(${name} ${language} ${ARGN})
    endmacro()
endif()
