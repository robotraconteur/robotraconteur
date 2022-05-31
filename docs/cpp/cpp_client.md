# C++ Clients {#cpp_client}

Robot Raconteur is an object-oriented client-service RPC framework. Services expose objects, which are made available to clients using "object references", sometimes referred to as "proxies". See \ref introduction for an overview of the Robot Raconteur framework.

The object types and their members are defined in "service definition" files. See \ref service_definition for more information on how service objects are defined. C++ uses `RobotRaconteurGen` to generate "thunk" source that implements the object and value types defined in service definitions. See \ref cpp_intro_cmake_thunk_source_gen, \ref robotraconteurgen, and \ref robotraconteur_generate_thunk for more information on thunk source and thunk source generation.

For C++, the thunk source generates an abstract class with pure virtual functions for accessing the members. The client uses this abstract interface class to interact with the service. The client returns a "stub" implementaton of the members, which call the remote node as needed.

Clients can choose between "synchronous" and "asynchronous" version of most member operations. Synchronous operations block the current thread, while asynchronous operations return immediately and invoke a supplied handler function when the operation is complete. See \ref cpp_threading_and_async for more information on asynchronous operations.


## Creating Client Connections {#cpp_client_connections}

Creating a client connection requires a node that has been set up. See \ref cpp_node_setup for details on how to set up a node.

Once a node has been set up, connections can be created to a service on a remote node using RobotRaconteur::RobotRaconteurNode::ConnectService or RobotRaconteur::RobotRaconteurNode::AsyncConnectService. These functions take a service URL, optional authentication information, optional client event callback, an optional desired object type, and for the asynchronous version a completion handler function. The connection operations return a RobotRaconteur::RRObjectPtr. This must be cast to the object type using RobotRaconteur::rr_cast.

Consider a simple example, with service definition type `example.my_service.MyObject` on a localhost node listening on TCP port 62222.

    // Assume that node has been set up
    using namespace RobotRaconteur;
    std::string url = "rr+tcp://localhost:62222?service=my_service";
    example::my_service::MyObjectPtr c = rr_cast<example::my_service::MyObject>(
        RobotRaconteurNode::s()->ConnectService(url)
    );

    // The connection "c" is now ready for use

An asynchronous example:

    // Assume that node has been set up
    using namespace RobotRaconteur;
    std::string url = "rr+tcp://localhost:62222?service=my_service";
    RobotRaconteurNode::s()->AsyncConnectService(
        url, "", nullptr, nullptr, "", 
        [](RRObjectPtr c1, RobotRaconteurExceptionPtr err)
        {
            if (err)
            {
                // Handle a connection error
                return;
            }
            example::my_service::MyObjectPtr c = rr_cast<example::my_service::MyObject>(c1);
            // "c" is now ready for use
        }
    );

    // Continue with other tasks and accept connection on handler invocation

### Robot Raconteur URLs {#cpp_client_urls}

URLs are used to connect to services. See \ref urls for more information on the URL format.

### Authentication {#cpp_client_auth}

Services may optionally use authentication to protect the service from unauthorized users. Authentication uses a "username" and "credentials". The username is a `std::string`, and the credentials are an `RRMapPtr<std::string,RRValue>`. If username is a zero length string and/or credentials is `nullptr`, authentication is disabled and the connection is anonymous. Typically the credentials map will contain a password entry. The following example uses password authentication:

    // Assume that node has been set up
    using namespace RobotRaconteur;
    std::string url = "rr+tcp://localhost:62222?service=my_service";
    std::string username = "myusername"
    std::string password = "password123"
    RRMapPtr<std::string,RRValue> credentials = AllocateEmptyRRMap<std::string,RRValue>();
    credentials->insert(std::make_pair("password",stringToRRArray(credentials)))
    example::my_service::MyObjectPtr c = rr_cast<example::my_service::MyObject>(
        RobotRaconteurNode::s()->ConnectService(url,username,credentials)
    );

    // The connection "c" is now ready for use

