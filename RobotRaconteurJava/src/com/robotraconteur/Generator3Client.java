package com.robotraconteur;

import com.robotraconteur.ServiceStub.AsyncRequestDirectorImpl;

public class Generator3Client<ParamType> implements Generator3<ParamType>
{
    protected WrappedGeneratorClient inner_gen;

    public Generator3Client(WrappedGeneratorClient inner_gen)
    {
        this.inner_gen = inner_gen;
    }

    public void next(ParamType param)
    {
        Object param1 = RobotRaconteurNode.s().packVarType(param);
        try
        {
        	
            MessageElement m = new MessageElement("parameter", param1);            
            try
            {
                inner_gen.next(m);
            }
            finally
            {
            	m.finalize();            	            	
            }
        }
        finally
        {
            if (param1 instanceof MessageElementData)
            {
            	((MessageElementData)param1).finalize();
            }
        }
    }
    public void asyncNext(ParamType param, Action1<RuntimeException> handler, int timeout)
    {
    	Object param1 = RobotRaconteurNode.s().packVarType(param);
        try
        {
        	
            MessageElement m = new MessageElement("parameter", param1);
            MessageElement m2 = null;
            try
            {
                m2 = inner_gen.next(m);                
                AsyncRequestDirectorImpl d = new AsyncRequestDirectorImpl(new endAsyncNext(), handler);
                int id = RRObjectHeap.addObject(d);
                inner_gen.asyncNext(m, timeout, d, id);                  
            }
            finally
            {
            	m.finalize();
            	if (m2 != null) m2.finalize();            	
            }
        }
        finally
        {
            if (param1 instanceof MessageElementData)
            {
            	((MessageElementData)param1).finalize();
            }
        }    	         
    }

    protected class endAsyncNext implements Action3<MessageElement, RuntimeException, Object>
    {
    public void action(MessageElement m, RuntimeException err, Object p)
    {
        Action1<RuntimeException> h = (Action1<RuntimeException>)p;
        h.action(err);  
    }
    }

    public void abort()
    {
        inner_gen.abort();
    }
    public void asyncAbort(Action1<RuntimeException> handler, int timeout)
    {
        AsyncVoidReturnDirectorImpl h = new AsyncVoidReturnDirectorImpl(handler);
        int id = RRObjectHeap.addObject(h);
        inner_gen.asyncAbort(timeout, h, id);
    }
    
    public void close()
    {
        inner_gen.close();
    }
    public void asyncClose(Action1<RuntimeException> handler, int timeout)
    {
        AsyncVoidReturnDirectorImpl h = new AsyncVoidReturnDirectorImpl(handler);
        int id = RRObjectHeap.addObject(h);
        inner_gen.asyncClose(timeout, h, id);
    }
}