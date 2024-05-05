function rrdisconnect(obj)
%disconnect Disconnect a client
%
%    See also RobotRaconteur.DisconnectService
RobotRaconteurMex('Disconnect',obj.rrobjecttype,obj.rrstubid);
