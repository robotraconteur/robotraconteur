/**
 * @file Service.h
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

#pragma once

#include "RobotRaconteur/RobotRaconteurNode.h"
#include "RobotRaconteur/DataTypes.h"
#include "RobotRaconteur/ServiceFactory.h"
#include "RobotRaconteur/ServiceDefinition.h"
#include "RobotRaconteur/ServiceStructure.h"
#include <istream>
#include <boost/atomic.hpp>

#pragma warning(push)
#pragma warning(disable : 4996)
#include <boost/signals2.hpp>

namespace RobotRaconteur
{

class ROBOTRACONTEUR_CORE_API ServerEndpoint;
class ROBOTRACONTEUR_CORE_API AuthenticatedUser;
class ROBOTRACONTEUR_CORE_API ServerContext;
class ROBOTRACONTEUR_CORE_API ServerContext_ObjectLock;
class ROBOTRACONTEUR_CORE_API ServerContext_MonitorObjectSkel;

class ROBOTRACONTEUR_CORE_API GeneratorServerBase;

class ROBOTRACONTEUR_CORE_API IRobotRaconteurMonitorObject;

/**
 * @brief Interface for service objects to receive service notifications
 *
 * Service objects are passed to the service, either when the service is registered
 * or using objrefs. The service initialized the object by configuring events,
 * pipes, callbacks, and wires for use. The object may implement IRRServiceObject
 * to receive notification of when this process is complete, and to receive
 * a ServerContextPtr and the service path of the object.
 * IRRServiceObject::RRServiceObjectInit() is called after the object has been
 * initialized to provide this information.
 *
 */
class ROBOTRACONTEUR_CORE_API IRRServiceObject
{
  public:
    /**
     * @brief Function called after service object has been initialized
     *
     * Override in the service object to receive notification the service object has
     * been initialized, a ServerContextPtr, and the service path.
     *
     * @param context The ServerContextPtr owning the object
     * @param service_path The object service path
     */
    virtual void RRServiceObjectInit(RR_WEAK_PTR<ServerContext> context, const std::string& service_path) = 0;
};

class ROBOTRACONTEUR_CORE_API ServiceSkel : public RR_ENABLE_SHARED_FROM_THIS<ServiceSkel>, private boost::noncopyable
{

  public:
    ServiceSkel();
    virtual ~ServiceSkel() {}

    virtual void Init(boost::string_ref s, RR_SHARED_PTR<RRObject> o, RR_SHARED_PTR<ServerContext> c);

    virtual void InitCallbackServers(RR_SHARED_PTR<RRObject> o);

    std::string GetServicePath() const;

  protected:
    std::string m_ServicePath;

  public:
    RR_SHARED_PTR<ServerContext> GetContext();

  protected:
    RR_WEAK_PTR<ServerContext> m_context;

    RR_SHARED_PTR<RRObject> uncastobj;

    std::vector<boost::tuple<RobotRaconteurVersion, std::string> > object_type_ver;

    boost::unordered_map<int32_t, RR_SHARED_PTR<GeneratorServerBase> > generators;
    boost::mutex generators_lock;

    int32_t get_new_generator_index();

  public:
    RR_SHARED_PTR<RRObject> GetUncastObject() const;

    virtual RR_INTRUSIVE_PTR<MessageEntry> CallGetProperty(RR_INTRUSIVE_PTR<MessageEntry> m) = 0;

    virtual RR_INTRUSIVE_PTR<MessageEntry> CallSetProperty(RR_INTRUSIVE_PTR<MessageEntry> m) = 0;

    virtual RR_INTRUSIVE_PTR<MessageEntry> CallFunction(RR_INTRUSIVE_PTR<MessageEntry> m) = 0;

  protected:
    static void EndAsyncCallGetProperty(RR_WEAK_PTR<ServiceSkel> skel, RR_INTRUSIVE_PTR<MessageElement> value,
                                        RR_SHARED_PTR<RobotRaconteurException> err, RR_INTRUSIVE_PTR<MessageEntry> m,
                                        RR_SHARED_PTR<ServerEndpoint> ep);

