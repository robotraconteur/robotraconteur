/**
 * @file LocalTransport.h
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
class ROBOTRACONTEUR_CORE_API LocalTransportConnection;

namespace detail
{
class LocalTransportFDs;
class LocalTransportDiscovery;
class LocalTransport_socket;
class LocalTransport_acceptor;
} // namespace detail

/**
 * @brief Transport for communication between processes using UNIX domain sockets
 *
 * It is recommended that ClientNodeSetup, ServerNodeSetup, or SecureServerNodeSetup
 * be used to construct this class.
 *
 * See \ref robotraconteur_url for more information on URLs.
 *
 * The LocalTransport implements transport connections between processes running on the
 * same host operating system using UNIX domain sockets. UNIX domain sockets are similar
 * to standard networking sockets, but are used when both peers are on the same machine
 * instead of connected through a network. This provides faster operation and greater
 * security, since the kernel simply passes data between the processes. UNIX domain
 * sockets work using Information Node (inode) files, which are special files on
 * the standard filesystem. Servers "listen" on a specified inode, and clients
 * use the inode as the address to connect. The LocalTransport uses UNIX sockets
 * in `SOCK_STREAM` mode. This provides a reliable stream transport connection similar
 * to TCP, but with significantly improved performance due the lower overhead.
 *
 * UNIX domain sockets were added to Windows 10 with the 1803 update. Robot Raconteur
 * switch to UNIX domain sockets for the LocalTransport on Windows in version 0.9.2.
 * Previous versions used Named Pipes, but these were inferior to UNIX sockets. The
 * LocalTransport will not function on versions of Windows prior to Windows 10 1803 update
 * due to the lack of support for UNIX sockets. A warning will be issued to the log if
 * the transport is not available, and all connection attempts will fail. All other
 * transports will continue to operate normally.
 *
 * The LocalTransport stores inode and node information files in the filesystem at various
 * operator system dependent locations. See the Robot Raconteur Standards documents
 * for details on where these files are stored.
 *
 * Discovery is implemented using file watchers. The file watchens must be activated
 * using the node setup flags, or by calling EnableNodeDiscoveryListening().
 * After being initialized the file watchers operate automatically.
 *
 * The LocalTransport can be used to dynamically assign NodeIDs to nodes based on a NodeName.
 * StartServerAsNodeName() and StartClientAsNodeName() take a NodeName that will identify the
 * node to clients, and manage a system-local NodeID corresponding to that NodeName. The
 * generated NodeIDs are stored on the local filesystem. If LocalTransport finds a corresponding
 * NodeID on the filesystem, it will load and use that NodeID. If it does not, a new random NodeID
 * is automatically generated.
 *
 * The server can be started in "public" or "private" mode. Private servers store their inode and
 * information in a location only the account owner can access, while "public" servers are
 * placed in a location that all users with the appropriate permissions can access. By default,
 * public LocalTransport servers are assigned to the "robotraconteur" group. Clients that belong to the
 * "robotraconteur" group will be able to connect to these public servers.
 *
 */
class ROBOTRACONTEUR_CORE_API LocalTransport : public Transport, public RR_ENABLE_SHARED_FROM_THIS<LocalTransport>
{
    friend class LocalTransportConnection;

  private:
    std::string socket_file_name;

    bool transportopen;

  public:
    RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<ITransportConnection> > TransportConnections;
    boost::mutex TransportConnections_lock;

    /**
     * @brief Construct a new LocalTransport
     *
     * Must use boost::make_shared<LocalTransport>()
     *
     * The use of RobotRaconteurNodeSetup and subclasses is recommended to construct
     * transports.
     *
     * The transport must be registered with the node using
     * RobotRaconteurNode::RegisterTransport() after construction.
     *
     * @param node The node that will use the transport. Default is the singleton node
     */
    LocalTransport(const RR_SHARED_PTR<RobotRaconteurNode>& node = RobotRaconteurNode::sp());

    RR_OVIRTUAL ~LocalTransport() RR_OVERRIDE;

    RR_OVIRTUAL bool IsServer() const RR_OVERRIDE;

    RR_OVIRTUAL bool IsClient() const RR_OVERRIDE;

    RR_OVIRTUAL std::string GetUrlSchemeString() const RR_OVERRIDE;

