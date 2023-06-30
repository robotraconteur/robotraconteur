classdef RobotRaconteur
    % Robot Raconteur: A communication framework for robotics, automation, and IoT
    %
    %   <a href="http://robotraconteur.com">Robot Raconteur Website</a>
    %   <a href="https://github.com/robotraconteur/robotraconteur">Robot Raconteur GitHub</a>
    %   <a href="https://www.mathworks.com/matlabcentral/fileexchange/80509-robot-raconteur-matlab">Robot Raconteur Matlab Toolbox</a>
    %
    %   Installation
    %
    %   The Robot Raconteur Matlab Toolbox contains the MATLAB implementation
    %   of the Robot Raconteur framework. The toolbox can be installed
    %   using the Add-On manager, by downloading from the MATLAB file
    %   exchange, or by downloading from the Robot Raconteur GitHub 
    %   Releases page. The prefered method to install is using the
    %   integrated Add-Ons manager. Click Add-Ons -> Search for 
    %   "Robot Raconteur" -> select "Robot Raconteur Matlab" ->
    %   click "Add from Github". Robot Raconteur will be automatically
    %   installed and added to the MATLAB path.
    %
    %   Introduction
    %
    %   See the following files for introductory material for Robot 
    %   Raconteur Matlab:
    %
    %   <a href="matlab:help RobotRaconteurFramework">RobotRaconteurFramework</a>
    %   <a href="https://github.com/robotraconteur/RobotRaconteur_MATLAB_Examples">Robot Raconteur Matlab Examples</a>
    %   <a href="matlab:help RobotRaconteurValueTypes">RobotRaconteurValueTypes</a>
    %   <a href="matlab:help RobotRaconteurObject">RobotRaconteurObject</a>
    %
    %   Global Functions
    %
    %   Robot Raconteur contains a number of global functions to interact
    %   with the local node. The most commonly used functions are:
    %
    %   <a href="matlab:help RobotRaconteur.ConnectService">RobotRaconteur.ConnectService</a> - Create a client connection to a service
    %   <a href="matlab:help RobotRaconteur.DisconnectService">RobotRaconteur.DisconnectService</a> - Close a client connection
    %   <a href="matlab:help RobotRaconteur.EnableEvents">RobotRaconteur.EnableEvents</a> - Enable events on a client object
    %   <a href="matlab:help RobotRaconteur.DisableEvents">RobotRaconteur.DisableEvents</a> - Disable events on a client object
    %   <a href="matlab:help RobotRaconteur.FindServiceByType">RobotRaconteur.FindServiceByType</a> - Find a service by type using discovery
    %   <a href="matlab:help RobotRaconteur.FindNodeByID">RobotRaconteur.FindNodeByID</a> - Find a node by NodeID using discovery
    %   <a href="matlab:help RobotRaconteur.FindNodeByName">RobotRaconteur.FindNodeByName</a> - Find a node by NodeName using discovery
    %   <a href="matlab:help RobotRaconteur.ProcessRequests">RobotRaconteur.ProcessRequests</a> - Process asynchronous ops such as events, callbacks, and async handlers
    %   <a href="matlab:help RobotRaconteur.RequestObjectLock">RobotRaconteur.RequestObjectLock</a> - Lock a client object
    %   <a href="matlab:help RobotRaconteur.ReleaseObjectLock">RobotRaconteur.ReleaseObjectLock</a> - Release a client object lock
    %   <a href="matlab:help RobotRaconteur.MonitorEnter">RobotRaconteur.MonitorEntor</a> - Create a monitor lock
    %   <a href="matlab:help RobotRaconteur.MonitorLock">RobotRaconteur.MonitorExit</a> - Release a monitor lock
    %   <a href="matlab:help RobotRaconteur.SubscribeServiceInfo2">RobotRaconteur.SubscribeServiceInfo2</a> - Create a ServiceInfo2Subscription
    %   <a href="matlab:help RobotRaconteur.SubscribeService">RobotRaconteur.SubscribeService</a> - Create a ServiceSubscription using a URL
    %   <a href="matlab:help RobotRaconteur.SubscribeServiceByType">RobotRaconteur.SubscribeServiceByType</a> - Create a ServiceSubscription to all services of a specified type
    %   <a href="matlab:help RobotRaconteur.SetLogLevel">RobotRaconteur.SetLogLevel</a> - Set the log level for Robot Raconteur
    %   <a href="matlab:help RobotRaconteur.OpenLogFile">RobotRaconteur.OpenLogFile</a> - Save log messages from Robot Raconteur to a file
    %
    %   An example of driving the Create robot from the tutorial:
    %
    %     o=RobotRaconteur.Connect('rr+tcp://localhost:52222/?service=Create');
    %     o.Drive(int16(100),int16(5000));
    %     pause(1);
    %     o.Drive(int16(0),int16(0));
    %
    %   See also RobotRaconteurFramework, RobotRaconteurAsyncOps,
    %     RobotRaconteurValueTypes, RobotRaconteurObject,
    %     RobotRaconteurGenerator, RobotRaconteurPipeEndpoint,
    %     RobotRaconteurWireConnection, RobotRaconteurMemoryClient,
    %     RobotRaconteurServiceInfo2Subscription, 
    %     RobotRaconteurServiceSubscription, RobotRaconteurPipeSubscription,
    %     RobotRaconteurWireSubscription, 
    
    methods
        function a=RobotRaconteur(c)
           valid_commands = {'ConnectService', 'DisconnectService', ...
               'EnableEvents', 'DisableEvents', 'FindServiceByType', ...
               'FindNodeByID', 'FindNodeByName', 'UpdateDetectedNodes', ...
               'GetDetectedNodes', 'ProcessRequests', 'RequestObjectLock', ...
               'ReleaseObjectLock', 'MonitorEnter', 'MonitorExit', ...
               'GetPulledServiceTypes', 'GetPulledServiceType', ...
               'nowUTC', 'Sleep', 'CreateRate', ...
               'SubscribeServiceByType', 'SubscribeServiceInfo2', ...
               'SubscribeService', 'GetServiceType', ...
               'GetRegisteredServiceTypes', 'RegisterServiceType', ...
               'StartLocalClient', 'StartLocalServer', 'StartTcpServer', ...
               'StartTcpServerPortSharer', 'GetTcpListenPort', ...
               'IsTcpPortSharerRunning', 'LoadTlsNodeCertificate', ...
               'IsTlsNodeCertificateLoaded', 'RegisterServer', ...
               'CloseService', 'ProcessServerRequests', 'Shutdown', ...
               'getTransactionTimeout', 'setTransactionTimeout', ...
               'getTransportTimeout', 'setTransportTimeout', ...
               'GetLogLevel', 'SetLogLevel', 'OpenLogFile', ...
               'CloseLogFile'};
           valid_commands2 = join(valid_commands, ', ');
           error(strcat('Valid commands are ', valid_commands2{1}));
        end
    end    
		
    methods(Static=true)
        
        function ret=Connect(url,username,credentials)
        % Connect Deprecated, use ConnectService
			if(nargin ==1)
				ret=RobotRaconteurMex('Connect',url); 
			else
				ret=RobotRaconteurMex('Connect',url,username,credentials); 
		    end
        end
		
		function obj=ConnectService(url,username,credentials)
        % RobotRaconteur.ConnectService Connect to a Robot Raconteur service.
        %    obj = ConnectService(url) Connect to a service at URL.
        %
        %    obj = ConnectService(url,username,credentials) Connect to a
        %    service at url with authentication.
        %
        %    Robot Raconteur connects to services using urls. These URLs
        %    specify the transport type to use, the connection information
        %    for the transport to connect to the remote node, optional node
        %    information including NodeID and/or NodeName, and the name of
        %    the service. Example URLs:
        %    
        %        rr+tcp://localhost:2354/?service=Create
        %        rr+tcp://localhost:2355/?service=Webcam
        %
        %    See <a href="matlab:help RobotRaconteurFramework">RobotRaconteurFramework</a> for more information on URLs
        %
        %    If the connection is successful, an object reference
        %    (sometimes called a proxy) is returned. This proxy can be used
        %    to communicate with the service. See <a href="matlab:help RobotRaconteurObject">RobotRaconteurObject</a>
        %    for more information on using the service.
        %
        %    Clients may also pass a username and credentials when
        %    connecting to authenticate with the service. The username is a
        %    string, while credentials is a containers.Map with string 
        %    keys. See examples for an authenticated connect example.
        %
        %    Client connections will be closed when MATLAB exits, or can be
        %    disconnected using RobotRaconteur.DisconnectService
        %
        %    Examples:
        %    
        %       % Connect to a service without authentication
        %       url = 'rr+tcp://localhost:2355?service=Webcam'
        %       c = RobotRaconteur.ConnectService(url)
        %
        %       % Do something with c
        %       RobotRaconteur.DisconnectService(c)
        %
        %
        %       % Connect and authenticate with password
        %       url = 'rr+tcp://localhost:2355?service=Webcam'
        %       username = 'my_username'
        %       credentials = containers.Map({'password'},{'my_password'})
        %       c = RobotRaconteur.ConnectService(url, username, password)
        %
        %    See also RobotRaconteur.DisconnectService
        
			if nargin ==1
				obj=RobotRaconteurMex('Connect',url); 
			else
				obj=RobotRaconteurMex('Connect',url,username,credentials); 
		    end
        end
        
        function Disconnect(objref)
        % Disconnect Deprecated, use DisconnectService
           disconnect(objref);
        end
        
		function DisconnectService(objref)
        % RobotRaconteur.DisconnectService Disconnect a Robot Raconteur
        % service.
        %    RobotRaconteur.DisconnectService(obj) Disconnect a previously
        %    connected service.
        %
        %    Example:
        %        
        %       url = 'rr+tcp://localhost:2355?service=Webcam'
        %       c = RobotRaconteur.ConnectService(url)
        %
        %       % Do something with c
        %       RobotRaconteur.DisconnectService(c)
        %
        %    See also RobotRaconteur.ConnectService
        
           disconnect(objref);
        end
		
        function EnableEvents(obj)
        % RobotRaconteur.EnableEvents Enable events for a Robot Raconteur
        % client object
        %    RobotRaconteur.EnableEvents(obj) Enable events for obj
        %
        %    By default, MATLAB will ignore events received from services.
        %    Use RobotRaconteur.EnableEvents to enable receiving events.
        %
        %    Use RobotRaconteurObject/addlistener to add an event listener
        %    and RobotRaconteur.ProcessRequests to process received events.
        %
        %    Example:
        %
        %       url = 'rr+tcp://localhost:2355?service=Webcam'
        %       c = RobotRaconteur.ConnectService(url)
        %     
        %       % Enable receiveng events 
        %       RobotRaconteur.EnableEvents(obj)
        %
        %       function Bump()
        %           disp('Bump!')
        %       end
        %
        %       % Add a listener
        %       addlistener(c,'Bump',@Bump)
        %
        %       % Process events
        %       disp('Waiting for events/callbacks')
        %       for i=1:100
        %           %Process any pending events or callbacks
        %           RobotRaconteur.ProcessRequests();
        %           pause(.1); 
        %       end
        %
        %   See also RobotRaconteur.ConnectService
        %   RobotRaconteur.DisableEvents RobotRaconteurObject/addlistener
        %   RobotRaconteur.ProcessRequests
        
           enableevents(obj);
        end
        
        function DisableEvents(obj)
        % RobotRaconteur.DisableEvents Disable events for a Robot Raconteur
        % client object
        %     RobotRaconteur.DisableEvents(obj) Disable events for obj
        %
        %     Disable events for an object previously enabled using
        %     RobotRaconteur.DisableEvents
        %
        %     See RobotRaconteur.EnableEvents for examples
        %
        % See also RobotRaconteur.EnableEvents
        % RobotRaconteur.ProcessRequests
        
           disableevents(obj);
        end
        
        function s=FindService(name)
        % FindService Deprecated, use RobotRaconteur.FindServiceByType
           s=RobotRaconteurMex('FindService',name); 
        end
        
		function s=FindServiceByType(typename)
        % RobotRaconteur.FindServiceByType Use discovery to find
        % services by the type of the root object
        %     discovered_services = FindServiceByType(typename) Find a 
        %     service with a root object that is typename or implements
        %     typename.
        %
        %     Robot Raconteur provides discovery to automatically find
        %     services on the local network. The FindServiceByType function
        %     uses discovery to find services that have a root object that
        %     is typename or implements typename. typename is a type
        %     defined in a service definition file. For example, the
        %     webcame service root object has the Robot Raconteur type
        %     'experimental.createwebcam2.WebcamHost'.
        %
        %     services is a cell array containing the detected services.
        %     Each service contains the fields Name, RootObjectType,
        %     RootObjectImplements, ConnectionURL, Attributes, NodeID, and
        %     NodeName. This metadata can be used to determine which
        %     service should be connected. Pass the contents of
        %     ConnectionURL to ConnectService to create a connection.
        %
        %     Example:
        %     
        %       % If the MATLAB toolbox was just loaded, update the detected
        %       % nodes
        %       RobotRaconter.UpdateDetectedNodes();
        %     
        %       % Find the available webcames
        %       services =
        %       RobotRaconteur.FindServiceByType('experimental.createwebcam2.WebcamHost');
        %
        %       % Make sure a service was detected
        %       assert(length(services) > 0, 'No services detected')
        %    
        %       % Connect to the first detected service
        %       c = RobotRaconteur.ConnectService(services{1}.ConnectionURL);
        %
        %       % c is now ready for use
        %
        %       % When done, disconnect service
        %       RobotRaconteur.DisconnectService(c);
        %
        %   See also RobotRaconteur.ConnectService
        %   RobotRaconteur.FindNodeByID RobotRaconteur.FindNodeByName
        
           s=RobotRaconteurMex('FindService',typename); 
        end
		
		function s=FindNodeByID(nodeid)
        % RobotRaconteur.FindNodeByID Use discovery to find a
        % node with a specified NodeID
        %     nodes = FindNodeByID(nodeid) Find nodes with nodeid
        %
        %     Note: This function is rarely used. If you are not sure why
        %     you need this function, you do not need it.
        %
        %     Robot Raconteur nodes have a NodeID and a NodeName. The
        %     NodeID is a unique 128-bit UUID. The NodeName is a human
        %     readable name that may not be unique. Use
        %     RobotRaconteur.FindNodeByID to find nodes on the local
        %     network that have a specific NodeID.
        %
        %     nodes is a cell array containing the detected nodes. Each
        %     node contains the fields NodeID, NodeName, ConnectionURL
        %
        %     Example:
        %
        %       % If the MATLAB toolbox was just loaded, update the detected
        %       % nodes
        %       RobotRaconter.UpdateDetectedNodes();
        %     
        %       % Find the available nodes
        %       nodes =
        %       RobotRaconteur.FindNodeByID('e1d5d1a9-906d-4d3c-95cb-e035fce95464');
        %
        %    See also RobotRaconteur.FindNodeByName
        %    RobotRaconteur.FindServiceByType
        
           s=RobotRaconteurMex('FindNodeByID',nodeid); 
        end
		
		function s=FindNodeByName(name)
        % RobotRaconteur.FindNodeByName Use discovery to find a
        % node with a specified NodeName
        %     nodes = FindNodeByName(nodename) Find nodes with name
        %
        %     Note: This function is rarely used. If you are not sure why
        %     you need this function, you do not need it.
        %
        %     Robot Raconteur nodes have a NodeID and a NodeName. The
        %     NodeID is a unique 128-bit UUID. The NodeName is a human
        %     readable name that may not be unique. Use
        %     RobotRaconteur.FindNodeByName to find nodes on the local
        %     network that have a specific NodeName.
        %
        %     nodes is a cell array containing the detected nodes. Each
        %     node contains the fields NodeID, NodeName, ConnectionURL
        %
        %     Example:
        %
        %       % If the MATLAB toolbox was just loaded, update the detected
        %       % nodes
        %       RobotRaconter.UpdateDetectedNodes();
        %     
        %       % Find the available nodes
        %       nodes =
        %       RobotRaconteur.FindNodeByName('experimental.createwebcam2');
        %
        %     See also RobotRaconteur.FindNodeByID
        %     RobotRaconteur.FindServiceByType
           s=RobotRaconteurMex('FindNodeByName',name); 
        end
		
		function UpdateDetectedNodes()
        % RobotRaconteur.UpdateDetectedNodes() Update the detected node
        % cache
        %
        %   When Robot Raconteur Matlab first starts up, it has not had
        %   time to detect nodes. Call
        %   RobotRaconteur.UpdateDetectedNodes(), or wait 10 seconds before
        %   performing discovery operations. If a node on the local network
        %   is not being detected, call
        %   RobotRaconteur.UpdateDetectedNodes() to refresh the local
        %   cache.
        %
        %   Example:
        %   
        %       RobotRaconteur.UpdateDiscoveredNodes()
        %       nodes = GetDetectedNodes()
        %
        %   See also RobotRaconteur.FindServiceByType
        %   RobotRaconteur.FindNodeByID RobotRaconteur.FindNodeByName
        %   RobotRaconteur.GetDetectedNodes
			RobotRaconteurMex('UpdateDetectedNodes');
		end
		
		function s=GetDetectedNodes()
        % RobotRaconteur.GetDetectedNodes Get the NodeIDs of the nodes
        % currently in the detected nodes cache
        %    nodes = RobotRaconteur.GetDetectedNodes() Get the detected
        %    nodes
        %
        %    Robot Raconteur mantains a cache of the nodes detected on the
        %    local network. When a discovery or subscription request
        %    occurrs, the nodes are interrogated to find what services are
        %    available. RobotRaconteur.GetDetectedNodes() returns the
        %    NodeID of the nodes currently in the detected nodes cache. Use
        %    RobotRaconteur.FindNodeById to return more information about a
        %    specific node.
        %
        %    Example:
        %
        %       RobotRaconteur.UpdateDetectedNodes();
        %
        %       % Get the detected nodes
        %       nodes = RobotRaconteur.GetDetectedNodes();
        %
        %       assert(length(nodes) > 0)
        %
        %       % Get information about a specific node
        %       node_info = RobotRaconteur.FindNodeByID(nodes{1});
        %
        %    See also RobotRaconteur.UpdateDetectedNodes
        %    RobotRaconteur.FindNodeByID
        
			s=RobotRaconteurMex('GetDetectedNodes');
		end
		
        function ProcessRequests()
        % RobotRaconteur.ProcessRequests Process events, callbacks, and
        % asynchronous handlers
        %
        %    MATLAB is a single threaded environment. This means that
        %    events coming from the service, callback requests, and 
        %    asynchronous operations, cannot be handled in the background. 
        %    MATLAB must use the main thread to process what would normally 
        %    be background operations. (See "Cooperative Multitasking" for
        %    more information.) For Robot Raconteur clients, call
        %    RobotRaconteur.ProcessRequests repeatedly to process these
        %    background operations, if they are being used.
        %
        %    See RobotRaconteur.EnableEvents for an example of using
        %    RobotRaconteur.ProcessRequests
        %
        %    See also RobotRaconteur.EnableEvents
        %    RobotRaconteurObject/addlistener
        
           RobotRaconteurMex('ProcessRequests');
        end
        
        function RequestObjectLock(obj,type)
        % RobotRaconteur.RequestObjectLock(obj,type) Request an
        % exclusive access lock to a service object. 
        %   RobotRaconteur.RequestObjectLock(obj,type) Called by clients to 
        %   request an exclusive lock on a service object and all 
        %   subobjects (objrefs) in the service. The exclusive
        %   access lock will prevent other users ("User" lock) or client
        %   connections ("Session" lock) lock from interacting with objects.
        %
        %   obj must be a object returned by RobotRaconteur.ConnectClient or
        %   an objref. type must be either 'User' or 'Client'. If type is
        %   ommitted, a 'User' lock in created.
        %
        %   Example:
        %
        %       url = 'rr+tcp://localhost:2354/?service=Create'
        %       c = RobotRaconteur.ConnectService(url)
        %
        %       % Request exclusive access to the robot from this client 
        %       % connection
        %       RobotRaconteur.RequestObjectLock(c, 'Client')
        %
        %       % Interact with the robot
        %       c.Drive(int16(100),int16(5000));
        %
        %       % Release the exclusive object lock
        %       RobotRaconteur.ReleaseObjectLock(c)
        %
        %   See also RobotRaconteur.ReleaseObjectLock
        %   RobotRaconteur.ConnectService
        
            if (nargin==1)
                type='User';
            end
            if (strcmp(type,'User'))
                lockop(obj,'RequestUserLock');
            elseif (strcmp(type,'Client'))
                lockop(obj,'RequestClientLock');
            else
                error('Unknown command')
            end
        end
        
        function ReleaseObjectLock(obj)
        % RobotRaconteur.ReleaseObjectLock(obj) Release a lock previously
        % created with RobotRaconteur.RequestObjectLock
        %
        %   Releases a previously created exclusive access lock. Locks are
        %   also released when client connections are closed, or when
        %   MATLAB is closed.
        %
        %   obj must be an object reference previously locked with
        %   RobotRaconteur.RequestObjectLock
        %
        %   See RobotRaconteur.RequestObjectLock for an example
        %
        %   See also RobotRaconteur.RequestObjectLock
        %   RobotRaconteur.ConnectService
        
            lockop(obj,'ReleaseUserLock');
        end
        
        function MonitorEnter(obj)
        % RobotRaconteur.MonitorEnter Creates a monitor lock on a 
        % specified object.
        %   RobotRaconteur.MonitorEnter(obj) Monitor locks are 
        %   intendended for short operations that require 
        %   guarding to prevent races, corruption, or other concurrency 
        %   problems. Monitors emulate a single thread locking the service 
        %   object.
        %
        %   obj must be a object returned by RobotRaconteur.ConnectClient 
        %   or an objref.
        %
        %   Example:
        %
        %       url = 'rr+tcp://localhost:2354/?service=Create'
        %       c = RobotRaconteur.ConnectService(url)
        %
        %       % Request a monitor lock
        %       RobotRaconteur.MonitorEnter(c)
        %
        %       % Read a potentially volatile value
        %       a = c.DistanceTraveled
        %
        %       % Release the monitor lock
        %       RobotRaconteur.MonitorExit(c)
        %
        %   See also RobotRaconteur.MonitorExit
        %   RobotRaconteur.ConnectService
        
           lockop(obj,'MonitorEnter'); 
        end
        
        function MonitorExit(obj)
        % RobotRaconteur.MonitorExit Release a monitor lock
        % created with RobotRaconteur.MonitorEnter
        %   RobotRaconteur.MonitorExit(obj)  Releases a previously created 
        %   monitor lock. Locks are also released when client connections 
        %   are closed, or when MATLAB is closed.
        %
        %   obj must be an object reference previously locked with
        %   RobotRaconteur.MonitorEnter
        %
        %   See RobotRaconteur.MonitorEnter for an example
        %
        %   See also RobotRaconteur.MonitorEnter
        %   RobotRaconteur.ConnectService
            lockop(obj,'MonitorExit'); 
        end
        
        function types=GetPulledServiceTypes(obj)
        % RobotRaconteur.GetPulledServiceTypes Get the names of 
        % the service definitions pulled by the client
        %    types=RobotRaconteur.GetPulledServiceTypes(obj)
        %    Robot Raconteur uses service definitions, also referred to as
        %    "service types", to define the data and object types used by a
        %    service. MATLAB clients "pull" these service definitions while
        %    forming a connection to the service. These service types are
        %    then used to enable "plug-and-play" operation. The
        %    RobotRaconteur.GetPulledServiceTypes function returns a cell
        %    array of strings containing the names of these pulled robdef
        %    types.
        %
        %    Example:
        %
        %       url = 'rr+tcp://localhost:2354/?service=Create'
        %       c = RobotRaconteur.ConnectService(url);
        %
        %       % Get the names of the pulled service types
        %       pulled_types = RobotRaconteur.GetPulledServiceTypes(c);
        %
        %       % For this service will print {'experimental.create2'}
        %       disp(pulled_types);
        %
        %       % Get and print the full text of the first pulled type
        %       robdef_text = RobotRaconteur.GetPulledServiceType(c,pulled_types{1});
        %       disp(robdef_text);
        %
        %   See also RobotRaconteur.GetPulledServiceType
        %   RobotRaconteur.ConnectService
        
           types=getpulledservicetypes(obj);
        end
        
        function robdef_text=GetPulledServiceType(obj,name)
        % types=RobotRaconteur.GetPulledServiceType(obj) Get a service
        %    definition pulled by the client
        %
        %    Robot Raconteur uses service definitions, also referred to as
        %    "service types", to define the data and object types used by a
        %    service. MATLAB clients "pull" these service definitions while
        %    forming a connection to the service. These service types are
        %    then used to enable "plug-and-play" operation. The
        %    RobotRaconteur.GetPulledServiceTypes function returns a cell
        %    array of strings containing the names of these pulled robdef
        %    types.
        %
        %    Example:
        %
        %       url = 'rr+tcp://localhost:2354/?service=Create'
        %       c = RobotRaconteur.ConnectService(url);
        %
        %       % Get and print the full text of the first pulled type
        %       robdef_text =
        %       RobotRaconteur.GetPulledServiceType(c,'experimental.create2');
        %       disp(robdef_text);
        %
        %   See also RobotRaconteur.GetPulledServiceTypes
        %   RobotRaconteur.ConnectService
        
           robdef_text=getpulledservicetype(obj,name);
        end

        function empty_structure=CreateStructure(obj,structure_type)
        % types=RobotRaconteur.CreateStructure(obj) Create a structure
        %    with fields set to default values
        %
        %    Create a new structure with fields set to default values.
        %    This is not required to be used, but is helpful to avoid
        %    errors when using structures with Robot Raconteur.
        %
        %    Example:
        %
        %    my_struct = RobotRaconteur.CreateStructure('experimental.my_definition.MyStructure'); 
            
            
               empty_structure=createstructure(obj,structure_type);
            end
        
		function t=nowUTC()
        % RobotRaconteur.nowUTC The current time in UTC time zone.
        %   t = RobotRaconteur.nowUTC Uses the internal node clock to get 
        %   the current time in UTC.  While this will normally use the 
        %   system clock, this may use simulation time in certain 
        %   circumstances
        %
        %   Example:
        %
        %       t = RobotRaconteur.nowUTC();
        %       disp(t);
        %
			ts=RobotRaconteurMex('NowUTC');
			t=datenum(datevec(ts,'yyyymmddTHHMMSS.FFF'));
		end
		
		function t=clockUTC()
        % clockUTC Deprecated, use nowUTC
			ts=RobotRaconteurMex('NowUTC');
			t=datevec(ts,'yyyymmddTHHMMSS.FFF');
		end

		function t=NowNodeTime()
			ts=RobotRaconteurMex('NowNodeTime');
			t=datenum(datevec(ts,'yyyymmddTHHMMSS.FFF'));
		end
		
		function t=NodeSyncTimeUTC()
			ts=RobotRaconteurMex('NodeSyncTimeUTC');
			t=datenum(datevec(ts,'yyyymmddTHHMMSS.FFF'));
		end

		function Sleep(duration)
        % RobotRaconteur.Sleep Sleeps for a specified duration.
        %   RobotRaconteur.Sleep(duration) Normally will sleep based on 
        %   the system clock, but in certain circumstances will 
        %   use simulation time. duration is the length of time to sleep
        %   in seconds.
        %
        %   Only use this function if the node may be using simulation
        %   time. Usee sleep() otherwise.
        %
        %   Example:
        %
        %       % Sleep for 500 ms
        %       RobotRaconteur.Sleep(0.5)
        %
        %   See also sleep RobotRaconteur.Rate
        
			RobotRaconteurMex('Sleep',double(duration));
		end
		
		function r=CreateRate(frequency)
        % r = RobotRaconteur.CreateRate Create a Rate with the specified
        % frequency
        %   r = RobotRaconteur.CreateRate(frequency) is used to create a
        %   Rate object that can be used to stability period loops with a
        %   specified frequency. The frequency will normally use the system
        %   clock, unless the node is using simulation time.
        %
        %   frequency is the Rate frequency in Hz
        %
        %   Example:
        %
        %       % Create the rate with a frequency of 10 Hz       
        %       r = RobotRacontur.CreateRate(10)
        %
        %       while true
        %           % Sleep on the rate to stabilize frequency
        %           r.Sleep()
        %
        %           % Do actions in the loop
        %           disp('loop!')
        %
        %  See also RobotRaconteurRate RobotRaconteur.Sleep
        
			r=RobotRaconteurRate(frequency);
		end
		
		function subscription=SubscribeServiceByType(service_types,filter)
        % RobotRaconteur.SubscribeServiceByType Subscribe to listen for 
        % available services and automatically connect.
        %   subscription = SubscribeServiceByType(service_types,filter) A 
        %   ServiceSubscription will track the availability of service 
        %   types and create connections when available.
        %
        %   service_types is a string or a cell array of strings of the
        %   service types, for example 'experimental.create2.Create'.
        %   
        %   filter is a struct containing the following fields:
        %
        %       'ServiceNames':     A cell array of strings containing the
        %                           allowed service names
        %       'TransportSchemes': A cell array of strings containing
        %                           allowed transport schemes
        %       'MaxConnections':   The number of maximum connections allowed
        %       'Predicate':        A callback function to check if a 
        %                           service should be connected
        %
        %   The returned ServiceSubscription object has a number of 
        %   functions available, including accessing object, pipe 
        %   subscriptions, and wire subscriptions. See
        %   <a href="matlab:help RobotRaconteurServiceSubscription">RobotRaconteurServiceSubscription</a> 
        %   for more information.
        %
        %   Example:
        %
        %       %Subscribe to the service
        %       service_type = 'experimental.create2.Create'
        %       sub = RobotRaconteur.SubscribeServiceByType(service_type);
        %       
        %       % Get the default client, waiting 5 seconds for the client
        %       % to be connected
        %       c = sub.GetDefaultClientWait(5);
        %       
        %       % Client c is now available for use
        %       c.Drive(int16(100),int16(5000));
        %
        %       % Close subscription. This will also close all client
        %       % connections
        %
        %       sub.Close();
        %
        %   See also RobotRaconteur.SubscribeService
        %   RobotRaconteur.SubscribeServiceInfo2 RobotRaconteurServiceInfo2Subscription
        
            if(nargin ==1)
				subscription=RobotRaconteurMex('SubscribeServiceByType',service_types); 
            else
				subscription=RobotRaconteurMex('SubscribeServiceByType',service_types,filter); 
            end
        end
		
        function ret=SubscribeServiceInfo2(service_types,filter)
        % RobotRaconteur.SubscribeServiceInfo2 Subscribe to listen for 
        % available services and return connection info and metadata.
        %   subscription = SubscribeServiceInfoByType(service_types,filter) 
        %   A ServiceInfo2Subscription will track the availability of 
        %   service types and provide connection information and metadata.
        %
        %   service_types is a string or a cell array of strings of the
        %   service types, for example 'experimental.create2.Create'.
        %   
        %   filter is a struct containing the following fields:
        %
        %       'ServiceNames':     A cell array of strings containing the
        %                           allowed service names
        %       'TransportSchemes': A cell array of strings containing
        %                           allowed transport schemes
        %       'MaxConnections':   The number of maximum connections allowed
        %       'Predicate':        A callback function to check if a 
        %                           service should be connected
        %
        %   The returned ServiceInfo2Subscription object allows the user
        %   to retreive the information about detected services.
        %
        %   Example:
        %
        %       %Subscribe to the service info
        %       service_type = 'experimental.create2.Create'
        %       sub = RobotRaconteur.SubscribeServiceInfo2(service_type);
        %       
        %       % Wait a few seconds for discovery to take place
        %       sleep(5)
        %       
        %       % Print out information about the first detected service
        %       % The return from GetDetectedServiceInfo2 has the same
        %       % format as RobotRaconteur.FindServiceByType
        %       service_infos = sub.GetDetectedServiceInfo2();
        %       disp(service_infos{1});
        %
        %       sub.Close();
        %
        %   See also RobotRaconteur.FindServiceByType
        %   RobotRaconteur.SubscribeService RobotRaconteurServiceInfo2Subscription
        
            if(nargin ==1)
				ret=RobotRaconteurMex('SubscribeServiceInfo2',service_types); 
            else
				ret=RobotRaconteurMex('SubscribeServiceInfo2',service_types,filter); 
            end
		end
		
		function subscription=SubscribeService(url,username,credentials)
        % RobotRaconteur.SubscribeService Subscribe to a service using one 
        % or more URL. Used to create robust connections to services
        %    subscription = SubscribeService(url,username,credentials) is
        %    used to create a subscription to a single service at a given
        %    URL. The subscription can be used to create a robust
        %    connection to a service, that will reconnect if the connection
        %    is lost. Wire and pipe subscriptions can be used to create
        %    robust connections to specific members. Use
        %    subscription.GetDefaultClient() or
        %    subscription.GetDefaultClientWait(t) to retrieve the client 
        %    object for use. Do not save this object, since it may cange if
        %    the subscription needs to create a new connection.
        %
        %    The arguments for RobotRaconteur.SubscribeService are the same
        %    as RobotRaconteur.ConnectService.
        %
        %    The returned ServiceSubscription object has a number of 
        %    functions available, including accessing object, pipe 
        %    subscriptions, and wire subscriptions. See 
        %    <a href="matlab:help RobotRaconteurServiceSubscription">RobotRaconteurServiceSubscription</a> for more information.
        %
        %    Example:
        %
        %       %Subscribe to the service
        %       url = 'rr+tcp://localhost:2354/?service=Create'
        %       sub = RobotRaconteur.SubscribeService(url);
        %       
        %       % Get the default client, waiting 5 seconds for the client
        %       % to be connected
        %       c = sub.GetDefaultClientWait(5);
        %       
        %       % Client c is now available for use
        %       c.Drive(int16(100),int16(5000));
        %
        %       % Close subscription. This will also close all client
        %       % connections
        %
        %       sub.Close();
        %
        %    See also RobotRaconteur.ConnectService, RobotRaconteur.SubscribeServiceByType,
        %        RobotRaconteurServiceSubscription
        %    
        
			if nargin ==1
				subscription=RobotRaconteurMex('SubscribeService',url); 
			else
				subscription=RobotRaconteurMex('SubscribeService',url,username,credentials); 
		    end
        end
        
		%Server Functions
		
		function n=GetServiceType(servicename)
        % Expert use only
			n=RobotRaconteurMex('GetServiceType',servicename);
		end
		
		function n=GetRegisteredServiceTypes()
        % Expert use only
			n=RobotRaconteurMex('GetRegisteredServiceTypes');
		end
		
		function RegisterServiceType(typestring)
        % Expert use only
			RobotRaconteurMex('RegisterServiceType',typestring);
		end
		
		function StartLocalClient(nodeid)
        % Expert use only
			RobotRaconteurMex('StartLocalClient',nodeid);			
        end
		
		function StartLocalServer(nodeid)
        % Expert use only
			RobotRaconteurMex('StartLocalServer',nodeid);			
        end
        
        function StartTcpServer(port)
        % Expert use only
           RobotRaconteurMex('StartTcpServer',int32(port)); 
        end
		
		function StartTcpServerUsingPortSharer()
        % Expert use only
			RobotRaconteurMex('StartTcpServerUsingPortSharer');
		end
		
		function port=GetTcpListenPort()
        % Expert use only
			port=RobotRaconteurMex('GetTcpListenPort');
		end
		
		function ret=IsTcpPortSharerRunning()
        % Expert use only
			ret=RobotRaconteurMex('IsTcpPortSharerRunning');
		end
		
		function LoadTlsNodeCertificate()
        % Expert use only
			RobotRaconteurMex('LoadTlsNodeCertificate');
		end
		
		function ret=IsTlsNodeCertificateLoaded()
        % Expert use only
			ret=RobotRaconteurMex('IsTlsNodeCertificateLoaded');
		end
						
		function RegisterService(name, type, obj, security)
        % Expert use only
			if (nargin==3)
				RobotRaconteurMex('RegisterService', name, type, obj);
			else
				RobotRaconteurMex('RegisterService', name, type, obj, security);
			end
		end
		
		function CloseService(name)
        % Expert use only
			RobotRaconteurMex('CloseService', name);
		
		end
		
		function ProcessServerRequests(timeout)
        % Expert use only
			if (nargin==0)
				RobotRaconteurMex('ProcessServerRequests',10);
			else
				if (timeout>10)
					timeout=10;
				end
				RobotRaconteurMex('ProcessServerRequests',timeout);
			end
		
		end
		
        function Shutdown()
        % RobotRaconteur.Shutdown Shuts down the Robot Raconteur node.
        %     The Robot Raconteur node is shut down on MATLAB exit or when
        %     the mex file is unloaded, so it is not normally necessary to
        %     call shutdown. Do not call unless specifically instructed.
        
           RobotRaconteurMex('Shutdown');
           clear RobotRaconteurMex
        end
		
		function timeout=getRequestTimeout()
        % RobotRaconteur.getRequestTimeout Get the request timeout
        % in seconds
        %   timeout = RobotRaconteur.GetRequestTimeout() Will return
        %   the request timeout in seconds. Requests are used by Robot
        %   Raconteur when a response is expected, for example during a
        %   property get/set, or a function call. Timeouts are used to
        %   prevent the request from hanging forever. Default timeout is 15
        %   seconds
        %
        %   Users do not normally need to adjust the request timeout
        
			timeout=RobotRaconteurMex('GetRequestTimeout');
		end
		
		function setRequestTimeout(timeout)
        % RobotRaconteur.setRequestTimeout Set the request timeout
        % in seconds
        %   RobotRaconteur.SetRequestTimeout(timeout) Will set
        %   the request timeout in seconds. Requests are used by Robot
        %   Raconteur when a response is expected, for example during a
        %   property get/set, or a function call. Timeouts are used to
        %   prevent the request from hanging forever. Default timeout is 15
        %   seconds.
        %
        %   Users do not normally need to adjust the request timeout
        
			RobotRaconteurMex('SetRequestTimeout',timeout);
		end
		
		function timeout=getTransportTimeout()
        % Expert use only
			timeout=RobotRaconteurMex('GetTransportTimeout');
		end
		
		function setTransportTimeout(timeout)
        % Expert use only
			RobotRaconteurMex('SetTransportTimeout',timeout);
		end

		function level=GetLogLevel()
        % RobotRaconteur.GetLogLevel Get the current lock level.
        %     level = RobotRaconteur.GetLogLevel() Robot Raconteur provides
        %     a detailed logging functionality with multiple log levels.
        %     Available log levels are 'DISABLE', 'FATAL', 'ERROR',
        %     'WARNING', 'INFO', 'DEBUG', and 'TRACE'.
        %
        %     Logs by default are sent to stderr on the system terminal. 
        %     If using Windows or a terminal is not available, use
        %     RobotRaconteur.OpenLogFile to save the log to a file.
        %
        %     See also RobotRaconteur.SetLogLevel
        %     RobotRaconteur.OpenLogFile
			level=RobotRaconteurMex('GetLogLevel');
		end

		function SetLogLevel(level)
        % RobotRaconteur.SetLogLevel Get the current lock level.
        %     RobotRaconteur.SetLogLevel(level) Robot Raconteur provides
        %     a detailed logging functionality with multiple log levels.
        %     Available log levels are 'DISABLE', 'FATAL', 'ERROR',
        %     'WARNING', 'INFO', 'DEBUG', and 'TRACE'. Use this function to
        %     change the log level.
        %
        %     Logs by default are sent to stderr on the system terminal. 
        %     If using Windows or a terminal is not available, use
        %     RobotRaconteur.OpenLogFile to save the log to a file.
        %
        %     Example:
        %
        %       RobotRaconteur.SetLogLevel('DEBUG');
        %
        %     See also RobotRaconteur.GetLogLevel
        %     RobotRaconteur.OpenLogFile
			RobotRaconteurMex('SetLogLevel',level);
		end
		
		function OpenLogFile(filename)
        % RobotRaconteur.OpenLogFile Open a log file and direct log
        % messages to file instead of stderr
        %    RobotRaconteur.OpenLogFile(filename) Opens a plain text file
        %    at filename, and directs log messages to the file. By default
        %    the logs are sent to stderr, but on Windows or when a terminal
        %    is not available, these log messages will not be shown. Save
        %    the log to a file so they can be reviewed.
        %
        %    Example:
        %
        %       % Open the log file in the current working directory
        %       RobotRaconteur.OpenLogFile('robotraconteur-log.txt');
        %
        %       % Increase the log level to show more information
        %       RobotRaconteur.SetLogLevel('DEBUG');
        %
        %   See also RobotRaconteur.CloseLogFile RobotRaconteur.SetLogLevel
        
			RobotRaconteurMex('OpenLogFile',filename);
		end

		function CloseLogFile()
        % RobotRaconteur.CloseLogFile Close the log file previously opened
        % with RobotRaconteur.OpenLogFile
        %
        %   Example:
        %
        %       % Open the log file in the current working directory
        %       RobotRaconteur.OpenLogFile('robotraconteur-log.txt');
        %
        %       % Do some activity that needs to be logged
        %
        %       RobotRaconteur.CloseLogFile()
        
			RobotRaconteurMex('CloseLogFile');
		end
	end
    
end

