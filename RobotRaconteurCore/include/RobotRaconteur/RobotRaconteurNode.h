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

#pragma once

#include "RobotRaconteur/DataTypes.h"
#include "RobotRaconteur/NodeID.h"
#include "RobotRaconteur/Message.h"
#include "RobotRaconteur/ServiceFactory.h"
#include "RobotRaconteur/Transport.h"
#include "RobotRaconteur/AutoResetEvent.h"
#include "RobotRaconteur/Error.h"
#include "RobotRaconteur/AsyncUtils.h"
#include "RobotRaconteur/ThreadPool.h"
#include "RobotRaconteur/Timer.h"
#include "RobotRaconteur/Discovery.h"
#include <queue>
#include <boost/asio.hpp>
#include <boost/unordered_map.hpp>
#include <boost/function.hpp>
#include <boost/bind/protect.hpp>
#include <boost/random.hpp>
#include <boost/random/random_device.hpp>

#ifdef ROBOTRACONTEUR_WINDOWS

#undef SendMessage

#endif

namespace boost
{
namespace random
{
	class random_device;
}
}

namespace RobotRaconteur
{

	enum RobotRaconteurObjectLockFlags
	{
		RobotRaconteurObjectLockFlags_USER_LOCK = 0,
		RobotRaconteurObjectLockFlags_CLIENT_LOCK
	};
	
	class ROBOTRACONTEUR_CORE_API ServiceSecurityPolicy;
	class ROBOTRACONTEUR_CORE_API ServiceInfo2Subscription;
	class ROBOTRACONTEUR_CORE_API ServiceSubscription;
	class ROBOTRACONTEUR_CORE_API ServiceSubscriptionFilter;

	namespace detail
	{
		class Discovery;
	}

	class ROBOTRACONTEUR_CORE_API RobotRaconteurNode : boost::noncopyable, public RR_ENABLE_SHARED_FROM_THIS<RobotRaconteurNode>
	{
	private:
		
		bool is_shutdown;
		boost::mutex shutdown_lock;
		boost::signals2::signal<void ()> shutdown_listeners;
				
		static bool is_init;

		bool instance_is_init;
		static boost::mutex init_lock;

	public:

		friend class detail::Discovery;
		friend class Transport;
		
		void Init();

		RobotRaconteurNode();

		virtual ~RobotRaconteurNode();
		

		static RobotRaconteurNode* s();

		static RR_SHARED_PTR<RobotRaconteurNode> sp();

		RobotRaconteur::NodeID NodeID();

		std::string NodeName();

		void SetNodeID(const RobotRaconteur::NodeID& id);

		void SetNodeName(const std::string& name);

		RR_SHARED_PTR<ServiceFactory> GetServiceType(const std::string& servicename);
		
		bool IsServiceTypeRegistered(const std::string& servicename);

		void RegisterServiceType(RR_SHARED_PTR<ServiceFactory> factory);


		void UnregisterServiceType(const std::string& type);


		std::vector<std::string> GetRegisteredServiceTypes();

		RR_INTRUSIVE_PTR<MessageElementStructure> PackStructure(RR_INTRUSIVE_PTR<RRStructure> structure);

		RR_INTRUSIVE_PTR<RRStructure> UnpackStructure(RR_INTRUSIVE_PTR<MessageElementStructure> structure);

		RR_INTRUSIVE_PTR<MessageElementPodArray> PackPodArray(RR_INTRUSIVE_PTR<RRPodBaseArray> structure);

		RR_INTRUSIVE_PTR<RRPodBaseArray> UnpackPodArray(RR_INTRUSIVE_PTR<MessageElementPodArray> structure);
		
		RR_INTRUSIVE_PTR<MessageElementPodMultiDimArray> PackPodMultiDimArray(RR_INTRUSIVE_PTR<RRPodBaseMultiDimArray> structure);

		RR_INTRUSIVE_PTR<RRPodBaseMultiDimArray> UnpackPodMultiDimArray(RR_INTRUSIVE_PTR<MessageElementPodMultiDimArray> structure);

		RR_INTRUSIVE_PTR<MessageElementNamedArray> PackNamedArray(RR_INTRUSIVE_PTR<RRNamedBaseArray> structure);

		RR_INTRUSIVE_PTR<RRNamedBaseArray> UnpackNamedArray(RR_INTRUSIVE_PTR<MessageElementNamedArray> structure);

		RR_INTRUSIVE_PTR<MessageElementNamedMultiDimArray> PackNamedMultiDimArray(RR_INTRUSIVE_PTR<RRNamedBaseMultiDimArray> structure);

		RR_INTRUSIVE_PTR<RRNamedBaseMultiDimArray> UnpackNamedMultiDimArray(RR_INTRUSIVE_PTR<MessageElementNamedMultiDimArray> structure);


		template <typename T>
		RR_INTRUSIVE_PTR<MessageElementMultiDimArray> PackMultiDimArray(RR_INTRUSIVE_PTR<RRMultiDimArray<T> > arr)
		{
			if (!arr) return RR_INTRUSIVE_PTR<MessageElementMultiDimArray>();

			std::vector<RR_INTRUSIVE_PTR<MessageElement> > ar;			
			ar.push_back(CreateMessageElement("dims",arr->Dims));
			ar.push_back(CreateMessageElement("array",arr->Array));			
			return CreateMessageElementMultiDimArray(ar);
		}

