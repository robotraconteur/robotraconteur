=========
Constants
=========


DataTypes Enum
==============

.. data:: RobotRaconteur.DataTypes_void_t  
   :value: 0

   void or null type

.. data:: RobotRaconteur.DataTypes_double_t
   :value: 1

   IEEE-754 64-bit floating point number

.. data:: RobotRaconteur.DataTypes_single_t
   :value: 2

   IEEE-754 32-bit floating point number

.. data:: RobotRaconteur.DataTypes_int8_t
   :value: 3

   8-bit signed integer

.. data:: RobotRaconteur.DataTypes_uint8_t
   :value: 4

   8-bit unsigned integer

.. data:: RobotRaconteur.DataTypes_int16_t
   :value: 5

   16-bit signed integer

.. data:: RobotRaconteur.DataTypes_uint16_t
   :value: 6

   16-bit unsigned integer

.. data:: RobotRaconteur.DataTypes_int32_t
   :value: 7

   32-bit signed integer

.. data:: RobotRaconteur.DataTypes_uint32_t
   :value: 8

   32-bit unsigned integer

.. data:: RobotRaconteur.DataTypes_int64_t
   :value: 9

   64-bit signed integer

.. data:: RobotRaconteur.DataTypes_uint64_t
   :value: 10

   64-bit unsigned integer

.. data:: RobotRaconteur.DataTypes_string_t
   :value: 11

   UTF-8 string

.. data:: RobotRaconteur.DataTypes_cdouble_t
   :value: 12

   128-bit complex double (real,imag)

.. data:: RobotRaconteur.DataTypes_csingle_t
   :value: 13

   64-bit complex float (real,imag)

.. data:: RobotRaconteur.DataTypes_bool_t
   :value: 14

   8-bit boolean

.. data:: RobotRaconteur.DataTypes_structure_t
   :value: 101

   structure (nested message type)

.. data:: RobotRaconteur.DataTypes_vector_t
   :value: 102

   map with int32 key (nested message type)

.. data:: RobotRaconteur.DataTypes_dictionary_t
   :value: 103

   map with string key (nested message type)

.. data:: RobotRaconteur.DataTypes_object_t
   :value: 104

   object type (not serializable)

.. data:: RobotRaconteur.DataTypes_varvalue_t
   :value: 105

   varvalue type (not serializable)

.. data:: RobotRaconteur.DataTypes_varobject_t
   :value: 106

   varobject type (not serializable)

.. data:: RobotRaconteur.DataTypes_list_t
   :value: 108

   list type (nested message type)

.. data:: RobotRaconteur.DataTypes_pod_t
   :value: 109

   pod type (nested message type)

.. data:: RobotRaconteur.DataTypes_pod_array_t
   :value: 110

   pod array type (nested message type)

.. data:: RobotRaconteur.DataTypes_pod_multidimarray_t
   :value: 111

   pod multidimarray type (nested message type)

.. data:: RobotRaconteur.DataTypes_enum_t
   :value: 112

   enum type (not serializable uses int32 for messages)

.. data:: RobotRaconteur.DataTypes_namedtype_t
   :value: 113

   namedtype definition (not serializable)

.. data:: RobotRaconteur.DataTypes_namedarray_t
   :value: 114

   namedarray type (not serializable)

.. data:: RobotRaconteur.DataTypes_namedarray_array_t
   :value: 115

   namedarray array type (nested message type)

.. data:: RobotRaconteur.DataTypes_namedarray_multidimarray_t
   :value: 116

   namedarray multidimarray type (nested message type)

.. data:: RobotRaconteur.DataTypes_multidimarray_t
   :value: 117

   multi-dimensional numeric array (nested message type)

.. data:: RobotRaconteur.DataTypes_ArrayTypes_none
   :value: 0

   type is not an array

.. data:: RobotRaconteur.DataTypes_ArrayTypes_array
   :value: 1

   type is a single dimensional array

.. data:: RobotRaconteur.DataTypes_ArrayTypes_multidimarray
   :value: 2

   type is a multidimensional array

.. data:: RobotRaconteur.DataTypes_ContainerTypes_none
   :value: 0

   type does not have a container

.. data:: RobotRaconteur.DataTypes_ContainerTypes_list
   :value: 1

   type has a list container

.. data:: RobotRaconteur.DataTypes_ContainerTypes_map_int32
   :value: 2

   type has a map with int32 keys container

.. data:: RobotRaconteur.DataTypes_ContainerTypes_map_string
   :value: 3

   type has a map with string keys container

