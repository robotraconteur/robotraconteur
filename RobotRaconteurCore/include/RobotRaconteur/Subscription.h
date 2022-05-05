/**
 * @file Subscription.h
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

#include "RobotRaconteur/Client.h"

#include <boost/tuple/tuple_comparison.hpp>
#include <boost/asio/strand.hpp>
#include <boost/unordered_set.hpp>

#pragma once

namespace RobotRaconteur
{
namespace detail
{
class Discovery;
class Discovery_nodestorage;
class ServiceInfo2Subscription_client;
class ServiceSubscription_client;
class ServiceSubscription_retrytimer;
class WireSubscription_connection;
class PipeSubscription_connection;
class WireSubscription_send_iterator;
class PipeSubscription_send_iterator;
class ServiceSubscription_custom_member_subscribers;
} // namespace detail

class ROBOTRACONTEUR_CORE_API WireConnectionBase;
template <typename T>
class WireConnection;
class ROBOTRACONTEUR_CORE_API WireSubscriptionBase;
template <typename T>
class WireSubscription;

class ROBOTRACONTEUR_CORE_API PipeEndpointBase;
template <typename T>
class PipeEndpoint;
class ROBOTRACONTEUR_CORE_API PipeSubscriptionBase;
template <typename T>
class PipeSubscription;

/**
 * @brief Subscription filter node information
 *
 * Specify a node by NodeID and/or NodeName. Also allows specifying
 * username and password.
 *
 * When using username and credentials, secure transports and specified NodeID should
 * be used. Using username and credentials without a transport that verifies the
 * NodeID could result in credentials being leaked.
 */
class ROBOTRACONTEUR_CORE_API ServiceSubscriptionFilterNode
{
  public:
    /** The NodeID to match. All zero NodeID will match any NodeID. **/
    ::RobotRaconteur::NodeID NodeID;
    /** The NodeName to match. Emtpy NodeName will match any NodeName. **/
    std::string NodeName;
    /** The username to use for authentication. Should only be used with secure transports and verified NodeID **/
    std::string Username;
    /** The credentials to use for authentication. Should only be used with secure transports and verified NodeID **/
    RR_INTRUSIVE_PTR<RRMap<std::string, RRValue> > Credentials;
};

/**
 * @brief Subscription filter
 *
 * The subscription filter is used with RobotRaconteurNode::SubscribeServiceByType() and
 * RobotRaconteurNode::SubscribeServiceInfo2() to decide which services should
 * be connected. Detected services that match the service type are checked against
 * the filter before connecting.
 *
 */
class ROBOTRACONTEUR_CORE_API ServiceSubscriptionFilter
{
  public:
    /** Vector of nodes that should be connected. Empty means match any node. **/
    std::vector<RR_SHARED_PTR<ServiceSubscriptionFilterNode> > Nodes;
    /** Vector service names that should be connected. Empty means match any service name. **/
    std::vector<std::string> ServiceNames;
    /** Vector of transport schemes. Empty means match any transport scheme. **/
    std::vector<std::string> TransportSchemes;
    /** A user specified predicate function. If nullptr, the predicate is not checked. **/
    boost::function<bool(const ServiceInfo2&)> Predicate;
    /** The maximum number of connections the subscription will create. Zero means unlimited connections. **/
    uint32_t MaxConnections;
};

/**
 * @brief ClientID for use with ServiceSubscription
 *
 * The ServiceSubscriptionClientID stores the NodeID
 * and ServiceName of a connected service.
 *
 */
class ROBOTRACONTEUR_CORE_API ServiceSubscriptionClientID
{
  public:
    /** The NodeID of the connected service **/
    ::RobotRaconteur::NodeID NodeID;
    /** The ServiceName of the connected service **/
    std::string ServiceName;

    /**
     * @brief Construct a ServiceSubscriptionClientID
     *
     * @param nodeid The NodeID
     * @param service_name The Service Name
     */
    ServiceSubscriptionClientID(const ::RobotRaconteur::NodeID& nodeid, boost::string_ref service_name);

    /**
     * @brief Construct an empty ServiceSubscriptionClientID
     *
     */
    ServiceSubscriptionClientID();

    /** Equality operator **/
    bool operator==(const ServiceSubscriptionClientID& id2) const;

    /** Inequality operator **/
    bool operator!=(const ServiceSubscriptionClientID& id2) const;

    /** Less-than operator **/
    bool operator<(const ServiceSubscriptionClientID& id2) const;
};

size_t hash_value(const ServiceSubscriptionClientID& id);

class IServiceSubscription
{
  public:
    friend class detail::Discovery;

  protected:
    virtual void Init(const std::vector<std::string>& service_types,
                      RR_SHARED_PTR<ServiceSubscriptionFilter> filter) = 0;
    virtual void NodeUpdated(RR_SHARED_PTR<detail::Discovery_nodestorage> storage) = 0;
    virtual void NodeLost(RR_SHARED_PTR<detail::Discovery_nodestorage> storage) = 0;

  public:
    virtual void Close() = 0;
};

/**
 * @brief Subscription for information about detected services
 *
 * Created using RobotRaconteurNode::SubscribeServiceInfo2()
 *
 * The ServiceInfo2Subscription class is used to track services with a specific service type as they are
 * detected on the local network and when they are lost. The currently detected services can also
 * be retrieved. The service information is returned using the ServiceInfo2 structure.
 *
 */
