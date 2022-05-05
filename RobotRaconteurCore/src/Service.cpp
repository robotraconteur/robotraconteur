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

#include "RobotRaconteur/Service.h"
#include "RobotRaconteur/ServiceDefinition.h"
#include "RobotRaconteur/Message.h"
#include "RobotRaconteur/Error.h"
#include <boost/algorithm/string.hpp>
#include "RobotRaconteur/RobotRaconteurNode.h"
#include "RobotRaconteur/AutoResetEvent.h"
#include "RobotRaconteur/DataTypes.h"
#include "RobotRaconteur/Security.h"

#include "Service_lock_private.h"

#include "RobotRaconteur/ErrorUtil.h"
#include "RobotRaconteur/Generator.h"

#include <boost/range/adaptors.hpp>
#include <boost/range/algorithm.hpp>
#include <boost/foreach.hpp>

#undef SendMessage

namespace RobotRaconteur
{
static void rr_context_emptyhandler(RR_SHARED_PTR<RobotRaconteurException>) {}

static void rr_context_node_handler(RR_SHARED_PTR<RobotRaconteurNode> n, RR_SHARED_PTR<RobotRaconteurException> e)
{
    n->HandleException(e.get());
}

ServiceSkel::ServiceSkel() { ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Service, -1, "ServiceSkel created"); }

void ServiceSkel::Init(boost::string_ref s, RR_SHARED_PTR<RRObject> o, RR_SHARED_PTR<ServerContext> c)
{

    ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Service, -1, s, "",
                                            "Begin initializing service skel with type \"" << GetObjectType() << "\"");

    try
    {
        if (!o)
            throw InvalidArgumentException("Service object must not be null");

        m_ServicePath = RR_MOVE(s.to_string());
        m_context = c;
        uncastobj = o;

        this->node = c->GetNode();

        RegisterEvents(o);
        InitPipeServers(o);
        InitWireServers(o);
        InitCallbackServers(o);

        monitorlock.reset();
        objectlock.reset();

        std::string object_type_q = GetObjectType();
        boost::string_ref object_def;
        boost::string_ref object_type;

        boost::tie(object_def, object_type) = SplitQualifiedName(object_type_q);
        RR_SHARED_PTR<ServiceDefinition> d = RRGetNode()->GetServiceType(object_def)->ServiceDef();
        object_type_ver.push_back(boost::make_tuple(d->StdVer, object_type_q));
        if (d->StdVer)
        {
            RR_SHARED_PTR<ServiceEntryDefinition> e = TryFindByName(d->Objects, object_type);
            if (e)
            {
                std::set<std::string> found_defs;
                std::set<RobotRaconteurVersion> found_versions;
                BOOST_FOREACH (const std::string& s, e->Implements)
                {
                    if (!boost::contains(s, "."))
                        continue;

                    boost::string_ref implement_def;
                    boost::tie(implement_def, boost::tuples::ignore) = SplitQualifiedName(s);
                    bool implement_def_b;
                    boost::tie(boost::tuples::ignore, implement_def_b) =
                        found_defs.insert(RR_MOVE(implement_def.to_string()));
                    if (!implement_def_b)
                        continue;

                    RR_SHARED_PTR<ServiceDefinition> d2 = RRGetNode()->GetServiceType(implement_def)->ServiceDef();
                    bool version_b;
                    boost::tie(boost::tuples::ignore, version_b) = found_versions.insert(d2->StdVer);
                    if (!version_b)
                        continue;

                    object_type_ver.push_back(boost::make_tuple(d2->StdVer, s));

                    if (!d2->StdVer)
                        break;
                }
            }
        }

        RR_SHARED_PTR<IRRServiceObject> init_object = RR_DYNAMIC_POINTER_CAST<IRRServiceObject>(o);
        if (init_object)
        {
            init_object->RRServiceObjectInit(c, s.to_string());
        }

        ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Service, -1, s, "", "Service skel initialized successfully");
    }
    catch (std::exception& exp)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(node, Service, -1, s, "",
                                                "Initializing service skel failed: " << exp.what());
    }
}

RR_SHARED_PTR<RobotRaconteurNode> ServiceSkel::RRGetNode()
{
    RR_SHARED_PTR<RobotRaconteurNode> n = node.lock();
    if (!n)
        throw InvalidOperationException("Node has been released");
    return n;
}

RR_WEAK_PTR<RobotRaconteurNode> ServiceSkel::RRGetNodeWeak() { return node; }

void ServiceSkel::InitCallbackServers(RR_SHARED_PTR<RRObject> o) {}

std::string ServiceSkel::GetServicePath() const { return m_ServicePath; }

RR_SHARED_PTR<ServerContext> ServiceSkel::GetContext()
{
    RR_SHARED_PTR<ServerContext> out = m_context.lock();
    if (!out)
        throw InvalidOperationException("Skel has been released");
    return out;
}

RR_SHARED_PTR<RRObject> ServiceSkel::GetUncastObject() const { return uncastobj; }

RR_SHARED_PTR<RRObject> ServiceSkel::GetSubObj(boost::string_ref name)
{
    std::vector<std::string> s1;
    boost::split(s1, name, boost::is_from_range('[', '['));
    //= name.Split(std::vector<int8_t>(tempVector2, tempVector2 + sizeof(tempVector2) / sizeof(tempVector2[0])));
    if (s1.size() == 1)
    {
        return GetSubObj(name, "");
    }
    else
    {
        std::string ind = detail::decode_index(boost::replace_last_copy(s1.at(1), "]", ""));
        return GetSubObj(s1.at(0), ind);
    }
}

void ServiceSkel::RegisterEvents(RR_SHARED_PTR<RRObject> obj1) {}

void ServiceSkel::UnregisterEvents(RR_SHARED_PTR<RRObject> obj1) {}

void ServiceSkel::InitPipeServers(RR_SHARED_PTR<RRObject> obj1) {}

void ServiceSkel::InitWireServers(RR_SHARED_PTR<RRObject> obj1) {}

void ServiceSkel::ObjRefChanged(boost::string_ref name)
{
    std::string path = GetServicePath() + "." + name;
    GetContext()->ReplaceObject(path);
}

void ServiceSkel::EndAsyncCallGetProperty(RR_WEAK_PTR<ServiceSkel> skel, RR_INTRUSIVE_PTR<MessageElement> value,
                                          RR_SHARED_PTR<RobotRaconteurException> err, RR_INTRUSIVE_PTR<MessageEntry> m,
                                          RR_SHARED_PTR<ServerEndpoint> ep)
{
    RR_SHARED_PTR<ServiceSkel> skel1 = skel.lock();
    if (!skel1)
        return;

    ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(skel1->node, Service, ep->GetLocalEndpoint(), skel1->m_ServicePath,
                                            m->MemberName, "Begin EndAsyncCallGetProperty");

    try
    {
        RR_INTRUSIVE_PTR<MessageEntry> ret = CreateMessageEntry(MessageEntryType_PropertyGetRes, m->MemberName);
        ret->RequestID = m->RequestID;
        ret->ServicePath = m->ServicePath;

        if (err)
        {
            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(
                skel1->node, Service, ep->GetLocalEndpoint(), skel1->m_ServicePath, m->MemberName,
                "EndAsyncCallGetProperty returning caught exception to caller: " << err->what());
            RobotRaconteurExceptionUtil::ExceptionToMessageEntry(*err, ret);
        }
        else
        {
            if (!value)
                throw InternalErrorException("Internal error");
            value->ElementName = "value";
            ret->AddElement(value);
        }

        boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)> h =
            boost::bind(&rr_context_emptyhandler, RR_BOOST_PLACEHOLDERS(_1));
        skel1->GetContext()->AsyncSendMessage(ret, ep, h);
        ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(skel1->node, Service, ep->GetLocalEndpoint(), skel1->m_ServicePath,
                                                m->MemberName, "EndAsyncCallGetProperty completed successfully");
    }
    catch (std::exception& exp)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(skel1->node, Service, ep->GetLocalEndpoint(), skel1->m_ServicePath,
                                                m->MemberName, "EndAsyncCallGetProperty failed: " << exp.what());
        RobotRaconteurNode::TryHandleException(skel1->node, &exp);
    }
}

void ServiceSkel::EndAsyncCallSetProperty(RR_WEAK_PTR<ServiceSkel> skel, RR_SHARED_PTR<RobotRaconteurException> err,
                                          RR_INTRUSIVE_PTR<MessageEntry> m, RR_SHARED_PTR<ServerEndpoint> ep)
{
    RR_SHARED_PTR<ServiceSkel> skel1 = skel.lock();
    if (!skel1)
        return;

    ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(skel1->node, Service, ep->GetLocalEndpoint(), skel1->m_ServicePath,
                                            m->MemberName, "Begin EndAsyncCallSetProperty");

    try
    {
        RR_INTRUSIVE_PTR<MessageEntry> ret = CreateMessageEntry(MessageEntryType_PropertySetRes, m->MemberName);
        ret->RequestID = m->RequestID;
        ret->ServicePath = m->ServicePath;

        if (err)
        {
            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(
                skel1->node, Service, ep->GetLocalEndpoint(), skel1->m_ServicePath, m->MemberName,
                "EndAsyncCallSetProperty returning caught exception to caller: " << err->what());
            RobotRaconteurExceptionUtil::ExceptionToMessageEntry(*err, ret);
        }

        boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)> h =
            boost::bind(&rr_context_emptyhandler, RR_BOOST_PLACEHOLDERS(_1));
        skel1->GetContext()->AsyncSendMessage(ret, ep, h);
        ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(skel1->node, Service, ep->GetLocalEndpoint(), skel1->m_ServicePath,
                                                m->MemberName, "EndAsyncCallSetProperty completed successfully");
    }
    catch (std::exception& exp)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(skel1->node, Service, ep->GetLocalEndpoint(), skel1->m_ServicePath,
                                                m->MemberName, "EndAsyncCallGetProperty failed: " << exp.what());
        RobotRaconteurNode::TryHandleException(skel1->node, &exp);
    }
}

void ServiceSkel::EndAsyncCallFunction(RR_WEAK_PTR<ServiceSkel> skel, RR_INTRUSIVE_PTR<MessageElement> ret,
                                       RR_SHARED_PTR<RobotRaconteurException> err, RR_INTRUSIVE_PTR<MessageEntry> m,
                                       RR_SHARED_PTR<ServerEndpoint> ep)
{
    RR_SHARED_PTR<ServiceSkel> skel1 = skel.lock();
    if (!skel1)
        return;

    ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(skel1->node, Service, ep->GetLocalEndpoint(), skel1->m_ServicePath,
                                            m->MemberName, "Begin EndAsyncCallFunction");

    try
    {
        RR_INTRUSIVE_PTR<MessageEntry> ret1 = CreateMessageEntry(MessageEntryType_FunctionCallRes, m->MemberName);
        ret1->RequestID = m->RequestID;
        ret1->ServicePath = m->ServicePath;

        if (err)
        {
            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(
                skel1->node, Service, ep->GetLocalEndpoint(), skel1->m_ServicePath, m->MemberName,
                "EndAsyncCallFunction returning caught exception to caller: " << err->what());
            RobotRaconteurExceptionUtil::ExceptionToMessageEntry(*err, ret1);
        }
        else
        {
            if (!ret)
            {
                ret1->AddElement("return", ScalarToRRArray<int32_t>(0));
            }
            else
            {
                if (ret->ElementName != "index")
                {
                    ret->ElementName = "return";
                }
                ret1->AddElement(ret);
            }
        }

        boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)> h =
            boost::bind(&rr_context_emptyhandler, RR_BOOST_PLACEHOLDERS(_1));
        skel1->GetContext()->AsyncSendMessage(ret1, ep, h);
        ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(skel1->node, Service, ep->GetLocalEndpoint(), skel1->m_ServicePath,
                                                m->MemberName, "EndAsyncCallFunction completed successfully");
    }
    catch (std::exception& exp)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(skel1->node, Service, ep->GetLocalEndpoint(), skel1->m_ServicePath,
                                                m->MemberName, "EndAsyncCallFunction failed: " << exp.what());
        RobotRaconteurNode::TryHandleException(skel1->node, &exp);
    }
}

