===============
Callback Member
===============

.. class:: Callback

   "callback" member type interface

   The Callback class implements the "callback" member type. Callbacks are declared in service definition
   files using the "callback" keyword within object declarations. They provide functionality similar to the
   "function" member, but the direction is reversed, allowing the service to call a function on a specified
   client. The desired client is specified using the Robot Raconteur endpoint identifier. Clients must
   configure the callback to use using the ``Function`` property.

   On the client side,
   the client specifies a function for the callback using the ``Function`` property.
   On the service side, the function ``GetFunction(endpoint)`` is used to retrieve
   the proxy function to call a client callback.

   This class is instantiated by the node. It should not be instantiated by the user.

   .. attribute:: Function

      Specifies the function that will be called for the callback. This is only available for the client

   .. method:: GetFunction(endpoint)

      Retrieves a function that will be executed on the client selected by the endpoint parameter. The
      endpoint can be determined through ServerEndpoint.GetCurrentEndpoint(). This is only available in a service.

      :param endpoint: The endpoint identifying the client to execute the function on
      :type endpoint: int
      :return: A proxy function that will be executed on the client
      :rtype: Callable
