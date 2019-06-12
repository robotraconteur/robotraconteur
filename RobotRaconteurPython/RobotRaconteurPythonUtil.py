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

from . import RobotRaconteurPython
from .RobotRaconteurPythonDataTypes import *
from . import RobotRaconteurPythonError
import operator
import traceback
import threading
import functools
import sys
import warnings
import weakref;
import codecs
import numbers
import os
from RobotRaconteur.RobotRaconteurPython import DataTypes_ContainerTypes_generator

if (sys.version_info  > (3,0)):
    from builtins import property
    from functools import reduce
else:
    from __builtin__ import property

import numpy

if (sys.version_info > (3,5)):
    import asyncio

def SplitQualifiedName(name):
    pos=name.rfind('.')
    if (pos==-1): raise Exception("Name is not qualified")
    return (name[0:pos],name[pos+1:])

def FindStructureByName(l,name):
    for i in l:
        if (i.Name==name):
            return i
    raise RobotRaconteurPythonError.ServiceException("Structure " + name + " not found")

def FindMemberByName(l,name):
    for i in l:
        if (i.Name==name):
            return i
    raise RobotRaconteurPythonError.MemberNotFoundException("Member " + name + " not found")

def FindMessageElementByName(l,name):
    return RobotRaconteurPython.MessageElement.FindElement(l,name)

def PackMessageElement(data,type1,obj=None,node=None):

    if (hasattr(obj,'rrinnerstub')):
        obj=obj.rrinnerstub

    if (isinstance(type1,str)):
        if (len(type1.split())==1): type1=type1+ " value"
        type2=RobotRaconteurPython.TypeDefinition()
        type2.FromString(type1)
        type1=type2    

    return RobotRaconteurPython._PackMessageElement(data, type1, obj, node)

def UnpackMessageElement(element,type1=None,obj=None,node=None):
    
    if (hasattr(obj,'rrinnerstub')):
        obj=obj.rrinnerstub
    
    if (not type1 is None):
        if (isinstance(type1,str)):
            if (len(type1.split())==1): type1=type1+ " value"
            type2=RobotRaconteurPython.TypeDefinition()
            type2.FromString(type1)
            type1=type2
            
    return RobotRaconteurPython._UnpackMessageElement(element, type1, obj, node)


def PackToRRArray(array,type1,destrrarray=None):

    return RobotRaconteurPython._PackToRRArray(array,type1,destrrarray)

def UnpackFromRRArray(rrarray,type1=None):

    if (rrarray is None): return None
    
    return RobotRaconteurPython._UnpackFromRRArray(rrarray, type1)

def NewStructure(StructType,obj=None,node=None):
    if (hasattr(obj,'rrinnerstub')):
        obj=obj.rrinnerstub
    return RobotRaconteurPython._NewStructure(StructType, obj, node)

def GetPodDType(pod_type,obj=None,node=None):
    if (hasattr(obj,'rrinnerstub')):
        obj=obj.rrinnerstub
    return RobotRaconteurPython._GetNumPyDescrForType(pod_type, obj, node)

def GetNamedArrayDType(namedarray_type,obj=None,node=None):
    if (hasattr(obj,'rrinnerstub')):
        obj=obj.rrinnerstub
    return RobotRaconteurPython._GetNumPyDescrForType(namedarray_type, obj, node)

def GetPrimitiveDTypeFromNamedArrayDType(dt):
    prim_dt = None
    count = 0
    for dt_i in dt.fields.values():              
        if len(dt_i[0].shape) == 0:
            prim_dt_i = dt_i[0].type
            dt_i_2 = dt_i[0]
            count_i = 1
        else:            
            prim_dt_i = dt_i[0].subdtype[0].type
            dt_i_2 = dt_i[0].subdtype[0]
            count_i = dt_i[0].shape[0]
                        
        if (prim_dt_i == numpy.void):            
            prim_dt_i, n = GetPrimitiveDTypeFromNamedArrayDType(dt_i_2)
            count_i *= n
        
        if prim_dt is None:
            prim_dt = prim_dt_i
        else:            
            assert prim_dt == prim_dt_i
        
        count += count_i
        
    return prim_dt, count

def NamedArrayToArray(named_array):    
    prim_dt, _ = GetPrimitiveDTypeFromNamedArrayDType(named_array.dtype)
    a = numpy.ascontiguousarray(named_array.reshape(named_array.shape + (1,)))    
    return a.view(prim_dt) 

def ArrayToNamedArray(a, named_array_dt):
    prim_dt, elem_count = GetPrimitiveDTypeFromNamedArrayDType(named_array_dt)
    a = numpy.ascontiguousarray(a)
    assert a.dtype == prim_dt, "Array type must match named array element type"
    assert a.shape[-1] == elem_count, "Last dimension must match named array size"
    b = a.view(named_array_dt)
    if len(b.shape) > 1 and b.shape[-1] == 1:
        return b.reshape(a.shape[0:-1],order="C")
    return b

def InitStub(stub):
    odef=stub.RR_objecttype
    mdict={}

    for i in range(len(odef.Members)):
        m=odef.Members[i]
        if (isinstance(m,RobotRaconteurPython.PropertyDefinition)):
            def inner_prop(m1):
                fget=lambda  self : stub_getproperty(stub,m1.Name,m1)
                fset=lambda self, value : stub_setproperty(stub,m1.Name,m1,value)
                return property(fget,fset)
            p1=inner_prop(m)

            mdict[m.Name]=p1

            def inner_async_prop(m1):
                fget=lambda  self,handler,timeout=RobotRaconteurPython.RR_TIMEOUT_INFINITE : stub_async_getproperty(stub,m1.Name,m1,handler,timeout)
                fset=lambda self, value,handler,timeout=RobotRaconteurPython.RR_TIMEOUT_INFINITE : stub_async_setproperty(stub,m1.Name,m1,value,handler,timeout)
                return fget, fset
            p1_async=inner_async_prop(m)

            mdict['async_get_' + m.Name]=p1_async[0]
            mdict['async_set_' + m.Name]=p1_async[1]


        if (isinstance(m,RobotRaconteurPython.FunctionDefinition)):
            def inner_func(m1):
                if (not m1.IsGenerator()):                
                    if (m1.ReturnType.Type==RobotRaconteurPython.DataTypes_void_t):
                        f=lambda self,*args : stub_functioncallvoid(stub,m1.Name,m1,*args)
                    else:
                        f=lambda self,*args : stub_functioncall(stub,m1.Name,m1,*args)
                    return f
                else:
                    return lambda self,*args : stub_functioncallgenerator(stub,m1.Name,m1,*args)
            f1=inner_func(m)
            mdict[m.Name]=f1

            def inner_async_func(m1):              
                if not m1.IsGenerator():
                    if (m1.ReturnType.Type==RobotRaconteurPython.DataTypes_void_t):
                        f=lambda self,*args : stub_async_functioncallvoid(stub,m1.Name,m1,*args)
                    else:
                        f=lambda self,*args : stub_async_functioncall(stub,m1.Name,m1,*args)
                    return f
                else:
                    return lambda self,*args : stub_async_functioncallgenerator(stub,m1.Name,m1,*args)

            f1_async=inner_async_func(m)
            mdict['async_' + m.Name]=f1_async

        if (isinstance(m,RobotRaconteurPython.EventDefinition)):
            def new_evt_hook():
                evt=EventHook()          
                fget=lambda self : evt
                def fset(self,value):
                    if (value is not evt):
                        raise RuntimeError("Invalid operation")
                return property(fget, fset)            
            mdict[m.Name]=new_evt_hook()

        if (isinstance(m,RobotRaconteurPython.ObjRefDefinition)):
            def inner_objref(m1):
                if(m1.ArrayType != RobotRaconteurPython.DataTypes_ArrayTypes_none or m1.ContainerType != RobotRaconteurPython.DataTypes_ContainerTypes_none):
                    f=lambda self,index : stub_objref(stub,m1.Name,index)
                else:
                    f=lambda self : stub_objref(stub,m1.Name)
                return f
            f1=inner_objref(m)
            mdict['get_%s' % m.Name]=f1

            def inner_async_objref(m1):
                if(m1.ArrayType != RobotRaconteurPython.DataTypes_ArrayTypes_none or m1.ContainerType != RobotRaconteurPython.DataTypes_ContainerTypes_none):
                    f=lambda self,index,handler,timeout=RobotRaconteurPython.RR_TIMEOUT_INFINITE : stub_async_objref(stub,m1.Name,index,handler,timeout)
                else:
                    f=lambda self,handler,timeout=RobotRaconteurPython.RR_TIMEOUT_INFINITE : stub_async_objref(stub,m1.Name,None,handler,timeout)
                return f
            f1=inner_async_objref(m)
            mdict['async_get_%s' % m.Name]=f1

        if (isinstance(m,RobotRaconteurPython.PipeDefinition)):
            def inner_pipe(m1):
                innerp=stub.GetPipe(m1.Name)
                outerp=Pipe(innerp)
                fget=lambda self : outerp
                return property(fget)
            p2=inner_pipe(m)
            mdict[m.Name]=p2

        if (isinstance(m,RobotRaconteurPython.CallbackDefinition)):
            def new_cb_client():
                cb=CallbackClient()
                fget = lambda self: cb
                return property(fget)            
            mdict[m.Name]=new_cb_client()

        if (isinstance(m,RobotRaconteurPython.WireDefinition)):
            def inner_wire(m1):
                innerw=stub.GetWire(m1.Name)
                outerw=Wire(innerw)
                fget=lambda self : outerw
                return property(fget)
            w=inner_wire(m)
            mdict[m.Name]=w

        if (isinstance(m,RobotRaconteurPython.MemoryDefinition)):
            if (RobotRaconteurPython.IsTypeNumeric(m.Type.Type)):
                def inner_memory(m1):                
                    if (m.Type.ArrayType == RobotRaconteurPython.DataTypes_ArrayTypes_array):
                        outerm=ArrayMemoryClient(stub.GetArrayMemory(m1.Name))
                    else:
                        outerm=MultiDimArrayMemoryClient(stub.GetMultiDimArrayMemory(m1.Name))
                    fget=lambda self: outerm
                    return property(fget)
                mem=inner_memory(m)
                mdict[m.Name]=mem
            else:
                memory_rr_type=RobotRaconteurPython._GetNamedTypeEntryType(m.Type,stub,stub.RRGetNode())
                if (memory_rr_type == RobotRaconteurPython.DataTypes_pod_t):
                    def inner_memory(m1):                
                        if (m.Type.ArrayType == RobotRaconteurPython.DataTypes_ArrayTypes_array):
                            pass
                            outerm=PodArrayMemoryClient(stub.GetPodArrayMemory(m1.Name),m.Type,stub,stub.RRGetNode())
                        else:
                            outerm=PodMultiDimArrayMemoryClient(stub.GetPodMultiDimArrayMemory(m1.Name),m.Type,stub,stub.RRGetNode())
                            pass
                        fget=lambda self: outerm
                        return property(fget)
                    mem=inner_memory(m)
                    mdict[m.Name]=mem
                elif (memory_rr_type == RobotRaconteurPython.DataTypes_namedarray_t):
                    def inner_memory(m1):                
                        if (m.Type.ArrayType == RobotRaconteurPython.DataTypes_ArrayTypes_array):
                            pass
                            outerm=NamedArrayMemoryClient(stub.GetNamedArrayMemory(m1.Name),m.Type,stub,stub.RRGetNode())
                        else:
                            outerm=NamedMultiDimArrayMemoryClient(stub.GetNamedMultiDimArrayMemory(m1.Name),m.Type,stub,stub.RRGetNode())
                            pass
                        fget=lambda self: outerm
                        return property(fget)
                    mem=inner_memory(m)
                    mdict[m.Name]=mem
                else:
                    assert False


    outerstub_type=type(str(odef.Name),(ServiceStub,),mdict)
    outerstub=outerstub_type()

    for i in range(len(odef.Members)):
        m=odef.Members[i]

        if (isinstance(m,RobotRaconteurPython.PipeDefinition)):
            p=getattr(outerstub,m.Name)
            p._obj=outerstub

        if (isinstance(m,RobotRaconteurPython.WireDefinition)):
            w=getattr(outerstub,m.Name)
            w._obj=outerstub

    director=WrappedServiceStubDirectorPython(outerstub,stub)
    stub.SetRRDirector(director,0)
    director.__disown__()

    stub.SetPyStub(outerstub)
   
    outerstub.rrinnerstub=stub;
    outerstub.rrlock=threading.RLock()
    return outerstub


def stub_getproperty(stub,name,type1):
    return UnpackMessageElement(stub.PropertyGet(name),type1.Type,stub,stub.RRGetNode())

def stub_setproperty(stub,name,type1,value):
    pvalue=PackMessageElement(value,type1.Type,stub,stub.RRGetNode())
    stub.PropertySet(name,pvalue)

def stub_functioncall(stub,name,type1,*args):
    m=RobotRaconteurPython.vectorptr_messageelement()
    i=0
    for p in type1.Parameters:
        a=PackMessageElement(args[i],p,stub,stub.RRGetNode())
        m.append(a)
        i+=1
    ret=stub.FunctionCall(name,m)
    return UnpackMessageElement(ret,type1.ReturnType,stub,stub.RRGetNode())

