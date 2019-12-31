
%typemap (javacode) RobotRaconteur::MessageElement
%{
public MessageElement(String name, Object data) 
  {
	  this();
      setElementName(name);
      setData(data);
  }

  public Object getData()
  {
    MessageElementData val=null;
	try
	{
      val = _GetData();
      if (val == null) return null;
      Object a = null;
      a = MessageElementDataUtil.toRRBaseArray(val);
      if (a != null)
      {
          if (DataTypeUtil.isNumber(val.getTypeID()))
          {
			  RRBaseArray ra = (RRBaseArray)a;
			  try
			  {
				return MessageElementDataUtil.rRBaseArrayToArray(ra);
			  }
			  finally
			  {
				ra.delete();
			  }
          }
          if (val.getTypeID() == DataTypes.DataTypes_string_t)
          {
			  RRBaseArray ra = (RRBaseArray)a;
			  try
			  {
			    return MessageElementDataUtil.rRBaseArrayToString(ra);
			  }
			  finally
			  {
				ra.delete();
			  }
			  
              
          }
		  if (a!=null)
		  {
		  if (a instanceof MessageElementData)
		  {
		  ((MessageElementData)a).delete();
		  }
		  }
          throw new RuntimeException(new DataTypeException("Unknown RRArray type"));
              
      }
      a = MessageElementDataUtil.toMessageElementNestedElementList(val);
      if (a != null) return a;
      throw new RuntimeException(new DataTypeException( "Unknown data type"));
	}
	finally
	{
		if (val!=null)	val.delete();
	}
      
  }

  private void setData(Object dat)
  {
      if (dat == null)
      {
          _SetData(null);
          return;
      }
      if (DataTypeUtil.isArray(dat))
      {
		  RRBaseArray rb=null;
		  try
		  {
			  rb=MessageElementDataUtil.arrayToRRBaseArray(dat);
			  _SetData(rb);
		  }
		  finally
		  {
			if (rb!=null) rb.delete();
		  }
          return;
      }

	  if (DataTypeUtil.isScalarNumber(dat))
      {
		  RRBaseArray rb=null;
		  try
		  {
			  rb=MessageElementDataUtil.arrayToRRBaseArray(DataTypeUtil.arrayFromScalar(dat));
			  _SetData(rb);
		  }
		  finally
		  {
			if (rb!=null) rb.delete();
		  }
          return;
      }
	  
      if (dat instanceof String)
      {
		  RRBaseArray rb=null;
		  try
		  {
		  rb=MessageElementDataUtil.stringToRRBaseArray((String)dat);
          _SetData(rb);
		  }
		  finally
		  {
			if (rb!=null) rb.delete();
		  }
          return;
      }

      if (dat instanceof MessageElementData)
      {
          _SetData((MessageElementData)dat);
          return;
      }

      String datatype = dat.getClass().toString();
      DataTypes elementtype = DataTypeUtil.typeIDFromString(datatype);
      

      throw new RuntimeException(new DataTypeException("Invalid MessageElement data type"));

  }
  
  public <T> T castData()
	{
		if (getData() == null)
		{
			return null;
		}
		try
		{
			return (T)getData();
		}
		catch (Exception e)
		{
		throw new DataTypeException("Could not cast data");
		}
	}
  
%}


%typemap("javacode") RobotRaconteur::MessageEntry
%{

	public final MessageElement addElement(String name, Object data)
	{
		MessageElement m = new MessageElement(name, data);
		this.addElement(m);
		m.delete();
		return m;

	}

%}

