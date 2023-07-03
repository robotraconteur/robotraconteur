/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (https://www.swig.org).
 * Version 4.1.1
 *
 * Do not make changes to this file unless you know what you are doing - modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package com.robotraconteur;

public class WrappedWireBroadcaster {
  private transient long swigCPtr;
  private transient boolean swigCMemOwn;

  protected WrappedWireBroadcaster(long cPtr, boolean cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = cPtr;
  }

  protected static long getCPtr(WrappedWireBroadcaster obj) {
    return (obj == null) ? 0 : obj.swigCPtr;
  }

  protected void swigSetCMemOwn(boolean own) {
    swigCMemOwn = own;
  }

  @SuppressWarnings("deprecation")
  protected void finalize() {
    delete();
  }

  public synchronized void delete() {
    if (swigCPtr != 0) {
      if (swigCMemOwn) {
        swigCMemOwn = false;
        RobotRaconteurJavaJNI.delete_WrappedWireBroadcaster(swigCPtr);
      }
      swigCPtr = 0;
    }
  }

  public void init(WrappedWireServer wire) {
    RobotRaconteurJavaJNI.WrappedWireBroadcaster_init(swigCPtr, this, WrappedWireServer.getCPtr(wire), wire);
  }

  public void setOutValue(MessageElement value) {
    RobotRaconteurJavaJNI.WrappedWireBroadcaster_setOutValue(swigCPtr, this, MessageElement.getCPtr(value), value);
  }

  public long getActiveWireConnectionCount() {
    return RobotRaconteurJavaJNI.WrappedWireBroadcaster_getActiveWireConnectionCount(swigCPtr, this);
  }

  public void setPredicateDirector(WrappedWireBroadcasterPredicateDirector f, int id) {
    RobotRaconteurJavaJNI.WrappedWireBroadcaster_setPredicateDirector(swigCPtr, this, WrappedWireBroadcasterPredicateDirector.getCPtr(f), f, id);
  }

  public int getOutValueLifespan() {
    return RobotRaconteurJavaJNI.WrappedWireBroadcaster_getOutValueLifespan(swigCPtr, this);
  }

  public void setOutValueLifespan(int millis) {
    RobotRaconteurJavaJNI.WrappedWireBroadcaster_setOutValueLifespan(swigCPtr, this, millis);
  }

  public void setPeekInValueCallback(WrappedWireServerPeekValueDirector director, int id) {
    RobotRaconteurJavaJNI.WrappedWireBroadcaster_setPeekInValueCallback(swigCPtr, this, WrappedWireServerPeekValueDirector.getCPtr(director), director, id);
  }

  public WrappedWireBroadcaster() {
    this(RobotRaconteurJavaJNI.new_WrappedWireBroadcaster(), true);
  }

}
