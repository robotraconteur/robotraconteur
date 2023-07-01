/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 4.0.2
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package com.robotraconteur;

public class RobotRaconteurJava implements RobotRaconteurJavaConstants {
  public static short getMessageFlags_ROUTING_INFO() {
    return RobotRaconteurJavaJNI.MessageFlags_ROUTING_INFO_get();
  }

  public static short getMessageFlags_ENDPOINT_INFO() {
    return RobotRaconteurJavaJNI.MessageFlags_ENDPOINT_INFO_get();
  }

  public static short getMessageFlags_PRIORITY() {
    return RobotRaconteurJavaJNI.MessageFlags_PRIORITY_get();
  }

  public static short getMessageFlags_UNRELIABLE() {
    return RobotRaconteurJavaJNI.MessageFlags_UNRELIABLE_get();
  }

  public static short getMessageFlags_META_INFO() {
    return RobotRaconteurJavaJNI.MessageFlags_META_INFO_get();
  }

  public static short getMessageFlags_STRING_TABLE() {
    return RobotRaconteurJavaJNI.MessageFlags_STRING_TABLE_get();
  }

  public static short getMessageFlags_MULTIPLE_ENTRIES() {
    return RobotRaconteurJavaJNI.MessageFlags_MULTIPLE_ENTRIES_get();
  }

  public static short getMessageFlags_EXTENDED() {
    return RobotRaconteurJavaJNI.MessageFlags_EXTENDED_get();
  }

  public static short getMessageFlags_Version2Compat() {
    return RobotRaconteurJavaJNI.MessageFlags_Version2Compat_get();
  }

  public static short getMessageEntryFlags_SERVICE_PATH_STR() {
    return RobotRaconteurJavaJNI.MessageEntryFlags_SERVICE_PATH_STR_get();
  }

  public static short getMessageEntryFlags_SERVICE_PATH_CODE() {
    return RobotRaconteurJavaJNI.MessageEntryFlags_SERVICE_PATH_CODE_get();
  }

  public static short getMessageEntryFlags_MEMBER_NAME_STR() {
    return RobotRaconteurJavaJNI.MessageEntryFlags_MEMBER_NAME_STR_get();
  }

  public static short getMessageEntryFlags_MEMBER_NAME_CODE() {
    return RobotRaconteurJavaJNI.MessageEntryFlags_MEMBER_NAME_CODE_get();
  }

  public static short getMessageEntryFlags_REQUEST_ID() {
    return RobotRaconteurJavaJNI.MessageEntryFlags_REQUEST_ID_get();
  }

  public static short getMessageEntryFlags_ERROR() {
    return RobotRaconteurJavaJNI.MessageEntryFlags_ERROR_get();
  }

  public static short getMessageEntryFlags_META_INFO() {
    return RobotRaconteurJavaJNI.MessageEntryFlags_META_INFO_get();
  }

  public static short getMessageEntryFlags_EXTENDED() {
    return RobotRaconteurJavaJNI.MessageEntryFlags_EXTENDED_get();
  }

  public static short getMessageEntryFlags_Version2Compat() {
    return RobotRaconteurJavaJNI.MessageEntryFlags_Version2Compat_get();
  }

  public static short getMessageElementFlags_ELEMENT_NAME_STR() {
    return RobotRaconteurJavaJNI.MessageElementFlags_ELEMENT_NAME_STR_get();
  }

  public static short getMessageElementFlags_ELEMENT_NAME_CODE() {
    return RobotRaconteurJavaJNI.MessageElementFlags_ELEMENT_NAME_CODE_get();
  }

  public static short getMessageElementFlags_ELEMENT_NUMBER() {
    return RobotRaconteurJavaJNI.MessageElementFlags_ELEMENT_NUMBER_get();
  }

  public static short getMessageElementFlags_ELEMENT_TYPE_NAME_STR() {
    return RobotRaconteurJavaJNI.MessageElementFlags_ELEMENT_TYPE_NAME_STR_get();
  }

  public static short getMessageElementFlags_ELEMENT_TYPE_NAME_CODE() {
    return RobotRaconteurJavaJNI.MessageElementFlags_ELEMENT_TYPE_NAME_CODE_get();
  }

  public static short getMessageElementFlags_META_INFO() {
    return RobotRaconteurJavaJNI.MessageElementFlags_META_INFO_get();
  }

  public static short getMessageElementFlags_EXTENDED() {
    return RobotRaconteurJavaJNI.MessageElementFlags_EXTENDED_get();
  }

