function mem=servicedef(obj)
%servicedef Get the service definition containing the object type as a string
%
%    See also RobotRaconteurObject/members
mem=RobotRaconteurMex('servicedef',obj.rrobjecttype,obj.rrstubid);
