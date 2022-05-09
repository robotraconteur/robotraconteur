/**
 * @file Error.h
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

//#include "RobotRaconteur/DataTypes.h"
#include "RobotRaconteur/RobotRaconteurConstants.h"
#include "RobotRaconteur/RobotRaconteurConfig.h"
#include <stdexcept>
#include <string>
#pragma once

namespace RobotRaconteur
{

class ROBOTRACONTEUR_CORE_API RRValue;

/**
 * @brief Base class for Robot Raconteur exceptions
 *
 * RobotRaconteurException represents an exception that can be serialized and passed
 * to a remote node. The MessageErrorType contains an enum of predefined error
 * codes that represent different potential errors. Each error code has a corresponding
 * C++ exception that extends RobotRaconteurException.
 *
 * User defined exception types can be declared in Service Definition files. The
 * C++ exception is generated with the thunk source. These user defined exceptions
 * extend RobotRaconteurRemoteException.
 *
 */
class ROBOTRACONTEUR_CORE_API RobotRaconteurException : public std::runtime_error
{

  public:
    /**
     * @brief Construct an empty RobtoRaconteurException()
     *
     */
    RobotRaconteurException();

    /**
     * @brief Construct a RobotRaconteurException
     *
     * @param ErrorCode The predefined error code for the exception
     * @param error The error type name corresponding to the ErrorCode, or a user defined
     * error type
     * @param message A message to pass to the user
     * @param sub_name An optional sub_name to further categorize the exception
     * @param param An optional exception parameter
     */
    RobotRaconteurException(MessageErrorType ErrorCode, const std::string& error, const std::string& message,
                            const std::string& sub_name, const RR_INTRUSIVE_PTR<RRValue>& param);

    /**
     * @brief Construct a RobotRaconteurException based on an existing exception
     *
     * This will result in an exception with error code MessageErrorType_UnknownException
     *
     * @param message A message to pass to the user
     * @param innerexception The existing exception
     */
    RobotRaconteurException(const std::string& message, std::exception& innerexception);

    /**
     * @brief Predefined error code
     *
     * Error code that specifies the type of error
     *
     */
    MessageErrorType ErrorCode;

    /**
     * @brief Name of the error
     *
     * Name corresponding to predefined error code, or a user defined
     * error type
     */
    std::string Error;

    /**
     * @brief Message to pass to the user
     *
     * Human readable message to pass to the user
     */
    std::string Message;

    /**
     * @brief An optional sub_name to further categorize the exception
     *
     * The sub_name should follow the Robot Raconteur naming rules. It should
     * either be a single name or a fully qualified name using service definition
     * naming rules
     *
     */
    std::string ErrorSubName;

    /**
     * @brief An optional parameter
     *
     * Use this placeholder if data needs to be passed with the exception
     */
    RR_INTRUSIVE_PTR<RRValue> ErrorParam;

    /**
     * @brief Returns a string representing the exception
     *
     * @return std::string
     */
    virtual std::string ToString();

    RR_OVIRTUAL const char* what() const throw() RR_OVERRIDE;

    RR_OVIRTUAL ~RobotRaconteurException() throw() RR_OVERRIDE {}

  private:
    std::string what_string;
};

// clang-format off
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
		M(ServiceDefinitionException, MessageErrorType_ServiceDefinitionError, "RobotRaconteur.ServiceDefinitionError") \
		M(OutOfRangeException, MessageErrorType_OutOfRange, "RobotRaconteur.OutOfRange") \
		M(KeyNotFoundException, MessageErrorType_KeyNotFound, "RobotRaconteur.KeyNotFound") \
		M(InvalidConfigurationException, MessageErrorType_InvalidConfiguration, "RobotRaconteur.InvalidConfiguration") \
		M(InvalidStateException, MessageErrorType_InvalidState, "RobotRaconteur.InvalidState") \
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
		M(StopIterationException, MessageErrorType_StopIteration, "RobotRaconteur.StopIteration") \
		M(OperationTimeoutException, MessageErrorType_OperationTimeout, "RobotRaconteur.OperationTimeout") \
		M(OperationCancelledException, MessageErrorType_OperationCancelled, "RobotRaconteur.OperationCancelled")
// clang-format on

#define RR_EXCEPTION_DECL_1(exp_cpp_type, exp_code, exp_type_str)                                                      \
    class ROBOTRACONTEUR_CORE_API exp_cpp_type : public RobotRaconteurException                                        \
    {                                                                                                                  \
      public:                                                                                                          \
        exp_cpp_type(const std::string& message, const std::string& sub_name = "",                                     \
                     const RR_INTRUSIVE_PTR<RRValue>& param_ = RR_INTRUSIVE_PTR<RRValue>());                           \
    };

#define RR_EXCEPTION_DECL_2(exp_cpp_type, exp_code)                                                                    \
    class ROBOTRACONTEUR_CORE_API exp_cpp_type : public RobotRaconteurException                                        \
    {                                                                                                                  \
      public:                                                                                                          \
        exp_cpp_type(const std::string& error, const std::string& message, const std::string& sub_name = "",           \
                     const RR_INTRUSIVE_PTR<RRValue>& param_ = RR_INTRUSIVE_PTR<RRValue>());                           \
        exp_cpp_type(std::exception& innerexception);                                                                  \
    };

RR_EXCEPTION_TYPES_INIT(RR_EXCEPTION_DECL_1, RR_EXCEPTION_DECL_2)

#undef RR_EXCEPTION_DECL_1
#undef RR_EXCEPTION_DECL_2

#ifdef GENERATING_DOCUMENTATION

/**
 * @brief Exception thrown when connection to remote node fails
 *
 * This exception is thrown if a connection cannot be created,
 * the connection fails, or the connection has been closed.
 *
 * Error code MessageErrorType_ConnectionError (1)
 */
class ConnectionException : public RobotRaconteurException
{
  public:
    /**
     * @brief Construct a ConnectionException
     *
     * @param message Message for the user
     * @param sub_name Optional error sub_name
     * @param param_ Optional error param
     */
    ConnectionException(const std::string& message, std::string sub_name = "",
                        const RR_INTRUSIVE_PTR<RRValue>& param_ = RR_INTRUSIVE_PTR<RRValue>());
};

/**
 * @brief Exception thrown when a protocol failure occurs on
 * a tranport connection
 *
 * Error code MessageErrorType_ProtocolError (2)
 *
 */
class ProtocolException : public RobotRaconteurException
{
  public:
    /**
     * @brief Construct a ProtocolException
     *
     * @param message Message for the user
     * @param sub_name Optional error sub_name
     * @param param_ Optional error param
     */
    ProtocolException(const std::string& message, std::string sub_name = "",
                      const RR_INTRUSIVE_PTR<RRValue>& param_ = RR_INTRUSIVE_PTR<RRValue>());
};