def stub_functioncallvoid(stub,name,type1,*args):
    m=RobotRaconteurPython.vectorptr_messageelement()
    i=0
    for p in type1.Parameters:
        a=PackMessageElement(args[i],p,stub)
        m.append(a)
        i+=1
    stub.FunctionCall(name,m)

def stub_functioncallgenerator(stub,name,type1,*args):
    m=RobotRaconteurPython.vectorptr_messageelement()
    i=0
    param_type = None
    for p in type1.Parameters:
        if (p.ContainerType != RobotRaconteurPython.DataTypes_ContainerTypes_generator):
            a=PackMessageElement(args[i],p,stub)
            m.append(a)
            i+=1
        else:
            param_type = p
    return GeneratorClient(stub.GeneratorFunctionCall(name,m), type1.ReturnType, param_type, stub, stub.RRGetNode())

class AsyncRequestDirectorImpl(RobotRaconteurPython.AsyncRequestDirector):
    def __init__(self,handler,isvoid,Type,stub,node):
        super(AsyncRequestDirectorImpl,self).__init__()
        self._handler=handler
        self._isvoid=isvoid
        self._Type=Type
        self._node=node
        self._stub=stub

    def handler(self,m,error_code,errorname,errormessage):

        if (self._isvoid):
            if (error_code!=0):
                err=RobotRaconteurPythonError.RobotRaconteurExceptionUtil.ErrorCodeToException(error_code,errorname,errormessage)
                self._handler(err)
                return
            else:
                self._handler(None)
        else:
            if (error_code!=0):
                err=RobotRaconteurPythonError.RobotRaconteurExceptionUtil.ErrorCodeToException(error_code,errorname,errormessage)
                self._handler(None,err)
                return
            else: 
                try:
                    a=UnpackMessageElement(m,self._Type,self._stub,self._node)
                except Exception as err:
                    self._handler(None,err)
                    return
                self._handler(a,None)
                return


def stub_async_getproperty(stub,name,type1,handler,timeout=-1):
    return async_call(stub.async_PropertyGet,(name,adjust_timeout(timeout)),AsyncRequestDirectorImpl,handler,directorargs=(False,type1.Type,stub,stub.RRGetNode()))

def stub_async_setproperty(stub,name,type1,value,handler,timeout=-1):
    pvalue=PackMessageElement(value,type1.Type,stub)
    return async_call(stub.async_PropertySet,(name,pvalue,adjust_timeout(timeout)),AsyncRequestDirectorImpl,handler,directorargs=(True,type1.Type,stub,stub.RRGetNode()))


def stub_async_functioncall(stub,name,type1,*args):
    m=RobotRaconteurPython.vectorptr_messageelement()
    i=0
    for p in type1.Parameters:
        a=PackMessageElement(args[i],p,stub)
        m.append(a)
        i+=1
    handler=args[i]
    if (len(args) > i+1):
        timeout=args[i+1]
    else:
        timeout=RobotRaconteurPython.RR_TIMEOUT_INFINITE
    return async_call(stub.async_FunctionCall,(name,m,adjust_timeout(timeout)),AsyncRequestDirectorImpl,handler,directorargs=(False,type1.ReturnType,stub,stub.RRGetNode()))

def stub_async_functioncallvoid(stub,name,type1,*args):
    m=RobotRaconteurPython.vectorptr_messageelement()
    i=0
    for p in type1.Parameters:
        a=PackMessageElement(args[i],p,stub)
        m.append(a)
        i+=1
    handler=args[i]
    if (len(args) > i+1):
        timeout=args[i+1]
    else:
        timeout=-1

    return async_call(stub.async_FunctionCall,(name,m,adjust_timeout(timeout)),AsyncRequestDirectorImpl,handler,directorargs=(True,type1.ReturnType,stub,stub.RRGetNode()))

def stub_async_functioncallgenerator(stub,name,type1,*args):
    m=RobotRaconteurPython.vectorptr_messageelement()
    i=0
    param_type = None
    for p in type1.Parameters:
        if (p.ContainerType != RobotRaconteurPython.DataTypes_ContainerTypes_generator):
            a=PackMessageElement(args[i],p,stub)
            m.append(a)
            i+=1
        else:
            param_type = p
    handler=args[i]
    if (len(args) > i+1):
        timeout=args[i+1]
    else:
        timeout=RobotRaconteurPython.RR_TIMEOUT_INFINITE
    return async_call(stub.async_GeneratorFunctionCall,(name,m,adjust_timeout(timeout)),AsyncGeneratorClientReturnDirectorImpl,handler,directorargs=(type1.ReturnType,param_type,stub,stub.RRGetNode()))


_stub_objref_lock=threading.Lock()

def stub_objref(stub,name,index=None):
    if (index is None):
        s=stub.FindObjRef(name)
        with _stub_objref_lock:
            s2=s.GetPyStub()
            if (s2 is not None): return s2
            return InitStub(s)

        return
    else:
        s=stub.FindObjRef(name,str(index))
        with _stub_objref_lock:
            s2=s.GetPyStub()
            if (s2 is not None): return s2
            return InitStub(s)

def stub_async_objref(stub,name,index,handler,timeout=-1):
    if (index is None):
        return async_call(stub.async_FindObjRef,(name,adjust_timeout(timeout)),AsyncStubReturnDirectorImpl,handler)
    else:
        return async_call(stub.async_FindObjRef,(name,str(index),adjust_timeout(timeout)),AsyncStubReturnDirectorImpl,handler)

class WrappedServiceStubDirectorPython(RobotRaconteurPython.WrappedServiceStubDirector):
    def __init__(self,stub,innerstub):
        self.stub=stub;
        self.innerstub=innerstub
        super(WrappedServiceStubDirectorPython, self).__init__()

    def DispatchEvent(self, name, args1):
        try:
            type1=FindMemberByName(self.innerstub.RR_objecttype.Members,name)
            #type1=[e for e in self.innerstub.RR_objecttype.Members if e.Name == name][0]
            args=[]        
            type2=RobotRaconteurPython.MemberDefinitionUtil.ToEvent(type1)
            for p in type2.Parameters:
                m=FindMessageElementByName(args1,p.Name)            
                a=UnpackMessageElement(m,p,self.stub,node=self.innerstub.RRGetNode())
                args.append(a)
            getattr(self.stub,name).fire(*args)
        except:
            traceback.print_exc()



    def CallbackCall(self, name, args1):
        try:
            type1=FindMemberByName(self.innerstub.RR_objecttype.Members,name)
            #type1=[e for e in self.innerstub.RR_objecttype.Members if e.Name == name][0]
            args=[]        
    
            type2=RobotRaconteurPython.MemberDefinitionUtil.ToCallback(type1)
            for p in type2.Parameters:
                m=FindMessageElementByName(args1,p.Name)
                
                a=UnpackMessageElement(m,p,self.stub,self.innerstub.RRGetNode())
                args.append(a)
            ret=getattr(self.stub,name).Function(*args)
    
            if (ret is None):
                m=RobotRaconteurPython.MessageElement()
                m.ElementName="return"
                m.ElementType=RobotRaconteurPython.DataTypes_void_t
                return m
            return PackMessageElement(ret,type2.ReturnType,self.stub,self.innerstub.RRGetNode())
        except:
            traceback.print_exc()

class ServiceStub(object):
    pass

class CallbackClient(object):
    def __init__(self):
        self.Function=None

class PipeEndpoint(object):
    def __init__(self,innerpipe, type, obj=None):
        self.__innerpipe=innerpipe
        self.__type=type
        self.PipeEndpointClosedCallback=None
        self._PacketReceivedEvent=EventHook()
        self._PacketAckReceivedEvent=EventHook()
        self.__obj=obj

    @property
    def Index(self):
        return self.__innerpipe.GetIndex()

    @property
    def Endpoint(self):
        return self.__innerpipe.GetEndpoint()

    @property
    def Available(self):
        return self.__innerpipe.Available()

    @property
    def IsUnreliable(self):
        return self.__innerpipe.IsUnreliable()

    @property
    def Direction(self):
        return self.__innerpipe.Direction()

    @property
    def RequestPacketAck(self):
        return self.__innerpipe.GetRequestPacketAck()

    @RequestPacketAck.setter
    def RequestPacketAck(self,value):
        self.__innerpipe.SetRequestPacketAck(value)

    @property
    def IgnoreReceived(self):
        return self.__innerpipe.GetIgnoreReceived()

    @IgnoreReceived.setter
    def IgnoreReceived(self,value):
        self.__innerpipe.SetIgnoreReceived(value)

    def Close(self):
        return self.__innerpipe.Close()

    def AsyncClose(self,handler,timeout=2):
        return async_call(self.__innerpipe.AsyncClose,(adjust_timeout(timeout),),AsyncVoidReturnDirectorImpl,handler)

    def SendPacket(self,packet):
        m=PackMessageElement(packet,self.__type,self.__obj,self.__innerpipe.GetNode())
        return self.__innerpipe.SendPacket(m)

    def AsyncSendPacket(self, packet, handler):
        m=PackMessageElement(packet,self.__type,self.__obj,self.__innerpipe.GetNode())
        return async_call(self.__innerpipe.AsyncSendPacket,(m,),AsyncUInt32ReturnDirectorImpl,handler)

    def ReceivePacket(self):
        m=self.__innerpipe.ReceivePacket()
        return UnpackMessageElement(m,self.__type,self.__obj,self.__innerpipe.GetNode())

    def PeekNextPacket(self):
        m=self.__innerpipe.PeekNextPacket()
        return UnpackMessageElement(m,self.__type,self.__obj,self.__innerpipe.GetNode())

    def ReceivePacketWait(self, timeout=RobotRaconteurPython.RR_TIMEOUT_INFINITE):
        m=self.__innerpipe.ReceivePacketWait(timeout)
        return UnpackMessageElement(m,self.__type,self.__obj,self.__innerpipe.GetNode())

    def PeekNextPacketWait(self, timeout=RobotRaconteurPython.RR_TIMEOUT_INFINITE):
        m=self.__innerpipe.PeekNextPacketWait(timeout)
        return UnpackMessageElement(m,self.__type,self.__obj,self.__innerpipe.GetNode())

    def TryReceivePacketWait(self, timeout=RobotRaconteurPython.RR_TIMEOUT_INFINITE, peek=False):
        m=RobotRaconteurPython.MessageElement()
        r=self.__innerpipe.TryReceivePacketWait(m, timeout, peek)
        return (r, UnpackMessageElement(m,self.__type,self.__obj,self.__innerpipe.GetNode()))
    
    @property
    def PacketReceivedEvent(self):
        return self._PacketReceivedEvent
    
    @PacketReceivedEvent.setter
    def PacketReceivedEvent(self, evt):
        if (evt is not self._PacketReceivedEvent):
            raise RuntimeError("Invalid operation")
    
    @property
    def PacketAckReceivedEvent(self):
        return self._PacketAckReceivedEvent
    
    @PacketAckReceivedEvent.setter 
    def PacketAckReceivedEvent(self, evt):
        if (evt is not self._PacketAckReceivedEvent):
            raise RuntimeError("Invalid operation")

class PipeEndpointDirector(RobotRaconteurPython.WrappedPipeEndpointDirector):
    def __init__(self,endpoint):
        self.__endpoint=endpoint
        super(PipeEndpointDirector, self).__init__()

    def PipeEndpointClosedCallback(self):

        if (not self.__endpoint.PipeEndpointClosedCallback is None):
            self.__endpoint.PipeEndpointClosedCallback(self.__endpoint)


    def PacketReceivedEvent(self):

        self.__endpoint.PacketReceivedEvent.fire(self.__endpoint)


    def PacketAckReceivedEvent(self,packetnum):

        self.__endpoint.PacketAckReceivedEvent.fire(self.__endpoint,packetnum)


class PipeAsyncConnectHandlerImpl(RobotRaconteurPython.AsyncPipeEndpointReturnDirector):
    def __init__(self,handler,innerpipe,obj):
        super(PipeAsyncConnectHandlerImpl,self).__init__()
        self._handler=handler
        self.__innerpipe=innerpipe
        self.__obj=obj

    def handler(self, innerendpoint, error_code,errorname,errormessage):
        if (error_code!=0):
            err=RobotRaconteurPythonError.RobotRaconteurExceptionUtil.ErrorCodeToException(error_code,errorname,errormessage)
            self._handler(None,err)
            return
        try:            
            outerendpoint=PipeEndpoint(innerendpoint,self.__innerpipe.Type,self.__obj)
            director=PipeEndpointDirector(outerendpoint)
            innerendpoint.SetRRDirector(director,0)
            director.__disown__()
        except Exception as err2:
            self._handler(None, err2)
            return
        self._handler(outerendpoint, None)

