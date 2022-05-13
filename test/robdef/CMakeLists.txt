set(ROBOTRACONTEUR_TEST_ROBDEF_DIR "${CMAKE_BINARY_DIR}/test/robdef")

set(TEST_ROBDEFS
    "com.robotraconteur.testing.TestService1.robdef" "com.robotraconteur.testing.TestService2.robdef"
    "com.robotraconteur.testing.TestService3.robdef" "com.robotraconteur.testing.TestService4.robdef"
    "com.robotraconteur.testing.TestService5.robdef")

foreach(f IN LISTS TEST_ROBDEFS)
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/${f}" "${ROBOTRACONTEUR_TEST_ROBDEF_DIR}/${f}" COPYONLY)
endforeach()

set(ROBOTRACONTEUR_TEST_ROBDEF_DIR ${ROBOTRACONTEUR_TEST_ROBDEF_DIR} PARENT_SCOPE)
