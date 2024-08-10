% exceptions_example.m - Example of using exceptions in MATLAB

% Connect to the service
url = 'rr+tcp://localhost:53224/?service=exception_example';
c = RobotRaconteur.ConnectService(url);

% MATLAB does not filter exceptions by type, so any errors are caught by the catch block
try
    c.my_exception_function1();
catch e
    disp(e.message);
end

try
    c.my_exception_function2();
catch e
    disp(e.message);
end
