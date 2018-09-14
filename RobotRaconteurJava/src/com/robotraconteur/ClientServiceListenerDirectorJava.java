package com.robotraconteur;

public class ClientServiceListenerDirectorJava extends ClientServiceListenerDirector
{

	public int listenerid;

	private Action3<ServiceStub, ClientServiceListenerEventType, Object> listener;
	public ServiceStub stub;

	public ClientServiceListenerDirectorJava(Action3<ServiceStub, ClientServiceListenerEventType, Object> listener)
	{
		
		this.listener = listener;
		


	}

	@Override
	public void callback(int code)
	{
		try
		{
			listener.action(stub, ClientServiceListenerEventType.swigToEnum(code), null);
		}
		catch (java.lang.Exception e)
		{

		}
	}

	

}