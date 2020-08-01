# Robot Raconteur Nodes and Communication {#nodes_and_communication}

Is discussed in the \ref introduction, Robot Raconteur is an an augmented object-oriented remote procedure call communication framework. The framework is designed for use with robotics and automation systems, providing specialized value types and object members. These advanced features are built on top of a simple message passing system. Member requests-response operations and data packets are serialized into simple messages, serialized, and transmitted between the nodes. The exact details of how this works is covered in the [Robot Raconteur Standards](https://github.com/johnwason/robotraconteur_standards/) documents. A simple description of how this works is covered follows in the rest of this page.

## Robot Raconteur Nodes {#nodes}

A Robot Raconteur "Node" is the central management and communication point for each independent Robot Raconteur instance. Communication always begins and ends at a node, and all communication flows between nodes (or loopback to the same node). Nodes serve as the central connection point for the various components of a Robot Raconteur instance. Messages and other data flow between different elements within the instance of the node through the node.

### NodeID and NodeName {#nodeid}

Nodes are identified using a "NodeID" and/or a "NodeName". A "NodeID" is a 128-bit version 4 Universally Unique Identified (UUID) as defined in [IETF RFC 4122](https://tools.ietf.org/html/rfc4122). Robot Raconteur UUID are always big-endian format, with curly-braces being optional. UUID are designed to provide randomly generated universally unique identifiers without the need for a central authority to ensure their uniqueness. Because of the vast possible identifiers (5.3 × 10^36 for version 4), a collision is highly unlikely. See the [wikipedia article]() for more information on UUID formats, statical properties, and text formats.

NodeIDs are expected to be unique for every node.

The following are examples of valid NodeIDs:

    {7cf6ea2a-3f7d-45d7-8781-f76179a475a2}
    66a56a8c-b49b-4fc7-878a-076ca88b1d55
    8edcf923-c94e-4484-9933-7b1df940b493

NodeIDs are typcially generated randomly for clients, but must be static for services so clients can correctly address the service. The \ref local_transport is typically used to handle assigning a fixed "NodeID" based on a "NodeName". The "NodeID" is generated on the first request, and stored on the filesystem to be reused.

"NodeName"s are a human-readable string used to name the node. They follow the same naming rules as Service Definition names. See \ref service_definition for more information on Service Definition names. Unlike NodeIDs, NodeNames are not guaranteed to be unique, but are instead intended to be a convenience for connection to a node when there is no ambiguity. (It is recommended that NodeID be used in production environments). When the Local Transport is used to assign a NodeID based on NodeName, the NodeName must be unique on the local system.

### Messages {#messages}

The remote procedure call of Robot Raconteur is built on top of a message passing system. The metadata about the operation being performed and the value type parameters are "packed" into messages. These messages can then be serialized into a binary data stream, and transmitted to the remote node. On the remote node, the message is deserialized, and "unpacked". The unpacked message can then be used by the remote procedure call layer to execute the operation. This process is complex, but the user does not need to be concerned with the details since it is encapsulated by the Robot Raconteur software and ``thunk`` code generators. See [Robot Raconteur Standards](https://github.com/johnwason/robotraconteur_standards/) documents for the details on how messages work.

Value types and each member operation have standardized formats for being packed into messages.

**Note: Messages are always serialized as little-endian, with the exception of UUID which are always big-endian.*

#### Value Types {#message_value_types}

Value types are passed *by value*, meaning that they are copied when transmitted between nodes. A change to the data on one node will not affect the other unless it is re-transmitted. This section contains a brief overview of value type packing. See the [Robot Raconteur Value Types Standard](https://github.com/johnwason/robotraconteur_standards/blob/master/draft/value_types.md) for more details.

##### Numeric Primitives and Primitive Arrays

Numeric Primitives and Primitive Arrays are packed as numeric arrays, with a specified type and element count. Scalars are packed as single element arrays. Complex types are interleaved, with the real component followed by the imaginary component for each element. Floating point numbers are serialzed using the [IEEE 754](https://en.wikipedia.org/wiki/IEEE_754) format. Signed integers use two's complement.

##### Strings

Strings are packed using UTF-8 as defined in [IETF RFC 3629](https://tools.ietf.org/html/rfc3629).

##### Multi-dimensional Arrays

Multi-dimensional arrays are packed as two fields, the dimensions of the array and a flattened data array. The dimensions is a `uint32` array, stored in column-major order. The data array is flattened in column-major (Fortran) order.

##### Structures

Structures are stored as a list of fields, each field having a name and a value. The value is any valid packed value type. The type of the structure is included with the packed value.

##### Pods

Pods are always stored in array from. The packed pod array contains an array of packed pods. The packed pods currently have the same format as structures. (Future message versions will use a more compact packing format.) The type of the pod is included with the packed value.

Pod multidimensional arrays use the same format as numeric multidimensional arrays, with a dimension and array field. The array field contains the flattened pod array.

##### Namedarrays

Namedarrays are packed as a numeric array with A x N elements, where A is the number of numeric elements in each namedarray element, and N is the number of namedarray elements. The type of the array is included with the numeric array.

Namedarray multidimensional arrays use the same format as numeric multidimensional arrays, with a dimension and array field. The array field contains the flattened namedarray array.

##### Maps

Maps are stored as a list of elements, each element with a key and a value. The key can either be a `string` or an `int32`. The value can be any valid packed value type.

##### Lists

Lists are stored as a list of elements, each element with an index and a value. The list is stored as an `int32`, starting at zero with each following element increasing by one. The value can be any valid packed value type.

##### Enums

Enums are always packed as a scalar `int32`. The type of the enum is not included in the packed message. Enums are syntactic sugar.

##### VarValue

`varvalue` is a wildcard type that can be specified in service definitions. When `varvalue` is used, the node will attempt to pack the data supplied into a valid message format. The receiver will use the message metadata to unpack the `varvalue`. Messages are always strongly typed, since during packing the node knows what the data types are. `varvalue` instructs the node to use reflection and metadata to accomplish the packing and unpacking.

#### Object Protocol {#message_object_protocol}

The remote procedure call layer uses messages to transmit operations between nodes. These operations and their behavior is called the "Object Protocol". This section contains a brief overview of the object protocol. See the [Robot Raconteur Object Protocol Standard](https://github.com/johnwason/robotraconteur_standards/blob/master/draft/object_protocol.md) for more details.

Messages passing between nodes can either be a request, a response, or a packet. Requests consist of a request message being sent to the remote node, with the expectation of a response. (The request and response messages are identified using a unique number to match requests and responses.) Packets are messages that are sent to the remote node without the expectation of a response. Packets may be reliable, meaning they are required to arrive at the remote node, or unreliable, meaning that they may be discarded due to an error, congestion, etc.

Requests, responses, and packets contain a "service path" and "member name" to identify the relevant object and member. See \ref servicepath.

##### Connection and Session Management

A number of messages are used to create, manage, and destroy client connections and sessions. These messages are normally invisible to the user.

##### Exceptions

Requests, responses, or packets may contain an exception. The exception type is specified with a code, and additional information about the error. See \ref exceptions.

##### `property` Member

Properties provide a "get" and "set" operation, to retrieve the current value of the property, or set the value. This is implemented using requests. For the "get" operation, the client sends a request with no values requesting the property value. The service sends a response containing the current property value, or an exception response. For the "set" operation, the client sends a request containing the new property value. The service sends an empty response, or an exception response.

##### `function` Member

Functions take zero or more value type parameters, and return a single value. For a normal (non-generator) function, the client sends a request  containing the parameters packed into the message. The service executes the requested function and returns a response with an optional return value packed into the message, or an exception response.

Function generators work slightly differently. The client sends a request with the parameters not marked as generator parameters packed into the message. The service executes the function, and returns an identifier for the generator returned by the function, or an exception response. The client then sends a request  for "next" with an optional parameter packed into the message. The "next" request includes the identifier of the generator previously returned by the function call. The service calls "next" on the specified generator, and returns a response with an optional return value packed into the message, or an exception response. A "StopIteration" exception is returned to signal the generator has no more values to return. The client sends "close" or "abort" to the service by packing the "StopIteration" or "AbortOperation" exceptions into the request.

##### `event` Member

Events are used to asynchronously notify all connected clients, optionally with arguments. Services send event packets to all connected clients, with optional parameters packed into the message.

##### `objref` Member

Objrefs are used to locate additional objects within a service. Objects are always owned by the service, and are identified by the client using a "service path". (See \ref servicepath) When a user executes the objref, the client will check to see if the type of the object is known. If it is not known, a request will be sent to the service for the type of the object. The service sends a response with the type of the object packed as string, or an exception response.

Sometimes a service may want to delete an object, or replace it with a different one. This is done using a `ReleaseServicePath` operation. The object (and all objref'd objects below it in the tree) are released, and packets are sent to all connected clients (or a specified subset) informing them that the service path has been released, and the client should re-query the object type.

##### `pipe` Member

Pipes are used to send a reliable stream of data packets between client and service. Pipes must be "connected" to begin sending data. Clients send a request packet, with an optional "index" packed into the message. The index is necessary because pipes can have multiple connections, allowing for more than one data stream. The service creates its endpoint based on the index (or a new index if the client specified -1), and sends a response with the index packed into the message, or an exception response.

Now that the pipe is connected, data can flow between the endpoint pairs. The process for sending data is the same for both client and service. The data being sent is formed into a packet, with the index, packet number, and data value packed into the message. The packet number is used to reorder the data on the receiver side. The packet may optionally be marked as "request ack". If an ack is requested, the receiver will send a response containing the index and the packet number. The ack is typically used for flow control.

The client closes the pipe connection by sending a close request, with the index packed into the message. The service sends a response, or an exception response. The service closes the pipe connection by sending a close notification packet to the client. No response is sent by the client.

##### `callback` Member

Callbacks are "reverse functions", with the request sent by the service instead of the client. the service sends a request containing the parameters packed into the message. The client executes the requested function and returns a response with an optional return value packed into the message, or an exception response. The service must specify which client to execute the function, since the service may have multiple clients connected. This is normally accomplished by using the "endpoint identifier" of the client.

Generators are not supported for callbacks.

##### `wire` Member

Wires are used to transmit a "most recent" value. The value can be sent either client to service, or service to client. Wires can either be connected to stream values, or "peek" and "poke" operations can be used to read and write values.

Connecting a wire initiates data streaming. The client creates the connection by sending a connect request. The service sends a response, or an exception response. Once connected, either wire connection can set its "out value". The wire connection generates a packet, with the value and timestamp packed into the message. The receiving wire connection unpacks the packet, and if the timestamp is newer, sets its "in value" to the received value. The transport may discard wire packets if the value is older, or for flow control. Wire connections are closed by the client sending a close request, with the service sending a response, or an exception response. Unlike pipes, wires are not indexed, so there can only be one wire connection per client.

"Peek" and "poke" operations can be used when a client does not require a streaming interface, but only needs an instaneous value. The client can "peek" both "in value" and "out value". The direction corresponds to the client. For peek, the client sends a peek request to the service. The service sends a response with its current value and the timestamp of the value, or an exception response. For poke, the client sends a poke request with the new value and timestamp packed into the message. The service sends a response, or an exception response.

##### `memory` Member

Memories represent a typed random access memory segment. Clients can execute "read" and "write" operations on a segment of the memory. Clients can also request parameters about the memory.

Clients retrieve a parameter by sending a request with the name of the parameter packed into the message. The service sends a response with the parameter value packed into the response.

Clients read a segment of the memory by sending a request with the offset and count of the memory elements to read. The service sends a response with the offset, the count, and the value of the segment packed into the message, or an exception response.

Clients write a segment of the memory by sending a request with the offset, the count, and the new value of the segment packed into the message. The service sends a response with the offset and segment packed into the message, or an exception response.

### Service Paths {#service_paths}

Service paths are used to identify objects within a service. When a service is created, a "root" object is specified. This object is returned when clients connect to the service as a reference. Clients access other objects within the service using `objref` members, which return references to other objects. `objref` form a hierarchical tree of objects, starting at the root and continuing through the other objects in the service. A service path string is used to represent this hierarchy, with each level separated by a "dot" ("."). For the root object, the name in the service path is the service name. Objects lower in the tree use the `objref` member name. `objref` indexed by an `int32` or `string` have square brackets with the index as a string appended to the name. (URL parameter encoding is used for non alphanumeric characters.)

When sending request, response, or packet messages, the service path is included to specify which object is being addressed. The message will typically also include the member name being addressed.

## Transports {#transports}

Transports are used to create connections between nodes. Clients create connections using \ref urls, which contain parameters the transport uses to create connections. Transports also implement discovery to help find nodes if the URL is not known.

The following transports are currently supported:

### TCP Transport

The TCP Transport provides communication between nodes over a TCP/IP LAN network or over the internet. It also implements discovery on a LAN network. TCP Transport can be used with the loopback adapter if both nodes are on the same system, but Local Transport is recomended in this configuration. TCP Transport supports WebSockets for use with web browsers and/or web servers. It also supports TLS encryption for secure communication.

URL Schemes: `rr+tcp`, `rr+ws`, `rrs+tcp`, `rrs+ws`, `rr+wss`, `rrs+wss`

The TCP transport supports both IPv4 and IPv6. IPv6 is recommended for LAN connections, since link-local addresses can be used. Link-local IPv6 address start with `ff80::`, followed by a 64-bit EUI-64 ID tied to the network adaptor. This link-local address is guaranteed to be fixed without configuration, eliminating the hassle of assigning fixed addresses for IPv4. IPv6 is used by default for discovery.

### Local Transport

The Local Transport is used for communication between nodes running on the same system using UNIX sockets. Node discovery uses the local filesystem to detect nodes.

URL Schemes: `rr+local`

### Intra Transport

The Intra Transport is used for communication between nodes running within the same process. For example, in a simulation there may be multiple devices being simulated running nodes within the same process. A singleton is used to detect and establish connections between nodes.

### Hardware Transport

The hardware transport provides experimental support for USB, PCIe, and Bluetooth devices. It is disabled by default.

## Robot Raconteur URLs {#urls}

URLs are used by clients to connect to services. The URL contains which transport to use, parameters for the transport, the NodeID and/or NodeName (optional), and the service name.

A URL has the following basic format:

`URL = scheme://[authority]/[path]?[nodeid=<nodeid>&][nodename=<nodename>&]service=<servicename>`

where the authority component may be empty, or for IP networks:

`authority = host[:port]`

The `host` may be "localhost", a DNS address name, IPv4 address, IPv6 address, or other transport specific hostname.

The `scheme` component starts with `rr` and may optionally hav a `+` followed by the transport to be used. For instance, `rr+tcp` would be a connection over TCP. The `rr` may optionally be `rrs` to specify that TLS should be used, for example `rrs+tcp` indicates that the connection will be made using TLS.

The components in brackets are optional. The `nodeid` and/or `nodename` can be used to specify the target node if it is ambiguous or for security requirements. The NodeID shall be in UUID canonical 8-4-4-4-12 hexadecimal string format.

The `path` component is not used by Robot Raconteur, but may be required for when Robot Raconteur is embedded in another server.

The `query` part contains the `service`, and optionally the `nodeid` and `nodename`. The rest of the query is ignored. This can be useful for when Robot Raconteur is embedded in another server.

The `transport` is a short representation of the transport in use, such as `tcp` or `local`.

A simple IPv4 URL example:

`rr+tcp://128.113.99.22:62432?service=example_service`

A simple IPv6 link local URL example:

`rr+tcp://[fe80::48ac:c24d:4a5b]?service=example_service`

A more complicated example using TLS, paths, and additional query components:

`rrs+ws://simple.example.robotraconteur.com/path/to/ws?rest_token=vwhqgfdoai&nodeid=82301bf1-e1ef-4e89-8d8f-0848291587ac&service=example_service&`

The following table contains the currently supported schemes and transports.

| Scheme | Transport | Required URL Components | Optional URL Components | Notes |
| ---    | ---       | ---                     | ---                     | ---   |
| `rr+tcp` | TCP Transport | `scheme`, `authority`, `service` | `port`, `nodeid`, `nodename` | Default port is 48653 |
| `rr+ws` | TCP Transport | `scheme`, `authority`, `service` | `port`, `path`, `nodeid`, `nodename` | WebSocket. Default port is 80. Path is HTTP server path |
| `rrs+tcp` | TCP Transport | `scheme`, `authority`, `service` | `port`, `nodeid`, `nodename` | TLS encrypted. Default port is 48653. `nodeid` is recommended for TLS verification. |
| `rrs+ws` | TCP Transport | `scheme`, `authority`, `service` | `port`, `path`, `nodeid`, `nodename` | WebSocket. Websocket messages are TLS encrypted. HTTP and message headers not encrypted. Default port is 80. Path is HTTP server path. `nodeid` is recommended for TLS verification. |
| `rr+wss`  | TCP Transport | `scheme`, `authority`, `service` | `port`, `path`, `nodeid`, `nodename` | WebSocket. HTTPS encrypts all data and headers. Default port is 443. Path is HTTP server path. |
| `rrs+wss` | WebSocket. Double encrypted. Default port is 443. Path is HTTP server path. `nodeid` is recommended for TLS encryption.
| `rr+local` | Local Transport | `scheme`, `service` `nodeid` and/or `nodename` | `authority` | NodeID and/or NodeName used to identify server node. |
| `rr+intra` | Intra Transport | `scheme`, `service` `nodeid` and/or `nodename` | | NodeID and/or NodeName used to identify server node. |
| `rr+usb` | Hardware Transport | `scheme`, `service` `nodeid` and/or `nodename` | | NodeID and/or NodeName used to identify device node. |
| `rr+pcie` | Hardware Transport | `scheme`, `service` `nodeid` and/or `nodename` | | NodeID and/or NodeName used to identify device node. |
| `rr+bluetooth` | Hardware Transport | `scheme`, `service` `nodeid` and/or `nodename` | | NodeID and/or NodeName used to identify device node. |

