// DataTypes Enum
%feature("docstring") DataTypes_void_t  "void or null type";
%feature("docstring") DataTypes_double_t "IEEE-754 64-bit floating point number";
%feature("docstring") DataTypes_single_t "IEEE-754 32-bit floating point number";
%feature("docstring") DataTypes_int8_t "8-bit signed integer";
%feature("docstring") DataTypes_uint8_t "8-bit unsigned integer";
%feature("docstring") DataTypes_int16_t "16-bit signed integer";
%feature("docstring") DataTypes_uint16_t "16-bit unsigned integer";
%feature("docstring") DataTypes_int32_t "32-bit signed integer";
%feature("docstring") DataTypes_uint32_t "32-bit unsigned integer";
%feature("docstring") DataTypes_int64_t "64-bit signed integer";
%feature("docstring") DataTypes_uint64_t "64-bit unsigned integer";
%feature("docstring") DataTypes_string_t "UTF-8 string";
%feature("docstring") DataTypes_cdouble_t "128-bit complex double (real,imag)";
%feature("docstring") DataTypes_csingle_t "64-bit complex float (real,imag)";
%feature("docstring") DataTypes_bool_t "8-bit boolean";
%feature("docstring") DataTypes_structure_t  "structure (nested message type)";
%feature("docstring") DataTypes_vector_t "map with int32 key (nested message type)";
%feature("docstring") DataTypes_dictionary_t "map with string key (nested message type)";
%feature("docstring") DataTypes_object_t "object type (not serializable)";
%feature("docstring") DataTypes_varvalue_t "varvalue type (not serializable)";
%feature("docstring") DataTypes_varobject_t "varobject type (not serializable)";
%feature("docstring") DataTypes_list_t "list type (nested message type)";
%feature("docstring") DataTypes_pod_t "pod type (nested message type)";
%feature("docstring") DataTypes_pod_array_t "pod array type (nested message type)";
%feature("docstring") DataTypes_pod_multidimarray_t "pod multidimarray type (nested message type)";
%feature("docstring") DataTypes_enum_t "enum type (not serializable uses int32 for messages)";
%feature("docstring") DataTypes_namedtype_t "namedtype definition (not serializable)";
%feature("docstring") DataTypes_namedarray_t "namedarray type (not serializable)";
%feature("docstring") DataTypes_namedarray_array_t "namedarray array type (nested message type)";
%feature("docstring") DataTypes_namedarray_multidimarray_t "namedarray multidimarray type (nested message type)";
%feature("docstring") DataTypes_multidimarray_t "multi-dimensional numeric array (nested message type)";

// DataTypes_ArrayTypes Enum
%feature("docstring") DataTypes_ArrayTypes_none "type is not an array";
%feature("docstring") DataTypes_ArrayTypes_array "type is a single dimensional array";      
%feature("docstring") DataTypes_ArrayTypes_multidimarray "type is a multidimensional array";


// DataTypes_ContainerTypes
%feature("docstring") DataTypes_ContainerTypes_none "type does not have a container";
%feature("docstring") DataTypes_ContainerTypes_list "type has a list container";
%feature("docstring") DataTypes_ContainerTypes_map_int32 "type has a map with int32 keys container";
%feature("docstring") DataTypes_ContainerTypes_map_string "type has a map with string keys container";
%feature("docstring") DataTypes_ContainerTypes_generator "type has a generator container. Only valid for use with function generator members";

// MessageFlags
%feature("docstring") MessageFlags_ROUTING_INFO "Message contains ROUTING_INFO section";
%feature("docstring") MessageFlags_ENDPOINT_INFO "Message contains ENDPOINT_INFO section";
%feature("docstring") MessageFlags_PRIORITY "Message contains PRIORITY section";
%feature("docstring") MessageFlags_UNRELIABLE "Message is unreliable and may be dropped";
%feature("docstring") MessageFlags_META_INFO "Message contains META_INFO section";
%feature("docstring") MessageFlags_STRING_TABLE "Message contains STRING_TABLE section";
%feature("docstring") MessageFlags_MULTIPLE_ENTRIES "Message contains MULTIPLE_ENTRIES section. If unset, message contains one entry";
%feature("docstring") MessageFlags_EXTENDED "Message contains EXTENDED section";
%feature("docstring") MessageFlags_Version2Compat "Message flags for compatibility with Message Format Version 2";
%feature("docstring") MessageEntryFlags_SERVICE_PATH_STR "MessageEntry contains SERVICE_PATH_STR section";
%feature("docstring") MessageEntryFlags_SERVICE_PATH_CODE "MessageEntry contains SERVICE_PATH_CODE section";
%feature("docstring") MessageEntryFlags_MEMBER_NAME_STR "MessageEntry contains MEMBER_NAME_STR section";
%feature("docstring") MessageEntryFlags_MEMBER_NAME_CODE "MessageEntry contains MEMBER_NAME_CODE section";
%feature("docstring") MessageEntryFlags_REQUEST_ID "MessageEntry contains REQUEST_ID section";
%feature("docstring") MessageEntryFlags_ERROR "MessageEntry contains ERROR section";
%feature("docstring") MessageEntryFlags_META_INFO "MessageEntry contains META_INFO section";
%feature("docstring") MessageEntryFlags_EXTENDED "MessageEntry contains EXTENDED section";
%feature("docstring") MessageEntryFlags_Version2Compat "MessageEntry flags for compatibility with Message Format Version 2";
%feature("docstring") MessageElementFlags_ELEMENT_NAME_STR "MessageElement contains ELEMENT_NAME_STR section";
%feature("docstring") MessageElementFlags_ELEMENT_NAME_CODE "MessageElement contains ELEMENT_NAME_CODE section";
%feature("docstring") MessageElementFlags_ELEMENT_NUMBER "MessageElement contains ELEMENT_NUMBER section";
%feature("docstring") MessageElementFlags_ELEMENT_TYPE_NAME_STR "MessageElement contains ELEMENT_TYPE_NAME_STR section";
%feature("docstring") MessageElementFlags_ELEMENT_TYPE_NAME_CODE "MessageElement contains ELEMENT_TYPE_NAME_CODE section";
%feature("docstring") MessageElementFlags_META_INFO "MessageElement contains META_INFO section";
%feature("docstring") MessageElementFlags_EXTENDED "MessageElement contains EXTENDED section";
%feature("docstring") MessageElementFlags_Version2Compat "MessageElement flags for compatibility with Message Format Version 2";

