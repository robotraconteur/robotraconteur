%This example shows how to expose an object as a service using MATLAB
%as the server.  It uses the "classdef" new style classes.

%%Startup section.  Only run this once per session

%Start the server. THIS MUST BE THE FIRST THING DONE USING ROBOT RACONTEUR
%DURING THE SESSION
RobotRaconteur.StartLocalServer('example.MatlabTestServer');
RobotRaconteur.StartTcpServer(4343);

%Define a service definition
servicedef1=['service example.MatlabTestService\n\n' ...
    'struct MatlabTestStruct\n' ...
    'field int32 val1\n' ...
    'field double[] val2\n' ...
    'end struct\n\n' ...
    'object MatlabTestObject\n' ...
    'property int32 testprop1\n' ...
    'property MatlabTestStruct testprop2\n' ...
    'function void testfunc1(int32 testval1, double testva2)\n' ...
    'function int32 testfunc2(int32 testval1)\n' ...
    'event testevent1()\n' ...
    'event testevent2(int32 testval1, int32 testval2)\n' ...
    'end object\n'];
%Use sprintf to convert the '\n' to newlines
servicedef=sprintf(servicedef1);

%Register the service definition
RobotRaconteur.RegisterServiceType(servicedef);

%Create the object
o=MatlabTestServiceClass();

%Register the object as a service
RobotRaconteur.RegisterService('MatlabTestService','example.MatlabTestService.MatlabTestObject',o);

%Use the following instead if you want authentication. "authdata"
%has the same format as the "PasswordFileUserAuthenticator"
%authdata=sprintf(['myusername 34819d7beeabb9260a5c854bc85b3e44 objectlock\n' ...
%'anotherusername 1910ea24600608b01b5efd7d4ea6a840 objectlock\n' ...
%'superuser f1bc69265be1165f64296bcb7ca180d5 objectlock,objectlockoverride\n']);
%RobotRaconteur.RegisterService('MatlabTestService','example.MatlabTestService.MatlabTestObject',o,authdata);

%%Runtime section.  Run this section repeatedly to execute requests
%to the service.

%Run the service.  Use Ctrl-C to exit
i=0;
while(1)
  %We need to process the incoming requests manually because MATLAB
  %has poor threading capabilities.  It will timeout in 1 second.
  RobotRaconteur.ProcessServerRequests(1); 
  %Fire off a few events
  notify(o,'testevent1');
  %Note that parameters are passed using the "RobotRaconteurEventData"
  %command with the parameters stored in a cell vector.
  notify(o,'testevent2',RobotRaconteurEventData({int32(10);int32(i)}))
  i=i+1;
end
    


