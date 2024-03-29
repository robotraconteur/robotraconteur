function(RR_SWIG_PARSE_ERRORS err_out)
    file(READ "${CMAKE_SOURCE_DIR}/RobotRaconteurCore/include/RobotRaconteur/Error.h" RR_ERROR_H_FILE)

    string(REGEX REPLACE "\\\\(\n|\r\n)" " " RR_ERROR_H_FILE2 ${RR_ERROR_H_FILE})
    string(REGEX MATCH "\\#define RR_EXCEPTION_TYPES_INIT\\(M,M2\\)([^\n\r]*)" RR_ERRORS_1 ${RR_ERROR_H_FILE2})
    string(REGEX REPLACE "\\#define RR_EXCEPTION_TYPES_INIT\\(M,M2\\)([^\n\r]*)" "\\1" RR_ERRORS_2 ${RR_ERRORS_1})
    string(REGEX MATCHALL "(M|M2)\\(([^,\\)]+),[ \t]+([^,\\)]+)(,[ \t]+([^\\)]+))?\\)" RR_ERRORS_3 ${RR_ERRORS_2})
    set(RR_ERRORS_7 "")
    foreach(RR_ERROR_4 ${RR_ERRORS_3})
        string(REPLACE "(" ", " RR_ERROR_5 ${RR_ERROR_4})
        string(REGEX REPLACE "\\)| " "" RR_ERROR_6 ${RR_ERROR_5})
        list(APPEND RR_ERRORS_7 ${RR_ERROR_6})
    endforeach()
    set(${err_out} ${RR_ERRORS_7} PARENT_SCOPE)
endfunction()

function(
    RR_SWIG_REPLACE_ERRORS
    M
    M2
    MOVERRIDE
    RR_ERRORS
    OVERRIDE
    txt_out)
    cmake_parse_arguments(RR_SWIG_REPLACE_ERRORS "REMOVE_OVERRIDE;OVERRIDE_ONLY" "" "IGNORE_CODE" ${ARGN})
    set(o "")
    foreach(override0 ${OVERRIDE})
        string(REPLACE "," ";" override1 ${override0})
        list(GET override1 0 override2)
        list(APPEND OVERRIDE_NAMES ${override2})
    endforeach()
    foreach(RR_ERROR ${RR_ERRORS})
        string(REPLACE "," ";" RR_ERROR2 ${RR_ERROR})
        list(GET RR_ERROR2 0 RR_ERROR_M)
        list(GET RR_ERROR2 1 RR_ERROR_TYPE)
        list(GET RR_ERROR2 2 RR_ERROR_CODE)
        list(FIND OVERRIDE_NAMES "${RR_ERROR_CODE}" _index)
        if(${_index} GREATER -1)
            list(GET OVERRIDE ${_index} override3)
            string(REPLACE "," ";" override4 ${override3})
            list(GET override4 1 RR_ERROR_TYPE)
            set(RR_ERROR_M "MOVERRIDE")
        endif()
        list(FIND RR_SWIG_REPLACE_ERRORS_IGNORE_CODE "${RR_ERROR_CODE}" _index2)
        if(${_index2} LESS 0 AND (NOT ((${_index} GREATER -1) AND RR_SWIG_REPLACE_ERRORS_REMOVE_OVERRIDE)
                                  AND (NOT ((${_index} LESS 0) AND RR_SWIG_REPLACE_ERRORS_OVERRIDE_ONLY))))
            if(RR_ERROR_M STREQUAL "M")
                list(GET RR_ERROR2 3 RR_ERROR_TYPE_STR)
                string(CONFIGURE "${M}" o_str1 @ONLY)
            elseif(RR_ERROR_M STREQUAL "M2")
                string(CONFIGURE "${M2}" o_str1 @ONLY)
            elseif(RR_ERROR_M STREQUAL "MOVERRIDE")
                list(GET RR_ERROR2 3 RR_ERROR_TYPE_STR)
                string(CONFIGURE "${MOVERRIDE}" o_str1 @ONLY)
            else()
                message(FATAL_ERROR "Error generating SWIG error files - ${RR_ERROR_M}")
            endif()
            set(o "${o}${o_str1}")
        endif()
    endforeach()
    set(${txt_out} ${o} PARENT_SCOPE)
endfunction()
