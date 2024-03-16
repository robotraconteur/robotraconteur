==========
Exceptions
==========

RobotRaconteurException
=======================

.. class:: RobotRaconteur.RobotRaconteurException(errorcode,errorname,message,errorsubname=None,errorparam=None)

   Base class for Robot Raconteur exceptions

   RobotRaconteurException represents an exception that can be serialized and passed
   to a remote node. The MessageErrorType contains an enum of predefined error
   codes that represent different potential errors. Each error code has a corresponding
   Python exception that extends RobotRaconteurException.

   User defined exception types can be declared in Service Definition files. The
   Python exception is accessed using RobotRaconteurNode.GetExceptionType()

   Construct a RobotRaconteurException

   :param errorcode: The predefined error code for the exception
   :type errorcode: int
   :param errorname: The error type name corresponding to the ErrorCode, or a user defined
    error type
   :type errorname: str
   :param message: A message to pass to the user
   :type message: str
   :param errorsubname: An optional sub_name to further categorize the exception
   :type errorsubname: str
   :param errorparam: An optional exception parameter
   :type errorparam: Any

   .. attribute:: errorcode

      (int) Predefined error code

	  Error code that specifies the type of error See MessageErrorType constants.

   .. attribute:: errorname

      (str) Name of the error

      Name corresponding to predefined error code, or a user defined
      error type

   .. attribute:: message

      (str) Message to pass to the user

      Human readable message to pass to the user

   .. attribute:: errorsubname

      (str) An optional sub_name to further categorize the exception

      The sub_name should follow the Robot Raconteur naming rules. It should
      either be a single name or a fully qualified name using service definition
      naming rules

   .. attribute:: errorparam

      (Any) An optional parameter

      Use this placeholder if data needs to be passed with the exception


Other Exceptions
================

.. class:: RobotRaconteur.ConnectionException(message=,subname=None,param_=None)

   Exception thrown when connection to remote node fails

   This exception is thrown if a connection cannot be created,
   the connection fails, or the connection has been closed.

   Error code MessageErrorType_ConnectionError (1)

   Construct a ConnectionException


   :param message: A message to pass to the user
   :type message: str
   :param subname: An optional sub_name to further categorize the exception
   :type subname: str
   :param param_: An optional exception parameter
   :type param|: Any

   .. attribute:: errorcode

      (int) Predefined error code

      Error code that specifies the type of error See MessageErrorType constants.

   .. attribute:: errorname

      (str) Name of the error

      Name corresponding to predefined error code, or a user defined
      error type

   .. attribute:: message

      (str) Message to pass to the user

      Human readable message to pass to the user

   .. attribute:: errorsubname

      (str) An optional sub_name to further categorize the exception

      The sub_name should follow the Robot Raconteur naming rules. It should
      either be a single name or a fully qualified name using service definition
      naming rules

   .. attribute:: errorparam

      (Any) An optional parameter

      Use this placeholder if data needs to be passed with the exception



.. class:: RobotRaconteur.ProtocolException(message=,subname=None,param_=None)

   Exception thrown when a protocol failure occurs on
   a tranport connection

   Error code MessageErrorType_ProtocolError (2)


   Construct a ProtocolException


   :param message: A message to pass to the user
   :type message: str
   :param subname: An optional sub_name to further categorize the exception
   :type subname: str
   :param param_: An optional exception parameter
   :type param|: Any

   .. attribute:: errorcode

      (int) Predefined error code

      Error code that specifies the type of error See MessageErrorType constants.

   .. attribute:: errorname

      (str) Name of the error

      Name corresponding to predefined error code, or a user defined
      error type

   .. attribute:: message

      (str) Message to pass to the user

      Human readable message to pass to the user

   .. attribute:: errorsubname

      (str) An optional sub_name to further categorize the exception

      The sub_name should follow the Robot Raconteur naming rules. It should
      either be a single name or a fully qualified name using service definition
      naming rules

   .. attribute:: errorparam

      (Any) An optional parameter

      Use this placeholder if data needs to be passed with the exception



.. class:: RobotRaconteur.ServiceNotFoundException(message=,subname=None,param_=None)

   Exception thrown when a service cannot be found
   on a remote node

   Error code MessageErrorType_ServiceNotFound (3)


   Construct a ServiceNotFoundException


   :param message: A message to pass to the user
   :type message: str
   :param subname: An optional sub_name to further categorize the exception
   :type subname: str
   :param param_: An optional exception parameter
   :type param|: Any

   .. attribute:: errorcode

      (int) Predefined error code

      Error code that specifies the type of error See MessageErrorType constants.

   .. attribute:: errorname

      (str) Name of the error

      Name corresponding to predefined error code, or a user defined
      error type

   .. attribute:: message

      (str) Message to pass to the user

      Human readable message to pass to the user

   .. attribute:: errorsubname

      (str) An optional sub_name to further categorize the exception

      The sub_name should follow the Robot Raconteur naming rules. It should
      either be a single name or a fully qualified name using service definition
      naming rules

   .. attribute:: errorparam

      (Any) An optional parameter

      Use this placeholder if data needs to be passed with the exception



.. class:: RobotRaconteur.ObjectNotFoundException(message=,subname=None,param_=None)

   Exception thrown when a service object cannot
   be found

   This error is thrown when a specified service path
   does not have an assosciate object. The object may
   have been released by the service, or the service
   path is invalid

   Error code MessageErrorType_ObjectNotFound (4)


   Construct a ObjectNotFoundException


   :param message: A message to pass to the user
   :type message: str
   :param subname: An optional sub_name to further categorize the exception
   :type subname: str
   :param param_: An optional exception parameter
   :type param|: Any

   .. attribute:: errorcode

      (int) Predefined error code

      Error code that specifies the type of error See MessageErrorType constants.

   .. attribute:: errorname

      (str) Name of the error

      Name corresponding to predefined error code, or a user defined
      error type

   .. attribute:: message

      (str) Message to pass to the user

      Human readable message to pass to the user

   .. attribute:: errorsubname

      (str) An optional sub_name to further categorize the exception

      The sub_name should follow the Robot Raconteur naming rules. It should
      either be a single name or a fully qualified name using service definition
      naming rules

   .. attribute:: errorparam

      (Any) An optional parameter

      Use this placeholder if data needs to be passed with the exception