// MessageEntryType Enum
%feature("docstring") MessageEntryType_Null "no-op";
%feature("docstring") MessageEntryType_StreamOp "Stream operation request (transport only)";
%feature("docstring") MessageEntryType_StreamOpRet "Stream operation response (transport only)";
%feature("docstring") MessageEntryType_StreamCheckCapability "Stream check capability request (transport only)";    
%feature("docstring") MessageEntryType_StreamCheckCapabilityRet "Stream check capability response (transport only)";
%feature("docstring") MessageEntryType_GetServiceDesc "Get service definition request";
%feature("docstring") MessageEntryType_GetServiceDescRet "Get service definition response";
%feature("docstring") MessageEntryType_ObjectTypeName "Get object qualified type name request";
%feature("docstring") MessageEntryType_ObjectTypeNameRet "Get object qualified type name response";
%feature("docstring") MessageEntryType_ServiceClosed "Service closed notification packet";
%feature("docstring") MessageEntryType_ServiceClosedRet "(reserved)";
%feature("docstring") MessageEntryType_ConnectClient "Connect client request";
%feature("docstring") MessageEntryType_ConnectClientRet "Connect client response";
%feature("docstring") MessageEntryType_DisconnectClient "Disconnect client request";
%feature("docstring") MessageEntryType_DisconnectClientRet "Disconnect client response";
%feature("docstring") MessageEntryType_ConnectionTest "Ping request";
%feature("docstring") MessageEntryType_ConnectionTestRet "Pong response";
%feature("docstring") MessageEntryType_GetNodeInfo "Get node information request (NodeID and NodeName)";
%feature("docstring") MessageEntryType_GetNodeInfoRet "Get node information response";
%feature("docstring") MessageEntryType_ReconnectClient "(reserved)";
%feature("docstring") MessageEntryType_ReconnectClientRet "(reserved)";
%feature("docstring") MessageEntryType_NodeCheckCapability "Get node capability request";
%feature("docstring") MessageEntryType_NodeCheckCapabilityRet "Get node capability response";
%feature("docstring") MessageEntryType_GetServiceAttributes "Get service attributes request";
%feature("docstring") MessageEntryType_GetServiceAttributesRet "Get service attributes response";
%feature("docstring") MessageEntryType_ConnectClientCombined "Connect client combined operation request";
%feature("docstring") MessageEntryType_ConnectClientCombinedRet "Connect client combined operation response";
%feature("docstring") MessageEntryType_EndpointCheckCapability "Get endpoint capability request";
%feature("docstring") MessageEntryType_EndpointCheckCapabilityRet "Get endpoint capabalitiy response";
%feature("docstring") MessageEntryType_ServiceCheckCapabilityReq "Get service capability request";
%feature("docstring") MessageEntryType_ServiceCheckCapabilityRet "Get service capability response";
%feature("docstring") MessageEntryType_ClientKeepAliveReq "Client keep alive request";
%feature("docstring") MessageEntryType_ClientKeepAliveRet "Client keep alive response";
%feature("docstring") MessageEntryType_ClientSessionOpReq "Client session management operation request";
%feature("docstring") MessageEntryType_ClientSessionOpRet "Client session management operation response";
%feature("docstring") MessageEntryType_ServicePathReleasedReq "Service path released event notification packet";
%feature("docstring") MessageEntryType_ServicePathReleasedRet "(reserved)";
%feature("docstring") MessageEntryType_PropertyGetReq "Property member get request";
%feature("docstring") MessageEntryType_PropertyGetRes "Property member get response";
%feature("docstring") MessageEntryType_PropertySetReq "Property member set request";
%feature("docstring") MessageEntryType_PropertySetRes "Property member set response";
%feature("docstring") MessageEntryType_FunctionCallReq "Function member call request";
%feature("docstring") MessageEntryType_FunctionCallRes "Function member call response";
%feature("docstring") MessageEntryType_GeneratorNextReq "Generater next call request";
%feature("docstring") MessageEntryType_GeneratorNextRes "Generater next call response";
%feature("docstring") MessageEntryType_EventReq "Event member notification";
%feature("docstring") MessageEntryType_EventRes "(reserved)";
%feature("docstring") MessageEntryType_PipePacket "Pipe member packet";
%feature("docstring") MessageEntryType_PipePacketRet "Pipe member packet ack";
%feature("docstring") MessageEntryType_PipeConnectReq "Pipe member connect request";
%feature("docstring") MessageEntryType_PipeConnectRet "Pipe member connect response";
%feature("docstring") MessageEntryType_PipeDisconnectReq "Pipe member close request";
%feature("docstring") MessageEntryType_PipeDisconnectRet "Pipe member close response";
%feature("docstring") MessageEntryType_PipeClosed "Pipe member closed event notification packet";
%feature("docstring") MessageEntryType_PipeClosedRet "(reserved)";
%feature("docstring") MessageEntryType_CallbackCallReq "Callback member call request";
%feature("docstring") MessageEntryType_CallbackCallRet "Callback member call response";
%feature("docstring") MessageEntryType_WirePacket "Wire member value packet";
%feature("docstring") MessageEntryType_WirePacketRet "(reserved)";
%feature("docstring") MessageEntryType_WireConnectReq "Wire member connect request";
%feature("docstring") MessageEntryType_WireConnectRet "Wire member connect response";
%feature("docstring") MessageEntryType_WireDisconnectReq "Wire member close request";
%feature("docstring") MessageEntryType_WireDisconnectRet "Wire member close response";
%feature("docstring") MessageEntryType_WireClosed "Wire member closed event notification packet";
%feature("docstring") MessageEntryType_WireClosedRet "(reserved)";
%feature("docstring") MessageEntryType_MemoryRead "Memory member read request";
%feature("docstring") MessageEntryType_MemoryReadRet "Memory member read response";
%feature("docstring") MessageEntryType_MemoryWrite "Memory member write request";
%feature("docstring") MessageEntryType_MemoryWriteRet "Memory member write response";
%feature("docstring") MessageEntryType_MemoryGetParam "Memory member get param request";
%feature("docstring") MessageEntryType_MemoryGetParamRet "Memory member get param response";
%feature("docstring") MessageEntryType_WirePeekInValueReq "Wire member peek InValue request";
%feature("docstring") MessageEntryType_WirePeekInValueRet "Wire member peek InValue response";
%feature("docstring") MessageEntryType_WirePeekOutValueReq "Wire member peek OutValue request";
%feature("docstring") MessageEntryType_WirePeekOutValueRet "Wire member peek OutValue response";
%feature("docstring") MessageEntryType_WirePokeOutValueReq "Wire member poke OutValue request";
%feature("docstring") MessageEntryType_WirePokeOutValueRet "Wire member poke OutValue response";

// MessageErrorType Enum
%feature("docstring") MessageErrorType_None "success";
%feature("docstring") MessageErrorType_ConnectionError "connection error";
%feature("docstring") MessageErrorType_ProtocolError "protocol error serializing messages";
%feature("docstring") MessageErrorType_ServiceNotFound "specified service not found";
%feature("docstring") MessageErrorType_ObjectNotFound "specified object not found";
%feature("docstring") MessageErrorType_InvalidEndpoint "specified endpoint not found";
%feature("docstring") MessageErrorType_EndpointCommunicationFatalError "communication with specified endpoint failed";
%feature("docstring") MessageErrorType_NodeNotFound "specified node not found";
%feature("docstring") MessageErrorType_ServiceError "service error";
%feature("docstring") MessageErrorType_MemberNotFound "specified member not found";
%feature("docstring") MessageErrorType_MemberFormatMismatch "message format incompatible with specified member";
%feature("docstring") MessageErrorType_DataTypeMismatch "data type did not match expected type";
%feature("docstring") MessageErrorType_DataTypeError "data type failure";
%feature("docstring") MessageErrorType_DataSerializationError "failure serializing data type";
%feature("docstring") MessageErrorType_MessageEntryNotFound "specified message entry not found";
%feature("docstring") MessageErrorType_MessageElementNotFound "specified message element not found";
%feature("docstring") MessageErrorType_UnknownError "unknown exception occurred check `error name`";
%feature("docstring") MessageErrorType_InvalidOperation "invalid operation attempted";
%feature("docstring") MessageErrorType_InvalidArgument "argument is invalid";
%feature("docstring") MessageErrorType_OperationFailed "the requested operation failed";
%feature("docstring") MessageErrorType_NullValue "invalid null value";
%feature("docstring") MessageErrorType_InternalError "internal error";
%feature("docstring") MessageErrorType_SystemResourcePermissionDenied "permission denied to a system resource";
%feature("docstring") MessageErrorType_OutOfSystemResource "system resource has been exhausted";
%feature("docstring") MessageErrorType_SystemResourceError "system resource error";
%feature("docstring") MessageErrorType_ResourceNotFound "a required resource was not found";
%feature("docstring") MessageErrorType_IOError "input/output error";
%feature("docstring") MessageErrorType_BufferLimitViolation "a buffer underrun/overrun has occurred";
%feature("docstring") MessageErrorType_ServiceDefinitionError "service definition parse or validation error";
%feature("docstring") MessageErrorType_OutOfRange "attempt to access an out of range element";
%feature("docstring") MessageErrorType_KeyNotFound "key not found";
%feature("docstring") MessageErrorType_InvalidConfiguration "invalid configuration specified";
%feature("docstring") MessageErrorType_InvalidState "invalid state";
%feature("docstring") MessageErrorType_RemoteError "error occurred on remote node";
%feature("docstring") MessageErrorType_RequestTimeout "request timed out";
%feature("docstring") MessageErrorType_ReadOnlyMember "attempt to write to a read only member";
%feature("docstring") MessageErrorType_WriteOnlyMember "attempt to read a write only member";
%feature("docstring") MessageErrorType_NotImplementedError "member not implemented";
%feature("docstring") MessageErrorType_MemberBusy "member is busy try again";
%feature("docstring") MessageErrorType_ValueNotSet "value has not been set";
%feature("docstring") MessageErrorType_AbortOperation "abort operation (generator only)";
%feature("docstring") MessageErrorType_OperationAborted "the operation has been aborted";
%feature("docstring") MessageErrorType_StopIteration "stop generator iteration (generator only)";
%feature("docstring") MessageErrorType_OperationTimeout "the operation has timed out";
%feature("docstring") MessageErrorType_OperationCancelled "the operation has been cancelled";
%feature("docstring") MessageErrorType_AuthenticationError "authentication has failed";
%feature("docstring") MessageErrorType_ObjectLockedError "the object is locked by another user or session";
%feature("docstring") MessageErrorType_PermissionDenied "permission to service object or resource denied";

// ClientServiceListenerEventType Enum
%feature("docstring") ClientServiceListenerEventType_ClientClosed "client has been closed";
%feature("docstring") ClientServiceListenerEventType_ClientConnectionTimeout "client connection has timed out";
%feature("docstring") ClientServiceListenerEventType_TransportConnectionConnected "client transport has been connected";
%feature("docstring") ClientServiceListenerEventType_TransportConnectionClosed "client transport connection has been closed or lost";
%feature("docstring") ClientServiceListenerEventType_ServicePathReleased "client has received notification that service path was released";

// ServerServiceListenerEventType Enum
%feature("docstring") ServerServiceListenerEventType_ServiceClosed "service has been closed";
%feature("docstring") ServerServiceListenerEventType_ClientConnected "client has connected";
%feature("docstring") ServerServiceListenerEventType_ClientDisconnected "client has disconnected";

// MemberDefinition_Direction Enum
%feature("docstring") MemberDefinition_Direction_both "member supports read and write";
%feature("docstring") MemberDefinition_Direction_readonly "member is readonly";  
%feature("docstring") MemberDefinition_Direction_writeonly "member is writeonly";