    static void EndAsyncCallSetProperty(RR_WEAK_PTR<ServiceSkel> skel, RR_SHARED_PTR<RobotRaconteurException> err,
                                        RR_INTRUSIVE_PTR<MessageEntry> m, RR_SHARED_PTR<ServerEndpoint> ep);

    static void EndAsyncCallFunction(RR_WEAK_PTR<ServiceSkel> skel, RR_INTRUSIVE_PTR<MessageElement> ret,
                                     RR_SHARED_PTR<RobotRaconteurException> err, RR_INTRUSIVE_PTR<MessageEntry> m,
                                     RR_SHARED_PTR<ServerEndpoint> ep);

  public:
    virtual RR_SHARED_PTR<RRObject> GetSubObj(boost::string_ref name, boost::string_ref ind) = 0;

    RR_SHARED_PTR<RRObject> GetSubObj(boost::string_ref name);

    virtual void RegisterEvents(RR_SHARED_PTR<RRObject> obj1);

    virtual void UnregisterEvents(RR_SHARED_PTR<RRObject> obj1);

    virtual void InitPipeServers(RR_SHARED_PTR<RRObject> obj1);

    virtual void InitWireServers(RR_SHARED_PTR<RRObject> obj1);

    void ObjRefChanged(boost::string_ref name);

    void SendEvent(RR_INTRUSIVE_PTR<MessageEntry> m);

    void ReleaseObject();

    virtual void ReleaseCastObject() = 0;

    void AsyncSendPipeMessage(RR_INTRUSIVE_PTR<MessageEntry> m, uint32_t e, bool unreliable,
                              RR_MOVE_ARG(boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)>) handler);

    void SendWireMessage(RR_INTRUSIVE_PTR<MessageEntry> m, uint32_t e);

    virtual void DispatchPipeMessage(RR_INTRUSIVE_PTR<MessageEntry> m, uint32_t e);

    virtual void DispatchWireMessage(RR_INTRUSIVE_PTR<MessageEntry> m, uint32_t e);

    virtual RR_INTRUSIVE_PTR<MessageEntry> CallPipeFunction(RR_INTRUSIVE_PTR<MessageEntry> m, uint32_t e);

    virtual RR_INTRUSIVE_PTR<MessageEntry> CallWireFunction(RR_INTRUSIVE_PTR<MessageEntry> m, uint32_t e);

    virtual RR_SHARED_PTR<void> GetCallbackFunction(uint32_t endpoint, boost::string_ref membername);

    virtual RR_INTRUSIVE_PTR<MessageEntry> CallMemoryFunction(RR_INTRUSIVE_PTR<MessageEntry> m,
                                                              RR_SHARED_PTR<Endpoint> e);

    RR_WEAK_PTR<ServerContext_ObjectLock> objectlock;
    boost::mutex objectlock_lock;

    RR_SHARED_PTR<ServerContext_MonitorObjectSkel> monitorlock;

    RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<ServerContext_MonitorObjectSkel> > monitorlocks;
    boost::mutex monitorlocks_lock;

    bool IsLocked();

    virtual bool IsRequestNoLock(RR_INTRUSIVE_PTR<MessageEntry> m);

    bool IsMonitorLocked();

    virtual std::string GetObjectType() = 0;

    virtual std::string GetObjectType(RobotRaconteurVersion client_version);

    RR_SHARED_PTR<RobotRaconteurNode> RRGetNode();
    RR_WEAK_PTR<RobotRaconteurNode> RRGetNodeWeak();

    virtual void CallGeneratorNext(RR_INTRUSIVE_PTR<MessageEntry> m, RR_SHARED_PTR<Endpoint> e);

    void SendGeneratorResponse(int32_t index, RR_INTRUSIVE_PTR<MessageEntry> m, RR_SHARED_PTR<ServerEndpoint> ep);

    void CleanupGenerators();

  protected:
    RR_WEAK_PTR<RobotRaconteurNode> node;
};

