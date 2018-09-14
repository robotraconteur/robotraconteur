package com.robotraconteur;

public class AsyncVoidReturnDirectorImpl extends AsyncVoidReturnDirector {
	Action1<RuntimeException> handler_func;
	
	public AsyncVoidReturnDirectorImpl(Action1<RuntimeException> handler_func)
	{
		this.handler_func=handler_func;
		
	}
	
	public void handler(long error_code, String errorname, String errormessage)
    {
       try
       {
        if (error_code!=0)
        {
        	
            MessageEntry merr=new MessageEntry();
           
                this.handler_func.action(RobotRaconteurExceptionUtil.errorCodeToException(MessageErrorType.swigToEnum((int)error_code),errorname,errormessage));
           
            
            return;
        }
        
        this.handler_func.action(null);
       }
		catch (Exception e)
		{
			MessageEntry merr = new MessageEntry();
			RobotRaconteurExceptionUtil.exceptionToMessageEntry(e, merr);
			RRDirectorExceptionHelper.setError(merr);
			merr.delete();
		}
        
    }
}