class ROBOTRACONTEUR_CORE_API ServiceInfo2Subscription : public IServiceSubscription,
                                                         public RR_ENABLE_SHARED_FROM_THIS<ServiceInfo2Subscription>,
                                                         private boost::noncopyable
{
  public:
    friend class detail::Discovery;

    typedef boost::signals2::connection event_connection;

    /**
     * @brief Returns a map of detected services.
     *
     * The returned map contains the detected nodes as ServiceInfo2. The map
     * is keyed with ServiceSubscriptionClientID.
     *
     * This function does not block.
     *
     * @return std::map<ServiceSubscriptionClientID, ServiceInfo2 > The detected services.
     */
    std::map<ServiceSubscriptionClientID, ServiceInfo2> GetDetectedServiceInfo2();

    /**
     * @brief Add a listener callback that is invoked when a service is detected
     *
     * The callback should have the signature:
     *
     *    void cb(ServiceInfo2SubscriptionPtr subscription, const ServiceSubscriptionClientID& client_id, const
     * ServiceInfo2& service_info)
     *
     * The returned event_connection can be used to modify or close the Boost.Signals2 connection.
     *
     * @param handler The callback function
     * @return event_connection The Boost.Signals2 connection
     */
    event_connection AddServiceDetectedListener(
        boost::function<void(RR_SHARED_PTR<ServiceInfo2Subscription>, const ServiceSubscriptionClientID&,
                             const ServiceInfo2&)>
            handler);

    /**
     * @brief Add a listener callback that is invoked when a service is lost
     *
     * The callback should have the signature:
     *
     *    void cb(ServiceInfo2SubscriptionPtr subscription, const ServiceSubscriptionClientID& client_id, const
     * ServiceInfo2& service_info)
     *
     * The returned event_connection can be used to modify or close the Boost.Signals2 connection.
     *
     * @param handler The callback function
     * @return event_connection The Boost.Signals2 connection
     */
    event_connection AddServiceLostListener(
        boost::function<void(RR_SHARED_PTR<ServiceInfo2Subscription>, const ServiceSubscriptionClientID&,
                             const ServiceInfo2&)>
            handler);

    /**
     * @brief Close the subscription
     *
     * Closes the subscription. Subscriptions are automatically closed when the node is shut down.
     *
     */
    virtual void Close();

    RR_SHARED_PTR<RobotRaconteurNode> GetNode();

  protected:
    boost::mutex this_lock;

    bool active;

    std::map<ServiceSubscriptionClientID, RR_SHARED_PTR<detail::ServiceInfo2Subscription_client> > clients;

    RR_WEAK_PTR<RobotRaconteurNode> node;
    RR_WEAK_PTR<detail::Discovery> parent;

    std::vector<std::string> service_types;
    RR_SHARED_PTR<ServiceSubscriptionFilter> filter;

    uint32_t retry_delay;

    boost::signals2::signal<void(RR_SHARED_PTR<ServiceInfo2Subscription>, const ServiceSubscriptionClientID&,
                                 const ServiceInfo2&)>
        detected_listeners;
    boost::signals2::signal<void(RR_SHARED_PTR<ServiceInfo2Subscription>, const ServiceSubscriptionClientID&,
                                 const ServiceInfo2&)>
        lost_listeners;

  public:
    // Do not call, use RobotRaconteurNode()->SubscribeServiceInfo2()
    ServiceInfo2Subscription(RR_SHARED_PTR<detail::Discovery> parent);

  protected:
    virtual void Init(const std::vector<std::string>& service_types, RR_SHARED_PTR<ServiceSubscriptionFilter> filter);
    virtual void NodeUpdated(RR_SHARED_PTR<detail::Discovery_nodestorage> storage);
    virtual void NodeLost(RR_SHARED_PTR<detail::Discovery_nodestorage> storage);

    void fire_ServiceDetectedListener(const ServiceSubscriptionClientID& noden, const ServiceInfo2& info);
    void fire_ServiceLostListener(const ServiceSubscriptionClientID& noden, const ServiceInfo2& info);
};

namespace detail
{
template <typename T>
void AsyncGetDefaultClient_handler_adapter(
    boost::function<void(RR_SHARED_PTR<T>, RR_SHARED_PTR<RobotRaconteurException>)>& handler,
    RR_SHARED_PTR<RRObject> obj, RR_SHARED_PTR<RobotRaconteurException> err)
{
    if (err)
    {
        handler(RR_SHARED_PTR<T>(), err);
        return;
    }

    RR_SHARED_PTR<T> obj1 = RR_DYNAMIC_POINTER_CAST<T>(obj);
    if (!obj1)
    {
        handler(RR_SHARED_PTR<T>(), RR_MAKE_SHARED<ConnectionException>("Cannot cast client to requested type"));
        return;
    }

    handler(obj1, RR_SHARED_PTR<RobotRaconteurException>());
}
} // namespace detail

/**
 * @brief Subscription that automatically connects services and manages lifecycle of connected services
 *
 * Created using RobotRaconteurNode::SubscribeService() or RobotRaconteurNode::SubscribeServiceByType(). The
 * ServiceSubscription class is used to automatically create and manage connections based on connection criteria.
 * RobotRaconteurNode::SubscribeService() is used to create a robust connection to a service with a specific URL.
 * RobotRaconteurNode::SubscribeServiceByType() is used to connect to services with a specified type, filtered with a
 * ServiceSubscriptionFilter. Subscriptions will create connections to matching services, and will retry the connection
 * if it fails or the connection is lost. This behavior allows subscriptions to be used to create robust connections.
 * The retry delay for connections can be modified using SetConnectRetryDelay() and GetConnectRetryDelay().
 *
 * The currently connected clients can be retrieved using the GetConnectedClients() function. A single "default client"
 * can be retrieved using the GetDefaultClient() function or TryGetDefaultClient() functions. Listeners for client
 * connect and disconnect events can be added  using the AddClientConnectListener() and AddClientDisconnectListener()
 * functions. If the user wants to claim a client, the ClaimClient() and ReleaseClient() functions will be used. Claimed
 * clients will no longer have their lifecycle managed by the subscription.
 *
 * Subscriptions can be used to create `pipe` and `wire` subscriptions. These member subscriptions aggregate
 * the packets and values being received from all services. They can also act as a "reverse broadcaster" to
 * send packets and values to all services that are actively connected. See PipeSubscription and WireSubscription.
 *
 */
