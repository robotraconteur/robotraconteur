%typemap("cscode") RobotRaconteur::HardwareTransport
%{

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

%}

%include "HardwareTransport.i"