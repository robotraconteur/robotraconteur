%feature("shadow") RobotRaconteur::ServiceSecurityPolicy::ServiceSecurityPolicy(boost::shared_ptr<RobotRaconteur::UserAuthenticator> Authenticator, std::map<std::string, std::string> Policies)
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
		from .RobotRaconteurPythonUtil import _UserAuthenticator_PackCredentials
		m=_UserAuthenticator_PackCredentials(credentials)
		return self._AuthenticateUser(username,m,context);
%}

}