class ROBOTRACONTEUR_CORE_API ServiceSubscription : public IServiceSubscription,
                                                    public RR_ENABLE_SHARED_FROM_THIS<ServiceSubscription>,
                                                    private boost::noncopyable
{
  public:
    friend class detail::Discovery;
    friend class detail::ServiceSubscription_retrytimer;
    friend class WireSubscriptionBase;
    friend class PipeSubscriptionBase;
    friend class detail::ServiceSubscription_custom_member_subscribers;

    typedef boost::signals2::connection event_connection;

    /**
     * @brief Returns a map of connected clients
     *
     * The returned map contains the connect clients. The map
     * is keyed with ServiceSubscriptionClientID.
     *
     * Clients must be cast to a type, similar to the client returned by
     * RobotRaconteurNode::ConnectService().
     *
     * Clients can be "claimed" using ClaimClient(). Once claimed, the subscription
     * will stop managing the lifecycle of the client.
     *
     * This function does not block.
     *
     * @return std::map<ServiceSubscriptionClientID, ServiceInfo2 > The detected services.
     */
    std::map<ServiceSubscriptionClientID, RR_SHARED_PTR<RRObject> > GetConnectedClients();

    /**
     * @brief Add a listener callback that is invoked when a client is connected
     *
     * The callback should have the signature:
     *
     *    void cb(ServiceSubscriptionPtr subscription, const ServiceSubscriptionClientID& client_id, RRObjectPtr client)
     *
     * The returned event_connection can be used to modify or close the Boost.Signals2 connection.
     *
     * @param handler The callback function
     * @return event_connection The Boost.Signals2 connection
     */
    event_connection AddClientConnectListener(
        boost::function<void(RR_SHARED_PTR<ServiceSubscription>, const ServiceSubscriptionClientID&,
                             RR_SHARED_PTR<RRObject>)>
            handler);

    /**
     * @brief Add a listener callback that is invoked when a client is disconnected
     *
     * The callback should have the signature:
     *
     *    void cb(ServiceSubscriptionPtr subscription, const ServiceSubscriptionClientID& client_id, RRObjectPtr client)
     *
     * The returned event_connection can be used to modify or close the Boost.Signals2 connection.
     *
     * @param handler The callback function
     * @return event_connection The Boost.Signals2 connection
     */
    event_connection AddClientDisconnectListener(
        boost::function<void(RR_SHARED_PTR<ServiceSubscription>, const ServiceSubscriptionClientID&,
                             RR_SHARED_PTR<RRObject>)>
            handler);

    /**
     * @brief Add a listener callback that is invoked when a client connection attempt fails
     *
     * The callback should have the signature:
     *
     *    void cb(ServiceSubscriptionPtr subscription, const ServiceSubscriptionClientID& client_id, const
     * std::vector<std::string>& url, RobotRaconteurExceptionPtr err)
     *
     * The returned event_connection can be used to modify or close the Boost.Signals2 connection.
     *
     * @param handler The callback function
     * @return event_connection The Boost.Signals2 connection
     */
    event_connection AddClientConnectFailedListener(
        boost::function<void(RR_SHARED_PTR<ServiceSubscription>, const ServiceSubscriptionClientID&,
                             const std::vector<std::string>&, RR_SHARED_PTR<RobotRaconteurException>)>
            handler);

    /**
     * @brief Close the subscription
     *
     * Close the subscription. Subscriptions are automatically closed when the node is shut down.
     *
     */
    virtual void Close();

    /**
     * @brief Claim a client that was connected by the subscription
     *
     * The subscription class will automatically manage the lifecycle of the connected clients. The clients
     * will be automatically disconnected and/or reconnected as necessary. If the user wants to disable
     * this behavior for a specific client connection, the client connection can be "claimed".
     *
     * @param client The client to be claimed
     */
    virtual void ClaimClient(RR_SHARED_PTR<RRObject> client);

    /**
     * @brief Release a client previously clamed with ClaimClient()
     *
     * Lifecycle management is returned to the subscription
     *
     * @param client The client to release claim
     */
    virtual void ReleaseClient(RR_SHARED_PTR<RRObject> client);

    /**
     * @brief Get the connect retry delay in milliseconds
     *
     * Default is 2.5 seconds
     *
     * @return uint32_t The retry delay in milliseconds
     */
    uint32_t GetConnectRetryDelay();

    /**
     * @brief Set the connect retry delay in milliseconds
     *
     * Default is 2.5 seconds
     *
     * @param delay_milliseconds The delay in milliseconds
     */
    void SetConnectRetryDelay(uint32_t delay_milliseconds);

    /**
     * @brief Creates a wire subscription
     *
     * Wire subscriptions aggregate the value received from the connected services. It can also act as a
     * "reverse broadcaster" to send values to clients. See WireSubscription.
     *
     * The optional service path may be an empty string to use the root object in the service. The first level of the
     * service path may be "*" to match any service name. For instance, the service path "*.sub_obj" will match
     * any service name, and use the "sub_obj" objref.
     *
     * @tparam T The type of the wire value. This must be specified since the subscription doesn't know the wire value
     * type
     * @param membername The member name of the wire
     * @param servicepath The service path of the object owning the wire member
     * @return RR_SHARED_PTR<WireSubscription<T> > The wire subscription
     */
    template <typename T>
    RR_SHARED_PTR<WireSubscription<T> > SubscribeWire(boost::string_ref membername, boost::string_ref servicepath = "")
    {
        RR_SHARED_PTR<WireSubscription<T> > o =
            RR_MAKE_SHARED<WireSubscription<T> >(shared_from_this(), membername, servicepath);
        SubscribeWire1(o);
        return o;
    }

    /**
     * @brief Creates a pipe subscription
     *
     * Pipe subscriptions aggregate the packets received from the connected services. It can also act as a
     * "reverse broadcaster" to send packets to clients. See PipeSubscription.
     *
     * The optional service path may be an empty string to use the root object in the service. The first level of the
     * service path may be "*" to match any service name. For instance, the service path "*.sub_obj" will match
     * any service name, and use the "sub_obj" objref.
     *
     * @tparam T The type of the pipe packets. This must be specified since the subscription does not know the pipe
     * packet type
     * @param membername The member name of the pipe
     * @param servicepath The service path of the object owning the pipe member
     * @return RR_SHARED_PTR<PipeSubscription<T> > The pipe subscription
     */
    template <typename T>
    RR_SHARED_PTR<PipeSubscription<T> > SubscribePipe(boost::string_ref membername, boost::string_ref servicepath = "",
                                                      uint32_t max_recv_packets = std::numeric_limits<uint32_t>::max())
    {
        RR_SHARED_PTR<PipeSubscription<T> > o =
            RR_MAKE_SHARED<PipeSubscription<T> >(shared_from_this(), membername, servicepath, max_recv_packets);
        SubscribePipe1(o);
        return o;
    }

    /**
     * @brief Get the "default client" connection
     *
     * The "default client" is the "first" client returned from the connected clients map. This is effectively
     * default, and is only useful if only a single client connection is expected. This is normally true
     * for RobotRaconteurNode::SubscribeService()
     *
     * Clients using GetDefaultClient() should not store a reference to the client. It should instead
     * call GetDefaultClient() right before using the client to make sure the most recenty connection
     * is being used. If possible, SubscribePipe() or SubscribeWire() should be used so the lifecycle
     * of pipes and wires can be managed automatically.
     *
     * @tparam T The type of the client object
     * @return RR_SHARED_PTR<T> The client connection
     */
    template <typename T>
    RR_SHARED_PTR<T> GetDefaultClient()
    {
        return rr_cast<T>(GetDefaultClientBase());
    }

    /**
     * @brief Try getting the "default client" connection
     *
     * Same as GetDefaultClient(), but returns a bool success instead of throwing
     * exceptions on failure.
     *
     * @tparam T The type of the client object
     * @param client_out [out] The client connection
     * @return true client_out is valid
     * @return false client_out is invalid, no client is not currently connected
     */
    template <typename T>
    bool TryGetDefaultClient(RR_SHARED_PTR<T>& client_out)
    {
        RR_SHARED_PTR<RRObject> c;
        if (!TryGetDefaultClientBase(c))
        {
            return false;
        }
        RR_SHARED_PTR<T> c1 = RR_DYNAMIC_POINTER_CAST<T>(c);
        if (!c1)
            return false;

        client_out = c1;
        return true;
    }

    /**
     * @brief Get the "default client" connection, waiting with timeout if not connected
     *
     * The "default client" is the "first" client returned from the connected clients map. This is effectively
     * default, and is only useful if only a single client connection is expected. This is normally true
     * for RobotRaconteurNode::SubscribeService()
     *
     * Clients using GetDefaultClient() should not store a reference to the client. It should instead
     * call GetDefaultClient() right before using the client to make sure the most recenty connection
     * is being used. If possible, SubscribePipe() or SubscribeWire() should be used so the lifecycle
     * of pipes and wires can be managed automatically.
     *
     * @param timeout Timeout in milliseconds, or RR_TIMEOUT_INFINITE for no timeout
     * @tparam T The type of the client object
     * @return RR_SHARED_PTR<T> The client connection
     */
    template <typename T>
    RR_SHARED_PTR<T> GetDefaultClientWait(int32_t timeout = RR_TIMEOUT_INFINITE)
    {
        return rr_cast<T>(GetDefaultClientWaitBase(timeout));
    }

    /**
     * @brief Try getting the "default client" connection, waiting with timeout if not connected
     *
     * Same as GetDefaultClientWait(), but returns a bool success instead of throwing
     * exceptions on failure.
     *
     * @tparam T The type of the client object
     * @param client_out [out] The client connection
     * @param timeout Timeout in milliseconds, or RR_TIMEOUT_INFINITE for no timeout
     * @return true client_out is valid
     * @return false client_out is invalid, no client is not currently connected
     */
    template <typename T>
    bool TryGetDefaultClientWait(RR_SHARED_PTR<T>& client_out, int32_t timeout = RR_TIMEOUT_INFINITE)
    {
        RR_SHARED_PTR<RRObject> c;
        if (!TryGetDefaultClientWaitBase(c, timeout))
        {
            return false;
        }
        RR_SHARED_PTR<T> c1 = RR_DYNAMIC_POINTER_CAST<T>(c);
        if (!c1)
            return false;

        client_out = c1;
        return true;
    }

    /**
     * @brief Asynchronously get the default client, with optional timeout
     *
     * Same as GetDefaultClientWait(), but returns asynchronously.
     *
     * @tparam T The type of the client object
     * @param handler The handler to call when default client is available, or times out
     * @param timeout Timeout in milliseconds, or RR_TIMEOUT_INFINITE for no timeout
     */
    template <typename T>
    void AsyncGetDefaultClient(boost::function<void(RR_SHARED_PTR<T>, RR_SHARED_PTR<RobotRaconteurException>)> handler,
                               int32_t timeout = RR_TIMEOUT_INFINITE)
    {
        AsyncGetDefaultClientBase(boost::bind(&detail::AsyncGetDefaultClient_handler_adapter<T>, handler,
                                              RR_BOOST_PLACEHOLDERS(_1), RR_BOOST_PLACEHOLDERS(_2)),
                                  timeout);
    }

    /**
     * @brief Get the service connection URL
     *
     * Returns the service connection URL. Only valid when subscription was created using
     * RobotRaconteurNode::SubscribeService(). Will throw an exception if subscription
     * was opened using RobotRaconteurNode::SubscribeServiceByType()
     *
     * @return std::vector<std::string>
     */
    std::vector<std::string> GetServiceURL();

    /**
     * @brief Update the service connection URL
     *
     * Updates the URL used to connect to the service. If close_connected is true,
     * existing connections will be closed. If false,
     * existing connections will not be closed.
     *
     * @param url The new URL to use to connect to service
     * @param username (Optional) The new username
     * @param credentials (Optional) The new credentials
     * @param object_type (Optional) The desired root object proxy type. Optional but highly recommended.
     * @param close_connected (Optional, default false) Close existing connections
     */
    void UpdateServiceURL(
        boost::string_ref url, boost::string_ref username = "",
        RR_INTRUSIVE_PTR<RRMap<std::string, RRValue> > credentials = (RR_INTRUSIVE_PTR<RRMap<std::string, RRValue> >()),
        boost::string_ref object_type = "", bool close_connected = false);

    /**
     * @brief Update the service connection URL
     *
     * Updates the URL used to connect to the service. If close_connected is true,
     * existing connections will be closed. If false,
     * existing connections will not be closed.
     *
     * @param url The new URLs to use to connect to service
     * @param username (Optional) The new username
     * @param credentials (Optional) The new credentials
     * @param object_type The desired root object proxy type. Optional but highly recommended.
     * @param close_connected (Optional, default false) Close existing connections
     */
    void UpdateServiceURL(
        const std::vector<std::string>& url, boost::string_ref username = "",
        RR_INTRUSIVE_PTR<RRMap<std::string, RRValue> > credentials = (RR_INTRUSIVE_PTR<RRMap<std::string, RRValue> >()),
        boost::string_ref object_type = "", bool close_connected = false);

    RR_SHARED_PTR<RobotRaconteurNode> GetNode();

  protected:
    boost::mutex this_lock;

    bool active;

    std::map<ServiceSubscriptionClientID, RR_SHARED_PTR<detail::ServiceSubscription_client> > clients;

    RR_WEAK_PTR<RobotRaconteurNode> node;
    RR_WEAK_PTR<detail::Discovery> parent;

    std::vector<std::string> service_types;
    RR_SHARED_PTR<ServiceSubscriptionFilter> filter;

    uint32_t retry_delay;

    boost::signals2::signal<void(RR_SHARED_PTR<ServiceSubscription>, const ServiceSubscriptionClientID&,
                                 RR_SHARED_PTR<RRObject>)>
        connect_listeners;
    boost::signals2::signal<void(RR_SHARED_PTR<ServiceSubscription>, const ServiceSubscriptionClientID&,
                                 RR_SHARED_PTR<RRObject>)>
        disconnect_listeners;
    boost::signals2::signal<void(RR_SHARED_PTR<ServiceSubscription>, const ServiceSubscriptionClientID&,
                                 const std::vector<std::string>&, RR_SHARED_PTR<RobotRaconteurException>)>
        connect_failed_listeners;

    RR_SHARED_PTR<RR_BOOST_ASIO_STRAND> listener_strand;

    boost::unordered_set<RR_SHARED_PTR<WireSubscriptionBase> > wire_subscriptions;
    boost::unordered_set<RR_SHARED_PTR<PipeSubscriptionBase> > pipe_subscriptions;

    bool use_service_url;
    std::vector<std::string> service_url;
    std::string service_url_username;
    RR_INTRUSIVE_PTR<RRMap<std::string, RRValue> > service_url_credentials;

  public:
    // Do not call, use RobotRaconteurNode()->SubscribeService()
    ServiceSubscription(RR_SHARED_PTR<detail::Discovery> parent);

  protected:
    virtual void Init(const std::vector<std::string>& service_types, RR_SHARED_PTR<ServiceSubscriptionFilter> filter);
    virtual void InitServiceURL(
        const std::vector<std::string>& url, boost::string_ref username = "",
        RR_INTRUSIVE_PTR<RRMap<std::string, RRValue> > credentials = (RR_INTRUSIVE_PTR<RRMap<std::string, RRValue> >()),
        boost::string_ref objecttype = "");
    virtual void NodeUpdated(RR_SHARED_PTR<detail::Discovery_nodestorage> storage);
    virtual void NodeLost(RR_SHARED_PTR<detail::Discovery_nodestorage> storage);

    void ClientConnected(RR_SHARED_PTR<RRObject> c, RR_SHARED_PTR<RobotRaconteurException> err,
                         RR_SHARED_PTR<detail::ServiceSubscription_client> c2, const std::vector<std::string>& url);
    void ConnectRetry(RR_SHARED_PTR<detail::ServiceSubscription_client> c2);
    void ConnectRetry2(RR_SHARED_PTR<detail::ServiceSubscription_client> c2);

    static void ClientEvent(RR_WEAK_PTR<ServiceSubscription> this_, RR_SHARED_PTR<ClientContext> ctx,
                            ClientServiceListenerEventType evt, RR_SHARED_PTR<void> p,
                            RR_WEAK_PTR<detail::ServiceSubscription_client> c2);

    void fire_ClientConnectListeners(const ServiceSubscriptionClientID& noden, RR_SHARED_PTR<RRObject> client);
    void fire_ClientDisconnectListeners(const ServiceSubscriptionClientID& noden, RR_SHARED_PTR<RRObject> client);
    void fire_ClientConnectFailedListeners(const ServiceSubscriptionClientID& noden,
                                           const std::vector<std::string>& url,
                                           RR_SHARED_PTR<RobotRaconteurException> err);

    void SubscribeWire1(RR_SHARED_PTR<WireSubscriptionBase> s);
    void SubscribePipe1(RR_SHARED_PTR<PipeSubscriptionBase> s);

    void WireSubscriptionClosed(RR_SHARED_PTR<WireSubscriptionBase> s);
    void PipeSubscriptionClosed(RR_SHARED_PTR<PipeSubscriptionBase> s);

    RR_SHARED_PTR<RRObject> GetDefaultClientBase();
    bool TryGetDefaultClientBase(RR_SHARED_PTR<RRObject>& client_out);

    RR_SHARED_PTR<RRObject> GetDefaultClientWaitBase(int32_t timeout = RR_TIMEOUT_INFINITE);
    bool TryGetDefaultClientWaitBase(RR_SHARED_PTR<RRObject>& client_out, int32_t timeout = RR_TIMEOUT_INFINITE);

    void AsyncGetDefaultClientBase(
        boost::function<void(RR_SHARED_PTR<RRObject>, RR_SHARED_PTR<RobotRaconteurException>)> handler,
        int32_t timeout = RR_TIMEOUT_INFINITE);
};

