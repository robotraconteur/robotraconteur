/**
 * @file RobotRaconteurConstants.h
 *
 * @author John Wason, PhD
 *
 * @copyright Copyright 2011-2020 Wason Technology, LLC
 *
 * @par License
 * Software License Agreement (Apache License)
 * @par
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * @par
 * http://www.apache.org/licenses/LICENSE-2.0
 * @par
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <stdint.h>

namespace RobotRaconteur
{

/**
 * @brief Type codes for types supported by Robot Raconteur
 *
 * Data type codes are used in messages and service definition
 * parsers.
 *
 * Data is always stored as little-endian, except for UUID which are big endian
 *
 */
enum DataTypes
{
    /** @brief void or null type */
    DataTypes_void_t = 0,
    /** @brief IEEE-754 64-bit floating point number */
    DataTypes_double_t,
    /** @brief IEEE-754 32-bit floating point number */
    DataTypes_single_t,
    /** @brief 8-bit signed integer */
    DataTypes_int8_t,
    /** @brief 8-bit unsigned integer */
    DataTypes_uint8_t,
    /** @brief 16-bit signed integer */
    DataTypes_int16_t,
    /** @brief 16-bit unsigned integer */
    DataTypes_uint16_t,
    /** @brief 32-bit signed integer */
    DataTypes_int32_t,
    /** @brief 32-bit unsigned integer */
    DataTypes_uint32_t,
    /** @brief 64-bit signed integer */
    DataTypes_int64_t,
    /** @brief 64-bit unsigned integer */
    DataTypes_uint64_t,
    /** @brief UTF-8 string */
    DataTypes_string_t,
    /** @brief 128-bit complex double (real,imag) */
    DataTypes_cdouble_t,
    /** @brief 64-bit complex float (real,imag) */
    DataTypes_csingle_t,
    /** @brief 8-bit boolean */
    DataTypes_bool_t,
    /** @brief structure (nested message type) */
    DataTypes_structure_t = 101,
    /** @brief map with int32 key (nested message type) */
    DataTypes_vector_t,
    /** @brief map with string key (nested message type) */
    DataTypes_dictionary_t,
    /** @brief object type (not serializable) */
    DataTypes_object_t,
    /** @brief varvalue type (not serializable) */
    DataTypes_varvalue_t,
    /** @brief varobject type (not serializable) */
    DataTypes_varobject_t,
    // DataTypes_multidimarray_t, - Deprecated!
    /** @brief list type (nested message type) */
    DataTypes_list_t = 108,
    /** @brief pod type (nested message type) */
    DataTypes_pod_t,
    /** @brief pod array type (nested message type) */
    DataTypes_pod_array_t,
    /** @brief pod multidimarray type (nested message type) */
    DataTypes_pod_multidimarray_t,
    /** @brief enum type (not serializable, uses int32 for messages) */
    DataTypes_enum_t,
    /** @brief namedtype definition (not serializable) */
    DataTypes_namedtype_t,
    /** @brief namedarray type (not serializable) */
    DataTypes_namedarray_t,
    /** @brief namedarray array type (nested message type) */
    DataTypes_namedarray_array_t,
    /** @brief namedarray multidimarray type (nested message type) */
    DataTypes_namedarray_multidimarray_t,
    /** @brief multi-dimensional numeric array (nested message type) */
    DataTypes_multidimarray_t // Use a new data type code for numeric arrays to avoid confusion
};

/**
 * @brief Array type enum for TypeDefinition parser class
 *
 */
enum DataTypes_ArrayTypes
{
    /** @brief type is not an array */
    DataTypes_ArrayTypes_none = 0,
    /** @brief type is a single dimensional array */
    DataTypes_ArrayTypes_array,
    /** @brief type is a multidimensional array */
    DataTypes_ArrayTypes_multidimarray
};

/**
 * @brief Container type enum for TypeDefinition parser class
 *
 */
