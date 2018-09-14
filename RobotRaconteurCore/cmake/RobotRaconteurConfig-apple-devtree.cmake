
get_filename_component(RobotRaconteur_DEVEL_DIR_ABS "${CMAKE_CURRENT_LIST_DIR}/../" ABSOLUTE)
set(RobotRaconteur_INCLUDE_DIRS "${RobotRaconteur_DEVEL_DIR_ABS}/out/include"  CACHE PATH "")

if (EXISTS "${RobotRaconteur_DEVEL_DIR_ABS}/out/lib/libRobotRaconteurCore.a" AND (EXISTS "${RobotRaconteur_DEVEL_DIR_ABS}/out_debug/lib/libRobotRaconteurCore.a") )
set(RobotRaconteur_LIBRARY "${RobotRaconteur_DEVEL_DIR_ABS}/out/lib/libRobotRaconteurCore.a" CACHE FILEPATH "")
elseif(EXISTS "${RobotRaconteur_DEVEL_DIR_ABS}/out/lib/libRobotRaconteurCore.a")
set(RobotRaconteur_LIBRARY "${RobotRaconteur_DEVEL_DIR_ABS}/out/lib/libRobotRaconteurCore.a" CACHE FILEPATH "")
elseif(EXISTS "${RobotRaconteur_DEVEL_DIR_ABS}/out_debug/lib/libRobotRaconteurCore.a")
set(RobotRaconteur_LIBRARY "${RobotRaconteur_DEVEL_DIR_ABS}/out_debug/lib/libRobotRaconteurCore.a" CACHE FILEPATH "")
endif()

include(${CMAKE_CURRENT_LIST_DIR}/../cmake/RobotRaconteurConfig.cmake)

set(RobotRaconteur_GEN "${RobotRaconteur_DEVEL_DIR_ABS}/out/bin/RobotRaconteurGen")

include(${CMAKE_CURRENT_LIST_DIR}/RobotRaconteurGenerateThunk.cmake)
