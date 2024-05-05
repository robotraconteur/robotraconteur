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
class ServiceSubscriptionManager_subscription;
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
class ROBOTRACONTEUR_CORE_API SubObjectSubscription;
class ROBOTRACONTEUR_CORE_API ServiceSubscriptionManager;

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
 * @brief Subscription filter attribute for use with ServiceSubscriptionFilter
 *
 */
class ROBOTRACONTEUR_CORE_API ServiceSubscriptionFilterAttribute
{
  public:
    /** @brief The attribute name. Empty for no name */
    std::string Name;
    /** @brief The string value of the attribute */
    std::string Value;
    /** @brief The regex value of the attribute */
    boost::regex ValueRegex;
    /** @brief True if ValueRegex is used, otherwise Value is matched */
    bool UseRegex;

    /**
     * @brief Construct a new Service Subscription Filter Attribute object
     *
     */
    ServiceSubscriptionFilterAttribute();
    /**
     * @brief Construct a new Service Subscription Filter Attribute object
     *
     * This is a nameless attribute for use with attribute lists
     *
     * @param value The attribute value
     */
    ServiceSubscriptionFilterAttribute(boost::string_ref value);
    /**
     * @brief Construct a new Service Subscription Filter Attribute object
     *
     * This is a nameless attribute for use with attribute lists. The value is compared using a regex
     *
     * @param value_regex The attribute value regex
     */
    ServiceSubscriptionFilterAttribute(const boost::regex& value_regex);
    /**
     * @brief Construct a new Service Subscription Filter Attribute object
     *
     * This is a named attribute for use with attribute maps
     *
     * @param name The attribute name
     * @param value The attribute value
     */
    ServiceSubscriptionFilterAttribute(boost::string_ref name, boost::string_ref value);

    /**
     * @brief Construct a new Service Subscription Filter Attribute object
     *
     * This is a named attribute for use with attribute maps. The value is compared using a regex
     *
     * @param name The attribute name
     * @param value_regex The attribute value regex
     */
    ServiceSubscriptionFilterAttribute(boost::string_ref name, const boost::regex& value_regex);

    /**
     * @brief Compare the attribute to a value
     *
     * @param value The value to compare
     * @return true
     * @return false
     */
    bool IsMatch(boost::string_ref value) const;
    /**
     * @brief Compare the attribute to a named value
     *
     * @param name The name to compare
     * @param value The value to compare
     * @return true
     * @return false
     */
    bool IsMatch(boost::string_ref name, boost::string_ref value) const;
    /**
     * @brief Compare the attribute to a value list using OR logic
     *
     * @param values The values to compare
     * @return true
     * @return false
     */
    bool IsMatch(const std::vector<std::string>& values) const;
    /**
     * @brief Compare the attribute to a value list using OR logic
     *
     * @param values The values to compare
     * @return true
     * @return false
     */
    bool IsMatch(const RR_INTRUSIVE_PTR<RRList<RRValue> >& values) const;
    /**
     * @brief Compare the attribute to a value map using OR logic
     *
     * @param values The value map to compare
     * @return true
     * @return false
     */
    bool IsMatch(const RR_INTRUSIVE_PTR<RRMap<std::string, RRValue> >& values) const;
    /**
     * @brief Compare the attribute to a value map using OR logic
     *
     * @param values The value map to compare
     * @return true
     * @return false
     */
    bool IsMatch(const std::map<std::string, std::string>& values) const;
};

/**
 * @brief Create a ServiceSubscriptionFilterAttribute from a regex string
 *
 * @param regex_value The regex string to compile
 * @return ServiceSubscriptionFilterAttribute The created attribute
 */
ROBOTRACONTEUR_CORE_API ServiceSubscriptionFilterAttribute
CreateServiceSubscriptionFilterAttributeRegex(boost::string_ref regex_value);
/**
 * @brief Create a ServiceSubscriptionFilterAttribute from a regex string
 *
 * @param name The attribute name
 * @param regex_value The regex string to compile
 * @return ROBOTRACONTEUR_CORE_API
 */
ROBOTRACONTEUR_CORE_API ServiceSubscriptionFilterAttribute
CreateServiceSubscriptionFilterAttributeRegex(boost::string_ref name, boost::string_ref regex_value);

/**
 * @brief Create a ServiceSubscriptionFilterAttribute from a combined identifier string
 *
 * The identifier may be a name, UUID, or a combination of both using a "|" to separate the name and UUID.
 *
 * @param value The identifier as a string
 * @return ServiceSubscriptionFilterAttribute The created attribute
 */
ROBOTRACONTEUR_CORE_API
ServiceSubscriptionFilterAttribute CreateServiceSubscriptionFilterAttributeCombinedIdentifier(
    boost::string_ref combined_identifier);
/**
 * @brief Create a ServiceSubscriptionFilterAttribute from an identifier
 *
 * @param identifier_name The identifier name
 * @param uuid_string The identifier UUID as a string
 * @return ServiceSubscriptionFilterAttribute The created attribute
 */
