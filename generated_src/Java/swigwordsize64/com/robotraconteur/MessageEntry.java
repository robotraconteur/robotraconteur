/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (https://www.swig.org).
 * Version 4.1.1
 *
 * Do not make changes to this file unless you know what you are doing - modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package com.robotraconteur;

public class MessageEntry extends RRValue {
  private transient long swigCPtr;
  private transient boolean swigCMemOwnDerived;

  protected MessageEntry(long cPtr, boolean cMemoryOwn) {
    super(RobotRaconteurJavaJNI.MessageEntry_SWIGSmartPtrUpcast(cPtr), true);
    swigCMemOwnDerived = cMemoryOwn;
    swigCPtr = cPtr;
  }

  protected static long getCPtr(MessageEntry obj) {
    return (obj == null) ? 0 : obj.swigCPtr;
  }

  @SuppressWarnings("deprecation")
  protected void finalize() {
    delete();
  }

  public synchronized void delete() {
    if(swigCPtr != 0 && swigCMemOwnDerived) {
      swigCMemOwnDerived = false;
      RobotRaconteurJavaJNI.delete_MessageEntry(swigCPtr);
    }
    swigCPtr = 0;
    super.delete();
  }


	public final MessageElement addElement(String name, Object data)
	{
		MessageElement m = new MessageElement(name, data);
		this.addElement(m);
		m.delete();
		return m;

	}


  public void setEntrySize(long value) {
    RobotRaconteurJavaJNI.MessageEntry_EntrySize_set(swigCPtr, this, value);
  }

  public long getEntrySize() {
    return RobotRaconteurJavaJNI.MessageEntry_EntrySize_get(swigCPtr, this);
  }

  public void setEntryFlags(short value) {
    RobotRaconteurJavaJNI.MessageEntry_EntryFlags_set(swigCPtr, this, value);
  }

  public short getEntryFlags() {
    return RobotRaconteurJavaJNI.MessageEntry_EntryFlags_get(swigCPtr, this);
  }

  public void setEntryType(MessageEntryType value) {
    RobotRaconteurJavaJNI.MessageEntry_EntryType_set(swigCPtr, this, value.swigValue());
  }

  public MessageEntryType getEntryType() {
    return MessageEntryType.swigToEnum(RobotRaconteurJavaJNI.MessageEntry_EntryType_get(swigCPtr, this));
  }

  public void setServicePath(String value) {
    RobotRaconteurJavaJNI.MessageEntry_ServicePath_set(swigCPtr, this, value);
  }

  public String getServicePath() {
    return RobotRaconteurJavaJNI.MessageEntry_ServicePath_get(swigCPtr, this);
  }

  public void setMemberName(String value) {
    RobotRaconteurJavaJNI.MessageEntry_MemberName_set(swigCPtr, this, value);
  }

  public String getMemberName() {
    return RobotRaconteurJavaJNI.MessageEntry_MemberName_get(swigCPtr, this);
  }

  public void setMemberNameCode(long value) {
    RobotRaconteurJavaJNI.MessageEntry_MemberNameCode_set(swigCPtr, this, value);
  }

  public long getMemberNameCode() {
    return RobotRaconteurJavaJNI.MessageEntry_MemberNameCode_get(swigCPtr, this);
  }

  public void setRequestID(long value) {
    RobotRaconteurJavaJNI.MessageEntry_RequestID_set(swigCPtr, this, value);
  }

  public long getRequestID() {
    return RobotRaconteurJavaJNI.MessageEntry_RequestID_get(swigCPtr, this);
  }

  public void setError(MessageErrorType value) {
    RobotRaconteurJavaJNI.MessageEntry_Error_set(swigCPtr, this, value.swigValue());
  }

  public MessageErrorType getError() {
    return MessageErrorType.swigToEnum(RobotRaconteurJavaJNI.MessageEntry_Error_get(swigCPtr, this));
  }

  public void setMetaData(String value) {
    RobotRaconteurJavaJNI.MessageEntry_MetaData_set(swigCPtr, this, value);
  }

  public String getMetaData() {
    return RobotRaconteurJavaJNI.MessageEntry_MetaData_get(swigCPtr, this);
  }

  public void setExtended(vector_uint8_t value) {
    RobotRaconteurJavaJNI.MessageEntry_Extended_set(swigCPtr, this, vector_uint8_t.getCPtr(value), value);
  }

  public vector_uint8_t getExtended() {
    long cPtr = RobotRaconteurJavaJNI.MessageEntry_Extended_get(swigCPtr, this);
    return (cPtr == 0) ? null : new vector_uint8_t(cPtr, false);
  }

  public void setElements(vectorptr_messageelement value) {
    RobotRaconteurJavaJNI.MessageEntry_elements_set(swigCPtr, this, vectorptr_messageelement.getCPtr(value), value);
  }

  public vectorptr_messageelement getElements() {
    long cPtr = RobotRaconteurJavaJNI.MessageEntry_elements_get(swigCPtr, this);
    return (cPtr == 0) ? null : new vectorptr_messageelement(cPtr, false);
  }

  public MessageEntry() {
    this(RobotRaconteurJavaJNI.new_MessageEntry__SWIG_0(), true);
  }

  public MessageEntry(MessageEntryType t, String n) {
    this(RobotRaconteurJavaJNI.new_MessageEntry__SWIG_1(t.swigValue(), n), true);
  }

  public long computeSize() {
    return RobotRaconteurJavaJNI.MessageEntry_computeSize(swigCPtr, this);
  }

  public MessageElement findElement(String name) {
    long cPtr = RobotRaconteurJavaJNI.MessageEntry_findElement(swigCPtr, this, name);
    return (cPtr == 0) ? null : new MessageElement(cPtr, true);
  }

  public MessageElement addElement(MessageElement m) {
    long cPtr = RobotRaconteurJavaJNI.MessageEntry_addElement(swigCPtr, this, MessageElement.getCPtr(m), m);
    return (cPtr == 0) ? null : new MessageElement(cPtr, true);
  }

}