/**
 * @brief Base class for WireSubscription
 *
 * Base class for templated WireSubscription
 *
 */
class ROBOTRACONTEUR_CORE_API WireSubscriptionBase : public RR_ENABLE_SHARED_FROM_THIS<WireSubscriptionBase>,
                                                     private boost::noncopyable
{
  public:
    friend class WireConnectionBase;
    friend class ServiceSubscription;
    friend class detail::WireSubscription_connection;
    friend class detail::WireSubscription_send_iterator;

    typedef boost::signals2::connection event_connection;

    RR_INTRUSIVE_PTR<RRValue> GetInValueBase(TimeSpec* time = NULL,
                                             RR_SHARED_PTR<WireConnectionBase>* connection = NULL);
    bool TryGetInValueBase(RR_INTRUSIVE_PTR<RRValue>& val, TimeSpec* time = NULL,
                           RR_SHARED_PTR<WireConnectionBase>* connection = NULL);

    /**
     * @brief Wait for a valid InValue to be received from a client
     *
     * Blocks the current thread until value is received or timeout
     *
     * @param timeout The timeout in milliseconds
     * @return true A value was received
     * @return false The wait timed out, or RR_TIMEOUT_INFINITE for no timeout
     */
    bool WaitInValueValid(int32_t timeout = RR_TIMEOUT_INFINITE);

    /**
     * @brief Get the number of wire connections currently connected
     *
     * @return size_t The number of active connections
     */
    size_t GetActiveWireConnectionCount();

    /**
     * @brief Get if InValue is currently being ignored
     *
     * @return true InValue is being ignored
     * @return false InValue is not being ignored
     */
    bool GetIgnoreInValue();

    /**
     * @brief Set if InValue should be ignored
     *
     * See WireConnection::SetIgnoreInValue()
     *
     * If true, InValue will be ignored for all wire connections.
     *
     * @param ignore
     */
    void SetIgnoreInValue(bool ignore);

    /**
     * @brief Get the InValue lifespan in milliseconds
     *
     * Get the lifespan of InValue in milliseconds. The value will expire after the specified
     * lifespan, becoming invalid. Use -1 for infinite lifespan.
     *
     * @return int32_t The lifespan in milliseconds. -1 for infinite
     */
    int32_t GetInValueLifespan();

    /**
     * @brief Set the InValue lifespan in milliseconds
     *
     * Set the lifespan of InValue in milliseconds. The value will expire after
     * the specified lifespan, becoming invalid. Use -1 for infinite lifespan.
     *
     * See also WireConnection::SetInValueLifespan()
     *
     * @param millis The lifespan in milliseconds. -1 for infinite
     */
    void SetInValueLifespan(int32_t millis);

    void SetOutValueAllBase(const RR_INTRUSIVE_PTR<RRValue>& val);

    /**
     * @brief Closes the wire subscription
     *
     * Wire subscriptions are automatically closed when the parent ServiceSubscription is closed
     * or when the node is shut down.
     *
     */
    void Close();

    WireSubscriptionBase(RR_SHARED_PTR<ServiceSubscription> parent, boost::string_ref membername,
                         boost::string_ref servicepath);

    RR_SHARED_PTR<RobotRaconteurNode> GetNode();

  protected:
    void ClientConnected(const ServiceSubscriptionClientID& client_id, RR_SHARED_PTR<RRObject> client);
    void ClientDisconnected(const ServiceSubscriptionClientID& client_id, RR_SHARED_PTR<RRObject> client);

    void WireConnectionClosed(RR_SHARED_PTR<detail::WireSubscription_connection> wire);
    void WireValueChanged(RR_SHARED_PTR<detail::WireSubscription_connection> wire, RR_INTRUSIVE_PTR<RRValue> value,
                          const TimeSpec& time);

    boost::mutex this_lock;
    boost::unordered_map<ServiceSubscriptionClientID, RR_SHARED_PTR<detail::WireSubscription_connection> > connections;
    boost::initialized<bool> closed;
    RR_WEAK_PTR<RobotRaconteurNode> node;
    RR_WEAK_PTR<ServiceSubscription> parent;

    RR_INTRUSIVE_PTR<RRValue> in_value;
    TimeSpec in_value_time;
    boost::initialized<bool> in_value_valid;
    RR_SHARED_PTR<WireConnectionBase> in_value_connection;
    int32_t in_value_lifespan;
    boost::posix_time::ptime in_value_time_local;

    boost::condition_variable in_value_wait;

    boost::initialized<bool> ignore_in_value;

    std::string membername;
    std::string servicepath;

    virtual void fire_WireValueChanged(RR_INTRUSIVE_PTR<RRValue> value, const TimeSpec& time,
                                       RR_SHARED_PTR<WireConnectionBase> connection);
    virtual bool isempty_WireValueChanged();
    RR_SHARED_PTR<detail::async_signal_pool_semaphore> wire_value_changed_semaphore;
};

