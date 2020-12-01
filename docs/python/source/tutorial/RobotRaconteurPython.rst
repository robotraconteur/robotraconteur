Robot Raconteur Python
======================

This document uses Python to demonstrate how Robot Raconteur works, and also serves as the reference for the Python
Robot Raconteur library. The examples require that the Robot Raconteur Python library be installed.

**For Windows and Mac OSX, use the ``pip`` command to install Robot Raconteur from PyPi:**

``pip install robotraconteur``

Other packages are required to run the examples:

``pip install pyserial pygame opencv``

**For Ubuntu, use the Robot Raconteur PPA:**

``sudo add-apt-repository ppa:robotraconteur/ppa`` ``sudo apt update``

Once the PPA is configured, install the Robot Raconteur packages:

``sudo apt install python-robotraconteur python3-robotraconteur``

Other packages are required to run examples:

``sudo apt install python-pygame python-opencv python-pyserial``

Ubuntu packages are available for 16.04 (Xenial), 18.04 (Bionic) and 20.20 (Focal).

When using Robot Raconteur in Python, the “thunk" code require to handle different service and data types is handled
dynamically so there is no need to generate out extra source code. Instead, the client will receive an object that has
all the correct members automatically on connect, and a service simply needs to have the correct functions and
properties. How this is accomplished will be discussed through the rest of this document. Python uses “duck typing" so
it is not necessary to worry about inheritance or interfaces, the functions and properties just need to exist. A
significant advantage of Python’s dynamic typing is Robot Raconteur can generate client interface objects dynamically on
connect so a client does not need any prior information about the service it is connecting to.

Python :math:`\leftrightarrow` Robot Raconteur data type mapping
----------------------------------------------------------------

An important aspect to working with Robot Raconteur is understanding the mapping between Robot Raconteur types and the
native types in the language using Robot Raconteur. For Python these are a little more complicated because Python does
not have as strong a typing system as other languages. Robot Raconteur uses numpy arrays for all numeric arrays of all
shapes.

Table `1 <#pythontypemap>`__ shows the mapping between Robot Raconteur and Python data types. For simple arrays, Robot
Raconteur expects *column* NumPy arrays of the correct type. Multi-dim arrays are normal NumPy arrays of the correct
type.

Structures are initialized using a special command in ``RobotRaconteurNode`` called ``NewStructure``. The
``NewStructure`` command takes the fully qualified name of the structure, and an optional client object reference.
The function ``GetStructureType`` returns the structure type that can be used as a constructor.

Pods are represented as ``numpy.array`` using custom ``dtype``. These ``dtype`` are initialized using the
``GetPodDType`` command in ``RobotRaconteurNode``. The ``GetPodDType`` command takes the fully qualified name of the
structure, and an optional client object reference. The returned ``dtype`` can be used as parameter with the
``numpy.zeros(shape, dtype)`` to initialize an array with the pod type. Note that pods are always stored in
``numpy.array``. For a scalar, use ``numpy.zeros((1,),dtype)``. ``numpy.array`` uses “array” style indexing
for fields. For example, to access the "y" field in a 2 dimensional array at index (1,3), use ``myarray[1][3]['y']``.
This can be used to get or set the value.

Namedarrays are represented as ``numpy.array`` using custom ``dtype``. These ``dtype`` are initialized using the
``GetNamedArrayDType`` command in ``RobotRaconteurNode``. The ``GetNamedArrayDType`` command takes the fully qualified
name of the namedarray, and an optional client object reference. The returned ``dtype`` can be used as parameter with the
``numpy.zeros(shape, dtype)`` to initialize an array with the pod type. Note that pods are always stored in
``numpy.array``. For a scalar, use ``numpy.zeros((1,),dtype)``. " namedarray can be converted to a normal numeric array
using the ``NamedArrayToArray`` command in ``RobotRaconteurNode``. A normal numeric array can be converted to namedarray
using ``ArrayToNamedArray`` command in ``RobotRaconteurNode``. The first dimension of the numeric array must
match the total number of numeric elements in a scalar namedarray. The normal numeric arrays will have one more
dimension than the namedarray. ``numpy.array`` uses “array” style indexing for fields. For example, to access
the "y" field in a 2 dimensional array at index (1,3), use ``myarray[1][3]['y']``. This can be used to get or set the
value.

Maps are ``dict`` in Python.

Lists are ``list`` in Python.

Enums are stored as ``int`` in Python.

| 

.. container::
   :name: pythontypemap

   .. table:: Robot Raconteur :math:`\leftrightarrow` Python Type Map

      ======================================================== ========================== ====================================================
      Robot Raconteur Type                                     Python Type                Notes
      ======================================================== ========================== ====================================================
      double, single                                           ``float``                  
      cdouble, csingle                                         ``complex``                
      int8, uint8, int16, uint16, int32, uint32, int64, uint64 ``int`` or ``long``        Depends on sys.maxint size
      double[]                                                 ``numpy.array``            ``numpy.array([1, 2, ...], dtype=numpy.float64)``
      single[]                                                 ``numpy.array``            ``numpy.array([1, 2, ...], dtype=numpy.float32)``
      int8[]                                                   ``numpy.array``            ``numpy.array([1, 2, ...], dtype=numpy.int8)``
      uint8[]                                                  ``numpy.array``            ``numpy.array([1, 2, ...], dtype=numpy.uint8)``
      int16[]                                                  ``numpy.array``            ``numpy.array([1, 2, ...], dtype=numpy.int16)``
      uint16[]                                                 ``numpy.array``            ``numpy.array([1, 2, ...], dtype=numpy.uint16)``
      int32[]                                                  ``numpy.array``            ``numpy.array([1, 2, ...], dtype=numpy.int32)``
      uint32[]                                                 ``numpy.array``            ``numpy.array([1, 2, ...], dtype=numpy.uint32)``
      int64[]                                                  ``numpy.array``            ``numpy.array([1, 2, ...], dtype=numpy.int64)``
      uint64[]                                                 ``numpy.array``            ``numpy.array([1, 2, ...], dtype=numpy.uint64)``
      cdouble[]                                                ``numpy.array``            ``numpy.array([1, 2, ...], dtype=numpy.complex128)``
      csingle[]                                                ``numpy.array``            ``numpy.array([1, 2, ...], dtype=numpy.complex64)``
      Multi-dim arrays                                         ``numpy.array``            Type maps same as array, more dimensions
      string                                                   ``string`` or ``unicode``  ``unicode`` always returned
      Map (int32 key)                                          ``dict``                   All keys must be ``int``
      Map (string key)                                         ``dict``                   All keys must be ``string`` or ``unicode``
      List                                                     ``list``                   Standard list of expected type
      structure                                                *varies*                   See text for more info
      pods                                                     ``numpy.array``            See text for more info
      namedarrays                                              ``numpy.array``            See text for more info
      enums                                                    ``int``                    
      varvalue                                                 ``RobotRaconteurVarValue`` See text for more info
      ======================================================== ========================== ====================================================

      

Python Reference to Functions
-----------------------------

Robot Raconteur frequently uses function references (called function handles or function pointers) to implement
callbacks for events and other situations where the library needs to notify the software. In Python, this is
accomplished using function references (also called function objecs depending on the author). Consider a simple module
“MyModule" shown in the following example:

::


   class myobj(object):
       def hello1(name):
           print "Hello " + name

   def hello2(name):
       print "Hello " + name
       
   o=myobj()
   ref1=o.hello1
   ref2=hello2

   ref1("John")
   ref2("John")

This example demonstrates that a function reference can be easily made by referencing the function without the argument
parenthesis. This method works for module and class functions.