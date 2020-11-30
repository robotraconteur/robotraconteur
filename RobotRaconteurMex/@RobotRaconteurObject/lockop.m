function lockop(obj,command)
%lockop Execute a Robot Raconteur object lock operation
%
%  See also RobotRaconteur.RequestObjectLock, RobotRaconteur.ReleaseObjectLock,
%      RobotRaconteur.MonitorEnter, RobotRaconteur.MonitorExit
RobotRaconteurMex('LockOp',obj.rrobjecttype,obj.rrstubid,command);
