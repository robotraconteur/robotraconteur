classdef RobotRaconteurRate < handle
	% RobotRaconteurRate Rate to stabilize a loop
    %    Rate is used to stabilize the period of a loop. Use
	%    RobotRaconteur.CreateRate() to create rates.
	%
	%    See also RobotRaconteur.CreateRate
	methods
		function obj=RobotRaconteurRate(frequency)
			obj.id=RobotRaconteurMex('CreateRate',frequency);
		end
		
		function Sleep(obj)
			% Sleep the calling thread until the current loop period expires
			RobotRaconteurMex('SleepRate',obj.id);
		end
		
		function delete(obj)
			RobotRaconteurMex('DeleteRate',obj.id);
		end
		
	end


end