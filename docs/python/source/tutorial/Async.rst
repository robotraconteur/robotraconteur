.. _asyncprog:

Asynchronous programming
========================

| Most of the functions in Robot Raconteur are “blocking" functions, meaning that they will block the current executing
  thread until the result of the operation is completed. An example is
| ``RRN.ConnectService()``. This function will begin the connection process and block the current thread until the
  connection is complete. This process can take anywhere from a few hundred milliseconds to several seconds. If the
  client is only accessing one device this is normally not a problem, but if the client needs to connect to a hundred
  devices this can become a severe problem as having a large number of threads becomes very computationally expensive
  and difficult to coordinate. The solution to this problem is to use “asynchronous functions". These functions begin
  the operation but return immediately. When the operation is completed, a supplied handler function is called by the
  thread pool.

Consider the synchronous connect function used in the previous examples:

.. code:: python

   c=RRN.ConnectService('rr+tcp://localhost:2354?service=Create')

The asynchronous equivalent would be:

.. code:: python

   def connect_handler(c,err):
       if (err is not None):
           # If "err" is not None it means that an exception occurred.
           # "err" contains the exception object
           print "An error occured! " + str(err)
           return
       print "Got the connection!"
       # Now "c" is ready for use

   # Start the connect process with a 5 second timeout
   c=RRN.AsyncConnectService('rr+tcp://localhost:2354?service=Create',None,None,None,connect_handler,5)
   # Do other tasks while connection is being created

The form of “Async" functions is normally the same as synchronous functions but with “Async" prepended and two extra
parameters: the handler function and the timeout. The handler function will take zero, one, or two arguments depending
on the return arguments. The handler function can be any free module function or a bound instance method. The last
argument is the timeout, which is
in seconds. (Note: other language bindings use milliseconds for the timeout). The default for most is
RR_TIMEOUT_INFINITE which means the function will never timeout. This is not recommended as it can cause a deadlock.
Always specify a timeout.

For object references created by the ``RRN.ConnectService()`` function, functions, properties, and objrefs are available
in asynchronous form. In general these functions operate the same as their standard synchronous counterparts but are
prepended by ``async_`` and have two extra parameters, “handler" and “timeout". The property forms use getter and setter
functions of the form ``async_get_`` and ``async_set_``. If the function produces a return value, the handler will have
the form ``handler(ret, err):``. For void functions, the handler will have the form ``handler(err):``.

In Python 3, passing “None” for the handler will return a future that can be used with the "await" keyword.
