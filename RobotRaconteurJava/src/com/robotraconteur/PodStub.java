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
                MessageElementUtil.addMessageElementDispose(
                    mm, MessageElementUtil.newMessageElementDispose(0, packPod((RRPod)s)));
                return new MessageElementNestedElementList(DataTypes.DataTypes_pod_array_t, getTypeName(), mm);
            }
            finally
            {
                if (mm != null)
                    mm.finalize();
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
            return unpackPod((MessageElementNestedElementList)m);
        case DataTypes_pod_array_t:
            return unpackPodArray((MessageElementNestedElementList)m);
        case DataTypes_pod_multidimarray_t:
            return unpackPodMultiDimArray((MessageElementNestedElementList)m);
        default:
            break;
        }

        throw new RuntimeException("Invalid pod data type");
    }

    protected MessageElementNestedElementList packPodArray(RRPod[] s2)
    {
        vectorptr_messageelement mm = new vectorptr_messageelement();
        try
        {
            for (int i = 0; i < s2.length; i++)
            {
                MessageElementUtil.addMessageElementDispose(
                    mm, MessageElementUtil.newMessageElementDispose(i, packPod(s2[i])));
            }
            return new MessageElementNestedElementList(DataTypes.DataTypes_pod_array_t, getTypeName(), mm);
        }
        finally
        {
            if (mm != null)
                mm.finalize();
        }
    }

    protected T[] unpackPodArray(MessageElementNestedElementList s2)
    {
        if (!s2.getTypeString().equals(getTypeName()))
            throw new DataTypeException("pod type mismatch");
        int count = 0;
        vectorptr_messageelement cdataElements = s2.getElements();
        try
        {
            T[] o = createArray(cdataElements.size());
            for (MessageElement e : cdataElements)
            {
                try
                {
                    if (count != MessageElementUtil.getMessageElementNumber(e))
                        throw new DataTypeException("Error in list format");
                    MessageElementNestedElementList md = (MessageElementNestedElementList)e.getData();
                    try
                    {
                        o[count] = unpackPod(md);
                    }
                    finally
                    {
                        if (md != null)
                            md.finalize();
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
            if (cdataElements != null)
                cdataElements.finalize();
        }
    }

    public MessageElementNestedElementList packPodMultiDimArray(PodMultiDimArray s3)
    {
        if (s3 == null)
            return null;
        vectorptr_messageelement l = new vectorptr_messageelement();
        try
        {

            MessageElementUtil.addMessageElementDispose(l, "dims", new UnsignedInts(s3.dims));
            MessageElementData s4 = packPodArray((RRPod[])s3.pod_array);
            try
            {
                MessageElementUtil.addMessageElementDispose(l, "array", s4);
                return new MessageElementNestedElementList(DataTypes.DataTypes_pod_multidimarray_t, getTypeName(), l);
            }
            finally
            {
                if (s4 != null)
                    s4.finalize();
            }
        }
        finally
        {
            if (l != null)
                l.finalize();
        }
    }

    public PodMultiDimArray unpackPodMultiDimArray(MessageElementNestedElementList s3)
    {
        if (!s3.getTypeString().equals(getTypeName()))
            throw new DataTypeException("pod type mismatch");
        PodMultiDimArray o = new PodMultiDimArray();
        vectorptr_messageelement marrayElements = s3.getElements();
        try
        {
            o.dims = (MessageElementUtil.<UnsignedInts>findElementAndCast(marrayElements, "dims")).value;
            MessageElementNestedElementList s2 =
                (MessageElementUtil.<MessageElementNestedElementList>findElementAndCast(marrayElements, "array"));
            try
            {
                o.pod_array = unpackPodArray(s2);
            }
            finally
            {
                if (s2 != null)
                    s2.finalize();
            }
        }
        finally
        {
            if (marrayElements != null)
                marrayElements.finalize();
        }
        return o;
    }

    public abstract MessageElementNestedElementList packPod(RRPod s1);
    public abstract T unpackPod(MessageElementNestedElementList m);
    public abstract String getTypeName();
    protected abstract T[] createArray(int count);
}