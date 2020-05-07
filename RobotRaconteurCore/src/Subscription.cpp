// Copyright 2011-2019 Wason Technology, LLC
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

#include "Subscription_private.h"

#include <boost/foreach.hpp>
#include <boost/range/adaptors.hpp>
#include <boost/range/algorithm.hpp>
#include <boost/algorithm/string.hpp>

namespace RobotRaconteur
{
	namespace detail
	{
		ServiceSubscription_client::ServiceSubscription_client()
		{
			
		}


		ServiceSubscription_retrytimer::ServiceSubscription_retrytimer(RR_SHARED_PTR<ServiceSubscription> parent)
		{
			this->parent = parent;
			this->node = parent->node;
		}

		void ServiceSubscription_retrytimer::Start(RR_SHARED_PTR<ServiceSubscription_client> c2, uint32_t timeout)
		{
			boost::mutex::scoped_lock lock(this_lock);
			this->c2 = c2;
			RR_SHARED_PTR<ServiceSubscription> p = parent.lock();
			if (!p) return;

			RR_SHARED_PTR<RobotRaconteurNode> n = p->node.lock();
			if (!n) return;

			if (cancelled.data()) return;

			RR_SHARED_PTR<boost::asio::deadline_timer> t(new boost::asio::deadline_timer(n->GetThreadPool()->get_io_context()));
			t->expires_from_now(boost::posix_time::milliseconds(timeout));
			RobotRaconteurNode::asio_async_wait(node, t, boost::bind(&ServiceSubscription_retrytimer::timer_handler, shared_from_this(), boost::asio::placeholders::error));

			this->timer = t;

			ROBOTRACONTEUR_LOG_TRACE_SOURCE(node, Subscription, -1, "Retry timer started with " << timeout << " ms timeout");

		}
		void ServiceSubscription_retrytimer::Cancel()
		{
			boost::mutex::scoped_lock lock(this_lock);
			cancelled.data() = true;
			timer.reset();

			ROBOTRACONTEUR_LOG_TRACE_SOURCE(node, Subscription, -1, "Retry timer cancelled");
		}
		int64_t ServiceSubscription_retrytimer::MillisecondsRemaining()
		{
			boost::mutex::scoped_lock lock(this_lock);
			if (!timer) return 0;
			return timer->expires_from_now().total_milliseconds();
		}

		void ServiceSubscription_retrytimer::timer_handler(RR_WEAK_PTR<ServiceSubscription_retrytimer> this_, const boost::system::error_code& ec)
		{
			if (ec) return;

			RR_SHARED_PTR<ServiceSubscription_retrytimer> this1 = this_.lock();
			if (!this1) return;

			{
				boost::mutex::scoped_lock lock(this1->this_lock);
				if (this1->cancelled) 
					return;
				this1->cancelled.data() = true;
			}

			RR_SHARED_PTR<ServiceSubscription> p =this1->parent.lock();
			if (!p) return;
			RR_SHARED_PTR<ServiceSubscription_client> c2 = this1->c2.lock();
			if (!c2) return;

			ROBOTRACONTEUR_LOG_TRACE_SOURCE(p->node, Subscription, -1, "Retry firing");
			p->ConnectRetry2(c2);
		}
	}
	static void ServiceSubscription_close_handler()
	{

	}

	ServiceSubscriptionClientID::ServiceSubscriptionClientID(const ::RobotRaconteur::NodeID& nodeid, boost::string_ref service_name)
	{
		this->NodeID = nodeid;
		this->ServiceName = RR_MOVE(service_name.to_string());
	}

	ServiceSubscriptionClientID::ServiceSubscriptionClientID()
	{

	}

	bool ServiceSubscriptionClientID::operator == (const ServiceSubscriptionClientID &id2) const
	{
		return boost::tie(this->NodeID, this->ServiceName) == boost::tie(id2.NodeID, id2.ServiceName);
	}

	bool ServiceSubscriptionClientID::operator != (const ServiceSubscriptionClientID &id2) const
	{
		return boost::tie(this->NodeID, this->ServiceName) != boost::tie(id2.NodeID, id2.ServiceName);
	}

	bool ServiceSubscriptionClientID::operator <(const ServiceSubscriptionClientID& id2) const
	{
		return boost::tie(this->NodeID, this->ServiceName) < boost::tie(id2.NodeID, id2.ServiceName);
	}

	static bool ServiceSubscription_FilterService(const std::vector<std::string>& service_types, RR_SHARED_PTR<ServiceSubscriptionFilter>& filter, RR_SHARED_PTR<detail::Discovery_nodestorage>& storage, const ServiceInfo2& info, std::vector<std::string>& urls, std::string& client_service_type, RR_SHARED_PTR<ServiceSubscriptionFilterNode>& filter_node)
	{
		if (!service_types.empty() && boost::range::find(service_types, info.RootObjectType) == service_types.end())
		{
			bool implements_match = false;
			BOOST_FOREACH(const std::string& implements, info.RootObjectImplements)
			{
				if (boost::range::find(service_types, implements) != service_types.end())
				{
					implements_match = true;
					client_service_type = implements;
					break;
				}
			}
			if (!implements_match) return false;
		}
		else
		{
			client_service_type = info.RootObjectType;
		}

		if (filter)
		{
			if (!filter->Nodes.empty())
			{

				BOOST_FOREACH(RR_SHARED_PTR<ServiceSubscriptionFilterNode>& f1, filter->Nodes)
				{
					if (f1->NodeID.IsAnyNode() && f1->NodeName.empty())
					{
						//Wildcard match, most likely an error...

						filter_node = f1;
						break;
					}

					if (!f1->NodeID.IsAnyNode() && !f1->NodeName.empty())
					{
						if (f1->NodeName == info.NodeName && f1->NodeID == info.NodeID)
						{
							filter_node = f1;
							break;
						}
					}

					if (f1->NodeID.IsAnyNode() && !f1->NodeName.empty())
					{
						if (f1->NodeName == info.NodeName)
						{
							filter_node = f1;
							break;
						}
					}

					if (!f1->NodeID.IsAnyNode() && f1->NodeName.empty())
					{
						if (f1->NodeID == info.NodeID)
						{
							filter_node = f1;
							break;
						}
					}
				}

				if (!filter_node)
				{
					return false;
				}

			}

			if (filter->TransportSchemes.empty())
			{
				boost::copy(info.ConnectionURL, std::back_inserter(urls));
			}
			else
			{
				BOOST_FOREACH(const std::string& url1, info.ConnectionURL)
				{
					BOOST_FOREACH(std::string& scheme1, filter->TransportSchemes)
					{
						if (boost::starts_with(url1, scheme1 + "://"))
						{
							urls.push_back(url1);
						}
					}
				}

				if (urls.empty())
				{
					//We didn't find a match with the ServiceInfo2 urls, attempt to use NodeDiscoveryInfo
					//TODO: test this....

					BOOST_FOREACH(const NodeDiscoveryInfoURL& url2, storage->info->URLs)
					{
						boost::string_ref url1 = url2.URL;
						BOOST_FOREACH(const std::string& scheme1, filter->TransportSchemes)
						{
							if (boost::starts_with(url1, scheme1 + "://"))
							{
								urls.push_back(boost::replace_last_copy(url1.to_string(), "RobotRaconteurServiceIndex", info.Name));
							}
						}
					}
				}
			}

			if (!filter->ServiceNames.empty())
			{
				if (boost::range::find(filter->ServiceNames, info.Name) == filter->ServiceNames.end())
				{
					return false;
				}
			}

			if (filter->Predicate)
			{
				if (!(filter->Predicate(info))) return false;
			}
		}
		else
		{
			urls = info.ConnectionURL;
		}

		return true;
	}

	//ServiceInfo2Subscription
	std::map<ServiceSubscriptionClientID, ServiceInfo2 > ServiceInfo2Subscription::GetDetectedServiceInfo2()
	{
		std::map<ServiceSubscriptionClientID, ServiceInfo2 > o;
		boost::mutex::scoped_lock lock(this_lock);

		BOOST_FOREACH(RR_SHARED_PTR<detail::ServiceInfo2Subscription_client>& c, clients | boost::adaptors::map_values)
		{			
			o.insert(std::make_pair(ServiceSubscriptionClientID(c->nodeid, c->service_name), c->service_info2));
		}

		return o;
	}

