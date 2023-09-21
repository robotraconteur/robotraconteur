/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 4.0.2
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package com.robotraconteur;

public class MessageElement extends RRValue {
  private transient long swigCPtr;
  private transient boolean swigCMemOwnDerived;

  protected MessageElement(long cPtr, boolean cMemoryOwn) {
    super(RobotRaconteurJavaJNI.MessageElement_SWIGSmartPtrUpcast(cPtr), true);
    swigCMemOwnDerived = cMemoryOwn;
    swigCPtr = cPtr;
  }

  protected static long getCPtr(MessageElement obj) {
    return (obj == null) ? 0 : obj.swigCPtr;
  }

  @SuppressWarnings("deprecation")
  protected void finalize() {
    delete();
  }

  public synchronized void delete() {
    if(swigCPtr != 0 && swigCMemOwnDerived) {
      swigCMemOwnDerived = false;
      RobotRaconteurJavaJNI.delete_MessageElement(swigCPtr);
    }
    swigCPtr = 0;
    super.delete();
  }

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
  

  public void setElementSize(long value) {
    RobotRaconteurJavaJNI.MessageElement_ElementSize_set(swigCPtr, this, value);
  }

  public long getElementSize() {
    return RobotRaconteurJavaJNI.MessageElement_ElementSize_get(swigCPtr, this);
  }

  public void setElementFlags(short value) {
    RobotRaconteurJavaJNI.MessageElement_ElementFlags_set(swigCPtr, this, value);
  }

  public short getElementFlags() {
    return RobotRaconteurJavaJNI.MessageElement_ElementFlags_get(swigCPtr, this);
  }

  public void setElementName(String value) {
    RobotRaconteurJavaJNI.MessageElement_ElementName_set(swigCPtr, this, value);
  }

  public String getElementName() {
    return RobotRaconteurJavaJNI.MessageElement_ElementName_get(swigCPtr, this);
  }

  public void setElementNameCode(long value) {
    RobotRaconteurJavaJNI.MessageElement_ElementNameCode_set(swigCPtr, this, value);
  }

  public long getElementNameCode() {
    return RobotRaconteurJavaJNI.MessageElement_ElementNameCode_get(swigCPtr, this);
  }

  public void setElementNumber(int value) {
    RobotRaconteurJavaJNI.MessageElement_ElementNumber_set(swigCPtr, this, value);
  }

  public int getElementNumber() {
    return RobotRaconteurJavaJNI.MessageElement_ElementNumber_get(swigCPtr, this);
  }

  public void setElementType(DataTypes value) {
    RobotRaconteurJavaJNI.MessageElement_ElementType_set(swigCPtr, this, value.swigValue());
  }

  public DataTypes getElementType() {
    return DataTypes.swigToEnum(RobotRaconteurJavaJNI.MessageElement_ElementType_get(swigCPtr, this));
  }

  public void setElementTypeName(String value) {
    RobotRaconteurJavaJNI.MessageElement_ElementTypeName_set(swigCPtr, this, value);
  }

  public String getElementTypeName() {
    return RobotRaconteurJavaJNI.MessageElement_ElementTypeName_get(swigCPtr, this);
  }

  public void setElementTypeNameCode(long value) {
    RobotRaconteurJavaJNI.MessageElement_ElementTypeNameCode_set(swigCPtr, this, value);
  }

  public long getElementTypeNameCode() {
    return RobotRaconteurJavaJNI.MessageElement_ElementTypeNameCode_get(swigCPtr, this);
  }

  public void setMetaData(String value) {
    RobotRaconteurJavaJNI.MessageElement_MetaData_set(swigCPtr, this, value);
  }

  public String getMetaData() {
    return RobotRaconteurJavaJNI.MessageElement_MetaData_get(swigCPtr, this);
  }

  public void setExtended(vector_uint8_t value) {
    RobotRaconteurJavaJNI.MessageElement_Extended_set(swigCPtr, this, vector_uint8_t.getCPtr(value), value);
  }

  public vector_uint8_t getExtended() {
    long cPtr = RobotRaconteurJavaJNI.MessageElement_Extended_get(swigCPtr, this);
    return (cPtr == 0) ? null : new vector_uint8_t(cPtr, false);
  }

  public void setDataCount(long value) {
    RobotRaconteurJavaJNI.MessageElement_DataCount_set(swigCPtr, this, value);
  }

  public long getDataCount() {
    return RobotRaconteurJavaJNI.MessageElement_DataCount_get(swigCPtr, this);
  }

  public MessageElement() {
    this(RobotRaconteurJavaJNI.new_MessageElement(), true);
  }

  private MessageElementData _GetData() {
    long cPtr = RobotRaconteurJavaJNI.MessageElement__GetData(swigCPtr, this);
    return (cPtr == 0) ? null : new MessageElementData(cPtr, true);
  }

  private void _SetData(MessageElementData value) {
    RobotRaconteurJavaJNI.MessageElement__SetData(swigCPtr, this, MessageElementData.getCPtr(value), value);
  }

  public long computeSize() {
    return RobotRaconteurJavaJNI.MessageElement_computeSize(swigCPtr, this);
  }

  public void updateData() {
    RobotRaconteurJavaJNI.MessageElement_updateData(swigCPtr, this);
  }

  public static MessageElement findElement(vectorptr_messageelement m, String name) {
    long cPtr = RobotRaconteurJavaJNI.MessageElement_findElement(vectorptr_messageelement.getCPtr(m), m, name);
    return (cPtr == 0) ? null : new MessageElement(cPtr, true);
  }

  public static boolean containsElement(vectorptr_messageelement m, String name) {
    return RobotRaconteurJavaJNI.MessageElement_containsElement(vectorptr_messageelement.getCPtr(m), m, name);
  }

}
