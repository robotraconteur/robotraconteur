if(BUILD_DOCUMENTATION)
    find_program(SPHINX_EXECUTABLE NAMES sphinx-build)

    if(NOT SPHINX_EXECUTABLE)
        message(WARNING "Sphinx is required to build Getting Started guide")
    else()
        file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/docs/getting_started)

        add_custom_target(
            RobotRaconteurGettingStarted_doc
            COMMAND ${SPHINX_EXECUTABLE} -c ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/source
                    ${CMAKE_BINARY_DIR}/docs/getting_started
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Generating Getting Started documentation with Sphinx"
            VERBATIM)
    endif()
endif()
