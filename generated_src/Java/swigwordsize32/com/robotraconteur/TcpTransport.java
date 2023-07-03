/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (https://www.swig.org).
 * Version 4.1.1
 *
 * Do not make changes to this file unless you know what you are doing - modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package com.robotraconteur;

public class TcpTransport extends Transport {
  private transient long swigCPtr;
  private transient boolean swigCMemOwnDerived;

  protected TcpTransport(long cPtr, boolean cMemoryOwn) {
    super(RobotRaconteurJavaJNI.TcpTransport_SWIGSmartPtrUpcast(cPtr), true);
    swigCMemOwnDerived = cMemoryOwn;
    swigCPtr = cPtr;
  }

  protected static long getCPtr(TcpTransport obj) {
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
        RobotRaconteurJavaJNI.delete_TcpTransport(swigCPtr);
      }
      swigCPtr = 0;
    }
    super.delete();
  }

public final void enableNodeDiscoveryListening(IPNodeDiscoveryFlags flags)
{
	enableNodeDiscoveryListening(flags.ordinal());
}

public final void enableNodeAnnounce(IPNodeDiscoveryFlags flags)
{
	enableNodeAnnounce(flags.ordinal());
}

public final boolean isTransportConnectionSecure(Object obj)
{
	if (!(obj instanceof ServiceStub))
	{
		throw new UnsupportedOperationException("Object is not a Robot Raconteur reference");
	}
	ServiceStub s = (ServiceStub)obj;

	return _IsTransportConnectionSecure(s.rr_innerstub);
}

public final boolean isSecurePeerIdentityVerified(Object obj)
{
	if (!(obj instanceof ServiceStub))
	{
		throw new UnsupportedOperationException("Object is not a Robot Raconteur reference");
	}
	ServiceStub s = (ServiceStub)obj;

	return _IsSecurePeerIdentityVerified(s.rr_innerstub);
}

