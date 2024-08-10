.. _value_types:

===========
Value Types
===========

.. contents:: Table of Contents
   :depth: 1
   :local:
   :backlinks: none

Overview
========

Robot Raconteur passes "Value Types" between clients and services by value. The values are "serialized" to
a binary format that can be transmitted over a network. The values are then "deserialized" back into the
original data type. The serialization and deserialization process is handled transparently by the
Robot Raconteur library.  Because Robot Raconteur supports multiple programming languages, the value types
are defined using Robot Raconteur "Service Definitions," which provide a common language-independent
format for defining data types. The Value Types are carefully designed to support robotics and automation
applications.

See also the `Robot Raconteur Framework Service Definition Documentation <https://robotraconteur.github.io/robotraconteur/doc/core/latest/cpp/service_definition.html>`_

The following data types are supported by Robot Raconteur:

* `bool` - A boolean value that can be either ``true`` or ``false``.
  * Arrays and multidimensional arrays of booleans.
* Numeric types including ``uint8``, ``int8``, ``uint16``, ``int16``, ``uint32``, ``int32``, ``uint64``, ``int64``,
  ``single``, ``double``, ``csingle``, and ``cdouble``.
  * Arrays and multidimensional arrays of numeric types.
* `string` - A string of characters.
* `struct` - A structure that contains a collection of fields.
* `pod` - A "Plain Old Data" structure that contains a collection of fields that are stored in contiguous memory.
  * Arrays and multidimensional arrays of POD types.
* `namedarray` - A structure that contains a collection of fields that are stored as a union between a structure and a
  numeric array in contiguous memory.
  * Arrays and multidimensional arrays of namedarray types.
* Collection types including ``list``, ``map{int32}``, and ``map{string}``.
  * Any of the above types can be used as the value type in a collection type.

The `struct`, `pod`, and `namedarray` types are defined using the Robot Raconteur "Service Definitions" format.

Object "members" are used to interact with services and pass data between clients and services. The examples
on this page use `property` types to pass data between clients and services. The value types can all
be used with all member types.

The example service definition ``experimental.value_types`` is used for the examples on this page. The service
definition is shown below:

.. collapse:: experimental.value_types.robdef (click to expand)

    .. literalinclude:: ../../../examples/features/value_types/robdef/experimental.value_types.robdef
        :linenos:

|
Numeric Types
=============

Robot Raconteur supports numeric types including ``uint8``, ``int8``, ``uint16``, ``int16``, ``uint32``, ``int32``,
``uint64``, ``int64``, ``single``, ``double``, ``csingle``, and ``cdouble``. For the purpose of this section,
``bool`` is also considered a numeric type. Scalar numbers are converted to the built-in numeric types in
the client programming language. Numeric types can also be stored in arrays and multidimensional arrays.

The following members from the ``experimental.value_types.ValueTypesExample`` object are used in the examples
for numeric types:

.. literalinclude:: ../../../examples/features/value_types/robdef/experimental.value_types.robdef
    :lines: 55-70

The examples below show how to use the numeric types in Python, MATLAB, LabView, C\#, and C++.

.. tabs::

    .. group-tab:: Python

        .. literalinclude:: ../../../examples/features/value_types/python/numeric_value_types.py
            :language: python
            :linenos:

    .. group-tab:: MATLAB

        .. literalinclude:: ../../../examples/features/value_types/matlab/numeric_value_types.m
            :language: matlab
            :linenos:

    .. group-tab:: LabView

        .. raw:: html

            <div style="overflow: auto; width: 100%; height: 400px;">
                <img src="https://raw.githubusercontent.com/robotraconteur/robotraconteur/master/examples/features/value_types/labview/numeric_value_types.png" style="max-width: none;">
            </div>

    .. group-tab:: C\#

        .. literalinclude:: ../../../examples/features/value_types/cs/numeric_value_types.cs
            :language: csharp
            :linenos:

    .. group-tab:: C++

        .. literalinclude:: ../../../examples/features/value_types/cpp/numeric_value_types.cpp
            :language: cpp
            :linenos:

String Type
===========

Robot Raconteur uses the built-in string types in the client programming language. Note that strings
cannot be stored in arrays since internally strings are already
stored as arrays of characters.

The following members from the ``experimental.value_types.ValueTypesExample`` object are used in the examples
for numeric types:

.. literalinclude:: ../../../examples/features/value_types/robdef/experimental.value_types.robdef
    :lines: 72

The examples below show how to use strings in Python, MATLAB, LabView, C\#, and C++.

.. tabs::

    .. group-tab:: Python

        .. literalinclude:: ../../../examples/features/value_types/python/string_value_type.py
            :language: python
            :linenos:

    .. group-tab:: MATLAB

        .. literalinclude:: ../../../examples/features/value_types/matlab/string_value_type.m
            :language: matlab
            :linenos:

    .. group-tab:: LabView

        .. raw:: html

            <div style="overflow: auto; width: 100%; height: 400px;">
                <img src="https://raw.githubusercontent.com/robotraconteur/robotraconteur/master/examples/features/value_types/labview/string_value_type.png" style="max-width: none;">
            </div>

    .. group-tab:: C\#

        .. literalinclude:: ../../../examples/features/value_types/cs/string_value_type.cs
            :language: csharp
            :linenos:

    .. group-tab:: C++

        .. literalinclude:: ../../../examples/features/value_types/cpp/string_value_type.cpp
            :language: cpp
            :linenos:

Structure Types
===============

Structures are a collection of fields that can contain any value type, including itself. Structures
are defined using the Robot Raconteur "Service Definitions" format. Structures are nullable. The following examples
demonstrate using the ``experimental.value_types.MyStructure`` structure type:

.. literalinclude:: ../../../examples/features/value_types/robdef/experimental.value_types.robdef
    :lines: 42-49


The following members from the ``experimental.value_types.ValueTypesExample`` object are used in the examples
for structure types:

.. literalinclude:: ../../../examples/features/value_types/robdef/experimental.value_types.robdef
    :lines: 85

The examples below show how to use structures in Python, MATLAB, LabView, C\#, and C++.

.. tabs::

    .. group-tab:: Python

        .. literalinclude:: ../../../examples/features/value_types/python/struct_value_types.py
            :language: python
            :linenos:

    .. group-tab:: MATLAB

        .. literalinclude:: ../../../examples/features/value_types/matlab/struct_value_types.m
            :language: matlab
            :linenos:

    .. group-tab:: LabView

        .. raw:: html

            <div style="overflow: auto; width: 100%; height: 400px;">
                <img src="https://raw.githubusercontent.com/robotraconteur/robotraconteur/master/examples/features/value_types/labview/struct_value_types.png" style="max-width: none;">
            </div>

    .. group-tab:: C\#

        .. literalinclude:: ../../../examples/features/value_types/cs/struct_value_types.cs
            :language: csharp
            :linenos:

    .. group-tab:: C++

        .. literalinclude:: ../../../examples/features/value_types/cpp/struct_value_types.cpp
            :language: cpp
            :linenos:

Pod Types
=========

Pods are a collections of fields that have a fixed memory layout. Pods have a restricted set of allowed
field types including numbers, arrays, namedarrays, and other pods. See the
`Robot Raconteur Framework Service Definition Documentation <https://robotraconteur.github.io/robotraconteur/doc/core/latest/cpp/service_definition.html>`_
for more information about the allowed contents of pods. Unlike structures, pods store all their
data within the pod itself in contiguous memory, and do not contain pointers to other data. They are guaranteed
to have a maximum binary size. The following examples
demonstrate using the ``experimental.value_types.MyPod`` pod type:

.. literalinclude:: ../../../examples/features/value_types/robdef/experimental.value_types.robdef
    :lines: 27-40

The following members from the ``experimental.value_types.ValueTypesExample`` object are used in the examples
for pod types:

.. literalinclude:: ../../../examples/features/value_types/robdef/experimental.value_types.robdef
    :lines: 80-82

The examples below show how to use pods in Python, MATLAB, LabView, C\#, and C++.

.. tabs::

    .. group-tab:: Python

        .. literalinclude:: ../../../examples/features/value_types/python/pod_value_types.py
            :language: python
            :linenos:

    .. group-tab:: MATLAB

        .. literalinclude:: ../../../examples/features/value_types/matlab/pod_value_types.m
            :language: matlab
            :linenos:

    .. group-tab:: LabView

        .. raw:: html

            <div style="overflow: auto; width: 100%; height: 400px;">
                <img src="https://raw.githubusercontent.com/robotraconteur/robotraconteur/master/examples/features/value_types/labview/pod_value_types.png" style="max-width: none;">
            </div>

    .. group-tab:: C\#

        .. literalinclude:: ../../../examples/features/value_types/cs/pod_value_types.cs
            :language: csharp
            :linenos:

    .. group-tab:: C++

        .. literalinclude:: ../../../examples/features/value_types/cpp/pod_value_types.cpp
            :language: cpp
            :linenos:

NamedArray Types
================

Namedarrays represent a union type between a structure and a numeric array. A vector is an example
of a type that can be represented either as a structure with three fields, or as a numeric array with
three elements. Namedarrays are very efficient compared to the other data types since they can
be transmitted or manipulated as a plain array. All fields in the namedarray must have
the same numeric type. Other namedarrays and arrays may also be used as fields, but they must
have the same underlying numeric type and a fixed size. The following examples demonstrate using the
``experimental.value_types.MyVector3``, ``experimental.value_types.MyQuaternion``, and ``experimental.value_types.MyPose``
namedarray types:

.. literalinclude:: ../../../examples/features/value_types/robdef/experimental.value_types.robdef
    :lines: 5-25

The following members from the ``experimental.value_types.ValueTypesExample`` object are used in the examples
for namedarray types:

.. literalinclude:: ../../../examples/features/value_types/robdef/experimental.value_types.robdef
    :lines: 74-78

The examples below show how to use namedarrays in Python, MATLAB, LabView, C\#, and C++.

