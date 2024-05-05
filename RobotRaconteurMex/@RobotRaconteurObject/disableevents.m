function disableevents(obj)
%disableevents Disable events for a RobotRaconteurObject
%
%    See also RobotRaconteur.DisableEvents
RobotRaconteurMex('disableevents',obj.rrobjecttype,obj.rrstubid);