/**
 * @brief Exception thrown when a service cannot be found
 * on a remote node
 *
 * Error code MessageErrorType_ServiceNotFound (3)
 *
 */
class ServiceNotFoundException : public RobotRaconteurException
{
  public:
    /**
     * @brief Construct a ServiceNotFoundException
     *
     * @param message Message for the user
     * @param sub_name Optional error sub_name
     * @param param_ Optional error param
     */
    ServiceNotFoundException(const std::string& message, std::string sub_name = "",
                             const RR_INTRUSIVE_PTR<RRValue>& param_ = RR_INTRUSIVE_PTR<RRValue>());
};

/**
 * @brief Exception thrown when a service object cannot
 * be found
 *
 * This error is thrown when a specified service path
 * does not have an assosciate object. The object may
 * have been released by the service, or the service
 * path is invalid
 *
 * Error code MessageErrorType_ObjectNotFound (4)
 *
 */
class ObjectNotFoundException : public RobotRaconteurException
{
  public:
    /**
     * @brief Construct an ObjectNotFoundException
     *
     * @param message Message for the user
     * @param sub_name Optional error sub_name
     * @param param_ Optional error param
     */
    ObjectNotFoundException(const std::string& message, std::string sub_name = "",
                            const RR_INTRUSIVE_PTR<RRValue>& param_ = RR_INTRUSIVE_PTR<RRValue>());
};

/**
 * @brief Exception thrown when an attempt is made
 * to send a message to an invalid endpoint
 *
 * Transports between two nodes terminate with a pair
 * of endpoints, one in each node. If the client, service,
 * service endpoint, or transport is destroyed, the endpoint
 * will be deleted. This exception is thrown if the
 * target endpoint is no longer available.
 *
 * Error code MessageErrorType_InvalidEndpoint (5)
 *
 */
class InvalidEndpointException : public RobotRaconteurException
{

  public:
    /**
     * @brief Construct an InvalidEndpointException
     *
     * @param message Message for the user
     * @param sub_name Optional error sub_name
     * @param param_ Optional error param
     */
    InvalidEndpointException(const std::string& message, std::string sub_name = "",
                             const RR_INTRUSIVE_PTR<RRValue>& param_ = RR_INTRUSIVE_PTR<RRValue>());
};

/**
 * @brief Exception thrown when an attempt to send a
 * message to an endpoint fails
 *
 * Transports between two nodes terminate with a pair
 * of endpoints, one in each node. Messages are sent
 * between endpoint pairs. If for some reason the endpoint
 * cannot send (or receive) the message, this exception
 * is thrown.
 *
 * Error code MessageErrorType_EndpointCommunicationFatalError (6)
 *
 */
class EndpointCommunicationFatalException : public RobotRaconteurException
{
  public:
    /**
     * @brief Construct an EndpointCommunicationFatalException
     *
     * @param message Message for the user
     * @param sub_name Optional error sub_name
     * @param param_ Optional error param
     */
    EndpointCommunicationFatalException(const std::string& message, std::string sub_name = "",
                                        const RR_INTRUSIVE_PTR<RRValue>& param_ = RR_INTRUSIVE_PTR<RRValue>());
};

/**
 * @brief Exception thrown if the specified node cannot be found
 *
 * When connecting to a service or sending a message, the NodeID
 * and/or NodeName are specified. If the specified node
 * cannot be found, this exception is thrown.
 *
 * Error code MessageErrorType_NodeNotFound (7)
 *
 */
class NodeNotFoundException : public RobotRaconteurException
{
  public:
    /**
     * @brief Construct a NodeNotFoundException
     *
     * @param message Message for the user
     * @param sub_name Optional error sub_name
     * @param param_ Optional error param
     */
    NodeNotFoundException(const std::string& message, std::string sub_name = "",
                          const RR_INTRUSIVE_PTR<RRValue>& param_ = RR_INTRUSIVE_PTR<RRValue>());
};

/**
 * @brief Exception thrown when an exception occurs during
 * an operation on a service
 *
 * ServiceException is a catch-all error for exceptions on services.
 * See the message field for an explanation of the error that occured.
 *
 * Error code MessageErrorType_ServiceError (8)
 *
 */
class ServiceException : public RobotRaconteurException
{
  public:
    /**
     * @brief Construct a ServiceException
     *
     * @param message Message for the user
     * @param sub_name Optional error sub_name
     * @param param_ Optional error param
     */
    ServiceException(const std::string& message, std::string sub_name = "",
                     const RR_INTRUSIVE_PTR<RRValue>& param_ = RR_INTRUSIVE_PTR<RRValue>());
};

/**
 * @brief Exception thrown when the specified object member is
 * not found
 *
 * Service objects have member that are declared in a service definition.
 * If an attempt is made to call a member that does not exist, this exception
 * is thrown.
 *
 * Error code MessageErrorType_MemberNotFound (9)
 *
 */
class MemberNotFoundException : public RobotRaconteurException
{
  public:
    /**
     * @brief Construct a MemberNotFoundException
     *
     * @param message Message for the user
     * @param sub_name Optional error sub_name
     * @param param_ Optional error param
     */
    MemberNotFoundException(const std::string& message, std::string sub_name = "",
                            const RR_INTRUSIVE_PTR<RRValue>& param_ = RR_INTRUSIVE_PTR<RRValue>());
};

/**
 * @brief Exception thrown when a request to a member has an
 * invalid MessageEntryType or the wrong message elements
 *
 * Error code MessageErrorType_MemberFormatMismatch (10)
 *
 */
class MemberFormatMismatchException : public RobotRaconteurException
{

  public:
    /**
     * @brief Construct a MemberFormatMismatchException
     *
     * @param message Message for the user
     * @param sub_name Optional error sub_name
     * @param param_ Optional error param
     */
    MemberFormatMismatchException(const std::string& message, std::string sub_name = "",
                                  const RR_INTRUSIVE_PTR<RRValue>& param_ = RR_INTRUSIVE_PTR<RRValue>());
};

/**
 * @brief Exception thrown when incorrect data is received
 * by a member
 *
 * Make sure the provided data matches the expected data types
 *
 * Error code MessageErrorType_DataTypeMismatch (11)
 *
 */
class DataTypeMismatchException : public RobotRaconteurException
{
  public:
    /**
     * @brief Construct a DataTypeMismatchException
     *
     * @param message Message for the user
     * @param sub_name Optional error sub_name
     * @param param_ Optional error param
     */
    DataTypeMismatchException(const std::string& message, std::string sub_name = "",
                              const RR_INTRUSIVE_PTR<RRValue>& param_ = RR_INTRUSIVE_PTR<RRValue>());
};

/**
 * @brief Exception thrown when unexpected or incompatible
 * data is provided
 *
 * DataTypeException is sometimes thrown when there is a
 * type mismatch instead of DataTypeMismatchException
 *
 * Make sure the provided data matches the expected data types
 *
 * Error code MessageErrorType_DataTypeError (12)
 *
 */
