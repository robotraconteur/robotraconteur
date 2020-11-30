function mem=members(obj)
%members Get the object service definition with members as a string
%
%   See also RobotRaconteurObject/servicedef
mem=RobotRaconteurMex('members',obj.rrobjecttype,obj.rrstubid);
