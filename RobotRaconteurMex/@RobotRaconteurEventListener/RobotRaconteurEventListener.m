function c=RobotRaconteurEventListener(objecttype,objectid,connectionid)
%RobotRaconteurEventListener Storage handle for active event connection. Use
%    del to delete event connection.
%
%    See also RobotRaconteurObject

c=struct;
c.objecttype=objecttype;
c.objectid=objectid;
c.connectionid=connectionid;
c=class(c,'RobotRaconteurEventListener');
