/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 4.0.2
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package com.robotraconteur;

public class NodeDirectoriesResourceAlreadyInUse {
  private transient long swigCPtr;
  protected transient boolean swigCMemOwn;

  protected NodeDirectoriesResourceAlreadyInUse(long cPtr, boolean cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = cPtr;
  }

  protected static long getCPtr(NodeDirectoriesResourceAlreadyInUse obj) {
    return (obj == null) ? 0 : obj.swigCPtr;
  }

  @SuppressWarnings("deprecation")
  protected void finalize() {
    delete();
  }

  public synchronized void delete() {
    if (swigCPtr != 0) {
      if (swigCMemOwn) {
        swigCMemOwn = false;
        RobotRaconteurJavaJNI.delete_NodeDirectoriesResourceAlreadyInUse(swigCPtr);
      }
      swigCPtr = 0;
    }
  }

  public NodeDirectoriesResourceAlreadyInUse() {
    this(RobotRaconteurJavaJNI.new_NodeDirectoriesResourceAlreadyInUse(), true);
  }

}
