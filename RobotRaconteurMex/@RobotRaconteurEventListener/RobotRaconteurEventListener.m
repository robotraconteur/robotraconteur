function c=RobotRaconteurEventListener(objecttype,objectid,connectionid)

c=struct;
c.objecttype=objecttype;
c.objectid=objectid;
c.connectionid=connectionid;
c=class(c,'RobotRaconteurEventListener');
