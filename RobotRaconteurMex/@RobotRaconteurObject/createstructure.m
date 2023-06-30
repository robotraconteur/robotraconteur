function empty_structure=createstructure(obj,structure_type)

    empty_structure=RobotRaconteurMex('CreateStructure',obj.rrobjecttype,obj.rrstubid,structure_type);