class Pipe(object):
    def __init__(self,innerpipe,obj=None):
        self._innerpipe=innerpipe        
        self._obj=obj

    def Connect(self,index):
        innerendpoint=self._innerpipe.Connect(index)
        outerendpoint=PipeEndpoint(innerendpoint,self._innerpipe.Type,self._obj)
        director=PipeEndpointDirector(outerendpoint)
        innerendpoint.SetRRDirector(director,0)
        director.__disown__()
        return outerendpoint

    def AsyncConnect(self,*args):
        if (isinstance(args[0], numbers.Number)):
            index=args[0]
            handler=args[1]
            if (len(args)>=3):
                timeout=args[2]
            else:
                timeout=RobotRaconteurPython.RR_TIMEOUT_INFINITE
        else:
            index=-1
            handler=args[0]
            if (len(args)>=2):
                timeout=args[1]
            else:
                timeout=RobotRaconteurPython.RR_TIMEOUT_INFINITE

        return async_call(self._innerpipe.AsyncConnect,(index,adjust_timeout(timeout)),PipeAsyncConnectHandlerImpl,handler,directorargs=(self._innerpipe,self._obj))

    @property
    def MemberName(self):
        return self._innerpipe.GetMemberName()        
    
    @property
    def Direction(self):
        return self._innerpipe.Direction()
    
    @property
    def PipeConnectCallback(self):
        raise Exception("Read only property")

    @PipeConnectCallback.setter
    def PipeConnectCallback(self, c):
        wrappedp=WrappedPipeServerConnectDirectorPython(self,self._innerpipe.Type, c)
        self._innerpipe.SetWrappedPipeConnectCallback(wrappedp,0)
        wrappedp.__disown__()

class WrappedPipeServerConnectDirectorPython(RobotRaconteurPython.WrappedPipeServerConnectDirector):
    def __init__(self,pipe,type,callback):
        self.pipe=pipe
        self.type=type
        self.callback=callback
        super(WrappedPipeServerConnectDirectorPython,self).__init__()

    def PipeConnectCallback(self,innerendpoint):
                
        outerendpoint=PipeEndpoint(innerendpoint,self.type)
        director=PipeEndpointDirector(outerendpoint)
        innerendpoint.SetRRDirector(director,0)
        director.__disown__()
        self.callback(outerendpoint)

class WireConnection(object):
    def __init__(self,innerwire, type, obj=None):
        self.__innerwire=innerwire
        self.__type=type
        self.WireConnectionClosedCallback=None
        self._WireValueChanged=EventHook()
        self.__obj=obj

    @property
    def Endpoint(self):
        return self.__innerwire.GetEndpoint()

    @property
    def Direction(self):
        return self.__innerwire.Direction()

    def Close(self):
        return self.__innerwire.Close()

    def AsyncClose(self,handler,timeout=2):
        return async_call(self.__innerwire.AsyncClose,(adjust_timeout(timeout),),AsyncVoidReturnDirectorImpl,handler)

    @property
    def InValue(self):
        m=self.__innerwire.GetInValue()
        return UnpackMessageElement(m,self.__type,self.__obj,self.__innerwire.GetNode())

    @property
    def OutValue(self):
        m=self.__innerwire.GetOutValue()
        return UnpackMessageElement(m,self.__type,self.__obj,self.__innerwire.GetNode())

    @OutValue.setter
    def OutValue(self,value):
        m=PackMessageElement(value,self.__type,self.__obj,self.__innerwire.GetNode())
        return self.__innerwire.SetOutValue(m)

    @property
    def LastValueReceivedTime(self):
        return self.__innerwire.GetLastValueReceivedTime()

    @property
    def LastValueSentTime(self):
        return self.__innerwire.GetLastValueSentTime()

    @property
    def InValueValid(self):
        return self.__innerwire.GetInValueValid()

    @property
    def OutValueValid(self):
        return self.__innerwire.GetOutValueValid()
        
    @property
    def IgnoreInValue(self):
        return self.__innerwire.GetIgnoreInValue()

    def TryGetInValue(self):
        ts=RobotRaconteurPython.TimeSpec()
        m=RobotRaconteurPython.MessageElement()
        res=self.__innerwire.TryGetInValue(m,ts)
        if not res:
            return (False,None, None)
        return (True, UnpackMessageElement(m,self.__type,self.__obj,self.__innerwire.GetNode()), ts)

    def TryGetOutValue(self):
        ts=RobotRaconteurPython.TimeSpec()
        m=RobotRaconteurPython.MessageElement()
        res=self.__innerwire.TryGetOutValue(m,ts)
        if not res:
            return (False,None, None)
        return (True, UnpackMessageElement(m,self.__type,self.__obj,self.__innerwire.GetNode()), ts)

    @IgnoreInValue.setter
    def IgnoreInValue(self,value):
        self.__innerwire.SetIgnoreInValue(value)

    def WaitInValueValid(self,timeout=RobotRaconteurPython.RR_TIMEOUT_INFINITE):
        return self.__innerwire.WaitInValueValid(timeout)

    def WaitOutValueValid(self,timeout=RobotRaconteurPython.RR_TIMEOUT_INFINITE):
        return self.__innerwire.WaitOutValueValid(timeout)
    
    @property
    def WireValueChanged(self):
        return self._WireValueChanged
    
    @WireValueChanged.setter
    def WireValueChanged(self,evt):
        if (evt is not self._WireValueChanged):
            raise RuntimeError("Invalid operation")

class WireConnectionDirector(RobotRaconteurPython.WrappedWireConnectionDirector):

    def __init__(self,endpoint,type,obj=None,innerep=None):
        self.__endpoint=endpoint
        self.__type=type
        self.__obj=obj
        self.__innerep=innerep
        super(WireConnectionDirector, self).__init__()

    def WireValueChanged(self,value,time):

        value2=UnpackMessageElement(value,self.__type,self.__obj,self.__innerep.GetNode())
        self.__endpoint.WireValueChanged.fire(self.__endpoint,value2,time)


    def WireConnectionClosedCallback(self):

        if (not self.__endpoint.WireConnectionClosedCallback is None):
            self.__endpoint.WireConnectionClosedCallback(self.__endpoint)


class WireAsyncConnectHandlerImpl(RobotRaconteurPython.AsyncWireConnectionReturnDirector):
    def __init__(self,handler,innerpipe,obj):
        super(WireAsyncConnectHandlerImpl,self).__init__()
        self._handler=handler
        self.__innerpipe=innerpipe
        self.__obj=obj

    def handler(self, innerendpoint, error_code,errorname,errormessage):
        if (error_code!=0):
            err=RobotRaconteurPythonError.RobotRaconteurExceptionUtil.ErrorCodeToException(error_code,errorname,errormessage)
            self._handler(None,err)
            return
        try:            
            outerendpoint=WireConnection(innerendpoint,self.__innerpipe.Type,self.__obj)
            director=WireConnectionDirector(outerendpoint,self.__innerpipe.Type,self.__obj,innerendpoint)
            innerendpoint.SetRRDirector(director,0)
            director.__disown__()
        except Exception as err2:
            self._handler(None, err2)
            return
        self._handler(outerendpoint, None)

class WireAsyncPeekReturnDirectorImpl(RobotRaconteurPython.AsyncWirePeekReturnDirector):
    def __init__(self,handler,innerpipe,obj):
        super(WireAsyncPeekReturnDirectorImpl,self).__init__()
        self._handler=handler
        self.__innerpipe=innerpipe
        self.__obj=obj

    def handler(self,m,ts,error_code,errorname,errormessage):
        if (error_code!=0):
            err=RobotRaconteurPythonError.RobotRaconteurExceptionUtil.ErrorCodeToException(error_code,errorname,errormessage)
            self._handler(None, None, err)
            return
        value=UnpackMessageElement(m,self.__innerpipe.Type,self.__obj,self.__innerpipe.GetNode())
        self._handler(value, ts, None)

class WrappedWireServerPeekValueDirectorImpl(RobotRaconteurPython.WrappedWireServerPeekValueDirector):
    def __init__(self, cb,innerpipe,obj):
        super(WrappedWireServerPeekValueDirectorImpl,self).__init__()
        self._cb=cb
        self.__innerpipe=innerpipe
        self.__obj=obj

    def PeekValue(self, ep):
        value = self._cb(ep)
        m=PackMessageElement(value,self.__innerpipe.Type,self.__obj,self.__innerpipe.GetNode())
        return m

class WrappedWireServerPokeValueDirectorImpl(RobotRaconteurPython.WrappedWireServerPokeValueDirector):
    def __init__(self, cb,innerpipe,obj):
        super(WrappedWireServerPokeValueDirectorImpl,self).__init__()
        self._cb=cb
        self.__innerpipe=innerpipe
        self.__obj=obj

    def PokeValue(self, m, ts, ep):
        value=UnpackMessageElement(m,self.__innerpipe.Type,self.__obj,self.__innerpipe.GetNode())
        self._cb(value, ts, ep)
        
class Wire(object):
    def __init__(self,innerpipe,obj=None):
        self._innerpipe=innerpipe        
        self._obj=obj

    def Connect(self):
        innerendpoint=self._innerpipe.Connect()
        outerendpoint=WireConnection(innerendpoint,self._innerpipe.Type,self._obj)
        director=WireConnectionDirector(outerendpoint,self._innerpipe.Type,self._obj,innerendpoint)
        innerendpoint.SetRRDirector(director,0)
        director.__disown__()
        return outerendpoint

    def AsyncConnect(self,handler,timeout=RobotRaconteurPython.RR_TIMEOUT_INFINITE):

        return async_call(self._innerpipe.AsyncConnect,(adjust_timeout(timeout),),WireAsyncConnectHandlerImpl,handler,directorargs=(self._innerpipe,self._obj))

    @property
    def MemberName(self):
        return self._innerpipe.GetMemberName()

    @property
    def Direction(self):
        return self._innerpipe.Direction()

    @property
    def WireConnectCallback(self):
        raise Exception("Read only property")

    @WireConnectCallback.setter
    def WireConnectCallback(self, c):
        wrappedp=WrappedWireServerConnectDirectorPython(self,self._innerpipe.Type, c)
        self._innerpipe.SetWrappedWireConnectCallback(wrappedp,0)
        wrappedp.__disown__()

    def PeekInValue(self):
        ts=RobotRaconteurPython.TimeSpec()
        m=self._innerpipe.PeekInValue(ts)
        return (UnpackMessageElement(m,self._innerpipe.Type,self._obj,self._innerpipe.GetNode()),ts)

    def PeekOutValue(self):
        ts=RobotRaconteurPython.TimeSpec()
        m=self._innerpipe.PeekOutValue(ts)
        return (UnpackMessageElement(m,self._innerpipe.Type,self._obj,self._innerpipe.GetNode()),ts)

    def PokeOutValue(self, value):
         m=PackMessageElement(value,self._innerpipe.Type,self._obj,self._innerpipe.GetNode())
         self._innerpipe.PokeOutValue(m)

    def AsyncPeekInValue(self, handler, timeout=RobotRaconteurPython.RR_TIMEOUT_INFINITE):
        return async_call(self._innerpipe.AsyncPeekInValue, (adjust_timeout(timeout),), WireAsyncPeekReturnDirectorImpl, handler, directorargs=(self._innerpipe,self._obj))

    def AsyncPeekOutValue(self, handler, timeout=RobotRaconteurPython.RR_TIMEOUT_INFINITE):
        return async_call(self._innerpipe.AsyncPeekOutValue, (adjust_timeout(timeout),), WireAsyncPeekReturnDirectorImpl, handler, directorargs=(self._innerpipe,self._obj))

    def AsyncPokeOutValue(self, value, handler, timeout=RobotRaconteurPython.RR_TIMEOUT_INFINITE):
         m=PackMessageElement(value,self._innerpipe.Type,self._obj,self._innerpipe.GetNode())
         return async_call(self._innerpipe.AsyncPokeOutValue, (m,adjust_timeout(timeout)), AsyncVoidReturnDirectorImpl, handler)

    @property
    def PeekInValueCallback(self):
        raise Exception("Read only property")

    @PeekInValueCallback.setter
    def PeekInValueCallback(self, c):
        cb=WrappedWireServerPeekValueDirectorImpl(c,self._innerpipe,self._obj)
        self._innerpipe.SetPeekInValueCallback(cb,0)
        cb.__disown__()

    @property
    def PeekOutValueCallback(self):
        raise Exception("Read only property")

    @PeekOutValueCallback.setter
    def PeekOutValueCallback(self, c):
        cb=WrappedWireServerPeekValueDirectorImpl(c,self._innerpipe,self._obj)
        self._innerpipe.SetPeekOutValueCallback(cb,0)
        cb.__disown__()

    @property
    def PokeOutValueCallback(self):
        raise Exception("Read only property")

    @PokeOutValueCallback.setter
    def PokeOutValueCallback(self, c):
        cb=WrappedWireServerPokeValueDirectorImpl(c,self._innerpipe,self._obj)
        self._innerpipe.SetPokeOutValueCallback(cb,0)
        cb.__disown__()

class WrappedWireServerConnectDirectorPython(RobotRaconteurPython.WrappedWireServerConnectDirector):
    def __init__(self,wire,type,callback):
        self.wire=wire
        self.type=type
        self.callback=callback
        super(WrappedWireServerConnectDirectorPython,self).__init__()

    def WireConnectCallback(self,innerendpoint):
                
        outerendpoint=WireConnection(innerendpoint,self.type)
        director=WireConnectionDirector(outerendpoint,self.type,innerep=innerendpoint)
        innerendpoint.SetRRDirector(director,0)
        director.__disown__()
        self.callback(outerendpoint)



