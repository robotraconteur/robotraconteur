function def=getpulledservicetype(obj,name)
%getpulledservicetype Get a service definition pulled by the client
%
%    See also RobotRaconteur.GetPulledServiceType
def=RobotRaconteurMex('GetPulledServiceType',obj.rrobjecttype,obj.rrstubid,name);