		template <typename T>
		RR_INTRUSIVE_PTR<RRMultiDimArray<T> > UnpackMultiDimArray(RR_INTRUSIVE_PTR<MessageElementMultiDimArray> ar)
		{
			if (!ar) return RR_INTRUSIVE_PTR<RRMultiDimArray<T> >();

			RR_INTRUSIVE_PTR<RRMultiDimArray<T> > arr=AllocateEmptyRRMultiDimArray<T>();
			arr->Dims=MessageElement::FindElement(ar->Elements,"dims")->CastData<RRArray<uint32_t> >();
			arr->Array=MessageElement::FindElement(ar->Elements,"array")->CastData<RRArray<T> >();			
			return arr;
		}

	
		RR_INTRUSIVE_PTR<MessageElementData> PackVarType(RR_INTRUSIVE_PTR<RRValue> vardata);

		RR_INTRUSIVE_PTR<RRValue> UnpackVarType(RR_INTRUSIVE_PTR<MessageElement> mvardata);
	
	private:
		template<typename K, typename T>
		class PackMapTypeSupport
		{
		public:
			static RR_INTRUSIVE_PTR<MessageElementMap<K> > PackMapType(RobotRaconteurNode* node, const RR_INTRUSIVE_PTR<RRValue> set)
			{
				BOOST_STATIC_ASSERT(sizeof(T) == 0);
			}

			static RR_INTRUSIVE_PTR<RRValue> UnpackMapType(RobotRaconteurNode* node, const RR_INTRUSIVE_PTR<MessageElementMap<K> > mset)
			{
				BOOST_STATIC_ASSERT(sizeof(T) == 0);
			}

		};

		template<typename T>
		class PackMapTypeSupport<int32_t, T>
		{
		public:
			template<typename U>
			static RR_INTRUSIVE_PTR<MessageElementMap<int32_t> > PackMapType(RobotRaconteurNode* node, const U& set)
			{
				if (!set) return RR_INTRUSIVE_PTR<MessageElementMap<int32_t> >();

				RR_INTRUSIVE_PTR<RRMap<int32_t, T> > set2 = rr_cast<RRMap<int32_t, T> >(set);

				std::vector<RR_INTRUSIVE_PTR<MessageElement> > mret;


				for (typename std::map<int32_t, RR_INTRUSIVE_PTR<T> >::iterator e = set2->begin(); e != set2->end(); e++)
				{
					int32_t key = e->first;

					RR_INTRUSIVE_PTR<MessageElementData> dat = node->PackAnyType<RR_INTRUSIVE_PTR<T> >(e->second);

					RR_INTRUSIVE_PTR<MessageElement> m = CreateMessageElement("", dat);
					m->ElementFlags &= ~MessageElementFlags_ELEMENT_NAME_STR;
					m->ElementFlags |= MessageElementFlags_ELEMENT_NUMBER;
					m->ElementNumber = key;
					mret.push_back(m);
				}

				return CreateMessageElementMap<int32_t>(mret);
			}

			static RR_INTRUSIVE_PTR<RRMap<int32_t,T> > UnpackMapType(RobotRaconteurNode* node, const RR_INTRUSIVE_PTR<MessageElementMap<int32_t> >& mset)
			{
				if (!mset) return RR_INTRUSIVE_PTR<RRMap<int32_t,T> >();

				RR_INTRUSIVE_PTR<RRMap<int32_t, T> > ret = AllocateEmptyRRMap<int32_t, T>();

				for (std::vector<RR_INTRUSIVE_PTR<MessageElement> >::iterator e = mset->Elements.begin(); e != mset->Elements.end(); e++)
				{
					RR_INTRUSIVE_PTR<MessageElement> m = *e;
					int32_t key;
					if (m->ElementFlags & MessageElementFlags_ELEMENT_NUMBER)
					{
						key = m->ElementNumber;
					}
					else if (m->ElementFlags & MessageElementFlags_ELEMENT_NAME_STR)
					{
						key = boost::lexical_cast<int32_t>(m->ElementName);
					}
					else
					{
						throw DataTypeException("Invalid map format");
					}

					RR_INTRUSIVE_PTR<T> dat = node->UnpackAnyType<RR_INTRUSIVE_PTR<T> >(m);
					ret->insert(std::make_pair(key, dat));
				}

				return ret;
			}
		};

		template<typename T>
		class PackMapTypeSupport<std::string, T>
		{
		public:
			template<typename U>
			static RR_INTRUSIVE_PTR<MessageElementMap<std::string> > PackMapType(RobotRaconteurNode* node, const U& set)
			{
				if (!set) return RR_INTRUSIVE_PTR<MessageElementMap<std::string> >();

				RR_INTRUSIVE_PTR<RRMap<std::string, T> > set2 = rr_cast<RRMap<std::string, T> >(set);

				std::vector<RR_INTRUSIVE_PTR<MessageElement> > mret;

				for (typename std::map<std::string, RR_INTRUSIVE_PTR<T> >::iterator e = set2->begin(); e != set2->end(); e++)
				{
					RR_INTRUSIVE_PTR<MessageElementData> dat = node->PackAnyType<RR_INTRUSIVE_PTR<T> >(e->second);

					RR_INTRUSIVE_PTR<MessageElement> m = CreateMessageElement("", dat);					
					m->ElementName = e->first;
					mret.push_back(m);
				}

				return CreateMessageElementMap<std::string>(mret);
			}