.. class:: RobotRaconteur.InvalidEndpointException(message=,subname=None,param_=None)

   Exception thrown when an attempt is made
   to send a message to an invalid endpoint

   Transports between two nodes terminate with a pair
   of endpoints, one in each node. If the client, service,
   service endpoint, or transport is destroyed, the endpoint
   will be deleted. This exception is thrown if the
   target endpoint is no longer available.

   Error code MessageErrorType_InvalidEndpoint (5)


   Construct a InvalidEndpointException


   :param message: A message to pass to the user
   :type message: str
   :param subname: An optional sub_name to further categorize the exception
   :type subname: str
   :param param_: An optional exception parameter
   :type param|: Any

   .. attribute:: errorcode

      (int) Predefined error code

      Error code that specifies the type of error See MessageErrorType constants.

   .. attribute:: errorname

      (str) Name of the error

      Name corresponding to predefined error code, or a user defined
      error type

   .. attribute:: message

      (str) Message to pass to the user

      Human readable message to pass to the user

   .. attribute:: errorsubname

      (str) An optional sub_name to further categorize the exception

      The sub_name should follow the Robot Raconteur naming rules. It should
      either be a single name or a fully qualified name using service definition
      naming rules

   .. attribute:: errorparam

      (Any) An optional parameter

      Use this placeholder if data needs to be passed with the exception



.. class:: RobotRaconteur.EndpointCommunicationFatalException(message=,subname=None,param_=None)

   Exception thrown when an attempt to send a
   message to an endpoint fails

   Transports between two nodes terminate with a pair
   of endpoints, one in each node. Messages are sent
   between endpoint pairs. If for some reason the endpoint
   cannot send (or receive) the message, this exception
   is thrown.

   Error code MessageErrorType_EndpointCommunicationFatalError (6)


   Construct a EndpointCommunicationFatalException


   :param message: A message to pass to the user
   :type message: str
   :param subname: An optional sub_name to further categorize the exception
   :type subname: str
   :param param_: An optional exception parameter
   :type param|: Any

   .. attribute:: errorcode

      (int) Predefined error code

      Error code that specifies the type of error See MessageErrorType constants.

   .. attribute:: errorname

      (str) Name of the error

      Name corresponding to predefined error code, or a user defined
      error type

   .. attribute:: message

      (str) Message to pass to the user

      Human readable message to pass to the user

   .. attribute:: errorsubname

      (str) An optional sub_name to further categorize the exception

      The sub_name should follow the Robot Raconteur naming rules. It should
      either be a single name or a fully qualified name using service definition
      naming rules

   .. attribute:: errorparam

      (Any) An optional parameter

      Use this placeholder if data needs to be passed with the exception



.. class:: RobotRaconteur.NodeNotFoundException(message=,subname=None,param_=None)

   Exception thrown if the specified node cannot be found

   When connecting to a service or sending a message, the NodeID
   and/or NodeName are specified. If the specified node
   cannot be found, this exception is thrown.

   Error code MessageErrorType_NodeNotFound (7)


   Construct a NodeNotFoundException


   :param message: A message to pass to the user
   :type message: str
   :param subname: An optional sub_name to further categorize the exception
   :type subname: str
   :param param_: An optional exception parameter
   :type param|: Any

   .. attribute:: errorcode

      (int) Predefined error code

      Error code that specifies the type of error See MessageErrorType constants.

   .. attribute:: errorname

      (str) Name of the error

      Name corresponding to predefined error code, or a user defined
      error type

   .. attribute:: message

      (str) Message to pass to the user

      Human readable message to pass to the user

   .. attribute:: errorsubname

      (str) An optional sub_name to further categorize the exception

      The sub_name should follow the Robot Raconteur naming rules. It should
      either be a single name or a fully qualified name using service definition
      naming rules

   .. attribute:: errorparam

      (Any) An optional parameter

      Use this placeholder if data needs to be passed with the exception



.. class:: RobotRaconteur.ServiceException(message=,subname=None,param_=None)

   Exception thrown when an exception occurs during
   an operation on a service

   ServiceException is a catch-all error for exceptions on services.
   See the message field for an explanation of the error that occured.

   Error code MessageErrorType_ServiceError (8)


   Construct a ServiceException


   :param message: A message to pass to the user
   :type message: str
   :param subname: An optional sub_name to further categorize the exception
   :type subname: str
   :param param_: An optional exception parameter
   :type param|: Any

   .. attribute:: errorcode

      (int) Predefined error code

      Error code that specifies the type of error See MessageErrorType constants.

   .. attribute:: errorname

      (str) Name of the error

      Name corresponding to predefined error code, or a user defined
      error type

   .. attribute:: message

      (str) Message to pass to the user

      Human readable message to pass to the user

   .. attribute:: errorsubname

      (str) An optional sub_name to further categorize the exception

      The sub_name should follow the Robot Raconteur naming rules. It should
      either be a single name or a fully qualified name using service definition
      naming rules

   .. attribute:: errorparam

      (Any) An optional parameter

      Use this placeholder if data needs to be passed with the exception



.. class:: RobotRaconteur.MemberNotFoundException(message=,subname=None,param_=None)

   Exception thrown when the specified object member is
   not found

   Service objects have member that are declared in a service definition.
   If an attempt is made to call a member that does not exist, this exception
   is thrown.

   Error code MessageErrorType_MemberNotFound (9)


   Construct a MemberNotFoundException


   :param message: A message to pass to the user
   :type message: str
   :param subname: An optional sub_name to further categorize the exception
   :type subname: str
   :param param_: An optional exception parameter
   :type param|: Any

   .. attribute:: errorcode

      (int) Predefined error code

      Error code that specifies the type of error See MessageErrorType constants.

   .. attribute:: errorname

      (str) Name of the error

      Name corresponding to predefined error code, or a user defined
      error type

   .. attribute:: message

      (str) Message to pass to the user

      Human readable message to pass to the user

   .. attribute:: errorsubname

      (str) An optional sub_name to further categorize the exception

      The sub_name should follow the Robot Raconteur naming rules. It should
      either be a single name or a fully qualified name using service definition
      naming rules

   .. attribute:: errorparam

      (Any) An optional parameter

      Use this placeholder if data needs to be passed with the exception



.. class:: RobotRaconteur.MemberFormatMismatchException(message=,subname=None,param_=None)

   Exception thrown when a request to a member has an
   invalid MessageEntryType or the wrong message elements

   Error code MessageErrorType_MemberFormatMismatch (10)


   Construct a MemberFormatMismatchException


   :param message: A message to pass to the user
   :type message: str
   :param subname: An optional sub_name to further categorize the exception
   :type subname: str
   :param param_: An optional exception parameter
   :type param|: Any

   .. attribute:: errorcode

      (int) Predefined error code

      Error code that specifies the type of error See MessageErrorType constants.

   .. attribute:: errorname

      (str) Name of the error

      Name corresponding to predefined error code, or a user defined
      error type

   .. attribute:: message

      (str) Message to pass to the user

      Human readable message to pass to the user

   .. attribute:: errorsubname

      (str) An optional sub_name to further categorize the exception

      The sub_name should follow the Robot Raconteur naming rules. It should
      either be a single name or a fully qualified name using service definition
      naming rules

   .. attribute:: errorparam

      (Any) An optional parameter

      Use this placeholder if data needs to be passed with the exception



