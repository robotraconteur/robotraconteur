%include "HardwareTransport.i"

%extend RobotRaconteur::HardwareTransport
{
%pythoncode
%{		
	DisableMessage3 = property(lambda self: self._GetDisableMessage3(), lambda self, val: self._SetDisableMessage3(val))
	DisableStringTable = property(lambda self: self._GetDisableStringTable(), lambda self, val: self._SetDisableStringTable(val))
	DisableAsyncMessageIO = property(lambda self: self._GetDisableAsyncMessageIO(), lambda self, val: self._SetDisableAsyncMessageIO(val))
%}
}