			static RR_INTRUSIVE_PTR<RRMap<std::string,T> > UnpackMapType(RobotRaconteurNode* node, const RR_INTRUSIVE_PTR<MessageElementMap<std::string> >& mset)
			{
				if (!mset) return RR_INTRUSIVE_PTR<RRMap<std::string,T> >();

				RR_INTRUSIVE_PTR<RRMap<std::string, T> > ret = AllocateEmptyRRMap<std::string, T>();

				for (std::vector<RR_INTRUSIVE_PTR<MessageElement> >::iterator e = mset->Elements.begin(); e != mset->Elements.end(); e++)
				{
					RR_INTRUSIVE_PTR<MessageElement> m = *e;

					if (!(m->ElementFlags & MessageElementFlags_ELEMENT_NAME_STR))
					{
						throw DataTypeException("Invalid map format");
					}

					std::string& key = m->ElementName;
					
					RR_INTRUSIVE_PTR<T> dat = node->UnpackAnyType<RR_INTRUSIVE_PTR<T> >(m);
					ret->insert(std::make_pair(key, dat));
				}

				return ret;
			}
		};

	public:

		template<typename K, typename T, typename U>
		RR_INTRUSIVE_PTR<MessageElementMap<K> > PackMapType(const U& set)
		{
			return PackMapTypeSupport<K, T>::PackMapType(this, set);
		}

		template<typename K, typename T>
		RR_INTRUSIVE_PTR<RRMap<K,T> > UnpackMapType(const RR_INTRUSIVE_PTR<MessageElementMap<K> >& mset)
		{
			return PackMapTypeSupport<K, T>::UnpackMapType(this, mset);
		}	


		template<typename T, typename U>
		RR_INTRUSIVE_PTR<MessageElementList > PackListType(U& set)
		{
			if (!set) return RR_INTRUSIVE_PTR<MessageElementList >();			

			RR_INTRUSIVE_PTR<RRList<T> > set2=rr_cast<RRList<T> >(set);			

			std::vector<RR_INTRUSIVE_PTR<MessageElement> > mret;

			
			typename RRList<T>::iterator set2_iter = set2->begin();
			for (int32_t i=0; i < (int32_t)set2->size(); i++)
			{
				int32_t key = i;

				RR_INTRUSIVE_PTR<MessageElementData> dat=PackAnyType<RR_INTRUSIVE_PTR<T> >(*set2_iter);

				RR_INTRUSIVE_PTR<MessageElement> m=CreateMessageElement("",dat);
				m->ElementFlags &= ~MessageElementFlags_ELEMENT_NAME_STR;
				m->ElementFlags |= MessageElementFlags_ELEMENT_NUMBER;
				m->ElementNumber = key;
				mret.push_back(m);
				++set2_iter;
			}

			return CreateMessageElementList(mret);		
		}

		template<typename T>
		RR_INTRUSIVE_PTR<RRList<T> > UnpackListType(const RR_INTRUSIVE_PTR<MessageElementList >& mset)
		{
			if (!mset) return RR_INTRUSIVE_PTR<RRList<T> >();			
			
			RR_INTRUSIVE_PTR<RRList<T> > ret=AllocateEmptyRRList<T>();

			for (int32_t i=0; i<(int32_t)mset->Elements.size(); i++)
			{
				RR_INTRUSIVE_PTR<MessageElement> m = mset->Elements.at(i);
				int32_t key;
				if (m->ElementFlags & MessageElementFlags_ELEMENT_NUMBER)
				{
					key = m->ElementNumber;
				}
				else if (m->ElementFlags & MessageElementFlags_ELEMENT_NAME_STR)
				{
					key = boost::lexical_cast<int32_t>(m->ElementName);
				}				 
				else
				{
					throw DataTypeException("Invalid list format");
				}
				
				if (key!=i) throw DataTypeException("Invalid list format");

				RR_INTRUSIVE_PTR<T> dat=UnpackAnyType<RR_INTRUSIVE_PTR<T> >(m);
				ret->push_back(dat);
			}

			return ret;
		}


	private:
		template<typename T > 
		class PackAnyTypeSupport
		{
		public:
			template<typename NodeType>
			static RR_INTRUSIVE_PTR<MessageElementData> PackAnyType(const RR_INTRUSIVE_PTR<RRValue>& data, NodeType node)
			{
				return node->PackVarType(data);
			}

			template<typename NodeType>
			static RR_INTRUSIVE_PTR<RRValue> UnpackAnyType(const RR_INTRUSIVE_PTR<MessageElement>& mdata, NodeType node)
			{
				return node->UnpackVarType(mdata);
			}
		};		

		template<typename T >
		class PackAnyTypeSupport<RR_INTRUSIVE_PTR<T> >
		{
		public:
			template<typename U, typename NodeType>
			static RR_INTRUSIVE_PTR<MessageElementData> PackAnyType(const U& data, NodeType node)
			{
				if (boost::is_base_of<RRStructure,T>::value)
				{ 
					return node->PackStructure(rr_cast<RRStructure>(data));
				}
				return node->PackVarType(data);
			}
			
			template<typename NodeType>
			static RR_INTRUSIVE_PTR<T> UnpackAnyType(const RR_INTRUSIVE_PTR<MessageElement>& mdata, NodeType node)
			{
				if (boost::is_base_of<RRStructure, T>::value)
				{
					return rr_cast<T>(node->UnpackStructure(mdata->CastData<MessageElementStructure>()));
				}

				return rr_cast<T>(node->UnpackVarType(mdata));
			}
		};

		template<typename T>
		class PackAnyTypeSupport<RR_INTRUSIVE_PTR<RRArray<T> > >
		{
		public:
			template<typename U, typename NodeType>
			static RR_INTRUSIVE_PTR<MessageElementData> PackAnyType(const U& data, NodeType node)
			{
				return RR_STATIC_POINTER_CAST<MessageElementData>(data);
			}

			template<typename NodeType>
			static RR_INTRUSIVE_PTR<RRArray<T> > UnpackAnyType(const RR_INTRUSIVE_PTR<MessageElement>& mdata, NodeType node)
			{
				return mdata->CastData<RRArray<T> >();
			}
		};
		
