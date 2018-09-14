// Copyright 2011-2018 Wason Technology, LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <stdint.h>


namespace RobotRaconteur
{



	enum DataTypes
	{
		DataTypes_void_t = 0,
		DataTypes_double_t,
		DataTypes_single_t,
		DataTypes_int8_t,
		DataTypes_uint8_t,
		DataTypes_int16_t,
		DataTypes_uint16_t,
		DataTypes_int32_t,
		DataTypes_uint32_t,
		DataTypes_int64_t,
		DataTypes_uint64_t,
		DataTypes_string_t,
		DataTypes_structure_t = 101,
		DataTypes_vector_t,
		DataTypes_dictionary_t,
		DataTypes_object_t,
		DataTypes_varvalue_t,
		DataTypes_varobject_t,
		DataTypes_multidimarray_t,
		DataTypes_list_t,
		DataTypes_cstructure_t,
		DataTypes_cstructure_array_t,
		DataTypes_cstructure_multidimarray_t,
		DataTypes_enum_t,
		DataTypes_namedtype_t,				
	};

	enum DataTypes_ArrayTypes
	{
		DataTypes_ArrayTypes_none = 0,
		DataTypes_ArrayTypes_array,
		DataTypes_ArrayTypes_multidimarray
	};

	enum DataTypes_ContainerTypes
	{
		DataTypes_ContainerTypes_none = 0,
		DataTypes_ContainerTypes_list,
		DataTypes_ContainerTypes_map_int32,
		DataTypes_ContainerTypes_map_string,
		DataTypes_ContainerTypes_generator
	};

	//Flags for MessageFlags entry in MessageHeader
	const uint16_t MessageFlags_PROTOCOL_VERSION_MINOR = 0x01;
	const uint16_t MessageFlags_SUBSTREAM_ID = 0x02;
	const uint16_t MessageFlags_SUBSTREAM_SEQUENCE_NUMBER = 0x04;
	const uint16_t MessageFlags_FRAGMENT = 0x08;
	const uint16_t MessageFlags_UNRELIABLE = 0x10;
	const uint16_t MessageFlags_UNRELIABLE_EXPIRATION = 0x20;
	const uint16_t MessageFlags_PRIORITY = 0x40;
	const uint16_t MessageFlags_ROUTING_INFO = 0x80;
	const uint16_t MessageFlags_ENDPOINT_INFO = 0x100;
	const uint16_t MessageFlags_META_INFO = 0x200;
	const uint16_t MessageFlags_MESSAGE_ID = 0x400;
	const uint16_t MessageFlags_STRING_TABLE = 0x800;
	const uint16_t MessageFlags_MULTIPLE_ENTRIES = 0x1000;
	const uint16_t MessageFlags_TRANSPORT_SPECIFIC = 0x2000;

	const uint16_t MessageFlags_Version2Compat = MessageFlags_ROUTING_INFO | MessageFlags_ENDPOINT_INFO | MessageFlags_META_INFO | MessageFlags_MESSAGE_ID | MessageFlags_MULTIPLE_ENTRIES;

	//Flags for EntryFlags in MessageEntry
	const uint16_t MessageEntryFlags_SERVICE_PATH_STR = 0x01;
	const uint16_t MessageEntryFlags_SERVICE_PATH_CODE = 0x02;
	const uint16_t MessageEntryFlags_MEMBER_NAME_STR = 0x04;
	const uint16_t MessageEntryFlags_MEMBER_NAME_CODE = 0x08;
	const uint16_t MessageEntryFlags_REQUEST_ID = 0x10;
	const uint16_t MessageEntryFlags_ERROR = 0x20;
	const uint16_t MessageEntryFlags_META_INFO = 0x40;
	const uint16_t MessageEntryFlags_TIMESPEC = 0x80;

	const uint16_t MessageEntryFlags_Version2Compat = MessageEntryFlags_SERVICE_PATH_STR | MessageEntryFlags_MEMBER_NAME_STR | MessageEntryFlags_REQUEST_ID | MessageEntryFlags_ERROR | MessageEntryFlags_META_INFO;

	//Flags for ElementFlags in MessageElement
	const uint16_t MessageElementFlags_ELEMENT_NAME_STR = 0x01;
	const uint16_t MessageElementFlags_ELEMENT_NAME_CODE = 0x02;
	const uint16_t MessageElementFlags_ELEMENT_NUMBER = 0x04;
	const uint16_t MessageElementFlags_ELEMENT_TYPE_NAME_STR = 0x08;
	const uint16_t MessageElementFlags_ELEMENT_TYPE_NAME_CODE = 0x10;
	const uint16_t MessageElementFlags_SEQUENCE_NUMBER = 0x20;
	const uint16_t MessageElementFlags_REQUEST_ACK = 0x40;
	const uint16_t MessageElementFlags_META_INFO = 0x80;

