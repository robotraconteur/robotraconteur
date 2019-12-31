package com.robotraconteur;

import java.lang.reflect.Array;
import java.util.List;
import java.util.Map;

public class DataTypeUtil {
	public static int size(DataTypes type)
    {
        switch (type)
        {
            case DataTypes_double_t:
                return 8;

            case DataTypes_single_t:
                return 4;
            case DataTypes_int8_t:
            case DataTypes_uint8_t:
                return 1;
            case DataTypes_int16_t:
            case DataTypes_uint16_t:
                return 2;
            case DataTypes_int32_t:
            case DataTypes_uint32_t:
                return 4;
            case DataTypes_int64_t:
            case DataTypes_uint64_t:
                return 8;
            case DataTypes_string_t:
                return 1;
            case DataTypes_cdouble_t:
                return 16;
            case DataTypes_csingle_t:
                return 8;
            case DataTypes_bool_t:
                return 1;            
            default:
            	break;

        }
        throw new RuntimeException(new DataTypeException(""));
       
    }
	
	public static DataTypes typeIDFromString(String stype)
	{
		//switch (stype)
        //{
            if(stype.equals( "null"))
                return DataTypes.DataTypes_void_t;
            if (stype.equals( "class [D") || stype.equals("class java.lang.Double"))
                return DataTypes.DataTypes_double_t;
            if(stype.equals( "class [F") || stype.equals("class java.lang.Float"))
                return DataTypes.DataTypes_single_t;
            if(stype.equals( "class [B") || stype.equals("class java.lang.Byte"))
                return DataTypes.DataTypes_int8_t;
            if(stype.equals( "class com.robotraconteur.UnsignedByte") || stype.equals("class com.robotraconteur.UnsignedBytes"))
                return DataTypes.DataTypes_uint8_t;
            if(stype.equals( "class [S") || stype.equals("class java.lang.Short"))
                return DataTypes.DataTypes_int16_t;
            if(stype.equals( "class com.robotraconteur.UnsignedShort") || stype.equals("class com.robotraconteur.UnsignedShorts"))
                return DataTypes.DataTypes_uint16_t;
            if(stype.equals( "class [I") || stype.equals("class java.lang.Integer"))
                return DataTypes.DataTypes_int32_t;
            if(stype.equals( "class com.robotraconteur.UnsignedInteger") || stype.equals("class com.robotraconteur.UnsignedIntegers"))
                return DataTypes.DataTypes_uint32_t;
            if(stype.equals( "class [J") || stype.equals("class java.lang.Long"))
                return DataTypes.DataTypes_int64_t;
            if(stype.equals( "class com.robotraconteur.UnsignedLong") || stype.equals("class com.robotraconteur.UnsignedLongs"))
                return DataTypes.DataTypes_uint64_t;
            if(stype.equals( "class java.lang.String"))
                return DataTypes.DataTypes_string_t;
            if(stype.equals( "class com.robotraconteur.CDouble") || stype.equals("class [Lcom.robotraconteur.CDouble"))
                return DataTypes.DataTypes_cdouble_t;
            if(stype.equals( "class com.robotraconteur.CSingle") || stype.equals("class [Lcom.robotraconteur.CSingle"))
                return DataTypes.DataTypes_csingle_t;
            if(stype.equals( "class [java.lang.Boolean") || stype.equals("class java.lang.Boolean"))
                return DataTypes.DataTypes_bool_t;            
            if(stype.equals( "com.robotraconteur.MessageElementNestedElementList"))
                return DataTypes.DataTypes_namedtype_t;
            if(stype.equals( "class java.lang.Object"))
                return DataTypes.DataTypes_varvalue_t;

        //}



        throw new RuntimeException(new DataTypeException ("Unknown data type"));
        
	}
	
