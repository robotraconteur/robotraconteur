// Copyright 2011-2020 Wason Technology, LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifdef ROBOTRACONTEUR_CORE_USE_STDAFX
#include "stdafx.h"
#endif

#include "RobotRaconteur/Client.h"
#include "RobotRaconteur/ServiceDefinition.h"
#include "RobotRaconteur/Message.h"
#include "RobotRaconteur/Error.h"
#include <boost/algorithm/string.hpp>
#include "RobotRaconteur/RobotRaconteurNode.h"
#include "RobotRaconteur/AutoResetEvent.h"
#include "RobotRaconteur/DataTypes.h"

#include "RobotRaconteur/ErrorUtil.h"

#include <boost/range/adaptors.hpp>
#include <boost/range/algorithm.hpp>
#include <boost/foreach.hpp>

#undef SendMessage

namespace RobotRaconteur
{
static void rr_context_emptyhandler(const RR_SHARED_PTR<RobotRaconteurException>&) {}

static void rr_context_node_handler(const RR_SHARED_PTR<RobotRaconteurNode>& n,
                                    const RR_SHARED_PTR<RobotRaconteurException>& e)
{
    n->HandleException(e.get());
}

ServiceStub::ServiceStub(boost::string_ref path, const RR_SHARED_PTR<ClientContext>& c)
{
    context = c;
    ServicePath = RR_MOVE(path.to_string());
    this->node = c->GetNode();
    RRMutex = RR_MAKE_SHARED<boost::recursive_mutex>();
    RREndpoint = c->GetLocalEndpoint();
    ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Client, RREndpoint, ServicePath, "",
                                            "ServiceStub created with type: " << ServiceStub::RRType());
}

RR_SHARED_PTR<ClientContext> ServiceStub::GetContext() // NOLINT(readability-make-member-function-const)
{
    RR_SHARED_PTR<ClientContext> out = context.lock();
    if (!out)
        throw InvalidOperationException("Stub has been closed");
    return out;
}

RR_INTRUSIVE_PTR<MessageEntry> ServiceStub::ProcessRequest(const RR_INTRUSIVE_PTR<MessageEntry>& m)
{

    m->ServicePath = ServicePath;
    return GetContext()->ProcessRequest(m);
}

void ServiceStub::AsyncProcessRequest(
    const RR_INTRUSIVE_PTR<MessageEntry>& m,
    RR_MOVE_ARG(
        boost::function<void(const RR_INTRUSIVE_PTR<MessageEntry>&, const RR_SHARED_PTR<RobotRaconteurException>&)>)
        handler,
    int32_t timeout)
{
    m->ServicePath = ServicePath;
    GetContext()->AsyncProcessRequest(m, RR_MOVE(handler), timeout);
}

RR_SHARED_PTR<RRObject> ServiceStub::FindObjRef(boost::string_ref n)
{

    return GetContext()->FindObjRef(ServicePath + "." + n);
}

void ServiceStub::AsyncFindObjRef(
    boost::string_ref n,
    RR_MOVE_ARG(boost::function<void(const RR_SHARED_PTR<RRObject>&, const RR_SHARED_PTR<RobotRaconteurException>&)>)
        handler,
    int32_t timeout)
{
    GetContext()->AsyncFindObjRef(ServicePath + "." + n, "", RR_MOVE(handler), timeout);
}

RR_SHARED_PTR<RRObject> ServiceStub::FindObjRef(boost::string_ref n, boost::string_ref i)
{

    return GetContext()->FindObjRef(ServicePath + "." + n + "[" + detail::encode_index(i) + "]");
}

RR_SHARED_PTR<RRObject> ServiceStub::FindObjRefTyped(boost::string_ref n, boost::string_ref objecttype)
{

    return GetContext()->FindObjRef(ServicePath + "." + n, objecttype);
}

RR_SHARED_PTR<RRObject> ServiceStub::FindObjRefTyped(boost::string_ref n, boost::string_ref i,
                                                     boost::string_ref objecttype)
{

    return GetContext()->FindObjRef(ServicePath + "." + n + "[" + detail::encode_index(i) + "]", objecttype);
}

void ServiceStub::AsyncFindObjRef(
    boost::string_ref n, boost::string_ref i,
    RR_MOVE_ARG(boost::function<void(const RR_SHARED_PTR<RRObject>&, const RR_SHARED_PTR<RobotRaconteurException>&)>)
        handler,
    int32_t timeout)
{
    GetContext()->AsyncFindObjRef(ServicePath + "." + n + "[" + detail::encode_index(i) + "]", "", RR_MOVE(handler),
                                  timeout);
}

void ServiceStub::AsyncFindObjRefTyped(
    boost::string_ref n, boost::string_ref objecttype,
    RR_MOVE_ARG(boost::function<void(const RR_SHARED_PTR<RRObject>&, const RR_SHARED_PTR<RobotRaconteurException>&)>)
        handler,
    int32_t timeout)
{
    GetContext()->AsyncFindObjRef(ServicePath + "." + n, objecttype, RR_MOVE(handler), timeout);
}

void ServiceStub::AsyncFindObjRefTyped(
    boost::string_ref n, boost::string_ref i, boost::string_ref objecttype,
    RR_MOVE_ARG(boost::function<void(const RR_SHARED_PTR<RRObject>&, const RR_SHARED_PTR<RobotRaconteurException>&)>)
        handler,
    int32_t timeout)
{
    GetContext()->AsyncFindObjRef(ServicePath + "." + n + "[" + detail::encode_index(i) + "]", objecttype,
                                  RR_MOVE(handler), timeout);
}

std::string ServiceStub::FindObjectType(boost::string_ref n)
{
    return GetContext()->FindObjectType(ServicePath + "." + n);
}

std::string ServiceStub::FindObjectType(boost::string_ref n, boost::string_ref i)
{
    return GetContext()->FindObjectType(ServicePath + "." + n + "[" + detail::encode_index(i) + "]");
}

void ServiceStub::AsyncFindObjectType(
    boost::string_ref n,
    RR_MOVE_ARG(boost::function<void(const RR_SHARED_PTR<std::string>&, const RR_SHARED_PTR<RobotRaconteurException>&)>)
        handler,
    int32_t timeout)
{
    GetContext()->AsyncFindObjectType(ServicePath + "." + n, RR_MOVE(handler), timeout);
}

void ServiceStub::AsyncFindObjectType(
    boost::string_ref n, boost::string_ref i,
    RR_MOVE_ARG(boost::function<void(const RR_SHARED_PTR<std::string>&, const RR_SHARED_PTR<RobotRaconteurException>&)>)
        handler,
    int32_t timeout)
{
    GetContext()->AsyncFindObjectType(ServicePath + "." + n + "[" + detail::encode_index(i) + "]", RR_MOVE(handler),
                                      timeout);
}

void ServiceStub::AsyncSendPipeMessage(
    const RR_INTRUSIVE_PTR<MessageEntry>& m, bool unreliable,
    const boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>&)>& handler)
{

    m->ServicePath = ServicePath;
    GetContext()->AsyncSendPipeMessage(m, unreliable, (handler));
}

void ServiceStub::SendWireMessage(const RR_INTRUSIVE_PTR<MessageEntry>& m)
{

    m->ServicePath = ServicePath;
    GetContext()->SendWireMessage(m);
}

void ServiceStub::DispatchPipeMessage(const RR_INTRUSIVE_PTR<MessageEntry>& m)
{
    ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Client, RREndpoint, ServicePath, m->MemberName,
                                            "Pipe packet received for nonexistant member");
}

void ServiceStub::DispatchWireMessage(const RR_INTRUSIVE_PTR<MessageEntry>& m)
{
    ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Client, RREndpoint, ServicePath, m->MemberName,
                                            "Wire packet received for nonexistant member");
}

RR_INTRUSIVE_PTR<MessageEntry> ServiceStub::CallbackCall(const RR_INTRUSIVE_PTR<MessageEntry>& m)
{
    ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Client, RREndpoint, ServicePath, m->MemberName,
                                            "Callback request received for nonexistant member");
    throw MemberNotFoundException("Member not found");
}

void ServiceStub::RRClose()
{
    ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Client, RREndpoint, ServicePath, "", "Client stub closed");
}

RR_SHARED_PTR<PipeClientBase> ServiceStub::RRGetPipeClient(boost::string_ref membername)
{
    throw MemberNotFoundException("PipeClient \"" + membername + "\" not found");
}

RR_SHARED_PTR<WireClientBase> ServiceStub::RRGetWireClient(boost::string_ref membername)
{
    throw MemberNotFoundException("WireClient \"" + membername + "\" not found");
}

RR_SHARED_PTR<RobotRaconteurNode> ServiceStub::RRGetNode()
{
    RR_SHARED_PTR<RobotRaconteurNode> n = node.lock();
    if (!n)
        throw InvalidOperationException("Node has been released");
    return n;
}

RR_WEAK_PTR<RobotRaconteurNode> ServiceStub::RRGetNodeWeak() { return node; }

RR_SHARED_PTR<ServiceFactory> ClientContext::GetServiceDef() const { return m_ServiceDef; }

ClientContext::ClientContext(const RR_SHARED_PTR<RobotRaconteurNode>& node)
    : Endpoint(node), use_combined_connection(false)
{

    request_number = 0;
    m_Connected = false;

    m_UserAuthenticated = false;
    use_pulled_types = false;

    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Client, -1, "ClientContext created");
}

ClientContext::ClientContext(const RR_SHARED_PTR<ServiceFactory>& service_def,
                             const RR_SHARED_PTR<RobotRaconteurNode>& node)
    : Endpoint(node), use_combined_connection(false)
{
    request_number = 0;
    m_Connected = false;

    m_UserAuthenticated = false;
    use_pulled_types = false;

    m_ServiceDef = service_def;

    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Client, -1, "ClientContext created");

    // rec_event = new AutoResetEvent(false);
}

RR_SHARED_PTR<RRObject> ClientContext::FindObjRef(boost::string_ref path, boost::string_ref objecttype2)
{
    ROBOTRACONTEUR_ASSERT_MULTITHREADED(node);

    RR_SHARED_PTR<detail::sync_async_handler<RRObject> > h = RR_MAKE_SHARED<detail::sync_async_handler<RRObject> >();
    AsyncFindObjRef(path, objecttype2,
                    boost::bind(&detail::sync_async_handler<RRObject>::operator(), h, RR_BOOST_PLACEHOLDERS(_1),
                                RR_BOOST_PLACEHOLDERS(_2)),
                    boost::numeric_cast<int32_t>(GetNode()->GetRequestTimeout()));
    return h->end();
}

void ClientContext::AsyncFindObjRef(
    boost::string_ref path, boost::string_ref objecttype2,
    RR_MOVE_ARG(boost::function<void(const RR_SHARED_PTR<RRObject>&, const RR_SHARED_PTR<RobotRaconteurException>&)>)
        handler,
    int32_t timeout)
{

    ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Client, GetLocalEndpoint(), path, "", "Begin FindObjRef");

    if (!GetConnected())
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(node, Client, GetLocalEndpoint(), path, "",
                                                "Service client not connected");
        throw ConnectionException("Service client not connected");
    }

    boost::mutex::scoped_lock lock2(FindObjRef_lock);
    boost::mutex::scoped_lock lock(stubs_lock);
    RR_SHARED_PTR<ServiceStub> stub;
    {
        RR_UNORDERED_MAP<MessageStringPtr, RR_SHARED_PTR<ServiceStub> >::iterator e1 = stubs.find(path);
        if (e1 != stubs.end())
        {
            stub = e1->second;
        }
        else
        {
            if (boost::range::find(active_stub_searches, path) != active_stub_searches.end())
            {
                active_stub_searches_handlers.push_back(boost::make_tuple(path.to_string(), handler));
                return;
            }
        }
    }

    if (stub)
    {
        ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Client, GetLocalEndpoint(), path, "", "Returning cached stub");
        RobotRaconteurNode::TryPostToThreadPool(
            node, boost::bind(handler, rr_cast<RRObject>(stub), RR_SHARED_PTR<RobotRaconteurException>()), true);
        return;
    }
    else
    {
        RR_INTRUSIVE_PTR<MessageEntry> e = CreateMessageEntry(MessageEntryType_ObjectTypeName, "");
        e->AddElement("clientversion", stringToRRArray(ROBOTRACONTEUR_VERSION_TEXT));
        // MessageElement m = e.AddElement("ObjectPath", path);
        e->ServicePath = path;
        RR_SHARED_PTR<detail::async_timeout_wrapper<RRObject> > t =
            RR_MAKE_SHARED<detail::async_timeout_wrapper<RRObject> >(
                GetNode(), boost::bind(&ClientContext::AsyncFindObjRef3, shared_from_this(), RR_BOOST_PLACEHOLDERS(_1),
                                       RR_BOOST_PLACEHOLDERS(_2), path.to_string(), handler));

        boost::function<void(const RR_SHARED_PTR<RRObject>&, const RR_SHARED_PTR<RobotRaconteurException>&)> h1(
            boost::bind(&detail::async_timeout_wrapper<RRObject>::operator(), t, RR_BOOST_PLACEHOLDERS(_1),
                        RR_BOOST_PLACEHOLDERS(_2)));
        AsyncProcessRequest(e,
                            boost::bind(&ClientContext::AsyncFindObjRef1, shared_from_this(), RR_BOOST_PLACEHOLDERS(_1),
                                        RR_BOOST_PLACEHOLDERS(_2), path.to_string(), objecttype2.to_string(), h1,
                                        timeout),
                            timeout);
        t->start_timer(timeout, RR_MAKE_SHARED<RequestTimeoutException>("Request timeout"));
        active_stub_searches.push_back(RR_MOVE(path.to_string()));
    }
}