class ArrayMemoryClient(object):
    def __init__(self,innermemory):
        self.__innermemory=innermemory

    @property
    def Length(self):
        return self.__innermemory.Length()

    def Read(self, memorypos, buffer, bufferpos, count):
        dat=RobotRaconteurPython.WrappedArrayMemoryClientUtil.Read(self.__innermemory, memorypos, count)
        t=RobotRaconteurPython.TypeDefinition()
        t.Type=self.__innermemory.ElementTypeID()
        t.ArrayType=RobotRaconteurPython.DataTypes_ArrayTypes_array
        t.ArrayVarLength=True
        t.ArrayLength=RobotRaconteurPython.vectorint32([0])
        buffer[bufferpos:(bufferpos+count)]=UnpackFromRRArray(dat,t)

    def Write(self,memorypos, buffer, bufferpos, count):
        t=RobotRaconteurPython.TypeDefinition()
        t.Type=self.__innermemory.ElementTypeID()
        t.ArrayType=RobotRaconteurPython.DataTypes_ArrayTypes_array
        t.ArrayVarLength=True
        t.ArrayLength=RobotRaconteurPython.vectorint32([0])
        dat=PackToRRArray(buffer[bufferpos:(bufferpos+count)],t)
        RobotRaconteurPython.WrappedArrayMemoryClientUtil.Write(self.__innermemory, memorypos,dat,0,count)

class MultiDimArrayMemoryClient(object):
    def __init__(self,innermemory):
        self.__innermemory=innermemory
        import RobotRaconteur        

    @property
    def DimCount(self):
        return self.__innermemory.DimCount()

    @property
    def Dimensions(self):
        return list(self.__innermemory.Dimensions())
   
    def Read(self, memorypos, buffer, bufferpos, count):

        dat=RobotRaconteurPython.WrappedMultiDimArrayMemoryClientUtil.Read(self.__innermemory,memorypos,count)
        tdims=RobotRaconteurPython.TypeDefinition()
        tdims.Type=RobotRaconteurPython.DataTypes_uint32_t
        tdims.ArrayType=RobotRaconteurPython.DataTypes_ArrayTypes_array
        tdims.ArrayVarLength=True
        tdims.ArrayLength=RobotRaconteurPython.vectorint32([0])
        dims=UnpackFromRRArray(dat.Dims,tdims)

        t=RobotRaconteurPython.TypeDefinition()
        t.Type=self.__innermemory.ElementTypeID()
        t.ArrayType=RobotRaconteurPython.DataTypes_ArrayTypes_array
        t.ArrayVarLength=True
        t.ArrayLength=RobotRaconteurPython.vectorint32([0])
        array=UnpackFromRRArray(dat.Array,t)        
        
        memind=[(slice(memorypos[i],(memorypos[i]+count[i]))) for i in range(len(count))]
        bufind=[(slice(bufferpos[i], (bufferpos[i]+count[i]))) for i in range(len(count))]
                
        buffer2=array.reshape(dims,order="F")
        buffer[tuple(bufind)]=buffer2


    def Write(self, memorypos, buffer, bufferpos, count):

        tdims=RobotRaconteurPython.TypeDefinition()
        tdims.Type=RobotRaconteurPython.DataTypes_uint32_t
        tdims.ArrayType=RobotRaconteurPython.DataTypes_ArrayTypes_array
        tdims.ArrayVarLength=True
        tdims.ArrayLength=RobotRaconteurPython.vectorint32([0])

        t=RobotRaconteurPython.TypeDefinition()
        t.Type=self.__innermemory.ElementTypeID()
        t.ArrayType=RobotRaconteurPython.DataTypes_ArrayTypes_array
        t.ArrayVarLength=True
        t.ArrayLength=RobotRaconteurPython.vectorint32([0])
        elementcount=reduce(operator.mul,count,1)
        dims=count
        
        memind=[slice(memorypos[i],(memorypos[i]+count[i])) for i in range(len(count))]
        bufind=[slice(bufferpos[i],(bufferpos[i]+count[i])) for i in range(len(count))]
                    
        array=buffer[tuple(bufind)].flatten(order="F")
        
        dims2=PackToRRArray(count,tdims)
        array2=PackToRRArray(array,t)
        
        writedat2=RobotRaconteurPython.RRMultiDimArrayUntyped()
        writedat2.Dims=dims2
        
        writedat2.Array=array2
        
        RobotRaconteurPython.WrappedMultiDimArrayMemoryClientUtil.Write(self.__innermemory,memorypos,writedat2,[0]*len(count),count)

class PodArrayMemoryClient_bufferdirector(RobotRaconteurPython.WrappedPodArrayMemoryClientBuffer):
    def __init__(self, buf, type1, obj, node):
        super(PodArrayMemoryClient_bufferdirector,self).__init__()
        self._buffer=buf
        self._obj=obj
        self._node=node
        self._type=type1
        
    def UnpackReadResult(self, res, bufferpos, count):
        res1=RobotRaconteurPython.MessageElementDataUtil.ToMessageElementPodArray(res)
        m=RobotRaconteurPython.MessageElement()
        m.SetData(res)
        m.ElementTypeName=res1.Type
        m.DataCount=len(res1.Elements)
        
        res1=UnpackMessageElement(m, self._type, self._obj, self._node)
        self._buffer[bufferpos:(bufferpos+count)] = res1
        
    def PackWriteRequest(self, bufferpos, count):
        buf1=self._buffer[bufferpos:(bufferpos+count)]
        m_data = PackMessageElement(buf1,self._type, self._obj, self._node).GetData()
        return RobotRaconteurPython.MessageElementDataUtil.ToMessageElementPodArray(m_data)

class PodArrayMemoryClient(object):
        
    def __init__(self,innermemory,type1,obj,node):
        self.__innermemory=innermemory
        self._type=type1
        self._obj=obj
        self._node=node

    @property
    def Length(self):
        return self.__innermemory.Length()

    def Read(self, memorypos, buf, bufferpos, count):
        b=PodArrayMemoryClient_bufferdirector(buf, self._type, self._obj, self._node)
        self.__innermemory.Read(memorypos, b, bufferpos, count)
        
    def Write(self,memorypos, buf, bufferpos, count):
        b=PodArrayMemoryClient_bufferdirector(buf, self._type, self._obj, self._node)
        self.__innermemory.Write(memorypos, b, bufferpos, count)

class PodMultiDimArrayMemoryClient_bufferdirector(RobotRaconteurPython.WrappedPodMultiDimArrayMemoryClientBuffer):
    def __init__(self, buf, type1, obj, node):
        super(PodMultiDimArrayMemoryClient_bufferdirector,self).__init__()
        self._buffer=buf
        self._obj=obj
        self._node=node
        self._type=type1
        
    def UnpackReadResult(self, res, bufferpos, count):
        res1=RobotRaconteurPython.MessageElementDataUtil.ToMessageElementPodMultiDimArray(res)
        m=RobotRaconteurPython.MessageElement()
        m.SetData(res)
        m.ElementTypeName=res1.Type
        m.DataCount=len(res1.Elements)
        
        res2=UnpackMessageElement(m, self._type, self._obj, self._node)
        bufind=[(slice(bufferpos[i], (bufferpos[i]+count[i]))) for i in range(len(count))]
        self._buffer[tuple(bufind)] = res2
        
    def PackWriteRequest(self, bufferpos, count):
        bufind=[(slice(bufferpos[i], (bufferpos[i]+count[i]))) for i in range(len(count))]
        buf1=self._buffer[tuple(bufind)]          
        m_data = PackMessageElement(buf1,self._type, self._obj, self._node).GetData()
        return RobotRaconteurPython.MessageElementDataUtil.ToMessageElementPodMultiDimArray(m_data)

class PodMultiDimArrayMemoryClient(object):
        
    def __init__(self,innermemory,type1,obj,node):
        self.__innermemory=innermemory
        self._type=type1
        self._obj=obj
        self._node=node

    @property
    def DimCount(self):
        return self.__innermemory.DimCount()
    
    @property
    def Dimensions(self):
        return list(self.__innermemory.Dimensions())

    def Read(self, memorypos, buf, bufferpos, count):
        b=PodMultiDimArrayMemoryClient_bufferdirector(buf, self._type, self._obj, self._node)
        self.__innermemory.Read(memorypos, b, bufferpos, count)
        
    def Write(self,memorypos, buf, bufferpos, count):
        b=PodMultiDimArrayMemoryClient_bufferdirector(buf, self._type, self._obj, self._node)
        self.__innermemory.Write(memorypos, b, bufferpos, count)

class NamedArrayMemoryClient_bufferdirector(RobotRaconteurPython.WrappedNamedArrayMemoryClientBuffer):
    def __init__(self, buf, type1, obj, node):
        super(NamedArrayMemoryClient_bufferdirector,self).__init__()
        self._buffer=buf
        self._obj=obj
        self._node=node
        self._type=type1
        
    def UnpackReadResult(self, res, bufferpos, count):
        res1=RobotRaconteurPython.MessageElementDataUtil.ToMessageElementNamedArray(res)
        m=RobotRaconteurPython.MessageElement()
        m.SetData(res)
        m.ElementTypeName=res1.Type
        m.DataCount=len(res1.Elements)
        
        res1=UnpackMessageElement(m, self._type, self._obj, self._node)
        self._buffer[bufferpos:(bufferpos+count)] = res1
        
    def PackWriteRequest(self, bufferpos, count):
        buf1=self._buffer[bufferpos:(bufferpos+count)]
        m_data = PackMessageElement(buf1,self._type, self._obj, self._node).GetData()
        return RobotRaconteurPython.MessageElementDataUtil.ToMessageElementNamedArray(m_data)

class NamedArrayMemoryClient(object):
        
    def __init__(self,innermemory,type1,obj,node):
        self.__innermemory=innermemory
        self._type=type1
        self._obj=obj
        self._node=node

    @property
    def Length(self):
        return self.__innermemory.Length()

    def Read(self, memorypos, buf, bufferpos, count):
        b=NamedArrayMemoryClient_bufferdirector(buf, self._type, self._obj, self._node)
        self.__innermemory.Read(memorypos, b, bufferpos, count)
        
    def Write(self,memorypos, buf, bufferpos, count):
        b=NamedArrayMemoryClient_bufferdirector(buf, self._type, self._obj, self._node)
        self.__innermemory.Write(memorypos, b, bufferpos, count)

class NamedMultiDimArrayMemoryClient_bufferdirector(RobotRaconteurPython.WrappedNamedMultiDimArrayMemoryClientBuffer):
    def __init__(self, buf, type1, obj, node):
        super(NamedMultiDimArrayMemoryClient_bufferdirector,self).__init__()
        self._buffer=buf
        self._obj=obj
        self._node=node
        self._type=type1
        
    def UnpackReadResult(self, res, bufferpos, count):
        res1=RobotRaconteurPython.MessageElementDataUtil.ToMessageElementNamedMultiDimArray(res)
        m=RobotRaconteurPython.MessageElement()
        m.SetData(res)
        m.ElementTypeName=res1.Type
        m.DataCount=len(res1.Elements)
        
        res2=UnpackMessageElement(m, self._type, self._obj, self._node)
        bufind=[(slice(bufferpos[i], (bufferpos[i]+count[i]))) for i in range(len(count))]
        self._buffer[tuple(bufind)] = res2        
        
    def PackWriteRequest(self, bufferpos, count):
        bufind=[(slice(bufferpos[i], (bufferpos[i]+count[i]))) for i in range(len(count))]
        buf1=self._buffer[tuple(bufind)]            
        m_data = PackMessageElement(buf1,self._type, self._obj, self._node).GetData()
        return RobotRaconteurPython.MessageElementDataUtil.ToMessageElementNamedMultiDimArray(m_data)

class NamedMultiDimArrayMemoryClient(object):
        
    def __init__(self,innermemory,type1,obj,node):
        self.__innermemory=innermemory
        self._type=type1
        self._obj=obj
        self._node=node

    @property
    def DimCount(self):
        return self.__innermemory.DimCount()
    
    @property
    def Dimensions(self):
        return list(self.__innermemory.Dimensions())

    def Read(self, memorypos, buf, bufferpos, count):
        b=NamedMultiDimArrayMemoryClient_bufferdirector(buf, self._type, self._obj, self._node)
        self.__innermemory.Read(memorypos, b, bufferpos, count)
        
    def Write(self,memorypos, buf, bufferpos, count):
        b=NamedMultiDimArrayMemoryClient_bufferdirector(buf, self._type, self._obj, self._node)
        self.__innermemory.Write(memorypos, b, bufferpos, count)

class ServiceInfo2(object):
    def __init__(self,info):
        self.Name=info.Name
        self.RootObjectType=info.RootObjectType
        self.RootObjectImplements=list(info.RootObjectImplements)
        self.ConnectionURL=list(info.ConnectionURL)
        self.NodeID=RobotRaconteurPython.NodeID(str(info.NodeID))
        self.NodeName=info.NodeName
        self.Attributes=UnpackMessageElement(info.Attributes,"varvalue{string} value")

class NodeInfo2(object):
    def __init__(self,info):
        self.NodeID=RobotRaconteurPython.NodeID(str(info.NodeID))
        self.NodeName=info.NodeName
        self.ConnectionURL=list(info.ConnectionURL)

