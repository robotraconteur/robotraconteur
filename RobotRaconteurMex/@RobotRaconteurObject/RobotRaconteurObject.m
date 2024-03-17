function c=RobotRaconteurObject(objecttype,stubid)
%RobotRaconteurObject Robot Raconteur client connection object
%    Robot Raconteur returns RobotRaconteurObject instances from
%    RobotRaconteur.ConnectClient, objref members, and ServiceSubscription.
%    These instances are "object references", sometimes referred to as
%    "object proxies". These instances provide access to client members,
%    which access the service. See <a href="matlab:help RobotRaconteur">RobotRaconteur</a>
%    for more information on the architecture of Robot Raconteur and how
%    object references are used, <a href="matlab:help RobotRaconteurFramework">RobotRaconteurFramework</a>
%    for an overview of the Robot Raconteur Framework, and
%    <a href="https://github.com/robotraconteur/RobotRaconteur_MATLAB_Examples">Robot Raconteur Examples</a>
%    for examples using RobotRaconteurObject.
%
%    Robot Raconteur MATLAB dynamically creates object references on demand
%    based on the information provided by the service. The "thunk" source
%    is uneccesary because of this capability. Two helper functions, "type"
%    and "members" returns the Robot Raconteur type of the object and
%    prints out the definition of the object type, respectively:
%
%        obj=RobotRaconteur.ConnectService('rr+tcp://localhost:2342?service=Create')
%        type(obj) % Return the object type
%        members(obj) % Print out the object definition
%
%    Note that exceptions are passed transparently to and from MATLAB using
%    standard error and try/catch notation.
%
%    The RobotRaconteurObject class uses "old style" class functions for
%    helper functions, and "dot" notation for member access. Examples of a
%    helper function and a property member access:
%
%       obj_type = type(obj) % Old style class function for helper function
%       distance_traveled = obj.DistanceTraveled % "dot" style notation to access member
%
%    The following list describes the member types and gives a short
%    example of the usage. In all examples, obj and webobj are connected
%    objects. The obj is type "experimental.create2.Create", while
%    webobj is type "experimental.createwebcam2.WebcamHost".
%
%    property
%
%      Properties are implemented as standard MATLAB properties. They can
%      be accessed using "dot" notation
%
%        dist = obj.DistanceTraveled;
%        obj.DistanceTraveled=uint16(100);
%
%    function
%
%      Functions are implemented as standard MATLAB functions. They can be
%      accessed using "dot" notation
%
%        obj.Drive(int16(200),int16(1000));
%
%      Generator functions will return instances of RobotRaconteurGenerator.
%      Call RobotRaconteurGenerator.Next() to advance the generator, and
%      call RobotRaconteurGenerator.Close() to close the generator.
%      See <a href="matlab:help RobotRaconteurGenerator">RobotRaconteurGenerator</a>
%
%    event
%
%      In MATLAB, a callback function handle is set to be called when an
%      event occurs by using
%      the "addlistener" helper function. Events are disabled by default.
%      To enable events on the object,
%      run RobotRaconteur.EnableEvents(obj), where obj is the object. The
%      callback function must have the correct number of arguments matching
%      the event definition. Because MATLAB is single threaded it is
%      necessary to call the RobotRaconteur.ProcessRequests() command
%      to execute the event pump.
%
%        RobotRaconteur.EnableEvents(obj);
%        addlistener(c,'Bump',@Bump); % Bump is a function
%        RobotRaconteur.ProcessRequests(); %Run repeatedly to process events
%
%    objref
%
%      The objref members are implemented through a function that is named
%      "get_" pre-pended to the member name of the objref. The index is the
%      argument to the function if there is an index. This can be accessed
%      using the "dot" notation.
%
%        obj2 = webojb.get_Webcams(0);
%
%    pipe
%
%      Pipes have a special syntax that allows connecting
%      RobotRaconteurPipeEndpoint objects. Replace FrameStream with the
%      name of the pipe being connected. pe is the connected
%      RobotRaconteurPipeEndpoint. The -1 can be replaced with a desired
%      endpoind index, if desired. In most cases -1 is used to allow
%      the service to automatically select a pipe endpoint index.
%
%        pe = obj2.FrameStream.Connect(-1)
%        count = pe.Available
%        datin = pe.ReceivePacket()
%        pe.SendPacket(datout)
%
%      See <a href="matlab:help RobotRaconteurPipeEndpoint">RobotRaconteurPipeEndpoint</a>
%
%    callback
%
%      Callbacks in MATLAB are function handles that have parameters and
%      return types matching the member definition. (Return from the
%      function is ignored if the function member has void return.) "Dot"
%      notation is used to specify the function handle. Because MATLAB is
%      single threaded, it is necessary to pump the callbacks using the
%      RobotRaconteur.ProcessRequests() command.
%
%        obj.play_callback = @my_play_callback; % my_play_callback is a function
%        RobotRaconteur.ProcessRequests(); % Run repeatedly to process callbacks and events
%
%    wire
%
%      Wires have a special syntax that allows connecting
%      RobotRaconteurWireConnection objects. Replace packets with the name
%      of the wire being connected. wc is the connected
%      RobotRaconteurWireConnection.
%
%        wc=obj.frame.Connect()
%        wc.OutValue = myoutdat;
%        myindat = wc.InValue
%
%      See <a href="matlab:help RobotRaconteurWireConnection">RobotRaconteurWireConnection</a>
%
%      Wires can also be accessed synchronously using "peek" and "poke"
%      without forming a connection. This is useful if only the
%      instantaneous value is needed instead of constantly updated
%      streaming data. The following functions are available:
%
%        their_indat = obj.frame.PeekInValue(); % Peek the current InValue
%        their_outdat = obj.frame.PeekOutValue(); % Peek the current OutValue
%        obj.frame.PokeOutValue(my_outdat); % Poke a new OutValue
%
%    memory
%
%      Memories in MATLAB work by returning a special proxy object
%      of type RobotRaconteurMemoryClient. This object has much of the
%      same functionality as a normal MATLAB array. The main difference is
%      to read and write the full array, use the (:) index slice notation.
%      The memory must be stored in a variable before use.
%
%        buffer = obj2.buffer
%        buf_size = size(buffer); % Use standard "size" function on memory
%        buffer(:) = mydat1; % Assign entire buffer
%        mydat2 = buffer(:); % Read entire buffer
%
%      See <a href="matlab:help RobotRaconteurMemoryClient">RobotRaconteurMemoryClient</a>
%
%    Robot Raconteur objects also support some asynchronous capabilities.
%    See <a href="matlab:help RobotRaconteurAsyncOps">RobotRaconteurAsyncOps</a>
%
%    See also RobotRaconteur, RobotRaconteurFramework,
%        RobotRaconteurValueTypes, RobotRaconteurAsyncOps,
%        RobotRaconteurGenerator, RobotRaconteurPipeEndpoint,
%        RobotRaconteurWireConnection, RobotRaconteurMemoryClient
%
c=struct;
c.rrobjecttype=objecttype;
c.rrstubid=stubid;

c=class(c,'RobotRaconteurObject');