void ClientContext::AsyncFindObjRef1(
    const RR_INTRUSIVE_PTR<MessageEntry>& ret, const RR_SHARED_PTR<RobotRaconteurException>& err,
    const std::string& path, const std::string& objecttype2,
    const boost::function<void(const RR_SHARED_PTR<RRObject>&, const RR_SHARED_PTR<RobotRaconteurException>&)>& handler,
    int32_t timeout)
{
    if (err)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(node, Client, GetLocalEndpoint(), path, "",
                                                "Error during FindObjRef: " << err->what());
        AsyncFindObjRef3(RR_SHARED_PTR<RRObject>(), err, path, handler);
        return;
    }
    else
    {
        ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Client, GetLocalEndpoint(), path, "",
                                                "Received FindObjRef response");

        try
        {
            std::string objecttype = ret->FindElement("objecttype")->CastDataToString();
            if (!objecttype2.empty() && objecttype != objecttype2)
            {
                RR_INTRUSIVE_PTR<MessageElement> objectimplements_m;
                if (ret->TryFindElement("objectimplements", objectimplements_m))
                {
                    std::vector<std::string> objectimplements =
                        RRListToStringVector(GetNode()->UnpackListType<RRArray<char> >(
                            objectimplements_m->CastDataToNestedList(DataTypes_list_t)));
                    if (boost::range::find(objectimplements, objecttype2) != objectimplements.end())
                    {
                        objecttype = objecttype2;
                    }
                }
            }

            if (objecttype.empty())
            {
                ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(node, Client, GetLocalEndpoint(), path, "",
                                                        "FindObjRef object type was not returned");
                throw ObjectNotFoundException("Object type was not returned.");
            }

            boost::string_ref objectdef = SplitQualifiedName(objecttype).get<0>();

            if (use_pulled_types)
            {

                std::vector<std::string> servicetypes = GetPulledServiceTypes();
                if (std::find(servicetypes.begin(), servicetypes.end(), objectdef) == servicetypes.end())
                {
                    ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Client, GetLocalEndpoint(), path, "",
                                                            "FindObjRef pulling object type: " << objecttype);
                    AsyncPullServiceDefinitionAndImports(
                        objectdef,
                        boost::bind(&ClientContext::AsyncFindObjRef2, shared_from_this(), RR_BOOST_PLACEHOLDERS(_1),
                                    RR_BOOST_PLACEHOLDERS(_2), objecttype, objectdef.to_string(), path, objecttype2,
                                    handler, timeout),
                        timeout);
                    return;
                }
            }

            AsyncFindObjRef2(RR_SHARED_PTR<PullServiceDefinitionAndImportsReturn>(),
                             RR_SHARED_PTR<RobotRaconteurException>(), objecttype, objectdef.to_string(), path,
                             objecttype2, (handler), timeout);
        }
        catch (std::exception& err)
        {
            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(node, Client, GetLocalEndpoint(), path, "",
                                                    "Error during FindObjRef: " << err.what());
            RR_SHARED_PTR<RobotRaconteurException> err1 =
                RobotRaconteurExceptionUtil::ExceptionToSharedPtr(err, MessageErrorType_ServiceError);
            AsyncFindObjRef3(RR_SHARED_PTR<RRObject>(), err1, path, handler);
        }
    }
}

void ClientContext::AsyncFindObjRef2(
    const RR_SHARED_PTR<PullServiceDefinitionAndImportsReturn>& ret, const RR_SHARED_PTR<RobotRaconteurException>& err,
    const std::string& objecttype1, const std::string& objectdef, const std::string& path,
    const std::string& objecttype2,
    const boost::function<void(const RR_SHARED_PTR<RRObject>&, const RR_SHARED_PTR<RobotRaconteurException>&)>& handler,
    int32_t timeout)
{
    RR_UNUSED(timeout);
    std::string objecttype = objecttype1;
    if (err)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(node, Client, GetLocalEndpoint(), path, "",
                                                "Error during FindObjRef: " << err->what());
        AsyncFindObjRef3(RR_SHARED_PTR<RRObject>(), err, path, handler);
        return;
    }
    else
    {

        try
        {

            RR_SHARED_PTR<ServiceStub> stub;

            if (use_pulled_types)
            {
                if (ret)
                {
                    std::vector<std::string> servicetypes = GetPulledServiceTypes();

                    if (std::find(servicetypes.begin(), servicetypes.end(), objectdef) == servicetypes.end())
                    {
                        std::vector<RR_SHARED_PTR<ServiceDefinition> > d = ret->defs;

                        std::vector<RR_SHARED_PTR<ServiceDefinition> > missingdefs =
                            std::vector<RR_SHARED_PTR<ServiceDefinition> >();
                        std::vector<std::string> missingdefs_names;

                        BOOST_FOREACH (RR_SHARED_PTR<ServiceDefinition>& di, d)
                        {

                            std::vector<std::string> servicetypes2 = GetPulledServiceTypes();
                            if (std::find(servicetypes2.begin(), servicetypes2.end(), (di->Name)) ==
                                servicetypes2.end())
                            {
                                missingdefs.push_back(di);
                                missingdefs_names.push_back(di->Name);
                            }
                        }
                        if (!missingdefs.empty())
                        {

                            std::vector<std::string> missingdefs_str;
                            BOOST_FOREACH (const RR_SHARED_PTR<ServiceDefinition>& ee, missingdefs)
                            {
                                missingdefs_str.push_back(ee->ToString());
                            }

                            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Client, GetLocalEndpoint(),
                                                               "FindObjRef using dynamic service factory for types: "
                                                                   << boost::join(missingdefs_names, ", "));

                            std::vector<RR_SHARED_PTR<ServiceFactory> > di2 =
                                GetNode()->GetDynamicServiceFactory()->CreateServiceFactories(missingdefs_str);

                            for (size_t i = 0; i < missingdefs.size(); i++)
                            {
                                boost::mutex::scoped_lock lock(pulled_service_types_lock);
                                pulled_service_types.insert(std::make_pair(di2.at(i)->GetServiceName(), di2.at(i)));
                            }
                        }
                    }
                }

                if (!objecttype2.empty() && objecttype1 != objecttype2)
                {
                    VerifyObjectImplements(objecttype, objecttype2);
                    objecttype = (objecttype2);
                }

                RR_SHARED_PTR<ServiceFactory> f = GetPulledServiceType(objectdef);

                ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(node, Client, GetLocalEndpoint(), path, "",
                                                        "FindObjRef returned type \"" << objecttype
                                                                                      << "\" creating stub");

                stub = f->CreateStub(objecttype, path, shared_from_this());
            }

            else
            {
                if (!objecttype2.empty() && objecttype1 != objecttype2)
                {
                    VerifyObjectImplements(objecttype, objecttype2);
                    objecttype = objecttype2;
                }

                ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(node, Client, GetLocalEndpoint(), path, "",
                                                        "FindObjRef returned type \"" << objecttype
                                                                                      << "\" creating stub");

                stub = GetServiceDef()->CreateStub(objecttype, path, shared_from_this());
            }

            {
                boost::mutex::scoped_lock lock(stubs_lock);
                RR_UNORDERED_MAP<MessageStringPtr, RR_SHARED_PTR<ServiceStub> >::iterator e1 = stubs.find(path);
                if (e1 == stubs.end())
                {
                    stubs.insert(std::make_pair(path, stub));
                }
                else
                {
                    stub = e1->second;
                }
            }

            RR_SHARED_PTR<RRObject> ret = RR_STATIC_POINTER_CAST<RRObject>(stub);
            AsyncFindObjRef3(ret, RR_SHARED_PTR<RobotRaconteurException>(), path, handler);
        }
        catch (std::exception& err2)
        {
            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(node, Client, GetLocalEndpoint(), path, "",
                                                    "Error during FindObjRef: " << err2.what());
            RR_SHARED_PTR<RobotRaconteurException> err1 =
                RobotRaconteurExceptionUtil::ExceptionToSharedPtr(err2, MessageErrorType_ServiceError);
            AsyncFindObjRef3(RR_SHARED_PTR<RRObject>(), err1, path, handler);
        }
    }
}

void ClientContext::AsyncFindObjRef3(
    const RR_SHARED_PTR<RRObject>& ret, const RR_SHARED_PTR<RobotRaconteurException>& err, const std::string& path,
    const boost::function<void(const RR_SHARED_PTR<RRObject>&, const RR_SHARED_PTR<RobotRaconteurException>&)>& handler)
{

    try
    {
        boost::mutex::scoped_lock lock(stubs_lock);
        active_stub_searches.remove(path);
        for (std::list<boost::tuple<
                 std::string, boost::function<void(const RR_SHARED_PTR<RRObject>&,
                                                   const RR_SHARED_PTR<RobotRaconteurException>&)> > >::iterator ee =
                 active_stub_searches_handlers.begin();
             ee != active_stub_searches_handlers.end();)
        {
            if (ee->get<0>() == path)
            {
                try
                {
                    RobotRaconteurNode::TryPostToThreadPool(node, boost::bind(ee->get<1>(), ret, err), true);
                }
                catch (std::exception&)
                {}

                ee = active_stub_searches_handlers.erase(ee);
            }
            else
            {
                ee++;
            }
        }
    }
    catch (std::exception&)
    {}
    if (err)
    {
        detail::InvokeHandlerWithException(node, handler, err);
    }
    else
    {
        detail::InvokeHandler(node, handler, ret);
    }
}

std::string ClientContext::FindObjectType(boost::string_ref path)
{
    ROBOTRACONTEUR_ASSERT_MULTITHREADED(node);

    RR_SHARED_PTR<detail::sync_async_handler<std::string> > h =
        RR_MAKE_SHARED<detail::sync_async_handler<std::string> >(
            RR_MAKE_SHARED<ServiceException>("Error retrieving object type"));
    AsyncFindObjectType(path,
                        boost::bind(&detail::sync_async_handler<std::string>::operator(), h, RR_BOOST_PLACEHOLDERS(_1),
                                    RR_BOOST_PLACEHOLDERS(_2)),
                        boost::numeric_cast<int32_t>(GetNode()->GetRequestTimeout()));
    return *h->end();
}

void ClientContext::AsyncFindObjectType(
    boost::string_ref path,
    RR_MOVE_ARG(boost::function<void(const RR_SHARED_PTR<std::string>&, const RR_SHARED_PTR<RobotRaconteurException>&)>)
        handler,
    int32_t timeout)
{
    ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Client, GetLocalEndpoint(), path, "", "Begin FindObjectType");

    RR_INTRUSIVE_PTR<MessageEntry> e = CreateMessageEntry(MessageEntryType_ObjectTypeName, "");
    e->AddElement("clientversion", stringToRRArray(ROBOTRACONTEUR_VERSION_TEXT));
    // MessageElement m = e.AddElement("ObjectPath", path);
    e->ServicePath = path;
    AsyncProcessRequest(e,
                        boost::bind(&ClientContext::AsyncFindObjectType1, shared_from_this(), RR_BOOST_PLACEHOLDERS(_1),
                                    RR_BOOST_PLACEHOLDERS(_2), path.to_string(), handler),
                        timeout);
}

void ClientContext::AsyncFindObjectType1(
    const RR_INTRUSIVE_PTR<MessageEntry>& ret, const RR_SHARED_PTR<RobotRaconteurException>& err,
    const std::string& path,
    const boost::function<void(const RR_SHARED_PTR<std::string>&, const RR_SHARED_PTR<RobotRaconteurException>&)>&
        handler)
{
    if (err)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(node, Client, GetLocalEndpoint(), path, "",
                                                "Error during FindObjectType: " << err->what());
        detail::InvokeHandlerWithException(node, handler, err);
        return;
    }
    else
    {
        try
        {
            std::string objecttype = ret->FindElement("objecttype")->CastDataToString();

            try
            {
                ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Client, GetLocalEndpoint(), path, "",
                                                        "FindObjectType returned \"" << objecttype << "\"");
                RR_SHARED_PTR<std::string> ret = RR_MAKE_SHARED<std::string>(objecttype);
                detail::InvokeHandler(node, handler, ret);
            }
            catch (std::exception& exp)
            {
                ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(node, Client, GetLocalEndpoint(), path, "",
                                                        "Error during FindObjectType: " << exp.what());
                RobotRaconteurNode::TryHandleException(node, &exp);
            }
        }
        catch (std::exception& err)
        {
            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(node, Client, GetLocalEndpoint(), path, "",
                                                    "Error during FindObjectType: " << err.what());
            detail::InvokeHandlerWithException(node, handler, err, MessageErrorType_ServiceError);
        }
    }
}

RR_INTRUSIVE_PTR<MessageEntry> ClientContext::ProcessRequest(const RR_INTRUSIVE_PTR<MessageEntry>& m)
{

    ROBOTRACONTEUR_ASSERT_MULTITHREADED(node);

    RR_INTRUSIVE_PTR<MessageEntry> rec_message;

    uint32_t myrequestid = 0;

    try
    {
        if (!GetConnected())
            throw ConnectionException("Service client not connected");

        RR_SHARED_PTR<outstanding_request> t = RR_MAKE_SHARED<outstanding_request>();
        t->evt = GetNode()->CreateAutoResetEvent();
        {
            boost::mutex::scoped_lock lock(outstanding_requests_lock);

            do
            {
                request_number++;
                if (request_number >= std::numeric_limits<uint32_t>::max())
                    request_number = 0;
                m->RequestID = request_number;
                myrequestid = request_number;
            } while (outstanding_requests.count(myrequestid) != 0);

            outstanding_requests.insert(std::make_pair(myrequestid, t));
        }

        ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Client, GetLocalEndpoint(), m->ServicePath, m->MemberName,
                                                "ProcessRequest sending message with requestid "
                                                    << myrequestid << " EntryType " << m->EntryType);
        // Console.WriteLine("Sent " + m.RequestID + " " + m.EntryType + " " + m.MemberName);
        SendMessage(m);

        boost::posix_time::ptime request_start = GetNode()->NowNodeTime();
        uint32_t request_timeout = GetNode()->GetRequestTimeout();
        while (true)
        {

            {
                boost::mutex::scoped_lock lock(outstanding_requests_lock);
                if (t->ret)
                    break;
            }
            GetNode()->CheckConnection(GetLocalEndpoint());

            t->evt->WaitOne(10);
            if ((GetNode()->NowNodeTime() - request_start).total_milliseconds() > request_timeout)
            {

                {
                    boost::mutex::scoped_lock lock(outstanding_requests_lock);
                    outstanding_requests.erase(myrequestid);
                }
                throw RequestTimeoutException("Request timeout");
            }
        }

        if (!GetConnected())
            throw ConnectionException("Service client not connected");

        {
            boost::mutex::scoped_lock lock(outstanding_requests_lock);
            outstanding_requests.erase(myrequestid);
            if (!t->ret)
                throw ConnectionException("Service client not connected");
            rec_message = t->ret;
        }

        if (rec_message->RequestID != myrequestid)
            throw InternalErrorException("This should be impossible!");

        ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Client, GetLocalEndpoint(), m->ServicePath, m->MemberName,
                                                "ProcessRequest received response with requestid " << myrequestid);
    }
    catch (std::exception& exp)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(node, Client, GetLocalEndpoint(), m->ServicePath, m->MemberName,
                                                "Error during ProcessRequest: " << exp.what());
        throw;
    }

    if (rec_message->Error != MessageErrorType_None)
    {
        if (rec_message->Error == MessageErrorType_RemoteError)
        {
            RR_SHARED_PTR<RobotRaconteurException> err =
                RobotRaconteurExceptionUtil::MessageEntryToException(rec_message);
            if (!err)
                RobotRaconteurExceptionUtil::ThrowMessageEntryException(rec_message);
            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(
                node, Client, GetLocalEndpoint(), m->ServicePath, m->MemberName,
                "Service returned remote error during ProcessRequest: " << err->what());
            m_ServiceDef->DownCastAndThrowException(*err);
        }

        if (rec_message->Error == MessageErrorType_StopIteration)
        {
            ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(
                node, Client, GetLocalEndpoint(), m->ServicePath, m->MemberName,
                "Service returned error during ProcessRequest: " << rec_message->Error);
        }
        else
        {
            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(
                node, Client, GetLocalEndpoint(), m->ServicePath, m->MemberName,
                "Service returned error during ProcessRequest: " << rec_message->Error);
        }

        RobotRaconteurExceptionUtil::ThrowMessageEntryException(rec_message);
    }

    ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Client, GetLocalEndpoint(), m->ServicePath, m->MemberName,
                                            "ProcessRequest with requestid " << myrequestid << " successful");

    return rec_message;
}

