# Object Locking {#locking}

Object locking is used to request exclusive access to objects. This prevents multiple clients from accessing objects. Robot Raconteur has three types of locks "User", "Client", and "Monitor".

## User {#user_locking}

The "User" lock provides a lock on an object within a service that is exclusive to a specific username.  The user must be authenticated and have the "objectlock" privilege.  The same user can authenticate multiple times from any location using this lock.  The lock works on the selected object, and all objects below the current object in the "objref" tree. (This means all objects that are referenced by the locked object's objrefs.)

Objects are locked by the client using the `RequestObjectLock` request, with an object specified. When locked, only the requsting user will be able to access the object. The lock is released with the `ReleaseObjectLock` request. Services may also lock and unlock objects directly.

If the user has the privilege "objectlockoverride" the user can release all locks even if the user did not initiate the lock.

Note that the lock will prevent requst operations from occurring, but will not stop wire connections, pipe endpoints, and events from functioning normally.  If exclusive wire and pipe connections are required, conflicting wire and pipe connections will need to be closed by the service object.

The "user" object lock will also lock all sub-`objref` in the service path hierarchy.

## Client {#client_locking}

The "Client" lock is identical to the "User" lock but only allows the requesting client to access the object.  This means that the user cannot access the same service object from a different connection even with the same username. The client sends a `RequestClientObjectLock` request with an object specified. The service will lock the object and remember which client requested the lock.

## Monitor {#monitor_locking}

The "Monitor" lock provides a global thread monitor (more often called "mutex") lock on a single object.  This means that globally only the current thread with the lock can access the object.  The "Monitor" lock is intended for short operations that cannot be interrupted and will timeout if 15 seconds elapses between operations on the client.  It does not affect other objects in the service path like "User" and "Client" locks.  Unlike "User" and "Client", the object must implement the monitor locking functionality explicitly.
