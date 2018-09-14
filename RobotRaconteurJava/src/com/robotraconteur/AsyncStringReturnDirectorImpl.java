package com.robotraconteur;

public class AsyncStringReturnDirectorImpl extends AsyncStringReturnDirector {
	Action2<String,RuntimeException> handler_func=null;
	
	
	public AsyncStringReturnDirectorImpl(Action2<String,RuntimeException> handler_func)
	{
		this.handler_func=handler_func;
		
	}	
	
	@Override
	public void handler(String s, long error_code, String errorname, String errormessage)
    {
       try
       {
        if (error_code!=0)
        {
        	
            
               
                this.handler_func.action(null,RobotRaconteurExceptionUtil.errorCodeToException(MessageErrorType.swigToEnum((int)error_code),errorname,errormessage));
            
            
            return;
        }
                
        handler_func.action(s,null);
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