class WrappedServiceSkelDirectorPython(RobotRaconteurPython.WrappedServiceSkelDirector):
    def __init__(self,obj):
        self.obj=obj
        super(WrappedServiceSkelDirectorPython,self).__init__()

    def Init(self,skel):
        
        self.skel=skel
        odef=skel.Type
        for i in range(len(odef.Members)):
            m=odef.Members[i]
            if (isinstance(m,RobotRaconteurPython.EventDefinition)):
                def inner_event(m1):

                    f=lambda *args : skel_dispatchevent(skel,m1.Name,m1,*args)
                    return f
                f1=inner_event(m)                
                evt=getattr(self.obj, m.Name)
                evt+=f1
            if (isinstance(m,RobotRaconteurPython.CallbackDefinition)):
                def inner_callback(m1):
                    if (m1.ReturnType.Type==RobotRaconteurPython.DataTypes_void_t):
                        f=lambda endpoint, *args : skel_callbackcallvoid(skel,m1.Name,m1,endpoint,*args)
                    else:
                        f=lambda endpoint, *args : skel_callbackcall(skel,m1.Name,m1,endpoint,*args)
                    return f
                f1=inner_callback(m)
                cs=CallbackServer(f1)
                
                setattr(self.obj,m.Name,cs)
            if (isinstance(m,RobotRaconteurPython.PipeDefinition)):
                p=skel.GetPipe(m.Name)
                outerp=Pipe(p)
                if (not hasattr(self.obj,m.Name)):
                    if ("readonly" in m.Modifiers):
                        setattr(self.obj,m.Name,PipeBroadcaster(outerp))                    
                    else:
                        setattr(self.obj,m.Name,outerp)
                else:             
                    setattr(self.obj,m.Name,outerp)
            if (isinstance(m,RobotRaconteurPython.WireDefinition)):
                w=skel.GetWire(m.Name)
                outerw=Wire(w)
                if (not hasattr(self.obj,m.Name)):
                    if ("readonly" in m.Modifiers):
                        setattr(self.obj,m.Name,WireBroadcaster(outerw))
                    elif ("writeonly" in m.Modifiers):
                        setattr(self.obj,m.Name,WireUnicastReceiver(outerw))
                    else:
                        setattr(self.obj,m.Name,outerw)
                else:                      
                    setattr(self.obj,m.Name,outerw)

    def _CallGetProperty(self, name):

        type1=FindMemberByName(self.skel.Type.Members,name)
        #type1=[e for e in self.skel.Type.Members if e.Name == name][0]
        type2=RobotRaconteurPython.MemberDefinitionUtil.ToProperty(type1)
        ret=getattr(self.obj,name)

        if (ret is None):
            m=RobotRaconteurPython.MessageElement()
            m.ElementName="value"
            m.ElementType=RobotRaconteurPython.DataTypes_void_t
            return m
        return PackMessageElement(ret,type2.Type,node=self.skel.RRGetNode())


    def _CallSetProperty(self, name, value):

        type1=FindMemberByName(self.skel.Type.Members,name)
        #type1=[e for e in self.skel.Type.Members if e.Name == name][0]
        type2=RobotRaconteurPython.MemberDefinitionUtil.ToProperty(type1)        
        a=UnpackMessageElement(value,type2.Type,node=self.skel.RRGetNode())
        setattr(self.obj,name,a)


    def _CallFunction(self, name, args1):

        type1=FindMemberByName(self.skel.Type.Members,name)
        type2=RobotRaconteurPython.MemberDefinitionUtil.ToFunction(type1)
        args=[]
        for p in type2.Parameters:
            if p.ContainerType == RobotRaconteurPython.DataTypes_ContainerTypes_generator:
                continue
            m=FindMessageElementByName(args1,p.Name)            
            a=UnpackMessageElement(m,p,node=self.skel.RRGetNode())
            args.append(a)
        ret=getattr(self.obj,name)(*args)

        if type2.IsGenerator():
            gen_return_type = None
            if type2.ReturnType.Type != RobotRaconteurPython.DataTypes_void_t:
                gen_return_type = type2.ReturnType.Clone()
                gen_return_type.RemoveContainers()
            gen_param_type = None
            if len(type2.Parameters) > 0 and type2.Parameters[-1].ContainerType == RobotRaconteurPython.DataTypes_ContainerTypes_generator:
                gen_param_type = type2.Parameters[-1]
            gen=WrappedGeneratorServerDirectorPython(ret, gen_return_type, gen_param_type, self.skel.RRGetNode())
            ind = self.skel.RegisterGeneratorServer(type2.Name, gen)
            gen.__disown__()
            return PackMessageElement(ind, "int32 index", node=self.skel.RRGetNode())           

        if (ret is None):
            m=RobotRaconteurPython.MessageElement()
            m.ElementName="return"
            m.ElementType=RobotRaconteurPython.DataTypes_void_t
            return m
        return PackMessageElement(ret,type2.ReturnType,node=self.skel.RRGetNode())


    def _GetSubObj(self, name, index):

        type1=FindMemberByName(self.skel.Type.Members,name)
        #type1=[e for e in self.skel.Type.Members if e.Name == name][0]
        type2=RobotRaconteurPython.MemberDefinitionUtil.ToObjRef(type1)
        if (type2.ArrayType == RobotRaconteurPython.DataTypes_ArrayTypes_array or (type2.ContainerType == RobotRaconteurPython.DataTypes_ContainerTypes_map_int32)):
            obj,objecttype=getattr(self.obj,'get_' + name)(str(index))
        elif (type2.ContainerType == RobotRaconteurPython.DataTypes_ContainerTypes_map_string):
            obj,objecttype=getattr(self.obj, 'get_' + name)(codecs.encode(index,'utf-8'))
        else:
            obj,objecttype=getattr(self.obj, 'get_' + name)()

        director=WrappedServiceSkelDirectorPython(obj)
        rrobj=RobotRaconteurPython.WrappedRRObject(objecttype,director,0)
        director.__disown__()
        return rrobj


    def _GetArrayMemory(self,name):

        m=getattr(self.obj, name)
        d=WrappedArrayMemoryDirectorPython(m)
        d.__disown__()
        return d


    def _GetMultiDimArrayMemory(self,name):
        m=getattr(self.obj, name)
        d=WrappedMultiDimArrayMemoryDirectorPython(m)
        d.__disown__()
        return d

    def _GetPodArrayMemory(self,name):
        type1=FindMemberByName(self.skel.Type.Members,name)
        type2=RobotRaconteurPython.MemberDefinitionUtil.ToMemory(type1)
        m=getattr(self.obj, name)
        d=WrappedPodArrayMemoryDirectorPython(m, type2.Type, self.skel.RRGetNode())
        d.__disown__()
        return d


    def _GetPodMultiDimArrayMemory(self,name):        
        type1=FindMemberByName(self.skel.Type.Members,name)
        type2=RobotRaconteurPython.MemberDefinitionUtil.ToMemory(type1)
        m=getattr(self.obj, name)
        d=WrappedPodMultiDimArrayMemoryDirectorPython(m, type2.Type, self.skel.RRGetNode())
        d.__disown__()
        return d

    def _GetNamedArrayMemory(self,name):
        type1=FindMemberByName(self.skel.Type.Members,name)
        type2=RobotRaconteurPython.MemberDefinitionUtil.ToMemory(type1)
        m=getattr(self.obj, name)
        d=WrappedNamedArrayMemoryDirectorPython(m, type2.Type, self.skel.RRGetNode())
        d.__disown__()
        return d
    
    def _GetNamedMultiDimArrayMemory(self,name):        
        type1=FindMemberByName(self.skel.Type.Members,name)
        type2=RobotRaconteurPython.MemberDefinitionUtil.ToMemory(type1)
        m=getattr(self.obj, name)
        d=WrappedNamedMultiDimArrayMemoryDirectorPython(m, type2.Type, self.skel.RRGetNode())
        d.__disown__()
        return d

    def MonitorEnter(self,timeout):

        self.obj.RobotRaconteurMonitorEnter(timeout)


    def MonitorExit(self):

        self.obj.RobotRaconteurMonitorExit()



    def ReleaseCastObj(self):
        self.obj=None
        self.skel=None

def skel_dispatchevent(skel,name,type1,*args):
    m=RobotRaconteurPython.vectorptr_messageelement()
    i=0
    node=skel.RRGetNode()
    for p in type1.Parameters:
        a=PackMessageElement(args[i],p,node=node)
        m.append(a)
        i+=1
    skel.WrappedDispatchEvent(name,m)

def skel_callbackcall(skel,name,type1,endpoint,*args):
    m=RobotRaconteurPython.vectorptr_messageelement()
    i=0
    node=skel.RRGetNode()
    for p in type1.Parameters:
        a=PackMessageElement(args[i],p,node=node)
        m.append(a)
        i+=1
    ret=skel.WrappedCallbackCall(name,endpoint,m)
    return UnpackMessageElement(ret,type1.ReturnType,node=node)

def skel_callbackcallvoid(skel,name,type1,endpoint,*args):
    m=RobotRaconteurPython.vectorptr_messageelement()
    i=0
    node=skel.RRGetNode()
    for p in type1.Parameters:
        a=PackMessageElement(args[i],p,node=node)
        m.append(a)
        i+=1
    skel.WrappedCallbackCall(name,endpoint,m)

class CallbackServer(object):
    def __init__(self,func):

        self.func=func
    def GetClientFunction(self,endpoint):
        func=self.func

        return lambda *args : func(endpoint,*args)



class WrappedArrayMemoryDirectorPython(RobotRaconteurPython.WrappedArrayMemoryDirector):
    def __init__(self,memory):
        self.memory=memory
        super(WrappedArrayMemoryDirectorPython,self).__init__()

    def Length(self):

        return self.memory.Length


    def Read(self,memorypos,buffer,bufferpos,count):

        buffer3=[0]*count
        self.memory.Read(memorypos,buffer3,bufferpos,count)        
        type1=RobotRaconteurPython.TypeDefinition()        
        type1.ArrayType=RobotRaconteurPython.DataTypes_ArrayTypes_array
        type1.ArrayVarLength=True
        type1.ArrayLength=RobotRaconteurPython.vectorint32([0])
        type1.Type=buffer.GetTypeID()
        PackToRRArray(buffer3,type1,buffer)



    def Write(self,memorypos,buffer,bufferpos,count):
                
        buffer3=UnpackFromRRArray(buffer,None)
        self.memory.Write(memorypos,buffer3,bufferpos,count)

class WrappedMultiDimArrayMemoryDirectorPython(RobotRaconteurPython.WrappedMultiDimArrayMemoryDirector):
    def __init__(self,memory):
        self.memory=memory
        super(WrappedMultiDimArrayMemoryDirectorPython,self).__init__()
        import RobotRaconteur        

    def Dimensions(self):

        d=self.memory.Dimensions
        d2=RobotRaconteurPython.vector_uint64_t()
        for d_i in d: d2.push_back(d_i)
        return d2


    def DimCount(self):

        return self.memory.DimCount

    def Read(self,p):

        rrmultidim=p.buffer
        rrt=RobotRaconteurPython.TypeDefinition()
        rrt.Type=rrmultidim.Array.GetTypeID()
        rrt.ArrayType=RobotRaconteurPython.DataTypes_ArrayTypes_array
        rrt.ArrayVarLength=True
        rrt.ArrayLength=RobotRaconteurPython.vectorint32([0])
        dims=list(p.count)
        elementcount=reduce(operator.mul,dims,1)
        
        readdat1=numpy.zeros(dims,dtype=RobotRaconteurPython._RRTypeIdToNumPyDataType(rrmultidim.Array.GetTypeID()))
        self.memory.Read(list(p.memorypos),readdat1,list(p.bufferpos),dims)

        PackToRRArray(readdat1.flatten(order="F"),rrt,rrmultidim.Array)
        
    def Write(self,p):

        memorypos=list(p.memorypos)
        bufferpos=list(p.bufferpos)
        count=list(p.count)

        rrmultidim=p.buffer
        md_dims=UnpackFromRRArray(rrmultidim.Dims)
        md_array=UnpackFromRRArray(rrmultidim.Array)        
        
        buffer=md_array.reshape(count,order="F")
        self.memory.Write(memorypos,buffer,bufferpos,count)
        

class WrappedPodArrayMemoryDirectorPython(RobotRaconteurPython.WrappedPodArrayMemoryDirector):
    def __init__(self,memory,type1,node):
        self.memory=memory
        self.node=node
        self.type=type1
        super(WrappedPodArrayMemoryDirectorPython,self).__init__()

    def Length(self):

        return self.memory.Length

    def Read(self,memorypos,bufferpos,count):
        dt=GetPodDType(self.type)
        buffer3=numpy.zeros((count,),dtype=dt)
        self.memory.Read(memorypos,buffer3,bufferpos,count)        
        m=PackMessageElement(buffer3,self.type,node=self.node)
        return RobotRaconteurPython.MessageElementDataUtil.ToMessageElementPodArray(m.GetData())
    
    def Write(self,memorypos,buffer_,bufferpos,count):        
        m=RobotRaconteurPython.MessageElement()
        m.SetData(buffer_)
        m.ElementTypeName=buffer_.Type
        m.DataCount=len(buffer_.Elements)
        buffer3=UnpackMessageElement(m,self.type, node=self.node)
        self.memory.Write(memorypos,buffer3,bufferpos,count)