class DataTypeException : public RobotRaconteurException
{
  public:
    /**
     * @brief Construct a DataTypeException
     *
     * @param message Message for the user
     * @param sub_name Optional error sub_name
     * @param param_ Optional error param
     */
    DataTypeException(const std::string& message, std::string sub_name = "",
                      const RR_INTRUSIVE_PTR<RRValue>& param_ = RR_INTRUSIVE_PTR<RRValue>());
};

/**
 * @brief Exception thrown when data cannot be serialized
 *
 * This exeception is thrown when the provide data cannot be serialized.
 * This typically occurs inside a transport.
 *
 * Check that the provided data matches the types supported by
 * the Robot Raconteur C++ library
 *
 * Error code MessageErrorType_DataSerializationError (13)
 *
 */
class DataSerializationException : public RobotRaconteurException
{
  public:
    /**
     * @brief Construct a DataSerializationException
     *
     * @param message Message for the user
     * @param sub_name Optional error sub_name
     * @param param_ Optional error param
     */
    DataSerializationException(const std::string& message, std::string sub_name = "",
                               const RR_INTRUSIVE_PTR<RRValue>& param_ = RR_INTRUSIVE_PTR<RRValue>());
};

/**
 * @brief Exception thrown when an expected MessageEntry
 * is not found
 *
 * Error code MessageErrorType_MessageEntryNotfound (14)
 *
 */
class MessageEntryNotFoundException : public RobotRaconteurException
{
  public:
    /**
     * @brief Construct a MessageEntryNotFoundException
     *
     * @param message Message for the user
     * @param sub_name Optional error sub_name
     * @param param_ Optional error param
     */
    MessageEntryNotFoundException(const std::string& message, std::string sub_name = "",
                                  const RR_INTRUSIVE_PTR<RRValue>& param_ = RR_INTRUSIVE_PTR<RRValue>());
};

/**
 * @brief Exception thrown wen an expected MessageElement
 * is not found
 *
 * This exception is thrown when an expected field or parameter
 * is not found.
 *
 * Error code MessageErrorType_MessageElementNotfound (15)
 *
 */
class MessageElementNotFoundException : public RobotRaconteurException
{
  public:
    /**
     * @brief Construct a MessageElementNotFoundException
     *
     * @param message Message for the user
     * @param sub_name Optional error sub_name
     * @param param_ Optional error param
     */
    MessageElementNotFoundException(const std::string& message, std::string sub_name = "",
                                    const RR_INTRUSIVE_PTR<RRValue>& param_ = RR_INTRUSIVE_PTR<RRValue>());
};

/**
 * @brief Exception representing an unknown exception type
 *
 * This exception is used to transmit exceptions that do not have a
 * MessageErrorType code. Check the Error field for the name
 * of the exception.
 *
 * Error code MessageErrorType_UnknownError (16)
 *
 */
class UnknownException : public RobotRaconteurException
{
  public:
    /**
     * @brief Construct an UnknownException
     *
     * @param error The name of the exception
     * @param message Message for the user
     * @param sub_name Optional error sub_name
     * @param param_ Optional error param
     */
    UnknownException(const std::string& error, const std::string& message, std::string sub_name = "",
                     const RR_INTRUSIVE_PTR<RRValue>& param_ = RR_INTRUSIVE_PTR<RRValue>());
};

/**
 * @brief Exception thrown when an invalid operation is attempted
 *
 * Error code MessageErrorType_InvalidOperation (17)
 *
 */
class InvalidOperationException : public RobotRaconteurException
{
  public:
    /**
     * @brief Construct an InvalidOperationException
     *
     * @param message Message for the user
     * @param sub_name Optional error sub_name
     * @param param_ Optional error param
     */
    InvalidOperationException(const std::string& message, std::string sub_name = "",
                              const RR_INTRUSIVE_PTR<RRValue>& param_ = RR_INTRUSIVE_PTR<RRValue>());
};

/**
 * @brief Exception thrown for an invalid argument
 *
 * Error code MessageErrorType_InvalidArgument (18)
 *
 */
class InvalidArgumentException : public RobotRaconteurException
{
  public:
    /**
     * @brief Construct an InvalidArgumentException
     *
     * @param message Message for the user
     * @param sub_name Optional error sub_name
     * @param param_ Optional error param
     */
    InvalidArgumentException(const std::string& message, std::string sub_name = "",
                             const RR_INTRUSIVE_PTR<RRValue>& param_ = RR_INTRUSIVE_PTR<RRValue>());
};

/**
 * @brief Exception thrown when an operation fails
 *
 * Error code MessageErrorType_OperationFailed (19)
 *
 */
class OperationFailedException : public RobotRaconteurException
{
  public:
    /**
     * @brief Construct an OperationFailedException
     *
     * @param message Message for the user
     * @param sub_name Optional error sub_name
     * @param param_ Optional error param
     */
    OperationFailedException(const std::string& message, std::string sub_name = "",
                             const RR_INTRUSIVE_PTR<RRValue>& param_ = RR_INTRUSIVE_PTR<RRValue>());
};

/**
 * @brief Exception thrown for an unexpected null value
 *
 * Error code MessageErrorType_NullValue (20)
 *
 */
class NullValueException : public RobotRaconteurException
{
  public:
    /**
     * @brief Construct a NullValueException
     *
     * @param message Message for the user
     * @param sub_name Optional error sub_name
     * @param param_ Optional error param
     */
    NullValueException(const std::string& message, std::string sub_name = "",
                       const RR_INTRUSIVE_PTR<RRValue>& param_ = RR_INTRUSIVE_PTR<RRValue>());
};

/**
 * @brief Exception thrown when an internal error has occurred
 *
 * Error code MessageErrorType_InternalError (21)
 *
 */
class InternalErrorException : public RobotRaconteurException
{
  public:
    /**
     * @brief Construct an InternalErrorException
     *
     * @param message Message for the user
     * @param sub_name Optional error sub_name
     * @param param_ Optional error param
     */
    InternalErrorException(const std::string& message, std::string sub_name = "",
                           const RR_INTRUSIVE_PTR<RRValue>& param_ = RR_INTRUSIVE_PTR<RRValue>());
};

/**
 * @brief Exception thrown when permission is denied to a service member
 *
 * Error code MessageErrorType_PermissionDenied (152)
 *
 */
class PermissionDeniedException : public RobotRaconteurException
{
  public:
    /**
     * @brief Construct a PermissionDeniedException
     *
     * @param message Message for the user
     * @param sub_name Optional error sub_name
     * @param param_ Optional error param
     */
    PermissionDeniedException(const std::string& message, std::string sub_name = "",
                              const RR_INTRUSIVE_PTR<RRValue>& param_ = RR_INTRUSIVE_PTR<RRValue>());
};

/**
 * @brief Exception thrown when permission to a system resource is denied
 *
 * Error code MessageErrorType_SystemResourcePermissionDenied (22)
 *
 */
