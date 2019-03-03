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

#include "RobotRaconteur/TcpTransport.h"
#include "ASIOStreamBaseTransport.h"
#include <list>

#include "websocket_stream.hpp"

#ifdef ROBOTRACONTEUR_USE_SCHANNEL
#include "TlsSchannelStreamAdapter.h"
#endif

#ifdef ROBOTRACONTEUR_USE_OPENSSL
#include <boost/asio/ssl.hpp>
#include "OpenSSLAuthContext.h"
#endif

#pragma once

#ifndef IPV6_ADD_MEMBERSHIP
#ifdef IPV6_JOIN_GROUP
#define IPV6_ADD_MEMBERSHIP IPV6_JOIN_GROUP
#endif
#endif

namespace RobotRaconteur
{
class TcpTransportConnection : public detail::ASIOStreamBaseTransport
	{
	public:
		
		friend class TcpTransport;

		TcpTransportConnection(RR_SHARED_PTR<TcpTransport> parent, std::string url, bool server, uint32_t local_endpoint);

		void AsyncAttachSocket(RR_SHARED_PTR<boost::asio::ip::tcp::socket> socket, boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)>& callback);

		void AsyncAttachWebSocket(RR_SHARED_PTR<boost::asio::ip::tcp::socket> socket, RR_SHARED_PTR<detail::websocket_stream<boost::asio::ip::tcp::socket&> > websocket, boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)>& callback);
#ifdef ROBOTRACONTEUR_USE_SCHANNEL
		void AsyncAttachWSSWebSocket(RR_SHARED_PTR<boost::asio::ip::tcp::socket> socket, RR_SHARED_PTR<detail::TlsSchannelAsyncStreamAdapter> wss_websocket_tls, RR_SHARED_PTR<detail::websocket_stream<detail::TlsSchannelAsyncStreamAdapter_ASIO_adapter&> > wss_websocket, boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)>& callback);
#endif

#ifdef ROBOTRACONTEUR_USE_OPENSSL
		void AsyncAttachWSSWebSocket(RR_SHARED_PTR<boost::asio::ip::tcp::socket> socket, RR_SHARED_PTR<boost::asio::ssl::stream<boost::asio::ip::tcp::socket& > > wss_websocket_tls, RR_SHARED_PTR<detail::websocket_stream<boost::asio::ssl::stream<boost::asio::ip::tcp::socket &> &> >  wss_websocket, RR_SHARED_PTR<boost::asio::ssl::context> wss_context, boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)>& callback);
#endif


	protected:

		void do_starttls1(std::string noden, const boost::system::error_code& error, boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)>& callback);

		void do_starttls2(RR_SHARED_PTR<RobotRaconteurException> error, boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)>& callback);

		//void do_starttls3(RR_SHARED_PTR<RobotRaconteurException> error, boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)>& callback);

		void do_starttls4(const std::string& servername, const boost::system::error_code& error);

		void do_starttls5(const boost::system::error_code& error);

		void do_starttls5_1(RR_SHARED_PTR<RRObject> parameter, RR_SHARED_PTR<RobotRaconteurException> err, boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)>& callback);

		void do_starttls6(const boost::system::error_code& error, RR_INTRUSIVE_PTR<Message> request);

		void do_starttls7(const boost::system::error_code& error, RR_INTRUSIVE_PTR<Message> request);

		void do_starttls8(RR_SHARED_PTR<RobotRaconteurException> error, RR_INTRUSIVE_PTR<Message> request);

		void do_starttls9(const boost::system::error_code& error);

	public:

		virtual void MessageReceived(RR_INTRUSIVE_PTR<Message> m);
	protected:
		virtual void async_write_some(const_buffers& b, boost::function<void (const boost::system::error_code& error, size_t bytes_transferred)>& handler);

		virtual void async_read_some(mutable_buffers& b, boost::function<void (const boost::system::error_code& error, size_t bytes_transferred)>& handler) ;
		
		virtual size_t available();

		virtual bool IsLargeTransferAuthorized();

				
	public:
		virtual void Close();

	protected:

		void Close1(const boost::system::error_code& ec);

		bool IsClosed();

		void ForceClose();

	public:

		virtual  uint32_t GetLocalEndpoint() ;

		virtual  uint32_t GetRemoteEndpoint() ;

		virtual void CheckConnection(uint32_t endpoint);

		virtual bool IsSecure();

		virtual bool IsSecurePeerIdentityVerified();

		virtual std::string GetSecurePeerIdentity();

	protected:

		RR_SHARED_PTR<boost::asio::ip::tcp::socket> socket;
		boost::mutex socket_lock;
		bool server;
		std::string url;

		RR_WEAK_PTR<TcpTransport> parent;

		uint32_t m_RemoteEndpoint;
		uint32_t m_LocalEndpoint;

		boost::recursive_mutex close_lock;
		bool closing;
		
		
		virtual void StreamOpMessageReceived(RR_INTRUSIVE_PTR<Message> m);

		//Stuff to support TLS
		
		bool is_tls;
		bool require_tls;
		RR_SHARED_PTR<boost::asio::deadline_timer> starttls_timer;
		boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)> starttls_handler;
		
		bool tls_mutual_auth;
		bool tls_handshaking;
		