%typemap(javacode) MessageElementDataUtil
%{
public static Object rRBaseArrayToArray(RRBaseArray a) 
  {
	if (a==null) return null;
	switch (a.getTypeID())
	{
		case DataTypes_double_t:
		{
	        double[] r=new double[(int)a.size()];
	        rRBaseArrayToDoubles(a,r,r.length);
	        return r;
		}
	    case DataTypes_single_t:
	    {
	        float[] r=new float[(int)a.size()];
	        rRBaseArrayToFloats(a,r,r.length);
	        return r;
		}
	    case DataTypes_int8_t:
	    {
	        byte[] r=new byte[(int)a.size()];
	        rRBaseArrayToBytes(a,r,r.length);
	        return r;
		}
	    case DataTypes_uint8_t:
	    
	    {
	        byte[] r=new byte[(int)a.size()];
	        rRBaseArrayToBytes(a,r,r.length);
	        return new UnsignedBytes(r);
		}
	    case DataTypes_int16_t:
	    {
	    	short[] r=new short[(int)a.size()];
	        rRBaseArrayToShorts(a,r,r.length);
	        return r;
	    }
	    case DataTypes_uint16_t:
	    {
	    	short[] r=new short[(int)a.size()];
	        rRBaseArrayToShorts(a,r,r.length);
	        return new UnsignedShorts(r);
	    }
	    case DataTypes_int32_t:
	    {
	    	int[] r=new int[(int)a.size()];
	        rRBaseArrayToInts(a,r,r.length);
	        return r;
	    }
	    case DataTypes_uint32_t:
	    {
	    	int[] r=new int[(int)a.size()];
	        rRBaseArrayToInts(a,r,r.length);
	        return new UnsignedInts(r);
	    }
	    case DataTypes_int64_t:
	    {
	    	long[] r=new long[(int)a.size()];
	        rRBaseArrayToLongs(a,r,r.length);
	        return r;
	    }
	    case DataTypes_uint64_t:
	    {
	    	long[] r=new long[(int)a.size()];
	        rRBaseArrayToLongs(a,r,r.length);
	        return new UnsignedLongs(r);
	    }
	    case DataTypes_cdouble_t:
	    {
	    	double[] o2=new double[(int)a.size()*2];
	    	rRBaseArrayComplexToDoubles(a,o2,o2.length);
	    	CDouble[] o3=new CDouble[(int)a.size()];
	    	for (int j=0; j<o3.length; j++)
	    	{
	    		o3[j] = new CDouble(o2[j *2], o2[j*2+1]);
	    	}
	    	return o3;
	    }
	    case DataTypes_csingle_t:
	    {
	    	float[] o2=new float[(int)a.size()*2];
	    	rRBaseArrayComplexToFloats(a,o2,o2.length);
	    	CSingle[] o3=new CSingle[(int)a.size()];
	    	for (int j=0; j<o3.length; j++)
	    	{
	    		o3[j] = new CSingle(o2[j *2], o2[j*2+1]);
	    	}
	    	return o3;
	    }
	    case DataTypes_bool_t:
	    {
	    	byte[] o2=new byte[(int)a.size()];
	    	rRBaseArrayBoolToBytes(a,o2,o2.length);
	    	boolean[] o3=new boolean[(int)a.size()];
	    	for (int j=0; j<o3.length; j++)
	    	{
	    		o3[j] = o2[j] != 0;
	    	}
	    	return o3;
	    }	    
	    default:
	    	break;
	
	}
	  
	throw new RuntimeException(new DataTypeException("Unknown RRArray type"));
  
  }
  
  public static RRBaseArray arrayToRRBaseArray(Object a) 
  {
	  if (a==null) return null;
	  if (a instanceof double[]) return doublesToRRBaseArray((double[])a,((double[])a).length);
	  if (a instanceof float[]) return floatsToRRBaseArray((float[])a,((float[])a).length);
	  if (a instanceof byte[]) return bytesToRRBaseArray((byte[])a,((byte[])a).length,DataTypes.DataTypes_int8_t);
	  if (a instanceof UnsignedBytes) return bytesToRRBaseArray(((UnsignedBytes)a).value,((UnsignedBytes)a).value.length,DataTypes.DataTypes_uint8_t);
	  if (a instanceof short[]) return shortsToRRBaseArray((short[])a,((short[])a).length,DataTypes.DataTypes_int16_t);
	  if (a instanceof UnsignedShorts) return shortsToRRBaseArray(((UnsignedShorts)a).value,((UnsignedShorts)a).value.length,DataTypes.DataTypes_uint16_t);
	  if (a instanceof int[]) return intsToRRBaseArray((int[])a,((int[])a).length,DataTypes.DataTypes_int32_t);
	  if (a instanceof UnsignedInts) return intsToRRBaseArray(((UnsignedInts)a).value,((UnsignedInts)a).value.length,DataTypes.DataTypes_uint32_t);
	  if (a instanceof long[]) return longsToRRBaseArray((long[])a,((long[])a).length,DataTypes.DataTypes_int64_t);
	  if (a instanceof UnsignedLongs) return longsToRRBaseArray(((UnsignedLongs)a).value,((UnsignedLongs)a).value.length,DataTypes.DataTypes_uint64_t);
	  if (a instanceof CDouble[])
	  {
		  CDouble[] a1=(CDouble[])a;
		  double[] b1=new double[a1.length * 2];
		  for (int j = 0; j<a1.length; j++)
          {
              b1[j * 2] = a1[j].real;
              b1[j * 2 + 1] = a1[j].imag;
          }
		  return doublesToComplexRRBaseArray(b1,b1.length);
	  }
	  if (a instanceof CSingle[])
	  {
		  CSingle[] a1=(CSingle[])a;
		  float[] b1=new float[a1.length * 2];
		  for (int j = 0; j<a1.length; j++)
          {
              b1[j * 2] = a1[j].real;
              b1[j * 2 + 1] = a1[j].imag;
          }
		  return floatsToComplexRRBaseArray(b1,b1.length);
	  }
	  if (a instanceof boolean[])
	  {
		  boolean[] a1=(boolean[])a;
		  byte[] b1=new byte[a1.length];
		  for (int j = 0; j<a1.length; j++)
          {
              b1[j] = (byte) (a1[j] ? 1 : 0);              
          }
		  return bytesToBoolRRBaseArray(b1,b1.length);
	  }
	  	  
	  throw new RuntimeException(new DataTypeException("Unknown Array type"));
	  
  }
  
  public static void arrayToRRBaseArray(Object a, RRBaseArray rra) 
  {
	  if (a instanceof double[]) {doublesToRRBaseArray((double[])a,((double[])a).length,rra); return; }
	  if (a instanceof float[]) {floatsToRRBaseArray((float[])a,((float[])a).length,rra); return;}
	  if (a instanceof byte[]) { bytesToRRBaseArray((byte[])a,((byte[])a).length,rra); return; }
	  if (a instanceof UnsignedBytes) {bytesToRRBaseArray(((UnsignedBytes)a).value,((UnsignedBytes)a).value.length,rra); return; }
	  if (a instanceof short[]) {shortsToRRBaseArray((short[])a,((short[])a).length,rra); return; }
	  if (a instanceof UnsignedShorts) {shortsToRRBaseArray(((UnsignedShorts)a).value,((UnsignedShorts)a).value.length,rra); return; }
	  if (a instanceof int[]) {intsToRRBaseArray((int[])a,((int[])a).length,rra); return; }
	  if (a instanceof UnsignedInts) { intsToRRBaseArray(((UnsignedInts)a).value,((UnsignedInts)a).value.length,rra); return; }
	  if (a instanceof long[]) { longsToRRBaseArray((long[])a,((long[])a).length,rra); return; }
	  if (a instanceof UnsignedLongs) { longsToRRBaseArray(((UnsignedLongs)a).value,((UnsignedLongs)a).value.length,rra); return; }
	  if (a instanceof CDouble[])
	  {
		  CDouble[] a1=(CDouble[])a;
		  double[] b1=new double[a1.length * 2];
		  for (int j = 0; j<a1.length; j++)
          {
              b1[j * 2] = a1[j].real;
              b1[j * 2 + 1] = a1[j].imag;
          }
		  doublesToComplexRRBaseArray(b1,b1.length,rra);
		  return;
	  }
	  if (a instanceof CSingle[])
	  {
		  CSingle[] a1=(CSingle[])a;
		  float[] b1=new float[a1.length * 2];
		  for (int j = 0; j<a1.length; j++)
          {
              b1[j * 2] = a1[j].real;
              b1[j * 2 + 1] = a1[j].imag;
          }
		  floatsToComplexRRBaseArray(b1,b1.length,rra);
		  return;
	  }
	  if (a instanceof boolean[])
	  {
		  boolean[] a1=(boolean[])a;
		  byte[] b1=new byte[a1.length];
		  for (int j = 0; j<a1.length; j++)
          {
              b1[j] = (byte) (a1[j] ? 1 : 0);              
          }
		  bytesToBoolRRBaseArray(b1,b1.length,rra);
		  return;
	  }	  
	  throw new RuntimeException(new DataTypeException("Unknown Array type"));
  }


%}