ROBOTRACONTEUR_CORE_API ServiceSubscriptionFilterAttribute
CreateServiceSubscriptionFilterAttributeIdentifier(boost::string_ref identifier_name, boost::string_ref uuid_string);
/**
 * @brief Create a ServiceSubscriptionFilterAttribute from an identifier
 *
 * @param name The attribute name
 * @param identifier_name The identifier name
 * @param uuid_string The identifier UUID as a string
 * @return ServiceSubscriptionFilterAttribute The created attribute
 */
ROBOTRACONTEUR_CORE_API ServiceSubscriptionFilterAttribute CreateServiceSubscriptionFilterAttributeIdentifier(
    boost::string_ref name, boost::string_ref identifier_name, boost::string_ref uuid_string);

/**
 * @brief Comparison operations for ServiceSubscriptionFilterAttributeGroup
 *
 */
enum ServiceSubscriptionFilterAttributeGroupOperation
{
    /** @brief OR operation */
    ServiceSubscriptionFilterAttributeGroupOperation_OR,
    /** @brief AND operation */
    ServiceSubscriptionFilterAttributeGroupOperation_AND,
    /** @brief NOR operation. Also used for NOT */
    ServiceSubscriptionFilterAttributeGroupOperation_NOR,
    /** @brief NAND operation */
    ServiceSubscriptionFilterAttributeGroupOperation_NAND
};

/**
 * @brief Subscription filter attribute group for use with ServiceSubscriptionFilter
 *
 * Used to combine multiple ServiceSubscriptionFilterAttribute objects for comparison using
 * AND, OR, NOR, or NAND logic. Other groups can be nested, to allow for complex comparisons.
 */
class ROBOTRACONTEUR_CORE_API ServiceSubscriptionFilterAttributeGroup
{
  public:
    /** @brief The attributes in the group */
    std::vector<ServiceSubscriptionFilterAttribute> Attributes;
    /** @brief The nested groups in the group */
    std::vector<ServiceSubscriptionFilterAttributeGroup> Groups;
    /** @brief The operation to use for matching the attributes and groups */
    ServiceSubscriptionFilterAttributeGroupOperation Operation;

    /** @brief True if string attributes will be split into a list with delimiter (default ",") */
    bool SplitStringAttribute;
    /** @brief Delimiter to use to split string attributes (default ",")*/
    char SplitStringDelimiter;

    /**
     * @brief Construct a new Service Subscription Filter Attribute Group object
     *
     */
    ServiceSubscriptionFilterAttributeGroup();
    /**
     * @brief Construct a new Service Subscription Filter Attribute Group object
     *
     * @param operation The operation to use for matching the attributes and groups
     */
    ServiceSubscriptionFilterAttributeGroup(ServiceSubscriptionFilterAttributeGroupOperation operation);
    /**
     * @brief Construct a new Service Subscription Filter Attribute Group object
     *
     * @param operation The operation to use for matching the attributes and groups
     * @param attributes The attributes in the group
     */
    ServiceSubscriptionFilterAttributeGroup(ServiceSubscriptionFilterAttributeGroupOperation operation,
                                            std::vector<ServiceSubscriptionFilterAttribute> attributes);
    /**
     * @brief Construct a new Service Subscription Filter Attribute Group object
     *
     * @param operation The operation to use for matching the attributes and groups
     * @param groups The nested groups in the group
     */
    ServiceSubscriptionFilterAttributeGroup(ServiceSubscriptionFilterAttributeGroupOperation operation,
                                            std::vector<ServiceSubscriptionFilterAttributeGroup> groups);

    /**
     * @brief Compare the group to a value
     *
     * @param value The value to compare
     * @return true
     * @return false
     */
    bool IsMatch(boost::string_ref value) const;

    /**
     * @brief Compare the group to a value
     *
     * @param value The value to compare
     * @return true
     * @return false
     */
    bool IsMatch(RR_INTRUSIVE_PTR<RRArray<char> >& value) const;

    /**
     * @brief Compare the group to a list of values
     *
     * @param values The values to compare
     * @return true
     * @return false
     */
    bool IsMatch(const std::vector<std::string>& values) const;
    /**
     * @brief Compare the group to a list of values
     *
     * @param values The values to compare
     * @return true
     * @return false
     */
    bool IsMatch(const RR_INTRUSIVE_PTR<RRList<RRValue> >& values) const;
    /**
     * @brief Compare the group to a map of values
     *
     * @param values The values to compare
     * @return true
     * @return false
     */
    bool IsMatch(const RR_INTRUSIVE_PTR<RRMap<std::string, RRValue> >& values) const;
    /**
     * @brief Compare the group to a map of values
     *
     * @param values The values to compare
     * @return true
     * @return false
     */
    bool IsMatch(const std::map<std::string, std::string>& values) const;