// MemberDefinition_NoLock
%feature("docstring") MemberDefinition_NoLock_none "member cannot be accessed by other users/sessions when object is locked";
%feature("docstring") MemberDefinition_NoLock_all "member can be accessed by other users/sessions when object is locked";
%feature("docstring") MemberDefinition_NoLock_read "member can be read by other users/sessions when object is locked";

// Preprocessor constants
%feature("docstring") RR_TIMEOUT_INFINITE "Disable timeout for asynchronous operations";
%feature("docstring") RR_VALUE_LIFESPAN_INFINITE "Set wire values to have infinite lifespan and will not expire";

// TransportCapabilityCode flags
%feature("docstring") TranspartCapabilityCode_PAGE_MASK "Page mask for transport capability code";
%feature("docstring") TransportCapabilityCode_MESSAGE2_BASIC_PAGE2000000 "Message Version 2 transport capability page code";
%feature("docstring") TransportCapabilityCode_MESSAGE2_BASIC_ENABLE "Enable Message Version 2 transport capability flag";
%feature("docstring") TransportCapabilityCode_MESSAGE2_BASIC_CONNECTCOMBINED2 "Enable Message Version 2 connect combined transport capability flag";
%feature("docstring") TransportCapabilityCode_MESSAGE4_BASIC_PAGE4000000 "Message Version 4 transport capability page code";
%feature("docstring") TransportCapabilityCode_MESSAGE4_BASIC_ENABLE "Enable Message Version 4 transport capability flag";
%feature("docstring") TransportCapabilityCode_MESSAGE4_BASIC_CONNECTCOMBINED2 "Enable Message Version 4 connect combine transport capability flag";
%feature("docstring") TransportCapabilityCode_MESSAGE4_STRINGTABLE_PAGE4100000 "Message Version 4 String Table capability page code";
%feature("docstring") TransportCapabilityCode_MESSAGE4_STRINGTABLE_ENABLE "Enable Message Version 4 String Table transport capability code";
%feature("docstring") TransportCapabilityCode_MESSAGE4_STRINGTABLE_MESSAGE_LOCAL2 "Enable Message Version 4 local String Table capability code";
%feature("docstring") TransportCapabilityCode_MESSAGE4_STRINGTABLE_STANDARD_TABLE4 "Enable Message Version 4 standard String Table capability code";

// RobotRaconteur_LogLevel Enum
%feature("docstring") RobotRaconteur_LogLevel_Trace "`trace` log level";
%feature("docstring") RobotRaconteur_LogLevel_Debug "`debug` log level";
%feature("docstring") RobotRaconteur_LogLevel_Info "`info` log level";
%feature("docstring") RobotRaconteur_LogLevel_Warning "`warning` log level";
%feature("docstring") RobotRaconteur_LogLevel_Error "`error` log level";
%feature("docstring") RobotRaconteur_LogLevel_Fatal "`fatal` log level";
%feature("docstring") RobotRaconteur_LogLevel_Disable "`disabled` log level";

// RobotRaconteur_LogComponent Enum
%feature("docstring") RobotRaconteur_LogComponent_Default "default component";
%feature("docstring") RobotRaconteur_LogComponent_Node "Robot Raconteur Node component";
%feature("docstring") RobotRaconteur_LogComponent_Transport "tranport component";
%feature("docstring") RobotRaconteur_LogComponent_Message "message or message serialization component";
%feature("docstring") RobotRaconteur_LogComponent_Client "client component";
%feature("docstring") RobotRaconteur_LogComponent_Service "service component";
%feature("docstring") RobotRaconteur_LogComponent_Member "member component";
%feature("docstring") RobotRaconteur_LogComponent_Pack "data message packing component";
%feature("docstring") RobotRaconteur_LogComponent_Unpack "data message unpacknig component";
%feature("docstring") RobotRaconteur_LogComponent_ServiceDefinition "service definition parser component";
%feature("docstring") RobotRaconteur_LogComponent_Discovery "node/service discovery component";
%feature("docstring") RobotRaconteur_LogComponent_Subscription "subscription component";
%feature("docstring") RobotRaconteur_LogComponent_NodeSetup "node setup component";
%feature("docstring") RobotRaconteur_LogComponent_Utility "utility component";
%feature("docstring") RobotRaconteur_LogComponent_RobDefLib "service definition standard library component (external)";
%feature("docstring") RobotRaconteur_LogComponent_User "user component (external)";
%feature("docstring") RobotRaconteur_LogComponent_UserClient "user client component (external)";
%feature("docstring") RobotRaconteur_LogComponent_UserService "user service component (external)";
%feature("docstring") RobotRaconteur_LogComponent_ThirdParty "third party library component (external)";


// RobotRaconteurNode
%feature("docstring") RobotRaconteur::RobotRaconteurNode """
The central node implementation

RobotRaconteurNode implements the current Robot Raconteur instance
and acts as the central switchpoint for the instance. The user 
registers types, connects clients, registers services, and 
registers transports through this class.

If the current program only needs one instance of RobotRaconteurNode,
the singleton can be used. The singleton is accessed using:

RobotRaconteurNode.s

The singleton node is typically aliased to ``RRN`` for shorthand.

If

.. code-block:: python

   from RobotRaconteur.Client import *

is used, then ``RRN`` is automatically imported. If not used, the following
is typically used:

.. code-block:: python
    
   import RobotRaconteur as RR
   RRN = RobotRaconteur.s

"""

%feature("docstring") RobotRaconteur::RobotRaconteurNode::__init__() """
Construct a new Robot Raconteur node.
"""

%feature("docstring") RobotRaconteur::RobotRaconteurNode::Init(size_t thread_count=20) """Initialize the node. Called automatically for ``RRN``

This function must be called to initialize background tasks before
using the node. It is called automatically by the
singleton accesors, so the user only needs to call this function
when not using the singleton. If a custom thread pool is being
used, the thread pool factory must be specified before 
calling init.

:param thread_count: The initial number of threads in the thread pool (default 20)
:type thread_count: int
"""

%feature("docstring") RobotRaconteur::RobotRaconteurNode::RegisterTransport(boost::shared_ptr<Transport> transport) """

Register a transport for use by the node

:param transport: The transport to register
:type transport: RobotRaconteur.Transport
:return: The transport internal id
:rtype: int

"""

%feature("docstring") RobotRaconteur::RobotRaconteurNode::SelectRemoteNodeURL(const std::vector<std::string>& urls) """

Select the \"best\" URL from a std::vector of candidates

Service discovery will often return a list of candidate URLs to 
use to connect to a node. This function uses hueristics to select
the \"best\" URL to use. The selection criteria ranks URLs in roughly
the following order, lower number being better:

1. \"rr+intra\" for IntraTransport
2. \"rr+local\" for LocalTransport
3. \"rr+pci\" or \"rr+usb\" for HardwareTransport
4. \"rrs+tcp://127.0.0.1\" for secure TcpTransport loopback
5. \"rrs+tcp://[::1]\" for secure TcpTransport IPv6 loopback
6. \"rrs+tcp://localhost\" for secure TcpTransport loopback
7. \"rrs+tcp://[fe80\" for secure TcpTransport link-local IPv6
8. \"rrs+tcp://\" for any secure TcpTransport
9. \"rr+tcp://127.0.0.1\" for TcpTransport loopback
10. \"rr+tcp://[::1]\" for TcpTransport IPv6 loopback
11. \"rr+tcp://localhost\" for TcpTransport loopback
12. \"rr+tcp://[fe80\" for TcpTransport link-local IPv6
13. \"rr+tcp://\" for any TcpTransport

:param urls: The candidate URLs
:type urls: List[str]
:return: The \"best\" URL out of the candidates
:rtype: str
"""

%feature("docstring") RobotRaconteur::RobotRaconteurNode::UnregisterServiceType(const std::string& type) """

Unregister a previously registered service type

This function is not recommended as the results can be unpredictable

:param type: The service type to unregister
:type type: str
"""

%feature("docstring") RobotRaconteur::RobotRaconteurNode::IsServiceTypeRegistered(const std::string& type) """

Test if a service type has been registered

:param type: The name of the service type to check
:type type: str
:rtype: bool
"""

%feature("docstring") RobotRaconteur::RobotRaconteurNode::CloseService(const std::string& sname) """

Closes a previously registered service

Services are automatically closed by Shutdown, so this function
is rarely used.

:param sname: The name of the service to close
:type sname: str
"""

%feature("docstring") RobotRaconteur::RobotRaconteurNode::Sleep(const boost::posix_time::time_duration& duration) """

Sleeps for a specified duration

Normally will sleep based on the system clock, but in certain
circumstances will use simulation time

:param duration: Duration to sleep in seconds
:type duration: float
"""

%feature("docstring") RobotRaconteur::RobotRaconteurNode::CreateRate(double frequency) """

Create a Rate object

Rate is used to stabilize periodic loops to a specified frequency
 
This function will normally return a WallRate instance

:param frequency: Frequency of loop in Hz
:type frequency: float
:return: The new Rate object
:rtype: RobotRaconteur.Rate
"""

%feature("docstring") RobotRaconteur::RobotRaconteurNode::CreateAutoResetEvent() """
Create an AutoResetEvent object
		  
Normally the AutoResetEvent will use the system clock for timeouts,
but in certain circumstances will use simulation time

:return: The new AutoResetEvent object
:rtype: RobotRaconteur.AutoResetEvent
"""

