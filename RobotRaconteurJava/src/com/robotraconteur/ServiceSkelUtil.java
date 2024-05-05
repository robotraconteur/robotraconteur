package com.robotraconteur;
import java.util.*;

public class ServiceSkelUtil
{
    public static java.lang.Class<?> findParentInterface(java.lang.Class<?> objtype)
    {

        Class<?>[] interfaces = objtype.getInterfaces();
        ArrayList<Class<?>> interfaces2 = new ArrayList<Class<?>>();
        for (Class<?> c : interfaces)
        {
            RobotRaconteurServiceObjectInterface flag = c.getAnnotation(RobotRaconteurServiceObjectInterface.class);
            if (flag != null)
                interfaces2.add(c);
        }

        if (interfaces2.isEmpty())
        {
            throw new DataTypeException("Object not a Robot Raconteur type");
        }
        if (interfaces2.size() == 1)
        {
            return interfaces2.get(0);
        }

        ArrayList<Class<?>> parentinterfaces = new ArrayList<Class<?>>();

        for (int i = 0; i < interfaces2.size(); i++)
        {
            boolean parent = true;
            for (int j = 0; j < interfaces2.size(); j++)
            {
                if (i != j)
                {
                    Class<?>[] ci = interfaces2.get(j).getInterfaces();
                    for (Class<?> ci2 : ci)
                    {
                        if (ci2 == interfaces2.get(i))
                        {
                            parent = false;
                        }
                    }
                }
            }

            if (parent)
            {
                parentinterfaces.add(interfaces2.get(i));
            }
        }

        if (parentinterfaces.size() != 1)
        {
            throw new DataTypeException(
                "Robot Raconteur types can only directly inheret one Robot Raconteur interface type");
        }

        return parentinterfaces.get(0);
    }
}