void ServiceSkel::SendEvent(RR_INTRUSIVE_PTR<MessageEntry> m)
{
    m->ServicePath = GetServicePath();
    GetContext()->SendEvent(m);
}

void ServiceSkel::ReleaseObject()
{
    ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Service, -1, m_ServicePath, "", "ServiceSkel releasing object");

    {
        boost::mutex::scoped_lock lock(monitorlocks_lock);
        BOOST_FOREACH (RR_SHARED_PTR<ServerContext_MonitorObjectSkel>& e, monitorlocks | boost::adaptors::map_values)
        {
            e->Shutdown();
        }
    }

    UnregisterEvents(uncastobj);
    ReleaseCastObject();
    uncastobj.reset();
}

void ServiceSkel::AsyncSendPipeMessage(RR_INTRUSIVE_PTR<MessageEntry> m, uint32_t e, bool unreliable,
                                       RR_MOVE_ARG(boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)>)
                                           handler)
{
    m->ServicePath = GetServicePath();

    if (!unreliable)
    {
        GetContext()->AsyncSendPipeMessage(m, e, unreliable, handler);
    }
    else
    {
        boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)> h =
            boost::bind(&rr_context_emptyhandler, RR_BOOST_PLACEHOLDERS(_1));
        GetContext()->AsyncSendPipeMessage(m, e, unreliable, h);
    }
}

void ServiceSkel::SendWireMessage(RR_INTRUSIVE_PTR<MessageEntry> m, uint32_t e)
{
    m->ServicePath = GetServicePath();
    GetContext()->SendWireMessage(m, e);
}

void ServiceSkel::DispatchPipeMessage(RR_INTRUSIVE_PTR<MessageEntry> m, uint32_t e)
{
    ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Service, e, m_ServicePath, m->MemberName,
                                            "Pipe packet received for nonexistant member");
}

void ServiceSkel::DispatchWireMessage(RR_INTRUSIVE_PTR<MessageEntry> m, uint32_t e)
{
    ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Service, e, m_ServicePath, m->MemberName,
                                            "Wire packet received for nonexistant member");
}

RR_INTRUSIVE_PTR<MessageEntry> ServiceSkel::CallPipeFunction(RR_INTRUSIVE_PTR<MessageEntry> m, uint32_t e)
{
    ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Service, e, m_ServicePath, m->MemberName,
                                            "Pipe command received for nonexistant member");
    throw MemberNotFoundException("Pipe " + m->MemberName.str() + " not found");
}

RR_INTRUSIVE_PTR<MessageEntry> ServiceSkel::CallWireFunction(RR_INTRUSIVE_PTR<MessageEntry> m, uint32_t e)
{
    ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Service, e, m_ServicePath, m->MemberName,
                                            "Wire command received for nonexistant member");
    throw MemberNotFoundException("Wire " + m->MemberName.str() + " not found");
}

RR_SHARED_PTR<void> ServiceSkel::GetCallbackFunction(uint32_t endpoint, boost::string_ref membername)
{
    ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Service, endpoint, m_ServicePath, membername,
                                            "Callback \"" << membername << "\" not found");
    throw MemberNotFoundException("Callback " + membername + " not found");
}

RR_INTRUSIVE_PTR<MessageEntry> ServiceSkel::CallMemoryFunction(RR_INTRUSIVE_PTR<MessageEntry> m,
                                                               RR_SHARED_PTR<Endpoint> e)
{
    ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Service, e->GetLocalEndpoint(), m_ServicePath, m->MemberName,
                                            "Memory request received for nonexistant member");
    throw MemberNotFoundException("Memory " + m->MemberName.str() + " not found");
}

bool ServiceSkel::IsLocked()
{
    boost::mutex::scoped_lock lock2(objectlock_lock);
    RR_SHARED_PTR<ServerContext_ObjectLock> lock = objectlock.lock();
    if (!lock)
        return false;

    return lock->IsLocked();
}

bool ServiceSkel::IsRequestNoLock(RR_INTRUSIVE_PTR<MessageEntry> m) { return false; }

bool ServiceSkel::IsMonitorLocked()
{
    // boost::mutex::scoped_lock lock2(monitorlocks_lock);
    if (!monitorlock)
        return false;
    return monitorlock->IsLocked();
}

std::string ServiceSkel::GetObjectType(RobotRaconteurVersion client_version)
{
    if (!client_version)
    {
        std::string object_type = GetObjectType();
        ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Service, -1, m_ServicePath, "",
                                                "GetObjectType returning \"" << object_type << "\"");
        return GetObjectType();
    }

    typedef boost::tuple<RobotRaconteurVersion, std::string> e_type;
    BOOST_FOREACH (e_type e, object_type_ver)
    {
        if (!e.get<0>())
        {
            ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(
                node, Service, -1, m_ServicePath, "",
                "GetObjectType returning \"" << e.get<1>() << "\" for client version " << client_version.ToString());
            return e.get<1>();
        }

        if (e.get<0>() <= client_version)
        {
            ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(
                node, Service, -1, m_ServicePath, "",
                "GetObjectType returning \"" << e.get<1>() << "\" for client version " << client_version.ToString());
            return e.get<1>();
        }
    }

    ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Service, -1, m_ServicePath, "",
                                            "GetObjectType could not determine a valid type for client version "
                                                << client_version.ToString() << ", newer clientversion required");
    throw ObjectNotFoundException("Service requires newer client version");
}

void ServiceSkel::CallGeneratorNext(RR_INTRUSIVE_PTR<MessageEntry> m, RR_SHARED_PTR<Endpoint> ep)
{
    ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Service, ep->GetLocalEndpoint(), m_ServicePath, m->MemberName,
                                            "CallGeneratorNext");
    int32_t index = RRArrayToScalar(m->FindElement("index")->CastData<RRArray<int32_t> >());
    RR_SHARED_PTR<GeneratorServerBase> gen;
    {
        boost::mutex::scoped_lock lock(generators_lock);
        boost::unordered_map<int32_t, RR_SHARED_PTR<GeneratorServerBase> >::iterator e = generators.find(index);
        if (e == generators.end())
        {
            throw InvalidOperationException("Invalid generator");
        }
        gen = e->second;
        gen->last_access_time = boost::posix_time::second_clock::universal_time();
    }

    if (gen->GetEndpoint() != ep->GetLocalEndpoint())
    {
        throw InvalidOperationException("Invalid generator");
    }
    gen->CallNext(m);
}

void ServiceSkel::SendGeneratorResponse(int32_t index, RR_INTRUSIVE_PTR<MessageEntry> m,
                                        RR_SHARED_PTR<ServerEndpoint> ep)
{
    ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Service, ep->GetLocalEndpoint(), m_ServicePath, m->MemberName,
                                            "SendGeneratorResponse generator id: " << index);
    if (m->Error != MessageErrorType_None)
    {
        RR_SHARED_PTR<GeneratorServerBase> gen;
        {
            boost::mutex::scoped_lock lock(generators_lock);
            boost::unordered_map<int32_t, RR_SHARED_PTR<GeneratorServerBase> >::iterator e = generators.find(index);
            if (e == generators.end())
            {
                throw InvalidOperationException("Invalid generator");
            }
            gen = e->second;
        }
    }

    boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)> h =
        boost::bind(&rr_context_emptyhandler, RR_BOOST_PLACEHOLDERS(_1));
    GetContext()->AsyncSendMessage(m, ep, h);
}

int32_t ServiceSkel::get_new_generator_index()
{
    RR_SHARED_PTR<RobotRaconteurNode> node = RRGetNode();
    int32_t index;
    do
    {
        index = node->GetRandomInt<int32_t>(0, std::numeric_limits<int32_t>::max());
    } while (generators.find(index) != generators.end());
    return index;
}

void ServiceSkel::CleanupGenerators()
{
    boost::posix_time::ptime destroy_time =
        boost::posix_time::second_clock::universal_time() - boost::posix_time::minutes(10);
    boost::mutex::scoped_lock lock(generators_lock);
    for (boost::unordered_map<int32_t, RR_SHARED_PTR<GeneratorServerBase> >::iterator e = generators.begin();
         e != generators.end();)
    {
        if (e->second->last_access_time < destroy_time)
        {
            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(node, Service, e->second->GetEndpoint(), m_ServicePath, "",
                                                    "Destroying generator id " << e->first << " due to timeout");
            e = generators.erase(e);
        }
        else
        {
            ++e;
        }
    }
}

RR_SHARED_PTR<ServiceFactory> ServerContext::GetServiceDef() const { return m_ServiceDef; }

RR_SHARED_PTR<ServiceFactory> ServerContext::GetRootObjectServiceDef(RobotRaconteurVersion client_version)
{
    std::string root_object_type = GetRootObjectType(client_version);
    boost::string_ref root_object_def;
    boost::tie(root_object_def, boost::tuples::ignore) = SplitQualifiedName(root_object_type);
    return GetNode()->GetServiceType(root_object_def);
}

std::vector<std::string> ServerContext::GetExtraImports()
{
    boost::mutex::scoped_lock lock(extra_imports_lock);
    return extra_imports;
}

void ServerContext::AddExtraImport(boost::string_ref import_)
{
    GetNode()->GetServiceType(import_);

    boost::mutex::scoped_lock lock(extra_imports_lock);

    if (boost::range::find(extra_imports, import_) != extra_imports.end())
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(node, Service, -1, GetServiceName(), "",
                                                "Extra import \"" << import_ << "\" already added");
        throw InvalidArgumentException("Extra import already added");
    }

    extra_imports.push_back(RR_MOVE(import_.to_string()));
    ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Service, -1, GetServiceName(), "",
                                            "Extra import \"" << import_ << "\" added");
}

bool ServerContext::RemoveExtraImport(boost::string_ref import_)
{
    boost::mutex::scoped_lock lock(extra_imports_lock);

    std::vector<std::string>::iterator e = boost::range::find(extra_imports, import_);

    if (e == extra_imports.end())
    {
        return false;
    }

    extra_imports.erase(e);
    ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Service, -1, GetServiceName(), "",
                                            "Extra import \"" << import_ << "\" removed");
    return true;
}

std::string ServerContext::GetServiceName() const { return m_ServiceName; }

std::string ServerContext::GetRootObjectType(RobotRaconteurVersion client_version)
{
    return GetObjectType(m_ServiceName, client_version);
}

ServerContext::ServerContext(RR_SHARED_PTR<ServiceFactory> f, RR_SHARED_PTR<RobotRaconteurNode> node)
{
    InitializeInstanceFields();
    m_ServiceDef = f;
    this->node = node;

    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Client, -1,
                                       "ServerContext created with service type \"" << f->GetServiceName() << "\"");
}

