

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

def RegisterServiceTypeFromFile(self, file_name):
	from .RobotRaconteurPythonUtil import ReadServiceDefinitionFile
	d = ReadServiceDefinitionFile(file_name)
	self._RegisterServiceType(str(d))
	
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
	
def RegisterService(self, name, objecttype, obj, securitypolicy=None):
	from .RobotRaconteurPythonUtil import WrappedServiceSkelDirectorPython, SplitQualifiedName
	director=WrappedServiceSkelDirectorPython(obj)
	rrobj=WrappedRRObject(objecttype,director,0)
	director.__disown__()
	return self._RegisterService(name,SplitQualifiedName(objecttype)[0],rrobj,securitypolicy)

__swig_getmethods__["NodeID"]=_NodeID
__swig_setmethods__["NodeID"]=_SetNodeID
__swig_getmethods__["NodeName"]=_NodeName
__swig_setmethods__["NodeName"]=_SetNodeName
__swig_getmethods__["ThreadPoolCount"]=_GetThreadPoolCount
__swig_setmethods__["ThreadPoolCount"]=_SetThreadPoolCount
	
__swig_getmethods__["RequestTimeout"]=lambda self : self._GetRequestTimeout()/1000.0
__swig_setmethods__["RequestTimeout"]=lambda self,t : self._SetRequestTimeout(t*1000)
__swig_getmethods__["TransportInactivityTimeout"]=lambda self : self._GetTransportInactivityTimeout()/1000.0
__swig_setmethods__["TransportInactivityTimeout"]=lambda self,t : self._SetTransportInactivityTimeout(t*1000)
__swig_getmethods__["EndpointInactivityTimeout"]=lambda self : self._GetEndpointInactivityTimeout()/1000.0
__swig_setmethods__["EndpointInactivityTimeout"]=lambda self,t : self._SetEndpointInactivityTimeout(t*1000)
__swig_getmethods__["MemoryMaxTransferSize"]=_GetMemoryMaxTransferSize
__swig_setmethods__["MemoryMaxTransferSize"]=_SetMemoryMaxTransferSize
__swig_getmethods__["NodeDiscoveryMaxCacheCount"]=_GetNodeDiscoveryMaxCacheCount
__swig_setmethods__["NodeDiscoveryMaxCacheCount"]=_SetNodeDiscoveryMaxCacheCount

def GetUseNumPy(self):
	from RobotRaconteur import UseNumPy
	return UseNumPy
					
	
def SetUseNumPy(self,value):
	import RobotRaconteur
	if (not value):
		RobotRaconteur.UseNumPy=False
		return
	try:
		numpy2=numpy
	except:
		raise Exception("NumPy is not available")
		
	RobotRaconteur.UseNumPy=True
		
__swig_getmethods__["UseNumPy"]=GetUseNumPy
__swig_setmethods__["UseNumPy"]=SetUseNumPy

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
	if (not t[1] in d.Exceptions): raise Exception('Invalid exception type')
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
	
__swig_getmethods__["RobotRaconteurVersion"]=_GetRobotRaconteurVersion

def NowUTC(self):
	return self._NowUTC()

def Shutdown(self):
	self._Shutdown()

def SubscribeService(self, service_types, filter_=None):
	from .RobotRaconteurPythonUtil import SubscribeService
	return SubscribeService(self, service_types, filter_)

def SubscribeServiceInfo2(self, service_types, filter_=None):
	from .RobotRaconteurPythonUtil import SubscribeServiceInfo2
	return SubscribeServiceInfo2(self, service_types, filter_)

%}


}