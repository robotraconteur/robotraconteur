classdef RobotRaconteurTestServerLib < handle
    %ROBOTRACONTEURTESTSERVERLIB Summary of this class goes here
    %   Detailed explanation goes here
    
        
    methods
        function obj = RobotRaconteurTestServerLib(libname,hfile)
            loadlibrary(libname,hfile);
        end
        
        function StartServer(obj,nodename)
            if nargin < 2
                nodename = 'matlab_testprog';
            end
            
            assert(calllib('robotraconteur_test_server_lib','robotraconteur_test_server_lib_start',nodename,strlength(nodename)) >= 0);
        end
        
        function url2=GetServiceURL(obj, servicename)
            url = zeros(1,4096,'int8');
            urlptr = libpointer('int8Ptr',url);
            url_len = uint32(4096);
            url_lenptr = libpointer('uint32Ptr',url_len);

            assert(calllib('robotraconteur_test_server_lib','robotraconteur_test_server_lib_get_service_url2',servicename,strlength(servicename),urlptr,url_lenptr)>=0);

            url_len = url_lenptr.Value;

            url2 = char(urlptr.Value(1:url_len));
            disp(char(url2))
        end
        
        function Shutdown(obj)
           calllib('robotraconteur_test_server_lib','robotraconteur_test_server_lib_shutdown');
        end
        
        function delete(obj)
           try
               calllib('robotraconteur_test_server_lib','robotraconteur_test_server_lib_shutdown');               
           catch
           end
           
           try
               unloadlibrary robotraconteur_test_server_lib
           catch
           end
        end
    end
end

