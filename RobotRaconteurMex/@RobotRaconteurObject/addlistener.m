function event=addlistener(obj,eventname,callback)
%addlistener Add a listener for an event member
%    addlistener is used to specify a callback function for event members. See
%    RobotRaconteurObject for more information on event member usage.
%
%    See also RobotRaconteurObject
event=RobotRaconteurMex('addlistener',obj.rrobjecttype,obj.rrstubid,eventname,callback);