/**
 * @brief Subscription for wire members that aggregates the values from client wire connections
 *
 * Wire subscriptions are created using the ServiceSubscription::SubscribeWire() function. This function takes the
 * type of the wire value, the name of the wire member, and an optional service path of the service
 * object that owns the wire member.
 *
 * Wire subscriptions aggregate the InValue from all active wire connections. When a client connects,
 * the wire subscriptions will automatically create wire connections to the wire member specified
 * when the WireSubscription was created using ServiceSubscription::SubscribeWire(). The InValue of
 * all the active wire connections are collected, and the most recent one is used as the current InValue
 * of the wire subscription. The current value, the timespec, and the wire connection can be accessed
 * using GetInValue() or TryGetInValue().
 *
 * The lifespan of the InValue can be configured using SetInValueLifespan(). It is recommended that
 * the lifespan be configured, so that the value will expire if the subscription stops receiving
 * fresh in values.
 *
 * The wire subscription can also be used to set the OutValue of all active wire connections. This behaves
 * similar to a "reverse broadcaster", sending the same value to all connected services.
 *
 * @tparam T The value type used by the wire
 */
template <typename T>
class WireSubscription : public WireSubscriptionBase
{
  public:
    WireSubscription(RR_SHARED_PTR<ServiceSubscription> parent, boost::string_ref membername,
                     boost::string_ref servicepath)
        : WireSubscriptionBase(parent, membername, servicepath)
    {}

