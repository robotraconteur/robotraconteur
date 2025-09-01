function RobotRaconteurObjectServerConnectEvent(obj, eventname, skelid)

cb=@(src,event_)(RobotRaconteurObjectServerDispatchEvent(skelid,eventname,event_));
addlistener(obj,eventname,cb);
