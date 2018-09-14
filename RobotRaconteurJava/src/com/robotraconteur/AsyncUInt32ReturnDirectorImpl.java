package com.robotraconteur;

public class AsyncUInt32ReturnDirectorImpl extends AsyncUInt32ReturnDirector {
Action2<Long,RuntimeException> handler_func=null;
	
	
	public AsyncUInt32ReturnDirectorImpl(Action2<Long,RuntimeException> handler_func)
	{
		this.handler_func=handler_func;
		
	}	
	
	@Override
	public void handler(long m, long error_code, String errorname, String errormessage)
    {
       try
       {
        if (error_code!=0)
        {
        	
            
                this.handler_func.action(null,RobotRaconteurExceptionUtil.errorCodeToException(MessageErrorType.swigToEnum((int)error_code),errorname,errormessage));
           
            
            return;
        }
        
        Long s=new Long(m);       
        
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