    /**
     * @brief Get the current InValue and metadata
     *
     * Throws ValueNotSetException if no valid value is available
     *
     * @param time [out] the LastValueReceivedTime of the InValue
     * @param connection [out] the wire connection that received the InValue
     * @return T the current InValue
     */
    T GetInValue(TimeSpec* time = NULL, typename RR_SHARED_PTR<WireConnection<T> >* connection = NULL)
    {
        RR_SHARED_PTR<WireConnectionBase> connection1;
        T o = RRPrimUtil<T>::PreUnpack(GetInValueBase(time, &connection1));
        if (connection1)
        {
            *connection = RR_DYNAMIC_POINTER_CAST<WireConnection<T> >(connection1);
        }
        return o;
    }

    /**
     * @brief Try getting the current InValue and metadata
     *
     * Same as GetInValue(), but returns a bool for success or failure instead of throwing
     * an exception.
     *
     * @param val [out] the current InValue
     * @param time [out] the LastValueReceivedTime of the InValue
     * @param connection [out] the wire connection that received the InValue
     * @return true Getting the InValue was successful
     * @return false There is no valid InValue
     */
    bool TryGetInValue(T& val, TimeSpec* time = NULL, typename RR_SHARED_PTR<WireConnection<T> >* connection = NULL)
    {
        RR_INTRUSIVE_PTR<RRValue> o;
        RR_SHARED_PTR<WireConnectionBase> connection1;
        if (!TryGetInValueBase(o, time, &connection1))
            return false;
        val = RRPrimUtil<T>::PreUnpack(o);
        if (connection && connection1)
        {
            *connection = RR_DYNAMIC_POINTER_CAST<WireConnection<T> >(connection1);
        }
        return true;
    }

    /**
     * @brief Set the OutValue for all active wire connections
     *
     * Behaves like a "reverse broadcaster". Calls WireConnection::SetOutValue()
     * for all connected wire connections.
     *
     * @tparam T The type of the value
     * @param val The new OutValue
     */
    void SetOutValueAll(const T& val) { SetOutValueAllBase(RRPrimUtil<T>::PrePack(val)); }

    /**
     * @brief Adds a wire value changed event listener function
     *
     * Add a listener function that is called when the InValue changes. The callback should have the
     * signature:
     *
     *     void cb(WireSubscriptionPtr<T> wire_connection, const T& val, const TimeSpec& ts)
     *
     * @param f The callback function
     * @return event_connection The Boost.Signals2 connection
     */
    event_connection AddWireValueChangedListener(
        boost::function<void(RR_SHARED_PTR<WireSubscription<T> >, const T&, const TimeSpec&)> f)
    {
        return wire_value_changed.connect(f);
    }

  protected:
    boost::signals2::signal<void(RR_SHARED_PTR<WireSubscription<T> >, const T&, const TimeSpec&)> wire_value_changed;