		template<typename K, typename T>
		class PackAnyTypeSupport<RR_INTRUSIVE_PTR<RRMap<K,T> > >
		{
		public:
			template<typename U, typename NodeType>
			static RR_INTRUSIVE_PTR<MessageElementData> PackAnyType(const U& data, NodeType node)
			{
				return node->template PackMapType<K,T>(data);
			}

			template<typename NodeType>
			static RR_INTRUSIVE_PTR<RRMap<K,T> > UnpackAnyType(const RR_INTRUSIVE_PTR<MessageElement>& mdata, NodeType node)
			{
				return node->template UnpackMapType<K,T>(mdata->CastData<MessageElementMap<K> >());
			}
		};
		
		template<typename T>
		class PackAnyTypeSupport<RR_INTRUSIVE_PTR<RRList<T> > >
		{
		public:
			template<typename U, typename NodeType>
			static RR_INTRUSIVE_PTR<MessageElementData> PackAnyType(const U& data, NodeType node)
			{
				return node->template PackListType<T>(data);
			}

			template<typename NodeType>
			static RR_INTRUSIVE_PTR<RRList<T> > UnpackAnyType(const RR_INTRUSIVE_PTR<MessageElement>& mdata, NodeType node)
			{
				return node->template UnpackListType<T>(mdata->CastData<MessageElementList >());
			}
		};

		template<typename T>
		class PackAnyTypeSupport<RR_INTRUSIVE_PTR<RRMultiDimArray<T> > >
		{
		public:
			template<typename U, typename NodeType>
			static RR_INTRUSIVE_PTR<MessageElementData> PackAnyType(const U& data, NodeType node)
			{
				return node->template PackMultiDimArray<T>(rr_cast<RRMultiDimArray<T> >(data));
			}

			template<typename NodeType>
			static RR_INTRUSIVE_PTR<RRMultiDimArray<T> > UnpackAnyType(const RR_INTRUSIVE_PTR<MessageElement>& mdata, NodeType node)
			{
				return node->template UnpackMultiDimArray<T>(mdata->CastData<MessageElementMultiDimArray >());
			}
		};

	public:
		
		template<typename T, typename U> RR_INTRUSIVE_PTR<MessageElementData> PackAnyType(const RR_INTRUSIVE_PTR<U>& data)
		{
			return PackAnyTypeSupport<T>::PackAnyType(data,this);
		}

		template<typename T> T UnpackAnyType(const RR_INTRUSIVE_PTR<MessageElement>& mdata)
		{
			return PackAnyTypeSupport<T>::UnpackAnyType(mdata,this);
		}
		

		virtual void Shutdown();

		typedef boost::signals2::connection shutdown_listener_connection;
		template<typename Handler>
		shutdown_listener_connection AddShutdownListener(BOOST_ASIO_MOVE_ARG(Handler) h)
		{
			return shutdown_listeners.connect(h);
		}
				
		uint32_t RegisterTransport(RR_SHARED_PTR<Transport> transport);

		void AsyncSendMessage(RR_INTRUSIVE_PTR<Message> m, boost::function<void (RR_SHARED_PTR<RobotRaconteurException> )>& callback);

		void SendMessage(RR_INTRUSIVE_PTR<Message> m);

		void MessageReceived(RR_INTRUSIVE_PTR<Message> m);
	protected:

		void TransportConnectionClosed(uint32_t endpoint);

	private:
		uint32_t RequestTimeout;
		boost::mutex RequestTimeout_lock;

		uint32_t TransportInactivityTimeout;
		boost::mutex TransportInactivityTimeout_lock;

		uint32_t EndpointInactivityTimeout;
		boost::mutex EndpointInactivityTimeout_lock;
		
		uint32_t MemoryMaxTransferSize;
		boost::mutex MemoryMaxTransferSize_lock;
				
	public:

		uint32_t GetRequestTimeout();
		void SetRequestTimeout(uint32_t timeout);

		uint32_t GetTransportInactivityTimeout();
		void SetTransportInactivityTimeout(uint32_t timeout);

		uint32_t GetEndpointInactivityTimeout();
		void SetEndpointInactivityTimeout(uint32_t timeout);

		uint32_t GetMemoryMaxTransferSize();
		void SetMemoryMaxTransferSize(uint32_t size);

	protected:
		RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<Endpoint> > endpoints;
		std::map<uint32_t,boost::posix_time::ptime> recent_endpoints;
		
		RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<Transport> > transports;
		boost::mutex transports_lock;
	
		RR_SHARED_PTR<RobotRaconteur::DynamicServiceFactory> dynamic_factory;
		boost::mutex dynamic_factory_lock;
	public:

		const RR_SHARED_PTR<RobotRaconteur::DynamicServiceFactory> GetDynamicServiceFactory() ;


		void SetDynamicServiceFactory(RR_SHARED_PTR<RobotRaconteur::DynamicServiceFactory> f);

		RR_INTRUSIVE_PTR<Message> GenerateErrorReturnMessage(RR_INTRUSIVE_PTR<Message> m, MessageErrorType err, const std::string &errname, const std::string &errdesc);



		RR_SHARED_PTR<ServerContext> RegisterService(const std::string &name, const std::string &servicetype, RR_SHARED_PTR<RRObject> obj, RR_SHARED_PTR<ServiceSecurityPolicy> securitypolicy = RR_SHARED_PTR<ServiceSecurityPolicy>());


		RR_SHARED_PTR<ServerContext> RegisterService(RR_SHARED_PTR<ServerContext> c);

		void CloseService(const std::string &sname);


		std::vector<std::string> GetRegisteredServiceNames();

