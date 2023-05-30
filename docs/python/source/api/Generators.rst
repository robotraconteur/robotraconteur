Generators
==========

.. class:: Generator

    Generators are used by generator functions. For clients, the generators are returned from generator function
    calls. For services, the implementation must return a generator that implements the ``Next``, ``Close``, and ``Abort``
    functions. If ``AsyncNext``, ``AsyncClose``, or ``AsyncAbort`` is implemented by a generator returned by a service,
    these asynchronous versions will be called instead of the synchronous versions.

   .. method:: Next(param = None)
      
      The “Next” function is the primary function for the generator. It receives zero or one parameter,
      and returns a return value or Null if the return type is `void`. This function is called repeatedly until
      the client closes the generator, or the “Next” function throws a ``StopIterationException``. The
      ``StopIterationException`` should be treated as an expected signal that the generator is complete.

      :param param: (Optional) The parameter to send to the generator “Next” function. This is only
       required if the generator has a parameter marked {generator}. Otherwise, this parameter
       should be None
      :return: The return value from the “Next” call. If the generator function return type is void, this will
       be None.
    
   .. method:: TryNext(param = None)
      
      "TryNext" is the same as "Next", except that it will not throw a ``StopIterationException``. Instead, it will
      return a tuple of ``(bool, T)`` where the first value is True if the generator is not complete, and the second
      value is the return value from the “Next” call. If the generator function return type is void, the second value
      will be None.

      :param param: (Optional) The parameter to send to the generator “Next” function. This is only
       required if the generator has a parameter marked {generator}. Otherwise, this parameter
       should be None
      :return: A tuple of ``(bool, T)`` where the first value is True if the generator is not complete, and the second
        value is the return value from the “Next” call. If the generator function return type is void, the second value
        will be None.

   .. method:: Close()

       Closes the generated. Internally, this works by sending the ``StopIterationException`` to the service. Once the 
       generator is closed, future requests will return with an exception. The “Close”
       command is considered a clean operation and does not signal an error condition.

   .. method:: Abort()

       Aborts the generated. Internally, this works by sending the ``AbortOperationException`` to the service. Once the 
       generator is aborted, future requests will return with an exception. The “Aborted”
       command is considered an error condition, and the operation that the generator represents should
       be immediately aborted and all data discarded. If this generator represents a physical action such
       as a robot motion, the motion should be stopped immediately.

   .. method:: AsyncNext(param, handler, timeout = -1)
       
       An asynchronous version of the “Next” function. The service will call this asynchronous 
       version of "Next" if it is available.

       If ``handler`` is None, returns an awaitable future.

       :param param: (Optional) The parameter to send to the generator “Next” function. This is only
         required if the generator has a parameter marked {generator}. Otherwise, this parameter
         should be None
       :param handler: The handler for when the operation is complete. This will pass the return
         value, or an exception
       :type handler: Callable[[T,Exception],None] if generator return type is not "void", 
         Callable[[Exception],None] if generator return type is "void"
       :param timeout: The timeout in seconds, or -1 for infinite
       :type timeout: float

   .. method:: AsyncClose(handler, timeout = -1)

       An asynchronous version of the “Close” function. The service will call this 
       asynchronous version of "Close" if it is available

       :param handler: The handler for when the operation is complete, including a possible exception
       :type handler: Callable[[Exception],None]
       :param timeout: The timeout in seconds, or -1 for infinite
       :type timeout: float

   .. method:: AsyncAbort(handler, timeout = -1)

       An asynchronous version of the “Abort” function. 
       The service will call this asynchronous version of "Abort" if it is available

       :param handler: The handler for when the operation is complete, including a possible exception
       :type handler: Callable[[Exception],None]
       :param timeout: The timeout in seconds, or -1 for infinite
       :type timeout: float
