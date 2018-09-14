function RobotRaconteurObjectServerConnectEvent(obj, eventname, skelid)

cb=@(src,evnt)(RobotRaconteurObjectServerDispatchEvent(skelid,eventname,evnt));
addlistener(obj,eventname,cb);