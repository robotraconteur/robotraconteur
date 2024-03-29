// This file is automatically generated. DO NOT EDIT!

#include "RobotRaconteurServiceIndex.h"
#pragma once

namespace RobotRaconteurServiceIndex
{

class ROBOTRACONTEUR_CORE_API RobotRaconteurServiceIndexFactory : public virtual RobotRaconteur::ServiceFactory
{
  public:
    RR_OVIRTUAL std::string GetServiceName() RR_OVERRIDE;
    RR_OVIRTUAL std::string DefString() RR_OVERRIDE;
    RR_OVIRTUAL RR_SHARED_PTR<RobotRaconteur::StructureStub> FindStructureStub(boost::string_ref s) RR_OVERRIDE;
    RR_OVIRTUAL RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementNestedElementList> PackStructure(
        const RR_INTRUSIVE_PTR<RobotRaconteur::RRStructure>& structin) RR_OVERRIDE;
    RR_OVIRTUAL RR_INTRUSIVE_PTR<RobotRaconteur::RRValue> UnpackStructure(
        const RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementNestedElementList>& mstructin) RR_OVERRIDE;
    RR_OVIRTUAL RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementNestedElementList> PackPodArray(
        const RR_INTRUSIVE_PTR<RobotRaconteur::RRPodBaseArray>& structure) RR_OVERRIDE;
    RR_OVIRTUAL RR_INTRUSIVE_PTR<RobotRaconteur::RRPodBaseArray> UnpackPodArray(
        const RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementNestedElementList>& structure) RR_OVERRIDE;
    RR_OVIRTUAL RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementNestedElementList> PackPodMultiDimArray(
        const RR_INTRUSIVE_PTR<RobotRaconteur::RRPodBaseMultiDimArray>& structure) RR_OVERRIDE;
    RR_OVIRTUAL RR_INTRUSIVE_PTR<RobotRaconteur::RRPodBaseMultiDimArray> UnpackPodMultiDimArray(
        const RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementNestedElementList>& structure) RR_OVERRIDE;
    RR_OVIRTUAL RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementNestedElementList> PackNamedArray(
        const RR_INTRUSIVE_PTR<RobotRaconteur::RRNamedBaseArray>& structure) RR_OVERRIDE;
    RR_OVIRTUAL RR_INTRUSIVE_PTR<RobotRaconteur::RRNamedBaseArray> UnpackNamedArray(
        const RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementNestedElementList>& structure) RR_OVERRIDE;
    RR_OVIRTUAL RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementNestedElementList> PackNamedMultiDimArray(
        const RR_INTRUSIVE_PTR<RobotRaconteur::RRNamedBaseMultiDimArray>& structure) RR_OVERRIDE;
    RR_OVIRTUAL RR_INTRUSIVE_PTR<RobotRaconteur::RRNamedBaseMultiDimArray> UnpackNamedMultiDimArray(
        const RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementNestedElementList>& structure) RR_OVERRIDE;
    RR_OVIRTUAL RR_SHARED_PTR<RobotRaconteur::ServiceStub> CreateStub(
        boost::string_ref objecttype, boost::string_ref path,
        const RR_SHARED_PTR<RobotRaconteur::ClientContext>& context) RR_OVERRIDE;
    RR_OVIRTUAL RR_SHARED_PTR<RobotRaconteur::ServiceSkel> CreateSkel(
        boost::string_ref objecttype, boost::string_ref path, const RR_SHARED_PTR<RobotRaconteur::RRObject>& obj,
        const RR_SHARED_PTR<RobotRaconteur::ServerContext>& context) RR_OVERRIDE;
    RR_OVIRTUAL void DownCastAndThrowException(RobotRaconteur::RobotRaconteurException& exp) RR_OVERRIDE { throw exp; };
    RR_OVIRTUAL RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException> DownCastException(
        const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>& exp) RR_OVERRIDE
    {
        return exp;
    };
};

class NodeInfo_stub : public virtual RobotRaconteur::StructureStub
{
  public:
    NodeInfo_stub(const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurNode>& node) : RobotRaconteur::StructureStub(node)
    {}
    RR_OVIRTUAL RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementNestedElementList> PackStructure(
        const RR_INTRUSIVE_PTR<RobotRaconteur::RRValue>& s) RR_OVERRIDE;
    RR_OVIRTUAL RR_INTRUSIVE_PTR<RobotRaconteur::RRStructure> UnpackStructure(
        const RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementNestedElementList>& m) RR_OVERRIDE;
};

class ServiceInfo_stub : public virtual RobotRaconteur::StructureStub
{
  public:
    ServiceInfo_stub(const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurNode>& node)
        : RobotRaconteur::StructureStub(node)
    {}
    RR_OVIRTUAL RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementNestedElementList> PackStructure(
        const RR_INTRUSIVE_PTR<RobotRaconteur::RRValue>& s) RR_OVERRIDE;
    RR_OVIRTUAL RR_INTRUSIVE_PTR<RobotRaconteur::RRStructure> UnpackStructure(
        const RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementNestedElementList>& m) RR_OVERRIDE;
};

class async_ServiceIndex
{
  public:
    virtual ~async_ServiceIndex() {}
    virtual void async_GetLocalNodeServices(
        boost::function<void(const RR_INTRUSIVE_PTR<RobotRaconteur::RRMap<int32_t, ServiceInfo> >&,
                             const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>&)>
            rr_handler,
        int32_t rr_timeout = RR_TIMEOUT_INFINITE) = 0;

