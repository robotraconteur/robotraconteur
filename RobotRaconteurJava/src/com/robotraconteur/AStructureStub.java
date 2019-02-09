package com.robotraconteur;

public abstract class AStructureStub<T,U> implements IAStructureStub2<T>
{
    public abstract U getNumericArrayFromAStructure(T s);

    public abstract T getAStructureFromNumericArray(U m);

    public abstract U getNumericArrayFromAStructureArray(T[] s);

    public abstract T[] getAStructureArrayFromNumericArray(U m);


    public MessageElementAStructureArray packAStructureToArray(T s2)
    {
    	vectorptr_messageelement mm = new vectorptr_messageelement();
    	try    	
        {
            MessageElementUtil.addMessageElementDispose(mm,
                MessageElementUtil.newMessageElementDispose("array", getNumericArrayFromAStructure(s2))
                );

            return new MessageElementAStructureArray(getTypeName(), mm);
        }
    	finally
    	{
    		mm.finalize();
    	}
    }

    public MessageElementAStructureArray packAStructureArray(T[] s2)
    {
        if (s2 == null) return null;

        vectorptr_messageelement mm = new vectorptr_messageelement();
        try
        {
            
            MessageElementUtil.addMessageElementDispose(mm,
                MessageElementUtil.newMessageElementDispose("array", getNumericArrayFromAStructureArray(s2))
                );
            
            return new MessageElementAStructureArray(getTypeName(), mm);
        }
        finally
        {
        	mm.finalize();
        }
    }

    public MessageElementAStructureMultiDimArray packAStructureMultiDimArray(AStructureMultiDimArray s3)
    {
        if (s3 == null) return null;
        vectorptr_messageelement l = new vectorptr_messageelement();
        try
        {
            MessageElementUtil.addMessageElementDispose(l, "dims", s3.dims);
            MessageElementUtil.addMessageElementDispose(l, "array", packAStructureArray((T[])s3.astruct_array));
            return new MessageElementAStructureMultiDimArray(getTypeName(), l);
            
        }
        finally
        {
        	l.finalize();
        }
    }

    public T unpackAStructureFromArray(MessageElementAStructureArray s2)
    {
        if (!s2.getType().equals(getTypeName())) throw new DataTypeException("astructure type mismatch");
        vectorptr_messageelement cdataElements = s2.getElements();
        {
            if (cdataElements.size() != 1) throw new DataTypeException("astructure type mismatch");

            U a = MessageElementUtil.<U>findElementAndCast(cdataElements, "array");

            return getAStructureFromNumericArray(a);                             
        }
    }

    public T[] unpackAStructureArray(MessageElementAStructureArray s2)
    {    	
        if (!s2.getType().equals(getTypeName())) throw new DataTypeException("astructure type mismatch");
        vectorptr_messageelement cdataElements = s2.getElements();
        try
        {
            if (cdataElements.size() != 1) throw new DataTypeException("cstructure type mismatch");

            U a = MessageElementUtil.<U>findElementAndCast(cdataElements, "array");

            return getAStructureArrayFromNumericArray(a);
        }
        finally
        {
        	cdataElements.finalize();
        }
    }

    public AStructureMultiDimArray unpackAStructureMultiDimArray(MessageElementAStructureMultiDimArray s3)
    {
        if (!s3.getType().equals(getTypeName())) throw new DataTypeException("astructure type mismatch");
        AStructureMultiDimArray o = new AStructureMultiDimArray();
        vectorptr_messageelement marrayElements = s3.getElements();
        try        
        {
            o.dims = (MessageElementUtil.<int[]>findElementAndCast(marrayElements, "dims"));
            MessageElementAStructureArray s2 = (MessageElementUtil.<MessageElementAStructureArray>findElementAndCast(marrayElements, "array"));
            try
            {
                o.astruct_array = unpackAStructureArray(s2);
                return o;
            }
            finally
            {
            	s2.finalize();
            }        
            
        }
        finally
        {
        	marrayElements.finalize();
        } 
    }

    public MessageElementData packStructure(Object s)
    {
    	if (s instanceof AStructureMultiDimArray)
        {
            return packAStructureMultiDimArray((AStructureMultiDimArray)s);
        }
    	
        if (!s.getClass().isArray())
        {
            return packAStructureToArray((T)s);
        }
        else
        {
            return packAStructureArray((T[])s);
        }
                
        //throw new DataTypeException("Unexpected message element type for PackAStructure");
    }
    public Object unpackStructure(MessageElementData m)
    {
        /*var m2 = m as MessageElementCStructure;
        if (m2 != null)
        {
            
            return UnpackCStructure(m2);           
        }*/

        
        if (m instanceof MessageElementAStructureArray)
        {
            return unpackAStructureArray((MessageElementAStructureArray)m);
        }

        
        if (m instanceof MessageElementAStructureMultiDimArray)
        {
            return unpackAStructureMultiDimArray((MessageElementAStructureMultiDimArray)m);
        }

        throw new DataTypeException("Unexpected message element type for UnpackAStructure");
    }

    public abstract String getTypeName();
}
