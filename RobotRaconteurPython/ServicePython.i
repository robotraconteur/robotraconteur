%include "Service.i"

%extend RobotRaconteur::ServerContext
{
%pythoncode %{
	def AddServerServiceListener(self,listener):
		from .RobotRaconteurPythonUtil import WrappedServerServiceListenerDirector
		s=WrappedServerServiceListenerDirector(listener,self)
		s.__disown__()
		self._AddServerServiceListener(s)
		
	def SetServiceAttributes(self,attributes):
		from .RobotRaconteurPythonUtil import PackMessageElement
		self._SetServiceAttributes(PackMessageElement(attributes,"varvalue{string} value",self))
		
	def ReleaseServicePath(self, path, endpoints=None):
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
	Username = property(lambda self: self._GetUsername())
	Privileges = property(lambda self: list(self._GetPrivileges()))
	Properties = property(lambda self: list(self._GetProperties()))
	LoginTime = property(lambda self: self._GetLoginTime())
	LastAccessTime = property(lambda self: self._GetLastAccessTime())
%}

}