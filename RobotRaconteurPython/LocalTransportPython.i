%include "LocalTransport.i"

%extend RobotRaconteur::LocalTransport
{
%pythoncode
%{
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

			Default: Async io enabled

			:rtype: bool
			""")
%}
}