    virtual void async_GetRoutedNodes(
        boost::function<void(const RR_INTRUSIVE_PTR<RobotRaconteur::RRMap<int32_t, NodeInfo> >&,
                             const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>&)>
            rr_handler,
        int32_t rr_timeout = RR_TIMEOUT_INFINITE) = 0;

    virtual void async_GetDetectedNodes(
        boost::function<void(const RR_INTRUSIVE_PTR<RobotRaconteur::RRMap<int32_t, NodeInfo> >&,
                             const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>&)>
            rr_handler,
        int32_t rr_timeout = RR_TIMEOUT_INFINITE) = 0;
};
class ServiceIndex_stub : public virtual ServiceIndex,
                          public virtual async_ServiceIndex,
                          public virtual RobotRaconteur::ServiceStub
{
  public:
    ServiceIndex_stub(boost::string_ref path, const RR_SHARED_PTR<RobotRaconteur::ClientContext>& c);

    RR_OVIRTUAL void RRInitStub() RR_OVERRIDE;
    RR_OVIRTUAL RR_INTRUSIVE_PTR<RobotRaconteur::RRMap<int32_t, ServiceInfo> > GetLocalNodeServices() RR_OVERRIDE;

    RR_OVIRTUAL RR_INTRUSIVE_PTR<RobotRaconteur::RRMap<int32_t, NodeInfo> > GetRoutedNodes() RR_OVERRIDE;

    RR_OVIRTUAL RR_INTRUSIVE_PTR<RobotRaconteur::RRMap<int32_t, NodeInfo> > GetDetectedNodes() RR_OVERRIDE;

    RR_OVIRTUAL boost::signals2::signal<void()>& get_LocalNodeServicesChanged() RR_OVERRIDE;

    RR_OVIRTUAL void DispatchEvent(const RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry>& m) RR_OVERRIDE;
    RR_OVIRTUAL void DispatchPipeMessage(const RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry>& m) RR_OVERRIDE;
    RR_OVIRTUAL void DispatchWireMessage(const RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry>& m) RR_OVERRIDE;
    RR_OVIRTUAL RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> CallbackCall(
        const RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry>& m) RR_OVERRIDE;
    RR_OVIRTUAL void RRClose() RR_OVERRIDE;

  private:
    boost::signals2::signal<void()> rrvar_LocalNodeServicesChanged;

    RR_OVIRTUAL void async_GetLocalNodeServices(
        boost::function<void(const RR_INTRUSIVE_PTR<RobotRaconteur::RRMap<int32_t, ServiceInfo> >&,
                             const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>&)>
            rr_handler,
        int32_t rr_timeout = RR_TIMEOUT_INFINITE) RR_OVERRIDE;

  protected:
    virtual void rrend_GetLocalNodeServices(
        const RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry>& m,
        const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>& err,
        boost::function<void(const RR_INTRUSIVE_PTR<RobotRaconteur::RRMap<int32_t, ServiceInfo> >&,
                             const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>&)>
            handler);

  public:
    RR_OVIRTUAL void async_GetRoutedNodes(
        boost::function<void(const RR_INTRUSIVE_PTR<RobotRaconteur::RRMap<int32_t, NodeInfo> >&,
                             const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>&)>
            rr_handler,
        int32_t rr_timeout = RR_TIMEOUT_INFINITE) RR_OVERRIDE;

  protected:
    virtual void rrend_GetRoutedNodes(
        const RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry>& m,
        const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>& err,
        boost::function<void(const RR_INTRUSIVE_PTR<RobotRaconteur::RRMap<int32_t, NodeInfo> >&,
                             const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>&)>
            handler);

  public:
    RR_OVIRTUAL void async_GetDetectedNodes(
        boost::function<void(const RR_INTRUSIVE_PTR<RobotRaconteur::RRMap<int32_t, NodeInfo> >&,
                             const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>&)>
            rr_handler,
        int32_t rr_timeout = RR_TIMEOUT_INFINITE) RR_OVERRIDE;

  protected:
    virtual void rrend_GetDetectedNodes(
        const RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry>& m,
        const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>& err,
        boost::function<void(const RR_INTRUSIVE_PTR<RobotRaconteur::RRMap<int32_t, NodeInfo> >&,
                             const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>&)>
            handler);

  public:
    RR_OVIRTUAL std::string RRType() RR_OVERRIDE;
};

class ServiceIndex_skel : public virtual RobotRaconteur::ServiceSkel
{
  public:
    ServiceIndex_skel();
    RR_OVIRTUAL void Init(boost::string_ref path, const RR_SHARED_PTR<RobotRaconteur::RRObject>& object,
                          const RR_SHARED_PTR<RobotRaconteur::ServerContext>& context) RR_OVERRIDE;
    RR_OVIRTUAL RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> CallGetProperty(
        const RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry>& m) RR_OVERRIDE;

