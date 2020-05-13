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

%csmethodmodifiers RobotRaconteur::RRLogRecord::ToString "public override";

%include "Logging.i"