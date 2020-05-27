%typemap(cstype) IPNodeDiscoveryFlags "IPNodeDiscoveryFlags"

%typemap("cscode") RobotRaconteur::TcpTransport
%{
public void EnableNodeDiscoveryListening(IPNodeDiscoveryFlags flags)
{
	EnableNodeDiscoveryListening((uint)flags);
}

public void EnableNodeAnnounce(IPNodeDiscoveryFlags flags)
{
	EnableNodeAnnounce((uint)flags);
}

public int DefaultReceiveTimeout
{
	get
	{
		return _GetDefaultReceiveTimeout();
	}
	set
	{
		_SetDefaultReceiveTimeout(value);
	}

}

public int DefaultConnectTimeout
{
	get
	{
		return _GetDefaultConnectTimeout();
	}
	set
	{
		_SetDefaultConnectTimeout(value);
	}

}

public int DefaultHeartbeatPeriod
{
	get
	{
		return _GetDefaultHeartbeatPeriod();
	}
	set
	{
		_SetDefaultHeartbeatPeriod(value);
	}

}

public int MaxMessageSize
{
	get
	{
		return _GetMaxMessageSize();
	}
	set
	{
		_SetMaxMessageSize(value);
	}
	
}

public int MaxConnectionCount
{
	get
	{
		return _GetMaxConnectionCount();
	}
	set
	{
		_SetMaxConnectionCount(value);
	}
	
}

public bool RequireTls
{
	get
	{
		return _GetRequireTls();
	}
	set
	{
		_SetRequireTls(value);
	}	
}

public bool IsTlsNodeCertificateLoaded
{
	get
	{
		return _IsTlsNodeCertificateLoaded();
	}
	
}




public bool IsPortSharerRunning
{
	get
	{
		return _IsPortSharerRunning();
	}
	
}

public bool IsTransportConnectionSecure(object obj)
{ 
	if (obj==null) throw new NullReferenceException();
	RobotRaconteur.ServiceStub s=obj as RobotRaconteur.ServiceStub;
	if (s==null) throw new InvalidOperationException("Object is not a Robot Raconteur reference");
	return _IsTransportConnectionSecure(s.rr_innerstub);

}

public bool IsSecurePeerIdentityVerified(object obj)
{ 
	if (obj==null) throw new NullReferenceException();
	RobotRaconteur.ServiceStub s=obj as RobotRaconteur.ServiceStub;
	if (s==null) throw new InvalidOperationException("Object is not a Robot Raconteur reference");
	return _IsSecurePeerIdentityVerified(s.rr_innerstub);

}

public string GetSecurePeerIdentity(object obj)
{ 
	if (obj==null) throw new NullReferenceException();
	RobotRaconteur.ServiceStub s=obj as RobotRaconteur.ServiceStub;
	if (s==null) throw new InvalidOperationException("Object is not a Robot Raconteur reference");
	return _GetSecurePeerIdentity(s.rr_innerstub);

}

public bool AcceptWebSockets
{
	get
	{
		return _GetAcceptWebSockets();
	}
	set
	{
		_SetAcceptWebSockets(value);
	}

}

public bool DisableMessage4
{
	get
	{
		return _GetDisableMessage4();
	}
	set
	{
		_SetDisableMessage4(value);
	}
}

public bool DisableStringTable
{
	get
	{
		return _GetDisableStringTable();
	}
	set
	{
		_SetDisableStringTable(value);
	}
}

public bool DisableAsyncMessageIO
{
	get
	{
		return _GetDisableAsyncMessageIO();
	}
	set
	{
		_SetDisableAsyncMessageIO(value);
	}
}

public int NodeAnnouncePeriod
{
	get
	{
		return _GetNodeAnnouncePeriod();
	}
	set
	{
		_SetNodeAnnouncePeriod(value);
	}

}

%}

%include "TcpTransport.i"