  public static short getMessageElementFlags_Version2Compat() {
    return RobotRaconteurJavaJNI.MessageElementFlags_Version2Compat_get();
  }

  public static long getTranspartCapabilityCode_PAGE_MASK() {
    return RobotRaconteurJavaJNI.TranspartCapabilityCode_PAGE_MASK_get();
  }

  public static long getTransportCapabilityCode_MESSAGE2_BASIC_PAGE() {
    return RobotRaconteurJavaJNI.TransportCapabilityCode_MESSAGE2_BASIC_PAGE_get();
  }

  public static long getTransportCapabilityCode_MESSAGE2_BASIC_ENABLE() {
    return RobotRaconteurJavaJNI.TransportCapabilityCode_MESSAGE2_BASIC_ENABLE_get();
  }

  public static long getTransportCapabilityCode_MESSAGE2_BASIC_CONNECTCOMBINED() {
    return RobotRaconteurJavaJNI.TransportCapabilityCode_MESSAGE2_BASIC_CONNECTCOMBINED_get();
  }

  public static long getTransportCapabilityCode_MESSAGE4_BASIC_PAGE() {
    return RobotRaconteurJavaJNI.TransportCapabilityCode_MESSAGE4_BASIC_PAGE_get();
  }

  public static long getTransportCapabilityCode_MESSAGE4_BASIC_ENABLE() {
    return RobotRaconteurJavaJNI.TransportCapabilityCode_MESSAGE4_BASIC_ENABLE_get();
  }

  public static long getTransportCapabilityCode_MESSAGE4_BASIC_CONNECTCOMBINED() {
    return RobotRaconteurJavaJNI.TransportCapabilityCode_MESSAGE4_BASIC_CONNECTCOMBINED_get();
  }

  public static long getTransportCapabilityCode_MESSAGE4_STRINGTABLE_PAGE() {
    return RobotRaconteurJavaJNI.TransportCapabilityCode_MESSAGE4_STRINGTABLE_PAGE_get();
  }

  public static long getTransportCapabilityCode_MESSAGE4_STRINGTABLE_ENABLE() {
    return RobotRaconteurJavaJNI.TransportCapabilityCode_MESSAGE4_STRINGTABLE_ENABLE_get();
  }

  public static long getTransportCapabilityCode_MESSAGE4_STRINGTABLE_MESSAGE_LOCAL() {
    return RobotRaconteurJavaJNI.TransportCapabilityCode_MESSAGE4_STRINGTABLE_MESSAGE_LOCAL_get();
  }

  public static long getTransportCapabilityCode_MESSAGE4_STRINGTABLE_STANDARD_TABLE() {
    return RobotRaconteurJavaJNI.TransportCapabilityCode_MESSAGE4_STRINGTABLE_STANDARD_TABLE_get();
  }

  public static String getRRDataTypeString(DataTypes type) {
    return RobotRaconteurJavaJNI.getRRDataTypeString(type.swigValue());
  }

  public static boolean isTypeRRArray(DataTypes type) {
    return RobotRaconteurJavaJNI.isTypeRRArray(type.swigValue());
  }

  public static boolean isTypeNumeric(DataTypes type) {
    return RobotRaconteurJavaJNI.isTypeNumeric(type.swigValue());
  }

  public static RRBaseArray allocateRRArrayByType(DataTypes type, long length) {
    long cPtr = RobotRaconteurJavaJNI.allocateRRArrayByType(type.swigValue(), length);
    return (cPtr == 0) ? null : new RRBaseArray(cPtr, true);
  }

  public static MultiDimArray_CalculateCopyIndicesIter multiDimArray_CalculateCopyIndicesBeginIter(vectoruint32 mema_dims, vectoruint32 mema_pos, vectoruint32 memb_dims, vectoruint32 memb_pos, vectoruint32 count) {
    long cPtr = RobotRaconteurJavaJNI.multiDimArray_CalculateCopyIndicesBeginIter(vectoruint32.getCPtr(mema_dims), mema_dims, vectoruint32.getCPtr(mema_pos), mema_pos, vectoruint32.getCPtr(memb_dims), memb_dims, vectoruint32.getCPtr(memb_pos), memb_pos, vectoruint32.getCPtr(count), count);
    return (cPtr == 0) ? null : new MultiDimArray_CalculateCopyIndicesIter(cPtr, true);
  }