    bool IsMatch(const RR_INTRUSIVE_PTR<RRValue>& value) const;
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
    /** Attributes to match */
    std::map<std::string, ServiceSubscriptionFilterAttributeGroup> Attributes;
    /** Operation to use to match attributes. Defaults to AND */
    ServiceSubscriptionFilterAttributeGroupOperation AttributesMatchOperation;
    /** A user specified predicate function. If nullptr, the predicate is not checked. **/
    boost::function<bool(const ServiceInfo2&)> Predicate;
    /** The maximum number of connections the subscription will create. Zero means unlimited connections. **/
    uint32_t MaxConnections;

    ServiceSubscriptionFilter();
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
                      const RR_SHARED_PTR<ServiceSubscriptionFilter>& filter) = 0;
    virtual void NodeUpdated(RR_SHARED_PTR<detail::Discovery_nodestorage> storage) = 0;
    virtual void NodeLost(RR_SHARED_PTR<detail::Discovery_nodestorage> storage) = 0;

  public:
    virtual void Close() = 0;
    virtual ~IServiceSubscription(){};
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
    friend class ServiceSubscriptionManager;

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
        boost::function<void(const RR_SHARED_PTR<ServiceInfo2Subscription>&, const ServiceSubscriptionClientID&,
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
        boost::function<void(const RR_SHARED_PTR<ServiceInfo2Subscription>&, const ServiceSubscriptionClientID&,
                             const ServiceInfo2&)>
            handler);

    /**
     * @brief Close the subscription
     *
     * Closes the subscription. Subscriptions are automatically closed when the node is shut down.
     *
     */
    RR_OVIRTUAL void Close() RR_OVERRIDE;

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

    boost::signals2::signal<void(const RR_SHARED_PTR<ServiceInfo2Subscription>&, const ServiceSubscriptionClientID&,
                                 const ServiceInfo2&)>
        detected_listeners;
    boost::signals2::signal<void(const RR_SHARED_PTR<ServiceInfo2Subscription>&, const ServiceSubscriptionClientID&,
                                 const ServiceInfo2&)>
        lost_listeners;

  public:
    // Do not call, use RobotRaconteurNode()->SubscribeServiceInfo2()
    ServiceInfo2Subscription(const RR_SHARED_PTR<detail::Discovery>& parent);

  protected:
    RR_OVIRTUAL void Init(const std::vector<std::string>& service_types,
                          const RR_SHARED_PTR<ServiceSubscriptionFilter>& filter) RR_OVERRIDE;
    RR_OVIRTUAL void NodeUpdated(RR_SHARED_PTR<detail::Discovery_nodestorage> storage) RR_OVERRIDE;
    RR_OVIRTUAL void NodeLost(RR_SHARED_PTR<detail::Discovery_nodestorage> storage) RR_OVERRIDE;

    void fire_ServiceDetectedListener(const ServiceSubscriptionClientID& noden, const ServiceInfo2& info);
    void fire_ServiceLostListener(const ServiceSubscriptionClientID& noden, const ServiceInfo2& info);
};

