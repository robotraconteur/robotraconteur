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

#include "Discovery_private.h"
#include "Subscription_private.h"

#include <boost/foreach.hpp>
#include <boost/range/adaptors.hpp>
#include <boost/range/algorithm.hpp>
#include <boost/algorithm/string.hpp>

namespace RobotRaconteur
{
	ServiceInfo2::ServiceInfo2()
	{

	}

	ServiceInfo2::ServiceInfo2(const RobotRaconteurServiceIndex::ServiceInfo &info, const RobotRaconteurServiceIndex::NodeInfo &ninfo)
	{

		Name = info.Name;
		RootObjectType = info.RootObjectType;
		if (info.RootObjectImplements)
		{
			BOOST_FOREACH(RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<char> >& e, *info.RootObjectImplements | boost::adaptors::map_values)
			{
				RootObjectImplements.push_back(RRArrayToString(e));
			}
		}
		if (info.ConnectionURL)
		{
			BOOST_FOREACH(RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<char> >& e, *info.ConnectionURL | boost::adaptors::map_values)
			{
				ConnectionURL.push_back(RRArrayToString(e));
			}
		}

		Attributes = info.Attributes->GetStorageContainer();
		NodeID = RobotRaconteur::NodeID(RRArrayToArray<uint8_t, 16>(ninfo.NodeID));
		NodeName = ninfo.NodeName;
	}

	namespace detail
	{
		
		Discovery_updatediscoverednodes::Discovery_updatediscoverednodes(RR_SHARED_PTR<RobotRaconteurNode> node)
		{
			active_count = 0;
			searching = true;
			this->node = node;
		}

		void Discovery_updatediscoverednodes::timeout_timer_callback(const TimerEvent& e)
		{
			boost::mutex::scoped_lock lock(work_lock);

			if (!e.stopped)
			{
				{
					//boost::mutex::scoped_lock lock(searching_lock);
					if (!searching) return;
					searching = false;
				}

				{
					boost::mutex::scoped_lock lock(timeout_timer_lock);
					try
					{
						if (timeout_timer) timeout_timer->Stop();
					}
					catch (std::exception&) {}
					timeout_timer.reset();
				}

				ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Discovery, -1, "UpdateDiscoveredNodes timed out, returning current results");

				detail::InvokeHandler(node, handler);
								
			}


		}

