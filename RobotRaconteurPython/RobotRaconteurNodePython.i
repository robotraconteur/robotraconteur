

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
	
	from .RobotRaconteurPythonUtil import PackMessageElement, WrappedClientServiceListenerDirector, AsyncStubReturnDirectorImpl, async_call, adjust_timeout

	if (username is None ): username=""
	if (credentials is not None): credentials=PackMessageElement(credentials,"varvalue{string}",None,self).GetData()
	listener2=None
	if (listener is not None):
		listener2=WrappedClientServiceListenerDirector(listener)
		listener2.__disown__()
	
	return async_call(self._AsyncConnectService,(url, username, credentials, listener2, "", adjust_timeout(timeout)), AsyncStubReturnDirectorImpl, handler)

def DisconnectService(self, stub):
	self._DisconnectService(stub.rrinnerstub)

def AsyncDisconnectService(self, stub, handler):
	from .RobotRaconteurPythonUtil import async_call, AsyncVoidNoErrReturnDirectorImpl
	return async_call(self._AsyncDisconnectService,(stub.rrinnerstub,), AsyncVoidNoErrReturnDirectorImpl,handler,noerror=True)

@classproperty.classpropertyreadonly
def s(self):
	return RobotRaconteurNode._get_s()

def RegisterServiceType(self, d):
	self._RegisterServiceType(d)

def RegisterServiceTypes(self, d):
	self._RegisterServiceTypes(d)

def RegisterServiceTypeFromFile(self, file_name):
	from .RobotRaconteurPythonUtil import ReadServiceDefinitionFile
	d = ReadServiceDefinitionFile(file_name)
	self._RegisterServiceType(str(d))

def RegisterServiceTypesFromFiles(self, file_names):
	from .RobotRaconteurPythonUtil import ReadServiceDefinitionFile
	d = []
	for file_name in file_names:
		d.append(str(ReadServiceDefinitionFile(file_name)))
	self._RegisterServiceTypes(d)
	
def GetServiceType(self, name):
	return self._GetServiceType(name)

def GetRegisteredServiceTypes(self):
	return self._GetRegisteredServiceTypes()
	
def GetPulledServiceTypes(self,obj):
	if (hasattr(obj,'rrinnerstub')):
		obj=obj.rrinnerstub
	return self._GetPulledServiceTypes(obj)
		
def GetPulledServiceType(self,obj,servicename):
	if (hasattr(obj,'rrinnerstub')):
		obj=obj.rrinnerstub
	return self._GetPulledServiceType(obj,servicename)

def GetServicePath(self,obj):
	if (hasattr(obj,'rrinnerstub')):
		obj=obj.rrinnerstub
	return self._GetServicePath(obj)
		
def NewStructure(self,type,obj=None):
	from .RobotRaconteurPythonUtil import NewStructure
	return NewStructure(type,obj,self)

def GetStructureType(self,type,obj=None):
	from .RobotRaconteurPythonUtil import GetStructureType
	return GetStructureType(type,obj,self)

def GetPodDType(self,type,obj=None):
	from .RobotRaconteurPythonUtil import GetPodDType
	return GetPodDType(type,obj,self)

def GetNamedArrayDType(self,type,obj=None):
	from .RobotRaconteurPythonUtil import GetNamedArrayDType
	return GetNamedArrayDType(type,obj,self)
	
def NamedArrayToArray(self,named_array):
	from .RobotRaconteurPythonUtil import NamedArrayToArray
	return NamedArrayToArray(named_array)

def ArrayToNamedArray(self,a,named_array_dt):
	from .RobotRaconteurPythonUtil import ArrayToNamedArray
	return ArrayToNamedArray(a,named_array_dt)

class ScopedMonitorLock(object):
    def __init__(self,obj,timeout=-1):
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
    	self.node.MonitorEnter(self.obj,timeout)
    	self.locked=True
    def unlock(self):
    	self.node.MonitorExit(self.obj,)
    	self.locked=False
    def release(self):
    	self.obj=None

def RequestObjectLock(self,obj,flags):
	return self._RequestObjectLock(obj.rrinnerstub,flags)
		
def ReleaseObjectLock(self,obj):
	return self._ReleaseObjectLock(obj.rrinnerstub)
		
def MonitorEnter(self,obj,timeout=-1):
	from .RobotRaconteurPythonUtil import adjust_timeout	
	self._MonitorEnter(obj.rrinnerstub,adjust_timeout(timeout))
			
def MonitorExit(self,obj):
	#obj.rrlock.release()
	self._MonitorExit(obj.rrinnerstub)

def AsyncRequestObjectLock(self,obj,flags,handler,timeout=RR_TIMEOUT_INFINITE):
	from .RobotRaconteurPythonUtil import async_call, adjust_timeout, AsyncStringReturnDirectorImpl
	return async_call(self._AsyncRequestObjectLock,(obj.rrinnerstub,flags,adjust_timeout(timeout)),AsyncStringReturnDirectorImpl,handler)
				
def AsyncReleaseObjectLock(self,obj,handler,timeout=RR_TIMEOUT_INFINITE):
	from .RobotRaconteurPythonUtil import async_call, adjust_timeout, AsyncStringReturnDirectorImpl
	return async_call(self._AsyncReleaseObjectLock,(obj.rrinnerstub,adjust_timeout(timeout)),AsyncStringReturnDirectorImpl,handler)

