function c=RobotRaconteurServiceInfo2Subscription(objecttype,stubid)
%RobotRaconteurServiceInfo2Subscription Subscription for information about detected services 
%
%    Created using RobotRaconteur.SubscribeServiceInfo2(). An example to find robot services:
%
%        service_info2_sub = RobotRaconteur.SubscribeServiceInfo2("com.robotraconteur.robotics.robot.Robot")
%
%    The ServiceInfo2Subscription class is used to track services with a specific service type as they are
%    detected on the local network and when they are lost. The currently detected services can also
%    be retrieved. The service information is returned using the ServiceInfo2 structure.
%
%    The following functions are available:
%
%    service_info2_sub.Close()          - Close the subscription
%    service_info2_sub.GetDetectedServiceInfo2() - Get currently detected services
%
%    See also RobotRaconteur.SubscribeServiceInfo2, RobotRaconteurServiceSubscription

c=struct;
c.rrobjecttype=objecttype;
c.rrstubid=stubid;

c=class(c,'RobotRaconteurServiceInfo2Subscription');