enum DataTypes_ContainerTypes
{
    /** @brief type does not have a container */
    DataTypes_ContainerTypes_none = 0,
    /** @brief type has a list container */
    DataTypes_ContainerTypes_list,
    /** @brief type has a map with int32 keys container */
    DataTypes_ContainerTypes_map_int32,
    /** @brief type has a map with string keys container */
    DataTypes_ContainerTypes_map_string,
    /**
     * @brief type has a generator container
     *
     * Only valid for use with function generator members
     */
    DataTypes_ContainerTypes_generator
};

// Flags for MessageFlags entry in MessageHeader

/** @brief Message contains ROUTING_INFO section */
const uint8_t MessageFlags_ROUTING_INFO = 0x01;
/** @brief Message contains ENDPOINT_INFO section */
const uint8_t MessageFlags_ENDPOINT_INFO = 0x02;
/** @brief Message contains PRIORITY section */
const uint8_t MessageFlags_PRIORITY = 0x04;
/** @brief Message is unreliable and may be dropped */
const uint8_t MessageFlags_UNRELIABLE = 0x08;
/** @brief Message contains META_INFO section */
const uint8_t MessageFlags_META_INFO = 0x10;
/** @brief Message contains STRING_TABLE section */
const uint8_t MessageFlags_STRING_TABLE = 0x20;
/** @brief Message contains MULTIPLE_ENTRIES section. If unset, message contains one entry */
const uint8_t MessageFlags_MULTIPLE_ENTRIES = 0x40;
/** @brief Message contains EXTENDED section */
const uint8_t MessageFlags_EXTENDED = 0x80;

/** @brief Message flags for compatibility with Message Format Version 2 */
const uint8_t MessageFlags_Version2Compat =
    MessageFlags_ROUTING_INFO | MessageFlags_ENDPOINT_INFO | MessageFlags_META_INFO | MessageFlags_MULTIPLE_ENTRIES;

// Flags for EntryFlags in MessageEntry
/** @brief MessageEntry contains SERVICE_PATH_STR section */
const uint8_t MessageEntryFlags_SERVICE_PATH_STR = 0x01;
/** @brief MessageEntry contains SERVICE_PATH_CODE section */
const uint8_t MessageEntryFlags_SERVICE_PATH_CODE = 0x02;
/** @brief MessageEntry contains MEMBER_NAME_STR section */
const uint8_t MessageEntryFlags_MEMBER_NAME_STR = 0x04;
/** @brief MessageEntry contains MEMBER_NAME_CODE section */
const uint8_t MessageEntryFlags_MEMBER_NAME_CODE = 0x08;
/** @brief MessageEntry contains REQUEST_ID section */
const uint8_t MessageEntryFlags_REQUEST_ID = 0x10;
/** @brief MessageEntry contains ERROR section */
const uint8_t MessageEntryFlags_ERROR = 0x20;
/** @brief MessageEntry contains META_INFO section */
const uint8_t MessageEntryFlags_META_INFO = 0x40;
/** @brief MessageEntry contains EXTENDED section */
const uint8_t MessageEntryFlags_EXTENDED = 0x80;

/** @brief MessageEntry flags for compatibility with Message Format Version 2 */
const uint8_t MessageEntryFlags_Version2Compat = MessageEntryFlags_SERVICE_PATH_STR |
                                                 MessageEntryFlags_MEMBER_NAME_STR | MessageEntryFlags_REQUEST_ID |
                                                 MessageEntryFlags_ERROR | MessageEntryFlags_META_INFO;

// Flags for ElementFlags in MessageElement
/** @brief MessageElement contains ELEMENT_NAME_STR section */
const uint8_t MessageElementFlags_ELEMENT_NAME_STR = 0x01;
/** @brief MessageElement contains ELEMENT_NAME_CODE section */
const uint8_t MessageElementFlags_ELEMENT_NAME_CODE = 0x02;
/** @brief MessageElement contains ELEMENT_NUMBER section */
const uint8_t MessageElementFlags_ELEMENT_NUMBER = 0x04;
/** @brief MessageElement contains ELEMENT_TYPE_NAME_STR section */
const uint8_t MessageElementFlags_ELEMENT_TYPE_NAME_STR = 0x08;
/** @brief MessageElement contains ELEMENT_TYPE_NAME_CODE section */
const uint8_t MessageElementFlags_ELEMENT_TYPE_NAME_CODE = 0x10;
/** @brief MessageElement contains META_INFO section */
const uint8_t MessageElementFlags_META_INFO = 0x20;
/** @brief MessageElement contains EXTENDED section */
const uint8_t MessageElementFlags_EXTENDED = 0x80;