%typemap("javacode") MemberDefinitionUtil %{

public static MemberDefinition swigCast(MemberDefinition i) 
{
	MemberDefinition o=null;
	o=toProperty(i);
	if (o!=null) return (PropertyDefinition)o;
	o=toFunction(i);
	if (o!=null) return (FunctionDefinition)o;
	o=toEvent(i);
	if (o!=null) return (EventDefinition)o;
	o=toObjRef(i);
	if (o!=null) return (ObjRefDefinition)o;
	o=toPipe(i);
	if (o!=null) return (PipeDefinition)o;
	o=toCallback(i);
	if (o!=null) return (CallbackDefinition)o;
	o=toWire(i);
	if (o!=null) return (WireDefinition)o;
	o=toMemory(i);
	if (o!=null) return (MemoryDefinition)o;
	throw new RuntimeException (new Exception("Invalid MemberDefinition"));
	
}

%}

%apply double[] {double *};
%apply float[] {float *};
%apply int8_t[] {int8_t *};
%apply uint8_t[] {uint8_t *};
%apply int16_t[] {int16_t *};
%apply uint16_t[] {uint16_t *};
%apply int32_t[] {int32_t *};
%apply uint32_t[] {uint32_t *};
%apply int64_t[] {int64_t *};
%apply uint64_t[] {uint64_t*};

%include "Message.i"