%feature("docstring") RobotRaconteur::RobotRaconteurNode::CompareLogLevel(RobotRaconteur_LogLevel log_level) """

Test if the specified log level would be accepted

:param log_level: Log level to test
:type log_level: int
:return: True if the log would be accepted, False if it would be ignored
:rtype: bool
"""

%feature("docstring") RobotRaconteur::RobotRaconteurNode::LogMessage(RobotRaconteur_LogLevel level, const std::string& message) """

Log a simple message using the current node

The record will be sent to the configured log handler,
or sent to std::cerr if none is configured

If the level of the message is below the current log level
for the node, the record will be ignored

"""

%feature("docstring") RobotRaconteur::RobotRaconteurNode::LogRecord(const RRLogRecord& record) """

Log a record to the node.

The record will be sent to the configured log handler,
or sent to std::cerr if none is configured

If the level of the message is below the current log level
for the node, it will be ignored

:param record: The record to log
:type record: RRLogRecord
"""

%feature("docstring") RobotRaconteur::RobotRaconteurNode::GetLogLevel() """

Get the current log level for the node

Default level is \"info\"

:return: The current log level
:rtype: int
"""

%feature("docstring") RobotRaconteur::RobotRaconteurNode::SetLogLevel(RobotRaconteur_LogLevel level) """

Set the log level for the node

Set RobotRaconteur.RobotRaconteur_LogLevel_Disable to disable logging

:param level: The desired log level
:type level: int
"""

%feature("docstring") RobotRaconteur::RobotRaconteurNode::SetLogLevelFromString(const std::string& level) """

Set the log level for the node from a string

Must be one of the following values: DISABLE, FATAL, ERROR, WARNING, INFO, DEBUG, TRACE

Defaults to WARNING

:param level: The desired log level
:type level: str
:return: The log level
:rtype: int
"""

%feature("docstring") RobotRaconteur::RobotRaconteurNode::SetLogLevelFromEnvVariable(const std::string& env_variable_name = "ROBOTRACONTEUR_LOG_LEVEL") """

Set the log level for the node from specified environmental variable

Retrieves the specified environmental variable and sets the log level based
on one of the following values: DISABLE, FATAL, ERROR, WARNING, INFO, DEBUG, TRACE

If an invalid value or the variable does not exist, the log level is left unchanged. 

:param env_variable_name: The environmental variable to use. Defaults to 
 ``ROBOTRACONTEUR_LOG_LEVEL``
:type env_variable_name: str
:return: The log level
:rtype: int
"""

%feature("docstring") RobotRaconteur::RobotRaconteurNode::GetLogRecordHandler() """

Get the currently configured log record handler

If None, records are sent to ``stdout``

:return: The log record handler
:rtype: LogRecordHandler
"""

%feature("docstring") RobotRaconteur::RobotRaconteurNode::SetLogRecordHandler(boost::shared_ptr<RobotRaconteur::LogRecordHandler> handler) """

Set the handler for log records

If handler is NULL, records are sent to std::cerr

:param handler: The log record handler function
:type handler: RobotRaconteur.LogRecordHandler
"""

%feature("docstring") RobotRaconteur::RobotRaconteurNode::CheckConnection(uint32_t client) """

Check that the TransportConnection associated with an endpoint
is connected

Raises `RobotRaconteur.ConnectionException` or other exception if connection is invalid.

:param endpoint: The LocalEndpoint identifier to check
:type endpoint: int
"""

%feature("docstring") RobotRaconteurNodeSetupFlags_NONE "No options enabled";
%feature("docstring") RobotRaconteurNodeSetupFlags_ENABLE_NODE_DISCOVERY_LISTENING "Enable node discovery listening on all transports";
%feature("docstring") RobotRaconteurNodeSetupFlags_ENABLE_NODE_ANNOUNCE "Enable node announce on all transports";
%feature("docstring") RobotRaconteurNodeSetupFlags_ENABLE_LOCAL_TRANSPORT "Enable LocalTransport";
%feature("docstring") RobotRaconteurNodeSetupFlags_ENABLE_TCP_TRANSPORT "Enable TcpTransport";
%feature("docstring") RobotRaconteurNodeSetupFlags_ENABLE_HARDWARE_TRANSPORT "Enable HardwareTransport";
%feature("docstring") RobotRaconteurNodeSetupFlags_LOCAL_TRANSPORT_START_SERVER "Start the LocalTransport server to listen for incoming clients";
%feature("docstring") RobotRaconteurNodeSetupFlags_LOCAL_TRANSPORT_START_CLIENT "Start the LocalTransport client with specified node name";
%feature("docstring") RobotRaconteurNodeSetupFlags_TCP_TRANSPORT_START_SERVER "Start the TcpTransport server to listen for incoming clients on the specified port";
%feature("docstring") RobotRaconteurNodeSetupFlags_TCP_TRANSPORT_START_SERVER_PORT_SHARER "Start the TcpTransport server to incoming for incoming clients using the port sharer";
%feature("docstring") RobotRaconteurNodeSetupFlags_DISABLE_MESSAGE4 "Disable Message Format Version 4 on all transports";
%feature("docstring") RobotRaconteurNodeSetupFlags_DISABLE_STRINGTABLE "Disable Message Format Version 4 string table on all transports";
%feature("docstring") RobotRaconteurNodeSetupFlags_DISABLE_TIMEOUTS "Disable all timeouts (useful for debugging)";
%feature("docstring") RobotRaconteurNodeSetupFlags_LOAD_TLS_CERT "Load the TLS certificate for TcpTransport";
%feature("docstring") RobotRaconteurNodeSetupFlags_REQUIRE_TLS "Require TLS for all clients on TcpTransport";
%feature("docstring") RobotRaconteurNodeSetupFlags_LOCAL_TRANSPORT_SERVER_PUBLIC "Make LocalTransport server listen for incoming clients from all users";
%feature("docstring") RobotRaconteurNodeSetupFlags_NODENAME_OVERRIDE "Allow NodeName to be configured using command line options";
%feature("docstring") RobotRaconteurNodeSetupFlags_NODEID_OVERRIDE "Allow NodeID to be configured using command line options";
%feature("docstring") RobotRaconteurNodeSetupFlags_TCP_PORT_OVERRIDE "Allow TCP port to be configured using command line options";
%feature("docstring") RobotRaconteurNodeSetupFlags_TCP_WEBSOCKET_ORIGIN_OVERRIDE "Allow TCP WebSocket origin control to be configured using command line options";
%feature("docstring") RobotRaconteurNodeSetupFlags_ENABLE_INTRA_TRANSPORT "Enable IntraTransport";
%feature("docstring") RobotRaconteurNodeSetupFlags_INTRA_TRANSPORT_START_SERVER "Start the IntraTransport server to listen for incoming clients";
%feature("docstring") RobotRaconteurNodeSetupFlags_LOCAL_TAP_ENABLE "Enable the LocalTap debug logging system";
%feature("docstring") RobotRaconteurNodeSetupFlags_LOCAL_TAP_NAME "Allow the user to set the LocalTap name";
%feature("docstring") RobotRaconteurNodeSetupFlags_ENABLE_ALL_TRANSPORTS "Convenience flag to enable all transports";
%feature("docstring") RobotRaconteurNodeSetupFlags_CLIENT_DEFAULT "Default configuration for client nodes (See ClientNodeSetup)";
%feature("docstring") RobotRaconteurNodeSetupFlags_CLIENT_DEFAULT_ALLOWED_OVERRIDE "Default allowed overrides for client nodes (See ClientNodeSetup)";
%feature("docstring") RobotRaconteurNodeSetupFlags_SERVER_DEFAULT "Default configuration for server nodes";
%feature("docstring") RobotRaconteurNodeSetupFlags_SERVER_DEFAULT_ALLOWED_OVERRIDE "Default allowed overrides for server nodes";
%feature("docstring") RobotRaconteurNodeSetupFlags_SECURE_SERVER_DEFAULT "Default configuration for server nodes requiring TLS network transports";
%feature("docstring") RobotRaconteurNodeSetupFlags_SECURE_SERVER_DEFAULT_ALLOWED_OVERRIDE "Default allowed overrides for server nodes requiring TLS network transports";


%feature("docstring") RobotRaconteur::CommandLineConfigParser """
Command line parser for node setup classes

The CommandLineConfigParser is used to parse command line options specified
when a program is launched. These options allow for the node configuration to be
changed without recompiling the software. See \\ref command_line_options for
a table of the standard command line options.

ClientNodeSetup, ServerNodeSetup, and SecureServerNodeSetup use this class to parse
the ``sys.argv`` parameters. The RobotRaconteurNodeSetup constructors will accept
either ``sys.argv``, or will accept an initialize CommandLineConfigParser.

The CommandLineConfig() constructor takes the \"allowed override\" flags, and the option prefix.
The \"allowed override\" specifies which options can be overridden using the command line. The
prefix option allows the command line flag prefix to be changed. By default it expects
all options to begin with `--robotraconteur-` followed by the name of the option. If there are
multiple nodes, it is necessary to change the prefix to be unique for each node. For instance,
\"robotraconteur1-\" for the first node and \"robotraconteur2-\" for the second node.

Users may add additional options to the parser. Use AddStringOption(),
AddBoolOption(), or AddIntOption() to add additional options.
"""

