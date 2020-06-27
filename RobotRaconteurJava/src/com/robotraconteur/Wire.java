package com.robotraconteur;

import java.util.Enumeration;
import java.util.Vector;



public class Wire<T>
{
	Object innerwire;

	
	private String m_Membername;
	private MemberDefinition_Direction direction;

	public final String getMemberName()
	{
		return this.m_Membername;
	}

	public final MemberDefinition_Direction getDirection()
	{
		return this.direction;
	}

	public Wire(WrappedWireClient innerwire)
	{
		
		this.innerwire = innerwire;
		m_Membername = innerwire.getMemberName();
		direction = innerwire.direction();
	}

	

	public Wire(WrappedWireServer innerwire)
	{		
		this.innerwire = innerwire;
		m_Membername = innerwire.getMemberName();
		direction = innerwire.direction();
	}

	protected void finalize() throws Throwable
	{
		if (innerwire instanceof WrappedWireServer)
		{
			//((WrappedWireServer)innerwire).setRR_Director(null);
		}
	}

	public final WireConnection connect()
	{		
		return new WireConnection(((WrappedWireClient)innerwire).connect());
	}
	
	public final void asyncConnect(Action2<WireConnection,RuntimeException> handler)
	{
		asyncConnect(handler,-1);
	}
	
	public final void asyncConnect(Action2<WireConnection,RuntimeException> handler, int timeout)
	{
		AsyncConnectDirectorImpl h = new AsyncConnectDirectorImpl(handler);
        int id = RRObjectHeap.addObject(h);
        ((WrappedWireClient)innerwire).asyncConnect(timeout,h,id);
	}

	private Action2<Wire<T>,Wire<T>.WireConnection> connect_callback = null;

	public final Action2<Wire<T>,Wire<T>.WireConnection> getWireConnectCallback()
	{
		throw new RuntimeException("Read only property");
	}
	public final void setWireConnectCallback(Action2<Wire<T>,Wire<T>.WireConnection> value)
	{
		connectdirectorclass c=new connectdirectorclass(this, value);
		int id=RRObjectHeap.addObject(c);
		((WrappedWireServer)this.innerwire).setWrappedWireConnectCallback(c,id);	
	}
		

	class peekdirectorclass extends AsyncWirePeekReturnDirector
	{
		Action3<T, TimeSpec, RuntimeException> handler_func;

		public peekdirectorclass(Action3<T, TimeSpec, RuntimeException> handler)
		{
			this.handler_func=handler;
		}

		@Override
		public void handler(MessageElement m, TimeSpec ts, HandlerErrorInfo error)
		{
			if (error.getError_code()!=0)
            {
            	
				this.handler_func.action(null,null,RobotRaconteurExceptionUtil.errorInfoToException(error));
                
                return;
            }
                                 	
            try
            {
			Object data = RobotRaconteurNode.s().unpackVarTypeDispose(m);
            	handler_func.action((T)data, ts, null);
            }
            finally
            {
            	if (m!=null) m.delete();	
            }
		}

	}

	public final T peekInValue()
	{
		TimeSpec ts=new TimeSpec();
		MessageElement m=((WrappedWireClient)this.innerwire).peekInValue(ts);
		Object data = RobotRaconteurNode.s().unpackVarTypeDispose(m);
		return (T)data;
	}

	public final T peekOutValue()
	{
		TimeSpec ts=new TimeSpec();
		MessageElement m=((WrappedWireClient)this.innerwire).peekOutValue(ts);
		Object data = RobotRaconteurNode.s().unpackVarTypeDispose(m);
		return (T)data;
	}

	public final void pokeOutValue(T value)
	{
		MessageElement m = null;
		try
		{
			m=MessageElementUtil.newMessageElementDispose("value", RobotRaconteurNode.s().packVarType(value));
			((WrappedWireClient)this.innerwire).pokeOutValue(m);
		}
		finally
		{
			if (m!=null) m.delete();
		}
	}

	public final void asyncPeekInValue(Action3<T, TimeSpec, RuntimeException> handler)
	{		
		asyncPeekInValue(handler, -1);
	}

	public final void asyncPeekInValue(Action3<T, TimeSpec, RuntimeException> handler, int timeout)
	{		
		peekdirectorclass h=new peekdirectorclass(handler);
		int id=RRObjectHeap.addObject(h);
		((WrappedWireClient)this.innerwire).asyncPeekInValue(timeout, h, id);		
	}

	public final void asyncPeekOutValue(Action3<T, TimeSpec, RuntimeException> handler)
	{		
		asyncPeekOutValue(handler, -1);
	}

	public final void asyncPeekOutValue(Action3<T, TimeSpec, RuntimeException> handler, int timeout)
	{		
		peekdirectorclass h=new peekdirectorclass(handler);
		int id=RRObjectHeap.addObject(h);
		((WrappedWireClient)this.innerwire).asyncPeekOutValue(timeout, h, id);		
	}

