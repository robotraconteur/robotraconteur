%typemap("cscode") RobotRaconteur::HardwareTransport
%{

public bool DisableMessage3
{
	get
	{
		return _GetDisableMessage3();
	}
	set
	{
		_SetDisableMessage3(value);
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

%}

%include "HardwareTransport.i"