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

#include "RobotRaconteurNode_connector_private.h"

#include <boost/foreach.hpp>
#include <boost/range/adaptors.hpp>
#include <boost/range/algorithm.hpp>
#include <boost/algorithm/string.hpp>

#include "RobotRaconteur/Client.h"
#include "RobotRaconteur/Service.h"

namespace RobotRaconteur
{
	namespace detail
	{

		static void RobotRaconteurNode_connector_empty_handler() {}


		static void RobotRaconteurNode_connector_empty_handler(RR_SHARED_PTR<RobotRaconteurException>) {}
			
		RobotRaconteurNode_connector::endpoint_cleanup::endpoint_cleanup(RR_SHARED_PTR<ClientContext> ep, RR_SHARED_PTR<RobotRaconteurNode> node)
		{
			this->ep = ep;
			this->node = node;
		}

		RobotRaconteurNode_connector::endpoint_cleanup::~endpoint_cleanup()
		{
			RR_SHARED_PTR<ClientContext> ep1;
			{
				boost::mutex::scoped_lock lock(eplock);
				ep1 = ep;
				ep.reset();
			}

			if (!ep1) return;
			try
			{
				node->DeleteEndpoint(ep1);
			}
			catch (std::exception&) {}
		}

		void RobotRaconteurNode_connector::endpoint_cleanup::release()
		{
			boost::mutex::scoped_lock lock(eplock);
			ep.reset();
		}			

		

		RobotRaconteurNode_connector::RobotRaconteurNode_connector(RR_SHARED_PTR<RobotRaconteurNode> node)
		{
			active_count = 0;
			connecting = true;
			transport_connected = false;
			this->node = node;
			delay_event = node->CreateAutoResetEvent();
		}
		
		void RobotRaconteurNode_connector::handle_error(const int32_t& key, RR_SHARED_PTR<RobotRaconteurException> err)
		{
			//std::cout << "Got error" << std::endl;
			{
				boost::mutex::scoped_lock lock(connecting_lock);
				if (!connecting) return;
			}


			{
				boost::mutex::scoped_lock lock(active_lock);
				active.remove(key);
				errors.push_back(err);

				//if(active.size()!=0) return;

			}

			boost::mutex::scoped_lock lock2(handler_lock);

			{
				boost::mutex::scoped_lock lock(active_lock);
				//std::cout << active.size() << std::endl;
				if (active.size() != 0) return;

			}

			if (connect_timer)
			{
				try
				{
					connect_timer->Stop();
				}
				catch (std::exception&) {}
				connect_timer.reset();
			}


			//All activities have completed, assume failure

			bool c;
			{
				boost::mutex::scoped_lock lock(connecting_lock);
				c = connecting;
				connecting = false;
			}

			if (!c) return;

			{
				boost::mutex::scoped_lock lock(connect_timer_lock);
				try
				{
					if (connect_timer) connect_timer->Stop();
				}
				catch (std::exception&) {}

				connect_timer.reset();
			}

			delay_event->Set();
						
			detail::InvokeHandlerWithException(node, handler, err);
			
		}