class ROBOTRACONTEUR_CORE_API UserAuthenticator;

/**
 * @brief Context for services registered in a node for use by clients
 *
 * Services are registered using the RobotRaconteurNode::RegisterService() family of functions.
 * The ServerContext manages the services, and dispatches requests and packets to the appropriate
 * service object members. Services may expose more than one object. The root object is specified
 * when the service is registered. Other objects are specified through ObjRef members. A name
 * for the service is also specified when the service is registered. This name forms the root
 * of the service path namespace. Other objects in the service have a unique service path
 * based on the ObjRef used to access the object.
 *
 * Services may handle multiple connected clients concurrently. Each client is assigned
 * a ServerEndpoint. The ServerEndpoint is unique to the client connection,
 * and interacts with ServerContext to complete requests and dispatch packets. When
 * the service needs to address a specific client, the ServerEndpoint or the
 * ServerEndpoint::GetLocalEndpoint() is used. (ServerEndpoint::GetLocalEndpoint() returns the
 * uint32_t local client ID.)
 *
 * Service attributes are a varvalue{string} types dictionary that is made available to
 * clients during service discovery. These attributes are used to help clients determine
 * which service should be selected for use. Because the attributes are passed to the clients
 * as part of the discovery process, they should be as concise as possible, and should
 * not use user defined types. Use ServerContext::SetAttributes() to set the service attributes
 * after registering the service.
 *
 * Security for the service is specified using a ServiceSecurityPolicy instance. This policy
 * is specified by passing as a parameter to RobotRaconteurNode::RegisterService(), or passing
 * the policy to the constructor.
 *
 * ServerContext implements authentication and object locking.
 * Server side functions are exposed by ServerContext for authentication, object locking,
 * and client management.
 *
 * Clients using dynamic typing such as Python and MATLAB will only pull service types
 * explicitly imported by the root object and objref objects that have been requested. Clients
 * will not pull service types of user-defined named types if that service type is not explicitly
 * imported. This can be problematic if new `struct`, `pod`, and/or `namedarray` types are introduced
 * that do not have corresponding objects. Extra imports is used to specify extra service definitions
 * the client should pull. Use ServerContext::AddExtraImport(), ServerContext::RemoveExtraImport(),
 * and ServerContext::GetExtraImports() to manage the extra imports passed to the client.
 *
 */
class ROBOTRACONTEUR_CORE_API ServerContext : public RR_ENABLE_SHARED_FROM_THIS<ServerContext>, boost::noncopyable
{
  public:
    /**
     * @brief Get the service attributes
     *
     * Get the currently registered service attributes
     *
     * @return std::map<std::string, RR_INTRUSIVE_PTR<RRValue> > The service attributes
     */
    std::map<std::string, RR_INTRUSIVE_PTR<RRValue> > GetAttributes();

    /**
     * @brief Set the service attributes
     *
     * Sets the service attributes. Attributes are made available to clients during
     * service discovery. Attributes should be concise and not use any user defined
     * types.
     *
     * @param attr The service attributes
     */
    void SetAttributes(const std::map<std::string, RR_INTRUSIVE_PTR<RRValue> >& attr);

  private:
    std::map<std::string, RR_INTRUSIVE_PTR<RRValue> > m_Attributes;
    boost::mutex m_Attributes_lock;

  public:
    RR_SHARED_PTR<ServiceFactory> GetServiceDef() const;

    RR_SHARED_PTR<ServiceFactory> GetRootObjectServiceDef(RobotRaconteurVersion client_version);

    virtual ~ServerContext() {}

  protected:
    RR_SHARED_PTR<ServiceFactory> m_ServiceDef;

  public:
    /**
     * @brief Get the current vector of extra service definition imports
     *
     * @return std::vector<std::string> The vector of extra imports
     */
    std::vector<std::string> GetExtraImports();

