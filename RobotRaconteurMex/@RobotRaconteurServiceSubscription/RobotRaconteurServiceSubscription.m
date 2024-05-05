function c=RobotRaconteurServiceSubscription(objecttype,stubid)
%RobotRaconteurServiceSubscription Subscription that automatically connects services and manages
%    lifecycle of connected services
%
%    Created using RobotRaconteur.SubscribeService() or RobotRaconteur.SubscribeServiceByType(). An
%    example:
%
%        service_sub = RobotRaconteur.SubscribeService("rr+tcp://localhost:56384?service=robot")
%
%    RobotRaconteur.SubscribeService()  is used to create a robust connection to a service with a specific URL.
%    RobotRaconteur.SubscribeServiceByType() is used to connect to services with a specified type, filtered with a
%    ServiceSubscriptionFilter. Subscriptions will create connections to matching services, and will retry the connection
%    if it fails or the connection is lost. This behavior allows subscriptions to be used to create robust connections.
%
%    The currently connected clients can be retrieved using the GetConnectedClients() function. A single "default client" can be
%    retrieved using the GetDefaultClient() function. If the user wants to claim a client, the ClaimClient() and ReleaseClient()
%    functions will be used. Claimed clients will no longer have their lifecycle managed by the subscription.
%
%    Subscriptions can be used to create \"pipe\" and \"wire\" subscriptions. These member subscriptions aggregate
%    the packets and values being received from all services. They can also act as a "reverse broadcaster" to
%    send packets and values to all services that are actively connected. See RobotRaconteurPipeSubscription and
%    RobotRaconteurWireSubscription.
%
%    The following functions are available:
%
%    service_sub.Close()          - Close the subscription
%    service_sub.GetConnectedClients() - Get currently connected clients. Returns a containers.Map of connected client IDs
%                                        and the client objects.
%    service_sub.SubscribeWire(wire_name, service_path) - Subscribe to a wire, returning a RobotRaconteurWireSubscription.
%                                                         The wire_name is the name of the wire member in the object.
%                                                         The service_path is optional and by default is the root object.
%    service_sub.SubscribePipe(pipe_name, service_path) - Subscribe to a pipe, returning a RobotRaconteurPipeSubscription.
%                                                         The pipe_name is the name of the wire member in the object.
%                                                         The service_path is optional and by default is the root object.
%    service_sub.SubscribeSubObject(service_path)       - Subscribe to a sub object based on service path, returning a
%                                                         RobotRaconteurSubObjectSubscription.
%    service_sub.ClaimClient(client_obj) - Claim a client returned by GetConnectedClients() or GetDefaultObject(). The claimed
%                                          client will no longer have its lifecycle managed by the subscription.
%    service_sub.ReleaseClient(client_obj) - Release a client previously claimed using ClaimClient()
%    service_sub.GetDefaultClient() - Get the "default client", which is the "first" client that is currently connected. In practice
%                                     this client is randomly selected from connected clients. This should be used when connecting
%                                     to a single client using a URL with RobotRaconteur.SubscribeService(). An error will be thrown
%                                     if no clients are connected.
%    service_sub.GetDefaultClientWait(timeout) - Same as GetDefaultClient(), but waits for a client to connect with an optional
%                                                timeout. timeout is in seconds.
%    service_sub.GetClientConnectFailures() - Get a cell array of strings containing error messages from connect failures. Used
%                                             to diagnose subscriptions that are not able to form client connections.
%
%
%    See also RobotRaconteur.SubscribeService, RobotRaconteur.SubscribeServiceByType, RobotRaconteurServiceSubscription,
%        RobotRaconteurWireSubscription, RobotRaconteurPipeSubscription, RobotRaconteurSubObjectSubscription

c=struct;
c.rrobjecttype=objecttype;
c.rrstubid=stubid;

c=class(c,'RobotRaconteurServiceSubscription');