.. tabs::

    .. group-tab:: Python

        .. literalinclude:: ../../../examples/features/value_types/python/namedarray_value_types.py
            :language: python
            :linenos:

    .. group-tab:: MATLAB

        .. literalinclude:: ../../../examples/features/value_types/matlab/namedarray_value_types.m
            :language: matlab
            :linenos:

    .. group-tab:: LabView

        .. raw:: html

            <div style="overflow: auto; width: 100%; height: 400px;">
                <img src="https://raw.githubusercontent.com/robotraconteur/robotraconteur/master/examples/features/value_types/labview/namedarray_value_types.png" style="max-width: none;">
            </div>

    .. group-tab:: C\#

        .. literalinclude:: ../../../examples/features/value_types/cs/namedarray_value_types.cs
            :language: csharp
            :linenos:

    .. group-tab:: C++

        .. literalinclude:: ../../../examples/features/value_types/cpp/namedarray_value_types.cpp
            :language: cpp
            :linenos:

Container Types
===============

Robot Raconteur containers store a collection of values using lists and maps. The available container
types are ``list``, ``map{int32}``, and ``map{string}``. The container types can store any value type,
except for other containers. Containers are nullable.

The following members from the ``experimental.value_types.ValueTypesExample`` object are used in the examples
for container types:

.. literalinclude:: ../../../examples/features/value_types/robdef/experimental.value_types.robdef
    :lines: 87-98

The examples below show how to use containers in Python, MATLAB, LabView, C\#, and C++.

.. tabs::

    .. group-tab:: Python

        .. literalinclude:: ../../../examples/features/value_types/python/container_value_types.py
            :language: python
            :linenos:

    .. group-tab:: MATLAB

        .. literalinclude:: ../../../examples/features/value_types/matlab/container_value_types.m
            :language: matlab
            :linenos:

    .. group-tab:: LabView

        .. raw:: html

            <div style="overflow: auto; width: 100%; height: 400px;">
                <img src="https://raw.githubusercontent.com/robotraconteur/robotraconteur/master/examples/features/value_types/labview/container_value_types.png" style="max-width: none;">
            </div>

    .. group-tab:: C\#

        .. literalinclude:: ../../../examples/features/value_types/cs/container_value_types.cs
            :language: csharp
            :linenos:

    .. group-tab:: C++

        .. literalinclude:: ../../../examples/features/value_types/cpp/container_value_types.cpp
            :language: cpp
            :linenos:

varvalue Type
=============

Varvalue is a special type that can store any value type, and is a form of variant. See the language
documentation for more details on how the variant is implemented in each language. The varvalue type
is nullable.

The following members from the ``experimental.value_types.ValueTypesExample`` object are used in the examples
for varvalue types:

.. literalinclude:: ../../../examples/features/value_types/robdef/experimental.value_types.robdef
    :lines: 100-107

The examples below show how to use varvalue in Python, MATLAB, LabView, C\#, and C++.

.. tabs::

    .. group-tab:: Python

        .. literalinclude:: ../../../examples/features/value_types/python/varvalue_value_types.py
            :language: python
            :linenos:

    .. group-tab:: MATLAB

        .. literalinclude:: ../../../examples/features/value_types/matlab/varvalue_value_types.m
            :language: matlab
            :linenos:

    .. group-tab:: LabView

        .. raw:: html

            <div style="overflow: auto; width: 100%; height: 400px;">
                <img src="https://raw.githubusercontent.com/robotraconteur/robotraconteur/master/examples/features/value_types/labview/varvalue_value_types.png" style="max-width: none;">
            </div>

    .. group-tab:: C\#

        .. literalinclude:: ../../../examples/features/value_types/cs/varvalue_value_types.cs
            :language: csharp
            :linenos:

    .. group-tab:: C++

        .. literalinclude:: ../../../examples/features/value_types/cpp/numeric_value_types.cpp
            :language: cpp
            :linenos:

Nullable Types
==============

Structures, containers, and varvalues are nullable, meaning they can be set to ``null``. All other
value types are not nullable.

The following members from the ``experimental.value_types.ValueTypesExample`` object are used in the examples
for nullable types:

.. literalinclude:: ../../../examples/features/value_types/robdef/experimental.value_types.robdef
    :lines: 110

The examples below show how to use nullable types in Python, MATLAB, LabView, C\#, and C++.

.. tabs::

    .. group-tab:: Python

        .. literalinclude:: ../../../examples/features/value_types/python/null_value_types.py
            :language: python
            :linenos:

    .. group-tab:: MATLAB

        .. literalinclude:: ../../../examples/features/value_types/matlab/null_value_types.m
            :language: matlab
            :linenos:

    .. group-tab:: LabView

        .. raw:: html

            <div style="overflow: auto; width: 100%; height: 400px;">
                <img src="https://raw.githubusercontent.com/robotraconteur/robotraconteur/master/examples/features/value_types/labview/null_value_types.png" style="max-width: none;">
            </div>

    .. group-tab:: C\#

        .. literalinclude:: ../../../examples/features/value_types/cs/null_value_types.cs
            :language: csharp
            :linenos:

    .. group-tab:: C++

        .. literalinclude:: ../../../examples/features/value_types/cpp/null_value_types.cpp
            :language: cpp
            :linenos:
