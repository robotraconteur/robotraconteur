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

Numeric Types
=============

Robot Raconteur supports numeric types including ``uint8``, ``int8``, ``uint16``, ``int16``, ``uint32``, ``int32``,
``uint64``, ``int64``, ``single``, ``double``, ``csingle``, and ``cdouble``. For the purpose of this section,
``bool`` is also considered a numeric type. Scalar numbers are converted to the built-in numeric types in
the client programming language. Numeric types can also be stored in arrays and multidimensional arrays.

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
                <img src="../../../examples/features/value_types/labview/numeric_value_types.png" style="max-width: none;">
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
                <img src="../../../examples/features/value_types/labview/string_value_type.png" style="max-width: none;">
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
                <img src="../../../examples/features/value_types/labview/struct_value_types.png" style="max-width: none;">
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
==========================

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
                <img src="../../../examples/features/value_types/labview/pod_value_types.png" style="max-width: none;">
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
                <img src="../../../examples/features/value_types/labview/namedarray_value_types.png" style="max-width: none;">
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
                <img src="../../../examples/features/value_types/labview/container_value_types.png" style="max-width: none;">
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
                <img src="../../../examples/features/value_types/labview/varvalue_value_types.png" style="max-width: none;">
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
                <img src="../../../examples/features/value_types/labview/null_value_types.png" style="max-width: none;">
            </div>

    .. group-tab:: C\#

        .. literalinclude:: ../../../examples/features/value_types/cs/null_value_types.cs
            :language: csharp
            :linenos:

    .. group-tab:: C++

        .. literalinclude:: ../../../examples/features/value_types/cpp/null_value_types.cpp
            :language: cpp
            :linenos:
