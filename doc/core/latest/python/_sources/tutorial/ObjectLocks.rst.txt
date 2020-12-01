.. _objectlocks:

Exclusive object locks
======================

During the first applications of the experimental version of Robot Raconteur there was a frequent problem of multiple
users trying to access a device remotely at the same time and causing confusing collisions. It became rapidly apparent
that some form of locking needed to be available. Robot Raconteur has three types of locks "User", "Client", and
"Monitor".

User
----

The “User" lock provides a lock on an object within a service that is exclusive to a specific username. The user must be
authenticated and have the “objectlock" privilege. The same user can authenticate multiple times from any location using
this lock. The lock works on the selected object, and all objects below the current object in the “objref" tree. (This
means all objects that are referenced by the locked object’s objrefs.) To lock an object, use:

.. code:: python

  RRN.RequestObjectLock(obj, RR.RobotRaconteurObjectLockFlags_USER_LOCK)

``obj`` must be a Robot Raconteur client object reference. It does not have to be the root object. This function will
raise an exception if the object is already locked by another user. To release the lock, use:

.. code:: python

  RRN.ReleaseObjectLock(obj)

If the user has the privilege “objectlockoverride" the user can release all locks even if the user did not initiate the
lock.

Note that the lock will prevent transactional operations from occurring, but will not stop wire connections, pipe
endpoints, and events from functioning normally. If exclusive wire and pipe connections are required conflicting wire
and pipe connections will need to be closed by the service object.

Client
------

The “Client" lock is identical to the “User" lock but only allows one unique connection. This means that the user cannot
access the same service object from a different connection even with the same username. To request a client lock:

.. code:: python

  RRN.RequestObjectLock(obj, RR.RobotRaconteurObjectLockFlags\_CLIENT\_LOCK)

To release a client lock:

.. code:: python

  RRN.ReleaseObjectLock(obj)


Monitor
-------

The “Monitor" lock provides a global thread monitor (more often called “mutex") lock on a single object. This means that
globally only the current thread with the lock can access the object. The “Monitor" lock is intended for short
operations that cannot be interrupted and will timeout if 15 seconds elapses between operations on the client. It does
not inherit to other objects like “User" and “Client" locks. Unlike “User" and “Client", the object must implement the
monitor locking functionality explicitly. (Note that the “RLock" type in Python does not support locking with timeout
which makes things a little less clear.) An example object that implements the required functionality:

.. code:: python

    class MyMonitorLockableObject:

        def __init__(self):
            self._lock=threading.RLock()
        
        def RobotRaconteurMonitorEnter(self):
            self._lock.acquire()

        def RobotRaconteurMonitorEnter(self,timeout):
            self._lock.acquire()

        def RobotRaconteurMonitorExit(self):
            self._lock.release()

To request a monitor lock on the client side, use:

.. code:: python

  with RR.RobotRaconteurNode.ScopedMonitorLock(obj):
    # your code ...

The lock will be released when the ``with`` statement block is exited.