void ClientContext::AsyncProcessRequest(
    const RR_INTRUSIVE_PTR<MessageEntry>& m,
    RR_MOVE_ARG(
        boost::function<void(const RR_INTRUSIVE_PTR<MessageEntry>&, const RR_SHARED_PTR<RobotRaconteurException>&)>)
        handler,
    int32_t timeout)
{
    try
    {
        if (!GetConnected())
            throw ConnectionException("Service client not connected");

        uint32_t myrequestid = 0;

        RR_SHARED_PTR<outstanding_request> t = RR_MAKE_SHARED<outstanding_request>();
        t->evt = GetNode()->CreateAutoResetEvent();
        t->handler.swap(handler);

        {
            boost::mutex::scoped_lock lock(outstanding_requests_lock);

            do
            {
                request_number++;
                if (request_number >= std::numeric_limits<uint32_t>::max())
                    request_number = 0;
                m->RequestID = request_number;
                myrequestid = request_number;
            } while (outstanding_requests.count(myrequestid) != 0);

            if (timeout != RR_TIMEOUT_INFINITE)
            {
                RR_SHARED_PTR<Timer> timer =
                    GetNode()->CreateTimer(boost::posix_time::milliseconds(timeout),
                                           boost::bind(&ClientContext::AsyncProcessRequest_timeout, shared_from_this(),
                                                       RR_BOOST_PLACEHOLDERS(_1), myrequestid),
                                           true);
                timer->Start();
                t->timer = timer;
            }

            outstanding_requests.insert(std::make_pair(myrequestid, t));

            try
            {
                if (!connection_test_timer)
                {
                    connection_test_timer = GetNode()->CreateTimer(
                        boost::posix_time::milliseconds(50),
                        boost::bind(&ClientContext::connection_test, shared_from_this(), RR_BOOST_PLACEHOLDERS(_1)));
                    connection_test_timer->Start();
                }
            }
            catch (std::exception&)
            {}
        }

        ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Client, GetLocalEndpoint(), m->ServicePath, m->MemberName,
                                                "AsyncProcessRequest sending message with requestid "
                                                    << myrequestid << " EntryType " << m->EntryType);
        // Console.WriteLine("Sent " + m.RequestID + " " + m.EntryType + " " + m.MemberName);
        boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>&)> h = boost::bind(
            &ClientContext::AsyncProcessRequest_err, shared_from_this(), RR_BOOST_PLACEHOLDERS(_1), myrequestid);
        AsyncSendMessage(m, h);
    }
    catch (std::exception& exp)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(node, Client, GetLocalEndpoint(), m->ServicePath, m->MemberName,
                                                "Error during AsyncProcessRequest: " << exp.what());
        throw;
    }
}

void ClientContext::connection_test(const TimerEvent& ev)
{
    RR_UNUSED(ev);
    try
    {
        GetNode()->CheckConnection(GetLocalEndpoint());
    }
    catch (RobotRaconteurException& exp)
    {
        typedef boost::tuple<uint32_t, boost::function<void(const RR_INTRUSIVE_PTR<MessageEntry>& ret,
                                                            const RR_SHARED_PTR<RobotRaconteurException>& error)> >
            handler_type;
        std::list<handler_type> handlers;

        {
            boost::mutex::scoped_lock lock(outstanding_requests_lock);
            typedef RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<outstanding_request> >::value_type e_type;
            BOOST_FOREACH (e_type& e, outstanding_requests)
            {
                e.second->evt->Set();

                if (e.second->handler)
                    handlers.push_back(boost::make_tuple(e.first, e.second->handler));
            }

            outstanding_requests.clear();

            if (connection_test_timer)
            {
                try
                {
                    connection_test_timer->Stop();
                }

                catch (std::exception&)
                {}
                connection_test_timer.reset();
            }
        }

        BOOST_FOREACH (handler_type& e, handlers)
        {
            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Client, GetLocalEndpoint(),
                                               "Connection lost during AsyncProcessRequest with requestid "
                                                   << e.get<0>());
            detail::InvokeHandlerWithException(node, e.get<1>(), exp);
        }
    }
}

void ClientContext::AsyncProcessRequest_err(const RR_SHARED_PTR<RobotRaconteurException>& error, uint32_t requestid)
{
    if (!error)
        return;
    try
    {
        RR_SHARED_PTR<outstanding_request> t;

        {
            boost::mutex::scoped_lock lock(outstanding_requests_lock);
            RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<outstanding_request> >::iterator e1 =
                outstanding_requests.find(requestid);
            if (e1 == outstanding_requests.end())
                return;
            t = e1->second;
            outstanding_requests.erase(e1);
        }

        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Client, GetLocalEndpoint(),
                                           "Error during AsyncProcessRequest with requestid " << requestid << ": "
                                                                                              << error->what());

        detail::InvokeHandlerWithException(node, t->handler, error);
    }
    catch (std::exception& exp)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Client, GetLocalEndpoint(),
                                           "Failed to handle error during AsyncProcessRequest: " << exp.what());
        RobotRaconteurNode::TryHandleException(node, &exp);
    }
}

void ClientContext::AsyncProcessRequest_timeout(const TimerEvent& error, uint32_t requestid)
{
    if (!error.stopped)
    {
        RR_SHARED_PTR<outstanding_request> t;
        try
        {

            {
                boost::mutex::scoped_lock lock(outstanding_requests_lock);
                RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<outstanding_request> >::iterator e1 =
                    outstanding_requests.find(requestid);
                if (e1 == outstanding_requests.end())
                    return;
                t = e1->second;
                outstanding_requests.erase(e1);
            }

            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Client, GetLocalEndpoint(),
                                               "AsyncProcessRequest with requestid " << requestid << " timed out");

            detail::InvokeHandlerWithException(node, t->handler,
                                               RR_MAKE_SHARED<RequestTimeoutException>("Request timeout"));
        }
        catch (std::exception& exp)
        {
            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Client, GetLocalEndpoint(),
                                               "Failed to handle error during AsyncProcessRequest: " << exp.what());
            RobotRaconteurNode::TryHandleException(node, &exp);
        }
    }
}

void ClientContext::SendMessage(const RR_INTRUSIVE_PTR<MessageEntry>& m)
{
    // m.ServiceName = ServiceName;

    if (!GetConnected())
        throw ConnectionException("Client has been disconnected");
    /*boost::shared_lock<boost::shared_mutex> lock(message_lock);

    if (!GetConnected())
    throw ConnectionException("Client has been disconnected");*/

    RR_INTRUSIVE_PTR<Message> mm = CreateMessage();
    mm->header = CreateMessageHeader();

    mm->entries.push_back(m);

    std::vector<std::string> v;
    boost::string_ref metadata1 = m->MetaData.str();
    boost::split(v, metadata1, boost::is_from_range('\n', '\n'), boost::algorithm::token_compress_on);

    if (std::find(v.begin(), v.end(), "unreliable") != v.end())
    {
        mm->header->MetaData = "unreliable\n";
    }

    // LastMessageSentTime = GetNode()->NowNodeTime();

    Endpoint::SendMessage(mm);
}

void ClientContext::AsyncSendMessage(
    const RR_INTRUSIVE_PTR<MessageEntry>& m,
    const boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>&)>& callback)
{
    // m.ServiceName = ServiceName;

    if (!GetConnected())
        throw ConnectionException("Client has been disconnected");
    /*boost::shared_lock<boost::shared_mutex> lock(message_lock);

    if (!GetConnected())
    throw ConnectionException("Client has been disconnected");*/

    RR_INTRUSIVE_PTR<Message> mm = CreateMessage();
    mm->header = CreateMessageHeader();

    mm->entries.push_back(m);

    // LastMessageSentTime = GetNode()->NowNodeTime();

    std::vector<std::string> v;
    boost::string_ref metadata1 = m->MetaData.str();
    boost::split(v, metadata1, boost::is_from_range('\n', '\n'), boost::algorithm::token_compress_on);

    if (std::find(v.begin(), v.end(), "unreliable") != v.end())
    {
        mm->header->MetaData = "unreliable\n";
    }

    Endpoint::AsyncSendMessage(mm, callback);
}

void ClientContext_empty_handler() {}

void ClientContext::MessageReceived(const RR_INTRUSIVE_PTR<Message>& m)
{

    if (!GetConnected())
        return;

    SetLastMessageReceivedTime(GetNode()->NowNodeTime());

    if (!m->entries.empty())
    {
        if (m->entries[0]->EntryType == MessageEntryType_ConnectClientRet ||
            m->entries[0]->EntryType == MessageEntryType_ConnectClientCombinedRet)
        {
            ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Client, GetLocalEndpoint(),
                                               "Client received connect client response RemoteNodeID: "
                                                   << m->header->SenderNodeID.ToString()
                                                   << " RemoteNodeName: " << m->header->SenderNodeName
                                                   << " RemoteEndpoint: " << m->header->SenderEndpoint);

            SetRemoteEndpoint(m->header->SenderEndpoint);
            SetRemoteNodeID(m->header->SenderNodeID);
            SetRemoteNodeName(m->header->SenderNodeName.str());
        }

        if (m->entries[0]->EntryType == MessageEntryType_EndpointCheckCapability)
        {
            CheckEndpointCapabilityMessage(m);
            return;
        }
    }

    BOOST_FOREACH (RR_INTRUSIVE_PTR<MessageEntry>& mm, m->entries)
    {
        if (mm->Error == MessageErrorType_InvalidEndpoint)
        {
            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Client, GetLocalEndpoint(),
                                               "Client received InvalidEndpoint error, assume session lost and close");
            AsyncClose(&ClientContext_empty_handler);
            return;
        }
        MessageEntryReceived(mm);
    }
}

void DispatchEvent_func(RR_WEAK_PTR<RobotRaconteurNode> node, uint32_t ep, const RR_SHARED_PTR<ServiceStub>& stub,
                        const RR_INTRUSIVE_PTR<MessageEntry>& m)
{

    try
    {
        stub->DispatchEvent(m);
    }
    catch (std::exception& exp)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(node, Client, ep, m->ServicePath, m->MemberName,
                                                "DispatchEvent failed: " << exp.what());
        RobotRaconteurNode::TryHandleException(node, &exp);
    }
}