  public static boolean RobotRaconteurVersion_eq(RobotRaconteurVersion v1, RobotRaconteurVersion v2) {
    return RobotRaconteurJavaJNI.RobotRaconteurVersion_eq(RobotRaconteurVersion.getCPtr(v1), v1, RobotRaconteurVersion.getCPtr(v2), v2);
  }

  public static boolean RobotRaconteurVersion_ne(RobotRaconteurVersion v1, RobotRaconteurVersion v2) {
    return RobotRaconteurJavaJNI.RobotRaconteurVersion_ne(RobotRaconteurVersion.getCPtr(v1), v1, RobotRaconteurVersion.getCPtr(v2), v2);
  }

  public static boolean RobotRaconteurVersion_gt(RobotRaconteurVersion v1, RobotRaconteurVersion v2) {
    return RobotRaconteurJavaJNI.RobotRaconteurVersion_gt(RobotRaconteurVersion.getCPtr(v1), v1, RobotRaconteurVersion.getCPtr(v2), v2);
  }

  public static boolean RobotRaconteurVersion_ge(RobotRaconteurVersion v1, RobotRaconteurVersion v2) {
    return RobotRaconteurJavaJNI.RobotRaconteurVersion_ge(RobotRaconteurVersion.getCPtr(v1), v1, RobotRaconteurVersion.getCPtr(v2), v2);
  }

  public static boolean RobotRaconteurVersion_lt(RobotRaconteurVersion v1, RobotRaconteurVersion v2) {
    return RobotRaconteurJavaJNI.RobotRaconteurVersion_lt(RobotRaconteurVersion.getCPtr(v1), v1, RobotRaconteurVersion.getCPtr(v2), v2);
  }

  public static boolean RobotRaconteurVersion_le(RobotRaconteurVersion v1, RobotRaconteurVersion v2) {
    return RobotRaconteurJavaJNI.RobotRaconteurVersion_le(RobotRaconteurVersion.getCPtr(v1), v1, RobotRaconteurVersion.getCPtr(v2), v2);
  }

  public static vectorserviceinfo2wrapped wrappedFindServiceByType(RobotRaconteurNode node, String servicetype, vectorstring transportschemes) {
    return new vectorserviceinfo2wrapped(RobotRaconteurJavaJNI.wrappedFindServiceByType(RobotRaconteurNode.getCPtr(node), node, servicetype, vectorstring.getCPtr(transportschemes), transportschemes), true);
  }

  public static void asyncWrappedFindServiceByType(RobotRaconteurNode node, String servicetype, vectorstring transportschemes, int timeout, AsyncServiceInfo2VectorReturnDirector handler, int id) {
    RobotRaconteurJavaJNI.asyncWrappedFindServiceByType(RobotRaconteurNode.getCPtr(node), node, servicetype, vectorstring.getCPtr(transportschemes), transportschemes, timeout, AsyncServiceInfo2VectorReturnDirector.getCPtr(handler), handler, id);
  }

  public static vectornodeinfo2 wrappedFindNodeByID(RobotRaconteurNode node, NodeID id, vectorstring transportschemes) {
    return new vectornodeinfo2(RobotRaconteurJavaJNI.wrappedFindNodeByID(RobotRaconteurNode.getCPtr(node), node, NodeID.getCPtr(id), id, vectorstring.getCPtr(transportschemes), transportschemes), true);
  }

  public static vectornodeinfo2 wrappedFindNodeByName(RobotRaconteurNode node, String name, vectorstring transportschemes) {
    return new vectornodeinfo2(RobotRaconteurJavaJNI.wrappedFindNodeByName(RobotRaconteurNode.getCPtr(node), node, name, vectorstring.getCPtr(transportschemes), transportschemes), true);
  }

  public static void asyncWrappedFindNodeByID(RobotRaconteurNode node, NodeID id, vectorstring transportschemes, int timeout, AsyncNodeInfo2VectorReturnDirector handler, int id1) {
    RobotRaconteurJavaJNI.asyncWrappedFindNodeByID(RobotRaconteurNode.getCPtr(node), node, NodeID.getCPtr(id), id, vectorstring.getCPtr(transportschemes), transportschemes, timeout, AsyncNodeInfo2VectorReturnDirector.getCPtr(handler), handler, id1);
  }

