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

#include "RobotRaconteur/TcpTransport.h"
#include "TcpTransport_private.h"
#include <boost/algorithm/string.hpp>
#include <boost/shared_array.hpp>
#include <RobotRaconteur/ServiceIndexer.h>
#include "RobotRaconteur/StringTable.h"
#include "LocalTransport_private.h"

#include <set>

#ifdef ROBOTRACONTEUR_WINDOWS
#include <IPHlpApi.h>
#include <Shlobj.h>
#define SHUT_RDWR SD_BOTH
#undef SendMessage
#else
#define closesocket close
#define SOCKET_ERROR -1
#define INVALID_SOCKET -1
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <signal.h>
#include <netdb.h>
#include <sys/socket.h>
#include <ifaddrs.h>
#endif

#ifdef ROBOTRACONTEUR_APPLE
#include <CoreFoundation.h>
#include <Security.h>
#endif

#include <boost/range/adaptors.hpp>
#include <boost/range/algorithm.hpp>
#include <boost/foreach.hpp>

namespace RobotRaconteur
{

namespace detail
{
	TcpConnector::TcpConnector(RR_SHARED_PTR<TcpTransport> parent)
	{
		this->parent = parent;
		connecting = false;
		active_count = 0;
		socket_connected = false;

		connect_timer = RR_MAKE_SHARED<boost::asio::deadline_timer>(boost::ref(parent->GetNode()->GetThreadPool()->get_io_service()));
		node = parent->GetNode();

	}

	void TcpConnector::Connect(std::vector<std::string> url, uint32_t endpoint, boost::function<void(RR_SHARED_PTR<ITransportConnection>, RR_SHARED_PTR<RobotRaconteurException>) > handler)
	{
		this->callback = handler;
		this->endpoint = endpoint;
		this->url = url.at(0);
		
		{
			boost::mutex::scoped_lock lock(this_lock);
			connecting = true;			
			_resolver = RR_MAKE_SHARED<boost::asio::ip::tcp::resolver>(boost::ref(parent->GetNode()->GetThreadPool()->get_io_service()));
			parent->AddCloseListener(_resolver, &boost::asio::ip::tcp::resolver::cancel);
		}

		std::vector<boost::asio::ip::tcp::resolver::query> queries;

		BOOST_FOREACH (std::string& e, url)
		{
			try
			{

				ParseConnectionURLResult url_res = ParseConnectionURL(e);


				/*boost::smatch url_result;
				boost::regex_search(*e,url_result,boost::regex("^([^:]+)://((?:\\[[A-Fa-f0-9:]+(?:\\%\\w*)?\\])|(?:[^\\[\\]\\:]+)):([^:/]+)/?(.*)$"));

				if (url_result.size()<4) throw InvalidArgumentException("Malformed URL");

				if (url_result[1]!="tcp" && url_result[1]!="tcps") throw InvalidArgumentException("Invalid transport type for TcpTransport");*/

				if (url_res.scheme != "tcp" && url_res.scheme != "rr+tcp" && url_res.scheme != "rrs+tcp") throw InvalidArgumentException("Invalid transport type for TcpTransport");
				if (url_res.host=="") throw ConnectionException("Invalid host for tcp transport");
				if (url_res.path != "" && url_res.path != "/") throw ConnectionException("Invalid host for tcp transport");
				std::string host = url_res.host;
				std::string port = boost::lexical_cast<std::string>(url_res.port);

				boost::trim_left_if(host, boost::is_from_range('[', '['));
				boost::trim_right_if(host, boost::is_from_range(']', ']'));
				queries.push_back(boost::asio::ip::tcp::resolver::query(host, port, boost::asio::ip::resolver_query_base::flags()));
			}
			catch (std::exception&)
			{
				if (url.size() == 1) throw;

			}
		}

		if (queries.size() == 0) throw ConnectionException("Could not find route to supplied address");
		
		{
			{
				{
					boost::mutex::scoped_lock lock(this_lock);
					if (!connecting) return;

					connect_timer->expires_from_now(boost::posix_time::milliseconds(parent->GetDefaultConnectTimeout()));
					RobotRaconteurNode::asio_async_wait(node, connect_timer, boost::bind(&TcpConnector::connect_timer_callback, shared_from_this(), boost::asio::placeholders::error));

					parent->AddCloseListener(connect_timer, boost::bind(&boost::asio::deadline_timer::cancel, _1));
				}

				BOOST_FOREACH(boost::asio::ip::tcp::resolver::query& e, queries)
				{
					int32_t key2;
					{
						boost::mutex::scoped_lock lock(this_lock);
						active_count++;
						key2 = active_count;
						
						RobotRaconteurNode::asio_async_resolve(node, _resolver, e, boost::bind(&TcpConnector::connect2, shared_from_this(), key2, boost::asio::placeholders::error, boost::asio::placeholders::iterator, callback));
						//std::cout << "Begin resolve" << std::endl;
						
						active.push_back(key2);
					}
				}
			}
		}
	}


	void TcpConnector::connect2(int32_t key, const boost::system::error_code& err, boost::asio::ip::tcp::resolver::iterator endpoint_iterator, boost::function<void(RR_SHARED_PTR<TcpTransportConnection>, RR_SHARED_PTR<RobotRaconteurException>) > callback)
	{
		if (err)
		{
			handle_error(key, err);
			return;
		}
		//std::cout << "End resolve" << std::endl;
		try
		{

			std::vector<boost::asio::ip::tcp::endpoint> ipv4;
			std::vector<boost::asio::ip::tcp::endpoint> ipv6;

			boost::asio::ip::tcp::resolver::iterator end;

			for (; endpoint_iterator != end; endpoint_iterator++)
			{
				if (endpoint_iterator->endpoint().address().is_v4()) ipv4.push_back(endpoint_iterator->endpoint());
				if (endpoint_iterator->endpoint().address().is_v6()) ipv6.push_back(endpoint_iterator->endpoint());
			}

			if (ipv4.size() == 0 && ipv6.size() == 0)
			{
				handle_error(key, boost::system::error_code(boost::system::errc::bad_address, boost::system::generic_category()));
				return;
			}


			BOOST_FOREACH (boost::asio::ip::tcp::endpoint& e, ipv4)
			{
				RR_SHARED_PTR<boost::asio::ip::tcp::socket> sock = RR_MAKE_SHARED<boost::asio::ip::tcp::socket>(boost::ref(parent->GetNode()->GetThreadPool()->get_io_service()));

				int32_t key2;
				{
					boost::mutex::scoped_lock lock(this_lock);
					if (!connecting) return;

					active_count++;
					key2 = active_count;

					RR_SHARED_PTR<boost::signals2::scoped_connection> sock_closer
						= RR_MAKE_SHARED<boost::signals2::scoped_connection>(
							parent->AddCloseListener(sock, boost::bind(&boost::asio::ip::tcp::socket::close, _1))
							);

					RobotRaconteurNode::asio_async_connect(node,sock, e, boost::bind(&TcpConnector::connected_callback, shared_from_this(), sock, sock_closer, key2, boost::asio::placeholders::error));
					//std::cout << "Start connect " << e->address() << ":" << e->port() << std::endl;
					
					active.push_back(key2);
				}
				boost::this_thread::sleep(boost::posix_time::milliseconds(5));								
			}


			std::vector<boost::asio::ip::address> local_ip;

			TcpTransport::GetLocalAdapterIPAddresses(local_ip);

			std::vector<uint32_t> scopeids;
			BOOST_FOREACH (boost::asio::ip::address& ee, local_ip)
			{
				if (ee.is_v6())
				{
					boost::asio::ip::address_v6 a6 = ee.to_v6();
					if (a6.is_link_local())
					{
						if (std::find(scopeids.begin(), scopeids.end(), a6.scope_id()) == scopeids.end())
						{
							scopeids.push_back(a6.scope_id());
						}
					}
				}
			}


			BOOST_FOREACH (boost::asio::ip::tcp::endpoint& e, ipv6)
			{

				std::vector<boost::asio::ip::tcp::endpoint> ipv62;

				if (!e.address().is_v6()) continue;

				boost::asio::ip::address_v6 addr = e.address().to_v6();
				uint16_t port = e.port();

				if (!addr.is_link_local() || (addr.is_link_local() && addr.scope_id() != 0))
				{
					ipv62.push_back(e);
				}
				else
				{
					//Link local address with no scope id, we need to try them all...

					BOOST_FOREACH (uint32_t e3, scopeids)
					{
						boost::asio::ip::address_v6 addr3 = addr;
						addr3.scope_id(e3);
						ipv62.push_back(boost::asio::ip::tcp::endpoint(addr3, port));
					}
				}


				BOOST_FOREACH (boost::asio::ip::tcp::endpoint& e2, ipv62)
				{

					RR_SHARED_PTR<boost::asio::ip::tcp::socket> sock = RR_MAKE_SHARED<boost::asio::ip::tcp::socket>(boost::ref(parent->GetNode()->GetThreadPool()->get_io_service()));

					int32_t key2;
					{
						boost::mutex::scoped_lock lock(this_lock);
						if (!connecting) return;

						active_count++;
						key2 = active_count;
						
						RR_SHARED_PTR<boost::signals2::scoped_connection> sock_closer =
							RR_MAKE_SHARED<boost::signals2::scoped_connection>(
								parent->AddCloseListener(sock, boost::bind(&boost::asio::ip::tcp::socket::close, _1))
								);

						RobotRaconteurNode::asio_async_connect(node, sock, e2, boost::bind(&TcpConnector::connected_callback, shared_from_this(), sock, sock_closer, key2, boost::asio::placeholders::error));
						
						active.push_back(key2);
					}

					//std::cout << "Start connect [" << e2->address() << "]:" << e2->port() << std::endl;

					boost::this_thread::sleep(boost::posix_time::milliseconds(5));					
				}
			}



			boost::mutex::scoped_lock lock(this_lock);
			active.remove(key);
		}
		catch (std::exception&)
		{
			handle_error(key, boost::system::error_code(boost::system::errc::io_error, boost::system::generic_category()));
			return;
		}


		bool all_stopped = false;
		{
			boost::mutex::scoped_lock lock(this_lock);
			all_stopped = active.size() == 0;
		}

		if (all_stopped)
		{
			boost::mutex::scoped_lock lock(this_lock);
			if (!connecting) return;
			connecting = false;
		}

		if (all_stopped)
		{
			if (errors.size() == 0)
			{
				callback(RR_SHARED_PTR<TcpTransportConnection>(), RR_MAKE_SHARED<ConnectionException>("Could not connect to remote node"));
				return;
			}

			BOOST_FOREACH (RR_SHARED_PTR<RobotRaconteurException>& e, errors)
			{
				RR_SHARED_PTR<NodeNotFoundException> e2 = RR_DYNAMIC_POINTER_CAST<NodeNotFoundException>(e);
				if (e2)
				{
					callback(RR_SHARED_PTR<TcpTransportConnection>(), e2);
					return;
				}
			}

			BOOST_FOREACH (RR_SHARED_PTR<RobotRaconteurException>& e, errors)
			{
				RR_SHARED_PTR<AuthenticationException> e2 = RR_DYNAMIC_POINTER_CAST<AuthenticationException>(e);
				if (e2)
				{
					callback(RR_SHARED_PTR<TcpTransportConnection>(), e2);
					return;
				}
			}

			callback(RR_SHARED_PTR<TcpTransportConnection>(), errors.back());
		}




	}

	void TcpConnector::connected_callback(RR_SHARED_PTR<boost::asio::ip::tcp::socket> socket, RR_SHARED_PTR<boost::signals2::scoped_connection> socket_closer, int32_t key, const boost::system::error_code& error)
	{
		if (error)
		{
			handle_error(key, error);
			return;
		}
		try
		{
			boost::mutex::scoped_lock lock(this_lock);
			bool c = connecting;
						
			if (!c)
			{
				/*std:: cout << "closing" << std::endl;
				try
				{
				socket->set_option(boost::asio::ip::tcp::socket::linger(true,5));
				}
				catch (...) {}
				try
				{
				socket->shutdown(boost::asio::ip::tcp::socket::shutdown_both);
				}
				catch(...) {}
				try
				{
				socket->close();
				}
				catch(...) {}*/

				return;
			}

			int32_t key2;
			{				
				active_count++;
				key2 = active_count;
				socket_connected = true;

				socket->set_option(boost::asio::ip::tcp::socket::linger(true, 5));
				boost::function<void(RR_SHARED_PTR<boost::asio::ip::tcp::socket>, RR_SHARED_PTR<ITransportConnection>, RR_SHARED_PTR<RobotRaconteurException>)> cb = boost::bind(&TcpConnector::connected_callback2, shared_from_this(), _1, key2, _2, _3);
				TcpTransport_attach_transport(parent, socket, url, false, endpoint, cb);
				
				active.push_back(key2);
			}
			
			active.remove(key);
		}
		catch (std::exception&)
		{
			handle_error(key, boost::system::error_code(boost::system::errc::io_error, boost::system::generic_category()));
		}
	}	

	void TcpConnector::connected_callback2(RR_SHARED_PTR<boost::asio::ip::tcp::socket> socket, int32_t key, RR_SHARED_PTR<ITransportConnection> connection, RR_SHARED_PTR<RobotRaconteurException> err)
	{
		if (err)
		{
			if (connection)
			{
				try
				{
					connection->Close();
				}
				catch (std::exception&) {}
			}
			//callback(RR_SHARED_PTR<TcpTransportConnection>(),err);
			handle_error(key, err);
			return;

		}
		try
		{
			{


				bool c;
				{
					boost::mutex::scoped_lock lock(this_lock);
					c = connecting;
					connecting = false;
				}
				if (!c)
				{

					try
					{
						//std::cout << "Closing 2" << std::endl;
						connection->Close();
					}
					catch (std::exception&) {}

					return;
				}

			}
			parent->register_transport(connection);

			{
				boost::mutex::scoped_lock lock(this_lock);
				if (connect_timer) connect_timer->cancel();
				connect_timer.reset();
			}

			try
			{

				//std::cout << "connect callback" << std::endl;
				callback(boost::dynamic_pointer_cast<ITransportConnection>(connection), RR_SHARED_PTR<RobotRaconteurException>());
			}
			catch (std::exception& exp)
			{
				RobotRaconteurNode::TryHandleException(node, &exp);
			}
		}
		catch (std::exception&)
		{
			handle_error(key, boost::system::error_code(boost::system::errc::io_error, boost::system::generic_category()));
		}

	}
	
	void TcpConnector::connect_timer_callback(const boost::system::error_code& e)
	{


		if (e != boost::asio::error::operation_aborted)
		{
			{
				boost::mutex::scoped_lock lock(this_lock);
				if (!connecting) return;
				connecting = false;
			}
			try
			{
				callback(RR_SHARED_PTR<TcpTransportConnection>(), RR_MAKE_SHARED<ConnectionException>("Connection timed out"));
			}
			catch (std::exception& exp)
			{
				RobotRaconteurNode::TryHandleException(node, &exp);
			}
		}


	}

	void TcpConnector::handle_error(const int32_t& key, const boost::system::error_code& err)
	{
		handle_error(key, RR_MAKE_SHARED<ConnectionException>(err.message()));
	}

	void TcpConnector::handle_error(const int32_t& key, RR_SHARED_PTR<RobotRaconteurException> err)
	{
		bool s;
		bool c;
		{
			boost::mutex::scoped_lock lock(this_lock);
			if (!connecting) return;
			
			active.remove(key);
			errors.push_back(err);

			if (active.size() != 0) return;
			s = socket_connected;
			if (active.size() != 0) return;		

		//return;
		//All activities have completed, assume failure
						
			c = connecting;
			connecting = false;
			
			if (!c) return;

			connect_timer.reset();
		}

		BOOST_FOREACH (RR_SHARED_PTR<RobotRaconteurException> e, errors)
		{
			RR_SHARED_PTR<NodeNotFoundException> e2 = RR_DYNAMIC_POINTER_CAST<NodeNotFoundException>(e);
			if (e2)
			{
				callback(RR_SHARED_PTR<TcpTransportConnection>(), e2);
				return;
			}
		}

		BOOST_FOREACH (RR_SHARED_PTR<RobotRaconteurException> e, errors)
		{
			RR_SHARED_PTR<AuthenticationException> e2 = RR_DYNAMIC_POINTER_CAST<AuthenticationException>(e);
			if (e2)
			{
				callback(RR_SHARED_PTR<TcpTransportConnection>(), e2);
				return;
			}
		}

		if (!s)
		{
			try
			{
				callback(RR_SHARED_PTR<TcpTransportConnection>(), err);
			}
			catch (std::exception& exp)
			{
				RobotRaconteurNode::TryHandleException(node, &exp);
			}
		}
		else
		{
			try
			{
				callback(RR_SHARED_PTR<TcpTransportConnection>(), RR_MAKE_SHARED<ConnectionException>("Could not connect to service"));
			}
			catch (std::exception& exp)
			{
				RobotRaconteurNode::TryHandleException(node, &exp);
			}
		}
	}
	
	
	//class TcpAcceptor
	void TcpAcceptor::AcceptSocket5(const boost::system::error_code& ec,
		RR_SHARED_PTR<boost::asio::ip::tcp::socket> socket,
		RR_SHARED_PTR<websocket_stream<boost::asio::ip::tcp::socket&> > websocket,
		RR_SHARED_PTR<boost::signals2::scoped_connection> socket_closer,
		boost::function<void(RR_SHARED_PTR<boost::asio::ip::tcp::socket>, RR_SHARED_PTR<ITransportConnection>, RR_SHARED_PTR<RobotRaconteurException>)>& handler
		)
	{
		if (ec)
		{
			RR_SHARED_PTR<ConnectionException> err = RR_MAKE_SHARED<ConnectionException>("Connection closed");
			handler(RR_SHARED_PTR<boost::asio::ip::tcp::socket>(), RR_SHARED_PTR<ITransportConnection>(), err);
			return;
		}

		try
		{
			RR_SHARED_PTR<TcpTransportConnection> t = RR_MAKE_SHARED<TcpTransportConnection>(parent, url, true, local_endpoint);
			boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)> h = boost::bind(handler, socket, t, _1);
			t->AsyncAttachWebSocket(socket, websocket, h);
			parent->AddCloseListener(t, &TcpTransportConnection::Close);
		}
		catch (std::exception&)
		{
			RobotRaconteurNode::TryPostToThreadPool(parent->GetNode(),boost::bind(handler, RR_SHARED_PTR<boost::asio::ip::tcp::socket>(), RR_SHARED_PTR<TcpTransportConnection>(), RR_MAKE_SHARED<ConnectionException>("Could not connect to service")),true);
		}
	}

	void TcpAcceptor::AcceptSocket4(RR_SHARED_PTR<std::string> dat, RR_SHARED_PTR<boost::asio::ip::tcp::socket> socket, RR_SHARED_PTR<boost::signals2::scoped_connection> socket_closer)
	{

	}

	void TcpAcceptor::AcceptSocket3(const boost::system::error_code& ec, RR_SHARED_PTR<boost::asio::deadline_timer> timer,
		RR_SHARED_PTR<boost::asio::ip::tcp::socket> socket,
		RR_SHARED_PTR<boost::signals2::scoped_connection> socket_closer,
		boost::function<void(RR_SHARED_PTR<boost::asio::ip::tcp::socket>, RR_SHARED_PTR<ITransportConnection>, RR_SHARED_PTR<RobotRaconteurException>)>& handler)
	{
		socket->async_receive(boost::asio::buffer(buf, 4), boost::asio::ip::tcp::socket::message_peek,
			boost::bind(&TcpAcceptor::AcceptSocket2, shared_from_this(), boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred, socket, socket_closer, handler));
	}

	void TcpAcceptor::AcceptSocket2(const boost::system::error_code& ec, size_t n,
		RR_SHARED_PTR<boost::asio::ip::tcp::socket> socket,
		RR_SHARED_PTR<boost::signals2::scoped_connection> socket_closer,
		boost::function<void(RR_SHARED_PTR<boost::asio::ip::tcp::socket>, RR_SHARED_PTR<ITransportConnection>, RR_SHARED_PTR<RobotRaconteurException>)>& handler)
	{

		if (ec)
		{
			RR_SHARED_PTR<ConnectionException> err = RR_MAKE_SHARED<ConnectionException>("Connection closed");
			handler(RR_SHARED_PTR<boost::asio::ip::tcp::socket>(), RR_SHARED_PTR<ITransportConnection>(), err);
			return;
		}

		if (n < 4)
		{
			boost::posix_time::time_duration diff = parent->GetNode()->NowUTC() - start_time;
			if (diff.total_milliseconds() > 5000)
			{
				RR_SHARED_PTR<ConnectionException> err = RR_MAKE_SHARED<ConnectionException>("Connection timed out");
				handler(RR_SHARED_PTR<boost::asio::ip::tcp::socket>(), RR_SHARED_PTR<ITransportConnection>(), err);
				return;
			}

			RR_SHARED_PTR<boost::asio::deadline_timer> timer = RR_MAKE_SHARED<boost::asio::deadline_timer>(boost::ref(socket->get_io_service()));
			timer->expires_from_now(boost::posix_time::milliseconds(10));
			RobotRaconteurNode::asio_async_wait(node, timer, boost::bind(&TcpAcceptor::AcceptSocket3, shared_from_this(),
				boost::asio::placeholders::error, timer, socket, socket_closer, handler));
			return;
		}

		std::string seed(buf, 4);

		if (seed == "RRAC")
		{
			try
			{
				RR_SHARED_PTR<TcpTransportConnection> t = RR_MAKE_SHARED<TcpTransportConnection>(parent, url, true, local_endpoint);
				boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)> h = boost::bind(handler, socket, t, _1);
				t->AsyncAttachSocket(socket, h);
				parent->AddCloseListener(t, &TcpTransportConnection::Close);
			}
			catch (std::exception&)
			{
				RobotRaconteurNode::TryPostToThreadPool(parent->GetNode(),boost::bind(handler, RR_SHARED_PTR<boost::asio::ip::tcp::socket>(), RR_SHARED_PTR<TcpTransportConnection>(), RR_MAKE_SHARED<ConnectionException>("Could not connect to service")),true);
			}
		}
		else if (parent->GetAcceptWebSockets())
		{
			if (seed == "GET " || seed == "GET\t")
			{
				RR_SHARED_PTR<websocket_stream<boost::asio::ip::tcp::socket&> > websocket = RR_MAKE_SHARED<websocket_stream<boost::asio::ip::tcp::socket&> >(boost::ref(*socket));
				boost::function<void(const std::string& protocol, const boost::system::error_code& ec)> h = boost::bind(&TcpAcceptor::AcceptSocket5, shared_from_this(),
					_2, socket, websocket, socket_closer, handler);
				websocket->async_server_handshake("robotraconteur.robotraconteur.com", parent->GetWebSocketAllowedOrigins(), h);
				return;
			}
			else
			{
				std::string response2_1 = "HTTP/1.1 404 File Not Found\r\n";
				RR_SHARED_PTR<std::string> response2 = RR_MAKE_SHARED<std::string>(response2_1);
				boost::asio::const_buffers_1 response2_buf=boost::asio::buffer(response2->c_str(), response2->size());
				RobotRaconteurNode::asio_async_write_some(node,socket, response2_buf,
					boost::bind(&TcpAcceptor::AcceptSocket4, response2, socket, socket_closer));

				RR_SHARED_PTR<ConnectionException> err = RR_MAKE_SHARED<ConnectionException>("Invalid protocol");
				handler(RR_SHARED_PTR<boost::asio::ip::tcp::socket>(), RR_SHARED_PTR<ITransportConnection>(), err);
				return;

			}
		}
		else
		{		
			RR_SHARED_PTR<ConnectionException> err = RR_MAKE_SHARED<ConnectionException>("Invalid protocol");
			handler(RR_SHARED_PTR<boost::asio::ip::tcp::socket>(), RR_SHARED_PTR<ITransportConnection>(), err);
		}
	}
	
	TcpAcceptor::TcpAcceptor(RR_SHARED_PTR<TcpTransport> parent, std::string url, uint32_t local_endpoint)
	{
		this->parent = parent;
		this->url = url;
		this->local_endpoint = local_endpoint;
		this->node = parent->GetNode();
	}

	void TcpAcceptor::AcceptSocket(RR_SHARED_PTR<boost::asio::ip::tcp::socket> socket, boost::function<void(RR_SHARED_PTR<boost::asio::ip::tcp::socket>, RR_SHARED_PTR<ITransportConnection>, RR_SHARED_PTR<RobotRaconteurException>)>& handler)
	{
		start_time = parent->GetNode()->NowUTC();

		RR_SHARED_PTR<boost::signals2::scoped_connection> socket_closer
			= RR_MAKE_SHARED<boost::signals2::scoped_connection>(
				parent->AddCloseListener(socket, boost::bind(&boost::asio::ip::tcp::socket::close, _1))
				);

		socket->async_receive(boost::asio::buffer(buf, 4), boost::asio::ip::tcp::socket::message_peek,
			boost::bind(&TcpAcceptor::AcceptSocket2, shared_from_this(), boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred, socket, socket_closer, handler));
	}

	//end class TcpAcceptor	

	//class TcpWebSocketConnected
	
	void TcpWebSocketConnector::Connect4(RR_SHARED_PTR<RobotRaconteurException> err,
		RR_SHARED_PTR<ITransportConnection> connection,
		RR_SHARED_PTR<boost::asio::ip::tcp::socket> socket,
		RR_SHARED_PTR<websocket_stream<boost::asio::ip::tcp::socket&> > websocket,
		boost::function<void(RR_SHARED_PTR<ITransportConnection>, RR_SHARED_PTR<RobotRaconteurException>) > handler
		)
	{
		if (err)
		{
			try
			{
				if (connection)
				{
					connection->Close();
				}
			}
			catch (std::exception&) {}

			handler(RR_SHARED_PTR<ITransportConnection>(), err);
			return;
		}

		parent->register_transport(connection);

		handler(connection, RR_SHARED_PTR<RobotRaconteurException>());
	}


	void TcpWebSocketConnector::Connect3(const boost::system::error_code& ec,
		RR_SHARED_PTR<boost::asio::ip::tcp::socket> socket,
		RR_SHARED_PTR<websocket_stream<boost::asio::ip::tcp::socket&> > websocket,
		RR_SHARED_PTR<boost::signals2::scoped_connection> socket_closer,
		boost::function<void(RR_SHARED_PTR<ITransportConnection>, RR_SHARED_PTR<RobotRaconteurException>) > handler
		)
	{

		if (ec)
		{
			RR_SHARED_PTR<ConnectionException> err = RR_MAKE_SHARED<ConnectionException>("Could not connect to remote websocket");
			handler(RR_SHARED_PTR<ITransportConnection>(), err);
			return;
		}


		socket->set_option(boost::asio::ip::tcp::socket::linger(true, 5));

		try
		{
			RR_SHARED_PTR<TcpTransportConnection> t = RR_MAKE_SHARED<TcpTransportConnection>(parent, url, false, endpoint);
			boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)> h = boost::bind(&TcpWebSocketConnector::Connect4, shared_from_this(),
				_1, t, socket, websocket, boost::protect(handler));
			t->AsyncAttachWebSocket(socket, websocket, h);
			parent->AddCloseListener(t, &TcpTransportConnection::Close);
		}
		catch (std::exception&)
		{
			handler(RR_SHARED_PTR<TcpTransportConnection>(), RR_MAKE_SHARED<ConnectionException>("Could not connect to service"));
		}
	}

	void TcpWebSocketConnector::Connect2(const boost::system::error_code& ec,
		RR_SHARED_PTR<boost::asio::ip::tcp::socket> socket,
		RR_SHARED_PTR<websocket_tcp_connector> socket_connector,
		boost::function<void(RR_SHARED_PTR<ITransportConnection>, RR_SHARED_PTR<RobotRaconteurException>) > handler)
	{
		if (ec)
		{
			RR_SHARED_PTR<ConnectionException> err = RR_MAKE_SHARED<ConnectionException>("Could not contact remote host");
			handler(RR_SHARED_PTR<ITransportConnection>(), err);
			return;
		}

		try
		{
			RR_SHARED_PTR<websocket_stream<boost::asio::ip::tcp::socket&> > websocket = RR_MAKE_SHARED<websocket_stream<boost::asio::ip::tcp::socket&> >(boost::ref(*socket));

			RR_SHARED_PTR<boost::signals2::scoped_connection> websocket_closer =
				RR_MAKE_SHARED<boost::signals2::scoped_connection>(
					parent->AddCloseListener(websocket, &websocket_stream<boost::asio::ip::tcp::socket&>::close)
					);				

			websocket->async_client_handshake(ws_url, "robotraconteur.robotraconteur.com",
				boost::bind(&TcpWebSocketConnector::Connect3, shared_from_this(), _1, socket, websocket, websocket_closer, boost::protect(handler)));
			
		}
		catch (std::exception&)
		{
			RR_SHARED_PTR<ConnectionException> err = RR_MAKE_SHARED<ConnectionException>("Could not contact remote host");
			handler(RR_SHARED_PTR<ITransportConnection>(), err);
			return;
		}
	}

		
	TcpWebSocketConnector::TcpWebSocketConnector(RR_SHARED_PTR<TcpTransport> parent)
	{
		this->parent = parent;
	}

	void TcpWebSocketConnector::Connect(std::string url, uint32_t endpoint, boost::function<void(RR_SHARED_PTR<ITransportConnection>, RR_SHARED_PTR<RobotRaconteurException>) > handler)
	{
		this->url = url;
		this->endpoint = endpoint;

		try
		{
			ParseConnectionURLResult url_res = ParseConnectionURL(url);

			if (url_res.scheme != "rr+ws" && url_res.scheme != "rrs+ws") throw InvalidArgumentException("Invalid transport type for TcpTransport");

			//if (url_result[1]!="tcp") throw InvalidArgumentException("Invalid transport type for TcpTransport");


			std::string host = url_res.host;
			std::string port = boost::lexical_cast<std::string>(url_res.port);

			std::string path = url_res.path;


			if (url_res.scheme == "rr+ws")
			{
				ws_url = boost::replace_first_copy(url, "rr+ws", "ws");
			}

			if (url_res.scheme == "rrs+ws")
			{
				ws_url = boost::replace_first_copy(url, "rrs+ws", "ws");
			}
			if (url_res.host == "") throw ConnectionException("Invalid host for usb transport");
			//std::cout << ws_url << std::endl;

			RR_SHARED_PTR<detail::websocket_tcp_connector> socket_connector =
				RR_MAKE_SHARED<detail::websocket_tcp_connector>(boost::ref(parent->GetNode()->GetThreadPool()->get_io_service()));

			socket_connector->connect(ws_url, boost::bind(&TcpWebSocketConnector::Connect2, shared_from_this(),
				_1, _2, socket_connector, boost::protect(handler)));
			parent->AddCloseListener(socket_connector, &detail::websocket_tcp_connector::cancel);
		}
		catch (std::exception&)
		{
			throw ConnectionException("Invalid URL for websocket connection");

		}
	}

	//end TcpWebSocketConnector

}	

