// Copyright 2011-2019 Wason Technology, LLC
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


//#include "RobotRaconteur/DataTypes.h"
#include "RobotRaconteur/RobotRaconteurConstants.h"
#include "RobotRaconteur/RobotRaconteurConfig.h"
#include <stdexcept>
#include <string>
#pragma once


namespace RobotRaconteur
{
	
	class ROBOTRACONTEUR_CORE_API RobotRaconteurException : public std::runtime_error
	{
	
	public:
		RobotRaconteurException();

		RobotRaconteurException(MessageErrorType ErrorCode, const std::string &error, const std::string &message);

		RobotRaconteurException(const std::string &message, std::exception &innerexception);

		MessageErrorType ErrorCode;

		std::string Error;

		std::string Message;

		virtual std::string ToString();

		virtual const char* what() const throw ();

		~RobotRaconteurException() throw () {}

	private:
		std::string what_string;
		void InitializeInstanceFields();
	};

#define RR_EXCEPTION_TYPES_INIT(M,M2) \
		M(ConnectionException, MessageErrorType_ConnectionError, "RobotRaconteur.ConnectionError") \
		M(ProtocolException, MessageErrorType_ProtocolError, "RobotRaconteur.ProtocolError") \
		M(ServiceNotFoundException, MessageErrorType_ServiceNotFound, "RobotRaconteur.ServiceNotFound") \
		M(ObjectNotFoundException, MessageErrorType_ObjectNotFound, "RobotRaconteur.ObjectNotFound") \
		M(InvalidEndpointException, MessageErrorType_InvalidEndpoint, "RobotRaconteur.InvalidEndpoint") \
		M(EndpointCommunicationFatalException, MessageErrorType_EndpointCommunicationFatalError, "RobotRaconteur.EndpointCommunicationFatalError") \
		M(NodeNotFoundException, MessageErrorType_NodeNotFound, "RobotRaconteur.NodeNotFound") \
		M(ServiceException, MessageErrorType_ServiceError,  "RobotRaconteur.ServiceError") \
		M(MemberNotFoundException, MessageErrorType_MemberNotFound, "RobotRaconteur.MemberNotFound") \
		M(MemberFormatMismatchException, MessageErrorType_MemberFormatMismatch, "RobotRaconteur.MemberFormatMismatch") \
		M(DataTypeMismatchException, MessageErrorType_DataTypeMismatch, "RobotRaconteur.DataTypeMismatch") \
		M(DataTypeException, MessageErrorType_DataTypeError, "RobotRaconteur.DataTypeError") \
		M(DataSerializationException, MessageErrorType_DataSerializationError, "RobotRaconteur.DataSerializationError") \
		M(MessageEntryNotFoundException, MessageErrorType_MessageEntryNotFound, "RobotRaconteur.MessageEntryNotFound") \
		M(MessageElementNotFoundException, MessageErrorType_MessageElementNotFound, "RobotRaconteur.MessageElementNotFound") \
		M2(UnknownException, MessageErrorType_UnknownError) \
		M(InvalidOperationException, MessageErrorType_InvalidOperation, "RobotRaconteur.InvalidOperation") \
		M(InvalidArgumentException, MessageErrorType_InvalidArgument, "RobotRaconteur.InvalidArgument") \
		M(OperationFailedException, MessageErrorType_OperationFailed, "RobotRaconteur.OperationFailed") \
		M(NullValueException, MessageErrorType_NullValue, "RobotRaconteur.NullValue") \
		M(InternalErrorException, MessageErrorType_InternalError, "RobotRaconteur.InternalError") \
		M(SystemResourcePermissionDeniedException, MessageErrorType_SystemResourcePermissionDenied, "RobotRaconteur.SystemResourcePermissionDenied") \
		M(OutOfSystemResourceException, MessageErrorType_OutOfSystemResource,  "RobotRaconteur.OutOfSystemResource") \
		M(SystemResourceException, MessageErrorType_SystemResourceError, "RobotRaconteur.SystemResourceError") \
		M(ResourceNotFoundException, MessageErrorType_ResourceNotFound, "RobotRaconteur.ResourceNotFound") \
		M(IOException, MessageErrorType_IOError, "RobotRaconteur.IOError") \
		M(BufferLimitViolationException, MessageErrorType_BufferLimitViolation, "RobotRaconteur.BufferLimitViolation") \
		M(ServiceDefinitionException, MessageErrorType_ServiceDefinitionError, "RobotRaconteur.SystemDefinitionError") \
		M(OutOfRangeException, MessageErrorType_OutOfRange, "RobotRaconteur.OutOfRange") \
		M(KeyNotFoundException, MessageErrorType_KeyNotFound, "RobotRaconteur.KeyNotFound") \
		M2(RobotRaconteurRemoteException, MessageErrorType_RemoteError) \
		M(RequestTimeoutException, MessageErrorType_RequestTimeout, "RobotRaconteur.RequestTimeout") \
		M(ReadOnlyMemberException, MessageErrorType_ReadOnlyMember, "RobotRaconteur.ReadOnlyMember") \
		M(WriteOnlyMemberException, MessageErrorType_WriteOnlyMember, "RobotRaconteur.WriteOnlyMember") \
		M(NotImplementedException, MessageErrorType_NotImplementedError, "RobotRaconteur.NotImplementedError") \
		M(MemberBusyException, MessageErrorType_MemberBusy, "RobotRaconteur.MemberBusy") \
		M(ValueNotSetException, MessageErrorType_ValueNotSet, "RobotRaconteur.ValueNotSet") \
		M(AuthenticationException, MessageErrorType_AuthenticationError, "RobotRaconteur.AuthenticationError") \
		M(ObjectLockedException, MessageErrorType_ObjectLockedError, "RobotRaconteur.ObjectLockedError") \
		M(PermissionDeniedException, MessageErrorType_PermissionDenied, "RobotRaconteur.PermissionDenied") \
		M(AbortOperationException, MessageErrorType_AbortOperation, "RobotRaconteur.AbortOperation") \
		M(OperationAbortedException, MessageErrorType_OperationAborted, "RobotRaconteur.OperationAborted") \
		M(StopIterationException, MessageErrorType_StopIteration, "RobotRaconteur.StopIteration")


#define RR_EXCEPTION_DECL_1(exp_cpp_type, exp_code, exp_type_str) \
	class ROBOTRACONTEUR_CORE_API exp_cpp_type : public RobotRaconteurException \
	{ \
	public: \
		exp_cpp_type(const std::string &message); \
	}; \

#define RR_EXCEPTION_DECL_2(exp_cpp_type, exp_code) \
	class ROBOTRACONTEUR_CORE_API exp_cpp_type : public RobotRaconteurException \
	{ \
	public: \
		exp_cpp_type(const std::string &error, const std::string &message); \
		exp_cpp_type(std::exception &innerexception); \
	}; \