.. data:: RobotRaconteur.DataTypes_ContainerTypes_generator
   :value: 4

   type has a generator container. Only valid for use with function generator members

MessageEntryType Enum
=====================

.. data:: RobotRaconteur.MessageEntryType_Null
   :value: 0

   no-op

.. data:: RobotRaconteur.MessageEntryType_StreamOp
   :value: 1

   Stream operation request (transport only)

.. data:: RobotRaconteur.MessageEntryType_StreamOpRet
   :value: 2

   Stream operation response (transport only)

.. data:: RobotRaconteur.MessageEntryType_StreamCheckCapability
   :value: 3

   Stream check capability request (transport only)

.. data:: RobotRaconteur.MessageEntryType_StreamCheckCapabilityRet
   :value: 4

   Stream check capability response (transport only)

.. data:: RobotRaconteur.MessageEntryType_GetServiceDesc
   :value: 101

   Get service definition request

.. data:: RobotRaconteur.MessageEntryType_GetServiceDescRet
   :value: 102

   Get service definition response

.. data:: RobotRaconteur.MessageEntryType_ObjectTypeName
   :value: 103

   Get object qualified type name request

.. data:: RobotRaconteur.MessageEntryType_ObjectTypeNameRet
   :value: 104

   Get object qualified type name response

.. data:: RobotRaconteur.MessageEntryType_ServiceClosed
   :value: 105

   Service closed notification packet

.. data:: RobotRaconteur.MessageEntryType_ServiceClosedRet
   :value: 106

   (reserved)

.. data:: RobotRaconteur.MessageEntryType_ConnectClient
   :value: 107

   Connect client request

.. data:: RobotRaconteur.MessageEntryType_ConnectClientRet
   :value: 108

   Connect client response

.. data:: RobotRaconteur.MessageEntryType_DisconnectClient
   :value: 109

   Disconnect client request

.. data:: RobotRaconteur.MessageEntryType_DisconnectClientRet
   :value: 110

   Disconnect client response

.. data:: RobotRaconteur.MessageEntryType_ConnectionTest
   :value: 111

   Ping request

.. data:: RobotRaconteur.MessageEntryType_ConnectionTestRet
   :value: 112

   Pong response

.. data:: RobotRaconteur.MessageEntryType_GetNodeInfo
   :value: 113

   Get node information request (NodeID and NodeName)

.. data:: RobotRaconteur.MessageEntryType_GetNodeInfoRet
   :value: 114

   Get node information response

.. data:: RobotRaconteur.MessageEntryType_ReconnectClient
   :value: 115

   (reserved)

.. data:: RobotRaconteur.MessageEntryType_ReconnectClientRet
   :value: 116

   (reserved)

.. data:: RobotRaconteur.MessageEntryType_NodeCheckCapability
   :value: 117

   Get node capability request

.. data:: RobotRaconteur.MessageEntryType_NodeCheckCapabilityRet
   :value: 118

   Get node capability response

.. data:: RobotRaconteur.MessageEntryType_GetServiceAttributes
   :value: 119

   Get service attributes request

.. data:: RobotRaconteur.MessageEntryType_GetServiceAttributesRet
   :value: 120

   Get service attributes response

.. data:: RobotRaconteur.MessageEntryType_ConnectClientCombined
   :value: 121

   Connect client combined operation request

.. data:: RobotRaconteur.MessageEntryType_ConnectClientCombinedRet
   :value: 122

   Connect client combined operation response

.. data:: RobotRaconteur.MessageEntryType_EndpointCheckCapability
   :value: 501

   Get endpoint capability request

.. data:: RobotRaconteur.MessageEntryType_EndpointCheckCapabilityRet       
   :value: 502

   Get endpoint capabalitiy response

.. data:: RobotRaconteur.MessageEntryType_ServiceCheckCapabilityReq        
   :value: 1101

   Get service capability request

.. data:: RobotRaconteur.MessageEntryType_ServiceCheckCapabilityRet        
   :value: 1102

   Get service capability response

.. data:: RobotRaconteur.MessageEntryType_ClientKeepAliveReq
   :value: 1105

   Client keep alive request

.. data:: RobotRaconteur.MessageEntryType_ClientKeepAliveRet
   :value: 1106

   Client keep alive response

.. data:: RobotRaconteur.MessageEntryType_ClientSessionOpReq
   :value: 1107

   Client session management operation request

.. data:: RobotRaconteur.MessageEntryType_ClientSessionOpRet
   :value: 1108

   Client session management operation response

