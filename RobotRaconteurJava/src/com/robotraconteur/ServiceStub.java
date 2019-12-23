package com.robotraconteur;

public abstract class ServiceStub
{
	public WrappedServiceStub rr_innerstub;
	private WrappedServiceStubDirectorJava director;

	private Object objref_lock=new Object();
	
	public ServiceStub(WrappedServiceStub innerstub)
	{
		director = new WrappedServiceStubDirectorJava(this);

		rr_innerstub = innerstub;
		//director.swigReleaseOwnership();
		innerstub.setRRDirector(director,RRObjectHeap.addObject(this));
		
		
	}

	protected void finalize() throws Throwable
	{
		//rr_innerstub.setRR_Director(null);
	}

	public abstract void dispatchEvent(String membername, vectorptr_messageelement m);

	public final Object findObjRef(String n)
	{
		WrappedServiceStub s = rr_innerstub.findObjRef(n);
		synchronized(objref_lock)
		{		
			if (s.getObjectHeapID()!=0)
			{
				return RRObjectHeap.getObject(s.getObjectHeapID());
				
			}
			else
			{
				ServiceFactory f = RobotRaconteurNode.s().getServiceType(s.getRR_objecttype().getServiceDefinition().getName());
				return f.createStub(s);
			}
		}
	}

	public final Object findObjRef(String n, String i)
	{
		WrappedServiceStub s = rr_innerstub.findObjRef(n,i);
		synchronized(objref_lock)
		{		
			if (s.getObjectHeapID()!=0)
			{
				return RRObjectHeap.getObject(s.getObjectHeapID());
				
			}
			else
			{
				ServiceFactory f = RobotRaconteurNode.s().getServiceType(s.getRR_objecttype().getServiceDefinition().getName());
				return f.createStub(s);
			}
		}
	}

	public final Object findObjRefTyped(String n, String objecttype)
	{
		WrappedServiceStub s = rr_innerstub.findObjRefTyped(n,objecttype);
		synchronized(objref_lock)
		{		
			if (s.getObjectHeapID()!=0)
			{
				return RRObjectHeap.getObject(s.getObjectHeapID());
				
			}
			else
			{
				ServiceFactory f = RobotRaconteurNode.s().getServiceType(s.getRR_objecttype().getServiceDefinition().getName());
				return f.createStub(s);
			}
		}
	}

	public final Object findObjRefTyped(String n, String i, String objecttype)
	{
		WrappedServiceStub s = rr_innerstub.findObjRefTyped(n,i,objecttype);
		synchronized(objref_lock)
		{		
			if (s.getObjectHeapID()!=0)
			{
				return RRObjectHeap.getObject(s.getObjectHeapID());
				
			}
			else
			{
				ServiceFactory f = RobotRaconteurNode.s().getServiceType(s.getRR_objecttype().getServiceDefinition().getName());
				return f.createStub(s);
			}
		}
	}
	
	public MessageElement callbackCall(String membername, vectorptr_messageelement m)
	{
		throw new MemberNotFoundException("Member not found");
	}
	
	public static class AsyncRequestDirectorImpl extends AsyncRequestDirector
    {

        protected Action3<MessageElement,RuntimeException,Object> handler_func;
        protected Object param;

        public AsyncRequestDirectorImpl(Action3<MessageElement, RuntimeException,Object> handler_func, Object param)
        {
            this.handler_func = handler_func;
            this.param = param;
        }
		
		@Override
        public void handler(MessageElement m, HandlerErrorInfo error)
        {
           
            if (error.getError_code()!=0)
            {
            	
				this.handler_func.action(null,RobotRaconteurExceptionUtil.errorInfoToException(error),param);
                
                return;
            }
                                 	
            try
            {
            	handler_func.action(m, null,param);
            }
            finally
            {
            	if (m!=null) m.delete();	
            }
            	
            

        }
    }