    RR_OVIRTUAL RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> CallSetProperty(
        const RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry>& m) RR_OVERRIDE;

    RR_OVIRTUAL RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> CallFunction(
        const RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry>& m) RR_OVERRIDE;

    RR_OVIRTUAL void ReleaseCastObject() RR_OVERRIDE;

    RR_OVIRTUAL void RegisterEvents(const RR_SHARED_PTR<RobotRaconteur::RRObject>& rrobj1) RR_OVERRIDE;

    RR_OVIRTUAL void UnregisterEvents(const RR_SHARED_PTR<RobotRaconteur::RRObject>& rrobj1) RR_OVERRIDE;

    RR_OVIRTUAL RR_SHARED_PTR<RobotRaconteur::RRObject> GetSubObj(boost::string_ref name,
                                                                  boost::string_ref ind) RR_OVERRIDE;

    RR_OVIRTUAL void InitPipeServers(const RR_SHARED_PTR<RobotRaconteur::RRObject>& rrobj1) RR_OVERRIDE;

    RR_OVIRTUAL void InitWireServers(const RR_SHARED_PTR<RobotRaconteur::RRObject>& rrobj1) RR_OVERRIDE;

    RR_OVIRTUAL void DispatchPipeMessage(const RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry>& m,
                                         uint32_t e) RR_OVERRIDE;

    RR_OVIRTUAL void DispatchWireMessage(const RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry>& m,
                                         uint32_t e) RR_OVERRIDE;

    RR_OVIRTUAL void InitCallbackServers(const RR_SHARED_PTR<RobotRaconteur::RRObject>& o) RR_OVERRIDE;

    RR_OVIRTUAL RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> CallPipeFunction(
        const RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry>& m, uint32_t e) RR_OVERRIDE;

    RR_OVIRTUAL RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> CallWireFunction(
        const RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry>& m, uint32_t e) RR_OVERRIDE;

    RR_OVIRTUAL RR_SHARED_PTR<void> GetCallbackFunction(uint32_t endpoint, boost::string_ref membername) RR_OVERRIDE;

    RR_OVIRTUAL RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> CallMemoryFunction(
        const RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry>& m,
        const RR_SHARED_PTR<RobotRaconteur::Endpoint>& e) RR_OVERRIDE;

    RR_OVIRTUAL std::string GetObjectType() RR_OVERRIDE;
    virtual RR_SHARED_PTR<RobotRaconteurServiceIndex::ServiceIndex> get_obj();

    void rr_LocalNodeServicesChanged_Handler();

  protected:
    boost::signals2::connection LocalNodeServicesChanged_rrconnection;
    bool rr_InitPipeServersRun;
    bool rr_InitWireServersRun;
};

} // namespace RobotRaconteurServiceIndex
