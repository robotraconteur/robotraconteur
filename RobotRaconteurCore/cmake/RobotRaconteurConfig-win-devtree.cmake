
get_filename_component(RobotRaconteur_DEVEL_DIR_ABS "${CMAKE_CURRENT_LIST_DIR}/../" ABSOLUTE)
set(RobotRaconteur_INCLUDE_DIRS "${RobotRaconteur_DEVEL_DIR_ABS}/out/include"  CACHE PATH "")

if(RobotRaconteur_USE_SHARED_CORE_LIB)
set(RobotRaconteur_CORE_LIB_NAME "@RobotRaconteur_SHARED_CORE_LIB_NAME@")
set(RobotRaconteur_CORE_LIB_NAME_RELEASE "${RobotRaconteur_CORE_LIB_NAME}.lib")
set(RobotRaconteur_CORE_LIB_NAME_DEBUG "${RobotRaconteur_CORE_LIB_NAME}-d.lib")
else()
set(RobotRaconteur_CORE_LIB_NAME "libRobotRaconteurCore")
set(RobotRaconteur_CORE_LIB_NAME_RELEASE "${RobotRaconteur_CORE_LIB_NAME}.lib")
set(RobotRaconteur_CORE_LIB_NAME_DEBUG "${RobotRaconteur_CORE_LIB_NAME}.lib")
endif()

if (EXISTS "${RobotRaconteur_DEVEL_DIR_ABS}/out/lib/${RobotRaconteur_CORE_LIB_NAME_RELEASE}")
set(RobotRaconteur_LIBRARY_RELEASE "${RobotRaconteur_DEVEL_DIR_ABS}/out/lib/${RobotRaconteur_CORE_LIB_NAME_RELEASE}" CACHE FILEPATH "")
else()
string(REPLACE "debug" "release" RobotRaconteur_LIBRARY_RELEASE_NMAKE_ABS "${RobotRaconteur_DEVEL_DIR_ABS}/out/lib/${RobotRaconteur_CORE_LIB_NAME_RELEASE}")
if (EXISTS "${RobotRaconteur_LIBRARY_RELEASE_NMAKE_ABS}")
set(RobotRaconteur_LIBRARY_RELEASE "${RobotRaconteur_LIBRARY_RELEASE_NMAKE_ABS}" CACHE FILEPATH "")
endif()
endif()

if (EXISTS "${RobotRaconteur_DEVEL_DIR_ABS}/out_debug/lib/${RobotRaconteur_CORE_LIB_NAME_DEBUG}")
set(RobotRaconteur_LIBRARY_DEBUG "${RobotRaconteur_DEVEL_DIR_ABS}/out_debug/lib/${RobotRaconteur_CORE_LIB_NAME_DEBUG}" CACHE FILEPATH "")
else()
string(REPLACE "release" "debug" RobotRaconteur_LIBRARY_DEBUG_NMAKE_ABS "${RobotRaconteur_DEVEL_DIR_ABS}/out_debug/lib/${RobotRaconteur_CORE_LIB_NAME_DEBUG}")
if (EXISTS "${RobotRaconteur_LIBRARY_DEBUG_NMAKE_ABS}")
set(RobotRaconteur_LIBRARY_DEBUG "${RobotRaconteur_LIBRARY_DEBUG_NMAKE_ABS}" CACHE FILEPATH "")
endif()
endif()

include(${CMAKE_CURRENT_LIST_DIR}/../cmake/RobotRaconteurConfig.cmake)

set(RobotRaconteur_GEN "${RobotRaconteur_DEVEL_DIR_ABS}/out/bin/RobotRaconteurGen")

include(${CMAKE_CURRENT_LIST_DIR}/RobotRaconteurGenerateThunk.cmake)
