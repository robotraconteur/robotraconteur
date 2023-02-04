package com.robotraconteur;

public class WrappedGenerator1ServerDirectorJava<ReturnType, ParamType> extends WrappedGeneratorServerDirector
{

    Generator1<ReturnType, ParamType> generator;
    public WrappedGenerator1ServerDirectorJava(Generator1<ReturnType, ParamType> generator)
    {
        if (generator == null)
            throw new NullPointerException("Generator must not be null");
        this.generator = generator;
        this.setObjectheapid(RRObjectHeap.addObject(this));
    }

    @Override public MessageElement next(MessageElement m, WrappedServiceSkelAsyncAdapter async_adapter)
    {
        try
        {
            ParamType p = (ParamType)RobotRaconteurNode.s().unpackVarType(m);
            ReturnType r = generator.next(p);
            Object r1 = RobotRaconteurNode.s().packVarType(r);
            try
            {
                MessageElement m_r = new MessageElement("return", r1);
                return m_r;
            }
            finally
            {
                if (p instanceof MessageElementData)
                {
                    ((MessageElementData)p).finalize();
                }
            }
        }
        catch (Exception e)
        {
            MessageEntry merr = new MessageEntry();
            try
            {
                RobotRaconteurExceptionUtil.exceptionToMessageEntry(e, merr);
                RRDirectorExceptionHelper.setError(merr, RRDirectorExceptionHelper.exceptionToStackTraceString(e));
                return null;
            }
            finally
            {
                merr.finalize();
            }
        }
        finally
        {
            if (m != null)
                m.finalize();
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
