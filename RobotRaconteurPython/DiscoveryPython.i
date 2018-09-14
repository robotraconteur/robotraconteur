%include "Discovery.i"

%extend RobotRaconteur::RobotRaconteurNode
{
%pythoncode %{
def FindServiceByType(self,servicetype,transportschemes):
	from .RobotRaconteurPythonUtil import ServiceInfo2
	ts2=vectorstring()
	for t in transportschemes:
		ts2.push_back(t)
	info1=WrappedFindServiceByType(self, servicetype,ts2)
	ret=[]
	for e in info1:
		ret.append(ServiceInfo2(e))
	return ret
	
def AsyncFindServiceByType(self,servicetype,transportschemes,handler,timeout=5):
	class ServiceInfo2Director(AsyncServiceInfo2VectorReturnDirector):
		def __init__(self,handler):			
			super(ServiceInfo2Director,self).__init__()
			self._handler=handler
		
		def handler(self,info1):
			from .RobotRaconteurPythonUtil import ServiceInfo2
			ret=[]			
			for e in info1:
				ret.append(ServiceInfo2(e))
			self._handler(ret)
	
	ts2=vectorstring()
	for t in transportschemes:
		ts2.push_back(t)
	
	from .RobotRaconteurPythonUtil import async_call, adjust_timeout
	return async_call(AsyncWrappedFindServiceByType,(self,servicetype,ts2,adjust_timeout(timeout)),ServiceInfo2Director,handler,False)
	
%}
}

//Find Nodes

%extend RobotRaconteur::RobotRaconteurNode
{

%pythoncode %{
def FindNodeByID(self,id,transportschemes):
	from .RobotRaconteurPythonUtil import NodeInfo2
	ts2=vectorstring()
	for t in transportschemes:
		ts2.push_back(t)
	info1=WrappedFindNodeByID(self,id,ts2)
	ret=[]
	for e in info1:
		ret.append(NodeInfo2(e))
	return ret

def FindNodeByName(self,name,transportschemes):
	from .RobotRaconteurPythonUtil import NodeInfo2
	ts2=vectorstring()
	for t in transportschemes:
		ts2.push_back(t)
	info1=WrappedFindNodeByName(self,name,ts2)
	ret=[]
	for e in info1:
		ret.append(NodeInfo2(e))
	return ret
	
def AsyncFindNodeByID(self,id,transportschemes,handler,timeout=5):
	class NodeInfo2Director(AsyncNodeInfo2VectorReturnDirector):
		def __init__(self,handler):
			super(NodeInfo2Director,self).__init__()
			self._handler=handler
		
		def handler(self,info1):
			from .RobotRaconteurPythonUtil import NodeInfo2
			ret=[]			
			for e in info1:
				ret.append(NodeInfo2(e))
			self._handler(ret)
	ts2=vectorstring()
	for t in transportschemes:
		ts2.push_back(t)
	
	from .RobotRaconteurPythonUtil import async_call, adjust_timeout
	return async_call(AsyncWrappedFindNodeByID,(self,id,ts2,adjust_timeout(timeout)),NodeInfo2Director,handler,False)
	
def AsyncFindNodeByName(self,name,transportschemes,handler,timeout=5):
	class NodeInfo2Director(AsyncNodeInfo2VectorReturnDirector):
		def __init__(self,handler):
			super(NodeInfo2Director,self).__init__()
			self._handler=handler
		
		def handler(self,info1):
			from .RobotRaconteurPythonUtil import NodeInfo2
			ret=[]			
			for e in info1:
				ret.append(NodeInfo2(e))
			self._handler(ret)
	ts2=vectorstring()
	for t in transportschemes:
		ts2.push_back(t)
	
	from .RobotRaconteurPythonUtil import async_call, adjust_timeout
	return async_call(AsyncWrappedFindNodeByName,(self,name,ts2,adjust_timeout(timeout)),NodeInfo2Director,handler,False)	

def UpdateDetectedNodes(self, schemes):
	ts2=vectorstring()
	for t in schemes:
		ts2.push_back(t)
	WrappedUpdateDetectedNodes(self,ts2)

def AsyncUpdateDetectedNodes(self,schemes,handler,timeout=5):
	ts2=vectorstring()
	for t in schemes:
		ts2.push_back(t)

	from .RobotRaconteurPythonUtil import async_call, adjust_timeout, AsyncVoidNoErrReturnDirectorImpl
	return async_call(AsyncWrappedUpdateDetectedNodes,(self,ts2, adjust_timeout(timeout),), AsyncVoidNoErrReturnDirectorImpl,handler,noerror=True)

def GetDetectedNodes(self):
	o1=WrappedGetDetectedNodes(self)
	return [NodeID(x) for x in o1]	

%}
}