.. class:: RobotRaconteur.DataTypeMismatchException(message=,subname=None,param_=None)

   Exception thrown when incorrect data is received
   by a member

   Make sure the provided data matches the expected data types

   Error code MessageErrorType_DataTypeMismatch (11)


   Construct a DataTypeMismatchException


   :param message: A message to pass to the user
   :type message: str
   :param subname: An optional sub_name to further categorize the exception
   :type subname: str
   :param param_: An optional exception parameter
   :type param|: Any

   .. attribute:: errorcode

      (int) Predefined error code

      Error code that specifies the type of error See MessageErrorType constants.

   .. attribute:: errorname

      (str) Name of the error

      Name corresponding to predefined error code, or a user defined
      error type

   .. attribute:: message

      (str) Message to pass to the user

      Human readable message to pass to the user

   .. attribute:: errorsubname

      (str) An optional sub_name to further categorize the exception

      The sub_name should follow the Robot Raconteur naming rules. It should
      either be a single name or a fully qualified name using service definition
      naming rules

   .. attribute:: errorparam

      (Any) An optional parameter

      Use this placeholder if data needs to be passed with the exception



.. class:: RobotRaconteur.DataTypeException(message=,subname=None,param_=None)

   Exception thrown when unexpected or incompatible
   data is provided

   DataTypeException is sometimes thrown when there is a
   type mismatch instead of DataTypeMismatchException

   Make sure the provided data matches the expected data types

   Error code MessageErrorType_DataTypeError (12)


   Construct a DataTypeException


   :param message: A message to pass to the user
   :type message: str
   :param subname: An optional sub_name to further categorize the exception
   :type subname: str
   :param param_: An optional exception parameter
   :type param|: Any

   .. attribute:: errorcode

      (int) Predefined error code

      Error code that specifies the type of error See MessageErrorType constants.

   .. attribute:: errorname

      (str) Name of the error

      Name corresponding to predefined error code, or a user defined
      error type

   .. attribute:: message

      (str) Message to pass to the user

      Human readable message to pass to the user

   .. attribute:: errorsubname

      (str) An optional sub_name to further categorize the exception

      The sub_name should follow the Robot Raconteur naming rules. It should
      either be a single name or a fully qualified name using service definition
      naming rules

   .. attribute:: errorparam

      (Any) An optional parameter

      Use this placeholder if data needs to be passed with the exception



.. class:: RobotRaconteur.DataSerializationException(message=,subname=None,param_=None)

   Exception thrown when data cannot be serialized

   This exeception is thrown when the provide data cannot be serialized.
   This typically occurs inside a transport.

   Check that the provided data matches the types supported by
   the Robot Raconteur C++ library

   Error code MessageErrorType_DataSerializationError (13)


   Construct a DataSerializationException


   :param message: A message to pass to the user
   :type message: str
   :param subname: An optional sub_name to further categorize the exception
   :type subname: str
   :param param_: An optional exception parameter
   :type param|: Any

   .. attribute:: errorcode

      (int) Predefined error code

      Error code that specifies the type of error See MessageErrorType constants.

   .. attribute:: errorname

      (str) Name of the error

      Name corresponding to predefined error code, or a user defined
      error type

   .. attribute:: message

      (str) Message to pass to the user

      Human readable message to pass to the user

   .. attribute:: errorsubname

      (str) An optional sub_name to further categorize the exception

      The sub_name should follow the Robot Raconteur naming rules. It should
      either be a single name or a fully qualified name using service definition
      naming rules

   .. attribute:: errorparam

      (Any) An optional parameter

      Use this placeholder if data needs to be passed with the exception



.. class:: RobotRaconteur.MessageEntryNotFoundException(message=,subname=None,param_=None)

   Exception thrown when an expected MessageEntry
   is not found

   Error code MessageErrorType_MessageEntryNotfound (14)


   Construct a MessageEntryNotFoundException


   :param message: A message to pass to the user
   :type message: str
   :param subname: An optional sub_name to further categorize the exception
   :type subname: str
   :param param_: An optional exception parameter
   :type param|: Any

   .. attribute:: errorcode

      (int) Predefined error code

      Error code that specifies the type of error See MessageErrorType constants.

   .. attribute:: errorname

      (str) Name of the error

      Name corresponding to predefined error code, or a user defined
      error type

   .. attribute:: message

      (str) Message to pass to the user

      Human readable message to pass to the user

   .. attribute:: errorsubname

      (str) An optional sub_name to further categorize the exception

      The sub_name should follow the Robot Raconteur naming rules. It should
      either be a single name or a fully qualified name using service definition
      naming rules

   .. attribute:: errorparam

      (Any) An optional parameter

      Use this placeholder if data needs to be passed with the exception



.. class:: RobotRaconteur.MessageElementNotFoundException(message=,subname=None,param_=None)

   Exception thrown wen an expected MessageElement
   is not found

   This exception is thrown when an expected field or parameter
   is not found.

   Error code MessageErrorType_MessageElementNotfound (15)


   Construct a MessageElementNotFoundException


   :param message: A message to pass to the user
   :type message: str
   :param subname: An optional sub_name to further categorize the exception
   :type subname: str
   :param param_: An optional exception parameter
   :type param|: Any

   .. attribute:: errorcode

      (int) Predefined error code

      Error code that specifies the type of error See MessageErrorType constants.

   .. attribute:: errorname

      (str) Name of the error

      Name corresponding to predefined error code, or a user defined
      error type

   .. attribute:: message

      (str) Message to pass to the user

      Human readable message to pass to the user

   .. attribute:: errorsubname

      (str) An optional sub_name to further categorize the exception

      The sub_name should follow the Robot Raconteur naming rules. It should
      either be a single name or a fully qualified name using service definition
      naming rules

   .. attribute:: errorparam

      (Any) An optional parameter

      Use this placeholder if data needs to be passed with the exception



.. class:: RobotRaconteur.UnknownException(message=,subname=None,param_=None)

   Exception representing an unknown exception type

   This exception is used to transmit exceptions that do not have a
   MessageErrorType code. Check the Error field for the name
   of the exception.

   Error code MessageErrorType_UnknownError (16)


   Construct a UnknownException


   :param message: A message to pass to the user
   :type message: str
   :param subname: An optional sub_name to further categorize the exception
   :type subname: str
   :param param_: An optional exception parameter
   :type param|: Any

   .. attribute:: errorcode

      (int) Predefined error code

      Error code that specifies the type of error See MessageErrorType constants.

   .. attribute:: errorname

      (str) Name of the error

      Name corresponding to predefined error code, or a user defined
      error type

   .. attribute:: message

      (str) Message to pass to the user

      Human readable message to pass to the user

   .. attribute:: errorsubname

      (str) An optional sub_name to further categorize the exception

      The sub_name should follow the Robot Raconteur naming rules. It should
      either be a single name or a fully qualified name using service definition
      naming rules

   .. attribute:: errorparam

      (Any) An optional parameter

      Use this placeholder if data needs to be passed with the exception