		RR_SHARED_PTR<ServerContext> GetService(const std::string &name);

		public:
			

		RR_INTRUSIVE_PTR<Message> SpecialRequest(RR_INTRUSIVE_PTR<Message> m, uint32_t transportid, RR_SHARED_PTR<ITransportConnection> tc);

		RR_SHARED_PTR<RRObject> ConnectService(const std::string &url, const std::string &username = "", RR_INTRUSIVE_PTR<RRMap<std::string,RRValue> > credentials=(RR_INTRUSIVE_PTR<RRMap<std::string,RRValue> >()), boost::function<void (RR_SHARED_PTR<ClientContext>,ClientServiceListenerEventType,RR_SHARED_PTR<void>)> listener = 0, const std::string& objecttype="");

		void AsyncConnectService(const std::string &url, const std::string &username, RR_INTRUSIVE_PTR<RRMap<std::string,RRValue> > credentials, boost::function<void (RR_SHARED_PTR<ClientContext>,ClientServiceListenerEventType,RR_SHARED_PTR<void>)> listener, const std::string& objecttype, boost::function<void(RR_SHARED_PTR<RRObject>,RR_SHARED_PTR<RobotRaconteurException>)> handler, int32_t timeout=RR_TIMEOUT_INFINITE);


		RR_SHARED_PTR<RRObject> ConnectService(const std::vector<std::string>& urls, const std::string &username = "", RR_INTRUSIVE_PTR<RRMap<std::string,RRValue> > credentials=(RR_INTRUSIVE_PTR<RRMap<std::string,RRValue> >()), boost::function<void (RR_SHARED_PTR<ClientContext>,ClientServiceListenerEventType,RR_SHARED_PTR<void>)> listener = 0, const std::string& objecttype="");

		void AsyncConnectService(const std::vector<std::string> &url, const std::string &username, RR_INTRUSIVE_PTR<RRMap<std::string,RRValue> > credentials, boost::function<void (RR_SHARED_PTR<ClientContext>,ClientServiceListenerEventType,RR_SHARED_PTR<void>)> listener, const std::string& objecttype, boost::function<void(RR_SHARED_PTR<RRObject>,RR_SHARED_PTR<RobotRaconteurException>)> handler, int32_t timeout=RR_TIMEOUT_INFINITE);

		void DisconnectService(RR_SHARED_PTR<RRObject> obj);

		void AsyncDisconnectService(RR_SHARED_PTR<RRObject> obj, boost::function<void()> handler);

		std::map<std::string, RR_INTRUSIVE_PTR<RRValue> > GetServiceAttributes(RR_SHARED_PTR<RRObject> obj);

		uint32_t RegisterEndpoint(RR_SHARED_PTR<Endpoint> e);

		void DeleteEndpoint(RR_SHARED_PTR<Endpoint> e);

		void CheckConnection(uint32_t endpoint);



	private:

		
		uint32_t transport_count;
		

		boost::shared_mutex transport_lock;
		boost::mutex endpoint_lock;

		static RobotRaconteurNode m_s;
		static RR_SHARED_PTR<RobotRaconteurNode> m_sp;

		RobotRaconteur::NodeID m_NodeID;
		std::string m_NodeName;

		bool NodeID_set;
		bool NodeName_set;

		RR_UNORDERED_MAP<std::string,RR_SHARED_PTR<ServiceFactory> > service_factories;
		boost::shared_mutex service_factories_lock;
		boost::mutex id_lock;

		RR_UNORDERED_MAP<std::string,RR_SHARED_PTR<ServerContext> > services;
		boost::shared_mutex services_lock;

		RR_SHARED_PTR<detail::Discovery> m_Discovery;
		boost::signals2::signal<void(const NodeDiscoveryInfo&, const std::vector<ServiceInfo2>& )> discovery_updated_listeners;

		boost::signals2::signal<void(const NodeDiscoveryInfo&)> discovery_lost_listeners;
				
	public:
		std::vector<NodeDiscoveryInfo> GetDetectedNodes();

		typedef boost::signals2::connection node_updated_listener_connection;
		template<typename Handler>
		node_updated_listener_connection AddNodeServicesDetectedListener(BOOST_ASIO_MOVE_ARG(Handler) h)
		{
			boost::shared_lock<boost::shared_mutex> l(thread_pool_lock);
			if (is_shutdown)
			{
				throw InvalidOperationException("Node has been shut down");
			}

			return discovery_updated_listeners.connect(h);
		}

		typedef boost::signals2::connection node_lost_listener_connection;
		template<typename Handler>
		node_lost_listener_connection AddNodeDetectionLostListener(BOOST_ASIO_MOVE_ARG(Handler) h)
		{
			boost::shared_lock<boost::shared_mutex> l(thread_pool_lock);
			if (is_shutdown)
			{
				throw InvalidOperationException("Node has been shut down");
			}
			return discovery_lost_listeners.connect(h);
		}

		void NodeAnnouncePacketReceived(const std::string& packet);

		void NodeDetected(const NodeDiscoveryInfo& info);

		void UpdateDetectedNodes(const std::vector<std::string>& schemes);

		void AsyncUpdateDetectedNodes(const std::vector<std::string>& schemes, boost::function<void()> handler, int32_t timeout = 5000);

		uint32_t GetNodeDiscoveryMaxCacheCount();
		void SetNodeDiscoveryMaxCacheCount(uint32_t count);

		RR_SHARED_PTR<ServiceInfo2Subscription> SubscribeServiceInfo2(const std::vector<std::string>& service_types, RR_SHARED_PTR<ServiceSubscriptionFilter> filter = RR_SHARED_PTR<ServiceSubscriptionFilter>());

