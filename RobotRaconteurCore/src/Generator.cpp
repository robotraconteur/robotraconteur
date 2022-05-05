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

#include "RobotRaconteur/Generator.h"
#include "RobotRaconteur/RobotRaconteurNode.h"
#include "RobotRaconteur/Client.h"
#include "RobotRaconteur/Service.h"
#include "RobotRaconteur/DataTypes.h"

#include <boost/range/adaptors.hpp>
#include <boost/range/algorithm.hpp>
#include <boost/foreach.hpp>

namespace RobotRaconteur
{
GeneratorClientBase::GeneratorClientBase(boost::string_ref name, int32_t id, RR_SHARED_PTR<ServiceStub> stub)
{
    this->node = stub->RRGetNode();
    this->endpoint = stub->GetContext()->GetLocalEndpoint();
    this->name = RR_MOVE(name.to_string());
    this->id = id;
    this->stub = stub;
    ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Client, endpoint, service_path, name,
                                            "Created generator with id " << id);
}

RR_SHARED_PTR<ServiceStub> GeneratorClientBase::GetStub()
{
    RR_SHARED_PTR<ServiceStub> out = stub.lock();
    if (!out)
        throw InvalidOperationException("Generator has been closed");
    return out;
}

void GeneratorClientBase::Abort()
{
    ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Client, endpoint, service_path, name,
                                            "Requesting generator abort with id " << id);
    RR_INTRUSIVE_PTR<MessageEntry> m = CreateMessageEntry(MessageEntryType_GeneratorNextReq, GetMemberName());
    AbortOperationException err("Generator abort requested");
    RobotRaconteurExceptionUtil::ExceptionToMessageEntry(err, m);
    m->AddElement("index", ScalarToRRArray(id));
    GetStub()->ProcessRequest(m);
}
void GeneratorClientBase::AsyncAbort(boost::function<void(RR_SHARED_PTR<RobotRaconteurException> err)> handler,
                                     int32_t timeout)
{
    ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Client, endpoint, service_path, name,
                                            "Requesting async generator abort with id " << id);
    RR_INTRUSIVE_PTR<MessageEntry> m = CreateMessageEntry(MessageEntryType_GeneratorNextReq, GetMemberName());
    AbortOperationException err("Generator abort requested");
    RobotRaconteurExceptionUtil::ExceptionToMessageEntry(err, m);
    m->AddElement("index", ScalarToRRArray(id));
    GetStub()->AsyncProcessRequest(m, boost::bind(handler, RR_BOOST_PLACEHOLDERS(_2)), timeout);
}

void GeneratorClientBase::Close()
{
    ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Client, endpoint, service_path, name,
                                            "Requesting generator close with id " << id);
    RR_INTRUSIVE_PTR<MessageEntry> m = CreateMessageEntry(MessageEntryType_GeneratorNextReq, GetMemberName());
    StopIterationException err("");
    RobotRaconteurExceptionUtil::ExceptionToMessageEntry(err, m);
    m->AddElement("index", ScalarToRRArray(id));
    GetStub()->ProcessRequest(m);
}
void GeneratorClientBase::AsyncClose(boost::function<void(RR_SHARED_PTR<RobotRaconteurException> err)> handler,
                                     int32_t timeout)
{
    ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Client, endpoint, service_path, name,
                                            "Requesting async generator close with id " << id);
    RR_INTRUSIVE_PTR<MessageEntry> m = CreateMessageEntry(MessageEntryType_GeneratorNextReq, GetMemberName());
    StopIterationException err("");
    RobotRaconteurExceptionUtil::ExceptionToMessageEntry(err, m);
    m->AddElement("index", ScalarToRRArray(id));
    GetStub()->AsyncProcessRequest(m, boost::bind(handler, RR_BOOST_PLACEHOLDERS(_2)), timeout);
}

std::string GeneratorClientBase::GetMemberName() { return name; }

