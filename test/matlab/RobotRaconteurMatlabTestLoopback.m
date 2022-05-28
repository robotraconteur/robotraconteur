

try
    clear lib
    libname = getenv('ROBOTRACONTEUR_TEST_SERVER_LIB');
    hfile = getenv('ROBOTRACONTEUR_TEST_SERVER_LIB_HFILE');
    lib=RobotRaconteurTestServerLib(fullfile(libname),fullfile(hfile));

    lib.StartServer('matlab_testprog');

    url1 = lib.GetServiceURL('RobotRaconteurTestService');
    url2 = lib.GetServiceURL('RobotRaconteurTestService2');

    RobotRaconteurMatlabTest(url1)
    RobotRaconteurMatlabTest2(url2)

    clear lib
    %quit(0)
catch e
    disp(e)
    clear lib
    %quit(1)    
end