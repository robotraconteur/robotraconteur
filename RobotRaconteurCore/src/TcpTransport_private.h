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

#include "RobotRaconteur/TcpTransport.h"
#include "RobotRaconteur/ASIOStreamBaseTransport.h"
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

    TcpTransportConnection(const RR_SHARED_PTR<TcpTransport>& parent, boost::string_ref url, bool server,
                           uint32_t local_endpoint);

    void AsyncAttachSocket(const RR_SHARED_PTR<boost::asio::ip::tcp::socket>& socket,
                           const boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>&)>& callback);

    void AsyncAttachWebSocket(const RR_SHARED_PTR<boost::asio::ip::tcp::socket>& socket,
                              const RR_SHARED_PTR<detail::websocket_stream<boost::asio::ip::tcp::socket&> >& websocket,
                              const boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>&)>& callback);
#ifdef ROBOTRACONTEUR_USE_SCHANNEL
    void AsyncAttachWSSWebSocket(
        const RR_SHARED_PTR<boost::asio::ip::tcp::socket>& socket,
        const RR_SHARED_PTR<detail::TlsSchannelAsyncStreamAdapter>& wss_websocket_tls,
        const RR_SHARED_PTR<detail::websocket_stream<detail::TlsSchannelAsyncStreamAdapter_ASIO_adapter&> >&
            wss_websocket,
        const boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>&)>& callback);
#endif

#ifdef ROBOTRACONTEUR_USE_OPENSSL
    void AsyncAttachWSSWebSocket(
        const RR_SHARED_PTR<boost::asio::ip::tcp::socket>& socket,
        const RR_SHARED_PTR<boost::asio::ssl::stream<boost::asio::ip::tcp::socket&> >& wss_websocket_tls,
        const RR_SHARED_PTR<detail::websocket_stream<boost::asio::ssl::stream<boost::asio::ip::tcp::socket&>&> >&
            wss_websocket,
        const RR_SHARED_PTR<boost::asio::ssl::context>& wss_context,
        const boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>&)>& callback);
#endif

  protected:
    void do_starttls1(const std::string& noden, const boost::system::error_code& error,
                      const boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>&)>& callback);

    void do_starttls2(const RR_SHARED_PTR<RobotRaconteurException>& error,
                      const boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>&)>& callback);

    // void do_starttls3(const RR_SHARED_PTR<RobotRaconteurException>& error,
    // const boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>&)>& callback);

    void do_starttls4(const std::string& servername, const boost::system::error_code& error);

    void do_starttls5(const boost::system::error_code& error);

    void do_starttls5_1(const RR_SHARED_PTR<RRObject>& parameter, const RR_SHARED_PTR<RobotRaconteurException>& err,
                        const boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>&)>& callback);

    void do_starttls6(const boost::system::error_code& error, const RR_INTRUSIVE_PTR<Message>& request);

    void do_starttls7(const boost::system::error_code& error, const RR_INTRUSIVE_PTR<Message>& request);

    void do_starttls8(const RR_SHARED_PTR<RobotRaconteurException>& error, const RR_INTRUSIVE_PTR<Message>& request);

    void do_starttls9(const boost::system::error_code& error);

  public:
    RR_OVIRTUAL void MessageReceived(const RR_INTRUSIVE_PTR<Message>& m) RR_OVERRIDE;

  protected:
    RR_OVIRTUAL void async_write_some(
        const_buffers& b,
        const boost::function<void(const boost::system::error_code& error, size_t bytes_transferred)>& handler)
        RR_OVERRIDE;

    RR_OVIRTUAL void async_read_some(
        mutable_buffers& b,
        const boost::function<void(const boost::system::error_code& error, size_t bytes_transferred)>& handler)
        RR_OVERRIDE;

    RR_OVIRTUAL size_t available() RR_OVERRIDE;

    RR_OVIRTUAL bool IsLargeTransferAuthorized() RR_OVERRIDE;

  public:
    RR_OVIRTUAL void Close() RR_OVERRIDE;

  protected:
    void Close1(const boost::system::error_code& ec);

    bool IsClosed();

    void ForceClose();

  public:
    RR_OVIRTUAL uint32_t GetLocalEndpoint() RR_OVERRIDE;

    RR_OVIRTUAL uint32_t GetRemoteEndpoint() RR_OVERRIDE;

    RR_OVIRTUAL void CheckConnection(uint32_t endpoint) RR_OVERRIDE;

    virtual bool IsSecure();

    virtual bool IsSecurePeerIdentityVerified();

    virtual std::string GetSecurePeerIdentity();

    RR_OVIRTUAL RR_SHARED_PTR<Transport> GetTransport() RR_OVERRIDE;

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

    RR_OVIRTUAL void StreamOpMessageReceived(const RR_INTRUSIVE_PTR<Message>& m) RR_OVERRIDE;

    // Stuff to support TLS

    bool is_tls;
    bool require_tls;
    RR_SHARED_PTR<boost::asio::deadline_timer> starttls_timer;
    boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>&)> starttls_handler;

    bool tls_mutual_auth;
    bool tls_handshaking;