.. data:: RobotRaconteur.MessageEntryType_ServicePathReleasedReq
   :value: 1109

   Service path released event notification packet

.. data:: RobotRaconteur.MessageEntryType_ServicePathReleasedRet
   :value: 1110

   (reserved)

.. data:: RobotRaconteur.MessageEntryType_PropertyGetReq
   :value: 1111

   Property member get request

.. data:: RobotRaconteur.MessageEntryType_PropertyGetRes
   :value: 1112

   Property member get response

.. data:: RobotRaconteur.MessageEntryType_PropertySetReq
   :value: 1113

   Property member set request

.. data:: RobotRaconteur.MessageEntryType_PropertySetRes
   :value: 1114

   Property member set response

.. data:: RobotRaconteur.MessageEntryType_FunctionCallReq
   :value: 1121

   Function member call request

.. data:: RobotRaconteur.MessageEntryType_FunctionCallRes
   :value: 1122

   Function member call response

.. data:: RobotRaconteur.MessageEntryType_GeneratorNextReq
   :value: 1123

   Generater next call request

.. data:: RobotRaconteur.MessageEntryType_GeneratorNextRes
   :value: 1124

   Generater next call response

.. data:: RobotRaconteur.MessageEntryType_EventReq
   :value: 1131

   Event member notification

.. data:: RobotRaconteur.MessageEntryType_EventRes
   :value: 1132

   (reserved)

.. data:: RobotRaconteur.MessageEntryType_PipePacket
   :value: 1141

   Pipe member packet

.. data:: RobotRaconteur.MessageEntryType_PipePacketRet
   :value: 1142

   Pipe member packet ack

.. data:: RobotRaconteur.MessageEntryType_PipeConnectReq
   :value: 1143

   Pipe member connect request

.. data:: RobotRaconteur.MessageEntryType_PipeConnectRet
   :value: 1144

   Pipe member connect response

.. data:: RobotRaconteur.MessageEntryType_PipeDisconnectReq
   :value: 1145

   Pipe member close request

.. data:: RobotRaconteur.MessageEntryType_PipeDisconnectRet
   :value: 1146

   Pipe member close response

.. data:: RobotRaconteur.MessageEntryType_PipeClosed
   :value: 1147

   Pipe member closed event notification packet

.. data:: RobotRaconteur.MessageEntryType_PipeClosedRet
   :value: 1148

   (reserved)

.. data:: RobotRaconteur.MessageEntryType_CallbackCallReq
   :value: 1151

   Callback member call request

.. data:: RobotRaconteur.MessageEntryType_CallbackCallRet
   :value: 1152

   Callback member call response

.. data:: RobotRaconteur.MessageEntryType_WirePacket
   :value: 1161

   Wire member value packet

.. data:: RobotRaconteur.MessageEntryType_WirePacketRet
   :value: 1162

   (reserved)

.. data:: RobotRaconteur.MessageEntryType_WireConnectReq
   :value: 1163

   Wire member connect request

.. data:: RobotRaconteur.MessageEntryType_WireConnectRet
   :value: 1164

   Wire member connect response

.. data:: RobotRaconteur.MessageEntryType_WireDisconnectReq
   :value: 1165

   Wire member close request

.. data:: RobotRaconteur.MessageEntryType_WireDisconnectRet
   :value: 1166

   Wire member close response

.. data:: RobotRaconteur.MessageEntryType_WireClosed
   :value: 1167

   Wire member closed event notification packet

.. data:: RobotRaconteur.MessageEntryType_WireClosedRet
   :value: 1168

   (reserved)

.. data:: RobotRaconteur.MessageEntryType_MemoryRead
   :value: 1171

   Memory member read request

.. data:: RobotRaconteur.MessageEntryType_MemoryReadRet
   :value: 1172

   Memory member read response

.. data:: RobotRaconteur.MessageEntryType_MemoryWrite
   :value: 1173

   Memory member write request

.. data:: RobotRaconteur.MessageEntryType_MemoryWriteRet
   :value: 1174

   Memory member write response

.. data:: RobotRaconteur.MessageEntryType_MemoryGetParam
   :value: 1175

   Memory member get param request

.. data:: RobotRaconteur.MessageEntryType_MemoryGetParamRet
   :value: 1176

   Memory member get param response

.. data:: RobotRaconteur.MessageEntryType_WirePeekInValueReq
   :value: 1181

   Wire member peek InValue request