namespace detail
{
template <typename T>
void AsyncGetDefaultClient_handler_adapter(
    const boost::function<void(const RR_SHARED_PTR<T>&, const RR_SHARED_PTR<RobotRaconteurException>&)>& handler,
    const RR_SHARED_PTR<RRObject>& obj, const RR_SHARED_PTR<RobotRaconteurException>& err)
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
    friend class SubObjectSubscription;
    friend class ServiceSubscriptionManager;

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
        boost::function<void(const RR_SHARED_PTR<ServiceSubscription>&, const ServiceSubscriptionClientID&,
                             const RR_SHARED_PTR<RRObject>&)>
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
        boost::function<void(const RR_SHARED_PTR<ServiceSubscription>&, const ServiceSubscriptionClientID&,
                             const RR_SHARED_PTR<RRObject>&)>
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
        boost::function<void(const RR_SHARED_PTR<ServiceSubscription>&, const ServiceSubscriptionClientID&,
                             const std::vector<std::string>&, const RR_SHARED_PTR<RobotRaconteurException>&)>
            handler);

    /**
     * @brief Close the subscription
     *
     * Close the subscription. Subscriptions are automatically closed when the node is shut down.
     *
     */
    RR_OVIRTUAL void Close() RR_OVERRIDE;

    /**
     * @brief Claim a client that was connected by the subscription
     *
     * The subscription class will automatically manage the lifecycle of the connected clients. The clients
     * will be automatically disconnected and/or reconnected as necessary. If the user wants to disable
     * this behavior for a specific client connection, the client connection can be "claimed".
     *
     * @param client The client to be claimed
     */
    virtual void ClaimClient(const RR_SHARED_PTR<RRObject>& client);

    /**
     * @brief Release a client previously clamed with ClaimClient()
     *
     * Lifecycle management is returned to the subscription
     *
     * @param client The client to release claim
     */
    virtual void ReleaseClient(const RR_SHARED_PTR<RRObject>& client);

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
     * call GetDefaultClient() right before using the client to make sure the most recently connection
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
    void AsyncGetDefaultClient(
        boost::function<void(const RR_SHARED_PTR<T>&, const RR_SHARED_PTR<RobotRaconteurException>&)> handler,
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
    void UpdateServiceURL(boost::string_ref url, boost::string_ref username = "",
                          const RR_INTRUSIVE_PTR<RRMap<std::string, RRValue> >& credentials =
                              (RR_INTRUSIVE_PTR<RRMap<std::string, RRValue> >()),
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
    void UpdateServiceURL(const std::vector<std::string>& url, boost::string_ref username = "",
                          const RR_INTRUSIVE_PTR<RRMap<std::string, RRValue> >& credentials =
                              (RR_INTRUSIVE_PTR<RRMap<std::string, RRValue> >()),
                          boost::string_ref object_type = "", bool close_connected = false);

    /**
     * @brief Update the service type and filter for subscription
     *
     * Updates the existing target service types and filter for a running subscription
     *
     * @param service_types A std::vector of service types to listen for, ie `com.robotraconteur.robotics.robot.Robot`
     * @param filter A filter to select individual services based on specified criteria
     */

    void UpdateServiceByType(
        const std::vector<std::string>& service_types,
        const RR_SHARED_PTR<ServiceSubscriptionFilter>& filter = RR_SHARED_PTR<ServiceSubscriptionFilter>());

    /**
     * @brief Creates a sub object subscription
     *
     * Sub objects are objects within a service that are not the root object. Sub objects are typically
     * referenced using objref members, however they can also be referenced using a service path.
     * The SubObjectSubscription class is used to automatically access sub objects of the default client.
     *
     * The service path is broken up into segments using periods. See the Robot Raconter
     * documentation for more information. The BuildServicePath() function can be used to assist
     * building service paths. The first level of the* service path may be "*" to match any service name.
     * For instance, the service path "*.sub_obj" will match any service name, and use the "sub_obj" objref
     *
     * @param servicepath The service path of the sub object
     * @param objecttype Optional object type to use for the sub object
     * @return RR_SHARED_PTR<SubObjectSubscription> The sub object subscription
     */
    RR_SHARED_PTR<SubObjectSubscription> SubscribeSubObject(boost::string_ref servicepath,
                                                            boost::string_ref objecttype = "");

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

    boost::signals2::signal<void(const RR_SHARED_PTR<ServiceSubscription>&, const ServiceSubscriptionClientID&,
                                 const RR_SHARED_PTR<RRObject>&)>
        connect_listeners;
    boost::signals2::signal<void(const RR_SHARED_PTR<ServiceSubscription>&, const ServiceSubscriptionClientID&,
                                 const RR_SHARED_PTR<RRObject>&)>
        disconnect_listeners;
    boost::signals2::signal<void(const RR_SHARED_PTR<ServiceSubscription>&, const ServiceSubscriptionClientID&,
                                 const std::vector<std::string>&, const RR_SHARED_PTR<RobotRaconteurException>&)>
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
    ServiceSubscription(const RR_SHARED_PTR<detail::Discovery>& parent);

  protected:
    RR_OVIRTUAL void Init(const std::vector<std::string>& service_types,
                          const RR_SHARED_PTR<ServiceSubscriptionFilter>& filter) RR_OVERRIDE;
    virtual void InitServiceURL(const std::vector<std::string>& url, boost::string_ref username = "",
                                const RR_INTRUSIVE_PTR<RRMap<std::string, RRValue> >& credentials =
                                    (RR_INTRUSIVE_PTR<RRMap<std::string, RRValue> >()),
                                boost::string_ref objecttype = "");
    RR_OVIRTUAL void NodeUpdated(RR_SHARED_PTR<detail::Discovery_nodestorage> storage) RR_OVERRIDE;
    RR_OVIRTUAL void NodeLost(RR_SHARED_PTR<detail::Discovery_nodestorage> storage) RR_OVERRIDE;

    void ClientConnected(const RR_SHARED_PTR<RRObject>& c, const RR_SHARED_PTR<RobotRaconteurException>& err,
                         const RR_SHARED_PTR<detail::ServiceSubscription_client>& c2,
                         const std::vector<std::string>& url);
    void ConnectRetry(const RR_SHARED_PTR<detail::ServiceSubscription_client>& c2);
    void ConnectRetry2(const RR_SHARED_PTR<detail::ServiceSubscription_client>& c2);

    static void ClientEvent(RR_WEAK_PTR<ServiceSubscription> this_, const RR_SHARED_PTR<ClientContext>& ctx,
                            ClientServiceListenerEventType evt, const RR_SHARED_PTR<void>& p,
                            RR_WEAK_PTR<detail::ServiceSubscription_client> c2);

    void fire_ClientConnectListeners(const ServiceSubscriptionClientID& noden, const RR_SHARED_PTR<RRObject>& client);
    void fire_ClientDisconnectListeners(const ServiceSubscriptionClientID& noden,
                                        const RR_SHARED_PTR<RRObject>& client);
    void fire_ClientConnectFailedListeners(const ServiceSubscriptionClientID& noden,
                                           const std::vector<std::string>& url,
                                           const RR_SHARED_PTR<RobotRaconteurException>& err);

    void SubscribeWire1(const RR_SHARED_PTR<WireSubscriptionBase>& s);
    void SubscribePipe1(const RR_SHARED_PTR<PipeSubscriptionBase>& s);

    void WireSubscriptionClosed(const RR_SHARED_PTR<WireSubscriptionBase>& s);
    void PipeSubscriptionClosed(const RR_SHARED_PTR<PipeSubscriptionBase>& s);

    RR_SHARED_PTR<RRObject> GetDefaultClientBase();
    bool TryGetDefaultClientBase(RR_SHARED_PTR<RRObject>& client_out);

    RR_SHARED_PTR<RRObject> GetDefaultClientWaitBase(int32_t timeout = RR_TIMEOUT_INFINITE);
    bool TryGetDefaultClientWaitBase(RR_SHARED_PTR<RRObject>& client_out, int32_t timeout = RR_TIMEOUT_INFINITE);

    void AsyncGetDefaultClientBase(
        boost::function<void(const RR_SHARED_PTR<RRObject>&, const RR_SHARED_PTR<RobotRaconteurException>&)> handler,
        int32_t timeout = RR_TIMEOUT_INFINITE);

    void SoftClose();
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

    virtual ~WireSubscriptionBase() {}

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

    WireSubscriptionBase(const RR_SHARED_PTR<ServiceSubscription>& parent, boost::string_ref membername,
                         boost::string_ref servicepath);

    RR_SHARED_PTR<RobotRaconteurNode> GetNode();

  protected:
    void ClientConnected(const ServiceSubscriptionClientID& client_id, const RR_SHARED_PTR<RRObject>& client);
    void ClientDisconnected(const ServiceSubscriptionClientID& client_id, const RR_SHARED_PTR<RRObject>& client);

    void WireConnectionClosed(const RR_SHARED_PTR<detail::WireSubscription_connection>& wire);
    void WireValueChanged(const RR_SHARED_PTR<detail::WireSubscription_connection>& wire,
                          const RR_INTRUSIVE_PTR<RRValue>& value, const TimeSpec& time);

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

    virtual void fire_WireValueChanged(const RR_INTRUSIVE_PTR<RRValue>& value, const TimeSpec& time,
                                       const RR_SHARED_PTR<WireConnectionBase>& connection);
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
    WireSubscription(const RR_SHARED_PTR<ServiceSubscription>& parent, boost::string_ref membername,
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
        boost::function<void(const RR_SHARED_PTR<WireSubscription<T> >&, const T&, const TimeSpec&)> f)
    {
        return wire_value_changed.connect(f);
    }

  protected:
    boost::signals2::signal<void(const RR_SHARED_PTR<WireSubscription<T> >&, const T&, const TimeSpec&)>
        wire_value_changed;

    RR_OVIRTUAL void fire_WireValueChanged(const RR_INTRUSIVE_PTR<RRValue>& value, const TimeSpec& time,
                                           const RR_SHARED_PTR<WireConnectionBase>& connection) RR_OVERRIDE
    {
        RR_UNUSED(connection);
        wire_value_changed(RR_STATIC_POINTER_CAST<WireSubscription<T> >(shared_from_this()),
                           RRPrimUtil<T>::PreUnpack(value), time);
    }

    RR_OVIRTUAL bool isempty_WireValueChanged() RR_OVERRIDE { return wire_value_changed.empty(); }
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

    virtual ~PipeSubscriptionBase() {}

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

    PipeSubscriptionBase(const RR_SHARED_PTR<ServiceSubscription>& parent, boost::string_ref membername,
                         boost::string_ref servicepath = "", int32_t max_recv_packets = -1,
                         int32_t max_send_backlog = 5);

    RR_SHARED_PTR<RobotRaconteurNode> GetNode();

  protected:
    void ClientConnected(const ServiceSubscriptionClientID& client_id, const RR_SHARED_PTR<RRObject>& client);
    void ClientDisconnected(const ServiceSubscriptionClientID& client_id, const RR_SHARED_PTR<RRObject>& client);

    void PipeEndpointClosed(const RR_SHARED_PTR<detail::PipeSubscription_connection>& pipe);
    void PipeEndpointPacketReceived(const RR_SHARED_PTR<detail::PipeSubscription_connection>& pipe,
                                    const RR_INTRUSIVE_PTR<RRValue>& value);

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
    PipeSubscription(const RR_SHARED_PTR<ServiceSubscription>& parent, boost::string_ref membername,
                     boost::string_ref servicepath = "", int32_t max_recv_packets = -1, int32_t max_send_backlog = 5)
        : PipeSubscriptionBase(parent, membername, servicepath, max_recv_packets)
    {
        RR_UNUSED(max_send_backlog);
    }

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
    event_connection AddPipePacketReceivedListener(boost::function<void(const RR_SHARED_PTR<PipeSubscription<T> >&)> f)
    {
        return pipe_packet_received.connect(f);
    }

  protected:
    boost::signals2::signal<void(const RR_SHARED_PTR<PipeSubscription<T> >&)> pipe_packet_received;

    RR_OVIRTUAL void fire_PipePacketReceived() RR_OVERRIDE
    {
        pipe_packet_received(RR_STATIC_POINTER_CAST<PipeSubscription<T> >(shared_from_this()));
    }

    RR_OVIRTUAL bool isempty_PipePacketReceived() RR_OVERRIDE { return pipe_packet_received.empty(); }
};

/**
 * @brief Subscription for sub objects of the default client
 *
 * SubObjectSubscription is used to access sub objects of the default client. Sub objects are objects within a service
 * that are not the root object. Sub objects are typically referenced using objref members, however they can also be
 * referenced using a service path. The SubObjectSubscription class is used to automatically access sub objects of the
 * default client.
 *
 * Use ServiceSubscription::SubscribeSubObject() to create a SubObjectSubscription.
 *
 * This class should not be used to access Pipe or Wire members. Use the ServiceSubscription::SubscribePipe() and
 * ServiceSubscription::SubscribeWire() functions to access Pipe and Wire members.
 *
 */
class ROBOTRACONTEUR_CORE_API SubObjectSubscription : public RR_ENABLE_SHARED_FROM_THIS<SubObjectSubscription>,
                                                      private boost::noncopyable
{

  public:
    friend class ServiceSubscription;

    virtual ~SubObjectSubscription() {}

    /**
     * @brief Get the "default client" sub object
     *
     * The sub object is retrieved from the default client. The default client is the first client
     * that connected to the service. If no clients are currently connected, an exception is thrown.
     *
     * Clients using GetDefaultClient() should not store a reference to the client. Call GetDefaultClient()
     * each time the client is needed.
     *
     * @tparam T The type of the sub object
     * @return RR_SHARED_PTR<T> The sub object
     */
    template <typename T>
    RR_SHARED_PTR<T> GetDefaultClient()
    {
        return rr_cast<T>(GetDefaultClientBase());
    }

    /**
     * @brief Try getting the "default client" sub object
     *
     * Same as GetDefaultClient(), but returns a bool for success or failure instead of throwing
     * an exception on failure.
     *
     * @tparam T The type of the sub object
     * @param client_out [out] The sub object
     * @return true The sub object was retrieved successfully
     * @return false The sub object could not be retrieved
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
     * @brief Get the "default client" sub object, waiting for a specified timeout
     *
     The sub object is retrieved from the default client. The default client is the first client
     * that connected to the service. If no clients are currently connected, an exception is thrown.
     *
     * Clients using GetDefaultClient() should not store a reference to the client. Call GetDefaultClient()
     * each time the client is needed.
     *
     * This function blocks the current thread until the client is retrieved or the timeout is reached.
     *
     * @tparam T The type of the sub object
     * @param timeout The timeout in milliseconds
     * @return RR_SHARED_PTR<T> The sub object
     */
    template <typename T>
    RR_SHARED_PTR<T> GetDefaultClientWait(int32_t timeout = RR_TIMEOUT_INFINITE)
    {
        return rr_cast<T>(GetDefaultClientWaitBase(timeout));
    }

    /**
     * @brief Try getting the "default client" sub object, waiting for a specified timeout
     *
     * Same as GetDefaultClientWait(), but returns a bool for success or failure instead of throwing
     * an exception on failure.
     *
     * @tparam T The type of the sub object
     * @param client_out [out] The sub object
     * @param timeout The timeout in milliseconds
     * @return true The sub object was retrieved successfully
     * @return false The sub object could not be retrieved
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
     * @brief Asynchronously get the "default client" sub object
     *
     * Asynchronous version of GetDefaultClient()
     *
     * The handler function is called when the client is retrieved or an error occurs.
     *
     * @tparam T The type of the sub object
     * @param handler The handler function
     * @param timeout The timeout in milliseconds
     */
    template <typename T>
    void AsyncGetDefaultClient(
        boost::function<void(const RR_SHARED_PTR<T>&, const RR_SHARED_PTR<RobotRaconteurException>&)> handler,
        int32_t timeout = RR_TIMEOUT_INFINITE)
    {
        AsyncGetDefaultClientBase(boost::bind(&detail::AsyncGetDefaultClient_handler_adapter<T>, handler,
                                              RR_BOOST_PLACEHOLDERS(_1), RR_BOOST_PLACEHOLDERS(_2)),
                                  timeout);
    }

    void Init();

    /**
     * @brief Closes the sub object subscription
     *
     * Sub object subscriptions are automatically closed when the parent ServiceSubscription is closed
     * or when the node is shut down.
     *
     */
    void Close();

    SubObjectSubscription(const RR_SHARED_PTR<ServiceSubscription>& parent, boost::string_ref servicepath,
                          boost::string_ref objecttype);

    RR_SHARED_PTR<RobotRaconteurNode> GetNode();

  protected:
    RR_SHARED_PTR<RRObject> GetDefaultClientBase();
    bool TryGetDefaultClientBase(RR_SHARED_PTR<RRObject>& client_out);

    RR_SHARED_PTR<RRObject> GetDefaultClientWaitBase(int32_t timeout = RR_TIMEOUT_INFINITE);
    bool TryGetDefaultClientWaitBase(RR_SHARED_PTR<RRObject>& client_out, int32_t timeout = RR_TIMEOUT_INFINITE);

    void AsyncGetDefaultClientBase(
        boost::function<void(const RR_SHARED_PTR<RRObject>&, const RR_SHARED_PTR<RobotRaconteurException>&)> handler,
        int32_t timeout = RR_TIMEOUT_INFINITE);

    RR_WEAK_PTR<ServiceSubscription> parent;
    RR_WEAK_PTR<RobotRaconteurNode> node;
    std::string servicepath;
    std::string objecttype;
};

/**
 * @brief Connection method for ServiceSubscriptionManager subscription
 *
 * Select between using URLs or service types for subscription
 *
 */
enum ServiceSubscriptionManager_CONNECTION_METHOD
{
    /** @brief Implicitly select between URL and service types */
    ServiceSubscriptionManager_CONNECTION_METHOD_DEFAULT,
    /** @brief Use URLs types for subscription */
    ServiceSubscriptionManager_CONNECTION_METHOD_URL,
    /** @brief Use service types for subscription */
    ServiceSubscriptionManager_CONNECTION_METHOD_TYPE
};

/**
 * @brief ServiceSubscriptionManager subscription connection information
 *
 * Contains the connection information for a ServiceSubscriptionManager subscription
 * and the local name of the subscription
 */
struct ROBOTRACONTEUR_CORE_API ServiceSubscriptionManagerDetails
{
    /** @brief The local name of the subscription */
    std::string Name;
    /** @brief The connection method to use, URL or service type */
    ServiceSubscriptionManager_CONNECTION_METHOD ConnectionMethod;
    /** @brief The URLs to use for subscription */
    std::vector<std::string> Urls;
    /** @brief The username to use for URLs (optional)*/
    std::string UrlUsername;
    /** @brief The credentials to use for URLs (optional)*/
    RR_INTRUSIVE_PTR<RRMap<std::string, RRValue> > UrlCredentials;
    /** @brief The service types to use for subscription */
    std::vector<std::string> ServiceTypes;
    /** @brief The filter to use for subscription when service type is used (optional) */
    RR_SHARED_PTR<ServiceSubscriptionFilter> Filter;
    /** @brief If the subscription is enabled */
    bool Enabled;

    /** @brief Construct a new ServiceSubscriptionManagerDetails object */
    ServiceSubscriptionManagerDetails();

    /**
     * @brief Construct a new ServiceSubscriptionManagerDetails object with parameters
     *
     * @param Name The local name of the subscription
     * @param ConnectionMethod The connection method to use, URL or service type
     * @param Urls The URLs to use for subscription
     * @param UrlUsername The username to use for URLs (optional)
     * @param UrlCredentials The credentials to use for URLs (optional)
     * @param ServiceTypes The service types to use for subscription
     * @param Filter The filter to use for subscription when service type is used (optional)
     * @param Enabled If the subscription is enabled
     */
    ServiceSubscriptionManagerDetails(
        const boost::string_ref& Name,
        ServiceSubscriptionManager_CONNECTION_METHOD ConnectionMethod =
            ServiceSubscriptionManager_CONNECTION_METHOD_DEFAULT,
        const std::vector<std::string>& Urls = std::vector<std::string>(), const boost::string_ref& UrlUsername = "",
        const RR_INTRUSIVE_PTR<RRMap<std::string, RRValue> >& UrlCredentials =
            RR_INTRUSIVE_PTR<RRMap<std::string, RRValue> >(),
        const std::vector<std::string>& ServiceTypes = std::vector<std::string>(),
        const RR_SHARED_PTR<ServiceSubscriptionFilter>& Filter = RR_SHARED_PTR<ServiceSubscriptionFilter>(),
        bool Enabled = true);
};

/**
 * @brief Class to manage multiple subscriptions to services
 *
 * ServiceSubscriptionManager is used to manage multiple subscriptions to services. Subscriptions
 * are created using information contained in ServiceSubscriptionManagerDetails structures. The subscriptions
 * can connect using URLs or service types. The subscriptions can be enabled or disabled, and can be
 * closed.
 *
 */
class ROBOTRACONTEUR_CORE_API ServiceSubscriptionManager
{
  public:
    /**
     * @brief Construct a new ServiceSubscriptionManager object
     *
     * @param node (optional) The node to use for the subscription manager
     */
    ServiceSubscriptionManager(const RR_SHARED_PTR<RobotRaconteurNode>& node = RobotRaconteurNode::sp());

    virtual ~ServiceSubscriptionManager();

    /**
     * @brief Initialize the subscription manager with a list of subscriptions
     *
     * @param details The list of subscriptions to initialize
     */
    void Init(const std::vector<ServiceSubscriptionManagerDetails>& details);

    /**
     * @brief Add a subscription to the manager
     *
     * @param details The subscription to add
     */
    void AddSubscription(const ServiceSubscriptionManagerDetails& details);

    /**
     * @brief Remove a subscription from the manager
     *
     * @param name The local name of the subscription to remove
     * @param close (optional) If true, close the subscription
     */
    void RemoveSubscription(const boost::string_ref& name, bool close = true);

    /**
     * @brief Enable a subscription
     *
     * @param name The local name of the subscription to enable
     */
    void EnableSubscription(const boost::string_ref& name);

    /**
     * @brief Disable a subscription
     *
     * @param name The local name of the subscription to disable
     * @param close (optional) If true, close subscription if connected
     */
    void DisableSubscription(const boost::string_ref& name, bool close = true);

    /**
     * @brief Get a subscription by name
     *
     * @param name The local name of the subscription
     * @param force_create (optional) If true, create the subscription if it does not exist
     * @return RR_SHARED_PTR<ServiceSubscription> The subscription
     */
    RR_SHARED_PTR<ServiceSubscription> GetSubscription(const boost::string_ref& name, bool force_create = true);

    /**
     * @brief Get if a subscription is connected
     *
     * @param name The local name of the subscription
     * @return bool True if the subscription is connected
     */
    bool IsConnected(const boost::string_ref& name);

    /**
     * @brief Get if a subscription is enabled
     *
     * @param name The local name of the subscription
     * @return bool True if the subscription is enabled
     */
    bool IsEnabled(const boost::string_ref& name);

    /**
     * @brief Close the subscription manager
     *
     * @param close_subscriptions (optional) If true, close all subscriptions
     */
    void Close(bool close_subscriptions = true);

    /**
     * @brief Get the names of all subscriptions
     *
     * @return std::vector<std::string> The list of subscription names
     */
    std::vector<std::string> GetSubscriptionNames();

    /**
     * @brief Get the details of all subscriptions
     *
     * @return std::vector<ServiceSubscriptionManagerDetails> The list of subscription details
     */
    std::vector<ServiceSubscriptionManagerDetails> GetSubscriptionDetails();

    /**
     * @brief Get the node used by the subscription manager
     *
     * @return RR_SHARED_PTR<RobotRaconteurNode> The node
     */
    RR_SHARED_PTR<RobotRaconteurNode> GetNode();

  protected:
    RR_WEAK_PTR<RobotRaconteurNode> node;

    boost::mutex this_lock;

    boost::unordered_map<std::string, detail::ServiceSubscriptionManager_subscription> subscriptions;

    RR_SHARED_PTR<ServiceSubscription> CreateSubscription(const ServiceSubscriptionManagerDetails& details);

    void UpdateSubscription(detail::ServiceSubscriptionManager_subscription& sub,
                            const ServiceSubscriptionManagerDetails& details, bool close = false);
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
    WireSubscription_send_iterator(const RR_SHARED_PTR<WireSubscriptionBase>& subscription);
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
    PipeSubscription_send_iterator(const RR_SHARED_PTR<PipeSubscriptionBase>& subscription);
    RR_SHARED_PTR<PipeEndpointBase> Next();
    void AsyncSendPacket(const RR_INTRUSIVE_PTR<RRValue>& packet);
    virtual ~PipeSubscription_send_iterator();
};

class ROBOTRACONTEUR_CORE_API ServiceSubscription_custom_member_subscribers
{
  public:
    static void SubscribeWire(const RR_SHARED_PTR<ServiceSubscription>& s,
                              const RR_SHARED_PTR<WireSubscriptionBase>& o);
    static void SubscribePipe(const RR_SHARED_PTR<ServiceSubscription>& s,
                              const RR_SHARED_PTR<PipeSubscriptionBase>& o);
};
} // namespace detail

#ifndef ROBOTRACONTEUR_NO_CXX11_TEMPLATE_ALIASES
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
/** @brief Convenience alias for SubObjectSubscription shared_ptr */
using SubObjectSubscriptionPtr = RR_SHARED_PTR<SubObjectSubscription>;
#endif

} // namespace RobotRaconteur