#ifdef ROBOTRACONTEUR_USE_SCHANNEL
    RR_SHARED_PTR<RobotRaconteur::detail::TlsSchannelAsyncStreamAdapter> tls_socket;
    RR_SHARED_PTR<RobotRaconteur::detail::TlsSchannelAsyncStreamAdapter> tls_websocket;
    RR_SHARED_PTR<detail::websocket_stream<RobotRaconteur::detail::TlsSchannelAsyncStreamAdapter_ASIO_adapter&> >
        wss_websocket;
    RR_SHARED_PTR<RobotRaconteur::detail::TlsSchannelAsyncStreamAdapter> wss_websocket_tls;
    RR_SHARED_PTR<RobotRaconteur::detail::TlsSchannelAsyncStreamAdapter> tls_wss_websocket;

#endif

#ifdef ROBOTRACONTEUR_USE_OPENSSL
    RR_SHARED_PTR<boost::asio::ssl::stream<boost::asio::ip::tcp::socket&> > tls_socket;
    RR_SHARED_PTR<detail::OpenSSLAuthContext> tls_context;
    RR_SHARED_PTR<boost::asio::ssl::stream<detail::websocket_stream<boost::asio::ip::tcp::socket&>&> > tls_websocket;
    RR_SHARED_PTR<detail::websocket_stream<boost::asio::ssl::stream<boost::asio::ip::tcp::socket&>&> > wss_websocket;
    RR_SHARED_PTR<boost::asio::ssl::stream<boost::asio::ip::tcp::socket&> > wss_websocket_tls;
    RR_SHARED_PTR<
        boost::asio::ssl::stream<detail::websocket_stream<boost::asio::ssl::stream<boost::asio::ip::tcp::socket&>&>&> >
        tls_wss_websocket;
    RR_SHARED_PTR<boost::asio::ssl::context> wss_context;
#endif

    RR_SHARED_PTR<detail::websocket_stream<boost::asio::ip::tcp::socket&> > websocket;
    bool use_websocket;
    bool use_wss_websocket;
};

void TcpTransport_attach_transport(
    const RR_SHARED_PTR<TcpTransport>& parent, const RR_SHARED_PTR<boost::asio::ip::tcp::socket>& socket,
    boost::string_ref url, bool server, uint32_t endpoint,
    boost::function<void(const RR_SHARED_PTR<boost::asio::ip::tcp::socket>&, const RR_SHARED_PTR<ITransportConnection>&,
                         const RR_SHARED_PTR<RobotRaconteurException>&)>& callback);

void TcpTransport_connected_callback2(const RR_SHARED_PTR<TcpTransport>& parent,
                                      const RR_SHARED_PTR<boost::asio::ip::tcp::socket>& socket,
                                      const RR_SHARED_PTR<ITransportConnection>& connection,
                                      const RR_SHARED_PTR<RobotRaconteurException>& err);