.. class:: RobotRaconteur.InvalidOperationException(message=,subname=None,param_=None)

   Exception thrown when an invalid operation is attempted

   Error code MessageErrorType_InvalidOperation (17)


   Construct a InvalidOperationException


   :param message: A message to pass to the user
   :type message: str
   :param subname: An optional sub_name to further categorize the exception
   :type subname: str
   :param param_: An optional exception parameter
   :type param|: Any

   .. attribute:: errorcode

      (int) Predefined error code

      Error code that specifies the type of error See MessageErrorType constants.

   .. attribute:: errorname

      (str) Name of the error

      Name corresponding to predefined error code, or a user defined
      error type

   .. attribute:: message

      (str) Message to pass to the user

      Human readable message to pass to the user

   .. attribute:: errorsubname

      (str) An optional sub_name to further categorize the exception

      The sub_name should follow the Robot Raconteur naming rules. It should
      either be a single name or a fully qualified name using service definition
      naming rules

   .. attribute:: errorparam

      (Any) An optional parameter

      Use this placeholder if data needs to be passed with the exception



.. class:: RobotRaconteur.InvalidArgumentException(message=,subname=None,param_=None)

   Exception thrown for an invalid argument

   Error code MessageErrorType_InvalidArgument (18)


   Construct a InvalidArgumentException


   :param message: A message to pass to the user
   :type message: str
   :param subname: An optional sub_name to further categorize the exception
   :type subname: str
   :param param_: An optional exception parameter
   :type param|: Any

   .. attribute:: errorcode

      (int) Predefined error code

      Error code that specifies the type of error See MessageErrorType constants.

   .. attribute:: errorname

      (str) Name of the error

      Name corresponding to predefined error code, or a user defined
      error type

   .. attribute:: message

      (str) Message to pass to the user

      Human readable message to pass to the user

   .. attribute:: errorsubname

      (str) An optional sub_name to further categorize the exception

      The sub_name should follow the Robot Raconteur naming rules. It should
      either be a single name or a fully qualified name using service definition
      naming rules

   .. attribute:: errorparam

      (Any) An optional parameter

      Use this placeholder if data needs to be passed with the exception



.. class:: RobotRaconteur.OperationFailedException(message=,subname=None,param_=None)

   Exception thrown when an operation fails

   Error code MessageErrorType_OperationFailed (19)


   Construct a OperationFailedException


   :param message: A message to pass to the user
   :type message: str
   :param subname: An optional sub_name to further categorize the exception
   :type subname: str
   :param param_: An optional exception parameter
   :type param|: Any

   .. attribute:: errorcode

      (int) Predefined error code

      Error code that specifies the type of error See MessageErrorType constants.

   .. attribute:: errorname

      (str) Name of the error

      Name corresponding to predefined error code, or a user defined
      error type

   .. attribute:: message

      (str) Message to pass to the user

      Human readable message to pass to the user

   .. attribute:: errorsubname

      (str) An optional sub_name to further categorize the exception

      The sub_name should follow the Robot Raconteur naming rules. It should
      either be a single name or a fully qualified name using service definition
      naming rules

   .. attribute:: errorparam

      (Any) An optional parameter

      Use this placeholder if data needs to be passed with the exception



.. class:: RobotRaconteur.NullValueException(message=,subname=None,param_=None)

   Exception thrown for an unexpected null value

   Error code MessageErrorType_NullValue (20)


   Construct a NullValueException


   :param message: A message to pass to the user
   :type message: str
   :param subname: An optional sub_name to further categorize the exception
   :type subname: str
   :param param_: An optional exception parameter
   :type param|: Any

   .. attribute:: errorcode

      (int) Predefined error code

      Error code that specifies the type of error See MessageErrorType constants.

   .. attribute:: errorname

      (str) Name of the error

      Name corresponding to predefined error code, or a user defined
      error type

   .. attribute:: message

      (str) Message to pass to the user

      Human readable message to pass to the user

   .. attribute:: errorsubname

      (str) An optional sub_name to further categorize the exception

      The sub_name should follow the Robot Raconteur naming rules. It should
      either be a single name or a fully qualified name using service definition
      naming rules

   .. attribute:: errorparam

      (Any) An optional parameter

      Use this placeholder if data needs to be passed with the exception



.. class:: RobotRaconteur.InternalErrorException(message=,subname=None,param_=None)

   Exception thrown when an internal error has occurred

   Error code MessageErrorType_InternalError (21)


   Construct a InternalErrorException


   :param message: A message to pass to the user
   :type message: str
   :param subname: An optional sub_name to further categorize the exception
   :type subname: str
   :param param_: An optional exception parameter
   :type param|: Any

   .. attribute:: errorcode

      (int) Predefined error code

      Error code that specifies the type of error See MessageErrorType constants.

   .. attribute:: errorname

      (str) Name of the error

      Name corresponding to predefined error code, or a user defined
      error type

   .. attribute:: message

      (str) Message to pass to the user

      Human readable message to pass to the user

   .. attribute:: errorsubname

      (str) An optional sub_name to further categorize the exception

      The sub_name should follow the Robot Raconteur naming rules. It should
      either be a single name or a fully qualified name using service definition
      naming rules

   .. attribute:: errorparam

      (Any) An optional parameter

      Use this placeholder if data needs to be passed with the exception



.. class:: RobotRaconteur.PermissionDeniedException(message=,subname=None,param_=None)

   Exception thrown when permission is denied to a service member

   Error code MessageErrorType_PermissionDenied (152)


   Construct a PermissionDeniedException


   :param message: A message to pass to the user
   :type message: str
   :param subname: An optional sub_name to further categorize the exception
   :type subname: str
   :param param_: An optional exception parameter
   :type param|: Any

   .. attribute:: errorcode

      (int) Predefined error code

      Error code that specifies the type of error See MessageErrorType constants.

   .. attribute:: errorname

      (str) Name of the error

      Name corresponding to predefined error code, or a user defined
      error type

   .. attribute:: message

      (str) Message to pass to the user

      Human readable message to pass to the user

   .. attribute:: errorsubname

      (str) An optional sub_name to further categorize the exception

      The sub_name should follow the Robot Raconteur naming rules. It should
      either be a single name or a fully qualified name using service definition
      naming rules

   .. attribute:: errorparam

      (Any) An optional parameter

      Use this placeholder if data needs to be passed with the exception