/** @brief MessageElement flags for compatibility with Message Format Version 2 */
const uint8_t MessageElementFlags_Version2Compat =
    MessageElementFlags_ELEMENT_NAME_STR | MessageElementFlags_ELEMENT_TYPE_NAME_STR | MessageElementFlags_META_INFO;

/**
 * @brief Message entry type codes
 *
 * Message entries are sent between nodes stored in messages, and represent
 * requests, responses, or packets. The type of the entry is specified through
 * the message entry type code. These type codes are similar to op-codes. This
 * enum contains the defined entry type codes.
 *
 * Odd codes represent requests or packets, even codes
 * represent responses.
 *
 * Entry types less than 500 are considered "special requests" that can be used
 * before a session is established.
 *
 */
enum MessageEntryType
{
    /** @brief no-op */
    MessageEntryType_Null = 0,
    /** @brief Stream operation request (transport only) */
    MessageEntryType_StreamOp = 1,
    /** @brief Stream operation response (transport only) */
    MessageEntryType_StreamOpRet,
    /** @brief Stream check capability request (transport only) */
    MessageEntryType_StreamCheckCapability,
    /** @brief Stream check capability response (transport only) */
    MessageEntryType_StreamCheckCapabilityRet,
    // MessageEntryType_StringTableOp, - Deprecated!
    // MessageEntryType_StringTableOpRet, - Deprecated!
    /** @brief Get service definition request */
    MessageEntryType_GetServiceDesc = 101,
    /** @brief Get service definition response */
    MessageEntryType_GetServiceDescRet,
    /** @brief Get object qualified type name request */
    MessageEntryType_ObjectTypeName,
    /** @brief Get object qualified type name response */
    MessageEntryType_ObjectTypeNameRet,
    /** @brief Service closed notification packet */
    MessageEntryType_ServiceClosed,
    /** @brief (reserved) */
    MessageEntryType_ServiceClosedRet,
    /** @brief Connect client request */
    MessageEntryType_ConnectClient,
    /** @brief Connect client response */
    MessageEntryType_ConnectClientRet,
    /** @brief Disconnect client request */
    MessageEntryType_DisconnectClient,
    /** @brief Disconnect client response */
    MessageEntryType_DisconnectClientRet,
    /** @brief Ping request */
    MessageEntryType_ConnectionTest,
    /** @brief Pong response */
    MessageEntryType_ConnectionTestRet,
    /** @brief Get node information request (NodeID and NodeName) */
    MessageEntryType_GetNodeInfo,
    /** @brief Get node information response */
    MessageEntryType_GetNodeInfoRet,
    /** @brief (reserved) */
    MessageEntryType_ReconnectClient,
    /** @brief (reserved) */
    MessageEntryType_ReconnectClientRet,
    /** @brief Get node capability request */
    MessageEntryType_NodeCheckCapability,
    /** @brief Get node capability response */
    MessageEntryType_NodeCheckCapabilityRet,
    /** @brief Get service attributes request */
    MessageEntryType_GetServiceAttributes,
    /** @brief Get service attributes response */
    MessageEntryType_GetServiceAttributesRet,
    /** @brief Connect client combined operation request */
    MessageEntryType_ConnectClientCombined,
    /** @brief Connect client combined operation response */
    MessageEntryType_ConnectClientCombinedRet,
    /** @brief Get endpoint capability request */
    MessageEntryType_EndpointCheckCapability = 501,
    /** @brief Get endpoint capabalitiy response */
    MessageEntryType_EndpointCheckCapabilityRet,
    /** @brief Get service capability request */
    MessageEntryType_ServiceCheckCapabilityReq = 1101,
    /** @brief Get service capability response */
    MessageEntryType_ServiceCheckCapabilityRet,
    /** @brief Client keep alive request */
    MessageEntryType_ClientKeepAliveReq = 1105,
    /** @brief Client keep alive response */
    MessageEntryType_ClientKeepAliveRet,
    /** @brief Client session management operation request */
    MessageEntryType_ClientSessionOpReq = 1107,
    /** @brief Client session management operation response */
    MessageEntryType_ClientSessionOpRet,
    /** @brief Service path released event notification packet */
    MessageEntryType_ServicePathReleasedReq,
    /** @brief (reserved) */
    MessageEntryType_ServicePathReleasedRet,
    /** @brief Property member get request */
    MessageEntryType_PropertyGetReq = 1111,
    /** @brief Property member get response */
    MessageEntryType_PropertyGetRes,
    /** @brief Property member set request */
    MessageEntryType_PropertySetReq,
    /** @brief Property member set response */
    MessageEntryType_PropertySetRes,
    /** @brief Function member call request */
    MessageEntryType_FunctionCallReq = 1121,
    /** @brief Function member call response */
    MessageEntryType_FunctionCallRes,
    /** @brief Generater next call request */
    MessageEntryType_GeneratorNextReq,
    /** @brief Generater next call response */
    MessageEntryType_GeneratorNextRes,
    /** @brief Event member notification */
    MessageEntryType_EventReq = 1131,
    /** @brief (reserved) */
    MessageEntryType_EventRes,
    /** @brief Pipe member packet */
    MessageEntryType_PipePacket = 1141,
    /** @brief Pipe member packet ack */
    MessageEntryType_PipePacketRet,
    /** @brief Pipe member connect request */
    MessageEntryType_PipeConnectReq,
    /** @brief Pipe member connect response */
    MessageEntryType_PipeConnectRet,
    /** @brief Pipe member close request */
    MessageEntryType_PipeDisconnectReq,
    /** @brief Pipe member close response */
    MessageEntryType_PipeDisconnectRet,
    /** @brief Pipe member closed event notification packet */
    MessageEntryType_PipeClosed,
    /** @brief (reserved) */
    MessageEntryType_PipeClosedRet,
    /** @brief Callback member call request */
    MessageEntryType_CallbackCallReq = 1151,
    /** @brief Callback member call response */
    MessageEntryType_CallbackCallRet,
    /** @brief Wire member value packet */
    MessageEntryType_WirePacket = 1161,
    /** @brief (reserved) */
    MessageEntryType_WirePacketRet,
    /** @brief Wire member connect request */
    MessageEntryType_WireConnectReq,
    /** @brief Wire member connect response */
    MessageEntryType_WireConnectRet,
    /** @brief Wire member close request */
    MessageEntryType_WireDisconnectReq,
    /** @brief Wire member close response */
    MessageEntryType_WireDisconnectRet,
    /** @brief Wire member closed event notification packet */
    MessageEntryType_WireClosed,
    /** @brief (reserved) */
    MessageEntryType_WireClosedRet,
    /** @brief Memory member read request */
    MessageEntryType_MemoryRead = 1171,
    /** @brief Memory member read response */
    MessageEntryType_MemoryReadRet,
    /** @brief Memory member write request */
    MessageEntryType_MemoryWrite,
    /** @brief Memory member write response */
    MessageEntryType_MemoryWriteRet,
    /** @brief Memory member get param request */
    MessageEntryType_MemoryGetParam,
    /** @brief Memory member get param response */
    MessageEntryType_MemoryGetParamRet,
    /** @brief Wire member peek InValue request */
    MessageEntryType_WirePeekInValueReq = 1181,
    /** @brief Wire member peek InValue response */
    MessageEntryType_WirePeekInValueRet,
    /** @brief Wire member peek OutValue request */
    MessageEntryType_WirePeekOutValueReq,
    /** @brief Wire member peek OutValue response */
    MessageEntryType_WirePeekOutValueRet,
    /** @brief Wire member poke OutValue request */
    MessageEntryType_WirePokeOutValueReq,
    /** @brief Wire member poke OutValue response */
    MessageEntryType_WirePokeOutValueRet,