  public static void asyncWrappedFindNodeByName(RobotRaconteurNode node, String name, vectorstring transportschemes, int timeout, AsyncNodeInfo2VectorReturnDirector handler, int id) {
    RobotRaconteurJavaJNI.asyncWrappedFindNodeByName(RobotRaconteurNode.getCPtr(node), node, name, vectorstring.getCPtr(transportschemes), transportschemes, timeout, AsyncNodeInfo2VectorReturnDirector.getCPtr(handler), handler, id);
  }

  public static void wrappedUpdateDetectedNodes(RobotRaconteurNode node, vectorstring schemes) {
    RobotRaconteurJavaJNI.wrappedUpdateDetectedNodes(RobotRaconteurNode.getCPtr(node), node, vectorstring.getCPtr(schemes), schemes);
  }

  public static void asyncWrappedUpdateDetectedNodes(RobotRaconteurNode node, vectorstring schemes, int timeout, AsyncVoidNoErrReturnDirector handler, int id1) {
    RobotRaconteurJavaJNI.asyncWrappedUpdateDetectedNodes(RobotRaconteurNode.getCPtr(node), node, vectorstring.getCPtr(schemes), schemes, timeout, AsyncVoidNoErrReturnDirector.getCPtr(handler), handler, id1);
  }

  public static vectorstring wrappedGetDetectedNodes(RobotRaconteurNode node) {
    return new vectorstring(RobotRaconteurJavaJNI.wrappedGetDetectedNodes(RobotRaconteurNode.getCPtr(node), node), true);
  }

  public static WrappedNodeInfo2 wrappedGetDetectedNodeCacheInfo(RobotRaconteurNode node, NodeID nodeid) {
    return new WrappedNodeInfo2(RobotRaconteurJavaJNI.wrappedGetDetectedNodeCacheInfo(RobotRaconteurNode.getCPtr(node), node, NodeID.getCPtr(nodeid), nodeid), true);
  }

  public static boolean wrappedTryGetDetectedNodeCacheInfo(RobotRaconteurNode node, NodeID nodeid, WrappedNodeInfo2 nodeinfo2) {
    return RobotRaconteurJavaJNI.wrappedTryGetDetectedNodeCacheInfo(RobotRaconteurNode.getCPtr(node), node, NodeID.getCPtr(nodeid), nodeid, WrappedNodeInfo2.getCPtr(nodeinfo2), nodeinfo2);
  }

  public static vectorptr_subscriptionclientid wrappedServiceInfo2SubscriptionServicesToVector(map_subscriptionserviceinfo2 infos) {
    return new vectorptr_subscriptionclientid(RobotRaconteurJavaJNI.wrappedServiceInfo2SubscriptionServicesToVector(map_subscriptionserviceinfo2.getCPtr(infos), infos), true);
  }

  public static WrappedServiceInfo2Subscription wrappedSubscribeServiceInfo2(RobotRaconteurNode node, vectorstring service_types, WrappedServiceSubscriptionFilter filter) {
    long cPtr = RobotRaconteurJavaJNI.wrappedSubscribeServiceInfo2__SWIG_0(RobotRaconteurNode.getCPtr(node), node, vectorstring.getCPtr(service_types), service_types, WrappedServiceSubscriptionFilter.getCPtr(filter), filter);
    return (cPtr == 0) ? null : new WrappedServiceInfo2Subscription(cPtr, true);
  }

  public static WrappedServiceInfo2Subscription wrappedSubscribeServiceInfo2(RobotRaconteurNode node, vectorstring service_types) {
    long cPtr = RobotRaconteurJavaJNI.wrappedSubscribeServiceInfo2__SWIG_1(RobotRaconteurNode.getCPtr(node), node, vectorstring.getCPtr(service_types), service_types);
    return (cPtr == 0) ? null : new WrappedServiceInfo2Subscription(cPtr, true);
  }

  public static vectorptr_subscriptionclientid wrappedServiceSubscriptionClientsToVector(map_subscriptionclients clients) {
    return new vectorptr_subscriptionclientid(RobotRaconteurJavaJNI.wrappedServiceSubscriptionClientsToVector(map_subscriptionclients.getCPtr(clients), clients), true);
  }

  public static WrappedServiceSubscription wrappedSubscribeServiceByType(RobotRaconteurNode node, vectorstring service_types, WrappedServiceSubscriptionFilter filter) {
    long cPtr = RobotRaconteurJavaJNI.wrappedSubscribeServiceByType__SWIG_0(RobotRaconteurNode.getCPtr(node), node, vectorstring.getCPtr(service_types), service_types, WrappedServiceSubscriptionFilter.getCPtr(filter), filter);
    return (cPtr == 0) ? null : new WrappedServiceSubscription(cPtr, true);
  }

