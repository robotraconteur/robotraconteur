function types=getpulledservicetypes(obj)
%getpulledservicetypes Get the names of the service definitions pulled by the client
%    Returns a cell array of strings
%
%    See also RobotRaconteur.GetPulledServiceTypes
types=RobotRaconteurMex('GetPulledServiceTypes',obj.rrobjecttype,obj.rrstubid);