### Client Listener {#cpp_client_listener}

The `listener` parameter is an optional callback function to listen for client events. The signature of the callback function is expected to match:

    void listener(ClientContextPtr, ClientServiceListenerEventType, const boost::shared_ptr<void>&)

The possible event types can be found in RobotRaconteur::ClientServiceListenerEventType .

An example of using the event listener:

    // Assume that node has been set up
    using namespace RobotRaconteur;
    std::string url = "rr+tcp://localhost:62222?service=my_service";
    example::my_service::MyObjectPtr c = rr_cast<example::my_service::MyObject>(
        RobotRaconteurNode::s()->ConnectService(
            url, "", nullptr, 
            [](ClientContextPtr ctx, ClientServiceListenerEventType evt, const boost::shared_ptr<void>& p)
            {
                switch (evt)
                {
                    case ClientServiceListenerEventType_ClientClosed:
                        std::cout << "Client connection has closed" << std::endl;
                        return;
                    case ClientServiceListenerEventType_TransportConnectionClosed:
                        std::cout << "Client connection has been lost" << std::endl;
                        return;
                    default:
                        return;
                }
            }
        )
    );

    // The connection "c" is now ready for use

### Object Type {#cpp_client_connect_object_type}

The `objecttype` optional parameter can be used to specify the fully qualified object type. Use of this parameter is recommended, since services may return a newer subclass of the expected object that the client doesn't understand. If `objecttype` is specified, the desired object type will always be returned, if the service supports that object type. This parameter is one of the primary ways Robot Raconteur supports forward compatibility with newer devices.

## Disconnecting Clients {#cpp_client_disconnect}

Clients are automatically disconnected when the node is shut down, so it is normally not necessary to disconnect client connections. If closing client connections is necessary, RobotRaconteur::RobotRaconteurNode::DisconnectServiceor RobotRaconteur::RobotRaconteurNode::AsyncDisconnectService can be used.

## Subscriptions {#cpp_client_subscriptions}

Service subscriptions can be used instead of directly connecting and disconnecting client connections. Subscriptions automatically manage the client lifecycle, and are recommended in most situations. See \ref cpp_subscriptions for more information.

## Client Members {#cpp_client_members}

The thunk source automatically generates member proxies to access the remote service object members. These proxies make member access nearly seamless, meaning the user can for the most part ignore the fact that these members are running on a remote service. Each member type has its own interface.

More information an object members and the object message protocol can be found in \ref service_definition and \ref message_object_protocol.

### Property Members {#cpp_client_property}

Property members allow clients to "get" and "set" a property value on the service object. Properties may use any valid Robot Raconteur value type.

Property members are implemented as two access functions in the object, a "get" and "set" function. The "get" function is the name of member prepended with `get_`. It takes no arguments, and returns the current value. The "set" function is the name of the member prepended with `set_`. It takes the new property value, and returns void.

For example, the property definition:

    property double my_property

Would generate the two access functions in the C++ abtract interface:

    virtual double get_my_property();
    virtual void set_my_property(double val);

Assuming that `c` is a client connection to an object that has the member `my_property`, the following example shows getting and setting `my_property`:

    // Assume "c" is a connected client object reference with property "my_property"

    double current_value = c->get_my_property();
    std::cout << "my_property is currently: " << current_value << std::endl;

    // Set "my_property" to a new value

    c->set_my_property(1.234);

Properties can be declared `readonly` or `writeonly` using member modifiers. If a property is `readonly`, the `set_` accessor function is not generated. If a property is `writeonly`, the `get_` accessor function is not generated.