RR_SHARED_PTR<RobotRaconteurNode> ServerContext::GetNode()
{
    RR_SHARED_PTR<RobotRaconteurNode> n = node.lock();
    if (!n)
        throw InvalidOperationException("Node has been released");
    return n;
}

void ServerContext::SendEvent(RR_INTRUSIVE_PTR<MessageEntry> m)
{

    try
    {
        RR_INTRUSIVE_PTR<Message> mm = CreateMessage();

        std::vector<RR_SHARED_PTR<ServerEndpoint> > cc;

        {
            {
                boost::mutex::scoped_lock lock(client_endpoints_lock);
                boost::copy(client_endpoints | boost::adaptors::map_values, std::back_inserter(cc));
            }

            BOOST_FOREACH (RR_SHARED_PTR<RobotRaconteur::ServerEndpoint>& c, cc)
            {

                if (m_RequireValidUser)
                {
                    try
                    {
                        if (c->GetAuthenticatedUsername() == "")
                        {
                            ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(
                                node, Service, c->GetLocalEndpoint(), m->ServicePath, m->MemberName,
                                "Skipping sending event due to authentication failure");
                            continue;
                        }
                    }
                    catch (AuthenticationException&)
                    {
                        ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Service, c->GetLocalEndpoint(), m->ServicePath,
                                                                m->MemberName,
                                                                "Skipping sending event due to authentication failure");
                        continue;
                    }
                }

                RR_INTRUSIVE_PTR<MessageEntry> m2;
                try
                {
                    m2 = ShallowCopyMessageEntry(m);
                }
                catch (std::exception& exp2)
                {
                    ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(node, Service, c->GetLocalEndpoint(), m->ServicePath,
                                                            m->MemberName,
                                                            "ShallowCopyMessage failed: " << exp2.what());
                    RobotRaconteurNode::TryHandleException(node, &exp2);
                    continue;
                }

                try
                {
                    GetNode()->CheckConnection(c->GetLocalEndpoint());
                    boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)> h =
                        boost::bind(&rr_context_emptyhandler, RR_BOOST_PLACEHOLDERS(_1));
                    AsyncSendMessage(m2, c, h);
                }
                catch (std::exception& exp2)
                {
                    ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(node, Service, c->GetLocalEndpoint(), m->ServicePath,
                                                            m->MemberName,
                                                            "Sending event to client failed: " << exp2.what());
                    try
                    {
                        RemoveClient(c);
                    }
                    catch (std::exception&)
                    {};
                }
            }
        }
    }
    catch (std::exception& exp)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(node, Service, -1, m->ServicePath, m->MemberName,
                                                "Error sending event: " << exp.what());
    }
}
#undef SendMessage

void ServerContext::SendMessage(RR_INTRUSIVE_PTR<MessageEntry> m, uint32_t e)
{

    RR_SHARED_PTR<ServerEndpoint> s;
    {
        boost::mutex::scoped_lock lock(client_endpoints_lock);
        RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<ServerEndpoint> >::iterator e1 = client_endpoints.find(e);
        if (e1 == client_endpoints.end())
        {
            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(node, Service, e, m->ServicePath, m->MemberName,
                                                    "Attempt to send message to invalid endpoint");
            throw InvalidEndpointException("Invalid client endpoint");
        }
        s = e1->second;
    }
    SendMessage(m, s);
}

void ServerContext::SendMessage(RR_INTRUSIVE_PTR<MessageEntry> m, RR_SHARED_PTR<Endpoint> e)
{
    // m.ServicePath = ServiceName;

    RR_INTRUSIVE_PTR<Message> mm = CreateMessage();
    mm->header = CreateMessageHeader();
    // mm.header.ReceiverEndpoint = RemoteEndpoint;
    mm->entries.push_back(m);

    e->SendMessage(mm);
}

void ServerContext::AsyncSendMessage(RR_INTRUSIVE_PTR<MessageEntry> m, uint32_t e,
                                     boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)>& callback)
{

    RR_SHARED_PTR<ServerEndpoint> s;
    {
        boost::mutex::scoped_lock lock(client_endpoints_lock);
        RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<ServerEndpoint> >::iterator e1 = client_endpoints.find(e);
        if (e1 == client_endpoints.end())
        {
            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(node, Service, e, m->ServicePath, m->MemberName,
                                                    "Attempt to send message to invalid endpoint");
            throw InvalidEndpointException("Invalid client endpoint");
        }
        s = e1->second;
    }

    AsyncSendMessage(m, s, (callback));
}

void ServerContext::AsyncSendMessage(RR_INTRUSIVE_PTR<MessageEntry> m, RR_SHARED_PTR<Endpoint> e,
                                     boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)>& callback)
{

    // m.ServicePath = ServiceName;

    RR_INTRUSIVE_PTR<Message> mm = CreateMessage();
    mm->header = CreateMessageHeader();
    // mm.header.ReceiverEndpoint = RemoteEndpoint;
    mm->entries.push_back(m);

    e->AsyncSendMessage(mm, callback);
}

void ServerContext::AsyncSendUnreliableMessage(RR_INTRUSIVE_PTR<MessageEntry> m, RR_SHARED_PTR<ServerEndpoint> e,
                                               boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)>& callback)
{
    RR_INTRUSIVE_PTR<Message> mm = CreateMessage();
    mm->header = CreateMessageHeader();
    // mm.header.ReceiverEndpoint = RemoteEndpoint;
    mm->entries.push_back(m);
    mm->header->MetaData = "unreliable\n";
    e->AsyncSendMessage(mm, callback);
}

void ServerContext::AsyncSendPipeMessage(RR_INTRUSIVE_PTR<MessageEntry> m, uint32_t e, bool unreliable,
                                         boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)>& callback)
{
    if (!unreliable)
    {
        ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Service, e, m->ServicePath, m->MemberName,
                                                "Service sending reliable pipe packet EntryType " << m->EntryType);
        AsyncSendMessage(m, e, (callback));
    }
    else
    {
        ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Service, e, m->ServicePath, m->MemberName,
                                                "Service sending unreliable pipe packet EntryType " << m->EntryType);
        RR_SHARED_PTR<ServerEndpoint> s;
        {
            boost::mutex::scoped_lock lock(client_endpoints_lock);
            RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<ServerEndpoint> >::iterator e1 = client_endpoints.find(e);
            if (e1 == client_endpoints.end())
            {
                ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(node, Service, e, m->ServicePath, m->MemberName,
                                                        "Attempt to send message to invalid endpoint");
                throw InvalidEndpointException("Invalid client endpoint");
            }
            s = e1->second;
        }

        AsyncSendUnreliableMessage(m, s, (callback));
    }
}

void ServerContext::SendWireMessage(RR_INTRUSIVE_PTR<MessageEntry> m, uint32_t e)
{
    ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Service, e, m->ServicePath, m->MemberName,
                                            "Service sending reliable wire packet EntryType " << m->EntryType);
    RR_SHARED_PTR<ServerEndpoint> s;
    {
        boost::mutex::scoped_lock lock(client_endpoints_lock);
        RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<ServerEndpoint> >::iterator e1 = client_endpoints.find(e);
        if (e1 == client_endpoints.end())
        {
            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(node, Service, e, m->ServicePath, m->MemberName,
                                                    "Attempt to send message to invalid endpoint");
            throw InvalidEndpointException("Invalid client endpoint");
        }
        s = e1->second;
    }
    boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)> h =
        boost::bind(&rr_context_emptyhandler, RR_BOOST_PLACEHOLDERS(_1));
    AsyncSendUnreliableMessage(m, s, h);
}

void ServerContext::SetSecurityPolicy(RR_SHARED_PTR<ServiceSecurityPolicy> policy)
{
    user_authenticator = policy->Authenticator;
    security_policies = policy->Policies;

    if (security_policies.count("requirevaliduser") != 0)
    {
        if (boost::algorithm::to_lower_copy(security_policies.at("requirevaliduser")) == "true")
        {
            m_RequireValidUser = true;
            ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Service, -1, GetServiceName(), "",
                                                    "\"requirevaliduser\" policy enabled");
        }
    }

    if (security_policies.count("allowobjectlock") != 0)
    {
        if (boost::algorithm::to_lower_copy(security_policies.at("allowobjectlock")) == "true")
        {
            AllowObjectLock = true;
            ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Service, -1, GetServiceName(), "",
                                                    "\"requirevaliduser\" policy enabled");
        }
    }
}

void ServerContext::SetBaseObject(boost::string_ref name, RR_SHARED_PTR<RRObject> o,
                                  RR_SHARED_PTR<ServiceSecurityPolicy> policy)
{
    if (base_object_set)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(node, Service, -1, name, "",
                                                "Attempt to set service root object when it is already set");
        throw InvalidOperationException("Base object already set");
    }

    try
    {
        m_ServiceName = RR_MOVE(name.to_string());

        if (policy != 0)
        {
            SetSecurityPolicy(policy);
        }

        m_CurrentServicePath.reset(new std::string(name.to_string()));
        m_CurrentServerContext.reset(new RR_SHARED_PTR<ServerContext>(shared_from_this()));

        RR_SHARED_PTR<ServiceSkel> s = GetServiceDef()->CreateSkel(o->RRType(), name, o, shared_from_this());

        m_RootObjectType = RR_MOVE(o->RRType()); // boost::algorithm::replace_all_copy(o->RRType(),"::",".");
        base_object_set = true;

        {
            boost::mutex::scoped_lock lock(skels_lock);
            skels.insert(std::make_pair(name, s));
        }

        m_CurrentServicePath.reset(0);

        m_CurrentServerContext.reset(0);

        ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Service, -1, name, "",
                                                "SetBaseObject completed successfully for service \""
                                                    << name << "\" with root object type \"" << m_RootObjectType
                                                    << "\"");
    }
    catch (std::exception& exp)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(node, Service, -1, name, "", "SetBaseObject failed: " << exp.what());
        throw;
    }
}

RR_SHARED_PTR<ServiceSkel> ServerContext::GetObjectSkel(MessageStringRef servicepath)
{

    ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Service, -1, servicepath, "", "GetObjectSkel");

    // object obj = null;

    try
    {
        static boost::regex r_service_path("^[a-zA-Z](?:\\w*[a-zA-Z0-9])?(\\.[a-zA-Z](?:\\w*[a-zA-Z0-9])?(?:\\[(?:[a-"
                                           "zA-Z0-9_]|\\%[0-9a-fA-F]{2})+\\])?)*$");

        std::vector<std::string> p;
        boost::string_ref servicepath1 = servicepath.str();
        if (!boost::regex_match(servicepath1.begin(), servicepath1.end(), r_service_path))
        {
            throw ServiceException("Invalid service path specified");
        }
        boost::split(p, servicepath1, boost::is_from_range('.', '.'));

        std::string ppath = p.at(0);

        RR_SHARED_PTR<ServiceSkel> skel;
        {
            boost::mutex::scoped_lock lock(skels_lock);

            RR_UNORDERED_MAP<MessageStringPtr, RR_SHARED_PTR<ServiceSkel> >::iterator e1 = skels.find(servicepath);
            if (e1 != skels.end())
                return e1->second;

            skel = skels.at(ppath);
        }
        // obj = skel.uncastobj;

        RR_SHARED_PTR<ServiceSkel> skel1 = skel;

        for (size_t i = 1; i < p.size(); i++)
        {
            {
                boost::mutex::scoped_lock lock(skels_lock);
                std::string ppath1 = ppath;

                ppath = ppath + "." + p.at(i);

                skel1.reset();
                RR_UNORDERED_MAP<MessageStringPtr, RR_SHARED_PTR<ServiceSkel> >::iterator e1 = skels.find(ppath);
                if (e1 != skels.end())
                {
                    skel1 = e1->second;
                }

                if (skel1 == 0)
                {

                    m_CurrentServicePath.reset(new std::string(ppath1));
                    m_CurrentServerContext.reset(new RR_SHARED_PTR<ServerContext>(shared_from_this()));
                    RR_SHARED_PTR<RRObject> obj1 = skel->GetSubObj(p.at(i));

                    m_CurrentServicePath.reset(0);

                    m_CurrentServerContext.reset(0);

                    if (!obj1)
                    {
                        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(node, Service, -1, GetServiceName(), servicepath,
                                                                "Requested object is null");
                        throw ServiceException("Requested object is null");
                    }

                    skel1 = GetServiceDef()->CreateSkel(obj1->RRType(), ppath, obj1, shared_from_this());
                    boost::mutex::scoped_lock lock2(skel->objectlock_lock);
                    RR_SHARED_PTR<ServerContext_ObjectLock> lock = skel->objectlock.lock();
                    if (lock)
                    {
                        lock->AddSkel(skel1);
                    }
                    skels.insert(std::make_pair(ppath, skel1));
                }

                skel = skel1;
            }
        }

        return skel;
    }
    catch (std::exception& exp)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(node, Service, -1, GetServiceName(), servicepath,
                                                "GetObjectSkel failed: " << exp.what());
        throw;
    }
}

void ServerContext::ReplaceObject(boost::string_ref path) { ReleaseServicePath(path); }

std::string ServerContext::GetObjectType(MessageStringRef servicepath, RobotRaconteurVersion client_version)
{
    try
    {
        // TODO: check client_version
        if (servicepath != GetServiceName())
        {
            if (m_RequireValidUser)
            {
                if (ServerEndpoint::GetCurrentAuthenticatedUser() == 0)
                    throw PermissionDeniedException("User must authenticate before accessing this service");
            }
        }

        RR_SHARED_PTR<ServiceSkel> s = GetObjectSkel(servicepath);

        return s->GetObjectType(client_version);
    }
    catch (std::exception& exp)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(node, Service, -1, GetServiceName(), "",
                                                "GetObjectType failed: " << exp.what());
        throw;
    }
}

RR_SHARED_PTR<ServerContext> ServerContext::GetCurrentServerContext()
{
    if (!m_CurrentServerContext.get())
        throw InvalidOperationException("Current server context not set");
    RR_SHARED_PTR<ServerContext> out = *m_CurrentServerContext.get();
    if (!out)
        throw InvalidOperationException("Current server context not set");
    return out;
}

boost::thread_specific_ptr<RR_SHARED_PTR<ServerContext> > ServerContext::m_CurrentServerContext;

std::string ServerContext::GetCurrentServicePath()
{
    if (m_CurrentServicePath.get() == 0)
        throw InvalidOperationException("Current server context not set");
    return std::string(*m_CurrentServicePath);
}

boost::thread_specific_ptr<std::string> ServerContext::m_CurrentServicePath;

RR_INTRUSIVE_PTR<MessageEntry> ServerContext::ProcessMessageEntry(RR_INTRUSIVE_PTR<MessageEntry> m,
                                                                  RR_SHARED_PTR<ServerEndpoint> c)
{

    ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Service, c->GetLocalEndpoint(), m->ServicePath, m->MemberName,
                                            "Begin ProcessMessageEntry with EntryType " << m->EntryType);

    bool noreturn = false;
    RR_INTRUSIVE_PTR<MessageEntry> ret = RR_INTRUSIVE_PTR<MessageEntry>();

    if (m->EntryType == MessageEntryType_ServicePathReleasedRet)
    {
        ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Service, c->GetLocalEndpoint(), m->ServicePath, m->MemberName,
                                                "Received ServicePathReleasedRet");
        return RR_INTRUSIVE_PTR<MessageEntry>();
    }

    try
    {
        // ClientSessionOp methods
        if (m->EntryType == MessageEntryType_ClientSessionOpReq)
        {
            return ClientSessionOp(m, c);
        }

        if (m->EntryType == MessageEntryType_ClientKeepAliveReq)
        {
            ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Service, c->GetLocalEndpoint(), m->ServicePath, m->MemberName,
                                                    "Received ClientKeepAlive request, sending response");
            ret = CreateMessageEntry(MessageEntryType_ClientKeepAliveRet, m->MemberName);
            ret->RequestID = m->RequestID;
            ret->ServicePath = m->ServicePath;
            return ret;
        }

        if (m->EntryType == MessageEntryType_ServiceCheckCapabilityReq)
        {
            ret = CheckServiceCapability(m, c);
        }

        if (m_RequireValidUser)
        {
            if (ServerEndpoint::GetCurrentAuthenticatedUser() == 0)
            {
                ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(
                    node, Service, c->GetLocalEndpoint(), m->ServicePath, m->MemberName,
                    "User attempted to access service without authenticating using EntryType " << m->EntryType);
                throw PermissionDeniedException("User must authenticate before accessing this service");
            }
        }

        if (m->EntryType == MessageEntryType_PipePacket || m->EntryType == MessageEntryType_PipePacketRet)
        {
            ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Service, c->GetLocalEndpoint(), m->ServicePath, m->MemberName,
                                                    "Received PipePacket, dispatching to member");
            GetObjectSkel(m->ServicePath)->DispatchPipeMessage(m, c->GetLocalEndpoint());
            ret.reset();
            noreturn = true;
        }

        if (m->EntryType == MessageEntryType_WirePacket)
        {
            ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Service, c->GetLocalEndpoint(), m->ServicePath, m->MemberName,
                                                    "Received WirePacket, dispatching to member");
            GetObjectSkel(m->ServicePath)->DispatchWireMessage(m, c->GetLocalEndpoint());
            ret.reset();
            noreturn = true;
        }

        m_CurrentServicePath.reset(new std::string(m->ServicePath.str().to_string()));
        m_CurrentServerContext.reset(new RR_SHARED_PTR<ServerContext>(shared_from_this()));

        if (m->EntryType == MessageEntryType_ObjectTypeName)
        {
            RobotRaconteurVersion v;
            RR_INTRUSIVE_PTR<MessageElement> m_ver;
            if (m->TryFindElement("clientversion", m_ver))
            {
                v.FromString(m_ver->CastDataToString());
                ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Service, c->GetLocalEndpoint(), m->ServicePath,
                                                        m->MemberName,
                                                        "Received ObjectTypeName with clientversion: " << v);
            }
            else
            {
                ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Service, c->GetLocalEndpoint(), m->ServicePath,
                                                        m->MemberName, "Received ObjectTypeName without clientversion");
            }

            ret = CreateMessageEntry(MessageEntryType_ObjectTypeNameRet, m->MemberName);
            std::string objtype = GetObjectType(m->ServicePath, v);
            ret->AddElement("objecttype", stringToRRArray(objtype));
        }

        // Object member methods

        if (m->EntryType == MessageEntryType_PropertyGetReq)
        {
            ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Service, c->GetLocalEndpoint(), m->ServicePath, m->MemberName,
                                                    "Received PropertyGet, dispatching to member");
            RR_SHARED_PTR<ServiceSkel> skel = GetObjectSkel(m->ServicePath);
            check_lock(skel, m);
            ret = skel->CallGetProperty(m);
            if (!ret)
                noreturn = true;
        }

        if (m->EntryType == MessageEntryType_PropertySetReq)
        {
            ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Service, c->GetLocalEndpoint(), m->ServicePath, m->MemberName,
                                                    "Received PropertySet, dispatching to member");
            RR_SHARED_PTR<ServiceSkel> skel = GetObjectSkel(m->ServicePath);
            check_lock(skel, m);
            ret = skel->CallSetProperty(m);
            if (!ret)
                noreturn = true;
        }

        if (m->EntryType == MessageEntryType_FunctionCallReq)
        {
            ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Service, c->GetLocalEndpoint(), m->ServicePath, m->MemberName,
                                                    "Received FunctionCall, dispatching to member");
            RR_SHARED_PTR<ServiceSkel> skel = GetObjectSkel(m->ServicePath);
            check_lock(skel, m);
            ret = skel->CallFunction(m);
            if (!ret)
                noreturn = true;
        }

        if (m->EntryType == MessageEntryType_PipeConnectReq || m->EntryType == MessageEntryType_PipeDisconnectReq)
        {
            ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Service, c->GetLocalEndpoint(), m->ServicePath, m->MemberName,
                                                    "Received PipeCommand, dispatching to member");
            RR_SHARED_PTR<ServiceSkel> skel = GetObjectSkel(m->ServicePath);
            check_lock(skel, m);
            ret = skel->CallPipeFunction(m, c->GetLocalEndpoint());
        }

        if (m->EntryType == MessageEntryType_WireConnectReq || m->EntryType == MessageEntryType_WireDisconnectReq ||
            m->EntryType == MessageEntryType_WirePeekInValueReq ||
            m->EntryType == MessageEntryType_WirePeekOutValueReq ||
            m->EntryType == MessageEntryType_WirePokeOutValueReq)
        {
            ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Service, c->GetLocalEndpoint(), m->ServicePath, m->MemberName,
                                                    "Received WireCommand, dispatching to member");
            RR_SHARED_PTR<ServiceSkel> skel = GetObjectSkel(m->ServicePath);
            check_lock(skel, m);
            ret = skel->CallWireFunction(m, c->GetLocalEndpoint());
        }

        if (m->EntryType == MessageEntryType_MemoryWrite || m->EntryType == MessageEntryType_MemoryRead ||
            m->EntryType == MessageEntryType_MemoryGetParam)
        {
            ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Service, c->GetLocalEndpoint(), m->ServicePath, m->MemberName,
                                                    "Received MemoryCommand, dispatching to member");
            RR_SHARED_PTR<ServiceSkel> skel = GetObjectSkel(m->ServicePath);
            check_lock(skel, m);
            ret = skel->CallMemoryFunction(m, c);
        }

        else if (m->EntryType == MessageEntryType_CallbackCallRet)
        {
            // Console.WriteLine("Got " + m.RequestID + " " + m.EntryType + " " + m.MemberName);
            noreturn = true;
            RR_SHARED_PTR<outstanding_request> t;
            uint32_t requestid = m->RequestID;
            {

                boost::mutex::scoped_lock lock(outstanding_requests_lock);
                uint32_t requestid = m->RequestID;
                RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<outstanding_request> >::iterator e1 =
                    outstanding_requests.find(m->RequestID);
                if (e1 == outstanding_requests.end())
                {
                    noreturn = true;
                    return ret;
                }
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
                noreturn = true;
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
                        }

                        if (m->Error == MessageErrorType_None)
                        {
                            ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(
                                node, Service, c->GetLocalEndpoint(), m->ServicePath, m->MemberName,
                                "ProcessCallbackRequest with requestid " << requestid << " successful");
                            t->handler(m, RR_SHARED_PTR<RobotRaconteurException>());
                        }
                        else if (m->Error == MessageErrorType_RemoteError)
                        {
                            RR_SHARED_PTR<RobotRaconteurException> err =
                                RobotRaconteurExceptionUtil::MessageEntryToException(m);
                            RR_SHARED_PTR<RobotRaconteurException> err2 = m_ServiceDef->DownCastException(err);
                            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(
                                node, Service, c->GetLocalEndpoint(), m->ServicePath, m->MemberName,
                                "Client returned remote error during ProcessCallbackRequest: " << err->what());
                            t->handler(RR_INTRUSIVE_PTR<MessageEntry>(), err2);
                        }
                        else
                        {
                            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(
                                node, Service, c->GetLocalEndpoint(), m->ServicePath, m->MemberName,
                                "Client returned error during ProcessCallbackRequest: " << m->Error);
                            t->handler(RR_INTRUSIVE_PTR<MessageEntry>(),
                                       RobotRaconteurExceptionUtil::MessageEntryToException(m));
                        }
                    }
                }
                catch (std::exception& exp2)
                {
                    ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(
                        node, Service, c->GetLocalEndpoint(), m->ServicePath, m->MemberName,
                        "Error handling AsyncProcessCallbackRequest response: " << exp2.what());
                    RobotRaconteurNode::TryHandleException(node, &exp2);
                }
            }
        }

        if (m->EntryType == MessageEntryType_GeneratorNextReq)
        {
            ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Service, c->GetLocalEndpoint(), m->ServicePath, m->MemberName,
                                                    "Received GeneratorNext, dispatching to member");
            RR_SHARED_PTR<ServiceSkel> skel = GetObjectSkel(m->ServicePath);
            check_lock(skel, m);
            skel->CallGeneratorNext(m, c);
            noreturn = true;
        }
    }
    catch (std::exception& e)
    {
        if (!noreturn)
        {
            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(
                node, Service, c->GetLocalEndpoint(), m->ServicePath, m->MemberName,
                "ProcessMessageEntry returning caught exception to caller: " << e.what());
            ret = CreateMessageEntry((static_cast<MessageEntryType>(m->EntryType + 1)), m->MemberName);
            RobotRaconteurExceptionUtil::ExceptionToMessageEntry(e, ret);
        }
        else
        {
            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(node, Service, c->GetLocalEndpoint(), m->ServicePath, m->MemberName,
                                                    "ProcessMessageEntry caught exception: " << e.what());
        }
    }
    catch (...)
    {
        if (!noreturn)
        {
            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(node, Service, c->GetLocalEndpoint(), m->ServicePath, m->MemberName,
                                                    "ProcessMessageEntry returning unknown exception to caller");
            ret = CreateMessageEntry((static_cast<MessageEntryType>(m->EntryType + 1)), m->MemberName);
            ret->Error = MessageErrorType_RemoteError;
            ret->AddElement("errorname", stringToRRArray("std::exception"));
            ret->AddElement("errorstring", stringToRRArray("Unknown exception occured in remote service"));
        }
        else
        {
            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(node, Service, c->GetLocalEndpoint(), m->ServicePath, m->MemberName,
                                                    "ProcessMessageEntry caught unknown exception");
        }
    }

    m_CurrentServicePath.reset(0);

    m_CurrentServerContext.reset(0);

    if (ret == 0 && !noreturn)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(
            node, Service, c->GetLocalEndpoint(), m->ServicePath, m->MemberName,
            "ProcessMessageEntry returning unknown request type exception to caller");
        ret = CreateMessageEntry(static_cast<MessageEntryType>(m->EntryType + 1), m->MemberName);
        ret->Error = MessageErrorType_ProtocolError;
        ret->AddElement("errorname", stringToRRArray("RobotRaconteur.ProtocolError"));
        ret->AddElement("errorstring", stringToRRArray("Unknown request type"));
    }

    if (!noreturn)
    {
        ret->ServicePath = m->ServicePath;
        ret->RequestID = m->RequestID;

        ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Service, c->GetLocalEndpoint(), m->ServicePath, m->MemberName,
                                                "ProcessMessageEntry completed successfully with return");
    }
    else
    {
        ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Service, c->GetLocalEndpoint(), m->ServicePath, m->MemberName,
                                                "ProcessMessageEntry completed successfully without return");
    }

    return ret;
}

