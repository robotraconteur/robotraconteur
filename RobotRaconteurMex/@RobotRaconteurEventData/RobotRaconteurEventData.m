classdef (ConstructOnLoad) RobotRaconteurEventData < event.EventData
   properties
      Parameters
   end

   methods
      function data = RobotRaconteurEventData(parameters)
         data.Parameters = parameters;
      end
   end
end