namespace detail
{

class TcpConnector : public RR_ENABLE_SHARED_FROM_THIS<TcpConnector>
{
  public:
    TcpConnector(const RR_SHARED_PTR<TcpTransport>& parent);

    void Connect(
        const std::vector<std::string>& url, uint32_t endpoint,
        boost::function<void(const RR_SHARED_PTR<ITransportConnection>&, const RR_SHARED_PTR<RobotRaconteurException>&)>
            handler);

  protected:
    // void attach_transport(const RR_SHARED_PTR<boost::asio::ip::tcp::socket>& socket, boost::function<void(
    // RR_SHARED_PTR<boost::asio::ip::tcp::socket> , RR_SHARED_PTR<SuperNodeTransportConnection> , myerr )>& callback);
#if BOOST_ASIO_VERSION < 101200
    void connect2(int32_t key, const boost::system::error_code& err,
                  boost::asio::ip::basic_resolver_iterator<boost::asio::ip::tcp> endpoint_iterator,
                  const boost::function<void(const RR_SHARED_PTR<TcpTransportConnection>&,
                                             const RR_SHARED_PTR<RobotRaconteurException>&)>& callback);
#else
    void connect2(int32_t key, const boost::system::error_code& err,
                  const boost::asio::ip::tcp::resolver::results_type& results,
                  const boost::function<void(const RR_SHARED_PTR<TcpTransportConnection>&,
                                             const RR_SHARED_PTR<RobotRaconteurException>&)>& callback);
#endif

    void connect3(const RR_SHARED_PTR<std::list<boost::asio::ip::tcp::endpoint> >& candidate_endpoints, int32_t key,
                  const boost::system::error_code& e);

    void connect4();

    void connected_callback(const RR_SHARED_PTR<boost::asio::ip::tcp::socket>& socket,
                            const RR_SHARED_PTR<boost::signals2::scoped_connection>& socket_closer, int32_t key,
                            const boost::system::error_code& error);

    void connected_callback2(const RR_SHARED_PTR<boost::asio::ip::tcp::socket>& socket, int32_t key,
                             const RR_SHARED_PTR<ITransportConnection>& connection,
                             const RR_SHARED_PTR<RobotRaconteurException>& err);

    void connect_timer_callback(const boost::system::error_code& e);

    RR_SHARED_PTR<TcpTransport> parent;

    RR_SHARED_PTR<boost::asio::deadline_timer> connect_timer;

    bool connecting;

    boost::mutex connecting_lock;

    boost::function<void(const RR_SHARED_PTR<ITransportConnection>&, const RR_SHARED_PTR<RobotRaconteurException>&)>
        callback;

    RR_SHARED_PTR<boost::asio::ip::tcp::resolver> _resolver;

    uint32_t endpoint;

    int32_t resolve_count;
    int32_t connect_count;

    std::list<int32_t> active;
    int32_t active_count;
    std::list<RR_SHARED_PTR<RobotRaconteurException> > errors;

    void handle_error(const int32_t& key, const boost::system::error_code& err);
    void handle_error(const int32_t& key, const RR_SHARED_PTR<RobotRaconteurException>& err);

    bool socket_connected;

    boost::mutex this_lock;
    std::string url;

    RR_SHARED_PTR<boost::asio::deadline_timer> backoff_timer;

    RR_WEAK_PTR<RobotRaconteurNode> node;

  public:
};

class TcpAcceptor : public RR_ENABLE_SHARED_FROM_THIS<TcpAcceptor>
{
  private:
    RR_SHARED_PTR<TcpTransport> parent;
    std::string url;
    uint32_t local_endpoint;

    boost::array<char, 4> buf;

    boost::posix_time::ptime start_time;

    RR_WEAK_PTR<RobotRaconteurNode> node;