class SystemResourcePermissionDeniedException : public RobotRaconteurException
{
  public:
    /**
     * @brief Construct a SystemResourcePermissionDeniedException
     *
     * @param message Message for the user
     * @param sub_name Optional error sub_name
     * @param param_ Optional error param
     */
    SystemResourcePermissionDeniedException(const std::string& message, std::string sub_name = "",
                                            const RR_INTRUSIVE_PTR<RRValue>& param_ = RR_INTRUSIVE_PTR<RRValue>());
};

/**
 * @brief Exception thrown when a system resource has been exhausted
 *
 * Error code MessageErrorType_OutOfsystemResource (23)
 *
 */
class OutOfSystemResourceException : public RobotRaconteurException
{
  public:
    /**
     * @brief Construct an OutOfSystemResourceException
     *
     * @param message Message for the user
     * @param sub_name Optional error sub_name
     * @param param_ Optional error param
     */
    OutOfSystemResourceException(const std::string& message, std::string sub_name = "",
                                 const RR_INTRUSIVE_PTR<RRValue>& param_ = RR_INTRUSIVE_PTR<RRValue>());
};

/**
 * @brief Exception thrown when a system resource error occurs
 *
 * Error code MessageErrorType_SystemResourceException (24)
 *
 */
class SystemResourceException : public RobotRaconteurException
{
  public:
    /**
     * @brief Construct a SystemResourceException
     *
     * @param message Message for the user
     * @param sub_name Optional error sub_name
     * @param param_ Optional error param
     */
    SystemResourceException(const std::string& message, std::string sub_name = "",
                            const RR_INTRUSIVE_PTR<RRValue>& param_ = RR_INTRUSIVE_PTR<RRValue>());
};

/**
 * @brief Exception thrown when a system resource is not found
 *
 * Error code MessageErrorType_ResourceNotFound (25)
 *
 */
class ResourceNotFoundException : public RobotRaconteurException
{
  public:
    /**
     * @brief Construct a ResourceNotFoundException
     *
     * @param message Message for the user
     * @param sub_name Optional error sub_name
     * @param param_ Optional error param
     */
    ResourceNotFoundException(const std::string& message, std::string sub_name = "",
                              const RR_INTRUSIVE_PTR<RRValue>& param_ = RR_INTRUSIVE_PTR<RRValue>());
};

/**
 * @brief Exception thrown when an input/output error occurs
 *
 * Error code MessageErrorType_IOError (26)
 *
 */
class IOException : public RobotRaconteurException
{
  public:
    /**
     * @brief Construct an IOException
     *
     * @param message Message for the user
     * @param sub_name Optional error sub_name
     * @param param_ Optional error param
     */
    IOException(const std::string& message, std::string sub_name = "",
                const RR_INTRUSIVE_PTR<RRValue>& param_ = RR_INTRUSIVE_PTR<RRValue>());
};

/**
 * @brief Exception thrown when a transport buffer limit is violated
 *
 * This exception typically occurs if there is a bug in
 * serialization/deserialization, or the data stream
 * has been corrupted
 *
 * Error code MessageErrorType_BufferLimitViolation (27)
 *
 */
class BufferLimitViolationException : public RobotRaconteurException
{
  public:
    /**
     * @brief Construct a BufferLimitViolationException
     *
     * @param message Message for the user
     * @param sub_name Optional error sub_name
     * @param param_ Optional error param
     */
    BufferLimitViolationException(const std::string& message, std::string sub_name = "",
                                  const RR_INTRUSIVE_PTR<RRValue>& param_ = RR_INTRUSIVE_PTR<RRValue>());
};

/**
 * @brief Exception thrown when a service definition cannot be
 * parsed or fails verification
 *
 * Error code MessageErrorType_ServiceDefinitionError (28)
 *
 */
class ServiceDefinitionException : public RobotRaconteurException
{
  public:
    /**
     * @brief Construct a new ServiceDefinitionException
     *
     * @param message Message for the user
     * @param sub_name Optional error sub_name
     * @param param_ Optional error param
     */
    ServiceDefinitionException(const std::string& message, std::string sub_name = "",
                               const RR_INTRUSIVE_PTR<RRValue>& param_ = RR_INTRUSIVE_PTR<RRValue>());
};

/**
 * @brief Exception thrown when an attempt to acces an array or container
 * index is out of range
 *
 * Error code MessageErrorType_OutOfRange (29)
 *
 */
class OutOfRangeException : public RobotRaconteurException
{
  public:
    /**
     * @brief Construct an OutOfRangeException
     *
     * @param message Message for the user
     * @param sub_name Optional error sub_name
     * @param param_ Optional error param
     */
    OutOfRangeException(const std::string& message, std::string sub_name = "",
                        const RR_INTRUSIVE_PTR<RRValue>& param_ = RR_INTRUSIVE_PTR<RRValue>());
};

/**
 * @brief Exception thrown when a key is not found in a map
 *
 * Error code MessageErrorType_KeyNotFound (30)
 *
 */
class KeyNotFoundException : public RobotRaconteurException
{
  public:
    /**
     * @brief Construct a KeyNotFound exception
     *
     * @param message Message for the user
     * @param sub_name Optional error sub_name
     * @param param_ Optional error param
     */

    KeyNotFoundException(const std::string& message, std::string sub_name = "",
                         const RR_INTRUSIVE_PTR<RRValue>& param_ = RR_INTRUSIVE_PTR<RRValue>());
};

/**
 * @brief Exception thrown when an invalid configuration is specified or encountered
 *
 * Error code MessageErrorType_InvalidConfiguration (31)
 *
 */
class InvalidConfigurationException : public RobotRaconteurException
{
  public:
    /**
     * @brief Construct an InvalidConfiguration exception
     *
     * @param message Message for the user
     * @param sub_name Optional error sub_name
     * @param param_ Optional error param
     */

    InvalidConfigurationException(const std::string& message, std::string sub_name = "",
                                  const RR_INTRUSIVE_PTR<RRValue>& param_ = RR_INTRUSIVE_PTR<RRValue>());
};

/**
 * @brief Exception thrown when an invalid state is specified or encountered
 *
 * Error code MessageErrorType_InvalidState (32)
 *
 */
class InvalidStateException : public RobotRaconteurException
{
  public:
    /**
     * @brief Construct an InvalidState exception
     *
     * @param message Message for the user
     * @param sub_name Optional error sub_name
     * @param param_ Optional error param
     */

    InvalidStateException(const std::string& message, std::string sub_name = "",
                          const RR_INTRUSIVE_PTR<RRValue>& param_ = RR_INTRUSIVE_PTR<RRValue>());
};

/**
 * @brief Exception thrown when an error occurs on a remote
 * member request
 *
 * User defined exceptions declared in service definitions extend
 * RobotRaconteurRemoteException
 *
 * Error code MessageErrorType_RemoteError (100)
 *
 */