		RR_SHARED_PTR<ServiceSubscription> SubscribeService(const std::vector<std::string>& service_types, RR_SHARED_PTR<ServiceSubscriptionFilter> filter = RR_SHARED_PTR<ServiceSubscriptionFilter>());

	protected:
		void CleanDiscoveredNodes();

		void FireNodeDetected(RR_SHARED_PTR<NodeDiscoveryInfo> node, RR_SHARED_PTR<std::vector<ServiceInfo2> > services);

		void FireNodeLost(RR_SHARED_PTR<NodeDiscoveryInfo> node);

	public:
		static std::string SelectRemoteNodeURL(const std::vector<std::string>& urls);
		
		std::vector<ServiceInfo2> FindServiceByType(const std::string &servicetype, const std::vector<std::string>& transportschemes);

		void AsyncFindServiceByType(const std::string &servicetype, const std::vector<std::string>& transportschemes, boost::function< void(RR_SHARED_PTR<std::vector<ServiceInfo2> >) > handler, int32_t timeout=5000);
		
		std::vector<NodeInfo2> FindNodeByID(const RobotRaconteur::NodeID& id, const std::vector<std::string>& transportschemes);

		void AsyncFindNodeByID(const RobotRaconteur::NodeID& id, const std::vector<std::string>& transportschemes, boost::function< void(RR_SHARED_PTR<std::vector<NodeInfo2> >) > handler, int32_t timeout=5000);

		std::vector<NodeInfo2> FindNodeByName(const std::string& name, const std::vector<std::string>& transportschemes);

		void AsyncFindNodeByName(const std::string& name, const std::vector<std::string>& transportschemes, boost::function< void(RR_SHARED_PTR<std::vector<NodeInfo2> >) > handler, int32_t timeout=5000);

	public:

		std::string RequestObjectLock(RR_SHARED_PTR<RRObject> obj, RobotRaconteurObjectLockFlags flags);

		void AsyncRequestObjectLock(RR_SHARED_PTR<RRObject> obj, RobotRaconteurObjectLockFlags flags, boost::function<void(RR_SHARED_PTR<std::string>,RR_SHARED_PTR<RobotRaconteurException>)> handler, int32_t timeout=RR_TIMEOUT_INFINITE);

		std::string ReleaseObjectLock(RR_SHARED_PTR<RRObject> obj);

		void AsyncReleaseObjectLock(RR_SHARED_PTR<RRObject> obj, boost::function<void(RR_SHARED_PTR<std::string>,RR_SHARED_PTR<RobotRaconteurException>)> handler, int32_t timeout=RR_TIMEOUT_INFINITE);

		void MonitorEnter(RR_SHARED_PTR<RRObject> obj, int32_t timeout = RR_TIMEOUT_INFINITE);

		void MonitorExit(RR_SHARED_PTR<RRObject> obj);

		class ROBOTRACONTEUR_CORE_API ScopedMonitorLock
		{
			bool locked;
			RR_SHARED_PTR<RRObject> obj;
			RR_WEAK_PTR<RobotRaconteurNode> node;
			
		public:

			RR_SHARED_PTR<RobotRaconteurNode> GetNode();
			
			ScopedMonitorLock(RR_SHARED_PTR<RRObject> obj, int32_t timeout=-1);

			void lock(int32_t timeout=-1);

			void unlock();

			void release();

			~ScopedMonitorLock();
			

		};

	private:
		
		

		
		RR_SHARED_PTR<Timer> PeriodicCleanupTask_timer;

		static void StartPeriodicCleanupTask(RR_SHARED_PTR<RobotRaconteurNode> node);

		void PeriodicCleanupTask(const TimerEvent& err);
		bool PeriodicCleanupTask_timerstarted;

		std::list<RR_SHARED_PTR<IPeriodicCleanupTask> > cleanupobjs;

		boost::mutex cleanupobjs_lock;

		boost::mutex cleanup_thread_lock;

	public:
		void AddPeriodicCleanupTask(RR_SHARED_PTR<IPeriodicCleanupTask> task);

		void RemovePeriodicCleanupTask(RR_SHARED_PTR<IPeriodicCleanupTask> task);

		

	public:
		//Utility functions to help retrieve objrefs with a specific type

		RR_SHARED_PTR<RRObject> FindObjRefTyped(RR_SHARED_PTR<RRObject> obj, const std::string& objref, const std::string& objecttype);

		RR_SHARED_PTR<RRObject> FindObjRefTyped(RR_SHARED_PTR<RRObject> obj, const std::string& objref, const std::string& index, const std::string& objecttype);

		void AsyncFindObjRefTyped(RR_SHARED_PTR<RRObject> obj, const std::string& objref, const std::string& objecttype, boost::function<void (RR_SHARED_PTR<RRObject>,RR_SHARED_PTR<RobotRaconteurException>)> handler, int32_t timeout=RR_TIMEOUT_INFINITE);

		void AsyncFindObjRefTyped(RR_SHARED_PTR<RRObject> obj, const std::string& objref, const std::string& index, const std::string& objecttype, boost::function<void (RR_SHARED_PTR<RRObject>,RR_SHARED_PTR<RobotRaconteurException>)> handler, int32_t timeout=RR_TIMEOUT_INFINITE);

		std::string FindObjectType(RR_SHARED_PTR<RRObject> obj, const std::string &n);

		std::string FindObjectType(RR_SHARED_PTR<RRObject> obj, const std::string &n, const std::string &i);
		
		void AsyncFindObjectType(RR_SHARED_PTR<RRObject> obj, const std::string &n, boost::function<void (RR_SHARED_PTR<std::string>,RR_SHARED_PTR<RobotRaconteurException>)> handler, int32_t timeout=RR_TIMEOUT_INFINITE);