The thunk source generates asynchronous accessor functions as well as the above synchronous accessor. These functions are stored in a separate abstract interface with the same name as the standard abstract interface prefixed with `async_`. The asynchronous property accessor functions are prefixed with `async_get_` and `async_set_`. Consider the following example, where `c` is the synchronous type `example.my_service.MyObject` that contains the member `my_property`:

    // Assume "c" is a connected synchronous client object
    using namespace example::my_service;
    boost::shared_ptr<async_MyObject> c_async = rr_cast<async_MyObject>(c);

    c_async->async_get_my_property(
        [](double val, RobotRaconteurExceptionPtr err)
        {
            if (err)
            {
                std::cout << "Error occurred getting property value: " << err->what() << std::endl;
            }

            std::cout << "my_property is currently: " << val << std::endl;
        }
    );

    c_async->async_set_my_property(
        1.234, [](RobotRaconteurExceptionPtr err)
        {
            if (err)
            {
                std::cout << "Error occurred setting property value: " << err->what() << std::endl;
            }

            std::cout << "my_property value set successful" << std::endl;
        }
    );

The `async_set_` accessor will not be generated if the property is declared `readonly`. The `async_get_` accessor will not be generated if the property is declared `writeonly`.

### Function Members {#cpp_client_function}

Function members allow clients to invoke a function on the service object. Functions may have zero or more value type parameters, and return a value or be declared `void` for no return. Functions may be "normal", not using a generator, or be "generator functions" which return a generator.

#### Normal Functions {#cpp_client_normal_functions}

Normal functions accept zero or more value type parameters, invoke the remote function with these parameters, and return the result, or `void`. They are implemented in the abstract interface as a C++ function with the same name as the member.

For example, the function definition:

    function double addTwoNumbers(int32 a, double b)

Would generate the C++ function in the abstract interface:

    virtual double addTwoNumbers(int32_t a, double b);

Assuming that `c` is a client connection to an object that has the member `addTwoNumbers`, the following example shows invoking the function:

    // Assume "c" is a connected client object reference with function "addTwoNumbers"

    double result = c->addTwoNumbers(10, 1.234);
    std::cout << "addTwoNumbers result: " << result << std::endl;

An example function definition with no parameters and void return:

    functon void do_something()

Would generate the C++ function in the abstract interface:

    virtual void do_something();

Functions may also be invoked asynchronously. Like properties, the asynchronous form is in the `async_` abstract interface. The asynchronous version of the function is the member name prefixed with `async_`. Examples of asynchronous function invocatons:

    // Assume "c" is a connected synchronous client object
    using namespace example::my_service;
    boost::shared_ptr<async_MyObject> c_async = rr_cast<async_MyObject>(c);

    c_async->async_addTwoNumbers(10, 1.234
        [](double ret, RobotRaconteurExceptionPtr err)
        {
            if (err)
            {
                std::cout << "Error occurred invoking function: " << err->what() << std::endl;
            }

            std::cout << "addTwoNumbers returned: " << ret << std::endl;
        }
    );

    c_async->async_do_something(
        [](RobotRaconteurExceptionPtr err)
        {
            if (err)
            {
                std::cout << "Error occured invoking function: " << err->what() << std::endl;
            }

            std::cout << "do_something() invocation successful" << std::endl;
        }
    );

#### Generator Functions {#cpp_client_generator_functions}

Generator functions are similar to normal functions, but instead of returning a value or void, they return a generator. A generator is similar to an iterator, or can implement a coroutine. See RobotRaconteur::Generator and \ref service_definition_function for more discussion on generators.

Generators operate by calling `Next()` repeatedly until no more values are available, an exception is thrown, or the client closes/aborts the generator. The generator may be Type 1, 2, or 3, depending on if the generator accepts of parameter for `Next()`, and/or if it returns a value from `Next()`.

| Generator Type | Has Return | Has Parameter |  C++ Template |
| ---            | ---        | ---           |  ---          |
| Type 1         | Yes        | Yes           | Generator<Return,Param> |
| Type 2         | Yes        | No            | Generator<Return,void>  |
| Type 3         | No         | Yes           | Generator<void,Param>   |

##### Type 1 Generators

An example definition of a Type 1 generator:

    function double{generator} addManyNumbers(int32 a, double{generator} b)