class RobotRaconteurRemoteException : public RobotRaconteurException
{
  public:
    /**
     * @brief Construct a RobotRaconteurRemoteException
     *
     * error should be the fully qualified type of the exception
     * for user defined exceptions
     *
     * @param error The name of the exception
     * @param message Message for the user
     * @param sub_name Optional error sub_name
     * @param param_ Optional error param
     */
    RobotRaconteurRemoteException(const std::string& error, const std::string& message, std::string sub_name = "",
                                  const RR_INTRUSIVE_PTR<RRValue>& param_ = RR_INTRUSIVE_PTR<RRValue>());

    /**
     * @brief Construct a RobotRaconteurRemoteException from an existing exception
     *
     * This constructor will use RTTI and what() to fill
     * the exception
     *
     * @param innerexception Used to initialize the exception
     */
    RobotRaconteurRemoteException(std::exception& innerexception);
};

/**
 * @brief The request timed out
 *
 * See RobotRaconteurNode::SetRequestTimeout(),
 * or the timeout passed to an asynchronous request
 *
 * Error code MessageErrorType_RequestTimeout (101)
 *
 */
class RequestTimeoutException : public RobotRaconteurException
{
  public:
    /**
     * @brief Construct a RequestTimeoutException
     *
     * @param message Message for the user
     * @param sub_name Optional error sub_name
     * @param param_ Optional error param
     */
    RequestTimeoutException(const std::string& message, std::string sub_name = "",
                            const RR_INTRUSIVE_PTR<RRValue>& param_ = RR_INTRUSIVE_PTR<RRValue>());
};

/**
 * @brief An attempt was made to write/set a read only member
 *
 * Error code MessageErrorType_ReadOnlyMember (102)
 *
 */
class ReadOnlyMemberException : public RobotRaconteurException
{
  public:
    /**
     * @brief Construct a ReadOnlyMemberException
     *
     * @param message Message for the user
     * @param sub_name Optional error sub_name
     * @param param_ Optional error param
     */
    ReadOnlyMemberException(const std::string& message, std::string sub_name = "",
                            const RR_INTRUSIVE_PTR<RRValue>& param_ = RR_INTRUSIVE_PTR<RRValue>());
};

/**
 * @brief An attempt was mode to read/get a write only member
 *
 * Error code MessageErrorType_WriteOnlyMember (103)
 *
 */
class WriteOnlyMemberException : public RobotRaconteurException
{
  public:
    /**
     * @brief Construct a WriteOnlyMemberException
     *
     * @param message Message for the user
     * @param sub_name Optional error sub_name
     * @param param_ Optional error param
     */
    WriteOnlyMemberException(const std::string& message, std::string sub_name = "",
                             const RR_INTRUSIVE_PTR<RRValue>& param_ = RR_INTRUSIVE_PTR<RRValue>());
};

/**
 * @brief Exception thrown if a member is not implemented
 *
 * Error code MessageErrorType_NotImplementedError (104)
 *
 */
class NotImplementedException : public RobotRaconteurException
{
  public:
    /**
     * @brief Construct a NotImplementedException
     *
     * @param message Message for the user
     * @param sub_name Optional error sub_name
     * @param param_ Optional error param
     */
    NotImplementedException(const std::string& message, std::string sub_name = "",
                            const RR_INTRUSIVE_PTR<RRValue>& param_ = RR_INTRUSIVE_PTR<RRValue>());
};

/**
 * @brief Thrown is a member is busy. Retry later
 *
 * Error code MessageErrorType_MemberBusy (105)
 *
 */
class MemberBusyException : public RobotRaconteurException
{
  public:
    /**
     * @brief Construct a MemberBusyException
     *
     * @param message Message for the user
     * @param sub_name Optional error sub_name
     * @param param_ Optional error param
     */
    MemberBusyException(const std::string& message, std::string sub_name = "",
                        const RR_INTRUSIVE_PTR<RRValue>& param_ = RR_INTRUSIVE_PTR<RRValue>());
};

/**
 * @brief Exception thrown if a value has not been set
 *
 * This exception is most often used by WireConnection::GetInValue()
 * and WireConnection::GetOutValue() if InValue or OutValue
 * have not been received or set
 *
 * Error code MessageErrorType_ValueNotSet (106)
 *
 */
class ValueNotSetException : public RobotRaconteurException
{
  public:
    /**
     * @brief Construct a ValueNotSetException
     *
     * @param message Message for the user
     * @param sub_name Optional error sub_name
     * @param param_ Optional error param
     */
    ValueNotSetException(const std::string& message, std::string sub_name = "",
                         const RR_INTRUSIVE_PTR<RRValue>& param_ = RR_INTRUSIVE_PTR<RRValue>());
};

/**
 * @brief Exception thrown when authentication is required or attempt
 * to authenticate fails
 *
 * Error code MessageErrorType_AuthenticationError (150)
 *
 */
class AuthenticationException : public RobotRaconteurException
{
  public:
    /**
     * @brief Construct an AuthenticationException
     *
     * @param message Message for the user
     * @param sub_name Optional error sub_name
     * @param param_ Optional error param
     */
    AuthenticationException(const std::string& message, std::string sub_name = "",
                            const RR_INTRUSIVE_PTR<RRValue>& param_ = RR_INTRUSIVE_PTR<RRValue>());
};

/**
 * @brief Exception thrown when attempting to access a locked
 * service object
 *
 * Service objects can be locked using RobotRaconteurNode::RequestObjectLock().
 * This exception is thrown if an attempt is made to access a service object
 * (or sub-object) that has been locked by another user or session.
 *
 * Error code MessageErrorType_ObjectLockedError (151)
 *
 */
class ObjectLockedException : public RobotRaconteurException
{
  public:
    /**
     * @brief Construct an ObjectLockedException
     *
     * @param message Message for the user
     * @param sub_name Optional error sub_name
     * @param param_ Optional error param
     */
    ObjectLockedException(const std::string& message, std::string sub_name = "",
                          const RR_INTRUSIVE_PTR<RRValue>& param_ = RR_INTRUSIVE_PTR<RRValue>());
};

/**
 * @brief Exception passed to generators to trigger an abort
 *
 * This is typically not thrown or received by the user
 *
 * Error code MessageErrorType_AbortOperation (107)
 *
 */
class AbortOperationException : public RobotRaconteurException
{
  public:
    /**
     * @brief Construct an AbortOperationException
     *
     * @param message Message for the user
     * @param sub_name Optional error sub_name
     * @param param_ Optional error param
     */
    AbortOperationException(const std::string& message, std::string sub_name = "",
                            const RR_INTRUSIVE_PTR<RRValue>& param_ = RR_INTRUSIVE_PTR<RRValue>());
};

/**
 * @brief Exception thrown when an operation is aborted
 *
 * This is thrown be generator functions when Abort()
 * is called
 *
 * Generators are destroyed after throwing
 * OperationAbortedException during Next()
 *
 * This error is passed to generators to trigger an abort
 *
 * Error code MessageErrorType_OperationAborted (108)
 *
 */