void ClientContext::MessageEntryReceived(const RR_INTRUSIVE_PTR<MessageEntry>& m)
{

    ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Client, GetLocalEndpoint(), m->ServicePath, m->MemberName,
                                            "Client received message entry EntryType " << m->EntryType << " Error "
                                                                                       << m->Error);

    {
        // boost::recursive_mutex::scoped_lock lock (rec_loc);
        if (m->EntryType == MessageEntryType_EventReq)
        {
            RR_SHARED_PTR<ServiceStub> stub;
            {
                boost::mutex::scoped_lock lock(stubs_lock);
                RR_UNORDERED_MAP<MessageStringPtr, RR_SHARED_PTR<ServiceStub> >::iterator e1 =
                    stubs.find(m->ServicePath.str());
                if (e1 == stubs.end())
                    return;
                stub = e1->second;
            }
            if (stub)
            {
                RobotRaconteurNode::TryPostToThreadPool(
                    node, boost::bind(&DispatchEvent_func, node, GetLocalEndpoint(), stub, m));
            }
            else
            {
                ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Client, GetLocalEndpoint(), m->ServicePath, m->MemberName,
                                                        "Client received event for nonexistant service path");
            }
            // stub.DispatchEvent(m);
        }
        else if (m->EntryType == MessageEntryType_PropertyGetRes || m->EntryType == MessageEntryType_PropertySetRes ||
                 m->EntryType == MessageEntryType_FunctionCallRes ||
                 m->EntryType == MessageEntryType_ObjectTypeNameRet ||
                 m->EntryType == MessageEntryType_ConnectClientRet ||
                 m->EntryType == MessageEntryType_ConnectClientCombinedRet ||
                 m->EntryType == MessageEntryType_DisconnectClientRet ||
                 m->EntryType == MessageEntryType_GetServiceDescRet ||
                 (m->EntryType >= MessageEntryType_PipeConnectReq &&
                  m->EntryType <= MessageEntryType_PipeDisconnectRet) ||
                 m->EntryType == MessageEntryType_ClientSessionOpRet ||
                 m->EntryType == MessageEntryType_WireConnectRet ||
                 m->EntryType == MessageEntryType_WireDisconnectRet ||
                 m->EntryType == MessageEntryType_WirePeekInValueRet ||
                 m->EntryType == MessageEntryType_WirePeekOutValueRet ||
                 m->EntryType == MessageEntryType_WirePokeOutValueRet ||
                 m->EntryType == MessageEntryType_MemoryReadRet || m->EntryType == MessageEntryType_MemoryWriteRet ||
                 m->EntryType == MessageEntryType_MemoryGetParamRet ||
                 m->EntryType == MessageEntryType_GeneratorNextRes)
        {
            // Console.WriteLine("Got " + m.RequestID + " " + m.EntryType + " " + m.MemberName);
            RR_SHARED_PTR<outstanding_request> t;
            uint32_t requestid = m->RequestID;
            try
            {
                boost::mutex::scoped_lock lock(outstanding_requests_lock);
                RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<outstanding_request> >::iterator e1 =
                    outstanding_requests.find(requestid);
                if (e1 == outstanding_requests.end())
                    return;
                t = e1->second;
                t->ret = m;
                try
                {
                    if (t->timer)
                        t->timer->Stop();
                }
                catch (std::exception&)
                {}

                t->evt->Set();
            }
            catch (std::exception&)
            {
                return;
            }

            if (t)
            {
                try
                {
                    if (t->handler)
                    {

                        {
                            boost::mutex::scoped_lock lock(outstanding_requests_lock);
                            outstanding_requests.erase(requestid);

                            try
                            {
                                if (connection_test_timer)
                                {
                                    bool async_running = false;
                                    BOOST_FOREACH (RR_SHARED_PTR<outstanding_request>& ee,
                                                   outstanding_requests | boost::adaptors::map_values)
                                    {
                                        if (ee->handler)
                                            async_running = true;
                                    }

                                    if (!async_running)
                                    {
                                        try
                                        {
                                            connection_test_timer->Stop();
                                        }
                                        catch (std::exception&)
                                        {}

                                        connection_test_timer.reset();
                                    }
                                }
                            }
                            catch (std::exception&)
                            {}
                        }

                        if (m->Error == MessageErrorType_None)
                        {
                            ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(
                                node, Client, GetLocalEndpoint(), m->ServicePath, m->MemberName,
                                "AsyncProcessRequest with requestid " << requestid << " successful");
                            t->handler(m, RR_SHARED_PTR<RobotRaconteurException>());
                        }
                        else if (m->Error == MessageErrorType_RemoteError)
                        {
                            RR_SHARED_PTR<RobotRaconteurException> err =
                                RobotRaconteurExceptionUtil::MessageEntryToException(m);
                            RR_SHARED_PTR<RobotRaconteurException> err2 = m_ServiceDef->DownCastException(err);
                            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(
                                node, Client, GetLocalEndpoint(), m->ServicePath, m->MemberName,
                                "Service returned remote error during ProcessRequest: " << err->what());
                            t->handler(RR_INTRUSIVE_PTR<MessageEntry>(), err2);
                        }
                        else
                        {
                            if (m->Error == MessageErrorType_StopIteration)
                            {
                                ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(
                                    node, Client, GetLocalEndpoint(), m->ServicePath, m->MemberName,
                                    "Service returned error during ProcessRequest: " << m->Error);
                            }
                            else
                            {
                                ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(
                                    node, Client, GetLocalEndpoint(), m->ServicePath, m->MemberName,
                                    "Service returned error during ProcessRequest: " << m->Error);
                            }
                            t->handler(RR_INTRUSIVE_PTR<MessageEntry>(),
                                       RobotRaconteurExceptionUtil::MessageEntryToException(m));
                        }
                    }
                }
                catch (std::exception& exp)
                {
                    ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(
                        node, Client, GetLocalEndpoint(), m->ServicePath, m->MemberName,
                        "Error handling AsyncProcessRequest response: " << exp.what());
                    RobotRaconteurNode::TryHandleException(node, &exp);
                }
            }
        }
        else if (m->EntryType == MessageEntryType_ServiceClosed)
        {
            ROBOTRACONTEUR_LOG_INFO_COMPONENT(node, Client, GetLocalEndpoint(),
                                              "Client received ServiceClosed, disconnecting");

            try
            {
                AsyncClose(&ClientContext_empty_handler);
            }
            catch (std::exception&)
            {
                // rec_loc.lock();

                throw;
            }

            // rec_loc.lock();
        }
        else if (m->EntryType == MessageEntryType_ClientKeepAliveRet)
        {}
        else if (m->EntryType == MessageEntryType_PipePacket || m->EntryType == MessageEntryType_PipeClosed ||
                 m->EntryType == MessageEntryType_PipePacketRet)
        {

            ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Client, GetLocalEndpoint(), m->ServicePath, m->MemberName,
                                                    "Received pipe packet entry type " << m->EntryType);

            RR_SHARED_PTR<ServiceStub> stub;
            {
                boost::mutex::scoped_lock lock(stubs_lock);
                RR_UNORDERED_MAP<MessageStringPtr, RR_SHARED_PTR<ServiceStub> >::iterator e1 =
                    stubs.find(m->ServicePath);
                if (e1 == stubs.end())
                    return;
                stub = e1->second;
            }
            if (stub)
            {
                stub->DispatchPipeMessage(m);
            }
            else
            {
                ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Client, GetLocalEndpoint(), m->ServicePath, m->MemberName,
                                                        "Client received pipe packet for nonexistant service path");
            }
            return;
        }
        else if (m->EntryType == MessageEntryType_WirePacket || m->EntryType == MessageEntryType_WireClosed)
        {
            ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Client, GetLocalEndpoint(), m->ServicePath, m->MemberName,
                                                    "Received wire packet entry type " << m->EntryType);

            RR_SHARED_PTR<ServiceStub> stub;
            {
                boost::mutex::scoped_lock lock(stubs_lock);
                RR_UNORDERED_MAP<MessageStringPtr, RR_SHARED_PTR<ServiceStub> >::iterator e1 =
                    stubs.find(m->ServicePath);
                if (e1 == stubs.end())
                    return;
                stub = e1->second;
            }
            if (stub)
            {
                stub->DispatchWireMessage(m);
            }
            else
            {
                ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Client, GetLocalEndpoint(), m->ServicePath, m->MemberName,
                                                        "Client received wire packet for nonexistant service path");
            }
        }
        else if (m->EntryType == MessageEntryType_ServicePathReleasedReq)
        {
            ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Client, GetLocalEndpoint(), m->ServicePath, m->MemberName,
                                                    "Service path released by service, destroying stubs");

            boost::mutex::scoped_lock lock(stubs_lock);
            MessageStringPtr& path = m->ServicePath;
            std::vector<std::string> objkeys;
            BOOST_FOREACH (const MessageStringPtr& ee, stubs | boost::adaptors::map_keys)
            {
                std::string path_str2 = path.str().to_string() + ".";
                if (path == ee || boost::starts_with(ee.str(), path_str2))
                {
                    objkeys.push_back(RR_MOVE(ee.str().to_string()));
                }
            }

            //= stubs.Keys->Where(x => (x->Length >= path.length()) && (x->substr(0, path.length()) ==
            // path))->ToArray(); if (objkeys.Count() == 0) throw new ServiceException("Unknown service path");

            BOOST_FOREACH (std::string& path1, objkeys)
            {
                try
                {
                    RR_UNORDERED_MAP<MessageStringPtr, RR_SHARED_PTR<ServiceStub> >::iterator e1 = stubs.find(path1);
                    if (e1 == stubs.end())
                        continue;
                    e1->second->RRClose();
                    stubs.erase(e1);
                }
                catch (std::exception&)
                {}
            }

            try
            {
                RR_SHARED_PTR<std::string> service_path_sp = RR_MAKE_SHARED<std::string>(path.str().to_string());
                ClientServiceListener(shared_from_this(), ClientServiceListenerEventType_ServicePathReleased,
                                      service_path_sp);
            }
            catch (std::exception& exp)
            {
                ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Client, GetLocalEndpoint(),
                                                   "Client listener callback raised exception: " << exp.what());
                RobotRaconteurNode::TryHandleException(node, &exp);
            }
        }
        else if (m->EntryType == MessageEntryType_CallbackCallReq)
        {
            ProcessCallbackCall(m);
        }
        else
        {
            ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Client, GetLocalEndpoint(), m->ServicePath, m->MemberName,
                                                    "Client received unkown EntryType: " << m->EntryType);
            throw ServiceException("Unknown service command");
        }
    }
}

/*RR_INTRUSIVE_PTR<MessageElementNestedElementList> ClientContext::PackStructure(const RR_SHARED_PTR<void> &s)
{
return GetServiceDef()->PackStructure(s);
;
}

template<typename T>
T ClientContext::UnpackStructure(const RR_INTRUSIVE_PTR<MessageElementNestedElementList> &l)
{
return GetServiceDef()->UnpackStructure<T>(l);
}*/

std::string ClientContext::GetServiceName() const { return m_ServiceName; }

bool ClientContext::GetConnected() const { return m_Connected; }

RR_SHARED_PTR<RRObject> ClientContext::ConnectService(const RR_SHARED_PTR<Transport>& c, boost::string_ref url,
                                                      boost::string_ref username,
                                                      const RR_INTRUSIVE_PTR<RRMap<std::string, RRValue> >& credentials,
                                                      boost::string_ref objecttype)
{
    ROBOTRACONTEUR_ASSERT_MULTITHREADED(node);

    RR_SHARED_PTR<detail::sync_async_handler<RRObject> > d = RR_MAKE_SHARED<detail::sync_async_handler<RRObject> >(
        RR_MAKE_SHARED<ConnectionException>("Connection timed out"));
    AsyncConnectService(c, url, username, credentials, objecttype,
                        boost::bind(&detail::sync_async_handler<RRObject>::operator(), d, RR_BOOST_PLACEHOLDERS(_1),
                                    RR_BOOST_PLACEHOLDERS(_2)),
                        boost::numeric_cast<int32_t>(GetNode()->GetRequestTimeout()));
    return d->end();
}

RR_SHARED_PTR<RRObject> ClientContext::ConnectService(const RR_SHARED_PTR<Transport>& c,
                                                      const RR_SHARED_PTR<ITransportConnection>& tc,
                                                      boost::string_ref url, boost::string_ref username,
                                                      const RR_INTRUSIVE_PTR<RRMap<std::string, RRValue> >& credentials,
                                                      boost::string_ref objecttype)
{
    ROBOTRACONTEUR_ASSERT_MULTITHREADED(node);

    RR_SHARED_PTR<detail::sync_async_handler<RRObject> > d = RR_MAKE_SHARED<detail::sync_async_handler<RRObject> >(
        RR_MAKE_SHARED<ConnectionException>("Connection timed out"));
    AsyncConnectService(c, tc, url, username, credentials, objecttype,
                        boost::bind(&detail::sync_async_handler<RRObject>::operator(), d, RR_BOOST_PLACEHOLDERS(_1),
                                    RR_BOOST_PLACEHOLDERS(_2)),
                        boost::numeric_cast<int32_t>(GetNode()->GetRequestTimeout()));
    return d->end();
}

void ClientContext::AsyncConnectService(
    const RR_SHARED_PTR<Transport>& c, boost::string_ref url, boost::string_ref username,
    const RR_INTRUSIVE_PTR<RRMap<std::string, RRValue> >& credentials, boost::string_ref objecttype,
    RR_MOVE_ARG(boost::function<void(const RR_SHARED_PTR<RRObject>&, const RR_SHARED_PTR<RobotRaconteurException>&)>)
        handler,
    int32_t timeout)
{
    try
    {
        boost::recursive_mutex::scoped_lock cl_lock(connect_lock);

        ParseConnectionURLResult url_res = ParseConnectionURL(url);

        m_ServiceName = url_res.service;

        if (!c->CanConnectService(url))
        {
            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Client, GetLocalEndpoint(),
                                               "Invalid transport provided to AsyncConnectService");
            throw ServiceException("Invalid transport");
        }

        RR_SHARED_PTR<detail::async_timeout_wrapper<RRObject> > d =
            RR_MAKE_SHARED<detail::async_timeout_wrapper<RRObject> >(
                GetNode(), handler,
                boost::bind(&detail::async_timeout_wrapper_closer<RRObject, ClientContext>, RR_BOOST_PLACEHOLDERS(_1)));

        boost::function<void(const RR_SHARED_PTR<RRObject>&, const RR_SHARED_PTR<RobotRaconteurException>&)> h1(
            boost::bind(&detail::async_timeout_wrapper<RRObject>::operator(), d, RR_BOOST_PLACEHOLDERS(_1),
                        RR_BOOST_PLACEHOLDERS(_2)));
        boost::function<void(const RR_SHARED_PTR<ITransportConnection>&, const RR_SHARED_PTR<RobotRaconteurException>&)>
            h(boost::bind(&ClientContext::AsyncConnectService1, shared_from_this(), c, RR_BOOST_PLACEHOLDERS(_1),
                          RR_BOOST_PLACEHOLDERS(_2), url.to_string(), username.to_string(), credentials,
                          objecttype.to_string(), h1));

        c->AsyncCreateTransportConnection(url, RR_STATIC_POINTER_CAST<Endpoint>(shared_from_this()), h);
        d->start_timer(timeout, RR_MAKE_SHARED<ConnectionException>("Timeout during operation"));
    }
    catch (std::exception& exp)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Client, GetLocalEndpoint(),
                                           "AsyncConnectService failed: " << exp.what());
        throw;
    }
}

void ClientContext::AsyncConnectService1(
    const RR_SHARED_PTR<Transport>& c, const RR_SHARED_PTR<ITransportConnection>& tc,
    const RR_SHARED_PTR<RobotRaconteurException>& e, const std::string& url, const std::string& username,
    const RR_INTRUSIVE_PTR<RRMap<std::string, RRValue> >& credentials, const std::string& objecttype,
    boost::function<void(const RR_SHARED_PTR<RRObject>&, const RR_SHARED_PTR<RobotRaconteurException>&)>& handler)
{
    boost::recursive_mutex::scoped_lock cl_lock(connect_lock);
    if (e)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Client, GetLocalEndpoint(),
                                           "AsyncConnectService failed: " << e->what());
        detail::InvokeHandlerWithException(node, handler, e);

        AsyncClose(&ClientContext_empty_handler);
        return;
    }
    else
    {
        try
        {
            AsyncConnectService(c, tc, std::string(url), std::string(username), credentials, objecttype,
                                RR_MOVE(handler), 0);
        }

        catch (std::exception& err)
        {
            detail::InvokeHandlerWithException(node, handler, err, MessageErrorType_ServiceError);
            AsyncClose(&ClientContext_empty_handler);
        }
    }
}

void ClientContext::AsyncConnectService(
    const RR_SHARED_PTR<Transport>& c, const RR_SHARED_PTR<ITransportConnection>& tc, boost::string_ref url,
    boost::string_ref username, const RR_INTRUSIVE_PTR<RRMap<std::string, RRValue> >& credentials,
    boost::string_ref objecttype,
    RR_MOVE_ARG(boost::function<void(const RR_SHARED_PTR<RRObject>&, const RR_SHARED_PTR<RobotRaconteurException>&)>)
        handler,
    int32_t timeout)
{
    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Client, GetLocalEndpoint(), "Begin AsyncConnectService with url: " << url);

    boost::recursive_mutex::scoped_lock cl_lock(connect_lock);

    try
    {
        this->connecttransport = c;
        this->connecturl = url.to_string();

        bool use_message4_ = tc->CheckCapabilityActive(TransportCapabilityCode_MESSAGE4_BASIC_PAGE |
                                                       TransportCapabilityCode_MESSAGE4_BASIC_ENABLE);

        bool use_combined_connection_ = false;

        if (!use_message4_)
        {
            use_combined_connection_ = tc->CheckCapabilityActive(
                TransportCapabilityCode_MESSAGE2_BASIC_PAGE | TransportCapabilityCode_MESSAGE2_BASIC_CONNECTCOMBINED);
        }
        else
        {
            use_combined_connection_ = tc->CheckCapabilityActive(
                TransportCapabilityCode_MESSAGE4_BASIC_PAGE | TransportCapabilityCode_MESSAGE4_BASIC_CONNECTCOMBINED);
        }

        use_combined_connection.store(use_combined_connection_);
        ParseConnectionURLResult url_res = ParseConnectionURL(url);

        m_ServiceName = url_res.service;

        if (!(url_res.nodeid.IsAnyNode() && !url_res.nodename.empty()))
        {
            // RR_SHARED_PTR<NodeID> remid = RR_MAKE_SHARED<NodeID>(s[0]);
            SetRemoteNodeID(url_res.nodeid);
        }
        else
        {
            SetRemoteNodeName(url_res.nodename);
        }

        TransportConnection = tc;

        SetTransport(c->TransportID);
        SetTransportConnection(tc);
        SetRemoteEndpoint(0);

        m_Connected = true;

        // std::cout << "AsyncConnectService" << std::endl;

        try
        {
            ClientServiceListener(shared_from_this(), ClientServiceListenerEventType_TransportConnectionConnected,
                                  RR_SHARED_PTR<RRObject>());
        }
        catch (std::exception& exp)
        {
            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Client, GetLocalEndpoint(),
                                               "Client listener callback raised exception: " << exp.what());
            RobotRaconteurNode::TryHandleException(node, &exp);
        }

        if (use_combined_connection.load())
        {
            ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Client, GetLocalEndpoint(),
                                               "AsyncConnectService using ConnectClientCombined");

            RR_INTRUSIVE_PTR<MessageEntry> m = CreateMessageEntry();
            m->ServicePath = GetServiceName();
            // m->MemberName = "connectclientcombined";
            m->EntryType = MessageEntryType_ConnectClientCombined;

            m->AddElement("clientversion", stringToRRArray(ROBOTRACONTEUR_VERSION_TEXT));
            m->AddElement("returnservicedefs", stringToRRArray("true"));
            if (!username.empty())
            {
                m->AddElement("username", stringToRRArray(username));
            }

            if (credentials)
            {
                m->AddElement("credentials", GetNode()->PackMapType<std::string, RRValue>(credentials));
            }

            AsyncProcessRequest(m,
                                boost::bind(&ClientContext::AsyncConnectService7, shared_from_this(),
                                            RR_BOOST_PLACEHOLDERS(_1), RR_BOOST_PLACEHOLDERS(_2),
                                            objecttype.to_string(), username.to_string(), handler),
                                timeout);

            return;
        }

        ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Client, GetLocalEndpoint(),
                                           "AsyncConnectService not using ConnectClientCombined");

        if (timeout == RR_TIMEOUT_INFINITE)
        {
            AsyncPullServiceDefinitionAndImports("",
                                                 boost::bind(&ClientContext::AsyncConnectService2, shared_from_this(),
                                                             RR_BOOST_PLACEHOLDERS(_1), RR_BOOST_PLACEHOLDERS(_2),
                                                             username.to_string(), credentials, objecttype.to_string(),
                                                             handler),
                                                 boost::numeric_cast<int32_t>(GetNode()->GetRequestTimeout()));
        }
        else
        {

            RR_SHARED_PTR<detail::async_timeout_wrapper<RRObject> > d =
                RR_MAKE_SHARED<detail::async_timeout_wrapper<RRObject> >(
                    GetNode(), handler,
                    boost::bind(&detail::async_timeout_wrapper_closer<RRObject, ClientContext>,
                                RR_BOOST_PLACEHOLDERS(_1)));
            boost::function<void(const RR_SHARED_PTR<RRObject>&, const RR_SHARED_PTR<RobotRaconteurException>&)> h1(
                boost::bind(&detail::async_timeout_wrapper<RRObject>::operator(), d, RR_BOOST_PLACEHOLDERS(_1),
                            RR_BOOST_PLACEHOLDERS(_2)));
            AsyncPullServiceDefinitionAndImports(
                "",
                boost::bind(&ClientContext::AsyncConnectService2, shared_from_this(), RR_BOOST_PLACEHOLDERS(_1),
                            RR_BOOST_PLACEHOLDERS(_2), username.to_string(), credentials, objecttype.to_string(), h1),
                boost::numeric_cast<int32_t>(GetNode()->GetRequestTimeout()));
            d->start_timer(timeout, RR_MAKE_SHARED<ConnectionException>("Timeout during operation"));
        }
    }
    catch (std::exception& exp)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Client, GetLocalEndpoint(),
                                           "AsyncConnectService failed: " << exp.what());
        throw;
    }
}