	const uint16_t MessageElementFlags_Version2Compat = MessageElementFlags_ELEMENT_NAME_STR | MessageElementFlags_ELEMENT_TYPE_NAME_STR | MessageElementFlags_META_INFO;

	enum MessageEntryType
	{

		MessageEntryType_Null = 0,
		MessageEntryType_StreamOp = 1,
		MessageEntryType_StreamOpRet,
		MessageEntryType_StreamCheckCapability,
		MessageEntryType_StreamCheckCapabilityRet,
		MessageEntryType_StringTableOp,
		MessageEntryType_StringTableOpRet,
		MessageEntryType_GetServiceDesc = 101,
		MessageEntryType_GetServiceDescRet,
		MessageEntryType_ObjectTypeName,
		MessageEntryType_ObjectTypeNameRet,
		MessageEntryType_ServiceClosed,
		MessageEntryType_ServiceClosedRet,
		MessageEntryType_ConnectClient,
		MessageEntryType_ConnectClientRet,
		MessageEntryType_DisconnectClient,
		MessageEntryType_DisconnectClientRet,
		MessageEntryType_ConnectionTest,
		MessageEntryType_ConnectionTestRet,
		MessageEntryType_GetNodeInfo,
		MessageEntryType_GetNodeInfoRet,
		MessageEntryType_ReconnectClient,
		MessageEntryType_ReconnectClientRet,
		MessageEntryType_NodeCheckCapability,
		MessageEntryType_NodeCheckCapabilityRet,
		MessageEntryType_GetServiceAttributes,
		MessageEntryType_GetServiceAttributesRet,
		MessageEntryType_ConnectClientCombined,
		MessageEntryType_ConnectClientCombinedRet,
		MessageEntryType_EndpointCheckCapability = 501,
		MessageEntryType_EndpointCheckCapabilityRet,
		MessageEntryType_ServiceCheckCapabilityReq = 1101,
		MessageEntryType_ServiceCheckCapabilityRet,
		MessageEntryType_ClientKeepAliveReq = 1105,
		MessageEntryType_ClientKeepAliveRet,
		MessageEntryType_ClientSessionOpReq = 1107,
		MessageEntryType_ClientSessionOpRet,
		MessageEntryType_ServicePathReleasedReq,
		MessageEntryType_ServicePathReleasedRet,
		MessageEntryType_PropertyGetReq = 1111,
		MessageEntryType_PropertyGetRes,
		MessageEntryType_PropertySetReq,
		MessageEntryType_PropertySetRes,
		MessageEntryType_FunctionCallReq = 1121,
		MessageEntryType_FunctionCallRes,
		MessageEntryType_GeneratorNextReq,
		MessageEntryType_GeneratorNextRes,
		MessageEntryType_EventReq = 1131,
		MessageEntryType_EventRes,
		MessageEntryType_PipePacket = 1141,
		MessageEntryType_PipePacketRet,
		MessageEntryType_PipeConnectReq,
		MessageEntryType_PipeConnectRet,
		MessageEntryType_PipeDisconnectReq,
		MessageEntryType_PipeDisconnectRet,
		MessageEntryType_PipeClosed,
		MessageEntryType_PipeClosedRet,
		MessageEntryType_CallbackCallReq = 1151,
		MessageEntryType_CallbackCallRet,
		MessageEntryType_WirePacket = 1161,
		MessageEntryType_WirePacketRet,
		MessageEntryType_WireConnectReq,
		MessageEntryType_WireConnectRet,
		MessageEntryType_WireDisconnectReq,
		MessageEntryType_WireDisconnectRet,
		MessageEntryType_WireClosed,
		MessageEntryType_WireClosedRet,
		MessageEntryType_MemoryRead = 1171,
		MessageEntryType_MemoryReadRet,
		MessageEntryType_MemoryWrite,
		MessageEntryType_MemoryWriteRet,
		MessageEntryType_MemoryGetParam,
		MessageEntryType_MemoryGetParamRet,
		MessageEntryType_WirePeekInValueReq=1181,
		MessageEntryType_WirePeekInValueRet,
		MessageEntryType_WirePeekOutValueReq,
		MessageEntryType_WirePeekOutValueRet,
		MessageEntryType_WirePokeOutValueReq,
		MessageEntryType_WirePokeOutValueRet,