    RR_OVIRTUAL void SendMessage(const RR_INTRUSIVE_PTR<Message>& m) RR_OVERRIDE;

    RR_OVIRTUAL void AsyncSendMessage(
        const RR_INTRUSIVE_PTR<Message>& m,
        const boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>&)>& handler) RR_OVERRIDE;

    RR_OVIRTUAL void AsyncCreateTransportConnection(
        boost::string_ref url, const RR_SHARED_PTR<Endpoint>& e,
        boost::function<void(const RR_SHARED_PTR<ITransportConnection>&,
                             const RR_SHARED_PTR<RobotRaconteurException>&)>& callback) RR_OVERRIDE;

    RR_OVIRTUAL RR_SHARED_PTR<ITransportConnection> CreateTransportConnection(
        boost::string_ref url, const RR_SHARED_PTR<Endpoint>& e) RR_OVERRIDE;

    RR_OVIRTUAL void CloseTransportConnection(const RR_SHARED_PTR<Endpoint>& e) RR_OVERRIDE;

  protected:
    virtual void AsyncCreateTransportConnection2(
        const RR_SHARED_PTR<detail::LocalTransport_socket>& socket, const std::string& noden,
        const RR_SHARED_PTR<ITransportConnection>& transport, const RR_SHARED_PTR<RobotRaconteurException>& err,
        boost::function<void(const RR_SHARED_PTR<ITransportConnection>&,
                             const RR_SHARED_PTR<RobotRaconteurException>&)>& callback);

    virtual void CloseTransportConnection_timed(const boost::system::error_code& err, const RR_SHARED_PTR<Endpoint>& e,
                                                const RR_SHARED_PTR<void>& timer);

  public:
    /**
     * @brief Check if the LocalTransport is supported on the current
     * operating system.
     *
     * Windows versions before Windows 10 1803 do not support the LocalTransport
     * due to lack of UNIX sockets. All other transports will continue to operate
     * normally.
     *
     * All versions of Linux and Mac OSX support the LocalTransport
     *
     * LocalTransport on Android and iOS is not officially supported
     *
     * @return true LocalTransport is supported
     * @return false LocalTransport is not supported
     */
    static bool IsLocalTransportSupported();

    /**
     * @brief Initialize the LocalTransport by assigning a NodeID based on NodeName
     *
     * Assigns the specified name to be the NodeName of the node, and manages
     * a corresponding NodeID. See LocalTransport for more information.
     *
     * Throws NodeNameAlreadyInUse if another node is using name
     *
     * @param name The NodeName
     */
    virtual void StartClientAsNodeName(boost::string_ref name);

    /**
     * @brief Start the server using the specified NodeName and assigns a NodeID
     *
     * The LocalTransport will listen on a UNIX domain socket for incoming clients,
     * using information files and inodes on the local filesystem. Clients
     * can locate the node using the NodeID and/or NodeName. The NodeName is assigned
     * to the node, and the transport manages a corresponding NodeID. See
     * LocalTransport for more information.
     *
     * Throws NodeNameAlreadyInUse if another node is using name
     *
     * Throws NodeIDAlreadyInUse if another node is using the managed
     * NodeID
     *
     * @param name The NodeName
     * @param public_ If true, other users can access the server. If not, only
     * the account owner can access the server.
     */
    virtual void StartServerAsNodeName(boost::string_ref name, bool public_ = false);

    /**
     * @brief Start the server using the specified NodeID
     *
     * The LocalTransport will listen on a UNIX domain socket for incoming clients,
     * using information files and inodes on the local filesystem. This function
     * leaves the NodeName blank, so clients must use NodeID to identify the node.
     *
     * Throws NodeIDAlreadyInUse if another node is using nodeid
     *
     * @param nodeid The NodeID
     * @param public_ If true, other users can access the server. If not, only
     * the account owner can access the server.
     */
    virtual void StartServerAsNodeID(const NodeID& nodeid, bool public_ = false);

    RR_OVIRTUAL bool CanConnectService(boost::string_ref url) RR_OVERRIDE;

    RR_OVIRTUAL void Close() RR_OVERRIDE;

    RR_OVIRTUAL void CheckConnection(uint32_t endpoint) RR_OVERRIDE;

    RR_OVIRTUAL void PeriodicCleanupTask() RR_OVERRIDE;

    RR_OVIRTUAL uint32_t TransportCapability(boost::string_ref name) RR_OVERRIDE;

    RR_OVIRTUAL void MessageReceived(const RR_INTRUSIVE_PTR<Message>& m) RR_OVERRIDE;

    RR_OVIRTUAL void AsyncGetDetectedNodes(
        const std::vector<std::string>& schemes,
        const boost::function<void(const RR_SHARED_PTR<std::vector<NodeDiscoveryInfo> >&)>& handler,
        int32_t timeout = RR_TIMEOUT_INFINITE) RR_OVERRIDE;

    /** @copydoc TcpTransport::GetMaxMessageSize() */
    virtual int32_t GetMaxMessageSize();
    /** @copydoc TcpTransport::SetMaxMessageSize() */
    virtual void SetMaxMessageSize(int32_t size);

    /** @copydoc TcpTransport::GetDisableMessage4() */
    virtual bool GetDisableMessage4();
    /** @copydoc TcpTransport::SetDisableMessage4() */
    virtual void SetDisableMessage4(bool d);

    /** @copydoc TcpTransport::GetDisableStringTable() */
    virtual bool GetDisableStringTable();
    /** @copydoc TcpTransport::SetDisableStringTable() */
    virtual void SetDisableStringTable(bool d);

    /** @copydoc TcpTransport::GetDisableAsyncMessageIO() */
    virtual bool GetDisableAsyncMessageIO();
    /** @copydoc TcpTransport::SetDisableAsyncMessageIO() */
    virtual void SetDisableAsyncMessageIO(bool d);

    /**
     * @brief Enable node discovery listening
     *
     * Starts file watchers to detect LocalTransport servers
     *
     */
    virtual void EnableNodeDiscoveryListening();

    /**
     * @brief Disable node discovery listening
     *
     */
    virtual void DisableNodeDiscoveryListening();

    template <typename T, typename F>
    boost::signals2::connection AddCloseListener(const RR_SHARED_PTR<T>& t, const F& f)
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
    RR_OVIRTUAL void LocalNodeServicesChanged() RR_OVERRIDE;

    RR_SHARED_PTR<detail::LocalTransport_acceptor> acceptor;

    boost::mutex acceptor_lock;

    static void handle_accept(const RR_SHARED_PTR<LocalTransport>& parent,
                              const RR_SHARED_PTR<detail::LocalTransport_acceptor>& acceptor,
                              const RR_SHARED_PTR<detail::LocalTransport_socket>& socket,
                              const boost::system::error_code& error);

    virtual void register_transport(const RR_SHARED_PTR<ITransportConnection>& connection);
    virtual void erase_transport(const RR_SHARED_PTR<ITransportConnection>& connection);

    boost::mutex fds_lock;
    RR_SHARED_PTR<detail::LocalTransportFDs> fds;

    boost::mutex parameter_lock;
    int32_t max_message_size;
    bool disable_message4;
    bool disable_string_table;
    bool disable_async_message_io;

    RR_SHARED_PTR<detail::LocalTransportDiscovery> discovery;
    boost::mutex discovery_lock;

    bool closed;
    boost::mutex closed_lock;
    boost::signals2::signal<void()> close_signal;
};

/**
 * @brief Exception thrown if the NodeName is already in use
 *
 *
 */
class ROBOTRACONTEUR_CORE_API NodeNameAlreadyInUse : public std::runtime_error
{
  public:
    NodeNameAlreadyInUse() : runtime_error("Could not start server: NodeName already in use"){};
};

/**
 * @brief Exception thrown if the NodeID is already in use
 *
 */
class ROBOTRACONTEUR_CORE_API NodeIDAlreadyInUse : public std::runtime_error
{
  public:
    NodeIDAlreadyInUse() : runtime_error("Could not start server: NodeID already in use"){};
};

#ifndef BOOST_NO_CXX11_TEMPLATE_ALIASES
/** @brief Convenience alias for LocalTransport shared_ptr */
using LocalTransportPtr = RR_SHARED_PTR<LocalTransport>;
#endif
} // namespace RobotRaconteur