%feature("docstring") RobotRaconteur::CommandLineConfigParser::SetDefaults(const std::string& node_name, uint16_t tcp_port, uint32_t default_flags) """
Set the default NodeName, TCP port, and flags

The command line options will be allowed to override the options
specified in allowed_overrides passed to CommandLineConfigParser().

:param node_name: The default NodeName
:type node_name str
:param tcp_port: The default TCP port
:type tcp_port: int
:param default_flags: The default flags
:type default_flags: int
"""

%feature("docstring") RobotRaconteur::CommandLineConfigParser::AddStringOption(const std::string& name, const std::string& descr) """
Add a new string option

:param name: The name of the option
:type name: str
:param descr: Description of the option
:type descr: str
"""

%feature("docstring") RobotRaconteur::CommandLineConfigParser::AddBoolOption(const std::string& name, const std::string& descr) """
Add a new bool option

:param name: The name of the option
:type name: str
:param descr: Description of the option
:type descr: str
"""

%feature("docstring") RobotRaconteur::CommandLineConfigParser::AddIntOption(const std::string& name, const std::string& descr) """
Add a new int32_t option

:param name: The name of the option
:type name: str
:param descr: Description of the option
:type descr: str
"""

%feature("docstring") RobotRaconteur::CommandLineConfigParser::ParseCommandLine(const std::vector<std::string>& args) """
Parse a specified string vector containing the options

Results are stored in the instance

:param args: The options as a string list
:type args: List[str]
"""

%feature("docstring") RobotRaconteur::CommandLineConfigParser::GetOptionOrDefaultAsString(const std::string& option) """
Get the option value as a string

Returns empty string if option not specified on command line

:param option: The name of the option
:type option: str
:return: The option value, or an empty string
:rtype: str
"""

%feature("docstring") RobotRaconteur::CommandLineConfigParser::GetOptionOrDefaultAsString(const std::string& option, const std::string& default_value) """
Get the option value as a string

Returns default_value if option not specified on command line

:param option: The name of the option
:type option: str
:param default_value: The default option value
:type default_value: str
:return: The option value, or default_value if not specified on command line
:rtype: str
"""

%feature("docstring") RobotRaconteur::CommandLineConfigParser::GetOptionOrDefaultAsBool(const std::string& option) """
Get the option value as a bool

Returns false if option not specified on command line

:param option: The name of the option
:type option: str
:return: The option value, or False
:rtype: bool
"""

%feature("docstring") RobotRaconteur::CommandLineConfigParser::GetOptionOrDefaultAsBool(const std::string& option, bool default_value) """
Get the option value as a bool

Returns default_value if option not specified on command line

:param option: The name of the option
:type option: bool
:param default_value: The default option value
:type default_value: bool
:return: The option value, or default_value if not specified on command line
:rtype: bool
"""

%feature("docstring") RobotRaconteur::CommandLineConfigParser::GetOptionOrDefaultAsInt(const std::string& option) """
Get the option value as an int

Returns -1 if option not specified on command line

:param option: The name of the option
:type option: str
:return: The option value, or -1
:rtype: int
"""

%feature("docstring") RobotRaconteur::CommandLineConfigParser::GetOptionOrDefaultAsInt(const std::string& option, int32_t default_value) """
Get the option value as an int

Returns default_value if option not specified on command line

:param option: The name of the option
:type option: str
:param default_value: The default option value
:type default_value: int
:return: The option value, or default_value if not specified on command line
:rtype: int
"""

%feature("docstring") RobotRaconteur::TimeSpec """
TimeSpec(seconds=0,nanoseconds=0)

Represents. a point in time. Used by \"wire\" members to 
timestamp packets

Time is always in UTC

Time is relative to the UNIX epoch \"1970-01-01T00:00:00Z\"

:param seconds: seconds from epoch
:type seconds: int
:param nanoseconds: nanoseconds from epoch
:type nanoseconds: int
"""

%feature("docstring") RobotRaconteur::TimeSpec::seconds """
Seconds since epoch
"""

%feature("docstring") RobotRaconteur::TimeSpec::nanoseconds """
Nanoseconds from epoch. Normalized to be between 0 and 1e9-1
"""

%feature("docstring") RobotRaconteur::TimeSpec::operator == (const TimeSpec &t2) """equality comparison"""
%feature("docstring") RobotRaconteur::TimeSpec::operator != (const TimeSpec &t2) """inequality comparison"""
%feature("docstring") RobotRaconteur::TimeSpec::operator - (const TimeSpec &t2) """subtraction operator"""
%feature("docstring") RobotRaconteur::TimeSpec::operator + (const TimeSpec &t2) """addition operator"""
%feature("docstring") RobotRaconteur::TimeSpec::operator > (const TimeSpec &t2) """greater-than comparison"""
%feature("docstring") RobotRaconteur::TimeSpec::operator >= (const TimeSpec &t2) """greater-than-or-equal comparison"""
%feature("docstring") RobotRaconteur::TimeSpec::operator < (const TimeSpec &t2) """less-then comparison"""
%feature("docstring") RobotRaconteur::TimeSpec::operator <= (const TimeSpec &t2) """less-than-or-equal comparison"""

%feature("docstring") RobotRaconteur::TimeSpec::cleanup_nanosecs() """normalize nanoseconds to be within 0 and 1e9-1"""


%feature("docstring") RobotRaconteur::HardwareTransport """
Transport for USB, Bluetooth, and PCIe hardware devices

**WARNING: THE HARDWARE TRANSPORT IS EXPERIMENTAL!**

The HardwareTransport is disabled by default by the node setup classes.
Use ``--robotraconteur-hardware-enable=true`` option to enable.

It is recommended that ClientNodeSetup, ServerNodeSetup, or SecureServerNodeSetup
be used to construct this class.

See \\ref robotraconteur_url for more information on URLs.

Contact Wason Technology, LLC for more information on the hardware
transport.

The use of RobotRaconteurNodeSetup and subclasses is recommended to construct
transports.

The transport must be registered with the node using
RobotRaconteurNode.RegisterTransport() after construction if node
setup is not used.

:param node: (optional) The node that will use the transport. Default is the singleton node
:type node: RobotRaconteur.RobotRaconteurNode
"""

%feature("docstring") RobotRaconteur::HardwareTransport::Close() """
Close the transport. Done automatically by node shutdown.
"""

%feature("docstring") RobotRaconteur::IntraTransport """
Transport for intra-process communication

It is recommended that ClientNodeSetup, ServerNodeSetup, or SecureServerNodeSetup
be used to construct this class.

See \\ref robotraconteur_url for more information on URLs.

The IntraTransport implements transport connections between nodes running
within the same process. This is often true for simulation environments, where
there may be multiple simulated devices running within the simulation. The
IntraTransport uses a singleton to keep track of the different nodes running
in the same process, and to form connections. The singleton also implements
discovery updates.

The use of RobotRaconteurNodeSetup and subclasses is recommended to construct
transports.

The transport must be registered with the node using
RobotRaconteurNode.RegisterTransport() after construction if node
setup is not used.

:param node: (optional) The node that will use the transport. Default is the singleton node
:type node: RobotRaconteur.RobotRaconteurNode
"""

%feature("docstring") RobotRaconteur::IntraTransport::Close() """
Close the transport. Done automatically by node shutdown.
"""

%feature("docstring") RobotRaconteur::IntraTransport::StartServer() """Start the server to listen for incoming client connections"""


%feature("docstring") RobotRaconteur::LocalTransport """
Transport for communication between processes using UNIX domain sockets

It is recommended that ClientNodeSetup, ServerNodeSetup, or SecureServerNodeSetup
be used to construct this class.

See \\ref robotraconteur_url for more information on URLs.

The LocalTransport implements transport connections between processes running on the
same host operating system using UNIX domain sockets. UNIX domain sockets are similar
to standard networking sockets, but are used when both peers are on the same machine
instead of connected through a network. This provides faster operation and greater
security, since the kernel simply passes data between the processes. UNIX domain
sockets work using Information Node (inode) files, which are special files on
the standard filesystem. Servers \"listen\" on a specified inode, and clients
use the inode as the address to connect. The LocalTransport uses UNIX sockets
in `SOCK_STREAM` mode. This provides a reliable stream transport connection similar
to TCP, but with significantly improved performance due the lower overhead.

UNIX domain sockets were added to Windows 10 with the 1803 update. Robot Raconteur
switch to UNIX domain sockets for the LocalTransport on Windows in version 0.9.2.
Previous versions used Named Pipes, but these were inferior to UNIX sockets. The
LocalTransport will not function on versions of Windows prior to Windows 10 1803 update
due to the lack of support for UNIX sockets. A warning will be issued to the log if 
the transport is not available, and all connection attempts will fail. All other
transports will continue to operate normally.

The LocalTransport stores inode and node information files in the filesystem at various
operator system dependent locations. See the Robot Raconteur Standards documents
for details on where these files are stored.

Discovery is implemented using file watchers. The file watchens must be activated
using the node setup flags, or by calling EnableNodeDiscoveryListening().
After being initialized the file watchers operate automatically.

The LocalTransport can be used to dynamically assign NodeIDs to nodes based on a NodeName.
StartServerAsNodeName() and StartClientAsNodeName() take a NodeName that will identify the
node to clients, and manage a system-local NodeID corresponding to that NodeName. The 
generated NodeIDs are stored on the local filesystem. If LocalTransport finds a corresponding
NodeID on the filesystem, it will load and use that NodeID. If it does not, a new random NodeID
is automatically generated.

The server can be started in \"public\" or \"private\" mode. Private servers store their inode and
information in a location only the account owner can access, while \"public\" servers are
placed in a location that all users with the appropriate permissions can access. By default,
public LocalTransport servers are assigned to the \"robotraconteur\" group. Clients that belong to the
\"robotraconteur\" group will be able to connect to these public servers.

The use of RobotRaconteurNodeSetup and subclasses is recommended to construct
transports.

The transport must be registered with the node using
RobotRaconteurNode.RegisterTransport() after construction if node
setup is not used.

:param node: (optional) The node that will use the transport. Default is the singleton node
:type node: RobotRaconteur.RobotRaconteurNode
"""