namespace detail {

	static void TcpTransportConnection_socket_read_adapter(RR_WEAK_PTR<RobotRaconteurNode> node, RR_WEAK_PTR<boost::asio::ip::tcp::socket> sock, mutable_buffers& b, boost::function<void(const boost::system::error_code&, size_t)>& handler)
	{
		RR_SHARED_PTR<boost::asio::ip::tcp::socket> sock1 = sock.lock();
		if (!sock1) return;
		RobotRaconteurNode::asio_async_read_some(node, sock1, b, handler);
	}

	static void TcpTransportConnection_socket_write_adapter(RR_WEAK_PTR<RobotRaconteurNode> node, RR_WEAK_PTR<boost::asio::ip::tcp::socket> sock, const_buffers& b, boost::function<void(const boost::system::error_code&, size_t)>& handler)
	{
		RR_SHARED_PTR<boost::asio::ip::tcp::socket> sock1 = sock.lock();
		if (!sock1) return;
		RobotRaconteurNode::asio_async_write_some(node, sock1, b, handler);
	}

	static void TcpTransportConnection_socket_close_adapter(RR_WEAK_PTR<boost::asio::ip::tcp::socket> sock)
	{
		RR_SHARED_PTR<boost::asio::ip::tcp::socket> sock1 = sock.lock();
		if (!sock1) return;
		sock1->close();
	}


#ifdef ROBOTRACONTEUR_USE_SCHANNEL
	
	void TcpWSSWebSocketConnector::Connect4(RR_SHARED_PTR<RobotRaconteurException> err,
		RR_SHARED_PTR<ITransportConnection> connection,
		RR_SHARED_PTR<boost::asio::ip::tcp::socket> socket,
		RR_SHARED_PTR<TlsSchannelAsyncStreamAdapter> tls_stream,
		RR_SHARED_PTR<websocket_stream<TlsSchannelAsyncStreamAdapter_ASIO_adapter&> > websocket,
		boost::function<void(RR_SHARED_PTR<ITransportConnection>, RR_SHARED_PTR<RobotRaconteurException>) > handler
		)
	{
		if (err)
		{
			try
			{
				if (connection)
				{
					connection->Close();
				}
			}
			catch (std::exception&) {}

			handler(RR_SHARED_PTR<ITransportConnection>(), err);
			return;
		}

		parent->register_transport(connection);

		handler(connection, RR_SHARED_PTR<RobotRaconteurException>());
	}


	void TcpWSSWebSocketConnector::Connect3(const boost::system::error_code& ec,
		RR_SHARED_PTR<boost::asio::ip::tcp::socket> socket,
		RR_SHARED_PTR<boost::signals2::scoped_connection> socket_closer,
		RR_SHARED_PTR<TlsSchannelAsyncStreamAdapter> tls_stream,
		RR_SHARED_PTR<websocket_stream<TlsSchannelAsyncStreamAdapter_ASIO_adapter&> > websocket,
		boost::function<void(RR_SHARED_PTR<ITransportConnection>, RR_SHARED_PTR<RobotRaconteurException>) > handler
		)
	{

		if (ec)
		{
			RR_SHARED_PTR<ConnectionException> err = RR_MAKE_SHARED<ConnectionException>("Could not connect to remote websocket");
			handler(RR_SHARED_PTR<ITransportConnection>(), err);
			return;
		}
				
		socket->set_option(boost::asio::ip::tcp::socket::linger(true, 5));

		try
		{
			RR_SHARED_PTR<TcpTransportConnection> t = RR_MAKE_SHARED<TcpTransportConnection>(parent, url, false, endpoint);
			boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)> h= boost::bind(&TcpWSSWebSocketConnector::Connect4, shared_from_this(),
				_1, t, socket, tls_stream, websocket, boost::protect(handler));
			t->AsyncAttachWSSWebSocket(socket, tls_stream, websocket, h);
			parent->AddCloseListener(t, &TcpTransportConnection::Close);
		}
		catch (std::exception&)
		{
			handler(RR_SHARED_PTR<TcpTransportConnection>(), RR_MAKE_SHARED<ConnectionException>("Could not connect to service"));
		}


	}

	void TcpWSSWebSocketConnector::Connect2_1(const boost::system::error_code& ec,
		RR_SHARED_PTR<boost::asio::ip::tcp::socket> socket,
		RR_SHARED_PTR<boost::signals2::scoped_connection> socket_closer,
		RR_SHARED_PTR<TlsSchannelAsyncStreamAdapter> tls_stream,
		boost::function<void(RR_SHARED_PTR<ITransportConnection>, RR_SHARED_PTR<RobotRaconteurException>) > handler)
	{
		if (ec)
		{
			RR_SHARED_PTR<ConnectionException> err = RR_MAKE_SHARED<ConnectionException>("Could not contact remote host");
			handler(RR_SHARED_PTR<ITransportConnection>(), err);
			return;
		}

		if (!tls_stream->VerifyRemoteHostnameCertificate(servername))
		{
			RR_SHARED_PTR<ConnectionException> err = RR_MAKE_SHARED<ConnectionException>("Could not verify server certificate");
			handler(RR_SHARED_PTR<ITransportConnection>(), err);
			return;
		}

		try
		{
			RR_SHARED_PTR<websocket_stream<TlsSchannelAsyncStreamAdapter_ASIO_adapter&> > websocket = RR_MAKE_SHARED<websocket_stream<TlsSchannelAsyncStreamAdapter_ASIO_adapter&> >(boost::ref(tls_stream->get_asio_adapter()));
			websocket->async_client_handshake(ws_url, "robotraconteur.robotraconteur.com",
				boost::bind(&TcpWSSWebSocketConnector::Connect3, shared_from_this(), _1, socket, socket_closer, tls_stream, websocket, boost::protect(handler)));
		}
		catch (std::exception&)
		{
			RR_SHARED_PTR<ConnectionException> err = RR_MAKE_SHARED<ConnectionException>("Could not contact remote host");
			handler(RR_SHARED_PTR<ITransportConnection>(), err);
			return;
		}
	}

	void TcpWSSWebSocketConnector::Connect2(const boost::system::error_code& ec,
		RR_SHARED_PTR<boost::asio::ip::tcp::socket> socket,
		RR_SHARED_PTR<websocket_tcp_connector> socket_connector,
		boost::function<void(RR_SHARED_PTR<ITransportConnection>, RR_SHARED_PTR<RobotRaconteurException>) > handler)
	{
		if (ec)
		{
			RR_SHARED_PTR<ConnectionException> err = RR_MAKE_SHARED<ConnectionException>("Could not contact remote host");
			handler(RR_SHARED_PTR<ITransportConnection>(), err);
			return;
		}

		try
		{
			
			RR_SHARED_PTR<boost::signals2::scoped_connection> socket_closer
				= RR_MAKE_SHARED<boost::signals2::scoped_connection>(
					parent->AddCloseListener(socket, boost::bind(&boost::asio::ip::tcp::socket::close, _1))
					);

			RR_WEAK_PTR<boost::asio::ip::tcp::socket> socket1 = socket;
			RR_SHARED_PTR<TlsSchannelAsyncStreamAdapterContext> context = RR_MAKE_SHARED<TlsSchannelAsyncStreamAdapterContext>(parent->GetNode()->NodeID());
			RR_SHARED_PTR<RobotRaconteur::detail::TlsSchannelAsyncStreamAdapter>
				tls_stream = RR_MAKE_SHARED<RobotRaconteur::detail::TlsSchannelAsyncStreamAdapter>(boost::ref(socket->get_io_service()),
					context,
					RobotRaconteur::detail::TlsSchannelAsyncStreamAdapter::client,
					servername,
					boost::bind(TcpTransportConnection_socket_read_adapter, node, socket1, _1, _2),
					boost::bind(TcpTransportConnection_socket_write_adapter, node, socket1, _1, _2),
					boost::bind(TcpTransportConnection_socket_close_adapter, socket1)
				);

			RobotRaconteurNode::asio_async_handshake(node, tls_stream,boost::bind(&TcpWSSWebSocketConnector::Connect2_1, shared_from_this(), _1, socket, socket_closer, tls_stream, boost::protect(handler)));
		}
		catch (std::exception&)
		{
			RR_SHARED_PTR<ConnectionException> err = RR_MAKE_SHARED<ConnectionException>("Could not contact remote host");
			handler(RR_SHARED_PTR<ITransportConnection>(), err);
			return;
		}
	}
		
	TcpWSSWebSocketConnector::TcpWSSWebSocketConnector(RR_SHARED_PTR<TcpTransport> parent)
	{
		this->parent = parent;
		this->node = parent->GetNode();
	}

	void TcpWSSWebSocketConnector::Connect(std::string url, uint32_t endpoint, boost::function<void(RR_SHARED_PTR<ITransportConnection>, RR_SHARED_PTR<RobotRaconteurException>) > handler)
	{
		this->url = url;
		this->endpoint = endpoint;

		try
		{
			ParseConnectionURLResult url_res = ParseConnectionURL(url);

			if (url_res.scheme != "rr+wss" && url_res.scheme != "rrs+wss") throw InvalidArgumentException("Invalid transport type for TcpTransport");

			//if (url_result[1]!="tcp") throw InvalidArgumentException("Invalid transport type for TcpTransport");


			std::string host = url_res.host;
			servername = host;
			std::string port = boost::lexical_cast<std::string>(url_res.port);

			std::string path = url_res.path;


			if (url_res.scheme == "rr+wss")
			{
				ws_url = boost::replace_first_copy(url, "rr+wss", "wss");
			}

			if (url_res.scheme == "rrs+wss")
			{
				ws_url = boost::replace_first_copy(url, "rrs+wss", "wss");
			}
			if (url_res.host == "") throw ConnectionException("Invalid host for usb transport");
			//std::cout << ws_url << std::endl;

			RR_SHARED_PTR<detail::websocket_tcp_connector> socket_connector =
				RR_MAKE_SHARED<detail::websocket_tcp_connector>(boost::ref(parent->GetNode()->GetThreadPool()->get_io_service()));

			socket_connector->connect(ws_url, boost::bind(&TcpWSSWebSocketConnector::Connect2, shared_from_this(),
				_1, _2, socket_connector, boost::protect(handler)));
			parent->AddCloseListener(socket_connector, &detail::websocket_tcp_connector::cancel);

		}
		catch (std::exception&)
		{
			throw ConnectionException("Invalid URL for websocket connection");

		}

	}

#endif

#ifdef ROBOTRACONTEUR_USE_OPENSSL
	
		void TcpWSSWebSocketConnector::Connect4(RR_SHARED_PTR<RobotRaconteurException> err,
			RR_SHARED_PTR<ITransportConnection> connection,
			RR_SHARED_PTR<boost::asio::ip::tcp::socket> socket,
			RR_SHARED_PTR<boost::asio::ssl::stream<boost::asio::ip::tcp::socket&> > tls_stream,
			RR_SHARED_PTR<websocket_stream<boost::asio::ssl::stream<boost::asio::ip::tcp::socket&> &> > websocket,
			boost::function<void(RR_SHARED_PTR<ITransportConnection>, RR_SHARED_PTR<RobotRaconteurException>) > handler
			)
		{
			if (err)
			{
				try
				{
					if (connection)
					{
						connection->Close();
					}
				}
				catch (std::exception&) {}

				handler(RR_SHARED_PTR<ITransportConnection>(), err);
				return;
			}

			parent->register_transport(connection);

			handler(connection, RR_SHARED_PTR<RobotRaconteurException>());
		}


		void TcpWSSWebSocketConnector::Connect3(const boost::system::error_code& ec,
			RR_SHARED_PTR<boost::asio::ip::tcp::socket> socket,
			RR_SHARED_PTR<boost::signals2::scoped_connection> socket_closer,
			RR_SHARED_PTR<boost::asio::ssl::stream<boost::asio::ip::tcp::socket&> > tls_stream,
			RR_SHARED_PTR<websocket_stream<boost::asio::ssl::stream<boost::asio::ip::tcp::socket&> &> > websocket,
			boost::function<void(RR_SHARED_PTR<ITransportConnection>, RR_SHARED_PTR<RobotRaconteurException>) > handler
			)
		{

			if (ec)
			{
				RR_SHARED_PTR<ConnectionException> err = RR_MAKE_SHARED<ConnectionException>("Could not connect to remote websocket");
				handler(RR_SHARED_PTR<ITransportConnection>(), err);
				return;
			}



			socket->set_option(boost::asio::ip::tcp::socket::linger(true, 5));

			try
			{
				RR_SHARED_PTR<TcpTransportConnection> t = RR_MAKE_SHARED<TcpTransportConnection>(parent, url, false, endpoint);
				boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)> h = boost::bind(&TcpWSSWebSocketConnector::Connect4, shared_from_this(),
					_1, t, socket, tls_stream, websocket, handler);
				t->AsyncAttachWSSWebSocket(socket, tls_stream, websocket, context, h);
				parent->AddCloseListener(t, &TcpTransportConnection::Close);

			}
			catch (std::exception&)
			{
				handler(RR_SHARED_PTR<TcpTransportConnection>(), RR_MAKE_SHARED<ConnectionException>("Could not connect to service"));
			}


		}

        
		void TcpWSSWebSocketConnector::Connect2_1(const boost::system::error_code& ec,
			RR_SHARED_PTR<boost::asio::ip::tcp::socket> socket,
			RR_SHARED_PTR<boost::signals2::scoped_connection> socket_closer,
			RR_SHARED_PTR<boost::asio::ssl::stream<boost::asio::ip::tcp::socket&> > tls_stream,
			boost::function<void(RR_SHARED_PTR<ITransportConnection>, RR_SHARED_PTR<RobotRaconteurException>) > handler)
		{
			if (ec)
			{
				RR_SHARED_PTR<ConnectionException> err = RR_MAKE_SHARED<ConnectionException>("Could not contact remote host");
				handler(RR_SHARED_PTR<ITransportConnection>(), err);
				return;
			}



			try
			{
				RR_SHARED_PTR<websocket_stream<boost::asio::ssl::stream<boost::asio::ip::tcp::socket&> &> > websocket = RR_MAKE_SHARED<websocket_stream<boost::asio::ssl::stream<boost::asio::ip::tcp::socket&> &> >(boost::ref(*tls_stream));
				websocket->async_client_handshake(ws_url, "robotraconteur.robotraconteur.com",
					boost::bind(&TcpWSSWebSocketConnector::Connect3, shared_from_this(), _1, socket, socket_closer, tls_stream, websocket, boost::protect(handler)));
			}
			catch (std::exception&)
			{
				RR_SHARED_PTR<ConnectionException> err = RR_MAKE_SHARED<ConnectionException>("Could not contact remote host");
				handler(RR_SHARED_PTR<ITransportConnection>(), err);
				return;
			}
		}
#ifdef ROBOTRACONTEUR_APPLE
        bool TcpWSSWebSocketConnector::verify_callback(bool preverified, boost::asio::ssl::verify_context& ctx, std::string servername)
        {
            if (preverified)
            {
                return  true;
            }
            
            STACK_OF(X509)* certs=X509_STORE_CTX_get1_chain(ctx.native_handle());
            
            std::vector<SecCertificateRef> certarray1;
            for (int i=0; i<sk_X509_num(certs); i++)
            {
                BIO* bio=BIO_new(BIO_s_mem());
                X509* cert=sk_X509_value(certs,i);
                i2d_X509_bio(bio, cert);
                int err=ERR_get_error();
                if (err)
                {
                    BOOST_FOREACH (SecCertificateRef& e, certarray1)
                    {
                        CFRelease(e);
                    }
                    sk_X509_pop_free(certs, X509_free);
                    BIO_free(bio);
                    return false;
                }
                
                BUF_MEM* mem=NULL;
                BIO_get_mem_ptr(bio, &mem);
                err = ERR_get_error();
                if (err)
                {
                    BOOST_FOREACH (SecCertificateRef& e, certarray1)
                    {
                        CFRelease(e);
                    }
                    sk_X509_pop_free(certs, X509_free);
                    BIO_free(bio);
                    return false;
                }
                CFDataRef d=CFDataCreate(NULL, (UInt8*)mem->data, mem->length );
                
                SecCertificateRef caRef=SecCertificateCreateWithData(NULL, d);
                
                BIO_free(bio);
                if (!caRef)
                {
                    BOOST_FOREACH (SecCertificateRef& e, certarray1)
                    {
                        CFRelease(e);
                    }
                    sk_X509_pop_free(certs, X509_free);
                    return false;
                }
                certarray1.push_back(caRef);
            }
            sk_X509_pop_free(certs, X509_free);
            
            CFArrayRef certarray=CFArrayCreate(NULL,(const void**)&certarray1[0],certarray1.size(),&kCFTypeArrayCallBacks);
            
            CFStringRef host2=CFStringCreateWithCString(NULL, servername.c_str(), kCFStringEncodingUTF8);
            SecPolicyRef policy=SecPolicyCreateSSL(true, host2);
            
            SecTrustResultType result=kSecTrustResultDeny;
            
            SecTrustRef trust;
            int code=SecTrustCreateWithCertificates(certarray, policy, &trust);
            
            if (code==0)
            {
                code=SecTrustEvaluate(trust,&result);
            }
            
            CFRelease(certarray);
            CFRelease(host2);
            CFRelease(policy);
            CFRelease(trust);
            
            if (result == kSecTrustResultUnspecified || result == kSecTrustResultProceed)
            {
                return true;
            }
            
            return false;
        }
#endif
		void TcpWSSWebSocketConnector::Connect2(const boost::system::error_code& ec,
			RR_SHARED_PTR<boost::asio::ip::tcp::socket> socket,			
			RR_SHARED_PTR<websocket_tcp_connector> socket_connector,
			boost::function<void(RR_SHARED_PTR<ITransportConnection>, RR_SHARED_PTR<RobotRaconteurException>) > handler)
		{
			if (ec)
			{
				RR_SHARED_PTR<ConnectionException> err = RR_MAKE_SHARED<ConnectionException>("Could not contact remote host");
				handler(RR_SHARED_PTR<ITransportConnection>(), err);
				return;
			}

			try
			{
				RR_SHARED_PTR<boost::signals2::scoped_connection> socket_closer
					= RR_MAKE_SHARED<boost::signals2::scoped_connection>(
						parent->AddCloseListener(socket, boost::bind(&boost::asio::ip::tcp::socket::close, _1))
						);

				context = RR_MAKE_SHARED<boost::asio::ssl::context>(boost::asio::ssl::context::tlsv1);
				context->set_default_verify_paths();

				context->set_verify_mode(boost::asio::ssl::verify_peer);
#ifndef ROBOTRACONTEUR_APPLE
				context->set_verify_callback(boost::asio::ssl::rfc2818_verification(servername));
#else
                context->set_verify_callback(boost::bind(&TcpWSSWebSocketConnector::verify_callback,_1,_2,servername));
#endif

				RR_SHARED_PTR<boost::asio::ssl::stream<boost::asio::ip::tcp::socket&> >
				tls_stream = RR_MAKE_SHARED<boost::asio::ssl::stream<boost::asio::ip::tcp::socket&> >(boost::ref(*socket),
					boost::ref(*context));

				RobotRaconteurNode::asio_async_handshake(node, tls_stream ,boost::asio::ssl::stream<boost::asio::ip::tcp::socket&>::client,
						boost::bind(&TcpWSSWebSocketConnector::Connect2_1, shared_from_this(), _1, socket, socket_closer, tls_stream, boost::protect(handler)));
			}
			catch (std::exception&)
			{
				RR_SHARED_PTR<ConnectionException> err = RR_MAKE_SHARED<ConnectionException>("Could not contact remote host");
				handler(RR_SHARED_PTR<ITransportConnection>(), err);
				return;
			}
		}

		TcpWSSWebSocketConnector::TcpWSSWebSocketConnector(RR_SHARED_PTR<TcpTransport> parent)
		{
			this->parent = parent;
			this->node = parent->GetNode();
		}

		void TcpWSSWebSocketConnector::Connect(std::string url, uint32_t endpoint, boost::function<void(RR_SHARED_PTR<ITransportConnection>, RR_SHARED_PTR<RobotRaconteurException>) > handler)
		{
			this->url = url;
			this->endpoint = endpoint;

			try
			{
				ParseConnectionURLResult url_res = ParseConnectionURL(url);

				if (url_res.scheme != "rr+wss" && url_res.scheme != "rrs+wss") throw InvalidArgumentException("Invalid transport type for TcpTransport");

				//if (url_result[1]!="tcp") throw InvalidArgumentException("Invalid transport type for TcpTransport");

				
				std::string host = url_res.host;
				servername = host;
				std::string port = boost::lexical_cast<std::string>(url_res.port);

				std::string path = url_res.path;


				if (url_res.scheme == "rr+wss")
				{
					ws_url = boost::replace_first_copy(url, "rr+wss", "wss");
				}

				if (url_res.scheme == "rrs+wss")
				{
					ws_url = boost::replace_first_copy(url, "rrs+wss", "wss");
				}

				//std::cout << ws_url << std::endl;

				RR_SHARED_PTR<detail::websocket_tcp_connector> socket_connector =
					RR_MAKE_SHARED<detail::websocket_tcp_connector>(boost::ref(parent->GetNode()->GetThreadPool()->get_io_service()));

				socket_connector->connect(ws_url, boost::bind(&TcpWSSWebSocketConnector::Connect2, shared_from_this(),
					_1, _2, socket_connector, boost::protect(handler)));
				parent->AddCloseListener(socket_connector, &detail::websocket_tcp_connector::cancel);

			}
			catch (std::exception&)
			{
				throw ConnectionException("Invalid URL for websocket connection");

			}

		}

