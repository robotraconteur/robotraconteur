%include "Discovery.i"

%extend RobotRaconteur::RobotRaconteurNode
{
%pythoncode %{
def FindServiceByType(self,servicetype,transportschemes):
	"""
	Use discovery to find available services by service type

	Uses discovery to find available services based on a service type. This
	service type is the type of the root object, ie
	\"com.robotraconteur.robotics.robot.Robot\". This process will update the detected
	node cache.

	:param servicetype: The service type to find, ie \"com.robotraconteur.robotics.robot.Robot\"
	:type servicetype: str
	:param transportschemes: A list of transport types to search, ie \"rr+tcp\", \"rr+local\", \"rrs+tcp\", etc
	:type transportschemes: List[str]
	:return: The discovered services
	:rtype: List[RobotRaconteur.ServiceInfo2]
	"""
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
	"""
	Asynchronously use discovery to find availabe services by service type

	Same as FindServiceByType() but returns asynchronously

	If ``handler`` is None, returns an awaitable future.

	:param servicetype: The service type to find, ie \"com.robotraconteur.robotics.robot.Robot\"
	:type servicetype: str
	:param transportschemes: A list of transport types to search, ie \"rr+tcp\", \"rr+local\", \"rrs+tcp\", etc
	:type transportschemes: List[str]
	:param handler: Handler to call on completion
	:type handler: Callable[[List[ServiceInfo2]],None]
	:param timeout: Timeout in seconds. Using a timeout greater than 5 seconds is not recommended.
	:type timeout: float
	"""
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
	"""
	Finds nodes on the network with a specified NodeID

	Updates the discovery cache and find nodes with the specified NodeID.
	This function returns unverified cache information.

	:param id: The NodeID to find
	:type id: RobotRaconteur.NodeID
	:param transportschemes: A list of transport types to search, ie \"rr+tcp\", \"rr+local\", \"rrs+tcp\", etc
	:type transportschemel: str
	:return: The discovered nodes
	:rtype: List[NodeInfo2]
	"""
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
	"""
	Finds nodes on the network with a specified NodeName

	Updates the discovery cache and find nodes with the specified NodeName.
	This function returns unverified cache information.

	:param name: The NodeName to find
	:type name: str
	:param transportschemes: A list of transport types to search, ie \"rr+tcp\", \"rr+local\", \"rrs+tcp\", etc
	:return: The discovered nodes
	:rtype: List[NodeInfo2]
	"""
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
	"""
	Asynchronously finds nodes on the network with the specified NodeID

	Same as FindNodeByID() but returns asynchronously

	If ``handler`` is None, returns an awaitable future.

	:param id: The NodeID to find
	:type id: RobotRaconteur.NodeID
	:param transportschemes: A list of transport types to search, ie \"rr+tcp\", \"rr+local\", \"rrs+tcp\", etc
	:type transportschemes: List[str]
	:param handler: Handler to call on completion
	:type handler: Callable[List[NodeInfo2],None]
	:param timeout: Timeout in seconds. Using a timeout greater than 5 seconds is not recommended.
	:type timeout: float
	"""
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
	"""
	Asynchronously finds nodes on the network with the specified NodeName

	Same as FindNodeByName() but returns asynchronously

	If ``handler`` is None, returns an awaitable future.

	:param name: The NodeName to find
	:type name: str
	:param transportschemes: A list of transport types to search, ie \"rr+tcp\", \"rr+local\", \"rrs+tcp\", etc
	:type transportschemes: List[str]
	:param handler: Handler to call on completion
	:type handler: Callable[List[NodeInfo2],None]
	:param timeout: Timeout in seconds. Using a timeout greater than 5 seconds is not recommended.
	:type timeout: float
	"""
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
	"""
	Update the detected nodes cache

	The node keeps a cache of detected nodes, but this may become stale
	if nodes are rapidly added and removed from the network. Call this
	function to update the detected nodes.

	:param schemes: A list of transport schemes, ie \"rr+tcp\", \"rr+local\", etc. to update.
	:type schemes: List[str]
	"""
	ts2=vectorstring()
	for t in schemes:
		ts2.push_back(t)
	WrappedUpdateDetectedNodes(self,ts2)

def AsyncUpdateDetectedNodes(self,schemes,handler,timeout=5):
	"""
	Asynchronously update the detected nodes cache

	Same as UpdateDetectedNodes() but returns asynchronously

	If ``handler`` is None, returns an awaitable future.

	:param schemes: A list of transport schemes, ie \"rr+tcp\", \"rr+local\", etc. to update.
	:type schemes: List[str]
	:param handler: The handler to call on completion
	:type handler: Callable[[],None]
	:param timeout: The timeout for the operation in seconds. This function will often run
	 for the full timeout, so values less than 5 seconds are recommended.
	:type timeout: float
	"""
	ts2=vectorstring()
	for t in schemes:
		ts2.push_back(t)

	from .RobotRaconteurPythonUtil import async_call, adjust_timeout, AsyncVoidNoErrReturnDirectorImpl
	return async_call(AsyncWrappedUpdateDetectedNodes,(self,ts2, adjust_timeout(timeout),), AsyncVoidNoErrReturnDirectorImpl,handler,noerror=True)

def GetDetectedNodes(self):
	"""
	Get the nodes currently detected by Transports

	Transports configured to listen for node discovery send detected node
	information to the parent node, where it is stored. Normally this information
	will expire after one minute, and needs to be constantly refreshed.

	This node information is not verified. It is the raw discovery
	information received by the transports. Verification is done
	when the node is interrogated for service information.

	:return: List of detected NodeID
	:rtype: List[RobotRaconteur.NodeID]
	"""
	o1=WrappedGetDetectedNodes(self)
	return [NodeID(x) for x in o1]

def GetDetectedNodeCacheInfo(self, nodeid):
	"""
	Get cached node discovery information

	Return current node information from the discovery cache. This
	information is unverified and is used for the first step in the
	discovery process.

	:param nodeid: The NodeID of the requested node
	:type nodeid: RobotRaconteur.NodeID
	:return: The node info
	:rtype: RobotRaconteur.NodeInfo2
	"""
	from .RobotRaconteurPythonUtil import NodeInfo2
	return NodeInfo2(WrappedGetDetectedNodeCacheInfo(self, nodeid))

def TryGetDetectedNodeCacheInfo(self, nodeid):
	"""
	Try get cached node discovery information

	Same as GetDetectedNodeCacheInfo, but returns bool
	for success or failure instead of throwing an exception
	"""
	from .RobotRaconteurPythonUtil import NodeInfo2 as NodeInfo2py
	ret = NodeInfo2()
	res = WrappedTryGetDetectedNodeCacheInfo(self,nodeid,ret)
	if not res:
		return False, None
	else:
		return True, NodeInfo2py(ret)
%}
}
