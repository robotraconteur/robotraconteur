function  value = subsref( obj, S )
value=RobotRaconteurMex('MemoryOp',obj.rrobjecttype,obj.rrstubid,obj.rrmembername,'read',S);

end

