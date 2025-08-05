# Exceptions {#exceptions}

Robot Raconteur uses exceptions in the event of an error or unexpected occurrence. Exceptions interupt the normal flow of execution, and are either "caught" by the program, caught by the Robot Raconteur framework, or terminate the program with an error condition. Many programming languages support exceptions; consult the documentation for the programming language of interest for more details on exceptions.

Robot Raconteur understands exceptions, and will forward an exception thrown on a remote node back to the caller during a request. The remote node will pack the exception into the response message, and the calling node will unpack and re-throw the exception. All response messages are capable of forwarding exceptions to the caller. Packets do not carry exceptions. See \ref message_object_protocol for a discussion of requests, responses, and packets.

## Robot Raconteur Built-in Exception Table

Robot Raconteur has a number of built-in exceptions. These exceptions are each assigned an "ErrorType" (a numeric code) and "ErrorName", which are stored in the message when the exception is packed.

| ErrorType | Code | ErrorName | Description |
| --- | --- | --- | --- |
| None | 0 | Success | *MessageEntry* does not contain an error |
| ConnectionError | 1 | RobotRaconteur.ConnectionError | Existing connection or connection creation has failed |
| ProtocolError | 2 | RobotRaconteur.ProtocolError | Protocol failure occurred on transport connection |
| ServiceNotFound | 3 | RobotRaconteur.ServiceNotFound | The requested service was not found |
| ObjectNotFound | 4 | RobotRaconteur.ObjectNotFound | The specified service path was invalid |
| InvalidEndpoint | 5 | RobotRaconteur.InvalidEndpoint | The specified endpoint was not found |
| EndpointCommunicationFatalError | 6 | RobotRaconteur.EndpointCommunicationFatalError | The specified endpoint has failed, and cannot be used for communication |
| NodeNotFound | 7 | RobotRaconteur.NodeNotFound | The requested node was not found |
| ServiceError | 8 | RobotRaconteur.ServiceError | The request service operation has failed |
| MemberNotFound | 9 | RobotRaconteur.MemberNotFound | The requested object member was not found |
| MemberFormatMismatch | 10 | RobotRaconteur.MemberFormatMismatch | The contents of the *message element* are invalid for the requested member |
| DataTypeMismatch | 11 | RobotRaconteur.DataTypeMismatch | The supplied *message element* does not contain the expected value type |
| DataTypeError | 12 | RobotRaconteur.DataTypeError | The supplied data is invalid for the requested usage |
| DataSerializationError | 13 | RobotRaconteur.DataSerializationError | The supplied message could not be serialized to the transform connection |
| MessageEntryNotFound | 14 | RobotRaconteur.MessageEntryNotFound | The required message entry was not found |
| MessageElementNotFound | 15 | RobotRaconteur.MessageElementNotFound | The required message element was not found |
| UnknownError | 16 | RobotRaconteur.UnknownError | An unknown error has occurred |
| InvalidOperation | 17 | RobotRaconteur.InvalidOperation | The requested operation was invalid for the current state |
| InvalidArgument | 18 | RobotRaconteur.InvalidArgument | The supplied argument was invalid for the operation or current state |
| OperationFailed | 19 | RobotRaconteur.OperationFailed | The requested operation has failed |
| NullValue | 20 | RobotRaconteur.NullValue | A value was null when a non-null value was required |
| InternalError | 21 | RobotRaconteur.InternalError | An unexpected internal error has occurred in the Robot Raconteur implementation |
| SystemResourcePermissionDenied | 22 | RobotRaconteur.SystemResourcePermissionDenied | Permission has been denied to a system resource such as file or device |
| OutOfSystemResource | 23 | RobotRaconteur.OutOfSystemResource | A system resource such as memory has been exhausted |
| SystemResourceError | 24 | RobotRaconteur.SystemResourceError | A system resource error has occurred |
| ResourceNotFound | 25 | RobotRaconteur.ResourceNotFound | A requested system resource was not found |
| IOError | 26 | RobotRaconteur.IOError | A generic IO error has occurred |
| BufferLimitViolation | 27 | RobotRaconteur.BufferLimitViolation | A buffer overrun has occurred in a transport |
| ServiceDefinitionError | 28 | RobotRaconteur.ServiceDefinitionError | The supplied service definition failed parsing or verification |
| OutOfRange | 29 | RobotRaconteur.OutOfRange | The supplied index was out of range for an array or list |
| KeyNotFound | 30 | RobotRaconteur.KeyNotFound | The supplied key was not found in a map |
| InvalidConfiguration | 31 | RobotRaconteur.InvalidConfiguration | An invalid configuration was specified or encountered |
| InvalidState | 32 | RobotRaconteur.InvalidState | An invalid state was specified or encountered |
| RemoteError | 100 | *user defined* | An (possibly user defined) error or exception has occurred on the remote node |
| RequestTimeout | 101 | RobotRaconteur.RequestTimeout | The request operation has timed out. Either the operation took too long or communication failed |
| ReadOnlyMember | 102 | RobotRaconteur.ReadOnlyMember | An attempt was made to write a read-only member |
| WriteOnlyMember | 103 | RobotRaconteur.WriteOnlyMember | An attempt was made to read a write-only member |
| NotImplementedError | 104 | RobotRaconteur.NotImplementedError | The requested member has not been implemented |
| MemberBusy | 105 | RobotRaconteur.MemberBusy | The requested member is busy. Try again |
| ValueNotSet | 106 | RobotRaconteur.ValueNotSet | The requested value has not been set and is currently undefined |
| AbortOperation | 107 | RobotRaconteur.AbortOperation | Only valid for generators. Abort the current iteration |
| OperationAborted | 108 | RobotRaconteur.OperationAborted | The current operation was aborted |
| StopIteration | 109 | RobotRaconteur.StopIteration | Only valid for generators. Close the generator, or generator is finished |
| OperationTimeout | 110 | RobotRaconteur.OperationTimeout | An operation did not complete in the expected time |
| OperationCancelled | 111 | RobotRaconteur.OperationCancelled | An operation was cancelled before starting |
| AuthenticationError | 150 | Authentication is required, or authentication failed |
| ObjectLockedError | 151 | The requested object is locked by another user or session |
| PermissionDenied | 152 | Permission to the requested resource was denied |

## Service definition exceptions

Service definitions may define new exception types using the `exception` keyword. These exceptions inherit `RobotRaconteurRemoteException`, which uses error type `RemoteError`. The fully qualified type of the exception is specified for `ErrorName`. This error name is used to unpack the exception to the correct exception defined in the service definition.

## Message

Each exception contains a human readable message describing the exception.

## Error sub-name

An optional error sub-name may be included in the exception to further specify the type of exception that occurred. The error sub-name is a string that should follow the naming rules of service definition members.

## Error parameter

An optional error parameter may be included in the message. This parameter should follow the rules for service attributes.

## Common Exceptions

The most common exception to encounter is the `ConnectionException`. This will be thrown when a connection is lost.

An `EndpointNotFoundException` typically means the service connection has been closed, or the remote node has shut down.

`ValueNotSet` exception is encountered when a Wire member has not received a value, or the current value has expired. This often happens when the wire connection is created, but the wire has not had an opportunity to receive a value. Add a delay, check to make sure the value is valid, or use `TryGetInValue` and check the return success to see if the value was ready.

`UnknownException` or `RemoteException` normally means that the remote node has thrown an exception. Check the log of the remote node to see what caused the error. Increase the log level if necessary. See \ref logging.
