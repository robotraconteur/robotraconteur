function(ROBOTRACONTEUR_GENERATE_THUNK SRCS)
    cmake_parse_arguments(RR_GEN "MASTER_HEADER;AUTO_IMPORT" "LANG;OUTDIR;MASTER_HEADER_FILENAME"
                          "INCLUDE_DIRS;IMPORT_DIRS;IMPORTS;CPP_EXTRA_INCLUDE" ${ARGN})

    if("${RR_GEN_LANG}" STREQUAL "")
        set(RR_GEN_LANG "cpp")
    endif()

    set(RR_GEN_FILES)
    set(THUNK_SRCS)
    set(THUNK_HDRS)

    set(RR_SERVICE_NAMES)

    if(RR_GEN_LANG STREQUAL "cpp")
        list(GET RR_GEN_UNPARSED_ARGUMENTS 0 HDRS)
        list(REMOVE_AT RR_GEN_UNPARSED_ARGUMENTS 0)
    endif()

    list(APPEND RR_GEN_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}")

    if(RR_GEN_IMPORT_DIRS)
        message(DEPRECATION "IMPORT_DIRS argument is deprecated, use INCLUDE_DIRS instead")
        foreach(f IN LISTS RR_GEN_IMPORT_DIRS)
            list(APPEND RR_GEN_INCLUDE_DIRS ${f})
        endforeach()
    endif()

    if(NOT RR_GEN_OUTDIR)
        set(RR_GEN_OUTDIR "${CMAKE_CURRENT_BINARY_DIR}")
    endif()

    foreach(f1 IN LISTS RR_GEN_UNPARSED_ARGUMENTS)
        set(f)
        if(EXISTS "${f1}")
            set(f "${f1}")
        else()
            foreach(p IN LISTS RR_GEN_INCLUDE_DIRS)
                if(EXISTS "${p}/${f1}")
                    set(f "${p}/${f1}")
                    break()
                endif()
            endforeach()
            if(NOT EXISTS "${f}")
                message(FATAL_ERROR "ROBOTRACONTEUR_GENERATE_THUNK could not find file ${f1}")
            endif()
        endif()

        file(STRINGS "${f}" f_service_name1 REGEX "^[ \t]*service +([A-Za-z0-9\\.]+)" LIMIT_COUNT 1)
        if("${f_service_name1}" STREQUAL "")
            message(FATAL_ERROR "Could not determine service name for ${f}")
        endif()
        string(REGEX REPLACE "^[ \t]*service +([A-Za-z0-9\\.]+)" "\\1" f_service_name ${f_service_name1})
        if("${f_service_name}" STREQUAL "")
            message(FATAL_ERROR "Could not determine service name for ${f}")
        endif()

        string(REPLACE "." "__" f_service_name2 ${f_service_name})

        list(APPEND RR_SERVICE_NAMES ${f_service_name})
        list(APPEND RR_GEN_FILES ${f})
        if(RR_GEN_LANG STREQUAL "cpp")
            list(APPEND THUNK_SRCS "${RR_GEN_OUTDIR}/${f_service_name2}_stubskel.cpp")
            list(APPEND THUNK_HDRS "${RR_GEN_OUTDIR}/${f_service_name2}.h")
            list(APPEND THUNK_HDRS "${RR_GEN_OUTDIR}/${f_service_name2}_stubskel.h")
        elseif(RR_GEN_LANG STREQUAL "csharp")
            list(APPEND THUNK_SRCS "${RR_GEN_OUTDIR}/${f_service_name2}.cs")
            list(APPEND THUNK_SRCS "${RR_GEN_OUTDIR}/${f_service_name2}_stubskel.cs")
        elseif(RR_GEN_LANG STREQUAL "java")
            string(REPLACE "." "/" f_service_name3 ${f_service_name})
            list(APPEND THUNK_SRCS "${RR_GEN_OUTDIR}/${f_service_name3}/${f_service_name2}Factory.java")
        else()
            message(FATAL_ERROR "Unknown RobotRaconteurGen language")
        endif()

    endforeach()

    message(STATUS "RobotRaconteurGen: generating thunk code for services: ${RR_SERVICE_NAMES}")

    set_source_files_properties(${THUNK_SRCS} ${THUNK_HDRS} PROPERTIES GENERATED TRUE)
    set(${SRCS} ${THUNK_SRCS} PARENT_SCOPE)
    if(RR_GEN_LANG STREQUAL "cpp")
        set(${HDRS} ${THUNK_HDRS} PARENT_SCOPE)
    endif()

    if(RR_GEN_OUTDIR)
        set(RR_GEN_OUTDIR_ARGS --output-dir ${RR_GEN_OUTDIR})
    endif()

    if(RR_GEN_MASTER_HEADER)
        if(NOT RR_GEN_MASTER_HEADER_FILENAME)
            set(RR_GEN_MASTER_HEADER_CMD --master-header "robotraconteur_generated.h")
        else()
            set(RR_GEN_MASTER_HEADER_CMD --master-header "${RR_GEN_MASTER_HEADER_FILENAME}")
        endif()
    endif()

    set(GEN_IMPORTS_ARGS)
    if(RR_GEN_IMPORTS)
        foreach(f1 IN LISTS RR_GEN_IMPORTS)
            set(f)
            if(EXISTS "${f1}")
                list(APPEND GEN_IMPORT_ARGS "--import=${f1}")
            else()
                foreach(p IN LISTS RR_GEN_INCLUDE_DIRS)
                    if(EXISTS "${p}/${f1}")
                        set(f "${p}/${f1}")
                        break()
                    endif()
                endforeach()
                if(NOT EXISTS "${f}")
                    message(FATAL_ERROR "ROBOTRACONTEUR_GENERATE_THUNK could not find file ${f1}")
                endif()
                list(APPEND GEN_IMPORT_ARGS "--import=${f}")
            endif()
        endforeach()
    endif()

    set(GEN_CPP_EXTRA_INCLUDE_ARGS)
    if(RR_GEN_CPP_EXTRA_INCLUDE)
        foreach(f1 IN LISTS RR_GEN_CPP_EXTRA_INCLUDE)
            list(APPEND GEN_CPP_EXTRA_INCLUDE_ARGS "--cpp-extra-include=${f1}")
        endforeach()
    endif()

    set(GEN_INCLUDE_DIRS_ARGS)
    if(RR_GEN_INCLUDE_DIRS)
        foreach(f1 IN LISTS RR_GEN_INCLUDE_DIRS)
            list(APPEND GEN_INCLUDE_DIRS_ARGS "--include-path=${f1}")
        endforeach()
    endif()

    set(GEN_AUTO_IMPORT_ARGS)
    if(RR_GEN_AUTO_IMPORT)
        set(GEN_AUTO_IMPORT_ARGS "--auto-import")
    endif()

    if(NOT RobotRaconteurGen_EXECUTABLE)
        if(NOT TARGET RobotRaconteurGen)
            message(FATAL_ERROR "RobotRaconteurGen not found")
        endif()
        set(RobotRaconteurGen_EXECUTABLE "$<TARGET_FILE:RobotRaconteurGen>")
        set(RR_GEN_DEPENDS RobotRaconteurGen)
    endif()

    add_custom_command(
        OUTPUT ${THUNK_SRCS} ${THUNK_HDRS}
        COMMAND ${CMAKE_COMMAND} -E make_directory ${RR_GEN_OUTDIR}
        COMMAND
            ${RobotRaconteurGen_EXECUTABLE} ARGS "--thunksource" "--lang=${RR_GEN_LANG}" ${RR_GEN_MASTER_HEADER_CMD}
            ${GEN_INCLUDE_DIRS_ARGS} ${RR_GEN_OUTDIR_ARGS} ${GEN_IMPORT_ARGS} ${GEN_AUTO_IMPORT_ARGS}
            ${GEN_CPP_EXTRA_INCLUDE_ARGS} ${RR_GEN_FILES}
        DEPENDS ${RR_GEN_FILES} ${RR_GEN_DEPENDS}
        COMMENT "Running RobotRaconteurGen for ${RR_SERVICE_NAMES}"
        VERBATIM)

endfunction()