#endif
		
}

TcpTransport::TcpTransport(RR_SHARED_PTR<RobotRaconteurNode> node)
	: Transport(node)
{
	if (!node) throw InvalidArgumentException("Node cannot be null");

	m_Port=0;
	default_connect_timeout=5000;
	default_receive_timeout=15000;
	max_message_size = 12 * 1024 * 1024;
	max_connection_count = 0;
	this->node=node;
	this->heartbeat_period=10000;
	this->require_tls=false;
	ipv4_acceptor_paused = false;
	ipv6_acceptor_paused = false;
	accept_websockets = true;

	allowed_websocket_origins.push_back("null");
	allowed_websocket_origins.push_back("file://");
	allowed_websocket_origins.push_back("chrome-extension://");
	allowed_websocket_origins.push_back("http://robotraconteur.com");
	allowed_websocket_origins.push_back("http://robotraconteur.com:80");
	allowed_websocket_origins.push_back("http://*.robotraconteur.com");
	allowed_websocket_origins.push_back("http://*.robotraconteur.com:80");
	allowed_websocket_origins.push_back("https://robotraconteur.com");
	allowed_websocket_origins.push_back("https://robotraconteur.com:443");
	allowed_websocket_origins.push_back("https://*.robotraconteur.com");
	allowed_websocket_origins.push_back("https://*.robotraconteur.com:443");

#ifndef ROBOTRACONTEUR_DISABLE_MESSAGE3
	disable_message3 = false;
#else
	disable_message3 = true;
#endif
#ifndef ROBOTRACONTEUR_DISABLE_STRINGTABLE
	disable_string_table = false;
#else
	disable_string_table = true;
#endif
	disable_async_message_io = false;
	closed = false;

}

TcpTransport::~TcpTransport()
{

}


void TcpTransport::Close()
{
	{
		boost::mutex::scoped_lock lock(closed_lock);
		if (closed) return;
		closed = true;
	}

	try
	{
		boost::mutex::scoped_lock lock(port_sharer_client_lock);
		if (port_sharer_client)
		{
			RR_SHARED_PTR<detail::TcpTransportPortSharerClient> c =
				RR_STATIC_POINTER_CAST<detail::TcpTransportPortSharerClient>(port_sharer_client);
			if (c)
			{
				c->Close();
			}
		}
	}
	catch (std::exception&) {}

	try
	{
		boost::mutex::scoped_lock lock(acceptor_lock);
		if (ipv4_acceptor) ipv4_acceptor->close();
		if (ipv6_acceptor) ipv6_acceptor->close();
	}
	catch (std::exception&) {}

	std::vector<RR_SHARED_PTR<ITransportConnection> > t;

	{
		boost::mutex::scoped_lock lock(TransportConnections_lock);
		boost::copy(TransportConnections | boost::adaptors::map_values, std::back_inserter(t));		
		BOOST_FOREACH (RR_WEAK_PTR<ITransportConnection>& e, incoming_TransportConnections)
		{
			RR_SHARED_PTR<ITransportConnection> t2=e.lock();
			if (t2) t.push_back(t2); 
		}
	}

	BOOST_FOREACH (RR_SHARED_PTR<ITransportConnection>& e, t)
	{
		try
		{
			e->Close();
			RR_SHARED_PTR<TcpTransportConnection> tt=RR_DYNAMIC_POINTER_CAST<TcpTransportConnection>(e);
			if (!tt->IsClosed())
			{
				closing_TransportConnections.push_back(tt);
			}
		}
		catch (std::exception&) {}
	}	
	
	try
	{
	DisableNodeAnnounce();
	}
	catch (std::exception&) {}

	try
	{
		DisableNodeDiscoveryListening();
	}
	catch (std::exception&) {}

	boost::posix_time::ptime t1=boost::posix_time::microsec_clock::universal_time();
	boost::posix_time::ptime t2=t1;

	while ((t2-t1).total_milliseconds() < 500)
	{
		bool stillopen=false;
		BOOST_FOREACH (RR_WEAK_PTR<ITransportConnection>& e, closing_TransportConnections)
		{
			try
			{
				RR_SHARED_PTR<ITransportConnection> t1=e.lock();
				if (!t1) continue;
				RR_SHARED_PTR<TcpTransportConnection> t2=RR_DYNAMIC_POINTER_CAST<TcpTransportConnection>(t1);
				if (!t2->IsClosed())
				{
					stillopen=true;
				}
			}
			catch (std::exception&) {}
		}

		if (!stillopen) return;
		boost::this_thread::sleep(boost::posix_time::milliseconds(25));
		t2=boost::posix_time::microsec_clock::universal_time();
	}

	BOOST_FOREACH (RR_WEAK_PTR<ITransportConnection>& e, closing_TransportConnections)
	{
		try
		{
			RR_SHARED_PTR<ITransportConnection> t1=e.lock();
			if (!t1) continue;
			RR_SHARED_PTR<TcpTransportConnection> t2=RR_DYNAMIC_POINTER_CAST<TcpTransportConnection>(t1);
			if (!t2->IsClosed())
			{
				t2->ForceClose();
			}
		}
		catch (std::exception&) {}
	}

	close_signal();
}

bool TcpTransport::IsServer() const
{
	return true;
}

bool TcpTransport::IsClient() const
{
	return true;
}

std::string TcpTransport::GetUrlSchemeString() const
{
	return "rr+tcp";
}

int32_t TcpTransport::GetListenPort()
{
	boost::mutex::scoped_lock lock(port_sharer_client_lock);
	if (port_sharer_client)
	{
		RR_SHARED_PTR<detail::TcpTransportPortSharerClient> c =
			RR_STATIC_POINTER_CAST<detail::TcpTransportPortSharerClient>(port_sharer_client);
		if (c)
		{
			return c->GetListenPort();
		}
	}

	return m_Port;
}

bool TcpTransport::CanConnectService(const std::string& url)
{
		
	if (url.size() < 6) return false;

	if (boost::starts_with(url,"tcp://"))
		return true;

	if (boost::starts_with(url, "rr+tcp://"))
		return true;

	if (boost::starts_with(url, "rrs+tcp://"))
		return true;

	if (boost::starts_with(url, "rr+ws://"))
		return true;

	if (boost::starts_with(url, "rrs+ws://"))
		return true;
	
	if (boost::starts_with(url, "rr+wss://"))
		return true;

	if (boost::starts_with(url, "rrs+wss://"))
		return true;

	return false;
}

void TcpTransport::AsyncCreateTransportConnection(const std::string& url, RR_SHARED_PTR<Endpoint> e, boost::function<void (RR_SHARED_PTR<ITransportConnection>, RR_SHARED_PTR<RobotRaconteurException> ) >& callback)
{
	{
		int32_t max_connections = GetMaxConnectionCount();
		if (max_connections > 0)
		{
			boost::mutex::scoped_lock lock(TransportConnections_lock);
			if ((int32_t)(TransportConnections.size()) > max_connections) throw ConnectionException("Too many active TCP connections");
		}
	}


	if (boost::starts_with(url, "rr+ws://") || boost::starts_with(url, "rrs+ws://"))
	{
		RR_SHARED_PTR<detail::TcpWebSocketConnector> cw = RR_MAKE_SHARED<detail::TcpWebSocketConnector>(shared_from_this());
		cw->Connect(url, e->GetLocalEndpoint(), callback);
		return;
	}

	if (boost::starts_with(url, "rr+wss://") || boost::starts_with(url, "rrs+wss://"))
	{
		RR_SHARED_PTR<detail::TcpWSSWebSocketConnector> cw = RR_MAKE_SHARED<detail::TcpWSSWebSocketConnector>(shared_from_this());
		cw->Connect(url, e->GetLocalEndpoint(), callback);
		return;
	}

	RR_SHARED_PTR<detail::TcpConnector> c=RR_MAKE_SHARED<detail::TcpConnector>(shared_from_this());
	std::vector<std::string> url2;
	url2.push_back(url);
	c->Connect(url2,e->GetLocalEndpoint(),callback);	
}   

RR_SHARED_PTR<ITransportConnection> TcpTransport::CreateTransportConnection(const std::string& url, RR_SHARED_PTR<Endpoint> e)
{
	RR_SHARED_PTR<detail::sync_async_handler<ITransportConnection> > d=RR_MAKE_SHARED<detail::sync_async_handler<ITransportConnection> >(RR_MAKE_SHARED<ConnectionException>("Timeout exception"));
	
	boost::function<void(RR_SHARED_PTR<ITransportConnection>, RR_SHARED_PTR<RobotRaconteurException>) > h = boost::bind(&detail::sync_async_handler<ITransportConnection>::operator(), d, _1, _2);
	AsyncCreateTransportConnection(url, e, h);

	return d->end();

}

void TcpTransport::CloseTransportConnection(RR_SHARED_PTR<Endpoint> e)
{
	
	RR_SHARED_PTR<ServerEndpoint> e2=boost::dynamic_pointer_cast<ServerEndpoint>(e);
	if (e2)
	{
		{
			try
			{
				boost::mutex::scoped_lock lock(TransportConnections_lock);
				RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<ITransportConnection> >::iterator e1 = TransportConnections.find(e->GetLocalEndpoint());
				if (e1 != TransportConnections.end())
				{
					closing_TransportConnections.push_back(e1->second);
				}
			}
			catch (std::exception&) {}
		}
		RR_SHARED_PTR<boost::asio::deadline_timer> timer=RR_MAKE_SHARED<boost::asio::deadline_timer>(boost::ref(GetNode()->GetThreadPool()->get_io_service()));
		timer->expires_from_now(boost::posix_time::milliseconds(100));
		RobotRaconteurNode::asio_async_wait(node, timer, boost::bind(&TcpTransport::CloseTransportConnection_timed, shared_from_this(),boost::asio::placeholders::error,e,timer));
		return;
	}


	RR_SHARED_PTR<ITransportConnection> t;
	{
		boost::mutex::scoped_lock lock(TransportConnections_lock);
		RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<ITransportConnection> >::iterator e1 = TransportConnections.find(e->GetLocalEndpoint());
		if (e1 == TransportConnections.end()) return;
		t = e1->second;
		TransportConnections.erase(e1);
	}

	if (t)
	{
		try
		{
			t->Close();
		}
		catch (std::exception&) {}

		RR_SHARED_PTR<TcpTransportConnection> tt=RR_DYNAMIC_POINTER_CAST<TcpTransportConnection>(t);
		if (tt)
		{
			if (!tt->IsClosed())
			{
				boost::mutex::scoped_lock lock(TransportConnections_lock);
				closing_TransportConnections.push_back(t);
			}
		}
	}
	
}

void TcpTransport::CloseTransportConnection_timed(const boost::system::error_code& err, RR_SHARED_PTR<Endpoint> e,RR_SHARED_PTR<void> timer)
{
	if (err) return;

	RR_SHARED_PTR<ITransportConnection> t;
	
	{
		boost::mutex::scoped_lock lock(TransportConnections_lock);
		RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<ITransportConnection> >::iterator e1 = TransportConnections.find(e->GetLocalEndpoint());
		if (e1 == TransportConnections.end()) return;
		t = e1->second;
	}
	
	if (t)
	{
		try
		{
			t->Close();
		}
		catch (std::exception&) {}
	}

}

void TcpTransport::SendMessage(RR_INTRUSIVE_PTR<Message> m)
{
	

	RR_SHARED_PTR<ITransportConnection> t;
	
	{
		boost::mutex::scoped_lock lock(TransportConnections_lock);
		RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<ITransportConnection> >::iterator e1 = TransportConnections.find(m->header->SenderEndpoint);
		if (e1 == TransportConnections.end()) throw ConnectionException("Transport connection to remote host not found");
		t = e1->second;
	}	

	t->SendMessage(m);
}

uint32_t TcpTransport::TransportCapability(const std::string& name)
{
	return 0;
}

void TcpTransport::PeriodicCleanupTask()
{
	boost::mutex::scoped_lock lock(TransportConnections_lock);
	for (RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<ITransportConnection> >::iterator e=TransportConnections.begin(); e!=TransportConnections.end(); )
	{
		try
		{
			RR_SHARED_PTR<TcpTransportConnection> e2=rr_cast<TcpTransportConnection>(e->second);
			if (!e2->IsConnected())
			{
				e=TransportConnections.erase(e);
			}
			else
			{
				e++;
			}
		}
		catch (std::exception&) {}
	}

	for (std::list<RR_WEAK_PTR<ITransportConnection> >::iterator e=closing_TransportConnections.begin(); e!=closing_TransportConnections.end();)
	{
		try
		{
			if (e->expired())
			{
				e=closing_TransportConnections.erase(e);
			}
			else
			{
				e++;
			}
		}
		catch (std::exception&) {}
	}

	for (std::list<RR_WEAK_PTR<ITransportConnection> >::iterator e=incoming_TransportConnections.begin(); e!=incoming_TransportConnections.end();)
	{
		try
		{
			if (e->expired())
			{
				e=incoming_TransportConnections.erase(e);
			}
			else
			{
				e++;
			}
		}
		catch (std::exception&) {}
	}
}

void TcpTransport::AsyncSendMessage(RR_INTRUSIVE_PTR<Message> m, boost::function<void (RR_SHARED_PTR<RobotRaconteurException> )>& handler)
{	

	RR_SHARED_PTR<ITransportConnection> t;
	
	{
		boost::mutex::scoped_lock lock(TransportConnections_lock);
		RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<ITransportConnection> >::iterator e1 = TransportConnections.find(m->header->SenderEndpoint);
		if (e1 == TransportConnections.end()) throw ConnectionException("Transport connection to remote host not found");
		t = e1->second;
	}

	t->AsyncSendMessage(m,handler);
}


void TcpTransport::StartServer(int32_t port)
{

#ifndef ROBOTRACONTEUR_ALLOW_TCP_LISTEN_PORT_48653
	if (port==48653) throw InvalidArgumentException("Port 48653 is reserved");
#endif
	
	boost::mutex::scoped_lock lock(acceptor_lock);
	std::vector<boost::asio::ip::address> local_addresses;
			
	TcpTransport::GetLocalAdapterIPAddresses(local_addresses);

	bool has_ip6=false;
	bool has_ip4=false;

	BOOST_FOREACH(boost::asio::ip::address& e,  local_addresses)
	{
		if (e.is_v6()) has_ip6=true;
		if (e.is_v4()) has_ip4=true;
	}


	if (has_ip4)
	{

		ipv4_acceptor=RR_MAKE_SHARED<boost::asio::ip::tcp::acceptor>(boost::ref(GetNode()->GetThreadPool()->get_io_service()),boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(),port));

		ipv4_acceptor->set_option(boost::asio::ip::tcp::socket::linger(false,5));


		RR_SHARED_PTR<boost::asio::ip::tcp::socket> socket=RR_MAKE_SHARED<boost::asio::ip::tcp::socket>(boost::ref(GetNode()->GetThreadPool()->get_io_service()));

		ipv4_acceptor->async_accept(*socket,boost::bind(&TcpTransport::handle_v4_accept,shared_from_this(),ipv4_acceptor,socket,boost::asio::placeholders::error));
	
		if (port==0)
		{
			port=ipv4_acceptor->local_endpoint().port();
		}

		ipv4_acceptor_paused = false;

	
	}

	if (has_ip6)
	{
		ipv6_acceptor=RR_MAKE_SHARED<boost::asio::ip::tcp::acceptor>(boost::ref(GetNode()->GetThreadPool()->get_io_service()),boost::asio::ip::tcp::v6());

		ipv6_acceptor->set_option(boost::asio::ip::tcp::socket::linger(false,5));

		int on = 1;
		setsockopt(ipv6_acceptor->native_handle(), IPPROTO_IPV6, IPV6_V6ONLY, (const char*)&on, sizeof(on));

		ipv6_acceptor->bind(boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v6(),port));

		ipv6_acceptor->listen();

		RR_SHARED_PTR<boost::asio::ip::tcp::socket> socket2=RR_MAKE_SHARED<boost::asio::ip::tcp::socket>(boost::ref(GetNode()->GetThreadPool()->get_io_service()));

		ipv6_acceptor->async_accept(*socket2,boost::bind(&TcpTransport::handle_v6_accept,shared_from_this(),ipv6_acceptor,socket2,boost::asio::placeholders::error));

		if (port==0)
		{
			port=ipv6_acceptor->local_endpoint().port();
		}

		ipv6_acceptor_paused = false;
	}

	m_Port=port;


}

void TcpTransport::StartServerUsingPortSharer()
{
	boost::mutex::scoped_lock lock(port_sharer_client_lock);
	RR_SHARED_PTR<detail::TcpTransportPortSharerClient> c;
	if (port_sharer_client)
	{
		c = RR_STATIC_POINTER_CAST<detail::TcpTransportPortSharerClient>(port_sharer_client);
	}
	if (!c)
	{
		c = RR_MAKE_SHARED<detail::TcpTransportPortSharerClient>(shared_from_this());
	}
	
	port_sharer_client = c;
	c->Start();
}

bool TcpTransport::IsPortSharerRunning()
{
	boost::mutex::scoped_lock lock(port_sharer_client_lock);
	if (!port_sharer_client) return false;
	RR_SHARED_PTR<detail::TcpTransportPortSharerClient> c = RR_STATIC_POINTER_CAST<detail::TcpTransportPortSharerClient>(port_sharer_client);
	if (!c) return false;
	return c->IsPortSharerConnected();
}

void TcpTransport::GetLocalAdapterIPAddresses(std::vector<boost::asio::ip::address>& addresses)
{

	addresses.clear();

	#ifdef ROBOTRACONTEUR_WINDOWS

		/* Declare and initialize variables */

		DWORD dwSize = 0;
		DWORD dwRetVal = 0;

		unsigned int i = 0;

		// Set the flags to pass to GetAdaptersAddresses
		ULONG flags = GAA_FLAG_INCLUDE_PREFIX;

		// default to unspecified address family (both)
		ULONG family = AF_UNSPEC;

		LPVOID lpMsgBuf = NULL;

		PIP_ADAPTER_ADDRESSES pAddresses = NULL;
		ULONG outBufLen = 0;
		ULONG Iterations = 0;

		PIP_ADAPTER_ADDRESSES pCurrAddresses = NULL;
		PIP_ADAPTER_UNICAST_ADDRESS pUnicast = NULL;
		PIP_ADAPTER_ANYCAST_ADDRESS pAnycast = NULL;
		PIP_ADAPTER_MULTICAST_ADDRESS pMulticast = NULL;
		IP_ADAPTER_DNS_SERVER_ADDRESS *pDnServer = NULL;
		IP_ADAPTER_PREFIX *pPrefix = NULL;

		pAddresses=0;

		outBufLen=15000;

		

		do {

			pAddresses=(IP_ADAPTER_ADDRESSES*)HeapAlloc(GetProcessHeap(), 0, outBufLen);
			if (pAddresses == NULL) {
				HeapFree(GetProcessHeap(), 0, pAddresses);
				throw SystemResourceException("Could not get local IP addresses");
			}

			dwRetVal =
				GetAdaptersAddresses(family, flags, NULL, pAddresses, &outBufLen);

			if (dwRetVal == ERROR_BUFFER_OVERFLOW) {
				HeapFree(GetProcessHeap(), 0, pAddresses);
				pAddresses = NULL;
			} else {
				break;
			}

			Iterations++;

		} while ((dwRetVal == ERROR_BUFFER_OVERFLOW) && (Iterations < 5));


		if (dwRetVal != NO_ERROR) {
			HeapFree(GetProcessHeap(), 0, pAddresses);
			throw SystemResourceException("Could not get local IP addresses");
		}


		pCurrAddresses = pAddresses;
        while (pCurrAddresses) {
			
			pUnicast = pCurrAddresses->FirstUnicastAddress;
            if (pUnicast != NULL) {
                for (i = 0; pUnicast != NULL; i++)
				{

					if (pUnicast->Address.lpSockaddr->sa_family==AF_INET)
					{
						struct sockaddr_in* ip1=(struct sockaddr_in*)pUnicast->Address.lpSockaddr;
						boost::asio::ip::address_v4::bytes_type b;
						memcpy((uint8_t*)&b[0],(uint8_t*)&ip1->sin_addr,4);
						addresses.push_back(boost::asio::ip::address_v4(b));
						
					}

					

					if (pUnicast->Address.lpSockaddr->sa_family==AF_INET6)
					{
						
						struct sockaddr_in6* ip1=(struct sockaddr_in6*)pUnicast->Address.lpSockaddr;
						boost::asio::ip::address_v6::bytes_type b;
						memcpy(&b[0],&ip1->sin6_addr,16);


						boost::asio::ip::address_v6 addr(b,ip1->sin6_scope_id);
						if (!(addr.is_v4_compatible() || addr.is_v4_mapped()))
						{
							addresses.push_back(addr);
						}
					}

                    pUnicast = pUnicast->Next;

				}


			}

			pCurrAddresses = pCurrAddresses->Next;
		}



		if (pAddresses)
		{
			HeapFree(GetProcessHeap(), 0, pAddresses);
		}
#else
//TODO fill this in
		struct ifaddrs *ifaddr, *ifa;
		int family;

		if (getifaddrs(&ifaddr)==-1)
		{
			throw SystemResourceException("Could not get any local IP addresses");
		}

		 for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next)
		 {
			   if (ifa->ifa_addr == NULL)
				   continue;

			   family = ifa->ifa_addr->sa_family;

			   if (ifa->ifa_addr->sa_family==AF_INET)
			   {
				   struct sockaddr_in* ip1=(struct sockaddr_in*)ifa->ifa_addr;
				   boost::asio::ip::address_v4::bytes_type b;
					memcpy((uint8_t*)&b[0],(uint8_t*)&ip1->sin_addr,4);
					addresses.push_back(boost::asio::ip::address_v4(b));
			   }

			   if (ifa->ifa_addr->sa_family==AF_INET6)
			   {
				   struct sockaddr_in6* ip1=(struct sockaddr_in6*)ifa->ifa_addr;
				   	boost::asio::ip::address_v6::bytes_type b;
					memcpy(&b[0],&ip1->sin6_addr,16);
					addresses.push_back(boost::asio::ip::address_v6(b,ip1->sin6_scope_id));
			   }
		 }

		 freeifaddrs(ifaddr);

#endif

		if(addresses.size()==0)
		{
			throw SystemResourceException("Could not get any local IP addresses");
		}

		/*for(std::vector<boost::asio::ip::address>::iterator e=addresses.begin(); e!=addresses.end(); ++e)
		{
			std::cout << e->to_string() << std::endl;
		}

		std::cout << std::endl;*/

}

void TcpTransport::handle_v4_accept(RR_SHARED_PTR<TcpTransport> parent,RR_SHARED_PTR<boost::asio::ip::tcp::acceptor> acceptor, RR_SHARED_PTR<boost::asio::ip::tcp::socket> socket, const boost::system::error_code& error)
{
	if (error) 
		return;

	try
	{
		socket->set_option(boost::asio::ip::tcp::socket::linger(true,5));
		
		//TcpTransport_attach_transport(parent,socket,"",true,0,boost::bind(&TcpTransport_connected_callback2,parent,_1,_2,_3));
		RR_SHARED_PTR<detail::TcpAcceptor> a = RR_MAKE_SHARED<detail::TcpAcceptor>(parent, "", 0);
		boost::function<void(RR_SHARED_PTR<boost::asio::ip::tcp::socket>, RR_SHARED_PTR<ITransportConnection>, RR_SHARED_PTR<RobotRaconteurException>)> h = boost::bind(&TcpTransport_connected_callback2, parent, _1, _2, _3);
		a->AcceptSocket(socket, h);

	}
	catch (std::exception& exp) 
	{
		RobotRaconteurNode::TryHandleException(parent->node, &exp);
	}


	int32_t connection_count = 0;
	{
		boost::mutex::scoped_lock lock(parent->TransportConnections_lock);
		connection_count = (int32_t)parent->TransportConnections.size();
	}

	int32_t max_connection_count = parent->GetMaxConnectionCount();

	boost::mutex::scoped_lock lock(parent->acceptor_lock);
	if (max_connection_count > 0)
	{			
		if (connection_count > max_connection_count)
		{
			parent->ipv4_acceptor_paused = true;
			return;
		}
	}
	
	RR_SHARED_PTR<boost::asio::ip::tcp::socket> socket2=RR_MAKE_SHARED<boost::asio::ip::tcp::socket>(boost::ref(parent->GetNode()->GetThreadPool()->get_io_service()));

	acceptor->async_accept(*socket2,boost::bind(&TcpTransport::handle_v4_accept,parent,acceptor,socket2,boost::asio::placeholders::error));


}

