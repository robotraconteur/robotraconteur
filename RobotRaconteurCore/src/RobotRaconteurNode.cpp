// Copyright 2011-2018 Wason Technology, LLC
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

#include "RobotRaconteur/RobotRaconteurNode.h"
#include <boost/algorithm/string.hpp>
#include "RobotRaconteur/Client.h"
#include "RobotRaconteur/Service.h"
#include "RobotRaconteur/RobotRaconteurServiceIndex.h"
#include "RobotRaconteur/ServiceIndexer.h"
#include "RobotRaconteur/DataTypes.h"
#include <boost/regex.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/foreach.hpp>
#include <boost/range/adaptors.hpp>
#include <boost/range/algorithm.hpp>
#include <boost/filesystem.hpp>

#include "Discovery_private.h"
#include "RobotRaconteurNode_connector_private.h"

namespace RobotRaconteur
{
	static void RobotRaconteurNode_empty_handler() {}


	static void RobotRaconteurNode_empty_handler(RR_SHARED_PTR<RobotRaconteurException> ) {}


RobotRaconteurNode RobotRaconteurNode::m_s;
RR_SHARED_PTR<RobotRaconteurNode> RobotRaconteurNode::m_sp;

bool RobotRaconteurNode::is_init=false;
boost::mutex RobotRaconteurNode::init_lock;

RobotRaconteurNode::RobotRaconteurNode()
{
	is_shutdown=false;
	NodeID_set=false;
	NodeName_set=false;
	PeriodicCleanupTask_timerstarted=false;

	transport_count=0;
	

	EndpointInactivityTimeout = 600000;
	TransportInactivityTimeout = 600000;
	RequestTimeout = 15000;
	MemoryMaxTransferSize = 102400;
	instance_is_init=false;
	
}

void RobotRaconteurNode::Init()
{

	boost::mutex::scoped_lock lock(init_lock);
	if (instance_is_init) return;

	{
		boost::mutex::scoped_lock lock(random_generator_lock);
		random_generator = RR_MAKE_SHARED<boost::random::random_device>();
	}

	RegisterServiceType(RR_MAKE_SHARED<RobotRaconteurServiceIndex::RobotRaconteurServiceIndexFactory>());
	RegisterService("RobotRaconteurServiceIndex","RobotRaconteurServiceIndex",RR_MAKE_SHARED<ServiceIndexer>(shared_from_this()));
	
	service_state_nonce = GetRandomString(16);

	instance_is_init=true;

	//Deal with possible race in boost::filesystem::path
	boost::filesystem::path::codecvt();

	m_Discovery = RR_MAKE_SHARED<detail::Discovery>(shared_from_this());
	
}

static void RobotRaconteurNode_emptydeleter(RobotRaconteurNode* n) {}

RobotRaconteurNode* RobotRaconteurNode::s()
{
	if(!is_init)
	{
		is_init=true;

		m_sp.reset(&m_s,&RobotRaconteurNode_emptydeleter);
		m_s._internal_accept_owner(&m_sp,&m_s);
		m_s.Init();
	}
	return &m_s;
}

RR_SHARED_PTR<RobotRaconteurNode> RobotRaconteurNode::sp()
{
	RobotRaconteurNode::s();
	return m_sp;
}

NodeID RobotRaconteurNode::NodeID()
{
	boost::mutex::scoped_lock lock(id_lock);
	if (!NodeID_set)
	{
		m_NodeID=RobotRaconteur::NodeID::NewUniqueID();
		NodeID_set=true;
	}

	return m_NodeID;
}

std::string RobotRaconteurNode::NodeName()
{
	boost::mutex::scoped_lock lock(id_lock);
	if (!NodeName_set)
	{
		m_NodeName="";
		NodeName_set=true;
	}
	return m_NodeName;
}

void RobotRaconteurNode::SetNodeID(const RobotRaconteur::NodeID& id)
{
	boost::mutex::scoped_lock lock(id_lock);
	if (NodeID_set) throw InvalidOperationException("NodeID already set");
	m_NodeID=id;
	NodeID_set=true;
}

void RobotRaconteurNode::SetNodeName(const std::string& name)
{
	if (name.size() > 1024) throw InvalidArgumentException("NodeName too long");
	if(!boost::regex_match(name,boost::regex("^[a-zA-Z][a-zA-Z0-9_\\.\\-]*$")))
	{
		throw InvalidArgumentException("\"" + name + "\" is an invalid NodeName");
	}

	boost::mutex::scoped_lock lock(id_lock);
	if (NodeName_set) throw InvalidOperationException("NodeName already set");
	m_NodeName=name;
	NodeName_set=true;
}

RR_SHARED_PTR<ServiceFactory> RobotRaconteurNode::GetServiceType(const std::string& servicename)
{
	
	boost::shared_lock<boost::shared_mutex> lock(service_factories_lock);
	RR_UNORDERED_MAP<std::string, RR_SHARED_PTR<ServiceFactory> >::iterator e1 = service_factories.find(servicename);
	if(e1==service_factories.end())
	{
		throw ServiceException("Unknown service type");
	}
	return e1->second;

}

bool RobotRaconteurNode::IsServiceTypeRegistered(const std::string& servicename)
{
	boost::shared_lock<boost::shared_mutex> lock(service_factories_lock);
	RR_UNORDERED_MAP<std::string, RR_SHARED_PTR<ServiceFactory> >::iterator e1 = service_factories.find(servicename);
	return e1 != service_factories.end();
}

void RobotRaconteurNode::RegisterServiceType(RR_SHARED_PTR<ServiceFactory> factory)
{
	boost::unique_lock<boost::shared_mutex> lock(service_factories_lock);
	
	if (boost::ends_with(factory->GetServiceName(),"_signed")) throw ServiceException("Could not verify signed service definition");

	if(service_factories.count(factory->GetServiceName())!=0)
	{
		throw ServiceException("Service type already registered");

		
	}

	
	factory->ServiceDef()->CheckVersion();

	factory->SetNode(shared_from_this());



	service_factories.insert(std::make_pair(factory->GetServiceName(),factory));
}

void RobotRaconteurNode::UnregisterServiceType(const std::string& type)
{
	boost::unique_lock<boost::shared_mutex> lock(service_factories_lock);
	RR_UNORDERED_MAP<std::string, RR_SHARED_PTR<ServiceFactory> >::iterator e1 = service_factories.find(type);
	if (e1==service_factories.end()) throw InvalidArgumentException("Service type not registered");
	service_factories.erase(e1);
}


std::vector<std::string> RobotRaconteurNode::GetRegisteredServiceTypes()
{
	boost::shared_lock<boost::shared_mutex> lock(service_factories_lock);
	std::vector<std::string> o;
	boost::copy(service_factories | boost::adaptors::map_keys, std::back_inserter(o));
	return o;
}

uint32_t RobotRaconteurNode::RegisterTransport(RR_SHARED_PTR<Transport> transport)
{
	{
		boost::mutex::scoped_lock lock(transports_lock);
		if (transport_count >= std::numeric_limits<uint32_t>::max())
			transport_count=0;
		else
			transport_count++;
		transport->TransportID=transport_count;
		transports.insert(std::make_pair(transport_count,transport));
	}

	RR_SHARED_PTR<ITransportTimeProvider> t=RR_DYNAMIC_POINTER_CAST<ITransportTimeProvider>(transport);
	if (t)
	{
		boost::unique_lock<boost::shared_mutex> lock(time_provider_lock);
		RR_SHARED_PTR<ITransportTimeProvider> t2=time_provider.lock();
		if (!t2)
		{
			time_provider=t;
		}

	}
	return transport->TransportID;
}


RR_INTRUSIVE_PTR<MessageElementStructure> RobotRaconteurNode::PackStructure(RR_INTRUSIVE_PTR<RRStructure> structure)
{
	
	if (!structure) return RR_INTRUSIVE_PTR<MessageElementStructure>();

	std::string type=structure->RRType();

	std::string servicetype=SplitQualifiedName(type).get<0>();
	//std::string structuretype=res[1];

	RR_SHARED_PTR<ServiceFactory> factory=GetServiceType(servicetype);

	return factory->PackStructure(structure);

}

RR_INTRUSIVE_PTR<RRStructure> RobotRaconteurNode::UnpackStructure(RR_INTRUSIVE_PTR<MessageElementStructure> structure)
{
	if (!structure) return RR_INTRUSIVE_PTR<RRStructure>();

	std::string type=structure->Type;

	
	std::string servicetype=SplitQualifiedName(type).get<0>();
	//std::string structuretype=res[1];

	RR_SHARED_PTR<ServiceFactory> factory=GetServiceType(servicetype);

	return rr_cast<RRStructure>(factory->UnpackStructure(structure));
	
}

RR_INTRUSIVE_PTR<MessageElementPodArray> RobotRaconteurNode::PackPodArray(RR_INTRUSIVE_PTR<RRPodBaseArray> a)
{

	if (!a) return RR_INTRUSIVE_PTR<MessageElementPodArray>();

	std::string type = a->RRElementTypeString();

	std::string servicetype = SplitQualifiedName(type).get<0>();
	//std::string structuretype=res[1];

	RR_SHARED_PTR<ServiceFactory> factory = GetServiceType(servicetype);

	return factory->PackPodArray(a);
}

RR_INTRUSIVE_PTR<RRPodBaseArray> RobotRaconteurNode::UnpackPodArray(RR_INTRUSIVE_PTR<MessageElementPodArray> a)
{
	if (!a) return RR_INTRUSIVE_PTR<RRPodBaseArray>();

	std::string type = a->Type;


	std::string servicetype = SplitQualifiedName(type).get<0>();
	//std::string structuretype=res[1];

	RR_SHARED_PTR<ServiceFactory> factory = GetServiceType(servicetype);

	return rr_cast<RRPodBaseArray>(factory->UnpackPodArray(a));
}

RR_INTRUSIVE_PTR<MessageElementPodMultiDimArray> RobotRaconteurNode::PackPodMultiDimArray(RR_INTRUSIVE_PTR<RRPodBaseMultiDimArray> a)
{

	if (!a) return RR_INTRUSIVE_PTR<MessageElementPodMultiDimArray>();

	std::string type = a->RRElementTypeString();

	std::string servicetype = SplitQualifiedName(type).get<0>();
	//std::string structuretype=res[1];

	RR_SHARED_PTR<ServiceFactory> factory = GetServiceType(servicetype);

	return factory->PackPodMultiDimArray(a);
}

RR_INTRUSIVE_PTR<RRPodBaseMultiDimArray> RobotRaconteurNode::UnpackPodMultiDimArray(RR_INTRUSIVE_PTR<MessageElementPodMultiDimArray> a)
{
	if (!a) return RR_INTRUSIVE_PTR<RRPodBaseMultiDimArray>();

	std::string type = a->Type;


	std::string servicetype = SplitQualifiedName(type).get<0>();
	//std::string structuretype=res[1];

	RR_SHARED_PTR<ServiceFactory> factory = GetServiceType(servicetype);

	return rr_cast<RRPodBaseMultiDimArray>(factory->UnpackPodMultiDimArray(a));
}


RR_INTRUSIVE_PTR<MessageElementNamedArray> RobotRaconteurNode::PackNamedArray(RR_INTRUSIVE_PTR<RRNamedBaseArray> a)
{

	if (!a) return RR_INTRUSIVE_PTR<MessageElementNamedArray>();

	std::string type = a->RRElementTypeString();

	std::string servicetype = SplitQualifiedName(type).get<0>();
	//std::string structuretype=res[1];

	RR_SHARED_PTR<ServiceFactory> factory = GetServiceType(servicetype);

	return factory->PackNamedArray(a);
}

RR_INTRUSIVE_PTR<RRNamedBaseArray> RobotRaconteurNode::UnpackNamedArray(RR_INTRUSIVE_PTR<MessageElementNamedArray> a)
{
	if (!a) return RR_INTRUSIVE_PTR<RRNamedBaseArray>();

	std::string type = a->Type;


	std::string servicetype = SplitQualifiedName(type).get<0>();
	//std::string structuretype=res[1];

	RR_SHARED_PTR<ServiceFactory> factory = GetServiceType(servicetype);

	return rr_cast<RRNamedBaseArray>(factory->UnpackNamedArray(a));
}

RR_INTRUSIVE_PTR<MessageElementNamedMultiDimArray> RobotRaconteurNode::PackNamedMultiDimArray(RR_INTRUSIVE_PTR<RRNamedBaseMultiDimArray> a)
{

	if (!a) return RR_INTRUSIVE_PTR<MessageElementNamedMultiDimArray>();

	std::string type = a->RRElementTypeString();

	std::string servicetype = SplitQualifiedName(type).get<0>();
	//std::string structuretype=res[1];

	RR_SHARED_PTR<ServiceFactory> factory = GetServiceType(servicetype);

	return factory->PackNamedMultiDimArray(a);
}

RR_INTRUSIVE_PTR<RRNamedBaseMultiDimArray> RobotRaconteurNode::UnpackNamedMultiDimArray(RR_INTRUSIVE_PTR<MessageElementNamedMultiDimArray> a)
{
	if (!a) return RR_INTRUSIVE_PTR<RRNamedBaseMultiDimArray>();

	std::string type = a->Type;


	std::string servicetype = SplitQualifiedName(type).get<0>();
	//std::string structuretype=res[1];

	RR_SHARED_PTR<ServiceFactory> factory = GetServiceType(servicetype);

	return rr_cast<RRNamedBaseMultiDimArray>(factory->UnpackNamedMultiDimArray(a));
}

RR_INTRUSIVE_PTR<MessageElementData> RobotRaconteurNode::PackVarType(RR_INTRUSIVE_PTR<RRValue> vardata)
{

	if (!vardata) return RR_INTRUSIVE_PTR<MessageElementData>();

	std::string type=vardata->RRType();

	std::string t1="RobotRaconteur.RRArray";
	if (type.compare(0,t1.length(),t1)==0)
	{
		return rr_cast<MessageElementData>(vardata);
	}

	std::string t2="RobotRaconteur.RRMap<int32_t>";
	if (type==t2)
	{
		return PackMapType<int32_t,RRValue>(vardata);
	}

	std::string t3="RobotRaconteur.RRMap<string>";
	if (type==t3)
	{
		return PackMapType<std::string,RRValue>(vardata);
	}
	
	std::string t6="RobotRaconteur.RRMap";
	if (type.compare(0,t6.size(),t6)==0)
	{
		//Unknown keytype type for map
		throw DataTypeException("Invalid map keytype");	
	}	

	std::string t5="RobotRaconteur.RRMultiDimArray";
	if (type.compare(0,t5.length(),t5)==0)
	{

		if (type=="RobotRaconteur.RRMultiDimArray<double>")
			return PackMultiDimArray(rr_cast<RRMultiDimArray<double> >(vardata));
		if (type=="RobotRaconteur.RRMultiDimArray<single>")
			return PackMultiDimArray(rr_cast<RRMultiDimArray<float> >(vardata));
		if (type=="RobotRaconteur.RRMultiDimArray<int8>")
			return PackMultiDimArray(rr_cast<RRMultiDimArray<int8_t> >(vardata));
		if (type=="RobotRaconteur.RRMultiDimArray<uint8>")
			return PackMultiDimArray(rr_cast<RRMultiDimArray<uint8_t> >(vardata));
		if (type=="RobotRaconteur.RRMultiDimArray<int16>")
			return PackMultiDimArray(rr_cast<RRMultiDimArray<int16_t> >(vardata));
		if (type=="RobotRaconteur.RRMultiDimArray<uint16>")
			return PackMultiDimArray(rr_cast<RRMultiDimArray<uint16_t> >(vardata));
		if (type=="RobotRaconteur.RRMultiDimArray<int32>")
			return PackMultiDimArray(rr_cast<RRMultiDimArray<int32_t> >(vardata));
		if (type=="RobotRaconteur.RRMultiDimArray<uint32>")
			return PackMultiDimArray(rr_cast<RRMultiDimArray<uint32_t> >(vardata));
		if (type=="RobotRaconteur.RRMultiDimArray<int64>")
			return PackMultiDimArray(rr_cast<RRMultiDimArray<int64_t> >(vardata));
		if (type=="RobotRaconteur.RRMultiDimArray<uint64>")
			return PackMultiDimArray(rr_cast<RRMultiDimArray<uint64_t> >(vardata));
		if (type == "RobotRaconteur.RRMultiDimArray<cdouble>")
			return PackMultiDimArray(rr_cast<RRMultiDimArray<cdouble> >(vardata));
		if (type == "RobotRaconteur.RRMultiDimArray<csingle>")
			return PackMultiDimArray(rr_cast<RRMultiDimArray<cfloat> >(vardata));
		if (type == "RobotRaconteur.RRMultiDimArray<bool>")
			return PackMultiDimArray(rr_cast<RRMultiDimArray<rr_bool> >(vardata));
		throw DataTypeException("Invalid MultiDimArray type");
	}

	std::string t8="RobotRaconteur.RRList";
	if (type==t8)
	{
		return PackListType<RRValue>(vardata);
	}

	std::string t9 = "RobotRaconteur.RRPodArray";
	if (type == t9)
	{
		return PackPodArray(rr_cast<RRPodBaseArray>(vardata));
	}

	std::string t11 = "RobotRaconteur.RRPodMultiDimArray";
	if (type == t11)
	{
		return PackPodMultiDimArray(rr_cast<RRPodBaseMultiDimArray>(vardata));
	}

	std::string t12 = "RobotRaconteur.RRNamedArray";
	if (type == t12)
	{
		return PackNamedArray(rr_cast<RRNamedBaseArray>(vardata));
	}

	std::string t13 = "RobotRaconteur.RRNamedMultiDimArray";
	if (type == t13)
	{
		return PackNamedMultiDimArray(rr_cast<RRNamedBaseMultiDimArray>(vardata));
	}

	return PackStructure(rr_cast<RRStructure>(vardata));
}

RR_INTRUSIVE_PTR<RRValue> RobotRaconteurNode::UnpackVarType(RR_INTRUSIVE_PTR<MessageElement> mvardata1)
{
	if (!mvardata1) return RR_INTRUSIVE_PTR<RRValue>();
	if (mvardata1->ElementType==DataTypes_void_t) return RR_INTRUSIVE_PTR<RRValue>();

	RR_INTRUSIVE_PTR<MessageElementData> mvardata=mvardata1->GetData();

	DataTypes type=mvardata->GetTypeID();

	if (IsTypeRRArray(type))
	{
		return rr_cast<RRValue>(mvardata);
	}

	if (type==DataTypes_structure_t)
	{
		return UnpackStructure(rr_cast<MessageElementStructure>(mvardata));
	}

	if (type==DataTypes_vector_t)
	{
		return UnpackMapType<int32_t,RRValue>(rr_cast<MessageElementMap<int32_t> >(mvardata));
	}

	if (type==DataTypes_dictionary_t)
	{
		return UnpackMapType<std::string,RRValue>(rr_cast<MessageElementMap<std::string> >(mvardata));
	}

	if (type==DataTypes_multidimarray_t)
	{
		DataTypes type1=MessageElement::FindElement(mvardata1->CastData<MessageElementMultiDimArray>()->Elements,"array")->ElementType;

		switch (type1)
		{	
		
		case DataTypes_double_t:
			return rr_cast<RRValue>(UnpackMultiDimArray<double>(rr_cast<MessageElementMultiDimArray>(mvardata)));
		case DataTypes_single_t:
			return rr_cast<RRValue>(UnpackMultiDimArray<float>(rr_cast<MessageElementMultiDimArray>(mvardata)));
		case DataTypes_int8_t:
			return rr_cast<RRValue>(UnpackMultiDimArray<int8_t>(rr_cast<MessageElementMultiDimArray>(mvardata)));
		case DataTypes_uint8_t:
			return rr_cast<RRValue>(UnpackMultiDimArray<uint8_t>(rr_cast<MessageElementMultiDimArray>(mvardata)));
		case DataTypes_int16_t:
			return rr_cast<RRValue>(UnpackMultiDimArray<int16_t>(rr_cast<MessageElementMultiDimArray>(mvardata)));
		case DataTypes_uint16_t:
			return rr_cast<RRValue>(UnpackMultiDimArray<uint16_t>(rr_cast<MessageElementMultiDimArray>(mvardata)));
		case DataTypes_int32_t:
			return rr_cast<RRValue>(UnpackMultiDimArray<int32_t>(rr_cast<MessageElementMultiDimArray>(mvardata)));
		case DataTypes_uint32_t:
			return rr_cast<RRValue>(UnpackMultiDimArray<uint32_t>(rr_cast<MessageElementMultiDimArray>(mvardata)));
		case DataTypes_int64_t:
			return rr_cast<RRValue>(UnpackMultiDimArray<int64_t>(rr_cast<MessageElementMultiDimArray>(mvardata)));
		case DataTypes_uint64_t:
			return rr_cast<RRValue>(UnpackMultiDimArray<uint64_t>(rr_cast<MessageElementMultiDimArray>(mvardata)));
		case DataTypes_cdouble_t:
			return rr_cast<RRValue>(UnpackMultiDimArray<cdouble>(rr_cast<MessageElementMultiDimArray>(mvardata)));
		case DataTypes_csingle_t:
			return rr_cast<RRValue>(UnpackMultiDimArray<cfloat>(rr_cast<MessageElementMultiDimArray>(mvardata)));
		case DataTypes_bool_t:
			return rr_cast<RRValue>(UnpackMultiDimArray<rr_bool>(rr_cast<MessageElementMultiDimArray>(mvardata)));
		default:
			throw DataTypeException("Invalid data type");
			
		}


		throw DataTypeException("Invalid MultiDimArray type");
	}

	if (type==DataTypes_list_t)
	{
		return UnpackListType<RRValue>(rr_cast<MessageElementList >(mvardata));
	}
		
	if (type == DataTypes_pod_array_t)
	{
		return UnpackPodArray(rr_cast<MessageElementPodArray>(mvardata));
	}

	if (type == DataTypes_pod_multidimarray_t)
	{
		return UnpackPodMultiDimArray(rr_cast<MessageElementPodMultiDimArray>(mvardata));
	}

	if (type == DataTypes_namedarray_array_t)
	{
		return UnpackNamedArray(rr_cast<MessageElementNamedArray>(mvardata));
	}

	if (type == DataTypes_namedarray_multidimarray_t)
	{
		return UnpackNamedMultiDimArray(rr_cast<MessageElementNamedMultiDimArray>(mvardata));
	}

	throw DataTypeException("Unknown data type");
}

void RobotRaconteurNode::Shutdown()
{
	{
		if (!instance_is_init) return;
		boost::mutex::scoped_lock lock2(shutdown_lock);
		if (is_shutdown) return;
		{
			boost::unique_lock<boost::shared_mutex> lock(thread_pool_lock);
			is_shutdown = true;
		}



		{
			boost::unique_lock<boost::shared_mutex> lock(services_lock);

			BOOST_FOREACH (RR_SHARED_PTR<ServerContext>& e, services | boost::adaptors::map_values)
			{
				try
				{
					e->Close();
				}
				catch (std::exception&) {}
			}
	
			services.clear();
		}

		std::vector<RR_SHARED_PTR<Endpoint> > endpointsv;
		{
			boost::mutex::scoped_lock lock(endpoint_lock);
			boost::copy(endpoints | boost::adaptors::map_values, std::back_inserter(endpointsv));
		}
	
		BOOST_FOREACH (RR_SHARED_PTR<Endpoint>& e, endpointsv)
		{
			try
			{
				RR_SHARED_PTR<ClientContext> e1=RR_DYNAMIC_POINTER_CAST<ClientContext>(e);
				if (e1)
					e1->AsyncClose(boost::bind(&RobotRaconteurNode_empty_handler));
			}
			catch (std::exception&) {}
		}
	

		{
			boost::mutex::scoped_lock lock(endpoint_lock);
			endpoints.clear();
		}

			//std::cout << "start transport close" << std::endl;

		{
			if (m_Discovery)
			{
				m_Discovery->Shutdown();
			}

		}

		{
			boost::mutex::scoped_lock lock(transports_lock);
			BOOST_FOREACH(RR_SHARED_PTR<Transport>& e, transports | boost::adaptors::map_values)
			{
				try
				{
					e->Close();
				}
				catch (std::exception&) {}
			}
	
			transports.clear();

		}

		{
			boost::mutex::scoped_lock lock(cleanupobjs_lock);

			cleanupobjs.clear();
		}
	}

	shutdown_listeners();

	{
		boost::unique_lock<boost::shared_mutex> lock(thread_pool_lock);
		if (this->PeriodicCleanupTask_timer)
		{
			try
			{
			this->PeriodicCleanupTask_timer->Stop();
			}
			catch (std::exception&) {}
			this->PeriodicCleanupTask_timer->Clear();
			this->PeriodicCleanupTask_timer.reset();
		}
	}
	
		
	{
		RR_SHARED_PTR<ThreadPool> thread_pool1;

		{
			boost::unique_lock<boost::shared_mutex> lock(thread_pool_lock);
			thread_pool1 = thread_pool;
		}
		if (thread_pool1)
		{
			thread_pool1->Shutdown();
		}
	}

	{
		boost::mutex::scoped_lock lock(exception_handler_lock);
		exception_handler.clear();
	}

	discovery_updated_listeners.disconnect_all_slots();
	discovery_lost_listeners.disconnect_all_slots();
	
}


void RobotRaconteurNode::ReleaseThreadPool()
{
	boost::mutex::scoped_lock lock2(shutdown_lock);

	if (!is_shutdown)
	{
		throw InvalidOperationException("Node must be shut down to release thread pool");
	}

	{
		boost::unique_lock<boost::shared_mutex> lock(thread_pool_lock);
		thread_pool.reset();
	}
}

RobotRaconteurNode::~RobotRaconteurNode()
{
	Shutdown();
	
	
}


void RobotRaconteurNode::SendMessage(RR_INTRUSIVE_PTR<Message> m)
{

	if (m->header->SenderNodeID != NodeID())
	{	
			throw ConnectionException("Could not route message");		
	}
		
	RR_SHARED_PTR<Endpoint> e;		
	{
		boost::mutex::scoped_lock lock (endpoint_lock);
		RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<Endpoint> >::iterator e1 = endpoints.find(m->header->SenderEndpoint);
		if (e1==endpoints.end()) throw InvalidEndpointException("Could not find endpoint");
		e = e1->second;
	}

	RR_SHARED_PTR<Transport> c;			
	{
		boost::shared_lock<boost::shared_mutex> lock (transport_lock);
		RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<Transport> >::iterator e1 = transports.find(e->GetTransport());
		if (e1==transports.end()) throw ConnectionException("Could not find transport");
		c = e1->second;
	}	

	c->SendMessage(m);

}

void RobotRaconteurNode::AsyncSendMessage(RR_INTRUSIVE_PTR<Message> m, boost::function<void (RR_SHARED_PTR<RobotRaconteurException> )>& callback)
{
	if (m->header->SenderNodeID != NodeID())
	{

		
			throw ConnectionException("Could not route message");
		
	}

	RR_SHARED_PTR<Endpoint> e;
	{
		boost::mutex::scoped_lock lock(endpoint_lock);
		RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<Endpoint> >::iterator e1 = endpoints.find(m->header->SenderEndpoint);
		if (e1 == endpoints.end()) throw InvalidEndpointException("Could not find endpoint");
		e = e1->second;
	}

	RR_SHARED_PTR<Transport> c;
	{
		boost::shared_lock<boost::shared_mutex> lock(transport_lock);
		RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<Transport> >::iterator e1 = transports.find(e->GetTransport());
		if (e1 == transports.end()) throw ConnectionException("Could not find transport");
		c = e1->second;
	}

	c->AsyncSendMessage(m,callback);

}

void RobotRaconteurNode::MessageReceived(RR_INTRUSIVE_PTR<Message> m)
{
	try
		{
		if (m->header->ReceiverNodeID != NodeID())
		{

				RR_INTRUSIVE_PTR<Message> eret = GenerateErrorReturnMessage(m, MessageErrorType_NodeNotFound, "RobotRaconteur.NodeNotFound", "Could not find route to remote node");
				if (eret->entries.size() > 0)
					SendMessage(eret);
		}

		else
		{
			
			RR_SHARED_PTR<Endpoint> e;
				
			{
				boost::mutex::scoped_lock lock (endpoint_lock);
				RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<Endpoint> >::iterator e1 = endpoints.find(m->header->ReceiverEndpoint);
				if (e1 != endpoints.end())
				{
					e = e1->second;
				}
			}

			if (e)
			{
				e->MessageReceived(m);
			}
			else
			{			
				
			RR_INTRUSIVE_PTR<Message> eret = GenerateErrorReturnMessage(m, MessageErrorType_InvalidEndpoint, "RobotRaconteur.InvalidEndpoint", "Invalid destination endpoint");
			if (eret->entries.size() > 0)
				SendMessage(eret);				
			}
		}
	}
	catch (std::exception& e)
	{
		HandleException(&e);
	}
}

void RobotRaconteurNode::TransportConnectionClosed(uint32_t endpoint)
{
	RR_SHARED_PTR<Endpoint> e;
	{
		boost::mutex::scoped_lock lock(endpoint_lock);
		RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<Endpoint> >::iterator e1 = endpoints.find(endpoint);
		if (e1 != endpoints.end())
		{
			e = e1->second;
		}
		else
		{
			return;
		}
	}

	e->TransportConnectionClosed(endpoint);

}

uint32_t RobotRaconteurNode::GetRequestTimeout()
{
	boost::mutex::scoped_lock lock(RequestTimeout_lock);
	return RequestTimeout;
}
void RobotRaconteurNode::SetRequestTimeout(uint32_t timeout)
{
	boost::mutex::scoped_lock lock(RequestTimeout_lock);
	RequestTimeout=timeout;
}

uint32_t RobotRaconteurNode::GetTransportInactivityTimeout()
{
	boost::mutex::scoped_lock lock(TransportInactivityTimeout_lock);
	return TransportInactivityTimeout;
}
void RobotRaconteurNode::SetTransportInactivityTimeout(uint32_t timeout)
{
	boost::mutex::scoped_lock lock(TransportInactivityTimeout_lock);
	TransportInactivityTimeout=timeout;
}

uint32_t RobotRaconteurNode::GetEndpointInactivityTimeout()
{
	boost::mutex::scoped_lock lock(EndpointInactivityTimeout_lock);
	return EndpointInactivityTimeout;
}

void RobotRaconteurNode::SetEndpointInactivityTimeout(uint32_t timeout)
{
	boost::mutex::scoped_lock lock(EndpointInactivityTimeout_lock);
	EndpointInactivityTimeout=timeout;
}

uint32_t RobotRaconteurNode::GetMemoryMaxTransferSize()
{
	boost::mutex::scoped_lock lock(MemoryMaxTransferSize_lock);
	return MemoryMaxTransferSize;
}

void RobotRaconteurNode::SetMemoryMaxTransferSize(uint32_t size)
{
	boost::mutex::scoped_lock lock(MemoryMaxTransferSize_lock);
	MemoryMaxTransferSize=size;
}


const RR_SHARED_PTR<RobotRaconteur::DynamicServiceFactory> RobotRaconteurNode::GetDynamicServiceFactory() 
{
	boost::mutex::scoped_lock lock(dynamic_factory_lock);
	return dynamic_factory;
}

void RobotRaconteurNode::SetDynamicServiceFactory(RR_SHARED_PTR<RobotRaconteur::DynamicServiceFactory> f)
{
	boost::mutex::scoped_lock lock(dynamic_factory_lock);

	if (this->dynamic_factory != 0)
		throw InvalidOperationException("Dynamic service factory already set");
	this->dynamic_factory = f;
}

RR_INTRUSIVE_PTR<Message> RobotRaconteurNode::GenerateErrorReturnMessage(RR_INTRUSIVE_PTR<Message> m, MessageErrorType err, const std::string &errname, const std::string &errdesc)
{
	RR_INTRUSIVE_PTR<Message> ret = CreateMessage();
	ret->header = CreateMessageHeader();
	ret->header->ReceiverNodeName = m->header->SenderNodeName;
	ret->header->SenderNodeName = m->header->ReceiverNodeName;
	ret->header->ReceiverNodeID = m->header->SenderNodeID;
	ret->header->ReceiverEndpoint = m->header->SenderEndpoint;
	ret->header->SenderEndpoint = m->header->ReceiverEndpoint;
	ret->header->SenderNodeID = m->header->ReceiverNodeID;
	BOOST_FOREACH (RR_INTRUSIVE_PTR<MessageEntry>& me, m->entries)
	{
		if ((static_cast<int32_t>(me->EntryType)) % 2 == 1)
		{
			RR_INTRUSIVE_PTR<MessageEntry> eret = CreateMessageEntry((MessageEntryType)(me->EntryType+1), me->MemberName);
			eret->RequestID = me->RequestID;
			eret->ServicePath = me->ServicePath;
			eret->AddElement("errorname", stringToRRArray(errname));
			eret->AddElement("errorstring", stringToRRArray(errdesc));
			eret->Error = err;
			ret->entries.push_back(eret);
		}
	}
	return ret;
}

RR_SHARED_PTR<ServerContext> RobotRaconteurNode::RegisterService(const std::string &name, const std::string &servicetype,  RR_SHARED_PTR<RRObject> obj, RR_SHARED_PTR<ServiceSecurityPolicy> securitypolicy)
{
		
	if(!boost::regex_match(name,boost::regex("^[a-zA-Z][a-zA-Z0-9_]*$")))
	{
		throw InvalidArgumentException("\"" + name + "\" is an invalid NodeName");
	}

	RR_SHARED_PTR<ServerContext> c;
	
	{
		boost::unique_lock<boost::shared_mutex> lock(services_lock);

		if (services.count(name)!=0)
		{
			throw InvalidOperationException("Service name in use");
		}

		c = RR_MAKE_SHARED<ServerContext>(GetServiceType(servicetype),shared_from_this());
		c->SetBaseObject(name, obj, securitypolicy);

		//RegisterEndpoint(c);
		services.insert(make_pair(name, c));		
	}

	UpdateServiceStateNonce();

	return c;
}

RR_SHARED_PTR<ServerContext> RobotRaconteurNode::RegisterService(RR_SHARED_PTR<ServerContext> c)
{
		
	if(!boost::regex_match(c->GetServiceName(),boost::regex("^[a-zA-Z][a-zA-Z0-9_]*$")))
	{
		throw InvalidArgumentException("\"" + c->GetServiceName() + "\" is an invalid NodeName");
	}
		
	{
		boost::unique_lock<boost::shared_mutex> lock(services_lock);
		if (services.count(c->GetServiceName())!=0)
		{
			CloseService(c->GetServiceName());
		}

		services.insert(std::make_pair(c->GetServiceName(), c));		
	}
		
	UpdateServiceStateNonce();

	return c;
}

void RobotRaconteurNode::CloseService(const std::string &sname)
{
		
	RR_SHARED_PTR<ServerContext> s;
	{
		boost::unique_lock<boost::shared_mutex> lock(services_lock);
		RR_UNORDERED_MAP<std::string, RR_SHARED_PTR<ServerContext> >::iterator e1 = services.find(sname);
		if (e1 == services.end()) throw ServiceException("Service not found");
		s = e1->second;
		s->Close();
		//DeleteEndpoint(s);
		services.erase(sname);
	}
}


std::vector<std::string> RobotRaconteurNode::GetRegisteredServiceNames()
{
	boost::shared_lock<boost::shared_mutex> lock(services_lock);
	std::vector<std::string> o;	
	BOOST_FOREACH (RR_SHARED_PTR<ServerContext>& e, services | boost::adaptors::map_values)
	{
		o.push_back(e->GetServiceName());
	}

	return o;
}


RR_SHARED_PTR<ServerContext> RobotRaconteurNode::GetService(const std::string &name)
{
			
	boost::shared_lock<boost::shared_mutex> lock(services_lock);
	RR_UNORDERED_MAP<std::string, RR_SHARED_PTR<ServerContext> >::iterator e1 = services.find(name);
	if (e1 != services.end())
	{
		return e1->second;
	}
		
	throw ServiceNotFoundException("Service " + name + " not found");	
}


RR_INTRUSIVE_PTR<Message> RobotRaconteurNode::SpecialRequest(RR_INTRUSIVE_PTR<Message> m, uint32_t transportid, RR_SHARED_PTR<ITransportConnection> tc)
{


	if (!(m->header->ReceiverNodeID == NodeID().GetAny() && (m->header->ReceiverNodeName == "" || m->header->ReceiverNodeName == NodeName())) && !(m->header->ReceiverNodeID == NodeID()))
	{		
		return GenerateErrorReturnMessage(m, MessageErrorType_NodeNotFound, "RobotRaconteur.NodeNotFound", "Could not find route to remote node");
	}


	if (m->header->ReceiverEndpoint != 0 && m->entries.size() == 1 && m->entries.at(0)->EntryType == MessageEntryType_ObjectTypeName)
	{
		//Workaround for security of getting object types
		MessageReceived(m);
		return RR_INTRUSIVE_PTR<Message>();

	}

	RR_INTRUSIVE_PTR<Message> ret = CreateMessage();
	ret->header = CreateMessageHeader();
	ret->header->ReceiverNodeName = m->header->SenderNodeName;
	ret->header->SenderNodeName = this->NodeName();
	ret->header->ReceiverNodeID = m->header->SenderNodeID;
	ret->header->ReceiverEndpoint = m->header->SenderEndpoint;
	ret->header->SenderEndpoint = m->header->ReceiverEndpoint;
	ret->header->SenderNodeID = this->NodeID();

	BOOST_FOREACH (RR_INTRUSIVE_PTR<MessageEntry>& e, m->entries)
	{
		RR_INTRUSIVE_PTR<MessageEntry> eret = ret->AddEntry((MessageEntryType)(((uint16_t)e->EntryType)+1),e->MemberName);
		eret->RequestID = e->RequestID;
		eret->ServicePath = e->ServicePath;

		switch (e->EntryType)
		{
			case MessageEntryType_GetNodeInfo:
				break;
			case MessageEntryType_ObjectTypeName:
			{
					std::string path = static_cast<std::string>(e->ServicePath);
					
					std::vector<std::string> s1;
					boost::split(s1,path,boost::is_from_range('.','.'));
					
					try
					{						
						RR_SHARED_PTR<ServerContext> s;

						s = GetService(s1.at(0));

						RobotRaconteurVersion v;
						RR_INTRUSIVE_PTR<MessageElement> m_ver;
						if (e->TryFindElement("clientversion",m_ver))
						{
							v.FromString(m_ver->CastDataToString());
						}

						std::string objtype = s->GetObjectType(path,v);
						eret->AddElement("objecttype", stringToRRArray(objtype));
						
					}
					catch (std::exception&)
					{
						eret->AddElement("errorname", stringToRRArray("RobotRaconteur.ObjectNotFoundException"));
						eret->AddElement("errorstring", stringToRRArray("Object not found"));
						eret->Error = MessageErrorType_ObjectNotFound;
					}
			}
				break;
			case MessageEntryType_GetServiceDesc:
			{
					//string name = (string)e.FindElement("servicename").Data;
					std::string name = e->ServicePath;
					try
					{
						std::string servicedef = "";

						bool is_service_type=false;
						bool is_service_type2 = false;

						RR_INTRUSIVE_PTR<MessageElement> el1;
						RR_INTRUSIVE_PTR<MessageElement> el2;
						is_service_type=e->TryFindElement("ServiceType", el1);
						is_service_type2=e->TryFindElement("servicetype", el2);

						if (is_service_type)
						{
							name = el1->CastDataToString();
							servicedef = GetServiceType(name)->DefString();
						}
						else if (is_service_type2)
						{
							name = el2->CastDataToString();
							servicedef = GetServiceType(name)->DefString();
						}
						else
						{
							RobotRaconteurVersion v;
							RR_INTRUSIVE_PTR<MessageElement> m_ver;
							if (e->TryFindElement("clientversion", m_ver))
							{
								v.FromString(m_ver->CastDataToString());
							}

							servicedef = GetService(name)->GetRootObjectServiceDef(v)->DefString();
							RR_INTRUSIVE_PTR<RRMap<std::string,RRValue> > attr=AllocateEmptyRRMap<std::string,RRValue>();
							attr->GetStorageContainer()=GetService(name)->GetAttributes();
							eret->AddElement("attributes", PackMapType<std::string, RRValue>(attr));
						}
						eret->AddElement("servicedef", stringToRRArray(servicedef));
					}
					catch (std::exception&)
					{
						eret->AddElement("errorname", stringToRRArray("RobotRaconteur.ServiceNotFoundException"));
						eret->AddElement("errorstring", stringToRRArray("Service not found"));
						eret->Error = MessageErrorType_ServiceNotFound;
					}
			}
				break;
			case MessageEntryType_ConnectClient:
			{
					std::string name = static_cast<std::string>(e->ServicePath);

					try
					{
						RR_SHARED_PTR<ServerEndpoint> se = RR_MAKE_SHARED<ServerEndpoint>(shared_from_this());

						RR_SHARED_PTR<ServerContext> c = GetService(name);
						se->service = c;

						se->SetRemoteEndpoint ( m->header->SenderEndpoint);
						se->SetRemoteNodeID ( m->header->SenderNodeID);
						RegisterEndpoint(se);

						se->SetTransport( transportid);
						se->SetTransportConnection(tc);

						c->AddClient(se);

						ret->header->SenderEndpoint = se->GetLocalEndpoint();
						
					}
					catch (std::exception&)
					{
						eret->AddElement("errorname", stringToRRArray("RobotRaconteur.ServiceNotFoundException"));
						eret->AddElement("errorstring", stringToRRArray("Service not found"));
						eret->Error = MessageErrorType_ServiceNotFound;
					}
			}
				break;
			case MessageEntryType_DisconnectClient:
			{


					try
					{
						std::string name = e->FindElement("servicename")->CastDataToString();
						RR_SHARED_PTR<ServerEndpoint> se;
						
						{
							boost::mutex::scoped_lock lock(endpoint_lock);
							se = RR_DYNAMIC_POINTER_CAST<ServerEndpoint>(endpoints.at(m->header->ReceiverEndpoint));
							if (se==0) throw InternalErrorException("");
						}

						
						{
							//boost::mutex::scoped_lock lock(services_lock);
							try
							{
								GetService(name)->RemoveClient(se);
							}
							catch (std::out_of_range&)
							{
								throw ServiceNotFoundException("Service " + name + " not found");
							}
						}


						try
						{
							RobotRaconteurNode::DeleteEndpoint(se);
						}
						catch (std::exception&)
						{
						}
						//eret.AddElement("servicedef", servicedef);
					}
					catch (std::exception&)
					{
						eret->AddElement("errorname", stringToRRArray("RobotRaconteur.ServiceNotFoundException"));
						eret->AddElement("errorstring", stringToRRArray("Service not found"));
						eret->Error = MessageErrorType_ServiceNotFound;
					}
			}
				break;
			case MessageEntryType_ConnectionTest:
				break;

			case MessageEntryType_NodeCheckCapability:
				eret->AddElement("return", ScalarToRRArray(static_cast<uint32_t>(0)));
				break;
			case MessageEntryType_GetServiceAttributes:
			{
					std::string path = static_cast<std::string>(e->ServicePath);
					
					std::vector<std::string> s1;
					boost::split(s1,path,boost::is_from_range('.','.'));
					try
					{
						RR_SHARED_PTR<ServerContext> s;

						s = GetService(s1.at(0));

						RR_INTRUSIVE_PTR<RRMap<std::string, RRValue> > attr=AllocateEmptyRRMap<std::string, RRValue>();
						attr->GetStorageContainer()= (s->GetAttributes());
						eret->AddElement("return", PackMapType<std::string, RRValue>(attr));
					}
					catch (std::exception&)
					{
						eret->AddElement("errorname", stringToRRArray("RobotRaconteur.ServiceError"));
						eret->AddElement("errorstring", stringToRRArray("Service not found"));
						eret->Error = MessageErrorType_ServiceError;
					}
			}
				break;


			case MessageEntryType_ConnectClientCombined:
			{
				std::string name = static_cast<std::string>(e->ServicePath);

				RR_SHARED_PTR<ServerContext> c;

				RobotRaconteurVersion v;
				RR_INTRUSIVE_PTR<MessageElement> m_ver;
				if (e->TryFindElement("clientversion", m_ver))
				{
					v.FromString(m_ver->CastDataToString());
				}

				try
				{
					c = GetService(name);
					std::string objtype= c->GetRootObjectType(v);
					eret->AddElement("objecttype", stringToRRArray(objtype));
				}
				catch (std::exception&)
				{
					eret->elements.clear();
					eret->AddElement("errorname", stringToRRArray("RobotRaconteur.ServiceNotFoundException"));
					eret->AddElement("errorstring", stringToRRArray("Service not found"));
					eret->Error = MessageErrorType_ServiceNotFound;
					break;
				}

				try
				{
					bool returnservicedef = true;

					try
					{
						RR_INTRUSIVE_PTR<MessageElement> returnservicedefs_el;
						if (e->TryFindElement("returnservicedefs", returnservicedefs_el))
						{
							std::string returnservicedef_str = returnservicedefs_el->CastDataToString();
							boost::trim(returnservicedef_str);
							if (boost::to_lower_copy(returnservicedef_str) == "false" || returnservicedef_str == "0")
							{
								returnservicedef = false;
							}
						}
					}
					catch (std::exception&) {}


					if (returnservicedef)
					{
						RR_SHARED_PTR<ServiceFactory> servicedef1 = c->GetRootObjectServiceDef(v);
						std::map<std::string, RR_SHARED_PTR<ServiceFactory> > defs;
						defs.insert(std::make_pair(servicedef1->GetServiceName(), servicedef1));

						while (true)
						{
							bool new_found = false;
							std::vector<std::string> v1;
							boost::range::copy(defs | boost::adaptors::map_keys, std::back_inserter(v1));
							BOOST_FOREACH(std::string& e, v1)
							{
								RR_SHARED_PTR<ServiceFactory> d1 = defs.at(e);
								BOOST_FOREACH(std::string& e2, d1->ServiceDef()->Imports)
								{
									if (defs.find(e2) == defs.end())
									{
										RR_SHARED_PTR<ServiceFactory> d2 = GetServiceType(e2);
										defs.insert(std::make_pair(d2->GetServiceName(), d2));
										new_found = true;
									}
								}
							}

							if (!new_found) break;
						}

						uint32_t n = 0;

						std::vector<RR_INTRUSIVE_PTR<MessageElement> > servicedef_list;
						BOOST_FOREACH(RR_SHARED_PTR<ServiceFactory> d1, defs | boost::adaptors::map_values)
						{
							RR_INTRUSIVE_PTR<MessageElement> e1 = CreateMessageElement();
							e1->ElementFlags |= MessageElementFlags_ELEMENT_NUMBER;
							e1->ElementFlags &= ~MessageElementFlags_ELEMENT_NAME_STR;
							e1->ElementNumber = n;
							e1->SetData(stringToRRArray(d1->DefString()));
							servicedef_list.push_back(e1);
							n++;
						}

						eret->AddElement("servicedefs", CreateMessageElementList(servicedef_list));
					}
				}
				catch (std::exception&)
				{
					eret->elements.clear();
					eret->AddElement("errorname", stringToRRArray("RobotRaconteur.ServiceNotFoundException"));
					eret->AddElement("errorstring", stringToRRArray("Service factory configuration error"));
					eret->Error = MessageErrorType_ServiceNotFound;
					break;
				}

				RR_SHARED_PTR<ServerEndpoint> se;

				try
				{

					se = RR_MAKE_SHARED<ServerEndpoint>(shared_from_this());
										
					RegisterEndpoint(se);

					se->service = c;

					se->SetRemoteEndpoint(m->header->SenderEndpoint);
					se->SetRemoteNodeID(m->header->SenderNodeID);

					se->SetTransport(transportid);
					se->SetTransportConnection(tc);

					c->AddClient(se);

					ret->header->SenderEndpoint = se->GetLocalEndpoint();

				}
				catch (std::exception&)
				{
					if (se)
					{
						try
						{
							DeleteEndpoint(se);
						}
						catch (std::exception&) {}
					}

					eret->elements.clear();
					eret->AddElement("errorname", stringToRRArray("RobotRaconteur.ServiceNotFoundException"));
					eret->AddElement("errorstring", stringToRRArray("Service not found"));
					eret->Error = MessageErrorType_ServiceNotFound;
					break;
				}

				try
				{
					if (c->RequireValidUser())
					{
						RR_INTRUSIVE_PTR<MessageElement> username_el;
						RR_INTRUSIVE_PTR<MessageElement> credentials_el;
						if (!e->TryFindElement("username", username_el))
						{
							throw AuthenticationException("Username not provided");
						}

						if (!e->TryFindElement("credentials", credentials_el))
						{
							throw AuthenticationException("Credentials not provided");
						}

						RR_INTRUSIVE_PTR<RRMap<std::string, RRValue> > credentials = rr_cast<RRMap<std::string,RRValue> >(UnpackMapType<std::string, RRValue>(credentials_el->CastData<MessageElementMap<std::string> >()));
						if (!credentials)
						{
							throw AuthenticationException("Credentials cannot be null");
						}

						std::string username = username_el->CastDataToString();

						se->AuthenticateUser(username, credentials->GetStorageContainer());
					}					
				}
				catch (std::exception&)
				{
					try
					{
						c->RemoveClient(se);
						DeleteEndpoint(se);
					}
					catch (std::exception&)
					{
						
					}

					eret->elements.clear();
					eret->AddElement("errorname", stringToRRArray("RobotRaconteur.AuthenticationError"));
					eret->AddElement("errorstring", stringToRRArray("Authentication Failed"));
					eret->Error = MessageErrorType_AuthenticationError;
					break;
				}
			}
			break;

			default:
				eret->Error = MessageErrorType_ProtocolError;
				eret->AddElement("errorname", stringToRRArray("RobotRaconteur.ProtocolError"));
				eret->AddElement("errorstring", stringToRRArray("Invalid Special Operation"));
				break;
		}
	}

	return ret;
}





void RobotRaconteurNode::AsyncConnectService(const std::string &url, const std::string &username, RR_INTRUSIVE_PTR<RRMap<std::string,RRValue> > credentials, boost::function<void (RR_SHARED_PTR<ClientContext>,ClientServiceListenerEventType,RR_SHARED_PTR<void>)> listener, const std::string& objecttype, boost::function<void(RR_SHARED_PTR<RRObject>,RR_SHARED_PTR<RobotRaconteurException>)> handler, int32_t timeout)
{
	std::vector<std::string> urls;
	urls.push_back(url);
	AsyncConnectService(urls,username,credentials,listener,objecttype,handler,timeout);
}

void RobotRaconteurNode::AsyncConnectService(const std::vector<std::string> &url, const std::string &username, RR_INTRUSIVE_PTR<RRMap<std::string,RRValue> > credentials, boost::function<void (RR_SHARED_PTR<ClientContext>,ClientServiceListenerEventType,RR_SHARED_PTR<void>)> listener, const std::string& objecttype, boost::function<void(RR_SHARED_PTR<RRObject>,RR_SHARED_PTR<RobotRaconteurException>)> handler, int32_t timeout)
{
	
	std::vector<RR_SHARED_PTR<Transport> > atransports;

	std::map<std::string,RR_WEAK_PTR<Transport> > connectors;
	{
		boost::shared_lock<boost::shared_mutex> lock(transport_lock);
		boost::copy(transports | boost::adaptors::map_values, std::back_inserter(atransports));		
	}

	
		BOOST_FOREACH(const std::string& e, url)
		{
			BOOST_FOREACH (RR_SHARED_PTR<Transport> end, atransports)
			{
				if (end == 0)
					continue;
				if (end->IsClient())
				{
					if (end->CanConnectService(e))
					{
						connectors.insert(std::make_pair(e,RR_WEAK_PTR<Transport>(end)));
				
					}
				}
			}
		}

		if (connectors.empty()) throw ConnectionException("Could not find any valid transports for requested connection URLs");

		RR_SHARED_PTR<detail::RobotRaconteurNode_connector> connector=RR_MAKE_SHARED<detail::RobotRaconteurNode_connector>(shared_from_this());
		GetThreadPool()->Post(boost::bind(&detail::RobotRaconteurNode_connector::connect, connector, connectors, username, credentials, listener, objecttype, boost::protect(handler), timeout));
		return;	
}

RR_SHARED_PTR<RRObject> RobotRaconteurNode::ConnectService(const std::vector<std::string>& urls, const std::string &username, RR_INTRUSIVE_PTR<RRMap<std::string,RRValue> > credentials, boost::function<void (RR_SHARED_PTR<ClientContext>,ClientServiceListenerEventType,RR_SHARED_PTR<void>)> listener, const std::string& objecttype)
{
	RR_SHARED_PTR<detail::sync_async_handler<RRObject> > h=RR_MAKE_SHARED<detail::sync_async_handler<RRObject> >(RR_MAKE_SHARED<ConnectionException>("Connection timed out"));
	AsyncConnectService(urls,username,credentials,listener,objecttype,boost::bind(&detail::sync_async_handler<RRObject>::operator(),h,_1,_2),this->GetRequestTimeout()*2);
	return h->end();
}

RR_SHARED_PTR<RRObject> RobotRaconteurNode::ConnectService(const std::string &url, const std::string &username, RR_INTRUSIVE_PTR<RRMap<std::string,RRValue> > credentials, boost::function<void (RR_SHARED_PTR<ClientContext>,ClientServiceListenerEventType,RR_SHARED_PTR<void>)> listener, const std::string& objecttype)
{
	std::vector<std::string> urls;
	urls.push_back(url);
	return ConnectService(urls,username,credentials,listener,objecttype);
}

void RobotRaconteurNode::DisconnectService(RR_SHARED_PTR<RRObject> obj)
{
	if (!obj) return;
	RR_SHARED_PTR<ServiceStub> stub = rr_cast<ServiceStub>(obj);
	
		stub->GetContext()->Close();

}

void RobotRaconteurNode::AsyncDisconnectService(RR_SHARED_PTR<RRObject> obj, boost::function<void()> handler)
{
	if (!obj) return;
	RR_SHARED_PTR<ServiceStub> stub = rr_cast<ServiceStub>(obj);
	
	stub->GetContext()->AsyncClose(RR_MOVE(handler));
}

std::map<std::string, RR_INTRUSIVE_PTR<RRValue> > RobotRaconteurNode::GetServiceAttributes(RR_SHARED_PTR<RRObject> obj)
{
	RR_SHARED_PTR<ServiceStub> stub = rr_cast<ServiceStub>(obj);
	return stub->GetContext()->GetAttributes();
}

uint32_t RobotRaconteurNode::RegisterEndpoint(RR_SHARED_PTR<Endpoint> e)
{
	
	{
		boost::random::uniform_int_distribution<uint32_t> distribution(0,std::numeric_limits<uint32_t>::max());

		boost::mutex::scoped_lock lock(endpoint_lock);
		uint32_t id;
		{
			boost::mutex::scoped_lock lock(random_generator_lock);
		do
		{
			id=distribution(*random_generator);
		}
		while (endpoints.count(id)!=0 || recent_endpoints.count(id)!=0);
		}
		e->SetLocalEndpoint(id);
		endpoints.insert(std::make_pair(id, e));
		return id;
		

	}
}

void RobotRaconteurNode::DeleteEndpoint(RR_SHARED_PTR<Endpoint> e)
{

	try
	{
		
		{			
			boost::mutex::scoped_lock lock(endpoint_lock);
			RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<Endpoint> >::iterator e1 = endpoints.find(e->GetLocalEndpoint());
			if (e1 != endpoints.end())			
			{
				endpoints.erase(e1);
				recent_endpoints.insert(std::make_pair(e->GetLocalEndpoint(),NowUTC()));
			}
		}
	}
	catch (std::exception&)
	{
	}

	try
	{
		RR_SHARED_PTR<Transport> c;		
		{
			boost::shared_lock<boost::shared_mutex> lock(transport_lock);
			RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<Transport> >::iterator e1 = transports.find(e->GetTransport());
			if (e1 != transports.end())
			{
				c = e1->second;
			}			
		}
		if (c) c->CloseTransportConnection(e);
	}
	catch (std::exception&)
	{
	}	
}

void RobotRaconteurNode::CheckConnection(uint32_t endpoint)
{	
	RR_SHARED_PTR<Endpoint> e;
	{
		boost::mutex::scoped_lock lock(endpoint_lock);
		RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<Endpoint> >::iterator e1 = endpoints.find(endpoint);
		if (e1 == endpoints.end()) throw InvalidEndpointException("Invalid Endpoint");
		e = e1->second;
	}

	RR_SHARED_PTR<Transport> c;		
	{
		boost::shared_lock<boost::shared_mutex> lock(transport_lock);
		RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<Transport> >::iterator e1 = transports.find(e->GetTransport());
		if (e1 == transports.end()) throw ConnectionException("Transport connection not found");
		c = e1->second;
	}
	c->CheckConnection(endpoint);
}

std::vector<NodeDiscoveryInfo> RobotRaconteurNode::GetDetectedNodes() 
{
	if (!m_Discovery) throw InvalidOperationException("Node not init");
	return m_Discovery->GetDetectedNodes();
}

void RobotRaconteurNode::NodeDetected(const NodeDiscoveryInfo& info)
{
	if (!m_Discovery) throw InvalidOperationException("Node not init");
	m_Discovery->NodeDetected(info);
}

void RobotRaconteurNode::UpdateDetectedNodes(const std::vector<std::string>& schemes)
{
	if (!m_Discovery) throw InvalidOperationException("Node not init");
	m_Discovery->UpdateDetectedNodes(schemes);
}

void RobotRaconteurNode::AsyncUpdateDetectedNodes(const std::vector<std::string>& schemes, boost::function<void()> handler, int32_t timeout)
{
	if (!m_Discovery) throw InvalidOperationException("Node not init");
	m_Discovery->AsyncUpdateDetectedNodes(schemes, handler, timeout);
}


void RobotRaconteurNode::NodeAnnouncePacketReceived(const std::string& packet)
{
	if (!m_Discovery) throw InvalidOperationException("Node not init");
	m_Discovery->NodeAnnouncePacketReceived(packet);
}

void RobotRaconteurNode::CleanDiscoveredNodes()
{
	if (!m_Discovery) throw InvalidOperationException("Node not init");
	m_Discovery->CleanDiscoveredNodes();
}

uint32_t RobotRaconteurNode::GetNodeDiscoveryMaxCacheCount()
{
	if (!m_Discovery) throw InvalidOperationException("Node not init");
	return m_Discovery->GetNodeDiscoveryMaxCacheCount();
}
void RobotRaconteurNode::SetNodeDiscoveryMaxCacheCount(uint32_t count)
{
	if (!m_Discovery) throw InvalidOperationException("Node not init");
	m_Discovery->SetNodeDiscoveryMaxCacheCount(count);
}

RR_SHARED_PTR<ServiceSubscription> RobotRaconteurNode::SubscribeService(const std::vector<std::string>& service_types, RR_SHARED_PTR<ServiceSubscriptionFilter> filter)
{
	if (!m_Discovery) throw InvalidOperationException("Node not init");
	return m_Discovery->SubscribeService(service_types, filter);
}

RR_SHARED_PTR<ServiceInfo2Subscription> RobotRaconteurNode::SubscribeServiceInfo2(const std::vector<std::string>& service_types, RR_SHARED_PTR<ServiceSubscriptionFilter> filter)
{
	if (!m_Discovery) throw InvalidOperationException("Node not init");
	return m_Discovery->SubscribeServiceInfo2(service_types, filter);
}

void RobotRaconteurNode::FireNodeDetected(RR_SHARED_PTR<NodeDiscoveryInfo> node, RR_SHARED_PTR<std::vector<ServiceInfo2> > services)
{
	discovery_updated_listeners(*node, *services);
}

void RobotRaconteurNode::FireNodeLost(RR_SHARED_PTR<NodeDiscoveryInfo> node)
{
	discovery_lost_listeners(*node);
}

std::string RobotRaconteurNode::SelectRemoteNodeURL(const std::vector<std::string>& urls)
{
	BOOST_FOREACH (const std::string& e, urls)
	{
		if (boost::starts_with(e,"rr+local://")) return e;
	}

	BOOST_FOREACH(const std::string& e, urls)
	{
		if (boost::starts_with(e, "rr+pci://")) return e;
	}

	BOOST_FOREACH(const std::string& e, urls)
	{
		if (boost::starts_with(e,"rr+usb://")) return e;
	}

	BOOST_FOREACH(const std::string& e, urls)
	{
		if (boost::starts_with(boost::to_lower_copy(e), "rrs+tcp://127.0.0.1")) return e;
	}

	BOOST_FOREACH(const std::string& e, urls)
	{
		if (boost::starts_with(boost::to_lower_copy(e), "rrs+tcp://[::1]")) return e;
	}

	BOOST_FOREACH(const std::string& e, urls)
	{
		if (boost::starts_with(boost::to_lower_copy(e), "rrs+tcp://localhost")) return e;
	}

	BOOST_FOREACH(const std::string& e, urls)
	{
		if (boost::starts_with(boost::to_lower_copy(e), "rrs+tcp://[fe80")) return e;
	}

	BOOST_FOREACH(const std::string& e, urls)
	{
		if (boost::starts_with(boost::to_lower_copy(e), "rrs+tcp://")) return e;
	}

	BOOST_FOREACH(const std::string& e, urls)
	{
		if (boost::starts_with(boost::to_lower_copy(e),"rr+tcp://127.0.0.1")) return e;
	}

	BOOST_FOREACH(const std::string& e, urls)
	{
		if (boost::starts_with(boost::to_lower_copy(e),"rr+tcp://[::1]")) return e;
	}

	BOOST_FOREACH(const std::string& e, urls)
	{
		if (boost::starts_with(boost::to_lower_copy(e),"rr+tcp://localhost")) return e;
	}

	BOOST_FOREACH(const std::string& e, urls)
	{
		if (boost::starts_with(boost::to_lower_copy(e),"rr+tcp://[fe80")) return e;
	}

	BOOST_FOREACH(const std::string& e, urls)
	{
		if (boost::starts_with(boost::to_lower_copy(e),"rr+tcp://")) return e;
	}

	return urls.at(0);
}


void RobotRaconteurNode::AsyncFindServiceByType(const std::string &servicetype, const std::vector<std::string>& transportschemes, boost::function<void(RR_SHARED_PTR<std::vector<ServiceInfo2> >) > handler, int32_t timeout)
{
	if (!m_Discovery) throw InvalidOperationException("Node not init");
	m_Discovery->AsyncFindServiceByType(servicetype, transportschemes, handler, timeout);

}

std::vector<ServiceInfo2> RobotRaconteurNode::FindServiceByType(const std::string &servicetype, const std::vector<std::string>& transportschemes)
{
	if (!m_Discovery) throw InvalidOperationException("Node not init");
	return m_Discovery->FindServiceByType(servicetype, transportschemes);
}

std::vector<NodeInfo2> RobotRaconteurNode::FindNodeByID(const RobotRaconteur::NodeID& id, const std::vector<std::string>& transportschemes)
{
	if (!m_Discovery) throw InvalidOperationException("Node not init");
	return m_Discovery->FindNodeByID(id, transportschemes);
}

void RobotRaconteurNode::AsyncFindNodeByID(const RobotRaconteur::NodeID& id, const std::vector<std::string>& transportschemes, boost::function< void(RR_SHARED_PTR<std::vector<NodeInfo2> >) > handler, int32_t timeout)
{
	if (!m_Discovery) throw InvalidOperationException("Node not init");
	m_Discovery->AsyncFindNodeByID(id, transportschemes, handler, timeout);
}

std::vector<NodeInfo2> RobotRaconteurNode::FindNodeByName(const std::string& name, const std::vector<std::string>& transportschemes)
{
	if (!m_Discovery) throw InvalidOperationException("Node not init");
	return m_Discovery->FindNodeByName(name, transportschemes);
}

void RobotRaconteurNode::AsyncFindNodeByName(const std::string& name, const std::vector<std::string>& transportschemes, boost::function< void(RR_SHARED_PTR<std::vector<NodeInfo2> >) > handler, int32_t timeout)
{
	if (!m_Discovery) throw InvalidOperationException("Node not init");
	m_Discovery->AsyncFindNodeByName(name, transportschemes, handler, timeout);
}

std::string RobotRaconteurNode::RequestObjectLock(RR_SHARED_PTR<RRObject> obj, RobotRaconteurObjectLockFlags flags)
{
	if (!(dynamic_cast<ServiceStub*>(obj.get()) != 0))
		throw InvalidArgumentException("Can only lock object opened through Robot Raconteur");
	RR_SHARED_PTR<ServiceStub> s = rr_cast<ServiceStub>(obj);

	return s->GetContext()->RequestObjectLock(obj,flags);


}

std::string RobotRaconteurNode::ReleaseObjectLock(RR_SHARED_PTR<RRObject> obj)
{
	if (!(dynamic_cast<ServiceStub*>(obj.get()) != 0))
		throw InvalidArgumentException("Can only unlock object opened through Robot Raconteur");
	RR_SHARED_PTR<ServiceStub> s = rr_cast<ServiceStub>(obj);

	return s->GetContext()->ReleaseObjectLock(obj);


}

void RobotRaconteurNode::AsyncRequestObjectLock(RR_SHARED_PTR<RRObject> obj, RobotRaconteurObjectLockFlags flags, boost::function<void(RR_SHARED_PTR<std::string>,RR_SHARED_PTR<RobotRaconteurException>)> handler, int32_t timeout)
{		
	RR_SHARED_PTR<ServiceStub> s = RR_DYNAMIC_POINTER_CAST<ServiceStub>(obj);
	if (!s) throw InvalidArgumentException("Can only lock object opened through Robot Raconteur");
	s->GetContext()->AsyncRequestObjectLock(obj,flags,RR_MOVE(handler),timeout);
}

void RobotRaconteurNode::AsyncReleaseObjectLock(RR_SHARED_PTR<RRObject> obj, boost::function<void(RR_SHARED_PTR<std::string>,RR_SHARED_PTR<RobotRaconteurException>)> handler, int32_t timeout)
{		
	RR_SHARED_PTR<ServiceStub> s = RR_DYNAMIC_POINTER_CAST<ServiceStub>(obj);
	if (!s) throw InvalidArgumentException("Can only unlock object opened through Robot Raconteur");
	s->GetContext()->AsyncReleaseObjectLock(obj,RR_MOVE(handler),timeout);
}

void RobotRaconteurNode::MonitorEnter(RR_SHARED_PTR<RRObject> obj, int32_t timeout)
{		
	RR_SHARED_PTR<ServiceStub> s = RR_DYNAMIC_POINTER_CAST<ServiceStub>(obj);
	if (!s) throw InvalidArgumentException("Only service stubs can be monitored by RobotRaconteurNode");
	s->GetContext()->MonitorEnter(obj,timeout);
}

void RobotRaconteurNode::MonitorExit(RR_SHARED_PTR<RRObject> obj)
{		
	RR_SHARED_PTR<ServiceStub> s = RR_DYNAMIC_POINTER_CAST<ServiceStub>(obj);
	if (!s) throw InvalidArgumentException("Only service stubs can be monitored by RobotRaconteurNode");
	s->GetContext()->MonitorExit(obj);
}

RR_SHARED_PTR<RobotRaconteurNode> RobotRaconteurNode::ScopedMonitorLock::GetNode()
{
	RR_SHARED_PTR<RobotRaconteurNode> n=node.lock();
	if (!n) throw InvalidOperationException("Node has been released");
	return n;
}

RobotRaconteurNode::ScopedMonitorLock::ScopedMonitorLock(RR_SHARED_PTR<RRObject> obj, int32_t timeout)
{
	this->obj=obj;
	RR_SHARED_PTR<ServiceStub> s = RR_DYNAMIC_POINTER_CAST<ServiceStub>(obj);
	if (!s) throw InvalidArgumentException("Only service stubs can be monitored by RobotRaconteurNode");
	this->node=s->RRGetNode();
	this->GetNode()->MonitorEnter(obj,timeout);
	locked=true;
}

void RobotRaconteurNode::ScopedMonitorLock::lock(int32_t timeout)
{				
	if (!obj) return;
	if (locked) throw InvalidOperationException("Already locked");
	this->GetNode()->MonitorEnter(obj,timeout);
	locked=true;
}

void RobotRaconteurNode::ScopedMonitorLock::unlock()
{
	if (!obj) return;
	if (!locked) throw InvalidOperationException("Not locked");
	this->GetNode()->MonitorExit(obj);
	locked=false;
}

void RobotRaconteurNode::ScopedMonitorLock::release()
{
	obj.reset();
}

RobotRaconteurNode::ScopedMonitorLock::~ScopedMonitorLock()
{
	if (locked)
	{
		unlock();
	}
}


void RobotRaconteurNode::PeriodicCleanupTask(const TimerEvent& err)
{
	
	if (err.stopped) return;

	{
		boost::posix_time::ptime now=NowUTC();
		
		std::vector<RR_SHARED_PTR<Endpoint> > e;
		
		{
			boost::mutex::scoped_lock lock (endpoint_lock);
			boost::copy(endpoints | boost::adaptors::map_values, std::back_inserter(e));
						
			for(std::map<uint32_t,boost::posix_time::ptime>::iterator e=recent_endpoints.begin(); e!=recent_endpoints.end(); )
			{
				int32_t seconds=(now-e->second).total_seconds();
				if (seconds > 300)
				{
					recent_endpoints.erase(e++);
				}
				else
				{
					e++;
				}
			}

		}

		BOOST_FOREACH (RR_SHARED_PTR<Endpoint>& ee, e)
		{
			try
			{
				ee->PeriodicCleanupTask();
			}
			catch (std::exception&)
			{
			}
		}
		

		std::vector<RR_SHARED_PTR<Transport> > c;		
		{
			boost::shared_lock<boost::shared_mutex> lock (transport_lock);
			boost::copy(transports | boost::adaptors::map_values, std::back_inserter(c));
		}
		
		BOOST_FOREACH (RR_SHARED_PTR<Transport>& cc, c)
		{
			try
			{
				cc->PeriodicCleanupTask();
			}
			catch (std::exception&)
			{
			}
		}
		
		

		std::vector<RR_SHARED_PTR<ServerContext> > sc;		
		{
			boost::shared_lock<boost::shared_mutex> lock (services_lock);
			boost::copy(services | boost::adaptors::map_values, std::back_inserter(sc));						
		}
		
		BOOST_FOREACH (RR_SHARED_PTR<ServerContext>& scc, sc)
		{
			try
			{
				scc->PeriodicCleanupTask();
			}
			catch (std::exception&)
			{
			}
		}
		
		try
		{

			CleanDiscoveredNodes();
		}
		catch (std::exception&)
		{
		}

		BOOST_FOREACH (RR_SHARED_PTR<IPeriodicCleanupTask>& t, cleanupobjs)
		{
			try
			{
				t->PeriodicCleanupTask();
			}
			catch (std::exception&)
			{
			}
		}
	}
	
}

void RobotRaconteurNode::AddPeriodicCleanupTask(RR_SHARED_PTR<IPeriodicCleanupTask> task)
{
	boost::mutex::scoped_lock lock(cleanupobjs_lock);
	
	cleanupobjs.push_back(task);
}

void RobotRaconteurNode::RemovePeriodicCleanupTask(RR_SHARED_PTR<IPeriodicCleanupTask> task)
{
	boost::mutex::scoped_lock lock(cleanupobjs_lock);
	cleanupobjs.remove(task);
}


RR_SHARED_PTR<RRObject> RobotRaconteurNode::FindObjRefTyped(RR_SHARED_PTR<RRObject> obj, const std::string& objref, const std::string& objecttype)
{		
	RR_SHARED_PTR<ServiceStub> s = RR_DYNAMIC_POINTER_CAST<ServiceStub>(obj);
	if (!s) throw InvalidArgumentException("Only service stubs can be have objrefs");
	return s->FindObjRefTyped(objref,objecttype);
}

RR_SHARED_PTR<RRObject> RobotRaconteurNode::FindObjRefTyped(RR_SHARED_PTR<RRObject> obj, const std::string& objref, const std::string& index, const std::string& objecttype)
{
	RR_SHARED_PTR<ServiceStub> s = RR_DYNAMIC_POINTER_CAST<ServiceStub>(obj);
	if (!s) throw InvalidArgumentException("Only service stubs can be have objrefs");
	return s->FindObjRefTyped(objref,index,objecttype);
}

void RobotRaconteurNode::AsyncFindObjRefTyped(RR_SHARED_PTR<RRObject> obj, const std::string& objref, const std::string& objecttype, boost::function<void (RR_SHARED_PTR<RRObject>,RR_SHARED_PTR<RobotRaconteurException>)> handler, int32_t timeout)
{
	RR_SHARED_PTR<ServiceStub> s = RR_DYNAMIC_POINTER_CAST<ServiceStub>(obj);
	if (!s) throw InvalidArgumentException("Only service stubs can be have objrefs");
	s->AsyncFindObjRefTyped(objref,objecttype,RR_MOVE(handler),timeout);
}

void RobotRaconteurNode::AsyncFindObjRefTyped(RR_SHARED_PTR<RRObject> obj, const std::string& objref, const std::string& index, const std::string& objecttype, boost::function<void (RR_SHARED_PTR<RRObject>,RR_SHARED_PTR<RobotRaconteurException>)> handler, int32_t timeout)
{
	RR_SHARED_PTR<ServiceStub> s = RR_DYNAMIC_POINTER_CAST<ServiceStub>(obj);
	if (!s) throw InvalidArgumentException("Only service stubs can be have objrefs");
	s->AsyncFindObjRefTyped(objref,index,objecttype,RR_MOVE(handler),timeout);
}

std::string RobotRaconteurNode::FindObjectType(RR_SHARED_PTR<RRObject> obj, const std::string &n)
{
	RR_SHARED_PTR<ServiceStub> s = RR_DYNAMIC_POINTER_CAST<ServiceStub>(obj);
	if (!s) throw InvalidArgumentException("Only service stubs can be have objrefs");
	return s->FindObjectType(n);
}

std::string RobotRaconteurNode::FindObjectType(RR_SHARED_PTR<RRObject> obj, const std::string &n, const std::string &i)
{
	RR_SHARED_PTR<ServiceStub> s = RR_DYNAMIC_POINTER_CAST<ServiceStub>(obj);
	if (!s) throw InvalidArgumentException("Only service stubs can be have objrefs");
	return s->FindObjectType(n,i);
}
	
void RobotRaconteurNode::AsyncFindObjectType(RR_SHARED_PTR<RRObject> obj, const std::string &n, boost::function<void (RR_SHARED_PTR<std::string>,RR_SHARED_PTR<RobotRaconteurException>)> handler, int32_t timeout)
{
	RR_SHARED_PTR<ServiceStub> s = RR_DYNAMIC_POINTER_CAST<ServiceStub>(obj);
	if (!s) throw InvalidArgumentException("Only service stubs can be have objrefs");
	s->AsyncFindObjectType(n,RR_MOVE(handler),timeout);
}

void RobotRaconteurNode::AsyncFindObjectType(RR_SHARED_PTR<RRObject> obj, const std::string &n, const std::string &i, boost::function<void (RR_SHARED_PTR<std::string>,RR_SHARED_PTR<RobotRaconteurException>)> handler, int32_t timeout)
{
	RR_SHARED_PTR<ServiceStub> s = RR_DYNAMIC_POINTER_CAST<ServiceStub>(obj);
	if (!s) throw InvalidArgumentException("Only service stubs can be have objrefs");
	s->AsyncFindObjectType(n,i,RR_MOVE(handler),timeout);
}

std::vector<std::string> RobotRaconteurNode::GetPulledServiceTypes(RR_SHARED_PTR<RRObject> obj)
{
	RR_SHARED_PTR<ServiceStub> s = RR_DYNAMIC_POINTER_CAST<ServiceStub>(obj);
	if (!s) throw InvalidArgumentException("Only service stubs can be have objrefs");
	return s->GetContext()->GetPulledServiceTypes();
}

RR_SHARED_PTR<ServiceFactory> RobotRaconteurNode::GetPulledServiceType(RR_SHARED_PTR<RRObject> obj, const std::string& type)
{
	RR_SHARED_PTR<ServiceStub> s = RR_DYNAMIC_POINTER_CAST<ServiceStub>(obj);
	if (!s) throw InvalidArgumentException("Only service stubs can be have objrefs");
	return s->GetContext()->GetPulledServiceType(type);
}


void RobotRaconteurNode::StartPeriodicCleanupTask(RR_SHARED_PTR<RobotRaconteurNode> node)
{
	node->PeriodicCleanupTask_timer=node->CreateTimer(boost::posix_time::seconds(5),boost::bind(&RobotRaconteurNode::PeriodicCleanupTask,node,_1));
	node->PeriodicCleanupTask_timer->Start();
}

RR_SHARED_PTR<ThreadPool> RobotRaconteurNode::GetThreadPool()
{
	{
		boost::shared_lock<boost::shared_mutex> lock(thread_pool_lock);
		if (thread_pool) 
			return thread_pool;
	}
	
	InitThreadPool(20);
	
	{
		boost::unique_lock<boost::shared_mutex> lock(thread_pool_lock);
		if (!thread_pool)
		{
			throw InternalErrorException("Could not initialize thread pool");
		}
			
		return thread_pool;
	}
}

bool RobotRaconteurNode::TryGetThreadPool(RR_SHARED_PTR<ThreadPool>& t)
{
	{
		boost::shared_lock<boost::shared_mutex> t_lock(thread_pool_lock);
		t = thread_pool;
	}
	if (t)
	{
		return true;
	}
	else
	{
		try
		{
			t = GetThreadPool();
		}
		catch (std::exception&)
		{
			return false;
		}
	}

	return t != NULL;
}

void RobotRaconteurNode::SetThreadPool(RR_SHARED_PTR<ThreadPool> pool)
{

	if (is_shutdown) throw InvalidOperationException("Node has been shutdown");
	boost::unique_lock<boost::shared_mutex> lock(thread_pool_lock);
	if (thread_pool) throw InvalidOperationException("Thread pool already set");
	thread_pool=pool;

}

RR_SHARED_PTR<ThreadPoolFactory> RobotRaconteurNode::GetThreadPoolFactory()
{
	
	boost::mutex::scoped_lock lock(thread_pool_factory_lock);

	if (!thread_pool_factory)
	{
		thread_pool_factory=RR_MAKE_SHARED<ThreadPoolFactory>();
				
	}

	return thread_pool_factory;
}

void RobotRaconteurNode::SetThreadPoolFactory(RR_SHARED_PTR<ThreadPoolFactory> factory)
{

	
	boost::mutex::scoped_lock lock(thread_pool_factory_lock);
	if (thread_pool_factory) throw InvalidOperationException("Thread pool already set");
	thread_pool_factory=factory;

}

int32_t RobotRaconteurNode::GetThreadPoolCount()
{
	return (int32_t)GetThreadPool()->GetThreadPoolCount();
}

void RobotRaconteurNode::SetThreadPoolCount(int32_t count)
{
	InitThreadPool(count);
	GetThreadPool()->SetThreadPoolCount(count);
}

bool RobotRaconteurNode::TryLockThreadPool(RR_WEAK_PTR<RobotRaconteurNode> node, thread_pool_lock_type& lock, bool shutdown_op)
{
	RR_SHARED_PTR<RobotRaconteurNode> node1 = node.lock();
	if (!node1) return false;
	thread_pool_lock_type lock1(node1->thread_pool_lock);
	if (!shutdown_op && node1->is_shutdown) return false;
	lock.swap(lock1);
	return true;
}

bool RobotRaconteurNode::InitThreadPool(int32_t thread_count)
{
	boost::unique_lock<boost::shared_mutex> lock(thread_pool_lock);
	if (thread_pool) return false;

	
	if (is_shutdown)
	{
		throw InvalidOperationException("Node has been shutdown");
	}

	thread_pool = GetThreadPoolFactory()->NewThreadPool(shared_from_this());
	thread_pool->SetThreadPoolCount(thread_count);
	
	if (!PeriodicCleanupTask_timerstarted)
	{
		PeriodicCleanupTask_timerstarted = true;
		thread_pool->Post(boost::bind(&StartPeriodicCleanupTask, shared_from_this()));
	}

	return true;
}

void RobotRaconteurNode::SetExceptionHandler(boost::function<void (const std::exception*)> handler)
{
	boost::mutex::scoped_lock lock(exception_handler_lock);
	exception_handler=handler;
}

boost::function<void (const std::exception*)> RobotRaconteurNode::GetExceptionHandler()
{
	boost::mutex::scoped_lock lock(exception_handler_lock);
	return exception_handler;
}

void RobotRaconteurNode::HandleException(const std::exception* exp)
{
	if (exp==NULL) return;

	boost::function<void (const std::exception*)> h;
	{
		boost::mutex::scoped_lock lock(exception_handler_lock);
		h=exception_handler;
	}

	if (h)
	{
		try
		{
			h(exp);
		}
		catch (...) {}
	}
}

bool RobotRaconteurNode::TryHandleException(RR_WEAK_PTR<RobotRaconteurNode> node, const std::exception* exp)
{
	RR_SHARED_PTR<RobotRaconteurNode> node1 = node.lock();
	if (!node1) return false;
	node1->HandleException(exp);
	return true;
}

boost::posix_time::ptime RobotRaconteurNode::NowUTC()
{
	boost::shared_lock<boost::shared_mutex> lock(time_provider_lock);

	RR_SHARED_PTR<ITransportTimeProvider> t=time_provider.lock();

	if (t)
	{
		return t->NowUTC();
	}
	else
	{
		return boost::posix_time::microsec_clock::universal_time();
	}
}



RR_SHARED_PTR<Timer> RobotRaconteurNode::CreateTimer(const boost::posix_time::time_duration& period, boost::function<void (const TimerEvent&)> handler, bool oneshot)
{
	
	boost::shared_lock<boost::shared_mutex> lock(time_provider_lock);
	RR_SHARED_PTR<ITransportTimeProvider> t=time_provider.lock();
	if (!t)
	{
		return RR_MAKE_SHARED<WallTimer>(period,handler,oneshot,shared_from_this());
	}
	else
	{
		return t->CreateTimer(period,handler,oneshot);
	}
}

RR_SHARED_PTR<Rate> RobotRaconteurNode::CreateRate(double frequency)
{
	boost::shared_lock<boost::shared_mutex> lock(time_provider_lock);
	RR_SHARED_PTR<ITransportTimeProvider> t=time_provider.lock();
	if (!t)
	{
		return RR_MAKE_SHARED<WallRate>(frequency,shared_from_this());
	}
	else
	{
		return t->CreateRate(frequency);
	}
}

void RobotRaconteurNode::Sleep(const boost::posix_time::time_duration& duration)
{
	RR_SHARED_PTR<ITransportTimeProvider> t;
	{
		boost::shared_lock<boost::shared_mutex> lock(time_provider_lock);
		t=time_provider.lock();
	}
	if (!t)
	{
		boost::this_thread::sleep(duration);
	}
	else
	{
		t->Sleep(duration);
	}

}

RR_SHARED_PTR<AutoResetEvent> RobotRaconteurNode::CreateAutoResetEvent()
{
	boost::shared_lock<boost::shared_mutex> lock(time_provider_lock);
	RR_SHARED_PTR<ITransportTimeProvider> t=time_provider.lock();
	if (!t)
	{
		return RR_MAKE_SHARED<AutoResetEvent>();
	}
	else
	{
		return t->CreateAutoResetEvent();
	}

}

void RobotRaconteurNode::DownCastAndThrowException(RobotRaconteurException& exp)
{
	std::string type=exp.Error;
	if (!boost::contains(type,"."))
	{
		return;
	}
	boost::tuple<std::string,std::string> stype=SplitQualifiedName(type);
	if (!IsServiceTypeRegistered(stype.get<0>()))
	{
		return;
	}

	GetServiceType(stype.get<0>())->DownCastAndThrowException(exp);

}

RR_SHARED_PTR<RobotRaconteurException> RobotRaconteurNode::DownCastException(RR_SHARED_PTR<RobotRaconteurException> exp)
{
	if (!exp) return exp;
	std::string type=exp->Error;
	if (!boost::contains(type,"."))
	{
		return exp;
	}
	boost::tuple<std::string,std::string> stype=SplitQualifiedName(type);
	if (!IsServiceTypeRegistered(stype.get<0>()))
	{
		return exp;
	}

	return GetServiceType(stype.get<0>())->DownCastException(exp);
}

std::string RobotRaconteurNode::GetServicePath(RR_SHARED_PTR<RRObject> obj)
{
	if (!(dynamic_cast<ServiceStub*>(obj.get()) != 0))
		throw InvalidArgumentException("Only service stubs can be have objrefs");
	RR_SHARED_PTR<ServiceStub> s = rr_cast<ServiceStub>(obj);
	return s->ServicePath;
}

bool RobotRaconteurNode::IsEndpointLargeTransferAuthorized(uint32_t endpoint)
{
	try
	{
		RR_SHARED_PTR<Endpoint> e;
		{
			boost::mutex::scoped_lock lock(endpoint_lock);
			RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<Endpoint> >::iterator e1 = endpoints.find(endpoint);
			if (e1 == endpoints.end()) return false;
			e = e1->second;
		}

		RR_SHARED_PTR<ClientContext> c = RR_DYNAMIC_POINTER_CAST<ClientContext>(e);
		if (c)
		{
			return true;
		}

		RR_SHARED_PTR<ServerEndpoint> s = RR_DYNAMIC_POINTER_CAST<ServerEndpoint>(e);
		if (s)
		{

			if (s->service->RequireValidUser())
			{
				if (s->GetCurrentAuthenticatedUser())
				{
					return true;
				}
				else
				{
					return false;
				}
			}
			else
			{
				return true;
			}

			return false;
		}

		return false;
		
	}
	catch (std::exception&)
	{		
		return false;
	}

}

std::string RobotRaconteurNode::GetRobotRaconteurVersion()
{
	return ROBOTRACONTEUR_VERSION;

}

std::string RobotRaconteurNode::GetRandomString(size_t count)
{
	std::string o;
	boost::mutex::scoped_lock lock(random_generator_lock);
	std::string strvals = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
	boost::random::uniform_int_distribution<uint32_t> distribution(0, (uint32_t)(strvals.size() - 1));
	for (size_t i = 0; i<count; i++)
	{
		o += strvals.at(distribution(*random_generator));
	}
	return o;
}

std::string RobotRaconteurNode::GetServiceStateNonce()
{
	boost::mutex::scoped_lock lock(service_state_nonce_lock);
	return service_state_nonce;
}

void RobotRaconteurNode::UpdateServiceStateNonce()
{
	{
		boost::mutex::scoped_lock lock(service_state_nonce_lock);

		std::string new_nonce;
		do
		{
			new_nonce = GetRandomString(16);
		} while (new_nonce == service_state_nonce);

		service_state_nonce = new_nonce;
	}
	
	{
		boost::mutex::scoped_lock lock(transports_lock);
		BOOST_FOREACH(RR_SHARED_PTR<Transport>& t, transports | boost::adaptors::map_values)
		{
			t->LocalNodeServicesChanged();
		}
	}
}

}