This results in the following function being generated in the abstract interface:

    virtual Generator<double,double> addManyNumbers(int32_t a);

The function returns a generator that expects a parameter and returns a value every call to `Next()`. An example using the generator function:

    // Assume "c" is a connected client object reference with function "addTwoNumbers"
    
    using namespace RobotRaconteur;
    Generator<double,double> gen = c->addManyNumbers(10);
    try
    {
        for (double i : {1.2,3.4,5.6})
        {
            next_res = gen->Next(i);
            std::cout << "addManyNumbers generator result: " << next_res << std::endl;
        }
        gen->Close();
    }
    catch (StopIterationException&)
    {
        std::cout << "addManyNumbers no more values" << std::endl;
    }

##### Type 2 Generators

An example definition of a Type 2 generator:

    function int32_t getSequence(int32 a, double b)

This results in the following function being generated in the abstract interface:

    virtual Generator<double,void> getSequence(int32_t a, double b);

The function returns a generator that does not expect a parameter and returns a value every call to `Next()`. An example using the generator function:

    // Assume "c" is a connected client object reference with function "getSequence"
    
    using namespace RobotRaconteur;
    Generator<double,void> gen = c->getSequence(10,1.23);
    try
    {
        for (size_t i=0; i<max_elems; i++)
        {
            next_res = gen->Next();
            std::cout << "getSequence generator result: " << next_res << std::endl;
        }
        throw InvalidOperationException("Generator returned too many elements");
    }
    catch (StopIterationException&)
    {
        std::cout << "getSequence no more values" << std::endl;
    }

This example receives values from `Next()` until a RobotRaconteur::StopIterationException is thrown, or until it receives more elements than expected.

##### Type 3 Generators

An example definition of a Type 3 generator:

    function void sendSequence(int32 a, double{generator} b)

This results in the following function being generated in the abstract interface:

    virtual Generator<void,double> sendSequence(int32_t a);

The function returns a generator that expects a parameter and does not return a value every call to `Next()`. An example using the generator function:

    // Assume "c" is a connected client object reference with function "sendSequence"
    
    using namespace RobotRaconteur;
    Generator<void,double> gen = c->sendSequence(10);
    
    for (double i : {1.2,3.4,5.6})
    {
        gen->Next(i);
    }
    gen->Close();

##### Asynchronous Generator Functions

Generator functions also have asynchronous C++ functions generated in the `async_` abstract interace. They follow the same rules as normal functions, but return RobotRaconteur::Generator to the handler function. RobotRaconteur::Generator implements asynchronous RobotRaconteur::Generator::AsyncNext(), RobotRaconteur::AsyncClose(), and RobotRaconteur::AsyncAbort() for use with asynchronous clients.

#### Generator Clients {#cpp_client_generators}

See RobotRaconteur::Generator for more information on generator objects. Client generators inherit from RobotRaconteur::Generator and implement communication with the remote generator on the service.

### Event Members {#cpp_client_event}

Events are used by the service to notify all connected clients an event has occurred. Events may have zero or more value type parameters. Events are sent to all connected clients. In C++, events are implemented using `boost::signals2::signal`. See the documentation for `boost::signals2::signal` for more information on using Boost.Signals2. An example event definition:

    event somethingHappened(string what, double when)

This event definition results in the following function being generated in the C++ abstract interface:

    virtual boost::signals2::signal<void(std::string, double)>& get_somethingHappened();

An example of using the event:

    // Assume "c" is a connected client object reference with event "somethingHappened"
    
    using namespace RobotRaconteur;
    c->get_somethingHappened().connect(
        [](std::string what, double when)
        {
            std::cout << "something happened: " << what << std::endl;
        }
    );

### ObjRef Members {#cpp_client_objref}

ObjRef members are used to access other objects within a service.  See \ref service_paths for more information on objrefs and service paths. An example objref definition:

   objref MyOtherObject other_object

This objref definition results in the following function being generated in the C++ abstract interface:

    virtual MyOtherObjectPtr get_other_object();

