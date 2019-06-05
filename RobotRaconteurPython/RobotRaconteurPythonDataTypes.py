# Copyright 2011-2019 Wason Technology, LLC
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

class EventHook(object):

    def __init__(self):
        self.__handlers = []

    def __iadd__(self, handler):
        self.__handlers.append(handler)
        return self

    def __isub__(self, handler):
        self.__handlers.remove(handler)
        return self

    def fire(self, *args, **keywargs):
        for handler in self.__handlers:
            handler(*args, **keywargs)

    def clearObjectHandlers(self, inObject):
        for theHandler in self.__handlers:
            if theHandler.im_self == inObject:
                self -= theHandler

class RobotRaconteurStructure(object):
    pass

class RobotRaconteurVarValue(object):
    def __init__(self,data,datatype):
        self.data=data
        self.datatype=datatype
        
    def __str__(self):
        return str(self.datatype) + ": " + str(self.data)
    
    def __repr__(self):
        return "RobotRaconteurVarValue: " + str(self.datatype) + ": " + repr(self.data)

class ArrayMemory(object):
    def __init__(self,memory=None):
        self.memory=memory

    def Attach(self,memory):
        self.memory=memory

    @property
    def Length(self):
        return len(self.memory)

    def Read(self,memorypos,buffer,bufferpos,count):
        buffer[bufferpos:(bufferpos+count)]=self.memory[memorypos:(memorypos+count)]

    def Write(self,memorypos,buffer,bufferpos,count):
        self.memory[memorypos:(memorypos+count)]=buffer[bufferpos:(bufferpos+count)]

class MultiDimArrayMemory(object):
    def __init__(self,memory=None):
        if (memory is None):
            self.memory=None        
        else:
            self.Attach(memory)

    def Attach(self,memory):
        
        if (not isinstance(memory,numpy.ndarray)): raise Exception("MultiDimArrayMemory memory must be MultiDimArray or numpy.ndarray type")
        self.memory=memory
    @property
    def Dimensions(self):
        return self.memory.shape

    @property
    def DimCount(self):
        return self.memory.ndim
    
    def Read(self,memorypos,buffer,bufferpos,count):
        memind=[slice(memorypos[i], (memorypos[i]+count[i])) for i in range(len(count))]
        bufind=[slice(bufferpos[i], (bufferpos[i]+count[i])) for i in range(len(count))]
        buffer[tuple(bufind)]=self.memory[tuple(memind)]

    def Write(self,memorypos,buffer,bufferpos,count):        
            memind=[slice(memorypos[i], (memorypos[i]+count[i])) for i in range(len(count))]
            bufind=[slice(bufferpos[i], (bufferpos[i]+count[i])) for i in range(len(count))]
            self.memory[tuple(memind)]=buffer[tuple(bufind)]

bool_dtype=numpy.uint8

from . import RobotRaconteurPython