  public static WrappedServiceSubscription wrappedSubscribeServiceByType(RobotRaconteurNode node, vectorstring service_types) {
    long cPtr = RobotRaconteurJavaJNI.wrappedSubscribeServiceByType__SWIG_1(RobotRaconteurNode.getCPtr(node), node, vectorstring.getCPtr(service_types), service_types);
    return (cPtr == 0) ? null : new WrappedServiceSubscription(cPtr, true);
  }

  public static WrappedServiceSubscription wrappedSubscribeService(RobotRaconteurNode node, vectorstring url, String username, MessageElementData credentials, String objecttype) {
    long cPtr = RobotRaconteurJavaJNI.wrappedSubscribeService__SWIG_0(RobotRaconteurNode.getCPtr(node), node, vectorstring.getCPtr(url), url, username, MessageElementData.getCPtr(credentials), credentials, objecttype);
    return (cPtr == 0) ? null : new WrappedServiceSubscription(cPtr, true);
  }

  public static WrappedServiceSubscription wrappedSubscribeService(RobotRaconteurNode node, vectorstring url, String username, MessageElementData credentials) {
    long cPtr = RobotRaconteurJavaJNI.wrappedSubscribeService__SWIG_1(RobotRaconteurNode.getCPtr(node), node, vectorstring.getCPtr(url), url, username, MessageElementData.getCPtr(credentials), credentials);
    return (cPtr == 0) ? null : new WrappedServiceSubscription(cPtr, true);
  }

  public static WrappedServiceSubscription wrappedSubscribeService(RobotRaconteurNode node, vectorstring url, String username) {
    long cPtr = RobotRaconteurJavaJNI.wrappedSubscribeService__SWIG_2(RobotRaconteurNode.getCPtr(node), node, vectorstring.getCPtr(url), url, username);
    return (cPtr == 0) ? null : new WrappedServiceSubscription(cPtr, true);
  }

  public static WrappedServiceSubscription wrappedSubscribeService(RobotRaconteurNode node, vectorstring url) {
    long cPtr = RobotRaconteurJavaJNI.wrappedSubscribeService__SWIG_3(RobotRaconteurNode.getCPtr(node), node, vectorstring.getCPtr(url), url);
    return (cPtr == 0) ? null : new WrappedServiceSubscription(cPtr, true);
  }

  public static WrappedServiceSubscription wrappedSubscribeService(RobotRaconteurNode node, String url, String username, MessageElementData credentials, String objecttype) {
    long cPtr = RobotRaconteurJavaJNI.wrappedSubscribeService__SWIG_4(RobotRaconteurNode.getCPtr(node), node, url, username, MessageElementData.getCPtr(credentials), credentials, objecttype);
    return (cPtr == 0) ? null : new WrappedServiceSubscription(cPtr, true);
  }

  public static WrappedServiceSubscription wrappedSubscribeService(RobotRaconteurNode node, String url, String username, MessageElementData credentials) {
    long cPtr = RobotRaconteurJavaJNI.wrappedSubscribeService__SWIG_5(RobotRaconteurNode.getCPtr(node), node, url, username, MessageElementData.getCPtr(credentials), credentials);
    return (cPtr == 0) ? null : new WrappedServiceSubscription(cPtr, true);
  }

  public static WrappedServiceSubscription wrappedSubscribeService(RobotRaconteurNode node, String url, String username) {
    long cPtr = RobotRaconteurJavaJNI.wrappedSubscribeService__SWIG_6(RobotRaconteurNode.getCPtr(node), node, url, username);
    return (cPtr == 0) ? null : new WrappedServiceSubscription(cPtr, true);
  }

  public static WrappedServiceSubscription wrappedSubscribeService(RobotRaconteurNode node, String url) {
    long cPtr = RobotRaconteurJavaJNI.wrappedSubscribeService__SWIG_7(RobotRaconteurNode.getCPtr(node), node, url);
    return (cPtr == 0) ? null : new WrappedServiceSubscription(cPtr, true);
  }

  public static GetUuidForNameAndLockResult getUuidForNameAndLock(NodeDirectories node_dirs, String name, vectorstring scope) {
    return new GetUuidForNameAndLockResult(RobotRaconteurJavaJNI.getUuidForNameAndLock(NodeDirectories.getCPtr(node_dirs), node_dirs, name, vectorstring.getCPtr(scope), scope), true);
  }

}
