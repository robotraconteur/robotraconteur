package com.robotraconteur;

public abstract class NamedArrayStub<T,U> implements INamedArrayStub2<T>
{
    public abstract U getNumericArrayFromNamedArrayStruct(T s);

    public abstract T getNamedArrayStructFromNumericArray(U m);

    public abstract U getNumericArrayFromNamedArray(T[] s);

    public abstract T[] getNamedArrayFromNumericArray(U m);


    public MessageElementNestedElementList packNamedArrayToArray(T s2)
    {
    	vectorptr_messageelement mm = new vectorptr_messageelement();
    	try    	
        {
            MessageElementUtil.addMessageElementDispose(mm,
                MessageElementUtil.newMessageElementDispose("array", getNumericArrayFromNamedArrayStruct(s2))
                );

            return new MessageElementNestedElementList(DataTypes.DataTypes_namedarray_array_t,getTypeName(), mm);
        }
    	finally
    	{
    		mm.finalize();
    	}
    }

    public MessageElementNestedElementList packNamedArray(T[] s2)
    {
        if (s2 == null) return null;

        vectorptr_messageelement mm = new vectorptr_messageelement();
        try
        {
            
            MessageElementUtil.addMessageElementDispose(mm,
                MessageElementUtil.newMessageElementDispose("array", getNumericArrayFromNamedArray(s2))
                );
            
            return new MessageElementNestedElementList(DataTypes.DataTypes_namedarray_array_t,getTypeName(), mm);
        }
        finally
        {
        	mm.finalize();
        }
    }

    public MessageElementNestedElementList packNamedMultiDimArray(NamedMultiDimArray s3)
    {
        if (s3 == null) return null;
        vectorptr_messageelement l = new vectorptr_messageelement();
        try
        {
            MessageElementUtil.addMessageElementDispose(l, "dims", new UnsignedInts(s3.dims));
            MessageElementUtil.addMessageElementDispose(l, "array", packNamedArray((T[])s3.namedarray_array));
            return new MessageElementNestedElementList(DataTypes.DataTypes_namedarray_multidimarray_t,getTypeName(), l);
            
        }
        finally
        {
        	l.finalize();
        }
    }

    public T unpackNamedArrayFromArray(MessageElementNestedElementList s2)
    {
        if (!s2.getType().equals(getTypeName())) throw new DataTypeException("namedarray type mismatch");
        vectorptr_messageelement cdataElements = s2.getElements();
        {
            if (cdataElements.size() != 1) throw new DataTypeException("namedarray type mismatch");

            U a = MessageElementUtil.<U>findElementAndCast(cdataElements, "array");

            return getNamedArrayStructFromNumericArray(a);                             
        }
    }

    public T[] unpackNamedArray(MessageElementNestedElementList s2)
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

    public NamedMultiDimArray unpackNamedMultiDimArray(MessageElementNestedElementList s3)
    {
        if (!s3.getType().equals(getTypeName())) throw new DataTypeException("namedarray type mismatch");
        NamedMultiDimArray o = new NamedMultiDimArray();
        vectorptr_messageelement marrayElements = s3.getElements();
        try        
        {
            o.dims = (MessageElementUtil.<UnsignedInts>findElementAndCast(marrayElements, "dims")).value;
            MessageElementNestedElementList s2 = (MessageElementUtil.<MessageElementNestedElementList>findElementAndCast(marrayElements, "array"));
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
        /*var m2 = m as MessageElementNestedElementList;
        if (m2 != null)
        {
            
            return UnpackPod(m2);           
        }*/

        if (((MessageElementData)m).getTypeID() == DataTypes.DataTypes_namedarray_array_t)
        {
            return unpackNamedArray((MessageElementNestedElementList)m);
        }
        
        if (((MessageElementData)m).getTypeID() == DataTypes.DataTypes_namedarray_multidimarray_t)
        {
            return unpackNamedMultiDimArray((MessageElementNestedElementList)m);
        }

        throw new DataTypeException("Unexpected message element type for UnpackNamedArray");
    }

    public abstract String getTypeName();
}