    virtual void fire_WireValueChanged(RR_INTRUSIVE_PTR<RRValue> value, const TimeSpec& time,
                                       RR_SHARED_PTR<WireConnectionBase> connection)
    {
        wire_value_changed(RR_STATIC_POINTER_CAST<WireSubscription<T> >(shared_from_this()),
                           RRPrimUtil<T>::PreUnpack(value), time);
    }

    virtual bool isempty_WireValueChanged() { return wire_value_changed.empty(); }
};

/**
 * @brief Base class for PipeSubscription
 *
 * Base class for templated PipeSubscription
 *
 */
class ROBOTRACONTEUR_CORE_API PipeSubscriptionBase : public RR_ENABLE_SHARED_FROM_THIS<PipeSubscriptionBase>,
                                                     private boost::noncopyable
{
  public:
    friend class PipeConnectionBase;
    friend class ServiceSubscription;
    friend class detail::PipeSubscription_connection;
    friend class detail::PipeSubscription_send_iterator;

    typedef boost::signals2::connection event_connection;

    RR_INTRUSIVE_PTR<RRValue> ReceivePacketBase();
    bool TryReceivePacketBase(RR_INTRUSIVE_PTR<RRValue>& packet);
    bool TryReceivePacketBaseWait(RR_INTRUSIVE_PTR<RRValue>& packet, int32_t timeout = RR_TIMEOUT_INFINITE,
                                  bool peek = false, RR_SHARED_PTR<PipeEndpointBase>* ep = NULL);

    /**
     * @brief Get the number of packets available to receive
     *
     * Use ReceivePacket(), TryReceivePacket(), or TryReceivePacketWait() to receive the packet
     *
     * @return size_t
     */
    size_t Available();

    /**
     * @brief Get the number of pipe endpoints currently connected
     *
     * @return size_t The number of active pipe endpoints
     */
    size_t GetActivePipeEndpointCount();

    /**
     * @brief Get if incoming packets are currently being ignored
     *
     * @return true Incoming packets are being ignored
     * @return false Incoming packets are not being ignored
     */
    bool GetIgnoreReceived();

    /**
     * @brief Set if incoming packets should be ignored
     *
     * See PipeEndpoint::SetIgnoreReceived();
     *
     * If true, receivedpackets will be ignored for all connected pipe endpoints
     *
     * @param ignore
     */
    void SetIgnoreReceived(bool ignore);

    void AsyncSendPacketAllBase(const RR_INTRUSIVE_PTR<RRValue>& packet);

    /**
     * @brief Closes the pipe subscription
     *
     * Pipe subscriptions are automatically closed when the parent ServiceSubscription is closed
     * or when the node is shut down.
     *
     */
    void Close();

    PipeSubscriptionBase(RR_SHARED_PTR<ServiceSubscription> parent, boost::string_ref membername,
                         boost::string_ref servicepath = "", int32_t max_recv_packets = -1,
                         int32_t max_send_backlog = 5);

    RR_SHARED_PTR<RobotRaconteurNode> GetNode();

  protected:
    void ClientConnected(const ServiceSubscriptionClientID& client_id, RR_SHARED_PTR<RRObject> client);
    void ClientDisconnected(const ServiceSubscriptionClientID& client_id, RR_SHARED_PTR<RRObject> client);

    void PipeEndpointClosed(RR_SHARED_PTR<detail::PipeSubscription_connection> pipe);
    void PipeEndpointPacketReceived(RR_SHARED_PTR<detail::PipeSubscription_connection> pipe,
                                    RR_INTRUSIVE_PTR<RRValue> packet);

    boost::mutex this_lock;
    boost::unordered_map<ServiceSubscriptionClientID, RR_SHARED_PTR<detail::PipeSubscription_connection> > connections;
    boost::initialized<bool> closed;
    RR_WEAK_PTR<ServiceSubscription> parent;
    RR_WEAK_PTR<RobotRaconteurNode> node;

    std::deque<boost::tuple<RR_INTRUSIVE_PTR<RRValue>, RR_SHARED_PTR<PipeEndpointBase> > > recv_packets;
    boost::condition_variable recv_packets_wait;

    std::string membername;
    std::string servicepath;

    boost::initialized<int32_t> max_recv_packets;
    boost::initialized<bool> ignore_incoming_packets;

    boost::initialized<int32_t> max_send_backlog;

    virtual void fire_PipePacketReceived();
    virtual bool isempty_PipePacketReceived();
    RR_SHARED_PTR<detail::async_signal_pool_semaphore> pipe_packet_received_semaphore;
};

/**
 * @brief Subscription for pipe members that aggregates incoming packets from client pipe endpoints
 *
 * Pipe subscriptions are created using the ServiceSubscription::SubscribePipe() function. This function takes the
 * the type of the pipe packets, the name of the pipe member, and an optional service path of the service
 * object that owns the pipe member.
 *
 * Pipe subscriptions collect all incoming packets from connect pipe endpoints. When a client connects,
 * the pipe subscription will automatically connect a pipe endpoint the pipe endpoint specified when
 * the PipeSubscription was created using ServiceSubscription::SubscribePipe(). The packets received
 * from each of the collected pipes are collected and placed into a common receive queue. This queue
 * is read using ReceivePacket(), TryReceivePacket(), or TryReceivePacketWait(). The number of packets
 * available to receive can be checked using Available().
 *
 * Pipe subscriptions can also be used to send packets to all connected pipe endpoints. This is done
 * with the AsyncSendPacketAll() function. This function behaves somewhat like a "reverse broadcaster",
 * sending the packets to all connected services.
 *
 * If the pipe subscription is being used to send packets but not receive them, the SetIgnoreInValue()
 * should be set to true to prevent packets from queueing.
 *
 * @tparam T The type of the pipe packets
 */
template <typename T>
class PipeSubscription : public PipeSubscriptionBase
{
  public:
    PipeSubscription(RR_SHARED_PTR<ServiceSubscription> parent, boost::string_ref membername,
                     boost::string_ref servicepath = "", int32_t max_recv_packets = -1, int32_t max_send_backlog = 5)
        : PipeSubscriptionBase(parent, membername, servicepath, max_recv_packets)
    {}

    /**
     * @brief Dequeue a packet from the receive queue
     *
     * If the receive queue is empty, an InvalidOperationException() is thrown
     *
     * @return T The dequeued packet
     */
    T ReceivePacket() { return RRPrimUtil<T>::PreUnpack(ReceivePacketBase()); }

    /**
     * @brief Try dequeuing a packet from the receive queue
     *
     * Same as ReceivePacket(), but returns a bool for success or failure instead of throwing
     * an exception
     *
     * @param packet [out] the dequeued packet
     * @return true The packet was dequeued successfully
     * @return false The queue is empty
     */