		//Dedicated transport message types.  These are an extension to the protocol
		//and not a base feature
		MessageEntryType_WireTransportOpReq = 11161,
		MessageEntryType_WireTransportOpRet,
		MessageEntryType_WireTransportEvent,
		MessageEntryType_WireTransportEventRet

	};

	enum MessageErrorType
	{

		MessageErrorType_None = 0,
		MessageErrorType_ConnectionError = 1,
		MessageErrorType_ProtocolError,
		MessageErrorType_ServiceNotFound,
		MessageErrorType_ObjectNotFound,
		MessageErrorType_InvalidEndpoint,
		MessageErrorType_EndpointCommunicationFatalError,
		MessageErrorType_NodeNotFound,
		MessageErrorType_ServiceError,
		MessageErrorType_MemberNotFound,
		MessageErrorType_MemberFormatMismatch,
		MessageErrorType_DataTypeMismatch,
		MessageErrorType_DataTypeError,
		MessageErrorType_DataSerializationError,
		MessageErrorType_MessageEntryNotFound,
		MessageErrorType_MessageElementNotFound,
		MessageErrorType_UnknownError,		
		MessageErrorType_InvalidOperation,
		MessageErrorType_InvalidArgument,
		MessageErrorType_OperationFailed,
		MessageErrorType_NullValue,
		MessageErrorType_InternalError,		
		MessageErrorType_SystemResourcePermissionDenied,
		MessageErrorType_OutOfSystemResource,
		MessageErrorType_SystemResourceError,		
		MessageErrorType_ResourceNotFound,
		MessageErrorType_IOError,
		MessageErrorType_BufferLimitViolation,
		MessageErrorType_ServiceDefinitionError,
		MessageErrorType_OutOfRange,
		MessageErrorType_KeyNotFound,
		MessageErrorType_RemoteError = 100,
		MessageErrorType_RequestTimeout,
		MessageErrorType_ReadOnlyMember,
		MessageErrorType_WriteOnlyMember,
		MessageErrorType_NotImplementedError,
		MessageErrorType_MemberBusy,
		MessageErrorType_ValueNotSet,
		MessageErrorType_AuthenticationError = 150,
		MessageErrorType_ObjectLockedError,
		MessageErrorType_PermissionDenied,
		MessageErrorType_AbortOperation,
		MessageErrorType_OperationAborted,
		MessageErrorType_StopIteration

	};

	enum ClientServiceListenerEventType
	{
		ClientServiceListenerEventType_ClientClosed = 1,
		ClientServiceListenerEventType_ClientConnectionTimeout,
		ClientServiceListenerEventType_TransportConnectionConnected,
		ClientServiceListenerEventType_TransportConnectionClosed
	};

	enum ServerServiceListenerEventType
	{
		ServerServiceListenerEventType_ServiceClosed = 1,
		ServerServiceListenerEventType_ClientConnected,
		ServerServiceListenerEventType_ClientDisconnected

	};

	enum MemberDefinition_Direction
	{
		MemberDefinition_Direction_both = 0,
		MemberDefinition_Direction_readonly,
		MemberDefinition_Direction_writeonly,
	};

#define RR_TIMEOUT_INFINITE -1

	const uint32_t TranspartCapabilityCode_PAGE_MASK = 0xFFF00000;
	const uint32_t TransportCapabilityCode_MESSAGE2_BASIC_PAGE = 0x02000000;
	const uint32_t TransportCapabilityCode_MESSAGE2_BASIC_ENABLE = 0x00000001;
	const uint32_t TransportCapabilityCode_MESSAGE2_BASIC_CONNECTCOMBINED = 0x00000002;
	const uint32_t TransportCapabilityCode_MESSAGE3_BASIC_PAGE = 0x03000000;
	const uint32_t TransportCapabilityCode_MESSAGE3_BASIC_ENABLE = 0x00000001;
	const uint32_t TransportCapabilityCode_MESSAGE3_BASIC_CONNECTCOMBINED = 0x00000002;
	const uint32_t TransportCapabilityCode_MESSAGE3_STRINGTABLE_PAGE = 0x03100000;
	const uint32_t TransportCapabilityCode_MESSAGE3_STRINGTABLE_ENABLE = 0x00000001;
	const uint32_t TransportCapabilityCode_MESSAGE3_STRINGTABLE_MESSAGE_LOCAL = 0x00000002;
	const uint32_t TransportCapabilityCode_MESSAGE3_STRINGTABLE_DYNAMIC_TABLE = 0x00000004;
	const uint32_t TransportCapabilityCode_MESSAGE3_STRINGTABLE_STANDARD_TABLE = 0x00000008;
}