.. data:: RobotRaconteur.MessageEntryType_WirePeekInValueRet
   :value: 1182

   Wire member peek InValue response

.. data:: RobotRaconteur.MessageEntryType_WirePeekOutValueReq
   :value: 1183

   Wire member peek OutValue request

.. data:: RobotRaconteur.MessageEntryType_WirePeekOutValueRet
   :value: 1184

   Wire member peek OutValue response

.. data:: RobotRaconteur.MessageEntryType_WirePokeOutValueReq
   :value: 1185

   Wire member poke OutValue request

.. data:: RobotRaconteur.MessageEntryType_WirePokeOutValueRet
   :value: 1186

   Wire member poke OutValue response

MessageErrorType Enum
=====================

.. data:: RobotRaconteur.MessageErrorType_None
   :value: 0

   success

.. data:: RobotRaconteur.MessageErrorType_ConnectionError
   :value: 1

   connection error

.. data:: RobotRaconteur.MessageErrorType_ProtocolError
   :value: 2

   protocol error serializing messages

.. data:: RobotRaconteur.MessageErrorType_ServiceNotFound
   :value: 3

   specified service not found

.. data:: RobotRaconteur.MessageErrorType_ObjectNotFound
   :value: 4

   specified object not found

.. data:: RobotRaconteur.MessageErrorType_InvalidEndpoint
   :value: 5

   specified endpoint not found

.. data:: RobotRaconteur.MessageErrorType_EndpointCommunicationFatalError  
   :value: 6

   communication with specified endpoint failed

.. data:: RobotRaconteur.MessageErrorType_NodeNotFound
   :value: 7

   specified node not found

.. data:: RobotRaconteur.MessageErrorType_ServiceError
   :value: 8

   service error

.. data:: RobotRaconteur.MessageErrorType_MemberNotFound
   :value: 9

   specified member not found

.. data:: RobotRaconteur.MessageErrorType_MemberFormatMismatch
   :value: 10

   message format incompatible with specified member

.. data:: RobotRaconteur.MessageErrorType_DataTypeMismatch
   :value: 11

   data type did not match expected type

.. data:: RobotRaconteur.MessageErrorType_DataTypeError
   :value: 12

   data type failure

.. data:: RobotRaconteur.MessageErrorType_DataSerializationError
   :value: 13

   failure serializing data type

.. data:: RobotRaconteur.MessageErrorType_MessageEntryNotFound
   :value: 14

   specified message entry not found

.. data:: RobotRaconteur.MessageErrorType_MessageElementNotFound
   :value: 15

   specified message element not found

.. data:: RobotRaconteur.MessageErrorType_UnknownError
   :value: 16

   unknown exception occurred check `error name`

.. data:: RobotRaconteur.MessageErrorType_InvalidOperation
   :value: 17

   invalid operation attempted

.. data:: RobotRaconteur.MessageErrorType_InvalidArgument
   :value: 18

   argument is invalid

.. data:: RobotRaconteur.MessageErrorType_OperationFailed
   :value: 19

   the requested operation failed

.. data:: RobotRaconteur.MessageErrorType_NullValue
   :value: 20

   invalid null value

.. data:: RobotRaconteur.MessageErrorType_InternalError
   :value: 21

   internal error

.. data:: RobotRaconteur.MessageErrorType_SystemResourcePermissionDenied   
   :value: 22

   permission denied to a system resource

.. data:: RobotRaconteur.MessageErrorType_OutOfSystemResource
   :value: 23

   system resource has been exhausted

.. data:: RobotRaconteur.MessageErrorType_SystemResourceError
   :value: 24

   system resource error

.. data:: RobotRaconteur.MessageErrorType_ResourceNotFound
   :value: 25

   a required resource was not found

.. data:: RobotRaconteur.MessageErrorType_IOError
   :value: 26

   input/output error

.. data:: RobotRaconteur.MessageErrorType_BufferLimitViolation
   :value: 27

   a buffer underrun/overrun has occurred

.. data:: RobotRaconteur.MessageErrorType_ServiceDefinitionError
   :value: 28

   service definition parse or validation error

.. data:: RobotRaconteur.MessageErrorType_OutOfRange
   :value: 29

   attempt to access an out of range element

.. data:: RobotRaconteur.MessageErrorType_KeyNotFound
   :value: 30

   key not found

.. data:: RobotRaconteur.MessageErrorType_InvalidConfiguration
   :value: 31

   invalid configuration specified

.. data:: RobotRaconteur.MessageErrorType_InvalidState
   :value: 32

   invalid state

