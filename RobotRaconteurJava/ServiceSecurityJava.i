
%typemap(javainterfaces) RobotRaconteur::UserAuthenticator "IUserAuthenticator";
%typemap(javacode) RobotRaconteur::UserAuthenticator
%{
	@Override
	public  AuthenticatedUser authenticateUser(String username, java.util.Map<String,Object> credentials, ServerContext context)
	{
		MessageElement c =(MessageElement) RobotRaconteurNode.s().<String,Object>packMapType(credentials,String.class, Object.class);
		return _AuthenticateUser(username, c, context);
	}


%}



%typemap(javacode) RobotRaconteur::ServiceSecurityPolicy
%{

	static private class WrappedUserAuthenticatorDirectorNET extends WrappedUserAuthenticatorDirector
	{
		IUserAuthenticator target;

		public WrappedUserAuthenticatorDirectorNET(IUserAuthenticator target)
		{
			this.target = target;
		}


		@Override
		public AuthenticatedUser authenticateUser(String username, MessageElement credentials, ServerContext context)
		{
			IUserAuthenticator t = target;
			if (t == null) throw new AuthenticationException("Authenticator internal error");
						
			java.util.Map<String, Object> c2 = (java.util.Map<String, Object>)RobotRaconteurNode.s().<String, Object>unpackMapType(credentials);

			AuthenticatedUser ret = t.authenticateUser(username, c2, context);
			return ret;

		}

	}

  public ServiceSecurityPolicy(IUserAuthenticator authenticator, java.util.Map<String,String> Policies) {
	 map_strstr Policies2=new map_strstr();
	 for (java.util.Map.Entry<String,String> m : Policies.entrySet())
	 {
		 Policies2.put(m.getKey(), m.getValue());
	 }
	 
	 NativeUserAuthenticator a2;
	 if (authenticator instanceof NativeUserAuthenticator)
	{
		a2= (NativeUserAuthenticator)authenticator;
	}
	else
	{

		WrappedUserAuthenticatorDirectorNET n = new WrappedUserAuthenticatorDirectorNET(authenticator);
		int id = RRObjectHeap.addObject(n);
		WrappedUserAuthenticator a3 = new WrappedUserAuthenticator();
		a3.setRRDirector(n, id);
		a2=a3;
		
	}
	 
	 swigCPtr=RobotRaconteurJavaJNI.new_ServiceSecurityPolicy(NativeUserAuthenticator.getCPtr(a2), a2, map_strstr.getCPtr(Policies2), Policies2);
	 
  }
%}

%include "ServiceSecurity.i"