	ServiceInfo2Subscription::event_connection ServiceInfo2Subscription::AddServiceDetectedListener(boost::function<void(RR_SHARED_PTR<ServiceInfo2Subscription>, const ServiceSubscriptionClientID&, const ServiceInfo2&)> handler)
	{
		return detected_listeners.connect(handler);
	}

	ServiceInfo2Subscription::event_connection ServiceInfo2Subscription::AddServiceLostListener(boost::function<void(RR_SHARED_PTR<ServiceInfo2Subscription>, const ServiceSubscriptionClientID&, const ServiceInfo2&)> handler)
	{
		return lost_listeners.connect(handler);
	}

	void ServiceInfo2Subscription::Close()
	{
		boost::mutex::scoped_lock lock(this_lock);
		if (!active) return;
		active = false;

		clients.clear();

		detected_listeners.disconnect_all_slots();
		lost_listeners.disconnect_all_slots();

		RR_SHARED_PTR<detail::Discovery> d = parent.lock();
		if (!d) return;

		lock.unlock();

		d->SubscriptionClosed(shared_from_this());

		ROBOTRACONTEUR_LOG_TRACE_SOURCE(node, Subscription, -1, "ServiceInfo2Subscription closed");
	}
		
	ServiceInfo2Subscription::ServiceInfo2Subscription(RR_SHARED_PTR<detail::Discovery> parent)
	{
		this->parent = parent;
		RR_SHARED_PTR<RobotRaconteurNode> n = parent->GetNode();
		this->node = n;
	}

	void ServiceInfo2Subscription::Init(const std::vector<std::string>& service_types, RR_SHARED_PTR<ServiceSubscriptionFilter> filter)
	{
		this->active = true;
		this->service_types = service_types;
		this->filter = filter;

		ROBOTRACONTEUR_LOG_TRACE_SOURCE(node, Subscription, -1, "ServiceInfo2Subscription initialized for service types: " << boost::join(service_types,", "));
	}

	void ServiceInfo2Subscription::NodeUpdated(RR_SHARED_PTR<detail::Discovery_nodestorage> storage)
	{
		boost::mutex::scoped_lock lock(this_lock);
		if (!active) return;
		if (!storage) return;
		if (!storage->services) return;
		if (!storage->info) return;

		RR_SHARED_PTR<RobotRaconteurNode> n = node.lock();
		if (!n) return;

		ROBOTRACONTEUR_LOG_TRACE_SOURCE(node, Subscription, -1, "ServiceInfo2Subscription received node update for " << storage->info->NodeID.ToString());

		BOOST_FOREACH(ServiceInfo2& info, *storage->services)
		{
			ServiceSubscriptionClientID k(storage->info->NodeID, info.Name);
			std::map<ServiceSubscriptionClientID, RR_SHARED_PTR<detail::ServiceInfo2Subscription_client> >::iterator e = clients.find(k);

			if (e != clients.end())
			{
				//If we are already dected, ignore
								
				ServiceInfo2& info2 = e->second->service_info2;

				if (info2.NodeName != info.NodeName
					|| info2.Name != info.Name
					|| info2.RootObjectType != info.RootObjectType
					|| info2.ConnectionURL != info.ConnectionURL
					|| info2.RootObjectImplements != info.RootObjectImplements)
				{
					ROBOTRACONTEUR_LOG_TRACE_SOURCE(node, Subscription, -1, "ServiceInfo2Subscription new service named \"" << info.Name << "\" detected on node " 
						<< info.NodeID.ToString());
					e->second->service_info2 = info;
					RobotRaconteurNode::TryPostToThreadPool(node, boost::bind(&ServiceInfo2Subscription::fire_ServiceDetectedListener, shared_from_this(), k, info));
				}

				e->second->last_node_update = n->NowUTC();

				return;
			}

			std::vector<std::string> urls;
			std::string client_service_type;
			RR_SHARED_PTR<ServiceSubscriptionFilterNode> filter_node;

			if (!ServiceSubscription_FilterService(service_types, filter, storage, info, urls, client_service_type, filter_node))
			{
				ROBOTRACONTEUR_LOG_TRACE_SOURCE(node, Subscription, -1, "ServiceInfo2Subscription ignoring service named \"" << info.Name << "\" on node " 
						<< info.NodeID.ToString() << " due to filter match failure");
				continue;
			}

			if (e == clients.end())
			{
				RR_SHARED_PTR<detail::ServiceInfo2Subscription_client> c2 = RR_MAKE_SHARED<detail::ServiceInfo2Subscription_client>();
				c2->nodeid = info.NodeID;				
				c2->service_name = info.Name;
				c2->service_info2 = info;
				c2->last_node_update = n->NowUTC();

				ServiceSubscriptionClientID noden(c2->nodeid, c2->service_name);

				clients.insert(std::make_pair(noden, c2));

				ROBOTRACONTEUR_LOG_TRACE_SOURCE(node, Subscription, -1, "ServiceInfo2Subscription adding new service named \"" << info.Name << "\" on node " 
						<< info.NodeID.ToString());

				RobotRaconteurNode::TryPostToThreadPool(node, boost::bind(&ServiceInfo2Subscription::fire_ServiceDetectedListener, shared_from_this(), noden, c2->service_info2));
			}			
		}

		for (std::map<ServiceSubscriptionClientID, RR_SHARED_PTR<detail::ServiceInfo2Subscription_client> >::iterator e = clients.begin(); e != clients.end();)
		{
			if (e->first.NodeID == storage->info->NodeID)
			{
				bool found = false;
				BOOST_FOREACH(ServiceInfo2& info, *storage->services)
				{
					if (info.Name == e->first.ServiceName)
					{
						found = true;
						break;
					}
				}
				if (!found)
				{
					ServiceInfo2 info1 = e->second->service_info2;
					ServiceSubscriptionClientID id1 = e->first;
					
					std::map<ServiceSubscriptionClientID, RR_SHARED_PTR<detail::ServiceInfo2Subscription_client> >::iterator e1 = e;
					e++;
					clients.erase(e1);

					ROBOTRACONTEUR_LOG_TRACE_SOURCE(node, Subscription, -1, "ServiceInfo2Subscription removing service named \"" << info1.Name << "\" on node " 
						<< info1.NodeID.ToString() << " due to service no longer advertising");

					RobotRaconteurNode::TryPostToThreadPool(node, boost::bind(&ServiceInfo2Subscription::fire_ServiceDetectedListener, shared_from_this(), id1, info1));
				}
				else
				{
					e++;
				}
			}
			else
			{
				e++;
			}
		}
	}

	void ServiceInfo2Subscription::NodeLost(RR_SHARED_PTR<detail::Discovery_nodestorage> storage)
	{
		boost::mutex::scoped_lock lock(this_lock);

		if (!storage) return;
		if (!storage->info) return;

		NodeID id1 = storage->info->NodeID;

		ROBOTRACONTEUR_LOG_TRACE_SOURCE(node, Subscription, -1, "ServiceInfo2Subscription received node lost for " << storage->info->NodeID.ToString());

		for (std::map<ServiceSubscriptionClientID, RR_SHARED_PTR<detail::ServiceInfo2Subscription_client> >::iterator e = clients.begin(); e != clients.end();)
		{
			if (e->first.NodeID == storage->info->NodeID)
			{				
				ServiceInfo2 info1 = e->second->service_info2;
				ServiceSubscriptionClientID id1 = e->first;

				std::map<ServiceSubscriptionClientID, RR_SHARED_PTR<detail::ServiceInfo2Subscription_client> >::iterator e1 = e;
				e++;
				clients.erase(e1);

				ROBOTRACONTEUR_LOG_TRACE_SOURCE(node, Subscription, -1, "ServiceInfo2Subscription removing service named \"" << info1.Name << "\" on node " 
						<< info1.NodeID.ToString() << " due to node lost");

				RobotRaconteurNode::TryPostToThreadPool(node, boost::bind(&ServiceInfo2Subscription::fire_ServiceLostListener, shared_from_this(), id1, info1));				
			}
			else
			{
				e++;
			}
		}
	}

	void ServiceInfo2Subscription::fire_ServiceDetectedListener(const ServiceSubscriptionClientID& noden, const ServiceInfo2& info)
	{
		detected_listeners(shared_from_this(), noden, info);
	}

	void ServiceInfo2Subscription::fire_ServiceLostListener(const ServiceSubscriptionClientID& noden, const ServiceInfo2& info)
	{
		lost_listeners(shared_from_this(), noden, info);
	}