.. class:: RobotRaconteur.SystemResourcePermissionDeniedException(message=,subname=None,param_=None)

   Exception thrown when permission to a system resource is denied

   Error code MessageErrorType_SystemResourcePermissionDenied (22)


   Construct a SystemResourcePermissionDeniedException


   :param message: A message to pass to the user
   :type message: str
   :param subname: An optional sub_name to further categorize the exception
   :type subname: str
   :param param_: An optional exception parameter
   :type param|: Any

   .. attribute:: errorcode

      (int) Predefined error code

      Error code that specifies the type of error See MessageErrorType constants.

   .. attribute:: errorname

      (str) Name of the error

      Name corresponding to predefined error code, or a user defined
      error type

   .. attribute:: message

      (str) Message to pass to the user

      Human readable message to pass to the user

   .. attribute:: errorsubname

      (str) An optional sub_name to further categorize the exception

      The sub_name should follow the Robot Raconteur naming rules. It should
      either be a single name or a fully qualified name using service definition
      naming rules

   .. attribute:: errorparam

      (Any) An optional parameter

      Use this placeholder if data needs to be passed with the exception



.. class:: RobotRaconteur.OutOfSystemResourceException(message=,subname=None,param_=None)

   Exception thrown when a system resource has been exhausted

   Error code MessageErrorType_OutOfsystemResource (23)


   Construct a OutOfSystemResourceException


   :param message: A message to pass to the user
   :type message: str
   :param subname: An optional sub_name to further categorize the exception
   :type subname: str
   :param param_: An optional exception parameter
   :type param|: Any

   .. attribute:: errorcode

      (int) Predefined error code

      Error code that specifies the type of error See MessageErrorType constants.

   .. attribute:: errorname

      (str) Name of the error

      Name corresponding to predefined error code, or a user defined
      error type

   .. attribute:: message

      (str) Message to pass to the user

      Human readable message to pass to the user

   .. attribute:: errorsubname

      (str) An optional sub_name to further categorize the exception

      The sub_name should follow the Robot Raconteur naming rules. It should
      either be a single name or a fully qualified name using service definition
      naming rules

   .. attribute:: errorparam

      (Any) An optional parameter

      Use this placeholder if data needs to be passed with the exception



.. class:: RobotRaconteur.SystemResourceException(message=,subname=None,param_=None)

   Exception thrown when a system resource error occurs

   Error code MessageErrorType_SystemResourceException (24)


   Construct a SystemResourceException


   :param message: A message to pass to the user
   :type message: str
   :param subname: An optional sub_name to further categorize the exception
   :type subname: str
   :param param_: An optional exception parameter
   :type param|: Any

   .. attribute:: errorcode

      (int) Predefined error code

      Error code that specifies the type of error See MessageErrorType constants.

   .. attribute:: errorname

      (str) Name of the error

      Name corresponding to predefined error code, or a user defined
      error type

   .. attribute:: message

      (str) Message to pass to the user

      Human readable message to pass to the user

   .. attribute:: errorsubname

      (str) An optional sub_name to further categorize the exception

      The sub_name should follow the Robot Raconteur naming rules. It should
      either be a single name or a fully qualified name using service definition
      naming rules

   .. attribute:: errorparam

      (Any) An optional parameter

      Use this placeholder if data needs to be passed with the exception



.. class:: RobotRaconteur.ResourceNotFoundException(message=,subname=None,param_=None)

   Exception thrown when a system resource is not found

   Error code MessageErrorType_ResourceNotFound (25)


   Construct a ResourceNotFoundException


   :param message: A message to pass to the user
   :type message: str
   :param subname: An optional sub_name to further categorize the exception
   :type subname: str
   :param param_: An optional exception parameter
   :type param|: Any

   .. attribute:: errorcode

      (int) Predefined error code

      Error code that specifies the type of error See MessageErrorType constants.

   .. attribute:: errorname

      (str) Name of the error

      Name corresponding to predefined error code, or a user defined
      error type

   .. attribute:: message

      (str) Message to pass to the user

      Human readable message to pass to the user

   .. attribute:: errorsubname

      (str) An optional sub_name to further categorize the exception

      The sub_name should follow the Robot Raconteur naming rules. It should
      either be a single name or a fully qualified name using service definition
      naming rules

   .. attribute:: errorparam

      (Any) An optional parameter

      Use this placeholder if data needs to be passed with the exception



.. class:: RobotRaconteur.IOException(message=,subname=None,param_=None)

   Exception thrown when an input/output error occurs

   Error code MessageErrorType_IOError (26)


   Construct a IOException


   :param message: A message to pass to the user
   :type message: str
   :param subname: An optional sub_name to further categorize the exception
   :type subname: str
   :param param_: An optional exception parameter
   :type param|: Any

   .. attribute:: errorcode

      (int) Predefined error code

      Error code that specifies the type of error See MessageErrorType constants.

   .. attribute:: errorname

      (str) Name of the error

      Name corresponding to predefined error code, or a user defined
      error type

   .. attribute:: message

      (str) Message to pass to the user

      Human readable message to pass to the user

   .. attribute:: errorsubname

      (str) An optional sub_name to further categorize the exception

      The sub_name should follow the Robot Raconteur naming rules. It should
      either be a single name or a fully qualified name using service definition
      naming rules

   .. attribute:: errorparam

      (Any) An optional parameter

      Use this placeholder if data needs to be passed with the exception



.. class:: RobotRaconteur.BufferLimitViolationException(message=,subname=None,param_=None)

   Exception thrown when a transport buffer limit is violated

   This exception typically occurs if there is a bug in
   serialization/deserialization, or the data stream
   has been corrupted

   Error code MessageErrorType_BufferLimitViolation (27)


   Construct a BufferLimitViolationException


   :param message: A message to pass to the user
   :type message: str
   :param subname: An optional sub_name to further categorize the exception
   :type subname: str
   :param param_: An optional exception parameter
   :type param|: Any

   .. attribute:: errorcode

      (int) Predefined error code

      Error code that specifies the type of error See MessageErrorType constants.

   .. attribute:: errorname

      (str) Name of the error

      Name corresponding to predefined error code, or a user defined
      error type

   .. attribute:: message

      (str) Message to pass to the user

      Human readable message to pass to the user

   .. attribute:: errorsubname

      (str) An optional sub_name to further categorize the exception

      The sub_name should follow the Robot Raconteur naming rules. It should
      either be a single name or a fully qualified name using service definition
      naming rules

   .. attribute:: errorparam

      (Any) An optional parameter

      Use this placeholder if data needs to be passed with the exception



.. class:: RobotRaconteur.ServiceDefinitionException(message=,subname=None,param_=None)

   Exception thrown when a service definition cannot be
   parsed or fails verification

   Error code MessageErrorType_ServiceDefinitionError (28)


   Construct a ServiceDefinitionException


   :param message: A message to pass to the user
   :type message: str
   :param subname: An optional sub_name to further categorize the exception
   :type subname: str
   :param param_: An optional exception parameter
   :type param|: Any

   .. attribute:: errorcode

      (int) Predefined error code

      Error code that specifies the type of error See MessageErrorType constants.

   .. attribute:: errorname

      (str) Name of the error

      Name corresponding to predefined error code, or a user defined
      error type

   .. attribute:: message

      (str) Message to pass to the user

      Human readable message to pass to the user

   .. attribute:: errorsubname

      (str) An optional sub_name to further categorize the exception

      The sub_name should follow the Robot Raconteur naming rules. It should
      either be a single name or a fully qualified name using service definition
      naming rules

   .. attribute:: errorparam

      (Any) An optional parameter

      Use this placeholder if data needs to be passed with the exception