  protected:
    void AcceptSocket6(const RR_SHARED_PTR<RobotRaconteurException>& err,
                       const RR_SHARED_PTR<boost::asio::ip::tcp::socket>& socket,
                       const RR_SHARED_PTR<TcpTransportConnection>& t,
                       const boost::function<void(const RR_SHARED_PTR<boost::asio::ip::tcp::socket>&,
                                                  const RR_SHARED_PTR<ITransportConnection>&,
                                                  const RR_SHARED_PTR<RobotRaconteurException>&)>& handler);

    void AcceptSocket5(const boost::system::error_code& ec, const RR_SHARED_PTR<boost::asio::ip::tcp::socket>& socket,
                       const RR_SHARED_PTR<websocket_stream<boost::asio::ip::tcp::socket&> >& websocket,
                       const RR_SHARED_PTR<boost::signals2::scoped_connection>& socket_closer,
                       const boost::function<void(const RR_SHARED_PTR<boost::asio::ip::tcp::socket>&,
                                                  const RR_SHARED_PTR<ITransportConnection>&,
                                                  const RR_SHARED_PTR<RobotRaconteurException>&)>& handler);

    static void AcceptSocket4(const RR_SHARED_PTR<std::string>& dat,
                              const RR_SHARED_PTR<boost::asio::ip::tcp::socket>& socket,
                              const RR_SHARED_PTR<boost::signals2::scoped_connection>& socket_closer);

    void AcceptSocket3(const boost::system::error_code& ec, const RR_SHARED_PTR<boost::asio::deadline_timer>& timer,
                       const RR_SHARED_PTR<boost::asio::ip::tcp::socket>& socket,
                       const RR_SHARED_PTR<boost::signals2::scoped_connection>& socket_closer,
                       const boost::function<void(const RR_SHARED_PTR<boost::asio::ip::tcp::socket>&,
                                                  const RR_SHARED_PTR<ITransportConnection>&,
                                                  const RR_SHARED_PTR<RobotRaconteurException>&)>& handler);

    void AcceptSocket2(const boost::system::error_code& ec, size_t n,
                       const RR_SHARED_PTR<boost::asio::ip::tcp::socket>& socket,
                       const RR_SHARED_PTR<boost::signals2::scoped_connection>& socket_closer,
                       const boost::function<void(const RR_SHARED_PTR<boost::asio::ip::tcp::socket>&,
                                                  const RR_SHARED_PTR<ITransportConnection>&,
                                                  const RR_SHARED_PTR<RobotRaconteurException>&)>& handler);

  public:
    TcpAcceptor(const RR_SHARED_PTR<TcpTransport>& parent, boost::string_ref url, uint32_t local_endpoint);

    void AcceptSocket(
        const RR_SHARED_PTR<boost::asio::ip::tcp::socket>& socket,
        boost::function<void(const RR_SHARED_PTR<boost::asio::ip::tcp::socket>&,
                             const RR_SHARED_PTR<ITransportConnection>&, const RR_SHARED_PTR<RobotRaconteurException>&)>
            handler);
};

class TcpWebSocketConnector : public RR_ENABLE_SHARED_FROM_THIS<TcpWebSocketConnector>
{
  protected:
    RR_SHARED_PTR<TcpTransport> parent;
    uint32_t endpoint;
    std::string url;
    std::string ws_url;
    RR_WEAK_PTR<RobotRaconteurNode> node;

    void Connect4(const RR_SHARED_PTR<RobotRaconteurException>& err,
                  const RR_SHARED_PTR<ITransportConnection>& connection,
                  const RR_SHARED_PTR<boost::asio::ip::tcp::socket>& socket,
                  const RR_SHARED_PTR<websocket_stream<boost::asio::ip::tcp::socket&> >& websocket,
                  const boost::function<void(const RR_SHARED_PTR<ITransportConnection>&,
                                             const RR_SHARED_PTR<RobotRaconteurException>&)>& handler);