    bool TryReceivePacket(T& packet)
    {
        RR_INTRUSIVE_PTR<RRValue> o;
        if (!TryReceivePacketBase(o))
            return false;
        packet = RRPrimUtil<T>::PreUnpack(o);
        return true;
    }

    /**
     * @brief Try dequeuing a packet from the receive queue, optionally waiting or peeking the packet
     *
     * @param packet [out] the dequeued packet
     * @param timeout The time to wait for a packet to be received in milliseconds if the queue is empty, or
     * RR_TIMEOUT_INFINITE to wait forever
     * @param peek If true, the packet is returned, but not dequeued. If false, the packet is dequeued
     * @param ep [out] the PipeEndpoint that received the packet
     * @return true The packet was dequeued successfully
     * @return false The receive queue is empty, or wait for packet timed out
     */
    bool TryReceivePacketWait(T& packet, int32_t timeout = RR_TIMEOUT_INFINITE, bool peek = false,
                              RR_SHARED_PTR<PipeEndpoint<T> >* ep = NULL)
    {
        RR_INTRUSIVE_PTR<RRValue> o;
        RR_SHARED_PTR<PipeEndpointBase> ep1;
        if (!TryReceivePacketBaseWait(o, timeout, peek, &ep1))
            return false;
        packet = RRPrimUtil<T>::PreUnpack(o);
        if (ep1)
        {
            *ep = RR_DYNAMIC_POINTER_CAST<PipeEndpoint<T> >(ep1);
        }
        return true;
    }

    /**
     * @brief Sends a packet to all connected pipe endpoints
     *
     * Calls AsyncSendPacket() on all connected pipe endpoints with the specified value.
     * Returns immediately, not waiting for transmission to complete.
     *
     * @param packet The packet to send
     */
    void AsyncSendPacketAll(const T& packet) { AsyncSendPacketAllBase(RRPrimUtil<T>::PrePack(packet)); }

    /**
     * @brief Adds a pipe packet received event listener function
     *
     * Add a listener function that is called when the pipe subscription receives a packet. The callback should
     * have the signature:
     *
     *     void cb(PipeSubscriptionPtr<T> pipe_subscription)
     *
     * The callback function does not include the received packets as an argument. The callback must call
     * one of the ReceivePacket() functions to dequeue the packets.
     *
     * @param f The callback function
     * @return event_connection The Boost.Signals2 connection
     */
    event_connection AddPipePacketReceivedListener(boost::function<void(RR_SHARED_PTR<PipeSubscription<T> >)> f)
    {
        return pipe_packet_received.connect(f);
    }

  protected:
    boost::signals2::signal<void(RR_SHARED_PTR<PipeSubscription<T> >)> pipe_packet_received;

    virtual void fire_PipePacketReceived()
    {
        pipe_packet_received(RR_STATIC_POINTER_CAST<PipeSubscription<T> >(shared_from_this()));
    }

    virtual bool isempty_PipePacketReceived() { return pipe_packet_received.empty(); }
};

namespace detail
{
class ROBOTRACONTEUR_CORE_API WireSubscription_send_iterator
{
  protected:
    RR_SHARED_PTR<WireSubscriptionBase> subscription;
    boost::mutex::scoped_lock subscription_lock;
    boost::unordered_map<ServiceSubscriptionClientID, RR_SHARED_PTR<WireSubscription_connection> >::iterator
        connections_iterator;
    boost::unordered_map<ServiceSubscriptionClientID, RR_SHARED_PTR<WireSubscription_connection> >::iterator
        current_connection;

  public:
    WireSubscription_send_iterator(RR_SHARED_PTR<WireSubscriptionBase> subscription);
    RR_SHARED_PTR<WireConnectionBase> Next();
    void SetOutValue(const RR_INTRUSIVE_PTR<RRValue>& value);
    virtual ~WireSubscription_send_iterator();
};

class ROBOTRACONTEUR_CORE_API PipeSubscription_send_iterator
{
  protected:
    RR_SHARED_PTR<PipeSubscriptionBase> subscription;
    boost::mutex::scoped_lock subscription_lock;
    boost::unordered_map<ServiceSubscriptionClientID, RR_SHARED_PTR<PipeSubscription_connection> >::iterator
        connections_iterator;
    boost::unordered_map<ServiceSubscriptionClientID, RR_SHARED_PTR<PipeSubscription_connection> >::iterator
        current_connection;

  public:
    PipeSubscription_send_iterator(RR_SHARED_PTR<PipeSubscriptionBase> subscription);
    RR_SHARED_PTR<PipeEndpointBase> Next();
    void AsyncSendPacket(const RR_INTRUSIVE_PTR<RRValue>& packet);
    virtual ~PipeSubscription_send_iterator();
};

class ROBOTRACONTEUR_CORE_API ServiceSubscription_custom_member_subscribers
{
  public:
    static void SubscribeWire(RR_SHARED_PTR<ServiceSubscription> s, RR_SHARED_PTR<WireSubscriptionBase> o);
    static void SubscribePipe(RR_SHARED_PTR<ServiceSubscription> s, RR_SHARED_PTR<PipeSubscriptionBase> o);
};
} // namespace detail

#ifndef BOOST_NO_CXX11_TEMPLATE_ALIASES
/** @brief Convenience alias for ServiceSubscriptionFilterNode shared_ptr */
using ServiceSubscriptionFilterNodePtr = RR_SHARED_PTR<ServiceSubscriptionFilterNode>;
/** @brief Convenience alias for ServiceSubscriptionFilter shared_ptr */
using ServiceSubscriptionFilterPtr = RR_SHARED_PTR<ServiceSubscriptionFilter>;
/** @brief Convenience alias for ServiceInfo2Subscription shared_ptr */
using ServiceInfo2SubscriptionPtr = RR_SHARED_PTR<ServiceInfo2Subscription>;
/** @brief Convenience alias for ServiceSubscription shared_ptr */
using ServiceSubscriptionPtr = RR_SHARED_PTR<ServiceSubscription>;
/** @brief Convenience alias for PipeSubscription shared_ptr */
template <typename T>
using PipeSubscriptionPtr = RR_SHARED_PTR<PipeSubscription<T> >;
/** @brief Convenience alias for WireSubscription shared_ptr */
template <typename T>
using WireSubscriptionPtr = RR_SHARED_PTR<WireSubscription<T> >;

#endif

} // namespace RobotRaconteur