.. class:: RobotRaconteur.OutOfRangeException(message=,subname=None,param_=None)

   Exception thrown when an attempt to acces an array or container
   index is out of range

   Error code MessageErrorType_OutOfRange (29)


   Construct a OutOfRangeException


   :param message: A message to pass to the user
   :type message: str
   :param subname: An optional sub_name to further categorize the exception
   :type subname: str
   :param param_: An optional exception parameter
   :type param|: Any

   .. attribute:: errorcode

      (int) Predefined error code

      Error code that specifies the type of error See MessageErrorType constants.

   .. attribute:: errorname

      (str) Name of the error

      Name corresponding to predefined error code, or a user defined
      error type

   .. attribute:: message

      (str) Message to pass to the user

      Human readable message to pass to the user

   .. attribute:: errorsubname

      (str) An optional sub_name to further categorize the exception

      The sub_name should follow the Robot Raconteur naming rules. It should
      either be a single name or a fully qualified name using service definition
      naming rules

   .. attribute:: errorparam

      (Any) An optional parameter

      Use this placeholder if data needs to be passed with the exception



.. class:: RobotRaconteur.KeyNotFoundException(message=,subname=None,param_=None)

   Exception thrown when a key is not found in a map

   Error code MessageErrorType_KeyNotFound (30)


   Construct a KeyNotFoundException


   :param message: A message to pass to the user
   :type message: str
   :param subname: An optional sub_name to further categorize the exception
   :type subname: str
   :param param_: An optional exception parameter
   :type param|: Any

   .. attribute:: errorcode

      (int) Predefined error code

      Error code that specifies the type of error See MessageErrorType constants.

   .. attribute:: errorname

      (str) Name of the error

      Name corresponding to predefined error code, or a user defined
      error type

   .. attribute:: message

      (str) Message to pass to the user

      Human readable message to pass to the user

   .. attribute:: errorsubname

      (str) An optional sub_name to further categorize the exception

      The sub_name should follow the Robot Raconteur naming rules. It should
      either be a single name or a fully qualified name using service definition
      naming rules

   .. attribute:: errorparam

      (Any) An optional parameter

      Use this placeholder if data needs to be passed with the exception



.. class:: RobotRaconteur.InvalidConfigurationException(message=,subname=None,param_=None)

   Exception thrown when an invalid configuration is specified or encountered

   Error code MessageErrorType_InvalidConfiguration (31)


   Construct a InvalidConfigurationException


   :param message: A message to pass to the user
   :type message: str
   :param subname: An optional sub_name to further categorize the exception
   :type subname: str
   :param param_: An optional exception parameter
   :type param|: Any

   .. attribute:: errorcode

      (int) Predefined error code

      Error code that specifies the type of error See MessageErrorType constants.

   .. attribute:: errorname

      (str) Name of the error

      Name corresponding to predefined error code, or a user defined
      error type

   .. attribute:: message

      (str) Message to pass to the user

      Human readable message to pass to the user

   .. attribute:: errorsubname

      (str) An optional sub_name to further categorize the exception

      The sub_name should follow the Robot Raconteur naming rules. It should
      either be a single name or a fully qualified name using service definition
      naming rules

   .. attribute:: errorparam

      (Any) An optional parameter

      Use this placeholder if data needs to be passed with the exception



.. class:: RobotRaconteur.InvalidStateException(message=,subname=None,param_=None)

   Exception thrown when an invalid state is specified or encountered

   Error code MessageErrorType_InvalidState (32)


   Construct a InvalidStateException


   :param message: A message to pass to the user
   :type message: str
   :param subname: An optional sub_name to further categorize the exception
   :type subname: str
   :param param_: An optional exception parameter
   :type param|: Any

   .. attribute:: errorcode

      (int) Predefined error code

      Error code that specifies the type of error See MessageErrorType constants.

   .. attribute:: errorname

      (str) Name of the error

      Name corresponding to predefined error code, or a user defined
      error type

   .. attribute:: message

      (str) Message to pass to the user

      Human readable message to pass to the user

   .. attribute:: errorsubname

      (str) An optional sub_name to further categorize the exception

      The sub_name should follow the Robot Raconteur naming rules. It should
      either be a single name or a fully qualified name using service definition
      naming rules

   .. attribute:: errorparam

      (Any) An optional parameter

      Use this placeholder if data needs to be passed with the exception



.. class:: RobotRaconteur.RobotRaconteurRemoteException(message=,subname=None,param_=None)

   Exception thrown when an error occurs on a remote
   member request

   User defined exceptions declared in service definitions extend
   RobotRaconteurRemoteException

   Error code MessageErrorType_RemoteError (100)


   Construct a RobotRaconteurRemoteException


   :param message: A message to pass to the user
   :type message: str
   :param subname: An optional sub_name to further categorize the exception
   :type subname: str
   :param param_: An optional exception parameter
   :type param|: Any

   .. attribute:: errorcode

      (int) Predefined error code

      Error code that specifies the type of error See MessageErrorType constants.

   .. attribute:: errorname

      (str) Name of the error

      Name corresponding to predefined error code, or a user defined
      error type

   .. attribute:: message

      (str) Message to pass to the user

      Human readable message to pass to the user

   .. attribute:: errorsubname

      (str) An optional sub_name to further categorize the exception

      The sub_name should follow the Robot Raconteur naming rules. It should
      either be a single name or a fully qualified name using service definition
      naming rules

   .. attribute:: errorparam

      (Any) An optional parameter

      Use this placeholder if data needs to be passed with the exception



.. class:: RobotRaconteur.RequestTimeoutException(message=,subname=None,param_=None)

   The request timed out

   See RobotRaconteurNode::SetRequestTimeout(),
   or the timeout passed to an asynchronous request

   Error code MessageErrorType_RequestTimeout (101)


   Construct a RequestTimeoutException


   :param message: A message to pass to the user
   :type message: str
   :param subname: An optional sub_name to further categorize the exception
   :type subname: str
   :param param_: An optional exception parameter
   :type param|: Any

   .. attribute:: errorcode

      (int) Predefined error code

      Error code that specifies the type of error See MessageErrorType constants.

   .. attribute:: errorname

      (str) Name of the error

      Name corresponding to predefined error code, or a user defined
      error type

   .. attribute:: message

      (str) Message to pass to the user

      Human readable message to pass to the user

   .. attribute:: errorsubname

      (str) An optional sub_name to further categorize the exception

      The sub_name should follow the Robot Raconteur naming rules. It should
      either be a single name or a fully qualified name using service definition
      naming rules

   .. attribute:: errorparam

      (Any) An optional parameter

      Use this placeholder if data needs to be passed with the exception



