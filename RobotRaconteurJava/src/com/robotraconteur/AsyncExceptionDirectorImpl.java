package com.robotraconteur;

class AsyncExceptionDirectorImpl extends AsyncVoidReturnDirector
{
    Action1<RuntimeException> handler_func = null;

    public AsyncExceptionDirectorImpl(Action1<RuntimeException> handler_func)
    {
        this.handler_func = handler_func;
    }

    @Override public void handler(HandlerErrorInfo error)
    {
        try
        {
            if (error.getError_code() != 0)
            {

                this.handler_func.action(RobotRaconteurExceptionUtil.errorInfoToException(error));

                return;
            }

            handler_func.action(new RuntimeException("Unknown exception"));
        }
        catch (Exception e)
        {
            MessageEntry merr = new MessageEntry();
            RobotRaconteurExceptionUtil.exceptionToMessageEntry(e, merr);
            RRDirectorExceptionHelper.setError(merr, RRDirectorExceptionHelper.exceptionToStackTraceString(e));
            merr.delete();
        }
    }
}
