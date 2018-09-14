function RobotRaconteurObjectServerDispatchEvent(skelid,eventname,evnt)

p={};
try
p=evnt.Parameters;
catch
end

RobotRaconteurMex('DispatchSkelEvent',skelid,eventname,p);