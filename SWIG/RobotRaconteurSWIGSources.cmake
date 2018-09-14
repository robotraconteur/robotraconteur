
#List of sources to use as dependencies for SWIG generation
set(RobotRaconteur_SWIG_sources
    ${CMAKE_CURRENT_LIST_DIR}/AsyncHandlerDirector.i
	${CMAKE_CURRENT_LIST_DIR}/Client.i
	${CMAKE_CURRENT_LIST_DIR}/CommonInclude.i
	${CMAKE_CURRENT_LIST_DIR}/DataTypes.i
	${CMAKE_CURRENT_LIST_DIR}/Discovery.i
	${CMAKE_CURRENT_LIST_DIR}/HardwareTransport.i
	${CMAKE_CURRENT_LIST_DIR}/LocalTransport.i
	${CMAKE_CURRENT_LIST_DIR}/MemoryMember.i
	${CMAKE_CURRENT_LIST_DIR}/Message.i
	${CMAKE_CURRENT_LIST_DIR}/NodeID.i
	${CMAKE_CURRENT_LIST_DIR}/PipeMember.i
	${CMAKE_CURRENT_LIST_DIR}/RobotRaconteurConstants.i
	${CMAKE_CURRENT_LIST_DIR}/RobotRaconteurNode.i
	${CMAKE_CURRENT_LIST_DIR}/Service.i
	${CMAKE_CURRENT_LIST_DIR}/ServiceDefinition.i
	${CMAKE_CURRENT_LIST_DIR}/ServiceSecurity.i
	${CMAKE_CURRENT_LIST_DIR}/Subscription.i
	${CMAKE_CURRENT_LIST_DIR}/TcpTransport.i
	${CMAKE_CURRENT_LIST_DIR}/Timer.i
	${CMAKE_CURRENT_LIST_DIR}/Transport.i
	${CMAKE_CURRENT_LIST_DIR}/WireMember.i	
)
