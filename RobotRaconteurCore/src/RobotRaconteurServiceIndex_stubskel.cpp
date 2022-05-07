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

// This file is automatically generated. DO NOT EDIT!

#include "RobotRaconteur/RobotRaconteurServiceIndex_stubskel.h"
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
namespace RobotRaconteurServiceIndex
{

std::string RobotRaconteurServiceIndexFactory::GetServiceName() { return "RobotRaconteurServiceIndex"; }
std::string RobotRaconteurServiceIndexFactory::DefString()
{
    std::string out("service RobotRaconteurServiceIndex\n"
                    "struct NodeInfo\n"
                    "field string NodeName\n"
                    "field uint8[16] NodeID\n"
                    "field string{int32} ServiceIndexConnectionURL\n"
                    "end struct\n"
                    "struct ServiceInfo\n"
                    "field string Name\n"
                    "field string RootObjectType\n"
                    "field string{int32} RootObjectImplements\n"
                    "field string{int32} ConnectionURL\n"
                    "field varvalue{string} Attributes\n"
                    "end struct\n"
                    "object ServiceIndex\n"
                    "function ServiceInfo{int32} GetLocalNodeServices()\n"
                    "function NodeInfo{int32} GetRoutedNodes()\n"
                    "function NodeInfo{int32} GetDetectedNodes()\n"
                    "event LocalNodeServicesChanged()\n"
                    "end object\n");
    return out;
}
RR_SHARED_PTR<RobotRaconteur::StructureStub> RobotRaconteurServiceIndexFactory::FindStructureStub(boost::string_ref s)
{
    std::vector<std::string> res;
    boost::split(res, s, boost::is_from_range('.', '.'));
    std::string servicetype = res.at(0);
    std::string objecttype = res.at(1);
    if (objecttype == "NodeInfo")
        return RobotRaconteur::rr_cast<RobotRaconteur::StructureStub>(RR_MAKE_SHARED<NodeInfo_stub>(GetNode()));
    if (objecttype == "ServiceInfo")
        return RobotRaconteur::rr_cast<RobotRaconteur::StructureStub>(RR_MAKE_SHARED<ServiceInfo_stub>(GetNode()));
    throw RobotRaconteur::ServiceException("Invalid structure stub type.");
}
RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementNestedElementList> RobotRaconteurServiceIndexFactory::PackStructure(
   const RR_INTRUSIVE_PTR<RobotRaconteur::RRStructure>& structin)
{
    std::string type = structin->RRType();
    std::vector<std::string> res;
    boost::split(res, type, boost::is_from_range('.', '.'));
    std::string servicetype = res.at(0);
    std::string objecttype = res.at(1);
    if (servicetype != "RobotRaconteurServiceIndex")
        return GetNode()->PackStructure(structin);
    RR_SHARED_PTR<RobotRaconteur::StructureStub> stub = FindStructureStub(type);
    return stub->PackStructure(structin);
    throw RobotRaconteur::ServiceException("Invalid structure stub type.");
}
RR_INTRUSIVE_PTR<RobotRaconteur::RRValue> RobotRaconteurServiceIndexFactory::UnpackStructure(
    const RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementNestedElementList>& mstructin)
{
    std::string type = mstructin->GetTypeString().str().to_string();
    std::vector<std::string> res;
    boost::split(res, type, boost::is_from_range('.', '.'));
    std::string servicetype = res.at(0);
    std::string objecttype = res.at(1);
    if (servicetype != "RobotRaconteurServiceIndex")
        return GetNode()->UnpackStructure(mstructin);
    RR_SHARED_PTR<RobotRaconteur::StructureStub> stub = FindStructureStub(type);
    return stub->UnpackStructure(mstructin);
    throw RobotRaconteur::ServiceException("Invalid structure stub type.");
}
RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementNestedElementList> RobotRaconteurServiceIndexFactory::PackPodArray(
   const RR_INTRUSIVE_PTR<RobotRaconteur::RRPodBaseArray>& structure)
{
    RR_UNUSED(structure);
    throw RobotRaconteur::ServiceException("Invalid pod type.");
}
RR_INTRUSIVE_PTR<RobotRaconteur::RRPodBaseArray> RobotRaconteurServiceIndexFactory::UnpackPodArray(
    const RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementNestedElementList>& structure)
{
    RR_UNUSED(structure);
    throw RobotRaconteur::ServiceException("Invalid pod type.");
}
RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementNestedElementList> RobotRaconteurServiceIndexFactory::
    PackPodMultiDimArray(const RR_INTRUSIVE_PTR<RobotRaconteur::RRPodBaseMultiDimArray>& structure)
{
    RR_UNUSED(structure);
    throw RobotRaconteur::ServiceException("Invalid pod type.");
}
RR_INTRUSIVE_PTR<RobotRaconteur::RRPodBaseMultiDimArray> RobotRaconteurServiceIndexFactory::UnpackPodMultiDimArray(
    const RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementNestedElementList>& structure)
{
    RR_UNUSED(structure);
    throw RobotRaconteur::ServiceException("Invalid pod type.");
}
RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementNestedElementList> RobotRaconteurServiceIndexFactory::PackNamedArray(
   const RR_INTRUSIVE_PTR<RobotRaconteur::RRNamedBaseArray>& structure)
{
    RR_UNUSED(structure);
    throw RobotRaconteur::ServiceException("Invalid pod type.");
}
RR_INTRUSIVE_PTR<RobotRaconteur::RRNamedBaseArray> RobotRaconteurServiceIndexFactory::UnpackNamedArray(
    const RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementNestedElementList>& structure)
{
    RR_UNUSED(structure);
    throw RobotRaconteur::ServiceException("Invalid pod type.");
}
RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementNestedElementList> RobotRaconteurServiceIndexFactory::
    PackNamedMultiDimArray(const RR_INTRUSIVE_PTR<RobotRaconteur::RRNamedBaseMultiDimArray>& structure)
{
    RR_UNUSED(structure);
    throw RobotRaconteur::ServiceException("Invalid pod type.");
}
RR_INTRUSIVE_PTR<RobotRaconteur::RRNamedBaseMultiDimArray> RobotRaconteurServiceIndexFactory::UnpackNamedMultiDimArray(
    const RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementNestedElementList>& structure)
{
    RR_UNUSED(structure);
    throw RobotRaconteur::ServiceException("Invalid pod type.");
}
RR_SHARED_PTR<RobotRaconteur::ServiceStub> RobotRaconteurServiceIndexFactory::CreateStub(
    boost::string_ref type, boost::string_ref path, const RR_SHARED_PTR<RobotRaconteur::ClientContext>& context)
{
    std::vector<std::string> res;
    boost::split(res, type, boost::is_from_range('.', '.'));
    std::string servicetype = res.at(0);
    std::string objecttype = res.at(1);
    if (servicetype != "RobotRaconteurServiceIndex")
        return GetNode()->GetServiceType(servicetype)->CreateStub(type, path, context);
    if (objecttype == "ServiceIndex")
    {
        RR_SHARED_PTR<ServiceIndex_stub> o = (RR_MAKE_SHARED<ServiceIndex_stub>(path, context));
        o->RRInitStub();
        return o;
    }
    throw RobotRaconteur::ServiceException("Invalid structure stub type.");
}
RR_SHARED_PTR<RobotRaconteur::ServiceSkel> RobotRaconteurServiceIndexFactory::CreateSkel(
    boost::string_ref type, boost::string_ref path, const RR_SHARED_PTR<RobotRaconteur::RRObject>& obj,
    const RR_SHARED_PTR<RobotRaconteur::ServerContext>& context)
{
    std::vector<std::string> res;
    boost::split(res, type, boost::is_from_range('.', '.'));
    std::string servicetype = res.at(0);
    std::string objecttype = res.at(1);
    if (servicetype != "RobotRaconteurServiceIndex")
        return GetNode()->GetServiceType(servicetype)->CreateSkel(type, path, obj, context);
    if (objecttype == "ServiceIndex")
    {
        RR_SHARED_PTR<ServiceIndex_skel> o = RR_MAKE_SHARED<ServiceIndex_skel>();
        o->Init(path, obj, context);
        return o;
    }
    throw RobotRaconteur::ServiceException("Invalid structure skel type.");
    return RR_SHARED_PTR<RobotRaconteur::ServiceSkel>();
}

RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementNestedElementList> NodeInfo_stub::PackStructure(
   const RR_INTRUSIVE_PTR<RobotRaconteur::RRValue>& s)
{
    RR_INTRUSIVE_PTR<NodeInfo> s2 = RobotRaconteur::rr_cast<NodeInfo>(s);
    std::vector<RR_INTRUSIVE_PTR<RobotRaconteur::MessageElement> > vret;
    vret.push_back(RobotRaconteur::CreateMessageElement(
        "NodeName",
        RobotRaconteur::rr_cast<RobotRaconteur::MessageElementData>(RobotRaconteur::stringToRRArray(s2->NodeName))));
    vret.push_back(RobotRaconteur::CreateMessageElement(
        "NodeID", RobotRaconteur::rr_cast<RobotRaconteur::MessageElementData>(s2->NodeID)));
    vret.push_back(RobotRaconteur::CreateMessageElement(
        "ServiceIndexConnectionURL",
        RobotRaconteur::rr_cast<RobotRaconteur::MessageElementData>(
            RRGetNode()->PackMapType<int32_t, RobotRaconteur::RRArray<char> >(s2->ServiceIndexConnectionURL))));
    return RobotRaconteur::CreateMessageElementNestedElementList(RobotRaconteur::DataTypes_structure_t,
                                                                 "RobotRaconteurServiceIndex.NodeInfo", RR_MOVE(vret));
}
RR_INTRUSIVE_PTR<RobotRaconteur::RRStructure> NodeInfo_stub::UnpackStructure(
    const RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementNestedElementList>& m)
{
    RR_INTRUSIVE_PTR<NodeInfo> ret(new NodeInfo());
    ret->NodeName = RobotRaconteur::RRArrayToString(RobotRaconteur::MessageElement::FindElement(m->Elements, "NodeName")
                                                        ->CastData<RobotRaconteur::RRArray<char> >());
    ret->NodeID = RobotRaconteur::MessageElement::FindElement(m->Elements, "NodeID")
                      ->CastData<RobotRaconteur::RRArray<uint8_t> >();
    ret->ServiceIndexConnectionURL =
        RobotRaconteur::rr_cast<RobotRaconteur::RRMap<int32_t, RobotRaconteur::RRArray<char> > >(
            (RRGetNode()->UnpackMapType<int32_t, RobotRaconteur::RRArray<char> >(
                RobotRaconteur::MessageElement::FindElement(m->Elements, "ServiceIndexConnectionURL")
                    ->CastDataToNestedList(RobotRaconteur::DataTypes_vector_t))));
    return ret;
}

RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementNestedElementList> ServiceInfo_stub::PackStructure(
   const RR_INTRUSIVE_PTR<RobotRaconteur::RRValue>& s)
{
    RR_INTRUSIVE_PTR<ServiceInfo> s2 = RobotRaconteur::rr_cast<ServiceInfo>(s);
    std::vector<RR_INTRUSIVE_PTR<RobotRaconteur::MessageElement> > vret;
    vret.push_back(RobotRaconteur::CreateMessageElement(
        "Name",
        RobotRaconteur::rr_cast<RobotRaconteur::MessageElementData>(RobotRaconteur::stringToRRArray(s2->Name))));
    vret.push_back(RobotRaconteur::CreateMessageElement("RootObjectType",
                                                        RobotRaconteur::rr_cast<RobotRaconteur::MessageElementData>(
                                                            RobotRaconteur::stringToRRArray(s2->RootObjectType))));
    vret.push_back(RobotRaconteur::CreateMessageElement(
        "RootObjectImplements",
        RobotRaconteur::rr_cast<RobotRaconteur::MessageElementData>(
            RRGetNode()->PackMapType<int32_t, RobotRaconteur::RRArray<char> >(s2->RootObjectImplements))));
    vret.push_back(RobotRaconteur::CreateMessageElement(
        "ConnectionURL", RobotRaconteur::rr_cast<RobotRaconteur::MessageElementData>(
                             RRGetNode()->PackMapType<int32_t, RobotRaconteur::RRArray<char> >(s2->ConnectionURL))));
    vret.push_back(RobotRaconteur::CreateMessageElement(
        "Attributes", RobotRaconteur::rr_cast<RobotRaconteur::MessageElementData>(
                          RRGetNode()->PackMapType<std::string, RobotRaconteur::RRValue>(s2->Attributes))));
    return RobotRaconteur::CreateMessageElementNestedElementList(
        RobotRaconteur::DataTypes_structure_t, "RobotRaconteurServiceIndex.ServiceInfo", RR_MOVE(vret));
}
RR_INTRUSIVE_PTR<RobotRaconteur::RRStructure> ServiceInfo_stub::UnpackStructure(
    const RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementNestedElementList>& m)
{
    RR_INTRUSIVE_PTR<ServiceInfo> ret(new ServiceInfo());
    ret->Name = RobotRaconteur::RRArrayToString(
        RobotRaconteur::MessageElement::FindElement(m->Elements, "Name")->CastData<RobotRaconteur::RRArray<char> >());
    ret->RootObjectType =
        RobotRaconteur::RRArrayToString(RobotRaconteur::MessageElement::FindElement(m->Elements, "RootObjectType")
                                            ->CastData<RobotRaconteur::RRArray<char> >());
    ret->RootObjectImplements =
        RobotRaconteur::rr_cast<RobotRaconteur::RRMap<int32_t, RobotRaconteur::RRArray<char> > >(
            (RRGetNode()->UnpackMapType<int32_t, RobotRaconteur::RRArray<char> >(
                RobotRaconteur::MessageElement::FindElement(m->Elements, "RootObjectImplements")
                    ->CastDataToNestedList(RobotRaconteur::DataTypes_vector_t))));
    ret->ConnectionURL = RobotRaconteur::rr_cast<RobotRaconteur::RRMap<int32_t, RobotRaconteur::RRArray<char> > >(
        (RRGetNode()->UnpackMapType<int32_t, RobotRaconteur::RRArray<char> >(
            RobotRaconteur::MessageElement::FindElement(m->Elements, "ConnectionURL")
                ->CastDataToNestedList(RobotRaconteur::DataTypes_vector_t))));
    ret->Attributes = RobotRaconteur::rr_cast<RobotRaconteur::RRMap<std::string, RobotRaconteur::RRValue> >(
        (RRGetNode()->UnpackMapType<std::string, RobotRaconteur::RRValue>(
            RobotRaconteur::MessageElement::FindElement(m->Elements, "Attributes")
                ->CastDataToNestedList(RobotRaconteur::DataTypes_dictionary_t))));
    return ret;
}

ServiceIndex_stub::ServiceIndex_stub(boost::string_ref path, const RR_SHARED_PTR<RobotRaconteur::ClientContext>& c)
    : RobotRaconteur::ServiceStub(path, c)
{}
void ServiceIndex_stub::RRInitStub() {}

RR_INTRUSIVE_PTR<RobotRaconteur::RRMap<int32_t, ServiceInfo> > ServiceIndex_stub::GetLocalNodeServices()
{
    RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> rr_req =
        RobotRaconteur::CreateMessageEntry(RobotRaconteur::MessageEntryType_FunctionCallReq, "GetLocalNodeServices");
    RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> rr_ret = ProcessRequest(rr_req);
    RR_INTRUSIVE_PTR<RobotRaconteur::MessageElement> rr_me = rr_ret->FindElement("return");
    return RobotRaconteur::rr_cast<RobotRaconteur::RRMap<int32_t, ServiceInfo> >(
        (RRGetNode()->UnpackMapType<int32_t, ServiceInfo>(
            rr_me->CastDataToNestedList(RobotRaconteur::DataTypes_vector_t))));
}

RR_INTRUSIVE_PTR<RobotRaconteur::RRMap<int32_t, NodeInfo> > ServiceIndex_stub::GetRoutedNodes()
{
    RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> rr_req =
        RobotRaconteur::CreateMessageEntry(RobotRaconteur::MessageEntryType_FunctionCallReq, "GetRoutedNodes");
    RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> rr_ret = ProcessRequest(rr_req);
    RR_INTRUSIVE_PTR<RobotRaconteur::MessageElement> rr_me = rr_ret->FindElement("return");
    return RobotRaconteur::rr_cast<RobotRaconteur::RRMap<int32_t, NodeInfo> >(
        (RRGetNode()->UnpackMapType<int32_t, NodeInfo>(
            rr_me->CastDataToNestedList(RobotRaconteur::DataTypes_vector_t))));
}

RR_INTRUSIVE_PTR<RobotRaconteur::RRMap<int32_t, NodeInfo> > ServiceIndex_stub::GetDetectedNodes()
{
    RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> rr_req =
        RobotRaconteur::CreateMessageEntry(RobotRaconteur::MessageEntryType_FunctionCallReq, "GetDetectedNodes");
    RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> rr_ret = ProcessRequest(rr_req);
    RR_INTRUSIVE_PTR<RobotRaconteur::MessageElement> rr_me = rr_ret->FindElement("return");
    return RobotRaconteur::rr_cast<RobotRaconteur::RRMap<int32_t, NodeInfo> >(
        (RRGetNode()->UnpackMapType<int32_t, NodeInfo>(
            rr_me->CastDataToNestedList(RobotRaconteur::DataTypes_vector_t))));
}

boost::signals2::signal<void()>& ServiceIndex_stub::get_LocalNodeServicesChanged()
{
    return rrvar_LocalNodeServicesChanged;
}

void ServiceIndex_stub::DispatchEvent(const RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry>& rr_m)
{
    if (rr_m->MemberName == "LocalNodeServicesChanged")
    {
        get_LocalNodeServicesChanged()();
        return;
    }
    throw RobotRaconteur::MemberNotFoundException("Member not found");
}
void ServiceIndex_stub::DispatchPipeMessage(const RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry>& m)
{
    RR_UNUSED(m);
    throw RobotRaconteur::MemberNotFoundException("Member not found");
}
void ServiceIndex_stub::DispatchWireMessage(const RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry>& m)
{
    RR_UNUSED(m);
    throw RobotRaconteur::MemberNotFoundException("Member not found");
}
RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> ServiceIndex_stub::CallbackCall(
    const RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry>& rr_m)
{
    RobotRaconteur::MessageStringRef ename = rr_m->MemberName;
    RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> rr_mr =
        RobotRaconteur::CreateMessageEntry(RobotRaconteur::MessageEntryType_CallbackCallRet, ename);
    rr_mr->ServicePath = rr_m->ServicePath;
    rr_mr->RequestID = rr_m->RequestID;
    throw RobotRaconteur::MemberNotFoundException("Member not found");
}

std::string ServiceIndex_stub::RRType() { return "RobotRaconteurServiceIndex.ServiceIndex"; }
void ServiceIndex_stub::RRClose()
{
    rrvar_LocalNodeServicesChanged.disconnect_all_slots();
    ServiceStub::RRClose();
}

void ServiceIndex_stub::async_GetLocalNodeServices(
    boost::function<void(const RR_INTRUSIVE_PTR<RobotRaconteur::RRMap<int32_t, ServiceInfo> >&,
                         const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>&)>
        rr_handler,
    int32_t rr_timeout)
{
    RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> rr_req =
        RobotRaconteur::CreateMessageEntry(RobotRaconteur::MessageEntryType_FunctionCallReq, "GetLocalNodeServices");
    AsyncProcessRequest(rr_req,
                        boost::bind(&ServiceIndex_stub::rrend_GetLocalNodeServices,
                                    RobotRaconteur::rr_cast<ServiceIndex_stub>(shared_from_this()),
                                    RR_BOOST_PLACEHOLDERS(_1), RR_BOOST_PLACEHOLDERS(_2), rr_handler),
                        rr_timeout);
}

void ServiceIndex_stub::rrend_GetLocalNodeServices(
    const RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry>& m, const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>& err,
    boost::function<void(const RR_INTRUSIVE_PTR<RobotRaconteur::RRMap<int32_t, ServiceInfo> >&,
                         const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>&)>
        handler)
{
    if (err)
    {
        handler(RR_INTRUSIVE_PTR<RobotRaconteur::RRMap<int32_t, ServiceInfo> >(), err);
        return;
    }
    if (m->Error != RobotRaconteur::MessageErrorType_None)
    {
        handler(RR_INTRUSIVE_PTR<RobotRaconteur::RRMap<int32_t, ServiceInfo> >(),
                RobotRaconteur::RobotRaconteurExceptionUtil::MessageEntryToException(m));
        return;
    }
    RR_INTRUSIVE_PTR<RobotRaconteur::MessageElement> me = m->FindElement("return");
    handler(RobotRaconteur::rr_cast<RobotRaconteur::RRMap<int32_t, ServiceInfo> >(
                (RRGetNode()->UnpackMapType<int32_t, ServiceInfo>(
                    me->CastDataToNestedList(RobotRaconteur::DataTypes_vector_t)))),
            RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>());
}
void ServiceIndex_stub::async_GetRoutedNodes(
    boost::function<void(const RR_INTRUSIVE_PTR<RobotRaconteur::RRMap<int32_t, NodeInfo> >&,
                         const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>&)>
        rr_handler,
    int32_t rr_timeout)
{
    RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> rr_req =
        RobotRaconteur::CreateMessageEntry(RobotRaconteur::MessageEntryType_FunctionCallReq, "GetRoutedNodes");
    AsyncProcessRequest(rr_req,
                        boost::bind(&ServiceIndex_stub::rrend_GetRoutedNodes,
                                    RobotRaconteur::rr_cast<ServiceIndex_stub>(shared_from_this()),
                                    RR_BOOST_PLACEHOLDERS(_1), RR_BOOST_PLACEHOLDERS(_2), rr_handler),
                        rr_timeout);
}

void ServiceIndex_stub::rrend_GetRoutedNodes(
    const RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry>& m, const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>& err,
    boost::function<void(const RR_INTRUSIVE_PTR<RobotRaconteur::RRMap<int32_t, NodeInfo> >&,
                         const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>&)>
        handler)
{
    if (err)
    {
        handler(RR_INTRUSIVE_PTR<RobotRaconteur::RRMap<int32_t, NodeInfo> >(), err);
        return;
    }
    if (m->Error != RobotRaconteur::MessageErrorType_None)
    {
        handler(RR_INTRUSIVE_PTR<RobotRaconteur::RRMap<int32_t, NodeInfo> >(),
                RobotRaconteur::RobotRaconteurExceptionUtil::MessageEntryToException(m));
        return;
    }
    RR_INTRUSIVE_PTR<RobotRaconteur::MessageElement> me = m->FindElement("return");
    handler(RobotRaconteur::rr_cast<RobotRaconteur::RRMap<int32_t, NodeInfo> >(
                (RRGetNode()->UnpackMapType<int32_t, NodeInfo>(
                    me->CastDataToNestedList(RobotRaconteur::DataTypes_vector_t)))),
            RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>());
}
void ServiceIndex_stub::async_GetDetectedNodes(
    boost::function<void(const RR_INTRUSIVE_PTR<RobotRaconteur::RRMap<int32_t, NodeInfo> >&,
                         const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>&)>
        rr_handler,
    int32_t rr_timeout)
{
    RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> rr_req =
        RobotRaconteur::CreateMessageEntry(RobotRaconteur::MessageEntryType_FunctionCallReq, "GetDetectedNodes");
    AsyncProcessRequest(rr_req,
                        boost::bind(&ServiceIndex_stub::rrend_GetDetectedNodes,
                                    RobotRaconteur::rr_cast<ServiceIndex_stub>(shared_from_this()),
                                    RR_BOOST_PLACEHOLDERS(_1), RR_BOOST_PLACEHOLDERS(_2), rr_handler),
                        rr_timeout);
}

void ServiceIndex_stub::rrend_GetDetectedNodes(
    const RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry>& m, const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>& err,
    boost::function<void(const RR_INTRUSIVE_PTR<RobotRaconteur::RRMap<int32_t, NodeInfo> >&,
                         const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>&)>
        handler)
{
    if (err)
    {
        handler(RR_INTRUSIVE_PTR<RobotRaconteur::RRMap<int32_t, NodeInfo> >(), err);
        return;
    }
    if (m->Error != RobotRaconteur::MessageErrorType_None)
    {
        handler(RR_INTRUSIVE_PTR<RobotRaconteur::RRMap<int32_t, NodeInfo> >(),
                RobotRaconteur::RobotRaconteurExceptionUtil::MessageEntryToException(m));
        return;
    }
    RR_INTRUSIVE_PTR<RobotRaconteur::MessageElement> me = m->FindElement("return");
    handler(RobotRaconteur::rr_cast<RobotRaconteur::RRMap<int32_t, NodeInfo> >(
                (RRGetNode()->UnpackMapType<int32_t, NodeInfo>(
                    me->CastDataToNestedList(RobotRaconteur::DataTypes_vector_t)))),
            RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>());
}

ServiceIndex_skel::ServiceIndex_skel()
{
    rr_InitPipeServersRun = false;
    rr_InitWireServersRun = false;
}

void ServiceIndex_skel::Init(boost::string_ref path, const RR_SHARED_PTR<RobotRaconteur::RRObject>& object,
                             const RR_SHARED_PTR<RobotRaconteur::ServerContext>& context)
{
    uncastobj = object;
    rr_InitPipeServersRun = false;
    rr_InitWireServersRun = false;
    ServiceSkel::Init(path, object, context);
}
RR_SHARED_PTR<RobotRaconteurServiceIndex::ServiceIndex> ServiceIndex_skel::get_obj()
{
    return RobotRaconteur::rr_cast<RobotRaconteurServiceIndex::ServiceIndex>(uncastobj);
}
void ServiceIndex_skel::ReleaseCastObject() {}
std::string ServiceIndex_skel::GetObjectType() { return "RobotRaconteurServiceIndex.ServiceIndex"; }
RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> ServiceIndex_skel::CallGetProperty(
    const RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry>& m)
{
    RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> mr =
        RobotRaconteur::CreateMessageEntry(RobotRaconteur::MessageEntryType_PropertyGetRes, m->MemberName);
    throw RobotRaconteur::MemberNotFoundException("Member not found");
}

RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> ServiceIndex_skel::CallSetProperty(
    const RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry>& m)
{
    RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> mr =
        RobotRaconteur::CreateMessageEntry(RobotRaconteur::MessageEntryType_PropertySetRes, m->MemberName);
    throw RobotRaconteur::MemberNotFoundException("Member not found");
}

RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> ServiceIndex_skel::CallFunction(
    const RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry>& rr_m)
{
    RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> rr_mr =
        RobotRaconteur::CreateMessageEntry(RobotRaconteur::MessageEntryType_FunctionCallRes, rr_m->MemberName);
    if (rr_m->MemberName == "GetLocalNodeServices")
    {
        RR_INTRUSIVE_PTR<RobotRaconteur::RRMap<int32_t, ServiceInfo> > rr_return = get_obj()->GetLocalNodeServices();
        rr_mr->AddElement(RobotRaconteur::CreateMessageElement(
            "return", RobotRaconteur::rr_cast<RobotRaconteur::MessageElementData>(
                          RRGetNode()->PackMapType<int32_t, ServiceInfo>(rr_return))));
        return rr_mr;
    }
    if (rr_m->MemberName == "GetRoutedNodes")
    {
        RR_INTRUSIVE_PTR<RobotRaconteur::RRMap<int32_t, NodeInfo> > rr_return = get_obj()->GetRoutedNodes();
        rr_mr->AddElement(RobotRaconteur::CreateMessageElement(
            "return", RobotRaconteur::rr_cast<RobotRaconteur::MessageElementData>(
                          RRGetNode()->PackMapType<int32_t, NodeInfo>(rr_return))));
        return rr_mr;
    }
    if (rr_m->MemberName == "GetDetectedNodes")
    {
        RR_INTRUSIVE_PTR<RobotRaconteur::RRMap<int32_t, NodeInfo> > rr_return = get_obj()->GetDetectedNodes();
        rr_mr->AddElement(RobotRaconteur::CreateMessageElement(
            "return", RobotRaconteur::rr_cast<RobotRaconteur::MessageElementData>(
                          RRGetNode()->PackMapType<int32_t, NodeInfo>(rr_return))));
        return rr_mr;
    }
    throw RobotRaconteur::MemberNotFoundException("Member not found");
}

void ServiceIndex_skel::rr_LocalNodeServicesChanged_Handler()
{
    RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> rr_mm =
        RobotRaconteur::CreateMessageEntry(RobotRaconteur::MessageEntryType_EventReq, "LocalNodeServicesChanged");
    SendEvent(rr_mm);
}
void ServiceIndex_skel::RegisterEvents(const RR_SHARED_PTR<RobotRaconteur::RRObject>& rrobj1)
{
    RR_SHARED_PTR<RobotRaconteurServiceIndex::ServiceIndex> obj =
        RobotRaconteur::rr_cast<RobotRaconteurServiceIndex::ServiceIndex>(rrobj1);
    LocalNodeServicesChanged_rrconnection = obj->get_LocalNodeServicesChanged().connect(
        boost::bind(&ServiceIndex_skel::rr_LocalNodeServicesChanged_Handler,
                    RobotRaconteur::rr_cast<ServiceIndex_skel>(shared_from_this())));
    RobotRaconteur::ServiceSkel::RegisterEvents(rrobj1);
}

void ServiceIndex_skel::UnregisterEvents(const RR_SHARED_PTR<RobotRaconteur::RRObject>& rrobj1)
{
    LocalNodeServicesChanged_rrconnection.disconnect();
    RobotRaconteur::ServiceSkel::UnregisterEvents(rrobj1);
}

RR_SHARED_PTR<RobotRaconteur::RRObject> ServiceIndex_skel::GetSubObj(boost::string_ref name, boost::string_ref ind)
{
    RR_UNUSED(name);
    RR_UNUSED(ind);
    throw RobotRaconteur::MemberNotFoundException("Member not found");
}

void ServiceIndex_skel::InitPipeServers(const RR_SHARED_PTR<RobotRaconteur::RRObject>& rrobj1)
{
    if (rr_InitPipeServersRun)
        return;
    rr_InitPipeServersRun = true;
    RR_SHARED_PTR<RobotRaconteurServiceIndex::ServiceIndex> obj =
        RobotRaconteur::rr_cast<RobotRaconteurServiceIndex::ServiceIndex>(rrobj1);
}

void ServiceIndex_skel::DispatchPipeMessage(const RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry>& m, uint32_t e)
{
    RR_UNUSED(m);
    RR_UNUSED(e);
    throw RobotRaconteur::MemberNotFoundException("Member not found");
}

RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> ServiceIndex_skel::CallPipeFunction(
    const RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry>& m, uint32_t e)
{
    RR_UNUSED(m);
    RR_UNUSED(e);
    throw RobotRaconteur::MemberNotFoundException("Member not found");
}

void ServiceIndex_skel::InitWireServers(const RR_SHARED_PTR<RobotRaconteur::RRObject>& rrobj1)
{
    if (rr_InitWireServersRun)
        return;
    rr_InitWireServersRun = true;
    RR_SHARED_PTR<RobotRaconteurServiceIndex::ServiceIndex> obj =
        RobotRaconteur::rr_cast<RobotRaconteurServiceIndex::ServiceIndex>(rrobj1);
}

void ServiceIndex_skel::DispatchWireMessage(const RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry>& m, uint32_t e)
{
    RR_UNUSED(m);
    RR_UNUSED(e);
    throw RobotRaconteur::MemberNotFoundException("Member not found");
}

RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> ServiceIndex_skel::CallWireFunction(
    const RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry>& m, uint32_t e)
{
    RR_UNUSED(m);
    RR_UNUSED(e);
    throw RobotRaconteur::MemberNotFoundException("Member not found");
}

void ServiceIndex_skel::InitCallbackServers(const RR_SHARED_PTR<RobotRaconteur::RRObject>& o)
{
    RR_SHARED_PTR<RobotRaconteurServiceIndex::ServiceIndex> obj =
        RobotRaconteur::rr_cast<RobotRaconteurServiceIndex::ServiceIndex>(o);
}
RR_SHARED_PTR<void> ServiceIndex_skel::GetCallbackFunction(uint32_t endpoint, boost::string_ref membername)
{
    RR_UNUSED(endpoint);
    RR_UNUSED(membername);
    throw RobotRaconteur::MemberNotFoundException("Member not found");
}

RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> ServiceIndex_skel::CallMemoryFunction(
    const RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry>& m, const RR_SHARED_PTR<RobotRaconteur::Endpoint>& e)
{
    RR_UNUSED(m);
    RR_UNUSED(e);
    throw RobotRaconteur::MemberNotFoundException("Member not found");
}

} // namespace RobotRaconteurServiceIndex
