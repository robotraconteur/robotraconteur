function t=constants(obj,service_type)
%constants Retrieve the constants from an object
%    Returns the constants of an object as a MATLAB struct

if nargin < 2
    t=RobotRaconteurMex('constants',obj.rrobjecttype,obj.rrstubid);
else
    t=RobotRaconteurMex('constants',obj.rrobjecttype,obj.rrstubid,service_type);
end