class OperationAbortedException : public RobotRaconteurException
{
  public:
    /**
     * @brief Construct a OperationAbortedException
     *
     * @param message Message for the user
     * @param sub_name Optional error sub_name
     * @param param_ Optional error param
     */
    OperationAbortedException(const std::string& message, std::string sub_name = "",
                              const RR_INTRUSIVE_PTR<RRValue>& param_ = RR_INTRUSIVE_PTR<RRValue>());
};

/**
 * @brief Exception thrown when a generator has finished sending results
 *
 * StopIterationException is not an error condition. It signals
 * that a generator is finished sending results.
 *
 * This error is passed to generators to trigger a close
 *
 * Error code MessageErrorType_StopIteration (109)
 *
 */
class StopIterationException : public RobotRaconteurException
{
  public:
    /**
     * @brief Construct a StopIterationException
     *
     * @param message Message for the user
     * @param sub_name Optional error sub_name
     * @param param_ Optional error param
     */
    StopIterationException(const std::string& message, std::string sub_name = "",
                           const RR_INTRUSIVE_PTR<RRValue>& param_ = RR_INTRUSIVE_PTR<RRValue>());
};

/**
 * @brief Exception thrown when an operation does not complete in the expected time
 *
 *
 * Error code MessageErrorType_OperationTimeout (110)
 *
 */
class OperationTimeoutException : public RobotRaconteurException
{
  public:
    /**
     * @brief Construct an OperationTimeoutException
     *
     * @param message Message for the user
     * @param sub_name Optional error sub_name
     * @param param_ Optional error param
     */
    OperationTimeoutException(const std::string& message, std::string sub_name = "",
                              const RR_INTRUSIVE_PTR<RRValue>& param_ = RR_INTRUSIVE_PTR<RRValue>());
};

/**
 * @brief Exception thrown when an operation is cancelled before it is started
 *
 *
 * Error code MessageErrorType_OperationCancelled (111)
 *
 */
class OperationCancelledException : public RobotRaconteurException
{
  public:
    /**
     * @brief Construct an OperationCancelledException
     *
     * @param message Message for the user
     * @param sub_name Optional error sub_name
     * @param param_ Optional error param
     */
    OperationCancelledException(const std::string& message, std::string sub_name = "",
                                const RR_INTRUSIVE_PTR<RRValue>& param_ = RR_INTRUSIVE_PTR<RRValue>());
};