void TcpTransport::handle_v6_accept(RR_SHARED_PTR<TcpTransport> parent, RR_SHARED_PTR<boost::asio::ip::tcp::acceptor> acceptor, RR_SHARED_PTR<boost::asio::ip::tcp::socket> socket, const boost::system::error_code& error)
{
	if (error) 
		return;

	try
	{
		socket->set_option(boost::asio::ip::tcp::socket::linger(true,5));
		//TcpTransport_attach_transport(parent,socket,"",true,0,boost::bind(&TcpTransport_connected_callback2,parent,_1,_2,_3));
		RR_SHARED_PTR<detail::TcpAcceptor> a = RR_MAKE_SHARED<detail::TcpAcceptor>(parent, "", 0);
		boost::function<void(RR_SHARED_PTR<boost::asio::ip::tcp::socket>, RR_SHARED_PTR<ITransportConnection>, RR_SHARED_PTR<RobotRaconteurException>)> h = boost::bind(&TcpTransport_connected_callback2, parent, _1, _2, _3);
		a->AcceptSocket(socket, h);
	}
	catch (std::exception& exp) 
	{
		RobotRaconteurNode::TryHandleException(parent->node, &exp);
	}

	int32_t connection_count = 0;
	{
		boost::mutex::scoped_lock lock(parent->TransportConnections_lock);
		connection_count = (int32_t)parent->TransportConnections.size();
	}

	int32_t max_connection_count = parent->GetMaxConnectionCount();

	boost::mutex::scoped_lock lock(parent->acceptor_lock);
	if (max_connection_count > 0)
	{		
		if (connection_count > max_connection_count)
		{
			parent->ipv6_acceptor_paused = true;
			return;
		}
	}
		
	RR_SHARED_PTR<boost::asio::ip::tcp::socket> socket2=RR_MAKE_SHARED<boost::asio::ip::tcp::socket>(boost::ref(parent->GetNode()->GetThreadPool()->get_io_service()));

	acceptor->async_accept(*socket2,boost::bind(&TcpTransport::handle_v6_accept,parent,acceptor,socket2,boost::asio::placeholders::error));


}

int32_t TcpTransport::GetDefaultHeartbeatPeriod()
{
	boost::mutex::scoped_lock lock(parameter_lock);
	return heartbeat_period;
}

void TcpTransport::SetDefaultHeartbeatPeriod(int32_t milliseconds)
{
	if (!(milliseconds>0)) throw InvalidArgumentException("Heartbeat must be positive");
	boost::mutex::scoped_lock lock(parameter_lock);
	heartbeat_period=milliseconds;
}

int32_t TcpTransport::GetDefaultReceiveTimeout()
{
	boost::mutex::scoped_lock lock(parameter_lock);
	return default_receive_timeout;
}
void TcpTransport::SetDefaultReceiveTimeout(int32_t milliseconds)
{
	if (!(milliseconds>0)) throw InvalidArgumentException("Timeout must be positive");
	boost::mutex::scoped_lock lock(parameter_lock);
	default_receive_timeout=milliseconds;
}
int32_t TcpTransport::GetDefaultConnectTimeout()
{
	boost::mutex::scoped_lock lock(parameter_lock);
	return default_connect_timeout;
}
void TcpTransport::SetDefaultConnectTimeout(int32_t milliseconds)
{
	if (!(milliseconds>0)) throw InvalidArgumentException("Timeout must be positive");
	boost::mutex::scoped_lock lock(parameter_lock);
	default_connect_timeout=milliseconds;
}

int32_t TcpTransport::GetMaxMessageSize()
{
	boost::mutex::scoped_lock lock(parameter_lock);
	return max_message_size;
}

void TcpTransport::SetMaxMessageSize(int32_t size)
{
	if (size < 16 * 1024 || size > 12 * 1024 * 1024) throw InvalidArgumentException("Invalid maximum message size");
	boost::mutex::scoped_lock lock(parameter_lock);
	max_message_size = size;
}

int32_t TcpTransport::GetMaxConnectionCount()
{
	boost::mutex::scoped_lock lock(parameter_lock);
	return max_connection_count;
}

void TcpTransport::SetMaxConnectionCount(int32_t count)
{
	if (count < -1) throw InvalidArgumentException("Invalid maximum connection count");
	boost::mutex::scoped_lock lock(parameter_lock);
	max_connection_count = count;	
}

bool TcpTransport::GetAcceptWebSockets()
{
	boost::mutex::scoped_lock lock(parameter_lock);
	return accept_websockets;
}

void TcpTransport::SetAcceptWebSockets(bool value)
{
	boost::mutex::scoped_lock lock(parameter_lock);
	accept_websockets=value;
}

std::vector<std::string> TcpTransport::GetWebSocketAllowedOrigins()
{
	boost::mutex::scoped_lock lock(parameter_lock);
	return allowed_websocket_origins;
}
void TcpTransport::AddWebSocketAllowedOrigin(const std::string& origin)
{
	boost::mutex::scoped_lock lock(parameter_lock);

	boost::smatch origin_result;
	if (!boost::regex_search(origin, origin_result, boost::regex("^([^:\\s]+)://(?:((?:\\[[A-Fa-f0-9\\:]+(?:\\%\\w*)?\\])|(?:[^\\[\\]\\:/\\?\\s]+))(?::([^\\:/\\?\\s]+))?)?$")))
	{
		throw InvalidArgumentException("Invalid WebSocket origin");
	}

	if (origin_result.size()<4) throw InvalidArgumentException("Invalid WebSocket origin");

	std::string host;
	std::string port;

	std::string scheme = origin_result[1];

	if (origin_result[2] != "")
	{
		host = origin_result[2];
		if (boost::starts_with(host, "*"))
		{
			std::string host2 = host.substr(1);

			if (host2.find("*") != std::string::npos)
			{
				throw InvalidArgumentException("Invalid WebSocket origin");
			}

			if (host2.size() != 0)
			{
				if (!boost::starts_with(host2, "."))
				{
					throw InvalidArgumentException("Invalid WebSocket origin");
				}
			}
		}
		else
		{
			if (host.find("*") != std::string::npos)
			{
				throw InvalidArgumentException("Invalid WebSocket origin");
			}
		}


		port = origin_result[3];

		if (port != "")
		{
			try
			{
				boost::lexical_cast<int>(port);
			}
			catch (std::exception)
			{
				throw InvalidArgumentException("Invalid WebSocket origin");
			}
		}

	}
	else if (origin_result[3] != "")
	{
		throw InvalidArgumentException("Invalid WebSocket origin");
	}

	allowed_websocket_origins.push_back(origin);

	if (scheme == "http" && port == "")
	{
		allowed_websocket_origins.push_back(origin + ":80");
	}

	if (scheme == "https" && port == "")
	{
		allowed_websocket_origins.push_back(origin + ":443");
	}

	if (scheme == "http" && port == "80")
	{
		allowed_websocket_origins.push_back(boost::replace_last_copy(origin, ":80", ""));
	}

	if (scheme == "https" && port == "443")
	{
		allowed_websocket_origins.push_back(boost::replace_last_copy(origin, ":443", ""));
	}
}
void TcpTransport::RemoveWebSocketAllowedOrigin(const std::string& origin)
{
	boost::mutex::scoped_lock lock(parameter_lock);
	allowed_websocket_origins.erase(std::remove(allowed_websocket_origins.begin(), allowed_websocket_origins.end(), origin), allowed_websocket_origins.end());
}

void TcpTransport::LoadTlsNodeCertificate()
{
#ifdef ROBOTRACONTEUR_WINDOWS
			
#ifdef ROBOTRACONTEUR_USE_SCHANNEL
			RR_SHARED_PTR<detail::TlsSchannelAsyncStreamAdapterContext> c=RR_STATIC_POINTER_CAST<detail::TlsSchannelAsyncStreamAdapterContext>(GetTlsContext());
			c->LoadCertificateFromMyStore();
#endif
#ifdef ROBOTRACONTEUR_USE_OPENSSL
			boost::scoped_array<char> sysdata_path1(new char[MAX_PATH]);
			if (FAILED(SHGetFolderPath(NULL,CSIDL_LOCAL_APPDATA | CSIDL_FLAG_CREATE,NULL,0,sysdata_path1.get())))
			{
			throw SystemResourceException("Could not get system information");
			}

			std::string sysdata_path(sysdata_path1.get());

			std::string certpath=sysdata_path + ROBOTRACONTEUR_PATHSEP + "RobotRaconteur" + ROBOTRACONTEUR_PATHSEP + "certificates" + ROBOTRACONTEUR_PATHSEP + GetNode()->NodeID().ToString() + ".p12";

			DWORD dwAttrib = GetFileAttributes(certpath.c_str());
			if (dwAttrib == INVALID_FILE_ATTRIBUTES)
			{
			throw SystemResourceException("Could not load noad certificate");
			}
			RR_SHARED_PTR<detail::OpenSSLAuthContext> c=RR_STATIC_POINTER_CAST<detail::OpenSSLAuthContext>(GetTlsContext());
			c->LoadPKCS12FromFile(certpath);
#endif

#else

			std::string certstore;

			char* certstoreenv=std::getenv("ROBOTRACONTEUR_USER_HOME");
			if (certstoreenv!=NULL)
			{
				std::string sysdata_path=std::string(certstoreenv);

				certstore=sysdata_path + ROBOTRACONTEUR_PATHSEP + ".config"  + ROBOTRACONTEUR_PATHSEP + "RobotRaconteur" + ROBOTRACONTEUR_PATHSEP + "certificates";

			}
			else
			{
				std::string sysdata_path=std::getenv("HOME");

				certstore=sysdata_path + ROBOTRACONTEUR_PATHSEP + ".config"  + ROBOTRACONTEUR_PATHSEP + "RobotRaconteur" + ROBOTRACONTEUR_PATHSEP + "certificates";
			}
			std::string certpath=certstore + ROBOTRACONTEUR_PATHSEP + GetNode()->NodeID().ToString() + ".p12";
			RR_SHARED_PTR<detail::OpenSSLAuthContext> c=RR_STATIC_POINTER_CAST<detail::OpenSSLAuthContext>(GetTlsContext());
			c->LoadPKCS12FromFile(certpath);


#endif


}

boost::shared_ptr<void> TcpTransport::GetTlsContext()
{
	boost::mutex::scoped_lock lock(tls_context_lock);
	if (!tls_context)
	{
#ifdef ROBOTRACONTEUR_USE_SCHANNEL
		NodeID id=GetNode()->NodeID();
		tls_context=RR_MAKE_SHARED<detail::TlsSchannelAsyncStreamAdapterContext>(id);
#endif

#ifdef ROBOTRACONTEUR_USE_OPENSSL
		NodeID id=GetNode()->NodeID();
		tls_context=RR_MAKE_SHARED<detail::OpenSSLAuthContext>(id);
#endif
		if (!tls_context) throw SystemResourceException("Could not initialize TLS");
	}
	
	return tls_context;

}

bool TcpTransport::GetRequireTls()
{
	boost::mutex::scoped_lock lock(tls_context_lock);
	return require_tls;
}

void TcpTransport::SetRequireTls(bool b)
{
	boost::mutex::scoped_lock lock(tls_context_lock);
	require_tls=b;
}

bool TcpTransport::IsTlsNodeCertificateLoaded()
{
#ifdef ROBOTRACONTEUR_USE_SCHANNEL
	RR_SHARED_PTR<detail::TlsSchannelAsyncStreamAdapterContext> c=RR_STATIC_POINTER_CAST<detail::TlsSchannelAsyncStreamAdapterContext>(GetTlsContext());
	return c->IsCertificateLoaded();
#endif

#ifdef ROBOTRACONTEUR_USE_OPENSSL
	RR_SHARED_PTR<detail::OpenSSLAuthContext> c=RR_STATIC_POINTER_CAST<detail::OpenSSLAuthContext>(GetTlsContext());
	return c->IsCertificateLoaded();
#endif

	throw NotImplementedException("Not implemented");
}

void TcpTransport_connected_callback2(RR_SHARED_PTR<TcpTransport> parent,RR_SHARED_PTR<boost::asio::ip::tcp::socket> socket, RR_SHARED_PTR<ITransportConnection> connection, RR_SHARED_PTR<RobotRaconteurException> err)
{
	//This is just an empty method.  The connected transport will register when it has a local endpoint.
	
}

void TcpTransport_attach_transport(RR_SHARED_PTR<TcpTransport> parent, RR_SHARED_PTR<boost::asio::ip::tcp::socket> socket,  std::string url, bool server, uint32_t endpoint, boost::function<void( RR_SHARED_PTR<boost::asio::ip::tcp::socket> , RR_SHARED_PTR<ITransportConnection> , RR_SHARED_PTR<RobotRaconteurException> )>& callback)
{
	try
	{
		RR_SHARED_PTR<TcpTransportConnection> t=RR_MAKE_SHARED<TcpTransportConnection>(parent,url,server,endpoint);
		boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)> h = boost::bind(callback, socket, t, _1);
		t->AsyncAttachSocket(socket,h);
		parent->AddCloseListener(t, &TcpTransportConnection::Close);
	}
	catch (std::exception& )
	{
		RobotRaconteurNode::TryPostToThreadPool(parent->GetNode(),boost::bind(callback,RR_SHARED_PTR<boost::asio::ip::tcp::socket>(), RR_SHARED_PTR<TcpTransportConnection>(),RR_MAKE_SHARED<ConnectionException>("Could not connect to service")),true);
	}

}

void TcpTransport::register_transport(RR_SHARED_PTR<ITransportConnection> connection)
{
	boost::mutex::scoped_lock lock(TransportConnections_lock);
	TransportConnections.insert(std::make_pair(connection->GetLocalEndpoint(),connection));
	//RR_WEAK_PTR<ITransportConnection> w=connection;
	//std::remove(incoming_TransportConnections.begin(), incoming_TransportConnections.end(), w);
}

void TcpTransport::incoming_transport(RR_SHARED_PTR<ITransportConnection> connection)
{
	boost::mutex::scoped_lock lock(TransportConnections_lock);
	//incoming_TransportConnections.push_back(connection);
}

void TcpTransport::erase_transport(RR_SHARED_PTR<ITransportConnection> connection)
{
	try
	{
		boost::mutex::scoped_lock lock(TransportConnections_lock);
		if (TransportConnections.count(connection->GetLocalEndpoint())!=0)
		{
			if (TransportConnections.at(connection->GetLocalEndpoint())==connection)
			{
			
				TransportConnections.erase(connection->GetLocalEndpoint());
			}
		}
	}
	catch (std::exception&) {}

	int32_t connection_count = 0;
	{
		boost::mutex::scoped_lock lock(TransportConnections_lock);
		connection_count = (int32_t)TransportConnections.size();
	}

	int32_t max_connection_count = GetMaxConnectionCount();

	boost::mutex::scoped_lock lock(acceptor_lock);
	if (max_connection_count > 0)
	{
		if (connection_count < max_connection_count)
		{
			if (ipv4_acceptor_paused)
			{
				RR_SHARED_PTR<boost::asio::ip::tcp::socket> socket2 = RR_MAKE_SHARED<boost::asio::ip::tcp::socket>(boost::ref(GetNode()->GetThreadPool()->get_io_service()));
				ipv4_acceptor->async_accept(*socket2, boost::bind(&TcpTransport::handle_v4_accept, shared_from_this(), ipv4_acceptor, socket2, boost::asio::placeholders::error));
				ipv4_acceptor_paused = false;
			}

			if (ipv6_acceptor_paused)
			{
				RR_SHARED_PTR<boost::asio::ip::tcp::socket> socket2 = RR_MAKE_SHARED<boost::asio::ip::tcp::socket>(boost::ref(GetNode()->GetThreadPool()->get_io_service()));
				ipv6_acceptor->async_accept(*socket2, boost::bind(&TcpTransport::handle_v6_accept, shared_from_this(), ipv6_acceptor, socket2, boost::asio::placeholders::error));
				ipv6_acceptor_paused = false;
			}

		}

	}

	TransportConnectionClosed(connection->GetLocalEndpoint());
}


void TcpTransport::MessageReceived(RR_INTRUSIVE_PTR<Message> m)
{
	GetNode()->MessageReceived(m);
}

void TcpTransport::EnableNodeDiscoveryListening(uint32_t flags)
{
		
	boost::mutex::scoped_lock lock(node_discovery_lock);
	if (!node_discovery)
	{
		node_discovery = RR_MAKE_SHARED<detail::IPNodeDiscovery>(shared_from_this());
	}
	boost::reinterpret_pointer_cast<detail::IPNodeDiscovery>(node_discovery)->StartListeningForNodes(flags);

}

void TcpTransport::DisableNodeDiscoveryListening()
{
	boost::mutex::scoped_lock lock(node_discovery_lock);
	if (!node_discovery)
		return;
	boost::reinterpret_pointer_cast<detail::IPNodeDiscovery>(node_discovery)->StopListeningForNodes();
}

void TcpTransport::EnableNodeAnnounce(uint32_t flags)
{
	boost::mutex::scoped_lock lock(node_discovery_lock);
	if (!node_discovery)
	{
		node_discovery = RR_MAKE_SHARED<detail::IPNodeDiscovery>(shared_from_this());
	}
	boost::reinterpret_pointer_cast<detail::IPNodeDiscovery>(node_discovery)->StartAnnouncingNode(flags);
}

void TcpTransport::DisableNodeAnnounce()
{
	boost::mutex::scoped_lock lock(node_discovery_lock);
	if (node_discovery == 0)
		return;
	boost::reinterpret_pointer_cast<detail::IPNodeDiscovery>(node_discovery)->StopAnnouncingNode();
}

int32_t TcpTransport::GetNodeAnnouncePeriod()
{
	boost::mutex::scoped_lock lock(node_discovery_lock);
	if (!node_discovery)
	{
		node_discovery = RR_MAKE_SHARED<detail::IPNodeDiscovery>(shared_from_this());
	}
	return boost::reinterpret_pointer_cast<detail::IPNodeDiscovery>(node_discovery)->GetNodeAnnouncePeriod();

}

void TcpTransport::SetNodeAnnouncePeriod(int32_t millis)
{
	boost::mutex::scoped_lock lock(node_discovery_lock);
	if (!node_discovery)
	{
		node_discovery = RR_MAKE_SHARED<detail::IPNodeDiscovery>(shared_from_this());
	}
	boost::reinterpret_pointer_cast<detail::IPNodeDiscovery>(node_discovery)->SetNodeAnnouncePeriod(millis);

}

void TcpTransport::SendDiscoveryRequest()
{
	boost::mutex::scoped_lock lock(node_discovery_lock);
	if (node_discovery)
	{
		boost::reinterpret_pointer_cast<detail::IPNodeDiscovery>(node_discovery)->SendDiscoveryRequestNow();
	}
}

bool TcpTransport::IsTransportConnectionSecure(uint32_t endpoint)
{
	RR_SHARED_PTR<ITransportConnection> t;
	{
		boost::mutex::scoped_lock lock(TransportConnections_lock);
		RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<ITransportConnection> >::iterator e1 = TransportConnections.find(endpoint);
		if (e1 == TransportConnections.end()) throw ConnectionException("Transport connection to remote host not found");
		t = e1->second;
	}

	return RR_DYNAMIC_POINTER_CAST<TcpTransportConnection>(t)->IsSecure();
}

bool TcpTransport::IsTransportConnectionSecure(RR_SHARED_PTR<Endpoint> endpoint)
{
	if (!endpoint) return false;

	RR_SHARED_PTR<ITransportConnection> t;			
	{
		boost::mutex::scoped_lock lock(TransportConnections_lock);
		RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<ITransportConnection> >::iterator e1 = TransportConnections.find(endpoint->GetLocalEndpoint());
		if (e1 == TransportConnections.end()) throw ConnectionException("Transport connection to remote host not found");
		t = e1->second;
	}
	
	return RR_DYNAMIC_POINTER_CAST<TcpTransportConnection>(t)->IsSecure();
}

bool TcpTransport::IsTransportConnectionSecure(RR_SHARED_PTR<RRObject> obj)
{
	RR_SHARED_PTR<ServiceStub> stub = RR_DYNAMIC_POINTER_CAST<ServiceStub>(obj);
	if (!stub) throw InvalidArgumentException("Object is not a connection");
	RR_SHARED_PTR<Endpoint> endpoint = stub->GetContext();

	RR_SHARED_PTR<ITransportConnection> t;

	{
		boost::mutex::scoped_lock lock(TransportConnections_lock);
		RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<ITransportConnection> >::iterator e1 = TransportConnections.find(endpoint->GetLocalEndpoint());
		if (e1 == TransportConnections.end()) throw ConnectionException("Transport connection to remote host not found");
		t = e1->second;
	}

	return RR_DYNAMIC_POINTER_CAST<TcpTransportConnection>(t)->IsSecure();
}

bool TcpTransport::IsTransportConnectionSecure(RR_SHARED_PTR<ITransportConnection> transport)
{
	RR_SHARED_PTR<TcpTransportConnection> t=RR_DYNAMIC_POINTER_CAST<TcpTransportConnection>(transport);
	if (!t) throw InvalidArgumentException("Invalid transport connection type");	

	return t->IsSecure();
}

bool TcpTransport::IsSecurePeerIdentityVerified(uint32_t endpoint)
{
	RR_SHARED_PTR<ITransportConnection> t;
	{
		boost::mutex::scoped_lock lock(TransportConnections_lock);
		RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<ITransportConnection> >::iterator e1 = TransportConnections.find(endpoint);
		if (e1 == TransportConnections.end()) throw ConnectionException("Transport connection to remote host not found");
		t = e1->second;
	}

	return RR_DYNAMIC_POINTER_CAST<TcpTransportConnection>(t)->IsSecurePeerIdentityVerified();

}

bool TcpTransport::IsSecurePeerIdentityVerified(RR_SHARED_PTR<Endpoint> endpoint)
{

	if (!endpoint) return false;

	RR_SHARED_PTR<ITransportConnection> t;	
	{
		boost::mutex::scoped_lock lock(TransportConnections_lock);
		RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<ITransportConnection> >::iterator e1 = TransportConnections.find(endpoint->GetLocalEndpoint());
		if (e1 == TransportConnections.end()) throw ConnectionException("Transport connection to remote host not found");
		t = e1->second;
	}
	
	return RR_DYNAMIC_POINTER_CAST<TcpTransportConnection>(t)->IsSecurePeerIdentityVerified();
	
}

bool TcpTransport::IsSecurePeerIdentityVerified(RR_SHARED_PTR<RRObject> obj)
{
	RR_SHARED_PTR<ServiceStub> stub = RR_DYNAMIC_POINTER_CAST<ServiceStub>(obj);
	if (!stub) throw InvalidArgumentException("Object is not a connection");
	RR_SHARED_PTR<Endpoint> endpoint = stub->GetContext();


	RR_SHARED_PTR<ITransportConnection> t;
	{
		boost::mutex::scoped_lock lock(TransportConnections_lock);
		RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<ITransportConnection> >::iterator e1 = TransportConnections.find(endpoint->GetLocalEndpoint());
		if (e1 == TransportConnections.end()) throw ConnectionException("Transport connection to remote host not found");
		t = e1->second;
	}

	return RR_DYNAMIC_POINTER_CAST<TcpTransportConnection>(t)->IsSecurePeerIdentityVerified();
}

bool TcpTransport::IsSecurePeerIdentityVerified(RR_SHARED_PTR<ITransportConnection> transport)
{
	RR_SHARED_PTR<TcpTransportConnection> t = RR_DYNAMIC_POINTER_CAST<TcpTransportConnection>(transport);
	if (!t) throw InvalidArgumentException("Invalid transport connection type");

	return t->IsSecurePeerIdentityVerified();
}

std::string TcpTransport::GetSecurePeerIdentity(uint32_t endpoint)
{
	RR_SHARED_PTR<ITransportConnection> t;
	{
		boost::mutex::scoped_lock lock(TransportConnections_lock);
		RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<ITransportConnection> >::iterator e1 = TransportConnections.find(endpoint);
		if (e1 == TransportConnections.end()) throw ConnectionException("Transport connection to remote host not found");
		t = e1->second;
	}

	return RR_DYNAMIC_POINTER_CAST<TcpTransportConnection>(t)->GetSecurePeerIdentity();

}

std::string TcpTransport::GetSecurePeerIdentity(RR_SHARED_PTR<Endpoint> endpoint)
{

	if (!endpoint) throw ConnectionException("Transport connection to remote host not found");

	RR_SHARED_PTR<ITransportConnection> t;
	{
		boost::mutex::scoped_lock lock(TransportConnections_lock);
		RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<ITransportConnection> >::iterator e1 = TransportConnections.find(endpoint->GetLocalEndpoint());
		if (e1 == TransportConnections.end()) throw ConnectionException("Transport connection to remote host not found");
		t = e1->second;
	}
	
	return RR_DYNAMIC_POINTER_CAST<TcpTransportConnection>(t)->GetSecurePeerIdentity();

}

std::string TcpTransport::GetSecurePeerIdentity(RR_SHARED_PTR<RRObject> obj)
{
	RR_SHARED_PTR<ServiceStub> stub = RR_DYNAMIC_POINTER_CAST<ServiceStub>(obj);
	if (!stub) throw InvalidArgumentException("Object is not a connection");
	RR_SHARED_PTR<Endpoint> endpoint = stub->GetContext();

	RR_SHARED_PTR<ITransportConnection> t;
	{
		boost::mutex::scoped_lock lock(TransportConnections_lock);
		RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<ITransportConnection> >::iterator e1 = TransportConnections.find(endpoint->GetLocalEndpoint());
		if (e1 == TransportConnections.end()) throw ConnectionException("Transport connection to remote host not found");
		t = e1->second;
	}

	return RR_DYNAMIC_POINTER_CAST<TcpTransportConnection>(t)->GetSecurePeerIdentity();

}

std::string TcpTransport::GetSecurePeerIdentity(RR_SHARED_PTR<ITransportConnection> transport)
{
	RR_SHARED_PTR<TcpTransportConnection> t = RR_DYNAMIC_POINTER_CAST<TcpTransportConnection>(transport);
	if (!t) throw InvalidArgumentException("Invalid transport connection type");


	return t->GetSecurePeerIdentity();

}

bool TcpTransport::GetDisableMessage3()
{
	boost::mutex::scoped_lock lock(parameter_lock);
	return disable_message3;
}
void TcpTransport::SetDisableMessage3(bool d)
{
	boost::mutex::scoped_lock lock(parameter_lock);
	disable_message3 = d;
}

bool TcpTransport::GetDisableStringTable()
{
	boost::mutex::scoped_lock lock(parameter_lock);
	return disable_string_table;
}
void TcpTransport::SetDisableStringTable(bool d) 
{
	boost::mutex::scoped_lock lock(parameter_lock);
	disable_string_table = d;
}

bool TcpTransport::GetDisableAsyncMessageIO()
{
	boost::mutex::scoped_lock lock(parameter_lock);
	return disable_async_message_io;
}
void TcpTransport::SetDisableAsyncMessageIO(bool d)
{
	boost::mutex::scoped_lock lock(parameter_lock);
	disable_async_message_io = d;
}