		void Discovery_updatediscoverednodes::getdetectednodes_callback(RR_SHARED_PTR<std::vector<NodeDiscoveryInfo> > ret, int32_t key)
		{
			boost::mutex::scoped_lock lock(work_lock);

			bool c;
			{
				//boost::mutex::scoped_lock lock(searching_lock);
				c = searching;
			}

			if (!c) return;

			BOOST_FOREACH(NodeDiscoveryInfo& e, *ret)
			{
				node->NodeDetected(e);
			}


			bool done = false;

			{
				boost::mutex::scoped_lock lock(active_lock);
				active.remove(key);
				if (active.size() == 0) done = true;
			}

			if (done)
			{
				{
					//boost::mutex::scoped_lock lock(searching_lock);
					c = searching;
					searching = false;
				}
				if (!c) return;

				{
					boost::mutex::scoped_lock lock(timeout_timer_lock);
					try
					{
						if (timeout_timer) timeout_timer->Stop();
					}
					catch (std::exception&) {}
					timeout_timer.reset();
				}

				ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Discovery, -1, "UpdateDiscoveredNodes completed successfully");
				detail::InvokeHandler(node, handler);
				
			}

		}

		void Discovery_updatediscoverednodes::UpdateDiscoveredNodes(const std::vector<std::string>& schemes, const std::vector<RR_SHARED_PTR<Transport> >& transports, RR_MOVE_ARG(boost::function<void()>) handler, int32_t timeout)
		{

			ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Discovery, -1, "Begin UpdateDiscoveredNodes");

			boost::mutex::scoped_lock lock(work_lock);

			this->handler = handler;
			this->schemes = schemes;
			searching = true;

			if (timeout != RR_TIMEOUT_INFINITE)
			{
				timeout_timer = node->CreateTimer(boost::posix_time::milliseconds(timeout), boost::bind(&Discovery_updatediscoverednodes::timeout_timer_callback, shared_from_this(), RR_BOOST_PLACEHOLDERS(_1)), true);
				timeout_timer->Start();
			}

			{
				boost::mutex::scoped_lock lock(active_lock);
				int32_t timeout1 = timeout;
				if (timeout1 > 0)
				{
					timeout1 = (3 * timeout) / 4;
				}

				BOOST_FOREACH(const RR_SHARED_PTR<Transport>& e, transports)
				{

					int32_t key = active_count++;
					try
					{
						ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Discovery, -1, "Begin GetDetectedNodes for transport " << e->GetUrlSchemeString());
						boost::function<void(RR_SHARED_PTR<std::vector<NodeDiscoveryInfo> >)> h = boost::bind(&Discovery_updatediscoverednodes::getdetectednodes_callback, shared_from_this(), RR_BOOST_PLACEHOLDERS(_1), key);
						e->AsyncGetDetectedNodes(schemes, h, timeout1);
					}
					catch (std::exception& exp) 
					{
						ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Discovery, -1, "Begin GetDetectedNodes failed for transport " << e->GetUrlSchemeString() << ": " << exp.what());
					}

					active.push_back(key);
				}


			}



			bool done = false;
			{
				boost::mutex::scoped_lock lock(active_lock);
				if (active.size() == 0) done = true;
			}

			if (done)
			{
				ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Discovery, -1, "No transports available for node discovery");
				detail::InvokeHandler(node, handler);
			}

		}		

		Discovery_findservicebytype::Discovery_findservicebytype(RR_SHARED_PTR<RobotRaconteurNode> node)
		{
			active_count = 0;
			searching = true;
			ret = RR_MAKE_SHARED<std::vector<ServiceInfo2> >();
			this->node = node;
		}

		void Discovery_findservicebytype::handle_error(const int32_t& key, RR_SHARED_PTR<RobotRaconteurException> err)
		{
			boost::recursive_mutex::scoped_lock lock2(work_lock);

			{
				//boost::mutex::scoped_lock lock(searching_lock);
				if (!searching) return;
			}

			ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Discovery, -1, "FindServiceByType candidate failed: " << err->what());

			{
				boost::mutex::scoped_lock lock(active_lock);
				active.remove(key);
				errors.push_back(err);

				if (active.size() != 0) return;

			}

			//All activities have completed, assume failure

			{
				//boost::mutex::scoped_lock lock(searching_lock);
				searching = false;
			}

			{
				boost::mutex::scoped_lock lock(timeout_timer_lock);
				try
				{
					if (timeout_timer) timeout_timer->Stop();
				}
				catch (std::exception&) {}
				timeout_timer.reset();
			}

			
			boost::mutex::scoped_lock lock(ret_lock);
			ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Discovery, -1, "FindServiceByType last candidate failed, returning " << ret->size() << " discovered services");
			detail::InvokeHandler(node, handler, ret);
			

		}

		void Discovery_findservicebytype::timeout_timer_callback(const TimerEvent& e)
		{
			boost::recursive_mutex::scoped_lock lock2(work_lock);

			if (!e.stopped)
			{
				{
					//boost::mutex::scoped_lock lock(searching_lock);
					if (!searching) return;
					searching = false;
				}

				{
					boost::mutex::scoped_lock lock(timeout_timer_lock);
					try
					{
						if (timeout_timer) timeout_timer->Stop();
					}
					catch (std::exception&) {}
					timeout_timer.reset();
				}
				

				boost::mutex::scoped_lock lock(ret_lock);
				ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Discovery, -1, "FindServiceByType timed out, returning " << ret->size() << " discovered services");
				detail::InvokeHandler(node, handler, ret);
				
			}


		}

		void Discovery_findservicebytype::rr_empty_handler()
		{
		}

		void Discovery_findservicebytype::serviceinfo_callback(RR_INTRUSIVE_PTR<MessageEntry> ret1, RR_SHARED_PTR<RobotRaconteurException> err, RR_SHARED_PTR<ServiceStub> client, std::string url, uint32_t key)
		{
			boost::recursive_mutex::scoped_lock lock2(work_lock);
			if (err)
			{
				ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Discovery, -1, "FindServiceByType getting ServiceInfo2 from " << url << " failed: " << err->what());

				try
				{
					node->AsyncDisconnectService(client, &Discovery_findservicebytype::rr_empty_handler);
				}
				catch (std::exception&)
				{
				}
				handle_error(key, err);
				return;
			}

			bool c;
			{
				//boost::mutex::scoped_lock lock(searching_lock);
				c = searching;

			}
			if (!c)
			{
				try
				{
					node->AsyncDisconnectService(client, &Discovery_findservicebytype::rr_empty_handler);
				}
				catch (std::exception&)
				{
				}
			}
			else
			{
				try
				{
					RR_SHARED_PTR<RobotRaconteurServiceIndex::NodeInfo> n = RR_MAKE_SHARED<RobotRaconteurServiceIndex::NodeInfo>();
					n->NodeID = ArrayToRRArray<uint8_t>(rr_cast<ServiceStub>(client)->GetContext()->GetRemoteNodeID().ToByteArray());
					n->NodeName = rr_cast<ServiceStub>(client)->GetContext()->GetRemoteNodeName();

					try
					{
						node->AsyncDisconnectService(client, &Discovery_findservicebytype::rr_empty_handler);
					}
					catch (std::exception&)
					{
					}

					boost::smatch url_result;
					boost::regex reg("^([^:]+)://(.*)$");
					boost::regex_search(url, url_result, reg);

					if (url_result.size() < 3) throw InvalidArgumentException("Malformed URL");

					std::string scheme = url_result[1];

					if (ret1->Error == RobotRaconteur::MessageErrorType_None)
					{
						RR_INTRUSIVE_PTR<RobotRaconteur::MessageElement> me = ret1->FindElement("return");
						RR_INTRUSIVE_PTR<RobotRaconteur::RRMap<int32_t, RobotRaconteurServiceIndex::ServiceInfo> > ret = RobotRaconteur::rr_cast<RobotRaconteur::RRMap<int32_t, RobotRaconteurServiceIndex::ServiceInfo  > >((node->UnpackMapType<int32_t, RobotRaconteurServiceIndex::ServiceInfo  >(me->CastData<RobotRaconteur::MessageElementNestedElementList >())));

						if (ret)
						{
							BOOST_FOREACH(RR_INTRUSIVE_PTR<RobotRaconteurServiceIndex::ServiceInfo>& ii, *ret | boost::adaptors::map_values)
							{
								if (!ii) continue;
								if (ii->RootObjectType == servicetype)
								{

									boost::mutex::scoped_lock lock(ret_lock);

									ServiceInfo2 si(*ii, *n);

									//TODO: what is this?
									BOOST_FOREACH(std::string& iii, si.ConnectionURL)
									{
										/*if (!boost::starts_with(*iii, scheme + "://"))
										{
										boost::smatch url_result2;
										boost::regex_search(*iii, url_result2, reg);

										if (url_result2.size() < 3) continue;
										*iii = scheme + "://" + url_result2[2];

										}*/

									}
									this->ret->push_back(si);

								}
								else
								{
									BOOST_FOREACH(RR_INTRUSIVE_PTR<RRArray<char> >& impl, *ii->RootObjectImplements | boost::adaptors::map_values)
									{
										if (RRArrayToString(impl) == servicetype)
										{
											boost::mutex::scoped_lock lock(ret_lock);

											ServiceInfo2 si(*ii, *n);
											//TODO: What is this?
											BOOST_FOREACH(std::string &iii, si.ConnectionURL)
											{
												/*if (!boost::starts_with(*iii, scheme + "://"))
												{
												boost::smatch url_result2;
												boost::regex_search(*iii, url_result2, reg);

												if (url_result2.size() < 3) continue;
												*iii = scheme + "://" + url_result2[2];

												}*/

											}
											this->ret->push_back(si);
											break;
										}
									}

								}
							}
						}

						ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Discovery, -1, "FindServiceByType getting ServiceInfo2 from " << url << " completed successfully");

					}
					else
					{
						ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Discovery, -1, "FindServiceByType getting ServiceInfo2 from " << url << " failed: " << ret1->Error);
					}
					bool done = false;

					{
						boost::mutex::scoped_lock lock(active_lock);
						active.remove(key);
						done = active.empty();
					}

					if (done)
					{

						bool c2;
						{
							//boost::mutex::scoped_lock lock(searching_lock);
							c2 = searching;
							searching = false;
						}

						if (c2)
						{
							{
								boost::mutex::scoped_lock lock(timeout_timer_lock);
								try
								{
									if (timeout_timer) timeout_timer->Stop();
								}
								catch (std::exception&) {}
								timeout_timer.reset();
							}

							
							boost::mutex::scoped_lock lock(ret_lock);
							ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Discovery, -1, "FindServiceByType completed successfully with " << this->ret->size() << " discovered services");
							detail::InvokeHandler(node, handler, this->ret);							
						}
					}

				}				
				catch (std::exception& err2)
				{
					ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Discovery, -1, "FindServiceByType getting ServiceInfo2 from " << url << " failed: " << err2.what());
					handle_error(key, RobotRaconteurExceptionUtil::ExceptionToSharedPtr(err2, MessageErrorType_ConnectionError));
				}
			}
		}

		void Discovery_findservicebytype::connect_callback(RR_SHARED_PTR<RRObject> client, RR_SHARED_PTR<RobotRaconteurException> err, std::string url, uint32_t key)
		{
			boost::recursive_mutex::scoped_lock lock2(work_lock);

			if (err)
			{
				ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Discovery, -1, "FindServiceByType connecting to " << url << " failed: " << err->what());
				handle_error(key, err);
				return;
			}

			bool c;
			{
				//boost::mutex::scoped_lock lock(searching_lock);
				c = searching;

			}
			if (!c)
			{
				try
				{
					node->AsyncDisconnectService(client, &Discovery_findservicebytype::rr_empty_handler);
				}
				catch (std::exception&)
				{
				}
			}
			else
			{
				try
				{

					ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Discovery, -1, "FindServiceByType connectted to " << url << ", begin getting ServiceInfo2");

					int32_t key2;
					{
						boost::mutex::scoped_lock lock(active_lock);
						active_count++;
						key2 = active_count;




						RR_SHARED_PTR<ServiceStub> client3 = rr_cast<ServiceStub>(client);
						RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> rr_req = RobotRaconteur::CreateMessageEntry(RobotRaconteur::MessageEntryType_FunctionCallReq, "GetLocalNodeServices");
						client3->AsyncProcessRequest(rr_req, boost::bind(&Discovery_findservicebytype::serviceinfo_callback, shared_from_this(), RR_BOOST_PLACEHOLDERS(_1), RR_BOOST_PLACEHOLDERS(_2), client3, url, key2), 5000);


						active.push_back(key2);
						active.remove(key);
					}
				}				
				catch (std::exception& err2)
				{
					ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Discovery, -1, "FindServiceByType begin getting ServiceInfo2 from url " << url << " failed: " << err2.what());
					try
					{
						node->AsyncDisconnectService(client, &Discovery_findservicebytype::rr_empty_handler);
					}
					catch (std::exception&)
					{
					}
					handle_error(key, RobotRaconteurExceptionUtil::ExceptionToSharedPtr(err2, MessageErrorType_ConnectionError));
				}
			}

		}
		
		void Discovery_findservicebytype::find2()
		{
			boost::recursive_mutex::scoped_lock lock2(work_lock);

			std::list<std::vector<std::string> > urls;

			std::vector<NodeDiscoveryInfo> n1 = node->GetDetectedNodes();

			BOOST_FOREACH(NodeDiscoveryInfo& ee, n1)
			{
				try
				{
					std::vector<std::string> urls1 = std::vector<std::string>();
					BOOST_FOREACH(NodeDiscoveryInfoURL& url, ee.URLs)
					{
						BOOST_FOREACH(const std::string& e, schemes)
						{
							std::string t2 = (e + "://");
							if (boost::starts_with(url.URL, t2))
							{
								urls1.push_back(url.URL);
								break;
							}
						}

					}

					if (!urls1.empty()) urls.push_back(urls1);
				}
				catch (std::exception& exp) {
					ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Discovery, -1, "FindServiceByType error processing NodeDiscoveryInfo for NodeID " << ee.NodeID.ToString() << ": " << exp.what() );
				}
			}

			if (urls.empty())
			{
				ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Discovery, -1, "FindServiceByType could not find any candidate URLs");
				RR_SHARED_PTR<std::vector<ServiceInfo2> > ret = RR_MAKE_SHARED<std::vector<ServiceInfo2> >();
				detail::PostHandler(node,handler, ret,true);
				return;
			}

			if (timeout_timer) timeout_timer->Start();

			BOOST_FOREACH(std::vector<std::string>& e, urls)
			{
				try
				{
					int32_t key;
					{
						boost::mutex::scoped_lock lock(active_lock);
						active_count++;
						key = active_count;

						ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Discovery, -1, "FindServiceByType connecting to node using candidate URLs " << boost::join(e,", "));
						node->AsyncConnectService(e, "", (RR_INTRUSIVE_PTR<RRMap<std::string, RRValue> >()), NULL, "", boost::bind(&Discovery_findservicebytype::connect_callback, shared_from_this(), RR_BOOST_PLACEHOLDERS(_1), RR_BOOST_PLACEHOLDERS(_2), e.front(), key), timeout);

						active.push_back(key);
					}
				}
				catch (std::exception& exp2)
				{
					ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Discovery, -1, "FindServiceByType connecting to node using candidate URLs " << boost::join(e,", ") 
						<< " failed: " << exp2.what());
				}
			}
		}

		void Discovery_findservicebytype::AsyncFindServiceByType(boost::string_ref servicetype, const std::vector<std::string>& schemes, RR_MOVE_ARG(boost::function<void(RR_SHARED_PTR<std::vector<ServiceInfo2> >)>) handler, int32_t timeout)
		{
			ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Discovery, -1, "Begin FindServiceByType for type \"" << servicetype << "\" with schemes " << boost::join(schemes, ", "));

			boost::recursive_mutex::scoped_lock lock2(work_lock);
			this->handler = handler;
			this->schemes = schemes;
			this->timeout = timeout;

			this->servicetype = RR_MOVE(servicetype.to_string());
			if (timeout != RR_TIMEOUT_INFINITE)
			{
				timeout_timer = node->CreateTimer(boost::posix_time::milliseconds(timeout), boost::bind(&Discovery_findservicebytype::timeout_timer_callback, shared_from_this(), RR_BOOST_PLACEHOLDERS(_1)), true);

				//timeout_timer->Start();

			}

			ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Discovery, -1, "FindServiceByType begin update detected nodes");
			//int32_t timeout2=(timeout==RR_TIMEOUT_INFINITE) ? RR_TIMEOUT_INFINITE : timeout/4;
			node->AsyncUpdateDetectedNodes(schemes, boost::bind(&Discovery_findservicebytype::find2, shared_from_this()), timeout / 4);
		}

		//class Discovery_updateserviceinfo

		Discovery_updateserviceinfo::Discovery_updateserviceinfo(RR_WEAK_PTR<RobotRaconteurNode> node)
		{
			this->node = node;
			retry_count = 0;
			backoff = 0;
			
		}

		void Discovery_updateserviceinfo::handle_error(RR_SHARED_PTR<RobotRaconteurException> err)
		{
			RR_SHARED_PTR<RobotRaconteurNode> n = node.lock();
			if (!n) return;

			if (client)
			{
				try
				{					
					n->AsyncDisconnectService(client, boost::bind(&Discovery_updateserviceinfo::rr_empty_handler));					
				}
				catch (std::exception&) {}
			}

			client.reset();

			retry_count++;
			if (retry_count < 3)
			{
				ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Discovery, -1, "UpdateServiceInfo for remote node " << this->remote_nodeid.ToString() << " failed, retrying: " << err->what());

				//Restart the process 3 times
				backoff = n->GetRandomInt(0, 500);
				RR_SHARED_PTR<Timer> t = n->CreateTimer(boost::posix_time::milliseconds(backoff),
					boost::bind(&Discovery_updateserviceinfo::backoff_timer_handler, shared_from_this(), RR_BOOST_PLACEHOLDERS(_1)),
					true);
				t->Start();

				timeout_timer = t;

				return;
			}

			ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Discovery, -1, "UpdateServiceInfo for remote node " << this->remote_nodeid.ToString() << " failed, out of retries: " << err->what());

			boost::function<void(RR_SHARED_PTR<Discovery_nodestorage>, RR_SHARED_PTR<std::vector<ServiceInfo2> >, boost::string_ref, RR_SHARED_PTR<RobotRaconteurException>)> handler2 = handler;
			handler.clear();

			{
				boost::mutex::scoped_lock lock2(storage->this_lock);
				if (storage->updater.lock() == shared_from_this())
				{
					storage->updater.reset();
				}
			}

			if (!handler2) return;

			RobotRaconteurNode::TryPostToThreadPool(node, boost::bind(handler2, storage, RR_SHARED_PTR<std::vector<ServiceInfo2> >(), service_nonce, err), true);
		}

		void Discovery_updateserviceinfo::rr_empty_handler()
		{

		}

		static std::vector<std::string> Discovery_updateserviceinfo_convertmap(RR_INTRUSIVE_PTR<RRMap<int32_t, RRArray<char> > > d)
		{			
			RR_NULL_CHECK(d);
			std::vector<std::string> o;
			o.reserve(d->size());
			BOOST_FOREACH(RR_INTRUSIVE_PTR<RRArray<char> > d2, *d | boost::adaptors::map_values)
			{
				o.push_back(RRArrayToString(d2));
			}
			return o;
		}

		void Discovery_updateserviceinfo::serviceinfo_handler(RR_INTRUSIVE_PTR<MessageEntry> ret1, RR_SHARED_PTR<RobotRaconteurException> err)
		{
			boost::mutex::scoped_lock lock(this_lock);

			if (err)
			{
				ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Discovery, -1, "UpdateServiceInfo getting ServiceInfo2 for remote node " << this->remote_nodeid.ToString() << " failed: " << err->what());
				handle_error(err);
				return;
			}

			RR_SHARED_PTR<RobotRaconteurNode> n = node.lock();
			if (!n)
			{
				handle_error(RR_MAKE_SHARED<ServiceException>("Node has been released"));
				return;
			}

			try
			{
				if (n)
				{
					n->AsyncDisconnectService(client, boost::bind(&Discovery_updateserviceinfo::rr_empty_handler));
				}
			}
			catch (std::exception&) {}

			if (!ret1)
			{
				ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Discovery, -1, "UpdateServiceInfo for remote node " << this->remote_nodeid.ToString() << " failed: Invalid return");
				handle_error(RR_MAKE_SHARED<ServiceException>("Invalid return"));
				return;
			}

			if (ret1->Error != MessageErrorType_None)
			{
				ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Discovery, -1, "UpdateServiceInfo getting ServiceInfo2 for remote node " << this->remote_nodeid.ToString() << " failed: " << ret1->Error);
				handle_error(RobotRaconteurExceptionUtil::MessageEntryToException(ret1));
				return;
			};
			
			RR_SHARED_PTR<std::vector<ServiceInfo2> > o = RR_MAKE_SHARED<std::vector<ServiceInfo2> >();

			try
			{
				RR_INTRUSIVE_PTR<RobotRaconteur::MessageElement> me = ret1->FindElement("return");
				// Limit size to protect from memory leak attacks
				if (me->ElementSize > 64*1024)
				{
					ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Discovery, -1, "UpdateServiceInfo dropping response from remote node " << this->remote_nodeid.ToString() << " to prevent overflow");
					handle_error(RR_MAKE_SHARED<ServiceException>("Return from remote node too large"));
					return;
				}
				RR_INTRUSIVE_PTR<RobotRaconteur::RRMap<int32_t, RobotRaconteurServiceIndex::ServiceInfo> > ret = RobotRaconteur::rr_cast<RobotRaconteur::RRMap<int32_t, RobotRaconteurServiceIndex::ServiceInfo> >((n->UnpackMapType<int32_t, RobotRaconteurServiceIndex::ServiceInfo>(me->CastDataToNestedList())));

				if (ret)
				{
					BOOST_FOREACH(RR_INTRUSIVE_PTR<RobotRaconteurServiceIndex::ServiceInfo>& e, *ret | boost::adaptors::map_values)
					{
						ServiceInfo2 o1;
						o1.NodeID = remote_nodeid;
						o1.NodeName = remote_nodename;
						o1.Name = e->Name;
						o1.ConnectionURL = Discovery_updateserviceinfo_convertmap(e->ConnectionURL);
						o1.RootObjectType = e->RootObjectType;
						o1.RootObjectImplements = Discovery_updateserviceinfo_convertmap(e->RootObjectImplements);
						o1.Attributes = e->Attributes->GetStorageContainer();
						o->push_back(o1);
					}
				}

			}
			catch (RobotRaconteurException& err)
			{
				ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Discovery, -1, "UpdateServiceInfo getting ServiceInfo2 for remote node " << this->remote_nodeid.ToString() << " failed: " << err.what());
				handle_error(RobotRaconteurExceptionUtil::DownCastException(err));
				return;
			}
			catch (std::exception& err)
			{
				ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Discovery, -1, "UpdateServiceInfo getting ServiceInfo2 for remote node " << this->remote_nodeid.ToString() << " failed: " << err.what());
				handle_error(RR_MAKE_SHARED<ServiceException>(err.what()));
				return;
			}

			boost::function<void(RR_SHARED_PTR<Discovery_nodestorage>, RR_SHARED_PTR<std::vector<ServiceInfo2> >, boost::string_ref, RR_SHARED_PTR<RobotRaconteurException>)> handler2 = handler;
			handler.clear();
						
			lock.unlock();	
			{
				boost::mutex::scoped_lock lock2(storage->this_lock);
				if (storage->updater.lock() == shared_from_this())
				{
					storage->updater.reset();
				}

			}

			ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Discovery, -1, "UpdateServiceInfo getting ServiceInfo2 for remote node " << this->remote_nodeid.ToString() << " completed successfully");

			if (!handler2) return;

			RobotRaconteurNode::TryPostToThreadPool(node, boost::bind(handler2, storage, o, service_nonce, RR_SHARED_PTR<RobotRaconteurException>()), true);
		}

		void Discovery_updateserviceinfo::connect_handler(RR_SHARED_PTR<RRObject> client, RR_SHARED_PTR<RobotRaconteurException> err)
		{
			boost::mutex::scoped_lock lock(this_lock);

			if (err)
			{
				ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Discovery, -1, "UpdateServiceInfo connect for remote node " << this->remote_nodeid.ToString() << " failed: " << err->what());
				handle_error(err);
				return;
			}

			RR_SHARED_PTR<RobotRaconteurNode> n = node.lock();
			if (!n)
			{
				handle_error(RR_MAKE_SHARED<ConnectionException>("Node has been released"));
				return;
			}

			this->client = client;

			try
			{
				ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Discovery, -1, "UpdateServiceInfo connected to remote node " << this->remote_nodeid.ToString() << ", begin getting ServiceInfo2");

				RR_SHARED_PTR<ServiceStub> client3 = rr_cast<ServiceStub>(client);
				remote_nodeid=client3->GetContext()->GetRemoteNodeID();
				remote_nodename = client3->GetContext()->GetRemoteNodeName();
				
				if (remote_nodeid != storage->info->NodeID
					|| (!storage->info->NodeName.empty() && remote_nodename != storage->info->NodeName))
				{
					//Very unlikely unless a node is on the fritz
					try
					{
						handle_error(RR_MAKE_SHARED <ConnectionException>("Node identification mismatch"));
						return;
					}
					catch (std::exception&) {}
				}

				RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> rr_req = RobotRaconteur::CreateMessageEntry(RobotRaconteur::MessageEntryType_FunctionCallReq, "GetLocalNodeServices");
				client3->AsyncProcessRequest(rr_req, boost::bind(&Discovery_updateserviceinfo::serviceinfo_handler, shared_from_this(), RR_BOOST_PLACEHOLDERS(_1), RR_BOOST_PLACEHOLDERS(_2)),5000);
			}
			catch (RobotRaconteurException& err)
			{
				ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Discovery, -1, "UpdateServiceInfo connect for remote node " << this->remote_nodeid.ToString() << " failed: " << err.what());
				handle_error(RobotRaconteurExceptionUtil::DownCastException(err));
				return;
			}
			catch (std::exception& err)
			{
				ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Discovery, -1, "UpdateServiceInfo connect for remote node " << this->remote_nodeid.ToString() << " failed: " << err.what());
				handle_error(RR_MAKE_SHARED<ServiceException>(err.what()));
				return;
			}

			
		}

		void Discovery_updateserviceinfo::backoff_timer_handler(const TimerEvent& evt)
		{
			boost::mutex::scoped_lock lock(this_lock);
			timeout_timer.reset();
						
			RR_SHARED_PTR<RobotRaconteurNode> n = node.lock();
			if (!n)
			{
				handle_error(RR_MAKE_SHARED<ConnectionException>("Node has been released"));
				return;
			}

			std::vector<std::string> urls;
			{
				boost::mutex::scoped_lock lock(storage->this_lock);
				urls.reserve(storage->info->URLs.size());
				BOOST_FOREACH(NodeDiscoveryInfoURL& u, storage->info->URLs)
				{
					urls.push_back(u.URL);
				}				
			}

			try
			{
				ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Discovery, -1, "UpdateServiceInfo connecting to remote node " << this->remote_nodeid.ToString()
					<< " using candidate URLs " << boost::join(urls,", "));
				n->AsyncConnectService(urls, "", RR_INTRUSIVE_PTR<RRMap<std::string, RRValue> >(), NULL, "",
					boost::bind(&Discovery_updateserviceinfo::connect_handler, shared_from_this(), RR_BOOST_PLACEHOLDERS(_1), RR_BOOST_PLACEHOLDERS(_2)), 15000);
			}
			catch (RobotRaconteurException& err)
			{
				ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Discovery, -1, "UpdateServiceInfo connecting to remote node " <<
				 this->remote_nodeid.ToString() << " using candidate URLs " << boost::join(urls,", ") << " failed: " << err.what());
				handle_error(RobotRaconteurExceptionUtil::DownCastException(err));
				return;
			}
			catch (std::exception& err)
			{
				ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Discovery, -1, "UpdateServiceInfo connecting to remote node " <<
				 this->remote_nodeid.ToString() << " using candidate URLs " << boost::join(urls,", ") << " failed: " << err.what());
				handle_error(RR_MAKE_SHARED<ServiceException>(err.what()));
				return;
			}

		}

		void Discovery_updateserviceinfo::AsyncUpdateServiceInfo(RR_SHARED_PTR<Discovery_nodestorage> storage, boost::string_ref service_nonce, RR_MOVE_ARG(boost::function<void(RR_SHARED_PTR<Discovery_nodestorage>, RR_SHARED_PTR<std::vector<ServiceInfo2> >, boost::string_ref, RR_SHARED_PTR<RobotRaconteurException>)>) handler, int32_t extra_backoff)
		{
			this->storage = storage;
			this->handler = handler;
			this->retry_count = 0;
			this->service_nonce = RR_MOVE(service_nonce.to_string());


			RR_SHARED_PTR<RobotRaconteurNode> n = node.lock();
			if (!n) return;

			backoff = n->GetRandomInt(100, 600) + extra_backoff;
			RR_SHARED_PTR<Timer> t = n->CreateTimer(boost::posix_time::milliseconds(backoff),
				boost::bind(&Discovery_updateserviceinfo::backoff_timer_handler, shared_from_this(), RR_BOOST_PLACEHOLDERS(_1)),
				true);
			t->Start();

			timeout_timer = t;
			ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Discovery, -1, "Begin UpdateServiceInfo to remote node " <<
				 this->remote_nodeid.ToString() << " using " << backoff << " ms backoff");
		}
		
		//class Discovery

		Discovery::Discovery(RR_SHARED_PTR<RobotRaconteurNode> node)
		{
			max_DiscoveredNodes.data() = 4096;
			this->node = node;
			ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Discovery, -1, "Discovery created");
		}

		std::vector<NodeDiscoveryInfo> Discovery::GetDetectedNodes()
		{
			std::vector<NodeDiscoveryInfo> o;
			boost::mutex::scoped_lock lock(m_DiscoveredNodes_lock);
			BOOST_FOREACH(RR_SHARED_PTR<Discovery_nodestorage>& o1, m_DiscoveredNodes | boost::adaptors::map_values)
			{
				boost::mutex::scoped_lock lock2(o1->this_lock);
				o.push_back(*o1->info);
			}

			ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Discovery, -1, "GetDetectedNodes returning " << o.size() << " detected nodes");

			return o;
		}

		static std::string Discovery_log_NodeDiscoveryInfoURLs_url_field(const NodeDiscoveryInfoURL& url)
		{
			return url.URL;
		}

		static std::string Discovery_log_NodeDiscoveryInfoURLs(const std::vector<NodeDiscoveryInfoURL>& urls)
		{
			return boost::join(urls | boost::adaptors::transformed(&Discovery_log_NodeDiscoveryInfoURLs_url_field), ", ");
		}

		void Discovery::NodeDetected(const NodeDiscoveryInfo& info)
		{
			if (info.NodeID.IsAnyNode()) return;

			RR_SHARED_PTR<RobotRaconteurNode> n = node.lock();
			if (!n) return;

			ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Discovery, -1, "Node detected NodeID " << info.NodeID.ToString() << " NodeName " << info.NodeName 
				<< " URLs " << Discovery_log_NodeDiscoveryInfoURLs(info.URLs));

			if (info.ServiceStateNonce.size() > 32) 
			{
				ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Discovery, -1, "Node detected NodeID " << info.NodeID.ToString() << " invalid ServiceStateNonce");
				return;
			}

			try
			{
				boost::mutex::scoped_lock lock(m_DiscoveredNodes_lock);

				if (info.NodeName.size() > 128)
				{
					ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Discovery, -1, "Node detected NodeID " << info.NodeID.ToString() << " invalid NodeName");
					return;
				}

				std::string id = info.NodeID.ToString();
				std::map<std::string, RR_SHARED_PTR<Discovery_nodestorage> >::iterator e1 = m_DiscoveredNodes.find(id);
				if (e1 == m_DiscoveredNodes.end())
				{
					if (m_DiscoveredNodes.size() >= max_DiscoveredNodes)
					{
						ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Discovery, -1, "Detected node NodeID " << info.NodeID.ToString() 
							<< " dropped due to full node cache");
						return;
					}
					RR_SHARED_PTR<NodeDiscoveryInfo> info2 = RR_MAKE_SHARED<NodeDiscoveryInfo>(info);
					for (std::vector<NodeDiscoveryInfoURL>::iterator e = info2->URLs.begin(); e != info2->URLs.end();)
					{
						bool valid = true;

						if (e->URL.size() > 256)
						{
							valid = false;
						}
						if (valid)
						{
							try
							{
								ParseConnectionURLResult u = ParseConnectionURL(e->URL);
								if (u.nodeid != info.NodeID)
								{
									valid = false;
								}
							}
							catch (std::exception& exp2)
							{
								ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Discovery, -1, "Node detected NodeID " << info.NodeID.ToString() << " provides invalid URL: " << e->URL << ": " << exp2.what());
								valid = false;
							}
						}

						if (valid)
						{
							ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Discovery, -1, "Node detected NodeID " << info.NodeID.ToString() << " accepting candidate URL " << e->URL);
							e++;
						}
						else
						{
							ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Discovery, -1, "Node detected NodeID " << info.NodeID.ToString() << " rejecting candidate URL " << e->URL);
							e = info2->URLs.erase(e);
						}
					}

					if (info2->URLs.size() == 0)
					{
						ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Discovery, -1, "Node detected NodeID " << info.NodeID.ToString() << " did not find any valid candidate URLs");
					 return;
					}

					RR_SHARED_PTR<Discovery_nodestorage> storage = RR_MAKE_SHARED<Discovery_nodestorage>();
					storage->info = info2;					

					RR_SHARED_PTR<Discovery_updateserviceinfo> update = RR_MAKE_SHARED<Discovery_updateserviceinfo>(node);
					storage->updater = update;

					storage->recent_service_nonce.push_back(info2->ServiceStateNonce);

					storage->retry_window_start = n->NowNodeTime();

					m_DiscoveredNodes.insert(std::make_pair(id, storage));
					
					if (!subscriptions.empty())
					{
						ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Discovery, -1, "Node detected NodeID " << info.NodeID.ToString() << " updating service info");
						update->AsyncUpdateServiceInfo(storage, info2->ServiceStateNonce, boost::bind(&Discovery::EndUpdateServiceInfo, shared_from_this(), RR_BOOST_PLACEHOLDERS(_1), RR_BOOST_PLACEHOLDERS(_2), RR_BOOST_PLACEHOLDERS(_3), RR_BOOST_PLACEHOLDERS(_4)));
					}
					return;
				}
				else
				{
					boost::mutex::scoped_lock lock2(e1->second->this_lock);
					RR_SHARED_PTR<NodeDiscoveryInfo>& i = e1->second->info;
					BOOST_FOREACH(const NodeDiscoveryInfoURL& e, info.URLs)
					{
						bool found = false;
						BOOST_FOREACH(NodeDiscoveryInfoURL& ee, i->URLs)
						{
							if (e.URL == ee.URL)
							{
								if (e.LastAnnounceTime > ee.LastAnnounceTime)
								{
									ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Discovery, -1, "Node detected NodeID " << info.NodeID.ToString() 
										<< " updating last announce time for candidate URL " << e.URL);
									ee.LastAnnounceTime = e.LastAnnounceTime;
								}
								found = true;
							}
						}

						if (!found)
						{
							if (e.URL.size() > 256)
							{
								continue;
							}
							try
							{
								ParseConnectionURLResult u = ParseConnectionURL(e.URL);
								if (u.nodeid != info.NodeID)
								{
									continue;
								}
							}
							catch (std::exception& exp2)
							{
								ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Discovery, -1, "Node detected NodeID " << info.NodeID.ToString() << " provides invalid URL: " << e.URL << ": " << exp2.what());
								continue;
							}
							ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Discovery, -1, "Node detected NodeID " << info.NodeID.ToString() << " accepting candidate URL " << e.URL);
							i->URLs.push_back(e);
						}
					}

					if (i->ServiceStateNonce != info.ServiceStateNonce && !info.ServiceStateNonce.empty())
					{
						ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Discovery, -1, "Node detected NodeID " << info.NodeID.ToString() << " updating ServiceStateNonce");
						i->ServiceStateNonce = info.ServiceStateNonce;
					}

					if (!info.ServiceStateNonce.empty())
					{
						if (boost::range::find(e1->second->recent_service_nonce, info.ServiceStateNonce) != e1->second->recent_service_nonce.end())
						{
							return;
						}
						else
						{
							e1->second->recent_service_nonce.push_back(info.ServiceStateNonce);
							if (e1->second->recent_service_nonce.size() > 16)
							{
								e1->second->recent_service_nonce.pop_front();
							}
						}
					}

					if (!subscriptions.empty())
					{

						if (((i->ServiceStateNonce != e1->second->last_update_nonce) || i->ServiceStateNonce.empty())
							&& !e1->second->updater.lock())
						{
							ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Discovery, -1, "Node detected NodeID " << info.NodeID.ToString() 
								<< " updating service info due to change in ServiceStateNonce");
							RetryUpdateServiceInfo(e1->second);
						}
					}
				}

				

			}
			catch (std::exception& exp)
			{
				ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Discovery, -1, "Node detected NodeID " << info.NodeID.ToString() << " failed: " << exp.what());
			}

		}

		void Discovery::EndUpdateServiceInfo(RR_SHARED_PTR<Discovery_nodestorage> storage, RR_SHARED_PTR<std::vector<ServiceInfo2> > info, boost::string_ref nonce, RR_SHARED_PTR<RobotRaconteurException> err)
		{

			RR_SHARED_PTR<RobotRaconteurNode> n = node.lock();
			if (!n) return;

			if (!info) return;

			boost::mutex::scoped_lock lock(m_DiscoveredNodes_lock);

			
			boost::mutex::scoped_lock lock2(storage->this_lock);
			storage->services = info;
			storage->last_update_nonce = RR_MOVE(nonce.to_string());
			storage->last_update_time = n->NowNodeTime();

			if (storage->last_update_nonce != storage->info->ServiceStateNonce)
			{
				//We missed an update, do another refresh but delay 5 seconds to prevent flooding
				if (!storage->updater.lock())
				{
					ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Discovery, -1, "ServiceStateNonce for " << storage->info->NodeID.ToString() << 
						" changed during update, retry");
					RetryUpdateServiceInfo(storage);
				}

			}
			else
			{
				storage->retry_count.data() = 0;
			}
			

			BOOST_FOREACH(RR_WEAK_PTR<IServiceSubscription> s, subscriptions)
			{
				RR_SHARED_PTR<IServiceSubscription> s1 = s.lock();
				if (!s1) continue;
				s1->NodeUpdated(storage);
			}

			RobotRaconteurNode::TryPostToThreadPool(node, boost::bind(&RobotRaconteurNode::FireNodeDetected, n, storage->info, storage->services));
			
		}

		void Discovery::RetryUpdateServiceInfo(RR_SHARED_PTR<Discovery_nodestorage> storage)
		{
			//If updater is running, return
			if (storage->updater.lock()) return;

			ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Discovery, -1, "UpdateServiceInfo retry requested for " << storage->info->NodeID.ToString());

			RR_SHARED_PTR<RobotRaconteurNode> n = node.lock();
			if (!n) return;

			boost::posix_time::ptime now = n->NowNodeTime();

			if (now > storage->retry_window_start + boost::posix_time::seconds(60))
			{
				storage->retry_window_start = now;
				storage->retry_count.data() = 0;
			}

			uint32_t retry_count = storage->retry_count.data()++;

			//Add progressive backoffs to prevent flooding

			uint32_t backoff = n->GetRandomInt<uint32_t>(100,600);
			if (retry_count > 3)
				backoff = n->GetRandomInt<uint32_t>(2000, 2500);
			if (retry_count > 5)
				backoff = n->GetRandomInt<uint32_t>(4500, 5500);
			if (retry_count > 8)
				backoff = n->GetRandomInt<uint32_t>(9000, 11000);
			if (retry_count > 12)
				backoff = n->GetRandomInt<uint32_t>(25000, 35000);

			//If nonce isn't in use, add 15 seconds to delay
			if (storage->info->ServiceStateNonce.empty() && storage->last_update_nonce.empty()
				&& !storage->last_update_time.is_not_a_date_time())
			{
				backoff += 15000;
			}
			

			ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Discovery, -1, "UpdateServiceInfo retry requested for " << storage->info->NodeID.ToString() 
			 << " using backoff " << backoff  << " ms for retry " << retry_count);

			RR_SHARED_PTR<Discovery_updateserviceinfo> update = RR_MAKE_SHARED<Discovery_updateserviceinfo>(node);
			storage->updater = update;
			update->AsyncUpdateServiceInfo(storage, storage->info->ServiceStateNonce, boost::bind(&Discovery::EndUpdateServiceInfo, shared_from_this(), RR_BOOST_PLACEHOLDERS(_1), RR_BOOST_PLACEHOLDERS(_2), RR_BOOST_PLACEHOLDERS(_3), RR_BOOST_PLACEHOLDERS(_4)), backoff);
		}

		void Discovery::UpdateDetectedNodes(const std::vector<std::string>& schemes)
		{
			ROBOTRACONTEUR_ASSERT_MULTITHREADED(node);

			RR_SHARED_PTR<detail::sync_async_handler<void> > t = RR_MAKE_SHARED<detail::sync_async_handler<void> >();
			boost::function<void()> h = boost::bind(&detail::sync_async_handler<void>::operator(), t);
			AsyncUpdateDetectedNodes(schemes, h);
			t->end_void();
		}

		void Discovery::AsyncUpdateDetectedNodes(const std::vector<std::string>& schemes, boost::function<void()>& handler, int32_t timeout)
		{
			RR_SHARED_PTR<RobotRaconteurNode> n = node.lock();
			if (!n) throw InvalidOperationException("Node has been released");

			ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Discovery, -1, "UpdateDetectedNodes requested for schemes " << boost::join(schemes,", "));

			std::vector<RR_SHARED_PTR<Transport> > t;
			{
				boost::mutex::scoped_lock lock(n->transports_lock);
				boost::copy(n->transports | boost::adaptors::map_values, std::back_inserter(t));
			}

			RR_SHARED_PTR<Discovery_updatediscoverednodes> d = RR_MAKE_SHARED<Discovery_updatediscoverednodes>(n);
			d->UpdateDiscoveredNodes(schemes, t, RR_MOVE(handler), timeout);
		}


		void Discovery::NodeAnnouncePacketReceived(boost::string_ref packet)
		{
			RR_SHARED_PTR<RobotRaconteurNode> n = node.lock();
			if (!n) return;

			try
			{
				std::string seed = "Robot Raconteur Node Discovery Packet";
				if (packet.substr(0, seed.length()) == seed)
				{
					std::vector<std::string> lines;
					boost::split(lines, packet, boost::is_from_range('\n', '\n'));

					std::vector<std::string> idline;
					boost::split(idline, lines.at(1), boost::is_from_range(',', ','));

					RobotRaconteur::NodeID nodeid(idline.at(0));

					std::string nodename;
					if (idline.size() > 1)
					{
						nodename = idline.at(1);
					}

					std::string url = lines.at(2);

					//If the URL or nodename is excessively long, just ignore it
					if (url.size() > 256)
					{
						ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Discovery, -1, "Parsing discovery text packet failed: invalid URL");
						return;
					}
					if (nodename.size() > 128)
					{
						nodename.clear();
					}

					try
					{
						ParseConnectionURLResult u = ParseConnectionURL(url);
						if (u.nodeid != nodeid)
						{
							ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Discovery, -1, "Parsing discovery text packet failed: invalid URL");
							return;
						}
					}
					catch (ConnectionException& exp2)
					{
						ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Discovery, -1, "Parsing discovery text packet URL failed: " << exp2.what());
						return;
					}

					std::string services_nonce;

					try
					{
						for (size_t i = 3; i < lines.size(); i++)
						{
							std::string& l = lines.at(i);
							boost::trim(l);
							if (l.empty()) continue;
							boost::regex r("^\\s*([\\w+\\.\\-]+)\\s*\\:\\s*(.*)\\s*$");
							boost::smatch r_match;
							if (!boost::regex_match(l, r_match, r))
							{
								continue;
							}

							std::string attr_name = boost::trim_copy(r_match[1].str());
							std::string attr_value = boost::trim_copy(r_match[2].str());

							if (attr_name == "ServiceStateNonce" && attr_value.size() < 32)
							{
								services_nonce = attr_value;
							}
						}
					}
					catch (std::exception& exp2)
					{
						ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Discovery, -1, "Parsing discovery text packet attributes failed: " << exp2.what());
					}	

					NodeDiscoveryInfo info;
					info.NodeID = nodeid;
					info.NodeName = nodename;
					NodeDiscoveryInfoURL url1;
					url1.LastAnnounceTime = n->NowNodeTime();
					url1.URL = url;
					info.URLs.push_back(url1);
					info.ServiceStateNonce = services_nonce;

					ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Discovery, -1, "Parsing discovery text packet successful for node " << info.NodeID.ToString());

					NodeDetected(info);
				}
				else
				{
					ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Discovery, -1, "Invalid discovery text packet received");
				}

			}
			catch (std::exception& exp)
			{
				ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Discovery, -1, "Parsing discovery text packet failed: " << exp.what());
			};

			//Console.WriteLine(packet);

		}

		void Discovery::CleanDiscoveredNodes()
		{

			RR_SHARED_PTR<RobotRaconteurNode> n = node.lock();
			if (!n) return;

			{
				boost::mutex::scoped_lock lock(m_DiscoveredNodes_lock);
				boost::posix_time::ptime now = n->NowNodeTime();



				std::vector<std::string> keys;
				boost::copy(m_DiscoveredNodes | boost::adaptors::map_keys, std::back_inserter(keys));

				BOOST_FOREACH(std::string& e, keys)
				{
					try
					{
						std::vector<NodeDiscoveryInfoURL> newurls = std::vector<NodeDiscoveryInfoURL>();
						std::map<std::string, RR_SHARED_PTR<Discovery_nodestorage> >::iterator e1 = m_DiscoveredNodes.find(e);
						if (e1 == m_DiscoveredNodes.end()) continue;
						boost::mutex::scoped_lock lock2(e1->second->this_lock);
						std::vector<NodeDiscoveryInfoURL> urls = e1->second->info->URLs;
						BOOST_FOREACH(NodeDiscoveryInfoURL& u, urls)
						{

							int64_t time = (now - u.LastAnnounceTime).total_milliseconds();
							if (time < 60000)
							{
								newurls.push_back(u);
							}
						}

						e1->second->info->URLs = newurls;

						if (newurls.empty())
						{
							ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Discovery, -1, "Discovery lost node " << e1->second->info->NodeID.ToString());

							RR_SHARED_PTR<Discovery_nodestorage> e2 = e1->second;

							BOOST_FOREACH(RR_WEAK_PTR<IServiceSubscription> s, subscriptions)
							{
								RR_SHARED_PTR<IServiceSubscription> s1 = s.lock();
								if (!s1) continue;
								try
								{
									s1->NodeLost(e2);
								}
								catch (std::exception&) {}
							}

							lock2.unlock();
							m_DiscoveredNodes.erase(e1);

							RobotRaconteurNode::TryPostToThreadPool(node, boost::bind(&RobotRaconteurNode::FireNodeLost, n, e2->info));
						}
					}
					catch (std::exception& exp2)
					{
						ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Discovery, -1, "Error cleaning node " << e << ": " << exp2.what());
					}
				}
			}
		}

		uint32_t Discovery::GetNodeDiscoveryMaxCacheCount()
		{
			boost::mutex::scoped_lock lock(m_DiscoveredNodes_lock);
			return max_DiscoveredNodes;
		}
		void Discovery::SetNodeDiscoveryMaxCacheCount(uint32_t count)
		{
			boost::mutex::scoped_lock lock(m_DiscoveredNodes_lock);
			max_DiscoveredNodes.data() = count;
			ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Discovery, -1, "NodeDiscoveryMaxCacheCount set to " << count);
		}


		void Discovery::AsyncFindServiceByType(boost::string_ref servicetype, const std::vector<std::string>& transportschemes, boost::function<void(RR_SHARED_PTR<std::vector<ServiceInfo2> >) >& handler, int32_t timeout)
		{
			RR_SHARED_PTR<RobotRaconteurNode> n = node.lock();
			if (!n) throw InvalidOperationException("Node has been released");


			std::list<std::vector<std::string> > all_urls;

			std::vector<ServiceInfo2> services;
			std::vector<std::string> nodeids;

			{
				boost::mutex::scoped_lock lock(m_DiscoveredNodes_lock);
				boost::copy(m_DiscoveredNodes | boost::adaptors::map_keys, std::back_inserter(nodeids));
			}
			for (size_t i = 0; i < nodeids.size(); i++)
			{
				boost::mutex::scoped_lock lock(m_DiscoveredNodes_lock);

				try
				{
					std::vector<std::string> urls = std::vector<std::string>();
					std::map<std::string, RR_SHARED_PTR<Discovery_nodestorage> >::iterator e1 = m_DiscoveredNodes.find(nodeids.at(i));
					if (e1 == m_DiscoveredNodes.end()) continue;
					boost::mutex::scoped_lock lock2(e1->second->this_lock);
					BOOST_FOREACH(NodeDiscoveryInfoURL& url, e1->second->info->URLs)
					{
						BOOST_FOREACH(const std::string& e, transportschemes)
						{
							std::string t2 = (e + "://");
							if (boost::starts_with(url.URL, t2))
							{
								urls.push_back(url.URL);
								break;
							}
						}
					}

					if (!urls.empty()) all_urls.push_back(urls);
				}
				catch (std::exception& exp)
				{
					ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Discovery, -1, "Error searching for candidate URLs for FindServiceByType: " << exp.what());
				}
			}

			RR_SHARED_PTR<Discovery_findservicebytype> f = RR_MAKE_SHARED<Discovery_findservicebytype>(n);
			f->AsyncFindServiceByType(servicetype, transportschemes, RR_MOVE(handler), timeout);

		}

		std::vector<ServiceInfo2> Discovery::FindServiceByType(boost::string_ref servicetype, const std::vector<std::string>& transportschemes)
		{
			ROBOTRACONTEUR_ASSERT_MULTITHREADED(node);

			RR_SHARED_PTR<detail::sync_async_handler<std::vector<ServiceInfo2> > > t = RR_MAKE_SHARED<detail::sync_async_handler<std::vector<ServiceInfo2> > >();
			boost::function< void(RR_SHARED_PTR<std::vector<ServiceInfo2> >) > h = boost::bind(&detail::sync_async_handler<std::vector<ServiceInfo2> >::operator(), t, RR_BOOST_PLACEHOLDERS(_1), RR_SHARED_PTR<RobotRaconteurException>());
			AsyncFindServiceByType(servicetype, transportschemes, h);
			return *t->end();
		}

		std::vector<NodeInfo2> Discovery::FindNodeByID(const RobotRaconteur::NodeID& id, const std::vector<std::string>& transportschemes)
		{
			ROBOTRACONTEUR_ASSERT_MULTITHREADED(node);

			RR_SHARED_PTR<detail::sync_async_handler<std::vector<NodeInfo2> > > n = RR_MAKE_SHARED<detail::sync_async_handler<std::vector<NodeInfo2> > >();
			boost::function< void(RR_SHARED_PTR<std::vector<NodeInfo2> >) > h = boost::bind(&detail::sync_async_handler<std::vector<NodeInfo2> >::operator(), n, RR_BOOST_PLACEHOLDERS(_1), RR_SHARED_PTR<RobotRaconteurException>());
			AsyncFindNodeByID(id, transportschemes, h);
			return *n->end();
		}

		void Discovery::AsyncFindNodeByID(const RobotRaconteur::NodeID& id, const std::vector<std::string>& transportschemes, boost::function< void(RR_SHARED_PTR<std::vector<NodeInfo2> >) >& handler, int32_t timeout)
		{
			ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Discovery, -1, "Begin AsyncFindNodeByID for remote node " << id.ToString());
			RobotRaconteur::NodeID id1 = id;

			boost::function<void()> h = boost::bind(&Discovery::EndAsyncFindNodeByID, shared_from_this(), id1, transportschemes, handler);
			AsyncUpdateDetectedNodes(transportschemes, h, timeout);
		}

		void Discovery::EndAsyncFindNodeByID(RobotRaconteur::NodeID id, std::vector<std::string> transportschemes, boost::function< void(RR_SHARED_PTR<std::vector<NodeInfo2> >) >& handler)
		{
			ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Discovery, -1, "AsyncFindNodeByID for remote node " << id.ToString() << " update complete");

			RR_SHARED_PTR<std::vector<NodeInfo2> > ret = RR_MAKE_SHARED<std::vector<NodeInfo2> >();
			std::string sid = id.ToString();
			{
				boost::mutex::scoped_lock lock(m_DiscoveredNodes_lock);
				std::map<std::string, RR_SHARED_PTR<Discovery_nodestorage> >::iterator e1 = m_DiscoveredNodes.find(sid);
				if (e1 != m_DiscoveredNodes.end())
				{
					boost::mutex::scoped_lock lock2(e1->second->this_lock);
					RR_SHARED_PTR<NodeDiscoveryInfo> ni = e1->second->info;
					NodeInfo2 n;
					n.NodeID = NodeID(sid);
					n.NodeName = ni->NodeName;

					BOOST_FOREACH(NodeDiscoveryInfoURL& url, ni->URLs)
					{
						BOOST_FOREACH(std::string& e, transportschemes)
						{
							try
							{

								ParseConnectionURLResult u = ParseConnectionURL(url.URL);

								if (u.scheme == e)
								{
									std::string short_url;
									if (u.port == -1)
									{
										short_url = u.scheme + "://" + u.host + u.path + "?nodeid=" + boost::replace_all_copy(boost::replace_all_copy(u.nodeid.ToString(), "{", ""), "}", "");
									}
									else
									{
										short_url = u.scheme + "://" + u.host + ":" + boost::lexical_cast<std::string>(u.port) + u.path + "?nodeid=" + boost::replace_all_copy(boost::replace_all_copy(u.nodeid.ToString(), "{", ""), "}", "");
									}
									n.ConnectionURL.push_back(short_url);
									break;
								}
							}
							catch (std::exception& exp2)
							{
								ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Discovery, -1, "AsyncFindNodeByID for remote node " << id.ToString() 
									<< " failed processing candidate: " << exp2.what());
							}
						}
					}

					if (!n.ConnectionURL.empty())
					{
						ret->push_back(n);
					}

				}
			}

			ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Discovery, -1, "AsyncFindNodeByID for remote node " << id.ToString()
				<< " completed successfully with " << ret->size() << " candidates");

			detail::InvokeHandler(node, handler, ret);

		}

		std::vector<NodeInfo2> Discovery::FindNodeByName(boost::string_ref name, const std::vector<std::string>& transportschemes)
		{
			ROBOTRACONTEUR_ASSERT_MULTITHREADED(node);

			RR_SHARED_PTR<detail::sync_async_handler<std::vector<NodeInfo2> > > n = RR_MAKE_SHARED<detail::sync_async_handler<std::vector<NodeInfo2> > >();
			boost::function< void(RR_SHARED_PTR<std::vector<NodeInfo2> >) > h = boost::bind(&detail::sync_async_handler<std::vector<NodeInfo2> >::operator(), n, RR_BOOST_PLACEHOLDERS(_1), RR_SHARED_PTR<RobotRaconteurException>());
			AsyncFindNodeByName(name, transportschemes, h);
			return *n->end();
		}

		void Discovery::AsyncFindNodeByName(boost::string_ref name, const std::vector<std::string>& transportschemes, boost::function< void(RR_SHARED_PTR<std::vector<NodeInfo2> >) >& handler, int32_t timeout)
		{
			ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Discovery, -1, "Begin AsyncFindNodeByName for remote node \"" << name << "\"");
			boost::function<void()> h = boost::bind(&Discovery::EndAsyncFindNodeByName, shared_from_this(), name.to_string(), transportschemes, handler);
			AsyncUpdateDetectedNodes(transportschemes, h, timeout);
		}


		void Discovery::EndAsyncFindNodeByName(std::string name, std::vector<std::string> transportschemes, boost::function< void(RR_SHARED_PTR<std::vector<NodeInfo2> >) >& handler)
		{
			ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Discovery, -1, "AsyncFindNodeByName for remote node \"" << name << "\" update complete");
			RR_SHARED_PTR<std::vector<NodeInfo2> > ret = RR_MAKE_SHARED<std::vector<NodeInfo2> >();

			{
				boost::mutex::scoped_lock lock(m_DiscoveredNodes_lock);
				BOOST_FOREACH(RR_SHARED_PTR<Discovery_nodestorage>& e, m_DiscoveredNodes | boost::adaptors::map_values)
				{
					boost::mutex::scoped_lock lock2(e->this_lock);
					if (e->info->NodeName == name)
					{
						NodeInfo2 n;
						n.NodeID = e->info->NodeID;
						n.NodeName = e->info->NodeName;

						BOOST_FOREACH(NodeDiscoveryInfoURL& url, e->info->URLs)
						{
							BOOST_FOREACH(std::string& e, transportschemes)
							{
								try
								{

									ParseConnectionURLResult u = ParseConnectionURL(url.URL);

									if (u.scheme == e)
									{
										std::string short_url;
										if (u.port == -1)
										{
											short_url = u.scheme + "://" + u.host + u.path + "?nodeid=" + boost::replace_all_copy(boost::replace_all_copy(u.nodeid.ToString(), "{", ""), "}", "");
										}
										else
										{
											short_url = u.scheme + "://" + u.host + ":" + boost::lexical_cast<std::string>(u.port) + u.path + "?nodeid=" + boost::replace_all_copy(boost::replace_all_copy(u.nodeid.ToString(), "{", ""), "}", "");
										}
										n.ConnectionURL.push_back(short_url);
										break;
									}
								}
								catch (std::exception& exp2)
								{
									ROBOTRACONTEUR_LOG_DEBUG_COMPONENT(node, Discovery, -1, "AsyncFindNodeByID for remote node \"" << name 
									<< "\" failed processing candidate: " << exp2.what());
								}
							}
						}

						if (!n.ConnectionURL.empty())
						{
							ret->push_back(n);
						}
					}

				}
			}

			ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Discovery, -1, "AsyncFindNodeByID for remote node \"" << name
				<< "\" completed successfully with " << ret->size() << " candidates");

			detail::InvokeHandler(node, handler, ret);

		}

		RR_SHARED_PTR<RobotRaconteurNode> Discovery::GetNode()
		{
			RR_SHARED_PTR<RobotRaconteurNode> n = node.lock();
			if (!n) throw InvalidOperationException("Node has been released");
			return n;
		}

		RR_SHARED_PTR<ServiceSubscription> Discovery::SubscribeService(const std::vector<std::string>& url, boost::string_ref username, RR_INTRUSIVE_PTR<RRMap<std::string,RRValue> > credentials,  boost::string_ref objecttype)
		{
			RR_SHARED_PTR<ServiceSubscription> s = RR_MAKE_SHARED<ServiceSubscription>(shared_from_this());
			s->InitServiceURL(url,username,credentials,objecttype);
			return s;
		}		

		RR_SHARED_PTR<ServiceSubscription> Discovery::SubscribeServiceByType(const std::vector<std::string>& service_types, RR_SHARED_PTR<ServiceSubscriptionFilter> filter)
		{
			RR_SHARED_PTR<ServiceSubscription> s = RR_MAKE_SHARED<ServiceSubscription>(shared_from_this());
			DoSubscribe(service_types, filter, s);
			return s;
		}

		RR_SHARED_PTR<ServiceInfo2Subscription> Discovery::SubscribeServiceInfo2(const std::vector<std::string>& service_types, RR_SHARED_PTR<ServiceSubscriptionFilter> filter)
		{
			RR_SHARED_PTR<ServiceInfo2Subscription> s = RR_MAKE_SHARED<ServiceInfo2Subscription>(shared_from_this());
			DoSubscribe(service_types, filter, s);
			return s;
		}

		void Discovery::DoSubscribe(const std::vector<std::string>& service_types, RR_SHARED_PTR<ServiceSubscriptionFilter> filter, RR_SHARED_PTR<IServiceSubscription> s)
		{

			ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Discovery, -1, "Begin DoSubscribe for service types " << boost::join(service_types, ", "));

			boost::mutex::scoped_lock lock(m_DiscoveredNodes_lock);
			subscriptions.push_back(s);
			s->Init(service_types, filter);

			std::vector<RR_SHARED_PTR<Discovery_nodestorage> > storage;
			boost::range::copy(m_DiscoveredNodes | boost::adaptors::map_values, std::back_inserter(storage));
			
			lock.unlock();

			BOOST_FOREACH(RR_SHARED_PTR<Discovery_nodestorage>& n, storage)
			{
				boost::mutex::scoped_lock(n->this_lock);

				if (n->last_update_nonce != n->info->ServiceStateNonce || n->info->ServiceStateNonce.empty())
				{
					try
					{
						ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Discovery, -1, "Subscription requesting update for node " << n->info->NodeID.ToString());
						RetryUpdateServiceInfo(n);
					}
					catch (std::exception& exp)
					{
						RobotRaconteurNode::TryHandleException(node, &exp);
					}
				}

				s->NodeUpdated(n);
			}			

		}

		void Discovery::SubscriptionClosed(RR_SHARED_PTR<IServiceSubscription> subscription)
		{
			boost::mutex::scoped_lock lock(m_DiscoveredNodes_lock);
			
			ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Discovery, -1, "Subscription closed");

			for (std::list<RR_WEAK_PTR<IServiceSubscription> >::iterator e = subscriptions.begin(); e != subscriptions.end(); )
			{
				RR_SHARED_PTR<IServiceSubscription> s = e->lock();
				if (!s)
				{
					e = subscriptions.erase(e);
					continue;
				}

				if (s == subscription)
				{
					e = subscriptions.erase(e);
				}
				else
				{
					e++;
				}				
			}

		}

		void Discovery::Shutdown()
		{
			std::list<RR_WEAK_PTR<IServiceSubscription> > subscriptions1;

			{
				boost::mutex::scoped_lock lock(m_DiscoveredNodes_lock);
				is_shutdown.data() = true;
				subscriptions = RR_MOVE(subscriptions1);
			}

			BOOST_FOREACH(RR_WEAK_PTR<IServiceSubscription>& s, subscriptions1)
			{
				RR_SHARED_PTR<IServiceSubscription> s1 = s.lock();
				if (s1)
				{
					s1->Close();
				}
			}

			ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Discovery, -1, "Discovery shut down");

		}
		
	}
}
