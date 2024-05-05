%typemap(cscode) RobotRaconteur::ServerContext
%{
public static string CurrentServicePath { get { return GetCurrentServicePath(); } }
public static ServerContext CurrentServerContext { get { return GetCurrentServerContext(); } }

public delegate void ServerServiceListenerDelegate(ServerContext service, ServerServiceListenerEventType ev, object parameter);
public void SetServiceAttributes(System.Collections.Generic.Dictionary<string, object> attributes)
{
  MessageElement m = new MessageElement("value",RobotRaconteurNode.s.PackVarType(attributes));
  _SetServiceAttributes(m);

}

public void AddServerServiceListener(ServerServiceListenerDelegate listener)
{
  ServerServiceListenerDirectorNET d = new ServerServiceListenerDirectorNET(this, listener);
  d.objectheapid = RRObjectHeap.AddObject(d);

  _AddServerServiceListener(d);
}

public void ReleaseServicePath(string path)
{
	_ReleaseServicePath(path);
}

public void ReleaseServicePath(string path, uint[] endpoints)
{
	vectoruint32 o=new vectoruint32();
	foreach(uint e in endpoints)
	{
		o.Add(e);
	}

	_ReleaseServicePath(path,o);
}

%}

%typemap(csclassmodifiers) RobotRaconteur::AuthenticatedUser "public partial class";

%typemap(cscode) RobotRaconteur::ServerEndpoint
%{
public static uint CurrentEndpoint { get { return GetCurrentEndpoint(); } }

%}

%typemap(csclassmodifiers) RobotRaconteur::WrappedServiceSkelAsyncAdapter "public partial class";

%include "Service.i"