	RR_EXCEPTION_TYPES_INIT(RR_EXCEPTION_DECL_1, RR_EXCEPTION_DECL_2)

#undef RR_EXCEPTION_DECL_1
#undef RR_EXCEPTION_DECL_2

	/*class ROBOTRACONTEUR_CORE_API ProtocolException : public RobotRaconteurException
	{
	public:
		ProtocolException(const std::string &message);

	};
	
	class ROBOTRACONTEUR_CORE_API ServiceNotFoundException : public RobotRaconteurException
	{
	public:
		ServiceNotFoundException(const std::string &message);

	};
	
	class ROBOTRACONTEUR_CORE_API ObjectNotFoundException : public RobotRaconteurException
	{
	public:
		ObjectNotFoundException(const std::string &message);

	};

	class ROBOTRACONTEUR_CORE_API InvalidEndpointException : public RobotRaconteurException
	{

	public:
		InvalidEndpointException(const std::string &message);

	};

	class ROBOTRACONTEUR_CORE_API EndpointCommunicationFatalException : public RobotRaconteurException
	{
	public:
		EndpointCommunicationFatalException(const std::string &message);

	};
	
	class ROBOTRACONTEUR_CORE_API NodeNotFoundException : public RobotRaconteurException
	{
	public:
		NodeNotFoundException(const std::string &message);

	};

	class ROBOTRACONTEUR_CORE_API ServiceException : public RobotRaconteurException
	{
	public:
		ServiceException(const std::string &message);

	};

	class ROBOTRACONTEUR_CORE_API MemberNotFoundException : public RobotRaconteurException
	{
	public:
		MemberNotFoundException(const std::string &message);

	};

	
	class ROBOTRACONTEUR_CORE_API MemberFormatMismatchException : public RobotRaconteurException
	{
	
	public:
		MemberFormatMismatchException(const std::string &message);

	};

	class ROBOTRACONTEUR_CORE_API DataTypeMismatchException : public RobotRaconteurException
	{
	public:
		DataTypeMismatchException(const std::string &message);

	};
	
	class ROBOTRACONTEUR_CORE_API DataTypeException : public RobotRaconteurException
	{
	public:
		DataTypeException(const std::string &message);

	};
	
	class ROBOTRACONTEUR_CORE_API DataSerializationException : public RobotRaconteurException
	{
	public:
		DataSerializationException(const std::string &message);

	};
	
	class ROBOTRACONTEUR_CORE_API MessageEntryNotFoundException : public RobotRaconteurException
	{
	public:
		MessageEntryNotFoundException(const std::string &message);

	};

	class ROBOTRACONTEUR_CORE_API MessageElementNotFoundException : public RobotRaconteurException
	{
	public:
		MessageElementNotFoundException(const std::string &message);

	};

	class ROBOTRACONTEUR_CORE_API UnknownException : public RobotRaconteurException
	{
	public:
		UnknownException(const std::string &error, const std::string &message);

	};

	class ROBOTRACONTEUR_CORE_API InvalidOperationException : public RobotRaconteurException
	{
	public:
		InvalidOperationException(const std::string &message);

	};

	class ROBOTRACONTEUR_CORE_API InvalidArgumentException : public RobotRaconteurException
	{
	public:
		InvalidArgumentException(const std::string &message);

	};

	class ROBOTRACONTEUR_CORE_API OperationFailedException : public RobotRaconteurException
	{
	public:
		OperationFailedException(const std::string &message);

	};

	class ROBOTRACONTEUR_CORE_API NullValueException : public RobotRaconteurException
	{
	public:
		NullValueException(const std::string &message);

	};

	class ROBOTRACONTEUR_CORE_API InternalErrorException : public RobotRaconteurException
	{
	public:
		InternalErrorException(const std::string &message);

	};

	class ROBOTRACONTEUR_CORE_API PermissionDeniedException : public RobotRaconteurException
	{
	public:
		PermissionDeniedException(const std::string &message);

	};

	class ROBOTRACONTEUR_CORE_API SystemResourcePermissionDeniedException : public RobotRaconteurException
	{
	public:
		SystemResourcePermissionDeniedException(const std::string &message);

	};

	class ROBOTRACONTEUR_CORE_API OutOfSystemResourceException : public RobotRaconteurException
	{
	public:
		OutOfSystemResourceException(const std::string &message);

	};

	class ROBOTRACONTEUR_CORE_API SystemResourceException : public RobotRaconteurException
	{
	public:
		SystemResourceException(const std::string &message);

	};

	class ROBOTRACONTEUR_CORE_API ResourceNotFoundException : public RobotRaconteurException
	{
	public:
		ResourceNotFoundException(const std::string &message);

	};

	class ROBOTRACONTEUR_CORE_API IOException : public RobotRaconteurException
	{
	public:
		IOException(const std::string &message);

	};

	class ROBOTRACONTEUR_CORE_API BufferLimitViolationException : public RobotRaconteurException
	{
	public:
		BufferLimitViolationException(const std::string &message);

	};

	class ROBOTRACONTEUR_CORE_API ServiceDefinitionException : public RobotRaconteurException
	{
	public:
		ServiceDefinitionException(const std::string &message);

	};

	class ROBOTRACONTEUR_CORE_API OutOfRangeException : public RobotRaconteurException
	{
	public:
		OutOfRangeException(const std::string &message);

	};

	class ROBOTRACONTEUR_CORE_API KeyNotFoundException : public RobotRaconteurException
	{
	public:
		KeyNotFoundException(const std::string &message);

	};

	class ROBOTRACONTEUR_CORE_API RobotRaconteurRemoteException : public RobotRaconteurException
	{
	public:
		RobotRaconteurRemoteException(const std::string &error, const std::string &message);

		RobotRaconteurRemoteException(std::exception &innerexception);

	};

	class ROBOTRACONTEUR_CORE_API RequestTimeoutException : public RobotRaconteurException
	{
	public:
		RequestTimeoutException(const std::string &message);

	};

	class ROBOTRACONTEUR_CORE_API ReadOnlyMemberException : public RobotRaconteurException
	{
	public:
		ReadOnlyMemberException(const std::string &message);

	};

	class ROBOTRACONTEUR_CORE_API WriteOnlyMemberException : public RobotRaconteurException
	{
	public:
		WriteOnlyMemberException(const std::string &message);

	};

	class ROBOTRACONTEUR_CORE_API NotImplementedException : public RobotRaconteurException
	{
	public:
		NotImplementedException(const std::string &message);

	};

	class ROBOTRACONTEUR_CORE_API MemberBusyException : public RobotRaconteurException
	{
	public:
		MemberBusyException(const std::string &message);

	};

	class ROBOTRACONTEUR_CORE_API ValueNotSetException : public RobotRaconteurException
	{
	public:
		ValueNotSetException(const std::string &message);

	};

	class ROBOTRACONTEUR_CORE_API AuthenticationException : public RobotRaconteurException
	{
	public:
		AuthenticationException(const std::string &message);

	};

	class ROBOTRACONTEUR_CORE_API ObjectLockedException : public RobotRaconteurException
	{
	public:
		ObjectLockedException(const std::string &message);

	};*/
	
#ifndef BOOST_NO_CXX11_TEMPLATE_ALIASES
using RobotRaconteurExceptionPtr = RR_SHARED_PTR<RobotRaconteurException>;
using RobotRaconteurExceptionConstPtr = RR_SHARED_PTR<const RobotRaconteurException>;

#define RR_EXCEPTION_DECL_1_PTR(exp_cpp_type, exp_code, exp_type_str) \
using exp_cpp_type ## Ptr = RR_SHARED_PTR<exp_cpp_type>; \
using exp_cpp_type ## ConstPtr = RR_SHARED_PTR<const exp_cpp_type>;

#define RR_EXCEPTION_DECL_2_PTR(exp_cpp_type, exp_code) \
using exp_cpp_type ## Ptr = RR_SHARED_PTR<exp_cpp_type>; \
using exp_cpp_type ## ConstPtr = RR_SHARED_PTR<const exp_cpp_type>;

RR_EXCEPTION_TYPES_INIT(RR_EXCEPTION_DECL_1_PTR, RR_EXCEPTION_DECL_2_PTR)

#undef RR_EXCEPTION_DECL_1_PTR
#undef RR_EXCEPTION_DECL_2_PTR
#endif

}
