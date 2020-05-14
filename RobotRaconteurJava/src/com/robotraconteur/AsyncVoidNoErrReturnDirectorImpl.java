package com.robotraconteur;

public class AsyncVoidNoErrReturnDirectorImpl extends AsyncVoidNoErrReturnDirector {
	Action handler_func;
	
	public AsyncVoidNoErrReturnDirectorImpl(Action handler_func)
	{
		this.handler_func=handler_func;
		
	}
	
	public void handler()
    {     
		try
		{
        this.handler_func.action();
		}
		catch (Exception e)
		{
			MessageEntry merr = new MessageEntry();
			RobotRaconteurExceptionUtil.exceptionToMessageEntry(e, merr);
			RRDirectorExceptionHelper.setError(merr,RRDirectorExceptionHelper.exceptionToStackTraceString(e));
			merr.delete();
		}
        
    }
}
