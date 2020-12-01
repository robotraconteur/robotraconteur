Authentication
==============

Robot Raconteur provides a built-in authentication system. During connection, a client can specify a “username" of type
``string``, and “credentials" of type ``varvalue{string}``. Normally the credentials contains a simple string entry for the
password, but some authentication methods may require more complex credentials like a token/password pair. A connection
example:

.. code:: python

  credentials={"password", RR.VarValue("mypassword","string")}
  obj=RRN.ConnectService( "rr+tcp://localhost:2354?service=Create","myusername",credentials)

Of course the current example service does not have authentication enabled. The first step is to create a
``UserAuthenticator``. The ``UserAuthenticator`` will receive the authentication requests and compare the username and
credentials. If they are correct, the user is authenticated. The only authenticator currently available is the
``PasswordFileUserAuthenticator``. This authenticator uses a plain-text string that contains the user definitions, one
per line. There are three entries separated by spaces: the username, the MD5 hash of the password, and a comma separated
list of credentials (no spaces between). The two credentials of interest are “objectlock" and “objectlockoverride". The
meaning of these is discussed in Section `[objectlocks] <#objectlocks>`__. Example `[passwordfile] <#passwordfile>`__
shows the contents of a simple password file. The MD5 hash for the password can be generated using “RobotRaconteurGen".

.. code::

  RobotRaconteurGen –md5passwordhash mypassword``

.. code::

  myusername 34819d7beeabb9260a5c854bc85b3e44 objectlock 
  anotherusername 1910ea24600608b01b5efd7d4ea6a840 objectlock 
  superuser f1bc69265be1165f64296bcb7ca180d5 objectlock,objectlockoverride``

The ``PasswordFileUserAuthenticator`` can now be initialized:

.. code:: python

  with open('passwords.txt') as content_file:
    content=content_file.read()
    p=RR.PasswordFileUserAuthenticator(content)

The next step is to create a ``ServiceSecurityPolicy`` that describes the security requirements. The policy contains the
authenticator and a dictionary of policies. Currently only “requirevaliduser" and “allowobjectlock" are valid, and both
should be set to “true".

.. code:: python

  policies={"requirevaliduser" : "true", "allowobjectlock" : "true"}
  s=RR.ServiceSecurityPolicy(p,policies)

Finally, the service can be registered using the policy.

.. code:: python

  RRN.RegisterService("Create","experimental.create.Create",obj)

| When the service is running, it may be useful to determine if there is a currently authenticated user. This is
  accomplished through the ``ServerEndpoint`` class static method. The current
| ``AuthenticatedUser`` can be retrieved:

.. code:: python

  user=RR.ServerEndpoint.GetCurrentAuthenticatedUser()

| Note that this call will raise an exception if no user is currently authenticated. The
| ``AuthenticatedUser`` contains the fields ``Username``, ``LoginTime``, ``LastAccessTime``, and ``Privileges`` fields
  to help determine the user currently accessing the service. This function will work during all transactional calls to
  the service.

The authenticated user will be logged out when the client connection is closed or after a timeout of typically 15
minutes.