void ServerContext::AsyncProcessCallbackRequest(
    RR_INTRUSIVE_PTR<MessageEntry> m, uint32_t endpoint,
    RR_MOVE_ARG(boost::function<void(RR_INTRUSIVE_PTR<MessageEntry>, RR_SHARED_PTR<RobotRaconteurException>)>) handler,
    int32_t timeout)
{
    try
    {
        uint32_t myrequestid;

        RR_SHARED_PTR<outstanding_request> t = RR_MAKE_SHARED<outstanding_request>();
        t->handler = handler;
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

            if (timeout != RR_TIMEOUT_INFINITE)
            {
                RR_SHARED_PTR<Timer> timer = GetNode()->CreateTimer(
                    boost::posix_time::milliseconds(timeout),
                    boost::bind(&ServerContext::AsyncProcessCallbackRequest_timeout, shared_from_this(),
                                RR_BOOST_PLACEHOLDERS(_1), endpoint, myrequestid),
                    true);
                timer->Start();
                t->timer = timer;
            }

            outstanding_requests.insert(std::make_pair(myrequestid, t));
        }

        ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Service, endpoint, m->ServicePath, m->MemberName,
                                                "AsyncProcessCallbackRequest sending message with requestid "
                                                    << myrequestid << " EntryType " << m->EntryType);

        boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)> h =
            boost::bind(&ServerContext::AsyncProcessCallbackRequest_err, shared_from_this(), RR_BOOST_PLACEHOLDERS(_1),
                        endpoint, myrequestid);
        AsyncSendMessage(m, endpoint, h);
    }
    catch (std::exception& exp)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(node, Service, endpoint, m->ServicePath, m->MemberName,
                                                "Error during AsyncProcessCallbackRequest: " << exp.what());
        throw;
    }
}

void ServerContext::AsyncProcessCallbackRequest_err(RR_SHARED_PTR<RobotRaconteurException> error, uint32_t endpoint,
                                                    uint32_t requestid)
{

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

        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(node, Service, endpoint, GetServiceName(), "",
                                                "Error during AsyncProcessCallbackRequest with requestid "
                                                    << requestid << ": " << error->what());

        detail::InvokeHandlerWithException(node, t->handler, error);
    }
    catch (std::exception& exp)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(
            node, Service, endpoint, GetServiceName(), "",
            "Failed to handle error during AsyncProcessCallbackRequest: " << exp.what());
        RobotRaconteurNode::TryHandleException(node, &exp);
    }
}

void ServerContext::AsyncProcessCallbackRequest_timeout(const TimerEvent& error, uint32_t endpoint, uint32_t requestid)
{
    if (!error.stopped)
    {
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

            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(node, Service, endpoint, GetServiceName(), "",
                                                    "AsyncProcessCallbackRequest with requestid " << requestid
                                                                                                  << " timed out");

            detail::InvokeHandlerWithException(node, t->handler,
                                               RR_MAKE_SHARED<RequestTimeoutException>("Request timed out"));
        }
        catch (std::exception& exp)
        {
            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(
                node, Service, endpoint, GetServiceName(), "",
                "Failed to handle error during AsyncProcessCallbackRequest: " << exp.what());
            RobotRaconteurNode::TryHandleException(node, &exp);
        }
    }
}

void ServerContext::Close()
{

    ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Service, -1, GetServiceName(), "",
                                            "Begin close service \"" << GetServiceName() << "\"");

    try
    {
        RR_INTRUSIVE_PTR<MessageEntry> e = CreateMessageEntry(MessageEntryType_ServiceClosed, "");
        SendEvent(e);
    }
    catch (std::exception&)
    {};

    {
        boost::mutex::scoped_lock lock(outstanding_requests_lock);
        BOOST_FOREACH (RR_SHARED_PTR<outstanding_request> e, outstanding_requests | boost::adaptors::map_values)
        {
            e->evt->Set();
        }
    }

    {
        boost::mutex::scoped_lock lock(outstanding_requests_lock);
        outstanding_requests.clear();
    }

    RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<ServerEndpoint> > endpoints;
    {
        boost::mutex::scoped_lock lock(client_endpoints_lock);

        endpoints = client_endpoints;
    }

    BOOST_FOREACH (RR_SHARED_PTR<ServerEndpoint>& ee, endpoints | boost::adaptors::map_values)
    {
        try
        {
            RemoveClient(ee);
        }
        catch (std::exception&)
        {}
    }

    {
        boost::mutex::scoped_lock lock(client_endpoints_lock);
        client_endpoints.clear();
    }

    {
        boost::mutex::scoped_lock lock(skels_lock);
        BOOST_FOREACH (RR_SHARED_PTR<ServiceSkel>& s, skels | boost::adaptors::map_values)
        {
            try
            {
                s->ReleaseObject();
            }
            catch (std::exception&)
            {};
        }
    }

    {
        boost::mutex::scoped_lock lock(monitor_thread_pool_lock);
        try
        {
            if (monitor_thread_pool)
                monitor_thread_pool->Shutdown();
            monitor_thread_pool.reset();
        }
        catch (std::exception&)
        {}
    }

    try
    {

        ServerServiceListener(shared_from_this(), ServerServiceListenerEventType_ServiceClosed,
                              RR_SHARED_PTR<RRObject>());
    }
    catch (std::exception& exp)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(node, Service, -1, GetServiceName(), "",
                                                "ServerServiceListener callback raised exception: " << exp.what());
        RobotRaconteurNode::TryHandleException(node, &exp);
    }

    ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Service, -1, GetServiceName(), "",
                                            "Close service complete \"" << GetServiceName() << "\"");
}

void ServerContext::MessageReceived(RR_INTRUSIVE_PTR<Message> m, RR_SHARED_PTR<ServerEndpoint> e)
{

    RR_INTRUSIVE_PTR<Message> mret = CreateMessage();
    mret->header = CreateMessageHeader();

    BOOST_FOREACH (RR_INTRUSIVE_PTR<MessageEntry>& mm, m->entries)
    {
        if (mm->Error == MessageErrorType_InvalidEndpoint)
        {
            ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Service, e->GetLocalEndpoint(), GetServiceName(), "",
                                                    "Received invalid endpoint, closing service");

            this->RemoveClient(e);
            return;
        }

        RR_INTRUSIVE_PTR<MessageEntry> mmret = ProcessMessageEntry(mm, e);
        if (mmret != 0)
            mret->entries.push_back(mmret);
    }
    if (mret->entries.size() > 0)
    {
        boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)> h =
            boost::bind(&rr_context_node_handler, GetNode(), RR_BOOST_PLACEHOLDERS(_1));
        e->AsyncSendMessage(mret, h);
    }
}

void ServerContext::AddClient(RR_SHARED_PTR<ServerEndpoint> cendpoint)
{

    {
        boost::mutex::scoped_lock lock(client_endpoints_lock);
        client_endpoints.insert(std::make_pair(cendpoint->GetLocalEndpoint(), cendpoint));
    }

    RobotRaconteurVersion client_version = cendpoint->GetClientVersion();
    if (client_version == RobotRaconteurVersion())
    {
        ROBOTRACONTEUR_LOG_INFO_COMPONENT_PATH(node, Service, cendpoint->GetLocalEndpoint(), GetServiceName(), "",
                                               "Client connected");
    }
    else
    {
        ROBOTRACONTEUR_LOG_INFO_COMPONENT_PATH(node, Service, cendpoint->GetLocalEndpoint(), GetServiceName(), "",
                                               "Client connected with Robot Raconteur version "
                                                   << client_version.ToString());
    }

    try
    {
        RR_SHARED_PTR<uint32_t> endpt = RR_MAKE_SHARED<uint32_t>(cendpoint->GetLocalEndpoint());
        ServerServiceListener(shared_from_this(), ServerServiceListenerEventType_ClientConnected, endpt);
    }
    catch (std::exception& exp)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(node, Service, -1, GetServiceName(), "",
                                                "ServerServiceListener callback raised exception: " << exp.what());
        RobotRaconteurNode::TryHandleException(node, &exp);
    }
}

