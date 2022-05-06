/**
 * @file IntraTransport.h
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
class ROBOTRACONTEUR_CORE_API IntraTransportConnection;

/**
 * @brief Transport for intra-process communication
 *
 * It is recommended that ClientNodeSetup, ServerNodeSetup, or SecureServerNodeSetup
 * be used to construct this class.
 *
 * See \ref robotraconteur_url for more information on URLs.
 *
 * The IntraTransport implements transport connections between nodes running
 * within the same process. This is often true for simulation environments, where
 * there may be multiple simulated devices running within the simulation. The
 * IntraTransport uses a singleton to keep track of the different nodes running
 * in the same process, and to form connections. The singleton also implements
 * discovery updates. Because the nodes are all in the same process, it is
 * not necessary to serialize messages being passed between nodes. Instead,
 * the messages are handed off to the receiving node by the transport. This
 * is in effect a shallow copy of the data being transmitted, and pointers to the
 * original data are passed to the receiving node. **Because the data is passed as a pointer,
 * any modification by the receiver will also modify the sender's buffer. Use with care!**
 *
 */

class ROBOTRACONTEUR_CORE_API IntraTransport : public Transport, public RR_ENABLE_SHARED_FROM_THIS<IntraTransport>
{
    friend class IntraTransportConnection;

  private:
    static std::list<RR_WEAK_PTR<IntraTransport> > peer_transports;
    static boost::mutex peer_transports_lock;

    bool transportopen;
    bool is_server;

  public:
    RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<ITransportConnection> > TransportConnections;
    boost::mutex TransportConnections_lock;

    /**
     * @brief Construct a new IntraTransport
     *
     * Must use boost::make_shared<IntraTransport>()
     *
     * The use of RobotRaconteurNodeSetup and subclasses is recommended to construct
     * transports.
     *
     * The transport must be registered with the node using
     * RobotRaconteurNode::RegisterTransport() after construction.
     *
     * @param node The node that will use the transport. Default is the singleton node
     */
    IntraTransport(RR_SHARED_PTR<RobotRaconteurNode> node = RobotRaconteurNode::sp());

    virtual ~IntraTransport();

  public:
    virtual bool IsServer() const;

    virtual bool IsClient() const;

    virtual std::string GetUrlSchemeString() const;

    virtual void SendMessage(RR_INTRUSIVE_PTR<Message> m);

    virtual void AsyncSendMessage(RR_INTRUSIVE_PTR<Message> m,
                                  boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)>& callback);

    virtual void AsyncCreateTransportConnection(
        boost::string_ref url, RR_SHARED_PTR<Endpoint> e,
        boost::function<void(RR_SHARED_PTR<ITransportConnection>, RR_SHARED_PTR<RobotRaconteurException>)>& callback);

    virtual RR_SHARED_PTR<ITransportConnection> CreateTransportConnection(boost::string_ref url,
                                                                          RR_SHARED_PTR<Endpoint> e);

    virtual void CloseTransportConnection(RR_SHARED_PTR<Endpoint> e);

    /**
     * @brief Start the server to listen for incoming client connections
     *
     */
    virtual void StartServer();

  protected:
    virtual void CloseTransportConnection_timed(const boost::system::error_code& err, RR_SHARED_PTR<Endpoint> e,
                                                RR_SHARED_PTR<void> timer);

  public:
    virtual bool CanConnectService(boost::string_ref url);

    virtual void Close();

    virtual void CheckConnection(uint32_t endpoint);

    virtual void PeriodicCleanupTask();

    uint32_t TransportCapability(boost::string_ref name);

    virtual void MessageReceived(RR_INTRUSIVE_PTR<Message> m);

    virtual void AsyncGetDetectedNodes(const std::vector<std::string>& schemes,
                                       boost::function<void(RR_SHARED_PTR<std::vector<NodeDiscoveryInfo> >)>& handler,
                                       int32_t timeout = RR_TIMEOUT_INFINITE);

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

    bool TryGetNodeInfo(NodeID& node_id, std::string& node_name, std::string& service_nonce);

    void LocalNodeServicesChanged();

    void SendNodeDiscovery();

  protected:
    virtual void register_transport(RR_SHARED_PTR<ITransportConnection> connection);
    virtual void erase_transport(RR_SHARED_PTR<ITransportConnection> connection);

    void NodeDetected(const NodeDiscoveryInfo& info);

    boost::mutex init_lock;
    bool is_init;

    void Init();

    bool closed;
    boost::mutex closed_lock;
    boost::signals2::signal<void()> close_signal;
};

#ifndef BOOST_NO_CXX11_TEMPLATE_ALIASES
/** @brief Convenience alias for IntraTransport shared_ptr */
using IntraTransportPtr = RR_SHARED_PTR<IntraTransport>;
#endif

} // namespace RobotRaconteur