	//ServiceSubscription
	std::map<ServiceSubscriptionClientID, RR_SHARED_PTR<RRObject> > ServiceSubscription::GetConnectedClients()
	{
	
		std::map<ServiceSubscriptionClientID, RR_SHARED_PTR<RRObject> > o;
		boost::mutex::scoped_lock lock(this_lock);

		BOOST_FOREACH(RR_SHARED_PTR<detail::ServiceSubscription_client>& c, clients | boost::adaptors::map_values)
		{
			RR_SHARED_PTR<RRObject> c2 = c->client.lock();
			if (!c2) continue;
			o.insert(std::make_pair(ServiceSubscriptionClientID(c->nodeid, c->service_name), c2));
		}

		return o;
	}

	ServiceSubscription::event_connection ServiceSubscription::AddClientConnectListener(boost::function<void(RR_SHARED_PTR<ServiceSubscription>, const ServiceSubscriptionClientID&, RR_SHARED_PTR<RRObject>)> handler)
	{
		return connect_listeners.connect(handler);
	}
	ServiceSubscription::event_connection ServiceSubscription::AddClientDisconnectListener(boost::function<void(RR_SHARED_PTR<ServiceSubscription>, const ServiceSubscriptionClientID&, RR_SHARED_PTR<RRObject>)> handler)
	{
		return disconnect_listeners.connect(handler);
	}

	void ServiceSubscription::Close()
	{
		boost::mutex::scoped_lock lock(this_lock);
		if (!active) return;
		active = false;
		
		RR_SHARED_PTR<RobotRaconteurNode> n = node.lock();
		if (n)
		{
			BOOST_FOREACH(RR_SHARED_PTR<WireSubscriptionBase> w, wire_subscriptions)
			{
				RobotRaconteurNode::TryPostToThreadPool(n, boost::bind(&WireSubscriptionBase::Close, w), true);
			}

			BOOST_FOREACH(RR_SHARED_PTR<PipeSubscriptionBase> p, pipe_subscriptions)
			{
				RobotRaconteurNode::TryPostToThreadPool(n, boost::bind(&PipeSubscriptionBase::Close, p), true);
			}

			BOOST_FOREACH(RR_SHARED_PTR<detail::ServiceSubscription_client>& c, clients | boost::adaptors::map_values)
			{
				c->claimed.data() = false;
				RR_SHARED_PTR<RRObject> c2 = c->client.lock();
				if (!c2) continue;
				try
				{
					n->AsyncDisconnectService(c2, ServiceSubscription_close_handler);
				}
				catch (std::exception&) {}
			}
		}

		wire_subscriptions.clear();
		clients.clear();

		connect_listeners.disconnect_all_slots();
		disconnect_listeners.disconnect_all_slots();

		RR_SHARED_PTR<detail::Discovery> d = parent.lock();
		if (!d) return;
			
		lock.unlock();

		d->SubscriptionClosed(shared_from_this());

		ROBOTRACONTEUR_LOG_TRACE_SOURCE(node, Subscription, -1, "ServiceSubscription closed");
	}

	static RR_SHARED_PTR<detail::ServiceSubscription_client> SeviceSubscription_FindClient(std::map<ServiceSubscriptionClientID, RR_SHARED_PTR<detail::ServiceSubscription_client> >& clients, RR_SHARED_PTR<RRObject> client)
	{
		RR_SHARED_PTR<ServiceStub> s = RR_DYNAMIC_POINTER_CAST<ServiceStub>(client);
		if (!client)
			throw InvalidArgumentException("Can only lock object opened through Robot Raconteur");
		
		RR_SHARED_PTR<ClientContext> c = s->GetContext();
		
		typedef std::map<ServiceSubscriptionClientID, RR_SHARED_PTR<detail::ServiceSubscription_client> >::const_iterator e_type;
		NodeID target_nodeid = c->GetRemoteNodeID();
		std::string target_servicename = c->GetServiceName();
		ServiceSubscriptionClientID target_subid(target_nodeid, target_servicename);

		e_type e = clients.find(target_subid);
		if (e != clients.end())
		{
			return e->second;
		}

		BOOST_FOREACH(RR_SHARED_PTR<detail::ServiceSubscription_client>& ee, clients | boost::adaptors::map_values)
		{
			RR_SHARED_PTR<RRObject> o = ee->client.lock();
			if (o == client)
			{
				return ee;
			}
		}

		return RR_SHARED_PTR<detail::ServiceSubscription_client>();

	}

	void ServiceSubscription::ClaimClient(RR_SHARED_PTR<RRObject> client)
	{
		try
		{
			boost::mutex::scoped_lock lock(this_lock);
			if (!active) throw InvalidOperationException("Service closed");

			RR_SHARED_PTR<detail::ServiceSubscription_client> sub =
				SeviceSubscription_FindClient(clients, client);

			if (!sub) throw InvalidArgumentException("Invalid client for ClaimClient");

			sub->claimed.data() = true;
			ROBOTRACONTEUR_LOG_TRACE_SOURCE(node, Subscription, -1, "ServiceSubscription claimed client " << sub->nodeid.ToString() << "," << sub->service_name);
		}
		catch (std::exception& exp)
		{
			ROBOTRACONTEUR_LOG_DEBUG_SOURCE(node, Subscription, -1, "ServiceSubscription ClaimClient failed: " << exp.what());
			throw;
		}
	}

	void ServiceSubscription::ReleaseClient(RR_SHARED_PTR<RRObject> client)
	{
		try
		{
			boost::mutex::scoped_lock lock(this_lock);
			if (!active)
			{
				RR_SHARED_PTR<RobotRaconteurNode> n = node.lock();
				if (!n) return;
				try
				{
					n->AsyncDisconnectService(client, &ServiceSubscription_close_handler);
				}
				catch (std::runtime_error&) {}
			}

			RR_SHARED_PTR<detail::ServiceSubscription_client> sub =
				SeviceSubscription_FindClient(clients, client);

			if (!sub) return;

			sub->claimed.data() = false;
			ROBOTRACONTEUR_LOG_TRACE_SOURCE(node, Subscription, -1, "ServiceSubscription released claimed client " << sub->nodeid.ToString() << "," << sub->service_name);
		}
		catch (std::exception& exp)
		{
			ROBOTRACONTEUR_LOG_DEBUG_SOURCE(node, Subscription, -1, "ServiceSubscription ReleaseClient failed: " << exp.what());
			throw;
		}
	}

	ServiceSubscription::ServiceSubscription(RR_SHARED_PTR<detail::Discovery> parent)
	{
		this->parent = parent;
		RR_SHARED_PTR<RobotRaconteurNode> n= parent->GetNode();
		this->retry_delay = 15000;

		this->node = n;

		listener_strand.reset(RR_BOOST_ASIO_NEW_STRAND(n->GetThreadPool()->get_io_context()));
	}

	void ServiceSubscription::Init(const std::vector<std::string>& service_types, RR_SHARED_PTR<ServiceSubscriptionFilter> filter)
	{
		this->active = true;
		this->service_types = service_types;
		this->filter = filter;

		ROBOTRACONTEUR_LOG_TRACE_SOURCE(node, Subscription, -1, "ServiceSubscription initialized for service types: " << boost::join(service_types,", "));
	}