#ifdef ROBOTRACONTEUR_USE_SCHANNEL
		RR_SHARED_PTR<RobotRaconteur::detail::TlsSchannelAsyncStreamAdapter> tls_socket;
		RR_SHARED_PTR<RobotRaconteur::detail::TlsSchannelAsyncStreamAdapter> tls_websocket;
		RR_SHARED_PTR<detail::websocket_stream<RobotRaconteur::detail::TlsSchannelAsyncStreamAdapter_ASIO_adapter&> > wss_websocket;
		RR_SHARED_PTR<RobotRaconteur::detail::TlsSchannelAsyncStreamAdapter> wss_websocket_tls;
		RR_SHARED_PTR<RobotRaconteur::detail::TlsSchannelAsyncStreamAdapter> tls_wss_websocket;

#endif		

#ifdef ROBOTRACONTEUR_USE_OPENSSL
		RR_SHARED_PTR<boost::asio::ssl::stream<boost::asio::ip::tcp::socket&> > tls_socket;
		RR_SHARED_PTR<detail::OpenSSLAuthContext> tls_context;
		RR_SHARED_PTR<boost::asio::ssl::stream<detail::websocket_stream<boost::asio::ip::tcp::socket& > & > > tls_websocket;
		RR_SHARED_PTR<detail::websocket_stream<boost::asio::ssl::stream<boost::asio::ip::tcp::socket& > & > > wss_websocket;
		RR_SHARED_PTR<boost::asio::ssl::stream<boost::asio::ip::tcp::socket& > > wss_websocket_tls;
		RR_SHARED_PTR<boost::asio::ssl::stream<detail::websocket_stream<boost::asio::ssl::stream<boost::asio::ip::tcp::socket& > & > & > > tls_wss_websocket;
		RR_SHARED_PTR<boost::asio::ssl::context> wss_context;