    /**
     * @brief Add an extra service definition import
     *
     * Clients using dynamic typing will not automatically pull service definitions unless
     * imported by the root object or an objref. If new `struct`, `pod`, or `namedarray` types
     * are introduced in a new service definition type without a corresponding object, an error will
     * occur. Use AddExtraImport() to add the name of the new service definition to add it to the
     * list of service definitions the client will pull.
     *
     * Service definition must have been registered using RobotRaconteurNode::RegisterServiceType()
     *
     * @param import_ The name of the service definition
     */
    void AddExtraImport(boost::string_ref import_);

    /**
     * @brief Removes an extra import service definition registered with AddExtraImport()
     *
     * See AddExtraImport()
     *
     * @param import_ The name of the service definition
     * @return true The service definition was removed
     * @return false The service definition was not found in the extra imports vector
     */
    bool RemoveExtraImport(boost::string_ref import_);

  protected:
    std::vector<std::string> extra_imports;
    boost::mutex extra_imports_lock;

  public:
    /**
     * @brief Get the name of the service
     *
     * The service name forms the root of the service path used to specify
     * objects within the service
     *
     * @return std::string The name of the service
     */
    std::string GetServiceName() const;

  protected:
    std::string m_ServiceName;

    RR_UNORDERED_MAP<MessageStringPtr, RR_SHARED_PTR<ServiceSkel> > skels;
    boost::mutex skels_lock;

    RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<ServerEndpoint> > client_endpoints;
    boost::mutex client_endpoints_lock;

  public:
    std::string GetRootObjectType(RobotRaconteurVersion client_version);

  protected:
    std::string m_RootObjectType;

    RR_WEAK_PTR<RobotRaconteurNode> node;

    /*public Message SendRequest(Message m)
    {
        return null;
    }*/

  public:
    /**
     * @brief Construct a new ServerContext instance
     *
     * The ServerContext object is constructed automatically using
     * RobotRaconteurNode::RegisterService(boost::string_ref, boost::string_ref,
     * boost::shared_ptr<RRObject>,  boost::shared_ptr< ServiceSecurityPolicy > securitypolicy).
     * It is recommended to use this method instead of constructing ServerContext.
     *
     * Must use boost::make_shared<ServerContext>() to construct
     *
     * Call SetBaseObject() after construction
     *
     * Register using RobotRaconteurNode::RegisterServiceType()
     *
     * @param f The service factory for the root object
     * @param node The node that will own this service
     */
    ServerContext(RR_SHARED_PTR<ServiceFactory> f, RR_SHARED_PTR<RobotRaconteurNode> node);

    RR_SHARED_PTR<RobotRaconteurNode> GetNode();

    virtual void SendEvent(RR_INTRUSIVE_PTR<MessageEntry> m);
#undef SendMessage
    virtual void SendMessage(RR_INTRUSIVE_PTR<MessageEntry> m, RR_SHARED_PTR<Endpoint> e);

    virtual void SendMessage(RR_INTRUSIVE_PTR<MessageEntry> m, uint32_t e);