public final String getSecurePeerIdentity(Object obj)
{
	if (!(obj instanceof ServiceStub))
	{
		throw new UnsupportedOperationException("Object is not a Robot Raconteur reference");
	}
	ServiceStub s = (ServiceStub)obj;

	return _GetSecurePeerIdentity(s.rr_innerstub);
}


  public TcpTransport() {
    this(RobotRaconteurJavaJNI.new_TcpTransport__SWIG_0(), true);
  }

  public TcpTransport(RobotRaconteurNode node) {
    this(RobotRaconteurJavaJNI.new_TcpTransport__SWIG_1(RobotRaconteurNode.getCPtr(node), node), true);
  }

  public int getDefaultReceiveTimeout() {
    return RobotRaconteurJavaJNI.TcpTransport_getDefaultReceiveTimeout(swigCPtr, this);
  }

  public void setDefaultReceiveTimeout(int milliseconds) {
    RobotRaconteurJavaJNI.TcpTransport_setDefaultReceiveTimeout(swigCPtr, this, milliseconds);
  }

  public int getDefaultConnectTimeout() {
    return RobotRaconteurJavaJNI.TcpTransport_getDefaultConnectTimeout(swigCPtr, this);
  }

  public void setDefaultConnectTimeout(int milliseconds) {
    RobotRaconteurJavaJNI.TcpTransport_setDefaultConnectTimeout(swigCPtr, this, milliseconds);
  }

  public int getDefaultHeartbeatPeriod() {
    return RobotRaconteurJavaJNI.TcpTransport_getDefaultHeartbeatPeriod(swigCPtr, this);
  }

  public void setDefaultHeartbeatPeriod(int milliseconds) {
    RobotRaconteurJavaJNI.TcpTransport_setDefaultHeartbeatPeriod(swigCPtr, this, milliseconds);
  }

  public String getUrlSchemeString() {
    return RobotRaconteurJavaJNI.TcpTransport_getUrlSchemeString(swigCPtr, this);
  }

  public vectorstring getServerListenUrls() {
    return new vectorstring(RobotRaconteurJavaJNI.TcpTransport_getServerListenUrls(swigCPtr, this), true);
  }

  public int getListenPort() {
    return RobotRaconteurJavaJNI.TcpTransport_getListenPort(swigCPtr, this);
  }

  public void startServer(int porte, boolean localhost_only) {
    RobotRaconteurJavaJNI.TcpTransport_startServer__SWIG_0(swigCPtr, this, porte, localhost_only);
  }

  public void startServer(int porte) {
    RobotRaconteurJavaJNI.TcpTransport_startServer__SWIG_1(swigCPtr, this, porte);
  }

  public void close() {
    RobotRaconteurJavaJNI.TcpTransport_close(swigCPtr, this);
  }

  public void enableNodeDiscoveryListening(long flags) {
    RobotRaconteurJavaJNI.TcpTransport_enableNodeDiscoveryListening__SWIG_0(swigCPtr, this, flags);
  }

  public void enableNodeDiscoveryListening() {
    RobotRaconteurJavaJNI.TcpTransport_enableNodeDiscoveryListening__SWIG_1(swigCPtr, this);
  }

  public void disableNodeDiscoveryListening() {
    RobotRaconteurJavaJNI.TcpTransport_disableNodeDiscoveryListening(swigCPtr, this);
  }

  public void enableNodeAnnounce(long flags) {
    RobotRaconteurJavaJNI.TcpTransport_enableNodeAnnounce__SWIG_0(swigCPtr, this, flags);
  }

  public void enableNodeAnnounce() {
    RobotRaconteurJavaJNI.TcpTransport_enableNodeAnnounce__SWIG_1(swigCPtr, this);
  }

  public void disableNodeAnnounce() {
    RobotRaconteurJavaJNI.TcpTransport_disableNodeAnnounce(swigCPtr, this);
  }

  public int getNodeAnnouncePeriod() {
    return RobotRaconteurJavaJNI.TcpTransport_getNodeAnnouncePeriod(swigCPtr, this);
  }

  public void setNodeAnnouncePeriod(int millis) {
    RobotRaconteurJavaJNI.TcpTransport_setNodeAnnouncePeriod(swigCPtr, this, millis);
  }

  public int getMaxMessageSize() {
    return RobotRaconteurJavaJNI.TcpTransport_getMaxMessageSize(swigCPtr, this);
  }

  public void setMaxMessageSize(int size) {
    RobotRaconteurJavaJNI.TcpTransport_setMaxMessageSize(swigCPtr, this, size);
  }

  public int getMaxConnectionCount() {
    return RobotRaconteurJavaJNI.TcpTransport_getMaxConnectionCount(swigCPtr, this);
  }

  public void setMaxConnectionCount(int count) {
    RobotRaconteurJavaJNI.TcpTransport_setMaxConnectionCount(swigCPtr, this, count);
  }

  public boolean getRequireTls() {
    return RobotRaconteurJavaJNI.TcpTransport_getRequireTls(swigCPtr, this);
  }

  public void setRequireTls(boolean require_tls) {
    RobotRaconteurJavaJNI.TcpTransport_setRequireTls(swigCPtr, this, require_tls);
  }

  public void loadTlsNodeCertificate() {
    RobotRaconteurJavaJNI.TcpTransport_loadTlsNodeCertificate(swigCPtr, this);
  }

  private boolean _IsTlsNodeCertificateLoaded() {
    return RobotRaconteurJavaJNI.TcpTransport__IsTlsNodeCertificateLoaded(swigCPtr, this);
  }

  public boolean isTransportConnectionSecure(long endpoint) {
    return RobotRaconteurJavaJNI.TcpTransport_isTransportConnectionSecure(swigCPtr, this, endpoint);
  }

  private boolean _IsTransportConnectionSecure(RRObject obj) {
    return RobotRaconteurJavaJNI.TcpTransport__IsTransportConnectionSecure(swigCPtr, this, RRObject.getCPtr(obj), obj);
  }

  public boolean isSecurePeerIdentityVerified(long endpoint) {
    return RobotRaconteurJavaJNI.TcpTransport_isSecurePeerIdentityVerified(swigCPtr, this, endpoint);
  }

  private boolean _IsSecurePeerIdentityVerified(RRObject obj) {
    return RobotRaconteurJavaJNI.TcpTransport__IsSecurePeerIdentityVerified(swigCPtr, this, RRObject.getCPtr(obj), obj);
  }

  public String getSecurePeerIdentity(long endpoint) {
    return RobotRaconteurJavaJNI.TcpTransport_getSecurePeerIdentity(swigCPtr, this, endpoint);
  }

  private String _GetSecurePeerIdentity(RRObject obj) {
    return RobotRaconteurJavaJNI.TcpTransport__GetSecurePeerIdentity(swigCPtr, this, RRObject.getCPtr(obj), obj);
  }

  public void startServerUsingPortSharer() {
    RobotRaconteurJavaJNI.TcpTransport_startServerUsingPortSharer(swigCPtr, this);
  }

  private boolean _IsPortSharerRunning() {
    return RobotRaconteurJavaJNI.TcpTransport__IsPortSharerRunning(swigCPtr, this);
  }

  public boolean getAcceptWebSockets() {
    return RobotRaconteurJavaJNI.TcpTransport_getAcceptWebSockets(swigCPtr, this);
  }

  public void setAcceptWebSockets(boolean value) {
    RobotRaconteurJavaJNI.TcpTransport_setAcceptWebSockets(swigCPtr, this, value);
  }

  public vectorstring getWebSocketAllowedOrigins() {
    return new vectorstring(RobotRaconteurJavaJNI.TcpTransport_getWebSocketAllowedOrigins(swigCPtr, this), true);
  }

  public void addWebSocketAllowedOrigin(String origin) {
    RobotRaconteurJavaJNI.TcpTransport_addWebSocketAllowedOrigin(swigCPtr, this, origin);
  }

  public void removeWebSocketAllowedOrigin(String origin) {
    RobotRaconteurJavaJNI.TcpTransport_removeWebSocketAllowedOrigin(swigCPtr, this, origin);
  }

  public boolean getDisableMessage4() {
    return RobotRaconteurJavaJNI.TcpTransport_getDisableMessage4(swigCPtr, this);
  }

  public void setDisableMessage4(boolean d) {
    RobotRaconteurJavaJNI.TcpTransport_setDisableMessage4(swigCPtr, this, d);
  }

  public boolean getDisableStringTable() {
    return RobotRaconteurJavaJNI.TcpTransport_getDisableStringTable(swigCPtr, this);
  }

  public void setDisableStringTable(boolean d) {
    RobotRaconteurJavaJNI.TcpTransport_setDisableStringTable(swigCPtr, this, d);
  }

  public boolean getDisableAsyncMessageIO() {
    return RobotRaconteurJavaJNI.TcpTransport_getDisableAsyncMessageIO(swigCPtr, this);
  }

  public void setDisableAsyncMessageIO(boolean d) {
    RobotRaconteurJavaJNI.TcpTransport_setDisableAsyncMessageIO(swigCPtr, this, d);
  }

  public static vectorstring getLocalAdapterIPAddresses() {
    return new vectorstring(RobotRaconteurJavaJNI.TcpTransport_getLocalAdapterIPAddresses(), true);
  }

}