void ServerContext::RemoveClient(RR_SHARED_PTR<ServerEndpoint> cendpoint)
{

    // TODO: possible deadlock

    std::string cusername = cendpoint->GetAuthenticatedUsername();
    uint32_t ce = cendpoint->GetLocalEndpoint();

    {
        boost::mutex::scoped_lock lock(ClientLockOp_lockobj);
        RR_UNORDERED_MAP<std::string, RR_SHARED_PTR<ServerContext_ObjectLock> > oo = active_object_locks;
        typedef RR_UNORDERED_MAP<std::string, RR_SHARED_PTR<ServerContext_ObjectLock> >::value_type o_type;
        BOOST_FOREACH (o_type& o, oo)
        {
            try
            {
                if (o.second->GetUsername() == cusername)
                {
                    if (o.second->GetEndpoint() == ce)
                    {
                        o.second->ReleaseLock();
                        active_object_locks.erase(o.first);
                    }
                    else
                    {

                        {

                            bool lastone = true;

                            {
                                boost::mutex::scoped_lock lock(client_endpoints_lock);
                                BOOST_FOREACH (RR_SHARED_PTR<ServerEndpoint>& cc,
                                               client_endpoints | boost::adaptors::map_values)
                                {
                                    if (cc->GetLocalEndpoint() != cendpoint->GetLocalEndpoint())
                                        if (cc->GetAuthenticatedUsername() == cusername)
                                            lastone = false;
                                }
                            }

                            if (lastone)
                            {
                                o.second->ReleaseLock();
                                active_object_locks.erase(o.first);
                            }
                        }
                    }
                }
            }
            catch (std::exception&)
            {}
        }
    }

    try

    {
        boost::mutex::scoped_lock lock(client_endpoints_lock);
        client_endpoints.erase(cendpoint->GetLocalEndpoint());
    }
    catch (std::exception&)
    {}

    try
    {
        GetNode()->DeleteEndpoint(cendpoint);
    }
    catch (std::exception&)
    {}

    if (cusername.empty())
    {
        ROBOTRACONTEUR_LOG_INFO_COMPONENT_PATH(node, Service, ce, GetServiceName(), "", "Client disconnected");
    }
    else
    {
        ROBOTRACONTEUR_LOG_INFO_COMPONENT_PATH(node, Service, ce, GetServiceName(), "",
                                               "Client with username \"" << cusername << "\" disconnected");
    }

    try
    {

        RR_SHARED_PTR<uint32_t> endpt = RR_MAKE_SHARED<uint32_t>(cendpoint->GetLocalEndpoint());
        ServerServiceListener(shared_from_this(), ServerServiceListenerEventType_ClientDisconnected, endpt);
    }
    catch (std::exception& exp)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(node, Service, -1, GetServiceName(), "",
                                                "ServerServiceListener callback raised exception: " << exp.what());
        RobotRaconteurNode::TryHandleException(node, &exp);
    }
}

void ServerContext::KickUser(boost::string_ref username)
{
    ROBOTRACONTEUR_LOG_INFO_COMPONENT_PATH(node, Service, -1, GetServiceName(), "",
                                           "Kicking user \"" << username << "\"");

    typedef boost::tuple<std::string, RR_SHARED_PTR<ServerEndpoint> > kicked_client_type;
    std::list<kicked_client_type> kicked_clients;
    {
        boost::mutex::scoped_lock lock(client_endpoints_lock);

        BOOST_FOREACH (RR_SHARED_PTR<ServerEndpoint>& e, client_endpoints | boost::adaptors::map_values)
        {
            try
            {
                std::string u = e->GetAuthenticatedUsername();
                if (username == u)
                {
                    kicked_clients.push_back(boost::make_tuple(u, e));
                }
            }
            catch (std::exception&)
            {}
        }
    }

    BOOST_FOREACH (kicked_client_type& ee, kicked_clients)
    {
        try
        {
            RemoveClient(ee.get<1>());
        }
        catch (std::exception& exp)
        {
            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(node, Service, ee.get<1>()->GetLocalEndpoint(), GetServiceName(),
                                                    "", "KickUser \"" << ee.get<0>() << "\" failed: " << exp.what());
        }
    }
}

RR_INTRUSIVE_PTR<MessageEntry> ServerContext::ClientSessionOp(RR_INTRUSIVE_PTR<MessageEntry> m,
                                                              RR_SHARED_PTR<ServerEndpoint> e)
{
    ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Service, e->GetLocalEndpoint(), m->ServicePath, m->MemberName,
                                            "ClientSessionOp");

    if (user_authenticator == 0 && !boost::starts_with(m->MemberName.str(), "Monitor"))
    {
        throw InvalidOperationException("User authentication not activated for this service");
    }

    RR_INTRUSIVE_PTR<MessageEntry> ret = CreateMessageEntry(MessageEntryType_ClientSessionOpRet, m->MemberName);
    ret->RequestID = m->RequestID;
    ret->ServicePath = m->ServicePath;

    const MessageStringPtr& command = m->MemberName;

    if (command == "AuthenticateUser")
    {
        std::string username = m->FindElement("username")->CastDataToString();
        RR_INTRUSIVE_PTR<RRMap<std::string, RRValue> > credentials =
            rr_cast<RRMap<std::string, RRValue> >((GetNode()->UnpackMapType<std::string, RRValue>(
                m->FindElement("credentials")->CastDataToNestedList(DataTypes_dictionary_t))));
        try
        {
            e->AuthenticateUser(username, credentials->GetStorageContainer());
            ROBOTRACONTEUR_LOG_INFO_COMPONENT_PATH(node, Service, e->GetLocalEndpoint(), m->ServicePath, m->MemberName,
                                                   "Client authenticated by service with username \"" << username
                                                                                                      << "\"");
            ret->AddElement("return", stringToRRArray("OK"));
            return ret;
        }
        catch (std::exception& exp2)
        {
            ROBOTRACONTEUR_LOG_INFO_COMPONENT_PATH(node, Service, e->GetLocalEndpoint(), m->ServicePath, m->MemberName,
                                                   "Client authentication by service failed with username \""
                                                       << username << "\": " << exp2.what());
            throw;
        }
    }
    else if (command == "LogoutUser")
    {
        std::string username = e->GetAuthenticatedUsername();
        e->LogoutUser();
        ROBOTRACONTEUR_LOG_INFO_COMPONENT_PATH(node, Service, e->GetLocalEndpoint(), m->ServicePath, m->MemberName,
                                               "Client \"" << username << "\" logged out");
        ret->AddElement("return", stringToRRArray("OK"));
        return ret;
    }
    else if (command == "RequestObjectLock" || command == "ReleaseObjectLock" || command == "RequestClientObjectLock" ||
             command == "ReleaseClientObjectLock" || command == "MonitorEnter" || command == "MonitorContinueEnter" ||
             command == "MonitorExit")
    {

        ClientLockOp(m, ret);
        return ret;
    }
    else
    {
        throw ProtocolException("Invalid ClientSessionOp command");
    }

    throw ProtocolException("Error evaluating ClientSessionOp command");
}

bool ServerContext::RequireValidUser() { return m_RequireValidUser; }

RR_SHARED_PTR<AuthenticatedUser> ServerContext::AuthenticateUser(
    boost::string_ref username, std::map<std::string, RR_INTRUSIVE_PTR<RRValue> >& credentials,
    RR_SHARED_PTR<ServerEndpoint> ep)
{

    if (!user_authenticator)
        throw AuthenticationException("Authentication not enabled");

    RR_SHARED_PTR<ITransportConnection> tc;
    if (ep)
    {
        tc = ep->GetTransportConnection();
    }

    return user_authenticator->AuthenticateUser(username, credentials, shared_from_this(), tc);
}

void ServerContext::ClientLockOp(RR_INTRUSIVE_PTR<MessageEntry> m, RR_INTRUSIVE_PTR<MessageEntry> ret)
{

    {
        boost::mutex::scoped_lock lock(ClientLockOp_lockobj);

        // if (m.ServicePath != ServiceName) throw new Exception("Only locking of root object currently supported");

        std::vector<std::string> priv;
        std::string username;
        if (!boost::starts_with(m->MemberName.str(), "Monitor"))
        {
            if (ServerEndpoint::GetCurrentAuthenticatedUser() == 0)
                throw PermissionDeniedException("User must be authenticated to lock object");
            priv = ServerEndpoint::GetCurrentAuthenticatedUser()->GetPrivileges();
            if (!(std::find(priv.begin(), priv.end(), ("objectlock")) != priv.end() ||
                  std::find(priv.begin(), priv.end(), "objectlockoverride") != priv.end()))
                throw ObjectLockedException("User does not have object locking privileges");
            username = ServerEndpoint::GetCurrentAuthenticatedUser()->GetUsername();
        }

        MessageStringPtr& servicepath = m->ServicePath;

        RR_SHARED_PTR<ServiceSkel> skel = GetObjectSkel(servicepath);

        if (m->MemberName == "RequestObjectLock")
        {
            RequestObjectLock(servicepath.str(), ServerEndpoint::GetCurrentAuthenticatedUser()->GetUsername());
            ret->AddElement("return", stringToRRArray("OK"));
        }
        else if (m->MemberName == "RequestClientObjectLock")
        {
            RequestClientObjectLock(servicepath.str(), ServerEndpoint::GetCurrentAuthenticatedUser()->GetUsername(),
                                    ServerEndpoint::GetCurrentEndpoint()->GetLocalEndpoint());
            ret->AddElement("return", stringToRRArray("OK"));
        }
        else if (m->MemberName == "ReleaseObjectLock")
        {
            bool override_ = std::find(priv.begin(), priv.end(), "objectlockoverride") != priv.end();

            ReleaseObjectLock(servicepath.str(), username, override_);

            ret->AddElement("return", stringToRRArray("OK"));
        }
        else if (m->MemberName == "MonitorEnter")
        {
            {
                boost::mutex::scoped_lock lock2(skel->monitorlocks_lock);
                if (skel->monitorlocks.find(ServerEndpoint::GetCurrentEndpoint()->GetLocalEndpoint()) !=
                    skel->monitorlocks.end())
                    throw InvalidOperationException("Already acquiring or acquired monitor lock");
            }
            RR_SHARED_PTR<ServerContext_MonitorObjectSkel> s = RR_MAKE_SHARED<ServerContext_MonitorObjectSkel>(skel);
            int32_t timeout = RRArrayToScalar(m->FindElement("timeout")->CastData<RRArray<int32_t> >());

            lock.unlock();
            std::string retcode = s->MonitorEnter(ServerEndpoint::GetCurrentEndpoint()->GetLocalEndpoint(), timeout);
            ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Service,
                                                    ServerEndpoint::GetCurrentEndpoint()->GetLocalEndpoint(),
                                                    servicepath, "", "MonitorEnter result: " << retcode);
            ret->AddElement("return", stringToRRArray(retcode));
        }
        else if (m->MemberName == "MonitorContinueEnter")
        {
            RR_SHARED_PTR<ServerContext_MonitorObjectSkel> s;
            {
                boost::mutex::scoped_lock lock2(skel->monitorlocks_lock);
                RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<ServerContext_MonitorObjectSkel> >::iterator e1 =
                    skel->monitorlocks.find(ServerEndpoint::GetCurrentEndpoint()->GetLocalEndpoint());
                if (e1 == skel->monitorlocks.end())
                    throw InvalidOperationException("Not acquiring monitor lock");

                s = e1->second;
            }
            lock.unlock();
            std::string retcode = s->MonitorContinueEnter(ServerEndpoint::GetCurrentEndpoint()->GetLocalEndpoint());
            ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Service,
                                                    ServerEndpoint::GetCurrentEndpoint()->GetLocalEndpoint(),
                                                    servicepath, "", "MonitorContinueEnter result: " << retcode);
            ret->AddElement("return", stringToRRArray(retcode));
        }
        else if (m->MemberName == "MonitorExit")
        {

            RR_SHARED_PTR<ServerContext_MonitorObjectSkel> s;

            {
                boost::mutex::scoped_lock lock2(skel->monitorlocks_lock);
                if (!skel->monitorlock)
                    return;
                RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<ServerContext_MonitorObjectSkel> >::iterator e1 =
                    skel->monitorlocks.find(ServerEndpoint::GetCurrentEndpoint()->GetLocalEndpoint());
                if (e1 == skel->monitorlocks.end())
                    throw InvalidOperationException("Not monitor locked");
                s = e1->second;
            }
            std::string retcode = s->MonitorExit(ServerEndpoint::GetCurrentEndpoint()->GetLocalEndpoint());
            ROBOTRACONTEUR_LOG_INFO_COMPONENT_PATH(node, Service,
                                                   ServerEndpoint::GetCurrentEndpoint()->GetLocalEndpoint(),
                                                   servicepath, "", "MonitorExit result: " << retcode);
            ret->AddElement("return", stringToRRArray(retcode));
        }
        else
        {
            throw InvalidOperationException("Invalid command");
        }
    }
}