#endif

		RR_SHARED_PTR<detail::websocket_stream<boost::asio::ip::tcp::socket&> > websocket;
		bool use_websocket;
		bool use_wss_websocket;

	};

	void TcpTransport_attach_transport(RR_SHARED_PTR<TcpTransport> parent,RR_SHARED_PTR<boost::asio::ip::tcp::socket> socket, std::string url, bool server, uint32_t endpoint, boost::function<void( RR_SHARED_PTR<boost::asio::ip::tcp::socket> , RR_SHARED_PTR<ITransportConnection> , RR_SHARED_PTR<RobotRaconteurException> )>& callback);

	void TcpTransport_connected_callback2(RR_SHARED_PTR<TcpTransport> parent, RR_SHARED_PTR<boost::asio::ip::tcp::socket> socket, RR_SHARED_PTR<ITransportConnection> connection, RR_SHARED_PTR<RobotRaconteurException> err);
	

	namespace detail
	{

		class TcpConnector : public RR_ENABLE_SHARED_FROM_THIS<TcpConnector>
		{
		public:
			TcpConnector(RR_SHARED_PTR<TcpTransport> parent);

			void Connect(std::vector<std::string> url, uint32_t endpoint, boost::function<void(RR_SHARED_PTR<ITransportConnection>, RR_SHARED_PTR<RobotRaconteurException>) > callback);
			

		protected:
			//void attach_transport(RR_SHARED_PTR<boost::asio::ip::tcp::socket> socket, boost::function<void( RR_SHARED_PTR<boost::asio::ip::tcp::socket> , RR_SHARED_PTR<SuperNodeTransportConnection> , myerr )>& callback);

			void connect2(int32_t key, const boost::system::error_code& err, boost::asio::ip::tcp::resolver::iterator endpoint_iterator, boost::function<void(RR_SHARED_PTR<TcpTransportConnection>, RR_SHARED_PTR<RobotRaconteurException>) > callback);

			void connected_callback(RR_SHARED_PTR<boost::asio::ip::tcp::socket> socket, RR_SHARED_PTR<boost::signals2::scoped_connection> socket_closer, int32_t key, const boost::system::error_code& error);

			void connected_callback2(RR_SHARED_PTR<boost::asio::ip::tcp::socket> socket, int32_t key, RR_SHARED_PTR<ITransportConnection> connection, RR_SHARED_PTR<RobotRaconteurException> err);

			void connect_timer_callback(const boost::system::error_code& e);

			RR_SHARED_PTR<TcpTransport> parent;

			RR_SHARED_PTR<boost::asio::deadline_timer> connect_timer;
			
			bool connecting;

			boost::mutex connecting_lock;

			boost::function<void(RR_SHARED_PTR<ITransportConnection>, RR_SHARED_PTR<RobotRaconteurException>) > callback;

			RR_SHARED_PTR<boost::asio::ip::tcp::resolver> _resolver;

			uint32_t endpoint;

			int32_t resolve_count;
			int32_t connect_count;
						
			std::list<int32_t> active;
			int32_t active_count;
			std::list<RR_SHARED_PTR<RobotRaconteurException> > errors;

			void handle_error(const int32_t& key, const boost::system::error_code& err);
			void handle_error(const int32_t& key, RR_SHARED_PTR<RobotRaconteurException> err);

			bool socket_connected;

			boost::mutex this_lock;
			std::string url;

			RR_WEAK_PTR<RobotRaconteurNode> node;			
		public:

		};



		class TcpAcceptor : public RR_ENABLE_SHARED_FROM_THIS < TcpAcceptor >
		{
		private:

			RR_SHARED_PTR<TcpTransport> parent;
			std::string url;
			uint32_t local_endpoint;

			char buf[4];

			boost::posix_time::ptime start_time;

			RR_WEAK_PTR<RobotRaconteurNode> node;

		protected:

			void AcceptSocket5(const boost::system::error_code& ec,
				RR_SHARED_PTR<boost::asio::ip::tcp::socket> socket,
				RR_SHARED_PTR<websocket_stream<boost::asio::ip::tcp::socket&> > websocket,
				RR_SHARED_PTR<boost::signals2::scoped_connection> socket_closer,
				boost::function<void(RR_SHARED_PTR<boost::asio::ip::tcp::socket>, RR_SHARED_PTR<ITransportConnection>, RR_SHARED_PTR<RobotRaconteurException>)>& handler
			);

			static void AcceptSocket4(RR_SHARED_PTR<std::string> dat, 
				RR_SHARED_PTR<boost::asio::ip::tcp::socket> socket,
				RR_SHARED_PTR<boost::signals2::scoped_connection> socket_closer);

			void AcceptSocket3(const boost::system::error_code& ec, RR_SHARED_PTR<boost::asio::deadline_timer> timer,
				RR_SHARED_PTR<boost::asio::ip::tcp::socket> socket,
				RR_SHARED_PTR<boost::signals2::scoped_connection> socket_closer,
				boost::function<void(RR_SHARED_PTR<boost::asio::ip::tcp::socket>, RR_SHARED_PTR<ITransportConnection>, RR_SHARED_PTR<RobotRaconteurException>)>& handler);

			void AcceptSocket2(const boost::system::error_code& ec, size_t n,
				RR_SHARED_PTR<boost::asio::ip::tcp::socket> socket,
				RR_SHARED_PTR<boost::signals2::scoped_connection> socket_closer,
				boost::function<void(RR_SHARED_PTR<boost::asio::ip::tcp::socket>, RR_SHARED_PTR<ITransportConnection>, RR_SHARED_PTR<RobotRaconteurException>)>& handler);


		public:

			TcpAcceptor(RR_SHARED_PTR<TcpTransport> parent, std::string url, uint32_t local_endpoint);

			void AcceptSocket(RR_SHARED_PTR<boost::asio::ip::tcp::socket> socket, boost::function<void(RR_SHARED_PTR<boost::asio::ip::tcp::socket>, RR_SHARED_PTR<ITransportConnection>, RR_SHARED_PTR<RobotRaconteurException>)>& handler);
		};

		class TcpWebSocketConnector : public RR_ENABLE_SHARED_FROM_THIS < TcpWebSocketConnector >
		{
		protected:
			RR_SHARED_PTR<TcpTransport> parent;
			uint32_t endpoint;
			std::string url;
			std::string ws_url;

		protected:

			void Connect4(RR_SHARED_PTR<RobotRaconteurException> err,
				RR_SHARED_PTR<ITransportConnection> connection,
				RR_SHARED_PTR<boost::asio::ip::tcp::socket> socket,
				RR_SHARED_PTR<websocket_stream<boost::asio::ip::tcp::socket&> > websocket,
				boost::function<void(RR_SHARED_PTR<ITransportConnection>, RR_SHARED_PTR<RobotRaconteurException>) > handler
			);			

			void Connect3(const boost::system::error_code& ec,
				RR_SHARED_PTR<boost::asio::ip::tcp::socket> socket,
				RR_SHARED_PTR<websocket_stream<boost::asio::ip::tcp::socket&> > websocket,
				RR_SHARED_PTR<boost::signals2::scoped_connection> socket_closer,
				boost::function<void(RR_SHARED_PTR<ITransportConnection>, RR_SHARED_PTR<RobotRaconteurException>) > handler
			);
			
			void Connect2(const boost::system::error_code& ec,
				RR_SHARED_PTR<boost::asio::ip::tcp::socket> socket,
				RR_SHARED_PTR<websocket_tcp_connector> socket_connector,
				boost::function<void(RR_SHARED_PTR<ITransportConnection>, RR_SHARED_PTR<RobotRaconteurException>) > handler);
		public:

			TcpWebSocketConnector(RR_SHARED_PTR<TcpTransport> parent);
			
			void Connect(std::string url, uint32_t endpoint, boost::function<void(RR_SHARED_PTR<ITransportConnection>, RR_SHARED_PTR<RobotRaconteurException>) > handler);			
		};

#ifdef ROBOTRACONTEUR_USE_SCHANNEL
		class TcpWSSWebSocketConnector : public RR_ENABLE_SHARED_FROM_THIS < TcpWSSWebSocketConnector >
		{
		protected:
			RR_SHARED_PTR<TcpTransport> parent;
			uint32_t endpoint;
			std::string url;
			std::string ws_url;
			std::string servername;
			RR_WEAK_PTR<RobotRaconteurNode> node;

		protected:
			
			void Connect4(RR_SHARED_PTR<RobotRaconteurException> err,
				RR_SHARED_PTR<ITransportConnection> connection,
				RR_SHARED_PTR<boost::asio::ip::tcp::socket> socket,
				RR_SHARED_PTR<TlsSchannelAsyncStreamAdapter> tls_stream,
				RR_SHARED_PTR<websocket_stream<TlsSchannelAsyncStreamAdapter_ASIO_adapter&> > websocket,
				boost::function<void(RR_SHARED_PTR<ITransportConnection>, RR_SHARED_PTR<RobotRaconteurException>) > handler
			);
			
			void Connect3(const boost::system::error_code& ec,
				RR_SHARED_PTR<boost::asio::ip::tcp::socket> socket,
				RR_SHARED_PTR<boost::signals2::scoped_connection> socket_closer,
				RR_SHARED_PTR<TlsSchannelAsyncStreamAdapter> tls_stream,
				RR_SHARED_PTR<websocket_stream<TlsSchannelAsyncStreamAdapter_ASIO_adapter&> > websocket,
				boost::function<void(RR_SHARED_PTR<ITransportConnection>, RR_SHARED_PTR<RobotRaconteurException>) > handler
			);
			
			void Connect2_1(const boost::system::error_code& ec,
				RR_SHARED_PTR<boost::asio::ip::tcp::socket> socket,
				RR_SHARED_PTR<boost::signals2::scoped_connection> socket_closer,
				RR_SHARED_PTR<TlsSchannelAsyncStreamAdapter> tls_stream,
				boost::function<void(RR_SHARED_PTR<ITransportConnection>, RR_SHARED_PTR<RobotRaconteurException>) > handler);
			
			void Connect2(const boost::system::error_code& ec,
				RR_SHARED_PTR<boost::asio::ip::tcp::socket> socket,
				RR_SHARED_PTR<websocket_tcp_connector> socket_connector,
				boost::function<void(RR_SHARED_PTR<ITransportConnection>, RR_SHARED_PTR<RobotRaconteurException>) > handler);
			
		public:
			
			TcpWSSWebSocketConnector(RR_SHARED_PTR<TcpTransport> parent);
			
			void Connect(std::string url, uint32_t endpoint, boost::function<void(RR_SHARED_PTR<ITransportConnection>, RR_SHARED_PTR<RobotRaconteurException>) > handler);
			
		};
#endif


#ifdef ROBOTRACONTEUR_USE_OPENSSL
		class TcpWSSWebSocketConnector : public RR_ENABLE_SHARED_FROM_THIS < TcpWSSWebSocketConnector >
		{
		protected:
			RR_SHARED_PTR<TcpTransport> parent;
			uint32_t endpoint;
			std::string url;
			std::string ws_url;
			std::string servername;
			RR_SHARED_PTR<boost::asio::ssl::context> context;

			RR_WEAK_PTR<RobotRaconteurNode> node;

		protected:

			void Connect4(RR_SHARED_PTR<RobotRaconteurException> err,
				RR_SHARED_PTR<ITransportConnection> connection,
				RR_SHARED_PTR<boost::asio::ip::tcp::socket> socket,
				RR_SHARED_PTR<boost::asio::ssl::stream<boost::asio::ip::tcp::socket&> > tls_stream,
				RR_SHARED_PTR<websocket_stream<boost::asio::ssl::stream<boost::asio::ip::tcp::socket&> &> > websocket,
				boost::function<void(RR_SHARED_PTR<ITransportConnection>, RR_SHARED_PTR<RobotRaconteurException>) > handler
			);
			
			void Connect3(const boost::system::error_code& ec,
				RR_SHARED_PTR<boost::asio::ip::tcp::socket> socket,
				RR_SHARED_PTR<boost::signals2::scoped_connection> socket_closer,
				RR_SHARED_PTR<boost::asio::ssl::stream<boost::asio::ip::tcp::socket&> > tls_stream,
				RR_SHARED_PTR<websocket_stream<boost::asio::ssl::stream<boost::asio::ip::tcp::socket&> &> > websocket,
				boost::function<void(RR_SHARED_PTR<ITransportConnection>, RR_SHARED_PTR<RobotRaconteurException>) > handler
			);
			
			void Connect2_1(const boost::system::error_code& ec,
				RR_SHARED_PTR<boost::asio::ip::tcp::socket> socket,
				RR_SHARED_PTR<boost::signals2::scoped_connection> socket_closer,
				RR_SHARED_PTR<boost::asio::ssl::stream<boost::asio::ip::tcp::socket&> > tls_stream,
				boost::function<void(RR_SHARED_PTR<ITransportConnection>, RR_SHARED_PTR<RobotRaconteurException>) > handler);
			
#ifdef ROBOTRACONTEUR_APPLE
			static bool verify_callback(bool preverified, boost::asio::ssl::verify_context& ctx, std::string servername);
#endif
			void Connect2(const boost::system::error_code& ec,
				RR_SHARED_PTR<boost::asio::ip::tcp::socket> socket,
				RR_SHARED_PTR<websocket_tcp_connector> socket_connector,
				boost::function<void(RR_SHARED_PTR<ITransportConnection>, RR_SHARED_PTR<RobotRaconteurException>) > handler);
		
		public:

			TcpWSSWebSocketConnector(RR_SHARED_PTR<TcpTransport> parent);
			
			void Connect(std::string url, uint32_t endpoint, boost::function<void(RR_SHARED_PTR<ITransportConnection>, RR_SHARED_PTR<RobotRaconteurException>) > handler);
		};
#endif


	}

	namespace detail
	{
		class IPNodeDiscovery : public RR_ENABLE_SHARED_FROM_THIS<IPNodeDiscovery>
		{
		public:


			IPNodeDiscovery(RR_SHARED_PTR<TcpTransport> parent);

			IPNodeDiscovery(RR_SHARED_PTR<RobotRaconteurNode> node);

			/// <summary>
			/// Starts listening for nodes
			/// </summary>
			/// <param name="flags">The IPv6 broadcost depth.  This should normally be all value "or'ed" together</param>
			void StartListeningForNodes(uint32_t flags);


			/// <summary>
			/// Disables the node auto discovery listening
			/// </summary>
			void StopListeningForNodes();

			/// <summary>
			/// Starts broadcasting node announce packets so clients can find this node
			/// </summary>
			/// <param name="flags">The IPv6 broadcost depth.  This should normally be all value "or'ed" together</param>
			void StartAnnouncingNode(uint32_t flags);

			/// <summary>
			/// Disables the node announcing
			/// </summary>
			void StopAnnouncingNode();

			void SendAnnounceNow();
			void SendDiscoveryRequestNow();

			int32_t GetNodeAnnouncePeriod();
			void SetNodeAnnouncePeriod(int32_t millis);

			RR_SHARED_PTR<RobotRaconteurNode> GetNode();

		private:

			static const int32_t ANNOUNCE_PORT;

			bool listening;
			bool broadcasting;
			int32_t broadcast_flags;
			int32_t listen_flags;

			int32_t listen_socket_flags;
			
			RR_SHARED_PTR<boost::asio::ip::udp::socket> ip4_listen;
			std::list<RR_SHARED_PTR<boost::asio::ip::udp::socket> > ip6_listen;
			std::list<long> ip6_listen_scope_ids;


			RR_SHARED_PTR<boost::asio::deadline_timer> broadcast_timer;

			RR_SHARED_PTR<boost::asio::deadline_timer> receive_update_timer;

			RR_SHARED_PTR<boost::asio::deadline_timer> discovery_request_timer;

			void start_listen_sockets();

			void start_ipv6_listen_socket(boost::asio::ip::udp::endpoint ep);

			void handle_receive(const boost::system::error_code& error, std::size_t bytes_transferred, RR_SHARED_PTR<boost::asio::ip::udp::socket> socket, RR_SHARED_PTR<boost::asio::ip::udp::endpoint> ep, boost::shared_array<uint8_t> buffer);

			void handle_receive_update_timer(const boost::system::error_code& error);

			void handle_broadcast_timer(const boost::system::error_code& error);

			void handle_send( const boost::system::error_code& /*error*/, std::size_t /*bytes_transferred*/, RR_SHARED_PTR<std::string> /*message*/);

			void broadcast_discovery_packet(const boost::asio::ip::address& source, const std::string& packet, IPNodeDiscoveryFlags flags);

			std::string generate_response_packet(const boost::asio::ip::address& source, const std::string& scheme, int port);

			void handle_request_timer(const boost::system::error_code& error, int32_t c);
						
			void NodeAnnounceReceived(const std::string &packet);

			boost::mutex change_lock;

			RR_WEAK_PTR<TcpTransport> parent;

			RR_WEAK_PTR<RobotRaconteurNode> node;

			template <typename SocketType>
			void Ip6AddMembership(SocketType& socket, const boost::asio::ip::address_v6& addr, long scope_id)
			{
				ipv6_mreq mreq;
				boost::asio::ip::address_v6::bytes_type b=addr.to_bytes();
				memcpy(&mreq.ipv6mr_multiaddr,&b[0],sizeof(in6_addr));
				mreq.ipv6mr_interface=scope_id;
				setsockopt(socket.native_handle(),IPPROTO_IPV6,IPV6_ADD_MEMBERSHIP,(char*)&mreq,sizeof(mreq));

			}

			int32_t broadcast_timer_period;
			boost::posix_time::ptime last_request_send_time;
			NodeID this_request_id;
		};

		class TcpTransportPortSharerClient : public RR_ENABLE_SHARED_FROM_THIS < TcpTransportPortSharerClient >
		{
		protected:
			RR_WEAK_PTR<TcpTransport> parent;
			boost::mutex this_lock;
			bool open;
#ifdef ROBOTRACONTEUR_WINDOWS
			RR_SHARED_PTR<boost::asio::windows::stream_handle> localsocket;
#else
			RR_SHARED_PTR<boost::asio::local::stream_protocol::socket> localsocket;			
#endif
			RR_SHARED_PTR<AutoResetEvent> delay_event;
			uint32_t port;
			bool sharer_connected;

		public:

			TcpTransportPortSharerClient(RR_SHARED_PTR<TcpTransport> parent);
			

			void Start();
			int32_t GetListenPort();
			void Close();
			RR_SHARED_PTR<TcpTransport> GetParent();
			bool IsPortSharerConnected();

		protected:

			void client_thread();

			void IncomingSocket(RR_SHARED_PTR<boost::asio::ip::tcp::socket> socket);

		};
#ifndef ROBOTRACONTEUR_WINDOWS
		
		namespace TcpTransportUtil
		{
			ssize_t read_fd(int fd, void *ptr, size_t nbytes, int *recvfd);
		}
		
		
#endif
		
	}


}
