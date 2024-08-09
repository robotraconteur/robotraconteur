Service Definition
==================

ServiceDefinition Example ``experimental.create3`` (below)
shows the code contained in the “experimental.create3.robdef" file. It
is a *service definition*. Service definition files are plain text files
that describe the *object types* and *value types* (data types). Object
types are *references*, meaning that on the client they are simply an
advanced reference to the service. Value types are the actual data that
are transmitted between client and service. They are always passed by
*value*, meaning that they are local to the client or service.

::


  #Service to provide sample interface to the iRobot Create
  #This example is for the original iRobot Create using the serial Open Interface (OI) protocol
  service experimental.create3

  stdver 0.10

  enum CreateStateFlags
    unknown = 0,
    bump_right = 0x1,
    bump_left = 0x2,
    wheel_drop_right = 0x4,
    wheel_drop_left = 0x8,
    wheel_drop_caster = 0x10,
    wall_sensor = 0x20,
    cliff_left = 0x40,
    cliff_front_left = 0x80,
    cliff_front_right = 0x100,
    cliff_right = 0x200,
    virtual_wall = 0x400,
    play_button = 0x800,
    advance_button = 0x1000,
    error = 0x800000
  end

  struct CreateState
    field double time
    field uint32 create_state_flags
    field double velocity
    field double radius
    field double right_wheel_velocity
    field double left_wheel_velocity
    field double distance_traveled
    field double angle_traveled
    field double battery_charge
    field double battery_capacity
  end

  object Create
    constant double DRIVE_STRAIGHT 32.767
    constant double SPIN_CLOCKWISE -1e-3
    constant double SPIN_COUNTERCLOCKWISE 1e-3

    function void drive(double velocity, double radius)
    function void drive_direct(double right_wheel_velocity, double left_wheel_velocity)
    function void stop()
    function void setf_leds(bool play, bool advance)

    property double distance_traveled [readonly]
    property double angle_traveled [readonly]
    property uint8 bumpers [readonly]

    event bump()

    wire CreateState create_state [readonly]

    # Callback to be called when the play button is pressed
    # claim_play_callback() will assign the current client as the target for the callback
    # Practical implementations will likely want to use a more sophisticated mechanism to assign the callback
    function void claim_play_callback()
    callback uint8[] play_callback(double distance_traveled, double angle_traveled)
  end

The first line in the service definition contains the keyword
``service`` followed by the name of the service type. “Namespaces”
follow similar rules to Java package names. For experimental software,
the name should be prefixed with “experimental”, for example
"experimental.create3". For hobbyists and standalone software, the name
should be prefixed with “community” and your username, for example
“community.myusername.create”, where “myusername” is replaced with your
``robotraconteur.com`` username. If a domain name for an organization is
available it can be used in the same way as Java packages, for example
"com.wasontech.examples.create2". Unless you have valid
ownership of a domain, “experimental” or “community” should be used.

