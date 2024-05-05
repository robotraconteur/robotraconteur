function t=type(obj)
%type Return the RobotRaconteur type of the object as a string
%
%   See also RobotRaconteurObject/members RobotRaconteurObject/servicedef
t=RobotRaconteurMex('type',obj.rrobjecttype,obj.rrstubid);
