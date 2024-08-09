.. _modifiers:

Member Modifiers
================

Member modifiers change the way a member behaves. Modifiers are specified between square brackets following the member
definition. The members ``distance_traveled``, ``angle_traveled``, ``bumbers``, and ``create_state`` in
"experimental.create." all use the ``readonly`` modifier. This means that the client can get these
values but cannot set them. The available member modifiers are as follows:

readonly
   |
   | **Valid Members:** ``property``, ``pipe``, ``wire``, ``memory``

   Specifies a member as readonly. For a ``wire``, the default implementation is ``WireBroadcaster``. For a ``pipe``,
   the default implementation is ``PipeBroadcaster``. For ``property`` and ``memory`` the setters are either disabled or
   not declared.

writeonly
   |
   | **Valid Members:** ``property``, ``pipe``, ``wire``, ``memory``

   Specifies a member as write. For a ``wire``, the default implementation is ``WireUnicastReceiver``. There is no
   default implementation for ``pipe``. For ``property`` and ``memory`` the getters are either disabled or not declared.

unreliable
   |
   | **Valid Members:** ``pipe``

   Specifies that a pipe is unreliable, meaning that packets may be lost and and/or returned out of order. Note that the
   ``unreliable`` keyword cannot be used with a ``PipeBroadcaster`` with a ``backlog`` value set. The backlog flow
   control requires that all sent packets arrive at their destination.

perclient
   |
   | **Valid Members:** ``property``

   Specifies that the value of a property is unique to the client. Most properties belong to the service and are the
   same for all connected clients. This modifier should be used for settings that are unique to each client such as
   session keys.

urgent
   |
   | **Valid Members:** ``property``, ``function``, ``event``

   Specifies that a member requires high priority delivery. This should be used for operations like aborting a motion.
   **Note that this does not qualify as a method to implement an e-stop.**
