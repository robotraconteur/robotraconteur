
%rename(_IsTransportConnectionSecure) IsTransportConnectionSecure;
%rename(_IsSecurePeerIdentityVerified) IsSecurePeerIdentityVerified;
%rename(_GetSecurePeerIdentity) GetSecurePeerIdentity;

%include "TcpTransport.i"

%extend RobotRaconteur::TcpTransport
{
%pythoncode
%{
	DefaultReceiveTimeout = property(lambda self: self._GetDefaultReceiveTimeout()/1000.0, lambda self, timeout: self._SetDefaultReceiveTimeout(timeout*1000))
	DefaultConnectTimeout = property(lambda self: self._GetDefaultConnectTimeout()/1000.0, lambda self, timeout: self._SetDefaultConnectTimeout(timeout*1000))
	DefaultHeartbeatPeriod = property(lambda self: self._GetDefaultHeartbeatPeriod()/1000.0, lambda self, timeout: self._SetDefaultHeartbeatPeriod(timeout*1000))
	MaxMessageSize = property(lambda self: self._GetMaxMessageSize(), lambda self, size: self._SetMaxMessageSize(size))
	MaxConnectionCount = property(lambda self: self._GetMaxConnectionCount(), lambda self, count: self._SetMaxConnectionCount(count))
	RequireTls = property(lambda self: self._GetRequireTls(), lambda self, val: self._SetRequireTls(val))
	IsTlsNodeCertificateLoaded = property(lambda self: self._IsTlsNodeCertificateLoaded(), lambda self: self._IsPortSharerRunning())
	AcceptWebSockets = property(lambda self: self._GetAcceptWebSockets(), lambda self, val: self._SetAcceptWebSockets(val))
	DisableMessage4 = property(lambda self: self._GetDisableMessage4(), lambda self, val: self._SetDisableMessage4(val))
	DisableStringTable = property(lambda self: self._GetDisableStringTable(), lambda self, val: self._SetDisableStringTable(val))
	DisableAsyncMessageIO = property(lambda self: self._GetDisableAsyncMessageIO(), lambda self, val: self._SetDisableAsyncMessageIO(val))
	NodeAnnouncePeriod = property(lambda self: self._GetNodeAnnouncePeriod()/1000.0, lambda self, timeout: self._SetNodeAnnouncePeriod(timeout*1000))
	
	def IsTransportConnectionSecure(self,obj):
		if (hasattr(obj,'rrinnerstub')):
			obj=obj.rrinnerstub
		return self._IsTransportConnectionSecure(obj)
		
	def IsSecurePeerIdentityVerified(self,obj):
		if (hasattr(obj,'rrinnerstub')):
			obj=obj.rrinnerstub
		return self._IsSecurePeerIdentityVerified(obj)
		
	def GetSecurePeerIdentity(self,obj):
		if (hasattr(obj,'rrinnerstub')):
			obj=obj.rrinnerstub
		return self._GetSecurePeerIdentity(obj)
%}

}