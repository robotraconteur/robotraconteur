classdef RobotRaconteurRate < handle
	methods
		function obj=RobotRaconteurRate(frequency)
			obj.id=RobotRaconteurMex('CreateRate',frequency);
		end
		
		function Sleep(obj)
			RobotRaconteurMex('SleepRate',obj.id);
		end
		
		function delete(obj)
			RobotRaconteurMex('DeleteRate',obj.id);
		end
		
	end


end