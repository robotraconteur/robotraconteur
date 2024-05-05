%typemap(javaimports) RobotRaconteur::ServerContext
%{
import java.util.*;
%}

%typemap(javacode) RobotRaconteur::ServerContext
%{

//public delegate void ServerServiceListenerDelegate(ServerContext service, ServerServiceListenerEventType ev, object parameter);
public void setServiceAttributes(Map<String,Object> attributes)
{
  MessageElement m = new MessageElement("value",RobotRaconteurNode.s().packVarType(attributes));
  _SetServiceAttributes(m);

}

/*
public void AddServerServiceListener(ServerServiceListenerDelegate listener)
{
  ServerServiceListenerDirectorJava d = new ServerServiceListenerDirectorJava(this, listener);
  d.objectheapid = RRObjectHeap.addObject(d);

  _AddServerServiceListener(d);
}*/

public void releaseServicePath(String path)
{
	_ReleaseServicePath(path);
}

public void releaseServicePath(String path, long[] endpoints)
{
	vectoruint32 o=new vectoruint32();
	for(int i=0; i<endpoints.length; i++)
	{
		o.add(endpoints[i]);
	}

	_ReleaseServicePath(path,o);
}

%}

%typemap("javaimports") RobotRaconteur::AuthenticatedUser
%{
import java.util.*;
%}

%typemap("javacode") RobotRaconteur::AuthenticatedUser
%{

public String getUsername()
  {
	return _GetUsername();
  }

  public String[] getPrivileges()
  {
	  vectorstring in=_GetPrivileges();
	String[] o=new String[(int)in.size()];
	for (int i=0; i<o.length; i++)
	{
		o[i]=in.get(i);
	}
	return o;
  }

  public String[] getProperties()
  {
	  vectorstring in=_GetProperties();
	String[] o=new String[(int)in.size()];
	for (int i=0; i<o.length; i++)
	{
		o[i]=in.get(i);
	}
	return o;
  }

  public TimeSpec getLoginTime()
  {
	  return _GetLoginTime();

  }

  public TimeSpec getLastAccessTime()
  {
	  return _GetLastAccessTime();


  }

%}

%include "Service.i"
