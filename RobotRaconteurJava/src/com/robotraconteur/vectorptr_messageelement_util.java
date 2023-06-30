package com.robotraconteur;

public class vectorptr_messageelement_util
{
    public static MessageElement findElement(vectorptr_messageelement m, String name)
    {
        for (int i = 0; i < m.size(); i++)
        {
            MessageElement m2 = m.get(i);
            if (name.equals(m2.getElementName()))
            {
                return m2;
            }
            else
            {
                m2.delete();
            }
        }
        throw new MessageElementNotFoundException("Message element " + name + " not found");
    }
}