#ifndef BOOST_NO_CXX11_TEMPLATE_ALIASES
/** @brief Convenience alias for ConnectionException shared_ptr */
using ConnectionExceptionPtr = RR_SHARED_PTR<ConnectionException>();
/** @brief Convenience alias for ConnectionException const shared_ptr */
using ConnectionExceptionConstPtr = RR_SHARED_PTR<const ConnectionException>();
/** @brief Convenience alias for ProtocolException shared_ptr */
using ProtocolExceptionPtr = RR_SHARED_PTR<ProtocolException>();
/** @brief Convenience alias for ProtocolException const shared_ptr */
using ProtocolExceptionConstPtr = RR_SHARED_PTR<const ProtocolException>();
/** @brief Convenience alias for ServiceNotFoundException shared_ptr */
using ServiceNotFoundExceptionPtr = RR_SHARED_PTR<ServiceNotFoundException>();
/** @brief Convenience alias for ServiceNotFoundException const shared_ptr */
using ServiceNotFoundExceptionConstPtr = RR_SHARED_PTR<const ServiceNotFoundException>();
/** @brief Convenience alias for ObjectNotFoundException shared_ptr */
using ObjectNotFoundExceptionPtr = RR_SHARED_PTR<ObjectNotFoundException>();
/** @brief Convenience alias for ObjectNotFoundException const shared_ptr */
using ObjectNotFoundExceptionConstPtr = RR_SHARED_PTR<const ObjectNotFoundException>();
/** @brief Convenience alias for InvalidEndpointException shared_ptr */
using InvalidEndpointExceptionPtr = RR_SHARED_PTR<InvalidEndpointException>();
/** @brief Convenience alias for InvalidEndpointException const shared_ptr */
using InvalidEndpointExceptionConstPtr = RR_SHARED_PTR<const InvalidEndpointException>();
/** @brief Convenience alias for EndpointCommunicationFatalException shared_ptr */
using EndpointCommunicationFatalExceptionPtr = RR_SHARED_PTR<EndpointCommunicationFatalException>();
/** @brief Convenience alias for EndpointCommunicationFatalException const shared_ptr */
using EndpointCommunicationFatalExceptionConstPtr = RR_SHARED_PTR<const EndpointCommunicationFatalException>();
/** @brief Convenience alias for NodeNotFoundException shared_ptr */
using NodeNotFoundExceptionPtr = RR_SHARED_PTR<NodeNotFoundException>();
/** @brief Convenience alias for NodeNotFoundException const shared_ptr */
using NodeNotFoundExceptionConstPtr = RR_SHARED_PTR<const NodeNotFoundException>();
/** @brief Convenience alias for ServiceException shared_ptr */
using ServiceExceptionPtr = RR_SHARED_PTR<ServiceException>();
/** @brief Convenience alias for ServiceException const shared_ptr */
using ServiceExceptionConstPtr = RR_SHARED_PTR<const ServiceException>();
/** @brief Convenience alias for MemberNotFoundException shared_ptr */
using MemberNotFoundExceptionPtr = RR_SHARED_PTR<MemberNotFoundException>();
/** @brief Convenience alias for MemberNotFoundException const shared_ptr */
using MemberNotFoundExceptionConstPtr = RR_SHARED_PTR<const MemberNotFoundException>();
/** @brief Convenience alias for MemberFormatMismatchException shared_ptr */
using MemberFormatMismatchExceptionPtr = RR_SHARED_PTR<MemberFormatMismatchException>();
/** @brief Convenience alias for MemberFormatMismatchException const shared_ptr */
using MemberFormatMismatchExceptionConstPtr = RR_SHARED_PTR<const MemberFormatMismatchException>();
/** @brief Convenience alias for DataTypeMismatchException shared_ptr */
using DataTypeMismatchExceptionPtr = RR_SHARED_PTR<DataTypeMismatchException>();
/** @brief Convenience alias for DataTypeMismatchException const shared_ptr */
using DataTypeMismatchExceptionConstPtr = RR_SHARED_PTR<const DataTypeMismatchException>();
/** @brief Convenience alias for DataTypeException shared_ptr */
using DataTypeExceptionPtr = RR_SHARED_PTR<DataTypeException>();
/** @brief Convenience alias for DataTypeException const shared_ptr */
using DataTypeExceptionConstPtr = RR_SHARED_PTR<const DataTypeException>();
/** @brief Convenience alias for DataSerializationException shared_ptr */
using DataSerializationExceptionPtr = RR_SHARED_PTR<DataSerializationException>();
/** @brief Convenience alias for DataSerializationException const shared_ptr */
using DataSerializationExceptionConstPtr = RR_SHARED_PTR<const DataSerializationException>();
/** @brief Convenience alias for MessageEntryNotFoundException shared_ptr */
using MessageEntryNotFoundExceptionPtr = RR_SHARED_PTR<MessageEntryNotFoundException>();
/** @brief Convenience alias for MessageEntryNotFoundException const shared_ptr */
using MessageEntryNotFoundExceptionConstPtr = RR_SHARED_PTR<const MessageEntryNotFoundException>();
/** @brief Convenience alias for MessageElementNotFoundException shared_ptr */
using MessageElementNotFoundExceptionPtr = RR_SHARED_PTR<MessageElementNotFoundException>();
/** @brief Convenience alias for MessageElementNotFoundException const shared_ptr */
using MessageElementNotFoundExceptionConstPtr = RR_SHARED_PTR<const MessageElementNotFoundException>();
/** @brief Convenience alias for UnknownException shared_ptr */
using UnknownExceptionPtr = RR_SHARED_PTR<UnknownException>();
/** @brief Convenience alias for UnknownException const shared_ptr */
using UnknownExceptionConstPtr = RR_SHARED_PTR<const UnknownException>();
/** @brief Convenience alias for InvalidOperationException shared_ptr */
using InvalidOperationExceptionPtr = RR_SHARED_PTR<InvalidOperationException>();
/** @brief Convenience alias for InvalidOperationException const shared_ptr */
using InvalidOperationExceptionConstPtr = RR_SHARED_PTR<const InvalidOperationException>();
/** @brief Convenience alias for InvalidArgumentException shared_ptr */
using InvalidArgumentExceptionPtr = RR_SHARED_PTR<InvalidArgumentException>();
/** @brief Convenience alias for InvalidArgumentException const shared_ptr */
using InvalidArgumentExceptionConstPtr = RR_SHARED_PTR<const InvalidArgumentException>();
/** @brief Convenience alias for OperationFailedException shared_ptr */
using OperationFailedExceptionPtr = RR_SHARED_PTR<OperationFailedException>();
/** @brief Convenience alias for OperationFailedException const shared_ptr */
using OperationFailedExceptionConstPtr = RR_SHARED_PTR<const OperationFailedException>();
/** @brief Convenience alias for NullValueException shared_ptr */
using NullValueExceptionPtr = RR_SHARED_PTR<NullValueException>();
/** @brief Convenience alias for NullValueException const shared_ptr */
using NullValueExceptionConstPtr = RR_SHARED_PTR<const NullValueException>();
/** @brief Convenience alias for InternalErrorException shared_ptr */
using InternalErrorExceptionPtr = RR_SHARED_PTR<InternalErrorException>();
/** @brief Convenience alias for InternalErrorException const shared_ptr */
using InternalErrorExceptionConstPtr = RR_SHARED_PTR<const InternalErrorException>();
/** @brief Convenience alias for SystemResourcePermissionDeniedException shared_ptr */
using SystemResourcePermissionDeniedExceptionPtr = RR_SHARED_PTR<SystemResourcePermissionDeniedException>();
/** @brief Convenience alias for SystemResourcePermissionDeniedException const shared_ptr */
using SystemResourcePermissionDeniedExceptionConstPtr = RR_SHARED_PTR<const SystemResourcePermissionDeniedException>();
/** @brief Convenience alias for OutOfSystemResourceException shared_ptr */
using OutOfSystemResourceExceptionPtr = RR_SHARED_PTR<OutOfSystemResourceException>();
/** @brief Convenience alias for OutOfSystemResourceException const shared_ptr */
using OutOfSystemResourceExceptionConstPtr = RR_SHARED_PTR<const OutOfSystemResourceException>();
/** @brief Convenience alias for SystemResourceException shared_ptr */
using SystemResourceExceptionPtr = RR_SHARED_PTR<SystemResourceException>();
/** @brief Convenience alias for SystemResourceException const shared_ptr */
using SystemResourceExceptionConstPtr = RR_SHARED_PTR<const SystemResourceException>();
/** @brief Convenience alias for ResourceNotFoundException shared_ptr */
using ResourceNotFoundExceptionPtr = RR_SHARED_PTR<ResourceNotFoundException>();
/** @brief Convenience alias for ResourceNotFoundException const shared_ptr */
using ResourceNotFoundExceptionConstPtr = RR_SHARED_PTR<const ResourceNotFoundException>();
/** @brief Convenience alias for IOException shared_ptr */
using IOExceptionPtr = RR_SHARED_PTR<IOException>();
/** @brief Convenience alias for IOException const shared_ptr */
using IOExceptionConstPtr = RR_SHARED_PTR<const IOException>();
/** @brief Convenience alias for BufferLimitViolationException shared_ptr */
using BufferLimitViolationExceptionPtr = RR_SHARED_PTR<BufferLimitViolationException>();
/** @brief Convenience alias for BufferLimitViolationException const shared_ptr */
using BufferLimitViolationExceptionConstPtr = RR_SHARED_PTR<const BufferLimitViolationException>();
/** @brief Convenience alias for ServiceDefinitionException shared_ptr */
using ServiceDefinitionExceptionPtr = RR_SHARED_PTR<ServiceDefinitionException>();
/** @brief Convenience alias for ServiceDefinitionException const shared_ptr */
using ServiceDefinitionExceptionConstPtr = RR_SHARED_PTR<const ServiceDefinitionException>();
/** @brief Convenience alias for OutOfRangeException shared_ptr */
using OutOfRangeExceptionPtr = RR_SHARED_PTR<OutOfRangeException>();
/** @brief Convenience alias for OutOfRangeException const shared_ptr */
using OutOfRangeExceptionConstPtr = RR_SHARED_PTR<const OutOfRangeException>();
/** @brief Convenience alias for KeyNotFoundException shared_ptr */
using KeyNotFoundExceptionPtr = RR_SHARED_PTR<KeyNotFoundException>();
/** @brief Convenience alias for KeyNotFoundException const shared_ptr */
using KeyNotFoundExceptionConstPtr = RR_SHARED_PTR<const KeyNotFoundException>();
/** @brief Convenience alias for InvalidConfigurationException shared_ptr */
using InvalidConfigurationExceptionPtr = RR_SHARED_PTR<InvalidConfigurationException>();
/** @brief Convenience alias for InvalidConfigurationException const shared_ptr */
using InvalidConfigurationExceptionConstPtr = RR_SHARED_PTR<const InvalidConfigurationException>();
/** @brief Convenience alias for InvalidStateException shared_ptr */
using InvalidStateExceptionPtr = RR_SHARED_PTR<InvalidStateException>();
/** @brief Convenience alias for InvalidStateException const shared_ptr */
using InvalidStateExceptionConstPtr = RR_SHARED_PTR<const InvalidStateException>();
/** @brief Convenience alias for RobotRaconteurRemoteException shared_ptr */
using RobotRaconteurRemoteExceptionPtr = RR_SHARED_PTR<RobotRaconteurRemoteException>();
/** @brief Convenience alias for RobotRaconteurRemoteException const shared_ptr */
using RobotRaconteurRemoteExceptionConstPtr = RR_SHARED_PTR<const RobotRaconteurRemoteException>();
/** @brief Convenience alias for RequestTimeoutException shared_ptr */
using RequestTimeoutExceptionPtr = RR_SHARED_PTR<RequestTimeoutException>();
/** @brief Convenience alias for RequestTimeoutException const shared_ptr */
using RequestTimeoutExceptionConstPtr = RR_SHARED_PTR<const RequestTimeoutException>();
/** @brief Convenience alias for ReadOnlyMemberException shared_ptr */
using ReadOnlyMemberExceptionPtr = RR_SHARED_PTR<ReadOnlyMemberException>();
/** @brief Convenience alias for ReadOnlyMemberException const shared_ptr */
using ReadOnlyMemberExceptionConstPtr = RR_SHARED_PTR<const ReadOnlyMemberException>();
/** @brief Convenience alias for WriteOnlyMemberException shared_ptr */
using WriteOnlyMemberExceptionPtr = RR_SHARED_PTR<WriteOnlyMemberException>();
/** @brief Convenience alias for WriteOnlyMemberException const shared_ptr */
using WriteOnlyMemberExceptionConstPtr = RR_SHARED_PTR<const WriteOnlyMemberException>();
/** @brief Convenience alias for NotImplementedException shared_ptr */
using NotImplementedExceptionPtr = RR_SHARED_PTR<NotImplementedException>();
/** @brief Convenience alias for NotImplementedException const shared_ptr */
using NotImplementedExceptionConstPtr = RR_SHARED_PTR<const NotImplementedException>();
/** @brief Convenience alias for MemberBusyException shared_ptr */
using MemberBusyExceptionPtr = RR_SHARED_PTR<MemberBusyException>();
/** @brief Convenience alias for MemberBusyException const shared_ptr */
using MemberBusyExceptionConstPtr = RR_SHARED_PTR<const MemberBusyException>();
/** @brief Convenience alias for ValueNotSetException shared_ptr */
using ValueNotSetExceptionPtr = RR_SHARED_PTR<ValueNotSetException>();
/** @brief Convenience alias for ValueNotSetException const shared_ptr */
using ValueNotSetExceptionConstPtr = RR_SHARED_PTR<const ValueNotSetException>();
/** @brief Convenience alias for AuthenticationException shared_ptr */
using AuthenticationExceptionPtr = RR_SHARED_PTR<AuthenticationException>();
/** @brief Convenience alias for AuthenticationException const shared_ptr */
using AuthenticationExceptionConstPtr = RR_SHARED_PTR<const AuthenticationException>();
/** @brief Convenience alias for ObjectLockedException shared_ptr */
using ObjectLockedExceptionPtr = RR_SHARED_PTR<ObjectLockedException>();
/** @brief Convenience alias for ObjectLockedException const shared_ptr */
using ObjectLockedExceptionConstPtr = RR_SHARED_PTR<const ObjectLockedException>();
/** @brief Convenience alias for PermissionDeniedException shared_ptr */
using PermissionDeniedExceptionPtr = RR_SHARED_PTR<PermissionDeniedException>();
/** @brief Convenience alias for PermissionDeniedException const shared_ptr */
using PermissionDeniedExceptionConstPtr = RR_SHARED_PTR<const PermissionDeniedException>();
/** @brief Convenience alias for AbortOperationException shared_ptr */
using AbortOperationExceptionPtr = RR_SHARED_PTR<AbortOperationException>();
/** @brief Convenience alias for AbortOperationException const shared_ptr */
using AbortOperationExceptionConstPtr = RR_SHARED_PTR<const AbortOperationException>();
/** @brief Convenience alias for OperationAbortedException shared_ptr */
using OperationAbortedExceptionPtr = RR_SHARED_PTR<OperationAbortedException>();
/** @brief Convenience alias for OperationAbortedException const shared_ptr */
using OperationAbortedExceptionConstPtr = RR_SHARED_PTR<const OperationAbortedException>();
/** @brief Convenience alias for StopIterationException shared_ptr */
using StopIterationExceptionPtr = RR_SHARED_PTR<StopIterationException>();
/** @brief Convenience alias for StopIterationException const shared_ptr */
using StopIterationExceptionConstPtr = RR_SHARED_PTR<const StopIterationException>();
/** @brief Convenience alias for OperationTimeoutException shared_ptr */
using OperationTimeoutExceptionPtr = RR_SHARED_PTR<OperationTimeoutException>();
/** @brief Convenience alias for OperationTimeoutException const shared_ptr */
using OperationTimeoutExceptionConstPtr = RR_SHARED_PTR<const OperationTimeoutException>();
/** @brief Convenience alias for OperationCancelledException shared_ptr */
using OperationCancelledExceptionPtr = RR_SHARED_PTR<OperationCancelledException>();
/** @brief Convenience alias for OperationCancelledException const shared_ptr */
using OperationCancelledExceptionConstPtr = RR_SHARED_PTR<const OperationCacnelledException>();
#endif