		void RobotRaconteurNode_connector::connected_client(RR_SHARED_PTR<RRObject> client, RR_SHARED_PTR<RobotRaconteurException> err, RR_SHARED_PTR<endpoint_cleanup> ep, int32_t key)
		{
			//std::cout << "Connected client" << std::endl;
			if (err)
			{
				bool c;
				{
					boost::mutex::scoped_lock lock(connecting_lock);
					c = connecting;
					connecting = false;
				}

				if (connect_timer)
				{
					try
					{
						connect_timer->Stop();
					}
					catch (std::exception&) {}
					connect_timer.reset();
				}

				try
				{
					if (client)
					{
						node->AsyncDisconnectService(client, boost::bind(&RobotRaconteurNode_connector_empty_handler));
					}
				}
				catch (std::exception&)
				{

				}

				if (c)
				{
					delay_event->Set();
					detail::InvokeHandlerWithException(node, handler, err);
				}

				return;
			}

			bool c;
			{
				boost::mutex::scoped_lock lock(connecting_lock);
				c = connecting;
				connecting = false;
			}
			if (!c)
			{
				try
				{
					if (client)
					{
						node->AsyncDisconnectService(client, boost::bind(&RobotRaconteurNode_connector_empty_handler));
					}
				}
				catch (std::exception& exp)
				{
					RobotRaconteurNode::TryHandleException(node, &exp);
				}
			}
			else
			{
				{
					boost::mutex::scoped_lock lock(connect_timer_lock);
					try
					{
						if (connect_timer) connect_timer->Stop();
					}
					catch (std::exception&) {}
					connect_timer.reset();
				}

				try
				{
					delay_event->Set();
					ep->release();
					if (listener != 0)
						rr_cast<ServiceStub>(client)->GetContext()->ClientServiceListener.connect(listener);
					//std::cout << "call connect handler" << std::endl;
					detail::InvokeHandler(node, handler, client);
				}
				catch (std::exception& exp)
				{
					RobotRaconteurNode::TryHandleException(node, &exp);
				}
			}
		}

		void RobotRaconteurNode_connector::connected_transport(RR_SHARED_PTR<Transport> transport, RR_SHARED_PTR<ITransportConnection> connection, RR_SHARED_PTR<RobotRaconteurException> err, std::string url, RR_SHARED_PTR<endpoint_cleanup> ep, int32_t key)
		{

			if (err)
			{
				//std::cout << "Transport connect error" << std::endl;
				handle_error(key, err);
				return;
			}

			boost::mutex::scoped_lock lock(connecting_lock);
			//std::cout << "Transport connected" << std::endl;
			bool c;
			bool tc;
			{

				c = connecting;
				tc = transport_connected;


			}
			if (!c)
			{
				//std::cout << "Node connector close transport" << std::endl;
				try
				{
					connection->Close();
				}
				catch (std::exception&)
				{
				}
			}
			else if (tc)
			{
				boost::mutex::scoped_lock lock(active_lock);
				active.remove(key);
				try
				{
					connection->Close();
				}
				catch (std::exception&)
				{
				}
			}
			else
			{
				try
				{
					int32_t key2;
					{
						boost::mutex::scoped_lock lock(active_lock);
						active_count++;
						key2 = active_count;




						//std::cout << "Node connector connect service" << std::endl;
						ep->ep->AsyncConnectService(transport, connection, url, username, credentials, objecttype, boost::protect(boost::bind(&RobotRaconteurNode_connector::connected_client, shared_from_this(), _1, _2, ep, key2)), timeout);

						transport_connected = true;

						active.push_back(key2);
						active.remove(key);

					}
				}
				catch (RobotRaconteurException& err2)
				{
					if (transport)
					{
						try
						{
							transport->Close();
						}
						catch (std::exception&) {}
					}

					connecting_lock.unlock();
					handle_error(key, RobotRaconteurExceptionUtil::DownCastException(err2));

				}
				catch (std::exception& err2)
				{
					if (transport)
					{
						try
						{
							transport->Close();
						}
						catch (std::exception&) {}
					}
					connecting_lock.unlock();
					handle_error(key, RR_MAKE_SHARED<ConnectionException>(err2.what()));

				}

			}

		}

		void RobotRaconteurNode_connector::connect_timer_callback(const TimerEvent& e)
		{


			//if (!e.stopped) //Allow the cancellation of the timer to kill the connect attempt
			{
				{
					boost::mutex::scoped_lock lock(connecting_lock);
					if (!connecting) return;
					connecting = false;
				}

				boost::mutex::scoped_lock lock2(handler_lock);

				if (connect_timer)
				{
					try
					{
						connect_timer->Stop();
					}
					catch (std::exception&) {}
					connect_timer.reset();
				}

				if (delay_event)
				{
					delay_event->Set();
				}
								
				detail::InvokeHandlerWithException(node, handler, RR_MAKE_SHARED<ConnectionException>("Connection timed out"));
				
			}


		}
		