	void ServiceSubscription::NodeUpdated(RR_SHARED_PTR<detail::Discovery_nodestorage> storage)
	{
		boost::mutex::scoped_lock lock(this_lock);
		if (!active) return;
		if (!storage) return;
		if (!storage->services) return;
		if (!storage->info) return;

		RR_SHARED_PTR<RobotRaconteurNode> n = node.lock();
		if (!n) return;

		ROBOTRACONTEUR_LOG_TRACE_SOURCE(node, Subscription, -1, "ServiceSubscription received node update for " << storage->info->NodeID.ToString());
		
		BOOST_FOREACH(ServiceInfo2& info, *storage->services)
		{
			ServiceSubscriptionClientID k(storage->info->NodeID, info.Name);
			std::map<ServiceSubscriptionClientID, RR_SHARED_PTR<detail::ServiceSubscription_client> >::iterator e = clients.find(k);

			if (e != clients.end())
			{
				//If we are already connected, ignore
				if (e->second->client.lock()) return;
			}

			
			std::vector<std::string> urls;
			std::string client_service_type;
			RR_SHARED_PTR<ServiceSubscriptionFilterNode> filter_node;

			if (!ServiceSubscription_FilterService(service_types, filter, storage, info, urls, client_service_type, filter_node))
			{
				ROBOTRACONTEUR_LOG_TRACE_SOURCE(node, Subscription, -1, "ServiceSubscription ignoring service named \"" << info.Name << "\" on node " 
						<< info.NodeID.ToString() << " due to filter match failure");
				continue;
			}
						
			if (e == clients.end())
			{
				RR_SHARED_PTR<detail::ServiceSubscription_client> c2 = RR_MAKE_SHARED<detail::ServiceSubscription_client>();
				c2->nodeid = info.NodeID;
				c2->nodename = info.NodeName;
				c2->service_name = info.Name;
				c2->connecting.data() = true;
				c2->service_type = client_service_type;
				c2->urls = urls;
				c2->last_node_update = n->NowUTC();

								
				if (filter_node && !filter_node->Username.empty() && filter_node->Credentials)
				{
					c2->username = filter_node->Username;
					c2->credentials = filter_node->Credentials;
				}
							
				try
				{
					ROBOTRACONTEUR_LOG_TRACE_SOURCE(node, Subscription, -1, "ServiceSubscription begin connect to service named \"" << info.Name << "\" on node " 
						<< info.NodeID.ToString());
					RR_WEAK_PTR<ServiceSubscription> weak_this = shared_from_this();
					n->AsyncConnectService(urls, c2->username, c2->credentials,
						boost::bind(&ServiceSubscription::ClientEvent, weak_this ,_1, _2, _3, c2),
						client_service_type,
						boost::bind(&ServiceSubscription::ClientConnected, shared_from_this(), _1, _2, c2),
						n->GetRequestTimeout() * 2);
					clients.insert(std::make_pair(ServiceSubscriptionClientID(c2->nodeid, c2->service_name), c2));
					continue;
				}
				catch (std::exception& exp2)
				{
					ROBOTRACONTEUR_LOG_DEBUG_SOURCE(node, Subscription, -1, "ServiceSubscription connect to service named \"" << info.Name << "\" on node " 
						<< info.NodeID.ToString() << " failed: " << exp2.what());
				}

				ConnectRetry(c2);

			}
			else
			{	
				
				RR_SHARED_PTR<detail::ServiceSubscription_client>& c2 = e->second;
				c2->urls = urls;
				c2->last_node_update = n->NowUTC();
								
				if (c2->retry_timer)
				{
					RR_SHARED_PTR<detail::ServiceSubscription_retrytimer> t = c2->retry_timer;
					if (t->MillisecondsRemaining() > 500)
					{
						c2->retry_timer.reset();
						t->Cancel();
						RR_SHARED_PTR<detail::ServiceSubscription_retrytimer> t2 = RR_MAKE_SHARED<detail::ServiceSubscription_retrytimer>(shared_from_this());
						c2->retry_timer = t2;

						uint32_t backoff = n->GetRandomInt<uint32_t>(100, 600);
						ROBOTRACONTEUR_LOG_TRACE_SOURCE(node, Subscription, -1, "ServiceSubscription begin backoff timer to retry connect service named \"" << info.Name << "\" on node " 
							<< info.NodeID.ToString());
						t2->Start(c2, backoff);
					}

				}
				
			}
		}

	}

	void ServiceSubscription::NodeLost(RR_SHARED_PTR<detail::Discovery_nodestorage> storage)
	{
		ROBOTRACONTEUR_LOG_TRACE_SOURCE(node, Subscription, -1, "ServiceSubscription received node lost for " << storage->info->NodeID.ToString());
	}

	void ServiceSubscription::ClientConnected(RR_SHARED_PTR<RRObject> c, RR_SHARED_PTR<RobotRaconteurException> err, RR_SHARED_PTR<detail::ServiceSubscription_client> c2)
	{
		RR_SHARED_PTR<RobotRaconteurNode> n = node.lock();
		if (!n) return;

		boost::mutex::scoped_lock lock(this_lock);
		
		if (err)
		{
			ROBOTRACONTEUR_LOG_DEBUG_SOURCE(node, Subscription, -1, "ServiceSubscription connecting to service named \"" << c2->service_name << "\" on node " 
						<< c2->nodeid.ToString() << "failed: " << err->what());
			ConnectRetry(c2);
			return;
		}

		ROBOTRACONTEUR_LOG_TRACE_SOURCE(node, Subscription, -1, "ServiceSubscription connected to service named \"" << c2->service_name << "\" on node " 
						<< c2->nodeid.ToString());

		c2->connecting.data() = false;
		c2->client = c;
		c2->error_count.data() = 0;

		if (n)
		{
			try
			{
				RobotRaconteurNode::TryPostToThreadPool(n, RR_BOOST_ASIO_STRAND_WRAP(*listener_strand,
					boost::bind(&ServiceSubscription::fire_ClientConnectListeners,
						shared_from_this(), ServiceSubscriptionClientID(c2->nodeid, c2->service_name), c)));
			}
			catch (std::exception&) {}
		}

		BOOST_FOREACH(RR_SHARED_PTR<WireSubscriptionBase> w, wire_subscriptions)
		{
			w->ClientConnected(c);
		}

		BOOST_FOREACH(RR_SHARED_PTR<PipeSubscriptionBase> p, pipe_subscriptions)
		{
			p->ClientConnected(c);
		}

	}

	void ServiceSubscription::ConnectRetry(RR_SHARED_PTR<detail::ServiceSubscription_client> c2)
	{
		RR_SHARED_PTR<RobotRaconteurNode> n = node.lock();
		if (!n) return;

		ROBOTRACONTEUR_LOG_TRACE_SOURCE(node, Subscription, -1, "ServiceSubscription begin retry timer config for service \"" << c2->service_name << "\" on node " 
						<< c2->nodeid.ToString() << " with error count " << c2->error_count);
		if (c2->error_count > 25)
		{
			ROBOTRACONTEUR_LOG_DEBUG_SOURCE(node, Subscription, -1, "ServiceSubscription for service \"" << c2->service_name << "\" on node " 
						<< c2->nodeid.ToString() << " with error count " << c2->error_count << " aborting retry due to too many errors");
			//Too many errors, give up
			c2->connecting.data() = false;
			return;
		}
		c2->error_count++;

		ROBOTRACONTEUR_LOG_TRACE_SOURCE(node, Subscription, -1, "ServiceSubscription starting retry timer for service \"" << c2->service_name << "\" on node " 
						<< c2->nodeid.ToString() << " with " << retry_delay << " ms delay");

		RR_SHARED_PTR<detail::ServiceSubscription_retrytimer> t = RR_MAKE_SHARED<detail::ServiceSubscription_retrytimer>(shared_from_this());
		c2->retry_timer = t;
		t->Start(c2, retry_delay);
		
	}

	void ServiceSubscription::ConnectRetry2(RR_SHARED_PTR<detail::ServiceSubscription_client> c2)
	{
		boost::mutex::scoped_lock lock(this_lock);
		
		c2->retry_timer.reset();

		RR_SHARED_PTR<RobotRaconteurNode> n = node.lock();
		if (!n) return;
		
		if (!active) return;

		if (n->NowUTC() > (c2->last_node_update + boost::posix_time::seconds(65)))
		{
			ROBOTRACONTEUR_LOG_TRACE_SOURCE(node, Subscription, -1, "ServiceSubscription retry for service \"" << c2->service_name << "\" on node " 
						<< c2->nodeid.ToString() << " aborted due to node lost");
			ServiceSubscriptionClientID k(c2->nodeid, c2->service_name);
			clients.erase(k);
			return;
		}
		try
		{
			ROBOTRACONTEUR_LOG_TRACE_SOURCE(node, Subscription, -1, "ServiceSubscription begin retry connect to service named \"" << c2->service_name << "\" on node " 
						<< c2->nodeid.ToString());
			RR_WEAK_PTR<ServiceSubscription> weak_this = shared_from_this();
			n->AsyncConnectService(c2->urls, c2->username, c2->credentials,
				boost::bind(&ServiceSubscription::ClientEvent, weak_this, _1, _2, _3, c2),
				c2->service_type,
				boost::bind(&ServiceSubscription::ClientConnected, shared_from_this(), _1, _2, c2),
				n->GetRequestTimeout() * 2);
			return;
		}
		catch (std::exception& exp)
		{
			ROBOTRACONTEUR_LOG_TRACE_SOURCE(node, Subscription, -1, "ServiceSubscription retry connect to service named \"" << c2->service_name << "\" on node " 
					<< c2->nodeid.ToString() << " failed: " << exp.what());
		}
				
		ConnectRetry(c2);

	}

