/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (https://www.swig.org).
 * Version 4.1.1
 *
 * Do not make changes to this file unless you know what you are doing - modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package com.robotraconteur;

public class ServiceInfo2Wrapped {
  private transient long swigCPtr;
  protected transient boolean swigCMemOwn;

  protected ServiceInfo2Wrapped(long cPtr, boolean cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = cPtr;
  }

  protected static long getCPtr(ServiceInfo2Wrapped obj) {
    return (obj == null) ? 0 : obj.swigCPtr;
  }

  protected static long swigRelease(ServiceInfo2Wrapped obj) {
    long ptr = 0;
    if (obj != null) {
      if (!obj.swigCMemOwn)
        throw new RuntimeException("Cannot release ownership as memory is not owned");
      ptr = obj.swigCPtr;
      obj.swigCMemOwn = false;
      obj.delete();
    }
    return ptr;
  }

  @SuppressWarnings("deprecation")
  protected void finalize() {
    delete();
  }

  public synchronized void delete() {
    if (swigCPtr != 0) {
      if (swigCMemOwn) {
        swigCMemOwn = false;
        RobotRaconteurJavaJNI.delete_ServiceInfo2Wrapped(swigCPtr);
      }
      swigCPtr = 0;
    }
  }

  public void setName(String value) {
    RobotRaconteurJavaJNI.ServiceInfo2Wrapped_Name_set(swigCPtr, this, value);
  }

  public String getName() {
    return RobotRaconteurJavaJNI.ServiceInfo2Wrapped_Name_get(swigCPtr, this);
  }

  public void setRootObjectType(String value) {
    RobotRaconteurJavaJNI.ServiceInfo2Wrapped_RootObjectType_set(swigCPtr, this, value);
  }

  public String getRootObjectType() {
    return RobotRaconteurJavaJNI.ServiceInfo2Wrapped_RootObjectType_get(swigCPtr, this);
  }

  public void setRootObjectImplements(vectorstring value) {
    RobotRaconteurJavaJNI.ServiceInfo2Wrapped_RootObjectImplements_set(swigCPtr, this, vectorstring.getCPtr(value), value);
  }

  public vectorstring getRootObjectImplements() {
    long cPtr = RobotRaconteurJavaJNI.ServiceInfo2Wrapped_RootObjectImplements_get(swigCPtr, this);
    return (cPtr == 0) ? null : new vectorstring(cPtr, false);
  }

  public void setConnectionURL(vectorstring value) {
    RobotRaconteurJavaJNI.ServiceInfo2Wrapped_ConnectionURL_set(swigCPtr, this, vectorstring.getCPtr(value), value);
  }

  public vectorstring getConnectionURL() {
    long cPtr = RobotRaconteurJavaJNI.ServiceInfo2Wrapped_ConnectionURL_get(swigCPtr, this);
    return (cPtr == 0) ? null : new vectorstring(cPtr, false);
  }

  public void setAttributes(MessageElement value) {
    RobotRaconteurJavaJNI.ServiceInfo2Wrapped_Attributes_set(swigCPtr, this, MessageElement.getCPtr(value), value);
  }

  public MessageElement getAttributes() {
    long cPtr = RobotRaconteurJavaJNI.ServiceInfo2Wrapped_Attributes_get(swigCPtr, this);
    return (cPtr == 0) ? null : new MessageElement(cPtr, true);
  }

  public void setNodeID(NodeID value) {
    RobotRaconteurJavaJNI.ServiceInfo2Wrapped_NodeID_set(swigCPtr, this, NodeID.getCPtr(value), value);
  }

  public NodeID getNodeID() {
    long cPtr = RobotRaconteurJavaJNI.ServiceInfo2Wrapped_NodeID_get(swigCPtr, this);
    return (cPtr == 0) ? null : new NodeID(cPtr, false);
  }

  public void setNodeName(String value) {
    RobotRaconteurJavaJNI.ServiceInfo2Wrapped_NodeName_set(swigCPtr, this, value);
  }

  public String getNodeName() {
    return RobotRaconteurJavaJNI.ServiceInfo2Wrapped_NodeName_get(swigCPtr, this);
  }

  public ServiceInfo2Wrapped() {
    this(RobotRaconteurJavaJNI.new_ServiceInfo2Wrapped(), true);
  }

}
