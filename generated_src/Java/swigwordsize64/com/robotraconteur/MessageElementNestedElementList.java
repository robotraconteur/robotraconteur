/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 4.0.2
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package com.robotraconteur;

public class MessageElementNestedElementList extends MessageElementData {
  private transient long swigCPtr;
  private transient boolean swigCMemOwnDerived;

  protected MessageElementNestedElementList(long cPtr, boolean cMemoryOwn) {
    super(RobotRaconteurJavaJNI.MessageElementNestedElementList_SWIGSmartPtrUpcast(cPtr), true);
    swigCMemOwnDerived = cMemoryOwn;
    swigCPtr = cPtr;
  }

  protected static long getCPtr(MessageElementNestedElementList obj) {
    return (obj == null) ? 0 : obj.swigCPtr;
  }

  @SuppressWarnings("deprecation")
  protected void finalize() {
    delete();
  }

  public synchronized void delete() {
    if(swigCPtr != 0 && swigCMemOwnDerived) {
      swigCMemOwnDerived = false;
      RobotRaconteurJavaJNI.delete_MessageElementNestedElementList(swigCPtr);
    }
    swigCPtr = 0;
    super.delete();
  }

  public void setType(DataTypes value) {
    RobotRaconteurJavaJNI.MessageElementNestedElementList_Type_set(swigCPtr, this, value.swigValue());
  }

  public DataTypes getType() {
    return DataTypes.swigToEnum(RobotRaconteurJavaJNI.MessageElementNestedElementList_Type_get(swigCPtr, this));
  }

  public void setTypeName(String value) {
    RobotRaconteurJavaJNI.MessageElementNestedElementList_TypeName_set(swigCPtr, this, value);
  }

  public String getTypeName() {
    return RobotRaconteurJavaJNI.MessageElementNestedElementList_TypeName_get(swigCPtr, this);
  }

  public void setElements(vectorptr_messageelement value) {
    RobotRaconteurJavaJNI.MessageElementNestedElementList_Elements_set(swigCPtr, this, vectorptr_messageelement.getCPtr(value), value);
  }

  public vectorptr_messageelement getElements() {
    long cPtr = RobotRaconteurJavaJNI.MessageElementNestedElementList_Elements_get(swigCPtr, this);
    return (cPtr == 0) ? null : new vectorptr_messageelement(cPtr, false);
  }

  public MessageElementNestedElementList(DataTypes type_, String type_name_, vectorptr_messageelement elements_) {
    this(RobotRaconteurJavaJNI.new_MessageElementNestedElementList(type_.swigValue(), type_name_, vectorptr_messageelement.getCPtr(elements_), elements_), true);
  }

  public String getTypeString() {
    return RobotRaconteurJavaJNI.MessageElementNestedElementList_getTypeString(swigCPtr, this);
  }

  public DataTypes getTypeID() {
    return DataTypes.swigToEnum(RobotRaconteurJavaJNI.MessageElementNestedElementList_getTypeID(swigCPtr, this));
  }

}