/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (https://www.swig.org).
 * Version 4.1.1
 *
 * Do not make changes to this file unless you know what you are doing - modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package com.robotraconteur;

public class vectorptr_typedefinition extends java.util.AbstractList<TypeDefinition> implements java.util.RandomAccess {
  private transient long swigCPtr;
  protected transient boolean swigCMemOwn;

  protected vectorptr_typedefinition(long cPtr, boolean cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = cPtr;
  }

  protected static long getCPtr(vectorptr_typedefinition obj) {
    return (obj == null) ? 0 : obj.swigCPtr;
  }

  protected static long swigRelease(vectorptr_typedefinition obj) {
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
        RobotRaconteurJavaJNI.delete_vectorptr_typedefinition(swigCPtr);
      }
      swigCPtr = 0;
    }
  }

  public vectorptr_typedefinition(TypeDefinition[] initialElements) {
    this();
    reserve(initialElements.length);

    for (TypeDefinition element : initialElements) {
      add(element);
    }
  }

  public vectorptr_typedefinition(Iterable<TypeDefinition> initialElements) {
    this();
    for (TypeDefinition element : initialElements) {
      add(element);
    }
  }

  public TypeDefinition get(int index) {
    return doGet(index);
  }

  public TypeDefinition set(int index, TypeDefinition e) {
    return doSet(index, e);
  }

  public boolean add(TypeDefinition e) {
    modCount++;
    doAdd(e);
    return true;
  }

  public void add(int index, TypeDefinition e) {
    modCount++;
    doAdd(index, e);
  }

  public TypeDefinition remove(int index) {
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

  public vectorptr_typedefinition() {
    this(RobotRaconteurJavaJNI.new_vectorptr_typedefinition__SWIG_0(), true);
  }

  public vectorptr_typedefinition(vectorptr_typedefinition other) {
    this(RobotRaconteurJavaJNI.new_vectorptr_typedefinition__SWIG_1(vectorptr_typedefinition.getCPtr(other), other), true);
  }

  public long capacity() {
    return RobotRaconteurJavaJNI.vectorptr_typedefinition_capacity(swigCPtr, this);
  }

  public void reserve(long n) {
    RobotRaconteurJavaJNI.vectorptr_typedefinition_reserve(swigCPtr, this, n);
  }

  public boolean isEmpty() {
    return RobotRaconteurJavaJNI.vectorptr_typedefinition_isEmpty(swigCPtr, this);
  }

  public void clear() {
    RobotRaconteurJavaJNI.vectorptr_typedefinition_clear(swigCPtr, this);
  }

  public vectorptr_typedefinition(int count, TypeDefinition value) {
    this(RobotRaconteurJavaJNI.new_vectorptr_typedefinition__SWIG_2(count, TypeDefinition.getCPtr(value), value), true);
  }

  private int doSize() {
    return RobotRaconteurJavaJNI.vectorptr_typedefinition_doSize(swigCPtr, this);
  }

  private void doAdd(TypeDefinition x) {
    RobotRaconteurJavaJNI.vectorptr_typedefinition_doAdd__SWIG_0(swigCPtr, this, TypeDefinition.getCPtr(x), x);
  }

  private void doAdd(int index, TypeDefinition x) {
    RobotRaconteurJavaJNI.vectorptr_typedefinition_doAdd__SWIG_1(swigCPtr, this, index, TypeDefinition.getCPtr(x), x);
  }

  private TypeDefinition doRemove(int index) {
    long cPtr = RobotRaconteurJavaJNI.vectorptr_typedefinition_doRemove(swigCPtr, this, index);
    return (cPtr == 0) ? null : new TypeDefinition(cPtr, true);
  }

  private TypeDefinition doGet(int index) {
    long cPtr = RobotRaconteurJavaJNI.vectorptr_typedefinition_doGet(swigCPtr, this, index);
    return (cPtr == 0) ? null : new TypeDefinition(cPtr, true);
  }

  private TypeDefinition doSet(int index, TypeDefinition val) {
    long cPtr = RobotRaconteurJavaJNI.vectorptr_typedefinition_doSet(swigCPtr, this, index, TypeDefinition.getCPtr(val), val);
    return (cPtr == 0) ? null : new TypeDefinition(cPtr, true);
  }

  private void doRemoveRange(int fromIndex, int toIndex) {
    RobotRaconteurJavaJNI.vectorptr_typedefinition_doRemoveRange(swigCPtr, this, fromIndex, toIndex);
  }

}
