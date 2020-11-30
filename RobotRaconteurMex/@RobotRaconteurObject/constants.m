function t=constants(obj)
%constants Retrieve the constants from an object
%    Returns the constants of an object as a MATLAB struct
t=RobotRaconteurMex('constants',obj.rrobjecttype,obj.rrstubid);