    virtual void AsyncSendMessage(RR_INTRUSIVE_PTR<MessageEntry> m, uint32_t e,
                                  boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)>& callback);

    virtual void AsyncSendMessage(RR_INTRUSIVE_PTR<MessageEntry> m, RR_SHARED_PTR<Endpoint> e,
                                  boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)>& callback);

    virtual void AsyncSendUnreliableMessage(RR_INTRUSIVE_PTR<MessageEntry> m, RR_SHARED_PTR<ServerEndpoint> e,
                                            boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)>& callback);

    virtual void AsyncSendPipeMessage(RR_INTRUSIVE_PTR<MessageEntry> m, uint32_t e, bool unreliable,
                                      boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)>& callback);

    virtual void SendWireMessage(RR_INTRUSIVE_PTR<MessageEntry> m, uint32_t e);

  private:
    // boost::mutex rec_sync;

    bool base_object_set;

  protected:
    /**
     * @brief Set the security policy of the service
     *
     * The security policy is set automatically using
     * RobotRaconteurNode::RegisterService(boost::string_ref, boost::string_ref,
     * boost::shared_ptr<RRObject>,  boost::shared_ptr< ServiceSecurityPolicy > securitypolicy).
     * It is recommended to use this method instead of SetSecurityPolicy().
     *
     * @param policy The security policy
     */
    void SetSecurityPolicy(RR_SHARED_PTR<ServiceSecurityPolicy> policy);

  public:
    virtual void SetBaseObject(boost::string_ref name, RR_SHARED_PTR<RRObject> o,
                               RR_SHARED_PTR<ServiceSecurityPolicy> policy = RR_SHARED_PTR<ServiceSecurityPolicy>());

    virtual RR_SHARED_PTR<ServiceSkel> GetObjectSkel(MessageStringRef servicepath);

    virtual void ReplaceObject(boost::string_ref path);

    virtual std::string GetObjectType(MessageStringRef servicepath, RobotRaconteurVersion client_version);

    /**
     * @brief Get the current ServerContext
     *
     * Returns the current server context during a request or packet event.
     * This is a thread-specific value and only
     * valid during the initial request or packet event invocation.
     *
     * @return RR_SHARED_PTR<ServerContext> The current server context
     */
    static RR_SHARED_PTR<ServerContext> GetCurrentServerContext();

  private:
    static boost::thread_specific_ptr<RR_SHARED_PTR<ServerContext> > m_CurrentServerContext;

  public:
    /**
     * @brief Get the current object service path
     *
     * Returns the service path of the current object during a request or
     * packet event.
     * This is a thread-specific value and only
     * valid during the initial request or packet event invocation.
     *
     * @return std::string The current object service path
     */
    static std::string GetCurrentServicePath();

  private:
    static boost::thread_specific_ptr<std::string> m_CurrentServicePath;

  public:
    virtual RR_INTRUSIVE_PTR<MessageEntry> ProcessMessageEntry(RR_INTRUSIVE_PTR<MessageEntry> m,
                                                               RR_SHARED_PTR<ServerEndpoint> c);

    virtual void Close();

    virtual void MessageReceived(RR_INTRUSIVE_PTR<Message> m, RR_SHARED_PTR<ServerEndpoint> e);

    virtual void AddClient(RR_SHARED_PTR<ServerEndpoint> cendpoint);

    virtual void RemoveClient(RR_SHARED_PTR<ServerEndpoint> cendpoint);

    /**
     * @brief Kicks a user with the specified username
     *
     * User must be authenticated.
     *
     * @param username The username to kick
     */
    virtual void KickUser(boost::string_ref username);

  private:
    RR_SHARED_PTR<UserAuthenticator> user_authenticator;
    std::map<std::string, std::string> security_policies;

    bool m_RequireValidUser;
    bool AllowObjectLock;

    virtual RR_INTRUSIVE_PTR<MessageEntry> ClientSessionOp(RR_INTRUSIVE_PTR<MessageEntry> m,
                                                           RR_SHARED_PTR<ServerEndpoint> e);

  public:
    /**
     * @brief Return if a valid user is required
     *
     * Security is configured through the SecurityPolicy object specified
     * with RobotRaconteur::RegisterService() or SetSecurityPolicy()
     *
     * @return true Users must authenticate to use the service
     * @return false Anonymous users are allowed
     */
    virtual bool RequireValidUser();

    virtual RR_SHARED_PTR<AuthenticatedUser> AuthenticateUser(
        boost::string_ref username, std::map<std::string, RR_INTRUSIVE_PTR<RRValue> >& credentials,
        RR_SHARED_PTR<ServerEndpoint> ep);

  private:
    boost::mutex ClientLockOp_lockobj;

    virtual void ClientLockOp(RR_INTRUSIVE_PTR<MessageEntry> m, RR_INTRUSIVE_PTR<MessageEntry> ret);

  public:
    /**
     * @brief Request an object lock on servicepath for user username
     *
     * This function handles incoming client requests, but may also be used
     * by the service directly
     *
     * @param servicepath The service path to lock
     * @param username The name of the user owning the lock
     */
    void RequestObjectLock(boost::string_ref servicepath, boost::string_ref username);

    /**
     * @brief Request a client lock on servicepath for a specific client connection
     *
     * This function handles incoming client requests, but may also be used
     * by the service directly. Client object locks lock for a specific client connection,
     * while client locks lock for a specific user. The specific client connection is
     * specified using endpoint.
     *
     * @param servicepath The service path to lock
     * @param username The name of the user owning the lock
     * @param endpoint The client endpoint ID of the client owning the lock
     */
    void RequestClientObjectLock(boost::string_ref servicepath, boost::string_ref username, uint32_t endpoint);

    /**
     * @brief Release a client lock on servicepath
     *
     * This function handles incoming client requests, but may also be used
     * by the service directly. Client locks can be released by the user that
     * created them if override_ is false, or by any user if override_ is true.
     *
     * The override_ parameter is set to true for client requests if the client has
     * the "objectlockoverride" permission.
     *
     * @param servicepath The service path to release lock
     * @param username The username requsting the lock release
     * @param override_ If false, only the creating username can release the lock. If true,
     * any username can release the lock
     */
    void ReleaseObjectLock(boost::string_ref servicepath, boost::string_ref username, bool override_);

    /**
     * @brief Get the name of the user that has locked the specified service path
     *
     * @param servicepath The service path
     * @return std::string The user owning the lock, or empty servicepath is not locked
     */
    std::string GetObjectLockUsername(boost::string_ref servicepath);

  protected:
    void check_lock(RR_SHARED_PTR<ServiceSkel> skel, RR_INTRUSIVE_PTR<MessageEntry> m);

    void check_monitor_lock(RR_SHARED_PTR<ServiceSkel> skel);

    RR_UNORDERED_MAP<std::string, RR_SHARED_PTR<ServerContext_ObjectLock> > active_object_locks;

  public:
    virtual void PeriodicCleanupTask();

  protected:
    RR_INTRUSIVE_PTR<MessageEntry> CheckServiceCapability(RR_INTRUSIVE_PTR<MessageEntry> m,
                                                          RR_SHARED_PTR<ServerEndpoint> c);

  public:
    boost::signals2::signal<void(RR_SHARED_PTR<ServerContext>, ServerServiceListenerEventType, RR_SHARED_PTR<void>)>
        ServerServiceListener;

  protected:
    void ReleaseServicePath1(const std::string& path);

  public:
    /**
     * @brief Release the specified service path and all sub objects
     *
     * Services take ownership of objects returned by objrefs, and will only request the object
     * once. Subsequent requests will return the cached object. If the objref has changed,
     * the service must call ReleaseServicePath() to tell the service to request the object again.
     *
     * Release service path will release the object specified by the service path
     * and all sub objects.
     *
     * This overload will notify all clients that the objref has been released. If the service
     * path contains a session key, use ReleaseServicePath(boost::string_ref, const std::vector<uint32_t>&)
     * to only notify the client that owns the session.
     *
     * @param path The service path to release
     */
    void ReleaseServicePath(boost::string_ref path);

    /**
     * @brief Release the specified service path and all sub objects
     *
     * Services take ownership of objects returned by objrefs, and will only request the object
     * once. Subsequent requests will return the cached object. If the objref has changed,
     * the service must call ReleaseServicePath() to tell the service to request the object again.
     *
     * Release service path will release the object specified by the service path
     * and all sub objects.
     *
     * This overload will notify the specified that the objref has been released. If the service
     * path contains a session key, this overload should be used so the session key is not leaked.
     *
     * @param path The service path to release
     * @param endpoints The client endpoint IDs to notify of the released service path
     */
    void ReleaseServicePath(boost::string_ref path, const std::vector<uint32_t>& endpoints);

    void AsyncProcessCallbackRequest(
        RR_INTRUSIVE_PTR<MessageEntry> m, uint32_t endpoint,
        RR_MOVE_ARG(boost::function<void(RR_INTRUSIVE_PTR<MessageEntry>, RR_SHARED_PTR<RobotRaconteurException>)>)
            handler,
        int32_t timeout = RR_TIMEOUT_INFINITE);

  private:
    void AsyncProcessCallbackRequest_err(RR_SHARED_PTR<RobotRaconteurException> error, uint32_t endpoint,
                                         uint32_t requestid);

    void AsyncProcessCallbackRequest_timeout(const TimerEvent&, uint32_t endpoint, uint32_t requestid);

    uint32_t request_number;

    struct outstanding_request
    {
        RR_INTRUSIVE_PTR<MessageEntry> ret;
        RR_SHARED_PTR<AutoResetEvent> evt;
        boost::function<void(RR_INTRUSIVE_PTR<MessageEntry> ret, RR_SHARED_PTR<RobotRaconteurException> error)> handler;
        RR_SHARED_PTR<Timer> timer;
    };

    boost::mutex outstanding_requests_lock;
    RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<outstanding_request> > outstanding_requests;

  public:
    RR_INTRUSIVE_PTR<MessageEntry> ProcessCallbackRequest(RR_INTRUSIVE_PTR<MessageEntry> m, uint32_t endpointid);

    /**
     * @brief Get the number of threads used by the monitor thread pool
     *
     * See SetMonitorThreadPoolCount()
     *
     * @return int32_t The number of threads
     */
    int32_t GetMonitorThreadPoolCount();

    /**
     * @brief Set the monitor lock thread pool count
     *
     * Monitor looks use a thread pool to manage tho lock. Each lock takes ownership of a thread,
     * and this thread is used to enforce single thread at a time access. By default 3 threads are
     * activated, allowing for 3 concurrent monitor locks. Use SetMonitorThreadPoolCount() to enable
     * more monitor locks if necessary.
     *
     * @param count The number of threads
     */
    void SetMonitorThreadPoolCount(int32_t count);

    /**
     * @brief Get the monitor lock thread pool
     *
     * See SetMonitorThreadPoolCount()
     *
     * @return RR_SHARED_PTR<ThreadPool> The monitor lock thread pool
     */
    RR_SHARED_PTR<ThreadPool> GetMonitorThreadPool();

  protected:
    RR_SHARED_PTR<ThreadPool> monitor_thread_pool;
    boost::mutex monitor_thread_pool_lock;

  private:
    void InitializeInstanceFields();
};

