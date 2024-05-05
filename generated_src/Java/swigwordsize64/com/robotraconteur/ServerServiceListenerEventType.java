/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 4.0.2
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package com.robotraconteur;

public enum ServerServiceListenerEventType {
  ServerServiceListenerEventType_ServiceClosed(1),
  ServerServiceListenerEventType_ClientConnected,
  ServerServiceListenerEventType_ClientDisconnected;

  public final int swigValue() {
    return swigValue;
  }

  public static ServerServiceListenerEventType swigToEnum(int swigValue) {
    ServerServiceListenerEventType[] swigValues = ServerServiceListenerEventType.class.getEnumConstants();
    if (swigValue < swigValues.length && swigValue >= 0 && swigValues[swigValue].swigValue == swigValue)
      return swigValues[swigValue];
    for (ServerServiceListenerEventType swigEnum : swigValues)
      if (swigEnum.swigValue == swigValue)
        return swigEnum;
    throw new IllegalArgumentException("No enum " + ServerServiceListenerEventType.class + " with value " + swigValue);
  }

  @SuppressWarnings("unused")
  private ServerServiceListenerEventType() {
    this.swigValue = SwigNext.next++;
  }

  @SuppressWarnings("unused")
  private ServerServiceListenerEventType(int swigValue) {
    this.swigValue = swigValue;
    SwigNext.next = swigValue+1;
  }

  @SuppressWarnings("unused")
  private ServerServiceListenerEventType(ServerServiceListenerEventType swigEnum) {
    this.swigValue = swigEnum.swigValue;
    SwigNext.next = this.swigValue+1;
  }

  private final int swigValue;

  private static class SwigNext {
    private static int next = 0;
  }
}

