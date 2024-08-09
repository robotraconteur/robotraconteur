Webcam Example
==============

Webcam Service
--------------

The example robot also has webcams that can be accessed using the Python OpenCV libraries. Example
`simple_webcam_service_multi.py <https://github.com/robotraconteur/robotraconteur/blob/master/examples/simple_webcam/python/service/simple_webcam_service_multi.py>`_
on GitHub is a program that exposes the webcams as a Robot Raconteur
service. The example is intended to demonstrate the usage of the ``objref``, ``pipe``, and ``memory`` members that were not
used in the iRobot Create examples.

.. code-block::

  #Service to provide sample interface to webcams

  # This interface is for example only. Most cameras should use the standard com.robotraconteur.imaging.Camera interface
  service experimental.simplewebcam3

  # The current version of Robot Raconteur robdef standards is 0.10
  stdver 0.10

  struct WebcamImage
      field int32 width
      field int32 height
      field int32 step
      field uint8[] data
  end

  struct WebcamImage_size
      field int32 width
      field int32 height
      field int32 step
  end

  object Webcam
      property string name [readonly]
      function WebcamImage capture_frame()

      function void start_streaming()
      function void stop_streaming()
      pipe WebcamImage frame_stream [readonly]

      function WebcamImage_size capture_frame_to_buffer()
      memory uint8[] buffer [readonly]
      memory uint8[*] multidimbuffer [readonly]

  end

  object WebcamHost
      property string{int32} webcam_names [readonly]
      objref Webcam{int32} webcams
  end

The service definition for the ``experimental.simplewebcam3`` shown in above
contains two objects: ``WebcamHost`` and ``Webcam``. The ``Webcam`` object type represents a single camera, and the
``WebcamHost`` object allows for the client to determine the number of webcams and retrieve the ``Webcam`` objects
through an ``objref`` member.

The class ``WebcamHost_impl`` implements the ``WebcamHost`` object type. The property ``webcam_names`` returns a map of
the indexes and names of the cameras, and is an example of the ``string{int32}`` Robot Raconteur type. The function
``get_webcams`` implements the ``webcams`` objref. Note that the objref is implemented by prepending ``get\_`` to the name
of the objref member. The index may come as a ``string`` even though an ``int32`` is expected, so convert the type to
``int`` before using. When returning an object from an objref, it is necessary to return the fully qualified Robot
Raconteur type of the object as a second parameter.

**Note: objects can only be registered as a service object ONCE. Objects cannot be returned by two separate objrefs.
Objrefs must form a “tree" structure, where the child branches are the return objects from objrefs.**

The ``Webcam_impl`` object implements the webcam functionality. The ``capture_frame`` function returns a single frame to
the client. The ``start_streaming`` and ``stop_streaming`` functions begin or stop a thread implemented by the
``frame_threadfunc`` function that sends streaming frames to the connected clients through the ``frame_stream`` pipe.

A ``PipeBroadcaster`` is used for this example. The ``PipeBroadcaster`` is similar to the ``WireBroadcaster``, sending
packets to all connected clients. While a ``PipeBroadcaster`` can be inferred for a ``readonly`` pipe and the attribute
set in the same manner as a ``WireBroadcaster``, for this example the ``PipeBroadcaster`` is initialized by the object
so the ``backlog`` can be specified. The ``backlog`` is used for flow control. If there are more packets “in-flight”
than the specified maximum, more will not be sent. The property “FrameStream” is implemented, with the getter
initializing the ``PipeBroadcaster``. The function ``frame_threadfunc`` demonstrates using the ``PipeBroadcaster`` to
send frames to the clients.

| The final members of interest in the ``WebcamService`` are the two memories, ``buffer`` and
| ``multidimbuffer``. These two members demonstrate how to use two flavors of memories that are either single
  dimensional or multi-dimensional. Memories are useful when data greater than about 10 MB needs to be transmitted
  between client and server, when there is a random-access block of memory, or in the future for shared memory
  applications. The function ``capture_frame_to_buffer`` captures the data and saves it to the buffers. Note that
  multi-dimensional arrays in Python are simply multi-dimensional NumPy arrays. Some processing is done to place the
  data in “Matlab" style image formats. A structure of type ``experimental.simplewebcam3.WebcamImage_size`` is returned to
  tell the client how big the image is.

The two memories are implemented in Python using properties. The ``buffer`` member returns an ``ArrayMemory`` object,
and the ``multidimbuffer`` returns a ``MultiDimArrayMemory`` object. Both contain their respective array and
multi-dimensional array. In this example a new memory object is returned every time. This is not generally the best way
to use the memory; instead, a persistent memory object should be used with a persistent memory block.

Webcam Client
-------------

Example `simple_webcam_client.py <https://github.com/robotraconteur/robotraconteur/blob/master/examples/simple_webcam/python/client/simple_webcam_client.py>`_
on GitHub is a program that will read the webcams and display the images. The
initialization and connection are similar to the iRobot Create example. The main difference is the use of the ``objrefs``,
which are used to get references to the webcams ``c1`` and ``c2``:

``c1=c_host.get_webcams(0)``

``c2=c_host.get_webcams(1)``

The rest of the program deals with OpenCV related functions to show the images.

Webcam Client (streaming)
-------------------------

Example `simple_webcam_host_streaming.py <https://github.com/robotraconteur/robotraconteur/blob/master/examples/simple_webcam/python/client/simple_webcam_client_streaming.py>`_
is a program that provides a “live" view of the
camera, although depending on the speed of the computer it may be fairly slow because Python is an interpreted language.
The program connects and retrieves the webcam object reference ``c`` the same way as the previous example, and the
connects to the pipe ``frame_stream``. The pipe index is given as the argument, and -1 means *any index*.

``p=c.frame_stream.Connect(-1)``

Next, a callback is added so that the function ``new_frame`` will be called when a new pipe packet arrives.

``p.PacketReceivedEvent+=new_frame``

This function will be called from a different thread by Robot Raconteur when a new frame has arrived. In the
``new_frame`` function, the variable ``current_frame`` is updated with the new value. The ``Available`` property in the
PipeEndpoint provides the number of packets waiting, and the ``ReceivePacket`` retrieves the next packet. Packets always
arrive in order.

The rest of the program handles showing the images as they arrive and shutting down, including closing the pipe.

``p.Close()``

Note that this example uses the single webcam version of the service ``simple_webcam_service.py``.

Webcam Client (memory)
----------------------

Example `simple_webcam_host_memory.py <https://github.com/robotraconteur/robotraconteur/blob/master/examples/simple_webcam/python/client/simple_webcam_client_memory.py>`_
on GitHub demonstrates the use of the memories. The memories have
functions ``Read`` and ``Write`` that allow for a segment of the memory to be read or written into or from a buffer. The
memory position, buffer, buffer position, and count are passed. For multi-dimensional arrays, the memory position,
buffer position, and count are lists. The ArrayMemory has the special property "Length" for the length of the array, and
the MultiDimArrayMemory has the special properties ``Dims``, ``DimCount``, and ``Complex``.

Note that this example uses the single webcam version of the service ``simple_webcam_service.py``.
