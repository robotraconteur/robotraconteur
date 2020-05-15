package com.robotraconteur;

public class AsyncStringReturnDirectorImpl extends AsyncStringReturnDirector {
	Action2<String,RuntimeException> handler_func=null;
	
	
	public AsyncStringReturnDirectorImpl(Action2<String,RuntimeException> handler_func)
	{
		this.handler_func=handler_func;
		
	}	
	
	@Override
	public void handler(String s, HandlerErrorInfo error)
    {
       try
       {
        if (error.getError_code()!=0)
        {
        	
            
               
                this.handler_func.action(null,RobotRaconteurExceptionUtil.errorInfoToException(error));
            
            
            return;
        }
                
        handler_func.action(s,null);
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
