# Runs RobotRaconteurGen on a robdef whose doc lines contain comment terminators and checks that no generated comment
# line is ended early by the doc text. Usage: cmake -DROBOTRACONTEURGEN=<exe> -DROBDEF=<file> -DOUT_DIR=<dir> -P
# check_docstring.cmake

file(REMOVE_RECURSE "${OUT_DIR}")
foreach(lang cpp java)
    file(MAKE_DIRECTORY "${OUT_DIR}/${lang}")
    execute_process(COMMAND "${ROBOTRACONTEURGEN}" --thunksource --lang=${lang} "--output-dir=${OUT_DIR}/${lang}"
                            "${ROBDEF}" RESULT_VARIABLE res)
    if(NOT res EQUAL 0)
        message(FATAL_ERROR "RobotRaconteurGen --lang=${lang} failed: ${res}")
    endif()
    file(GLOB_RECURSE gen_files "${OUT_DIR}/${lang}/*")
    set(doc_lines 0)
    foreach(f IN LISTS gen_files)
        file(STRINGS "${f}" lines REGEX "rr_injected_")
        foreach(l IN LISTS lines)
            # Lines starting with '*' are doc comment lines, others are the robdef string literal
            if(l MATCHES "^[ \t]*\\*")
                math(EXPR doc_lines "${doc_lines} + 1")
                # javac also decodes \uXXXX escapes inside comments
                if(l MATCHES "\\*/" OR (lang STREQUAL "java" AND l MATCHES "\\\\u"))
                    message(FATAL_ERROR "Doc line can end the comment in ${f}: ${l}")
                endif()
            endif()
        endforeach()
    endforeach()
    if(doc_lines LESS 3)
        message(FATAL_ERROR "Expected doc comment lines not found in ${lang} output (found ${doc_lines})")
    endif()
endforeach()