%feature("docstring") RobotRaconteur::LocalTransport::Close() """
Close the transport. Done automatically by node shutdown.
"""

%feature("docstring") RobotRaconteur::LocalTransport::IsLocalTransportSupported() """
Check if the LocalTransport is supported on the current
operating system.

Windows versions before Windows 10 1803 do not support the LocalTransport
due to lack of UNIX sockets. All other transports will continue to operate
normally.

All versions of Linux and Mac OSX support the LocalTransport

LocalTransport on Android and iOS is not officially supported

:return: True LocalTransport is supported, otherwise False
:rtype: bool
"""

%feature("docstring") RobotRaconteur::LocalTransport::StartClientAsNodeName(const std::string& name) """
Initialize the LocalTransport by assigning a NodeID based on NodeName

Assigns the specified name to be the NodeName of the node, and manages
a corresponding NodeID. See LocalTransport for more information.

Throws NodeNameAlreadyInUse if another node is using name

:param name: The node name
:type name: str
"""
%feature("docstring") RobotRaconteur::LocalTransport::StartServerAsNodeName(const std::string& name) """
StartServerAsNodeName(name, public_ = False)

Start the server using the specified NodeName and assigns a NodeID

The LocalTransport will listen on a UNIX domain socket for incoming clients,
using information files and inodes on the local filesystem. Clients
can locate the node using the NodeID and/or NodeName. The NodeName is assigned
to the node, and the transport manages a corresponding NodeID. See
LocalTransport for more information.

Throws NodeNameAlreadyInUse if another node is using name

Throws NodeIDAlreadyInUse if another node is using the managed
NodeID

:param name: The NodeName
:type name: str
:param public_: If True, other users can access the server. If False, only
  the account owner can access the server.
:type public_: bool
"""
%feature("docstring") RobotRaconteur::LocalTransport::StartServerAsNodeID(const NodeID& nodeid) """
StartServerAsNodeId(name, public_ = False)

The LocalTransport will listen on a UNIX domain socket for incoming clients,
using information files and inodes on the local filesystem. This function
leaves the NodeName blank, so clients must use NodeID to identify the node.

Throws NodeIDAlreadyInUse if another node is using nodeid

:param nodeid: The NodeID
:type nodeid: RobotRaconteur.NodeID
:param public_: If True, other users can access the server. If False, only 
 the account owner can access the server.
:type public_: bool
"""

%feature("docstring") RobotRaconteur::TcpTransport """
Transport for Transport Control Protocol Internet Protocol (TCP/IP) networks

It is recommended that ClientNodeSetup, ServerNodeSetup, or SecureServerNodeSetup
be used to construct this class.

See \\ref robotraconteur_url for more information on URLs.

The TcpTransport implements transport connections over TCP/IP networks. TCP/IP is the 
most common protocol used for Internet and Local Area Network (LAN) communication, including
Ethernet and WiFi connections. The Transport Control Protocol (TCP) is a reliable stream
protocol that establishes connections between devices using IP address and port pairs.
Each adapter has an assigned address, and applications create connections on different ports.
TcpTransport listens to the port specified in StartServer(), and the client uses
a URL containing the IP address and port of the listening transport. The TcpTransport
uses the established connection to pass messages between nodes.

The IP protocol is available in two major versions, IPv4 and IPv6. The most common
is IPv4, and its 32 bit address is typically written as four numbers, 
ie 172.17.12.174. IPv4 has a number of critical limitations, the greatest being 
its 2^32 address limit (approximately 4 billion). This is a problem when there are 
tens of billions of internet connected devices already present. IPv6 introduces a 128 
bit address space, which allows for approximately 3.4x10^38 possible addresses. The major
advantage for Robot Raconteur is the introduction of \"link-local\" addresses. These addresses
begin with \"FE80::\" and finish with an \"EUI-64\" address, which is tied to the MAC address
of the adaptor. IPv4 addresses need to be assigned to devices locally, and have a tendency
to change. IPv6 addresses are permanently assigned to the adapter itself, meaning that
network configuration for LAN communication is essentially automatic. Robot Raconteur
will prefer IPv6 connections when possible for this reason.

The TcpTransport is capable of using \"raw\" streams that implement the Robot Raconteur
message protocols, or to use HTTP WebSockets. HTTP WebSockets allow Robot Raconteur
to communicate seamlessly with browsers and HTTP servers without requiring
additional plugins. WebSockets provide additional security using \"origins\". See
AddWebSocketAllowedOrigin() for more information.

The TcpTransport supports TLS encryption using certificates. See \\ref tls_security for
more information on TLS. The TcpTransport supports four modes of TLS encryption:

======= ============================================ ===========
Scheme  Description                                  Direction
======= ============================================ ===========
rrs+tcp \"Raw\" protocol with TLS                      Both
rr+wss  Websocket over HTTPS                         Client Only
rrs+ws  Websocket with RobotRaconteur TLS over HTTP  Both
rrs+wss Websocket with RobotRaconteur TLS over HTTPS Client Only
======= ============================================ ===========

The different combinations of TLS and HTTPS for websockets are used for different scenarios.
Robot Raconteur Core can initiate HTTPS connections, but cannot accept them. Accepting
HTTPS connections requires a certificate issued by an authority like GoDaddy or Digicert, 
and is typically used with an HTTP server running RobotRaconteurWeb.

TLS certificates for Robot Raconteur nodes are issued by Wason Technology, LLC using
a root certificate that is \"burned in\" to Robot Raconteur Core. All devices running
Robot Raconteur will support this certificate chain.

Discovery for the TcpTransport is accomplished using User Defined Protocol (UDP) multicast
and/or broadcast packets. Broadcast packets are sent to all connected devices, while
multicast is sent to devices that have registered to receive them. Unlike TCP, the packets
sent to broadcast or multicast are sent to the entire network. This allows for devices
to find each other on the network. 

For IPv4, the broadcast address 255.255.255.255 on port 48653
is used for discovery. By default, IPv4 is disabled in favor of IPv6. IPv6 uses the multicast 
following multicast addresses:

========== ========== ===== ========
Address    Scope      Port  Default?
========== ========== ===== ========
FF01::BA86 Node-Local 48653 Disabled
FF02::BA86 Link-Local 48653 Enabled
FF05::BA86 Site-Local 48653 Disabled
========== ========== ===== ========

By default, discovery will only occur on the link-local IPv6 scope. This will
find nodes on the local subnet, but will not attempt to pass through any routers.

The use of RobotRaconteurNodeSetup and subclasses is recommended to construct
transports.

The transport must be registered with the node using
RobotRaconteurNode.RegisterTransport() after construction if node
setup is not used.

:param node: (optional) The node that will use the transport. Default is the singleton node
:type node: RobotRaconteur.RobotRaconteurNode
"""

%feature("docstring") RobotRaconteur::TcpTransport::Close() """
Close the transport. Done automatically by node shutdown.
"""

%feature("docstring") RobotRaconteur::TcpTransport::EnableNodeDiscoveryListening(uint32_t flags=(IPNodeDiscoveryFlags_LINK_LOCAL)) """
Enable node discovery listening

By default enables listining on IPv6 link-local scope

See IPNodeDiscoveryFlags constants

:param flags: The flags specifying the scope
:type flags: int
"""

%feature("docstring") RobotRaconteur::TcpTransport::DisableNodeDiscoveryListening() """
Disable node discovery listening
"""

%feature("docstring") RobotRaconteur::TcpTransport::EnableNodeAnnounce(uint32_t flags=IPNodeDiscoveryFlags_LINK_LOCAL) """
Enable node discovery announce

By default enables announce on IPv6 link-local scope

See IPNodeDiscoveryFlags constants

:param flags: The flags specifying the scope
:type flags: int
"""
%feature("docstring") RobotRaconteur::TcpTransport::DisableNodeAnnounce() """
Disable node discovery announce
"""

