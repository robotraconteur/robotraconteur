

%include "RobotRaconteurNode.i"

%extend RobotRaconteur::RobotRaconteurNode
{
public:
    void Init(size_t thread_count=20)
    {
    $self->SetDynamicServiceFactory(RR_MAKE_SHARED<RobotRaconteur::WrappedDynamicServiceFactory>());
    $self->SetThreadPoolFactory(RR_MAKE_SHARED<RobotRaconteur::PythonThreadPoolFactory>());

    if (thread_count!=20)
    {
        boost::shared_ptr<RobotRaconteur::ThreadPool> thread_pool=$self->GetThreadPoolFactory()->NewThreadPool($self->shared_from_this());
        thread_pool->SetThreadPoolCount(thread_count);
        $self->SetThreadPool(thread_pool);
        $self->GetThreadPool();
    }
    
    $self->Init();
    
    }

%pythoncode %{
def ConnectService(self,*args):
    """
    ConnectService(url,username=None,credentials=None,listener=None)
    Create a client connection to a remote service using a URL

    Synchronously creates a connection to a remote service using a URL. URLs are either provided by
    the service, or are determined using discovery functions such as FindServiceByType().
    This function is the primary way to create client connections.

    username and credentials can be used to specify authentication information. Credentials will
    often contain a \"password\" or token entry.

    The listener is a function that is called during various events. See ClientServiceListenerEventType
    constants for a description of the possible events.

    ConnectService will attempt to instantiate a client object reference (proxy) based on the type
    information provided by the service. The type information will contain the type of the object,
    and all the implemented types. The client will normally want a specific one of the implement types.
    Specify this desired type in objecttype to avoid future compatibility issues.

    :param url: The URL of the service to connect
    :type url: Union[str,List[str]]
    :param username: An optional username for authentication, None for no authentication
    :type username: str
    :param credentials: Optional credentials for authentication, None for no authentication
    :type credentials: Dict[str,Any]
    :param listener: Optional listener callback function
    :type listener: Callable[[object,int,Any],None]
    :return: The client object reference (proxy).
    """
    from .RobotRaconteurPythonUtil import PackMessageElement, WrappedClientServiceListenerDirector, InitStub
    args2=list(args)
    if (len(args) >= 3):
        if (args[1]==None): args2[1]=""
        args2[2]=PackMessageElement(args[2],"varvalue{string}",None,self).GetData()
    if (len(args)>=4):
        args2[3]=WrappedClientServiceListenerDirector(args[3])
        args2[3].__disown__()
    if (len(args2) > 4):
        args2=args2[0:4]
    res=self._ConnectService(*args2)
    stub=InitStub(res)
    if (len(args2)>=4):
        args2[3].stub=stub
    return stub

def AsyncConnectService(self, url, username, credentials, listener, handler, timeout=RR_TIMEOUT_INFINITE):
    """
    Asynchronously create a client connection to a remote service using a URL

    Same as ConnectService but returns asynchronously. See ConnectService() for more details
    on client connections.

    handler is called after the client connection succeeds are fails. On success, the object reference
    is returned and the exception is None. On failure, the returned object is None and the exception
    contains a subclass of Exception.

    If ``handler`` is None, returns an awaitable future.

    :param url: The URL of the service to connect
    :type url: Union[str,List[str]]
    :param username: A username for authentication, None for no authentication
    :type username: str
    :param credentials: credentials for authentication, None for no authentication
    :type credentials: Dict[str,Any]
    :param listener: A listener callback function, None for no listener
    :type listener: Callable[[object,int,Any],None]
    :param handler: A handler function to receive the object reference or an exception
    :type handler: Callable[[object,Exception],None]
    :param timeout: Timeout in seconds, or -1 for no timeout
    :type timeout: float
    """
    from .RobotRaconteurPythonUtil import PackMessageElement, WrappedClientServiceListenerDirector, AsyncStubReturnDirectorImpl, async_call, adjust_timeout

    if (username is None ): username=""
    if (credentials is not None): credentials=PackMessageElement(credentials,"varvalue{string}",None,self).GetData()
    listener2=None
    if (listener is not None):
        listener2=WrappedClientServiceListenerDirector(listener)
        listener2.__disown__()
    
    return async_call(self._AsyncConnectService,(url, username, credentials, listener2, "", adjust_timeout(timeout)), AsyncStubReturnDirectorImpl, handler)

def DisconnectService(self, stub):
    """
    Disconnects a client connection to a service

    Synchronously disconnects a client connection. Client connections
    are automatically closed by Shutdown(), so this function
    is optional. 

    :param obj: The root object of the service to disconnect
    """
    self._DisconnectService(stub.rrinnerstub)

def AsyncDisconnectService(self, stub, handler):
    """
    Asynchronously disconnects a client connection to a service

    Same as DisconnectService() but returns asynchronously.

    If ``handler`` is None, returns an awaitable future.

    :param obj: The root object of the client to disconnect
    :param handler: The handler to call when complete
    :type handler: Callable[[],None]
    """
    from .RobotRaconteurPythonUtil import async_call, AsyncVoidNoErrReturnDirectorImpl
    return async_call(self._AsyncDisconnectService,(stub.rrinnerstub,), AsyncVoidNoErrReturnDirectorImpl,handler,noerror=True)

@classproperty.classpropertyreadonly
def s(self):
    return RobotRaconteurNode._get_s()

def RegisterServiceType(self, d):
    """
    Register a service type

    If passed a string, the string will be parsed as a service definition file

    The service type will be verified. All imported types must already have
    been registered for verification to succeed. It is recommended that
    RegisterServiceTypes() be used for lists of service types.

    :param d: The service definition implementing the type to register
    :type d: Union[RobotRaconteur.ServiceDefinition,str]
    """
    self._RegisterServiceType(d)

def RegisterServiceTypes(self, d):
    """
    Register a list of service types

    If passed a string, the string will be parsed as a service definition file

    The service types will be verified. All imported types must be included in the list
    or already have	been registered for verification to succeed. 

    :param d: The service types implementing the types to register
    :type d: Union[List[RobotRaconteur.ServiceDefinition],List[str]]
    """
    self._RegisterServiceTypes(d)

def RegisterServiceTypeFromFile(self, file_name):
    """
    Register a service type from a file

    The file_name must point to a plain text \"robdef\" file. The file_name
    may leave off the \".robdef\" extension.

    The path in the environmental variable ``ROBOTRACONTEUR_ROBDEF_PATH`` will
    be searched if the file is not found in the working directory.

    The service type will be verified. All imported types must already have
    been registered for verification to succeed. It is recommended that
    RegisterServiceTypesFromFiles() be used for lists of service types.

    :param d: The filename of the service type to load
    :type d: str
    """
    from .RobotRaconteurPythonUtil import ReadServiceDefinitionFile
    d = ReadServiceDefinitionFile(file_name)
    self._RegisterServiceType(str(d))

def RegisterServiceTypesFromFiles(self, file_names, auto_import = False):
    """
    Register a list of service type from files

    The file names in file_names must point to plain text \"robdef\" files. The 
    file names may leave off the \".robdef\" extension.

    The path in the environmental variable ``ROBOTRACONTEUR_ROBDEF_PATH`` will
    be searched if a file is not found in the working directory.

    The service types will be verified. All imported types must already have
    been registered or included in the file_names list for verification to 
    succeed.

    If auto_import is True, missing service type files will be loaded.

    :param file_names: The filenames of the service types to load
    :type file_names: List[str]
    :param auto_import: If True, attempt to import missing service types
    :type auto_import: bool
    """
    from .RobotRaconteurPythonUtil import ReadServiceDefinitionFiles
    d = ReadServiceDefinitionFiles(file_names, auto_import)
    self._RegisterServiceTypes(d)
    
def GetServiceType(self, name):
    """
    Returns a previously registered service type

    :param name: The name of the service type to retrieve
    :type name: str
    :rtype: ServiceDefinition
    """
    return self._GetServiceType(name)

def GetRegisteredServiceTypes(self):
    """
    Return names of registered service types

    :rtype: List[str] 
    """
    return self._GetRegisteredServiceTypes()
    
def GetPulledServiceTypes(self,obj):
    """
    Get the names of service types pulled by a client

    Clients pull service definitions from services and create
    instances of ServiceFactory if a DynamicServiceFactory has 
    been configured. GetPulledServiceTypes returns a list of  the
    names of these pulled service types. Use GetPulledServiceType()
    to retrieve the ServiceFactory for a specific type.

    :param obj: Client object referenece returned by ConnectService() or AsyncConnectService()
    :return: The names of the pulled service types
    :rtype: List[str]
    """
    if (hasattr(obj,'rrinnerstub')):
        obj=obj.rrinnerstub
    return self._GetPulledServiceTypes(obj)
        
def GetPulledServiceType(self,obj,servicetype):
    """
    Get a ServiceFactory created from a service type pulled by a client

    Clients pull service definitions from services and create
    instances of ServiceFactory if a DynamicServiceFactory has 
    been configured. GetPulledServiceType() returns a
    generated ServiceFactory. Use GetPulledServiceTypes() to 
    return a list of available service types.

    :param obj: Client object referenece returned by ConnectService() or AsyncConnectService()
    :param servicetype: The name of the service type
    :type servicetype: str
    :return: The pulled service factory
    :rtype: RobotRaconteur.ServiceDefinition
    """
    if (hasattr(obj,'rrinnerstub')):
        obj=obj.rrinnerstub
    return self._GetPulledServiceType(obj,servicetype)

def GetServicePath(self,obj):
    """
    Get the ServicePath of a client object reference
    obj must be returned by ConnectService(), AsyncConnectService(),
    or an ``objref``

    :param obj: The object to query
    :return: The servicepath of the object
    :rtype: str
    """
    if (hasattr(obj,'rrinnerstub')):
        obj=obj.rrinnerstub
    return self._GetServicePath(obj)

def NewStructure(self,type,obj=None):
    """
    Returns a new Robot Raconteur structure with type ``structtype``

    :param structtype: The fully qualified type of the structure
    :type structtype: str
    :param obj: The client connection object reference to be used with this structure.  
      This is necessary because each client maintains type information.  A client must be provided from 
      which type information can be queried. For services this parameter is unnecessary.
    :return: The new structure instance
    """
    from .RobotRaconteurPythonUtil import NewStructure
    return NewStructure(type,obj,self)

def GetStructureType(self,type,obj=None):
    """
    Returns a constructor for Robot Raconteur structure with type ``structtype``

    :param structtype: The fully qualified type of the structure
    :type structtype: str
    :param obj: The client connection object reference to be used with this structure.  
      This is necessary because each client maintains type information.  A client must be provided from 
      which type information can be queried. For services this parameter is unnecessary.
    :return: The constructer for the structure type
    :rtype: Callable[[],<structtype>]
    """
    from .RobotRaconteurPythonUtil import GetStructureType
    return GetStructureType(type,obj,self)

def GetPodDType(self,type,obj=None):
    """
    Returns the numpy dtype for ``podtype``

    Use with numpy.array() or numpy.zeros() to create new pod arrays.

    :param type: The fully qualified type of the pod
    :type type: str
    :param obj: The client connection object reference to be used with this pod type.  
      This is necessary because each client maintains type information.  A client must be provided from 
      which type information can be queried. For services this parameter is unnecessary.
    :return: The dtype for the specified podtype
    :rtype: numpy.dtype
    """
    from .RobotRaconteurPythonUtil import GetPodDType
    return GetPodDType(type,obj,self)

def GetNamedArrayDType(self,type,obj=None):
    """
    Returns the numpy dtype for ``namedarraytype``

    Use with numpy.array() or numpy.zeros() to create new namedarray arrays.

    :param type: The fully qualified type of the namedarray
    :type type: str
    :param obj: The client connection object reference to be used with this namedarray type.  
        This is necessary because each client maintains type information.  A client must be provided from 
        which type information can be queried. For services this parameter is unnecessary.
    :return: The dtype for the specified namedarray
    :rtype: numpy.dtype
    """
    from .RobotRaconteurPythonUtil import GetNamedArrayDType
    return GetNamedArrayDType(type,obj,self)
    
def NamedArrayToArray(self,named_array):
    """
    Converts a namedarray type into a primitive array with the namedarray numeric type. 
    This function will return an array with one more dimension than the input array, 
    with the first dimension set to the element count of the named array.
    
    :param namedarray: The namedarray to convert stored in a ``numpy.ndarray``
    :type namedarray: numpy.ndarray
    :return: The converted numeric array
    :rtype: numpy.ndarray
    """
    from .RobotRaconteurPythonUtil import NamedArrayToArray
    return NamedArrayToArray(named_array)

def ArrayToNamedArray(self,a,named_array_dt):
    """
    Converts a numeric array into a namedarray. The type of the namedarray is 
    specified using ``dt``, which is returned from GetNamedArrayDType().
    The input numeric array must have the correct numeric type, and the first dimension 
    must match the element count of the namedarray. The output array will 
    have one fewer dimensions than the input array.

    :param a: The numeric array to convert
    :type a: numpy.ndarray
    :param dt: The data type of the desired namedarray type
    :type dt: numpy.dtype
    :return: The converted namedarray
    :rtype: numpy.ndarray
    """
    from .RobotRaconteurPythonUtil import ArrayToNamedArray
    return ArrayToNamedArray(a,named_array_dt)

class ScopedMonitorLock(object):
    """
    Wrapper for RobotRaconteurNode.MonitorEnter() and 
    RobotRaconteurNode.MonitorExit() supporting ``with``
    statement scoping
    """
    def __init__(self,obj,timeout=-1):
        """
        Create a monitor lock for the specified object
 
        Creates a monitor lock by calling RobotRaconteur.MonitorEnter().
        Object will be locked once the scoped lock is created.

        :param obj: The object to monitor lock
        :param timeout: The timeout in seconds to acquire the monitor lock, 
           or -1 for infinite
        :type timeout: float
        """
        self.obj=obj
        self.timeout=timeout
        self.node=obj.rrinnerstub.RRGetNode()
        self.locked=False
    def __enter__(self):
        self.node.MonitorEnter(self.obj,self.timeout)
        self.locked=True
    def __exit__(self,type, value, traceback):
        self.node.MonitorExit(self.obj)
        self.locked=False
    def lock(self,timeout):
        """
        Relock the object after calling unlock()

        :param timeou:t The timeout in seconds to acquire the monitor lock, 
         or -1 for infinite
        :type timeout: float
        """
        self.node.MonitorEnter(self.obj,timeout)
        self.locked=True
    def unlock(self):
        """
        Releases the monitor lock

        The ScopedMonitorLock destructor will release
        the lock automatically, so in most cases it is
        not necessary to call this function
        """
        self.node.MonitorExit(self.obj,)
        self.locked=False
    def release(self):
        """
        Release the monitor lock from the class

        The monitor lock is released from the ScopedMonitorLock
        instance. The monitor lock will not be released
        when the ``with`` block exits. 
        """
        self.obj=None

def RequestObjectLock(self,obj,flags):
    """
    Request an exclusive access lock to a service object

    Called by clients to request an exclusive lock on a service object and
    all subobjects (\"objrefs\") in the service. The exclusive access lock will
    prevent other users (\"User\" lock) or client connections  (\"Session\" lock) 
    from interacting with the objects.

    :param obj: The object to lock. Must be returned by ConnectService or returned by an \"objref\"
    :param flags: Select either a \"User\" lock with RobotRaconteur.RobotRaconteurObjectLockFlags_USER_LOCK or \"Session\" 
      lock with RobotRaconteur.RobotRaconteurObjectLockFlags_CLIENT_LOCK
    :return: \"OK\" on success
    :rtype: str
    """
    return self._RequestObjectLock(obj.rrinnerstub,flags)
        
def ReleaseObjectLock(self,obj):
    """
    Release an excluse access lock previously locked with RequestObjectLock()
    or AsyncRequestObjectLock()

    Object must have previously been locked using RequestObjectLock() or
    AsyncRequestObjectLock()

    :param obj: The object previously locked
    :return: \"OK\" on success
    :rtype: str
    """
    return self._ReleaseObjectLock(obj.rrinnerstub)
        
def MonitorEnter(self,obj,timeout=-1):
    """
    Creates a monitor lock on a specified object

    Monitor locks are intendended for short operations that require
    guarding to prevent races, corruption, or other concurrency problems.
    Monitors emulate a single thread locking the service object.

    Use of ScopedMonitorLock instead of this function is highly recommended
    
    Monitor locks do not lock any sub-objects (objref)

    :param: obj The object to lock
    :param timeout: The timeout in seconds to acquire the monitor lock, 
        or -1 for infinite
    """
    from .RobotRaconteurPythonUtil import adjust_timeout	
    self._MonitorEnter(obj.rrinnerstub,adjust_timeout(timeout))
            
def MonitorExit(self,obj):
    """
    Releases a monitor lock

    Use of ScopedMonitorLock instead of this function is highly recommended
    

    :param obj: The object previously locked by MonitorEnter()
    """
    #obj.rrlock.release()
    self._MonitorExit(obj.rrinnerstub)

def AsyncRequestObjectLock(self,obj,flags,handler,timeout=RR_TIMEOUT_INFINITE):
    """
    Asynchronously request an exclusive access lock to a service object

    Same as RequestObjectLock() but returns asynchronously

    If ``handler`` is None, returns an awaitable future.

    :param obj: The object to lock. Must be returned by ConnectService or returned by an \"objref\"
    :param flags: Select either a \"User\" lock with RobotRaconteur.RobotRaconteurObjectLockFlags_USER_LOCK or \"Session\" 
      lock with RobotRaconteur.RobotRaconteurObjectLockFlags_CLIENT_LOCK
    :param handler: Handler to call on completion
    :type handler: Callable[[str,Exception],None]
    :param timeout: Timeout in seconds, or -1 for infinite
    :type timeout: float
    """
    from .RobotRaconteurPythonUtil import async_call, adjust_timeout, AsyncStringReturnDirectorImpl
    return async_call(self._AsyncRequestObjectLock,(obj.rrinnerstub,flags,adjust_timeout(timeout)),AsyncStringReturnDirectorImpl,handler)
                
def AsyncReleaseObjectLock(self,obj,handler,timeout=RR_TIMEOUT_INFINITE): 
    """
    Asynchronously release an excluse access lock previously locked 
    with RequestObjectLock() or AsyncRequestObjectLock()

    Same as ReleaseObjectLock() but returns asynchronously

    If ``handler`` is None, returns an awaitable future.

    :param obj: The object previously locked
    :param handler: Handler to call on completion
    :type handler: Callable[[str,Exception],None]
    :param timeout: Timeout in seconds, or -1 for infinite
    :type timout: float
    """
    from .RobotRaconteurPythonUtil import async_call, adjust_timeout, AsyncStringReturnDirectorImpl
    return async_call(self._AsyncReleaseObjectLock,(obj.rrinnerstub,adjust_timeout(timeout)),AsyncStringReturnDirectorImpl,handler)

def GetServiceAttributes(self,obj):
    """
    Get the service attributes of a client connection

    Returns the service attributes of a client connected using 
    ConnectService()

    :param obj: The root object of the client to use to retrieve service attributes
    :return: The service attributes
    :rtype: Dict[str,Any]
    """
    from .RobotRaconteurPythonUtil import UnpackMessageElement
    return UnpackMessageElement(self._GetServiceAttributes(obj.rrinnerstub),"varvalue{string} value",None,self)

def GetServiceNodeID(self,obj):
    """
    Get the service NodeID of the remote node from a client connection

    Returns the NodeID of the remote node that a client is connected

    :param obj: The root object of the client to use to retrieve service attributes
    :return: The NodeID
    :rtype: NodeID
    """
    return self._GetServiceNodeID(obj.rrinnerstub)

def GetServiceNodeName(self,obj):
    """
    Get the service NodeName of the remote node from a client connection

    Returns the NodeName of the remote node that a client is connected

    :param obj: The root object of the client to use to retrieve service attributes
    :return: The NodeName
    :rtype: str
    """
    return self._GetServiceNodeName(obj.rrinnerstub)

def GetServiceName(self,obj):
    """
    Get the name of a service from a client connection

    Returns the service name of the remote service that a client is connected

    :param: obj The root object of the client to use to retrieve service attributes
    :return: The service name
    :rtype: str
    """
    return self._GetServiceName(obj.rrinnerstub)

def GetObjectServicePath(self,obj):
    """
    Get the service path of a client object

    :param obj: The object to get the service path for
    :return: The object's service path
    :rtype: str
    """
    return self._GetObjectServicePath(obj.rrinnerstub)
    
def RegisterService(self, name, objecttype, obj, securitypolicy=None):
    """
    Registers a service for clients to connect

    The supplied object becomes the root object in the service. Other objects may
    be accessed by clients using \"objref\" members. The name of the service must conform
    to the naming rules of Robot Raconteur member names. A service is closed using
    either CloseService() or when Shutdown() is called.

    Multiple services can be registered within the same node. Service names
    within a single node must be unique.

    :param name: The name of the service, must follow member naming rules
    :type name: str
    :param objecttype: The fully qualified Robot Raconteur type of the object
    :param obj: The root object of the service
    :param securitypolicy: An optional security policy for the service to control authentication
      and other security functions
    :type securitypolicy: ServiceSecurityPolicy
    :return: The instantiated ServerContext. This object is owned
      by the node and the return can be safely ignored.
    :rtype: ServerContext
    """
    from .RobotRaconteurPythonUtil import WrappedServiceSkelDirectorPython, SplitQualifiedName
    director=WrappedServiceSkelDirectorPython(obj)
    rrobj=WrappedRRObject(objecttype,director,0)
    director.__disown__()
    return self._RegisterService(name,SplitQualifiedName(objecttype)[0],rrobj,securitypolicy)

NodeID = property(lambda self: self._NodeID(), lambda self,nodeid: self._SetNodeID(nodeid), 
    doc = """
    (RobotRaconteur.NodeID) The current NodeID. If one has not been set, one will be automatically generated
    when read. NodeID cannot be set after it has been configured.	
    """)
NodeName =property(lambda self: self._NodeName(), lambda self,nodename: self._SetNodeName(nodename),
    doc = """
    (str) The current NodeName. If one has not been set, it will be the empty string. Cannot be set after it has been configured.	
    """)
ThreadPoolCount = property(lambda self: self._GetThreadPoolCount(), lambda self,c: self._SetThreadPoolCount(c),
    doc = """
    (int) The size of the native thread pool. May be configured dynamically.
    """)
    
RequestTimeout = property(lambda self : self._GetRequestTimeout()/1000.0, lambda self,t : self._SetRequestTimeout(t*1000),
    doc = """
    (float) The timeout for requests in seconds

    Requests are calls to a remote node that expect a response. \"function\",
    \"property\", \"callback\", \"memory\", and setup calls in \"pipe\" and \"wire\"
    are all requests. All other Robot Raconteur functions that call the remote
    node and expect a response are requests. Default timeout is 15 seconds.
    """)
TransportInactivityTimeout = property(lambda self : self._GetTransportInactivityTimeout()/1000.0, lambda self,t : self._SetTransportInactivityTimeout(t*1000),
    doc = """
    (float) The timeout for transport activity in seconds

    Sets a timeout for transport inactivity. If no message
    is sent or received on the transport for the specified time,
    the transport is closed. Default timeout is 10 minutes.
    """)
EndpointInactivityTimeout = property(lambda self : self._GetEndpointInactivityTimeout()/1000.0, lambda self,t : self._SetEndpointInactivityTimeout(t*1000),
    doc = """
    (float) The timeout for endpoint activity in seconds

    Sets a timeout for endpoint inactivity. If no message
    is sent or received by the endpoint for the specified time,
    the endpoint is closed. Default timeout is 10 minutes.
    """
)
MemoryMaxTransferSize = property(lambda self: self._GetMemoryMaxTransferSize(), lambda self,m: self._SetMemoryMaxTransferSize(m),
    doc = """
    (int) The maximum chunk size for memory transfers in bytes

    \"memory\" members break up large transfers into chunks to avoid
    sending messages larger than the transport maximum, which is normally
    approximately 10 MB. The memory max transfer size is the largest
    data chunk the memory will send, in bytes. Default is 100 kB.
    """
)
NodeDiscoveryMaxCacheCount = property(lambda self: self._GetNodeDiscoveryMaxCacheCount(), lambda self,c: self._SetNodeDiscoveryMaxCacheCount(c),
    doc = """
    the maximum number of detected nodes that will be cached

    The node keeps a cache of detected nodes. The NodeDiscoveryMaxCacheCount
    sets an upper limit to how many detected nodes are cached. By default set to 4096
    """
)

def GetConstants(self,servicetype, obj=None):
    """
    Returns a structure that contains the constants of the specified service type

    :param servicetype: The name of the service definition
    :type servicetype: str
    :param obj: The client connection object reference to be used with to retrive service types.  
            This is necessary because each client maintains type information.  A client must be provided from 
            which type information can be queried. For services this parameter is unnecessary.
    :rtype: Dict[str,Any]
    """
    from .RobotRaconteurPythonUtil import ServiceDefinitionConstants
    if obj is None:
        d=self.GetServiceType(servicetype)
    else:
        d=self.GetPulledServiceType(obj,servicetype)
    return ServiceDefinitionConstants(d,self,obj)
        
def GetExceptionType(self, exceptionname, obj=None):
    """
    Returns a reference to the exception class of fully qualified type ``exceptiontype``. 
    Note that this is a class reference, not an instance.

    :param exceptionname: The fully qualified name of the exception type
    :type exceptionname: str
    :param obj: The client connection object reference to be used with to retrive service types.  
            This is necessary because each client maintains type information.  A client must be provided from 
            which type information can be queried. For services this parameter is unnecessary.
    :return: Exception type
    """
    from .RobotRaconteurPythonUtil import SplitQualifiedName
    from .RobotRaconteurPythonError import GetExceptionType
    t=SplitQualifiedName(exceptionname)
    if (obj is None):
        d=self.GetServiceType(t[0])
    else:
        d=self.GetPulledServiceType(obj,t[0])
    if (not any(x.Name == t[1] for x in d.Exceptions)): 
        raise Exception('Invalid exception type')
    return GetExceptionType(exceptionname)
        
def FindObjectType(self,obj,member,ind=None):
    """
    Returns the fully qualified object type that would be returned by an \"objref\" member

    :param obj: The object with the desired \"objref\"
    :param objref: The name of the \"objref\" member
    :type objref: str
    :param ind: The index for the \"objref\"
    :type ind: Union[int,str]
    :return: The fully qualified object type
    :rtype: str
    """
    if (ind is None):
        return self._FindObjectType(obj.rrinnerstub,member)
    else:
        return self._FindObjectType(obj.rrinnerstub,member,ind)

def AsyncFindObjectType(self,obj,member,handler,timeout=RR_TIMEOUT_INFINITE):
    """
    Asynchronously returns an objref as a specific type

    Same as FindObjectType() but returns asynchronously

    If ``handler`` is None, returns an awaitable future.

    :param obj: The object with the desired \"objref\"
    :param objref: The name of the \"objref\" member
    :type objref: str
    :param handler: A handler function to receive the object type or an exception
    :type handler: Callable[[str,Exception],None]
    :param timeout: Timeout is milliseconds, or -1 for infinite
    :type timeout: float
    """
    from .RobotRaconteurPythonUtil import async_call, adjust_timeout, AsyncStringReturnDirectorImpl
    return async_call(self._AsyncFindObjectType,(obj.rrinnerstub,member,adjust_timeout(timeout)),AsyncStringReturnDirectorImpl,handler)
            
def AsyncFindObjectTypeInd(self,obj,member,ind,handler,timeout=RR_TIMEOUT_INFINITE):
    """
    Asynchronously returns an objref as a specific type

    Same as FindObjectType() but returns asynchronously

    If ``handler`` is None, returns an awaitable future.

    :param obj: The object with the desired \"objref\"
    :param objref: The name of the \"objref\" member
    :type objref: str
    :param ind: The index for the \"objref\"
    :type ind: Union[int,str]
    :param handler: A handler function to receive the object type or an exception
    :type handler: Callable[[str,Exception],None]
    :param timeout: Timeout is milliseconds, or -1 for infinite
    :type timeout: float
    """
    from .RobotRaconteurPythonUtil import async_call, adjust_timeout, AsyncStringReturnDirectorImpl
    return async_call(self._AsyncFindObjectType,(obj.rrinnerstub,member,ind,adjust_timeout(timeout)),AsyncStringReturnDirectorImpl,handler)


def SetExceptionHandler(self, handler):
    """
    Set an exception handler function

    The ThreadPool will catch exceptions that are uncaught
    by other logic and pass the exception to the specified
    exception handler. User handler functions that throw exceptions
    will also be caught and passed to the specified handler
    function

    :param handler: The handler function for uncaught exceptions
    :type handler: Callable[[Exception],None]
    """
    from .RobotRaconteurPythonUtil import ExceptionHandlerDirectorImpl
    if (handler is None):
        self._ClearExceptionHandler()
    else:
        d=ExceptionHandlerDirectorImpl(handler)
        d.__disown__()
        self._SetExceptionHandler(d,0)


def CreateTimer(self,period,handler,oneshot=False):
    """
    Create a Timer object

    This function will normally return a WallTimer instance

    Start() must be called after timer creation

    :param period: The period of the timer in seconds
    :type period: float
    :param handler: The handler function to call when timer times out
    :type handler: Callable[[TimerEvent],None]
    :param oneshot: True if timer is a one-shot timer, False for repeated timer
    :type oneshot: bool
    :return: The new Timer object. Must call Start()
    :rtype: RobotRaconteur.Timer
    """
    from .RobotRaconteurPythonUtil import AsyncTimerEventReturnDirectorImpl
    
    handler2=AsyncTimerEventReturnDirectorImpl(handler)
    handler2.__disown__()
    ret= self._CreateTimer(period,oneshot,handler2,0)
        
    return ret
        
def PostToThreadPool(self, handler):
    """
    Posts a function to be called by the node's native thread pool.

    :param handler: The function to call
    :type handler: Callable[[],None]
    """
    from .RobotRaconteurPythonUtil import async_call, AsyncVoidNoErrReturnDirectorImpl
    return async_call(self._PostToThreadPool,(), AsyncVoidNoErrReturnDirectorImpl,handler,noerror=True)
    
RobotRaconteurVersion = property(lambda self: self._GetRobotRaconteurVersion(),
    doc = """(str) Returns the Robot Raconteur library version"""
)

def NowUTC(self):
    """
    The current time in UTC time zone

    Uses the internal node clock to get the current time in UTC.
    While this will normally use the system clock, this may
    use simulation time in certain circumstances

    :return: The current time in UTC
    :rtype: datetime.DateTime
    """
    return self._NowUTC()

def NowTimeSpec(self):
    """
    The current time node time as a TimeSpec

    The current node time as a TimeSpec. See NowNodeTime()

    :return: The current node time as a TimeSpec
    :rtype: RobotRaconteur.TimeSpec
    """
    return self._NowTimeSpec()

def NowNodeTime(self):
    """
    The current node time

    UTC time is not monotonic, due to the introduction of leap-seconds, and the possibility
    of the system clock being updated by the user. For a real-time systems, 
	this is unaccetpable and can lead to system instability. The "node time" used by Robot Raconteur
	is synchronized to UTC at startup, and is then steadily increasing from that initial time.
	It will ignore changes to the system clock, and will also ignore corrections like leap
	seconds.

    :return: The current node time
    :rtype: datetime.DateTime
    """
    return self._NowNodeTime()

def NodeSyncTimeUTC(self):
    """
    The sync time of the node as a TimeSpec

    See NodeSyncTimeUTC()

    :return: The node sync time as a TimeSpec
    :rtype: datetime.DateTime
    """
    return self._NodeSyncTimeUTC()

def NodeSyncTimeSpec(self):
    """
    The sync time of the node as a TimeSpec

    See NodeSyncTimeUTC()

    :return: The node sync time as a TimeSpec
    :rtype: RobotRaconteur.TimeSpec
    """
    return self._NodeSyncTimeSpec()

def Shutdown(self):
    """
    Shuts down the node. Called automatically by ClientNodeSetup and ServerNodeSetup

    Shutdown must be called before program exit to avoid segfaults and other undefined
    behavior. The singleton node is automatically shut down in Python. The use of 
    ClientNodeSetup and ServerNodeSetup is recommended to automate
    the node lifecycle. Calling this function does the following:
    1. Closes all services and releases all service objects
    2. Closes all client connections
    3. Shuts down discovery
    4. Shuts down all transports
    5. Notifies all shutdown listeners
    6. Releases all periodic cleanup task listeners
    7. Shuts down and releases the thread pool
            
    """
    self._Shutdown()

def SubscribeServiceByType(self, service_types, filter_=None):
    """
    Subscribe to listen for available services and automatically connect

    A ServiceSubscription will track the availability of service types and
    create connections when available.

    :param service_types: A list of service types to listen for, ie \"com.robotraconteur.robotics.robot.Robot\"
    :type service_types: List[str]
    :param filter: A filter to select individual services based on specified criteria
    :type filter: RobotRaconteur.ServiceSubscriptionFilter
    :return: The active subscription
    :rtype: ServiceSubscription
    """
    from .RobotRaconteurPythonUtil import SubscribeServiceByType
    return SubscribeServiceByType(self, service_types, filter_)

def SubscribeServiceInfo2(self, service_types, filter_=None):
    """
    Subscribe to listen for available services information

    A ServiceInfo2Subscription will track the availability of service types and
    inform when services become available or are lost. If connections to
    available services are also required, ServiceSubscription should be used.

    :param service_types: A list of service types to listen for, ie \"com.robotraconteur.robotics.robot.Robot\"
    :type service_types: List[str]
    :param filter: A filter to select individual services based on specified criteria
    :type filter: RobotRaconteur.ServiceSubscriptionFilter
    :return: The active subscription
    :rtype: ServiceInfo2Subscription
    """
    from .RobotRaconteurPythonUtil import SubscribeServiceInfo2
    return SubscribeServiceInfo2(self, service_types, filter_)

def SubscribeService(self,*args):
    """
    Subscribe to a service using one or more URL. Used to create robust connections to services

    Creates a ServiceSubscription assigned to a service with one or more candidate connection URLs. The
    subscription will attempt to maintain a peristent connection, reconnecting if the connection is lost.

    :param url: One or more candidate connection urls
    :type url: Union[str,List[str]]
    :param username: An optional username for authentication
    :type username: str
    :param credentials: Optional credentials for authentication
    :type credentials: Dict[str,Any]
    :return: The subscription object
    :rtype: RobotRaconteur.ServiceSubscription
    """
    from .RobotRaconteurPythonUtil import SubscribeService
    return SubscribeService(self, *args)

%}


}