%RobotRaconteurAsyncOps
%
%    See <a href="matlab:help RobotRaconteurObject">RobotRaconteurObject</a> before this section.
%
%    Robot Raconteur MATLAB supports some asynchronous operations. These
%    functions are prepended with "async_", and return the result of the
%    operation to a handler function once complete, instead of blocking
%    the current thread. Property get, property set,
%    and functions can be called asynchronously. The asynchronous version
%    is called with the normal parameters for the call, a handler function,
%    a user defined callback parameter, and a timeout in seconds. The
%    invocation returns immediately, and the handler function is called
%    once the operation has completed. Because of the single threaded
%    nature of MATLAB the handler will only be called
%    when the function RobotRaconteur.ProcessRequests() is called.
%
%    async property get
%
%      The asynchronous property gets are accessed by prepending
%      "async_get_" to the member name and passing the asynchronous
%      parameter. The form is:
%
%        obj.async_get_propertyname(handler,param,timeout)
%
%      where propertyname is the name of the property to get, handler is a
%      function handle of the form:
%
%        function myhandler(param, value, error)
%
%      param is any value that you want to passfrom the call to the handler,
%      and timeout is the timeout for the call in seconds. error will be
%      empty if no error occurs or will be astring containing the error
%      message if an error does occur. An example of getting
%      DistanceTraveled asynchronously:
%
%        obj.async_get_DistanceTraveled(@myhandler,1,1)
%
%    async property set
%
%      The asynchronous property sets are accessed by prepending "async_set"
%      to the member name and possing the asynchronous parameters. The form
%      is:
%
%        obj.async_set_propertyname(value,handler,param,timeout)
%
%      where propertyname is the name of the property to set, value is the
%      value to set, handler is a function handle of the form:
%
%        function myhandler(param,error)
%
%      param is any value that you want to pass from the call to the
%      handler, and timeout is the timeout for the call in seconds. error
%      will be empty if no error occurs or will be a string containing
%      containing the error message if an error does occur. An example
%      of setting DistanceTraveled asynchronously:
%
%        obj.async_set_DistanceTraveled(10, @myhandler,1,0.5)
%
%     In this case the operation will fail since the property is readonly.
%
%    async function
%
%      Functions are accessed asynchronously by prepending "async_" to the
%      function name. The three extra asynchronous parameters are added to
%      the parameters list after the member parameters. The form is:
%
%        obj.async_functionname(<parameters>,handler,param,timeout)
%
%      where functioname is the name of the function, <parameters> are
%      the zero or more parameters that are part of the original function
%      call, handler is a function of the form:
%
%        function myhandler(param, error) % void return functions
%        function myhandler(param, ret, error) % non-void return functions
%
%      param is any value that you want to pass from the call to the
%      handler, and timeout is the timeout for the call in seconds. error
%      will be empty if no error occurs or will be a string containing the
%      error message if an error does occur. An example of calling Drive
%      asynchronously:
%
%        obj.async_Drive(int16(200),int16(1000),@myhandler,1,0.5)
%
%    Generators also support limited asynchronous operations. See
%    <a href="matlab:help RobotRaconteurGenerator">RobotRaconteurGenerator</a>
%
%    A common use of asynchronous operations is to read multiple sensors
%    simultaneously. For instance, it may be necessary to read a few dozen
%    light sensors for an advanced lighting control system. Asynchronous
%    operations are very efficient because they do not use any resources
%    between the call and handler. An example (c contains a cell array
%    of sensors to read)
%
%      function [res, err]=readsensors(c,timeout)
%
% 	     N=length(c);
% 	     res=cell(N,1);
% 	     err=cell(N,1);
% 	     activekeys=[];
%
% 	     for i=1:N
% 		    c1=connections{i};
% 		    c1.async_ReadSensor(@myhandler,i,timeout);
% 		    activekeys=[activekeys;i];
% 	     end
%
% 	     while (~isempty(activekeys))
% 		    RobotRaconteur.ProcessRequests();
% 	     end
%
%     function myhandler(key1, value1, err1)
% 	     res{key1}=value1
% 	     err{key1}=err1
% 	     activekeys(activekeys==key1)=[];%
%     end
%     end
