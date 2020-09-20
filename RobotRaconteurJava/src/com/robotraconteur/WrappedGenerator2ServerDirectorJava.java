package com.robotraconteur;

public class WrappedGenerator2ServerDirectorJava<ReturnType> extends WrappedGeneratorServerDirector {

	Generator2<ReturnType> generator;
    public WrappedGenerator2ServerDirectorJava(Generator2<ReturnType> generator)
    {
        if (generator == null) throw new NullPointerException("Generator must not be null");
        this.generator = generator;
        this.setObjectheapid(RRObjectHeap.addObject(this));
    }
    
    @Override
    public MessageElement next(MessageElement m, WrappedServiceSkelAsyncAdapter async_adapter)
    {        
        try
        {            
            ReturnType r = generator.next();
            Object r1 = RobotRaconteurNode.s().packVarType(r);            
            MessageElement m_r = new MessageElement("return", r1);
            return m_r;            
        }
        catch (Exception e)
        {
        	MessageEntry merr = new MessageEntry();
        	try
            {
                RobotRaconteurExceptionUtil.exceptionToMessageEntry(e, merr);
                RRDirectorExceptionHelper.setError(merr,RRDirectorExceptionHelper.exceptionToStackTraceString(e));
                return null;
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

    @Override
    public void abort(WrappedServiceSkelAsyncAdapter async_adapter)
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
                RRDirectorExceptionHelper.setError(merr,RRDirectorExceptionHelper.exceptionToStackTraceString(e));
                return;
            }
        	finally
        	{
        		merr.finalize();
        	}
        }
    }
    
    @Override
    public void close(WrappedServiceSkelAsyncAdapter async_adapter)
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
                RRDirectorExceptionHelper.setError(merr,RRDirectorExceptionHelper.exceptionToStackTraceString(e));
                return;
            }
        	finally
        	{
        		merr.finalize();
        	}
        }
    }
}