class WrappedPodMultiDimArrayMemoryDirectorPython(RobotRaconteurPython.WrappedPodMultiDimArrayMemoryDirector):
    def __init__(self,memory,type1,node):
        super(WrappedPodMultiDimArrayMemoryDirectorPython,self).__init__()
        self.memory=memory
        self.type=type1
        self.node=node
                
    def Dimensions(self):

        d=self.memory.Dimensions
        d2=RobotRaconteurPython.vector_uint64_t()
        for d_i in d: d2.push_back(d_i)
        return d2

    def DimCount(self):
        return self.memory.DimCount

    def Read(self,memorypos,bufferpos,count):

        dims=list(count)        
        dt=GetPodDType(self.type)
                
        readdat1=numpy.zeros(dims,dtype=dt)

        self.memory.Read(list(memorypos),readdat1,list(bufferpos),dims)

        m = PackMessageElement(readdat1, self.type, node=self.node)
        return RobotRaconteurPython.MessageElementDataUtil.ToMessageElementPodMultiDimArray(m.GetData())
        
    def Write(self,memorypos_,buffer_, bufferpos_, count_):
        try:
            memorypos=list(memorypos_)
            bufferpos=list(bufferpos_)
            count=list(count_)
            
            buffer1=RobotRaconteurPython.MessageElementDataUtil.ToMessageElementPodMultiDimArray(buffer_)
            m=RobotRaconteurPython.MessageElement()
            m.SetData(buffer_)
            m.ElementTypeName=buffer1.Type
            m.DataCount=len(buffer1.Elements)
            buffer2=UnpackMessageElement(m,self.type, node=self.node)       
            self.memory.Write(memorypos,buffer2,bufferpos,count)
        except:
            traceback.print_exc()
            raise

class WrappedNamedArrayMemoryDirectorPython(RobotRaconteurPython.WrappedNamedArrayMemoryDirector):
    def __init__(self,memory,type1,node):
        self.memory=memory
        self.node=node
        self.type=type1
        super(WrappedNamedArrayMemoryDirectorPython,self).__init__()

    def Length(self):

        return self.memory.Length

    def Read(self,memorypos,bufferpos,count):
        dt=GetNamedArrayDType(self.type)
        buffer3=numpy.zeros((count,),dtype=dt)
        self.memory.Read(memorypos,buffer3,bufferpos,count)        
        m=PackMessageElement(buffer3,self.type,node=self.node)
        return RobotRaconteurPython.MessageElementDataUtil.ToMessageElementNamedArray(m.GetData())
    
    def Write(self,memorypos,buffer_,bufferpos,count):        
        m=RobotRaconteurPython.MessageElement()
        m.SetData(buffer_)
        m.ElementTypeName=buffer_.Type
        m.DataCount=len(buffer_.Elements)
        buffer3=UnpackMessageElement(m,self.type, node=self.node)
        self.memory.Write(memorypos,buffer3,bufferpos,count)

class WrappedNamedMultiDimArrayMemoryDirectorPython(RobotRaconteurPython.WrappedNamedMultiDimArrayMemoryDirector):
    def __init__(self,memory,type1,node):
        super(WrappedNamedMultiDimArrayMemoryDirectorPython,self).__init__()
        self.memory=memory
        self.type=type1
        self.node=node
                
    def Dimensions(self):

        d=self.memory.Dimensions
        d2=RobotRaconteurPython.vector_uint64_t()
        for d_i in d: d2.push_back(d_i)
        return d2

    def DimCount(self):
        return self.memory.DimCount

    def Read(self,memorypos,bufferpos,count):
        dims=list(count)
        dt=GetNamedArrayDType(self.type)
        readdat1=numpy.zeros(dims,dtype=dt)
        self.memory.Read(list(memorypos),readdat1,list(bufferpos),dims)
        m = PackMessageElement(readdat1, self.type, node=self.node)
        return RobotRaconteurPython.MessageElementDataUtil.ToMessageElementNamedMultiDimArray(m.GetData())
        
    def Write(self,memorypos_,buffer_, bufferpos_, count_):
        try:
            memorypos=list(memorypos_)
            bufferpos=list(bufferpos_)
            count=list(count_)
            
            buffer1=RobotRaconteurPython.MessageElementDataUtil.ToMessageElementNamedMultiDimArray(buffer_)
            m=RobotRaconteurPython.MessageElement()
            m.SetData(buffer_)
            m.ElementTypeName=buffer1.Type
            m.DataCount=len(buffer1.Elements)
            buffer2=UnpackMessageElement(m,self.type, node=self.node)       
            self.memory.Write(memorypos,buffer2,bufferpos,count)
        except:
            traceback.print_exc()
            raise

class WrappedClientServiceListenerDirector(RobotRaconteurPython.ClientServiceListenerDirector):
    def __init__(self,callback):
        self.callback=callback

        super(WrappedClientServiceListenerDirector,self).__init__()

    def Callback(self,code):

       self.callback(self.stub,code,None)


class WrappedServerServiceListenerDirector(RobotRaconteurPython.ServerServiceListenerDirector):
    def __init__(self,callback,context):
        self.callback=callback
        self.context=context

        super(WrappedServerServiceListenerDirector,self).__init__()

    def Callback(self,code,endpoint):

        self.callback(self.context,code,endpoint)

class RRConstants(object):
    pass

def convert_constant(const):
    
    if not const.VerifyValue():
        raise Exception("Invalid constant " + const.Name)
    t=const.Type
    
    if (t.Type==RobotRaconteurPython.DataTypes_string_t):
        return const.Name, const.ValueToString()

    if RobotRaconteurPython.IsTypeNumeric(t.Type):
        
        if (t.Type==RobotRaconteurPython.DataTypes_double_t or t.Type==RobotRaconteurPython.DataTypes_single_t):
            if (t.ArrayType == RobotRaconteurPython.DataTypes_ArrayTypes_array):
                s3=const.Value.strip().lstrip('{').rstrip('}')
                return const.Name, [float(i) for i in s3.split(',')]
            else:
                return const.Name, float(const.Value)
        else:
            if (t.ArrayType == RobotRaconteurPython.DataTypes_ArrayTypes_array):
                s3=const.Value.strip().lstrip('{').rstrip('}')
                return const.Name, [int(i) for i in s3.split(',')]
            else:
                return const.Name, const.Value
    
    if t.Type==RobotRaconteurPython.DataTypes_namedtype_t:
        struct_fields=const.ValueToStructFields()
        f=dict()
        for struct_field in struct_fields:
            f[struct_field.Name]=struct_field.ConstantRefName
        return const.Name, f

    raise Exception("Unknown constant type")

def ServiceDefinitionConstants(servicedef, node, obj):
    o=dict()
    for s in servicedef.Options:
        s2=s.split(None,1)
        if (s2[0]=="constant"):
            c=RobotRaconteurPython.ConstantDefinition(servicedef)
            c.FromString(s)
            name,val=convert_constant(c)
            o[name]=val

    for c in servicedef.Constants:
        name,val=convert_constant(c)
        o[name]=val
    
    elem_o=dict()
    for e in servicedef.Objects:
        o2=dict()
        for s in e.Options:

            s2=s.split(None,1)
            if (s2[0]=="constant"):
                c=RobotRaconteurPython.ConstantDefinition(servicedef)
                c.FromString(s)
                name,val=convert_constant(c)                
                o2[name]=val
        
        for c in e.Constants:
            name,val=convert_constant(c)
            o2[name]=val
        
        if (len(o2)>0):
            elem_o[e.Name]=o2
    
    for _, c_value in o.items():
        if isinstance(c_value,dict):
            for f_name, f_value in c_value.items():                
                if not f_value in o:
                    raise Exception("Invalid struct reference type: " + f_value)
                c_value[f_name]=o[f_value]
                
                
    for n,v in elem_o.values():
        o[n]=v
    
    for e in servicedef.Enums:
        o_enum=dict()
        for v in e.Values:
            o_enum[v.Name]=v.Value
        o[e.Name] = o_enum
        
    
    return o   

def adjust_timeout(t):
    if (t<0):
        return -1
    else:
        return int(t*1000)

class AsyncStubReturnDirectorImpl(RobotRaconteurPython.AsyncStubReturnDirector):
    def __init__(self,handler):
        super(AsyncStubReturnDirectorImpl,self).__init__()
        self._handler=handler

    def handler(self, innerstub2, error_code,errorname,errormessage):
        if (error_code!=0):
            err=RobotRaconteurPythonError.RobotRaconteurExceptionUtil.ErrorCodeToException(error_code,errorname,errormessage)
            self._handler(None,err)
            return
        try:            
            stub=innerstub2.GetPyStub()
            if (stub is None):
                stub=InitStub(innerstub2)
                innerstub2.SetPyStub(stub)
        except Exception as err2:
            self._handler(None, err2)
            return
        self._handler(stub, None)


class AsyncVoidReturnDirectorImpl(RobotRaconteurPython.AsyncVoidReturnDirector):
    def __init__(self,handler):
        super(AsyncVoidReturnDirectorImpl,self).__init__()
        self._handler=handler

    def handler(self, error_code,errorname,errormessage):
        if (error_code!=0):
            err=RobotRaconteurPythonError.RobotRaconteurExceptionUtil.ErrorCodeToException(error_code,errorname,errormessage)
            self._handler(err)
            return        
        self._handler(None)

class AsyncVoidNoErrReturnDirectorImpl(RobotRaconteurPython.AsyncVoidNoErrReturnDirector):
    def __init__(self,handler):
        super(AsyncVoidNoErrReturnDirectorImpl,self).__init__()
        self._handler=handler

    def handler(self):
        self._handler()

class AsyncStringReturnDirectorImpl(RobotRaconteurPython.AsyncStringReturnDirector):
    def __init__(self,handler):
        super(AsyncStringReturnDirectorImpl,self).__init__()
        self._handler=handler

    def handler(self, istr, error_code,errorname,errormessage):
        if (error_code!=0):
            err=RobotRaconteurPythonError.RobotRaconteurExceptionUtil.ErrorCodeToException(error_code,errorname,errormessage)
            self._handler(None,err)
            return       
        self._handler(istr, None)

class AsyncUInt32ReturnDirectorImpl(RobotRaconteurPython.AsyncUInt32ReturnDirector):
    def __init__(self,handler):
        super(AsyncUInt32ReturnDirectorImpl,self).__init__()
        self._handler=handler

    def handler(self, e, error_code,errorname,errormessage):
        if (error_code!=0):
            err=RobotRaconteurPythonError.RobotRaconteurExceptionUtil.ErrorCodeToException(error_code,errorname,errormessage)
            self._handler(None,err)
            return       
        self._handler(e, None)

class AsyncTimerEventReturnDirectorImpl(RobotRaconteurPython.AsyncTimerEventReturnDirector):
    def __init__(self,handler):
        super(AsyncTimerEventReturnDirectorImpl,self).__init__()
        self._handler=handler

    def handler(self, ev, error_code, errorname, errormessage):        
        self._handler(ev)

def async_call(func, args, directorclass, handler, noerror=False, directorargs=()):
    d=None
    if (handler is None):
        if (sys.version_info > (3,5)):
            loop = asyncio.get_event_loop()
            d = asyncio.Future()
                                    
            def handler3(*args):
                if noerror:
                    if len(args) == 0:
                        loop.call_soon_threadsafe(d.set_result,None)
                    else:
                        loop.call_soon_threadsafe(d.set_result,args[0])
                else:
                    ret = None
                    if len(args) == 2:
                        ret = args[0]
                    if args[-1] is None:
                        loop.call_soon_threadsafe(d.set_result,ret)
                    else:
                        loop.call_soon_threadsafe(d.set_exception,args[-1])
            handler = lambda *args1: handler3(*args1)            
        else:
            raise Exception("handler must not be None")
            
    handler2=directorclass(handler,*directorargs)
    args2=list(args)
    args2.extend([handler2,0])
    handler2.__disown__()
    func(*args2)
    return d

class ExceptionHandlerDirectorImpl(RobotRaconteurPython.AsyncVoidReturnDirector):
    def __init__(self,handler):
        super(ExceptionHandlerDirectorImpl,self).__init__()
        self._handler=handler

    def handler(self, error_code,errorname,errormessage):
        if (error_code!=0):
            err=RobotRaconteurPythonError.RobotRaconteurExceptionUtil.ErrorCodeToException(error_code,errorname,errormessage)
            self._handler(err)
            return

class WrappedPipeBroadcasterPredicateDirectorPython(RobotRaconteurPython.WrappedPipeBroadcasterPredicateDirector):
    def __init__(self, f):
        super(WrappedPipeBroadcasterPredicateDirectorPython,self).__init__()
        self.f=f

    def Predicate(self, client_endpoint, index_):
        try:
            return self.f(client_endpoint, index_)
        except:
            traceback.print_exc()
            return True