#endif // GENERATING_DOCUMENTATION

#ifndef BOOST_NO_CXX11_TEMPLATE_ALIASES
/** @brief Convenience alias for RobotRaconteurException shared_ptr */
using RobotRaconteurExceptionPtr = RR_SHARED_PTR<RobotRaconteurException>;
/** @brief Convenience alias for RobotRaconteurException const shared_ptr */
using RobotRaconteurExceptionConstPtr = RR_SHARED_PTR<const RobotRaconteurException>;

#define RR_EXCEPTION_DECL_1_PTR(exp_cpp_type, exp_code, exp_type_str)                                                  \
    using exp_cpp_type##Ptr = RR_SHARED_PTR<exp_cpp_type>;                                                             \
    using exp_cpp_type##ConstPtr = RR_SHARED_PTR<const exp_cpp_type>;

#define RR_EXCEPTION_DECL_2_PTR(exp_cpp_type, exp_code)                                                                \
    using exp_cpp_type##Ptr = RR_SHARED_PTR<exp_cpp_type>;                                                             \
    using exp_cpp_type##ConstPtr = RR_SHARED_PTR<const exp_cpp_type>;

RR_EXCEPTION_TYPES_INIT(RR_EXCEPTION_DECL_1_PTR, RR_EXCEPTION_DECL_2_PTR)

#undef RR_EXCEPTION_DECL_1_PTR
#undef RR_EXCEPTION_DECL_2_PTR
#endif

} // namespace RobotRaconteur
