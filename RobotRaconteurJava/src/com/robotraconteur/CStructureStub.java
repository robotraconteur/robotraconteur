package com.robotraconteur;

public abstract class CStructureStub<T> implements ICStructureStub
{
	public MessageElementData packStructure(Object s)
	{
		if (s instanceof RRCStructure)
		{
			vectorptr_messageelement mm = new vectorptr_messageelement();
			try
			{
                MessageElementUtil.addMessageElementDispose(mm,
                    MessageElementUtil.newMessageElementDispose(0, packCStructure((RRCStructure)s))
                    );	            
	            return new MessageElementCStructureArray(getTypeName(), mm);            
			}
			finally
			{
				if (mm!=null) mm.finalize();
			}			
		}
		
		if (s instanceof RRCStructure[])
		{
			return packCStructureArray((RRCStructure[])s);			
		}
		
		if (s instanceof CStructureMultiDimArray)
		{
			return packCStructureMultiDimArray((CStructureMultiDimArray)s);
		}
		
		throw new DataTypeException("Invalid cstructure data type");		
	}

	public Object unpackStructure(MessageElementData m)
	{
		switch (m.getTypeID())
		{
		case DataTypes_cstructure_t:
			return unpackCStructure((MessageElementCStructure)m);
		case DataTypes_cstructure_array_t:
			return unpackCStructureArray((MessageElementCStructureArray)m);
		case DataTypes_cstructure_multidimarray_t:
			return unpackCStructureMultiDimArray((MessageElementCStructureMultiDimArray)m);
		default:
			break;		
		}
		
		throw new RuntimeException("Invalid cstructure data type");
	}
	
	protected MessageElementCStructureArray packCStructureArray(RRCStructure[] s2)
	{
		vectorptr_messageelement mm = new vectorptr_messageelement();
		try
		{            
            for (int i = 0; i < s2.length; i++)
            {
                MessageElementUtil.addMessageElementDispose(mm,
                    MessageElementUtil.newMessageElementDispose(i, packCStructure(s2[i]))
                    );
            }
            return new MessageElementCStructureArray(getTypeName(), mm);            
		}
		finally
		{
			if (mm!=null) mm.finalize();
		}		
	}
	
	protected T[] unpackCStructureArray(MessageElementCStructureArray s2)
	{
		if (!s2.getTypeString().equals(getTypeName())) throw new DataTypeException("cstructure type mismatch");
        int count = 0;
        vectorptr_messageelement cdataElements = s2.getElements();
        try
        {
            T[] o = createArray(cdataElements.size());
            for (MessageElement e : cdataElements)
            {
               try
                {
                    if (count != MessageElementUtil.getMessageElementNumber(e)) throw new DataTypeException("Error in list format");
                    MessageElementCStructure md = (MessageElementCStructure)e.getData();
                    try
                    {
                        o[count] = unpackCStructure(md);                            
                    } 
                    finally
                    {
                    	if (md!=null) md.finalize();
                    }
                    count++;
                }
               finally
               {
            	   e.finalize();
               }
            }
            return o;
        }
        finally
        {
        	if (cdataElements != null) cdataElements.finalize();
        }
	}
	
	public MessageElementCStructureMultiDimArray packCStructureMultiDimArray(CStructureMultiDimArray s3)
    {
        if (s3 == null) return null;
        vectorptr_messageelement l = new vectorptr_messageelement();
        try
        {
            MessageElementUtil.addMessageElementDispose(l, "dims", s3.dims);
            MessageElementData s4 = packCStructureArray((RRCStructure[])s3.cstruct_array);
            try
            {
                MessageElementUtil.addMessageElementDispose(l, "array", s4);
                return new MessageElementCStructureMultiDimArray(getTypeName(), l);
            }
            finally
            {
            	if (s4!=null) s4.finalize();
            }
        }
        finally
        {
        	if (l!=null) l.finalize();
        }
    }
	
	public CStructureMultiDimArray unpackCStructureMultiDimArray(MessageElementCStructureMultiDimArray s3)
    {
        if (!s3.getTypeString().equals(getTypeName())) throw new DataTypeException("cstructure type mismatch");
        CStructureMultiDimArray o = new CStructureMultiDimArray();
        vectorptr_messageelement marrayElements = s3.getElements();
        try
        {
            o.dims = (MessageElementUtil.<int[]>findElementAndCast(marrayElements, "dims"));
            MessageElementCStructureArray s2 = (MessageElementUtil.<MessageElementCStructureArray>findElementAndCast(marrayElements, "array"));
            try            
            {
                o.cstruct_array = unpackCStructureArray(s2);
            }
            finally
            {
            	if (s2!=null) s2.finalize();
            }
        }
        finally
        {
        	if (marrayElements!=null) marrayElements.finalize();
        }
        return o;
    }
	
	
	public abstract MessageElementCStructure packCStructure(RRCStructure s1);
	public abstract T unpackCStructure(MessageElementCStructure m);
	public abstract String getTypeName();
	protected abstract T[] createArray(int count); 
}