    // Dedicated transport message types.  These are an extension to the protocol
    // and not a base feature
    MessageEntryType_WireTransportOpReq = 11161,
    MessageEntryType_WireTransportOpRet,
    MessageEntryType_WireTransportEvent,
    MessageEntryType_WireTransportEventRet
};

/**
 * @brief Message error type codes enum
 *
 */
enum MessageErrorType
{
    /** @brief success */
    MessageErrorType_None = 0,
    /** @brief connection error */
    MessageErrorType_ConnectionError = 1,
    /** @brief protocol error serializing messages */
    MessageErrorType_ProtocolError,
    /** @brief specified service not found */
    MessageErrorType_ServiceNotFound,
    /** @brief specified object not found */
    MessageErrorType_ObjectNotFound,
    /** @brief specified endpoint not found */
    MessageErrorType_InvalidEndpoint,
    /** @brief communication with specified endpoint failed */
    MessageErrorType_EndpointCommunicationFatalError,
    /** @brief specified node not found */
    MessageErrorType_NodeNotFound,
    /** @brief service error */
    MessageErrorType_ServiceError,
    /** @brief specified member not found */
    MessageErrorType_MemberNotFound,
    /** @brief message format incompatible with specified member */
    MessageErrorType_MemberFormatMismatch,
    /** @brief data type did not match expected type */
    MessageErrorType_DataTypeMismatch,
    /** @brief data type failure */
    MessageErrorType_DataTypeError,
    /** @brief failure serializing data type */
    MessageErrorType_DataSerializationError,
    /** @brief specified message entry not found */
    MessageErrorType_MessageEntryNotFound,
    /** @brief specified message element not found */
    MessageErrorType_MessageElementNotFound,
    /** @brief unknown exception occurred, check `error name` */
    MessageErrorType_UnknownError,
    /** @brief invalid operation attempted */
    MessageErrorType_InvalidOperation,
    /** @brief argument is invalid */
    MessageErrorType_InvalidArgument,
    /** @brief the requested operation failed */
    MessageErrorType_OperationFailed,
    /** @brief invalid null value */
    MessageErrorType_NullValue,
    /** @brief internal error */
    MessageErrorType_InternalError,
    /** @brief permission denied to a system resource */
    MessageErrorType_SystemResourcePermissionDenied,
    /** @brief system resource has been exhausted */
    MessageErrorType_OutOfSystemResource,
    /** @brief system resource error */
    MessageErrorType_SystemResourceError,
    /** @brief a required resource was not found */
    MessageErrorType_ResourceNotFound,
    /** @brief input/output error */
    MessageErrorType_IOError,
    /** @brief a buffer underrun/overrun has occurred */
    MessageErrorType_BufferLimitViolation,
    /** @brief service definition parse or validation error */
    MessageErrorType_ServiceDefinitionError,
    /** @brief attempt to access an out of range element */
    MessageErrorType_OutOfRange,
    /** @brief key not found */
    MessageErrorType_KeyNotFound,
    /** @brief invalid configuration specified */
    MessageErrorType_InvalidConfiguration,
    /** @brief invalid state */
    MessageErrorType_InvalidState,
    /** @brief error occurred on remote node */
    MessageErrorType_RemoteError = 100,
    /** @brief request timed out */
    MessageErrorType_RequestTimeout,
    /** @brief attempt to write to a read only member */
    MessageErrorType_ReadOnlyMember,
    /** @brief attempt to read a write only member */
    MessageErrorType_WriteOnlyMember,
    /** @brief member not implemented */
    MessageErrorType_NotImplementedError,
    /** @brief member is busy, try again */
    MessageErrorType_MemberBusy,
    /** @brief value has not been set */
    MessageErrorType_ValueNotSet,
    /** @brief abort operation (generator only) */
    MessageErrorType_AbortOperation,
    /** @brief the operation has been aborted */
    MessageErrorType_OperationAborted,
    /** @brief stop generator iteration (generator only) */
    MessageErrorType_StopIteration,
    /** @brief the operation has timed out */
    MessageErrorType_OperationTimeout,
    /** @brief the operation has been cancelled */
    MessageErrorType_OperationCancelled,
    /** @brief authentication has failed */
    MessageErrorType_AuthenticationError = 150,
    /** @brief the object is locked by another user or session */
    MessageErrorType_ObjectLockedError,
    /** @brief permission to service object or resource denied */
    MessageErrorType_PermissionDenied
};