void TcpTransport::LocalNodeServicesChanged()
{
	boost::mutex::scoped_lock lock(node_discovery_lock);
	if (node_discovery)
	{
		boost::reinterpret_pointer_cast<detail::IPNodeDiscovery>(node_discovery)->SendAnnounceNow();
	}
}

TcpTransportConnection::TcpTransportConnection(RR_SHARED_PTR<TcpTransport> parent, std::string url, bool server, uint32_t local_endpoint) : ASIOStreamBaseTransport(parent->GetNode())
{
	this->parent=parent;
	this->server=server;
	this->m_LocalEndpoint=local_endpoint;
	this->m_RemoteEndpoint=0;
	this->ReceiveTimeout=parent->GetDefaultReceiveTimeout();
	this->HeartbeatPeriod=parent->GetDefaultHeartbeatPeriod();
	this->disable_message3 = parent->GetDisableMessage3();
	this->disable_string_table = parent->GetDisableStringTable();
	this->disable_async_io = parent->GetDisableAsyncMessageIO();
	this->url=url;

	this->is_tls=false;
	this->require_tls=parent->GetRequireTls();
	this->closing=false;
	this->max_message_size = parent->GetMaxMessageSize();
	this->tls_mutual_auth=false;
	this->tls_handshaking = false;
	this->use_websocket = false;
	this->use_wss_websocket = false;
}


void TcpTransportConnection::AsyncAttachSocket(RR_SHARED_PTR<boost::asio::ip::tcp::socket> socket, boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)>& callback)
{
	this->socket=socket;

	try
	{
		this->socket->set_option(boost::asio::ip::tcp::socket::linger(true,5));
	}
	catch (std::exception&) {}

	int send_timeout=15000;
	setsockopt(socket->native_handle(),SOL_SOCKET,SO_SNDTIMEO,(const char*)&send_timeout,sizeof(send_timeout));
	socket->set_option(boost::asio::ip::tcp::no_delay(true));

	std::string noden;
	if (!server)
	{
		ParseConnectionURLResult url_res = ParseConnectionURL(url);
		
		target_nodeid = url_res.nodeid;
		
		target_nodename = url_res.nodename;
		if (!(url_res.nodeid.IsAnyNode() && url_res.nodename != ""))
		{
			noden = url_res.nodeid.ToString();
		}
		else
		{
			noden = url_res.nodename;
		}
		
	}
	if (!server && boost::starts_with(url,"rrs"))
	{
		this->send_paused=true;
		this->send_pause_request=true;

		heartbeat_timer=RR_MAKE_SHARED<boost::asio::deadline_timer>(boost::ref(_io_service));
		{
			boost::mutex::scoped_lock lock(recv_lock);
			BeginReceiveMessage1();
		}

		
		string_table3 = RR_MAKE_SHARED<detail::StringTable>(server);

		boost::system::error_code ec;
		if (!RobotRaconteurNode::TryPostToThreadPool(node, boost::bind(&TcpTransportConnection::do_starttls1, RR_STATIC_POINTER_CAST<TcpTransportConnection>(shared_from_this()), noden, ec, callback)))
		{
			RobotRaconteurNode::TryPostToThreadPool(node, boost::bind(callback, RR_MAKE_SHARED<ConnectionException>("Node closed")),true);
		}

		return;
	}

	if (!server)
	{
		ASIOStreamBaseTransport::AsyncAttachStream(server,target_nodeid, target_nodename, callback);
		return;
	}
	else
	{
		ASIOStreamBaseTransport::AsyncAttachStream(server,target_nodeid, target_nodename, callback);
	}
	
}

void TcpTransportConnection::AsyncAttachWebSocket(RR_SHARED_PTR<boost::asio::ip::tcp::socket> socket, RR_SHARED_PTR<detail::websocket_stream<boost::asio::ip::tcp::socket&> > websocket, boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)>& callback)
{
	this->socket = socket;
	this->websocket = websocket;
	use_websocket = true;

	try
	{
		this->socket->set_option(boost::asio::ip::tcp::socket::linger(true, 5));
	}
	catch (std::exception&) {}

	int send_timeout = 15000;
	setsockopt(socket->native_handle(), SOL_SOCKET, SO_SNDTIMEO, (const char*)&send_timeout, sizeof(send_timeout));
	socket->set_option(boost::asio::ip::tcp::no_delay(true));

	std::string noden;
	if (!server)
	{
		ParseConnectionURLResult url_res = ParseConnectionURL(url);

		target_nodeid = url_res.nodeid;

		target_nodename = url_res.nodename;

		if (!(url_res.nodeid.IsAnyNode() && url_res.nodename != ""))
		{
			noden = url_res.nodeid.ToString();
		}
		else
		{
			noden = url_res.nodename;
		}
	}
	if (!server && boost::starts_with(url, "rrs+ws://"))
	{
		this->send_paused = true;
		this->send_pause_request = true;

		string_table3 = RR_MAKE_SHARED<detail::StringTable>(server);

		heartbeat_timer = RR_MAKE_SHARED<boost::asio::deadline_timer>(boost::ref(_io_service));
		{
			boost::mutex::scoped_lock lock(recv_lock);
			BeginReceiveMessage1();
		}

		boost::system::error_code ec;
		if (!RobotRaconteurNode::TryPostToThreadPool(node, boost::bind(&TcpTransportConnection::do_starttls1, RR_STATIC_POINTER_CAST<TcpTransportConnection>(shared_from_this()), noden, ec, callback)))
		{
			RobotRaconteurNode::TryPostToThreadPool(node, boost::bind(callback, RR_MAKE_SHARED<ConnectionException>("Node closed")), true);
		}

		
		return;
	}

	if (!server)
	{
		ASIOStreamBaseTransport::AsyncAttachStream(server, target_nodeid, target_nodename, callback);
		return;
	}
	else
	{
		ASIOStreamBaseTransport::AsyncAttachStream(server, target_nodeid, target_nodename, callback);
	}

}

#ifdef ROBOTRACONTEUR_USE_SCHANNEL
void TcpTransportConnection::AsyncAttachWSSWebSocket(RR_SHARED_PTR<boost::asio::ip::tcp::socket> socket, RR_SHARED_PTR<detail::TlsSchannelAsyncStreamAdapter> wss_websocket_tls, RR_SHARED_PTR<detail::websocket_stream<detail::TlsSchannelAsyncStreamAdapter_ASIO_adapter&> > wss_websocket, boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)>& callback)
{
	this->socket = socket;
	
	this->wss_websocket = wss_websocket;
	this->wss_websocket_tls = wss_websocket_tls;
	use_wss_websocket = true;

	try
	{
		this->socket->set_option(boost::asio::ip::tcp::socket::linger(true, 5));
	}
	catch (std::exception&) {}

	int send_timeout = 15000;
	setsockopt(socket->native_handle(), SOL_SOCKET, SO_SNDTIMEO, (const char*)&send_timeout, sizeof(send_timeout));
	socket->set_option(boost::asio::ip::tcp::no_delay(true));

	std::string noden;
	if (!server)
	{
		ParseConnectionURLResult url_res = ParseConnectionURL(url);

		target_nodeid = url_res.nodeid;

		target_nodename = url_res.nodename;

		if (!(url_res.nodeid.IsAnyNode() && url_res.nodename != ""))
		{
			noden = url_res.nodeid.ToString();
		}
		else
		{
			noden = url_res.nodename;
		}
	}
	if (!server && boost::starts_with(url, "rrs+wss://"))
	{
		this->send_paused = true;
		this->send_pause_request = true;

		string_table3 = RR_MAKE_SHARED<detail::StringTable>(server);

		heartbeat_timer = RR_MAKE_SHARED<boost::asio::deadline_timer>(boost::ref(_io_service));
		{
			boost::mutex::scoped_lock lock(recv_lock);
			BeginReceiveMessage1();
		}


		boost::system::error_code ec;		
		if (!RobotRaconteurNode::TryPostToThreadPool(node, boost::bind(&TcpTransportConnection::do_starttls1, RR_STATIC_POINTER_CAST<TcpTransportConnection>(shared_from_this()), noden, ec, callback)))
		{
			RobotRaconteurNode::TryPostToThreadPool(node, boost::bind(callback, RR_MAKE_SHARED<ConnectionException>("Node closed")), true);
		}
		return;
	}

	if (!server)
	{
		ASIOStreamBaseTransport::AsyncAttachStream(server, target_nodeid, target_nodename, callback);
		return;
	}
	else
	{
		ASIOStreamBaseTransport::AsyncAttachStream(server, target_nodeid, target_nodename, callback);
	}

}
#endif

#ifdef ROBOTRACONTEUR_USE_OPENSSL
void TcpTransportConnection::AsyncAttachWSSWebSocket(RR_SHARED_PTR<boost::asio::ip::tcp::socket> socket, RR_SHARED_PTR<boost::asio::ssl::stream<boost::asio::ip::tcp::socket& > > wss_websocket_tls, RR_SHARED_PTR<detail::websocket_stream<boost::asio::ssl::stream<boost::asio::ip::tcp::socket &> &> >  wss_websocket, RR_SHARED_PTR<boost::asio::ssl::context> wss_context, boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)>& callback)
{
	this->socket = socket;

	this->wss_websocket = wss_websocket;
	this->wss_websocket_tls = wss_websocket_tls;
	this->wss_context=wss_context;
	use_wss_websocket = true;

	try
	{
		this->socket->set_option(boost::asio::ip::tcp::socket::linger(true, 5));
	}
	catch (std::exception&) {}

	int send_timeout = 15000;
	setsockopt(socket->native_handle(), SOL_SOCKET, SO_SNDTIMEO, (const char*)&send_timeout, sizeof(send_timeout));
	socket->set_option(boost::asio::ip::tcp::no_delay(true));

	std::string noden;
	if (!server)
	{
		ParseConnectionURLResult url_res = ParseConnectionURL(url);

		target_nodeid = url_res.nodeid;

		target_nodename = url_res.nodename;

		if (!(url_res.nodeid.IsAnyNode() && url_res.nodename != ""))
		{
			noden = url_res.nodeid.ToString();
		}
		else
		{
			noden = url_res.nodename;
		}
	}
	if (!server && boost::starts_with(url, "rrs+wss://"))
	{
		this->send_paused = true;
		this->send_pause_request = true;

		heartbeat_timer = RR_MAKE_SHARED<boost::asio::deadline_timer>(boost::ref(_io_service));
		{
			boost::mutex::scoped_lock lock(recv_lock);
			BeginReceiveMessage1();
		}


		boost::system::error_code ec;		
		if (!RobotRaconteurNode::TryPostToThreadPool(node, boost::bind(&TcpTransportConnection::do_starttls1, RR_STATIC_POINTER_CAST<TcpTransportConnection>(shared_from_this()), noden, ec, callback)))
		{
			detail::PostHandlerWithException(node, callback, RR_MAKE_SHARED<ConnectionException>("Node closed"), true, false);
		}
		return;
	}

	if (!server)
	{
		ASIOStreamBaseTransport::AsyncAttachStream(server, target_nodeid, target_nodename, callback);
		return;
	}
	else
	{
		ASIOStreamBaseTransport::AsyncAttachStream(server, target_nodeid, target_nodename, callback);
	}

}
#endif

/*void TcpTransportConnection::do_starttls3(RR_SHARED_PTR<RobotRaconteurException> error, boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)>& callback)
{
	if (error)
	{
		callback(error);
		return;
	}

	AsyncPauseSend(boost::bind(&TcpTransportConnection::do_starttls1, RR_STATIC_POINTER_CAST<TcpTransportConnection>(shared_from_this()), _1,boost::protect(callback)));
}*/

void TcpTransportConnection::do_starttls1(std::string noden, const boost::system::error_code& error, boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)>& callback)
{
	if (error)
	{
		callback(RR_MAKE_SHARED<ConnectionException>("Could not initialize TLS connection"));
		return;
	}

	

	try
	{
		//TODO: Fix the node id
		boost::mutex::scoped_lock lock(streamop_lock);
		//TODO: cancel if closed
		
		NodeID target_nodeid=NodeID::GetAny();
		std::string target_nodename;
		if (noden.find("{")!=std::string::npos)
		{
			target_nodeid=NodeID(noden);
		}
		else
		{
			target_nodename=noden;
		}

		RR_INTRUSIVE_PTR<Message> m = CreateMessage();
		m->header = CreateMessageHeader();
		m->header->ReceiverNodeName = target_nodename;
		//m->header->SenderNodeName = GetNode()->NodeName();
		//m->header->SenderNodeID = GetNode()->NodeID();
		m->header->ReceiverNodeID = target_nodeid;
		
		RR_INTRUSIVE_PTR<MessageEntry> mm=CreateMessageEntry(MessageEntryType_StreamOp,"STARTTLS");
		
		RR_SHARED_PTR<TcpTransport> p = parent.lock();
		if (!p) throw InvalidOperationException("Transport shutdown");

#ifdef ROBOTRACONTEUR_USE_SCHANNEL
		RR_SHARED_PTR<detail::TlsSchannelAsyncStreamAdapterContext> context = RR_STATIC_POINTER_CAST<detail::TlsSchannelAsyncStreamAdapterContext>(p->GetTlsContext());
		if (context->IsCertificateLoaded())
		{
			mm->AddElement("mutualauth", stringToRRArray("true"));
		}
#endif

#ifdef ROBOTRACONTEUR_USE_OPENSSL
		RR_SHARED_PTR<detail::OpenSSLAuthContext> context = RR_STATIC_POINTER_CAST<detail::OpenSSLAuthContext>(p->GetTlsContext());
		if (context->IsCertificateLoaded())
		{
			mm->AddElement("mutualauth", stringToRRArray("true"));
		}
#endif

		m->entries.push_back(mm);

		if (starttls_timer)
		{
			lock.unlock();
			callback(RR_MAKE_SHARED<ConnectionException>("Could not initialize TLS connection"));
			return;
		}

		starttls_handler=callback;

		starttls_timer=RR_MAKE_SHARED<boost::asio::deadline_timer>(boost::ref(_io_service),boost::posix_time::milliseconds(5000));
		//RR_WEAK_PTR<ASIOStreamBaseTransport> t=RR_STATIC_POINTER_CAST<ASIOStreamBaseTransport>(shared_from_this());
		boost::system::error_code ec1(boost::system::errc::timed_out, boost::system::generic_category());
		RobotRaconteurNode::asio_async_wait(node, starttls_timer, boost::bind(&TcpTransportConnection::do_starttls4, RR_STATIC_POINTER_CAST<TcpTransportConnection>(shared_from_this()),"",ec1));
		streamop_waiting=true;
		tls_handshaking = true;
		lock.unlock();
		{
			boost::mutex::scoped_lock lock2(send_lock);
			boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)> h = boost::bind(&TcpTransportConnection::do_starttls2, RR_STATIC_POINTER_CAST<TcpTransportConnection>(shared_from_this()), _1, callback);
			BeginSendMessage(m, h);
		}


	}
	catch (std::exception)
	{
		
		{
			boost::mutex::scoped_lock lock(streamop_lock);
			starttls_handler.clear();
			starttls_timer.reset();
			streamop_waiting = false;
			tls_handshaking = false;
		}
		callback(RR_MAKE_SHARED<ConnectionException>("Could not initialize TLS connection"));
		return;
	}

	//callback(RR_SHARED_PTR<RobotRaconteurException>());
}

void TcpTransportConnection::do_starttls2(RR_SHARED_PTR<RobotRaconteurException> error, boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)>& callback)
{
	if (error)
	{
		{
			boost::mutex::scoped_lock lock(streamop_lock);
			starttls_handler.clear();
			starttls_timer.reset();
		}
		callback(RR_MAKE_SHARED<ConnectionException>("Could not initialize TLS connection"));
		return;
	}
}


namespace detail
{
	static void TcpTransportConnection_websocket_read_adapter(detail::websocket_stream<boost::asio::ip::tcp::socket&> * sock, mutable_buffers& b, boost::function<void(const boost::system::error_code&, size_t)>& handler)
	{
		sock->async_read_some(b, handler);
	}

	static void TcpTransportConnection_websocket_write_adapter(detail::websocket_stream<boost::asio::ip::tcp::socket&>* sock, const_buffers& b, boost::function<void(const boost::system::error_code&, size_t)>& handler)
	{
		sock->async_write_some(b, handler);
	}

	static void TcpTransportConnection_websocket_close_adapter(detail::websocket_stream<boost::asio::ip::tcp::socket&>* sock)
	{
		sock->close();
	}

#ifdef ROBOTRACONTEUR_USE_SCHANNEL
	static void TcpTransportConnection_wss_websocket_read_adapter(detail::websocket_stream<detail::TlsSchannelAsyncStreamAdapter_ASIO_adapter&> * sock, mutable_buffers& b, boost::function<void(const boost::system::error_code&, size_t)>& handler)
	{
		sock->async_read_some(b, handler);
	}

	static void TcpTransportConnection_wss_websocket_write_adapter(detail::websocket_stream<detail::TlsSchannelAsyncStreamAdapter_ASIO_adapter&>* sock, const_buffers& b, boost::function<void(const boost::system::error_code&, size_t)>& handler)
	{
		sock->async_write_some(b, handler);
	}

	static void TcpTransportConnection_wss_websocket_close_adapter(detail::websocket_stream<detail::TlsSchannelAsyncStreamAdapter_ASIO_adapter&>* sock)
	{
		sock->close();
	}
#endif
}

void TcpTransportConnection::do_starttls4(const std::string& servername, const boost::system::error_code& error)
{
	boost::mutex::scoped_lock lock(streamop_lock);

	boost::system::error_code ec2 = error;

	if (!tls_handshaking)
	{
		return;
	}
	tls_handshaking = false;

	if (ec2)
	{
		boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)> starttls_handler1=starttls_handler;
		starttls_handler.clear();
		starttls_timer.reset();
		lock.unlock();
		detail::InvokeHandlerWithException(node, starttls_handler, RR_MAKE_SHARED<ConnectionException>("Could not initialize TLS connection"));
		return;
	}

	try
	{

		starttls_timer.reset();

		RR_SHARED_PTR<TcpTransport> p = parent.lock();
		if (!p) throw InvalidOperationException("Transport shutdown");

#ifdef ROBOTRACONTEUR_USE_SCHANNEL
		if (!use_websocket && !use_wss_websocket)
		{
			RR_WEAK_PTR<boost::asio::ip::tcp::socket> socket1 = socket;
			RR_SHARED_PTR<detail::TlsSchannelAsyncStreamAdapterContext> context = RR_STATIC_POINTER_CAST<detail::TlsSchannelAsyncStreamAdapterContext>(p->GetTlsContext());
			tls_socket = RR_MAKE_SHARED<RobotRaconteur::detail::TlsSchannelAsyncStreamAdapter>(boost::ref(socket->get_io_service()),
				context,
				RobotRaconteur::detail::TlsSchannelAsyncStreamAdapter::client,
				servername,
				boost::bind(detail::TcpTransportConnection_socket_read_adapter, node, socket1, _1, _2),
				boost::bind(detail::TcpTransportConnection_socket_write_adapter, node, socket1, _1, _2),
				boost::bind(detail::TcpTransportConnection_socket_close_adapter, socket1)
				);
			RobotRaconteurNode::asio_async_handshake(node, tls_socket, boost::bind(&TcpTransportConnection::do_starttls5, RR_STATIC_POINTER_CAST<TcpTransportConnection>(shared_from_this()), _1));
		}
		else if (use_websocket)
		{
			RR_SHARED_PTR<detail::TlsSchannelAsyncStreamAdapterContext> context = RR_STATIC_POINTER_CAST<detail::TlsSchannelAsyncStreamAdapterContext>(p->GetTlsContext());
			tls_websocket = RR_MAKE_SHARED<RobotRaconteur::detail::TlsSchannelAsyncStreamAdapter>(boost::ref(socket->get_io_service()),
				context,
				RobotRaconteur::detail::TlsSchannelAsyncStreamAdapter::client,
				servername,
				boost::bind(detail::TcpTransportConnection_websocket_read_adapter, websocket.get(), _1, _2),
				boost::bind(detail::TcpTransportConnection_websocket_write_adapter, websocket.get(), _1, _2),
				boost::bind(detail::TcpTransportConnection_websocket_close_adapter, websocket.get())
				);
			RobotRaconteurNode::asio_async_handshake(node, tls_websocket, boost::bind(&TcpTransportConnection::do_starttls5, RR_STATIC_POINTER_CAST<TcpTransportConnection>(shared_from_this()), _1));

		}
		else
		{
			RR_SHARED_PTR<detail::TlsSchannelAsyncStreamAdapterContext> context = RR_STATIC_POINTER_CAST<detail::TlsSchannelAsyncStreamAdapterContext>(p->GetTlsContext());
			tls_wss_websocket = RR_MAKE_SHARED<RobotRaconteur::detail::TlsSchannelAsyncStreamAdapter>(boost::ref(socket->get_io_service()),
				context,
				RobotRaconteur::detail::TlsSchannelAsyncStreamAdapter::client,
				servername,
				boost::bind(detail::TcpTransportConnection_wss_websocket_read_adapter, wss_websocket.get(), _1, _2),
				boost::bind(detail::TcpTransportConnection_wss_websocket_write_adapter, wss_websocket.get(), _1, _2),
				boost::bind(detail::TcpTransportConnection_wss_websocket_close_adapter, wss_websocket.get())
				);
			RobotRaconteurNode::asio_async_handshake(node, tls_wss_websocket, boost::bind(&TcpTransportConnection::do_starttls5, RR_STATIC_POINTER_CAST<TcpTransportConnection>(shared_from_this()), _1));
		}
#endif

#ifdef ROBOTRACONTEUR_USE_OPENSSL
		if (!use_websocket && !use_wss_websocket)
		{
			tls_context=RR_STATIC_POINTER_CAST<detail::OpenSSLAuthContext>(p->GetTlsContext());
			tls_socket = RR_MAKE_SHARED<boost::asio::ssl::stream<boost::asio::ip::tcp::socket&> >(boost::ref(*socket), boost::ref(*(tls_context->GetClientCredentials())));
			RobotRaconteurNode::asio_async_handshake(node, tls_socket, boost::asio::ssl::stream<boost::asio::ip::tcp::socket&>::client, boost::bind(&TcpTransportConnection::do_starttls5, RR_STATIC_POINTER_CAST<TcpTransportConnection>(shared_from_this()), _1));
		}
		else if (use_websocket)
		{
			tls_context=RR_STATIC_POINTER_CAST<detail::OpenSSLAuthContext>(p->GetTlsContext());
			tls_websocket = RR_MAKE_SHARED<boost::asio::ssl::stream<detail::websocket_stream<boost::asio::ip::tcp::socket&> &> >(boost::ref(*websocket), boost::ref(*(tls_context->GetClientCredentials())));
			RobotRaconteurNode::asio_async_handshake(node, tls_websocket, boost::asio::ssl::stream<detail::websocket_stream<boost::asio::ip::tcp::socket&> &>::client, boost::bind(&TcpTransportConnection::do_starttls5, RR_STATIC_POINTER_CAST<TcpTransportConnection>(shared_from_this()), _1));

		}
		else
		{

			tls_context=RR_STATIC_POINTER_CAST<detail::OpenSSLAuthContext>(p->GetTlsContext());
			tls_wss_websocket = RR_MAKE_SHARED<boost::asio::ssl::stream<detail::websocket_stream<boost::asio::ssl::stream<boost::asio::ip::tcp::socket&> &> &> >(boost::ref(*wss_websocket), boost::ref(*(tls_context->GetClientCredentials())));
			RobotRaconteurNode::asio_async_handshake(node, tls_wss_websocket, boost::asio::ssl::stream<detail::websocket_stream<boost::asio::ssl::stream<boost::asio::ip::tcp::socket&> &> &>::client, boost::bind(&TcpTransportConnection::do_starttls5, RR_STATIC_POINTER_CAST<TcpTransportConnection>(shared_from_this()), _1));

		}

#endif
	}
	catch (std::exception&)
	{
		boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)> starttls_handler1 = starttls_handler;
		starttls_handler.clear();
		starttls_timer.reset();
		lock.unlock();
		detail::InvokeHandlerWithException(node, starttls_handler1, RR_MAKE_SHARED<ConnectionException>("Could not initialize TLS connection"));
		return;
	}


}

void TcpTransportConnection::do_starttls5(const boost::system::error_code& error)
{
	boost::mutex::scoped_lock lock(streamop_lock);
	if (error)
	{
		boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)> starttls_handler1=starttls_handler;
		starttls_handler.clear();
		starttls_timer.reset();
		lock.unlock();
		detail::InvokeHandlerWithException(node, starttls_handler1, RR_MAKE_SHARED<ConnectionException>("Could not initialize TLS connection"));
		return;
	}

	starttls_timer.reset();
	

	{
		boost::mutex::scoped_lock lock1(socket_lock);
		is_tls=true;
	}

	boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)> starttls_handler1=starttls_handler;
	starttls_handler.clear();
	lock.unlock();

	NodeID RemoteNodeID1;
	{
		boost::shared_lock<boost::shared_mutex> lock(RemoteNodeID_lock);
		RemoteNodeID1=RemoteNodeID;
	}
#ifdef ROBOTRACONTEUR_USE_SCHANNEL
	if (!use_websocket && !use_wss_websocket)
	{
		if (!tls_socket->VerifyRemoteNodeCertificate(RemoteNodeID1))
		{
			detail::InvokeHandlerWithException(node, starttls_handler1, RR_MAKE_SHARED<AuthenticationException>("Could not verify server TLS certificate"));
			return;
		}
	}
	else if (use_websocket)
	{
		if (!tls_websocket->VerifyRemoteNodeCertificate(RemoteNodeID1))
		{
			detail::InvokeHandlerWithException(node, starttls_handler1, RR_MAKE_SHARED<AuthenticationException>("Could not verify server TLS certificate"));
			return;
		}
	}
	else
	{
		if (!tls_wss_websocket->VerifyRemoteNodeCertificate(RemoteNodeID1))
		{
			detail::InvokeHandlerWithException(node, starttls_handler1, RR_MAKE_SHARED<AuthenticationException>("Could not verify server TLS certificate"));
			return;
		}
	}
