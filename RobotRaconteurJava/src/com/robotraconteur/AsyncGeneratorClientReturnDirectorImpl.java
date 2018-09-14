package com.robotraconteur;

class AsyncGeneratorClientReturnDirectorImpl extends AsyncGeneratorClientReturnDirector
{

    protected Action3<WrappedGeneratorClient, RuntimeException, Object> handler_func;
    protected Object param;

    public AsyncGeneratorClientReturnDirectorImpl(Action3<WrappedGeneratorClient, RuntimeException, Object> handler_func, Object param)
    {
        this.handler_func = handler_func;
        this.param = param;
    }

    public void handler(WrappedGeneratorClient m, long error_code, String errorname, String errormessage)
    {
        
            try
            {
                if (error_code != 0)
                {
                    MessageEntry merr = new MessageEntry();
                    try
                    {

                        this.handler_func.action(null, RobotRaconteurExceptionUtil.errorCodeToException(MessageErrorType.swigToEnum((int)error_code), errorname, errormessage), param);
                        return;
                    }
                    finally
                    {
                    	merr.finalize();
                    }
                }

                handler_func.action(m, null, param);

            }
            catch (Exception e)
            {
                MessageEntry merr = new MessageEntry();
                try
                {
                    RobotRaconteurExceptionUtil.exceptionToMessageEntry(e, merr);
                    RRDirectorExceptionHelper.setError(merr);
                }
                finally
                {
                	merr.finalize();
                }
            }
            finally
            {
            	if (m!=null) m.finalize();            	
            }
        

    }
}