    void Connect3(const boost::system::error_code& ec, const RR_SHARED_PTR<boost::asio::ip::tcp::socket>& socket,
                  const RR_SHARED_PTR<websocket_stream<boost::asio::ip::tcp::socket&> >& websocket,
                  const RR_SHARED_PTR<boost::signals2::scoped_connection>& socket_closer,
                  const boost::function<void(const RR_SHARED_PTR<ITransportConnection>&,
                                             const RR_SHARED_PTR<RobotRaconteurException>&)>& handler);

    void Connect2(const boost::system::error_code& ec, const RR_SHARED_PTR<boost::asio::ip::tcp::socket>& socket,
                  const RR_SHARED_PTR<websocket_tcp_connector>& socket_connector,
                  const boost::function<void(const RR_SHARED_PTR<ITransportConnection>&,
                                             const RR_SHARED_PTR<RobotRaconteurException>&)>& handler);

  public:
    TcpWebSocketConnector(const RR_SHARED_PTR<TcpTransport>& parent);

    void Connect(
        boost::string_ref url, uint32_t endpoint,
        boost::function<void(const RR_SHARED_PTR<ITransportConnection>&, const RR_SHARED_PTR<RobotRaconteurException>&)>
            handler);
};

#ifdef ROBOTRACONTEUR_USE_SCHANNEL
class TcpWSSWebSocketConnector : public RR_ENABLE_SHARED_FROM_THIS<TcpWSSWebSocketConnector>
{
  protected:
    RR_SHARED_PTR<TcpTransport> parent;
    uint32_t endpoint;
    std::string url;
    std::string ws_url;
    std::string servername;
    RR_WEAK_PTR<RobotRaconteurNode> node;

  protected:
    void Connect4(const RR_SHARED_PTR<RobotRaconteurException>& err,
                  const RR_SHARED_PTR<ITransportConnection>& connection,
                  const RR_SHARED_PTR<boost::asio::ip::tcp::socket>& socket,
                  const RR_SHARED_PTR<TlsSchannelAsyncStreamAdapter>& tls_stream,
                  RR_SHARED_PTR<websocket_stream<TlsSchannelAsyncStreamAdapter_ASIO_adapter&> > websocket,
                  const boost::function<void(const RR_SHARED_PTR<ITransportConnection>&,
                                             const RR_SHARED_PTR<RobotRaconteurException>&)>& handler);

    void Connect3(const boost::system::error_code& ec, const RR_SHARED_PTR<boost::asio::ip::tcp::socket>& socket,
                  const RR_SHARED_PTR<boost::signals2::scoped_connection>& socket_closer,
                  const RR_SHARED_PTR<TlsSchannelAsyncStreamAdapter>& tls_stream,
                  RR_SHARED_PTR<websocket_stream<TlsSchannelAsyncStreamAdapter_ASIO_adapter&> > websocket,
                  const boost::function<void(const RR_SHARED_PTR<ITransportConnection>&,
                                             const RR_SHARED_PTR<RobotRaconteurException>&)>& handler);

    void Connect2_1(const boost::system::error_code& ec, const RR_SHARED_PTR<boost::asio::ip::tcp::socket>& socket,
                    const RR_SHARED_PTR<boost::signals2::scoped_connection>& socket_closer,
                    const RR_SHARED_PTR<TlsSchannelAsyncStreamAdapter>& tls_stream,
                    const boost::function<void(const RR_SHARED_PTR<ITransportConnection>&,
                                               const RR_SHARED_PTR<RobotRaconteurException>&)>& handler);

    void Connect2(const boost::system::error_code& ec, const RR_SHARED_PTR<boost::asio::ip::tcp::socket>& socket,
                  const RR_SHARED_PTR<websocket_tcp_connector>& socket_connector,
                  const boost::function<void(const RR_SHARED_PTR<ITransportConnection>&,
                                             const RR_SHARED_PTR<RobotRaconteurException>&)>& handler);

  public:
    TcpWSSWebSocketConnector(const RR_SHARED_PTR<TcpTransport>& parent);

