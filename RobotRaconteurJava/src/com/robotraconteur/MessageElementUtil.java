package com.robotraconteur;

import java.lang.reflect.Array;
import java.util.List;
import java.util.Map;

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
	
	public static <T> MessageElement packArray(String name, T val)
    {
        if (val == null)
        {
            throw new NullPointerException();
        }
        return newMessageElementDispose(name, val );
    }

    public static MessageElement packMultiDimArray(String name, MultiDimArray val)
    {
        if (val == null)
        {
            throw new NullPointerException();
        }

        return newMessageElementDispose(name, RobotRaconteurNode.s().packMultiDimArray(val));
    }

    public static MessageElement packString(String name, String val)
    {
        if (val == null)
        {
            throw new NullPointerException();
        }
        return newMessageElementDispose(name, val);
    }

    public static MessageElement packStructure(String name, Object val)
    {
        return newMessageElementDispose(name, RobotRaconteurNode.s().packStructure(val));
    }
            
    public static MessageElement packVarType(String name, Object val)
    {
        return newMessageElementDispose(name, RobotRaconteurNode.s().packVarType(val));
    }

    public static <K,T> MessageElement packMapType(String name, Map<K,T> val, Class<?> Ktype, Class<?> Ttype)
    {
        return newMessageElementDispose(name, RobotRaconteurNode.s().<K, T>packMapType(val, Ktype, Ttype));
    }

    public static <T> MessageElement packListType(String name, List<T> val, Class<?> Ttype)
    {
        return newMessageElementDispose(name, RobotRaconteurNode.s().<T>packListType(val, Ttype));
    }
    
    public static <T> MessageElement packPodToArray(String name, T val)
    {
        return newMessageElementDispose(name ,RobotRaconteurNode.s().packStructure(val));
    }

    public static <T> MessageElement packPodArray(String name, T[] val)
    {
        return newMessageElementDispose(name, RobotRaconteurNode.s().packStructure(val));
    }

    public static <T> MessageElement packPodMultiDimArray(String name, PodMultiDimArray val)
    {
        return newMessageElementDispose(name, RobotRaconteurNode.s().packStructure(val));
    }

    public static <T> MessageElement packNamedArrayToArray(String name, T val)
    {
        return newMessageElementDispose(name ,RobotRaconteurNode.s().packStructure(val));
    }

    public static <T> MessageElement packNamedArray(String name, T[] val)
    {
        return newMessageElementDispose(name, RobotRaconteurNode.s().packStructure(val));
    }

    public static <T> MessageElement packNamedMultiDimArray(String name, NamedMultiDimArray val)
    {
        return newMessageElementDispose(name, RobotRaconteurNode.s().packStructure(val));
    }
    
    public static <T> T unpackArray(MessageElement m)
    {
        T a = MessageElementUtil.<T>castDataAndDispose(m);
        if (a == null) throw new NullPointerException();
        return a;
    }

    public static MultiDimArray unpackMultiDimArray(MessageElement m)
    {
        MultiDimArray a = RobotRaconteurNode.s().unpackMultiDimArrayDispose(MessageElementUtil.<MessageElementNestedElementList>castDataAndDispose(m));
        if (a == null) throw new NullPointerException();
        return a;
    }

    public static String unpackString(MessageElement m)
    {
        String s = MessageElementUtil.<String>castDataAndDispose(m);
        if (s == null) throw new NullPointerException();
        return s;
    }

    public static <T> T unpackStructure(MessageElement m)
    {
        return RobotRaconteurNode.s().<T>unpackStructureDispose(MessageElementUtil.<MessageElementNestedElementList>castDataAndDispose(m));
    }

    public static Object unpackVarType(MessageElement m)
    {
        return RobotRaconteurNode.s().unpackVarTypeDispose(m);
    }

    public static <K,T> Map<K,T> unpackMapType(MessageElement m)
    {
        return (Map<K, T>)RobotRaconteurNode.s().<K,T>unpackMapTypeDispose(m.getData());
    }

    public static <T> List<T> unpackListType(MessageElement m)
    {
        return (List<T>)RobotRaconteurNode.s().<T>unpackListTypeDispose(m.getData());
    }
   
    public static <T> T unpackPodFromArray(MessageElement m)
    {
        return (RobotRaconteurNode.s().<T[]>unpackStructure(MessageElementUtil.<MessageElementNestedElementList>castDataAndDispose(m)))[0];
    }

    public static <T> T[] unpackPodArray(MessageElement m)
    {
        return RobotRaconteurNode.s().<T[]>unpackStructure(MessageElementUtil.<MessageElementNestedElementList>castDataAndDispose(m));
    }

    public static <T> PodMultiDimArray unpackPodMultiDimArray(MessageElement m)
    {
        return RobotRaconteurNode.s().unpackStructure(MessageElementUtil.<MessageElementNestedElementList>castDataAndDispose(m));
    }
    
    public static <T> T unpackNamedArrayFromArray(MessageElement m)
    {
        return (RobotRaconteurNode.s().<T[]>unpackStructure(MessageElementUtil.<MessageElementNestedElementList>castDataAndDispose(m)))[0];
    }

    public static <T> T[] unpackNamedArray(MessageElement m)
    {
        return RobotRaconteurNode.s().<T[]>unpackStructure(MessageElementUtil.<MessageElementNestedElementList>castDataAndDispose(m));
    }

    public static <T> NamedMultiDimArray unpackNamedMultiDimArray(MessageElement m)
    {
        return RobotRaconteurNode.s().unpackStructure(MessageElementUtil.<MessageElementNestedElementList>castDataAndDispose(m));
    }
}
