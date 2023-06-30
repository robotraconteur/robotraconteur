include(vcpkg_common_functions)

vcpkg_configure_cmake(
    SOURCE_PATH
    ${CMAKE_CURRENT_LIST_DIR}/../../..
    OPTIONS
    -DBUILD_GEN=OFF
    -DBUILD_TEST=OFF
    -DBUILD_TESTING=OFF
    -DBUILD_SHARED_LIBS=OFF
    -DOPENSSL_USE_STATIC_LIBS=ON)

vcpkg_install_cmake()

vcpkg_fixup_cmake_targets(CONFIG_PATH "lib/cmake/RobotRaconteur")

vcpkg_copy_pdbs()

file(REMOVE_RECURSE ${CURRENT_PACKAGES_DIR}/bin/)
file(REMOVE_RECURSE ${CURRENT_PACKAGES_DIR}/debug/bin)
file(REMOVE_RECURSE ${CURRENT_PACKAGES_DIR}/debug/include)
file(REMOVE_RECURSE ${CURRENT_PACKAGES_DIR}/debug/share)
