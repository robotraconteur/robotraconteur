function c=RobotRaconteurMemoryClient(objecttype,stubid,membername)

c=struct;
c.rrobjecttype=objecttype;
c.rrstubid=stubid;
c.rrmembername=membername;

c=class(c,'RobotRaconteurMemoryClient');