void ClientContext::AsyncConnectService2(
    const RR_SHARED_PTR<PullServiceDefinitionAndImportsReturn>& d, const RR_SHARED_PTR<RobotRaconteurException>& e,
    const std::string& username, const RR_INTRUSIVE_PTR<RRMap<std::string, RRValue> >& credentials,
    const std::string& objecttype,
    boost::function<void(const RR_SHARED_PTR<RRObject>&, const RR_SHARED_PTR<RobotRaconteurException>&)>& handler)
{
    boost::recursive_mutex::scoped_lock cl_lock(connect_lock);
    // std::cout << "AsyncConnectService2" << std::endl;
    if (e)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Client, GetLocalEndpoint(),
                                           "AsyncConnectService failed: " << e->what());
        detail::InvokeHandlerWithException(node, handler, e);

        AsyncClose(&ClientContext_empty_handler);
        return;
    }
    else
    {
        try
        {
            {
                boost::mutex::scoped_lock lock(pulled_service_defs_lock);
                BOOST_FOREACH (RR_SHARED_PTR<ServiceDefinition>& e, d->defs)
                {
                    if (pulled_service_defs.find(e->Name) == pulled_service_defs.end())
                    {
                        pulled_service_defs.insert(std::make_pair(e->Name, e));
                    }
                }
            }

            if (d->attributes)
            {
                boost::mutex::scoped_lock lock(m_Attributes_lock);
                m_Attributes.swap(d->attributes->GetStorageContainer());
            }

            // Determine the type of the root object

            RR_INTRUSIVE_PTR<MessageEntry> e = CreateMessageEntry(MessageEntryType_ObjectTypeName, "");
            // e.AddElement("servicepath", ServiceName);
            e->ServicePath = GetServiceName();
            e->AddElement("clientversion", stringToRRArray(ROBOTRACONTEUR_VERSION_TEXT));

            // std::cout << "AsyncConnectService2_1" << std::endl;

            ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Client, GetLocalEndpoint(),
                                               "AsyncConnectService pulled service types");

            AsyncProcessRequest(e,
                                boost::bind(&ClientContext::AsyncConnectService3, shared_from_this(),
                                            RR_BOOST_PLACEHOLDERS(_1), RR_BOOST_PLACEHOLDERS(_2), username, credentials,
                                            std::string(objecttype), d, handler),
                                boost::numeric_cast<int32_t>(GetNode()->GetRequestTimeout()));
        }
        catch (std::exception& err)
        {
            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Client, GetLocalEndpoint(),
                                               "AsyncConnectService failed: " << err.what());
            detail::InvokeHandlerWithException(node, handler, err, MessageErrorType_ServiceError);

            AsyncClose(&ClientContext_empty_handler);
        }
    }
}

void ClientContext::AsyncConnectService3(
    const RR_INTRUSIVE_PTR<MessageEntry>& ret, const RR_SHARED_PTR<RobotRaconteurException>& e,
    const std::string& username, const RR_INTRUSIVE_PTR<RRMap<std::string, RRValue> >& credentials,
    const std::string& objecttype, const RR_SHARED_PTR<PullServiceDefinitionAndImportsReturn>& d,
    const boost::function<void(const RR_SHARED_PTR<RRObject>&, const RR_SHARED_PTR<RobotRaconteurException>&)>& handler)
{
    boost::recursive_mutex::scoped_lock cl_lock(connect_lock);
    // std::cout << "AsyncConnectService3" << std::endl;
    if (e)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Client, GetLocalEndpoint(),
                                           "AsyncConnectService failed: " << e->what());
        detail::InvokeHandlerWithException(node, handler, e);
        AsyncClose(&ClientContext_empty_handler);
        return;
    }
    else
    {
        try
        {

            std::string type = ret->FindElement("objecttype")->CastDataToString();
            if (type.empty())
                throw ObjectNotFoundException("Could not find object type");
            ;

            // If we want a desired type, try to figure out if the upcast is valid
            if (!objecttype.empty() && type != objecttype)
            {
                RR_INTRUSIVE_PTR<MessageElement> objectimplements_m;
                if (ret->TryFindElement("objectimplements", objectimplements_m))
                {
                    std::vector<std::string> objectimplements =
                        RRListToStringVector(GetNode()->UnpackListType<RRArray<char> >(
                            objectimplements_m->CastDataToNestedList(DataTypes_list_t)));
                    if (boost::range::find(objectimplements, objecttype) != objectimplements.end())
                    {
                        type = objecttype;
                        AsyncConnectService4(d, RR_SHARED_PTR<RobotRaconteurException>(), username, credentials,
                                             objecttype, type, (handler));
                    }
                }

                VerifyObjectImplements(type, objecttype);
                type = objecttype;

                // The type has already been pulled by now, no need to try again
                // AsyncPullServiceDefinitionAndImports(SplitQualifiedName(type).get<0>(),
                // boost::bind(&ClientContext::AsyncConnectService4, shared_from_this(), RR_BOOST_PLACEHOLDERS(_1),
                // RR_BOOST_PLACEHOLDERS(_2), username, credentials, objecttype, type, handler),
                // GetNode()->GetRequestTimeout()); return;
            }

            ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Client, GetLocalEndpoint(),
                                               "AsyncConnectService retrieved root object type: " << type);

            // std::cout << "AsyncConnectService3_1" << std::endl;
            AsyncConnectService4(d, RR_SHARED_PTR<RobotRaconteurException>(), username, credentials, objecttype, type,
                                 (handler));
        }
        catch (RobotRaconteurException& err)
        {
            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Client, GetLocalEndpoint(),
                                               "AsyncConnectService failed: " << err.what());
            detail::InvokeHandlerWithException(node, handler, err);

            AsyncClose(&ClientContext_empty_handler);
        }
        catch (std::exception& err)
        {
            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Client, GetLocalEndpoint(),
                                               "AsyncConnectService failed: " << err.what());
            detail::InvokeHandlerWithException(node, handler, err, MessageErrorType_ServiceError);
            AsyncClose(&ClientContext_empty_handler);
        }
    }
}

void ClientContext::AsyncConnectService4(
    const RR_SHARED_PTR<PullServiceDefinitionAndImportsReturn>& d, const RR_SHARED_PTR<RobotRaconteurException>& e,
    const std::string& username, const RR_INTRUSIVE_PTR<RRMap<std::string, RRValue> >& credentials,
    const std::string& objecttype, const std::string& type,
    const boost::function<void(const RR_SHARED_PTR<RRObject>&, const RR_SHARED_PTR<RobotRaconteurException>&)>& handler)
{
    boost::recursive_mutex::scoped_lock cl_lock(connect_lock);
    // std::cout << "AsyncConnectService4" << std::endl;
    if (e)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Client, GetLocalEndpoint(),
                                           "AsyncConnectService failed: " << e->what());
        detail::InvokeHandlerWithException(node, handler, e);
        AsyncClose(&ClientContext_empty_handler);
        return;
    }
    else
    {
        try
        {
            if (GetNode()->GetDynamicServiceFactory())
            {
                use_pulled_types = true;

                std::vector<RR_SHARED_PTR<ServiceDefinition> > missingdefs =
                    std::vector<RR_SHARED_PTR<ServiceDefinition> >();

                BOOST_FOREACH (RR_SHARED_PTR<RobotRaconteur::ServiceDefinition>& di, d->defs)
                {

                    std::vector<std::string> stypes = GetPulledServiceTypes();
                    if (std::find(stypes.begin(), stypes.end(), (di->Name)) == stypes.end())
                    {
                        missingdefs.push_back(di);
                    }
                }
                if (!missingdefs.empty())
                {
                    std::vector<std::string> missingdefs_vec;
                    std::vector<std::string> missingdefs_names;
                    BOOST_FOREACH (RR_SHARED_PTR<ServiceDefinition>& e, missingdefs)
                    {
                        missingdefs_names.push_back(e->Name);
                        missingdefs_vec.push_back(e->ToString());
                    }

                    ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Client, GetLocalEndpoint(),
                                                       "AsyncConnectService using dynamic service factory for types: "
                                                           << boost::join(missingdefs_names, ", "));

                    std::vector<RR_SHARED_PTR<ServiceFactory> > di2 =
                        GetNode()->GetDynamicServiceFactory()->CreateServiceFactories(missingdefs_vec);

                    for (size_t i = 0; i < missingdefs.size(); i++)
                    {
                        boost::mutex::scoped_lock lock(pulled_service_types_lock);
                        pulled_service_types.insert(std::make_pair(di2.at(i)->GetServiceName(), di2.at(i)));
                    }
                }

                m_ServiceDef = GetPulledServiceType(d->defs.at(0)->Name);
            }
            else
            {
                try
                {

                    m_ServiceDef = GetNode()->GetServiceType(d->defs.at(0)->Name);
                }
                catch (std::exception&)
                {
                    throw ServiceException("Could not find correct service factory for remote service");
                }
            }

            ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Client, GetLocalEndpoint(),
                                               "AsyncConnectService retrieved additional service types");

            // std::cout << "AsyncConnectService4_1" << std::endl;
            RR_INTRUSIVE_PTR<MessageEntry> e2 = CreateMessageEntry();
            e2->ServicePath = GetServiceName();
            // e2->MemberName = "registerclient";
            e2->EntryType = MessageEntryType_ConnectClient;
            AsyncProcessRequest(e2,
                                boost::bind(&ClientContext::AsyncConnectService5, shared_from_this(),
                                            RR_BOOST_PLACEHOLDERS(_1), RR_BOOST_PLACEHOLDERS(_2), username, credentials,
                                            objecttype, type, d, handler),
                                boost::numeric_cast<int32_t>(GetNode()->GetRequestTimeout()));
        }
        catch (std::exception& err)
        {
            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Client, GetLocalEndpoint(),
                                               "AsyncConnectService failed: " << err.what());
            detail::InvokeHandlerWithException(node, handler, err, MessageErrorType_ServiceError);
            AsyncClose(&ClientContext_empty_handler);
        }
    }
}

void ClientContext::AsyncConnectService5(
    const RR_INTRUSIVE_PTR<MessageEntry>& ret, const RR_SHARED_PTR<RobotRaconteurException>& e,
    const std::string& username, const RR_INTRUSIVE_PTR<RRMap<std::string, RRValue> >& credentials,
    const std::string& objecttype, const std::string& type,
    const RR_SHARED_PTR<PullServiceDefinitionAndImportsReturn>& d,
    const boost::function<void(const RR_SHARED_PTR<RRObject>&, const RR_SHARED_PTR<RobotRaconteurException>&)>& handler)
{
    RR_UNUSED(ret);
    RR_UNUSED(objecttype);
    boost::recursive_mutex::scoped_lock cl_lock(connect_lock);
    // std::cout << "AsyncConnectService5" << std::endl;
    if (e)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Client, GetLocalEndpoint(),
                                           "AsyncConnectService failed: " << e->what());
        detail::InvokeHandlerWithException(node, handler, e);
        AsyncClose(&ClientContext_empty_handler);
        return;
    }
    else
    {
        try
        {

            if (!username.empty())
            {
                ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Client, GetLocalEndpoint(),
                                                   "AsyncConnectService authenticating with username " << username);

                try
                {
                    // std::cout << "AsyncConnectService5_1" << std::endl;
                    AsyncAuthenticateUser(username, credentials,
                                          boost::bind(&ClientContext::AsyncConnectService6, shared_from_this(),
                                                      RR_BOOST_PLACEHOLDERS(_1), RR_BOOST_PLACEHOLDERS(_2), type,
                                                      username, d, handler),
                                          boost::numeric_cast<int32_t>(GetNode()->GetRequestTimeout()));
                }
                catch (AuthenticationException& ex)
                {
                    try
                    {
                        AsyncClose(&ClientContext_empty_handler);
                    }
                    catch (std::exception&)
                    {}
                    throw;
                }
            }
            else
            {
                // std::cout << "AsyncConnectService5_1" << std::endl;
                AsyncConnectService6(RR_MAKE_SHARED<std::string>("OK"), RR_SHARED_PTR<RobotRaconteurException>(), type,
                                     username, d, (handler));
            }
        }
        catch (std::exception& err)
        {
            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Client, GetLocalEndpoint(),
                                               "AsyncConnectService failed: " << e->what());
            detail::InvokeHandlerWithException(node, handler, err, MessageErrorType_ServiceError);

            AsyncClose(&ClientContext_empty_handler);
        }
    }
}