class PipeBroadcaster(object):
   

    def __init__(self,pipe,maximum_backlog=-1):
        self.pipe=pipe

        self._innerpipe=RobotRaconteurPython.WrappedPipeBroadcaster()
        self._innerpipe.Init(pipe._innerpipe, maximum_backlog)
        self._obj=pipe._obj
        self._type=pipe._innerpipe.Type
    
    def AsyncSendPacket(self, packet, handler):
        m=PackMessageElement(packet,self._type,self._obj)
        return async_call(self._innerpipe.AsyncSendPacket,(m,),AsyncVoidNoErrReturnDirectorImpl,handler)

    def SendPacket(self, packet):
        m=PackMessageElement(packet,self._type,self._obj)
        self._innerpipe.SendPacket(m)
        
    @property
    def ActivePipeEndpointCount(self):
        return self._innerpipe.GetActivePipeEndpointCount()

    def SetPredicate(self, f):
        p=WrappedPipeBroadcasterPredicateDirectorPython(f)
        self._innerpipe.SetPredicateDirector(p, 0)
        p.__disown__()
        
    @property
    def MaximumBacklog(self):
        return self._innerpipe.GetMaximumBacklog()
    
    @MaximumBacklog.setter
    def MaximumBacklog(self, maximum_backlog):
        self._innerpipe.SetMaximumBacklog(maximum_backlog)

class WrappedWireBroadcasterPredicateDirectorPython(RobotRaconteurPython.WrappedWireBroadcasterPredicateDirector):
    def __init__(self, f):
        super(WrappedWireBroadcasterPredicateDirectorPython,self).__init__()
        self.f=f

    def Predicate(self, client_endpoint):
        try:
            return self.f(client_endpoint)
        except:
            traceback.print_exc()
            return True

class WireBroadcaster(object):
    def __init__(self, wire):
        self._wire=wire
        self._innerpipe=RobotRaconteurPython.WrappedWireBroadcaster()        
        self._innerpipe.Init(wire._innerpipe)
        self._obj=wire._obj
        self._type=wire._innerpipe.Type

    @property
    def OutValue(self):
        raise Exception("Write only property")

    @OutValue.setter
    def OutValue(self, value):
        m=PackMessageElement(value,self._type,self._obj)
        self._innerpipe.SetOutValue(m)
                
    @property
    def ActiveWireConnectionCount(self):
        return self._innerpipe.GetActiveWireConnectionCount()

    def SetPredicate(self, f):
        p=WrappedWireBroadcasterPredicateDirectorPython(f)
        self._innerpipe.SetPredicateDirector(p, 0)
        p.__disown__()

class WrappedWireUnicastReceiverInValueChangedImpl(RobotRaconteurPython.WrappedWireServerPokeValueDirector):
    def __init__(self, rec):
        super(WrappedWireUnicastReceiverInValueChangedImpl,self).__init__()
        self._rec=weakref.ref(rec)      

    def PokeValue(self, m, ts, ep):
        rec=self._rec()
        if (rec is None): return
        value=UnpackMessageElement(m,rec._wire._innerpipe.Type,None,rec._wire._innerpipe.GetNode())
        rec.InValueChanged.fire(value, ts, ep)

class WireUnicastReceiver(object):
    def __init__(self, wire):
        self._wire=wire
        self._innerpipe=RobotRaconteurPython.WrappedWireUnicastReceiver()        
        self._innerpipe.Init(wire._innerpipe)
        self._obj=wire._obj
        self._type=wire._innerpipe.Type
        self._InValueChanged=EventHook()
        cb=WrappedWireUnicastReceiverInValueChangedImpl(self)
        self._innerpipe.AddInValueChangedListener(cb, 0)
        cb.__disown__()
        

    @property
    def InValue(self):
        ts=RobotRaconteurPython.TimeSpec()
        (m,ep)=self._innerpipe.GetInValue(ts)
        return (UnpackMessageElement(m,self._type,self._obj,self._wire._innerpipe.GetNode()),ts)
    
    @property
    def InValueChanged(self):
        return self._InValueChanged
    
    @InValueChanged.setter
    def InValueChanged(self, evt):
        if (evt is not self._InValueChanged):
            raise RuntimeError("Invalid operation")

class GeneratorClient(object):
    def __init__(self, inner_gen, return_type, param_type, obj, node):
        self._inner_gen=inner_gen
        self._obj=obj
        self._node=node        
        self._return_type = return_type
        self._param_type = param_type
    
    def _pack_param(self, param):
        param_type1=RobotRaconteurPython.TypeDefinition()
        self._param_type.CopyTo(param_type1)
        param_type1.RemoveContainers()            
        param1 = PackMessageElement(param, param_type1, self._obj, self._node)
        return param1
    
    def _unpack_return(self, ret):
        return_type1=RobotRaconteurPython.TypeDefinition()
        self._return_type.CopyTo(return_type1)
        return_type1.RemoveContainers()
        return UnpackMessageElement(ret,return_type1, self._obj, self._node)  
    
    def Next(self, param = None):
        if (self._param_type is not None and self._param_type.ContainerType == DataTypes_ContainerTypes_generator):
            param1 = self._pack_param(param)
        else:
            assert param is None
            param1 = None    
        ret1 = self._inner_gen.Next(param1)        
        return self._unpack_return(ret1)
            
    def AsyncNext(self, param, handler, timeout=RobotRaconteurPython.RR_TIMEOUT_INFINITE):
        if (self._param_type is not None and self._param_type.ContainerType == DataTypes_ContainerTypes_generator):
            param1 = self._pack_param(param)
        else:
            assert param is None
            param1 = None    
        
        return_type1=RobotRaconteurPython.TypeDefinition()
        self._return_type.CopyTo(return_type1)
        return_type1.RemoveContainers()
        
        if (self._return_type.Type != RobotRaconteurPython.DataTypes_void_t):
            return async_call(self._inner_gen.AsyncNext,(param1,adjust_timeout(timeout)),AsyncRequestDirectorImpl,handler,directorargs=(False,return_type1,self._obj,self._node))
        else:
            return async_call(self._inner_gen.AsyncNext,(param1,adjust_timeout(timeout)),AsyncRequestDirectorImpl,handler,directorargs=(True,return_type1,self._obj,self._node))
    
    def Abort(self):
        self._inner_gen.Abort()
    def AsyncAbort(self, handler, timeout=RobotRaconteurPython.RR_TIMEOUT_INFINITE):
        return async_call(self._inner_gen.AsyncAbort,(adjust_timeout(timeout),),AsyncVoidReturnDirectorImpl,handler)
    
    def Close(self):
        self._inner_gen.Close()
    def AsyncClose(self, handler, timeout=RobotRaconteurPython.RR_TIMEOUT_INFINITE):
        return async_call(self._inner_gen.AsyncClose,(adjust_timeout(timeout),),AsyncVoidReturnDirectorImpl,handler)           

    def NextAll(self):
        ret=[]
        try:
            while True:
                ret.append(self.Next())
        except RobotRaconteurPythonError.StopIterationException: pass
        return ret
    
    #Add compatibility for iterator protocols
    def __iter__(self):
        if self._return_type is None or self._param_type is not None:
            raise TypeError('Generator must be type 1 for iterable')
        return self
    
    def __next__(self):
        return self.send(None)
    
    def next(self):
        return self.send(None)
    
    def send(self, param):
        try:
            return self.Next(param)
        except RobotRaconteurPythonError.StopIterationException:
            raise StopIteration()

class AsyncGeneratorClientReturnDirectorImpl(RobotRaconteurPython.AsyncGeneratorClientReturnDirector):
    def __init__(self, handler, return_type, param_type, obj, node):
        super(AsyncGeneratorClientReturnDirectorImpl,self).__init__()
        self._handler=handler
        self._return_type=return_type
        self._param_type=param_type
        self._obj=obj
        self._node=node
        
    def handler(self, gen, error_code, errorname, errormessage):
        if (error_code!=0):
            err=RobotRaconteurPythonError.RobotRaconteurExceptionUtil.ErrorCodeToException(error_code,errorname,errormessage)
            self._handler(None,err)
            return
        
        gen2=GeneratorClient(gen, self._return_type, self._param_type, self._obj, self._node)
        self._handler(gen2, None)

class IteratorGenerator(object):
    def __init__(self, obj):
        self._iter=iter(obj)
        self._lock=threading.Lock()
        self._closed=False
        self._aborted=False
        
    def Next(self):
        with self._lock:
            if self._aborted:
                raise RobotRaconteurPythonError.OperationAbortedException()
            if self._closed:
                raise RobotRaconteurPythonError.StopIterationException()            
            try:
                return next(self._iter)
            except StopIteration:
                raise RobotRaconteurPythonError.StopIterationException()
    
    def Close(self):
        with self._lock:
            self._closed=True
            if hasattr(self._iter, "close"):
                self._iter.close()
    
    def Abort(self):
        with self._lock:
            self._aborted=True
            if hasattr(self._iter, "close"):
                self._iter.close()

class WrappedGeneratorServerDirectorPython(RobotRaconteurPython.WrappedGeneratorServerDirector):
    def __init__(self, gen, return_type, param_type, node):
        super(WrappedGeneratorServerDirectorPython,self).__init__()        
        self._return_type = return_type
        self._param_type = param_type
        self._node = node
        if hasattr(gen, "Next"):
            self._gen = gen
        else:
            if not hasattr(gen, "__iter__"):
                raise TypeError("Invalid generator")
            self._gen = IteratorGenerator(gen)
        
    def Next(self, m):
        if self._param_type is None:
            ret = self._gen.Next()
        else:
            param = UnpackMessageElement(m,self._param_type,node=self._node)
            ret = self._gen.Next(param)
        
        if ret is None:
            mret=RobotRaconteurPython.MessageElement()
            mret.ElementName="return"
            mret.ElementType=RobotRaconteurPython.DataTypes_void_t
            return mret
        
        return PackMessageElement(ret,self._return_type,node=self._node)
        
    def Abort(self):
        self._gen.Abort()
        
    def Close(self):
        self._gen.Close()
        
_trace_hook=sys.gettrace()

class ServiceSubscriptionClientID(object):
    def __init__(self, *args):
        if (len(args) == 1):
            self.NodeID=args[0].NodeID
            self.ServiceName=args[0].ServiceName
        elif (len(args) == 2):
            self.NodeID=args[0];
            self.ServiceName=args[1];
        else:
            self.NodeID=None
            self.ServiceName=None

    def __eq__(self, other):
        if (not hasattr(other,'NodeID') or not hasattr(other,'ServiceName')):
            return False;
        return (self.NodeID == other.NodeID) and (self.ServiceName == other.ServiceName)

    def __neq__(self,other):
        return not self == other

    def __hash__(self):
      return hash((str(self.NodeID), self.ServiceName))

class ServiceSubscriptionFilterNode(object):
    def __init__(self):
        self.NodeID=None
        self.NodeName=None
        self.Username=None
        self.Credentials=None

class ServiceSubscriptionFilter(object):
    def __init__(self):
        self.Nodes=[]
        self.ServiceNames=[]
        self.TransportSchemes=[]
        self.Predicate=None
        self.MaxConnections=1000000

class WrappedServiceInfo2SubscriptionDirectorPython(RobotRaconteurPython.WrappedServiceInfo2SubscriptionDirector):
    def __init__(self, subscription):
        super(WrappedServiceInfo2SubscriptionDirectorPython,self).__init__()
        self._subscription=weakref.ref(subscription);

    def ServiceDetected(self, subscription, id, info):
        s = self._subscription()
        if (s is None):
            return       
        try:
            s.ServiceDetected.fire(s, ServiceSubscriptionClientID(id), ServiceInfo2(info))
        except: pass

    def ServiceLost(self, subscription, id, info):
        s = self._subscription()
        if (s is None):
            return        
        try:
            s.ServiceLost.fire(s, ServiceSubscriptionClientID(id), ServiceInfo2(info))
        except: pass

class ServiceInfo2Subscription(object):
    def __init__(self, subscription):
        self._subscription=subscription        
        self._ServiceDetected=EventHook()
        self._ServiceLost=EventHook()
        director=WrappedServiceInfo2SubscriptionDirectorPython(self)
        subscription.SetRRDirector(director,0)
        director.__disown__()
    
    
    def GetDetectedServiceInfo2(self):
        o=dict()
        c1=self._subscription.GetDetectedServiceInfo2()
        for c2 in c1.items():
            id1=ServiceSubscriptionClientID(c2[0])
            stub=ServiceInfo2(c2[1])
            o[id1]=stub        
        return o

    def Close(self):
        self._subscription.Close()
            
    @property
    def ServiceDetected(self):
        return self._ServiceDetected
    
    @ServiceDetected.setter
    def ServiceDetected(self, evt):
        if (evt is not self._ServiceDetected):
            raise RuntimeError("Invalid operation")
    
    @property
    def ServiceLost(self):
        return self._ServiceLost
    
    @ServiceLost.setter
    def ServiceLost(self, evt):
        if (evt is not self._ServiceLost):
            raise RuntimeError("Invalid operation")

class WrappedServiceSubscriptionDirectorPython(RobotRaconteurPython.WrappedServiceSubscriptionDirector):
    def __init__(self, subscription):
        super(WrappedServiceSubscriptionDirectorPython,self).__init__()
        self._subscription=weakref.ref(subscription);

    def ClientConnected(self, subscription, id, client):
        s = self._subscription()
        if (s is None):
            return
        
        client2=s._GetClientStub(client)
        
        try:
            s.ClientConnected.fire(s, ServiceSubscriptionClientID(id), client2)
        except: pass

    def ClientDisconnected(self, subscription, id, client):
        s = self._subscription()
        if (s is None):
            return

        client2=s._GetClientStub(client)

        try:
            s.ClientDisconnected.fire(s, ServiceSubscriptionClientID(id), client2)
        except: pass