		void AsyncFindObjectType(RR_SHARED_PTR<RRObject> obj, const std::string &n, const std::string &i, boost::function<void (RR_SHARED_PTR<std::string>,RR_SHARED_PTR<RobotRaconteurException>)> handler, int32_t timeout=RR_TIMEOUT_INFINITE);


	private:
		RR_SHARED_PTR<ThreadPool> thread_pool;
		boost::shared_mutex thread_pool_lock;
		RR_SHARED_PTR<ThreadPoolFactory> thread_pool_factory;
		boost::mutex thread_pool_factory_lock;

	public:

		RR_SHARED_PTR<ThreadPool> GetThreadPool();

		bool TryGetThreadPool(RR_SHARED_PTR<ThreadPool>& t);

		void SetThreadPool(RR_SHARED_PTR<ThreadPool> pool);

		RR_SHARED_PTR<ThreadPoolFactory> GetThreadPoolFactory();

		void SetThreadPoolFactory(RR_SHARED_PTR<ThreadPoolFactory> factory);

		int32_t GetThreadPoolCount();

		void SetThreadPoolCount(int32_t count);

		template<typename HandlerType>
		static bool TryPostToThreadPool(RR_WEAK_PTR<RobotRaconteurNode> node, BOOST_ASIO_MOVE_ARG(HandlerType) h, bool shutdown_op=false)
		{			
			RR_SHARED_PTR<RobotRaconteurNode> node1 = node.lock();
			{
				boost::shared_lock<boost::shared_mutex> l(node1->thread_pool_lock);
				if (!shutdown_op && node1->is_shutdown) return false;
			}
			RR_SHARED_PTR<ThreadPool> t;
			if (!node1->TryGetThreadPool(t)) return false;
			return t->TryPost(RR_MOVE(h));
		}

		typedef boost::shared_lock<boost::shared_mutex> thread_pool_lock_type;

		static bool TryLockThreadPool(RR_WEAK_PTR<RobotRaconteurNode> node, thread_pool_lock_type& lock, bool shutdown_op=false);

		//Do not call ReleaseThreadPool unless you really know what you are doing. In most cases it will be destroyed automatically.
		void ReleaseThreadPool();

	protected:

		bool InitThreadPool(int32_t thread_count);

	public:

		std::vector<std::string> GetPulledServiceTypes(RR_SHARED_PTR<RRObject> obj);

		RR_SHARED_PTR<ServiceFactory> GetPulledServiceType(RR_SHARED_PTR<RRObject> obj, const std::string& type);

		void SetExceptionHandler(boost::function<void (const std::exception*)> handler);

		boost::function<void (const std::exception*)> GetExceptionHandler();

		void HandleException(const std::exception* exp);

		static bool TryHandleException(RR_WEAK_PTR<RobotRaconteurNode> node, const std::exception* exp);

	protected:

		boost::function<void (const std::exception*) > exception_handler;
		boost::mutex exception_handler_lock;

		boost::mutex random_generator_lock;
		RR_SHARED_PTR<boost::random::random_device> random_generator;

	public:

		virtual boost::posix_time::ptime NowUTC();

	protected:

		RR_WEAK_PTR<ITransportTimeProvider> time_provider;
		boost::shared_mutex time_provider_lock;

	public:

		virtual RR_SHARED_PTR<Timer> CreateTimer(const boost::posix_time::time_duration& period, boost::function<void (const TimerEvent&)> handler, bool oneshot=false);
		
		virtual RR_SHARED_PTR<Rate> CreateRate(double frequency);

		virtual void Sleep(const boost::posix_time::time_duration& duration);

		virtual RR_SHARED_PTR<AutoResetEvent> CreateAutoResetEvent();

		void DownCastAndThrowException(RobotRaconteurException& exp);

		RR_SHARED_PTR<RobotRaconteurException> DownCastException(RR_SHARED_PTR<RobotRaconteurException> exp);

		virtual std::string GetServicePath(RR_SHARED_PTR<RRObject> obj);

		bool IsEndpointLargeTransferAuthorized(uint32_t endpoint);

		std::string GetRobotRaconteurVersion();

		template<typename T>
		T GetRandomInt(T min, T max)
		{
			boost::mutex::scoped_lock lock(random_generator_lock);
			boost::random::uniform_int_distribution<T> d(min, max);
			return d(*random_generator);
		}

		template<typename T>
		std::vector<T> GetRandomInts(size_t count, T min, T max)
		{
			std::vector<T> o;
			o.resize(count);
			boost::mutex::scoped_lock lock(random_generator_lock);
			for (size_t i = 0; i < count; i++)
			{
				boost::random::uniform_int_distribution<T> d(min, max);
				o[i]=d(*random_generator);
			}
			return o;
		}

		std::string GetRandomString(size_t count);

	protected:

		std::string service_state_nonce;
		boost::mutex service_state_nonce_lock;

	public:

		std::string GetServiceStateNonce();
		void UpdateServiceStateNonce();

	public:

		//Race free functions for ASIO
		template<typename T, typename F>
		static bool asio_async_wait(RR_WEAK_PTR<RobotRaconteurNode> node, RR_SHARED_PTR<T>& t, BOOST_ASIO_MOVE_ARG(F) f)
		{
			RR_SHARED_PTR<RobotRaconteurNode> node1 = node.lock();
			if (!node1) return false;
			boost::shared_lock<boost::shared_mutex> l(node1->thread_pool_lock);
			if (node1->is_shutdown)
			{
				l.unlock();
				RR_SHARED_PTR<ThreadPool> t;
				if (!node1->TryGetThreadPool(t)) return false;
				return t->TryPost(boost::bind(f, boost::asio::error::operation_aborted));
			}

			t->async_wait(f);
			node1->shutdown_listeners.connect(
				boost::signals2::signal<void()>::slot_type(
					boost::bind(&T::cancel, t.get())
				).track(t));
			return true;
		}