    void Connect(
        boost::string_ref url, uint32_t endpoint,
        boost::function<void(const RR_SHARED_PTR<ITransportConnection>&, const RR_SHARED_PTR<RobotRaconteurException>&)>
            handler);
};
#endif

#ifdef ROBOTRACONTEUR_USE_OPENSSL
class TcpWSSWebSocketConnector : public RR_ENABLE_SHARED_FROM_THIS<TcpWSSWebSocketConnector>
{
  protected:
    RR_SHARED_PTR<TcpTransport> parent;
    uint32_t endpoint;
    std::string url;
    std::string ws_url;
    std::string servername;
    RR_SHARED_PTR<boost::asio::ssl::context> context;

    RR_WEAK_PTR<RobotRaconteurNode> node;

    void Connect4(
        const RR_SHARED_PTR<RobotRaconteurException>& err, const RR_SHARED_PTR<ITransportConnection>& connection,
        const RR_SHARED_PTR<boost::asio::ip::tcp::socket>& socket,
        const RR_SHARED_PTR<boost::asio::ssl::stream<boost::asio::ip::tcp::socket&> >& tls_stream,
        const RR_SHARED_PTR<websocket_stream<boost::asio::ssl::stream<boost::asio::ip::tcp::socket&>&> >& websocket,
        const boost::function<void(const RR_SHARED_PTR<ITransportConnection>&,
                                   const RR_SHARED_PTR<RobotRaconteurException>&)>& handler);

    void Connect3(
        const boost::system::error_code& ec, const RR_SHARED_PTR<boost::asio::ip::tcp::socket>& socket,
        const RR_SHARED_PTR<boost::signals2::scoped_connection>& socket_closer,
        const RR_SHARED_PTR<boost::asio::ssl::stream<boost::asio::ip::tcp::socket&> >& tls_stream,
        const RR_SHARED_PTR<websocket_stream<boost::asio::ssl::stream<boost::asio::ip::tcp::socket&>&> >& websocket,
        const boost::function<void(const RR_SHARED_PTR<ITransportConnection>&,
                                   const RR_SHARED_PTR<RobotRaconteurException>&)>& handler);

    void Connect2_1(const boost::system::error_code& ec, const RR_SHARED_PTR<boost::asio::ip::tcp::socket>& socket,
                    const RR_SHARED_PTR<boost::signals2::scoped_connection>& socket_closer,
                    const RR_SHARED_PTR<boost::asio::ssl::stream<boost::asio::ip::tcp::socket&> >& tls_stream,
                    const boost::function<void(const RR_SHARED_PTR<ITransportConnection>&,
                                               const RR_SHARED_PTR<RobotRaconteurException>&)>& handler);

#ifdef ROBOTRACONTEUR_APPLE
    static bool verify_callback(bool preverified, boost::asio::ssl::verify_context& ctx, boost::string_ref servername);
#endif
    void Connect2(const boost::system::error_code& ec, const RR_SHARED_PTR<boost::asio::ip::tcp::socket>& socket,
                  const RR_SHARED_PTR<websocket_tcp_connector>& socket_connector,
                  const boost::function<void(const RR_SHARED_PTR<ITransportConnection>&,
                                             const RR_SHARED_PTR<RobotRaconteurException>&)>& handler);

  public:
    TcpWSSWebSocketConnector(const RR_SHARED_PTR<TcpTransport>& parent);

    void Connect(
        boost::string_ref url, uint32_t endpoint,
        boost::function<void(const RR_SHARED_PTR<ITransportConnection>&, const RR_SHARED_PTR<RobotRaconteurException>&)>
            handler);
};
#endif

} // namespace detail

namespace detail
{
class IPNodeDiscovery : public RR_ENABLE_SHARED_FROM_THIS<IPNodeDiscovery>
{
  public:
    IPNodeDiscovery(const RR_SHARED_PTR<TcpTransport>& parent);