void ServerContext::RequestObjectLock(boost::string_ref servicepath, boost::string_ref username)
{
    RR_SHARED_PTR<ServiceSkel> skel = GetObjectSkel(servicepath);
    boost::mutex::scoped_lock lock(skels_lock);
    if (skel->IsLocked())
        throw ObjectLockedException("Object already locked");
    for (RR_UNORDERED_MAP<MessageStringPtr, RR_SHARED_PTR<ServiceSkel> >::iterator s = skels.begin(); s != skels.end();
         ++s)
    {
        if (boost::starts_with(s->first.str(), servicepath))
            if (s->second->IsLocked())
                throw ObjectLockedException("Object already locked");
    }

    RR_SHARED_PTR<ServerContext_ObjectLock> o = RR_MAKE_SHARED<ServerContext_ObjectLock>(username, skel);

    for (RR_UNORDERED_MAP<MessageStringPtr, RR_SHARED_PTR<ServiceSkel> >::iterator s = skels.begin(); s != skels.end();
         ++s)
    {
        if (boost::starts_with(s->first.str(), servicepath))
            o->AddSkel(s->second);
    }

    active_object_locks.insert(make_pair(o->GetRootServicePath(), o));

    ROBOTRACONTEUR_LOG_INFO_COMPONENT_PATH(node, Service, -1, servicepath, "",
                                           "Object locked by user \"" << username << "\"");
}

void ServerContext::RequestClientObjectLock(boost::string_ref servicepath, boost::string_ref username,
                                            uint32_t endpoint)
{
    RR_SHARED_PTR<ServiceSkel> skel = GetObjectSkel(servicepath);
    boost::mutex::scoped_lock lock(skels_lock);
    if (skel->IsLocked())
        throw ObjectLockedException("Object already locked");
    for (RR_UNORDERED_MAP<MessageStringPtr, RR_SHARED_PTR<ServiceSkel> >::iterator s = skels.begin(); s != skels.end();
         ++s)
    {
        if (boost::starts_with(s->first.str(), servicepath))
            if (s->second->IsLocked())
                throw ObjectLockedException("Object already locked");
    }

    RR_SHARED_PTR<ServerContext_ObjectLock> o = RR_MAKE_SHARED<ServerContext_ObjectLock>(username, skel, endpoint);
    for (RR_UNORDERED_MAP<MessageStringPtr, RR_SHARED_PTR<ServiceSkel> >::iterator s = skels.begin(); s != skels.end();
         ++s)
    {
        if (boost::starts_with(s->first.str(), servicepath))
            o->AddSkel(s->second);
    }
    active_object_locks.insert(make_pair(o->GetRootServicePath(), o));

    ROBOTRACONTEUR_LOG_INFO_COMPONENT_PATH(node, Service, -1, servicepath, "",
                                           "Object session locked by user \"" << username << "\" ep " << endpoint);
}

void ServerContext::ReleaseObjectLock(boost::string_ref servicepath, boost::string_ref username, bool override_)
{
    RR_SHARED_PTR<ServiceSkel> skel = GetObjectSkel(servicepath);
    if (!skel->IsLocked())
        return;
    boost::mutex::scoped_lock lock2(skel->objectlock_lock);
    RR_SHARED_PTR<ServerContext_ObjectLock> lock = skel->objectlock.lock();
    if (!lock)
        return;
    if (lock->GetRootServicePath() != servicepath)
        throw ObjectLockedException("Cannot release inherited lock");
    if (username != lock->GetUsername() && !override_)
        throw ObjectLockedException("Service locked by user " + lock->GetUsername());
    if (lock->GetEndpoint() != 0)
    {
        if (ServerEndpoint::GetCurrentEndpoint()->GetLocalEndpoint() != lock->GetEndpoint() && !override_)
            if (username != lock->GetUsername() && !override_)
                throw ObjectLockedException("Service locked by other session");
    }

    lock2.unlock();
    try
    {
        lock->ReleaseLock();
    }
    catch (std::exception&)
    {}
    lock2.lock();
    if (active_object_locks.count(skel->GetServicePath()) != 0)
        active_object_locks.erase(skel->GetServicePath());

    if (username != lock->GetUsername() && override_)
    {
        ROBOTRACONTEUR_LOG_INFO_COMPONENT_PATH(node, Service, -1, servicepath, "",
                                               "Object lock released using override by user \"" << username << "\"");
    }
    else
    {
        ROBOTRACONTEUR_LOG_INFO_COMPONENT_PATH(node, Service, -1, servicepath, "",
                                               "Object lock released by user \"" << username << "\"");
    }
}

std::string ServerContext::GetObjectLockUsername(boost::string_ref servicepath)
{
    RR_SHARED_PTR<ServiceSkel> skel = GetObjectSkel(servicepath);
    if (!skel->IsLocked())
        return "";
    RR_SHARED_PTR<ServerContext_ObjectLock> lock = skel->objectlock.lock();
    if (!lock)
        return "";

    return lock->GetUsername();
}

void ServerContext::check_lock(RR_SHARED_PTR<ServiceSkel> skel, RR_INTRUSIVE_PTR<MessageEntry> m)
{
    check_monitor_lock(skel);
    if (skel->IsLocked())
    {
        boost::mutex::scoped_lock lock2(skel->objectlock_lock);
        RR_SHARED_PTR<ServerContext_ObjectLock> lock = skel->objectlock.lock();
        if (!lock)
            return;
        if (skel->IsRequestNoLock(m))
            return;
        try
        {
            if (lock->GetUsername() == ServerEndpoint::GetCurrentAuthenticatedUser()->GetUsername() &&
                lock->GetEndpoint() == 0)
                return;
            if (lock->GetUsername() == ServerEndpoint::GetCurrentAuthenticatedUser()->GetUsername() &&
                lock->GetEndpoint() == ServerEndpoint::GetCurrentEndpoint()->GetLocalEndpoint())
                return;
        }
        catch (AuthenticationException&)
        {}
        throw ObjectLockedException("Object locked by " + lock->GetUsername());
    }
}

void ServerContext::check_monitor_lock(RR_SHARED_PTR<ServiceSkel> skel)
{
    boost::mutex::scoped_lock lock2(skel->monitorlocks_lock);
    if (skel->IsMonitorLocked())
    {
        if (skel->monitorlock->GetLocalEndpoint() == ServerEndpoint::GetCurrentEndpoint()->GetLocalEndpoint())
        {
            skel->monitorlock->MonitorRefresh(ServerEndpoint::GetCurrentEndpoint()->GetLocalEndpoint());
        }
        else
        {
            throw InvalidOperationException(
                "Object is currently monitor locked. Use MonitorEnter to obtain monitor lock");
        }
    }
}

void ServerContext::PeriodicCleanupTask()
{
    boost::mutex::scoped_lock lock(skels_lock);
    BOOST_FOREACH (RR_SHARED_PTR<ServiceSkel> s, skels | boost::adaptors::map_values)
    {
        s->CleanupGenerators();
    }
}

RR_INTRUSIVE_PTR<MessageEntry> ServerContext::CheckServiceCapability(RR_INTRUSIVE_PTR<MessageEntry> m,
                                                                     RR_SHARED_PTR<ServerEndpoint> c)
{
    RR_INTRUSIVE_PTR<MessageEntry> ret = CreateMessageEntry(MessageEntryType_ServiceCheckCapabilityRet, m->MemberName);
    ret->ServicePath = m->ServicePath;
    ret->RequestID = m->RequestID;
    ret->AddElement("return", ScalarToRRArray(static_cast<uint32_t>(0)));
    return ret;
}

void ServerContext::ReleaseServicePath1(const std::string& path)
{
    if (path == GetServiceName())
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(node, Service, -1, path, "", "Attempt to release root object");
        throw ServiceException("Root object cannot be released");
    }

    {
        boost::mutex::scoped_lock lock(skels_lock);
        std::vector<std::string> objkeys;
        BOOST_FOREACH (const MessageStringPtr& k, skels | boost::adaptors::map_keys)
        {
            std::string path_param1 = path + ".";
            if (k == path || boost::starts_with(k.str(), path_param1))
            {
                objkeys.push_back(RR_MOVE(k.str().to_string()));
            }
        }

        if (objkeys.size() == 0)
        {
            ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(node, Service, -1, path, "", "Unknown service path");
            throw ServiceException("Unknown service path");
        }

        BOOST_FOREACH (std::string& path1, objkeys)
        {
            RR_UNORDERED_MAP<MessageStringPtr, RR_SHARED_PTR<ServiceSkel> >::iterator e1 = skels.find(path1);
            if (e1 == skels.end())
                continue;
            RR_SHARED_PTR<ServiceSkel> s = e1->second;

            if (s->IsLocked())
            {

                {
                    boost::mutex::scoped_lock lock2(s->objectlock_lock);
                    RR_SHARED_PTR<ServerContext_ObjectLock> lock = s->objectlock.lock();
                    if (!lock)
                        return;
                    boost::mutex::scoped_lock lock3(ClientLockOp_lockobj);
                    if (lock->GetRootServicePath() == path1)
                    {
                        active_object_locks.erase(lock->GetUsername());
                        lock->ReleaseLock();
                    }
                    else
                    {
                        lock->ReleaseSkel(s);
                    }
                }
            }

            // s->ReleaseCastObject();
            skels.erase(path1);
            s->ReleaseObject();
            ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Service, -1, path, "", "Object released");
        }
    }
}