.. data:: RobotRaconteur.MessageErrorType_RemoteError
   :value: 100

   error occurred on remote node

.. data:: RobotRaconteur.MessageErrorType_RequestTimeout
   :value: 101

   request timed out

.. data:: RobotRaconteur.MessageErrorType_ReadOnlyMember
   :value: 102

   attempt to write to a read only member

.. data:: RobotRaconteur.MessageErrorType_WriteOnlyMember
   :value: 103

   attempt to read a write only member

.. data:: RobotRaconteur.MessageErrorType_NotImplementedError
   :value: 104

   member not implemented

.. data:: RobotRaconteur.MessageErrorType_MemberBusy
   :value: 105

   member is busy try again

.. data:: RobotRaconteur.MessageErrorType_ValueNotSet
   :value: 106

   value has not been set

.. data:: RobotRaconteur.MessageErrorType_AbortOperation
   :value: 107

   abort operation (generator only)

.. data:: RobotRaconteur.MessageErrorType_OperationAborted
   :value: 108

   the operation has been aborted

.. data:: RobotRaconteur.MessageErrorType_StopIteration
   :value: 109

   stop generator iteration (generator only)

.. data:: RobotRaconteur.MessageErrorType_OperationTimeout
   :value: 110

   the operation has timed out

.. data:: RobotRaconteur.MessageErrorType_OperationCancelled
   :value: 111

   the operation has been cancelled

.. data:: RobotRaconteur.MessageErrorType_AuthenticationError
   :value: 150

   authentication has failed

.. data:: RobotRaconteur.MessageErrorType_ObjectLockedError
   :value: 151

   the object is locked by another user or session

.. data:: RobotRaconteur.MessageErrorType_PermissionDenied
   :value: 152

   permission to service object or resource denied

ClientServiceListenerEventType Enum
===================================

.. data:: RobotRaconteur.ClientServiceListenerEventType_ClientClosed       
   :value: 1

   client has been closed

.. data:: RobotRaconteur.ClientServiceListenerEventType_ClientConnectionTimeout
   :value: 2

   client connection has timed out

.. data:: RobotRaconteur.ClientServiceListenerEventType_TransportConnectionConnected
   :value: 3

   client transport has been connected

.. data:: RobotRaconteur.ClientServiceListenerEventType_TransportConnectionClosed
   :value: 4

   client transport connection has been closed or lost

.. data:: RobotRaconteur.ClientServiceListenerEventType_ServicePathReleased   :value: 5

   client has received notification that service path was released

ServerServiceListenerEventType Enum
===================================

.. data:: RobotRaconteur.ServerServiceListenerEventType_ServiceClosed      
   :value: 1

   service has been closed

.. data:: RobotRaconteur.ServerServiceListenerEventType_ClientConnected    
   :value: 2

   client has connected

.. data:: RobotRaconteur.ServerServiceListenerEventType_ClientDisconnected 
   :value: 3

   client has disconnected

MemberDefinition Enum
=====================

.. data:: RobotRaconteur.MemberDefinition_Direction_both
   :value: 0

   member supports read and write

.. data:: RobotRaconteur.MemberDefinition_Direction_readonly
   :value: 1

   member is readonly

.. data:: RobotRaconteur.MemberDefinition_Direction_writeonly
   :value: 2

   member is writeonly

.. data:: RobotRaconteur.MemberDefinition_NoLock_none
   :value: 0

   member cannot be accessed by other users/sessions when object is locked 

.. data:: RobotRaconteur.MemberDefinition_NoLock_all
   :value: 1

   member can be accessed by other users/sessions when object is locked    

.. data:: RobotRaconteur.MemberDefinition_NoLock_read
   :value: 2

   member can be read by other users/sessions when object is locked        

Misc Constants
==============

.. data:: RobotRaconteur.RR_TIMEOUT_INFINITE
   :value: -1

   Disable timeout for asynchronous operations

.. data:: RobotRaconteur.RR_VALUE_LIFESPAN_INFINITE
   :value: -1

   Set wire values to have infinite lifespan and will not expire

RobotRaconteurObjectLockFlags Enum
==================================

.. data:: RobotRaconteur.RobotRaconteurObjectLockFlags_USER_LOCK
   :value: 0

   User level lock

	The object will be accesible for all client connections
	authenticated by the current user

.. data:: RobotRaconteur.RobotRaconteurObjectLockFlags_CLIENT_LOCK
   :value: 1

   Client level lock

	Only the current client connection will have access 
	to the locked object