	public static boolean typeIDFromString_known(String stype)
	{
		//switch (stype)
        //{
            if(stype.equals( "null")) return true;
                
            if(stype.equals( "class [D") || stype.equals("class java.lang.Double")) return true;
               
            if (stype.equals( "class [F") || stype.equals("class java.lang.Float")) return true;
                
            if(stype.equals( "class [B") || stype.equals("class java.lang.Byte")) return true;
                
            if(stype.equals( "class com.robotraconteur.UnsignedByte") || stype.equals("class com.robotraconteur.UnsignedBytes")) return true;
               
            if(stype.equals( "class [S") || stype.equals("class java.lang.Short")) return true;
                
            if(stype.equals( "class com.robotraconteur.UnsignedShort") || stype.equals("class com.robotraconteur.UnsignedShorts")) return true;
               
            if(stype.equals( "class [I") || stype.equals("class java.lang.Integer")) return true;
                
            if(stype.equals( "class com.robotraconteur.UnsignedInteger") || stype.equals("class com.robotraconteur.UnsignedIntegers")) return true;
                
            if (stype.equals( "class [J") || stype.equals("class java.lang.Long")) return true;
                
            if(stype.equals("class com.robotraconteur.UnsignedLong") || stype.equals("class com.robotraconteur.UnsignedLongs")) return true;
                
            if(stype.equals( "class java.lang.String")) return true;
            
            if(stype.equals( "class com.robotraconteur.CDouble") || stype.equals("class [Lcom.robotraconteur.CDouble")) return true;
                
            if(stype.equals( "class com.robotraconteur.CSingle") || stype.equals("class [Lcom.robotraconteur.CSingle")) return true;
                
            if(stype.equals( "class [java.lang.Boolean") || stype.equals("class java.lang.Boolean")) return true;
                         
            if(stype.equals( "com.robotraconteur.MessageElementNestedElementList")) return true;
                
            if(stype.equals( "class java.lang.Object"))
                return true;

        //}

        return false;
	}
	
	public static boolean isNumber(DataTypes t)
    {
		switch (t)
        {
            case DataTypes_double_t:
            case DataTypes_single_t:
            case DataTypes_int8_t:
            case DataTypes_uint8_t:
            case DataTypes_int16_t:
            case DataTypes_uint16_t:
            case DataTypes_int32_t:
            case DataTypes_uint32_t:
            case DataTypes_int64_t:
            case DataTypes_uint64_t:
            case DataTypes_cdouble_t:
            case DataTypes_csingle_t:
            case DataTypes_bool_t:            
                return true;
            default:
                return false;
        }
    }
        
    public static Object arrayFromDataType(DataTypes t, int length)
    {
        switch (t)
        {

            case DataTypes_double_t:
                return new double[length];
            case DataTypes_single_t:
                return new float[length];
            case DataTypes_int8_t:
                return new byte[length];
            case DataTypes_uint8_t:
            	return new UnsignedBytes(length);
            case DataTypes_int16_t:
                return new short[length];
            case DataTypes_uint16_t:
            	return new UnsignedShorts(length);
            case DataTypes_int32_t:
                return new int[length];
            case DataTypes_uint32_t:
                return new UnsignedInts(length);
            case DataTypes_int64_t:
                return new long[length];
            case DataTypes_uint64_t:
            	return new UnsignedLongs(length);
            case DataTypes_cdouble_t:
            	return new CDouble[length];
            case DataTypes_csingle_t:
            	return new CSingle[length];
            case DataTypes_bool_t:
            	return new boolean[length];
            case DataTypes_string_t:
                return null;
            case DataTypes_structure_t:
                return null;
            default:
            	break;
            	
        }
        throw new RuntimeException(new DataTypeException( "Could not create array for data type"));
    }
    
    public static boolean isArray(Object o)
    {
    	if (o instanceof double[]) return true;
    	if (o instanceof float[]) return true;
    	if (o instanceof byte[]) return true;
    	if (o instanceof short[]) return true;
    	if (o instanceof int[]) return true;
    	if (o instanceof long[]) return true;
    	if (o instanceof UnsignedBytes) return true;
    	if (o instanceof UnsignedShorts) return true;
    	if (o instanceof UnsignedInts) return true;
    	if (o instanceof UnsignedLongs) return true;
    	if (o instanceof CDouble[]) return true;
    	if (o instanceof CSingle[]) return true;
    	if (o instanceof boolean[]) return true;
    	return false;
    	    	
    }
    
    public static boolean isScalarNumber(Object o)
    {
    	if (o instanceof Double) return true;
    	if (o instanceof Float) return true;
    	if (o instanceof Byte) return true;
    	if (o instanceof Short) return true;
    	if (o instanceof Integer) return true;
    	if (o instanceof Long) return true;
    	if (o instanceof UnsignedByte) return true;
    	if (o instanceof UnsignedShort) return true;
    	if (o instanceof UnsignedInt) return true;
    	if (o instanceof UnsignedLong) return true;
    	if (o instanceof CDouble) return true;
    	if (o instanceof CSingle) return true;
    	if (o instanceof Boolean) return true;
    	return false;
    	
    	
    }
    
    public static boolean isScalarNumberType(Class o)
    {
    	if (o == Double.class) return true;
    	if (o == Float.class) return true;
    	if (o == Byte.class) return true;
    	if (o == Short.class) return true;
    	if (o == Integer.class) return true;
    	if (o == Long.class) return true;
    	if (o == UnsignedByte.class) return true;
    	if (o == UnsignedShort.class) return true;
    	if (o == UnsignedInt.class) return true;
    	if (o == UnsignedLong.class) return true;
    	if (o == CDouble.class) return true;
    	if (o == CSingle.class) return true;
    	if (o == Boolean.class) return true;
    	return false;
    	
    	
    }
    
