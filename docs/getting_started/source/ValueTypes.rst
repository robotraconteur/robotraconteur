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
