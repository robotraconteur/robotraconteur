%rename(_getitem) std::vector<boost::shared_ptr<RobotRaconteur::MemberDefinition> >::getitem;
%rename(_getitemcopy) std::vector<boost::shared_ptr<RobotRaconteur::MemberDefinition> >::getitemcopy;

%typemap(csclassmodifiers) std::vector<boost::shared_ptr<RobotRaconteur::MemberDefinition> > "public partial class";

%typemap(csclassmodifiers) RobotRaconteur::RobotRaconteurVersion "public partial class";

%copyctor RobotRaconteur::TypeDefinition;
%typemap(csclassmodifiers) RobotRaconteur::TypeDefinition "public partial class";

%typemap(cscode) RobotRaconteur::TypeDefinition
%{
public MemberDefinition Member
{
	get { return MemberDefinitionUtil.SwigCast(_GetMember()); }
	set {_SetMember(value); }

}	
%}

%typemap("cscode") MemberDefinitionUtil %{

public static MemberDefinition SwigCast(MemberDefinition i)
{
	MemberDefinition o=null;
	o=ToProperty(i);
	if (o!=null) return (PropertyDefinition)o;
	o=ToFunction(i);
	if (o!=null) return (FunctionDefinition)o;
	o=ToEvent(i);
	if (o!=null) return (EventDefinition)o;
	o=ToObjRef(i);
	if (o!=null) return (ObjRefDefinition)o;
	o=ToPipe(i);
	if (o!=null) return (PipeDefinition)o;
	o=ToCallback(i);
	if (o!=null) return (CallbackDefinition)o;
	o=ToWire(i);
	if (o!=null) return (WireDefinition)o;
	o=ToMemory(i);
	if (o!=null) return (MemoryDefinition)o;
	throw new Exception("Invalid MemberDefinition");
}

%}

%include "ServiceDefinition.i"