	void ServiceSubscription::ClientEvent(RR_WEAK_PTR<ServiceSubscription> this_, RR_SHARED_PTR<ClientContext> ctx, ClientServiceListenerEventType evt, RR_SHARED_PTR<void> p, RR_WEAK_PTR<detail::ServiceSubscription_client> c2)
	{
		
		RR_SHARED_PTR<ServiceSubscription> this1 = this_.lock();
		if (!this1) return;
		RR_SHARED_PTR<detail::ServiceSubscription_client> c2_1 = c2.lock();
		RR_SHARED_PTR<RobotRaconteurNode> n = this1->node.lock();
		try
		{
			if (!this1 || !c2_1) return;

			boost::mutex::scoped_lock lock(this1->this_lock);

			if (evt == ClientServiceListenerEventType_ClientClosed
				|| evt == ClientServiceListenerEventType_ClientConnectionTimeout
				|| evt == ClientServiceListenerEventType_TransportConnectionClosed)
			{				
				RR_SHARED_PTR<RRObject> client = c2_1->client.lock();
				if (client)
				{
					ROBOTRACONTEUR_LOG_TRACE_SOURCE(n, Subscription, -1, "ServiceSubscription connection to service named \"" << c2_1->service_name << "\" on node " 
						<< c2_1->nodeid.ToString() << " lost");

					if (rr_cast<ServiceStub>(client)->GetContext() == ctx)
					{
						c2_1->client.reset();
						this1->ConnectRetry(c2_1);

						if (c2_1->claimed.data())
						{
							c2_1->claimed.data() = false;
						}
						else
						{
							try
							{
								if (n)
								{
									n->AsyncDisconnectService(client, &ServiceSubscription_close_handler);
								}
							}
							catch (std::exception&) {}
						}

						if (n)
						{
							try
							{
								RobotRaconteurNode::TryPostToThreadPool(n, RR_BOOST_ASIO_STRAND_WRAP(*this1->listener_strand,
									boost::bind(&ServiceSubscription::fire_ClientDisconnectListeners,
										this1, ServiceSubscriptionClientID(c2_1->nodeid, c2_1->service_name), client)));
							}
							catch (std::exception&) {}
						}
						
					}
				}
				else
				{
					c2_1->claimed.data() = false;
				}
			}

		}
		catch (std::exception& exp)
		{
			ROBOTRACONTEUR_LOG_TRACE_SOURCE(n, Subscription, -1, "ServiceSubscription processing event for service named \"" << c2_1->service_name << "\" on node " 
					<< c2_1->nodeid.ToString() << " failed: " << exp.what());
		}
	}

	uint32_t ServiceSubscription::GetConnectRetryDelay()
	{
		boost::mutex::scoped_lock lock(this_lock);
		return retry_delay;
	}

	void ServiceSubscription::SetConnectRetryDelay(uint32_t delay_milliseconds)
	{
		boost::mutex::scoped_lock lock(this_lock);
		retry_delay = delay_milliseconds;
		ROBOTRACONTEUR_LOG_TRACE_SOURCE(node, Subscription, -1, "ConnectRetryDelay set to " << delay_milliseconds << " ms");

	}

	void ServiceSubscription::fire_ClientConnectListeners(const ServiceSubscriptionClientID& noden, RR_SHARED_PTR<RRObject> client)
	{
		connect_listeners(shared_from_this(), noden, client);
	}
	void ServiceSubscription::fire_ClientDisconnectListeners(const ServiceSubscriptionClientID& noden, RR_SHARED_PTR<RRObject> client)
	{
		disconnect_listeners(shared_from_this(), noden, client);
	}

	void ServiceSubscription::SubscribeWire1(RR_SHARED_PTR<WireSubscriptionBase> s)
	{

		ROBOTRACONTEUR_LOG_TRACE_SOURCE_PATH(node, Subscription, -1, "", s->membername, "ServiceSubscription SubscribeWire");

		boost::mutex::scoped_lock lock(this_lock);
		wire_subscriptions.insert(s);

		BOOST_FOREACH(RR_SHARED_PTR<detail::ServiceSubscription_client> client, clients | boost::adaptors::map_values)
		{
			RR_SHARED_PTR<RRObject> client1 = client->client.lock();
			if (client1)
			{
				s->ClientConnected(client1);
			}
		}

	}

	void ServiceSubscription::WireSubscriptionClosed(RR_SHARED_PTR<WireSubscriptionBase> s)
	{
		ROBOTRACONTEUR_LOG_TRACE_SOURCE_PATH(node, Subscription, -1, "", s->membername, "ServiceSubscription wire subscription closed");
		{
			boost::mutex::scoped_lock lock(this_lock);
			wire_subscriptions.erase(s);
		}
	}

	void ServiceSubscription::SubscribePipe1(RR_SHARED_PTR<PipeSubscriptionBase> s)
	{
		ROBOTRACONTEUR_LOG_TRACE_SOURCE_PATH(node, Subscription, -1, "", s->membername, "ServiceSubscription SubscribePipe");
		boost::mutex::scoped_lock lock(this_lock);
		pipe_subscriptions.insert(s);

		BOOST_FOREACH(RR_SHARED_PTR<detail::ServiceSubscription_client> client, clients | boost::adaptors::map_values)
		{
			RR_SHARED_PTR<RRObject> client1 = client->client.lock();
			if (client1)
			{
				s->ClientConnected(client1);
			}
		}

	}

	void ServiceSubscription::PipeSubscriptionClosed(RR_SHARED_PTR<PipeSubscriptionBase> s)
	{
		ROBOTRACONTEUR_LOG_TRACE_SOURCE_PATH(node, Subscription, -1, "", s->membername, "ServiceSubscription pipe subscription closed");
		{
			boost::mutex::scoped_lock lock(this_lock);
			pipe_subscriptions.erase(s);
		}
	}

	//class WireSubscriptionBase

	static void WireSubscriptionBase_emptyhandler(RR_SHARED_PTR<RobotRaconteurException> e)
	{

	}

	RR_INTRUSIVE_PTR<RRValue> WireSubscriptionBase::GetInValueBase(TimeSpec* time, RR_SHARED_PTR<WireConnectionBase>* connection)
	{
		RR_INTRUSIVE_PTR<RRValue> o;
		if (!TryGetInValueBase(o, time, connection))
		{
			throw ValueNotSetException("In value not valid");
		}
		return o;
	}
	bool WireSubscriptionBase::TryGetInValueBase(RR_INTRUSIVE_PTR<RRValue>& val, TimeSpec* time, RR_SHARED_PTR<WireConnectionBase>* connection)
	{
		boost::mutex::scoped_lock lock(this_lock);
		if (!in_value_valid) return false;

		val = in_value;
		if (time)
		{
			*time = in_value_time;
		}

		if (connection)
		{
			*connection = in_value_connection;
		}

		return true;
	}

	bool WireSubscriptionBase::WaitInValueValid(int32_t timeout)
	{
		boost::mutex::scoped_lock lock(this_lock);
		if (in_value_valid) return true;
		if (closed) return false;
		if (timeout == 0) return in_value_valid;
		if (timeout < 0)
		{
			in_value_wait.wait(lock);
		}
		else
		{
			in_value_wait.wait_for(lock, boost::chrono::milliseconds(timeout));
		}

		return in_value_valid;
	}

	bool WireSubscriptionBase::GetIgnoreInValue()
	{
		boost::mutex::scoped_lock lock(this_lock);
		return ignore_in_value.data();
	}
	void WireSubscriptionBase::SetIgnoreInValue(bool ignore)
	{
		boost::mutex::scoped_lock lock(this_lock);
		ignore_in_value.data() = ignore;

		BOOST_FOREACH(RR_SHARED_PTR<detail::WireSubscription_connection> c, connections)
		{
			RR_SHARED_PTR<WireConnectionBase> c1 = c->connection.lock();
			if (c1)
			{
				try
				{
					c1->SetIgnoreInValue(ignore);
				}
				catch (std::exception&) {}
			}
		}

		if (ignore)
		{
			in_value_valid.data() = false;
			in_value.reset();
			in_value_time = TimeSpec();
		}

		ROBOTRACONTEUR_LOG_TRACE_SOURCE_PATH(node, Subscription, -1, "", membername, "IgnoreInValue set to " << ignore);
	}

