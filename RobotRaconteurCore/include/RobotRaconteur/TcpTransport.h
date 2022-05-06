/**
 * @file TcpTransport.h
 *
 * @author John Wason, PhD
 *
 * @copyright Copyright 2011-2020 Wason Technology, LLC
 *
 * @par License
 * Software License Agreement (Apache License)
 * @par
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * @par
 * http://www.apache.org/licenses/LICENSE-2.0
 * @par
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "RobotRaconteur/RobotRaconteurNode.h"
#include <boost/shared_array.hpp>

#pragma once

namespace RobotRaconteur
{
/**
 * @brief Configuration flags for IP network node discovery
 *
 */
enum IPNodeDiscoveryFlags
{
    /** @brief Use IPv6 FF01:: node local  multicast addresses */
    IPNodeDiscoveryFlags_NODE_LOCAL = 0x1,
    /** @brief Use IPv6 FF02:: link local  multicast addresses */
    IPNodeDiscoveryFlags_LINK_LOCAL = 0x2,
    /** @brief Use IPv6 FF05:: site local  multicast addresses */
    IPNodeDiscoveryFlags_SITE_LOCAL = 0x4,
    /** @brief Use IPv4 255.255.255.255 broadcast addresses */
    IPNodeDiscoveryFlags_IPV4_BROADCAST = 0x8
};

class TcpTransportConnection;
namespace detail
{
class TcpConnector;
class TcpWebSocketConnector;
class TcpWSSWebSocketConnector;
} // namespace detail