    protected void rr_async_PropertyGet(String name, Action3<MessageElement, RuntimeException, Object> handler, Object param, int timeout)
    {
        AsyncRequestDirectorImpl d = new AsyncRequestDirectorImpl(handler, param);
        int id=RRObjectHeap.addObject(d);

        rr_innerstub.async_PropertyGet(name, timeout, d,id);

    }

    protected void rr_async_PropertySet(String name, MessageElement value, Action3<MessageElement,RuntimeException, Object> handler, Object param, int timeout)
    {
        AsyncRequestDirectorImpl d = new AsyncRequestDirectorImpl(handler, param);
        int id = RRObjectHeap.addObject(d);

        rr_innerstub.async_PropertySet(name, value, timeout, d, id);

    }

    protected void rr_async_FunctionCall(String name, vectorptr_messageelement p, Action3<MessageElement, RuntimeException, Object> handler, Object param, int timeout)
    {
        AsyncRequestDirectorImpl d = new AsyncRequestDirectorImpl(handler, param);
        int id = RRObjectHeap.addObject(d);

        rr_innerstub.async_FunctionCall(name, p, timeout, d, id);
    }
    
    protected void rr_async_GeneratorFunctionCall(String name, vectorptr_messageelement p, Action3<WrappedGeneratorClient, RuntimeException, Object> handler, Object param, int timeout)
    {
        AsyncGeneratorClientReturnDirectorImpl d = new AsyncGeneratorClientReturnDirectorImpl(handler, param);
        int id = RRObjectHeap.addObject(d);

        rr_innerstub.async_GeneratorFunctionCall(name, p, timeout, d, id);
    }

    public void asyncFindObjRef(String n, Action2<Object, RuntimeException> handler)
    {
    	asyncFindObjRef(n,handler,-1);    
    }
    
    public void asyncFindObjRef(String n, Action2<Object, RuntimeException> handler, int timeout)
    {
        AsyncStubReturnDirectorImpl<Object> d = new AsyncStubReturnDirectorImpl<Object>(handler);
        int id1=RRObjectHeap.addObject(d);
        rr_innerstub.async_FindObjRef(n, timeout, d, id1);
    }

    public void asyncFindObjRef(String n, String i, Action2<Object, RuntimeException> handler)
    {
    	asyncFindObjRef(n,i,handler,-1);
    }
    
    public void asyncFindObjRef(String n, String i, Action2<Object, RuntimeException> handler, int timeout)
    {
        AsyncStubReturnDirectorImpl<Object> d = new AsyncStubReturnDirectorImpl<Object>(handler);
        int id1 = RRObjectHeap.addObject(d);
        rr_innerstub.async_FindObjRef(n, i, timeout, d, id1);
    }

    public <T> void asyncFindObjRefTyped(String n, String type, Action2<T, RuntimeException> handler)
    {
    	asyncFindObjRefTyped(n,type,handler,-1);
    }
    
    public <T> void asyncFindObjRefTyped(String n, String type, Action2<T, RuntimeException> handler, int timeout)
    {
        AsyncStubReturnDirectorImpl<T> d = new AsyncStubReturnDirectorImpl<T>(handler);
        int id1 = RRObjectHeap.addObject(d);
        rr_innerstub.async_FindObjRefTyped(n, type, timeout, d, id1);
    }

    public <T> void asyncFindObjRefTyped(String n, String i, String type, Action2<T, RuntimeException> handler)
    {
    	asyncFindObjRefTyped(n,i,type,handler,-1);
    }
    
    public <T> void asyncFindObjRefTyped(String n, String i, String type, Action2<T, RuntimeException> handler, int timeout)
    {
        AsyncStubReturnDirectorImpl<T> d = new AsyncStubReturnDirectorImpl<T>(handler);
        int id1 = RRObjectHeap.addObject(d);
        rr_innerstub.async_FindObjRefTyped(n, i, type, timeout, d, id1);
    }
    

}