#endif

#ifdef ROBOTRACONTEUR_USE_OPENSSL
	
	if (!use_websocket && !use_wss_websocket)
	{
		if (!tls_context->VerifyRemoteNodeCertificate(tls_socket->native_handle(),RemoteNodeID1))
		{
			detail::InvokeHandlerWithException(node, starttls_handler1, RR_MAKE_SHARED<AuthenticationException>("Could not verify server TLS certificate"));
			return;
		}
	}
	else if (use_websocket)
	{
		if (!tls_context->VerifyRemoteNodeCertificate(tls_websocket->native_handle(),RemoteNodeID1))
		{
			detail::InvokeHandlerWithException(node, starttls_handler1, RR_MAKE_SHARED<AuthenticationException>("Could not verify server TLS certificate"));
			return;
		}

	}
	else
	{
		if (!tls_context->VerifyRemoteNodeCertificate(tls_wss_websocket->native_handle(),RemoteNodeID1))
		{
			detail::InvokeHandlerWithException(node, starttls_handler1, RR_MAKE_SHARED<AuthenticationException>("Could not verify server TLS certificate"));
			return;
		}
	}

#endif

	//lock.unlock();

	try
	{
	AsyncResumeSend();
	AsyncResumeReceive();
	}
	catch (std::exception&)
	{				
		detail::InvokeHandlerWithException(node, starttls_handler1, RR_MAKE_SHARED<ConnectionException>("Could not initialize TLS connection"));
		return;
	}
	
	try
	{
		RR_SHARED_PTR<AsyncAttachStream_args> args=RR_MAKE_SHARED<AsyncAttachStream_args>(target_nodeid, target_nodename);

		boost::function<void(RR_SHARED_PTR<RRObject>, RR_SHARED_PTR<RobotRaconteurException>)> h = boost::bind(&TcpTransportConnection::do_starttls5_1, RR_STATIC_POINTER_CAST<TcpTransportConnection>(shared_from_this()), _1, _2, starttls_handler1);
		AsyncStreamOp("CreateConnection", args, h);

		//starttls_handler1(RR_SHARED_PTR<RobotRaconteurException>());
		return;
	}
	catch(std::exception& e)
	{
		detail::InvokeHandlerWithException(node, starttls_handler1, RR_MAKE_SHARED<ConnectionException>(e.what()));
	}

}

void TcpTransportConnection::do_starttls5_1(RR_SHARED_PTR<RRObject> parameter, RR_SHARED_PTR<RobotRaconteurException> err, boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)>& callback)
{
	//std::cout << "AsyncAttachStream1" << std::endl;
	if (err)
	{
		try
		{
			Close();
		}
		catch (std::exception&) {}
		RobotRaconteurNode::TryPostToThreadPool(node,boost::bind(callback,err),true);
		return;
	}	

	try
	{		
		{
			boost::mutex::scoped_lock lock(heartbeat_timer_lock);
			if (heartbeat_timer)
			{
				heartbeat_timer->expires_from_now(boost::posix_time::milliseconds(400));
				heartbeat_timer->async_wait(boost::bind(&TcpTransportConnection::heartbeat_timer_func, RR_STATIC_POINTER_CAST<TcpTransportConnection>(shared_from_this()),boost::asio::placeholders::error));  
			}

		}

		RobotRaconteurNode::TryPostToThreadPool(node,boost::bind(callback,RR_SHARED_PTR<ConnectionException>()), true);
	}
	catch (std::exception& e)
	{
		RobotRaconteurNode::TryPostToThreadPool(node, boost::bind(callback,RR_MAKE_SHARED<ConnectionException>(e.what())), true);
	}

}

void TcpTransportConnection::do_starttls6(const boost::system::error_code& error, RR_INTRUSIVE_PTR<Message> request)
{
	if(error)
	{
		Close();
		return;
	}

	boost::function<void(const boost::system::error_code&) > h = boost::bind(&TcpTransportConnection::do_starttls7, RR_STATIC_POINTER_CAST<TcpTransportConnection>(shared_from_this()), _1, request);
	AsyncPauseSend(h);
}

void TcpTransportConnection::do_starttls7(const boost::system::error_code& error, RR_INTRUSIVE_PTR<Message> request)
{
	if(error)
	{
		Close();
		return;
	}

	boost::mutex::scoped_lock lock(send_lock);

	/*{
		boost::mutex::scoped_lock lock1(socket_lock);
		is_tls=true;
	}*/

	
	RR_INTRUSIVE_PTR<Message> mret = CreateMessage();
	mret->header = CreateMessageHeader();
	mret->header->SenderNodeName = GetNode()->NodeName();
	mret->header->ReceiverNodeName = request->header->SenderNodeName;
	mret->header->SenderNodeID = GetNode()->NodeID();
	mret->header->ReceiverNodeID = request->header->SenderNodeID;
	RR_INTRUSIVE_PTR<MessageEntry> mmret = CreateMessageEntry(MessageEntryType_StreamOpRet,"STARTTLS");
	RR_SHARED_PTR<TcpTransport> p = parent.lock();
	if (!p)
	{
		Close();
		return;
	}

	if (!p->IsTlsNodeCertificateLoaded())
	{
		mmret->Error = MessageErrorType_ConnectionError;
		mmret->AddElement("errorname", stringToRRArray("RobotRaconteur.ConnectionError"));
		mmret->AddElement("errorstring", stringToRRArray("Server certificate not loaded"));

	}
	else
	{

		if (tls_mutual_auth)
		{
			mmret->AddElement("mutualauth", stringToRRArray("true"));
		}
	}
	mret->entries.push_back(mmret);
	boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)> h = boost::bind(&TcpTransportConnection::do_starttls8, RR_STATIC_POINTER_CAST<TcpTransportConnection>(shared_from_this()), _1, request);
	BeginSendMessage(mret, h);
}

void TcpTransportConnection::do_starttls8(RR_SHARED_PTR<RobotRaconteurException> error, RR_INTRUSIVE_PTR<Message> request)
{
	if(error)
	{
		Close();
		return;
	}
	RR_SHARED_PTR<TcpTransport> p=parent.lock();
	if (!p) throw InvalidOperationException("Transport shutdown");
#ifdef ROBOTRACONTEUR_USE_SCHANNEL
	RR_SHARED_PTR<detail::TlsSchannelAsyncStreamAdapterContext> context=RR_STATIC_POINTER_CAST<detail::TlsSchannelAsyncStreamAdapterContext>(p->GetTlsContext());
	
	if (!use_websocket && !use_wss_websocket)
	{
		RR_WEAK_PTR<boost::asio::ip::tcp::socket> socket1 = socket;
		tls_socket = RR_MAKE_SHARED<RobotRaconteur::detail::TlsSchannelAsyncStreamAdapter>(boost::ref(socket->get_io_service()),
			context,
			RobotRaconteur::detail::TlsSchannelAsyncStreamAdapter::server,
			"",
			boost::bind(detail::TcpTransportConnection_socket_read_adapter, node, socket1, _1, _2),
			boost::bind(detail::TcpTransportConnection_socket_write_adapter, node, socket1, _1, _2),
			boost::bind(detail::TcpTransportConnection_socket_close_adapter, socket1)
			);
		tls_socket->set_mutual_auth(tls_mutual_auth);
		RobotRaconteurNode::asio_async_handshake(node, tls_socket, boost::bind(&TcpTransportConnection::do_starttls9, RR_STATIC_POINTER_CAST<TcpTransportConnection>(shared_from_this()),_1));
	}
	else if (use_websocket)
	{
		tls_websocket = RR_MAKE_SHARED<RobotRaconteur::detail::TlsSchannelAsyncStreamAdapter>(boost::ref(socket->get_io_service()),
			context,
			RobotRaconteur::detail::TlsSchannelAsyncStreamAdapter::server,
			"",
			boost::bind(detail::TcpTransportConnection_websocket_read_adapter, websocket.get(), _1, _2),
			boost::bind(detail::TcpTransportConnection_websocket_write_adapter, websocket.get(), _1, _2),
			boost::bind(detail::TcpTransportConnection_websocket_close_adapter, websocket.get())
			);
		tls_websocket->set_mutual_auth(tls_mutual_auth);
		RobotRaconteurNode::asio_async_handshake(node, tls_websocket, boost::bind(&TcpTransportConnection::do_starttls9, RR_STATIC_POINTER_CAST<TcpTransportConnection>(shared_from_this()),_1));

	}
	else
	{
		tls_wss_websocket = RR_MAKE_SHARED<RobotRaconteur::detail::TlsSchannelAsyncStreamAdapter>(boost::ref(socket->get_io_service()),
			context,
			RobotRaconteur::detail::TlsSchannelAsyncStreamAdapter::server,
			"",
			boost::bind(detail::TcpTransportConnection_wss_websocket_read_adapter, wss_websocket.get(), _1, _2),
			boost::bind(detail::TcpTransportConnection_wss_websocket_write_adapter, wss_websocket.get(), _1, _2),
			boost::bind(detail::TcpTransportConnection_wss_websocket_close_adapter, wss_websocket.get())
			);
		tls_wss_websocket->set_mutual_auth(tls_mutual_auth);
		RobotRaconteurNode::asio_async_handshake(node, tls_wss_websocket,boost::bind(&TcpTransportConnection::do_starttls9, RR_STATIC_POINTER_CAST<TcpTransportConnection>(shared_from_this()),_1));

	}
#endif


#ifdef ROBOTRACONTEUR_USE_OPENSSL
	if (!use_websocket && !use_wss_websocket)
	{
		tls_context=RR_STATIC_POINTER_CAST<detail::OpenSSLAuthContext>(p->GetTlsContext());
		tls_socket=RR_MAKE_SHARED<boost::asio::ssl::stream<boost::asio::ip::tcp::socket&> >(boost::ref(*socket),boost::ref(*(tls_context->GetServerCredentials())));

		if (tls_mutual_auth)
		{
			::SSL_set_verify(tls_socket->native_handle(), SSL_VERIFY_PEER ,::SSL_CTX_get_verify_callback(tls_context->GetServerCredentials()->native_handle()));
		}
		RobotRaconteurNode::asio_async_handshake(node, tls_socket, boost::asio::ssl::stream<boost::asio::ip::tcp::socket&>::server, boost::bind(&TcpTransportConnection::do_starttls9, RR_STATIC_POINTER_CAST<TcpTransportConnection>(shared_from_this()),_1));
	}
	else if (use_websocket)
	{
		tls_context=RR_STATIC_POINTER_CAST<detail::OpenSSLAuthContext>(p->GetTlsContext());
		tls_websocket=RR_MAKE_SHARED<boost::asio::ssl::stream<detail::websocket_stream<boost::asio::ip::tcp::socket&> &> >(boost::ref(*websocket),boost::ref(*(tls_context->GetServerCredentials())));

		if (tls_mutual_auth)
		{
			::SSL_set_verify(tls_websocket->native_handle(), SSL_VERIFY_PEER ,::SSL_CTX_get_verify_callback(tls_context->GetServerCredentials()->native_handle()));
		}
		RobotRaconteurNode::asio_async_handshake(node, tls_websocket, boost::asio::ssl::stream<boost::asio::ip::tcp::socket&>::server, boost::bind(&TcpTransportConnection::do_starttls9, RR_STATIC_POINTER_CAST<TcpTransportConnection>(shared_from_this()),_1));

	}
	else
	{
		tls_context=RR_STATIC_POINTER_CAST<detail::OpenSSLAuthContext>(p->GetTlsContext());
		tls_wss_websocket=RR_MAKE_SHARED<boost::asio::ssl::stream<detail::websocket_stream<boost::asio::ssl::stream<boost::asio::ip::tcp::socket&> &> &> >(boost::ref(*wss_websocket),boost::ref(*(tls_context->GetServerCredentials())));

		if (tls_mutual_auth)
		{
			::SSL_set_verify(tls_wss_websocket->native_handle(), SSL_VERIFY_PEER ,::SSL_CTX_get_verify_callback(tls_context->GetServerCredentials()->native_handle()));
		}
		RobotRaconteurNode::asio_async_handshake(node, tls_wss_websocket, boost::asio::ssl::stream<detail::websocket_stream<boost::asio::ssl::stream<boost::asio::ip::tcp::socket&> &> &>::server, boost::bind(&TcpTransportConnection::do_starttls9, RR_STATIC_POINTER_CAST<TcpTransportConnection>(shared_from_this()),_1));

	}

#endif




}

void TcpTransportConnection::do_starttls9(const boost::system::error_code& error)
{
	if(error)
	{
		Close();
		return;
	}

	{
		boost::mutex::scoped_lock lock1(socket_lock);
		is_tls=true;
	}

	try
	{
	AsyncResumeSend();
	AsyncResumeReceive();
	}
	catch (std::exception&)
	{
		Close();
		return;
	}


}

void TcpTransportConnection::MessageReceived(RR_INTRUSIVE_PTR<Message> m)
{
	NodeID RemoteNodeID1;
	uint32_t local_ep;
	uint32_t remote_ep;
	{
		boost::shared_lock<boost::shared_mutex> lock(RemoteNodeID_lock);
		RemoteNodeID1=RemoteNodeID;
		local_ep=m_LocalEndpoint;
		remote_ep=m_RemoteEndpoint;
	}


	if (require_tls && !is_tls)
	{
		//If tls is required, tls is not active, and the message received is not "STARTTLS", abort the connection
		bool bad_message=true;
		if (m->entries.size()==1)
		{
			if (m->entries[0]->EntryType==MessageEntryType_StreamOp && m->entries[0]->MemberName=="STARTTLS")
			{
				bad_message=false;
			}
		}

		if (bad_message)
		{
			//This may not be the nicest way to handle the situation but it is the safest
			Close();
		}


	}

	//TODO: Finish the endpoint checking procedure

	if (is_tls)
	{
		if (!RemoteNodeID1.IsAnyNode())
		{
			if (RemoteNodeID1 != m->header->SenderNodeID)
			{
				RR_INTRUSIVE_PTR<Message> ret1=GetNode()->GenerateErrorReturnMessage(m,MessageErrorType_NodeNotFound,"RobotRaconteur.NodeNotFound","Invalid sender node");
				if (ret1->entries.size() >0)
				{
					boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)> h = boost::bind(&TcpTransportConnection::SimpleAsyncEndSendMessage, RR_STATIC_POINTER_CAST<TcpTransportConnection>(shared_from_this()), _1);
					AsyncSendMessage(ret1, h);
					return;
				}

			}

		}

		if (local_ep!=0 && remote_ep!=0)
		{
			if (local_ep!=m->header->ReceiverEndpoint || remote_ep!=m->header->SenderEndpoint)
			{
				RR_INTRUSIVE_PTR<Message> ret1=GetNode()->GenerateErrorReturnMessage(m,MessageErrorType_InvalidEndpoint,"RobotRaconteur.InvalidEndpoint","Invalid sender endpoint");
				if (ret1->entries.size() >0)
				{
					boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)> h =  boost::bind(&TcpTransportConnection::SimpleAsyncEndSendMessage, RR_STATIC_POINTER_CAST<TcpTransportConnection>(shared_from_this()), _1);
					AsyncSendMessage(ret1, h);
					return;
				}
			}

		}
	}

	RR_SHARED_PTR<TcpTransport> p=parent.lock();
	if (!p) return;

	RR_INTRUSIVE_PTR<Message> ret = p->SpecialRequest(m, shared_from_this());
	if (ret != 0)
	{
		try
		{
			if ((m->entries.at(0)->EntryType == MessageEntryType_ConnectionTest || m->entries.at(0)->EntryType == MessageEntryType_ConnectionTestRet))
			{
				if (m->entries.at(0)->Error != MessageErrorType_None)
				{
					Close();
					return;
				}
			}

			if ((ret->entries.at(0)->EntryType == MessageEntryType_ConnectClientRet || ret->entries.at(0)->EntryType == MessageEntryType_ReconnectClient || ret->entries.at(0)->EntryType == MessageEntryType_ConnectClientCombinedRet) && ret->entries.at(0)->Error == MessageErrorType_None)
			{
				if (ret->entries.at(0)->Error == MessageErrorType_None)
				{
					if (ret->header->SenderNodeID == GetNode()->NodeID())
					{
						{
							boost::unique_lock<boost::shared_mutex> lock(RemoteNodeID_lock);
							if (m_LocalEndpoint != 0)
							{
								throw InvalidOperationException("Already connected");
							}

							m_RemoteEndpoint = ret->header->ReceiverEndpoint;
							m_LocalEndpoint = ret->header->SenderEndpoint;
						}


						p->register_transport(RR_STATIC_POINTER_CAST<TcpTransportConnection>(shared_from_this()));
					}
				}
					
			}
			
			boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)> h = boost::bind(&TcpTransportConnection::SimpleAsyncEndSendMessage, RR_STATIC_POINTER_CAST<TcpTransportConnection>(shared_from_this()), _1);
			AsyncSendMessage(ret, h);
		}
		catch (std::exception&)
		{
			Close();
		}

		return;
	}
	
	
	try
	{
		
		if (m->entries.size()==1)
		{
			if ((m->entries[0]->EntryType == MessageEntryType_ConnectClientRet || m->entries[0]->EntryType == MessageEntryType_ConnectClientCombinedRet) && remote_ep==0)
			{
				boost::unique_lock<boost::shared_mutex> lock(RemoteNodeID_lock);
				if (m_RemoteEndpoint == 0)
				{
					m_RemoteEndpoint = m->header->SenderEndpoint;
				}
				remote_ep = m_RemoteEndpoint;
			}
		}

		//We shouldn't get here without having a service connection
		if (is_tls)
		{
			if (local_ep == 0 || remote_ep == 0)
			{
				if (m->entries.size() != 1)
				{
					//Someone is doing something nasty
					Close();
					return;
				}

				uint16_t command = m->entries[0]->EntryType;

				if (command > 500) //Check if special command
				{
					//Someone is doing something nasty
					Close();
					return;
				}
			}

		}


		boost::asio::ip::address addr=socket->local_endpoint().address();
		uint16_t port=socket->local_endpoint().port();

		std::string scheme="rr+tcp";


		if (is_tls)
		{
			scheme="rrs+tcp";
		}

		if (use_websocket)
		{
			scheme = "rr+ws";
		}

		if (use_websocket && is_tls)
		{
			scheme = "rrs+ws";
		}

		if (use_wss_websocket)
		{
			scheme = "rr+wss";
		}

		if (use_wss_websocket & is_tls)
		{
			scheme = "rrs+wss";
		}

		std::string connecturl;
		if (addr.is_v4())
		{
			connecturl=scheme + "://" + addr.to_string() + ":" + boost::lexical_cast<std::string>(port) + "/";
		}
		else
		{
			boost::asio::ip::address_v6 addr2=addr.to_v6();
			addr2.scope_id(0);
			if (addr2.is_v4_mapped())
			{
				connecturl = scheme + "://" + addr2.to_v4().to_string() + ":" + boost::lexical_cast<std::string>(port)+"/";
			}
			else
			{
				connecturl = scheme + "://[" + addr2.to_string() + "]:" + boost::lexical_cast<std::string>(port)+"/";
			}
		}

		Transport::m_CurrentThreadTransportConnectionURL.reset(new std::string(connecturl));
		Transport::m_CurrentThreadTransport.reset(  new RR_SHARED_PTR<ITransportConnection>(RR_STATIC_POINTER_CAST<TcpTransportConnection>(shared_from_this())));
		p->MessageReceived(m);
	}
	catch (std::exception& exp)
	{
		RobotRaconteurNode::TryHandleException(node, &exp);
		Close();
	}

		
	Transport::m_CurrentThreadTransportConnectionURL.reset(0);
	Transport::m_CurrentThreadTransport.reset(0);


	
}


void TcpTransportConnection::StreamOpMessageReceived(RR_INTRUSIVE_PTR<Message> m)
{
	if (m->entries.size()==1)
	{
		RR_INTRUSIVE_PTR<MessageEntry> mm=m->entries.at(0);
		if (mm->MemberName == "STARTTLS")
		{
			if (mm->EntryType==MessageEntryType_StreamOp)
			{
				if ((m->header->ReceiverNodeID == NodeID::GetAny() && m->header->ReceiverNodeName=="")
					|| (m->header->ReceiverNodeID == GetNode()->NodeID() && m->header->ReceiverNodeName=="")
					|| (m->header->ReceiverNodeID == NodeID::GetAny() && m->header->ReceiverNodeName==GetNode()->NodeName())
					|| (m->header->ReceiverNodeID == GetNode()->NodeID() && m->header->ReceiverNodeName==GetNode()->NodeName()))
				{
					{
						boost::unique_lock<boost::shared_mutex> lock(RemoteNodeID_lock);
						RemoteNodeID=m->header->SenderNodeID;
					}

					RR_INTRUSIVE_PTR<MessageElement> e;
					if(mm->TryFindElement("mutualauth", e))
					{
						std::string dat = e->CastDataToString();
						if (dat == "true")
						{
							tls_mutual_auth = true;
						}
					}					

					//We have a match!
					boost::function<void(const boost::system::error_code&)> h = boost::bind(&TcpTransportConnection::do_starttls6, RR_STATIC_POINTER_CAST<TcpTransportConnection>(shared_from_this()), _1, m);
					AsyncPauseReceive(h);
					return;
				}
				else
				{
					//Oops, requesting a bad nodeid/nodename query

					
					RR_INTRUSIVE_PTR<Message> mret = CreateMessage();
					mret->header = CreateMessageHeader();
					mret->header->SenderNodeName = GetNode()->NodeName();
					mret->header->ReceiverNodeName = m->header->SenderNodeName;
					mret->header->SenderNodeID = GetNode()->NodeID();
					mret->header->ReceiverNodeID = m->header->SenderNodeID;

					RR_INTRUSIVE_PTR<MessageEntry> mmret = CreateMessageEntry(MessageEntryType_StreamOpRet,  mm->MemberName);
					mmret->Error = MessageErrorType_NodeNotFound;
					mmret->AddElement("errorname", stringToRRArray("RobotRaconteur.NodeNotFound"));
					mmret->AddElement("errorstring", stringToRRArray("Node not found"));
					mret->entries.push_back(mmret);

					boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)> h = boost::bind(&TcpTransportConnection::StreamOp_EndSendMessage, RR_STATIC_POINTER_CAST<TcpTransportConnection>(shared_from_this()), _1);
					AsyncSendMessage(mret,h);

					return;
				}

				
			}

			if (mm->EntryType==MessageEntryType_StreamOpRet)
			{
				streamop_waiting=false;
				if (mm->Error!=0)
				{

					RR_SHARED_PTR<RobotRaconteurException> ee=RobotRaconteurExceptionUtil::MessageEntryToException(mm);

					boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)> starttls_handler1=starttls_handler;
					starttls_handler.clear();
					detail::InvokeHandlerWithException(node, starttls_handler1, ee);
					return;
				}

				std::string servername=m->header->SenderNodeID.ToString();
				if (target_nodeid==NodeID::GetAny() && target_nodename=="")
				{
					boost::unique_lock<boost::shared_mutex> lock(RemoteNodeID_lock);
					RemoteNodeID=m->header->SenderNodeID;
				}
				else if (target_nodeid==m->header->SenderNodeID)
				{
					boost::unique_lock<boost::shared_mutex> lock(RemoteNodeID_lock);
					RemoteNodeID=target_nodeid;
				}
				else if (target_nodename==m->header->SenderNodeName && target_nodeid==NodeID::GetAny())
				{
					boost::unique_lock<boost::shared_mutex> lock(RemoteNodeID_lock);
					RemoteNodeID=m->header->SenderNodeID;
				}
				else
				{

					boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)> starttls_handler1=starttls_handler;
					starttls_handler.clear();
					detail::InvokeHandlerWithException(node, starttls_handler1, RR_MAKE_SHARED<ConnectionException>("Could not create TLS connection"));
					return;
				}

				RR_INTRUSIVE_PTR<MessageElement> e;
				if (mm->TryFindElement("mutualauth", e))
				{
					std::string dat = e->CastDataToString();
					if (dat == "true")
					{
						tls_mutual_auth = true;
					}
				}				

				boost::function<void(const boost::system::error_code&)> h = boost::bind(&TcpTransportConnection::do_starttls4, RR_STATIC_POINTER_CAST<TcpTransportConnection>(shared_from_this()), servername, _1);
				AsyncPauseReceive(h);
				return;
			}
		}
	}

	return ASIOStreamBaseTransport::StreamOpMessageReceived(m);
}

void TcpTransportConnection::async_write_some(const_buffers& b, boost::function<void (const boost::system::error_code& error, size_t bytes_transferred)>& handler)
{
	boost::mutex::scoped_lock lock(socket_lock);

	if (is_tls)
	{		
		if (!use_websocket && !use_wss_websocket)
		{
			RobotRaconteurNode::asio_async_write_some(node, tls_socket, b, handler);
			return;
		}
		else if (use_websocket)
		{
			RobotRaconteurNode::asio_async_write_some(node, tls_websocket, b, handler);
			return;
		}
		else
		{
			RobotRaconteurNode::asio_async_write_some(node, tls_wss_websocket, b, handler);			
			return;
		}
	}

	if (use_websocket)
	{
		RobotRaconteurNode::asio_async_write_some(node, websocket, b, handler);
		return;
	}

	if (use_wss_websocket)
	{		
		RobotRaconteurNode::asio_async_write_some(node, wss_websocket, b, handler);
		return;
	}

	RobotRaconteurNode::asio_async_write_some(node, socket, b, handler);
}