		void RobotRaconteurNode_connector::connect(const std::map<std::string, RR_WEAK_PTR<Transport> >& connectors, const std::string &username, RR_INTRUSIVE_PTR<RRMap<std::string, RRValue> > credentials, boost::function<void(RR_SHARED_PTR<ClientContext>, ClientServiceListenerEventType, RR_SHARED_PTR<void>)> listener, const std::string& objecttype, boost::function<void(RR_SHARED_PTR<RRObject>, RR_SHARED_PTR<RobotRaconteurException>)> handler, int32_t timeout)
		{

			this->connectors = connectors;
			this->username = username;
			this->credentials = credentials;
			this->listener = listener;
			this->objecttype = objecttype;
			this->handler = handler;
			this->timeout = timeout;

			int32_t key;
			{
				boost::mutex::scoped_lock lock(active_lock);
				active_count++;
				key = active_count;
				active.push_back(key);
			}

			try
			{
				boost::mutex::scoped_lock lock2(handler_lock);

				size_t active_started = 0;

				std::vector<std::string> urls;
				boost::copy(connectors | boost::adaptors::map_keys, std::back_inserter(urls));

				while (urls.size() > 0)
				{
					std::string url = node->SelectRemoteNodeURL(urls);
					urls.erase(std::remove(urls.begin(), urls.end(), url), urls.end());

					RR_SHARED_PTR<Transport> t = connectors.at(url).lock();
					if (!t) continue;
					int32_t key2;

					try
					{
						RR_SHARED_PTR<ClientContext> c = RR_MAKE_SHARED<ClientContext>(node);
						node->RegisterEndpoint(rr_cast<Endpoint>(c));
						RR_SHARED_PTR<endpoint_cleanup> ep = RR_MAKE_SHARED<endpoint_cleanup>(c, node);

						boost::mutex::scoped_lock lock(active_lock);
						active_count++;
						key2 = active_count;

						boost::function<void(RR_SHARED_PTR<ITransportConnection>, RR_SHARED_PTR<RobotRaconteurException>) > h = boost::protect(boost::bind(&RobotRaconteurNode_connector::connected_transport, shared_from_this(), t, _1, _2, url, ep, key2));
						t->AsyncCreateTransportConnection(url, c, h);

						active.push_back(key2);
						active_started++;
					}
					catch (std::exception&)
					{
						if (urls.empty())
						{
							throw;
						}
					}

					if (urls.size() == 0) break;

					{
						boost::mutex::scoped_lock lock(connecting_lock);
						if (!connecting) break;
					}

					//node->Sleep(boost::posix_time::milliseconds(15));
					delay_event->WaitOne(15);

					{
						boost::mutex::scoped_lock lock(connecting_lock);
						if (!connecting) break;
					}
				}

				if (active_started == 0) throw ConnectionException("Could not connect to service");

				if (timeout != RR_TIMEOUT_INFINITE)
				{
					boost::mutex::scoped_lock lock(connect_timer_lock);
					connect_timer = node->CreateTimer(boost::posix_time::milliseconds(timeout), boost::bind(&RobotRaconteurNode_connector::connect_timer_callback, shared_from_this(), _1), true);
					connect_timer->Start();
				}

				{
					boost::mutex::scoped_lock lock(active_lock);
					active.remove(key);
				}

			}
			catch (RobotRaconteurException& err2)
			{
				handle_error(key, RobotRaconteurExceptionUtil::DownCastException(err2));
			}
			catch (std::exception& err2)
			{
				handle_error(key, RR_MAKE_SHARED<ConnectionException>(err2.what()));
			}
		}		

	}

}

