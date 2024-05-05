%rename(_doGet) std::vector<boost::shared_ptr<RobotRaconteur::MemberDefinition> >::doGet;

%typemap(javainterfaces) RobotRaconteur::RobotRaconteurVersion  "java.lang.Comparable<RobotRaconteurVersion>";

%typemap("javacode") RobotRaconteur::RobotRaconteurVersion %{

public boolean equals(Object o)
{
	if (o instanceof RobotRaconteurVersion)
	{
		return RobotRaconteurJava.RobotRaconteurVersion_eq(this, (RobotRaconteurVersion)o);
	}
	return false;
}

public int compareTo(RobotRaconteurVersion o)
{
	if (RobotRaconteurJava.RobotRaconteurVersion_eq(this,o)) return 0;
	if (RobotRaconteurJava.RobotRaconteurVersion_lt(this,o)) return -1;
	if (RobotRaconteurJava.RobotRaconteurVersion_gt(this,o)) return 1;
	throw new IllegalArgumentException("");
}

%}

%typemap("javacode") std::vector<boost::shared_ptr<RobotRaconteur::MemberDefinition> >
%{
public MemberDefinition doGet(int i)
{
	return MemberDefinitionUtil.swigCast(_doGet(i));
}
%}

%typemap("javacode") MemberDefinitionUtil %{

public static MemberDefinition swigCast(MemberDefinition i)
{
	MemberDefinition o=null;
	o=toProperty(i);
	if (o!=null) return (PropertyDefinition)o;
	o=toFunction(i);
	if (o!=null) return (FunctionDefinition)o;
	o=toEvent(i);
	if (o!=null) return (EventDefinition)o;
	o=toObjRef(i);
	if (o!=null) return (ObjRefDefinition)o;
	o=toPipe(i);
	if (o!=null) return (PipeDefinition)o;
	o=toCallback(i);
	if (o!=null) return (CallbackDefinition)o;
	o=toWire(i);
	if (o!=null) return (WireDefinition)o;
	o=toMemory(i);
	if (o!=null) return (MemoryDefinition)o;
	throw new RuntimeException (new Exception("Invalid MemberDefinition"));

}

%}

%include "ServiceDefinition.i"
