function  obj = subsasgn( obj, S, value )
dummy=RobotRaconteurMex('MemoryOp',obj.rrobjecttype,obj.rrstubid,obj.rrmembername,'write',S,value);

end

