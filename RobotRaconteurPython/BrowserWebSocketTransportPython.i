%include "BrowserWebSocketTransport.i"

%extend RobotRaconteur::BrowserWebSocketTransport
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

        Async message io is not well supported 
        by this transport. Enabling async message io
        is not recommended.

		Default: Async io disabled

		:rtype: bool
		""")
%}

}