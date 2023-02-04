classdef (ConstructOnLoad) RobotRaconteurVarValue < handle
   properties
      data
      datatype
   end

   methods
      function this = RobotRaconteurVarValue(data, datatype)
         this.data = data;
         this.datatype = datatype;
      end
   end
end