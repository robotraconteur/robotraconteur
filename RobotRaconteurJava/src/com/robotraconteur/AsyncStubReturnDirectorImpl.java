package com.robotraconteur;

public class AsyncStubReturnDirectorImpl<T> extends AsyncStubReturnDirector {
	Action2<T,RuntimeException> handler_func=null;
	ServiceFactory factory=null;
	
	public AsyncStubReturnDirectorImpl(Action2<T,RuntimeException> handler_func)
	{
		this.handler_func=handler_func;
		this.factory=null;		
	}
	
	public AsyncStubReturnDirectorImpl(Action2<T,RuntimeException> handler_func, ServiceFactory f)
	{
		this.handler_func=handler_func;
		this.factory=f;		
	}
	
	@Override
	public void handler(WrappedServiceStub innerstub, HandlerErrorInfo error)
    {
       try
       {
        if (error.getError_code()!=0)
        {
        	
           
                this.handler_func.action(null,RobotRaconteurExceptionUtil.errorInfoToException(error));
            
            
            return;
        }
        
        Object s=null;
        T s1=null;
        try
        {        	
        	int id1=innerstub.getObjectHeapID();
        	if (id1!=0)
        	{
        		handler_func.action((T)RRObjectHeap.getObject(id1),null);
        		return;
        	}
        	
        	ServiceFactory f;
        	if (factory==null)
        	{
        		f=RobotRaconteurNode.s().getServiceType(innerstub.getRR_objecttype().getServiceDefinition().getName());
        	}
        	else
        	{
        		f=factory;
        	}
        	s=f.createStub(innerstub);
        	s1=(T)s;
        	
        }
        catch (RuntimeException e)
        {
        	handler_func.action(null,e);
        	return;
        }
        
        handler_func.action(s1,null);
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

