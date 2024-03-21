function c=RobotRaconteurSubObjectSubscription(objecttype,stubid)
%RobotRaconteurServiceSubscription Creates a sub object subscription
%
%    Created using RobotRaconteurServiceSubscription.SubscribeSubObject('*.my_sub_object')
%
%        subobject_sub = subscription.SubscribeSubObject('rr+tcp://localhost:56384?service=robot')
%
% Sub objects are objects within a service that are not the root object. Sub objects are typically
% referenced using objref members, however they can also be referenced using a service path.
% The SubObjectSubscription class is used to automatically access sub objects of the default client.
%
% The service path is broken up into segments using periods. See the Robot Raconter
% documentation for more information. The first level of the* service path may be "*" to match any service name.
% For instance, the service path "*.sub_obj" will match any service name, and use the "sub_obj" objref
%
%    The following functions are available:
%
%    subobject_sub.Close()          - Close the subscription
%    service_sub.GetDefaultClient() - Get the "default client", which is the "first" client that is currently connected. In practice
%                                     this client is randomly selected from connected clients. This should be used when connecting
%                                     to a single client using a URL with RobotRaconteur.SubscribeService(). An error will be thrown
%                                     if no clients are connected.
%    service_sub.GetDefaultClientWait(timeout) - Same as GetDefaultClient(), but waits for a client to connect with an optional
%                                                timeout. timeout is in seconds.
%    See also RobotRaconteur.SubscribeService, RobotRaconteur.SubscribeServiceByType, RobotRaconteurServiceSubscription,
%        RobotRaconteurWireSubscription, RobotRaconteurPipeSubscription

c=struct;
c.rrobjecttype=objecttype;
c.rrstubid=stubid;

c=class(c,'RobotRaconteurSubObjectSubscription');