/**
 * @brief Enum of client listener events
 *
 */
enum ClientServiceListenerEventType
{
    /** @brief client has been closed */
    ClientServiceListenerEventType_ClientClosed = 1,
    /** @brief client connection has timed out */
    ClientServiceListenerEventType_ClientConnectionTimeout,
    /** @brief client transport has been connected */
    ClientServiceListenerEventType_TransportConnectionConnected,
    /** @brief client transport connection has been closed or lost */
    ClientServiceListenerEventType_TransportConnectionClosed,
    /** @brief client has received notification that service path was released */
    ClientServiceListenerEventType_ServicePathReleased
};

/**
 * @brief Enum of service listener events
 *
 */
enum ServerServiceListenerEventType
{
    /** @brief service has been closed */
    ServerServiceListenerEventType_ServiceClosed = 1,
    /** @brief client has connected */
    ServerServiceListenerEventType_ClientConnected,
    /** @brief client has disconnected */
    ServerServiceListenerEventType_ClientDisconnected
};

/**
 * @brief Member direction enum
 *
 * Use member modifiers to declare member direction (readonly,writeonly)
 *
 */
enum MemberDefinition_Direction
{
    /** @brief member supports read and write */
    MemberDefinition_Direction_both = 0,
    /** @brief member is readonly */
    MemberDefinition_Direction_readonly,
    /** @brief member is writeonly */
    MemberDefinition_Direction_writeonly,
};

