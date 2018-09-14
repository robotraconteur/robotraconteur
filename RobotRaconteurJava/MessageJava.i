
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
      a = MessageElementDataUtil.toMessageElementStructure(val);
      if (a != null) return a;
      a = MessageElementDataUtil.toMessageElementMap_int32_t(val);
      if (a != null) return a;
      a = MessageElementDataUtil.toMessageElementMap_string(val);
      if (a != null) return a;
	  a = MessageElementDataUtil.toMessageElementList(val);
      if (a != null) return a;
      a = MessageElementDataUtil.toMessageElementMultiDimArray(val);
      if (a != null) return a;
	  a = MessageElementDataUtil.toMessageElementCStructure(val);
      if (a != null) return a;
	  a = MessageElementDataUtil.toMessageElementCStructureArray(val);
      if (a != null) return a;
	  a = MessageElementDataUtil.toMessageElementCStructureMultiDimArray(val);
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
	        double[] r=new double[(int)a.length()];
	        rRBaseArrayToDoubles(a,r,r.length);
	        return r;
		}
	    case DataTypes_single_t:
	    {
	        float[] r=new float[(int)a.length()];
	        rRBaseArrayToFloats(a,r,r.length);
	        return r;
		}
	    case DataTypes_int8_t:
	    {
	        byte[] r=new byte[(int)a.length()];
	        rRBaseArrayToBytes(a,r,r.length);
	        return r;
		}
	    case DataTypes_uint8_t:
	    
	    {
	        byte[] r=new byte[(int)a.length()];
	        rRBaseArrayToBytes(a,r,r.length);
	        return new UnsignedBytes(r);
		}
	    case DataTypes_int16_t:
	    {
	    	short[] r=new short[(int)a.length()];
	        rRBaseArrayToShorts(a,r,r.length);
	        return r;
	    }
	    case DataTypes_uint16_t:
	    {
	    	short[] r=new short[(int)a.length()];
	        rRBaseArrayToShorts(a,r,r.length);
	        return new UnsignedShorts(r);
	    }
	    case DataTypes_int32_t:
	    {
	    	int[] r=new int[(int)a.length()];
	        rRBaseArrayToInts(a,r,r.length);
	        return r;
	    }
	    case DataTypes_uint32_t:
	    {
	    	int[] r=new int[(int)a.length()];
	        rRBaseArrayToInts(a,r,r.length);
	        return new UnsignedInts(r);
	    }
	    case DataTypes_int64_t:
	    {
	    	long[] r=new long[(int)a.length()];
	        rRBaseArrayToLongs(a,r,r.length);
	        return r;
	    }
	    case DataTypes_uint64_t:
	    {
	    	long[] r=new long[(int)a.length()];
	        rRBaseArrayToLongs(a,r,r.length);
	        return new UnsignedLongs(r);
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

%include "Message.i"