%feature("docstring") RobotRaconteur::TimerEvent """
Timer event structure

Contains information about the state of the timer. Passed to the
callback on invocation.
"""

%feature("docstring") RobotRaconteur::TimerEvent::stopped """
True if timer has been stopped

:rtype: bool
"""

%feature("docstring") RobotRaconteur::TimerEvent::last_expected """
The last expected callback invocation time

:rtype: datetime.datetime
"""
	
%feature("docstring") RobotRaconteur::TimerEvent::last_real """
The real last callback invocation time

:rtype: datetime.datetime
"""

%feature("docstring") RobotRaconteur::TimerEvent::current_expected """
The current expected invocation time

:rtype: datetime.datetime
"""

%feature("docstring") RobotRaconteur::TimerEvent::current_real """
The current invocation time

:rtype: datetime.datetime
"""

%feature("docstring") RobotRaconteur::Timer """

A timer to invoke a callback
 
Timers invoke a callback at a specified rate. The timer
can either be one-short, or repeating.
 
Use RobotRaconteurNode.CreateTimer() to create timers.
"""

%feature("docstring") RobotRaconteur::Timer::Start() """
Start the timer

Must be called after RobotRaconteurNode.CreateTimer()
"""

%feature("docstring") RobotRaconteur::Timer::Stop() """
Stop the timer
"""

%feature("docstring") RobotRaconteur::Timer::GetPeriod() """
Get the period of the timer in seconds

:rtype: float
"""

%feature("docstring") RobotRaconteur::Timer::SetPeriod(const boost::posix_time::time_duration& period) """
Set the period of the timer in seconds

:param period: Period in seconds
:type period: float
"""

%feature("docstring") RobotRaconteur::Timer::IsRunning() """
Get if the timer is running

:rtype: bool
"""

%feature("docstring") RobotRaconteur::Rate """
Rate to stabilize a loop

Rate is used to stabilize the period of a loop. Use
RobotRaconteur.CreateRate() to create rates.
"""

%feature("docstring") RobotRaconteur::Rate::Sleep() """
Sleep the calling thread until the current loop period expires
"""

%feature("docstring") RobotRaconteur::AutoResetEvent """
Synchronization event for thread synchronization. Resets automatically after
being triggered

Construct using RobotRaconteurNode.CreateAutoResetEvent()
"""

%feature("docstring") RobotRaconteur::AutoResetEvent::Set() """
Set the event, releasing waiting threads
"""

%feature("docstring") RobotRaconteur::AutoResetEvent::Reset() """
Reset the event
"""

%feature("docstring") RobotRaconteur::AutoResetEvent::WaitOne(int32_t timeout) """
WaitOne(timeout=0)

Block the current thread until Set() is called, or timeout
expires

Timeout is based on the RobotRaconteurNode time provider

:param timeout: The timeout in seconds
:type timeout: float
:return: True if event was set, False if not
:rtype: bool
"""


%feature("docstring") RobotRaconteur::NodeID """
NodeID(Union[None,str,numpy.ndarray])

NodeID UUID storage and generation

Robot Raconteur uses NodeID and NodeName to uniquely identify a node.
NodeID is a UUID (Universally Unique ID), while NodeName is a string. The
NodeID is expected to be unique, while the NodeName is set by the user 
and may not be unique. The NodeID class represents the UUID NodeID.

A UUID is a 128-bit randomly generated number that is statistically guaranteed
to be unique to a very high probability. NodeID uses the Boost.UUID library
to generate, manage, and store the UUID.

The UUID can be loaded from a string, bytes, or generated randomly at runtime.
It can be converted to a string.

The LocalTransport and ServerNodeSetup classes will automatically assign
a NodeID to a node when the local transport is started with a specified node name.
The generated NodeID is stored on the local system, and is associated with the node name.
It will be loaded when a node is started with the same NodeName.

NodeID with all zeros is considered \"any\" node.
"""

%feature("docstring") RobotRaconteur::NodeID::ToString() """
Convert the NodeID UUID to string with \"B\" format

Convert the UUID string to 8-4-4-4-12 \"B\" format (with brackets)

{xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx}

:return: The string
:rtype: str
"""

%feature("docstring") RobotRaconteur::NodeID::NewUniqueID() """
Generate a new random NodeID UUID

Returned UUID is statistically guaranteed to be unique

:return: NodeID The newly generated UUID
:rtype: RobotRaconteur.NodeID
"""

%feature("docstring") RobotRaconteur::NodeID::operator == (const NodeID &id2) """
Test if NodeID is equal
"""

%feature("docstring") RobotRaconteur::NodeID::operator != (const NodeID &id2) """
Test if NodeID is not equal
"""

%feature("docstring") RobotRaconteur::NodeID::IsAnyNode() """
Is the NodeID UUID all zeros

The all zero UUID respresents \"any\" node, or an unset NodeID

:return: True The NodeID UUID is all zeros, representing any node, False The NodeID UUID is not all zeros
:rtype: bool
"""

%feature("docstring") RobotRaconteur::NodeID::GetAny() """
Get the \"any\" NodeId

:return: The \"any\" NodeID
:rtype: RobotRaconteur.NodeID
"""

%feature("docstring") RobotRaconteur::NodeID::ToByteArray() """
Convert the NodeID UUID to bytes

:return: The UUID as bytes
:rtype: bytearray
"""

%feature("docstring") RobotRaconteur::NodeID::FromByteArray(const char bytes[16]) """
Returns a new NodeID from bytes

:param bytes: The bytes
:type bytes: bytearray
:return: The new NodeID instance with the specified UUID
:rtype: RobotRaconteur.NodeID
"""

%feature("docstring") RobotRaconteur::ServerContext """
ServerContext()

Context for services registered in a node for use by clients

Services are registered using the RobotRaconteurNode.RegisterService() family of functions.
The ServerContext manages the services, and dispatches requests and packets to the appropriate
service object members. Services may expose more than one object. The root object is specified
when the service is registered. Other objects are specified through ObjRef members. A name
for the service is also specified when the service is registered. This name forms the root 
of the service path namespace. Other objects in the service have a unique service path
based on the ObjRef used to access the object.

Services may handle multiple connected clients concurrently. Each client is assigned
a ServerEndpoint. The ServerEndpoint is unique to the client connection,
and interacts with ServerContext to complete requests and dispatch packets. When
the service needs to address a specific client, the ServerEndpoint or the
ServerEndpoint.GetCurrentEndpoint() is used. (ServerEndpoint.GetCurrentEndpoint() returns the
int local client ID.)

Service attributes are a varvalue{string} types dictionary that is made available to
clients during service discovery. These attributes are used to help clients determine
which service should be selected for use. Because the attributes are passed to the clients
as part of the discovery process, they should be as concise as possible, and should
not use user defined types. Use ServerContext.SetAttributes() to set the service attributes
after registering the service. 

Security for the service is specified using a ServiceSecurityPolicy instance. This policy
is specified by passing as a parameter to RobotRaconteurNode.RegisterService(), or passing
the policy to the constructor.

ServerContext implements authentication and object locking. 
Server side functions are exposed by ServerContext for authentication, object locking,
and client management.

Clients using dynamic typing such as Python and MATLAB will only pull service types
explicitly imported by the root object and objref objects that have been requested. Clients
will not pull service types of user-defined named types if that service type is not explicitly
imported. This can be problematic if new `struct`, `pod`, and/or `namedarray` types are introduced
that do not have corresponding objects. Extra imports is used to specify extra service definitions
the client should pull. Use ServerContext.AddExtraImport(), ServerContext.RemoveExtraImport(),
and ServerContext.GetExtraImports() to manage the extra imports passed to the client.
"""

%feature("docstring") RobotRaconteur::ServerContext::GetCurrentServicePath() """
Get the current object service path

Returns the service path of the current object during a request or
packet event.
This is a thread-specific value and only
valid during the initial request or packet event invocation.

:return: The current object service path
:rtype: str
"""

%feature("docstring") RobotRaconteur::ServerContext::GetCurrentServerContext() """
Get the current ServerContext

Returns the current server context during a request or packet event.
This is a thread-specific value and only
valid during the initial request or packet event invocation.

:return: The current server context
:rtype: RobotRaconteur.ServerContext
"""

%feature("docstring") RobotRaconteur::ServerContext::KickUser(const std::string& username) """
Kicks a user with the specified username

User must be authenticated.

:param username: The username to kick
:type username: str
"""

%feature("docstring") RobotRaconteur::ServerContext::RequestObjectLock(const std::string& servicepath, const std::string& username) """
Request an object lock on servicepath for user username

This function handles incoming client requests, but may also be used
by the service directly

:param servicepath: The service path to lock
:type servicepath: str
:param username: The name of the user owning the lock
:type username: str
"""

%feature("docstring") RobotRaconteur::ServerContext::RequestClientObjectLock(const std::string& servicepath, const std::string& username, uint32_t endpoint) """
Request a client lock on servicepath for a specific client connection

This function handles incoming client requests, but may also be used
by the service directly. Client object locks lock for a specific client connection,
while client locks lock for a specific user. The specific client connection is 
specified using endpoint.

:param servicepath: The service path to lock
:type servicepath: str
:param username: The name of the user owning the lock
:type username: str
:param endpoint: The client endpoint ID of the client owning the lock
:type int:
"""

