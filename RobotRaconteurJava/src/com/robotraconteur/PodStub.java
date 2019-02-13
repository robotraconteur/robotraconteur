package com.robotraconteur;

public abstract class PodStub<T> implements IPodStub
{
	public MessageElementData packStructure(Object s)
	{
		if (s instanceof RRPod)
		{
			vectorptr_messageelement mm = new vectorptr_messageelement();
			try
			{
                MessageElementUtil.addMessageElementDispose(mm,
                    MessageElementUtil.newMessageElementDispose(0, packPod((RRPod)s))
                    );	            
	            return new MessageElementPodArray(getTypeName(), mm);            
			}
			finally
			{
				if (mm!=null) mm.finalize();
			}			
		}
		
		if (s instanceof RRPod[])
		{
			return packPodArray((RRPod[])s);			
		}
		
		if (s instanceof PodMultiDimArray)
		{
			return packPodMultiDimArray((PodMultiDimArray)s);
		}
		
		throw new DataTypeException("Invalid pod data type");		
	}

	public Object unpackStructure(MessageElementData m)
	{
		switch (m.getTypeID())
		{
		case DataTypes_pod_t:
			return unpackPod((MessageElementPod)m);
		case DataTypes_pod_array_t:
			return unpackPodArray((MessageElementPodArray)m);
		case DataTypes_pod_multidimarray_t:
			return unpackPodMultiDimArray((MessageElementPodMultiDimArray)m);
		default:
			break;		
		}
		
		throw new RuntimeException("Invalid pod data type");
	}
	
	protected MessageElementPodArray packPodArray(RRPod[] s2)
	{
		vectorptr_messageelement mm = new vectorptr_messageelement();
		try
		{            
            for (int i = 0; i < s2.length; i++)
            {
                MessageElementUtil.addMessageElementDispose(mm,
                    MessageElementUtil.newMessageElementDispose(i, packPod(s2[i]))
                    );
            }
            return new MessageElementPodArray(getTypeName(), mm);            
		}
		finally
		{
			if (mm!=null) mm.finalize();
		}		
	}
	
	protected T[] unpackPodArray(MessageElementPodArray s2)
	{
		if (!s2.getTypeString().equals(getTypeName())) throw new DataTypeException("pod type mismatch");
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
                    MessageElementPod md = (MessageElementPod)e.getData();
                    try
                    {
                        o[count] = unpackPod(md);                            
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
	
	public MessageElementPodMultiDimArray packPodMultiDimArray(PodMultiDimArray s3)
    {
        if (s3 == null) return null;
        vectorptr_messageelement l = new vectorptr_messageelement();
        try
        {
        	       	
            MessageElementUtil.addMessageElementDispose(l, "dims", new UnsignedInts(s3.dims));
            MessageElementData s4 = packPodArray((RRPod[])s3.pod_array);
            try
            {
                MessageElementUtil.addMessageElementDispose(l, "array", s4);
                return new MessageElementPodMultiDimArray(getTypeName(), l);
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
	
	public PodMultiDimArray unpackPodMultiDimArray(MessageElementPodMultiDimArray s3)
    {
        if (!s3.getTypeString().equals(getTypeName())) throw new DataTypeException("pod type mismatch");
        PodMultiDimArray o = new PodMultiDimArray();
        vectorptr_messageelement marrayElements = s3.getElements();
        try
        {
            o.dims = (MessageElementUtil.<UnsignedInts>findElementAndCast(marrayElements, "dims")).value;
            MessageElementPodArray s2 = (MessageElementUtil.<MessageElementPodArray>findElementAndCast(marrayElements, "array"));
            try            
            {
                o.pod_array = unpackPodArray(s2);
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
	
	
	public abstract MessageElementPod packPod(RRPod s1);
	public abstract T unpackPod(MessageElementPod m);
	public abstract String getTypeName();
	protected abstract T[] createArray(int count); 
}