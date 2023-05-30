function c=RobotRaconteurGenerator(objecttype,stubid)
% RobotRaconteurGenerator Generator returned from Robot Raconteur generator functions
%    Generators are used with generator functions to implement simple coroutines. They are
%    returned by function members with a parameter and/or return marked with the
%    generator container type. Robot Raconteur generators are modeled on Python generators,
%    and are intended to be used in two scenarios:
%    1. Transfering large parameter values or return values that would be over the message
%    transfer limit (typically around 10 MB, unless jumbo messages are enabled).
%    2. Long running operations that return updates or require periodic input. Generators
%    are used to implement functionality similar to "actions" in ROS.
%	 
%    Generators are a generalization of iterators, where a value is returned every time
%    the iterator is advanced until there are no more values. Python and Robot Raconteur iterators
%    add the option of passing a parameter every advance, allowing for simple coroutines. The
%    generator is advanced by calling the Next() or async_Next() functions. These functions
%    will either return a value or throw StopIterationException if there are no more values. Next()
%    and async_Next() may also throw any valid Robot Raconteur exception.
%	  
%    Generators can be terminated with either the Close() or Abort() functions. Close() should be
%    used to cleanly close the generator, and is not considered an error condition. Next(), if called
%    after close, should throw StopIterationException. Abort() is considered an error condition, and 
%    will cause any action assosciated with the generator to be aborted as quickly as possible (ie faulting
%    a robot). If Next() is called after Abort(), OperationAbortedException should be thrown. async_Close()
%    and async_Abort() are also available for asynchronous operation.
%
%    The generator uses "dot" notations for its members. The following synchronous functions are available:
%
%    ret = generator.Next(param) - Advance the generator. Optional param and/or ret values.
%    res_ret = generator.TryNext(param) - Advance the generator. Optional param and/or ret values. Returns a two element cell array with success and return value.
%    generator.Close()           - Close the generator
%    generator.Abort()           - Abort the generator
%
%    The generator also provides asynchronous versions of the above functions. The function RobotRaconteur.ProcessRequests()
%    must be called to process asynchronous returns.
%
%    generator.async_Next(handler)        - Asynchronous version of Next without parameter. Handler must be a function handle
%    generator.async_Next(param, handler) - Asynchronous version of Next with parameter. Handler must be a function handle
%    generator.async_Close(handler)       - Asynchronous version of Close. Handler must be a function handle
%    generator.async_Abort(handler)       - Asynchronous version of Abort. Handler must be a function handle

c=struct;
c.rrobjecttype=objecttype;
c.rrstubid=stubid;

c=class(c,'RobotRaconteurGenerator');
