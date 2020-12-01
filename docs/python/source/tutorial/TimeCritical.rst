.. _realtimewire:

Time-critical software with Wire member
=======================================

The “Wire" member is a unique feature of Robot Raconteur that is designed to transmit a constantly changing value. This
is intended to emulate a physical wire carrying an analog value, but is capable of carrying any valid Robot Raconteur
data type. The “wire" is full-duplex meaning that it can send
data in both directions, (unless using ``readonly`` or ``writeonly`` modifiers) and it only provides the latest value. 
When the ``OutValue`` of one ``WireConnection`` is
changed, a packet is generated that contains the data and a timestamp of type ``TimeSpec``. This packet is transmitted
through the channel and received by the other ``WireConnection`` in the pair. If the timestamp is newer, the ``InValue``
is updated. The timestamps can also be read through ``LastValueReceivedTime`` and ``LastValueSentTime``.

The Wire member is **non-blocking**, meaning that the ``OutValue`` is set, the new value will be placed in the send
queue and control will return immediately to the caller. If the an older value exists it will be discarded and replaced
with the new value.

The ``LastValueReceivedTime`` property can be used to detect how old the ``InValue`` data is. The ``TimeSpec`` returned
is in the **remote** node’s clock. This means that it cannot be compared directly to the local node clock. Clock
synchronization is not directly supported by Robot Raconteur. Different transports may provide this functionality.

Using the wire ``InValueLifespan`` property is recommended to prevent stale data from being used.

Robot Raconteur will in the future add real-time “sideband" transports for Wire connections. This will be implemented
using the QUIC transport.
