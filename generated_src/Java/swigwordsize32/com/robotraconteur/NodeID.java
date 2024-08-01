/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 4.0.2
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package com.robotraconteur;

import java.nio.*;

public class NodeID {
  private transient long swigCPtr;
  protected transient boolean swigCMemOwn;

  protected NodeID(long cPtr, boolean cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = cPtr;
  }

  protected static long getCPtr(NodeID obj) {
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
        RobotRaconteurJavaJNI.delete_NodeID(swigCPtr);
      }
      swigCPtr = 0;
    }
  }


/*public byte[] toByteArray()
{
vector_uint8_t o1=ToByteArrayC();
byte[] o2=new byte[(int)o1.size()];
for (int i=0; i<o1.size(); i++)
{
	ByteBuffer b=ByteBuffer.allocate(2);
	b.putShort(o1.get(i));

	o2[i]=b.array()[1];
}
return o2;
}

public NodeID(byte[] id)
{
	vector_uint8_t id2=new vector_uint8_t(16);
	for (int i=0; i<id.length; i++)
	{
		ByteBuffer bb=ByteBuffer.allocate(2);
		bb.put((byte)0);
		bb.put(id[i]);
		bb.rewind();
		id2.set(i, bb.getShort());
	}

	swigCMemOwn=true;
	swigCPtr=RobotRaconteurJavaJNI.new_NodeID__SWIG_1(vector_uint8_t.getCPtr(id2),id2);

}*/



  public NodeID() {
    this(RobotRaconteurJavaJNI.new_NodeID__SWIG_0(), true);
  }

  public  String toString() {
    return RobotRaconteurJavaJNI.NodeID_toString__SWIG_0(swigCPtr, this);
  }

  public  String toString(String format) {
    return RobotRaconteurJavaJNI.NodeID_toString__SWIG_1(swigCPtr, this, format);
  }

  public static NodeID newUniqueID() {
    return new NodeID(RobotRaconteurJavaJNI.NodeID_newUniqueID(), true);
  }

  public boolean equals(NodeID id2) {
    return RobotRaconteurJavaJNI.NodeID_equals(swigCPtr, this, NodeID.getCPtr(id2), id2);
  }

  public boolean ne(NodeID id2) {
    return RobotRaconteurJavaJNI.NodeID_ne(swigCPtr, this, NodeID.getCPtr(id2), id2);
  }

  public boolean lt(NodeID id2) {
    return RobotRaconteurJavaJNI.NodeID_lt(swigCPtr, this, NodeID.getCPtr(id2), id2);
  }

  public boolean isAnyNode() {
    return RobotRaconteurJavaJNI.NodeID_isAnyNode(swigCPtr, this);
  }

  public static NodeID getAny() {
    return new NodeID(RobotRaconteurJavaJNI.NodeID_getAny(), true);
  }

  public NodeID(String id) {
    this(RobotRaconteurJavaJNI.new_NodeID__SWIG_1(id), true);
  }

  public NodeID(NodeID id) {
    this(RobotRaconteurJavaJNI.new_NodeID__SWIG_2(NodeID.getCPtr(id), id), true);
  }

}
