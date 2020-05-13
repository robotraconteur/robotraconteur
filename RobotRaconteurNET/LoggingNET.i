%typemap("cscode") RobotRaconteur::RRLogRecord
%{
public RobotRaconteurNode Node
{
	get
	{
		return _GetNode();
	}
	set
	{
		_SetNode(value);
	}

}
%}
%include "Logging.i"