	public final void asyncPokeOutValue(T value, Action1<RuntimeException> handler)
	{
		asyncPokeOutValue(value, handler, -1);
	}

	public final void asyncPokeOutValue(T value, Action1<RuntimeException> handler, int timeout)
	{
		MessageElement m = null;
		try
		{
			m=MessageElementUtil.newMessageElementDispose("value", RobotRaconteurNode.s().packVarType(value));
			AsyncVoidReturnDirectorImpl h=new AsyncVoidReturnDirectorImpl(handler);
			int id=RRObjectHeap.addObject(h);
			((WrappedWireClient)this.innerwire).asyncPokeOutValue(m, timeout, h, id);
		}
		finally
		{
			if (m!=null) m.delete();
		}
	}

	/*public static interface WireConnectCallbackFunction<U>
	{
		void connect(Wire<U> wire, Wire<U>.WireConnection connection);
	}

	public static interface WireClosedCallbackFunction<U>
	{
		void closed(Wire<U>.WireConnection wire);
	}



	public static interface WireValueListener<U>
	{
		void valuechanged(Wire<U>.WireConnection connection, U value, TimeSpec time);
	}*/

	private class connectdirectorclass extends WrappedWireServerConnectDirector
	{
		private Wire<T> wire;
		private Action2<Wire<T>,Wire<T>.WireConnection> handler;
		
		public connectdirectorclass(Wire<T> wire, Action2<Wire<T>,Wire<T>.WireConnection> handler)
		{
			this.wire = wire;
			this.handler=handler;
		}

