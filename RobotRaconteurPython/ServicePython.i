%include "Service.i"

%extend RobotRaconteur::ServerContext
{
%pythoncode %{
	def AddServerServiceListener(self,listener):
		"""
		Add a service listener

		:param listener: Callable listener function
		:type listener: Callable[[RobotRaconteur.ServerContext,int,object]]
		"""
		from .RobotRaconteurPythonUtil import WrappedServerServiceListenerDirector
		s=WrappedServerServiceListenerDirector(listener,self)
		s.__disown__()
		self._AddServerServiceListener(s)

	def SetServiceAttributes(self,attributes):
		"""
		Set the service attributes

		Sets the service attributes. Attributes are made available to clients during
		service discovery. Attributes should be concise and not use any user defined
		types.

		:param attr: The service attributes
		:type attr: Dict[str,Any]
		"""
		from .RobotRaconteurPythonUtil import PackMessageElement
		self._SetServiceAttributes(PackMessageElement(attributes,"varvalue{string} value",self))

	def ReleaseServicePath(self, path, endpoints=None):
		"""
		Release the specified service path and all sub objects

		Services take ownership of objects returned by objrefs, and will only request the object
		once. Subsequent requests will return the cached object. If the objref has changed,
		the service must call ReleaseServicePath() to tell the service to request the object again.

		Release service path will release the object specified by the service path
		and all sub objects.

		This overload will notify the specified that the objref has been released. If the service
		path contains a session key, this overload should be used so the session key is not leaked.

		:param path: The service path to release
		:type path: str
		:param endpoints: (Optional) The client endpoint IDs to notify of the released service path,
		 or None to notify all endpoints
		:type endpoints: List[int]
		"""
		if (endpoints is None):
			self._ReleaseServicePath(path)
		else:
			v=vectoruint32()
			for e in endpoints:
				v.append(e)
			self._ReleaseServicePath(path,v)
%}
}

%extend RobotRaconteur::AuthenticatedUser
{

%pythoncode %{
	Username = property(lambda self: self._GetUsername(), doc = "The authenticated username")
	Privileges = property(lambda self: list(self._GetPrivileges()), doc = "The user privileges")
	Properties = property(lambda self: list(self._GetProperties()), doc = "The user properties")
	LoginTime = property(lambda self: self._GetLoginTime(), doc = "The user login time")
	LastAccessTime = property(lambda self: self._GetLastAccessTime(), doc = "The user last access time")
%}

}