class ServiceSubscription(object):
    def __init__(self, subscription):
        self._subscription=subscription        
        self._ClientConnected=EventHook()
        self._ClientDisconnected=EventHook()
        director=WrappedServiceSubscriptionDirectorPython(self)
        subscription.SetRRDirector(director,0)
        director.__disown__()
    
    def _GetClientStub(self, innerstub):
        if (innerstub is None):
            return None
        s2=innerstub.GetPyStub()
        if (s2 is not None): return s2
        return InitStub(innerstub)
        
    def GetConnectedClients(self):
        o=dict()
        c1=self._subscription.GetConnectedClients()
        for c2 in c1.items():
            id1=ServiceSubscriptionClientID(c2[0])
            stub=self._GetClientStub(c2[1])
            o[id1]=stub        
        return o

    def Close(self):
        self._subscription.Close()

    @property
    def ConnectRetryDelay(self):
        return self._subscription.GetConnectRetryDelay() / 1000.0

    @ConnectRetryDelay.setter
    def ConnectRetryDelay(self,value):
        if (value < 1):
            raise Exception("Invalid ConnectRetryDelay value")
        self._subscription.SetConnectRetryDelay(int(value*1000.0))

    def SubscribeWire(self, wire_name):
        s=self._subscription.SubscribeWire(wire_name)
        return WireSubscription(s)

    def SubscribePipe(self, pipe_name):
        s=self._subscription.SubscribePipe(pipe_name)
        return PipeSubscription(s)
    
    @property
    def ClientConnected(self):
        return self._ClientConnected
    
    @ClientConnected.setter
    def ClientConnected(self, evt):
        if (evt is not self._ClientConnected):
            raise RuntimeError("Invalid operation")
    
    @property
    def ClientDisconnected(self):
        return self._ClientDisconnected
    
    @ClientDisconnected.setter
    def ClientDisconnected(self, evt):
        if (evt is not self._ClientDisconnected):
            raise RuntimeError("Invalid operation")
            
class WrappedWireSubscriptionDirectorPython(RobotRaconteurPython.WrappedWireSubscriptionDirector):
    def __init__(self,subscription):
        super(WrappedWireSubscriptionDirectorPython,self).__init__()
        self._subscription=weakref.ref(subscription)

    def WireValueChanged(self, subscription, value, time):
        s=self._subscription()
        if (s is None):
            return

        try:            
            v=RobotRaconteurPython._UnpackMessageElement(value, None, None, None, True)            
            s.WireValueChanged.fire(s,v,time)
        except:
            traceback.print_exc()

class WireSubscription(object):
    def __init__(self, subscription):
        self._subscription=subscription
        director=WrappedWireSubscriptionDirectorPython(self)
        subscription.SetRRDirector(director,0)
        director.__disown__()
        self._WireValueChanged=EventHook()

    def _UnpackValue(self, m):
        return RobotRaconteurPython._UnpackMessageElement(m, None, None, None, True)

    @property
    def InValue(self):
        return self._UnpackValue(self._subscription.GetInValue())

    @property
    def InValueWithTimeSpec(self):
        t=RobotRaconteurPython.TimeSpec()
        m=self._subscription.GetInValue(t)
        return (self._UnpackValue(m), t)

    def WaitInValueValid(self, timeout=-1):
        return self._subscription.WaitInValueValid(adjust_timeout(timeout))

    @property
    def ActiveWireConnectionCount(self):
        return self._subscription.GetActiveWireConnectionCount()

    @property
    def IgnoreInValue(self):
        return self._subscription.GetIgnoreInValue()

    @IgnoreInValue.setter
    def IgnoreInValue(self, ignore):
        self._subscription.SetIgnoreInValue(ignore)

    def SetOutValueAll(self, value):        
        iter=RobotRaconteurPython.WrappedWireSubscription_send_iterator(self._subscription)
        try:
            while iter.Next() is not None:
                m=PackMessageElement(value,iter.GetType(),iter.GetStub())
                iter.SetOutValue(m)                
        finally:
            del iter

    def Close(self):
        self._subscription.Close()
        
    @property
    def WireValueChanged(self):
        return self._WireValueChanged
    
    @WireValueChanged.setter
    def WireValueChanged(self, evt):
        if (evt is not self._WireValueChanged):
            raise RuntimeError("Invalid operation")

class WrappedPipeSubscriptionDirectorPython(RobotRaconteurPython.WrappedPipeSubscriptionDirector):
    def __init__(self,subscription):
        super(WrappedPipeSubscriptionDirectorPython,self).__init__()
        self._subscription=weakref.ref(subscription)

    def PipePacketReceived(self, subscription):
        s=self._subscription()
        if (s is None):
            return

        try:            
            s.PipePacketReceived.fire(s)
        except:
            traceback.print_exc()

class PipeSubscription(object):
    def __init__(self, subscription):
        self._subscription=subscription
        director=WrappedPipeSubscriptionDirectorPython(self)
        subscription.SetRRDirector(director,0)
        director.__disown__()
        self._PipePacketReceived=EventHook()

    def _UnpackValue(self, m):
        return RobotRaconteurPython._UnpackMessageElement(m, None, None, None, True)

    def ReceivePacket(self):
        return self._UnpackValue(self._subscription.ReceivePacket())

    def TryReceivePacket(self):
        return self.TryReceivePacketWait(0)

    def TryReceivePacketWait(self, timeout=-1):
        m=RobotRaconteurPython.MessageElement()
        res=self._subscription.TryReceivePacketWait(m, adjust_timeout(timeout))
        if (not res):
            return (False, None)
        else:
            return (True, self._UnpackValue(m))

    @property
    def Available(self):
        return self._subscription.Available()

    def AsyncSendPacketAll(self, packet):        
        iter=RobotRaconteurPython.WrappedPipeSubscription_send_iterator(self._subscription)
        try:
            while iter.Next() is not None:
                m=PackMessageElement(packet,iter.GetType(),iter.GetStub())
                iter.AsyncSendPacket(m)                
        finally:
            del iter

    @property
    def ActivePipeEndpointCount(self):
        return self._subscription.GetActivePipeEndpointCount()

    def Close(self):
        return self._subscription.Close()
    
    @property
    def PipePacketReceived(self):
        return self._PipePacketReceived
    
    @PipePacketReceived.setter
    def PipePacketReceived(self, evt):
        if (evt is not self._PipePacketReceived):
            raise RuntimeError("Invalid operation")   

class WrappedServiceSubscriptionFilterPredicateDirectorPython(RobotRaconteurPython.WrappedServiceSubscriptionFilterPredicateDirector):
    def __init__(self, f):
        super(WrappedServiceSubscriptionFilterPredicateDirectorPython,self).__init__()
        self._f=f

    def Predicate(self, info):
        info2=ServiceInfo2(info)
        return self._f(info2)

def _SubscribeService_LoadFilter(node, filter_):
    filter2=None
    if (filter_ is not None):
        filter2=RobotRaconteurPython.WrappedServiceSubscriptionFilter()
        if (filter_.ServiceNames is not None):
            for s in filter_.ServiceNames:
                filter2.ServiceNames.append(s)
        if (filter_.TransportSchemes is not None):
            for s in filter_.TransportSchemes:
                filter2.TransportSchemes.append(s)
        filter2.MaxConnections=filter_.MaxConnections

        if (filter_.Nodes is not None):
            nodes2=RobotRaconteurPython.vectorptr_wrappedservicesubscriptionnode()
            for n1 in filter_.Nodes:
                if (n1 is None):
                    continue
                n2=RobotRaconteurPython.WrappedServiceSubscriptionFilterNode()
                if (n1.NodeID is not None): n2.NodeID=n1.NodeID
                if (n1.NodeName is not None): n2.NodeName=n1.NodeName
                if (n1.Username is not None): n2.Username=n1.Username
                if (n1.Credentials is not None):
                    n2.Credentials=PackMessageElement(n1.Credentials,"varvalue{string}",None,node).GetData()
                
                nodes2.append(n2)
            filter2.Nodes=nodes2

        if (filter_.Predicate is not None):
            director=WrappedServiceSubscriptionFilterPredicateDirectorPython(filter_.Predicate)
            filter2.SetRRPredicateDirector(director, 0)
            director.__disown__()
    return filter2

def SubscribeServiceInfo2(node, service_types, filter_=None):

    filter2=_SubscribeService_LoadFilter(node, filter_)

    service_types2=RobotRaconteurPython.vectorstring()
    if (sys.version_info  > (3,0)):
        if (isinstance(service_types, str)):
            service_types2.append(service_types)
        else:
            for s in service_types: service_types2.append(s)
    else:
        if (isinstance(service_types, (str, unicode))):
            service_types2.append(service_types)
        else:
            for s in service_types: service_types2.append(s)

    
    sub1=RobotRaconteurPython.WrappedSubscribeServiceInfo2(node, service_types2, filter2)
    return ServiceInfo2Subscription(sub1)

def SubscribeService(node, service_types, filter_=None):

    filter2=_SubscribeService_LoadFilter(node, filter_)

    service_types2=RobotRaconteurPython.vectorstring()
    if (sys.version_info  > (3,0)):
        if (isinstance(service_types, str)):
            service_types2.append(service_types)
        else:
            for s in service_types: service_types2.append(s)
    else:
        if (isinstance(service_types, (str, unicode))):
            service_types2.append(service_types)
        else:
            for s in service_types: service_types2.append(s)

    
    sub1=RobotRaconteurPython.WrappedSubscribeService(node, service_types2, filter2)
    return ServiceSubscription(sub1)   

class WrappedUserAuthenticatorDirectorPython(RobotRaconteurPython.WrappedUserAuthenticatorDirector):
	def __init__(self,target):
		super(WrappedUserAuthenticatorDirectorPython,self).__init__();
		self.target=target
		
	def AuthenticateUser(self, username, credentials, context):
				
		c2=UnpackMessageElement(credentials,"varvalue{string}")
		ret=self.target.AuthenticateUser(username,c2,context)
		return ret
		
def _UserAuthenticator_PackCredentials(credentials):
	return PackMessageElement(credentials,"varvalue{string}")
		

def ReadServiceDefinitionFile(servicedef_name):
    f_name = None
    if (os.path.isfile(servicedef_name)):
        f_name=servicedef_name
    elif(os.path.isfile(servicedef_name + '.robdef')):
        f_name=servicedef_name + '.robdef'
    elif not os.path.isabs(servicedef_name):
        p = os.getenv("ROBOTRACONTEUR_ROBDEF_PATH", None)
        if p is not None:
            p1=p.split(os.pathsep)
            for p2 in p1:
                p3=p2.strip()
                if (os.path.isfile(os.path.join(p3, servicedef_name))):
                    f_name=os.path.join(p3, servicedef_name)
                if (os.path.isfile(os.path.join(p3, servicedef_name + '.robdef'))):
                    f_name=os.path.join(p3, servicedef_name + '.robdef')
    
    if f_name is None:
        raise IOError("Service definition file %s not found" % servicedef_name)
    
    with codecs.open(f_name, 'r', 'utf-8-sig') as f:
        return f.read()

class RobotRaconteurNodeSetup(object):
    def __init__(self, node_name, tcp_port, flags, node=None):
        if node_name is None:
            node_name = ""
        if tcp_port is None:
            tcp_port = 0
        if flags is None:
            flags = 0
        if node is None:
            node=RobotRaconteurPython.RobotRaconteurNode.s
        self.__setup=RobotRaconteurPython.WrappedRobotRaconteurNodeSetup(node,node_name,tcp_port,flags)    
        self.tcp_transport=self.__setup.GetTcpTransport()
        self.local_transport=self.__setup.GetLocalTransport()
        self.hardware_transport=self.__setup.GetHardwareTransport()
        self.__node=node
        
    def __enter__(self):
        return self
    
    def __exit__(self, etype, value, traceback):
        self.__node.Shutdown()
        
class ClientNodeSetup(RobotRaconteurNodeSetup):
    def __init__(self, node_name=None, flags=RobotRaconteurPython.RobotRaconteurNodeSetupFlags_CLIENT_DEFAULT, node=None):
        super(ClientNodeSetup,self).__init__(node_name,0,flags,node)
            
class ServerNodeSetup(RobotRaconteurNodeSetup):
    def __init__(self, node_name, tcp_port, flags=RobotRaconteurPython.RobotRaconteurNodeSetupFlags_SERVER_DEFAULT, node=None):
        super(ServerNodeSetup,self).__init__(node_name,tcp_port,flags,node)

class SecureServerNodeSetup(RobotRaconteurNodeSetup):
    def __init__(self, node_name, tcp_port, flags=RobotRaconteurPython.RobotRaconteurNodeSetupFlags_SECURE_SERVER_DEFAULT, node=None):
        super(SecureServerNodeSetup,self).__init__(node_name,tcp_port,flags,node)


def settrace():
    #This function enables debugging for the threads started by the ThreadPool
    #You may see a warning in Eclipse; it can safely be ignored.
    t=_trace_hook
    if (t is not None):
        sys.settrace(t)