/**
 * @brief Server endpoint representing a client connection
 *
 * Robot Raconteur creates endpoint pairs between a client and service. For clients, this endpoint
 * is a ClientContext. For services, the endpoint becomes a ServerEndpoint. ServerEndpoints are used
 * to address a specific client connected to a service, since services may have multiple clients
 * connected concurrently. ServerEndpoints also provide client authentication information.
 *
 * Use ServerEndpoint::GetCurrentEndpoint()->GetLocalEndpoint() to retrieve the uint32_t
 * current endpoint ID. Use ServerEndpoint::GetCurrentAuthenticatedUser() to retrieve
 * the current user authentication information.
 *
 */
class ROBOTRACONTEUR_CORE_API ServerEndpoint : public Endpoint, public RR_ENABLE_SHARED_FROM_THIS<ServerEndpoint>
{
  public:
    RR_SHARED_PTR<ServerContext> service;

  private:
    static boost::thread_specific_ptr<RR_SHARED_PTR<ServerEndpoint> > m_CurrentEndpoint;

  public:
    /**
     * @brief Returns the current server endpoint
     *
     * Returns the current server endpoint during a request or packet event.
     * This is a thread-specific value and only valid during the initial
     * request or packet event invocation.
     *
     * Throws InvalidOperationException if not during a request or packet event
     *
     * @return RR_SHARED_PTR<ServerEndpoint> The current server endpoint
     */
    static RR_SHARED_PTR<ServerEndpoint> GetCurrentEndpoint();

