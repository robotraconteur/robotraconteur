find_package(SWIG REQUIRED)

function(package_source_swig_net SWIG_NET_EXTRA_ARGS SWIG_NET_OUTDIR outfiles)
include(${CMAKE_SOURCE_DIR}/RobotRaconteurNET/NETSwigVars.cmake)
add_custom_command(
    TARGET package_source_swig
	COMMAND "${CMAKE_COMMAND}" -E make_directory ${SWIG_NET_OUTDIR}
	COMMAND "${CMAKE_COMMAND}" -E env "SWIG_LIB=${SWIG_DIR}" 
	"${SWIG_EXECUTABLE}" -csharp -namespace RobotRaconteur 
	-DSWIG2_CSHARP -outfile RobotRaconteurNET_SWIG.cs 
	-outdir ${SWIG_NET_OUTDIR} -c++ -dllimport RobotRaconteurNETNative 
	-I${CMAKE_SOURCE_DIR}/RobotRaconteurCore/include -I${CMAKE_SOURCE_DIR} 
	-I${CMAKE_SOURCE_DIR}/SWIG -o ${SWIG_NET_OUTDIR}/RobotRaconteurNETCSHARP_wrap.cxx 
	${SWIG_NET_EXTRA_ARGS}
	${CMAKE_SOURCE_DIR}/RobotRaconteurNET/RobotRaconteurNET.i)
set(${outfiles} ${SWIG_NET_OUTDIR}/RobotRaconteurNETCSHARP_wrap.cxx PARENT_SCOPE)
endfunction()

function(package_source_swig_java SWIG_JAVA_EXTRA_ARGS SWIG_JAVA_OUTDIR outfiles)
include(${CMAKE_SOURCE_DIR}/RobotRaconteurJava/JavaSwigVars.cmake)
add_custom_command(
    TARGET package_source_swig
	COMMAND "${CMAKE_COMMAND}" -E make_directory ${SWIG_JAVA_OUTDIR}
	COMMAND "${CMAKE_COMMAND}" -E env "SWIG_LIB=${SWIG_DIR}" 
	"${SWIG_EXECUTABLE}" -java -package com.robotraconteur
	-outdir ${SWIG_JAVA_OUTDIR} -c++
	-I${CMAKE_SOURCE_DIR}/RobotRaconteurCore/include -I${CMAKE_SOURCE_DIR} 
	-I${CMAKE_SOURCE_DIR}/SWIG -o ${SWIG_JAVA_OUTDIR}/RobotRaconteurJavaJAVA_wrap.cxx 
	${SWIG_JAVA_EXTRA_ARGS}
	${CMAKE_SOURCE_DIR}/RobotRaconteurJava/RobotRaconteurJava.i)
set(${outfiles} ${SWIG_NET_OUTDIR}/RobotRaconteurJavaJAVA_wrap.cxx PARENT_SCOPE)
endfunction()
 
function(package_source_swig_python SWIG_PYTHON_EXTRA_ARGS SWIG_PYTHON_OUTDIR outfiles)
include(${CMAKE_SOURCE_DIR}/RobotRaconteurPython/PythonSwigVars.cmake)
add_custom_command(
    TARGET package_source_swig
	COMMAND "${CMAKE_COMMAND}" -E make_directory ${SWIG_PYTHON_OUTDIR}
	COMMAND "${CMAKE_COMMAND}" -E env "SWIG_LIB=${SWIG_DIR}" 
	"${SWIG_EXECUTABLE}" -python -relativeimport
	-outdir ${SWIG_PYTHON_OUTDIR} -c++
	-I${CMAKE_SOURCE_DIR}/RobotRaconteurCore/include -I${CMAKE_SOURCE_DIR}
	-I${CMAKE_SOURCE_DIR}/RobotRaconteurPython
	-I${CMAKE_SOURCE_DIR}/SWIG -o ${SWIG_PYTHON_OUTDIR}/RobotRaconteurPythonPYTHON_wrap.cxx 
	${SWIG_PYTHON_EXTRA_ARGS}
	${CMAKE_SOURCE_DIR}/RobotRaconteurPython/RobotRaconteurPython.i)
set(${outfiles} ${SWIG_PYTHON_OUTDIR}/RobotRaconteurPythonPYTHON.cxx PARENT_SCOPE)
endfunction()

set(PACKAGE_SWIG_SOURCE_ALL OFF CACHE BOOL "Always build swig source")
if(PACKAGE_SWIG_SOURCE_ALL)
add_custom_target(package_source_swig ALL)
else()
add_custom_target(package_source_swig)
set_target_properties(package_source_swig PROPERTIES EXCLUDE_FROM_ALL 1 EXCLUDE_FROM_DEFAULT_BUILD 1)
endif()

package_source_swig_net("" ${CMAKE_BINARY_DIR}/generated_src/NET/swigwordsize32 net_sources_32)
package_source_swig_java("" ${CMAKE_BINARY_DIR}/generated_src/Java/swigwordsize32/com/robotraconteur java_sources_32)
package_source_swig_python("" ${CMAKE_BINARY_DIR}/generated_src/Python/swigwordsize32 python_sources_32)
package_source_swig_python("" ${CMAKE_BINARY_DIR}/generated_src/Python3/swigwordsize32 python3_sources_32)
package_source_swig_net("-DSWIGWORDSIZE64" ${CMAKE_BINARY_DIR}/generated_src/NET/swigwordsize64 net_sources_64)
package_source_swig_java("-DSWIGWORDSIZE64" ${CMAKE_BINARY_DIR}/generated_src/Java/swigwordsize64/com/robotraconteur java_sources_64)
package_source_swig_python("-DSWIGWORDSIZE64" ${CMAKE_BINARY_DIR}/generated_src/Python/swigwordsize64 python_sources_64)
package_source_swig_python("-DSWIGWORDSIZE64" ${CMAKE_BINARY_DIR}/generated_src/Python3/swigwordsize64 python3_sources_64)