void ClientContext::AsyncConnectService6(
    const RR_SHARED_PTR<std::string>& ret, const RR_SHARED_PTR<RobotRaconteurException>& e, const std::string& type,
    const std::string& username, const RR_SHARED_PTR<PullServiceDefinitionAndImportsReturn>& d,
    const boost::function<void(const RR_SHARED_PTR<RRObject>&, const RR_SHARED_PTR<RobotRaconteurException>&)>& handler)
{
    RR_UNUSED(d);
    RR_UNUSED(username);
    boost::recursive_mutex::scoped_lock cl_lock(connect_lock);
    // std::cout << "AsyncConnectService6" << std::endl;
    if (e)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Client, GetLocalEndpoint(),
                                           "AsyncConnectService failed: " << e->what());
        detail::InvokeHandlerWithException(node, handler, e);
        AsyncClose(&ClientContext_empty_handler);
        return;
    }
    else if (*ret != "OK")
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Client, GetLocalEndpoint(),
                                           "AsyncConnectService authentication failed");
        detail::InvokeHandlerWithException(node, handler,
                                           RR_MAKE_SHARED<AuthenticationException>("Invalid username or credentials"));

        AsyncClose(&ClientContext_empty_handler);
        return;
    }
    else
    {
        try
        {
            RR_SHARED_PTR<ServiceStub> stub = GetServiceDef()->CreateStub(type, GetServiceName(), shared_from_this());
            {
                boost::mutex::scoped_lock lock(stubs_lock);
                stubs.insert(make_pair(GetServiceName(), stub));
            }

            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Client, GetLocalEndpoint(),
                                               "AsyncConnectService completed successfully");

            // std::cout << "handler" << std::endl;
            RR_SHARED_PTR<RRObject> ret = RR_STATIC_POINTER_CAST<RRObject>(stub);
            detail::InvokeHandler(node, handler, ret);
        }
        catch (std::exception& err)
        {
            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Client, GetLocalEndpoint(),
                                               "AsyncConnectService initializing client stub failed: " << err.what());
            detail::InvokeHandlerWithException(node, handler, err, MessageErrorType_ServiceError);
            AsyncClose(&ClientContext_empty_handler);
        }
    }
}

void ClientContext::AsyncConnectService7(
    const RR_INTRUSIVE_PTR<MessageEntry>& ret, const RR_SHARED_PTR<RobotRaconteurException>& e,
    const std::string& objecttype, const std::string& username,
    const boost::function<void(const RR_SHARED_PTR<RRObject>&, const RR_SHARED_PTR<RobotRaconteurException>&)>& handler)
{
    boost::recursive_mutex::scoped_lock cl_lock(connect_lock);

    if (e)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Client, GetLocalEndpoint(),
                                           "AsyncConnectService failed: " << e->what());
        detail::InvokeHandlerWithException(node, handler, e);
        AsyncClose(&ClientContext_empty_handler);
        return;
    }

    try
    {
        {
            boost::mutex::scoped_lock lock(pulled_service_defs_lock);
            RR_INTRUSIVE_PTR<MessageElementNestedElementList> l =
                ret->FindElement("servicedefs")->CastDataToNestedList(DataTypes_list_t);
            if (!l)
                throw ServiceException("servicedefs cannot be null on connect");
            BOOST_FOREACH (const RR_INTRUSIVE_PTR<MessageElement>& l1, l->Elements)
            {
                RR_SHARED_PTR<ServiceDefinition> d1 = RR_MAKE_SHARED<ServiceDefinition>();
                std::vector<ServiceDefinitionParseException> warnings;
                d1->FromString(l1->CastDataToString(), warnings);
                pulled_service_defs.insert(std::make_pair(d1->Name, d1));
            }
        }

        std::string type = ret->FindElement("objecttype")->CastDataToString();

        if (type.empty())
            throw ObjectNotFoundException("Could not find object type");

        // If we want a desired type, try to figure out if the upcast is valid
        if (!objecttype.empty() && type != objecttype)
        {
            bool found = false;
            RR_INTRUSIVE_PTR<MessageElement> objectimplements_m;
            if (ret->TryFindElement("objectimplements", objectimplements_m))
            {
                std::vector<std::string> objectimplements =
                    RRListToStringVector(GetNode()->UnpackListType<RRArray<char> >(
                        objectimplements_m->CastDataToNestedList(DataTypes_list_t)));
                if (boost::range::find(objectimplements, objecttype) != objectimplements.end())
                {
                    type = objecttype;
                    found = true;
                }
            }

            if (!found)
            {
                VerifyObjectImplements(type, objecttype);
                type = objecttype;
            }
        }

        try
        {
            if (GetNode()->GetDynamicServiceFactory())
            {
                boost::mutex::scoped_lock lock(pulled_service_defs_lock);
                use_pulled_types = true;

                std::vector<std::string> pulleddefs_str;
                BOOST_FOREACH (const RR_SHARED_PTR<ServiceDefinition>& ee,
                               pulled_service_defs | boost::adaptors::map_values)
                {
                    pulleddefs_str.push_back(ee->ToString());
                }

                std::vector<RR_SHARED_PTR<ServiceFactory> > di2 =
                    GetNode()->GetDynamicServiceFactory()->CreateServiceFactories(pulleddefs_str);

                for (size_t i = 0; i < pulleddefs_str.size(); i++)
                {
                    boost::mutex::scoped_lock lock(pulled_service_types_lock);
                    pulled_service_types.insert(std::make_pair(di2.at(i)->GetServiceName(), di2.at(i)));
                }

                m_ServiceDef = GetPulledServiceType(SplitQualifiedName(type).get<0>());
            }
            else
            {
                m_ServiceDef = GetNode()->GetServiceType(SplitQualifiedName(type).get<0>());
            }
        }
        catch (std::exception&)
        {
            throw ServiceException("Could not find correct service factory for remote service");
        }

        RR_SHARED_PTR<ServiceStub> stub = GetServiceDef()->CreateStub(type, GetServiceName(), shared_from_this());
        {
            boost::mutex::scoped_lock lock(stubs_lock);
            stubs.insert(make_pair(GetServiceName(), stub));
        }

        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Client, GetLocalEndpoint(),
                                           "AsyncConnectService completed successfully");

        if (!username.empty())
        {
            m_AuthenticatedUsername = username;
            m_UserAuthenticated = true;
        }

        RR_SHARED_PTR<RRObject> ret = RR_STATIC_POINTER_CAST<RRObject>(stub);
        detail::InvokeHandler(node, handler, ret);
    }
    catch (std::exception& err)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Client, GetLocalEndpoint(),
                                           "AsyncConnectService failed: " << err.what());
        detail::InvokeHandlerWithException(node, handler, err, MessageErrorType_ServiceError);
        AsyncClose(&ClientContext_empty_handler);
    }
}

bool ClientContext::VerifyObjectImplements2(boost::string_ref objecttype, boost::string_ref implementstype)
{
    if (objecttype == implementstype)
        return true;

    boost::tuple<boost::string_ref, boost::string_ref> s1 = SplitQualifiedName(objecttype);

    if (pulled_service_defs.find(s1.get<0>().to_string()) == pulled_service_defs.end())
    {
        RR_SHARED_PTR<ServiceDefinition> d1 = PullServiceDefinition(s1.get<0>()).def;
        pulled_service_defs.insert(std::make_pair(d1->Name, d1));
    }

    std::map<std::string, RR_SHARED_PTR<ServiceDefinition> >::iterator e1 =
        pulled_service_defs.find(s1.get<0>().to_string());
    if (e1 == pulled_service_defs.end())
        return false;

    RR_SHARED_PTR<ServiceDefinition> d = e1->second;

    RR_SHARED_PTR<ServiceEntryDefinition> o;

    BOOST_FOREACH (RR_SHARED_PTR<ServiceEntryDefinition>& e, d->Objects)
    {
        if (e->Name == s1.get<1>())
            o = e;
    }

    if (!o)
        return false;

    BOOST_FOREACH (std::string& e, o->Implements)
    {
        std::string deftype = d->Name;
        std::string objtype;

        if (!boost::contains(e, "."))
        {
            objtype = e;
        }
        else
        {
            boost::tuple<boost::string_ref, boost::string_ref> s2 = SplitQualifiedName(e);
            deftype = s2.get<0>().to_string();
            objtype = s2.get<1>().to_string();
        }

        // NOLINTBEGIN(performance-inefficient-string-concatenation)
        if ((deftype + "." + objtype) == implementstype)
            return true;

        if (VerifyObjectImplements2(deftype + "." + objtype, implementstype))
            return true;
        // NOLINTEND(performance-inefficient-string-concatenation)
    }

    return false;
}

bool ClientContext::VerifyObjectImplements(boost::string_ref objecttype, boost::string_ref implementstype)
{
    boost::mutex::scoped_lock lock(pulled_service_defs_lock);
    if (!VerifyObjectImplements2(objecttype, implementstype))
        throw ServiceException("Invalid object type");
    return true;
}

void ClientContext::Close()
{
    ROBOTRACONTEUR_ASSERT_MULTITHREADED(node);

    RR_SHARED_PTR<detail::sync_async_handler<void> > h =
        RR_MAKE_SHARED<detail::sync_async_handler<void> >(RR_MAKE_SHARED<RequestTimeoutException>(""));
    AsyncClose(boost::bind(&detail::sync_async_handler<void>::operator(), h));

    try
    {
        h->end_void();
    }
    catch (std::exception&)
    {}
}

void ClientContext::AsyncClose(RR_MOVE_ARG(boost::function<void()>) handler)
{

    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Client, GetLocalEndpoint(), "Begin AsyncClose");

    boost::mutex::scoped_lock lock3(close_lock);
    {
        boost::mutex::scoped_lock lock(m_Connected_lock);
        if (!m_Connected)
            return;
    }
    try
    {
        RR_INTRUSIVE_PTR<MessageEntry> e = CreateMessageEntry(MessageEntryType_DisconnectClient, "");
        e->AddElement("servicename", stringToRRArray(GetServiceName()));

        AsyncProcessRequest(e,
                            boost::bind(&ClientContext::AsyncClose1, shared_from_this(), RR_BOOST_PLACEHOLDERS(_1),
                                        RR_BOOST_PLACEHOLDERS(_2), handler),
                            500);
    }
    catch (std::exception& err)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Client, GetLocalEndpoint(), "AsyncClose failed: " << err.what());

        {
            boost::mutex::scoped_lock lock(m_Connected_lock);
            m_Connected = false;
        }

        RobotRaconteurNode::TryPostToThreadPool(node,
                                                boost::bind(&ClientContext::AsyncClose1, shared_from_this(),
                                                            RR_INTRUSIVE_PTR<MessageEntry>(),
                                                            RR_SHARED_PTR<RobotRaconteurException>(), handler),
                                                true);
    }
}

void ClientContext::AsyncClose1(const RR_INTRUSIVE_PTR<MessageEntry>& m,
                                const RR_SHARED_PTR<RobotRaconteurException>& err,
                                const boost::function<void()>& handler)
{
    RR_UNUSED(err);
    {

        {
            boost::mutex::scoped_lock lock(m_Connected_lock);
            m_Connected = false;
        }

        std::list<boost::function<void(const RR_INTRUSIVE_PTR<MessageEntry>& ret,
                                       const RR_SHARED_PTR<RobotRaconteurException>& error)> >
            handlers;

        {
            boost::mutex::scoped_lock lock(outstanding_requests_lock);
            BOOST_FOREACH (RR_SHARED_PTR<outstanding_request>& e, outstanding_requests | boost::adaptors::map_values)
            {
                e->evt->Set();

                if (e->handler)
                    handlers.push_back(e->handler);
            }

            outstanding_requests.clear();

            if (connection_test_timer)
            {
                try
                {
                    connection_test_timer->Stop();
                }
                catch (std::exception&)
                {}
                connection_test_timer.reset();
            }
        }

        BOOST_FOREACH (boost::function<void(const RR_INTRUSIVE_PTR<MessageEntry>& ret,
                                            const RR_SHARED_PTR<RobotRaconteurException>& error)>& e,
                       handlers)
        {
            try
            {
                e(RR_INTRUSIVE_PTR<MessageEntry>(), RR_MAKE_SHARED<ConnectionException>("Connection closed"));
            }
            catch (std::exception& exp)
            {
                if (m)
                {
                    ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(
                        node, Client, GetLocalEndpoint(), m->ServicePath, m->MemberName,
                        "Error handling AsyncProcessRequest connection closed: " << exp.what());
                }
                else
                {
                    ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(
                        node, Client, GetLocalEndpoint(),
                        "Error handling AsyncProcessRequest connection closed: " << exp.what());
                }
                RobotRaconteurNode::TryHandleException(node, &exp);
            }
        }

        // TODO fix this...

        {
            boost::mutex::scoped_lock lock(stubs_lock);
            BOOST_FOREACH (RR_SHARED_PTR<ServiceStub>& s, stubs | boost::adaptors::map_values)
            {
                try
                {
                    s->RRClose();
                }
                catch (std::exception&)
                {}
            }
        }

        {
            boost::mutex::scoped_lock lock(stubs_lock);
            stubs.clear();
        }
        {
            boost::mutex::scoped_lock lock(outstanding_requests_lock);
            outstanding_requests.clear();
        }
        // m_Connected = false;

        try
        {

            ClientServiceListener(shared_from_this(), ClientServiceListenerEventType_ClientClosed,
                                  RR_SHARED_PTR<RRObject>());
        }
        catch (std::exception& exp)
        {
            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Client, GetLocalEndpoint(),
                                               "Client listener callback raised exception: " << exp.what());
            RobotRaconteurNode::TryHandleException(node, &exp);
        }
        catch (...)
        {}

        ROBOTRACONTEUR_LOG_INFO_COMPONENT(node, Client, GetLocalEndpoint(), "Client closed");

        detail::InvokeHandler(node, handler);
    }

    // boost::this_thread::sleep(boost::posix_time::milliseconds(100));
    GetNode()->DeleteEndpoint(shared_from_this());
}

void ClientContext::AsyncSendPipeMessage(
    const RR_INTRUSIVE_PTR<MessageEntry>& m, bool unreliable,
    const boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>&)>& handler)
{
    if (!unreliable)
    {
        ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Client, GetLocalEndpoint(), m->ServicePath, m->MemberName,
                                                "Client sending reliable pipe packet EntryType " << m->EntryType);
        AsyncSendMessage(m, (handler));
    }
    else
    {
        ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Client, GetLocalEndpoint(), m->ServicePath, m->MemberName,
                                                "Client sending unreliable pipe packet EntryType " << m->EntryType);

        RR_INTRUSIVE_PTR<Message> mm = CreateMessage();
        mm->header = CreateMessageHeader();
        mm->header->MetaData = "unreliable\n";

        mm->entries.push_back(m);
        // m.EntryType= MessageEntryType.PipePacket;
        boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>&)> h =
            boost::bind(&rr_context_emptyhandler, RR_BOOST_PLACEHOLDERS(_1));
        Endpoint::AsyncSendMessage(mm, h);
    }
}

void ClientContext::SendWireMessage(const RR_INTRUSIVE_PTR<MessageEntry>& m)
{
    ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Client, GetLocalEndpoint(), m->ServicePath, m->MemberName,
                                            "Client sending unreliable wire packet EntryType " << m->EntryType);

    RR_INTRUSIVE_PTR<Message> mm = CreateMessage();
    mm->header = CreateMessageHeader();
    mm->header->MetaData = "unreliable\n";

    mm->entries.push_back(m);
    // m.EntryType= MessageEntryType.PipePacket;
    boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>&)> h =
        boost::bind(&rr_context_emptyhandler, RR_BOOST_PLACEHOLDERS(_1));
    Endpoint::AsyncSendMessage(mm, h);
}

