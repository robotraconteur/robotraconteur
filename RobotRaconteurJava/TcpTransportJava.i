
%typemap("javacode") RobotRaconteur::TcpTransport
%{
public final void enableNodeDiscoveryListening(IPNodeDiscoveryFlags flags)
{
	enableNodeDiscoveryListening(flags.ordinal());
}

public final void enableNodeAnnounce(IPNodeDiscoveryFlags flags)
{
	enableNodeAnnounce(flags.ordinal());
}

public final boolean isTransportConnectionSecure(Object obj)
{
	if (!(obj instanceof ServiceStub))
	{
		throw new UnsupportedOperationException("Object is not a Robot Raconteur reference");
	}
	ServiceStub s = (ServiceStub)obj;

	return _IsTransportConnectionSecure(s.rr_innerstub);
}

public final boolean isSecurePeerIdentityVerified(Object obj)
{
	if (!(obj instanceof ServiceStub))
	{
		throw new UnsupportedOperationException("Object is not a Robot Raconteur reference");
	}
	ServiceStub s = (ServiceStub)obj;

	return _IsSecurePeerIdentityVerified(s.rr_innerstub);
}

public final String getSecurePeerIdentity(Object obj)
{
	if (!(obj instanceof ServiceStub))
	{
		throw new UnsupportedOperationException("Object is not a Robot Raconteur reference");
	}
	ServiceStub s = (ServiceStub)obj;

	return _GetSecurePeerIdentity(s.rr_innerstub);
}

%}

%include "TcpTransport.i"