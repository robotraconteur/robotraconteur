/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 4.0.2
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package com.robotraconteur;

public class NodeDirectoriesUtil {
  private transient long swigCPtr;
  protected transient boolean swigCMemOwn;

  protected NodeDirectoriesUtil(long cPtr, boolean cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = cPtr;
  }

  protected static long getCPtr(NodeDirectoriesUtil obj) {
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
        RobotRaconteurJavaJNI.delete_NodeDirectoriesUtil(swigCPtr);
      }
      swigCPtr = 0;
    }
  }

  public static NodeDirectories getDefaultNodeDirectories(RobotRaconteurNode node) {
    return new NodeDirectories(RobotRaconteurJavaJNI.NodeDirectoriesUtil_getDefaultNodeDirectories__SWIG_0(RobotRaconteurNode.getCPtr(node), node), true);
  }

  public static NodeDirectories getDefaultNodeDirectories() {
    return new NodeDirectories(RobotRaconteurJavaJNI.NodeDirectoriesUtil_getDefaultNodeDirectories__SWIG_1(), true);
  }

  public static String getLogonUserName() {
    return RobotRaconteurJavaJNI.NodeDirectoriesUtil_getLogonUserName();
  }

  public static boolean isLogonUserRoot() {
    return RobotRaconteurJavaJNI.NodeDirectoriesUtil_isLogonUserRoot();
  }

  public static boolean readInfoFile(FilesystemPath fname, map_strstr data) {
    return RobotRaconteurJavaJNI.NodeDirectoriesUtil_readInfoFile(FilesystemPath.getCPtr(fname), fname, map_strstr.getCPtr(data), data);
  }

  public static NodeDirectoriesFD createPidFile(FilesystemPath path) {
    long cPtr = RobotRaconteurJavaJNI.NodeDirectoriesUtil_createPidFile(FilesystemPath.getCPtr(path), path);
    return (cPtr == 0) ? null : new NodeDirectoriesFD(cPtr, true);
  }

  public static NodeDirectoriesFD createInfoFile(FilesystemPath path, map_strstr info) {
    long cPtr = RobotRaconteurJavaJNI.NodeDirectoriesUtil_createInfoFile(FilesystemPath.getCPtr(path), path, map_strstr.getCPtr(info), info);
    return (cPtr == 0) ? null : new NodeDirectoriesFD(cPtr, true);
  }

  public static void refreshInfoFile(NodeDirectoriesFD h_info, map_strstr updated_info) {
    RobotRaconteurJavaJNI.NodeDirectoriesUtil_refreshInfoFile(NodeDirectoriesFD.getCPtr(h_info), h_info, map_strstr.getCPtr(updated_info), updated_info);
  }

  public NodeDirectoriesUtil() {
    this(RobotRaconteurJavaJNI.new_NodeDirectoriesUtil(), true);
  }

}
