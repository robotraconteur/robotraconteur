
%rename(_IsTransportConnectionSecure) IsTransportConnectionSecure;
%rename(_IsSecurePeerIdentityVerified) IsSecurePeerIdentityVerified;
%rename(_GetSecurePeerIdentity) GetSecurePeerIdentity;

%include "TcpTransport.i"

%extend RobotRaconteur::TcpTransport
{
%pythoncode
%{
	__swig_getmethods__["DefaultReceiveTimeout"]= lambda self: self._GetDefaultReceiveTimeout()/1000.0
	__swig_setmethods__["DefaultReceiveTimeout"]= lambda self, timeout: self._SetDefaultReceiveTimeout(timeout*1000)
	__swig_getmethods__["DefaultConnectTimeout"]= lambda self: self._GetDefaultConnectTimeout()/1000.0
	__swig_setmethods__["DefaultConnectTimeout"]= lambda self, timeout: self._SetDefaultConnectTimeout(timeout*1000)
	__swig_getmethods__["DefaultHeartbeatPeriod"]= lambda self: self._GetDefaultHeartbeatPeriod()/1000.0
	__swig_setmethods__["DefaultHeartbeatPeriod"]= lambda self, timeout: self._SetDefaultHeartbeatPeriod(timeout*1000)
	__swig_getmethods__["MaxMessageSize"]= lambda self: self._GetMaxMessageSize()
	__swig_setmethods__["MaxMessageSize"]= lambda self, size: self._SetMaxMessageSize(size)
	__swig_getmethods__["MaxConnectionCount"]= lambda self: self._GetMaxConnectionCount()
	__swig_setmethods__["MaxConnectionCount"]= lambda self, count: self._SetMaxConnectionCount(count)
	__swig_getmethods__["RequireTls"]= lambda self: self._GetRequireTls()
	__swig_setmethods__["RequireTls"]= lambda self, val: self._SetRequireTls(val)
	__swig_getmethods__["IsTlsNodeCertificateLoaded"]= lambda self: self._IsTlsNodeCertificateLoaded()
	__swig_getmethods__["IsPortSharerRunning"]= lambda self: self._IsPortSharerRunning()
	__swig_getmethods__["AcceptWebSockets"]= lambda self: self._GetAcceptWebSockets()	
	__swig_setmethods__["AcceptWebSockets"]= lambda self, val: self._SetAcceptWebSockets(val)
	__swig_getmethods__["DisableMessage3"]= lambda self: self._GetDisableMessage3()
	__swig_setmethods__["DisableMessage3"]= lambda self, val: self._SetDisableMessage3(val)
	__swig_getmethods__["DisableStringTable"]= lambda self: self._GetDisableStringTable()
	__swig_setmethods__["DisableStringTable"]= lambda self, val: self._SetDisableStringTable(val)
	__swig_getmethods__["DisableAsyncMessageIO"]= lambda self: self._GetDisableAsyncMessageIO()
	__swig_setmethods__["DisableAsyncMessageIO"]= lambda self, val: self._SetDisableAsyncMessageIO(val)
	__swig_getmethods__["NodeAnnouncePeriod"]= lambda self: self._GetNodeAnnouncePeriod()/1000.0
	__swig_setmethods__["NodeAnnouncePeriod"]= lambda self, timeout: self._SetNodeAnnouncePeriod(timeout*1000)
	
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