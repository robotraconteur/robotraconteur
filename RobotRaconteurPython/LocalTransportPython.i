%include "LocalTransport.i"

%extend RobotRaconteur::LocalTransport
{
%pythoncode
%{		
	DisableMessage4 = property(lambda self: self._GetDisableMessage4(), lambda self, val: self._SetDisableMessage4(val))
	DisableStringTable = property(lambda self: self._GetDisableStringTable(), lambda self, val: self._SetDisableStringTable(val))
	DisableAsyncMessageIO = property(lambda self: self._GetDisableAsyncMessageIO(), lambda self, val: self._SetDisableAsyncMessageIO(val))
%}
}