void TcpTransportConnection::async_read_some(mutable_buffers& b, boost::function<void (const boost::system::error_code& error, size_t bytes_transferred)>& handler)
{
	boost::mutex::scoped_lock lock(socket_lock);
	
	if (is_tls)
	{
		if (!use_websocket && !use_wss_websocket)
		{
			RobotRaconteurNode::asio_async_read_some(node, tls_socket, b, handler);			
			return;
		}
		else if (use_websocket)
		{
			RobotRaconteurNode::asio_async_read_some(node, tls_websocket, b, handler);			
			return;
		}
		else 
		{
			RobotRaconteurNode::asio_async_read_some(node, tls_wss_websocket, b, handler);			
			return;
		}

	}
	
	if (use_websocket)
	{
		RobotRaconteurNode::asio_async_read_some(node, websocket, b, handler);		
		return;
	}

	if (use_wss_websocket)
	{		RobotRaconteurNode::asio_async_read_some(node, wss_websocket, b, handler);
		
		return;
	}

	RobotRaconteurNode::asio_async_read_some(node, socket, b, handler);
}

size_t TcpTransportConnection::available()
{
	boost::mutex::scoped_lock lock(socket_lock);

	return socket->available();
}


void TcpTransportConnection::Close()
{
	boost::recursive_mutex::scoped_lock lock(close_lock);
	if(closing) return;
	closing=true;
	{
		boost::mutex::scoped_lock lock(socket_lock);

		if (false /*is_tls*/)
		{
			try
			{
				if (!use_websocket && !use_wss_websocket)
				{
					RobotRaconteurNode::asio_async_shutdown(node, tls_socket, boost::bind(&TcpTransportConnection::Close1, RR_STATIC_POINTER_CAST<TcpTransportConnection>(shared_from_this()), _1));
				}
				else if (use_websocket)
				{
					RobotRaconteurNode::asio_async_shutdown(node, tls_websocket, boost::bind(&TcpTransportConnection::Close1, RR_STATIC_POINTER_CAST<TcpTransportConnection>(shared_from_this()), _1));
				}
				else 
				{
					RobotRaconteurNode::asio_async_shutdown(node, tls_wss_websocket, boost::bind(&TcpTransportConnection::Close1, RR_STATIC_POINTER_CAST<TcpTransportConnection>(shared_from_this()), _1));
				}
			}
			catch (std::exception&)
			{
				RobotRaconteurNode::TryPostToThreadPool(node, boost::bind(&TcpTransportConnection::ForceClose,RR_STATIC_POINTER_CAST<TcpTransportConnection>(shared_from_this())));
			}

		}
		else
		{
		

		try
		{
		if (socket->is_open())
		{
		socket->shutdown(boost::asio::ip::tcp::socket::shutdown_both);
		}
	
		}
		catch (std::exception&) {}


		try
		{
			if (is_tls && !use_websocket && !use_wss_websocket)
			{
				socket->close();
			}
			else if (use_websocket)
			{
				websocket->close();
			}
			else if (use_wss_websocket)
			{
				wss_websocket->close();
			}
			else
			{
				socket->close();
			}
		}
		catch (std::exception&) {}
		}
	}

	{
		boost::mutex::scoped_lock lock2(streamop_lock);
		starttls_timer.reset();
		if (starttls_handler)
		{
			boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)> starttls_handler1=starttls_handler;
			starttls_handler.clear();
			detail::PostHandlerWithException(node, starttls_handler1,RR_MAKE_SHARED<ConnectionException>("Transport connection closed"),true);
		}


	}


	try
	{
		RR_SHARED_PTR<TcpTransport> p=parent.lock();
		if (p) p->erase_transport(RR_STATIC_POINTER_CAST<TcpTransportConnection>(shared_from_this()));
	}
	catch (std::exception&) {}

	

	ASIOStreamBaseTransport::Close();

}

void TcpTransportConnection::Close1(const boost::system::error_code& ec)
{
	ForceClose();
}

bool TcpTransportConnection::IsClosed()
{
	boost::mutex::scoped_lock lock(socket_lock);
	return socket->is_open();
}

void TcpTransportConnection::ForceClose()
{
	boost::mutex::scoped_lock lock(socket_lock);


	try
		{
			
			if (is_tls)
			{				
				//tls_socket.shutdown();
				//tls_socket.reset();
				is_tls=false;
			}
		}
		catch (std::exception&) {}

	try
	{
	if (socket->is_open())
	{
	socket->shutdown(boost::asio::ip::tcp::socket::shutdown_both);
	}
	
	}
	catch (std::exception&) {}

	try
	{
		socket->close();
	}
	catch (std::exception&) {}

}

void TcpTransport::CheckConnection(uint32_t endpoint)
{	
	RR_SHARED_PTR<ITransportConnection> t;
	{
		boost::mutex::scoped_lock lock(TransportConnections_lock);
		RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<ITransportConnection> >::iterator e1 = TransportConnections.find(endpoint);
		if (e1 == TransportConnections.end()) throw ConnectionException("Transport connection to remote host not found");
		t = e1->second;
	}
	t->CheckConnection(endpoint);
}

uint32_t TcpTransportConnection::GetLocalEndpoint() 
{
	boost::shared_lock<boost::shared_mutex> lock(RemoteNodeID_lock);
	return m_LocalEndpoint;
}

uint32_t TcpTransportConnection::GetRemoteEndpoint()
{
	boost::shared_lock<boost::shared_mutex> lock(RemoteNodeID_lock);
	return m_RemoteEndpoint;
}

void TcpTransportConnection::CheckConnection(uint32_t endpoint)
{
	if (endpoint!=m_LocalEndpoint || !connected.load()) throw ConnectionException("Connection lost");
}

bool TcpTransportConnection::IsSecure()
{
	return is_tls;
}


//TODO finish this
bool TcpTransportConnection::IsSecurePeerIdentityVerified()
{
	boost::mutex::scoped_lock lock(socket_lock);
	if (!is_tls) return false;
	if (!tls_socket && !tls_websocket) return false;
#ifdef ROBOTRACONTEUR_USE_SCHANNEL
	if (!use_websocket && !use_wss_websocket)
	{
		RR_SHARED_PTR<detail::TlsSchannelAsyncStreamAdapter> s = tls_socket;
		lock.unlock();
		boost::shared_lock<boost::shared_mutex> lock2(RemoteNodeID_lock);
		return s->VerifyRemoteNodeCertificate(RemoteNodeID);
	}
	else if (use_websocket)
	{
		RR_SHARED_PTR<detail::TlsSchannelAsyncStreamAdapter> s = tls_websocket;
		lock.unlock();
		boost::shared_lock<boost::shared_mutex> lock2(RemoteNodeID_lock);
		return s->VerifyRemoteNodeCertificate(RemoteNodeID);
	}
	else
	{
		RR_SHARED_PTR<detail::TlsSchannelAsyncStreamAdapter> s = tls_wss_websocket;
		lock.unlock();
		boost::shared_lock<boost::shared_mutex> lock2(RemoteNodeID_lock);
		return s->VerifyRemoteNodeCertificate(RemoteNodeID);
	}

#endif

#ifdef ROBOTRACONTEUR_USE_OPENSSL
	if (!use_websocket && !use_wss_websocket)
	{
		long res=SSL_get_verify_result(tls_socket->native_handle());
		if ( res!= X509_V_OK && res != X509_V_ERR_UNHANDLED_CRITICAL_EXTENSION) return false;
		X509* cert = SSL_get_peer_certificate(tls_socket->native_handle());
		if (!cert) return false;

		char buf[256];
		memset(buf,0,256);

		X509_NAME_oneline(X509_get_subject_name(cert), buf, 256);

		lock.unlock();
		std::string buf2(buf);

		boost::shared_lock<boost::shared_mutex> lock2(RemoteNodeID_lock);
		if (buf2!="/CN=Robot Raconteur Node " + RemoteNodeID.ToString())
		{
			return false;
		}

		//lock2.unlock();

		RR_SHARED_PTR<TcpTransport> p=parent.lock();
		if (!p) throw InvalidOperationException("Transport shutdown");

		RR_SHARED_PTR<detail::OpenSSLAuthContext> c=RR_STATIC_POINTER_CAST<detail::OpenSSLAuthContext>(p->GetTlsContext());

		return c->VerifyRemoteNodeCertificate(tls_socket->native_handle(),RemoteNodeID);
	}
	else if (use_websocket)
	{
		long res=SSL_get_verify_result(tls_websocket->native_handle());
		if ( res!= X509_V_OK && res != X509_V_ERR_UNHANDLED_CRITICAL_EXTENSION) return false;
		X509* cert = SSL_get_peer_certificate(tls_websocket->native_handle());
		if (!cert) return false;

		char buf[256];
		memset(buf,0,256);

		X509_NAME_oneline(X509_get_subject_name(cert), buf, 256);

		lock.unlock();
		std::string buf2(buf);

		boost::shared_lock<boost::shared_mutex> lock2(RemoteNodeID_lock);
		if (buf2!="/CN=Robot Raconteur Node " + RemoteNodeID.ToString())
		{
			return false;
		}

		//lock2.unlock();

		RR_SHARED_PTR<TcpTransport> p=parent.lock();
		if (!p) throw InvalidOperationException("Transport shutdown");

		RR_SHARED_PTR<detail::OpenSSLAuthContext> c=RR_STATIC_POINTER_CAST<detail::OpenSSLAuthContext>(p->GetTlsContext());

		return c->VerifyRemoteNodeCertificate(tls_websocket->native_handle(),RemoteNodeID);

	}
	else
	{
		long res=SSL_get_verify_result(tls_wss_websocket->native_handle());
		if ( res!= X509_V_OK && res != X509_V_ERR_UNHANDLED_CRITICAL_EXTENSION) return false;
		X509* cert = SSL_get_peer_certificate(tls_wss_websocket->native_handle());
		if (!cert) return false;

		char buf[256];
		memset(buf,0,256);

		X509_NAME_oneline(X509_get_subject_name(cert), buf, 256);

		lock.unlock();
		std::string buf2(buf);

		boost::shared_lock<boost::shared_mutex> lock2(RemoteNodeID_lock);
		if (buf2!="/CN=Robot Raconteur Node " + RemoteNodeID.ToString())
		{
			return false;
		}

		//lock2.unlock();

		RR_SHARED_PTR<TcpTransport> p=parent.lock();
		if (!p) throw InvalidOperationException("Transport shutdown");

		RR_SHARED_PTR<detail::OpenSSLAuthContext> c=RR_STATIC_POINTER_CAST<detail::OpenSSLAuthContext>(p->GetTlsContext());

		return c->VerifyRemoteNodeCertificate(tls_wss_websocket->native_handle(),RemoteNodeID);
	}

#endif 


}

std::string TcpTransportConnection::GetSecurePeerIdentity()
{
	boost::shared_lock<boost::shared_mutex> lock2(RemoteNodeID_lock);
	if (!IsSecurePeerIdentityVerified())
		throw AuthenticationException("Peer identity is not verified");
	return RemoteNodeID.ToString();

}

bool TcpTransportConnection::IsLargeTransferAuthorized()
{
	boost::shared_lock<boost::shared_mutex> lock2(RemoteNodeID_lock);
	if (m_RemoteEndpoint == 0 || m_LocalEndpoint == 0) return false;
	return GetNode()->IsEndpointLargeTransferAuthorized(m_LocalEndpoint);
}

namespace detail
{
	IPNodeDiscovery::IPNodeDiscovery(RR_SHARED_PTR<TcpTransport> parent)
	{
		listening=false;
		broadcasting=false;
		broadcast_flags=0;
		listen_flags = 0;
		listen_socket_flags = 0;

		this->parent=parent;
		this->node=parent->GetNode();
		this->broadcast_timer_period = 55000;
	}

	IPNodeDiscovery::IPNodeDiscovery(RR_SHARED_PTR<RobotRaconteurNode> node)
	{
		listening=false;
		broadcasting=false;
		broadcast_flags=0;
		listen_flags = 0;
		listen_socket_flags = 0;
		this->node=node;
		this->broadcast_timer_period = 10000;
	}

	RR_SHARED_PTR<RobotRaconteurNode> IPNodeDiscovery::GetNode()
	{
		RR_SHARED_PTR<RobotRaconteurNode> t=node.lock();
		if (!t) throw InvalidOperationException("Transport has been released");
		return t;
	}

	void IPNodeDiscovery::start_listen_sockets()
	{
		int32_t flags = broadcast_flags | listen_flags;

		if (flags == listen_socket_flags)
			return;

		ip4_listen.reset();
		ip6_listen.clear();

		boost::this_thread::sleep(boost::posix_time::milliseconds(5));

		std::vector<boost::asio::ip::address> local_addresses;

		TcpTransport::GetLocalAdapterIPAddresses(local_addresses);

		bool has_ip6 = false;

		BOOST_FOREACH(boost::asio::ip::address& e, local_addresses)
		{
			if (e.is_v6()) has_ip6 = true;
		}

		//has_ip6=true;


		ip4_listen = RR_MAKE_SHARED<boost::asio::ip::udp::socket>(boost::ref(GetNode()->GetThreadPool()->get_io_service()));



		ip4_listen->open(boost::asio::ip::udp::v4());

		//ip4_listen->set_option(boost::asio::ip::udp::socket::linger(false,10));

		ip4_listen->set_option(boost::asio::ip::udp::socket::reuse_address(true));
#ifdef ROBOTRACONTEUR_WINDOWS
		u_long exclusiveaddruse = 0;
		::setsockopt(ip4_listen->native_handle(), SOL_SOCKET, SO_EXCLUSIVEADDRUSE, (const char*)&exclusiveaddruse, sizeof(exclusiveaddruse));
#endif

		ip4_listen->set_option(boost::asio::ip::udp::socket::broadcast(true));
		ip4_listen->bind(boost::asio::ip::udp::endpoint(boost::asio::ip::address_v4::any(), ANNOUNCE_PORT));

		ip4_listen->set_option(boost::asio::ip::multicast::enable_loopback(true));

		boost::shared_array<uint8_t> data(new uint8_t[8192]);

		RR_SHARED_PTR<boost::asio::ip::udp::endpoint> ep4 = RR_MAKE_SHARED<boost::asio::ip::udp::endpoint>(boost::asio::ip::address_v4::any(), ANNOUNCE_PORT);

		ip4_listen->async_receive_from(boost::asio::buffer(data.get(), 8192), *ep4, boost::bind(&IPNodeDiscovery::handle_receive, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred, ip4_listen, ep4, data));



		if (has_ip6)
		{


			receive_update_timer = RR_MAKE_SHARED<boost::asio::deadline_timer>(boost::ref(GetNode()->GetThreadPool()->get_io_service()));

			try
			{
				start_ipv6_listen_socket(boost::asio::ip::udp::endpoint(boost::asio::ip::address_v6::any(), ANNOUNCE_PORT));
			}
			catch (std::exception&) {}

			RobotRaconteurNode::TryPostToThreadPool(node, boost::bind(&IPNodeDiscovery::handle_receive_update_timer, shared_from_this(), boost::system::error_code()), true);

		}


	}

	void IPNodeDiscovery::start_ipv6_listen_socket(boost::asio::ip::udp::endpoint ep)
	{
		int32_t flags = broadcast_flags | listen_flags;

		if (boost::range::count(ip6_listen_scope_ids, ep.address().to_v6().scope_id())!=0)
		{
			return;
		}
		
		RR_SHARED_PTR<boost::asio::ip::udp::socket> ip6_listen1;

		ip6_listen1=RR_MAKE_SHARED<boost::asio::ip::udp::socket>(boost::ref(GetNode()->GetThreadPool()->get_io_service()));

			

		ip6_listen1->open(boost::asio::ip::udp::v6());
		ip6_listen1->set_option(boost::asio::ip::udp::socket::reuse_address(true));
#ifdef ROBOTRACONTEUR_WINDOWS
		u_long exclusiveaddruse = 0;
		::setsockopt(ip6_listen1->native_handle(), SOL_SOCKET, SO_EXCLUSIVEADDRUSE, (const char*)&exclusiveaddruse, sizeof(exclusiveaddruse));
#endif
		//ip6_listen->set_option(boost::asio::ip::udp::socket::broadcast(true));


		//ip6_listen->set_option(boost::asio::ip::udp::socket::linger(false,10));
		ip6_listen1->bind(ep);

		ip6_listen1->set_option(boost::asio::ip::multicast::enable_loopback(true));

		if ((static_cast<uint32_t>(flags) & (static_cast<uint32_t>(IPNodeDiscoveryFlags_NODE_LOCAL))) != 0)
		{
			boost::system::error_code ec;
			Ip6AddMembership(*ip6_listen1,boost::asio::ip::address_v6::from_string("FF01::BA86"),ep.address().to_v6().scope_id());
			
		}

		if ((static_cast<uint32_t>(flags) & (static_cast<uint32_t>(IPNodeDiscoveryFlags_LINK_LOCAL))) != 0)
		{
			boost::system::error_code ec;
			Ip6AddMembership(*ip6_listen1,boost::asio::ip::address_v6::from_string("FF02::BA86"),ep.address().to_v6().scope_id());
		}

		if ((static_cast<uint32_t>(flags) & (static_cast<uint32_t>(IPNodeDiscoveryFlags_SITE_LOCAL))) != 0)
		{
			boost::system::error_code ec;
			Ip6AddMembership(*ip6_listen1,boost::asio::ip::address_v6::from_string("FF05::BA86"),ep.address().to_v6().scope_id());
		}
						

		boost::shared_array<uint8_t> data2=boost::shared_array<uint8_t>(new uint8_t[8192]);
		RR_SHARED_PTR<boost::asio::ip::udp::endpoint> ep6=RR_MAKE_SHARED<boost::asio::ip::udp::endpoint>(boost::asio::ip::address_v6::any(),ANNOUNCE_PORT);
		ip6_listen1->async_receive_from(boost::asio::buffer(data2.get(),8192),*ep6,boost::bind(&IPNodeDiscovery::handle_receive,shared_from_this(),boost::asio::placeholders::error,boost::asio::placeholders::bytes_transferred,ip6_listen1,ep6,data2));

		
		ip6_listen.push_back(ip6_listen1);
		ip6_listen_scope_ids.push_back(ep.address().to_v6().scope_id());
	}

	void IPNodeDiscovery::StartListeningForNodes(uint32_t flags)
	{
		{
			boost::mutex::scoped_lock lock(change_lock);

			this_request_id = NodeID::NewUniqueID();

			if (flags == 0) flags = IPNodeDiscoveryFlags_NODE_LOCAL | IPNodeDiscoveryFlags_SITE_LOCAL | IPNodeDiscoveryFlags_LINK_LOCAL | IPNodeDiscoveryFlags_IPV4_BROADCAST;

			if (listening)
				throw InvalidOperationException("Already listening for nodes");

			//if (!flags & IPNodeDiscoveryFlags_IPV4_BROADCAST)
			//	flags |= IPNodeDiscoveryFlags_IPV4_BROADCAST;
			listening = true;
			listen_flags = flags;

			start_listen_sockets();

		}

		SendDiscoveryRequestNow();
	}

	void IPNodeDiscovery::handle_receive(const boost::system::error_code& error, std::size_t bytes_transferred, RR_SHARED_PTR<boost::asio::ip::udp::socket> socket, RR_SHARED_PTR<boost::asio::ip::udp::endpoint> ep, boost::shared_array<uint8_t> buffer)
	{
		{
		boost::mutex::scoped_lock lock(change_lock);
		if (!listening) return;
		}

		//if (error) return;

		if (!error)
		{
			std::string s((char*)buffer.get(),bytes_transferred);
			try
			{

			NodeAnnounceReceived(s);
			}
			catch (std::exception&) {}
		}

		boost::mutex::scoped_lock lock(change_lock);
				
		socket->async_receive_from(boost::asio::buffer(buffer.get(),8192),*ep,boost::bind(&IPNodeDiscovery::handle_receive,shared_from_this(),boost::asio::placeholders::error,boost::asio::placeholders::bytes_transferred,socket,ep,buffer));
	}

	void IPNodeDiscovery::handle_receive_update_timer(const boost::system::error_code& error)
	{
		
		if (error==boost::asio::error::operation_aborted)
		{
			return;
		}

		boost::mutex::scoped_lock lock1(change_lock);

		if (!listening) return;

		std::vector<boost::asio::ip::address> local_addresses;
			
		TcpTransport::GetLocalAdapterIPAddresses(local_addresses);

		BOOST_FOREACH (boost::asio::ip::address& e, local_addresses)
		{
			if (e.is_v6())
			{
				try
				{
					size_t count;
					
						
					count=std::count(ip6_listen_scope_ids.begin(),ip6_listen_scope_ids.end(),e.to_v6().scope_id());
					
					if (count==0)
					{
						boost::asio::ip::address_v6 addr6=boost::asio::ip::address_v6::any();
						addr6.scope_id(e.to_v6().scope_id());
						start_ipv6_listen_socket(boost::asio::ip::udp::endpoint(e,ANNOUNCE_PORT));
					}
				}
				catch (std::exception&) {}
			}
		}
		
		
		receive_update_timer->expires_from_now(boost::posix_time::seconds(5));
		RobotRaconteurNode::asio_async_wait(node, receive_update_timer, boost::bind(&IPNodeDiscovery::handle_receive_update_timer,shared_from_this(),boost::asio::placeholders::error));

	}

	void IPNodeDiscovery::NodeAnnounceReceived(const std::string &packet)
	{
		if (listening)
		{
			try
			{
				std::string seed = "Robot Raconteur Node Discovery Packet";
				if (packet.substr(0, seed.length()) == seed)
				{
					std::vector<std::string> s1;
					boost::split(s1, packet, boost::is_from_range('\n', '\n'));
					if (s1.size() < 3) return;
					std::vector<std::string> s2;
					boost::split(s2, s1.at(1), boost::is_from_range(',', ','));
					NodeID id(s2.at(0));
					if (id != GetNode()->NodeID())
						GetNode()->NodeAnnouncePacketReceived(packet);

				}
			}
			catch (std::exception&) {}
		}

		if (broadcasting)
		{
			try
			{
				std::string seed = "Robot Raconteur Discovery Request Packet";
				if (packet.substr(0, seed.length()) == seed)
				{
					std::vector<std::string> s1;
					boost::split(s1, packet, boost::is_from_range('\n', '\n'));
					if (s1.size() < 3) return;
					std::vector<std::string> s2;
					boost::split(s2, s1.at(1), boost::is_from_range(',', ','));
					NodeID id(s2.at(0));
					if (id != this_request_id)
						SendAnnounceNow();

				}
			}
			catch (std::exception&) {}
		}
	}

	void IPNodeDiscovery::StopListeningForNodes()
	{
		
		boost::mutex::scoped_lock lock(change_lock);
		if (!listening) return;
		listening = false;
		
		if (ip4_listen) 
		{
			ip4_listen->shutdown(boost::asio::ip::udp::socket::shutdown_both);
			ip4_listen->close();
		}

		boost::mutex ip6_listen_lock;
		BOOST_FOREACH (RR_SHARED_PTR<boost::asio::ip::udp::socket>& ip6_listen1, ip6_listen)
		{
			if (ip6_listen1) 
			{
				(ip6_listen1)->shutdown(boost::asio::ip::udp::socket::shutdown_both);
				(ip6_listen1)->close();
			}
		}		
		receive_update_timer->cancel();

	}

	void IPNodeDiscovery::StartAnnouncingNode(uint32_t flags)
	{
		boost::mutex::scoped_lock lock(change_lock);
		if (broadcasting) return;
		broadcasting=true;
		if (flags==0) flags=IPNodeDiscoveryFlags_NODE_LOCAL | IPNodeDiscoveryFlags_SITE_LOCAL | IPNodeDiscoveryFlags_LINK_LOCAL;

		broadcast_flags=flags;

		broadcast_timer=RR_MAKE_SHARED<boost::asio::deadline_timer>(boost::ref(GetNode()->GetThreadPool()->get_io_service()));

		broadcast_timer->expires_from_now(boost::posix_time::milliseconds(500));
		RobotRaconteurNode::asio_async_wait(node, broadcast_timer, boost::bind(&IPNodeDiscovery::handle_broadcast_timer, shared_from_this(), boost::asio::placeholders::error));

		//RobotRaconteurNode::TryPostToThreadPool(node, boost::bind(&IPNodeDiscovery::handle_broadcast_timer,shared_from_this(),boost::system::error_code()));

		start_listen_sockets();

	}

	void IPNodeDiscovery::StopAnnouncingNode()
	{
		boost::mutex::scoped_lock lock(change_lock);

		if (!broadcasting) return;

		broadcasting=false;

		broadcast_timer->cancel();

	}

	void IPNodeDiscovery::handle_broadcast_timer(const boost::system::error_code& error)
	{
		if (error == boost::asio::error::operation_aborted)
		{
			return;
		}

		RR_SHARED_PTR<TcpTransport> p = parent.lock();
		if (!p) return;
		int port = p->GetListenPort();

		if (port != 0)
		{
			std::vector<boost::asio::ip::address> local_addresses1;

			TcpTransport::GetLocalAdapterIPAddresses(local_addresses1);

			local_addresses1.push_back(boost::asio::ip::address_v4::loopback());

			std::set<boost::asio::ip::address> local_addresses(local_addresses1.begin(), local_addresses1.end());
			
			/*Transport::m_CurrentThreadTransportConnectionURL.reset(new std::string("tcp://localhost:0/"));
			RR_SHARED_PTR<ServiceIndexer> indexer=RR_MAKE_SHARED<ServiceIndexer>();
			RR_INTRUSIVE_PTR<RobotRaconteur::RRMap<int32_t,RobotRaconteurServiceIndex::ServiceInfo> > services=indexer->GetLocalNodeServices();
			Transport::m_CurrentThreadTransportConnectionURL.reset(0);

			std::vector<std::string> service_data2;
			for (std::map<int32_t,RR_INTRUSIVE_PTR<RobotRaconteurServiceIndex::ServiceInfo> >::iterator e=services->map.begin(); e!=services->map.end(); e++)
			{
			std::vector<std::string> implements;
			implements.push_back(e->second->RootObjectType);
			RR_INTRUSIVE_PTR<RobotRaconteur::RRMap<int32_t,RobotRaconteur::RRArray<char> > > impl= e->second->RootObjectImplements;
			for (std::map<int32_t,RR_INTRUSIVE_PTR<RRArray<char> > >::iterator e2=impl->map.begin(); e2!=impl->map.end(); e2++)
			{
			implements.push_back(RRArrayToString(e2->second));
			}

			std::string one="Service: " + e->second->Name + "=" + boost::join(implements,",");
			service_data2.push_back(one);

			}
			std::string service_data=boost::join(service_data2,"\n");
			*/

			
			std::vector<std::string> tcpschemes;
			//tcpschemes.push_back("tcp");
						
			tcpschemes.push_back("rr+tcp");
			

			if (p->IsTlsNodeCertificateLoaded())
			{
				tcpschemes.push_back("rrs+tcp");
				
			}
			
			BOOST_FOREACH (const boost::asio::ip::address& e, local_addresses)
			{
				BOOST_FOREACH (std::string& ee, tcpschemes)
				{
					
					try
					{
						if (e.is_v6())
						{
							boost::asio::ip::address_v6 v6addr = e.to_v6();
							if (!(v6addr.is_link_local() || v6addr.is_loopback())) continue;

						}

						std::string packetdata = generate_response_packet(e, ee, port);
						
						broadcast_discovery_packet(e, packetdata, (IPNodeDiscoveryFlags)broadcast_flags);
						
					}
					catch (std::exception&) {}


				}
			}
		}

		boost::mutex::scoped_lock lock (change_lock);
		if (!broadcasting) return;
		boost::posix_time::time_duration fromnow = broadcast_timer->expires_from_now();
		if (fromnow > boost::posix_time::milliseconds(broadcast_timer_period) || fromnow <= boost::posix_time::milliseconds(0))
		{
			broadcast_timer->expires_from_now(boost::posix_time::milliseconds(broadcast_timer_period));
			RobotRaconteurNode::asio_async_wait(node, broadcast_timer, boost::bind(&IPNodeDiscovery::handle_broadcast_timer, shared_from_this(), boost::asio::placeholders::error));
		}
	}

	
	void IPNodeDiscovery::handle_send( const boost::system::error_code& /*error*/, std::size_t /*bytes_transferred*/, RR_SHARED_PTR<std::string> /*message*/)
	{
	}