def GetServiceAttributes(self,obj):
	from .RobotRaconteurPythonUtil import UnpackMessageElement
	return UnpackMessageElement(self._GetServiceAttributes(obj.rrinnerstub),"varvalue{string} value",None,self)

def GetServiceNodeID(self,obj):
	return self._GetServiceNodeID(obj.rrinnerstub)

def GetServiceNodeName(self,obj):
	return self._GetServiceNodeName(obj.rrinnerstub)

def GetServiceName(self,obj):
	return self._GetServiceName(obj.rrinnerstub)
	
def RegisterService(self, name, objecttype, obj, securitypolicy=None):
	from .RobotRaconteurPythonUtil import WrappedServiceSkelDirectorPython, SplitQualifiedName
	director=WrappedServiceSkelDirectorPython(obj)
	rrobj=WrappedRRObject(objecttype,director,0)
	director.__disown__()
	return self._RegisterService(name,SplitQualifiedName(objecttype)[0],rrobj,securitypolicy)

NodeID = property(lambda self: self._NodeID(), lambda self,nodeid: self._SetNodeID(nodeid))
NodeName =property(lambda self: self._NodeName(), lambda self,nodename: self._SetNodeName(nodename))
ThreadPoolCount = property(lambda self: self._GetThreadPoolCount(), lambda self,c: self._SetThreadPoolCount(c))
	
RequestTimeout = property(lambda self : self._GetRequestTimeout()/1000.0, lambda self,t : self._SetRequestTimeout(t*1000))
TransportInactivityTimeout = property(lambda self : self._GetTransportInactivityTimeout()/1000.0, lambda self,t : self._SetTransportInactivityTimeout(t*1000))
EndpointInactivityTimeout = property(lambda self : self._GetEndpointInactivityTimeout()/1000.0, lambda self,t : self._SetEndpointInactivityTimeout(t*1000))
MemoryMaxTransferSize = property(lambda self: self._GetMemoryMaxTransferSize(), lambda self,m: self._SetMemoryMaxTransferSize(m))
NodeDiscoveryMaxCacheCount = property(lambda self: self._GetNodeDiscoveryMaxCacheCount(), lambda self,c: self._SetNodeDiscoveryMaxCacheCount(c))

def GetConstants(self,servicetype, obj=None):
	from .RobotRaconteurPythonUtil import ServiceDefinitionConstants
	if obj is None:
		d=self.GetServiceType(servicetype)
	else:
		d=self.GetPulledServiceType(obj,servicetype)
	return ServiceDefinitionConstants(d,self,obj)
		
def GetExceptionType(self, exceptionname, obj=None):
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
	if (ind is None):
		return self._FindObjectType(obj.rrinnerstub,member)
	else:
		return self._FindObjectType(obj.rrinnerstub,member,ind)

def AsyncFindObjectType(self,obj,member,handler,timeout=RR_TIMEOUT_INFINITE):
	from .RobotRaconteurPythonUtil import async_call, adjust_timeout, AsyncStringReturnDirectorImpl
	return async_call(self._AsyncFindObjectType,(obj.rrinnerstub,member,adjust_timeout(timeout)),AsyncStringReturnDirectorImpl,handler)
			
def AsyncFindObjectTypeInd(self,obj,member,ind,handler,timeout=RR_TIMEOUT_INFINITE):
	from .RobotRaconteurPythonUtil import async_call, adjust_timeout, AsyncStringReturnDirectorImpl
	return async_call(self._AsyncFindObjectType,(obj.rrinnerstub,member,ind,adjust_timeout(timeout)),AsyncStringReturnDirectorImpl,handler)

def SetExceptionHandler(self, handler):
	from .RobotRaconteurPythonUtil import ExceptionHandlerDirectorImpl
	if (handler is None):
		self._ClearExceptionHandler()
	else:
		d=ExceptionHandlerDirectorImpl(handler)
		d.__disown__()
		self._SetExceptionHandler(d,0)
			
def CreateTimer(self,period,handler,oneshot=False):
	from .RobotRaconteurPythonUtil import AsyncTimerEventReturnDirectorImpl
	
	handler2=AsyncTimerEventReturnDirectorImpl(handler)
	handler2.__disown__()
	ret= self._CreateTimer(period,oneshot,handler2,0)
		
	return ret
		
def PostToThreadPool(self, handler):
	from .RobotRaconteurPythonUtil import async_call, AsyncVoidNoErrReturnDirectorImpl
	return async_call(self._PostToThreadPool,(), AsyncVoidNoErrReturnDirectorImpl,handler,noerror=True)
	
RobotRaconteurVersion = property(lambda self: self._GetRobotRaconteurVersion())

def NowUTC(self):
	return self._NowUTC()

def Shutdown(self):
	self._Shutdown()

def SubscribeServiceByType(self, service_types, filter_=None):
	from .RobotRaconteurPythonUtil import SubscribeServiceByType
	return SubscribeServiceByType(self, service_types, filter_)

def SubscribeServiceInfo2(self, service_types, filter_=None):
	from .RobotRaconteurPythonUtil import SubscribeServiceInfo2
	return SubscribeServiceInfo2(self, service_types, filter_)

def SubscribeService(self,*args):
	from .RobotRaconteurPythonUtil import SubscribeService
	return SubscribeService(self, *args)

%}


}