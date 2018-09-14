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
	}
	@Override
	public MessageElement _CallFunction(String name, vectorptr_messageelement args)
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
			RRDirectorExceptionHelper.setError(merr);
			merr.delete();
			return null;
		}
		finally
		{
			if (args!=null) args.delete();		
		}
	}

	@Override
	public MessageElement _CallGetProperty(String name)
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
			RRDirectorExceptionHelper.setError(merr);
			merr.delete();
			return null;
		}
		

	}

	@Override
	public void _CallSetProperty(String name, MessageElement m)
	{		
		try
		{			
			callSetProperty(name, m);
		}
		catch (Exception e)
		{
			MessageEntry merr = new MessageEntry();
			RobotRaconteurExceptionUtil.exceptionToMessageEntry(e, merr);
			RRDirectorExceptionHelper.setError(merr);
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
			RRDirectorExceptionHelper.setError(merr);
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
			RRDirectorExceptionHelper.setError(merr);
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
			RRDirectorExceptionHelper.setError(merr);
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
			RRDirectorExceptionHelper.setError(merr);
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
			RRDirectorExceptionHelper.setError(merr);
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
	public WrappedCStructureArrayMemoryDirector _GetCStructureArrayMemory(String name)
    {
        try
        {
            return getCStructureArrayMemory(name);
        }
        catch (Exception e)
        {
        	MessageEntry merr = new MessageEntry();
            try
            {
                RobotRaconteurExceptionUtil.exceptionToMessageEntry(e, merr);
                RRDirectorExceptionHelper.setError(merr);
                return null;
            }
            finally
            {
            	merr.finalize();
            }
        }
    }

	@Override
    public WrappedCStructureMultiDimArrayMemoryDirector _GetCStructureMultiDimArrayMemory(String name)
    {
        try
        {
            return getCStructureMultiDimArrayMemory(name);
        }
        catch (Exception e)
        {
        	MessageEntry merr = new MessageEntry();
            try
            {
                RobotRaconteurExceptionUtil.exceptionToMessageEntry(e, merr);
                RRDirectorExceptionHelper.setError(merr);
                return null;
            }
            finally
            {
            	merr.finalize();
            }
        }
    }

    public WrappedCStructureArrayMemoryDirector getCStructureArrayMemory(String name)
    {
        throw new MemberNotFoundException("Member not found");
    }

    public WrappedCStructureMultiDimArrayMemoryDirector getCStructureMultiDimArrayMemory(String name)
    {
        throw new MemberNotFoundException("Member not found");
    }



}