	void WireSubscriptionBase::SetOutValueAllBase(const RR_INTRUSIVE_PTR<RRValue>& val)
	{
		ROBOTRACONTEUR_LOG_TRACE_SOURCE_PATH(node, Subscription, -1, "", membername, "Setting out value to all connected wires");
		boost::mutex::scoped_lock lock(this_lock);
		BOOST_FOREACH(RR_SHARED_PTR<detail::WireSubscription_connection> c, connections)
		{
			RR_SHARED_PTR<WireConnectionBase> c1 = c->connection.lock();
			if (c1)
			{
				try
				{
					c1->SetOutValueBase(val);
				}
				catch (std::exception& exp)
				{
					ROBOTRACONTEUR_LOG_TRACE_SOURCE_PATH(node, Subscription, -1, "", membername, "SetOutValue failed for subscription connection: " << exp.what());
				}
			}
		}
	}

	size_t WireSubscriptionBase::GetActiveWireConnectionCount()	{
		boost::mutex::scoped_lock lock(this_lock);
		return connections.size();
	}

	void WireSubscriptionBase::Close()
	{

		boost::unordered_set<RR_SHARED_PTR<detail::WireSubscription_connection> > connections1;
		{
			boost::mutex::scoped_lock lock(this_lock);
			closed.data() = true;
			in_value_wait.notify_all();
			connections.swap(connections1);
		}

		BOOST_FOREACH(RR_SHARED_PTR<detail::WireSubscription_connection> connection, connections1)
		{
			RR_SHARED_PTR<WireConnectionBase> wire = connection->connection.lock();
			if (!wire) continue;
			try
			{
				wire->AsyncClose(&WireSubscriptionBase_emptyhandler, 5000);
			}
			catch (std::exception&) {}
		}

		RR_SHARED_PTR<ServiceSubscription> p = parent.lock();
		if (p)
		{
			p->WireSubscriptionClosed(shared_from_this());
		}

		ROBOTRACONTEUR_LOG_TRACE_SOURCE_PATH(node, Subscription, -1, "", membername, "WireSubscription closed");
	}

	WireSubscriptionBase::WireSubscriptionBase(RR_SHARED_PTR<ServiceSubscription> parent, boost::string_ref membername)
	{
		this->parent = parent;
		this->node = parent->node;
		this->membername = RR_MOVE(membername.to_string());
		this->wire_value_changed_semaphore = RR_MAKE_SHARED<detail::async_signal_pool_semaphore>(parent->node.lock());
	}

	void WireSubscriptionBase::ClientConnected(RR_SHARED_PTR<RRObject> client)
	{
		RR_SHARED_PTR<ServiceSubscription> parent1 = parent.lock();
		if (!parent1) return;
		RR_SHARED_PTR<RobotRaconteurNode> n = parent1->node.lock();
		if (!n) return;

		boost::mutex::scoped_lock lock(this_lock);
		if (closed.data()) return;

		ROBOTRACONTEUR_LOG_TRACE_SOURCE_PATH(node, Subscription, -1, "", membername, "ServiceSubscription client connected, begin connect wire");

		try
		{
			RR_SHARED_PTR<ServiceStub> stub = RR_DYNAMIC_POINTER_CAST<ServiceStub>(client);
			if (!stub) return;

			RR_SHARED_PTR<WireClientBase> wire_client=stub->RRGetWireClient(membername);
			wire_client->AsyncConnect_internal(boost::bind(&WireSubscriptionBase::ClientConnected1, shared_from_this(), client, _1, _2), n->GetRequestTimeout());

		}
		catch (std::exception& exp) {
			ROBOTRACONTEUR_LOG_TRACE_SOURCE_PATH(node, Subscription, -1, "", membername, "ServiceSubscription client connect wire failed: " << exp.what());
		}
	}

	
	void WireSubscriptionBase::ClientConnected1(RR_WEAK_PTR<RRObject> client, RR_SHARED_PTR<WireConnectionBase> connection, RR_SHARED_PTR<RobotRaconteurException> err)
	{
		RR_SHARED_PTR<RRObject> client1 = client.lock();
		if (!client1) return;

		if (err)
		{
			ROBOTRACONTEUR_LOG_TRACE_SOURCE_PATH(node, Subscription, -1, "", membername, "ServiceSubscription client connect wire failed: " << err->what());
			return;
		}


		RR_SHARED_PTR<detail::WireSubscription_connection> c;
		{
			boost::mutex::scoped_lock lock(this_lock);

			if (closed.data())
			{
				try
				{
					connection->AsyncClose(&WireSubscriptionBase_emptyhandler, 5000);
				}
				catch (std::exception()) {}
				return;
			}

			c = RR_MAKE_SHARED<detail::WireSubscription_connection>(shared_from_this(), connection, client1);
			connections.insert(c);
		}

		connection->SetIgnoreInValue(ignore_in_value.data());
		connection->AddListener(c);

		ROBOTRACONTEUR_LOG_TRACE_SOURCE_PATH(node, Subscription, -1, "", membername, "ServiceSubscription client connect wire connection completed successfully");
		
	}

	void WireSubscriptionBase::WireConnectionClosed(RR_SHARED_PTR<detail::WireSubscription_connection> wire)
	{
		ROBOTRACONTEUR_LOG_TRACE_SOURCE_PATH(node, Subscription, -1, "", membername, "ServiceSubscription client wire connection closed");
		boost::mutex::scoped_lock lock(this_lock);
		connections.erase(wire);		
	}
	void WireSubscriptionBase::WireValueChanged(RR_SHARED_PTR<detail::WireSubscription_connection> wire, RR_INTRUSIVE_PTR<RRValue> value, const TimeSpec& time)
	{
		RR_SHARED_PTR<WireConnectionBase> connection = wire->connection.lock();
		if (!connection) return;

		ROBOTRACONTEUR_LOG_TRACE_SOURCE_PATH(node, Subscription, -1, "", membername, "Wire subscription received InValue");
						
		{
			boost::mutex::scoped_lock lock(this_lock);

			if (ignore_in_value.data())
				return;
			
			in_value = value;
			in_value_time = time;
			in_value_connection = connection;
			in_value_valid.data() = true;

			in_value_wait.notify_all();
		}

		
		if (!isempty_WireValueChanged())
		{
			wire_value_changed_semaphore->try_fire_next(
				boost::bind(&WireSubscriptionBase::fire_WireValueChanged, shared_from_this(), value, time, connection));
		}
		
		
	}

	void WireSubscriptionBase::fire_WireValueChanged(RR_INTRUSIVE_PTR<RRValue> value, const TimeSpec& time, RR_SHARED_PTR<WireConnectionBase> connection)
	{

	}

	bool WireSubscriptionBase::isempty_WireValueChanged()
	{
		return false;
	}

	namespace detail
	{
		WireSubscription_connection::WireSubscription_connection(RR_SHARED_PTR<WireSubscriptionBase> parent, RR_SHARED_PTR<WireConnectionBase> connection, RR_SHARED_PTR<RRObject> client)
		{
			this->parent = parent;
			this->client = client;
			this->connection = connection;
		}

		void WireSubscription_connection::WireConnectionClosed(RR_SHARED_PTR<WireConnectionBase> connection)
		{
			RR_SHARED_PTR<WireSubscriptionBase> p = parent.lock();
			if (!p) return;
			p->WireConnectionClosed(shared_from_this());
		}

		void WireSubscription_connection::WireValueChanged(RR_SHARED_PTR<WireConnectionBase> connection, RR_INTRUSIVE_PTR<RRValue> value, const TimeSpec& time)
		{
			RR_SHARED_PTR<WireSubscriptionBase> p = parent.lock();
			if (!p) return;
			p->WireValueChanged(shared_from_this(), value, time);
		}

		WireSubscription_connection::~WireSubscription_connection()
		{
		}

		void WireSubscription_connection::SetOutValue(const RR_INTRUSIVE_PTR<RRValue>& value)
		{
			RR_SHARED_PTR<WireConnectionBase> c = connection.lock();
			if (!c) return;
			c->SetOutValueBase(value);
		}

		WireSubscription_send_iterator::WireSubscription_send_iterator(RR_SHARED_PTR<WireSubscriptionBase> subscription)
		{
			this->subscription = subscription;
			boost::mutex::scoped_lock lock1(subscription->this_lock);
			subscription_lock.swap(lock1);
			connections_iterator = subscription->connections.begin();
		}