Next in the service there should be “stdver" and the minimum version of
Robot Raconteur required to access the service. For now this should be
“0.10". Example "experimental.create3" does not show
it, but there can also be one or more “import" to reference structures
and objects in other service definitions. The rest of service definition
defines the *structures* and *objects* of the service definition. (Lines
starting with “#" are comments.)

Value types
-----------

Value types are the data that are passed between the client and service.
Value types can be *primitives*, *structures*, *pods*, *namedarrays*,
*maps*, *multidimensional arrays*, or *enums*.

Primitives
   |
   | Primitives consist of scalar numbers, single dimensional number
     arrays, and strings. Table `1 <#primitives>`__ contains the
     primitives that are available for use. Primitive numerical types
     can be turned into arrays by appending brackets “[]" to the end,
     for example ``int32[]`` is an array of 32 bit signed integers. If a
     fixed size array is desired, a number can be included between the
     brackets for the desired array size, for example ``int32[8]`` has a
     fixed length of 8 integers. If an array is desired that has a
     maximum size, a “-" sign can be included in the brackets, for
     example ``int32[100-]`` can have up to 100 integers. Strings are
     always arrays so the brackets are not valid. The ``void`` type is
     only used for functions that do not have a return value.

   |

   .. container::
      :name: primitives

      .. table:: Robot Raconteur Primitives

         ======= ============= =======================================
         Type    Bytes/Element Description
         ======= ============= =======================================
         void    0             Void
         double  8             Double precision floating point
         single  4             Single precision floating point
         int8    1             Signed 8-bit integer
         uint8   1             Unsigned 8-bit integer
         int16   2             Signed 16-bit integer
         uint16  2             Unsigned 16-bit integer
         int32   4             Signed 32-bit integer
         uint32  4             Unsigned 32-bit integer
         int64   8             Signed 64-bit integer
         uint64  8             Unsigned 64-bit integer
         string  1             UTF-8 string
         cdouble 16            Complex double precision floating point
         csingle 8             Complex single precision floating point
         bool    1             Logical boolean
         ======= ============= =======================================

Structures
   |
   | Structures are collections of value types; structures can contain
     primitives, other structures, maps, or multidimensional arrays.
     Example "experimental.create3" shows the
     definition of the structure ``CreateState``. A structure is
     started with the keyword ``struct`` followed by the structure name.
     It is ended with the ``end`` keyword. The entries in the structure
     are defined with the keyword ``field`` followed by the type, and
     finally the name of the field. If a structure from a different
     service definition is used, first the referenced service definition
     is imported at the top of the service definition and the structure
     is referenced by the external service definition “dot" the name of
     the structure.

Pods
   |
   | Pods (short for “plain-old-data”) are similar to structures, but
     are more restricted to ensure they always have the same size. All data
     stored in pods are stored contiguously (c-style), while structs use
     pointers to the data. Pods can only contain pods, arrays of pods
     (fixed or max length), namedarrays, and namedarrays arrays (fixed
     or max length). Only types with fixed or maximum size may be used;
     strings,
     structs, lists, and maps may not be stored in pods. A pod is
     started with the keyword ``pod`` followed by the pod name. It is
     ended with the ``end`` keyword. The entries in the pod are defined
     with the keyword ``field`` followed by the type, and finally the
     name of the field. If a pod from a different service definition is
     used, first the referenced service definition is imported at the
     top of the service definition and the pod is referenced by the
     external service definition “dot" the name of the pod. Pods can be
     used with arrays and multi-dim arrays.

Namedarrays
   |
   | Namedarrays are a union type designed to store numeric arrays that
     also have specific meanings attached to each entry. An example is a
     3D vector. The vector can either be viewed as a 3x1 array, or as a
     structure containing (x,y,z). A namedarray stores the contained
     data as a primitive array, but allows the data to be viewed as a
     structure. Namedarrays should be used when possible since they have
     the most compact memory format. Namedarrays can only contain
     numeric primitives, fixed numeric primitive arrays (no
     multidimarrays), other namedarrays (with the same numeric type),
     and fixed arrays of namedarrays. A namedarray is started with the
     keyword ``namedarray`` followed by the namedarray name. It is ended
     with the ``end`` keyword. The entries in the namedarray are defined
     with the keyword ``field`` followed by the type, and finally the
     name of the field. If a namedarray from a different service
     definition is used, first the referenced service definition is
     imported at the top of the service definition and the namedarray is
     referenced by the external service definition “dot" the name of the
     namedarray. Namedarrays can be used with arrays and multi-dim
     arrays.

Maps
   |
   | Maps can either be keyed by ``int32`` or ``string``. In other
     languages they would be called “Dictionary", “Hashtable", or “Map".
     The data is a value type (but not another map). They are created
     with curly braces. For example, ``string{int32}`` would be a map of
     strings keyed by an integer. ``string{string}`` would be a map of
     strings keyed by another string. ``CreateState{string}`` and
     ``int32[]{int32}`` are also valid examples.
   | ``string{int32}{int32}`` is *not* valid. There can only be one
     dimension of keying.

Lists
   |
   | Lists follow similar rules to maps. They are created with curly
     braces. For example,
   | ``string{list}`` would be a list of strings. ``CreateState{list}``
     and ``int32[]{list}`` are also valid examples.
   | ``string{list}{list}`` is *not* valid. There can only be one
     dimension of lists.

Multidimensional Arrays
   |
   | The multidimensional arrays allow for the transmission of real or
     complex matrices of any dimension. They are defined by putting a
     “*" inside the brackets of an array. For example, ``double[*]``
     defines a multidimensional array of doubles. Multidimensional
     arrays can also have fixed dimensions. For example ``double[3,3]``
     defines a 3x3 matrix. The dimensions are in matrix (column-major)
     order.

Enums
   Enums are a special representation of int32 that names each value.
   Enums are aliases, with the value be stored as int32 internally. An
   enum is started with the keyword ``enum`` followed by the enum name.
   It is ended with the ``end`` keyword. The values are specified with a
   “name" = “value" format, separated by commas. Values can be signed
   integers, unsigned hexadecimal, or omitted to implicitly increment
   from the last value.

   ::

      enum myenum
          value1 = -1,
          value2 = 0xF1,
          value3,
          value4
      end

varvalue
   |
   | In certain situations it may be desirable to put in a “wildcard"
     value type. The varvalue type allows this. Use with caution!

**Note: structs, maps, and lists can be null. All other types are not
nullable. (NULL, None, etc. depending on language).**

.. _objecttypes:

Object types
------------

Objects begin with the keyword ``object`` followed by the name of the
object, and closed with the keywords ``end``. Objects have *members*
that implement functionality. Within Robot Raconteur there are eight
types of members: Properties, Functions, Events, ObjRefs, Pipes,
Callbacks, Wires, and Memories . They are defined between ``object`` and
``end``.

Properties (Keyword: ``property``)
   |
   | Properties are similar to class variables (field). They can be
     written to (set) or read from (get). A property can take on any
     value type. A property is defined within an object with the keyword
     ``property`` followed by the value type of the property, and
     finally the name of the property. (All member names must be
     unique). An example:

   ``property double myvar``

   Properties can use modifiers ``readonly``, ``writeonly``, ``urgent``,
   and/or ``perclient``. See :doc:`MemberModifiers`.

Functions (Keyword: ``function``)
   |
   | Functions take zero or more value type parameters, and return a
     single value type. The parameters of the functions must all have
     unique names. The return value of the function may be ``void`` if
     there is no return. A function is defined by the keyword
     ``function`` followed by the return type, followed by the name of
     the function. The parameters follow as a comma separated list of
     parameter type and parameter name. The parameter list is enclosed
     with parenthesis. An example:

   ``function double addTwoNumbers(int32 a, double b)``

   Functions can also return a "generator," which is a form of iterator.
   (These generators are modeled after Python generators.) This is
   useful for long running operations or to return large amounts of
   data. Generators take three forms. The first is when each iteration
   of the generator takes a parameter and returns a value. This takes
   the form:

   ``function double{generator} addManyNumbers(int32 a, double{generator} b)``

   In this example, the “a” parameter is sent with the function call,
   while “b” and “return” are sent and received using the “Next”
   function of the generator.

   The next form of the generator returns a value each iteration of the
   generator.

   ``function double{generator} getSequence(int32 a, double b)``

   In this example, “a” and “b” are sent with the function call, and
   “return” is returned using the “Next” function of the generator.

   The last form takes a parameter each iteration.

   ``function void accumulateNumbers(double{generator} b)``

   Note that the generator return must be “void” or a generator type.
   Each call to “Next” will receive a parameter.

   Generators will throw either “StopIterationException” to signal that
   the generator is finished, or it will throw
   “OperationAbortedException” to signal that there was an error and the
   generator should be destroyed. Generators clients must call “Close”
   or “Abort” on a generator if a “StopIterationException” or other
   exception is not received.

   Generators that represent long running operations should return from
   “Next” with updated status information at least every 10 seconds to
   prevent timeout errors.

   Functions can use the ``urgent`` modifier. See See :doc:`MemberModifiers`.

Events (Keyword: ``event``)
   |
   | Events provide a way for the service to notify clients that an
     event has occurred. When an event is fired, every client reference
     receives the event. How the event is handled is language-specific.
     An event is defined similar to a function, however there is no
     return. The parameters are passed to the client. An example:

   ``event somethingHappened(string what, double when)``

   Note that events do not have flow control, so they should be used
   sparingly.

   Events can use the ``urgent`` modifier. See :doc:`MemberModifiers`.

Object References (Keyword: ``objref``)
   |
   | A service consists of any number of objects. The *root object* is
     the object first referenced when connection to a service. The other
     object references are obtained through the ``objref`` members.
     These members return a reference to the specified object. An objref
     is defined by the keyword ``objref`` followed by the object type
     followed by the objref member name. The object type can be
     ``varobject`` to return any valid object type (Use with caution!).
     The objref can also be indexed by number ([],{int32}) or by string
     ({string}). This returns a different reference based on the index.
     It does not return a set of references. An example:

   ``objref mysubobj anotherobj{string}``

   If an object from a different service definition is used, first the
   referenced service definition is imported at the top of the service
   definition and the object is referenced by the external service
   definition “dot" the name of the object.

Pipes (Keyword: ``pipe``)
   |
   | Pipes provide full-duplex first-in, first-out (FIFO) connections
     between the client and service. Pipes are unique to each client,
     and are indexed so that the same member can handle multiple
     connections. The pipe member allows for the creation of
     “PipeEndpoint" pairs. One endpoint is on the client side, and the
     other is on the server side. For each connected pipe endpoint pair,
     packets that are sent by the client appear at service end, and
     packets that are sent by the service end up on the client side.
     Packets can be retrieved in order from the receive queue in the
     “PipeEndpoint". The type of the packets is defined by the member
     definition. An endpoint can request a Packet Acknowledgment to be
     sent once the packet is received by setting “RequestPacketAck" to
     true. “SendPacket" is used to send packets, and “ReceivePacket" is
     used to receive the next packet in the queue. “Available" can be
     used to determine is more packets are available to receive. Pipe
     endpoint pairs are created with the “Connect" function on the
     client. Either the client or the service can close the endpoint
     pair using the “Close" function. A pipe is specified by the keyword
     ``pipe`` followed by the packet type, followed by the member name
     of the pipe. An example:

   ``pipe double[] sensordata``

   Pipes can use modifiers ``readonly``, ``writeonly``, and
   ``unreliable``. See :doc:`MemberModifiers`.

Callbacks (Keyword: ``callback``)
   |
   | Callbacks are essentially “reverse functions", meaning that they
     allow a service to call a function on a client. Because a service
     can have multiple clients connected, the service must specify which
     client to call. The syntax is equivalent to the “function", just
     replace “function" with “callback". An example:

   ``callback double addTwoNumbersOnClient(int32 a, double b)``

Wires (Keyword: ``wire``)
   |
   | Wires are very similar to pipes, however rather than providing a
     stream of packets the wire is used when only the “most recent"
     value is of interest. It is similar in concept to a “port" in
     Simulink. Wires may be transmitted over lossy channels or channels
     with latency where packets may not arrive or may arrive out of
     order. In these situations the lost or out of order packet will be
     ignored and only the newest value will be used. Each packet has a
     timestamp of when it is sent (from the sender’s clock). Wires are
     full duplex like pipes meaning it has two-way communication, but
     unlike pipes they are not indexed so there is only one connection
     per client object reference, per connection. The wire allows for a
     “WireConnection"
     pair to be created with one “WireConnection" on the client and the
     other “WireConnection" on the service. The “WireConnection" is used
     by setting the
     “OutValue" to the current value. This sends the new value to the
     opposite “WireConnection", which updates its “InValue". The same
     can be reversed. For instance, setting the “OutValue" on the
     service changes the “InValue" on the client, and setting the
     “OutValue" on the client changes the “InValue" on the service. It
     as also possible to receive the “LastValueReceivedTime" and
     “LastValueSentTime" to read the timestamps on the values. Note that
     “LastValueReceivedTime" is in the *sender’s* clock, not the local
     clock and is generated when it is first transmitted. Either the
     client or the service can close the “WireConnection" pair using the
     “Close" function.
     As of Robot Raconteur version 0.12, wires also have "lifespan"
     for "InValue" and "OutValue", which will invalidate the received
     data after a specified time after reception. This prevents the
     wire from returning stale data. Use the "InValueLifespan"
     and "OutValueLifespan" to configure.

   The wire provides the basis for real-time communication. (See
   also :doc:`TimeCritical`.) An example wire
   member definition:

   ``wire double[2] currentposition``

   Wires can use modifiers ``readonly`` or ``writeonly``.
   See :doc:`MemberModifiers`.

Memories (Keyword: ``memory``)
   |
   | Memories represent a random-access segment of numeric primitive
     arrays, numeric primitive multi-dim arrays, pod arrays, pod
     multi-dim arrays, namedarrays arrays, and namedarrays multi-dim
     arrays. The memory member is available for two reasons: it will
     break down large read and writes into smaller calls to prevent
     buffer overruns (most transports limit message sizes to 10 MB, unless
     "jumbo" messages are enabled) and
     the memory also provides the basis for future shared-memory
     segments. An example:

   ``memory double[] datahistory``

   Memories can use modifiers ``readonly`` or ``writeonly``.
   See :doc:`MemberModifiers`.

Constants
---------

Constants can be specified using the ``constant`` keyword. The constants
can be numbers, single dimensional arrays, or strings. Constants can
exist either in the global service definition scope, in objects, or in
structs.

| ``constant uint32 myconst 0xFB``
| ``constant double[] myarray {10.3, 584.9, 594}``
| ``constant string mystring "Hello world!"``

Exceptions
----------

Robot Raconteur will transparently pass exceptions thrown by the
receiver to the caller for transactions such as functions, properties,
callbacks, and memory reads/writes. Normally these exceptions are of the
type RobotRaconteurRemoteException which is a universal container for
all types of exceptions. In some cases it is useful to have named
exceptions that can be passed from receiver to caller and keep their
class type. These custom exceptions inherit from
RobotRaconteurRemoteException. Service definitions can define these
exceptions. Exceptions are defined by starting the line with
``exception`` followed by the name of the exception. For example, the
following line will define the exception “MyException” which can then be
used in any of the supported languages:

``exception MyException``

Using
-----

To reduce the clutter in a service definition file, the “using”
statement can be used to alias an imported type.

``using example.importeddef.obj1``

“as” can be used to change the name locally.

``using exmaple.importeddef.obj1 as another_obj1``

Robot Raconteur naming
----------------------

When naming things like service definitions, objects, structures, and
members, certain rules must be followed. The name must consist of
letters, numbers, and underscores (_). Names must start with a letter
and may not start with any uppercase/lowercase combination of
"RobotRaconteur", "RR", "get\_", "set\_", or "_async\_". Service
names may not end with “_signed".
