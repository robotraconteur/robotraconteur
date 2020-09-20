package com.robotraconteur;

public abstract class ServiceSkel extends WrappedServiceSkelDirector
{

	protected WrappedServiceSkel innerskel;

	public ServiceSkel(Object o)
	{
		uncastobj = o;
		if (o == null)
		{
			assert false;
		}




	}

	public void initCallbackServers(Object o)
	{
	}


	public final String getServicePath()
	{
		return "";
	}

	protected Object uncastobj;

	public final Object getUncastObject()
	{
		return uncastobj;
	}

	public abstract MessageElement callGetProperty(String name);

	public abstract void callSetProperty(String name, MessageElement m);

	public abstract MessageElement callFunction(String name, vectorptr_messageelement m);

	public abstract Object getSubObj(String name, String ind);


	public void registerEvents(Object obj1)
	{

	}

	public void unregisterEvents(Object obj1)
	{

	}

	public void initPipeServers(Object obj1)
	{
	}
	public void initWireServers(Object obj1)
	{
	}


	@Override
	public void releaseCastObject()
	{
		//RRObjectHeap.DeleteObject(innerskelid);
		innerskel=null;

	}

//C# TO JAVA CONVERTER WARNING: Unsigned integer types have no direct equivalent in Java:
//ORIGINAL LINE: public virtual object GetCallbackFunction(uint endpoint, string membername)
	public Object getCallbackFunction(long endpoint, String membername)
	{
		throw new MemberNotFoundException("Callback " + membername + " not found");
	}

	//public virtual MessageEntry CallMemoryFunction(MessageEntry m, Endpoint e) { throw new MemberNotFoundException("Memory " + m.MemberName + " not found"); }

	public int innerskelid;

	@Override
	public void init(WrappedServiceSkel skel)
	{	
		innerskel=skel;

		registerEvents(uncastobj);
		initPipeServers(uncastobj);
		initCallbackServers(uncastobj);
		initWireServers(uncastobj);

		if (uncastobj instanceof IRRServiceObject)
		{
			((IRRServiceObject)uncastobj).rRInitServiceObject(skel.getContext(), skel.getServicePath());
		}

	}
	@Override
	public MessageElement _CallFunction(String name, vectorptr_messageelement args, WrappedServiceSkelAsyncAdapter async_adapter)
	{		
		MessageElement r=null;
		try
		{			
			r=callFunction(name, args);
			return r ;
		}
		catch (Exception e)
		{
			MessageEntry merr = new MessageEntry();
			RobotRaconteurExceptionUtil.exceptionToMessageEntry(e, merr);
			RRDirectorExceptionHelper.setError(merr,RRDirectorExceptionHelper.exceptionToStackTraceString(e));
			merr.delete();
			return null;
		}
		finally
		{
			if (args!=null) args.delete();		
		}
	}

	@Override
	public MessageElement _CallGetProperty(String name, WrappedServiceSkelAsyncAdapter async_adapter)
	{
		MessageElement r=null;
		try
		{
			r=callGetProperty(name);
			return r;
		}
		catch (Exception e)
		{
			MessageEntry merr = new MessageEntry();
			RobotRaconteurExceptionUtil.exceptionToMessageEntry(e, merr);
			RRDirectorExceptionHelper.setError(merr,RRDirectorExceptionHelper.exceptionToStackTraceString(e));
			merr.delete();
			return null;
		}
		

	}

	@Override
	public void _CallSetProperty(String name, MessageElement m, WrappedServiceSkelAsyncAdapter async_adapter)
	{		
		try
		{			
			callSetProperty(name, m);
		}
		catch (Exception e)
		{
			MessageEntry merr = new MessageEntry();
			RobotRaconteurExceptionUtil.exceptionToMessageEntry(e, merr);
			RRDirectorExceptionHelper.setError(merr,RRDirectorExceptionHelper.exceptionToStackTraceString(e));
			merr.delete();
		}
		finally
		{
			if (m!=null) m.delete();
			
		}
	}

	public abstract String getRRType();

	@Override
	public WrappedRRObject _GetSubObj(String name, String index)
	{
		WrappedRRObject o=null;
		try
		{
			Object newobj = getSubObj(name, index);
			if (newobj == null)
			{
				return null;
			}
			ServiceSkel skel = RobotRaconteurNode.s().getServiceType(RobotRaconteurNode.splitQualifiedName(getRRType())[0]).createSkel(newobj);
			int id = RRObjectHeap.addObject(skel);
			skel.innerskelid = id;
			o = new WrappedRRObject(skel.getRRType(), skel,id);
			return o;
		}
		catch (Exception e)
		{
			MessageEntry merr = new MessageEntry();
			RobotRaconteurExceptionUtil.exceptionToMessageEntry(e, merr);
			RRDirectorExceptionHelper.setError(merr,RRDirectorExceptionHelper.exceptionToStackTraceString(e));
			merr.delete();
			return null;
		}
		
		
	}