  private:
    static boost::thread_specific_ptr<RR_SHARED_PTR<AuthenticatedUser> > m_CurrentAuthenticatedUser;

  public:
    /**
     * @brief Returns the current authenticated user
     *
     * Users that have been authenticated have a corresponding
     * AuthenticatedUser object associated with the ServerEndpoint.
     * GetCurrentAuthenticatedUser() returns the AuthenticatedUser
     * associated with the current ServerEndpoint during a request
     * or packet event. This is a thread-specific value and only valid during
     * the initial request or packet event invocation.
     *
     * Throws PermissionDeniedException or AuthenticationException
     * if there is no AuthenticatedUser set in the current thread.
     *
     * @return RR_SHARED_PTR<AuthenticatedUser> The AuthenticatedUser
     */
    static RR_SHARED_PTR<AuthenticatedUser> GetCurrentAuthenticatedUser();

  private:
    RR_SHARED_PTR<AuthenticatedUser> endpoint_authenticated_user;

  public:
    const std::string GetAuthenticatedUsername() const;

    virtual void MessageReceived(RR_INTRUSIVE_PTR<Message> m);

    void AuthenticateUser(boost::string_ref username, std::map<std::string, RR_INTRUSIVE_PTR<RRValue> >& credentials);

    void LogoutUser();