.. class:: RobotRaconteur.ReadOnlyMemberException(message=,subname=None,param_=None)

   An attempt was made to write/set a read only member

   Error code MessageErrorType_ReadOnlyMember (102)


   Construct a ReadOnlyMemberException


   :param message: A message to pass to the user
   :type message: str
   :param subname: An optional sub_name to further categorize the exception
   :type subname: str
   :param param_: An optional exception parameter
   :type param|: Any

   .. attribute:: errorcode

      (int) Predefined error code

      Error code that specifies the type of error See MessageErrorType constants.

   .. attribute:: errorname

      (str) Name of the error

      Name corresponding to predefined error code, or a user defined
      error type

   .. attribute:: message

      (str) Message to pass to the user

      Human readable message to pass to the user

   .. attribute:: errorsubname

      (str) An optional sub_name to further categorize the exception

      The sub_name should follow the Robot Raconteur naming rules. It should
      either be a single name or a fully qualified name using service definition
      naming rules

   .. attribute:: errorparam

      (Any) An optional parameter

      Use this placeholder if data needs to be passed with the exception



.. class:: RobotRaconteur.WriteOnlyMemberException(message=,subname=None,param_=None)

   An attempt was mode to read/get a write only member

   Error code MessageErrorType_WriteOnlyMember (103)


   Construct a WriteOnlyMemberException


   :param message: A message to pass to the user
   :type message: str
   :param subname: An optional sub_name to further categorize the exception
   :type subname: str
   :param param_: An optional exception parameter
   :type param|: Any

   .. attribute:: errorcode

      (int) Predefined error code

      Error code that specifies the type of error See MessageErrorType constants.

   .. attribute:: errorname

      (str) Name of the error

      Name corresponding to predefined error code, or a user defined
      error type

   .. attribute:: message

      (str) Message to pass to the user

      Human readable message to pass to the user

   .. attribute:: errorsubname

      (str) An optional sub_name to further categorize the exception

      The sub_name should follow the Robot Raconteur naming rules. It should
      either be a single name or a fully qualified name using service definition
      naming rules

   .. attribute:: errorparam

      (Any) An optional parameter

      Use this placeholder if data needs to be passed with the exception



.. class:: RobotRaconteur.NotImplementedException(message=,subname=None,param_=None)

   Exception thrown if a member is not implemented

   Error code MessageErrorType_NotImplementedError (104)


   Construct a NotImplementedException


   :param message: A message to pass to the user
   :type message: str
   :param subname: An optional sub_name to further categorize the exception
   :type subname: str
   :param param_: An optional exception parameter
   :type param|: Any

   .. attribute:: errorcode

      (int) Predefined error code

      Error code that specifies the type of error See MessageErrorType constants.

   .. attribute:: errorname

      (str) Name of the error

      Name corresponding to predefined error code, or a user defined
      error type

   .. attribute:: message

      (str) Message to pass to the user

      Human readable message to pass to the user

   .. attribute:: errorsubname

      (str) An optional sub_name to further categorize the exception

      The sub_name should follow the Robot Raconteur naming rules. It should
      either be a single name or a fully qualified name using service definition
      naming rules

   .. attribute:: errorparam

      (Any) An optional parameter

      Use this placeholder if data needs to be passed with the exception



.. class:: RobotRaconteur.MemberBusyException(message=,subname=None,param_=None)

   Thrown is a member is busy. Retry later

   Error code MessageErrorType_MemberBusy (105)


   Construct a MemberBusyException


   :param message: A message to pass to the user
   :type message: str
   :param subname: An optional sub_name to further categorize the exception
   :type subname: str
   :param param_: An optional exception parameter
   :type param|: Any

   .. attribute:: errorcode

      (int) Predefined error code

      Error code that specifies the type of error See MessageErrorType constants.

   .. attribute:: errorname

      (str) Name of the error

      Name corresponding to predefined error code, or a user defined
      error type

   .. attribute:: message

      (str) Message to pass to the user

      Human readable message to pass to the user

   .. attribute:: errorsubname

      (str) An optional sub_name to further categorize the exception

      The sub_name should follow the Robot Raconteur naming rules. It should
      either be a single name or a fully qualified name using service definition
      naming rules

   .. attribute:: errorparam

      (Any) An optional parameter

      Use this placeholder if data needs to be passed with the exception



.. class:: RobotRaconteur.ValueNotSetException(message=,subname=None,param_=None)

   Exception thrown if a value has not been set

   This exception is most often used by WireConnection::GetInValue()
   and WireConnection::GetOutValue() if InValue or OutValue
   have not been received or set

   Error code MessageErrorType_ValueNotSet (106)


   Construct a ValueNotSetException


   :param message: A message to pass to the user
   :type message: str
   :param subname: An optional sub_name to further categorize the exception
   :type subname: str
   :param param_: An optional exception parameter
   :type param|: Any

   .. attribute:: errorcode

      (int) Predefined error code

      Error code that specifies the type of error See MessageErrorType constants.

   .. attribute:: errorname

      (str) Name of the error

      Name corresponding to predefined error code, or a user defined
      error type

   .. attribute:: message

      (str) Message to pass to the user

      Human readable message to pass to the user

   .. attribute:: errorsubname

      (str) An optional sub_name to further categorize the exception

      The sub_name should follow the Robot Raconteur naming rules. It should
      either be a single name or a fully qualified name using service definition
      naming rules

   .. attribute:: errorparam

      (Any) An optional parameter

      Use this placeholder if data needs to be passed with the exception



.. class:: RobotRaconteur.AuthenticationException(message=,subname=None,param_=None)

   Exception thrown when authentication is required or attempt
   to authenticate fails

   Error code MessageErrorType_AuthenticationError (150)


   Construct a AuthenticationException


   :param message: A message to pass to the user
   :type message: str
   :param subname: An optional sub_name to further categorize the exception
   :type subname: str
   :param param_: An optional exception parameter
   :type param|: Any

   .. attribute:: errorcode

      (int) Predefined error code

      Error code that specifies the type of error See MessageErrorType constants.

   .. attribute:: errorname

      (str) Name of the error

      Name corresponding to predefined error code, or a user defined
      error type

   .. attribute:: message

      (str) Message to pass to the user

      Human readable message to pass to the user

   .. attribute:: errorsubname

      (str) An optional sub_name to further categorize the exception

      The sub_name should follow the Robot Raconteur naming rules. It should
      either be a single name or a fully qualified name using service definition
      naming rules

   .. attribute:: errorparam

      (Any) An optional parameter

      Use this placeholder if data needs to be passed with the exception



