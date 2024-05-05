%feature("pythonappend") std::vector<boost::shared_ptr<RobotRaconteur::MemberDefinition> >::__getitem__ %{
a=MemberDefinitionUtil.ToProperty(val)
if (not a is None):
	return a
a=MemberDefinitionUtil.ToFunction(val)
if (not a is None):
	return a
a=MemberDefinitionUtil.ToEvent(val)
if (not a is None):
	return a
a=MemberDefinitionUtil.ToObjRef(val)
if (not a is None):
	return a
a=MemberDefinitionUtil.ToPipe(val)
if (not a is None):
	return a
a=MemberDefinitionUtil.ToCallback(val)
if (not a is None):
	return a
a=MemberDefinitionUtil.ToWire(val)
if (not a is None):
	return a
a=MemberDefinitionUtil.ToMemory(val)
if (not a is None):
	return a
return val

%}

%include "ServiceDefinition.i"

%extend RobotRaconteur::RobotRaconteurVersion {

%pythoncode %{
    def __eq__(self, other):
        return RobotRaconteurVersion_eq(self,other)

    def __ne___(self, other):
        return RobotRaconteurVersion_ne(self,other)

    def __gt__(self, other):
        return RobotRaconteurVersion_gt(self,other)

    def __ge__(self, other):
        return RobotRaconteurVersion_ge(self,other)

    def __lt__(self, other):
        return RobotRaconteurVersion_lt(self,other)

    def __le__(self, other):
        return RobotRaconteurVersion_le(self,other)

    def  __str__(self):
        return self.ToString()
%}

}