PullServiceDefinitionReturn ClientContext::PullServiceDefinition(boost::string_ref ServiceType)
{
    ROBOTRACONTEUR_ASSERT_MULTITHREADED(node);

    RR_SHARED_PTR<detail::sync_async_handler<PullServiceDefinitionReturn> > d =
        RR_MAKE_SHARED<detail::sync_async_handler<PullServiceDefinitionReturn> >(
            RR_MAKE_SHARED<ServiceException>("Could not pull service definition"));
    AsyncPullServiceDefinition(ServiceType,
                               boost::bind(&detail::sync_async_handler<PullServiceDefinitionReturn>::operator(), d,
                                           RR_BOOST_PLACEHOLDERS(_1), RR_BOOST_PLACEHOLDERS(_2)),
                               boost::numeric_cast<int32_t>(GetNode()->GetRequestTimeout()));
    RR_SHARED_PTR<PullServiceDefinitionReturn> ret = d->end();
    if (!ret)
        throw NullValueException("Unexpected null return from service");
    return *ret;
}

void ClientContext::AsyncPullServiceDefinition(
    boost::string_ref ServiceType,
    RR_MOVE_ARG(boost::function<void(const RR_SHARED_PTR<PullServiceDefinitionReturn>&,
                                     const RR_SHARED_PTR<RobotRaconteurException>&)>) handler,
    int32_t timeout)
{
    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Client, GetLocalEndpoint(),
                                       "Begin AsyncPullServiceDefinition for type \"" << ServiceType << "\"");

    RR_INTRUSIVE_PTR<MessageEntry> e3 = CreateMessageEntry(MessageEntryType_GetServiceDesc, "");
    // e.AddElement("servicepath", ServiceName);

    if (!ServiceType.empty())
        e3->AddElement("ServiceType", stringToRRArray(ServiceType));

    e3->ServicePath = GetServiceName();
    e3->AddElement("clientversion", stringToRRArray(ROBOTRACONTEUR_VERSION_TEXT));

    AsyncProcessRequest(e3,
                        boost::bind(&ClientContext::AsyncPullServiceDefinition1, shared_from_this(),
                                    RR_BOOST_PLACEHOLDERS(_1), RR_BOOST_PLACEHOLDERS(_2), ServiceType.to_string(),
                                    handler),
                        timeout);
}

void ClientContext::AsyncPullServiceDefinition1(
    const RR_INTRUSIVE_PTR<MessageEntry>& ret3, const RR_SHARED_PTR<RobotRaconteurException>& err,
    const std::string& ServiceType,
    boost::function<void(const RR_SHARED_PTR<PullServiceDefinitionReturn>&,
                         const RR_SHARED_PTR<RobotRaconteurException>&)>& handler)
{
    if (err)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Client, GetLocalEndpoint(),
                                           "AsyncPullServiceDefinition failed: " << err->what());
        detail::InvokeHandlerWithException(node, handler, err);
        return;
    }
    else
    {
        RR_SHARED_PTR<PullServiceDefinitionReturn> ret = RR_MAKE_SHARED<PullServiceDefinitionReturn>();
        try
        {

            if (ret3->Error != MessageErrorType_None)
                throw RobotRaconteurExceptionUtil::MessageEntryToException(ret3);
            std::string def = ret3->FindElement("servicedef")->CastDataToString();
            if (def.empty())
                throw ServiceNotFoundException("Could not find service definition");

            RR_SHARED_PTR<ServiceDefinition> d = RR_MAKE_SHARED<ServiceDefinition>();
            std::vector<ServiceDefinitionParseException> w;
            d->FromString(def, w);

            ret->def = d;

            if (ServiceType.empty())
            {
                RR_INTRUSIVE_PTR<MessageElement> attributes;
                if (ret3->TryFindElement("attributes", attributes))
                {
                    RR_INTRUSIVE_PTR<RRMap<std::string, RRValue> > attributes1 = rr_cast<RRMap<std::string, RRValue> >(
                        (GetNode()->UnpackMapType<std::string, RRValue>(attributes->CastDataToNestedList())));
                    if (attributes1)
                    {
                        ret->attributes = attributes1;
                    }
                }

                RR_INTRUSIVE_PTR<MessageElement> extra_imports;
                if (ret3->TryFindElement("extraimports", extra_imports))
                {
                    RR_INTRUSIVE_PTR<RRList<RRArray<char> > > extra_imports1 =
                        rr_cast<RRList<RRArray<char> > >(GetNode()->UnpackListType<RRArray<char> >(
                            extra_imports->CastDataToNestedList(DataTypes_list_t)));
                    if (extra_imports1)
                    {
                        BOOST_FOREACH (const RR_INTRUSIVE_PTR<RRArray<char> >& import_, *extra_imports1)
                        {
                            ret->extra_imports.insert(RRArrayToString(import_));
                        }
                    }
                }
            }

            ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Client, GetLocalEndpoint(),
                                               "AsyncPullServiceDefinition completed successfully");

            detail::InvokeHandler(node, handler, ret);
        }
        catch (std::exception& err)
        {
            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Client, GetLocalEndpoint(),
                                               "AsyncPullServiceDefinition failed: " << err.what());
            detail::InvokeHandlerWithException(node, handler, err, MessageErrorType_ConnectionError);
        }
    }
}

std::map<std::string, RR_INTRUSIVE_PTR<RRValue> > ClientContext::GetAttributes()
{
    boost::mutex::scoped_lock lock(m_Attributes_lock);
    return m_Attributes;
}

void ClientContext::AsyncPullServiceDefinitionAndImports(
    boost::string_ref servicetype,
    RR_MOVE_ARG(boost::function<void(const RR_SHARED_PTR<PullServiceDefinitionAndImportsReturn>&,
                                     const RR_SHARED_PTR<RobotRaconteurException>&)>) handler,
    int32_t timeout)
{
    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Client, GetLocalEndpoint(),
                                       "Begin AsyncPullServiceDefinitionAndImports for type \"" << servicetype << "\"");
    boost::posix_time::ptime timeout_time = GetNode()->NowNodeTime() + boost::posix_time::milliseconds(timeout);

    AsyncPullServiceDefinition(
        servicetype,
        boost::bind(&ClientContext::AsyncPullServiceDefinitionAndImports1, shared_from_this(),
                    RR_BOOST_PLACEHOLDERS(_1), RR_BOOST_PLACEHOLDERS(_2), servicetype.to_string(),
                    RR_SHARED_PTR<PullServiceDefinitionAndImportsReturn>(), handler, timeout_time),
        boost::numeric_cast<int32_t>((timeout_time - GetNode()->NowNodeTime()).total_milliseconds()));
}

void ClientContext::AsyncPullServiceDefinitionAndImports1(
    const RR_SHARED_PTR<PullServiceDefinitionReturn>& pull_ret, const RR_SHARED_PTR<RobotRaconteurException>& err,
    const std::string& servicetype, const RR_SHARED_PTR<PullServiceDefinitionAndImportsReturn>& current,
    boost::function<void(const RR_SHARED_PTR<PullServiceDefinitionAndImportsReturn>&,
                         const RR_SHARED_PTR<RobotRaconteurException>&)>& handler,
    boost::posix_time::ptime timeout_time)
{
    RR_SHARED_PTR<PullServiceDefinitionAndImportsReturn> current2 = current;
    if (err)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Client, GetLocalEndpoint(),
                                           "AsyncPullServiceDefinition failed: " << err->what());
        detail::InvokeHandlerWithException(node, handler, err);
        return;
    }
    else
    {
        try
        {
            if (!current2)
            {
                current2 = RR_MAKE_SHARED<PullServiceDefinitionAndImportsReturn>();
                current2->defs.push_back(pull_ret->def);
                current2->attributes = pull_ret->attributes;
                current2->extra_imports = pull_ret->extra_imports;
            }
            else
            {
                if (!TryFindByName(current2->defs, pull_ret->def->Name))
                {
                    current2->defs.push_back(pull_ret->def);
                }
            }

            std::set<std::string> needed_defs = current2->extra_imports;

            BOOST_FOREACH (const RR_SHARED_PTR<ServiceDefinition>& d, current2->defs)
            {
                boost::range::copy(d->Imports, std::inserter(needed_defs, needed_defs.begin()));
            }

            BOOST_FOREACH (const RR_SHARED_PTR<ServiceDefinition>& d, current2->defs)
            {
                needed_defs.erase(d->Name);
            }

            if (needed_defs.empty())
            {
                ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Client, GetLocalEndpoint(),
                                                   "AsyncPullServiceDefinitionAndImports completed successfully");
                detail::InvokeHandler(node, handler, current2);
                return;
            }
            else
            {
                AsyncPullServiceDefinition(
                    *needed_defs.begin(),
                    boost::bind(&ClientContext::AsyncPullServiceDefinitionAndImports1, shared_from_this(),
                                RR_BOOST_PLACEHOLDERS(_1), RR_BOOST_PLACEHOLDERS(_2), std::string(servicetype),
                                current2, handler, timeout_time),
                    boost::numeric_cast<int32_t>((timeout_time - GetNode()->NowNodeTime()).total_milliseconds()));
                return;
            }
        }
        catch (std::exception& err)
        {
            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Client, GetLocalEndpoint(),
                                               "Begin AsyncPullServiceDefinition failed: " << err.what());
            detail::InvokeHandlerWithException(node, handler, err, MessageErrorType_ConnectionError);
        }
    }
}

PullServiceDefinitionAndImportsReturn ClientContext::PullServiceDefinitionAndImports(boost::string_ref servicetype)
{

    ROBOTRACONTEUR_ASSERT_MULTITHREADED(node);

    RR_SHARED_PTR<detail::sync_async_handler<PullServiceDefinitionAndImportsReturn> > d =
        RR_MAKE_SHARED<detail::sync_async_handler<PullServiceDefinitionAndImportsReturn> >(
            RR_MAKE_SHARED<ServiceException>("Could not pull service definition"));
    AsyncPullServiceDefinitionAndImports(
        servicetype,
        boost::bind(&detail::sync_async_handler<PullServiceDefinitionAndImportsReturn>::operator(), d,
                    RR_BOOST_PLACEHOLDERS(_1), RR_BOOST_PLACEHOLDERS(_2)),
        boost::numeric_cast<int32_t>(GetNode()->GetRequestTimeout()));
    RR_SHARED_PTR<PullServiceDefinitionAndImportsReturn> ret = d->end();
    if (!ret)
        throw NullValueException("Unexpected null return from service");
    return *ret;
}

std::vector<std::string> ClientContext::GetPulledServiceTypes()
{
    boost::mutex::scoped_lock lock(pulled_service_types_lock);

    std::vector<std::string> o;
    boost::copy(pulled_service_types | boost::adaptors::map_keys, std::back_inserter(o));

    return o;
}

RR_SHARED_PTR<ServiceFactory> ClientContext::GetPulledServiceType(boost::string_ref type)
{
    boost::mutex::scoped_lock lock(pulled_service_types_lock);
    RR_UNORDERED_MAP<std::string, RR_SHARED_PTR<ServiceFactory> >::iterator e1 =
        pulled_service_types.find(type.to_string());
    if (e1 == pulled_service_types.end())
    {
        throw ServiceException("Unknown service type");
    }
    return e1->second;
}

const bool ClientContext::GetUserAuthenticated()
{
    boost::mutex::scoped_lock lock(m_Authentication_lock);
    return m_UserAuthenticated;
}

const std::string ClientContext::GetAuthenticatedUsername()
{
    boost::mutex::scoped_lock lock(m_Authentication_lock);
    return m_AuthenticatedUsername;
}

std::string ClientContext::AuthenticateUser(boost::string_ref username, const RR_INTRUSIVE_PTR<RRValue>& credentials)
{
    ROBOTRACONTEUR_ASSERT_MULTITHREADED(node);

    RR_SHARED_PTR<detail::sync_async_handler<std::string> > d =
        RR_MAKE_SHARED<detail::sync_async_handler<std::string> >(RR_MAKE_SHARED<AuthenticationException>("Timed out"));
    AsyncAuthenticateUser(username, credentials,
                          boost::bind(&detail::sync_async_handler<std::string>::operator(), d,
                                      RR_BOOST_PLACEHOLDERS(_1), RR_BOOST_PLACEHOLDERS(_2)));
    return *d->end();
}

void ClientContext::AsyncAuthenticateUser(
    boost::string_ref username, const RR_INTRUSIVE_PTR<RRValue>& credentials,
    RR_MOVE_ARG(boost::function<void(const RR_SHARED_PTR<std::string>&, const RR_SHARED_PTR<RobotRaconteurException>&)>)
        handler,
    int32_t timeout)
{

    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Client, GetLocalEndpoint(),
                                       "Begin AuthenticateUser with username \"" << username << "\"");
    try
    {
        boost::mutex::scoped_lock lock(m_Authentication_lock);

        RR_INTRUSIVE_PTR<MessageEntry> m = CreateMessageEntry(MessageEntryType_ClientSessionOpReq, "AuthenticateUser");
        m->ServicePath = GetServiceName();
        m->AddElement("username", stringToRRArray(username));
        if (dynamic_cast<RRMap<std::string, RRValue>*>(credentials.get()) != 0)
        {
            m->AddElement("credentials", GetNode()->PackMapType<std::string, RRValue>(credentials));
        }
        else if (dynamic_cast<MessageElement*>(credentials.get()) != 0)
        {
            RR_INTRUSIVE_PTR<MessageElement> mcredentials = rr_cast<MessageElement>(credentials);
            mcredentials->ElementName = "credentials";
            m->AddElement(mcredentials);
        }
        AsyncProcessRequest(m,
                            boost::bind(&ClientContext::AsyncAuthenticateUser2, shared_from_this(),
                                        RR_BOOST_PLACEHOLDERS(_1), RR_BOOST_PLACEHOLDERS(_2), username.to_string(),
                                        handler),
                            timeout);
    }
    catch (std::exception& exp)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Client, GetLocalEndpoint(), "AuthenticateUser failed: " << exp.what());
        throw;
    }
}

void ClientContext::AsyncAuthenticateUser2(
    const RR_INTRUSIVE_PTR<MessageEntry>& ret, const RR_SHARED_PTR<RobotRaconteurException>& err,
    const std::string& username,
    boost::function<void(const RR_SHARED_PTR<std::string>&, const RR_SHARED_PTR<RobotRaconteurException>&)>& handler)
{
    if (err)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Client, GetLocalEndpoint(),
                                           "AuthenticateUser failed: " << err->what());
        detail::InvokeHandlerWithException(node, handler, err);
        return;
    }

    m_AuthenticatedUsername = username;
    m_UserAuthenticated = true;
    std::string res = ret->FindElement("return")->CastDataToString();
    RR_SHARED_PTR<std::string> res2 = RR_MAKE_SHARED<std::string>(res);

    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Client, GetLocalEndpoint(),
                                       "AuthenticateUser completed successfully with username \"" << username << "\"");

    detail::InvokeHandler(node, handler, res2);
}