    IPNodeDiscovery(const RR_SHARED_PTR<RobotRaconteurNode>& node);

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
    uint32_t broadcast_flags;
    uint32_t listen_flags;

    uint32_t listen_socket_flags;

    RR_SHARED_PTR<boost::asio::ip::udp::socket> ip4_listen;
    std::list<RR_SHARED_PTR<boost::asio::ip::udp::socket> > ip6_listen;
    std::list<unsigned long> ip6_listen_scope_ids;

    RR_SHARED_PTR<boost::asio::deadline_timer> broadcast_timer;

    RR_SHARED_PTR<boost::asio::deadline_timer> receive_update_timer;

    RR_SHARED_PTR<boost::asio::deadline_timer> discovery_request_timer;

    std::map<int32_t, RR_SHARED_PTR<boost::asio::deadline_timer> > backoff_timers;

    void start_listen_sockets();

    void start_listen_sockets2(int32_t key, const boost::system::error_code& ec);

    void start_ipv6_listen_socket(const boost::asio::ip::udp::endpoint& ep);

    void handle_receive(const boost::system::error_code& error, std::size_t bytes_transferred,
                        const RR_SHARED_PTR<boost::asio::ip::udp::socket>& socket,
                        const RR_SHARED_PTR<boost::asio::ip::udp::endpoint>& ep,
                        const boost::shared_array<uint8_t>& buffer);

    void handle_receive_update_timer(const boost::system::error_code& error);

    void handle_broadcast_timer(const boost::system::error_code& error);

    void handle_send(const boost::system::error_code& /*error*/, std::size_t /*bytes_transferred*/,
                     const RR_SHARED_PTR<std::string>& /*message*/);

    void broadcast_discovery_packet(const boost::asio::ip::address& source, boost::string_ref packet,
                                    IPNodeDiscoveryFlags flags);

    std::string generate_response_packet(const boost::asio::ip::address& source, boost::string_ref scheme, int port);

    void handle_request_timer(const boost::system::error_code& error, int32_t c);

    void NodeAnnounceReceived(boost::string_ref packet, const boost::asio::ip::udp::endpoint& send_ep);

    boost::mutex change_lock;

    RR_WEAK_PTR<TcpTransport> parent;

    RR_WEAK_PTR<RobotRaconteurNode> node;

    template <typename SocketType>
    void Ip6AddMembership(SocketType& socket, const boost::asio::ip::address_v6& addr, unsigned int scope_id)
    {
        ipv6_mreq mreq = {};
        boost::asio::ip::address_v6::bytes_type b = addr.to_bytes();
        memcpy(&mreq.ipv6mr_multiaddr, &b[0], sizeof(in6_addr));
        mreq.ipv6mr_interface = scope_id;
        setsockopt(socket.native_handle(), IPPROTO_IPV6, IPV6_ADD_MEMBERSHIP, reinterpret_cast<char*>(&mreq),
                   sizeof(mreq));
    }

    int32_t broadcast_timer_period;
    boost::posix_time::ptime last_request_send_time;
    NodeID this_request_id;
};

class TcpTransportPortSharerClient : public RR_ENABLE_SHARED_FROM_THIS<TcpTransportPortSharerClient>
{
  protected:
    RR_WEAK_PTR<TcpTransport> parent;
    RR_WEAK_PTR<RobotRaconteurNode> node;
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
    TcpTransportPortSharerClient(const RR_SHARED_PTR<TcpTransport>& parent);

    void Start();
    int32_t GetListenPort();
    void Close();
    RR_SHARED_PTR<TcpTransport> GetParent();
    bool IsPortSharerConnected();

  protected:
    void client_thread();

    void IncomingSocket(const RR_SHARED_PTR<boost::asio::ip::tcp::socket>& socket);
};
#ifndef ROBOTRACONTEUR_WINDOWS

namespace TcpTransportUtil
{
ssize_t read_fd(int fd, void* ptr, size_t nbytes, int* recvfd);
}

#endif

} // namespace detail

} // namespace RobotRaconteur
