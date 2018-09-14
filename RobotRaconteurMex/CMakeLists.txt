set(BUILD_MATLAB_MEX OFF CACHE BOOL OFF)

IF(WIN32)
IF (RobotRaconteur_USE_SHARED_CORE_LIB)
ADD_DEFINITIONS(-DROBOTRACONTEUR_CORE_IMPORTS)
ENDIF()
ENDIF()

if (CMAKE_SIZEOF_VOID_P EQUAL 8)
	set(win64 true)
endif()

if(CMAKE_GENERATOR STREQUAL Xcode)
string(REPLACE "i386" "" CMAKE_OSX_ARCHITECTURES ${CMAKE_OSX_ARCHITECTURES})
set(CMAKE_SHARED_LINKER_FLAGS ${CMAKE_SHARED_LINKER_FLAGS} -Wl,-exported_symbols_list,${CMAKE_CURRENT_SOURCE_DIR}/osx_mex_exports.txt)
endif()

if(CMAKE_COMPILER_IS_GNUCXX)
set(CMAKE_SHARED_LINKER_FLAGS ${CMAKE_SHARED_LINKER_FLAGS} -Wl,--exclude-libs,ALL)
endif()

INCLUDE(FindMatlab)
FIND_PACKAGE(Matlab COMPONENTS MX_LIBRARY REQUIRED)

ADD_DEFINITIONS(-DMATLAB_MEX_FILE -DMATLAB_DEFAULT_RELEASE=R2017b)

#INCLUDE(BuildMex.cmake)

INCLUDE_DIRECTORIES(${Matlab_INCLUDE_DIRS})

matlab_add_mex(NAME RobotRaconteurMex SRC RobotRaconteurMex.cpp RobotRaconteurMex.h OUTPUT_NAME RobotRaconteurMex LINK_TO ${RobotRaconteur_CORE_LIBRARY} ${Boost_LIBRARIES} ${Matlab_LIBRARIES} ${ROBOTRACONTEUR_EXTRA_LIBS})

FILE(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/out/Matlab")
FILE(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/out_debug/Matlab")
FILE(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/out_reldebug/Matlab")

RRSetTargetDirs(RobotRaconteurMex "Matlab" "Matlab")

add_custom_command(TARGET RobotRaconteurMex PRE_BUILD COMMAND "${CMAKE_COMMAND}" -E make_directory $<TARGET_FILE_DIR:RobotRaconteurMex>)

add_custom_command(TARGET RobotRaconteurMex POST_BUILD COMMAND "${CMAKE_COMMAND}" -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/@RobotRaconteur  $<TARGET_FILE_DIR:RobotRaconteurMex>/@RobotRaconteur)
add_custom_command(TARGET RobotRaconteurMex POST_BUILD COMMAND "${CMAKE_COMMAND}" -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/@RobotRaconteurObject $<TARGET_FILE_DIR:RobotRaconteurMex>/@RobotRaconteurObject)
add_custom_command(TARGET RobotRaconteurMex POST_BUILD COMMAND "${CMAKE_COMMAND}" -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/@RobotRaconteurEventListener $<TARGET_FILE_DIR:RobotRaconteurMex>/@RobotRaconteurEventListener)
add_custom_command(TARGET RobotRaconteurMex POST_BUILD COMMAND "${CMAKE_COMMAND}" -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/@RobotRaconteurMemoryClient $<TARGET_FILE_DIR:RobotRaconteurMex>/@RobotRaconteurMemoryClient)
add_custom_command(TARGET RobotRaconteurMex POST_BUILD COMMAND "${CMAKE_COMMAND}" -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/@RobotRaconteurRate $<TARGET_FILE_DIR:RobotRaconteurMex>/@RobotRaconteurRate)
add_custom_command(TARGET RobotRaconteurMex POST_BUILD COMMAND "${CMAKE_COMMAND}" -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/@RobotRaconteurEventData $<TARGET_FILE_DIR:RobotRaconteurMex>/@RobotRaconteurData)
add_custom_command(TARGET RobotRaconteurMex POST_BUILD COMMAND "${CMAKE_COMMAND}" -E copy ${CMAKE_CURRENT_SOURCE_DIR}/RobotRaconteurObjectServerConnectEvent.m $<TARGET_FILE_DIR:RobotRaconteurMex>/RobotRaconteurObjectServerConnectEvent.m)
add_custom_command(TARGET RobotRaconteurMex POST_BUILD COMMAND "${CMAKE_COMMAND}" -E copy ${CMAKE_CURRENT_SOURCE_DIR}/RobotRaconteurObjectServerDispatchEvent.m $<TARGET_FILE_DIR:RobotRaconteurMex>/RobotRaconteurObjectServerDispatchEvent.m)



