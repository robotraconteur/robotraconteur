package com.robotraconteur;

import java.security.KeyException;

public class RRObjectHeap
{
	static class RRObjectHeap_support extends RRNativeObjectHeapSupport
	{
		@Override
		public void deleteObject_i(int id)
		{
			RRObjectHeap.deleteObject(id);
		}
	}
	
	
	public static java.util.HashMap<Integer, Object> objects = new java.util.HashMap<Integer, Object>();
	public static int objectcount = 0;

	
	static RRObjectHeap_support support;
	
	static {
		support=new RRObjectHeap_support();
		RRNativeObjectHeapSupport.set_Support(support);
	}
	
	
	public static int addObject(Object o)
	{
		synchronized (objects)
		{
			if (objectcount > Integer.MAX_VALUE-10) objectcount=0;
			objectcount++;
			while (objects.containsKey(objectcount)) objectcount++;
			int i = objectcount;
			objects.put(i, o);
			return i;
		}
	}

	public static void deleteObject(int id)
	{
		synchronized (objects)
		{
			try
			{
				
				objects.remove(id);
			}
			catch (java.lang.Exception e)
			{
			}
		}
	}
	
	public static Object getObject(int id)
	{
		synchronized(objects)
		{
				Object o=objects.get(id);
				if (o==null) throw new RuntimeException("Object not found");
				return o;
			
			
		
		
		}
		
		
	}
}