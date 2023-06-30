%This is an example of an object that can be used as a service object in
%MATLAB.  Note that MATLAB service objects are limited to functions,
%properties, and events.  The class MUST inherent from "handle".  This
%class is a stub.  See the MATLAB help documentation for more information
%on how to use the "classdef" command.

classdef MatlabTestServiceClass < handle
    properties
       testprop1=int32(10);
       testprop2=[];
    end
    
    methods
        function testfunc1(c,testval1,testval2)
           disp(testval1)
           disp(testval2)
        end
        
        function r=testfunc2(c,testval1)
           r=testval1+c.testprop1; 
        end
    end
    
    events
       testevent1;
       testevent2;
    end
    
end