		RR_SHARED_PTR<WireConnectionBase> WireSubscription_send_iterator::Next()
		{
			if (connections_iterator == subscription->connections.end())
			{
				return RR_SHARED_PTR<WireConnectionBase>();
			}
			boost::unordered_set<RR_SHARED_PTR<WireSubscription_connection> >::iterator c;
			while (true)
			{
				c = connections_iterator++;
				RR_SHARED_PTR<WireConnectionBase> c2=(*c)->connection.lock();
				if (c2)
				{
					current_connection = c;
					return c2;
				}

				if (connections_iterator == subscription->connections.end())
				{
					current_connection = connections_iterator;
					return RR_SHARED_PTR<WireConnectionBase>();
				}
				
			}			
		}

		void WireSubscription_send_iterator::SetOutValue(const RR_INTRUSIVE_PTR<RRValue>& value)
		{
			if (current_connection == subscription->connections.end())
			{
				return;
			}

			try
			{
				(*current_connection)->SetOutValue(value);
			}
			catch (std::exception&) {}
		}

		WireSubscription_send_iterator::~WireSubscription_send_iterator()
		{

		}

	}

	//class PipeSubscriptionBase

	static void PipeSubscriptionBase_emptyhandler(RR_SHARED_PTR<RobotRaconteurException> e)
	{

	}

	RR_INTRUSIVE_PTR<RRValue> PipeSubscriptionBase::ReceivePacketBase()
	{
		RR_INTRUSIVE_PTR<RRValue> o;
		if (!TryReceivePacketBase(o))
		{
			throw InvalidOperationException("PipeSubscription Receive Queue Empty");
		}
		return o;
	}

	bool PipeSubscriptionBase::TryReceivePacketBase(RR_INTRUSIVE_PTR<RRValue>& packet)
	{
		return TryReceivePacketBaseWait(packet, 0, false);
	}

	bool PipeSubscriptionBase::TryReceivePacketBaseWait(RR_INTRUSIVE_PTR<RRValue>& packet, int32_t timeout, bool peek, RR_SHARED_PTR<PipeEndpointBase>* ep)
	{
		boost::mutex::scoped_lock lock(this_lock);
		if (recv_packets.empty())
		{
			if (timeout == 0)
			{
				return false;
			}

			if (closed) return false;

			if (timeout < 0)
			{
				recv_packets_wait.wait(lock);
			}
			else
			{
				recv_packets_wait.wait_for(lock, boost::chrono::milliseconds(timeout));
			}

			if (recv_packets.empty()) return false;
		}

		packet = recv_packets.front().get<0>();
		if (ep)
		{
			*ep = recv_packets.front().get<1>();
		}
		if (!peek)
		{
			recv_packets.pop_front();
		}
		return true;
	}

	size_t PipeSubscriptionBase::Available()
	{
		boost::mutex::scoped_lock lock(this_lock);
		return recv_packets.size();
	}

	bool PipeSubscriptionBase::GetIgnoreReceived()
	{
		boost::mutex::scoped_lock lock(this_lock);

		return ignore_incoming_packets.data();
	}

	void PipeSubscriptionBase::SetIgnoreReceived(bool ignore)
	{
		boost::mutex::scoped_lock lock(this_lock);
		ignore_incoming_packets.data() = ignore;

		BOOST_FOREACH(RR_SHARED_PTR<detail::PipeSubscription_connection> c, connections)
		{
			RR_SHARED_PTR<PipeEndpointBase> c1 = c->connection.lock();
			if (c1)
			{
				try
				{
					c1->SetIgnoreReceived(ignore);
				}
				catch (std::exception&) {}
			}
		}

		if (ignore)
		{
			recv_packets.clear();
		}

		ROBOTRACONTEUR_LOG_TRACE_SOURCE_PATH(node, Subscription, -1, "", membername, "IgnoreReceived set to " << ignore);
	}

	static void PipeSubscriptionBase_empty_send_handler(uint32_t, RR_SHARED_PTR<RobotRaconteurException>) {}

	void PipeSubscriptionBase::AsyncSendPacketAllBase(const RR_INTRUSIVE_PTR<RRValue>& packet)
	{
		ROBOTRACONTEUR_LOG_TRACE_SOURCE_PATH(node, Subscription, -1, "", membername, "Sending packet to all connected pipe endpoints");

		boost::mutex::scoped_lock lock(this_lock);
		BOOST_FOREACH(RR_SHARED_PTR<detail::PipeSubscription_connection> c, connections)
		{
			if (c->DoSendPacket())
			{
				c->AsyncSendPacket(packet);
			}
		}
	}

	size_t PipeSubscriptionBase::GetActivePipeEndpointCount()
	{
		boost::mutex::scoped_lock lock(this_lock);
		return connections.size();
	}

	void PipeSubscriptionBase::Close()
	{

		boost::unordered_set<RR_SHARED_PTR<detail::PipeSubscription_connection> > connections1;
		{
			boost::mutex::scoped_lock lock(this_lock);
			closed.data() = true;
			recv_packets_wait.notify_all();
			connections.swap(connections1);
		}

		BOOST_FOREACH(RR_SHARED_PTR<detail::PipeSubscription_connection> connection, connections1)
		{
			RR_SHARED_PTR<PipeEndpointBase> pipe = connection->connection.lock();
			if (!pipe) continue;
			try
			{
				pipe->AsyncClose(&PipeSubscriptionBase_emptyhandler, 5000);
			}
			catch (std::exception&) {}
		}

		RR_SHARED_PTR<ServiceSubscription> p = parent.lock();
		if (p)
		{
			p->PipeSubscriptionClosed(shared_from_this());
		}

		ROBOTRACONTEUR_LOG_TRACE_SOURCE_PATH(node, Subscription, -1, "", membername, "PipeSubscription closed");
	}

	PipeSubscriptionBase::PipeSubscriptionBase(RR_SHARED_PTR<ServiceSubscription> parent, boost::string_ref membername, int32_t max_recv_packets, int32_t max_send_backlog)		
	{
		this->parent = parent;
		this->node = parent->node;
		this->membername = RR_MOVE(membername.to_string());
		this->max_recv_packets.data() = max_recv_packets;
		this->pipe_packet_received_semaphore = RR_MAKE_SHARED<detail::async_signal_pool_semaphore>(parent->node.lock());
		this->max_send_backlog.data() = max_send_backlog;
	}

	void PipeSubscriptionBase::ClientConnected(RR_SHARED_PTR<RRObject> client)
	{
		RR_SHARED_PTR<ServiceSubscription> parent1 = parent.lock();
		if (!parent1) return;
		RR_SHARED_PTR<RobotRaconteurNode> n = parent1->node.lock();
		if (!n) return;

		boost::mutex::scoped_lock lock(this_lock);
		if (closed.data()) return;

		ROBOTRACONTEUR_LOG_TRACE_SOURCE_PATH(node, Subscription, -1, "", membername, "ServiceSubscription client connected, begin connect pipe endpoint");

		try
		{
			RR_SHARED_PTR<ServiceStub> stub = RR_DYNAMIC_POINTER_CAST<ServiceStub>(client);
			if (!stub) return;

			RR_SHARED_PTR<PipeClientBase> pipe_client = stub->RRGetPipeClient(membername);
			pipe_client->AsyncConnect_internal(-1, boost::bind(&PipeSubscriptionBase::ClientConnected1, shared_from_this(), client, _1, _2),n->GetRequestTimeout());

		}
		catch (std::exception& exp) {
			ROBOTRACONTEUR_LOG_TRACE_SOURCE_PATH(node, Subscription, -1, "", membername, "ServiceSubscription client connect pipe endpoint failed: " << exp.what());
		}
	}