	void IPNodeDiscovery::broadcast_discovery_packet(const boost::asio::ip::address& source, const std::string& packet, IPNodeDiscoveryFlags flags)
	{
		boost::asio::io_service ios;
		boost::asio::ip::udp::socket s(ios);

		if (source.is_v4())
		{
			s.open(boost::asio::ip::udp::v4());
		}
		else
		{
			s.open(boost::asio::ip::udp::v6());
		}

		//s.set_option(boost::asio::ip::udp::socket::linger(false,10));

		RR_SHARED_PTR<std::string> shared_message = RR_MAKE_SHARED<std::string>(packet);

		s.set_option(boost::asio::ip::udp::socket::reuse_address(true));
#ifdef ROBOTRACONTEUR_WINDOWS
		u_long exclusiveaddruse = 0;
		::setsockopt(s.native_handle(), SOL_SOCKET, SO_EXCLUSIVEADDRUSE, (const char*)&exclusiveaddruse, sizeof(exclusiveaddruse));
#endif

		boost::asio::ip::udp::endpoint bind_ep(boost::asio::ip::udp::endpoint(source, ANNOUNCE_PORT));

		boost::system::error_code bind_ec;
		s.bind(bind_ep, bind_ec);
		if (bind_ec) return;

		if (source.is_v4())
		{
			if ((static_cast<uint32_t>(flags)& (static_cast<uint32_t>(IPNodeDiscoveryFlags_IPV4_BROADCAST))) != 0)
			{
				boost::system::error_code ec2;
				boost::asio::ip::udp::endpoint dest_ep(boost::asio::ip::address_v4::broadcast(), ANNOUNCE_PORT);
				s.set_option(boost::asio::ip::multicast::enable_loopback(true));
				s.set_option(boost::asio::ip::udp::socket::broadcast(true));
				//s.async_send_to(boost::asio::buffer(*shared_message),dest_ep,boost::bind(&IPNodeDiscovery::handle_send,shared_from_this(),boost::asio::placeholders::error,boost::asio::placeholders::bytes_transferred,shared_message));
				s.send_to(boost::asio::buffer(*shared_message), dest_ep, 0, ec2);
			}
		}
		else
		{
			boost::asio::ip::address_v6 e6 = source.to_v6();
			s.set_option(boost::asio::ip::multicast::enable_loopback(true));

			if ((static_cast<uint32_t>(flags)& (static_cast<uint32_t>(IPNodeDiscoveryFlags_NODE_LOCAL))) != 0)
			{
				boost::system::error_code ec1;
				boost::system::error_code ec2;
				Ip6AddMembership(s, boost::asio::ip::address_v6::from_string("FF01::BA86"), e6.scope_id());
				boost::asio::ip::address_v6 ip6addra = boost::asio::ip::address_v6::from_string("FF01::BA86");
				ip6addra.scope_id(e6.scope_id());
				boost::asio::ip::udp::endpoint dest_ep(ip6addra, ANNOUNCE_PORT);

				//s.async_send_to(boost::asio::buffer(*shared_message),dest_ep,boost::bind(&IPNodeDiscovery::handle_send,shared_from_this(),boost::asio::placeholders::error,boost::asio::placeholders::bytes_transferred,shared_message));
				s.send_to(boost::asio::buffer(*shared_message), dest_ep, 0, ec2);
			}

			boost::asio::ip::address_v6 e6_2 = e6;
			e6_2.scope_id(0);
			if (!source.is_loopback() && e6_2 != boost::asio::ip::address_v6::from_string("FF80::1"))
			{

				if ((static_cast<uint32_t>(flags)& (static_cast<uint32_t>(IPNodeDiscoveryFlags_LINK_LOCAL))) != 0)
				{
					boost::system::error_code ec1;
					boost::system::error_code ec2;
					Ip6AddMembership(s, boost::asio::ip::address_v6::from_string("FF02::BA86"), e6.scope_id());
					boost::asio::ip::address_v6 ip6addra = boost::asio::ip::address_v6::from_string("FF02::BA86");
					ip6addra.scope_id(e6.scope_id());
					boost::asio::ip::udp::endpoint dest_ep(ip6addra, ANNOUNCE_PORT);
					//s.async_send_to(boost::asio::buffer(*shared_message),dest_ep,boost::bind(&IPNodeDiscovery::handle_send,shared_from_this(),boost::asio::placeholders::error,boost::asio::placeholders::bytes_transferred,shared_message));
					s.send_to(boost::asio::buffer(*shared_message), dest_ep, 0, ec2);
				}

				if ((static_cast<uint32_t>(flags)& (static_cast<uint32_t>(IPNodeDiscoveryFlags_SITE_LOCAL))) != 0)
				{
					boost::system::error_code ec1;
					boost::system::error_code ec2;
					Ip6AddMembership(s, boost::asio::ip::address_v6::from_string("FF05::BA86"), e6.scope_id());
					boost::asio::ip::address_v6 ip6addra = boost::asio::ip::address_v6::from_string("FF05::BA86");
					ip6addra.scope_id(e6.scope_id());
					boost::asio::ip::udp::endpoint dest_ep(ip6addra, ANNOUNCE_PORT);
					//s.async_send_to(boost::asio::buffer(*shared_message),dest_ep,boost::bind(&IPNodeDiscovery::handle_send,shared_from_this(),boost::asio::placeholders::error,boost::asio::placeholders::bytes_transferred,shared_message));
					s.send_to(boost::asio::buffer(*shared_message), dest_ep, 0, ec2);
				}
			}
		}

		try
		{
			s.shutdown(boost::asio::ip::udp::socket::shutdown_both);
			s.close();
		}
		catch (std::exception&) {}
	}

	std::string IPNodeDiscovery::generate_response_packet(const boost::asio::ip::address& source, const std::string& scheme, int port)
	{
		std::string nodeidstring = GetNode()->NodeID().ToString();
		std::string packetdata = "Robot Raconteur Node Discovery Packet\n";
		packetdata += (GetNode()->NodeName() == "") ? nodeidstring + "\n" : nodeidstring + "," + GetNode()->NodeName() + "\n";
		
		
		std::string nodeidstr2 = GetNode()->NodeID().ToString("D");

		if (source.is_v4())
		{
			packetdata += scheme + "://" + source.to_string() + ":" + boost::lexical_cast<std::string>(port) + "/?nodeid=" + nodeidstr2 + "&service=RobotRaconteurServiceIndex\n";
		}
		else
		{
			boost::asio::ip::address_v6 e2 = source.to_v6();
			e2.scope_id(0);
			packetdata += scheme + "://[" + e2.to_string() + "]:" + boost::lexical_cast<std::string>(port) + "/?nodeid=" + nodeidstr2 + "&service=RobotRaconteurServiceIndex\n";
		}

		std::string service_data = "ServiceStateNonce: " + GetNode()->GetServiceStateNonce();

		std::string packetdata2 = packetdata + service_data + "\n";
		if (packetdata2.size() <= 2048)
		{
			packetdata = packetdata2;
		}

		return packetdata;
	}

	void IPNodeDiscovery::SendAnnounceNow()
	{
		boost::mutex::scoped_lock lock(change_lock);

		if (!broadcasting) return;

		if (broadcast_timer)
		{
			boost::posix_time::time_duration fromnow = broadcast_timer->expires_from_now();
			if (fromnow > boost::posix_time::milliseconds(500) || broadcast_timer->expires_from_now() < boost::posix_time::milliseconds(0))
			{
				broadcast_timer->expires_from_now(boost::posix_time::milliseconds(500));
				RobotRaconteurNode::asio_async_wait(node, broadcast_timer, boost::bind(&IPNodeDiscovery::handle_broadcast_timer, shared_from_this(), boost::asio::placeholders::error));
			}
		}

	}

	void IPNodeDiscovery::SendDiscoveryRequestNow()
	{
		boost::mutex::scoped_lock lock(change_lock);
		if (!listening) return;
		last_request_send_time = boost::posix_time::microsec_clock::universal_time();

		if (!discovery_request_timer)
		{
			RR_SHARED_PTR<TcpTransport> p1 = parent.lock();
			if (!p1) return;

			discovery_request_timer= RR_MAKE_SHARED<boost::asio::deadline_timer>(boost::ref(p1->GetNode()->GetThreadPool()->get_io_service()));
			uint32_t delay=p1->GetNode()->GetRandomInt<uint32_t>(250, 1000);
			discovery_request_timer->expires_from_now(boost::posix_time::milliseconds(delay));
			RobotRaconteurNode::asio_async_wait(node, discovery_request_timer, boost::bind(&IPNodeDiscovery::handle_request_timer, shared_from_this(), boost::asio::placeholders::error, 3));
		}
	}

	void IPNodeDiscovery::handle_request_timer(const boost::system::error_code& error, int32_t c)
	{		
		if (error == boost::asio::error::operation_aborted)
		{
			return;
		}

		RR_SHARED_PTR<TcpTransport> p1 = parent.lock();
		if (!p1) return;

		boost::mutex::scoped_lock lock(change_lock);

		std::string packetdata = "Robot Raconteur Discovery Request Packet\n";
		packetdata+=this_request_id.ToString() + "\n";

		std::vector<boost::asio::ip::address> local_addresses1;

		TcpTransport::GetLocalAdapterIPAddresses(local_addresses1);

		local_addresses1.push_back(boost::asio::ip::address_v4::loopback());

		std::set<boost::asio::ip::address> local_addresses(local_addresses1.begin(), local_addresses1.end());

		BOOST_FOREACH(const boost::asio::ip::address& e, local_addresses)
		{
			try
			{
				if (e.is_v6())
				{
					boost::asio::ip::address_v6 v6addr = e.to_v6();
					if (!(v6addr.is_link_local() || v6addr.is_loopback())) continue;
				}

				std::string packetdata2 = packetdata;

				broadcast_discovery_packet(e, packetdata, (IPNodeDiscoveryFlags)listen_flags);

			}
			catch (std::exception&) {}			
		}

		c--;

		if (c > 0)
		{
			uint32_t delay = p1->GetNode()->GetRandomInt<uint32_t>(900, 1500);
			discovery_request_timer->expires_from_now(boost::posix_time::milliseconds(delay));
			RobotRaconteurNode::asio_async_wait(node, discovery_request_timer, boost::bind(&IPNodeDiscovery::handle_request_timer, shared_from_this(), boost::asio::placeholders::error, c));
		}
		else
		{
			if ((last_request_send_time + boost::posix_time::milliseconds(1000))
				> boost::posix_time::microsec_clock::universal_time())
			{
				discovery_request_timer->expires_from_now(boost::posix_time::seconds(5));
				RobotRaconteurNode::asio_async_wait(node, discovery_request_timer, boost::bind(&IPNodeDiscovery::handle_request_timer, shared_from_this(), boost::asio::placeholders::error, 3));
			}
			else
			{
				discovery_request_timer.reset();
			}
		}

	}

	int32_t IPNodeDiscovery::GetNodeAnnouncePeriod()
	{
		boost::mutex::scoped_lock lock(change_lock);
		return broadcast_timer_period;
	}

	void IPNodeDiscovery::SetNodeAnnouncePeriod(int32_t millis)
	{
		boost::mutex::scoped_lock lock(change_lock);
		if (millis < 500) throw InvalidOperationException("Invalid node announce period");
		broadcast_timer_period = millis;
	}

	const int32_t IPNodeDiscovery::ANNOUNCE_PORT = 48653;


	TcpTransportPortSharerClient::TcpTransportPortSharerClient(RR_SHARED_PTR<TcpTransport> parent)
	{
		this->parent = parent;
		open = false;
		delay_event = RR_MAKE_SHARED<AutoResetEvent>();
		port = 0;
		sharer_connected = false;
	}

	void TcpTransportPortSharerClient::Start()
	{
		boost::mutex::scoped_lock lock(this_lock);
		if (open) throw InvalidOperationException("Already running server");
		open = true;
		boost::thread(boost::bind(&TcpTransportPortSharerClient::client_thread, shared_from_this()));
	}

	int32_t TcpTransportPortSharerClient::GetListenPort()
	{
		boost::mutex::scoped_lock lock(this_lock);
		return port;
	}

	void TcpTransportPortSharerClient::Close()
	{
		boost::mutex::scoped_lock lock(this_lock);
		open = false;
		if (localsocket)
		{
			boost::system::error_code ec;
			if (localsocket)
			{
				localsocket->close(ec);
			}
			localsocket.reset();
		}

		delay_event->Set();
	}
	
	RR_SHARED_PTR<TcpTransport> TcpTransportPortSharerClient::GetParent()
	{
		RR_SHARED_PTR<TcpTransport> p = parent.lock();
		if (!p) throw new InvalidOperationException("Parent released");
		return p;
	}

	void TcpTransportPortSharerClient::client_thread()
	{
		while (true)
		{
			{		
				boost::mutex::scoped_lock lock(this_lock);
				if (!open) break;
				localsocket.reset();
			}

			try
			{
				NodeID nodeid;
				std::string nodename;
				{
					RR_SHARED_PTR<RobotRaconteurNode> node = GetParent()->GetNode();
					nodeid = node->NodeID();
					nodename = node->NodeName();
				}

				if (nodeid.IsAnyNode()) throw InternalErrorException("Internal error");

				std::string outdata = nodeid.ToString() + " " + nodename;
				boost::trim(outdata);

#ifdef ROBOTRACONTEUR_WINDOWS

				std::string fname;

				boost::optional<boost::filesystem::path> p1_1 = detail::LocalTransportUtil::GetTransportPublicSearchPath();
				if (p1_1)
				{
					boost::filesystem::path p1 = *p1_1;
					p1 /= "..";
					p1 /= "tcp";
					p1 /= "portsharer";
					p1 /= "portsharer.info";
					p1.normalize();

					std::map<std::string, std::string> info;
					if (detail::LocalTransportUtil::ReadInfoFile(p1, info))
					{

						std::map<std::string, std::string>::iterator fname1 = info.find("socket");
						if (fname1 != info.end())
						{
							fname = fname1->second;
						}
					}
				}
					
				if (fname.empty())
				{
					boost::filesystem::path p1 = detail::LocalTransportUtil::GetTransportPrivateSocketPath();
					p1 /= "..";
					p1 /= "tcp";
					p1 /= "portsharer";
					p1 /= "portsharer.info";
					p1.normalize();

					std::map<std::string, std::string> info;
					if (detail::LocalTransportUtil::ReadInfoFile(p1, info))
					{

						std::map<std::string, std::string>::iterator fname1 = info.find("socket");
						if (fname1 != info.end())
						{
							fname = fname1->second;
						}
					}

				}

				::WaitNamedPipeA(fname.c_str(), 100);
				HANDLE h = CreateFileA(fname.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED | SECURITY_SQOS_PRESENT | SECURITY_IDENTIFICATION, NULL);
				if (h == INVALID_HANDLE_VALUE)
				{
					delay_event->WaitOne(1000);
					continue;
				}

				DWORD params = PIPE_READMODE_MESSAGE;
				if (!SetNamedPipeHandleState(h, &params, NULL, NULL))
				{
					::CloseHandle(h);
					throw InternalErrorException("Internal error");
				}

				RR_SHARED_PTR<boost::asio::windows::stream_handle> l;
				{
					boost::mutex::scoped_lock lock(this_lock);
					RR_SHARED_PTR<RobotRaconteurNode> node = GetParent()->GetNode();
					l = RR_MAKE_SHARED<boost::asio::windows::stream_handle>(boost::ref(node->GetThreadPool()->get_io_service()), h);

					/*if (!detail::LocalTransportUtil::IsPipeSameUserOrService(l->native_handle(), true))
					{
						l.reset();
						delay_event->WaitOne(1000);
						continue;
					}*/

					localsocket = l;
				}

#else
				std::string fname="/var/run/robotraconteur/transport/tcp/portsharer/portsharer.sock";
				RR_SHARED_PTR<boost::asio::local::stream_protocol::socket> l
					=RR_MAKE_SHARED<boost::asio::local::stream_protocol::socket>(boost::ref(GetParent()->GetNode()->GetThreadPool()->get_io_service()));
				l->connect(boost::asio::local::stream_protocol::endpoint(fname));

				{
					boost::mutex::scoped_lock lock(this_lock);
					localsocket = l;
				}
#endif

				if (!l)
				{
					delay_event->WaitOne(1000);
					continue;
				}

				boost::asio::write(*l, boost::asio::buffer(outdata.c_str(), outdata.size()));

				int port1;

				{
					uint8_t indata2[4096];


					int nread = (int32_t)l->read_some(boost::asio::buffer(&indata2, sizeof(indata2)));

					if (nread == 0) throw InvalidOperationException("Connection closed");
					std::string indata1((char*)indata2, nread);

					if (!boost::starts_with(indata1, "OK"))
					{
						throw OperationFailedException("Return error");
					}

					std::vector<std::string> indata_s;
					boost::split(indata_s, indata1, boost::is_any_of(" \t"),boost::algorithm::token_compress_on);
					if (indata_s.size() != 2) throw OperationFailedException("Return error");

					port1 = boost::lexical_cast<int32_t>(indata_s.at(1));

					{
						boost::mutex::scoped_lock lock(this_lock);
						port = port1;
					}

				}

				while (true)
				{
					{
						boost::mutex::scoped_lock lock(this_lock);
						if (!open) break;
						sharer_connected = true;
					}

#ifdef ROBOTRACONTEUR_WINDOWS
					uint8_t buf[4096];
					int bytes_read = (int32_t)l->read_some(boost::asio::buffer(buf, 4096));

					if (bytes_read != sizeof(WSAPROTOCOL_INFOW)) throw InternalErrorException("Invalid data");

					LPWSAPROTOCOL_INFOW prot=(LPWSAPROTOCOL_INFOW)buf;

					SOCKET sock=::WSASocketW(prot->iAddressFamily, SOCK_STREAM, IPPROTO_TCP, prot, 0, WSA_FLAG_OVERLAPPED);
					if (sock == INVALID_SOCKET) continue;
					try
					{
						boost::mutex::scoped_lock lock(this_lock);
						RR_SHARED_PTR<RobotRaconteurNode> node = GetParent()->GetNode();
						RR_SHARED_PTR<boost::asio::ip::tcp::socket> ssocket;
						if (prot->iAddressFamily == AF_INET)
						{
							ssocket = RR_MAKE_SHARED<boost::asio::ip::tcp::socket>(boost::ref(node->GetThreadPool()->get_io_service()), boost::asio::ip::tcp::v4(), sock);
						}
						else if (prot->iAddressFamily == AF_INET6)
						{
							ssocket = RR_MAKE_SHARED<boost::asio::ip::tcp::socket>(boost::ref(node->GetThreadPool()->get_io_service()), boost::asio::ip::tcp::v6(), sock);
						}
						else
						{
							continue;
						}
						IncomingSocket(ssocket);
					}
					catch (std::exception&) {}
#else

					int sock;

					uint8_t buf[1024];


					if (detail::TcpTransportUtil::read_fd(l->native_handle(), buf, sizeof(buf),&sock)!=1)
					{
						break;
					}

					if (sock==0) break;
					if (sock<0) continue;

					try
					{
						boost::mutex::scoped_lock lock(this_lock);
						RR_SHARED_PTR<RobotRaconteurNode> node = GetParent()->GetNode();
						RR_SHARED_PTR<boost::asio::ip::tcp::socket> ssocket;


						struct sockaddr sock_addr;
						socklen_t address_len=sizeof(struct sockaddr);

						if (getsockname(sock, &sock_addr, &address_len)<0) continue;



						if (sock_addr.sa_family == AF_INET)
						{
							ssocket = RR_MAKE_SHARED<boost::asio::ip::tcp::socket>(boost::ref(node->GetThreadPool()->get_io_service()), boost::asio::ip::tcp::v4(), sock);
						}
						else if (sock_addr.sa_family == AF_INET6)
						{
							ssocket = RR_MAKE_SHARED<boost::asio::ip::tcp::socket>(boost::ref(node->GetThreadPool()->get_io_service()), boost::asio::ip::tcp::v6(), sock);
						}
						else
						{
							continue;
						}

						IncomingSocket(ssocket);

					}
					catch (std::exception&) {}
#endif

				}
					


			}
			catch (std::exception&) {}

			{
				boost::mutex::scoped_lock lock(this_lock);
				sharer_connected = false;
				boost::system::error_code ec;
				if (localsocket)
				{
					localsocket->close();
				}
				localsocket.reset();
				port = 0;
			}

			delay_event->WaitOne(1000);
			
		}

		{
			boost::mutex::scoped_lock lock(this_lock);
			boost::system::error_code ec;
			if (localsocket)
			{
				localsocket->close();
			}
			localsocket.reset();
			port = 0;
		}

		
	}

	void TcpTransportPortSharerClient::IncomingSocket(RR_SHARED_PTR<boost::asio::ip::tcp::socket> socket)
	{
		RR_SHARED_PTR<TcpTransport> parent1 = GetParent();
		try
		{
			socket->set_option(boost::asio::ip::tcp::socket::linger(true, 5));

			//TcpTransport_attach_transport(parent1, socket, "", true, 0, boost::bind(&TcpTransport_connected_callback2, parent1, _1, _2, _3));
			RR_SHARED_PTR<detail::TcpAcceptor> a = RR_MAKE_SHARED<detail::TcpAcceptor>(parent1, "", 0);
			boost::function<void(RR_SHARED_PTR<boost::asio::ip::tcp::socket>, RR_SHARED_PTR<ITransportConnection>, RR_SHARED_PTR<RobotRaconteurException>)> h 
				= boost::bind(&TcpTransport_connected_callback2, parent1, _1, _2, _3);
			a->AcceptSocket(socket, h);
		}
		catch (std::exception& exp)
		{
			try
			{
				RobotRaconteurNode::TryHandleException(parent1->GetNode(), &exp);
			}
			catch (std::exception&) {}
		}

	}

	bool TcpTransportPortSharerClient::IsPortSharerConnected()
	{
		boost::mutex::scoped_lock lock(this_lock);
		return sharer_connected;
	}

#ifndef ROBOTRACONTEUR_WINDOWS
#ifdef SUNOS5
#undef HAVE_MSGHDR_MSG_CONTROL
#else
#define HAVE_MSGHDR_MSG_CONTROL
#endif
	
	namespace TcpTransportUtil
	{
		ssize_t
		read_fd(int fd, void *ptr, size_t nbytes, int *recvfd)
		{
			struct msghdr   msg;
			struct iovec    iov[1];
			ssize_t         n;
		#ifndef  HAVE_MSGHDR_MSG_CONTROL
			int             newfd;
		#endif

		#ifdef  HAVE_MSGHDR_MSG_CONTROL
			union {
				struct cmsghdr    cm;
				char              control[CMSG_SPACE(sizeof(int))];
			} control_un;
			struct cmsghdr  *cmptr;

			msg.msg_control = control_un.control;
			msg.msg_controllen = sizeof(control_un.control);
		#else
			msg.msg_accrights = (caddr_t) &newfd;
			msg.msg_accrightslen = sizeof(int);
		#endif

			msg.msg_name = NULL;
			msg.msg_namelen = 0;

			iov[0].iov_base = ptr;
			iov[0].iov_len = nbytes;
			msg.msg_iov = iov;
			msg.msg_iovlen = 1;

			if ( (n = recvmsg(fd, &msg, 0)) <= 0)
				return(n);

		#ifdef  HAVE_MSGHDR_MSG_CONTROL
			if ( (cmptr = CMSG_FIRSTHDR(&msg)) != NULL &&
				cmptr->cmsg_len == CMSG_LEN(sizeof(int))) {
				if (cmptr->cmsg_level != SOL_SOCKET)
					throw SystemResourceException("control level != SOL_SOCKET");
				if (cmptr->cmsg_type != SCM_RIGHTS)
					throw SystemResourceException("control type != SCM_RIGHTS");
				*recvfd = *((int *) CMSG_DATA(cmptr));
			} else
				*recvfd = -1;       /* descriptor was not passed */
		#else
		/* *INDENT-OFF* */
			if (msg.msg_accrightslen == sizeof(int))
				*recvfd = newfd;
			else
				*recvfd = -1;       /* descriptor was not passed */
		/* *INDENT-ON* */
		#endif

			return(n);
		}
		/* end read_fd */


	}

	
#endif

}





}
