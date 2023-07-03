/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (https://www.swig.org).
 * Version 4.1.1
 *
 * Do not make changes to this file unless you know what you are doing - modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package com.robotraconteur;

public class Transport {
  private transient long swigCPtr;
  private transient boolean swigCMemOwn;

  protected Transport(long cPtr, boolean cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = cPtr;
  }

  protected static long getCPtr(Transport obj) {
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
        RobotRaconteurJavaJNI.delete_Transport(swigCPtr);
      }
      swigCPtr = 0;
    }
  }

  public String getUrlSchemeString() {
    return RobotRaconteurJavaJNI.Transport_getUrlSchemeString(swigCPtr, this);
  }

  public static String getCurrentTransportConnectionURL() {
    return RobotRaconteurJavaJNI.Transport_getCurrentTransportConnectionURL();
  }

  public vectorstring getServerListenUrls() {
    return new vectorstring(RobotRaconteurJavaJNI.Transport_getServerListenUrls(swigCPtr, this), true);
  }

}
