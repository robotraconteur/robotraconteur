/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 4.0.2
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package com.robotraconteur;

public class vectornodeinfo2 extends java.util.AbstractList<WrappedNodeInfo2> implements java.util.RandomAccess {
  private transient long swigCPtr;
  protected transient boolean swigCMemOwn;

  protected vectornodeinfo2(long cPtr, boolean cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = cPtr;
  }

  protected static long getCPtr(vectornodeinfo2 obj) {
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
        RobotRaconteurJavaJNI.delete_vectornodeinfo2(swigCPtr);
      }
      swigCPtr = 0;
    }
  }

  public vectornodeinfo2(WrappedNodeInfo2[] initialElements) {
    this();
    reserve(initialElements.length);

    for (WrappedNodeInfo2 element : initialElements) {
      add(element);
    }
  }

  public vectornodeinfo2(Iterable<WrappedNodeInfo2> initialElements) {
    this();
    for (WrappedNodeInfo2 element : initialElements) {
      add(element);
    }
  }

  public WrappedNodeInfo2 get(int index) {
    return doGet(index);
  }

  public WrappedNodeInfo2 set(int index, WrappedNodeInfo2 e) {
    return doSet(index, e);
  }

  public boolean add(WrappedNodeInfo2 e) {
    modCount++;
    doAdd(e);
    return true;
  }

  public void add(int index, WrappedNodeInfo2 e) {
    modCount++;
    doAdd(index, e);
  }

  public WrappedNodeInfo2 remove(int index) {
    modCount++;
    return doRemove(index);
  }

  protected void removeRange(int fromIndex, int toIndex) {
    modCount++;
    doRemoveRange(fromIndex, toIndex);
  }

  public int size() {
    return doSize();
  }

  public vectornodeinfo2() {
    this(RobotRaconteurJavaJNI.new_vectornodeinfo2__SWIG_0(), true);
  }

  public vectornodeinfo2(vectornodeinfo2 other) {
    this(RobotRaconteurJavaJNI.new_vectornodeinfo2__SWIG_1(vectornodeinfo2.getCPtr(other), other), true);
  }

  public long capacity() {
    return RobotRaconteurJavaJNI.vectornodeinfo2_capacity(swigCPtr, this);
  }

  public void reserve(long n) {
    RobotRaconteurJavaJNI.vectornodeinfo2_reserve(swigCPtr, this, n);
  }

  public boolean isEmpty() {
    return RobotRaconteurJavaJNI.vectornodeinfo2_isEmpty(swigCPtr, this);
  }

  public void clear() {
    RobotRaconteurJavaJNI.vectornodeinfo2_clear(swigCPtr, this);
  }

  public vectornodeinfo2(int count, WrappedNodeInfo2 value) {
    this(RobotRaconteurJavaJNI.new_vectornodeinfo2__SWIG_2(count, WrappedNodeInfo2.getCPtr(value), value), true);
  }

  private int doSize() {
    return RobotRaconteurJavaJNI.vectornodeinfo2_doSize(swigCPtr, this);
  }

  private void doAdd(WrappedNodeInfo2 x) {
    RobotRaconteurJavaJNI.vectornodeinfo2_doAdd__SWIG_0(swigCPtr, this, WrappedNodeInfo2.getCPtr(x), x);
  }

  private void doAdd(int index, WrappedNodeInfo2 x) {
    RobotRaconteurJavaJNI.vectornodeinfo2_doAdd__SWIG_1(swigCPtr, this, index, WrappedNodeInfo2.getCPtr(x), x);
  }

  private WrappedNodeInfo2 doRemove(int index) {
    return new WrappedNodeInfo2(RobotRaconteurJavaJNI.vectornodeinfo2_doRemove(swigCPtr, this, index), true);
  }

  private WrappedNodeInfo2 doGet(int index) {
    return new WrappedNodeInfo2(RobotRaconteurJavaJNI.vectornodeinfo2_doGet(swigCPtr, this, index), false);
  }

  private WrappedNodeInfo2 doSet(int index, WrappedNodeInfo2 val) {
    return new WrappedNodeInfo2(RobotRaconteurJavaJNI.vectornodeinfo2_doSet(swigCPtr, this, index, WrappedNodeInfo2.getCPtr(val), val), true);
  }

  private void doRemoveRange(int fromIndex, int toIndex) {
    RobotRaconteurJavaJNI.vectornodeinfo2_doRemoveRange(swigCPtr, this, fromIndex, toIndex);
  }

}