	void PipeSubscriptionBase::ClientConnected1(RR_WEAK_PTR<RRObject> client, RR_SHARED_PTR<PipeEndpointBase> connection, RR_SHARED_PTR<RobotRaconteurException> err)
	{
		RR_SHARED_PTR<RRObject> client1 = client.lock();
		if (!client1) return;

		if (err)
		{
			ROBOTRACONTEUR_LOG_TRACE_SOURCE_PATH(node, Subscription, -1, "", membername, "ServiceSubscription client connect pipe endpoint failed: " << err->what());
			return;
		}


		RR_SHARED_PTR<detail::PipeSubscription_connection> c;
		{
			boost::mutex::scoped_lock lock(this_lock);

			if (closed.data())
			{
				try
				{
					connection->AsyncClose(&PipeSubscriptionBase_emptyhandler, 5000);
				}
				catch (std::exception()) {}
				return;
			}

			c = RR_MAKE_SHARED<detail::PipeSubscription_connection>(shared_from_this(), connection, client1);
			connections.insert(c);
		}
		connection->SetIgnoreReceived(ignore_incoming_packets.data());
		connection->AddListener(c);
		
		ROBOTRACONTEUR_LOG_TRACE_SOURCE_PATH(node, Subscription, -1, "", membername, "ServiceSubscription client connect pipe endpoint completed successfully");
	}

	void PipeSubscriptionBase::PipeEndpointClosed(RR_SHARED_PTR<detail::PipeSubscription_connection> pipe)
	{
		ROBOTRACONTEUR_LOG_TRACE_SOURCE_PATH(node, Subscription, -1, "", membername, "ServiceSubscription client pipe endpoint closed");
		boost::mutex::scoped_lock lock(this_lock);
		connections.erase(pipe);
				
	}
	void PipeSubscriptionBase::PipeEndpointPacketReceived(RR_SHARED_PTR<detail::PipeSubscription_connection> pipe, RR_INTRUSIVE_PTR<RRValue> value)
	{
		//RR_SHARED_PTR<RRObject> client = wire->client.lock();
		//if (!client) return;

		ROBOTRACONTEUR_LOG_TRACE_SOURCE_PATH(node, Subscription, -1, "", membername, "Pipe subscription received packet");

		boost::mutex::scoped_lock lock(this_lock);
		
		recv_packets.push_back(boost::make_tuple(value,pipe->connection));

		if (max_recv_packets >= 0)
		{
			while (recv_packets.size() > boost::numeric_cast<size_t>(max_recv_packets.data()))
			{
				recv_packets.pop_front();
			}
		}

		if (recv_packets.empty()) return;

		recv_packets_wait.notify_all();

		lock.unlock();
		if (!isempty_PipePacketReceived())
		{
			pipe_packet_received_semaphore->try_fire_next(
				boost::bind(&PipeSubscriptionBase::fire_PipePacketReceived, shared_from_this()));
		}
		
	}

	void PipeSubscriptionBase::fire_PipePacketReceived()
	{

	}

	bool PipeSubscriptionBase::isempty_PipePacketReceived()
	{
		return false;
	}

	namespace detail
	{
		PipeSubscription_connection::PipeSubscription_connection(RR_SHARED_PTR<PipeSubscriptionBase> parent, RR_SHARED_PTR<PipeEndpointBase> connection, RR_SHARED_PTR<RRObject> client)
		{
			this->parent = parent;
			this->client = client;
			this->connection = connection;
		}

		void PipeSubscription_connection::PipeEndpointClosed(RR_SHARED_PTR<PipeEndpointBase> connection)
		{
			RR_SHARED_PTR<PipeSubscriptionBase> p = parent.lock();
			if (!p) return;
			p->PipeEndpointClosed(shared_from_this());
		}

		void PipeSubscription_connection::PipePacketReceived(RR_SHARED_PTR<PipeEndpointBase> connection, boost::function<bool(RR_INTRUSIVE_PTR<RRValue>&)> receive_packet_func)
		{
			RR_SHARED_PTR<PipeSubscriptionBase> p = parent.lock();
			if (!p) return;
			
			RR_INTRUSIVE_PTR<RRValue> packet;
			while (receive_packet_func(packet))
			{
				p->PipeEndpointPacketReceived(shared_from_this(), packet);
			}

		}

		void PipeSubscription_connection::PipePacketAckReceived(RR_SHARED_PTR<PipeEndpointBase> endpoint, uint32_t pnum)
		{
			RR_SHARED_PTR<PipeSubscriptionBase> p = parent.lock();
			if (!p) return;

			boost::mutex::scoped_lock lock(p->this_lock);

			try
			{
				if (std::count(backlog.begin(), backlog.end(), pnum) == 0)
				{
					forward_backlog.push_back(pnum);
				}
				else
				{
					backlog.remove(pnum);
				}
			}
			catch (std::exception&) {}

		}

		bool PipeSubscription_connection::DoSendPacket()
		{
			RR_SHARED_PTR<PipeSubscriptionBase> p = parent.lock();
			if (!p) return false;
			
			int32_t maximum_backlog = p->max_send_backlog;

			if (maximum_backlog > -1 && (boost::numeric_cast<int32_t>(backlog.size()) + boost::numeric_cast<int32_t>(active_sends.size())) > maximum_backlog)
			{
				return false;
			}
			return true;
		}

		void PipeSubscription_connection::AsyncSendPacket(const RR_INTRUSIVE_PTR<RRValue>& packet)
		{
			RR_SHARED_PTR<PipeEndpointBase> ep = connection.lock();;
			if (!ep) return;

			active_send_count.data() = active_send_count.data() < std::numeric_limits<int32_t>::max() ? active_send_count.data() + 1 : 0;
			int32_t send_key = active_send_count.data();
			active_sends.push_back(send_key);
			if (!send_copy_element.data())
			{
				ep->AsyncSendPacketBase(packet, boost::bind(&PipeSubscription_connection::pipe_packet_send_handler, this->shared_from_this(), _1, _2, send_key));
			}
			else
			{
				RR_INTRUSIVE_PTR<MessageElement> packet2 = ShallowCopyMessageElement(rr_cast<MessageElement>(packet));
				ep->AsyncSendPacketBase(packet2, boost::bind(&PipeSubscription_connection::pipe_packet_send_handler, this->shared_from_this(), _1, _2, send_key));
			}
		}

		PipeSubscription_connection::~PipeSubscription_connection()
		{

		}

		void PipeSubscription_connection::pipe_packet_send_handler(RR_WEAK_PTR<PipeSubscription_connection> connection, int32_t pnum, RR_SHARED_PTR<RobotRaconteurException> err, int32_t send_key)
		{
			RR_SHARED_PTR<PipeSubscription_connection> connection1 = connection.lock();
			if (!connection1) return;

			RR_SHARED_PTR<PipeSubscriptionBase> p = connection1->parent.lock();
			if (!p) return;

			boost::mutex::scoped_lock lock(p->this_lock);

			connection1->active_sends.remove(send_key);
			
		}

		PipeSubscription_send_iterator::PipeSubscription_send_iterator(RR_SHARED_PTR<PipeSubscriptionBase> subscription)
		{
			this->subscription = subscription;
			boost::mutex::scoped_lock lock1(subscription->this_lock);
			subscription_lock.swap(lock1);
			connections_iterator = subscription->connections.begin();
		}

		RR_SHARED_PTR<PipeEndpointBase> PipeSubscription_send_iterator::Next()
		{
			if (connections_iterator == subscription->connections.end())
			{
				return RR_SHARED_PTR<PipeEndpointBase>();
			}
			boost::unordered_set<RR_SHARED_PTR<PipeSubscription_connection> >::iterator c;
			while (true)
			{
				c = connections_iterator++;
				RR_SHARED_PTR<PipeEndpointBase> c2 = (*c)->connection.lock();
				if ((*c)->DoSendPacket())
				{
					current_connection = c;
					return c2;
				}

				if (connections_iterator == subscription->connections.end())
				{
					return RR_SHARED_PTR<PipeEndpointBase>();
				}
			}
		}

		void PipeSubscription_send_iterator::AsyncSendPacket(const RR_INTRUSIVE_PTR<RRValue>& packet)
		{
			if (current_connection != subscription->connections.end())
			{
				try
				{
					(*current_connection)->AsyncSendPacket(packet);
				}
				catch (std::exception&) {}
			}
		}

		PipeSubscription_send_iterator::~PipeSubscription_send_iterator()
		{

		}

		void ServiceSubscription_custom_member_subscribers::SubscribeWire(RR_SHARED_PTR<ServiceSubscription> s, RR_SHARED_PTR<WireSubscriptionBase> o)
		{
			s->SubscribeWire1(o);
		}
		void ServiceSubscription_custom_member_subscribers::SubscribePipe(RR_SHARED_PTR<ServiceSubscription> s, RR_SHARED_PTR<PipeSubscriptionBase> o)
		{
			s->SubscribePipe1(o);
		}


	}
	
}
