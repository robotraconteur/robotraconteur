# Copyright 2011-2018 Wason Technology, LLC
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

try:
    import numpy
except:
    pass

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

class MultiDimArray(object):
    """ <summary>
     Represents an unpacked MultiDimArray.  This class stores
     the real and complex data as column vectors that is
     reshaped based on Dims.
     </summary>
    """
    def __init__(self, Dims=None, Real=None, Imag=None):
        self.Complex = False
        if (not Dims is  None):
            self.DimCount = len(Dims)
        else:
            self.DimCount=[0]
        self.Dims = Dims
        self.Real = Real
        self.Imag = None
        if Imag != None:
            self.Complex = True
            self.Imag = Imag

    def RetrieveSubArray(self, memorypos, buffer, bufferpos, count):
        v=RobotRaconteurPython.vectorint32
        if self.Complex != buffer.Complex:
            raise Exception("Complex mismatch")       

        iter=RobotRaconteurPython.MultiDimArray_CalculateCopyIndicesBeginIter(self.DimCount, v(self.Dims), v(memorypos), buffer.DimCount, v(buffer.Dims), v(bufferpos), v(count))
        while True:
            (c, indexa, indexb, l)=iter.Next()
            if not c:
                return
            buffer.Real[indexb:(indexb+l)]=self.Real[indexa:(indexa+l)]
            if self.Complex:         
                buffer.Imag[indexb:(indexb+l)]=self.Imag[indexa:(indexa+l)]

    def AssignSubArray(self, memorypos, buffer, bufferpos, count):
        v=RobotRaconteurPython.vectorint32  
        if self.Complex != buffer.Complex:
            raise Exception("Complex mismatch")
        iter=RobotRaconteurPython.MultiDimArray_CalculateCopyIndicesBeginIter(self.DimCount, v(self.Dims), v(memorypos), buffer.DimCount, v(buffer.Dims), v(bufferpos), v(count))
        while True:
            (c, indexa, indexb, l)=iter.Next()
            if not c:
                return
            #Array.Copy(memb.Real, indexb[i], mema.Real, indexa[i], count[0])
            self.Real[indexa:(indexa+l)]=buffer.Real[indexb:(indexb+l)]
            if self.Complex:
                #Array.Copy(memb.Imag, indexb[i], mema.Imag, indexa[i], count[0])
                self.Imag[indexa:(indexa+l)]=buffer.Imag[indexb:(indexb+l)]

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
            self.__isnumpy=False
        else:
            self.Attach(memory)

    def Attach(self,memory):
        if (isinstance(memory,MultiDimArray)):
            self.memory=memory
            self.__isnumpy=False
        else:
            if (not RobotRaconteurPython.RobotRaconteurNode.s.UseNumPy):
                raise Exception("Invalid MultiDimArrayMemory data type, numpy not enabled")
            if (type(memory) is list or type(memory) is set or type(memory) is tuple):
                if (len(memory)!=2): raise Exception("Complex numpy array tuple must be length 2")
                if (not isinstance(memory[0],numpy.ndarray) or not isinstance(memory[1],numpy.ndarray)): raise Exception("MultiDimArrayMemory memory must be MultiDimArray or numpy.ndarray type")
                if (not numpy.array_equal(memory[0].shape,memory[1].shape)): raise Exception("Complex matrices must have same shape")
                self.memory=memory
                self.__isnumpy=True
                self.__isnumpy_list=True
            else:

                if (not isinstance(memory,numpy.ndarray)): raise Exception("MultiDimArrayMemory memory must be MultiDimArray or numpy.ndarray type")
                self.__isnumpy=True
                self.__isnumpy_list=False
                self.memory=memory
    @property
    def Dimensions(self):
        if (not self.__isnumpy):
            return self.memory.Dims
        else:
            if (self.__isnumpy_list):
                return self.memory[0].shape
            else:
                return self.memory.shape

    @property
    def DimCount(self):
        if (not self.__isnumpy):
            return self.memory.DimCount
        else:
            if (self.__isnumpy_list):
                return self.memory[0].ndim
            else:
                return self.memory.ndim

    @property
    def Complex(self):
        if (not self.__isnumpy):
            return self.memory.Complex
        else:
            if (self.__isnumpy_list):
                return True
            else:
                return (self.memory.dtype==numpy.complex64 or self.memory.dtype==numpy.complex128)

    def Read(self,memorypos,buffer,bufferpos,count):
        if (not self.__isnumpy):
            self.memory.RetrieveSubArray(memorypos,buffer,bufferpos,count)
        else:
            memind=[slice(memorypos[i], (memorypos[i]+count[i])) for i in range(len(count))]
            bufind=[slice(bufferpos[i], (bufferpos[i]+count[i])) for i in range(len(count))]
            if (self.__isnumpy_list):
                realb=buffer[0]
                imagb=buffer[1]
                realm=self.memory[0]
                imagm=self.memory[1]
                realb[bufind]=realm[memind]
                imagb[bufind]=imagm[memind]
            else:
                buffer[bufind]=self.memory[memind]

    def Write(self,memorypos,buffer,bufferpos,count):
        if (not self.__isnumpy):
            self.memory.AssignSubArray(memorypos,buffer,bufferpos,count)
        else:
            memind=[slice(memorypos[i], (memorypos[i]+count[i])) for i in range(len(count))]
            bufind=[slice(bufferpos[i], (bufferpos[i]+count[i])) for i in range(len(count))]
            if (self.__isnumpy_list):
                realb=buffer[0]
                imagb=buffer[1]
                realm=self.memory[0]
                imagm=self.memory[1]
                realm[memind]=realb[bufind]
                imagm[memind]=imagb[bufind]
            else:
                self.memory[memind]=buffer[bufind]

class CStructureMultiDimArray(object):
    def __init__(self, Dims=None, cstruct_array=None):
        self.Dims=Dims
        self.cstruct_array=cstruct_array
    
    def RetrieveSubArray(self, memorypos, buffer, bufferpos, count):
        v=RobotRaconteurPython.vectorint32
        iter_=RobotRaconteurPython.MultiDimArray_CalculateCopyIndicesBeginIter(len(self.Dims), v(self.Dims), v(memorypos), len(buffer.Dims), v(buffer.Dims), v(bufferpos), v(count))
        while True:
            (c, indexa, indexb, l)=iter_.Next()
            if not c:
                return
            buffer.cstruct_array[indexb:(indexb+l)]=self.cstruct_array[indexa:(indexa+l)]
            
    def AssignSubArray(self, memorypos, buffer, bufferpos, count):
        v=RobotRaconteurPython.vectorint32  
        iter_=RobotRaconteurPython.MultiDimArray_CalculateCopyIndicesBeginIter(len(self.Dims), v(self.Dims), v(memorypos), len(buffer.Dims), v(buffer.Dims), v(bufferpos), v(count))
        while True:
            (c, indexa, indexb, l)=iter_.Next()
            if not c:
                return            
            self.cstruct_array[indexa:(indexa+l)]=buffer.cstruct_array[indexb:(indexb+l)]
            

from . import RobotRaconteurPython