    public static Object scalarFromArray(Object o)
    {
    	if (o instanceof double[]) return new Double(((double[])o)[0]);
    	if (o instanceof float[]) return new Float(((float[])o)[0]);
    	if (o instanceof byte[]) return new Byte(((byte[])o)[0]);
    	if (o instanceof short[]) return new Short(((short[])o)[0]);
    	if (o instanceof int[]) return new Integer(((int[])o)[0]);
    	if (o instanceof long[]) return new Long(((long[])o)[0]);
    	if (o instanceof UnsignedBytes) return ((UnsignedBytes)o).get(0);
    	if (o instanceof UnsignedShorts) return ((UnsignedShorts)o).get(0);
    	if (o instanceof UnsignedInts) return ((UnsignedInts)o).get(0);
    	if (o instanceof UnsignedLongs) return ((UnsignedLongs)o).get(0);
    	if (o instanceof CDouble[]) return ((CDouble[])o)[0];
    	if (o instanceof CSingle[]) return ((CSingle[])o)[0];
    	if (o instanceof boolean[]) return ((boolean[])o)[0];
    	throw new DataTypeException("Argument is not an array");
    	    	
    }
    
    public static Object arrayFromScalar(Object o)
    {
    	if (o instanceof Double) return new double[] {(Double)o};
    	if (o instanceof Float) return new float[] {(Float)o};
    	if (o instanceof Byte) return new byte[] {(Byte)o};
    	if (o instanceof Short) return new short[] {(Short)o};
    	if (o instanceof Integer) return new int[] {(Integer)o};
    	if (o instanceof Long) return new long[] {(Long)o};
    	if (o instanceof UnsignedByte) return ((UnsignedByte)o).array();
    	if (o instanceof UnsignedShort) return ((UnsignedShort)o).array();
    	if (o instanceof UnsignedInt) return ((UnsignedInt)o).array();
    	if (o instanceof UnsignedLong) return ((UnsignedLong)o).array();
    	if (o instanceof CDouble) return new CDouble[] {(CDouble)o};
    	if (o instanceof CSingle) return new CSingle[] {(CSingle)o};
    	if (o instanceof Boolean) return new boolean[] {(Boolean)o};
    	throw new DataTypeException("Argument is not a number");
    	
    }
    
    public static int getArrayLength(Object arr)
    {
    	if (arr instanceof double[]) return ((double[])arr).length;
		if (arr instanceof float[]) return ((float[])arr).length;
		
		if (arr instanceof byte[]) return ((byte[])arr).length;
		if (arr instanceof UnsignedBytes) return ((UnsignedBytes)arr).value.length;
		if (arr instanceof short[]) return ((short[])arr).length;
		if (arr instanceof UnsignedShorts) return ((UnsignedShorts)arr).value.length;
		if (arr instanceof int[]) return ((int[])arr).length;
		if (arr instanceof UnsignedInts) return ((UnsignedInts)arr).value.length;
		if (arr instanceof long[]) return ((long[])arr).length;
		if (arr instanceof UnsignedLongs) return ((UnsignedLongs)arr).value.length;
		if (arr instanceof CDouble[]) return ((CDouble[])arr).length;
		if (arr instanceof CSingle[]) return ((CSingle[])arr).length;
		if (arr instanceof boolean[]) return ((boolean[])arr).length;
		
		throw new RuntimeException(new DataTypeException("Invalid memory data type"));
		
    	
    }
    
	public static void arraycopy(Object src, int srcPos, Object dest, int destPos, int length)
	{
		if (src instanceof UnsignedBytes)
		{
			UnsignedBytes src2=(UnsignedBytes)src;
			UnsignedBytes dest2=(UnsignedBytes)dest;
			System.arraycopy(src2.value,srcPos,dest2.value,destPos,length);
			return;
		}
		
		if (src instanceof UnsignedShorts)
		{
			UnsignedShorts src2=(UnsignedShorts)src;
			UnsignedShorts dest2=(UnsignedShorts)dest;
			System.arraycopy(src2.value,srcPos,dest2.value,destPos,length);
			return;
		}
		
		if (src instanceof UnsignedInts)
		{
			UnsignedInts src2=(UnsignedInts)src;
			UnsignedInts dest2=(UnsignedInts)dest;
			System.arraycopy(src2.value,srcPos,dest2.value,destPos,length);
			return;
		}
		
		if (src instanceof UnsignedLongs)
		{
			UnsignedLongs src2=(UnsignedLongs)src;
			UnsignedLongs dest2=(UnsignedLongs)dest;
			System.arraycopy(src2.value,srcPos,dest2.value,destPos,length); 
			return;
		}
		
		System.arraycopy(src,srcPos,dest,destPos,length);
	}
	