void ServerContext::ReleaseServicePath(boost::string_ref path)
{

    ReleaseServicePath1(path.to_string());

    RR_INTRUSIVE_PTR<MessageEntry> m = CreateMessageEntry(MessageEntryType_ServicePathReleasedReq, "");
    m->ServicePath = path;

    ROBOTRACONTEUR_LOG_INFO_COMPONENT_PATH(node, Service, -1, path, "", "Service path released");

    SendEvent(m);
}

void ServerContext::ReleaseServicePath(boost::string_ref path, const std::vector<uint32_t>& endpoints)
{

    ReleaseServicePath1(path.to_string());

    ROBOTRACONTEUR_LOG_INFO_COMPONENT_PATH(node, Service, -1, path, "", "Service path released");

    RR_INTRUSIVE_PTR<MessageEntry> m = CreateMessageEntry(MessageEntryType_ServicePathReleasedReq, "");
    m->ServicePath = path;

    std::vector<RR_SHARED_PTR<ServerEndpoint> > cc;

    {
        {
            boost::mutex::scoped_lock lock(client_endpoints_lock);
            BOOST_FOREACH (uint32_t e, endpoints)
            {
                RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<ServerEndpoint> >::iterator e1 = client_endpoints.find(e);
                if (e1 != client_endpoints.end())
                {
                    cc.push_back(e1->second);
                }
            }
        }

        BOOST_FOREACH (RR_SHARED_PTR<RobotRaconteur::ServerEndpoint>& c, cc)
        {

            if (m_RequireValidUser)
            {
                try
                {
                    if (c->GetAuthenticatedUsername() == "")
                        continue;
                }
                catch (AuthenticationException&)
                {
                    continue;
                }
            }

            try
            {
                GetNode()->CheckConnection(c->GetLocalEndpoint());
                boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)> h =
                    boost::bind(&rr_context_emptyhandler, RR_BOOST_PLACEHOLDERS(_1));
                AsyncSendMessage(ShallowCopyMessageEntry(m), c, h);
            }
            catch (std::exception& exp)
            {
                ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(
                    node, Service, c->GetLocalEndpoint(), path, "",
                    "Error sending ReleaseServicePath event to client: " << exp.what());
                try
                {
                    RemoveClient(c);
                }
                catch (std::exception&)
                {};
            }
        }
    }
}

RR_INTRUSIVE_PTR<MessageEntry> ServerContext::ProcessCallbackRequest(RR_INTRUSIVE_PTR<MessageEntry> m,
                                                                     uint32_t endpointid)
{
    ROBOTRACONTEUR_ASSERT_MULTITHREADED(node);

    RR_INTRUSIVE_PTR<MessageEntry> rec_message;
    uint32_t myrequestid = 0;

    try
    {
        RR_SHARED_PTR<ServerEndpoint> e;
        {
            boost::mutex::scoped_lock lock(client_endpoints_lock);
            RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<ServerEndpoint> >::iterator e1 = client_endpoints.find(endpointid);
            if (e1 == client_endpoints.end())
                throw InvalidEndpointException("Invalid client endpoint");
            e = e1->second;
        }

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

        ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Service, endpointid, m->ServicePath, m->MemberName,
                                                "ProcessCallbackRequest sending message with requestid "
                                                    << myrequestid << " EntryType " << m->EntryType);

        // Console.WriteLine("Sent " + m.RequestID + " " + m.EntryType + " " + m.MemberName);
        SendMessage(m, e);

        boost::posix_time::ptime request_start = GetNode()->NowNodeTime();
        uint32_t request_timeout = GetNode()->GetRequestTimeout();
        while (true)
        {

            {
                boost::mutex::scoped_lock lock(outstanding_requests_lock);
                if (t->ret)
                    break;
            }
            GetNode()->CheckConnection(e->GetLocalEndpoint());

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

        {
            boost::mutex::scoped_lock lock(outstanding_requests_lock);
            outstanding_requests.erase(myrequestid);
            rec_message = t->ret;
        }

        if (rec_message->RequestID != myrequestid)
            throw InternalErrorException("This should be impossible!");

        ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Service, endpointid, m->ServicePath, m->MemberName,
                                                "ProcessRequest received response with requestid " << myrequestid);
    }
    catch (std::exception& exp)
    {
        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(node, Service, endpointid, m->ServicePath, m->MemberName,
                                                "Error during ProcessCallbackRequest: " << exp.what());
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
                node, Service, endpointid, m->ServicePath, m->MemberName,
                "Client callback returned remote error during ProcessCallbackRequest: " << err->what());
            m_ServiceDef->DownCastAndThrowException(*err);
        }

        ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(
            node, Service, endpointid, m->ServicePath, m->MemberName,
            "Client callback returned error during ProcessCallbackRequest: " << rec_message->Error);
        RobotRaconteurExceptionUtil::ThrowMessageEntryException(rec_message);
    }

    ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Service, endpointid, m->ServicePath, m->MemberName,
                                            "ProcessCallbackRequest with requestid " << myrequestid << " successful");

    return rec_message;
}

RR_SHARED_PTR<ThreadPool> ServerContext::GetMonitorThreadPool()
{
    boost::mutex::scoped_lock lock(monitor_thread_pool_lock);

    if (!monitor_thread_pool)
    {
        RR_SHARED_PTR<ThreadPoolFactory> factory = GetNode()->GetThreadPoolFactory();
        if (!factory)
        {
            factory = RR_MAKE_SHARED<ThreadPoolFactory>();
        }

        monitor_thread_pool = factory->NewThreadPool(GetNode());
        monitor_thread_pool->SetThreadPoolCount(5);

        ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Service, -1, GetServiceName(), "",
                                                "Initialized monitor lock thread pool with 5 threads");
    }

    return monitor_thread_pool;
}

int32_t ServerContext::GetMonitorThreadPoolCount()
{
    return boost::numeric_cast<int32_t>(GetMonitorThreadPool()->GetThreadPoolCount());
}

void ServerContext::SetMonitorThreadPoolCount(int32_t count)
{
    GetMonitorThreadPool()->SetThreadPoolCount(count);
    ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Service, -1, GetServiceName(), "",
                                            "Monitor thread pool count set to " << count << "threads");
}

void ServerContext::InitializeInstanceFields()
{

    m_RootObjectType.clear();

    base_object_set = false;
    m_RequireValidUser = false;
    AllowObjectLock = false;

    request_number = 0;
}

std::map<std::string, RR_INTRUSIVE_PTR<RRValue> > ServerContext::GetAttributes()
{
    boost::mutex::scoped_lock lock(m_Attributes_lock);
    return m_Attributes;
}
void ServerContext::SetAttributes(const std::map<std::string, RR_INTRUSIVE_PTR<RRValue> >& attr)
{
    boost::mutex::scoped_lock lock(m_Attributes_lock);

    BOOST_FOREACH (const RR_INTRUSIVE_PTR<RRValue>& e, attr | boost::adaptors::map_values)
    {
        RR_INTRUSIVE_PTR<RRBaseArray> a = RR_DYNAMIC_POINTER_CAST<RRBaseArray>(e);
        if (!a)
            throw InvalidArgumentException("Attributes but be numbers or strings");
    }

    m_Attributes = attr;

    try
    {
        GetNode()->UpdateServiceStateNonce();
    }
    catch (std::exception&)
    {}

    ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Service, -1, GetServiceName(), "", "Service attributes set");
}

boost::thread_specific_ptr<RR_SHARED_PTR<ServerEndpoint> > ServerEndpoint::m_CurrentEndpoint;

RR_SHARED_PTR<ServerEndpoint> ServerEndpoint::GetCurrentEndpoint()
{

    if (!m_CurrentEndpoint.get())
        throw InvalidOperationException("Current ServiceEndpoint Not set");
    RR_SHARED_PTR<ServerEndpoint> out = *m_CurrentEndpoint.get();
    if (!out)
        throw InvalidOperationException("Current ServiceEndpoint Not set");
    return out;
}

boost::thread_specific_ptr<RR_SHARED_PTR<AuthenticatedUser> > ServerEndpoint::m_CurrentAuthenticatedUser;

RR_SHARED_PTR<AuthenticatedUser> ServerEndpoint::GetCurrentAuthenticatedUser()
{
    if (!m_CurrentAuthenticatedUser.get())
        throw PermissionDeniedException("User is not authenticated");
    RR_SHARED_PTR<AuthenticatedUser> out = *m_CurrentAuthenticatedUser.get();
    if (!out)
        throw AuthenticationException("User is not authenticated");
    return out;
}

const std::string ServerEndpoint::GetAuthenticatedUsername() const
{
    if (endpoint_authenticated_user == 0)
        return "";

    return endpoint_authenticated_user->GetUsername();
}

void ServerEndpoint::MessageReceived(RR_INTRUSIVE_PTR<Message> m)
{
    if (m->entries.size() > 0)
    {
        if (m->entries.at(0)->EntryType == MessageEntryType_EndpointCheckCapability)
        {
            CheckEndpointCapabilityMessage(m);
            return;
        }
    }
    {

        SetLastMessageReceivedTime(GetNode()->NowNodeTime());
    }
    m_CurrentEndpoint.reset(new RR_SHARED_PTR<ServerEndpoint>(shared_from_this()));
    m_CurrentAuthenticatedUser.reset(new RR_SHARED_PTR<AuthenticatedUser>(endpoint_authenticated_user));
    if (endpoint_authenticated_user != 0)
        endpoint_authenticated_user->UpdateLastAccess();
    service->MessageReceived(m, shared_from_this());
    m_CurrentEndpoint.reset(0);
    m_CurrentAuthenticatedUser.reset(0);
}

void ServerEndpoint::AuthenticateUser(boost::string_ref username,
                                      std::map<std::string, RR_INTRUSIVE_PTR<RRValue> >& credentials)
{

    RR_SHARED_PTR<AuthenticatedUser> u = service->AuthenticateUser(username, credentials, shared_from_this());
    endpoint_authenticated_user = u;
    m_CurrentAuthenticatedUser.reset(new RR_SHARED_PTR<AuthenticatedUser>(u));
}

void ServerEndpoint::LogoutUser()
{
    endpoint_authenticated_user.reset();
    m_CurrentAuthenticatedUser.reset(0);
}

void ServerEndpoint::PeriodicCleanupTask()
{
    if ((GetNode()->NowNodeTime() - GetLastMessageReceivedTime()).total_milliseconds() >
        GetNode()->GetEndpointInactivityTimeout())
    {
        service->RemoveClient(shared_from_this());
    }
}

void ServerEndpoint::SetTransportConnection(RR_SHARED_PTR<ITransportConnection> c)
{
    Endpoint::SetTransportConnection(c);
    bool m = c->CheckCapabilityActive(TransportCapabilityCode_MESSAGE4_BASIC_PAGE |
                                      TransportCapabilityCode_MESSAGE4_BASIC_ENABLE);
}

void ServerEndpoint::InitializeInstanceFields() { endpoint_authenticated_user.reset(); }

RobotRaconteurVersion ServerEndpoint::GetClientVersion()
{
    boost::mutex::scoped_lock lock(this_lock);
    return client_version;
}
void ServerEndpoint::SetClientVersion(const RobotRaconteurVersion& version)
{
    boost::mutex::scoped_lock lock(this_lock);
    client_version = version;
}

} // namespace RobotRaconteur
