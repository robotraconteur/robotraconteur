%feature("shadow") RobotRaconteur::ServiceSecurityPolicy::ServiceSecurityPolicy(const boost::shared_ptr<RobotRaconteur::UserAuthenticator>& Authenticator, std::map<std::string, std::string> Policies)
%{
	def __init__(self,*args):
		#This is a shadowed function
		from .RobotRaconteurPythonUtil import WrappedUserAuthenticatorDirectorPython
		args2=list(args)
		if (not isinstance(args2[0],NativeUserAuthenticator)):
			t=WrappedUserAuthenticatorDirectorPython(args2[0])
			t2=WrappedUserAuthenticator()
			t2.SetRRDirector(t,0)
			t.__disown__()
			args2[0]=t2
		this = _RobotRaconteurPython.new_ServiceSecurityPolicy(*args2)
		try: self.this.append(this)
		except: self.this = this

%}

%include "ServiceSecurity.i"

%extend RobotRaconteur::UserAuthenticator
{
%pythoncode %{
	def AuthenticateUser(self, username, credentials, context):
		"""
		Authenticate a user using username and credentials

		The service will call AuthenticateUser when clients attempt to authenticate. The
		username and credentials are passed to RobotRaconteurNode.ConnectService() or
		RobotRaconteurNode.AsyncConnectService(). The authenticator must analyze these
		values to decide how to authenticate the user.

		On successful authentication, return a populated AuthenticatedUser object. On
		failure, throw an exception.

		See \\ref security for more information.

		:param username: The username provided by the client
		:type username: str
		:param credentials: The credentials provided by the client
		:type credentials: Dict[str,Any]
		:param context: The context of the service requesting authentication
		:type context: RobotRaconteur.ServerContext
		:return: An authenticated user object
		:rtype: RobotRaconteur.AuthenticatedUser
		"""
		from .RobotRaconteurPythonUtil import _UserAuthenticator_PackCredentials
		m=_UserAuthenticator_PackCredentials(credentials)
		return self._AuthenticateUser(username,m,context);
%}

}