    virtual void PeriodicCleanupTask();

  public:
    virtual void SetTransportConnection(RR_SHARED_PTR<ITransportConnection> c);

  private:
    RobotRaconteurVersion client_version;
    boost::mutex this_lock;

  public:
    RobotRaconteurVersion GetClientVersion();
    void SetClientVersion(const RobotRaconteurVersion& version);

  private:
    void InitializeInstanceFields();

  public:
    ServerEndpoint(RR_SHARED_PTR<RobotRaconteurNode> node) : Endpoint(node) { InitializeInstanceFields(); }
};

/**
 * @brief Service object monitor lock notification
 *
 * Service objects must implement IRobotRaconteurMonitorObject for
 * monitor locking to function. Services call RobotRaconteurMonitorEnter()
 * with an optional timeout to request the lock, and call RobotRaconteurMonitorExit()
 * to release the monitor lock. RobotRaconteurMonitorEnter() should block
 * until a thread-exclusive lock can be established.
 */
class ROBOTRACONTEUR_CORE_API IRobotRaconteurMonitorObject
{
  public:
    /**
     * @brief Request a thread-exclusive lock without timeout
     *
     * May block until lock can be established
     */
    virtual void RobotRaconteurMonitorEnter() = 0;

    /**
     * @brief Request a thread-exclusive lock with timeout
     *
     * May block until lock can be established, up to the specified timeout.
     *
     * @param timeout Lock request timeout in milliseconds
     */
    virtual void RobotRaconteurMonitorEnter(int32_t timeout) = 0;

    /**
     * @brief Release the thread-exclusive monitor lock
     *
     */
    virtual void RobotRaconteurMonitorExit() = 0;

    virtual ~IRobotRaconteurMonitorObject() {}
};

/**
 * @brief Base class for default service object implementations
 *
 * RobotRaconteurGen will generate default implementations for service objects.
 * See default_object_gen.md for more information.
 *
 */
class ROBOTRACONTEUR_CORE_API RRObject_default_impl : public virtual RRObject
{
  protected:
    boost::mutex this_lock;
};

#ifndef BOOST_NO_CXX11_TEMPLATE_ALIASES
using ServiceSkelPtr = RR_SHARED_PTR<ServiceSkel>;
/** @brief Convenience alias for ServerContext shared_ptr */
using ServerContextPtr = RR_SHARED_PTR<ServerContext>;
/** @brief Convenience alias for ServerEndpoint shared_ptr */
using ServerEndpointPtr = RR_SHARED_PTR<ServerEndpoint>;
#endif

} // namespace RobotRaconteur

#pragma warning(pop)