RR_INTRUSIVE_PTR<MessageElement> GeneratorClientBase::NextBase(RR_INTRUSIVE_PTR<MessageElement> v)
{
    ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Client, endpoint, service_path, name,
                                            "Calling generator next with id " << id);
    RR_INTRUSIVE_PTR<MessageEntry> m = CreateMessageEntry(MessageEntryType_GeneratorNextReq, GetMemberName());
    m->AddElement("index", ScalarToRRArray(id));
    if (v)
    {
        v->ElementName = "parameter";
        m->elements.push_back(v);
    }
    RR_INTRUSIVE_PTR<MessageEntry> ret = GetStub()->ProcessRequest(m);
    RR_INTRUSIVE_PTR<MessageElement> mret;
    ret->TryFindElement("return", mret);
    return mret;
}
void GeneratorClientBase::AsyncNextBase(
    RR_INTRUSIVE_PTR<MessageElement> v,
    boost::function<void(RR_INTRUSIVE_PTR<MessageElement> m, RR_SHARED_PTR<RobotRaconteurException> err,
                         RR_SHARED_PTR<RobotRaconteurNode>)>
        handler,
    int32_t timeout)
{
    ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Client, endpoint, service_path, name,
                                            "Calling async generator next with id " << id);
    RR_INTRUSIVE_PTR<MessageEntry> m = CreateMessageEntry(MessageEntryType_GeneratorNextReq, GetMemberName());
    m->AddElement("index", ScalarToRRArray(id));
    if (v)
    {
        v->ElementName = "parameter";
        m->elements.push_back(v);
    }
    RR_WEAK_PTR<RobotRaconteurNode> node = GetStub()->RRGetNode();
    GetStub()->AsyncProcessRequest(m, boost::bind(&GeneratorClientBase::AsyncNextBase1, RR_BOOST_PLACEHOLDERS(_1),
                                                  RR_BOOST_PLACEHOLDERS(_2), handler, node));
}

void GeneratorClientBase::AsyncNextBase1(
    RR_INTRUSIVE_PTR<MessageEntry> ret, RR_SHARED_PTR<RobotRaconteurException> err,
    boost::function<void(RR_INTRUSIVE_PTR<MessageElement>, RR_SHARED_PTR<RobotRaconteurException>,
                         RR_SHARED_PTR<RobotRaconteurNode>)>
        handler,
    RR_WEAK_PTR<RobotRaconteurNode> node)
{
    RR_SHARED_PTR<RobotRaconteurNode> node1 = node.lock();

    if (!node1)
    {
        handler(RR_INTRUSIVE_PTR<MessageElement>(), RR_MAKE_SHARED<InvalidOperationException>("Node has been released"),
                node1);
        return;
    }

    RR_INTRUSIVE_PTR<MessageElement> mret;
    if (err)
    {
        handler(mret, err, node1);
        return;
    }

    ret->TryFindElement("return", mret);
    handler(mret, err, node1);
}

GeneratorServerBase::GeneratorServerBase(boost::string_ref name, int32_t index, RR_SHARED_PTR<ServiceSkel> skel,
                                         RR_SHARED_PTR<ServerEndpoint> ep)
{
    this->name = RR_MOVE(name.to_string());
    this->index = index;
    this->skel = skel;
    this->ep = ep;
    this->last_access_time = boost::posix_time::second_clock::universal_time();
    this->node = skel->RRGetNodeWeak();
    this->service_path = skel->GetServicePath();
    ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Service, ep->GetLocalEndpoint(), service_path, name,
                                            "Created generator skel with id " << index);
}

uint32_t GeneratorServerBase::GetEndpoint() { return ep->GetLocalEndpoint(); }

void GeneratorServerBase::EndAsyncCallNext(RR_WEAK_PTR<ServiceSkel> skel, RR_INTRUSIVE_PTR<MessageElement> ret,
                                           RR_SHARED_PTR<RobotRaconteurException> err, int32_t index,
                                           RR_INTRUSIVE_PTR<MessageEntry> m, RR_SHARED_PTR<ServerEndpoint> ep)
{
    RR_SHARED_PTR<ServiceSkel> skel1 = skel.lock();
    if (!skel1)
        return;

    try
    {
        RR_INTRUSIVE_PTR<MessageEntry> ret1 = CreateMessageEntry(MessageEntryType_GeneratorNextRes, m->MemberName);
        ret1->RequestID = m->RequestID;
        ret1->ServicePath = m->ServicePath;

        if (err)
        {
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
                ret->ElementName = "return";
                ret1->AddElement(ret);
            }
        }

        skel1->SendGeneratorResponse(index, ret1, ep);
    }
    catch (std::exception& exp)
    {
        RR_SHARED_PTR<RobotRaconteurNode> node;
        try
        {
            node = skel1->RRGetNode();
        }
        catch (std::exception&)
        {}
        RobotRaconteurNode::TryHandleException(node, &exp);
    }
}

namespace detail
{
void GeneratorClient_AsyncNext2(RR_INTRUSIVE_PTR<MessageElement> v2, RR_SHARED_PTR<RobotRaconteurException> err,
                                RR_SHARED_PTR<RobotRaconteurNode> node,
                                boost::function<void(RR_SHARED_PTR<RobotRaconteurException> err)> handler)
{
    if (err)
    {
        detail::InvokeHandlerWithException(node, handler, err);
        return;
    }
    detail::InvokeHandler(node, handler);
}
} // namespace detail

} // namespace RobotRaconteur