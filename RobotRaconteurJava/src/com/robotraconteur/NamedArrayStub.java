package com.robotraconteur;

public abstract class NamedArrayStub<T,U> implements INamedArrayStub2<T>
{
    public abstract U getNumericArrayFromNamedArrayStruct(T s);

    public abstract T getNamedArrayStructFromNumericArray(U m);

    public abstract U getNumericArrayFromNamedArray(T[] s);

    public abstract T[] getNamedArrayFromNumericArray(U m);


    public MessageElementNamedArray packNamedArrayToArray(T s2)
    {
    	vectorptr_messageelement mm = new vectorptr_messageelement();
    	try    	
        {
            MessageElementUtil.addMessageElementDispose(mm,
                MessageElementUtil.newMessageElementDispose("array", getNumericArrayFromNamedArrayStruct(s2))
                );

            return new MessageElementNamedArray(getTypeName(), mm);
        }
    	finally
    	{
    		mm.finalize();
    	}
    }

    public MessageElementNamedArray packNamedArray(T[] s2)
    {
        if (s2 == null) return null;

        vectorptr_messageelement mm = new vectorptr_messageelement();
        try
        {
            
            MessageElementUtil.addMessageElementDispose(mm,
                MessageElementUtil.newMessageElementDispose("array", getNumericArrayFromNamedArray(s2))
                );
            
            return new MessageElementNamedArray(getTypeName(), mm);
        }
        finally
        {
        	mm.finalize();
        }
    }

    public MessageElementNamedMultiDimArray packNamedMultiDimArray(NamedMultiDimArray s3)
    {
        if (s3 == null) return null;
        vectorptr_messageelement l = new vectorptr_messageelement();
        try
        {
            MessageElementUtil.addMessageElementDispose(l, "dims", new UnsignedInts(s3.dims));
            MessageElementUtil.addMessageElementDispose(l, "array", packNamedArray((T[])s3.namedarray_array));
            return new MessageElementNamedMultiDimArray(getTypeName(), l);
            
        }
        finally
        {
        	l.finalize();
        }
    }

    public T unpackNamedArrayFromArray(MessageElementNamedArray s2)
    {
        if (!s2.getType().equals(getTypeName())) throw new DataTypeException("namedarray type mismatch");
        vectorptr_messageelement cdataElements = s2.getElements();
        {
            if (cdataElements.size() != 1) throw new DataTypeException("namedarray type mismatch");

            U a = MessageElementUtil.<U>findElementAndCast(cdataElements, "array");

            return getNamedArrayStructFromNumericArray(a);                             
        }
    }

    public T[] unpackNamedArray(MessageElementNamedArray s2)
    {    	
        if (!s2.getType().equals(getTypeName())) throw new DataTypeException("namedarray type mismatch");
        vectorptr_messageelement cdataElements = s2.getElements();
        try
        {
            if (cdataElements.size() != 1) throw new DataTypeException("pod type mismatch");

            U a = MessageElementUtil.<U>findElementAndCast(cdataElements, "array");

            return getNamedArrayFromNumericArray(a);
        }
        finally
        {
        	cdataElements.finalize();
        }
    }

    public NamedMultiDimArray unpackNamedMultiDimArray(MessageElementNamedMultiDimArray s3)
    {
        if (!s3.getType().equals(getTypeName())) throw new DataTypeException("namedarray type mismatch");
        NamedMultiDimArray o = new NamedMultiDimArray();
        vectorptr_messageelement marrayElements = s3.getElements();
        try        
        {
            o.dims = (MessageElementUtil.<UnsignedInts>findElementAndCast(marrayElements, "dims")).value;
            MessageElementNamedArray s2 = (MessageElementUtil.<MessageElementNamedArray>findElementAndCast(marrayElements, "array"));
            try
            {
                o.namedarray_array = unpackNamedArray(s2);
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
    	if (s instanceof NamedMultiDimArray)
        {
            return packNamedMultiDimArray((NamedMultiDimArray)s);
        }
    	
        if (!s.getClass().isArray())
        {
            return packNamedArrayToArray((T)s);
        }
        else
        {
            return packNamedArray((T[])s);
        }
                
        //throw new DataTypeException("Unexpected message element type for PackNamedArray");
    }
    public Object unpackStructure(MessageElementData m)
    {
        /*var m2 = m as MessageElementPod;
        if (m2 != null)
        {
            
            return UnpackPod(m2);           
        }*/

        
        if (m instanceof MessageElementNamedArray)
        {
            return unpackNamedArray((MessageElementNamedArray)m);
        }

        
        if (m instanceof MessageElementNamedMultiDimArray)
        {
            return unpackNamedMultiDimArray((MessageElementNamedMultiDimArray)m);
        }

        throw new DataTypeException("Unexpected message element type for UnpackNamedArray");
    }

    public abstract String getTypeName();
}