The function name is the member name prefixed with `get_`.

An example using this objref:

    // Assume "c" is a connected client object reference with event "somethingHappened"
    
    using namespace RobotRaconteur;
    MyOtherObjectPtr obj2 = c->get_other_object();
    obj2->some_function();

ObjRefs may also be indexed by `int32` or `string`. The following are member definitions for `int32` indexed objref:

    objref MyOtherObject[] other_object1
    objref MyOtherObject{int32} other_object2

They result in the following functions being generated in the abstract interface:

    virtual MyOtherObjectPtr get_other_object1(int32_t index);
    virtual MyOtherObjectPtr get_other_object2(int32_t index);

The following is an objref definition for a `string` indexed objref:

    objref MyOtherObject{string} other_object3

It results in the following function being generated in the abstract interface:

    virtual MyOtherObjectPtr get_other_object3(const std::string& index);

ObjRefs may also be invoked asynchronously. Like properties, the asynchronous form is in the `async_` abstract interface. The asynchronous version of the objref is the member name prefixed with `async_get_`. An example of invoking an objref asynchronously:

    // Assume "c" is a connected synchronous client object
    using namespace example::my_service;
    boost::shared_ptr<async_MyObject> c_async = rr_cast<async_MyObject>(c);

    c_async->async_get_other_object2(12345,
        [](MyOtherObjectPtr obj, RobotRaconteurExceptionPtr err)
        {
            if (err)
            {
                std::cout << "Error occured invoking objref: " << err->what() << std::endl;
                return;
            }

            // Use the objref
            obj->async_some_function(
                [](RobotRaconteurExceptionPtr err)
                {
                    if (err)
                    {
                        std::cout << "Error occured invoking function: " << err->what() << std::endl;
                    }
                }
            )
        }
    );

### Pipe Members {#cpp_client_pipe}

Pipe members provide reliable (or optionally unreliable) data streams between clients and service, in either direction. See \ref RobotRaconteur::Pipe for a discussion of pipes.

An example pipe definition:

    pipe double[] sensordata

Results in the following functions being generated in the abstract interface:

    virtual PipePtr<RRArrayPtr<double>> get_sensordata();
    virtual void set_sensordata(PipePtr<RRArrayPtr<double>> pipe);

A get acessor prefixed with `get_` and a set accesor prefixed with `set_` are generated. These are used to get and set the pipe member object in the service. For the client, only the get accessor is used.

The `get_` accessor is used to retrieve the pipe so it can be used. An example of using a pipe client:

    // Assume "c" is a connected client object reference with pipe "sensordata"
    
    using namespace RobotRaconteur;
    PipePtr<RRArrayPtr<double>> sensordata = c->get_sensordata();
    PipeEndpointPtr<RRArrayPtr<double>> sensordata_ep = sensordata->Connect(-1);
    double data[] = {1.23, 4.56};
    sensordata_ep->SendPacket(AttachRRArrayCopy(data,2));
    RRArrayPtr<double> recv_data = sensordata_ep->ReceivePacketWait(100);
    std::cout << "Got recv_data len: " << recv_data->size() << std::endl;

Pipes can also be used asynchronously. Retrieving a pipe from the abstract interface never blocks since the pipe client exists locally. An example of using the pipe asynchronously:

    // Assume "c" is a connected client object reference with pipe "sensordata"
    
    using namespace RobotRaconteur;

    // Retrieve the pipe client. This never blocks.
    PipePtr<RRArrayPtr<double>> sensordata = c->get_sensordata();

    //Connect pipe asynchronously
    sensordata->AsyncConnect(-1, 
        [](PipeEndpointPtr<RRArrayPtr<double>> sensordata_ep, RobotRaconteurExceptionPtr err)
        {
            if (err)
            {
                std::cout << "Connecting pipe failed: " << err->what() << std::endl;
                return;
            }

            // Connect to signal to be notified when packets arrive
            sensordata_ep->PacketReceivedEvent.connect(
                [](PipeEndpointPtr<RRArrayPtr<double>> ep)
                {
                    while (ep->Available() > 0)
                    {
                        RRArrayPtr<double> recv_data = sensordata_ep->ReceivePacket();
                        std::cout << "Got recv_data len: " << recv_data->size() << std::endl;
                    }
                }
            )

            double data[] = {1.23, 4.56};

            // Asynchronously send packet
            sensordata_ep->AsyncSendPacket(AttachRRArrayCopy(data,2),
                [](RobotRaconteurExceptionPtr err)
                {
                    if (err)
                    {
                        std::cout << "Sending pipe packet failed: " << err->what() << std::endl;
                        return;
                    }

                    // Continue with the tasks
                }
            );

        }
    );

