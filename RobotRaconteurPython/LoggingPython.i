%extend RobotRaconteur::RRLogRecord
{
%pythoncode %{
Node = property(lambda self: self._GetNode(), lambda self,node: self._SetNode(node))
%}
}

%rename(__str__) RobotRaconteur::RRLogRecord::ToString();

%include "Logging.i"