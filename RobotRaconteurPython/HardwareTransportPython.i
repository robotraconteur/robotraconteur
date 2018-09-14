%include "HardwareTransport.i"

%extend RobotRaconteur::HardwareTransport
{
%pythoncode
%{		
	__swig_getmethods__["DisableMessage3"]= lambda self: self._GetDisableMessage3()
	__swig_setmethods__["DisableMessage3"]= lambda self, val: self._SetDisableMessage3(val)
	__swig_getmethods__["DisableStringTable"]= lambda self: self._GetDisableStringTable()
	__swig_setmethods__["DisableStringTable"]= lambda self, val: self._SetDisableStringTable(val)
	__swig_getmethods__["DisableAsyncMessageIO"]= lambda self: self._GetDisableAsyncMessageIO()
	__swig_setmethods__["DisableAsyncMessageIO"]= lambda self, val: self._SetDisableAsyncMessageIO(val)	
%}
}