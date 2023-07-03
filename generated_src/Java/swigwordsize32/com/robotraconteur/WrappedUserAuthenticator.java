/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (https://www.swig.org).
 * Version 4.1.1
 *
 * Do not make changes to this file unless you know what you are doing - modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package com.robotraconteur;

public class WrappedUserAuthenticator extends NativeUserAuthenticator {
  private transient long swigCPtr;
  private transient boolean swigCMemOwnDerived;

  protected WrappedUserAuthenticator(long cPtr, boolean cMemoryOwn) {
    super(RobotRaconteurJavaJNI.WrappedUserAuthenticator_SWIGSmartPtrUpcast(cPtr), true);
    swigCMemOwnDerived = cMemoryOwn;
    swigCPtr = cPtr;
  }

  protected static long getCPtr(WrappedUserAuthenticator obj) {
    return (obj == null) ? 0 : obj.swigCPtr;
  }

  protected void swigSetCMemOwn(boolean own) {
    swigCMemOwnDerived = own;
    super.swigSetCMemOwn(own);
  }

  @SuppressWarnings("deprecation")
  protected void finalize() {
    delete();
  }

  public synchronized void delete() {
    if (swigCPtr != 0) {
      if (swigCMemOwnDerived) {
        swigCMemOwnDerived = false;
        RobotRaconteurJavaJNI.delete_WrappedUserAuthenticator(swigCPtr);
      }
      swigCPtr = 0;
    }
    super.delete();
  }

  public void setRRDirector(WrappedUserAuthenticatorDirector director, int id) {
    RobotRaconteurJavaJNI.WrappedUserAuthenticator_setRRDirector(swigCPtr, this, WrappedUserAuthenticatorDirector.getCPtr(director), director, id);
  }

  public WrappedUserAuthenticator() {
    this(RobotRaconteurJavaJNI.new_WrappedUserAuthenticator(), true);
  }

}