These example use only a few of the possible functions in RobotRaconteur::Pipe and RobotRaconteur::PipeEndpoint for the full API.

Pipes declared `readonly` may only receive packets on the client side. Pipes declared `writeonly` may only send packets on the client side.

### Callback Members {#cpp_client_callback}

Callbacks allow the service to invoke a function on a specific client. The definition is nearly identical to a `function` member, except the keyword is `callback` and generators are not supported. An example callback definition:

    callback double addTwoNumbersOnClient(int32 a, double b)

Results in the following functions being generated in the abstract interface:

    virtual CallbackPtr<boost::function<double (int32_t, double)> > get_addTwoNumbersOnClient();
    virtual void set_addTwoNumbersOnClient(CallbackPtr<boost::function<double (int32_t, double)> > callback);

A get acessor prefixed with `get_` and a set accesor prefixed with `set_` are generated. These are used to get and set the callback member object in the service. For the client, only the get accessor is used.

The calback member object is a RobotRaconteur::Callback used to set the callback function on the client, and get the callback proxy on the service.

An example using the callback client:

    // Assume "c" is a connected client object reference with callback "addTwoNumbersOnClient"
    
    using namespace RobotRaconteur;

    // Get the callback member object
    CallbackPtr<boost::function<double (int32_t, double)> > cb = c->get_addTwoNumbersOnClient();

    // Set the callback function
    c->SetClientFunction([](int32_t a, double b)
        {
            std::cout << "Service invoke callback with parameters " << a << "and" << b << std::endl;
            return (double)(a + b);
        }
    );

The RobotRaconteur::Callback object implementation does not support asynchronous operation.

### Wire Members {#cpp_client_wire}

Wire members provide a "most recent" values. They are typically used to communicate a real-time signal, such as a robot joint angle. See RobotRaconteur::Wire for a discussion of wires.

An example wire definition:

    wire double[2] currentposition

Results in the following functions being generated in the abstract interface:

    virtual WirePtr<RRArrayPtr<double>> get_currentposition();
    virtual void set_currentposition(WirePtr<RRArrayPtr<double>> wire);

A get acessor prefixed with `get_` and a set accesor prefixed with `set_` are generated. These are used to get and set the wire member object in the service. For the client, only the get accessor is used.

The `get_` accessor is used to retrieve the wire so it can be used. An example of using a wire client in streaming operation:

    // Assume "c" is a connected client object reference with wire "currentposition"
    
    using namespace RobotRaconteur;
    WirePtr<RRArrayPtr<double>> currentposition = c->currentposition();

    // Connect a WireConnection to receive streaming updates
    WireConnectionPtr<RRArrayPtr<double>> currentposition_cn = currentposition->Connect();
    double data[] = {1.23, 4.56};
    currentposition_cn->SetOutValue(AttachRRArrayCopy(data,2));
    currentposition_cn->WaitInValueValid(100);
    RRArrayPtr<double> in_value = currentposition_cn->GetInValue();
    std::cout << "Got in_value len: " << in_value->size() << std::endl;

    // Get the TimeSpec of the current InValue
    TimeSpec in_value_ts = currentposition_cn->GetLastValueReceivedTime();

