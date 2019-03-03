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

#include "RobotRaconteur/RobotRaconteurNode.h"
#include <boost/shared_array.hpp>

#pragma once

namespace RobotRaconteur
{
	enum IPNodeDiscoveryFlags
	{
		IPNodeDiscoveryFlags_NODE_LOCAL = 0x1,
		IPNodeDiscoveryFlags_LINK_LOCAL = 0x2,
		IPNodeDiscoveryFlags_SITE_LOCAL = 0x4,
		IPNodeDiscoveryFlags_IPV4_BROADCAST = 0x8

	};
		

	class TcpTransportConnection;
	namespace detail
	{
		class TcpConnector;
		class TcpWebSocketConnector;
		class TcpWSSWebSocketConnector;
	}

	class ROBOTRACONTEUR_CORE_API TcpTransport : public Transport, public RR_ENABLE_SHARED_FROM_THIS<TcpTransport>
	{
		friend class TcpTransportConnection;
		friend class detail::TcpConnector;
		friend class detail::TcpWebSocketConnector;
		friend class detail::TcpWSSWebSocketConnector;

	private:
		int32_t m_Port;		
		
	public:
		RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<ITransportConnection> > TransportConnections;
		boost::mutex TransportConnections_lock;
		
		std::list<RR_WEAK_PTR<ITransportConnection> > closing_TransportConnections;
		std::list<RR_WEAK_PTR<ITransportConnection> > incoming_TransportConnections;

		TcpTransport(RR_SHARED_PTR<RobotRaconteurNode> node=RobotRaconteurNode::sp());
		
		virtual ~TcpTransport();


	public:
		virtual bool IsServer() const;
		virtual bool IsClient() const;

		
		virtual int32_t GetDefaultReceiveTimeout();
		virtual void SetDefaultReceiveTimeout(int32_t milliseconds);
		virtual int32_t GetDefaultConnectTimeout();
		virtual void SetDefaultConnectTimeout(int32_t milliseconds);

		virtual std::string GetUrlSchemeString() const;

		virtual int32_t GetListenPort();

		virtual void SendMessage(RR_INTRUSIVE_PTR<Message> m);

		virtual void AsyncSendMessage(RR_INTRUSIVE_PTR<Message> m, boost::function<void (RR_SHARED_PTR<RobotRaconteurException> )>& callback);

		virtual void AsyncCreateTransportConnection(const std::string& url, RR_SHARED_PTR<Endpoint> e, boost::function<void (RR_SHARED_PTR<ITransportConnection>, RR_SHARED_PTR<RobotRaconteurException> ) >& callback);


		virtual RR_SHARED_PTR<ITransportConnection> CreateTransportConnection(const std::string& url, RR_SHARED_PTR<Endpoint> e);

		

		virtual void CloseTransportConnection(RR_SHARED_PTR<Endpoint> e);

	protected:

		virtual void CloseTransportConnection_timed(const boost::system::error_code& err, RR_SHARED_PTR<Endpoint> e, RR_SHARED_PTR<void> timer);
		
	public:

		

		virtual void StartServer(int32_t porte);


		virtual void StartServerUsingPortSharer();
		virtual bool IsPortSharerRunning();

		
		virtual bool CanConnectService(const std::string& url);
				

		
		virtual void Close();

		virtual void CheckConnection(uint32_t endpoint);

		
		void EnableNodeDiscoveryListening(uint32_t flags=(IPNodeDiscoveryFlags_LINK_LOCAL));

		void DisableNodeDiscoveryListening();

		
		void EnableNodeAnnounce(uint32_t flags=(IPNodeDiscoveryFlags_LINK_LOCAL));

		void DisableNodeAnnounce();

		int32_t GetNodeAnnouncePeriod();
		void SetNodeAnnouncePeriod(int32_t millis);

		void SendDiscoveryRequest();

		virtual void PeriodicCleanupTask();		

		uint32_t TransportCapability(const std::string& name);


		static void GetLocalAdapterIPAddresses(std::vector<boost::asio::ip::address>& addresses);

		virtual void MessageReceived(RR_INTRUSIVE_PTR<Message> m);

		virtual int32_t GetDefaultHeartbeatPeriod();
		virtual void SetDefaultHeartbeatPeriod(int32_t milliseconds);

		virtual int32_t GetMaxMessageSize();
		virtual void SetMaxMessageSize(int32_t size);
		virtual int32_t GetMaxConnectionCount();
		virtual void SetMaxConnectionCount(int32_t count);

		virtual bool GetRequireTls();
		virtual void SetRequireTls(bool require_tls);

