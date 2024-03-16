# Copyright 2011-2020 Wason Technology, LLC
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

from __future__ import absolute_import

import numpy
import sys

if sys.version_info[0] >= 3:
    import reprlib


class EventHook(object):
    """
    EventHook is used to implement multiple listener events in
    Robot Raconteur Python. Callback functions are registered with the
    EventHook and are all notified when the event is fired.
    """
    __slots__ = "__handlers"

    def __init__(self):
        """
        Initialize new instance
        """
        self.__handlers = []

    def __iadd__(self, handler):
        """
        Adds a callback function to be notified of events
        """
        self.__handlers.append(handler)
        return self

    def __isub__(self, handler):
        """
        Removes a callback function
        """
        self.__handlers.remove(handler)
        return self

    def fire(self, *args, **keywargs):
        """
        Fires the event
        """
        for handler in self.__handlers:
            handler(*args, **keywargs)

    def clearObjectHandlers(self, inObject):
        for theHandler in self.__handlers:
            if theHandler.im_self == inObject:
                self -= theHandler


class RobotRaconteurStructure(object):
    __slots__ = []

    if sys.version_info[0] >= 3:
        @reprlib.recursive_repr()
        def __repr__(self):
            return '{}({})'.format(self.__class__.__name__, ', '.join(['{}={}'.format(k,
                                                                                      repr(getattr(self, k))) for k in self.__slots__]))


class RobotRaconteurVarValue(object):
    __slots__ = ["data", "datatype"]

    def __init__(self, data, datatype):
        self.data = data
        """The data stored in the ``varvalue``"""
        self.datatype = datatype
        """(str,RobotRaconteur.TypeDefinition) The type of the ``varvalue`` data"""

    def __str__(self):
        return str(self.datatype) + ": " + str(self.data)

    def __repr__(self):
        return "RobotRaconteurVarValue: " + str(self.datatype) + ": " + repr(self.data)


VarValue = RobotRaconteurVarValue


class ArrayMemory(object):
    """
    The ``ArrayMemory`` is designed to represent a large array that is read in smaller
    pieces. It is used with the \"memory\" member to allow for random access to an array.
    """
    __slots__ = ["memory"]

    def __init__(self, memory=None):
        self.memory = memory
        """(numpy.ndarray) The underlying array of the memory"""

    def Attach(self, memory):
        """
        Attach the memory to an array

        :param memory: The data to attach
        :type memory: numpy.ndarray
        """
        self.memory = memory

    @property
    def Length(self):
        """
        The length of the memory

        :rtype: int
        """
        return len(self.memory)

    def Read(self, memorypos, buffer, bufferpos, count):
        """
        Reads data from the memory into ``buffer``

        :param memorypos: The start position in the memory array
        :type memorypos: int
        :param buffer: The buffer to read the data into
        :type buffer: numpy.ndarray
        :param bufferpos: The start position in the supplied buffer
        :type bufferpos: int
        :param count: The number of elements to read
        :type count: int
        """
        buffer[bufferpos:(bufferpos + count)
               ] = self.memory[memorypos:(memorypos + count)]

    def Write(self, memorypos, buffer, bufferpos, count):
        """
        Writes data from ``buffer`` into the memory

        :param memorypos: The start position in the memory array
        :type memorypos: int
        :param buffer: The buffer to write the data from
        :type buffer: numpy.ndarray
        :param bufferpos: The start position in the supplied buffer
        :type bufferpos: int
        :param count: The number of elements to write
        :type count: int
        """
        self.memory[memorypos:(memorypos + count)
                    ] = buffer[bufferpos:(bufferpos + count)]


class MultiDimArrayMemory(object):
    """
    The ``MultiDimArrayMemory`` is designed to represent a large
    multi-dimensional array that is read in smaller pieces. It is used with the
    \"memory\" member to allow for random access to an multi-dimensional array.  It works with
    numpy.ndarray.  For the ``memorypos``, ``bufferpos``, and ``count`` parameters in the
    functions, a list is used to represent the dimensions.  The dimensions are column-major
    as is numpy.ndarray.
    """
    __slots__ = ["memory"]

    def __init__(self, memory=None):
        if (memory is None):
            self.memory = None
            """(numpy.ndarray) The underlying array of the memory"""
        else:
            self.Attach(memory)

    def Attach(self, memory):
        """
        Attach the memory to an array

        :param memory: The data to attach
        :type memory: numpy.ndarray
        """
        if (not isinstance(memory, numpy.ndarray)):
            raise Exception(
                "MultiDimArrayMemory memory must be numpy.ndarray type")
        self.memory = memory

    @property
    def Dimensions(self):
        """
        The dimensions of the memory

        :rtype: List[int]
        """
        return self.memory.shape

    @property
    def DimCount(self):
        """
        The number of dimensions

        :rtype: int
        """
        return self.memory.ndim

    def Read(self, memorypos, buffer, bufferpos, count):
        """
        Reads data from the memory into ``buffer``

        :param memorypos: The start position in the array
        :type memorypos: List[int]
        :param buffer: The buffer to read the data into
        :type buffer: numpy.ndarray
        :param bufferpos: The start position int he buffer
        :type bufferpos: List[int]
        :param count: The shape of elements to read
        :type count: List[int]
        """
        memind = [slice(memorypos[i], (memorypos[i] + count[i]))
                  for i in range(len(count))]
        bufind = [slice(bufferpos[i], (bufferpos[i] + count[i]))
                  for i in range(len(count))]
        buffer[tuple(bufind)] = self.memory[tuple(memind)]

    def Write(self, memorypos, buffer, bufferpos, count):
        """
        Writes data into the memory from ``buffer``

        :param memorypos: The start position in the array
        :type memorypos: List[int]
        :param buffer: The buffer to write the data from
        :type buffer: numpy.ndarray
        :param bufferpos: The start position in the buffer
        :type bufferpos: List[int]
        :param count: The shape of elements to write
        :type count: List[int]
        """
        memind = [slice(memorypos[i], (memorypos[i] + count[i]))
                  for i in range(len(count))]
        bufind = [slice(bufferpos[i], (bufferpos[i] + count[i]))
                  for i in range(len(count))]
        self.memory[tuple(memind)] = buffer[tuple(bufind)]


bool_dtype = numpy.uint8

from . import RobotRaconteurPython
