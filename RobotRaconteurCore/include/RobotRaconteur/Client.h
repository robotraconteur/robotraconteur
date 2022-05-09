/**
 * @file Client.h
 *
 * @author John Wason, PhD
 *
 * @copyright Copyright 2011-2020 Wason Technology, LLC
 *
 * @par License
 * Software License Agreement (Apache License)
 * @par
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * @par
 * http://www.apache.org/licenses/LICENSE-2.0
 * @par
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include "RobotRaconteur/RobotRaconteurNode.h"
#include "RobotRaconteur/DataTypes.h"
#include "RobotRaconteur/ServiceFactory.h"
#include "RobotRaconteur/ServiceDefinition.h"
#include "RobotRaconteur/ServiceStructure.h"
#include <boost/atomic.hpp>

#pragma warning(push)
#pragma warning(disable : 4996)
#include <boost/signals2.hpp>

namespace RobotRaconteur
{
class ROBOTRACONTEUR_CORE_API ClientContext;
class ROBOTRACONTEUR_CORE_API PipeClientBase;
class ROBOTRACONTEUR_CORE_API WireClientBase;

class ROBOTRACONTEUR_CORE_API ServiceStub : public virtual RRObject, public RR_ENABLE_SHARED_FROM_THIS<ServiceStub>
{

  public:
    ServiceStub(boost::string_ref path, const RR_SHARED_PTR<ClientContext>& c);

    std::string ServicePath;

    RR_WEAK_PTR<ClientContext> context;

    uint32_t RREndpoint;

    RR_SHARED_PTR<boost::recursive_mutex> RRMutex;

    RR_SHARED_PTR<ClientContext> GetContext();

    RR_INTRUSIVE_PTR<MessageEntry> ProcessRequest(const RR_INTRUSIVE_PTR<MessageEntry>& m);

    void AsyncProcessRequest(const RR_INTRUSIVE_PTR<MessageEntry>& m,
                             RR_MOVE_ARG(boost::function<void(const RR_INTRUSIVE_PTR<MessageEntry>&,
                                                              const RR_SHARED_PTR<RobotRaconteurException>&)>) handler,
                             int32_t timeout = RR_TIMEOUT_INFINITE);

    virtual void DispatchEvent(const RR_INTRUSIVE_PTR<MessageEntry>& m) = 0;

    RR_SHARED_PTR<RRObject> FindObjRef(boost::string_ref n);

    void AsyncFindObjRef(boost::string_ref n,
                         RR_MOVE_ARG(boost::function<void(const RR_SHARED_PTR<RRObject>&,
                                                          const RR_SHARED_PTR<RobotRaconteurException>&)>) handler,
                         int32_t timeout = RR_TIMEOUT_INFINITE);

    RR_SHARED_PTR<RRObject> FindObjRef(boost::string_ref n, boost::string_ref i);

    RR_SHARED_PTR<RRObject> FindObjRefTyped(boost::string_ref n, boost::string_ref objecttype);

    RR_SHARED_PTR<RRObject> FindObjRefTyped(boost::string_ref n, boost::string_ref i, boost::string_ref objecttype);

    void AsyncFindObjRef(boost::string_ref n, boost::string_ref i,
                         RR_MOVE_ARG(boost::function<void(const RR_SHARED_PTR<RRObject>&,
                                                          const RR_SHARED_PTR<RobotRaconteurException>&)>) handler,
                         int32_t timeout = RR_TIMEOUT_INFINITE);

    void AsyncFindObjRefTyped(boost::string_ref n, boost::string_ref objecttype,
                              RR_MOVE_ARG(boost::function<void(const RR_SHARED_PTR<RRObject>&,
                                                               const RR_SHARED_PTR<RobotRaconteurException>&)>) handler,
                              int32_t timeout = RR_TIMEOUT_INFINITE);

    void AsyncFindObjRefTyped(boost::string_ref n, boost::string_ref i, boost::string_ref objecttype,
                              RR_MOVE_ARG(boost::function<void(const RR_SHARED_PTR<RRObject>&,
                                                               const RR_SHARED_PTR<RobotRaconteurException>&)>) handler,
                              int32_t timeout = RR_TIMEOUT_INFINITE);

    template <typename T>
    static void EndAsyncFindObjRef(
        const RR_SHARED_PTR<RRObject>& obj, const RR_SHARED_PTR<RobotRaconteurException>& err,
        const boost::function<void(const RR_SHARED_PTR<T>&, const RR_SHARED_PTR<RobotRaconteurException>&)>& handler);

    std::string FindObjectType(boost::string_ref n);

    std::string FindObjectType(boost::string_ref n, boost::string_ref i);

    void AsyncFindObjectType(boost::string_ref n,
                             RR_MOVE_ARG(boost::function<void(const RR_SHARED_PTR<std::string>&,
                                                              const RR_SHARED_PTR<RobotRaconteurException>&)>) handler,
                             int32_t timeout = RR_TIMEOUT_INFINITE);

    void AsyncFindObjectType(boost::string_ref n, boost::string_ref i,
                             RR_MOVE_ARG(boost::function<void(const RR_SHARED_PTR<std::string>&,
                                                              const RR_SHARED_PTR<RobotRaconteurException>&)>) handler,
                             int32_t timeout = RR_TIMEOUT_INFINITE);

    void AsyncSendPipeMessage(const RR_INTRUSIVE_PTR<MessageEntry>& m, bool unreliable,
                              const boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>&)>& handler);

    void SendWireMessage(const RR_INTRUSIVE_PTR<MessageEntry>& m);

    virtual void DispatchPipeMessage(const RR_INTRUSIVE_PTR<MessageEntry>& m);

    virtual void DispatchWireMessage(const RR_INTRUSIVE_PTR<MessageEntry>& m);

    virtual RR_INTRUSIVE_PTR<MessageEntry> CallbackCall(const RR_INTRUSIVE_PTR<MessageEntry>& m);

    RR_OVIRTUAL std::string RRType() RR_OVERRIDE { return "RobotRaconteur.ServiceStub"; }

    virtual void RRClose();

    virtual void RRInitStub() = 0;

    virtual RR_SHARED_PTR<PipeClientBase> RRGetPipeClient(boost::string_ref membername);

    virtual RR_SHARED_PTR<WireClientBase> RRGetWireClient(boost::string_ref membername);

    RR_SHARED_PTR<RobotRaconteurNode> RRGetNode();
    RR_WEAK_PTR<RobotRaconteurNode> RRGetNodeWeak();

  protected:
    RR_WEAK_PTR<RobotRaconteurNode> node;
};

struct ROBOTRACONTEUR_CORE_API PullServiceDefinitionReturn
{
    RR_SHARED_PTR<ServiceDefinition> def;
    RR_INTRUSIVE_PTR<RRMap<std::string, RRValue> > attributes;
    std::set<std::string> extra_imports;
};

struct ROBOTRACONTEUR_CORE_API PullServiceDefinitionAndImportsReturn
{
    std::vector<RR_SHARED_PTR<ServiceDefinition> > defs;
    RR_INTRUSIVE_PTR<RRMap<std::string, RRValue> > attributes;
    std::set<std::string> extra_imports;
};

class ROBOTRACONTEUR_CORE_API ClientContext : public Endpoint, public RR_ENABLE_SHARED_FROM_THIS<ClientContext>
{
  protected:
    RR_UNORDERED_MAP<MessageStringPtr, RR_SHARED_PTR<ServiceStub> > stubs;
    boost::mutex stubs_lock;
    std::list<std::string> active_stub_searches;
    std::list<boost::tuple<std::string, boost::function<void(const RR_SHARED_PTR<RRObject>&,
                                                             const RR_SHARED_PTR<RobotRaconteurException>&)> > >
        active_stub_searches_handlers;

    RR_SHARED_PTR<ServiceFactory> m_ServiceDef;

  public:
    RR_SHARED_PTR<ServiceFactory> GetServiceDef() const;

    ClientContext(const RR_SHARED_PTR<RobotRaconteurNode>& node);

    ClientContext(const RR_SHARED_PTR<ServiceFactory>& service_def, const RR_SHARED_PTR<RobotRaconteurNode>& node);

    RR_SHARED_PTR<RRObject> FindObjRef(boost::string_ref path, boost::string_ref objecttype = "");

    void AsyncFindObjRef(boost::string_ref path, boost::string_ref objecttype2,
                         RR_MOVE_ARG(boost::function<void(const RR_SHARED_PTR<RRObject>&,
                                                          const RR_SHARED_PTR<RobotRaconteurException>&)>) handler,
                         int32_t timeout = RR_TIMEOUT_INFINITE);

    template <typename T>
    static void EndAsyncFindObjRef(
        const RR_SHARED_PTR<RRObject>& obj, const RR_SHARED_PTR<RobotRaconteurException>& err,
        const boost::function<void(const RR_SHARED_PTR<T>&, const RR_SHARED_PTR<RobotRaconteurException>&)>& handler)
    {
        if (err)
        {
            handler(RR_SHARED_PTR<T>(), err);
            return;
        }
        else
        {
            RR_SHARED_PTR<T> castobj = RR_DYNAMIC_POINTER_CAST<T>(obj);
            if (!castobj)
            {
                handler(RR_SHARED_PTR<T>(), RR_MAKE_SHARED<DataTypeMismatchException>("Invalid object type returned"));
                return;
            }
            handler(castobj, RR_SHARED_PTR<RobotRaconteurException>());
        }
    }

  protected:
    void AsyncFindObjRef1(const RR_INTRUSIVE_PTR<MessageEntry>& ret, const RR_SHARED_PTR<RobotRaconteurException>& err,
                          const std::string& path, const std::string& objecttype2,
                          const boost::function<void(const RR_SHARED_PTR<RRObject>&,
                                                     const RR_SHARED_PTR<RobotRaconteurException>&)>& handler,
                          int32_t timeout);

    void AsyncFindObjRef2(const RR_SHARED_PTR<PullServiceDefinitionAndImportsReturn>& ret,
                          const RR_SHARED_PTR<RobotRaconteurException>& err, const std::string& objecttype,
                          const std::string& objectdef, const std::string& path, const std::string& objecttype2,
                          const boost::function<void(const RR_SHARED_PTR<RRObject>&,
                                                     const RR_SHARED_PTR<RobotRaconteurException>&)>& handler,
                          int32_t timeout);

    void AsyncFindObjRef3(const RR_SHARED_PTR<RRObject>& ret, const RR_SHARED_PTR<RobotRaconteurException>& err,
                          const std::string& path,
                          const boost::function<void(const RR_SHARED_PTR<RRObject>&,
                                                     const RR_SHARED_PTR<RobotRaconteurException>&)>& handler);

  public:
    std::string FindObjectType(boost::string_ref path);

    void AsyncFindObjectType(boost::string_ref path,
                             RR_MOVE_ARG(boost::function<void(const RR_SHARED_PTR<std::string>&,
                                                              const RR_SHARED_PTR<RobotRaconteurException>&)>) handler,
                             int32_t timeout = RR_TIMEOUT_INFINITE);

    bool VerifyObjectImplements(boost::string_ref objecttype, boost::string_ref implementstype);

  protected:
    void AsyncFindObjectType1(const RR_INTRUSIVE_PTR<MessageEntry>& ret,
                              const RR_SHARED_PTR<RobotRaconteurException>& err, const std::string& path,
                              const boost::function<void(const RR_SHARED_PTR<std::string>&,
                                                         const RR_SHARED_PTR<RobotRaconteurException>&)>& handler);

  private:
    boost::mutex pulled_service_defs_lock;
    std::map<std::string, RR_SHARED_PTR<ServiceDefinition> > pulled_service_defs;

    bool VerifyObjectImplements2(boost::string_ref objecttype, boost::string_ref implementstype);

    struct outstanding_request
    {
        RR_INTRUSIVE_PTR<MessageEntry> ret;
        RR_SHARED_PTR<AutoResetEvent> evt;
        boost::function<void(const RR_INTRUSIVE_PTR<MessageEntry>& ret, const RR_SHARED_PTR<RobotRaconteurException>&)>
            handler;
        RR_SHARED_PTR<Timer> timer;
    };

    boost::mutex outstanding_requests_lock;
    RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<outstanding_request> > outstanding_requests;

    boost::mutex FindObjRef_lock;

  public:
    RR_INTRUSIVE_PTR<MessageEntry> ProcessRequest(const RR_INTRUSIVE_PTR<MessageEntry>& m);

    void AsyncProcessRequest(const RR_INTRUSIVE_PTR<MessageEntry>& m,
                             RR_MOVE_ARG(boost::function<void(const RR_INTRUSIVE_PTR<MessageEntry>&,
                                                              const RR_SHARED_PTR<RobotRaconteurException>&)>) handler,
                             int32_t timeout = RR_TIMEOUT_INFINITE);

    using Endpoint::SendMessage;
    void SendMessage(const RR_INTRUSIVE_PTR<MessageEntry>& m);

    using Endpoint::AsyncSendMessage;
    void AsyncSendMessage(const RR_INTRUSIVE_PTR<MessageEntry>& m,
                          const boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>&)>& callback);

  private:
    void AsyncProcessRequest_err(const RR_SHARED_PTR<RobotRaconteurException>& error, uint32_t requestid);

    void AsyncProcessRequest_timeout(const TimerEvent& error, uint32_t requestid);

    uint32_t request_number;

    RR_SHARED_PTR<Timer> connection_test_timer;

    void connection_test(const TimerEvent& ev);

  public:
    RR_OVIRTUAL void MessageReceived(const RR_INTRUSIVE_PTR<Message>& m) RR_OVERRIDE;

  protected:
    void MessageEntryReceived(const RR_INTRUSIVE_PTR<MessageEntry>& m);

    std::string m_ServiceName;

  public:
    std::string GetServiceName() const;

    bool GetConnected() const;

  protected:
    bool m_Connected;
    boost::mutex m_Connected_lock;
    boost::mutex close_lock;

  private:
    RR_SHARED_PTR<Transport> connecttransport;
    std::string connecturl;

  public:
    std::map<std::string, RR_INTRUSIVE_PTR<RRValue> > GetAttributes();

  private:
    std::map<std::string, RR_INTRUSIVE_PTR<RRValue> > m_Attributes;
    boost::mutex m_Attributes_lock;

  public:
    RR_SHARED_PTR<RRObject> ConnectService(const RR_SHARED_PTR<Transport>& c, boost::string_ref url,
                                           boost::string_ref username = "",
                                           const RR_INTRUSIVE_PTR<RRMap<std::string, RRValue> >& credentials =
                                               (RR_INTRUSIVE_PTR<RRMap<std::string, RRValue> >()),
                                           boost::string_ref objecttype = "");

    RR_SHARED_PTR<RRObject> ConnectService(const RR_SHARED_PTR<Transport>& c,
                                           const RR_SHARED_PTR<ITransportConnection>& tc, boost::string_ref url,
                                           boost::string_ref username = "",
                                           const RR_INTRUSIVE_PTR<RRMap<std::string, RRValue> >& credentials =
                                               (RR_INTRUSIVE_PTR<RRMap<std::string, RRValue> >()),
                                           boost::string_ref objecttype = "");

    void AsyncConnectService(
        const RR_SHARED_PTR<Transport>& c, boost::string_ref url, boost::string_ref username,
        const RR_INTRUSIVE_PTR<RRMap<std::string, RRValue> >& credentials, boost::string_ref objecttype,
        RR_MOVE_ARG(
            boost::function<void(const RR_SHARED_PTR<RRObject>&, const RR_SHARED_PTR<RobotRaconteurException>&)>)
            handler,
        int32_t timeout = RR_TIMEOUT_INFINITE);

    void AsyncConnectService(
        const RR_SHARED_PTR<Transport>& c, const RR_SHARED_PTR<ITransportConnection>& tc, boost::string_ref url,
        boost::string_ref username, const RR_INTRUSIVE_PTR<RRMap<std::string, RRValue> >& credentials,
        boost::string_ref objecttype,
        RR_MOVE_ARG(
            boost::function<void(const RR_SHARED_PTR<RRObject>&, const RR_SHARED_PTR<RobotRaconteurException>&)>)
            handler,
        int32_t timeout = RR_TIMEOUT_INFINITE);

  protected:
    // These functions are part of the connection process
    void AsyncConnectService1(
        const RR_SHARED_PTR<Transport>& c, const RR_SHARED_PTR<ITransportConnection>& tc,
        const RR_SHARED_PTR<RobotRaconteurException>& e, const std::string& url, const std::string& username,
        const RR_INTRUSIVE_PTR<RRMap<std::string, RRValue> >& credentials, const std::string& objecttype,
        boost::function<void(const RR_SHARED_PTR<RRObject>&, const RR_SHARED_PTR<RobotRaconteurException>&)>& handler);

    void AsyncConnectService2(
        const RR_SHARED_PTR<PullServiceDefinitionAndImportsReturn>& d, const RR_SHARED_PTR<RobotRaconteurException>& e,
        const std::string& username, const RR_INTRUSIVE_PTR<RRMap<std::string, RRValue> >& credentials,
        const std::string& objecttype,
        boost::function<void(const RR_SHARED_PTR<RRObject>&, const RR_SHARED_PTR<RobotRaconteurException>&)>& handler);

    void AsyncConnectService3(
        const RR_INTRUSIVE_PTR<MessageEntry>& ret, const RR_SHARED_PTR<RobotRaconteurException>& e,
        const std::string& username, const RR_INTRUSIVE_PTR<RRMap<std::string, RRValue> >& credentials,
        const std::string& objecttype, const RR_SHARED_PTR<PullServiceDefinitionAndImportsReturn>& d,
        const boost::function<void(const RR_SHARED_PTR<RRObject>&, const RR_SHARED_PTR<RobotRaconteurException>&)>&
            handler);

    void AsyncConnectService4(const RR_SHARED_PTR<PullServiceDefinitionAndImportsReturn>& d,
                              const RR_SHARED_PTR<RobotRaconteurException>& e, const std::string& username,
                              const RR_INTRUSIVE_PTR<RRMap<std::string, RRValue> >& credentials,
                              const std::string& objecttype, const std::string& type,
                              const boost::function<void(const RR_SHARED_PTR<RRObject>&,
                                                         const RR_SHARED_PTR<RobotRaconteurException>&)>& handler);

    void AsyncConnectService5(const RR_INTRUSIVE_PTR<MessageEntry>& ret,
                              const RR_SHARED_PTR<RobotRaconteurException>& e, const std::string& username,
                              const RR_INTRUSIVE_PTR<RRMap<std::string, RRValue> >& credentials,
                              const std::string& objecttype, const std::string& type,
                              const RR_SHARED_PTR<PullServiceDefinitionAndImportsReturn>& d,
                              const boost::function<void(const RR_SHARED_PTR<RRObject>&,
                                                         const RR_SHARED_PTR<RobotRaconteurException>&)>& handler);

    void AsyncConnectService6(const RR_SHARED_PTR<std::string>& ret, const RR_SHARED_PTR<RobotRaconteurException>& e,
                              const std::string& type, const std::string& username,
                              const RR_SHARED_PTR<PullServiceDefinitionAndImportsReturn>& d,
                              const boost::function<void(const RR_SHARED_PTR<RRObject>&,
                                                         const RR_SHARED_PTR<RobotRaconteurException>&)>& handler);

    void AsyncConnectService7(const RR_INTRUSIVE_PTR<MessageEntry>& ret,
                              const RR_SHARED_PTR<RobotRaconteurException>& e, const std::string& objecttype,
                              const std::string& username,
                              const boost::function<void(const RR_SHARED_PTR<RRObject>&,
                                                         const RR_SHARED_PTR<RobotRaconteurException>&)>& handler);

    boost::recursive_mutex connect_lock;

  public:
    void Close();

    void AsyncClose(RR_MOVE_ARG(boost::function<void()>) handler);

  protected:
    void AsyncClose1(const RR_INTRUSIVE_PTR<MessageEntry>& m, const RR_SHARED_PTR<RobotRaconteurException>& err,
                     const boost::function<void()>& handler);

  public:
    void AsyncSendPipeMessage(const RR_INTRUSIVE_PTR<MessageEntry>& m, bool unreliable,
                              const boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>&)>& handler);

    void SendWireMessage(const RR_INTRUSIVE_PTR<MessageEntry>& m);

    PullServiceDefinitionReturn PullServiceDefinition(boost::string_ref ServiceType = "");

    void AsyncPullServiceDefinition(boost::string_ref ServiceType,
                                    RR_MOVE_ARG(boost::function<void(const RR_SHARED_PTR<PullServiceDefinitionReturn>&,
                                                                     const RR_SHARED_PTR<RobotRaconteurException>&)>)
                                        handler,
                                    int32_t timeout = RR_TIMEOUT_INFINITE);

  protected:
    void AsyncPullServiceDefinition1(const RR_INTRUSIVE_PTR<MessageEntry>& ret3,
                                     const RR_SHARED_PTR<RobotRaconteurException>& err, const std::string& ServiceType,
                                     boost::function<void(const RR_SHARED_PTR<PullServiceDefinitionReturn>&,
                                                          const RR_SHARED_PTR<RobotRaconteurException>&)>& handler);

  public:
    PullServiceDefinitionAndImportsReturn PullServiceDefinitionAndImports(boost::string_ref servicetype = "");

    void AsyncPullServiceDefinitionAndImports(
        boost::string_ref servicetype,
        RR_MOVE_ARG(boost::function<void(const RR_SHARED_PTR<PullServiceDefinitionAndImportsReturn>&,
                                         const RR_SHARED_PTR<RobotRaconteurException>&)>) handler,
        int32_t timeout = RR_TIMEOUT_INFINITE);

  protected:
    void AsyncPullServiceDefinitionAndImports1(
        const RR_SHARED_PTR<PullServiceDefinitionReturn>& pull_ret, const RR_SHARED_PTR<RobotRaconteurException>& err,
        const std::string& servicetype, const RR_SHARED_PTR<PullServiceDefinitionAndImportsReturn>& current,
        boost::function<void(const RR_SHARED_PTR<PullServiceDefinitionAndImportsReturn>&,
                             const RR_SHARED_PTR<RobotRaconteurException>&)>& handler,
        boost::posix_time::ptime timeout_time);

  public:
    std::vector<std::string> GetPulledServiceTypes();

    RR_SHARED_PTR<ServiceFactory> GetPulledServiceType(boost::string_ref type);

  private:
    boost::mutex pulled_service_types_lock;
    RR_UNORDERED_MAP<std::string, RR_SHARED_PTR<ServiceFactory> > pulled_service_types;

    bool use_pulled_types;

    RR_SHARED_PTR<ITransportConnection> TransportConnection;

    boost::mutex m_Authentication_lock;

    bool m_UserAuthenticated;

  public:
    const bool GetUserAuthenticated();

  private:
    std::string m_AuthenticatedUsername;

  public:
    const std::string GetAuthenticatedUsername();

    std::string AuthenticateUser(boost::string_ref username, const RR_INTRUSIVE_PTR<RRValue>& credentials);

    void AsyncAuthenticateUser(
        boost::string_ref username, const RR_INTRUSIVE_PTR<RRValue>& credentials,
        RR_MOVE_ARG(
            boost::function<void(const RR_SHARED_PTR<std::string>&, const RR_SHARED_PTR<RobotRaconteurException>&)>)
            handler,
        int32_t timeout = RR_TIMEOUT_INFINITE);

  protected:
    void AsyncAuthenticateUser2(const RR_INTRUSIVE_PTR<MessageEntry>& ret,
                                const RR_SHARED_PTR<RobotRaconteurException>& err, const std::string& username,
                                boost::function<void(const RR_SHARED_PTR<std::string>&,
                                                     const RR_SHARED_PTR<RobotRaconteurException>&)>& handler);

  public:
    std::string LogoutUser();

    std::string RequestObjectLock(const RR_SHARED_PTR<RRObject>& obj, RobotRaconteurObjectLockFlags flags);

    void AsyncRequestObjectLock(
        const RR_SHARED_PTR<RRObject>& obj, RobotRaconteurObjectLockFlags flags,
        RR_MOVE_ARG(
            boost::function<void(const RR_SHARED_PTR<std::string>&, const RR_SHARED_PTR<RobotRaconteurException>&)>)
            handler,
        int32_t timeout = RR_TIMEOUT_INFINITE);

  protected:
    void EndAsyncLockOp(const RR_INTRUSIVE_PTR<MessageEntry>& ret, const RR_SHARED_PTR<RobotRaconteurException>& err,
                        boost::function<void(const RR_SHARED_PTR<std::string>&,
                                             const RR_SHARED_PTR<RobotRaconteurException>&)>& handler);

  public:
    std::string ReleaseObjectLock(const RR_SHARED_PTR<RRObject>& obj);

    void AsyncReleaseObjectLock(
        const RR_SHARED_PTR<RRObject>& obj,
        RR_MOVE_ARG(
            boost::function<void(const RR_SHARED_PTR<std::string>&, const RR_SHARED_PTR<RobotRaconteurException>&)>)
            handler,
        int32_t timeout = RR_TIMEOUT_INFINITE);

    std::string MonitorEnter(const RR_SHARED_PTR<RRObject>& obj, int32_t timeout);

    void MonitorExit(const RR_SHARED_PTR<RRObject>& obj);

    RR_OVIRTUAL void PeriodicCleanupTask() RR_OVERRIDE;

    uint32_t CheckServiceCapability(boost::string_ref name);

    //		public event ClientServiceListenerDelegate ClientServiceListener;

    boost::signals2::signal<void(const RR_SHARED_PTR<ClientContext>&, ClientServiceListenerEventType,
                                 const RR_SHARED_PTR<void>&)>
        ClientServiceListener;

  protected:
    void ProcessCallbackCall(const RR_INTRUSIVE_PTR<MessageEntry>& m);

    boost::atomic<bool> use_combined_connection;

  public:
    RR_OVIRTUAL void TransportConnectionClosed(uint32_t endpoint) RR_OVERRIDE;

    virtual void TransportConnectionClosed1();
};

template <typename T>
void ServiceStub::EndAsyncFindObjRef(
    const RR_SHARED_PTR<RRObject>& obj, const RR_SHARED_PTR<RobotRaconteurException>& err,
    const boost::function<void(const RR_SHARED_PTR<T>&, const RR_SHARED_PTR<RobotRaconteurException>&)>& handler)
{
    ClientContext::EndAsyncFindObjRef(obj, err, handler);
}

#ifndef BOOST_NO_CXX11_TEMPLATE_ALIASES
using ServiceStubPtr = RR_SHARED_PTR<ServiceStub>;
using ServiceStubConstPtr = RR_SHARED_PTR<const ServiceStub>;
#endif

} // namespace RobotRaconteur