/**
 * @brief Member locking options enum
 *
 * Use member modifiers to declare lock options
 *
 */
enum MemberDefinition_NoLock
{
    /** @brief member cannot be accessed by other users/sessions when object is locked */
    MemberDefinition_NoLock_none = 0,
    /** @brief member can be accessed by other users/sessions when object is locked */
    MemberDefinition_NoLock_all,
    /** @brief member can be read by other users/sessions when object is locked */
    MemberDefinition_NoLock_read
};

/**
 * @brief Disable timeout for asynchronous operations
 *
 * Pass `RR_TIMEOUT_INFINITE` for the `timeout` parameter in asynchronous
 * operations to disable timeout for that operation.
 *
 */
#define RR_TIMEOUT_INFINITE -1

/**
 * @brief Set wire values to have infinite lifespan and will not expire
 *
 * Wire values may optionally expire after a given period in milliseconds.
 * Set `RR_TIMEOUT_INFINITE` to disable value expiration.
 */
#define RR_VALUE_LIFESPAN_INFINITE -1

/** @brief Page mask for transport capability code */
const uint32_t TranspartCapabilityCode_PAGE_MASK = 0xFFF00000;
/** @brief Message Version 2 transport capability page code */
const uint32_t TransportCapabilityCode_MESSAGE2_BASIC_PAGE = 0x02000000;
/** @brief Enable Message Version 2 transport capability flag */
const uint32_t TransportCapabilityCode_MESSAGE2_BASIC_ENABLE = 0x00000001;
/** @brief Enable Message Version 2 connect combined transport capability flag */
const uint32_t TransportCapabilityCode_MESSAGE2_BASIC_CONNECTCOMBINED = 0x00000002;
/** @brief Message Version 4 transport capability page code */
const uint32_t TransportCapabilityCode_MESSAGE4_BASIC_PAGE = 0x04000000;
/** @brief Enable Message Version 4 transport capability flag */
const uint32_t TransportCapabilityCode_MESSAGE4_BASIC_ENABLE = 0x00000001;
/** @brief Enable Message Version 4 connect combine transport capability flag */
const uint32_t TransportCapabilityCode_MESSAGE4_BASIC_CONNECTCOMBINED = 0x00000002;
/** @brief Message Version 4 String Table capability page code */
const uint32_t TransportCapabilityCode_MESSAGE4_STRINGTABLE_PAGE = 0x04100000;
/** @brief Enable Message Version 4 String Table transport capability code */
const uint32_t TransportCapabilityCode_MESSAGE4_STRINGTABLE_ENABLE = 0x00000001;
/** @brief Enable Message Version 4 local String Table capability code */
const uint32_t TransportCapabilityCode_MESSAGE4_STRINGTABLE_MESSAGE_LOCAL = 0x00000002;
/** @brief Enable Message Version 4 standard String Table capability code */
const uint32_t TransportCapabilityCode_MESSAGE4_STRINGTABLE_STANDARD_TABLE = 0x00000004;