std::string ClientContext::LogoutUser()
{
    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Client, GetLocalEndpoint(), "Begin LogoutUser");

    try
    {
        boost::mutex::scoped_lock lock(m_Authentication_lock);
        if (!GetUserAuthenticated())
            throw InvalidOperationException("User is not authenticated");

        m_UserAuthenticated = false;
        m_AuthenticatedUsername.clear();

        RR_INTRUSIVE_PTR<MessageEntry> m = CreateMessageEntry(MessageEntryType_ClientSessionOpReq, "LogoutUser");
        m->ServicePath = GetServiceName();
        m->AddElement("username", stringToRRArray(GetAuthenticatedUsername()));
        RR_INTRUSIVE_PTR<MessageEntry> ret = ProcessRequest(m);
        return ret->FindElement("return")->CastDataToString();
    }
    catch (std::exception& exp)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Client, GetLocalEndpoint(), "LogoutUser failed: " << exp.what());
        throw;
    }
}

std::string ClientContext::RequestObjectLock(const RR_SHARED_PTR<RRObject>& obj, RobotRaconteurObjectLockFlags flags)
{
    ROBOTRACONTEUR_ASSERT_MULTITHREADED(node);

    RR_SHARED_PTR<detail::sync_async_handler<std::string> > t =
        RR_MAKE_SHARED<detail::sync_async_handler<std::string> >();
    AsyncRequestObjectLock(obj, flags,
                           boost::bind(&detail::sync_async_handler<std::string>::operator(), t,
                                       RR_BOOST_PLACEHOLDERS(_1), RR_BOOST_PLACEHOLDERS(_2)),
                           boost::numeric_cast<int32_t>(GetNode()->GetRequestTimeout()));
    return *t->end();
}

void ClientContext::AsyncRequestObjectLock(
    const RR_SHARED_PTR<RRObject>& obj, RobotRaconteurObjectLockFlags flags,
    RR_MOVE_ARG(boost::function<void(const RR_SHARED_PTR<std::string>&, const RR_SHARED_PTR<RobotRaconteurException>&)>)
        handler,
    int32_t timeout)
{
    try
    {
        RR_SHARED_PTR<ServiceStub> s = RR_DYNAMIC_POINTER_CAST<ServiceStub>(obj);
        if (!s)
            throw InvalidArgumentException("Can only lock object opened through Robot Raconteur");

        std::string command;
        if (flags == RobotRaconteurObjectLockFlags_USER_LOCK)
        {
            command = "RequestObjectLock";
        }
        else if (flags == RobotRaconteurObjectLockFlags_CLIENT_LOCK)
        {
            command = "RequestClientObjectLock";
        }
        else
            throw InvalidArgumentException("Unknown flags");

        ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Client, GetLocalEndpoint(),
                                           "Begin AsyncRequestObjectLock for servicepath \""
                                               << s->ServicePath << "\" with command " << command);

        RR_INTRUSIVE_PTR<MessageEntry> m = CreateMessageEntry(MessageEntryType_ClientSessionOpReq, command);
        m->ServicePath = s->ServicePath;

        AsyncProcessRequest(m,
                            boost::bind(&ClientContext::EndAsyncLockOp, shared_from_this(), RR_BOOST_PLACEHOLDERS(_1),
                                        RR_BOOST_PLACEHOLDERS(_2), handler),
                            timeout);
    }
    catch (std::exception& e)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Client, GetLocalEndpoint(),
                                           "AsyncRequestObjectLock failed: " << e.what());
        throw;
    }
}

void ClientContext::EndAsyncLockOp(
    const RR_INTRUSIVE_PTR<MessageEntry>& ret, const RR_SHARED_PTR<RobotRaconteurException>& err,
    boost::function<void(const RR_SHARED_PTR<std::string>&, const RR_SHARED_PTR<RobotRaconteurException>&)>& handler)
{
    if (err)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Client, GetLocalEndpoint(),
                                           "Object lock operation failed: " << err->what())
        detail::InvokeHandlerWithException(node, handler, err);
        return;
    }
    try
    {
        std::string rets = ret->FindElement("return")->CastDataToString();
        RR_SHARED_PTR<std::string> rets2 = RR_MAKE_SHARED<std::string>(rets);
        ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Client, GetLocalEndpoint(),
                                           "Object lock operation completed successfully for servicepath \""
                                               << ret->ServicePath << "\"");
        detail::InvokeHandler(node, handler, rets2);
    }
    catch (std::exception& err)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Client, GetLocalEndpoint(),
                                           "Object lock operation failed: " << err.what());
        detail::InvokeHandlerWithException(node, handler, err, MessageErrorType_ServiceError);
    }
}

std::string ClientContext::ReleaseObjectLock(const RR_SHARED_PTR<RRObject>& obj)
{
    ROBOTRACONTEUR_ASSERT_MULTITHREADED(node);

    RR_SHARED_PTR<detail::sync_async_handler<std::string> > t =
        RR_MAKE_SHARED<detail::sync_async_handler<std::string> >();
    AsyncReleaseObjectLock(obj,
                           boost::bind(&detail::sync_async_handler<std::string>::operator(), t,
                                       RR_BOOST_PLACEHOLDERS(_1), RR_BOOST_PLACEHOLDERS(_2)),
                           boost::numeric_cast<int32_t>(GetNode()->GetRequestTimeout()));
    return *t->end();
}

void ClientContext::AsyncReleaseObjectLock(
    const RR_SHARED_PTR<RRObject>& obj,
    RR_MOVE_ARG(boost::function<void(const RR_SHARED_PTR<std::string>&, const RR_SHARED_PTR<RobotRaconteurException>&)>)
        handler,
    int32_t timeout)
{
    try
    {
        RR_SHARED_PTR<ServiceStub> s = RR_DYNAMIC_POINTER_CAST<ServiceStub>(obj);

        if (!s)
            throw InvalidArgumentException("Can only unlock object opened through Robot Raconteur");

        ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Client, GetLocalEndpoint(),
                                           "Begin AsyncReleaseObjectLock for servicepath \"" << s->ServicePath << "\"");

        RR_INTRUSIVE_PTR<MessageEntry> m = CreateMessageEntry(MessageEntryType_ClientSessionOpReq, "ReleaseObjectLock");
        m->ServicePath = s->ServicePath;

        AsyncProcessRequest(m,
                            boost::bind(&ClientContext::EndAsyncLockOp, shared_from_this(), RR_BOOST_PLACEHOLDERS(_1),
                                        RR_BOOST_PLACEHOLDERS(_2), handler),
                            timeout);
    }
    catch (std::exception& exp)
    {
        ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Client, GetLocalEndpoint(),
                                           "AsyncReleaseObjectLock failed: " << exp.what());
        throw;
    }
}

std::string ClientContext::MonitorEnter(const RR_SHARED_PTR<RRObject>& obj, int32_t timeout)
{
    try
    {
        RR_SHARED_PTR<ServiceStub> stub2 = RR_DYNAMIC_POINTER_CAST<ServiceStub>(obj);
        if (!stub2)
            throw InvalidArgumentException("Can only unlock object opened through Robot Raconteur");

        ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Client, GetLocalEndpoint(),
                                           "Begin MonitorEnter for servicepath \"" << stub2->ServicePath << "\"");

        bool iserror = true;

        {

            RR_SHARED_PTR<ServiceStub> s = rr_cast<ServiceStub>(obj);
            s->RRMutex->lock();

            bool keep_trying = true;
            RR_INTRUSIVE_PTR<MessageEntry> m = CreateMessageEntry(MessageEntryType_ClientSessionOpReq, "MonitorEnter");
            m->ServicePath = s->ServicePath;
            m->AddElement("timeout", ScalarToRRArray<int32_t>(timeout));

            RR_INTRUSIVE_PTR<MessageEntry> ret = ProcessRequest(m);
            std::string retcode = ret->FindElement("return")->CastDataToString();

            if (retcode == "OK")
            {
                ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Client, GetLocalEndpoint(),
                                                   "MonitorEnter for successful for servicepath \"" << s->ServicePath
                                                                                                    << "\"");
                return "OK";
            }
            if (retcode == "Continue")
            {
                while (keep_trying)
                {
                    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Client, GetLocalEndpoint(),
                                                       "MonitorEnter continue enter for servicepath \""
                                                           << s->ServicePath << "\"");
                    RR_INTRUSIVE_PTR<MessageEntry> m1 =
                        CreateMessageEntry(MessageEntryType_ClientSessionOpReq, "MonitorContinueEnter");
                    m1->ServicePath = s->ServicePath;

                    RR_INTRUSIVE_PTR<MessageEntry> ret1 = ProcessRequest(m1);
                    std::string retcode1 = ret1->FindElement("return")->CastDataToString();
                    if (retcode1 == "OK")
                    {
                        ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Client, GetLocalEndpoint(),
                                                           "MonitorEnter for successful for servicepath \""
                                                               << s->ServicePath << "\"");
                        return "OK";
                    }
                    if (retcode1 != "Continue")
                    {
                        s->RRMutex->unlock();
                        throw ProtocolException("Unknown return code");
                    }
                }
            }
            else
            {
                s->RRMutex->unlock();
                throw ProtocolException("Unknown return code");
            }
            s->RRMutex->unlock();
        }

        throw ProtocolException("Unknown return code");
    }
    catch (std::exception& exp)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Client, GetLocalEndpoint(), "MonitorEnter failed: " << exp.what());
        throw;
    }
}

void ClientContext::MonitorExit(const RR_SHARED_PTR<RRObject>& obj)
{
    try
    {
        RR_SHARED_PTR<ServiceStub> stub2 = RR_DYNAMIC_POINTER_CAST<ServiceStub>(obj);
        if (!stub2)
        {
            throw InvalidArgumentException("Can only unlock object opened through Robot Raconteur");
        }

        ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Client, GetLocalEndpoint(),
                                           "Begin MonitorExit for servicepath \"" << stub2->ServicePath << "\"");

        try
        {
            RR_SHARED_PTR<ServiceStub> s = rr_cast<ServiceStub>(obj);

            boost::recursive_mutex::scoped_lock lock2(*s->RRMutex);

            RR_INTRUSIVE_PTR<MessageEntry> m = CreateMessageEntry(MessageEntryType_ClientSessionOpReq, "MonitorExit");
            m->ServicePath = s->ServicePath;

            RR_INTRUSIVE_PTR<MessageEntry> ret = ProcessRequest(m);
            std::string retcode = ret->FindElement("return")->CastDataToString();
            if (retcode != "OK")
            {
                s->RRMutex->unlock();
                throw ProtocolException("Unknown return code");
            }
        }
        catch (std::exception&)
        {}
        try
        {
            if (stub2)
            {
                stub2->RRMutex->unlock();
            }
        }
        catch (std::exception&)
        {}

        ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Client, GetLocalEndpoint(),
                                           "MonitorExit successful for servicepath \"" << stub2->ServicePath << "\"");
    }
    catch (std::exception& exp)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Client, GetLocalEndpoint(), "MonitorExit failed: " << exp.what());
    }
}

void ClientContext::PeriodicCleanupTask()
{
    if (!GetConnected())
        return;

    boost::posix_time::ptime t;
    {

        t = GetLastMessageReceivedTime();
    }

    if ((GetNode()->NowNodeTime() - t).total_milliseconds() > GetNode()->GetEndpointInactivityTimeout())
    {
        // This may result in a rare segfault so we can't automatically delete
        // Close();
        // Instead send a message to the listener that the connection has timed out
        try
        {
            ClientServiceListener(shared_from_this(), ClientServiceListenerEventType_ClientConnectionTimeout,
                                  RR_SHARED_PTR<RRObject>());
        }
        catch (std::exception& exp)
        {
            RobotRaconteurNode::TryHandleException(node, &exp);
        }
    }

    if (GetRemoteEndpoint() != 0)
    {
        if ((GetNode()->NowNodeTime() - GetLastMessageSentTime()).total_milliseconds() > 60000)
        {
            ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Client, GetLocalEndpoint(), "Client sending keep alive request");
            try
            {
                RR_INTRUSIVE_PTR<MessageEntry> m = CreateMessageEntry(MessageEntryType_ClientKeepAliveReq, "");
                m->ServicePath = m_ServiceName;
                m->RequestID = 0;
                boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>&)> h =
                    boost::bind(&rr_context_emptyhandler, RR_BOOST_PLACEHOLDERS(_1));
                AsyncSendMessage(m, h);
            }
            catch (std::exception& exp)
            {
                ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Client, GetLocalEndpoint(),
                                                   "Client keep alive message failed: " << exp.what());
            }
        }
    }
}

uint32_t ClientContext::CheckServiceCapability(boost::string_ref name)
{
    RR_INTRUSIVE_PTR<MessageEntry> m = CreateMessageEntry(MessageEntryType_ServiceCheckCapabilityReq, name);
    m->ServicePath = m_ServiceName;
    RR_INTRUSIVE_PTR<MessageEntry> ret = ProcessRequest(m);
    uint32_t res = RRArrayToScalar(ret->FindElement("return")->CastData<RRArray<uint32_t> >());
    return res;
}

void ClientContext::ProcessCallbackCall(const RR_INTRUSIVE_PTR<MessageEntry>& m)
{

    ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Client, GetLocalEndpoint(), m->ServicePath, m->MemberName,
                                            "Received callback call request with requestid " << m->RequestID);

    RR_INTRUSIVE_PTR<MessageEntry> ret;
    try
    {
        RR_SHARED_PTR<ServiceStub> stub;
        {
            boost::mutex::scoped_lock lock(stubs_lock);
            RR_UNORDERED_MAP<MessageStringPtr, RR_SHARED_PTR<ServiceStub> >::iterator e1 = stubs.find(m->ServicePath);
            if (e1 == stubs.end())
                throw ServiceException("Stub not found");
            stub = e1->second;
        }

        ret = stub->CallbackCall(m);
        if (!ret)
            throw InternalErrorException("Null pointer exception");
    }
    catch (std::exception& e)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(node, Client, GetLocalEndpoint(), m->ServicePath, m->MemberName,
                                                "Client callback returning caught exception to caller: " << e.what());
        ret = CreateMessageEntry((MessageEntryType(m->EntryType + 1)), m->MemberName);
        ret->ServicePath = m->ServicePath;
        ret->RequestID = m->RequestID;
        RobotRaconteurExceptionUtil::ExceptionToMessageEntry(e, ret);
    }

    boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>&)> h =
        boost::bind(&rr_context_node_handler, GetNode(), RR_BOOST_PLACEHOLDERS(_1));
    AsyncSendMessage(ret, h);

    ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Client, GetLocalEndpoint(), m->ServicePath, m->MemberName,
                                            "Client callback completed successfully");
}

void ClientContext::TransportConnectionClosed(uint32_t endpoint)
{
    if (endpoint != GetLocalEndpoint())
    {
        return;
    }

    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Client, GetLocalEndpoint(), "Client received transport connection closed");

    try
    {
        RobotRaconteurNode::TryPostToThreadPool(
            node, boost::bind(&ClientContext::TransportConnectionClosed1, shared_from_this()), true);
    }
    catch (std::exception&)
    {}
}

void ClientContext::TransportConnectionClosed1()
{
    try
    {
        ClientServiceListener(shared_from_this(), ClientServiceListenerEventType_TransportConnectionClosed,
                              RR_SHARED_PTR<RRObject>());
    }
    catch (std::exception& exp)
    {
        RobotRaconteurNode::TryHandleException(node, &exp);
    }
}

} // namespace RobotRaconteur