%feature("docstring") RobotRaconteur::ServerContext::ReleaseObjectLock(const std::string& servicepath, const std::string& username, bool override_) """
Release a client lock on servicepath

This function handles incoming client requests, but may also be used
by the service directly. Client locks can be released by the user that
created them if override_ is false, or by any user if override_ is true. 

The override_ parameter is set to true for client requests if the client has
the \"objectlockoverride\" permission.

:param servicepath: The service path to release lock
:type str:
:param username: The username requsting the lock release
:type username:
:param override_: If False, only the creating username can release the lock. If True, 
 any username can release the lock
:type override_: bool
"""

%feature("docstring") RobotRaconteur::ServerContext::GetObjectLockUsername(const std::string& servicepath) """
Get the name of the user that has locked the specified service path

:param servicepath: The service path
:type servicepath: str
:return: The user owning the lock, or empty servicepath is not locked
:rtype: str
"""

%feature("docstring") RobotRaconteur::ServerContext::GetNode() """
Get the Node

:rtype: RobotRaconteur.RobotRaconteurNode
"""

%feature("docstring") RobotRaconteur::ServerContext::GetExtraImports() """
Get the current list of extra service definition imports

:return: The list of extra imports
:rtype: List[str]
"""

%feature("docstring") RobotRaconteur::ServerContext::AddExtraImport(const std::string& import_) """
Add an extra service definition import

Clients using dynamic typing will not automatically pull service definitions unless
imported by the root object or an objref. If new \"struct\", \"po\", or \"namedarray\" types
are introduced in a new service definition type without a corresponding object, an error will
occur. Use AddExtraImport() to add the name of the new service definition to add it to the 
list of service definitions the client will pull.

Service definition must have been registered using RobotRaconteurNode::RegisterServiceType()

:param import_: The name of the service definition
:type import_: str
"""

%feature("docstring") RobotRaconteur::ServerContext::RemoveExtraImport(const std::string& import_) """
Removes an extra import service definition registered with AddExtraImport()

See AddExtraImport()

:param import_: The name of the service definition
:type import_: str
:return: True The service definition was removed, False The service definition was not found in the extra imports vector
:rtype: bool 
"""

%feature("docstring") RobotRaconteur::ServerEndpoint """
ServerEndpoint()

Server endpoint representing a client connection

Robot Raconteur creates endpoint pairs between a client and service. For clients, this endpoint
is a ClientContext. For services, the endpoint becomes a ServerEndpoint. ServerEndpoints are used
to address a specific client connected to a service, since services may have multiple clients
connected concurrently. ServerEndpoints also provide client authentication information.

Use ServerEndpoint.GetCurrentEndpoint() to retrieve the int32
current endpoint ID. Use ServerEndpoint.GetCurrentAuthenticatedUser() to retrieve
the current user authentication information.
"""

%feature("docstring") RobotRaconteur::ServerEndpoint::GetCurrentEndpoint() """
Returns the current server endpoint

Returns the current server endpoint during a request or packet event.
This is a thread-specific value and only valid during the initial
request or packet event invocation.

Throws InvalidOperationException if not during a request or packet event

:return: The current server endpoint id
:rtype: int
"""

%feature("docstring") RobotRaconteur::ServerEndpoint::GetCurrentAuthenticatedUser() """
Returns the current authenticated user

Users that have been authenticated have a corresponding
AuthenticatedUser object associated with the ServerEndpoint.
GetCurrentAuthenticatedUser() returns the AuthenticatedUser
associated with the current ServerEndpoint during a request
or packet event. This is a thread-specific value and only valid during 
the initial request or packet event invocation.

Throws PermissionDeniedException or AuthenticationException
if there is no AuthenticatedUser set in the current thread. 

:return: The AuthenticatedUser
:rtype: RobotRaconteur.AuthenticatedUser
"""

%feature("docstring") RobotRaconteur::AuthenticatedUser """
AuthenticatedUser()

Class representing an authenticated user

Use ServerEndpoint.GetCurrentAuthenticatedUser() to retrieve the
authenticated user making a request

See \\ref security for more information.
"""

%feature("docstring") RobotRaconteur::ServiceSecurityPolicy """
Security policy for Robot Raconteur service

The security policy sets an authenticator, and a set of policies. PasswordFileUserAuthenticator is
an example of an authenticator. The valid options for Policies are as follows:

+------------------+-----------------+-----------------+-----------------+
| Policy name      | Possible Values | Default         | Description     |
+==================+=================+=================+=================+
| requirevaliduser | true,false      | false           | Set to \"true\"   |
|                  |                 |                 | to require a    |
|                  |                 |                 | user be         |
|                  |                 |                 | authenticated   |
|                  |                 |                 | before          |
|                  |                 |                 | accessing       |
|                  |                 |                 | service         |
+------------------+-----------------+-----------------+-----------------+
| allowobjectlock  | true,false      | false           | If \"true\" allow |
|                  |                 |                 | users to        |
|                  |                 |                 | request object  |
|                  |                 |                 | locks.          |
|                  |                 |                 | r               |
|                  |                 |                 | equirevaliduser |
|                  |                 |                 | most also be    |
|                  |                 |                 | \"true\"          |
+------------------+-----------------+-----------------+-----------------+

The security policy is passed as a parameter to RobotRaconteurNode.RegisterService().

See \\ref security for more information.

:param Authenticator: The user authenticator
:type Authenticator: RobotRaconteur.Authenticator
:param Policies: The security policies
:type Policies: Dict[str,str]
"""

%feature("docstring") RobotRaconteur::UserAuthenticator """
NativeUserAuthenticator()

Base class for user authenticators

Used with ServiceSecurityPolicy to secure services

Override AuthenticateUser to implement different types
of user authenticators.

"""

%feature("docstring") RobotRaconteur::PasswordFileUserAuthenticator """
Simple authenticator using a list of username, password hash, and privileges stored in a file or string

The password user authenticator expects a string containing a list of users,
one per line. Each line contains the username, password as md5 hash, and privileges, separated by white spaces.
An example of authentication string contents:

.. code-block:: 

   user1 79e262a81dd19d40ae008f74eb59edce objectlock
   user2 309825a0951b3cf1f25e27b61cee8243 objectlock
   superuser1 11e5dfc68422e697563a4253ba360615 objectlock,objectlockoverride


The password is md5 hashed. This hash can be generated using the ``--md5passwordhash`` command in the \"RobotRaconteurGen\" utility. 
The privileges are comma separated. Valid privileges are as follows:

================== =====================================================
Privilege Name     Description
================== =====================================================
objectlock         Allow user to lock objects
objectlockoverride Allow user to unlock object locks made by other users
================== =====================================================

:param data: The file text
:type data: str
"""

%feature("docstring") RobotRaconteur::RRLogRecord """
Robot Raconteur log record

Records information about a logging event

See \\ref logging for more information.
"""

%feature("docstring") RobotRaconteur::RRLogRecord::Level """(int) The log level """
%feature("docstring") RobotRaconteur::RRLogRecord::Component """(int) The source component"""
%feature("docstring") RobotRaconteur::RRLogRecord::ComponentName """(str) The source component name"""
%feature("docstring") RobotRaconteur::RRLogRecord::ComponentObjectID """(str) The source component object ID"""
%feature("docstring") RobotRaconteur::RRLogRecord::Endpoint """(int) The source endpoint"""
%feature("docstring") RobotRaconteur::RRLogRecord::ServicePath """(str) The service path of the source object"""
%feature("docstring") RobotRaconteur::RRLogRecord::Member """(str) The source member"""
%feature("docstring") RobotRaconteur::RRLogRecord::Message """(str) Human readable log message"""
%feature("docstring") RobotRaconteur::RRLogRecord::Time """(datetime.datetime) Time of logging event"""
%feature("docstring") RobotRaconteur::RRLogRecord::SourceFile """(str) The sourcecode filename"""
%feature("docstring") RobotRaconteur::RRLogRecord::SourceLine """(int) The line within the sourcecode file"""
%feature("docstring") RobotRaconteur::RRLogRecord::ThreadID """(str) The source thread"""
%feature("docstring") RobotRaconteur::RRLogRecord::FiberID """(str) The source coroutine fiber"""

%feature("docstring") RobotRaconteur::LogRecordHandler """
LogRecordHandler()

Base class of log record handler

By default, RobotRaconteurNode will print log records to ``stderr``. Use
RobotRaconteur.RobotRaconteurNode.SetLogRecordHandler() to specify a LogRecordHandler
to accept log records instead of printing them to the terminal.

See \\ref logging for more information.
"""

%feature("docstring") RobotRaconteur::FileLogRecordHandler """
Log record handler that saves to a file

See \\ref logging for more information.
"""

%feature("docstring") RobotRaconteur::FileLogRecordHandler::OpenFile(const std::string& filename, bool append = true) """
Open a file to store log records

:param filename: The filename
:type filename: str
:param append: If True, log messages are appended. If False, the file is truncated when opened
:type append: bool
"""