package com.robotraconteur;

class AsyncExceptionDirectorImpl extends AsyncVoidReturnDirector {
Action1<RuntimeException> handler_func=null;
	
	
	public AsyncExceptionDirectorImpl(Action1<RuntimeException> handler_func)
	{
		this.handler_func=handler_func;
		
	}	
	
	@Override
	public void handler(long error_code, String errorname, String errormessage)
    {
        try
        {
        if (error_code!=0)
        {
        	
           
            this.handler_func.action(RobotRaconteurExceptionUtil.errorCodeToException(MessageErrorType.swigToEnum((int)error_code),errorname,errormessage));
            
            
            return;
        }
                        
        handler_func.action(new RuntimeException("Unknown exception"));
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
