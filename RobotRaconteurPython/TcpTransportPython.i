
%rename(_IsTransportConnectionSecure) IsTransportConnectionSecure;
%rename(_IsSecurePeerIdentityVerified) IsSecurePeerIdentityVerified;
%rename(_GetSecurePeerIdentity) GetSecurePeerIdentity;

%include "TcpTransport.i"

%extend RobotRaconteur::TcpTransport
{
%pythoncode
%{
	DefaultReceiveTimeout = property(lambda self: self._GetDefaultReceiveTimeout()/1000.0, lambda self, timeout: self._SetDefaultReceiveTimeout(timeout*1000),
		doc = """
		Set the default receive timeout in seconds

		If no messages are received within the timeout, the connection
		is assumed to be lost.

		Default: 15 seconds

		:rtype: float
		""")
	DefaultConnectTimeout = property(lambda self: self._GetDefaultConnectTimeout()/1000.0, lambda self, timeout: self._SetDefaultConnectTimeout(timeout*1000),
		doc = """
		Set the default connect timeout in seconds

		If the connection is not completed within the timeout, the connection
		attempt will be aborted.

		Default: 5 seconds

		:rtype: float
		"""
	)
	DefaultHeartbeatPeriod = property(lambda self: self._GetDefaultHeartbeatPeriod()/1000.0, lambda self, timeout: self._SetDefaultHeartbeatPeriod(timeout*1000),
		doc = """
		Set the default heartbeat period in seconds

		The heartbeat is used to keep the connection alive
		if no communication is occuring between nodes.

		Default: 5 seconds

		:rtype: float
		"""
	)
	MaxMessageSize = property(lambda self: self._GetMaxMessageSize(), lambda self, size: self._SetMaxMessageSize(size),
		doc = """
		Set the maximum serialized message size

		Default: 10 MB

		:rtype: int
		""")
	MaxConnectionCount = property(lambda self: self._GetMaxConnectionCount(), lambda self, count: self._SetMaxConnectionCount(count),
		doc = """
		Set the maximum number of concurrent connections
		
		Default: 0 for unlimited

		:rtype: int
		""")
	RequireTls = property(lambda self: self._GetRequireTls(), lambda self, val: self._SetRequireTls(val),
		doc = """
		Set if all connections require TLS

		If true, all connections require TLS

		:rtype: bool
		""")
	IsTlsNodeCertificateLoaded = property(lambda self: self._IsTlsNodeCertificateLoaded(),
		doc = """
	Check if TLS certificate is loaded

	:rtype: bool
	""")
	AcceptWebSockets = property(lambda self: self._GetAcceptWebSockets(), lambda self, val: self._SetAcceptWebSockets(val),
		doc = """
		Set if the transport will accept incoming HTTP websocket connections
		
		Default: true

		:rtype: bool
		""")
	DisableMessage4 = property(lambda self: self._GetDisableMessage4(), lambda self, val: self._SetDisableMessage4(val),
		doc = """
		Set disable Message Format Version 4

		Message Format Version 2 will be used

		Default: Message V4 is enabled

		:rtype: bool
		""")
	DisableStringTable = property(lambda self: self._GetDisableStringTable(), lambda self, val: self._SetDisableStringTable(val),
		doc = """
		Set disable string table

		Default: false

		RobotRaconteurNodeSetup and its subclasses
		will disable the string table by default

		:rtype: bool
		""")
		
	DisableAsyncMessageIO = property(lambda self: self._GetDisableAsyncMessageIO(), lambda self, val: self._SetDisableAsyncMessageIO(val),
		doc = """
		Set if async message io is disabled

		Async message io has better memory handling, at the
		expense of slightly higher latency.

		Default: Async io enabled

		:rtype: bool
		""")
		
	NodeAnnouncePeriod = property(lambda self: self._GetNodeAnnouncePeriod()/1000.0, lambda self, timeout: self._SetNodeAnnouncePeriod(timeout*1000),
		doc = """
		Set the period between node announce in seconds

		Default 55 seconds

		:rtype: float
		"""
	
	)
	
	def IsTransportConnectionSecure(self,obj):
		"""
		Check if specified client or endpoint is using TLS for its transport

		Throws ConnectionException if the client or endpoint is invalid or the connection
		is not using TcpTransport

		:param obj: The client or endpoint to check
		:return: True The connection is using TLS, otherwise False
		:rtype: bool
		"""
		if (hasattr(obj,'rrinnerstub')):
			obj=obj.rrinnerstub
		return self._IsTransportConnectionSecure(obj)
		
	def IsSecurePeerIdentityVerified(self,obj):
		"""
		Check if specified peer is using TLS and has been
		verified using a certificate

		Checks if the peer server node has a valid certificate, or if the peer client
		has been verified using mutual authentication.

		Throws ConnectionException if the endpoint is invalid or the connection
		is not using TcpTransport

		:param obj: The client or endpoint to check
		:return: True The connection is using TLS, otherwise False
		:rtype: bool
		"""
		if (hasattr(obj,'rrinnerstub')):
			obj=obj.rrinnerstub
		return self._IsSecurePeerIdentityVerified(obj)
		
	def GetSecurePeerIdentity(self,obj):
		"""
		Get the identity of the peer if secured using TLS

		Get the identity of a peer verified using TLS certificates. Returns a NodeID in string
		format. Will throw AuthenticationException if the peer has not been verified.

		:param obj: The client or endpoint to check
		:return: The verified peer NodeID as a string
		:rtype: str
		"""
		if (hasattr(obj,'rrinnerstub')):
			obj=obj.rrinnerstub
		return self._GetSecurePeerIdentity(obj)
%}

}