		@Override
		public void wireConnectCallback(WrappedWireConnection pipeendpoint)
		{
			try
			{				
				handler.action(wire, new WireConnection(pipeendpoint));
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


	private class AsyncConnectDirectorImpl extends AsyncWireConnectionReturnDirector {
		Action2<WireConnection,RuntimeException> handler_func=null;
		
		
		public AsyncConnectDirectorImpl(Action2<WireConnection,RuntimeException> handler_func)
		{
			this.handler_func=handler_func;
			
		}	
		
		public void handler(WrappedWireConnection m, HandlerErrorInfo error)
	    {
	       
	        if (error.getError_code()!=0)
	        {
	        	
	            
	                this.handler_func.action(null,RobotRaconteurExceptionUtil.errorInfoToException(error));
	           
	            
	            return;
	        }
	        
	        WireConnection e=null;
	       
	        try
	        {
	        	e=new WireConnection(m);
	        }
	        catch (RuntimeException ee)
	        {
	        	handler_func.action(null,ee);
	        	return;
	        }
	        
	        handler_func.action(e,null);

	    }
	}
	
	class peekcallbackclass extends WrappedWireServerPeekValueDirector
	{
		Func1<Long, T> cb;

		public peekcallbackclass(Func1<Long,T> cb)
		{
			this.cb=cb;
		}

		@Override
		public MessageElement peekValue(long ep)
		{
			T value = cb.func(ep);

			Object dat=RobotRaconteurNode.s().packVarType(value);
			MessageElement m=new MessageElement("value", dat);
			return m;
		}
	}

	class pokecallbackclass extends WrappedWireServerPokeValueDirector
	{
		Action3<T,TimeSpec,Long> cb;

		public pokecallbackclass(Action3<T,TimeSpec,Long> cb)
		{
			this.cb=cb;
		}

		@Override
		public void pokeValue(MessageElement el, TimeSpec ts, long ep)
		{
			try
			{
				T value = (T)RobotRaconteurNode.s().unpackVarType(el);						
				this.cb.action(value, ts, ep);			
			}
			finally
			{
				if (el!=null) el.delete();
			}
		}
	}
	
	public final Func1<Long,T> getPeekInValueCallback()
	{
		throw new RuntimeException("Read only property");
	}

	public final Func1<Long,T> getPeekOutValueCallback()
	{
		throw new RuntimeException("Read only property");
	}

	public final Action3<Long, T, TimeSpec> getPokeOutValueCallback()
	{
		throw new RuntimeException("Read only property");
	}

	public final void setPeekInValueCallback(Func1<Long,T> cb)
	{
		peekcallbackclass c=new peekcallbackclass(cb);
		int id=RRObjectHeap.addObject(c);
		((WrappedWireServer)innerwire).setPeekInValueCallback(c, id);
	}

	public final void setPeekOutValueCallback(Func1<Long,T> cb)
	{
		peekcallbackclass c=new peekcallbackclass(cb);
		int id=RRObjectHeap.addObject(c);
		((WrappedWireServer)innerwire).setPeekOutValueCallback(c, id);
	}

	public final void setPokeOutValueCallback(Action3<T, TimeSpec, Long> cb)
	{
		pokecallbackclass c=new pokecallbackclass(cb);
		int id=RRObjectHeap.addObject(c);
		((WrappedWireServer)innerwire).setPokeOutValueCallback(c, id);
	}

	public class WireConnection
	{
		private WrappedWireConnection innerwire;

		private directorclass director;

		public WireConnection(WrappedWireConnection innerwire)
		{
			this.innerwire = innerwire;
			director = new directorclass(this);
			
			innerwire.setRRDirector(director,RRObjectHeap.addObject(this));
		}

		protected void finalize() throws Throwable
		{
			//innerwire.setRR_Director(null);
		}

		public final long getEndpoint()
		{
			return innerwire.getEndpoint();
		}

		public final MemberDefinition_Direction getDirection()
		{
			return innerwire.direction();
		}

		public final T getInValue()
		{
			MessageElement m = innerwire.getInValue();
			
			Object data = RobotRaconteurNode.s().unpackVarTypeDispose(m);
			return (T)data;
			
		}


		public final T getOutValue()
		{
			MessageElement m = innerwire.getOutValue();
			Object data = RobotRaconteurNode.s().unpackVarTypeDispose(m);
			return (T)data;
		}

		public final void setOutValue(T value)
		{
			MessageElement m = null;
			try
			{
				m=MessageElementUtil.newMessageElementDispose("value", RobotRaconteurNode.s().packVarType(value));
				innerwire.setOutValue(m);
			}
			finally
			{
				if (m!=null) m.delete();
			}
			

		}


		public final TimeSpec getLastValueReceivedTime()
		{
			return innerwire.getLastValueReceivedTime();
		}


		public final TimeSpec getLastValueSentTime()
		{
			return innerwire.getLastValueReceivedTime();
		}

		public final boolean getInValueValid()
		{
			return innerwire.getInValueValid();
	
		}
		
		public final boolean getOutValueValid()
		{
			return innerwire.getOutValueValid();
		}

		public final boolean waitInValueValid(int timeout)
		{
			return innerwire.waitInValueValid(timeout);
		}

		public final boolean waitInValueValid()
		{
			return waitInValueValid(-1);
		}

		public final boolean waitOutValueValid(int timeout)
		{
			return innerwire.waitOutValueValid(timeout);
		}

		public final boolean waitOutValueValid()
		{
			return waitOutValueValid(-1);
		}

		public final void close()
		{
			innerwire.close();
		}

		public final void asyncClose(Action1<RuntimeException> handler)
		{
			asyncClose(handler,-1);
			
		}
		
		public final void asyncClose(Action1<RuntimeException> handler, int timeout)
		{
			AsyncVoidReturnDirectorImpl h = new AsyncVoidReturnDirectorImpl(handler);
            int id1 = RRObjectHeap.addObject(h);
            innerwire.asyncClose(timeout, h, id1);
		}

		private Action1<Wire<T>.WireConnection> close_callback;

		public final Action1<Wire<T>.WireConnection> getWireCloseCallback()
		{
			return close_callback;
		}
		public final void setWireCloseCallback(Action1<Wire<T>.WireConnection> value)
		{
			close_callback =value;
			
		}
		
		private Vector<Action3<Wire<T>.WireConnection, T, TimeSpec>> wirelisteners=new Vector<Action3<Wire<T>.WireConnection, T, TimeSpec>>();
		public void addWireValueListener(Action3<Wire<T>.WireConnection, T, TimeSpec> listener)
		{
			synchronized(wirelisteners)
			{
				wirelisteners.add(listener);
			}
		}
		public void removeWireValueListener(Action3<Wire<T>.WireConnection, T, TimeSpec> listener)
		{
			synchronized(wirelisteners)
			{
				wirelisteners.remove(listener);
			}
		}
		
		public boolean getIgnoreInValue()
		{
			return innerwire.getIgnoreInValue();
		}
		
		public void setIgnoreInValue(boolean ignore)
		{
			innerwire.setIgnoreInValue(ignore);
		}

		public int getInValueLifespan()
		{
			return innerwire.getInValueLifespan();
		}

		public void setInValueLifespan(int millis)
		{
			innerwire.setInValueLifespan(millis);
		}

		public int getOutValueLifespan()
		{
			return innerwire.getOutValueLifespan();
		}

		public void setOutValueLifespan(int millis)
		{
			innerwire.setOutValueLifespan(millis);
		}
		
		private class directorclass extends WrappedWireConnectionDirector
		{

			private WireConnection wire;

			public directorclass(WireConnection wire)
			{
				this.wire = wire;

			}

			@Override
			public void wireConnectionClosedCallback()
			{
				try
				{
					if (wire.getWireCloseCallback() == null)
					{
						return;
					}
					wire.getWireCloseCallback().action(wire);
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
			public void wireValueChanged(MessageElement m, TimeSpec time)
			{
				try
				{					
					Object data = RobotRaconteurNode.s().unpackVarTypeDispose(m);
					
					synchronized (wire.wirelisteners)
					{
					for(Enumeration<Action3<Wire<T>.WireConnection, T, TimeSpec>> e= wire.wirelisteners.elements(); e.hasMoreElements(); )
					{
						Action3<Wire<T>.WireConnection, T, TimeSpec> ee=e.nextElement();
						ee.action(wire,(T)data,time);
						
					}
					}
					
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
	}

}