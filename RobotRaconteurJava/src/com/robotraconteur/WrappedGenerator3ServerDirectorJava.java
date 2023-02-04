package com.robotraconteur;

public class WrappedGenerator3ServerDirectorJava<ParamType> extends WrappedGeneratorServerDirector
{
    Generator3<ParamType> generator;
    public WrappedGenerator3ServerDirectorJava(Generator3<ParamType> generator)
    {
        if (generator == null)
            throw new NullPointerException("Generator must not be null");
        this.generator = generator;
        this.setObjectheapid(RRObjectHeap.addObject(this));
    }

    @Override public MessageElement next(MessageElement m, WrappedServiceSkelAsyncAdapter async_adapter)
    {
        MessageEntry merr = null;
        try
        {
            ParamType p = (ParamType)RobotRaconteurNode.s().unpackVarType(m);
            generator.next(p);
            return null;
        }
        catch (Exception e)
        {
            merr = new MessageEntry();
            RobotRaconteurExceptionUtil.exceptionToMessageEntry(e, merr);
            RRDirectorExceptionHelper.setError(merr, RRDirectorExceptionHelper.exceptionToStackTraceString(e));
            return null;
        }
        finally
        {
            if (m != null)
                m.finalize();
            if (merr != null)
                merr.finalize();
        }
    }

    @Override public void abort(WrappedServiceSkelAsyncAdapter async_adapter)
    {
        try
        {
            generator.abort();
        }
        catch (Exception e)
        {
            MessageEntry merr = new MessageEntry();
            try
            {
                RobotRaconteurExceptionUtil.exceptionToMessageEntry(e, merr);
                RRDirectorExceptionHelper.setError(merr, RRDirectorExceptionHelper.exceptionToStackTraceString(e));
                return;
            }
            finally
            {
                merr.finalize();
            }
        }
    }

    @Override public void close(WrappedServiceSkelAsyncAdapter async_adapter)
    {
        try
        {
            generator.close();
        }
        catch (Exception e)
        {
            MessageEntry merr = new MessageEntry();
            try
            {
                RobotRaconteurExceptionUtil.exceptionToMessageEntry(e, merr);
                RRDirectorExceptionHelper.setError(merr, RRDirectorExceptionHelper.exceptionToStackTraceString(e));
                return;
            }
            finally
            {
                merr.finalize();
            }
        }
    }
}
