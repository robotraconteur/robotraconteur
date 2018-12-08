package com.robotraconteur;

import java.lang.ref.WeakReference;
import java.util.*;

public class WireBroadcaster<T> {

	protected WrappedWireBroadcaster innerwire;
	protected TypeDefinition type;
	protected Wire<T> wire;
	
	static class WrappedWireBroadcasterPredicateDirectorJava extends WrappedWireBroadcasterPredicateDirector
    {
        Func1<Long, Boolean> f;

        public WrappedWireBroadcasterPredicateDirectorJava(Func1<Long, Boolean> f)
        {
            this.f = f;
        }

		@Override
        public boolean predicate(long client_endpoint)
        {
            return f.func(client_endpoint);
        }
    }

	public WireBroadcaster(Wire<T> wire)
	{
		this.wire=wire;
		this.innerwire=new WrappedWireBroadcaster();
		this.innerwire.init((WrappedWireServer)wire.innerwire);
		this.type=((WrappedWireServer)wire.innerwire).getType();
	}
	
	public Wire<T> getWire()
	{
		return wire;
	}
	
	public void setOutValue(T value)
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
	
	public long getActiveWireConnectionCount()
	{
		return innerwire.getActiveWireConnectionCount();
	}
	
	public void setPredicate(Func1<Long, Boolean> f)
	{
		WrappedWireBroadcasterPredicateDirectorJava p 
			= new WrappedWireBroadcasterPredicateDirectorJava(f);
        int id = RRObjectHeap.addObject(p);
        innerwire.setPredicateDirector(p, id);
	}
	
}
