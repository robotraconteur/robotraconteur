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

#include "RobotRaconteur/ServiceIndexer.h"
#include <boost/algorithm/string.hpp>

using namespace RobotRaconteurServiceIndex;
using namespace std;

namespace RobotRaconteur
{

ServiceIndexer::ServiceIndexer(RR_SHARED_PTR<RobotRaconteurNode> node) { this->node = node; }

RR_SHARED_PTR<RobotRaconteurNode> ServiceIndexer::GetNode()
{
    RR_SHARED_PTR<RobotRaconteurNode> n = node.lock();
    if (!n)
        throw InvalidOperationException("Node has been released");
    return n;
}

RR_INTRUSIVE_PTR<RobotRaconteur::RRMap<int32_t, RobotRaconteurServiceIndex::ServiceInfo> > ServiceIndexer::
    GetLocalNodeServices()
{
    if (Transport::GetCurrentTransportConnectionURL() == "")
        throw ServiceException("GetLocalNodeServices must be called through a transport that supports node discovery");

    RR_INTRUSIVE_PTR<RRMap<int32_t, ServiceInfo> > o = AllocateEmptyRRMap<int32_t, ServiceInfo>();
    int32_t count = 0;

    vector<string> names = GetNode()->GetRegisteredServiceNames();
    for (vector<string>::iterator e = names.begin(); e != names.end(); ++e)
    {
        try
        {
            RR_SHARED_PTR<ServerContext> c = GetNode()->GetService(*e);
            RR_INTRUSIVE_PTR<ServiceInfo> s(new ServiceInfo());
            s->Attributes = AllocateRRMap<string, RRValue>(c->GetAttributes());

            for (std::map<std::string, RR_INTRUSIVE_PTR<RRValue> >::iterator e = s->Attributes->begin();
                 e != s->Attributes->end();)
            {
                RR_INTRUSIVE_PTR<RRBaseArray> a = RR_DYNAMIC_POINTER_CAST<RRBaseArray>(e->second);
                if (!a)
                {
                    s->Attributes->erase(e++);
                }
                else
                {
                    e++;
                }
            }

            s->Name = c->GetServiceName();
            s->RootObjectType = c->GetRootObjectType(RobotRaconteurVersion());
            s->ConnectionURL = AllocateEmptyRRMap<int32_t, RRArray<char> >();
            s->ConnectionURL->insert(make_pair(
                1, stringToRRArray(Transport::GetCurrentTransportConnectionURL() + "?nodeid=" +
                                   boost::replace_first_copy(
                                       boost::replace_first_copy(GetNode()->NodeID().ToString(), "{", ""), "}", "") +
                                   "&service=" + s->Name)));
            s->RootObjectImplements = AllocateEmptyRRMap<int32_t, RRArray<char> >();

            boost::tuple<std::string, std::string> servicetype;
            std::string roottype = c->GetRootObjectType(RobotRaconteurVersion());
            boost::tuple<boost::string_ref, boost::string_ref> servicetype1 = SplitQualifiedName(roottype);
            servicetype = boost::make_tuple(servicetype1.get<0>().to_string(), servicetype1.get<1>().to_string());

            vector<RR_SHARED_PTR<ServiceEntryDefinition> > objs = c->GetServiceDef()->ServiceDef()->Objects;

            RR_SHARED_PTR<ServiceEntryDefinition> obj;
            obj.reset();

            for (vector<RR_SHARED_PTR<ServiceEntryDefinition> >::iterator ee = objs.begin(); ee != objs.end(); ++ee)
            {
                if ((*ee)->Name == servicetype.get<1>())
                {
                    obj = *ee;
                }
            }

            if (!obj)
                continue;

            int32_t icount = 0;
            for (vector<string>::iterator ee = obj->Implements.begin(); ee != obj->Implements.end(); ++ee)
            {
                s->RootObjectImplements->insert(make_pair(icount, stringToRRArray(*ee)));
                icount++;
            }

            o->insert(make_pair(count, s));
            count++;
        }
        catch (std::exception&)
        {}
    }

    return o;
}

RR_INTRUSIVE_PTR<RobotRaconteur::RRMap<int32_t, RobotRaconteurServiceIndex::NodeInfo> > ServiceIndexer::GetRoutedNodes()
{
    RR_INTRUSIVE_PTR<RRMap<int32_t, NodeInfo> > o = AllocateEmptyRRMap<int32_t, NodeInfo>();
    /*if (GetNode()->IsMessageRouterAvailable())
    {

        std::vector<RR_INTRUSIVE_PTR<MessageRouter::Route> > r=GetNode()->GetMessageRouter()->GetActiveRoutes();

        size_t len=r.size();



        for (size_t i=0; i<len; i++)
        {
            RR_SHARED_PTR<NodeInfo> ii=RR_MAKE_SHARED<NodeInfo>();
            ii->NodeID=VectorToRRArray<uint8_t>(r.at(i)->NodeID.ToByteArray());
            ii->NodeName=r.at(i)->NodeName;

            RR_INTRUSIVE_PTR<RRMap<int32_t,RRArray<char> > > curl=RR_MAKE_SHARED<RRMap<int32_t,RRArray<char> > >();
            curl->map.insert(make_pair(0,stringToRRArray(r.at(i)->ConnectURL)));

            ii->ServiceIndexConnectionURL=curl;
            o->map.insert(make_pair((int32_t)i,ii));
        }
    }*/

    return o;
}

RR_INTRUSIVE_PTR<RobotRaconteur::RRMap<int32_t, RobotRaconteurServiceIndex::NodeInfo> > ServiceIndexer::
    GetDetectedNodes()
{

    std::vector<NodeDiscoveryInfo> nodeids = GetNode()->GetDetectedNodes();

    int32_t count = 0;

    RR_INTRUSIVE_PTR<RRMap<int32_t, NodeInfo> > o = AllocateEmptyRRMap<int32_t, NodeInfo>();

    for (std::vector<NodeDiscoveryInfo>::iterator e = nodeids.begin(); e != nodeids.end(); ++e)
    {

        RR_INTRUSIVE_PTR<NodeInfo> ii(new NodeInfo());
        ii->NodeID = ArrayToRRArray<uint8_t>(e->NodeID.ToByteArray());
        ii->NodeName = e->NodeName;

        RR_INTRUSIVE_PTR<RRMap<int32_t, RRArray<char> > > curl = AllocateEmptyRRMap<int32_t, RRArray<char> >();
        for (size_t j = 0; j < e->URLs.size(); j++)
        {
            curl->insert(make_pair(boost::numeric_cast<int32_t>(j), stringToRRArray(e->URLs.at(j).URL)));
        }

        ii->ServiceIndexConnectionURL = curl;
        o->insert(make_pair(boost::numeric_cast<int32_t>(count), ii));
        count++;
    }

    return o;
}

boost::signals2::signal<void()>& ServiceIndexer::get_LocalNodeServicesChanged() { return ev; }

} // namespace RobotRaconteur
