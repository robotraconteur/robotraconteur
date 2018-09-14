
%typemap("javacode") RobotRaconteur::HardwareTransport
%{
static {
	try
	{
	java.lang.Class cls=java.lang.Class.forName("com.robotraconteur.AndroidHardwareDirectorImpl");
	if (cls != null)
	{
	java.lang.reflect.Method m=cls.getMethod("init", cls);
	m.invoke(null,null);
	}
	}
	catch (Exception e) {}
		
}
%}

%include "HardwareTransport.i"