/**
 * @brief Transport for Transport Control Protocol Internet Protocol (TCP/IP) networks
 *
 * It is recommended that ClientNodeSetup, ServerNodeSetup, or SecureServerNodeSetup
 * be used to construct this class.
 *
 * See \ref robotraconteur_url for more information on URLs.
 *
 * The TcpTransport implements transport connections over TCP/IP networks. TCP/IP is the
 * most common protocol used for Internet and Local Area Network (LAN) communication, including
 * Ethernet and WiFi connections. The Transport Control Protocol (TCP) is a reliable stream
 * protocol that establishes connections between devices using IP address and port pairs.
 * Each adapter has an assigned address, and applications create connections on different ports.
 * TcpTransport listens to the port specified in StartServer(), and the client uses
 * a URL containing the IP address and port of the listening transport. The TcpTransport
 * uses the established connection to pass messages between nodes.
 *
 * The IP protocol is available in two major versions, IPv4 and IPv6. The most common
 * is IPv4, and its 32 bit address is typically written as four numbers,
 * ie 172.17.12.174. IPv4 has a number of critical limitations, the greatest being
 * its 2^32 address limit (approximately 4 billion). This is a problem when there are
 * tens of billions of internet connected devices already present. IPv6 introduces a 128
 * bit address space, which allows for approximately 3.4×10^38 possible addresses. The major
 * advantage for Robot Raconteur is the introduction of "link-local" addresses. These addresses
 * begin with "FE80::" and finish with an "EUI-64" address, which is tied to the MAC address
 * of the adaptor. IPv4 addresses need to be assigned to devices locally, and have a tendency
 * to change. IPv6 addresses are permanently assigned to the adapter itself, meaning that
 * network configuration for LAN communication is essentially automatic. Robot Raconteur
 * will prefer IPv6 connections when possible for this reason.
 *
 * The TcpTransport is capable of using "raw" streams that implement the Robot Raconteur
 * message protocols, or to use HTTP WebSockets. HTTP WebSockets allow Robot Raconteur
 * to communicate seamlessly with browsers and HTTP servers without requiring
 * additional plugins. WebSockets provide additional security using "origins". See
 * AddWebSocketAllowedOrigin() for more information.
 *
 * The TcpTransport supports TLS encryption using certificates. See \ref tls_security for
 * more information on TLS. The TcpTransport supports four modes of TLS encryption:
 *
 * | Scheme | Description | Direction |
 * | ---    | ---         | --- |
 * | rrs+tcp | "Raw" protocol with TLS | Both |
 * | rr+wss | Websocket over HTTPS | Client Only |
 * | rrs+ws | Websocket with RobotRaconteur TLS over HTTP | Both |
 * | rrs+wss | Websocket with RobotRaconteur TLS over HTTPS | Client Only |
 *
 * The different combinations of TLS and HTTPS for websockets are used for different scenarios.
 * Robot Raconteur Core can initiate HTTPS connections, but cannot accept them. Accepting
 * HTTPS connections requires a certificate issued by an authority like GoDaddy or Digicert,
 * and is typically used with an HTTP server running RobotRaconteurWeb.
 *
 * TLS certificates for Robot Raconteur nodes are issued by Wason Technology, LLC using
 * a root certificate that is "burned in" to Robot Raconteur Core. All devices running
 * Robot Raconteur will support this certificate chain.
 *
 * Discovery for the TcpTransport is accomplished using User Defined Protocol (UDP) multicast
 * and/or broadcast packets. Broadcast packets are sent to all connected devices, while
 * multicast is sent to devices that have registered to receive them. Unlike TCP, the packets
 * sent to broadcast or multicast are sent to the entire network. This allows for devices
 * to find each other on the network.
 *
 * For IPv4, the broadcast address 255.255.255.255 on port 48653
 * is used for discovery. By default, IPv4 is disabled in favor of IPv6. IPv6 uses the multicast
 * following multicast addresses:
 *
 * | Address | Scope | Port | Default? |
 * | ---     | ---  | --- | --- |
 * | FF01::BA86 | Node-Local | 48653 | Disabled |
 * | FF02::BA86 | Link-Local | 48653 | Enabled |
 * | FF05::BA86 | Site-Local | 48653 | Disabled |
 *
 * By default, discovery will only occur on the link-local IPv6 scope. This will
 * find nodes on the local subnet, but will not attempt to pass through any routers.
 *
 */
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

    /**
     * @brief Construct a new TcpTransport
     *
     * Must use boost::make_shared<TcpTransport>()
     *
     * The use of RobotRaconteurNodeSetup and subclasses is recommended to construct
     * transports.
     *
     * The transport must be registered with the node using
     * RobotRaconteurNode::RegisterTransport() after construction.
     *
     * @param node The node that will use the transport. Default is the singleton node
     */
    TcpTransport(RR_SHARED_PTR<RobotRaconteurNode> node = RobotRaconteurNode::sp());

    virtual ~TcpTransport();

  public:
    virtual bool IsServer() const;
    virtual bool IsClient() const;

    /**
     * @brief Get the default receive timeout
     *
     * If no messages are received within the timeout, the connection
     * is assumed to be lost.
     *
     * Default: 15 seconds
     *
     * @return int32_t The timeout in milliseconds
     */
    virtual int32_t GetDefaultReceiveTimeout();

    /**
     * @brief Set the default receive timeout
     *
     * If no messages are received within the timeout, the connection
     * is assumed to be lost.
     *
     * Default: 15 seconds
     *
     * @param milliseconds The timeout in milliseconds
     */
    virtual void SetDefaultReceiveTimeout(int32_t milliseconds);

    /**
     * @brief Get the default connect timeout
     *
     * If the connection is not completed within the timeout, the connection
     * attempt will be aborted.
     *
     * @return int32_t The timeout in milliseconds
     */
    virtual int32_t GetDefaultConnectTimeout();

    /**
     * @brief Set the default connect timeout
     *
     * If the connection is not completed within the timeout, the connection
     * attempt will be aborted.
     *
     * Default: 5 seconds
     *
     * @param milliseconds The timeout in milliseconds
     */
    virtual void SetDefaultConnectTimeout(int32_t milliseconds);

    virtual std::string GetUrlSchemeString() const;

    virtual int32_t GetListenPort();

    virtual void SendMessage(RR_INTRUSIVE_PTR<Message> m);

    virtual void AsyncSendMessage(RR_INTRUSIVE_PTR<Message> m,
                                  boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)>& callback);

    virtual void AsyncCreateTransportConnection(
        boost::string_ref url, RR_SHARED_PTR<Endpoint> e,
        boost::function<void(RR_SHARED_PTR<ITransportConnection>, RR_SHARED_PTR<RobotRaconteurException>)>& callback);

    virtual RR_SHARED_PTR<ITransportConnection> CreateTransportConnection(boost::string_ref url,
                                                                          RR_SHARED_PTR<Endpoint> e);

    virtual void CloseTransportConnection(RR_SHARED_PTR<Endpoint> e);

  protected:
    virtual void CloseTransportConnection_timed(const boost::system::error_code& err, RR_SHARED_PTR<Endpoint> e,
                                                RR_SHARED_PTR<void> timer);

  public:
    /**
     * @brief Start the server on the specified TCP port
     *
     * The official Robot Raconteur port 48653 is reserved
     * for the port sharer
     *
     * @param porte The port to listen on
     */
    virtual void StartServer(int32_t porte);

    /**
     * @brief Start the server using the TCP port sharer
     *
     * The TCP port sharer is a utility program that accepts incoming
     * connections on the official Robot Raconteur port 48653 and forwards
     * to nodes based on the NodeID and/or NodeName requested by the client
     *
     */
    virtual void StartServerUsingPortSharer();

    /**
     * @brief Check if the port sharer is running
     *
     * @return true The port sharer is running and connected
     * @return false The port sharer is not running or is not connected
     */
    virtual bool IsPortSharerRunning();

    virtual bool CanConnectService(boost::string_ref url);

    virtual void Close();

    virtual void CheckConnection(uint32_t endpoint);

    /**
     * @brief Enable node discovery listening
     *
     * By default enables listining on IPv6 link-local scope
     *
     * @param flags The flags specifying the scope
     */
    void EnableNodeDiscoveryListening(uint32_t flags = (IPNodeDiscoveryFlags_LINK_LOCAL));

    /**
     * @brief Disable node discovery listening
     *
     */
    void DisableNodeDiscoveryListening();

    /**
     * @brief Enable node discovery announce
     *
     * By default enables announce on IPv6 link-local scope
     *
     * @param flags The flags specifying the scope
     */
    void EnableNodeAnnounce(uint32_t flags = (IPNodeDiscoveryFlags_LINK_LOCAL));

    /**
     * @brief Disable node discovery announce
     *
     */
    void DisableNodeAnnounce();

    /**
     * @brief Get the period between node announce
     *
     * Default 55 seconds
     *
     * @return int32_t The period in milliseconds
     */
    int32_t GetNodeAnnouncePeriod();

    /**
     * @brief Set the period between node announce
     *
     * Default 55 seconds
     *
     * @param millis The period in milliseconds
     */
    void SetNodeAnnouncePeriod(int32_t millis);

    void SendDiscoveryRequest();

    virtual void PeriodicCleanupTask();

    uint32_t TransportCapability(boost::string_ref name);

    static void GetLocalAdapterIPAddresses(std::vector<boost::asio::ip::address>& addresses);

    virtual void MessageReceived(RR_INTRUSIVE_PTR<Message> m);

    /**
     * @brief Get the default heartbeat period
     *
     * The heartbeat is used to keep the connection alive
     * if no communication is occuring between nodes.
     *
     * Default: 5 seconds
     *
     * @return int32_t The period in milliseconds
     */
    virtual int32_t GetDefaultHeartbeatPeriod();
    /**
     * @brief Set the default heartbeat period
     *
     * The heartbeat is used to keep the connection alive
     * if no communication is occuring between nodes.
     *
     * Default: 5 seconds
     *
     * @param milliseconds The heartbeat in milliseconds
     */
    virtual void SetDefaultHeartbeatPeriod(int32_t milliseconds);

    /**
     * @brief Get the maximum serialized message size
     *
     * Default: 10 MB
     *
     * @return int32_t The size in bytes
     */
    virtual int32_t GetMaxMessageSize();

    /**
     * @brief Set the maximum serialized message size
     *
     * Default: 10 MB
     *
     * @param size The size in bytes
     */
    virtual void SetMaxMessageSize(int32_t size);

    /**
     * @brief Get the maximum number of concurrent connections
     *
     * Default: 0 for unlimited
     *
     * @return int32_t The number of connections
     */
    virtual int32_t GetMaxConnectionCount();
    /**
     * @brief Set the maximum number of concurrent connections
     *
     * Default: 0 for unlimited
     *
     * @param count The number of connections
     */
    virtual void SetMaxConnectionCount(int32_t count);

    /**
     * @brief Get if all connections require TLS
     *
     * @return true All connections require TLS
     * @return false Unencrypted connections allowed
     */
    virtual bool GetRequireTls();

    /**
     * @brief Set if all connections require TLS
     *
     * @param require_tls If true, all connections require TLS
     */
    virtual void SetRequireTls(bool require_tls);

    /**
     * @brief Load the TLS certificate from the system
     *
     * The TLS certificate is stored on the filesystem. See \ref tls_security
     * for more information on storage location.
     *
     */
    virtual void LoadTlsNodeCertificate();

    /**
     * @brief Check if TLS certificate is loaded
     *
     * @return true The TLS certificate has been loaded
     * @return false No TLS certificate is loaded
     */
    virtual bool IsTlsNodeCertificateLoaded();

    /**
     * @brief Check if specified endpoint is using TLS for its transport
     *
     * Throws ConnectionException if the endpoint is invalid or the connection
     * is not using TcpTransport
     *
     * @param endpoint The endpoint to check
     * @return true The connection is using TLS
     * @return false The connection is not using TLS
     */
    virtual bool IsTransportConnectionSecure(uint32_t endpoint);

    /** @copydoc IsTransportConnectionSecure(uint32_t) */
    virtual bool IsTransportConnectionSecure(RR_SHARED_PTR<Endpoint> endpoint);

    /** @copydoc IsTransportConnectionSecure(uint32_t) */
    virtual bool IsTransportConnectionSecure(RR_SHARED_PTR<RRObject> obj);

    /** @copydoc IsTransportConnectionSecure(uint32_t) */
    virtual bool IsTransportConnectionSecure(RR_SHARED_PTR<ITransportConnection> transport);

    /**
     * @brief Check if specified endpoint peer is using TLS and has been
     * verified using a certificate
     *
     * Checks if the peer server node has a valid certificate, or if the peer client
     * has been verified using mutual authentication.
     *
     * Throws ConnectionException if the endpoint is invalid or the connection
     * is not using TcpTransport
     *
     * @param endpoint The endpoint to check
     * @return true The peer node has been verified with a certificate
     * @return false The peer node has not been verified
     */
    virtual bool IsSecurePeerIdentityVerified(uint32_t endpoint);

    /** @copydoc IsSecurePeerIdentityVerified(uint32_t endpoint)*/
    virtual bool IsSecurePeerIdentityVerified(RR_SHARED_PTR<Endpoint> endpoint);
    /** @copydoc IsSecurePeerIdentityVerified(uint32_t endpoint)*/
    virtual bool IsSecurePeerIdentityVerified(RR_SHARED_PTR<RRObject> obj);
    /** @copydoc IsSecurePeerIdentityVerified(uint32_t endpoint)*/
    virtual bool IsSecurePeerIdentityVerified(RR_SHARED_PTR<ITransportConnection> tranpsort);

    /**
     * @brief Get the identity of the peer if secured using TLS
     *
     * Get the identity of a peer verified using TLS certificates. Returns a NodeID in string
     * format. Will throw AuthenticationException if the peer has not been verified.
     *
     * @param endpoint The endpoint to check
     * @return std::string The verified peer NodeID as a string
     */
    virtual std::string GetSecurePeerIdentity(uint32_t endpoint);

    /** @copydoc GetSecurePeerIdentity(uint32_t) */
    virtual std::string GetSecurePeerIdentity(RR_SHARED_PTR<Endpoint> endpoint);
    /** @copydoc GetSecurePeerIdentity(uint32_t) */
    virtual std::string GetSecurePeerIdentity(RR_SHARED_PTR<RRObject> obj);
    /** @copydoc GetSecurePeerIdentity(uint32_t) */
    virtual std::string GetSecurePeerIdentity(RR_SHARED_PTR<ITransportConnection> transport);

    /**
     * @brief Get if the transport will accept incoming HTTP WebSocket connections
     *
     * Default: true
     *
     * @return true The tranport will accept WebSocket connections
     * @return false The transport will reject WebSocket connections
     */
    virtual bool GetAcceptWebSockets();

    /**
     * @brief Set if the transport will accept incoming HTTP websocket connections
     *
     * Default: true
     *
     * @param value If true, the transport will accept incoming WebSocket connections
     */
    virtual void SetAcceptWebSockets(bool value);

    /**
     * @brief Get the currently configured WebSocket origins
     *
     * See AddWebSocketAllowedOrigin()
     *
     * @return std::vector<std::string> The currently configured WebSocket origins
     */
    virtual std::vector<std::string> GetWebSocketAllowedOrigins();

    /**
     * @brief Add a WebSocket allowed origin
     *
     * WebSockets are vulnerable to an attack method called "cross-site scripting" (XSS). In
     * XSS, a malicious website will attempt to create a connection to an arbitrary website or local
     * device and attempt to hack or otherwise interact with it. WebSockets protect against this attack
     * using an "Origin" HTTP header field specified in the header upon connection. This header
     * field contains the domain name, the IP address, and/or the port of the web page that is
     * attempting to initiate the connection. This header field is filled by the browser, and cannot
     * be modified by the web page. If the web page is loaded from the local filesystem, the origin
     * will be the empty string ("") or null ("null"). The Robot Raconteur Core library uses the
     * empty string origin when initiating WebSocket connections. By default, it accepts
     * the following origins:
     *
     * * (empty string)
     * * "null"
     * * "file://"
     * * "http://robotraconteur.com"
     * * "http://robotraconteur.com:80"
     * * "http://*.robotraconteur.com"
     * * "http://*.robotraconteur.com:80"
     * * "https://robotraconteur.com"
     * * "https://robotraconteur.com:443"
     * * "https://*.robotraconteur.com"
     * * "https://*.robotraconteur.com:443"
     *
     * The star symbol can be used for a subdomain wildcard when matching origins.
     *
     * Additional allowed origins can be added using this function, or the
     * `--robotraconteur-tcp-ws-add-origin=` command line option if
     * the RobotRaconteurNodeSetup classes are being used.
     *
     *
     * @param origin The origin to add
     */
    virtual void AddWebSocketAllowedOrigin(boost::string_ref origin);

    /**
     * @brief Remove a previously added WebSocket origin
     *
     * See AddWebSocketAllowedOrigin()
     *
     * @param origin The origin to remove
     */
    virtual void RemoveWebSocketAllowedOrigin(boost::string_ref origin);

    /**
     * @brief Get disable Message Format Version 4
     *
     * Message Format Version 2 will be used
     *
     * Default: Message V4 is enabled
     *
     * @return true Disable Message V4
     * @return false Enable Message V4
     */
    virtual bool GetDisableMessage4();

    /**
     * @brief Set disable Message Format Version 4
     *
     * Message Format Version 2 will be used
     *
     * Default: Message V4 is enabled
     *
     * @param d If true, Message V4 is disabled
     */
    virtual void SetDisableMessage4(bool d);

    /**
     * @brief Get disable string table
     *
     * Default: false
     *
     * RobotRaconteurNodeSetup and its subclasses
     * will disable the string table by default
     *
     * @return true Disable the string table
     * @return false String table is not disabled
     */
    virtual bool GetDisableStringTable();

    /**
     * @brief Set disable string table
     *
     * Default: false
     *
     * RobotRaconteurNodeSetup and its subclasses
     * will disable the string table by default
     *
     * @param d If true, string table is disabled
     */
    virtual void SetDisableStringTable(bool d);

    /**
     * @brief Get if async message io is disabled
     *
     * Async message io has better memory handling, at the
     * expense of slightly higher latency.
     *
     * Default: Async io enabled
     *
     * @return true Async message io is disabled
     * @return false Async message io is not disabled
     */
    virtual bool GetDisableAsyncMessageIO();

    /**
     * @brief Set if async message io is disabled
     *
     * Async message io has better memory handling, at the
     * expense of slightly higher latency.
     *
     * Default: Async io enabled
     *
     * @param d If true, async io is disabled
     */
    virtual void SetDisableAsyncMessageIO(bool d);

    template <typename T, typename F>
    boost::signals2::connection AddCloseListener(RR_SHARED_PTR<T> t, const F& f)
    {
        boost::mutex::scoped_lock lock(closed_lock);
        if (closed)
        {
            lock.unlock();
            boost::bind(f, t)();
            return boost::signals2::connection();
        }

        return close_signal.connect(boost::signals2::signal<void()>::slot_type(boost::bind(f, t.get())).track(t));
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

    static void handle_v4_accept(RR_SHARED_PTR<TcpTransport> parent,
                                 RR_SHARED_PTR<boost::asio::ip::tcp::acceptor> acceptor,
                                 RR_SHARED_PTR<boost::asio::ip::tcp::socket> socket,
                                 const boost::system::error_code& error);

    static void handle_v6_accept(RR_SHARED_PTR<TcpTransport> parent,
                                 RR_SHARED_PTR<boost::asio::ip::tcp::acceptor> acceptor,
                                 RR_SHARED_PTR<boost::asio::ip::tcp::socket> socket,
                                 const boost::system::error_code& error);

    virtual void register_transport(RR_SHARED_PTR<ITransportConnection> connection);
    virtual void erase_transport(RR_SHARED_PTR<ITransportConnection> connection);
    virtual void incoming_transport(RR_SHARED_PTR<ITransportConnection> connection);

    boost::mutex parameter_lock;
    int32_t heartbeat_period;
    int32_t default_connect_timeout;
    int32_t default_receive_timeout;
    int32_t max_message_size;
    int32_t max_connection_count;
    bool disable_message4;
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

#ifndef BOOST_NO_CXX11_TEMPLATE_ALIASES
/** @brief Convenience alias for TcpTransport shared_ptr */
using TcpTransportPtr = RR_SHARED_PTR<TcpTransport>;
#endif

} // namespace RobotRaconteur