Wire function `GetInValue()`, `SetOutValue()`, and `GetLastValueReceivedTime()` are inherently asynchronous, since they are intended to stream real-time data. In purely asynchronous code, `WaitInValueValid()` cannot be used. `TryGetInValue()` should be used instead to poll if a value is available.. Asynchronous code should also use `AsyncConnect()` instead of `Connect()` to connect the wire.

The above example uses a connected wire for streaming data. Sometimes the client only needs to read the `InValue` or set the `OutValue` instantaneously, not requiring a streaming connection. The "peek" and "poke" functions are provided for this purpose. They work like property "get" and "set", sending a request and receiving a response instead of passively sending packets periodically. See RobotRaconteur::Wire for a discussion on peek and poke.

An example of using peek and poke:

    // Assume "c" is a connected client object reference with wire "currentposition"
    
    using namespace RobotRaconteur;
    WirePtr<RRArrayPtr<double>> currentposition = c->currentposition();

    // Connect a WireConnection to receive streaming updates
    double data[] = {1.23, 4.56};
    currentposition_cn->PokeOutValue(AttachRRArrayCopy(data,2));
    TimeSpec in_value_ts;
    RRArrayPtr<double> in_value = currentposition_cn->PeekInValue(in_value_ts);
    std::cout << "Got in_value len: " << in_value->size() << std::endl;

The above example uses the synchronous functions `PeekInValue()` and `PokeOutValue()`. Asynchronous versions of these functions are also available. See RobotRaconteur::Wire.

Wires may be declared `readonly` or `writeonly`. Wires declared `readonly` wires may only use InValue on the client side. Wires declared `writeonly` may only use OutValue on the client side.

### Memory Members {#cpp_client_memory}

Memories are used to read and write a memory segment on the service. Memories may be numeric arrays, numeric multidimarrays, pod arrays, pod multidimarrays, namedarray arrays, or namedarray multidimarrays. The following table shows which memory class is used for each value and array type:

| Type | C++ Memory Class |
|---   | ---              |
| numeric array | RobotRaconteur::ArrayMemory |
| numeric multidimarray | RobotRaconteur::MultiDimArrayMemory |
| pod array | RobotRaconteur::PodArrayMemory |
| pod multidimarray | RobotRaconteur::PodMultiDimArrayMemory |
| namedarray array | RobotRaconteur::NamedArrayMemory |
| namedarray multidimarray | RobotRaconteur::NamedMultiDimArrayMemory |

All of the above C++ classes take a template of the scalar type used by the memory. See each individual class for more information.

A numeric array memory client and a numeric multidimarray memory client will be used as examples. Pod and namedarray memories are identical, except for the memory class and the value types being utilized.

Example array memory definition:

    memory double[] datahistory
    

Results in a single function being generated in the abstract interface:

    virtual ArrayMemoryPtr<double> get_datahistory();

The accessor function name is the member name prefixed with `get_`. On the client, this returns the array memory client. This accessor never blocks.

An example of using the array memory client:

    // Assume "c" is a connected client object reference with memory "datahistory"

    ArrayMemoryPtr<double> datahistory = c->get_datahistory();

    // Get the length of the memory
    uint64_t datahistory_len = datahistory->Length();

    // Read a segment of the memory into read_buf
    RRArrayPtr<double> read_buf = AllocateRRArray<double>(100);
    datahistory->Read(10, read_buf, 0, 100);

    // Write a portion of write_buf to segment of memory
    RRArrayPtr<double> write_buf = AllocateRRArray<double>(100);
    // TODO: fill write_buf with data
    datahistory->Write(5, write_buf, 15, 20);

Example array memory definition:

    memory double[*] datahistory2
    

Results in a single function being generated in the abstract interface:

    virtual MultiDimArrayMemoryPtr<double> get_datahistory2();

An example of using the array memory client:

    // Assume "c" is a connected client object reference with memory "datahistory2"

    MultiDimArrayMemoryPtr<double> datahistory2 = c->get_datahistory2();

    // Get the length of the memory
    std::vector<uint64_t> datahistory2_dims = datahistory2->Dimensions();

    // Read a segment of the memory into read_buf
    RRMultiDimArrayPtr<double> read_buf = AllocateEmptyRRMultiDimArray<double>({10,10});
    datahistory->Read({20,5}, read_buf, {0,0}, {10,10});

    // Write a portion of write_buf to segment of memory
    RRMultiDimArrayPtr<double> write_buf = AllocateEmptyRRMultiDimArray<double>({10,10});
    // TODO: fill write_buf with data
    datahistory->Write({5,10}, write_buf, {0,0}, {10,10});

The array memory clients do not support asynchronous operations.

It is recommended that clients aquire a monitor lock before memory operations. See \ref cpp_client_monitor_locks.

## Object Locking {#cpp_client_object_locking}

Robot Raconteur clients can request user, client, and monitor locks. These different lock types are discussed in \ref locking. These locks are activated and released using functions in RobotRaconteur::RobotRaconteurNode.

Locking normally requires the user to be authenticated, and for the user to have locking privileges.

### User Locks {#cpp_client_user_locks}

Users locks are requested using the RobotRaconteur::RobotRaconteurNode::RequestObjectLock() function with the `RobotRaconteurObjectLockFlags_USER_LOCK` flag specified. RobotRaconteur::ObjectLockedException will be thrown if the object is already locked. The lock is released using RobotRaconteur::RobotRaconteurNode::ReleaseObjectLock(). An example of user locks:

    // Assume "c" is a connected client object reference

    RobotRaconteurNode::s()->RequestObjectLock(c, RobotRaconteurObjectLockFlags_USER_LOCK);

    // If successful, the object is now locked for any session owned by the current user

    // When done with the lock, release it.

    RobotRaconteurNode::s()->ReleaseObjectLock(c);

Asynchronous versions of the lock and unlock functions are available. See RobotRaconteur::RobootRaconteurNode::AsyncRequestObjectLock() and RobotRaconteur::RobotRaconteurNode::AsyncReleaseObjectLock().

### Client Locks {#cpp_client_client_locks}

Client locks are requested using the RobotRaconteur::RobotRaconteurNode::RequestObjectLock() function with the `RobotRaconteurObjectLockFlags_CLIENT_LOCK` flag specified. RobotRaconteur::ObjectLockedException will be thrown if the object is already locked. The lock is released using RobotRaconteur::RobotRaconteurNode::ReleaseObjectLock(). An example of user locks:

    using namespace RobotRaconteur;
    
    // Assume "c" is a connected client object reference

    RobotRaconteurNode::s()->RequestObjectLock(c, RobotRaconteurObjectLockFlags_CLIENT_LOCK);

    // If successful, the object is now locked, only allowing the current session to use the object

    // When done with the lock, release it.

    RobotRaconteurNode::s()->ReleaseObjectLock(c);

Asynchronous versions of the lock and unlock functions are available. See RobotRaconteur::RobootRaconteurNode::AsyncRequestObjectLock() and RobotRaconteur::RobotRaconteurNode::AsyncReleaseObjectLock().

### Monitor Locks {#cpp_client_monitor_locks}

Monitor locks provide a single-threaded lock on the service object. There is no asynchronous version since it is inherently a threaded concept. Monitor locks are created using the RobotRaconteur::RobotRaconteurNode::MonitorEnter() function, and released using the RobotRaconteurNode::MonitorExit() function. A convenience scoped lock fence is available, RobotRaconteur::RobotRaconteurNode::ScopedMonitorLock. Its use is recommended. An example:

    using namespace RobotRaconteur;
    
    // Assume "c" is a connected client object reference

    {
        RobotRaconteurNode::ScopedMonitorLock monitor_lock(c,1000);

        // If successful, the object is now monitor locked.

    }

    // Lock is released when monitor_lock is destroyed