/**
 * @brief Log level enum
 *
 * Enum of possible log levels. Set log level using
 * RobotRaconteurNode::SetLogLevel(),
 * `ROBOTRACONTEUR_LOG_LEVEL` environmental variable, or
 * `--robotraconteur-log-level` node setup command line option.
 */
enum RobotRaconteur_LogLevel
{
    /** @brief `trace` log level */
    RobotRaconteur_LogLevel_Trace,
    /** @brief `debug` log level */
    RobotRaconteur_LogLevel_Debug,
    /** @brief `info` log level */
    RobotRaconteur_LogLevel_Info,
    /** @brief `warning` log level */
    RobotRaconteur_LogLevel_Warning,
    /** @brief `error` log level */
    RobotRaconteur_LogLevel_Error,
    /** @brief `fatal` log level */
    RobotRaconteur_LogLevel_Fatal,
    /** @brief `disabled` log level */
    RobotRaconteur_LogLevel_Disable = 1000
};

/**
 * @brief Log component enum
 *
 * Log records contain the code of the component where
 * the log record was generated
 *
 */
enum RobotRaconteur_LogComponent
{
    /** @brief default component */
    RobotRaconteur_LogComponent_Default,
    /** @brief Robot Raconteur Node component */
    RobotRaconteur_LogComponent_Node,
    /** @brief tranport component */
    RobotRaconteur_LogComponent_Transport,
    /** @brief message or message serialization component */
    RobotRaconteur_LogComponent_Message,
    /** @brief client component */
    RobotRaconteur_LogComponent_Client,
    /** @brief service component */
    RobotRaconteur_LogComponent_Service,
    /** @brief member component */
    RobotRaconteur_LogComponent_Member,
    /** @brief data message packing component */
    RobotRaconteur_LogComponent_Pack,
    /** @brief data message unpacknig component */
    RobotRaconteur_LogComponent_Unpack,
    /** @brief service definition parser component */
    RobotRaconteur_LogComponent_ServiceDefinition,
    /** @brief node/service discovery component */
    RobotRaconteur_LogComponent_Discovery,
    /** @brief subscription component */
    RobotRaconteur_LogComponent_Subscription,
    /** @brief node setup component */
    RobotRaconteur_LogComponent_NodeSetup,
    /** @brief utility component */
    RobotRaconteur_LogComponent_Utility,
    /** @brief service definition standard library component (external) */
    RobotRaconteur_LogComponent_RobDefLib,
    /** @brief user component (external) */
    RobotRaconteur_LogComponent_User,
    /** @brief user client component (external) */
    RobotRaconteur_LogComponent_UserClient,
    /** @brief user service component (external) */
    RobotRaconteur_LogComponent_UserService,
    /** @brief third party library component (external) */
    RobotRaconteur_LogComponent_ThirdParty
};

} // namespace RobotRaconteur
