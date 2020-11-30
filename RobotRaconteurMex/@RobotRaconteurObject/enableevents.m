function enableevents(obj)
%enableevents Enable events for a RobotRaconteurObject
%
%    See also RobotRaconteur.EnableEvents
RobotRaconteurMex('enableevents',obj.rrobjecttype,obj.rrstubid);