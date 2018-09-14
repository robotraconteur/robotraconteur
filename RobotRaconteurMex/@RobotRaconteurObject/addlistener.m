function event=addlistener(obj,eventname,callback)

event=RobotRaconteurMex('addlistener',obj.rrobjecttype,obj.rrstubid,eventname,callback);