		virtual void LoadTlsNodeCertificate();

		virtual bool IsTlsNodeCertificateLoaded();
		
		virtual bool IsTransportConnectionSecure(uint32_t endpoint);
		virtual bool IsTransportConnectionSecure(RR_SHARED_PTR<Endpoint> endpoint);
		virtual bool IsTransportConnectionSecure(RR_SHARED_PTR<RRObject> obj);
		virtual bool IsTransportConnectionSecure(RR_SHARED_PTR<ITransportConnection> transport);

		virtual bool IsSecurePeerIdentityVerified(uint32_t endpoint);
		virtual bool IsSecurePeerIdentityVerified(RR_SHARED_PTR<Endpoint> endpoint);
		virtual bool IsSecurePeerIdentityVerified(RR_SHARED_PTR<RRObject> obj);
		virtual bool IsSecurePeerIdentityVerified(RR_SHARED_PTR<ITransportConnection> tranpsort);

		virtual std::string GetSecurePeerIdentity(uint32_t endpoint);
		virtual std::string GetSecurePeerIdentity(RR_SHARED_PTR<Endpoint> endpoint);
		virtual std::string GetSecurePeerIdentity(RR_SHARED_PTR<RRObject> obj);
		virtual std::string GetSecurePeerIdentity(RR_SHARED_PTR<ITransportConnection> transport);

		virtual bool GetAcceptWebSockets();
		virtual void SetAcceptWebSockets(bool value);

		virtual std::vector<std::string> GetWebSocketAllowedOrigins();
		virtual void AddWebSocketAllowedOrigin(const std::string& origin);
		virtual void RemoveWebSocketAllowedOrigin(const std::string& origin);

		virtual bool GetDisableMessage3();
		virtual void SetDisableMessage3(bool d);

		virtual bool GetDisableStringTable();
		virtual void SetDisableStringTable(bool d);

		virtual bool GetDisableAsyncMessageIO();
		virtual void SetDisableAsyncMessageIO(bool d);

		template<typename T, typename F>
		boost::signals2::connection AddCloseListener(RR_SHARED_PTR<T> t, const F& f)
		{
			boost::mutex::scoped_lock lock(closed_lock);
			if (closed)
			{
				lock.unlock();
				boost::bind(f, t) ();
				return boost::signals2::connection();
			}

			return close_signal.connect(boost::signals2::signal<void()>::slot_type(
				boost::bind(f, t.get())
			).track(t));			
		}

	protected:

		virtual void LocalNodeServicesChanged();

	protected:

		boost::mutex node_discovery_lock;
		RR_SHARED_PTR<void> node_discovery;

		RR_SHARED_PTR<boost::asio::ip::tcp::acceptor> ipv4_acceptor;
		RR_SHARED_PTR<boost::asio::ip::tcp::acceptor> ipv6_acceptor;
		boost::mutex acceptor_lock;
		bool ipv4_acceptor_paused;
		bool ipv6_acceptor_paused;

		static void handle_v4_accept(RR_SHARED_PTR<TcpTransport> parent,RR_SHARED_PTR<boost::asio::ip::tcp::acceptor> acceptor, RR_SHARED_PTR<boost::asio::ip::tcp::socket> socket, const boost::system::error_code& error);

		static void handle_v6_accept(RR_SHARED_PTR<TcpTransport> parent,RR_SHARED_PTR<boost::asio::ip::tcp::acceptor> acceptor, RR_SHARED_PTR<boost::asio::ip::tcp::socket> socket, const boost::system::error_code& error);

		virtual void register_transport(RR_SHARED_PTR<ITransportConnection> connection);
		virtual void erase_transport(RR_SHARED_PTR<ITransportConnection> connection);
		virtual void incoming_transport(RR_SHARED_PTR<ITransportConnection> connection);


		boost::mutex parameter_lock;
		int32_t heartbeat_period;
		int32_t default_connect_timeout;
		int32_t default_receive_timeout;
		int32_t max_message_size;
		int32_t max_connection_count;
		bool disable_message3;
		bool disable_string_table;
		bool disable_async_message_io;
		
		boost::shared_ptr<void> GetTlsContext();

		boost::shared_ptr<void> tls_context;
		boost::mutex tls_context_lock;
		bool require_tls;

		RR_SHARED_PTR<void> port_sharer_client;
		boost::mutex port_sharer_client_lock;

		bool accept_websockets;

		std::vector<std::string> allowed_websocket_origins;

		bool closed;
		boost::signals2::signal<void()> close_signal;
		boost::mutex closed_lock;
	};



	
}