.. class:: RobotRaconteur.ObjectLockedException(message=,subname=None,param_=None)

   Exception thrown when attempting to access a locked
   service object

   Service objects can be locked using RobotRaconteurNode::RequestObjectLock().
   This exception is thrown if an attempt is made to access a service object
   (or sub-object) that has been locked by another user or session.

   Error code MessageErrorType_ObjectLockedError (151)


   Construct a ObjectLockedException


   :param message: A message to pass to the user
   :type message: str
   :param subname: An optional sub_name to further categorize the exception
   :type subname: str
   :param param_: An optional exception parameter
   :type param|: Any

   .. attribute:: errorcode

      (int) Predefined error code

      Error code that specifies the type of error See MessageErrorType constants.

   .. attribute:: errorname

      (str) Name of the error

      Name corresponding to predefined error code, or a user defined
      error type

   .. attribute:: message

      (str) Message to pass to the user

      Human readable message to pass to the user

   .. attribute:: errorsubname

      (str) An optional sub_name to further categorize the exception

      The sub_name should follow the Robot Raconteur naming rules. It should
      either be a single name or a fully qualified name using service definition
      naming rules

   .. attribute:: errorparam

      (Any) An optional parameter

      Use this placeholder if data needs to be passed with the exception



.. class:: RobotRaconteur.AbortOperationException(message=,subname=None,param_=None)

   Exception passed to generators to trigger an abort

   This is typically not thrown or received by the user

   Error code MessageErrorType_AbortOperation (107)


   Construct a AbortOperationException


   :param message: A message to pass to the user
   :type message: str
   :param subname: An optional sub_name to further categorize the exception
   :type subname: str
   :param param_: An optional exception parameter
   :type param|: Any

   .. attribute:: errorcode

      (int) Predefined error code

      Error code that specifies the type of error See MessageErrorType constants.

   .. attribute:: errorname

      (str) Name of the error

      Name corresponding to predefined error code, or a user defined
      error type

   .. attribute:: message

      (str) Message to pass to the user

      Human readable message to pass to the user

   .. attribute:: errorsubname

      (str) An optional sub_name to further categorize the exception

      The sub_name should follow the Robot Raconteur naming rules. It should
      either be a single name or a fully qualified name using service definition
      naming rules

   .. attribute:: errorparam

      (Any) An optional parameter

      Use this placeholder if data needs to be passed with the exception



.. class:: RobotRaconteur.OperationAbortedException(message=,subname=None,param_=None)

   Exception thrown when an operation is aborted

   This is thrown be generator functions when Abort()
   is called

   Generators are destroyed after throwing
   OperationAbortedException during Next()

   This error is passed to generators to trigger an abort

   Error code MessageErrorType_OperationAborted (108)


   Construct a OperationAbortedException


   :param message: A message to pass to the user
   :type message: str
   :param subname: An optional sub_name to further categorize the exception
   :type subname: str
   :param param_: An optional exception parameter
   :type param|: Any

   .. attribute:: errorcode

      (int) Predefined error code

      Error code that specifies the type of error See MessageErrorType constants.

   .. attribute:: errorname

      (str) Name of the error

      Name corresponding to predefined error code, or a user defined
      error type

   .. attribute:: message

      (str) Message to pass to the user

      Human readable message to pass to the user

   .. attribute:: errorsubname

      (str) An optional sub_name to further categorize the exception

      The sub_name should follow the Robot Raconteur naming rules. It should
      either be a single name or a fully qualified name using service definition
      naming rules

   .. attribute:: errorparam

      (Any) An optional parameter

      Use this placeholder if data needs to be passed with the exception



.. class:: RobotRaconteur.StopIterationException(message=,subname=None,param_=None)

   Exception thrown when a generator has finished sending results

   StopIterationException is not an error condition. It signals
   that a generator is finished sending results.

   This error is passed to generators to trigger a close

   Error code MessageErrorType_StopIteration (109)


   Construct a StopIterationException


   :param message: A message to pass to the user
   :type message: str
   :param subname: An optional sub_name to further categorize the exception
   :type subname: str
   :param param_: An optional exception parameter
   :type param|: Any

   .. attribute:: errorcode

      (int) Predefined error code

      Error code that specifies the type of error See MessageErrorType constants.

   .. attribute:: errorname

      (str) Name of the error

      Name corresponding to predefined error code, or a user defined
      error type

   .. attribute:: message

      (str) Message to pass to the user

      Human readable message to pass to the user

   .. attribute:: errorsubname

      (str) An optional sub_name to further categorize the exception

      The sub_name should follow the Robot Raconteur naming rules. It should
      either be a single name or a fully qualified name using service definition
      naming rules

   .. attribute:: errorparam

      (Any) An optional parameter

      Use this placeholder if data needs to be passed with the exception



.. class:: RobotRaconteur.OperationTimeoutException(message=,subname=None,param_=None)

   Exception thrown when an operation does not complete in the expected time


   Error code MessageErrorType_OperationTimeout (110)


   Construct a OperationTimeoutException


   :param message: A message to pass to the user
   :type message: str
   :param subname: An optional sub_name to further categorize the exception
   :type subname: str
   :param param_: An optional exception parameter
   :type param|: Any

   .. attribute:: errorcode

      (int) Predefined error code

      Error code that specifies the type of error See MessageErrorType constants.

   .. attribute:: errorname

      (str) Name of the error

      Name corresponding to predefined error code, or a user defined
      error type

   .. attribute:: message

      (str) Message to pass to the user

      Human readable message to pass to the user

   .. attribute:: errorsubname

      (str) An optional sub_name to further categorize the exception

      The sub_name should follow the Robot Raconteur naming rules. It should
      either be a single name or a fully qualified name using service definition
      naming rules

   .. attribute:: errorparam

      (Any) An optional parameter

      Use this placeholder if data needs to be passed with the exception



.. class:: RobotRaconteur.OperationCancelledException(message=,subname=None,param_=None)

   Exception thrown when an operation is cancelled before it is started


   Error code MessageErrorType_OperationCancelled (111)


   Construct a OperationCancelledException


   :param message: A message to pass to the user
   :type message: str
   :param subname: An optional sub_name to further categorize the exception
   :type subname: str
   :param param_: An optional exception parameter
   :type param|: Any

   .. attribute:: errorcode

      (int) Predefined error code

      Error code that specifies the type of error See MessageErrorType constants.

   .. attribute:: errorname

      (str) Name of the error

      Name corresponding to predefined error code, or a user defined
      error type

   .. attribute:: message

      (str) Message to pass to the user

      Human readable message to pass to the user

   .. attribute:: errorsubname

      (str) An optional sub_name to further categorize the exception

      The sub_name should follow the Robot Raconteur naming rules. It should
      either be a single name or a fully qualified name using service definition
      naming rules

   .. attribute:: errorparam

      (Any) An optional parameter

      Use this placeholder if data needs to be passed with the exception
