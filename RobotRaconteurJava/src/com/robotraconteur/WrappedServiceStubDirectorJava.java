package com.robotraconteur;
import java.lang.ref.*;

public class WrappedServiceStubDirectorJava extends WrappedServiceStubDirector
{
	private WeakReference<ServiceStub> stub;

	public WrappedServiceStubDirectorJava(ServiceStub stub)
	{
		this.stub = new WeakReference<ServiceStub>(stub);
	}

	@Override
	public MessageElement callbackCall(String CallbackName, vectorptr_messageelement args)
	{
		try
		{			
			MessageElement ret =null;
			try
			{
			ret = ((ServiceStub)stub.get()).callbackCall(CallbackName, args);
			return ret;
			}
			finally
			{
				if (args!=null) args.delete();				
			}
		}
		catch (Exception e)
		{
			MessageEntry merr = new MessageEntry();
			RobotRaconteurExceptionUtil.exceptionToMessageEntry(e, merr);
			RRDirectorExceptionHelper.setError(merr,RRDirectorExceptionHelper.exceptionToStackTraceString(e));
			merr.delete();
			return null;
		}
	}

	@Override
	public void dispatchEvent(String EventName, vectorptr_messageelement args)
	{
		try
		{			
			try
			{
			((ServiceStub)stub.get()).dispatchEvent(EventName, args);
			}
			finally
			{
				if (args!=null) args.delete();
			
			}
		}
		catch (java.lang.Exception e)
		{
		}
	}


}