		template<typename T, typename B, typename F>
		static bool asio_async_read_some(RR_WEAK_PTR<RobotRaconteurNode> node, RR_SHARED_PTR<T>& t, B& b, BOOST_ASIO_MOVE_ARG(F) f)
		{
			RR_SHARED_PTR<RobotRaconteurNode> node1 = node.lock();
			if (!node1) return false;
			boost::shared_lock<boost::shared_mutex> l(node1->thread_pool_lock);
			if (node1->is_shutdown)
			{
				l.unlock();
				RR_SHARED_PTR<ThreadPool> t;
				if (!node1->TryGetThreadPool(t)) return false;
				return t->TryPost(boost::bind(f, boost::asio::error::operation_aborted, 0));
			}

			t->async_read_some(b,f);			
			return true;
		}

		template<typename T, typename B, typename F>
		static bool asio_async_write_some(RR_WEAK_PTR<RobotRaconteurNode> node, RR_SHARED_PTR<T>& t, B& b, BOOST_ASIO_MOVE_ARG(F) f)
		{
			RR_SHARED_PTR<RobotRaconteurNode> node1 = node.lock();
			if (!node1) return false;
			boost::shared_lock<boost::shared_mutex> l(node1->thread_pool_lock);
			if (!node1->thread_pool) return false;
			t->async_write_some(b, f);
			return true;
		}

		template<typename T, typename B, typename F>
		static bool asio_async_connect(RR_WEAK_PTR<RobotRaconteurNode> node, RR_SHARED_PTR<T>& t, const B& b, BOOST_ASIO_MOVE_ARG(F) f)
		{
			RR_SHARED_PTR<RobotRaconteurNode> node1 = node.lock();
			if (!node1) return false;
			boost::shared_lock<boost::shared_mutex> l(node1->thread_pool_lock);
			if (!node1->thread_pool) return false;
			t->async_connect(b, f);
			return true;
		}

		template<typename T, typename B, typename F>
		static bool asio_async_resolve(RR_WEAK_PTR<RobotRaconteurNode> node, RR_SHARED_PTR<T>& t, const B& b, BOOST_ASIO_MOVE_ARG(F) f)
		{
			RR_SHARED_PTR<RobotRaconteurNode> node1 = node.lock();
			if (!node1) return false;
			boost::shared_lock<boost::shared_mutex> l(node1->thread_pool_lock);
			if (node1->is_shutdown)
			{
				l.unlock();
				RR_SHARED_PTR<ThreadPool> t;
				if (!node1->TryGetThreadPool(t)) return false;
				boost::asio::ip::tcp::resolver::iterator iter;
				return t->TryPost(boost::bind(f, boost::asio::error::operation_aborted, iter));
			}

			t->async_resolve(b, f);
			return true;
		}

		template<typename T, typename F>
		static bool asio_async_handshake(RR_WEAK_PTR<RobotRaconteurNode> node, RR_SHARED_PTR<T>& t, BOOST_ASIO_MOVE_ARG(F) f)
		{
			RR_SHARED_PTR<RobotRaconteurNode> node1 = node.lock();
			if (!node1) return false;
			boost::shared_lock<boost::shared_mutex> l(node1->thread_pool_lock);
			if (node1->is_shutdown)
			{
				l.unlock();
				RR_SHARED_PTR<ThreadPool> t;
				if (!node1->TryGetThreadPool(t)) return false;
				boost::asio::ip::tcp::resolver::iterator iter;
				return t->TryPost(boost::bind(f, boost::asio::error::operation_aborted, iter));
			}

			t->async_handshake(f);
			return true;
		}

		template<typename T, typename U, typename F>
		static bool asio_async_handshake(RR_WEAK_PTR<RobotRaconteurNode> node, RR_SHARED_PTR<T>& t, const U& u, BOOST_ASIO_MOVE_ARG(F) f)
		{
			RR_SHARED_PTR<RobotRaconteurNode> node1 = node.lock();
			if (!node1) return false;
			boost::shared_lock<boost::shared_mutex> l(node1->thread_pool_lock);
			if (node1->is_shutdown)
			{
				l.unlock();
				RR_SHARED_PTR<ThreadPool> t;
				if (!node1->TryGetThreadPool(t)) return false;
				boost::asio::ip::tcp::resolver::iterator iter;
				return t->TryPost(boost::bind(f, boost::asio::error::operation_aborted, iter));
			}

			t->async_handshake(u, f);
			return true;
		}

		template<typename T, typename F>
		static bool asio_async_shutdown(RR_WEAK_PTR<RobotRaconteurNode> node, RR_SHARED_PTR<T>& t, BOOST_ASIO_MOVE_ARG(F) f)
		{
			RR_SHARED_PTR<RobotRaconteurNode> node1 = node.lock();
			if (!node1) return false;
			boost::shared_lock<boost::shared_mutex> l(node1->thread_pool_lock);
			if (node1->is_shutdown)
			{
				l.unlock();
				RR_SHARED_PTR<ThreadPool> t;
				if (!node1->TryGetThreadPool(t)) return false;
				boost::asio::ip::tcp::resolver::iterator iter;
				return t->TryPost(boost::bind(f, boost::asio::error::operation_aborted, iter));
			}

			t->async_shutdown(f);
			return true;
		}

	};


}