	@Override
	public void monitorEnter(int timeout)
	{
		try
		{

			if (!(uncastobj instanceof IRobotRaconteurMonitorObject))
			{
				throw new UnsupportedOperationException("Object is not monitor lockable");
			}

			((IRobotRaconteurMonitorObject)uncastobj).robotRaconteurMonitorEnter(timeout);
		}
		catch (Exception e)
		{
			MessageEntry merr = new MessageEntry();
			RobotRaconteurExceptionUtil.exceptionToMessageEntry(e, merr);
			RRDirectorExceptionHelper.setError(merr,RRDirectorExceptionHelper.exceptionToStackTraceString(e));
			merr.delete();

		}

	}

	@Override
	public void monitorExit()
	{
		try
		{
			if (!(uncastobj instanceof IRobotRaconteurMonitorObject))
			{
				throw new UnsupportedOperationException("Object is not monitor lockable");
			}

			((IRobotRaconteurMonitorObject)uncastobj).robotRaconteurMonitorExit();
		}

		catch (Exception e)
		{
			MessageEntry merr = new MessageEntry();
			RobotRaconteurExceptionUtil.exceptionToMessageEntry(e, merr);
			RRDirectorExceptionHelper.setError(merr,RRDirectorExceptionHelper.exceptionToStackTraceString(e));
			merr.delete();

		}

	}


	@Override
	public WrappedArrayMemoryDirector _GetArrayMemory(String name)
	{
		try
		{
			return getArrayMemory(name);
		}
		catch (Exception e)
		{
			MessageEntry merr = new MessageEntry();
			RobotRaconteurExceptionUtil.exceptionToMessageEntry(e, merr);
			RRDirectorExceptionHelper.setError(merr,RRDirectorExceptionHelper.exceptionToStackTraceString(e));
			merr.delete();
			return null;
		}
	}

	@Override
	public WrappedMultiDimArrayMemoryDirector _GetMultiDimArrayMemory(String name)
	{
		try
		{
			return getMultiDimArrayMemory(name);
		}
		catch (Exception e)
		{
			MessageEntry merr = new MessageEntry();
			RobotRaconteurExceptionUtil.exceptionToMessageEntry(e, merr);
			RRDirectorExceptionHelper.setError(merr,RRDirectorExceptionHelper.exceptionToStackTraceString(e));
			merr.delete();
			return null;
		}
	}

	public WrappedArrayMemoryDirector getArrayMemory(String name)
	{
		throw new MemberNotFoundException("Member not found");
	}

	public WrappedMultiDimArrayMemoryDirector getMultiDimArrayMemory(String name)
	{
		throw new MemberNotFoundException("Member not found");
	}
	
	@Override
	public WrappedPodArrayMemoryDirector _GetPodArrayMemory(String name)
    {
        try
        {
            return getPodArrayMemory(name);
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
    }

	@Override
    public WrappedPodMultiDimArrayMemoryDirector _GetPodMultiDimArrayMemory(String name)
    {
        try
        {
            return getPodMultiDimArrayMemory(name);
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
    }

    public WrappedPodArrayMemoryDirector getPodArrayMemory(String name)
    {
        throw new MemberNotFoundException("Member not found");
    }

    public WrappedPodMultiDimArrayMemoryDirector getPodMultiDimArrayMemory(String name)
    {
        throw new MemberNotFoundException("Member not found");
    }
    
    // namedarray memories
    
    @Override
	public WrappedNamedArrayMemoryDirector _GetNamedArrayMemory(String name)
    {
        try
        {
            return getNamedArrayMemory(name);
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
    }

	@Override
    public WrappedNamedMultiDimArrayMemoryDirector _GetNamedMultiDimArrayMemory(String name)
    {
        try
        {
            return getNamedMultiDimArrayMemory(name);
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
    }

    public WrappedNamedArrayMemoryDirector getNamedArrayMemory(String name)
    {
        throw new MemberNotFoundException("Member not found");
    }

    public WrappedNamedMultiDimArrayMemoryDirector getNamedMultiDimArrayMemory(String name)
    {
        throw new MemberNotFoundException("Member not found");
    }
    
    



}