	public static <T> T verifyArrayLength(T a, int len, boolean varlength)
    {
        if (a == null) throw new NullPointerException();
        if (len != 0)
        {
            if (varlength && Array.getLength(a) > len)
            {
                throw new DataTypeException("Array dimension mismatch");
            }
            if (!varlength && Array.getLength(a) != len)
            {
                throw new DataTypeException("Array dimension mismatch");
            }
        }
        return a;
    }


    public static MultiDimArray verifyArrayLength(MultiDimArray a, int n_elems, int[] len)
    {
        if (a.dims.length != len.length)
        {
            throw new DataTypeException("Array dimension mismatch");
        }

        for (int i = 0; i < len.length; i++)
        {
            if (a.dims[i] != len[i])
            {
                throw new DataTypeException("Array dimension mismatch");
            }
        }
        return a;
    }

    public static PodMultiDimArray verifyArrayLength(PodMultiDimArray a, int n_elems, int[] len)
    {
        if (a.dims.length != len.length)
        {
            throw new DataTypeException("Array dimension mismatch");
        }

        for (int i = 0; i < len.length; i++)
        {
            if (a.dims[i] != len[i])
            {
                throw new DataTypeException("Array dimension mismatch");
            }
        }
        return a;
    }

    public static NamedMultiDimArray verifyArrayLength(NamedMultiDimArray a, int n_elems, int[] len)
    {
        if (a.dims.length != len.length)
        {
            throw new DataTypeException("Array dimension mismatch");
        }

        for (int i = 0; i < len.length; i++)
        {
            if (a.dims[i] != len[i])
            {
                throw new DataTypeException("Array dimension mismatch");
            }
        }
        return a;
    }
    
    public static <T> List<T> verifyArrayLength1(List<T> a, int len, boolean varlength)
    {
    	if (a!=null)
    	{
	        for (T aa : a)
	        {        	
	            verifyArrayLength(aa, len, varlength);
	        }
    	}

        return a;
    }

    public static <K,T> Map<K, T> verifyArrayLength1(Map<K, T> a, int len, boolean varlength)
    {
    	if (a != null)
    	{
        for (T aa : a.values())
        {
            verifyArrayLength(aa, len, varlength);
        }
    	}
        return a;
    }

    public static List<MultiDimArray> verifyArrayLength2(List<MultiDimArray> a, int n_elems, int[] len)
    {
    	if (a!=null)
    	{
	        for (MultiDimArray aa : a)
	        {
	            verifyArrayLength(aa, n_elems, len);
	        }
    	}

        return a;
    }

    public static <K> Map<K, MultiDimArray> verifyArrayLength2(Map<K, MultiDimArray> a, int n_elems, int[] len)
    {
    	if (a!=null)
    	{
	        for (MultiDimArray aa : a.values())
	        {
	            verifyArrayLength(aa, n_elems, len);
	        }
    	}

        return a;
    }

    public static List<PodMultiDimArray> verifyArrayLength3(List<PodMultiDimArray> a, int n_elems, int[] len)
    {
    	if (a!=null)
    	{
	        for (PodMultiDimArray aa : a)
	        {
	            verifyArrayLength(aa, n_elems, len);
	        }
    	}

        return a;
    }

    public static <K> Map<K, PodMultiDimArray> verifyArrayLength3(Map<K, PodMultiDimArray> a, int n_elems, int[] len)
    {
    	if (a!=null)
    	{
	        for (PodMultiDimArray aa : a.values())
	        {
	            verifyArrayLength(aa, n_elems,len);
	        }
    	}

        return a;
    }
    
    public static List<NamedMultiDimArray> verifyArrayLength4(List<NamedMultiDimArray> a, int n_elems, int[] len)
    {
    	if (a!=null)
    	{
	        for (NamedMultiDimArray aa : a)
	        {
	            verifyArrayLength(aa, n_elems, len);
	        }
    	}

        return a;
    }

    public static <K> Map<K, NamedMultiDimArray> verifyArrayLength4(Map<K, NamedMultiDimArray> a, int n_elems, int[] len)
    {
    	if (a!=null)
    	{
	        for (NamedMultiDimArray aa : a.values())
	        {
	            verifyArrayLength(aa, n_elems,len);
	        }
    	}

        return a;
    }
	
}
