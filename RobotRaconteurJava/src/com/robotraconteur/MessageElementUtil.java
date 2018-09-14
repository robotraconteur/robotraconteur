package com.robotraconteur;

public class MessageElementUtil {
	public static MessageElement newMessageElementDispose(String name, Object data)
	{
		MessageElement m=null;
		try
		{
			m=new MessageElement(name,data);
			
			return m;
			
		}
		catch(RuntimeException e)
		{
			if (m!=null) m.delete();
			throw e;
		
		}
		finally
		{
			if (data!=null)
			{
				if (data instanceof MessageElementData)
				{
					((MessageElementData)data).delete();
				}
			}
		}
		
		
	}
	
	public static void addMessageElementDispose(vectorptr_messageelement vct, String name, Object data)
	{
		MessageElement m=newMessageElementDispose(name,data);
		try
		{
			vct.add(m);
		}
		finally
		{
			m.delete();
		}		
	}

	public static MessageElement newMessageElementDispose(int number, Object data)
	{
		MessageElement m=null;
		try
		{
			m=new MessageElement("", data);
			m.setElementNumber(number);
			m.setElementFlags((short)( m.getElementFlags() & ~RobotRaconteurJava.getMessageElementFlags_ELEMENT_NAME_STR()));
			m.setElementFlags((short)( m.getElementFlags() | RobotRaconteurJava.getMessageElementFlags_ELEMENT_NUMBER()));
						
			return m;
			
		}
		catch(RuntimeException e)
		{
			if (m!=null) m.delete();
			throw e;
		
		}
		finally
		{
			if (data!=null)
			{
				if (data instanceof MessageElementData)
				{
					((MessageElementData)data).delete();
				}
			}
		}
		
		
	}
	
	public static void addMessageElementDispose(vectorptr_messageelement vct, int number, Object data)
	{
		MessageElement m=newMessageElementDispose(number,data);
		try
		{
			vct.add(m);
		}
		finally
		{
			m.delete();
		}		
	}
	
	public static void addMessageElementDispose(vectorptr_messageelement vct, MessageElement m )
	{
		
		try
		{
			vct.add(m);
		}
		finally
		{
			if (m!=null) m.delete();
		}
		
		
	}
	
	public static <T> T findElementAndCast(vectorptr_messageelement elems, String name)
	{
		MessageElement e=MessageElement.findElement(elems,name);
		try
		{
			return e.<T>castData();
		}
		finally
		{
			e.delete();
		}
	}
	
	
	public static <T> T castDataAndDispose(MessageElement m)
	{
		Object dat=null;
		try
		{
			dat=m.getData();
			return (T)dat;
		}
		catch (RuntimeException e)
		{
			if (dat!=null)
			{
				if (dat instanceof MessageElementData)
				{
					((MessageElementData)dat).delete();
				}
				
			}
			throw e;
		}
		finally
		{
			if (m!=null) m.delete();
		}
		
	}

	public static int getMessageElementNumber(MessageElement e)
	{	
		if ((e.getElementFlags() & RobotRaconteurJava.getMessageElementFlags_ELEMENT_NUMBER())!=0)
        {
            return e.getElementNumber();
        }
        else if ((e.getElementFlags() & RobotRaconteurJava.getMessageElementFlags_ELEMENT_NAME_STR())!=0)
        {
            return Integer.parseInt(e.getElementName());
        }
        else
        {
            throw new RuntimeException("Could not determine Element Number");
        }
	}
}
