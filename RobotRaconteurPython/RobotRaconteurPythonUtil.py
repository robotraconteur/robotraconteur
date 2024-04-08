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

from __future__ import absolute_import, print_function

from . import RobotRaconteurPython
from .RobotRaconteurPythonDataTypes import *
from . import RobotRaconteurPythonError
import operator
import traceback
import threading
import functools
import sys
import warnings
import weakref
import codecs
import numbers
import os
from RobotRaconteur.RobotRaconteurPython import DataTypes_ContainerTypes_generator
import numpy
import itertools

if (sys.version_info > (3, 0)):
    from builtins import property
    from functools import reduce
else:
    from __builtin__ import property

import numpy

if (sys.version_info > (3, 5)):
    import asyncio

_traceback_requested = str(os.environ.get('ROBOTRACONTEUR_PYTHON_TRACEBACK_PRINT_EXC', "0")
                           ).lower() in ("1", "true", "yes", "on")


def _print_exc_traceback(exc):
    if _traceback_requested:
        print("RobotRaconteurPython async caught exception:", file=sys.stderr)
        traceback.print_exception(exc, file=sys.stderr)


def SplitQualifiedName(name):
    pos = name.rfind('.')
    if (pos == -1):
        raise Exception("Name is not qualified")
    return (name[0:pos], name[pos + 1:])


def FindStructureByName(l, name):
    for i in l:
        if (i.Name == name):
            return i
    raise RobotRaconteurPythonError.ServiceException(
        "Structure " + name + " not found")


def FindMemberByName(l, name):
    for i in l:
        if (i.Name == name):
            return i
    raise RobotRaconteurPythonError.MemberNotFoundException(
        "Member " + name + " not found")


def FindMessageElementByName(l, name):
    return RobotRaconteurPython.MessageElement.FindElement(l, name)


def PackMessageElement(data, type1, obj=None, node=None):

    if (hasattr(obj, 'rrinnerstub')):
        obj = obj.rrinnerstub

    if (isinstance(type1, str)):
        if (len(type1.split()) == 1):
            type1 = type1 + " value"
        type2 = RobotRaconteurPython.TypeDefinition()
        type2.FromString(type1)
        type1 = type2

    return RobotRaconteurPython._PackMessageElement(data, type1, obj, node)


def UnpackMessageElement(element, type1=None, obj=None, node=None):

    if (hasattr(obj, 'rrinnerstub')):
        obj = obj.rrinnerstub

    if (not type1 is None):
        if (isinstance(type1, str)):
            if (len(type1.split()) == 1):
                type1 = type1 + " value"
            type2 = RobotRaconteurPython.TypeDefinition()
            type2.FromString(type1)
            type1 = type2

    return RobotRaconteurPython._UnpackMessageElement(element, type1, obj, node)


def PackToRRArray(array, type1, destrrarray=None):

    return RobotRaconteurPython._PackToRRArray(array, type1, destrrarray)


def UnpackFromRRArray(rrarray, type1=None):

    if (rrarray is None):
        return None

    return RobotRaconteurPython._UnpackFromRRArray(rrarray, type1)


def CreateStructureType(name, dict_):
    def struct_init(s):
        for k, v in dict_.items():
            init_type, init_args = v
            if init_type is None:
                setattr(s, k, None)
            else:
                setattr(s, k, init_type(*init_args))
    slots = list(dict_.keys())
    return type(name, (RobotRaconteurStructure,), {'__init__': struct_init, "__slots__": slots})


def CreateZeroArray(dtype, dims):
    if dims is None:
        return numpy.zeros((0,), dtype)
    else:
        return numpy.zeros(dims, dtype)


def NewStructure(StructType, obj=None, node=None):
    if (hasattr(obj, 'rrinnerstub')):
        obj = obj.rrinnerstub
    return RobotRaconteurPython._NewStructure(StructType, obj, node)


def GetStructureType(StructType, obj=None, node=None):
    if (hasattr(obj, 'rrinnerstub')):
        obj = obj.rrinnerstub
    return RobotRaconteurPython._GetStructureType(StructType, obj, node)


def GetPodDType(pod_type, obj=None, node=None):
    if (hasattr(obj, 'rrinnerstub')):
        obj = obj.rrinnerstub
    return RobotRaconteurPython._GetNumPyDescrForType(pod_type, obj, node)


def GetNamedArrayDType(namedarray_type, obj=None, node=None):
    if (hasattr(obj, 'rrinnerstub')):
        obj = obj.rrinnerstub
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
        return b.reshape(a.shape[0:-1], order="C")
    return b


def InitStub(stub):
    odef = stub.RR_objecttype
    mdict = {}

    for i in range(len(odef.Members)):
        m = odef.Members[i]
        if (isinstance(m, RobotRaconteurPython.PropertyDefinition)):
            def inner_prop(m1):
                def fget(self): return stub_getproperty(stub, m1.Name, m1)
                def fset(self, value): return stub_setproperty(
                    stub, m1.Name, m1, value)
                return property(fget, fset)
            p1 = inner_prop(m)

            mdict[m.Name] = p1

            def inner_async_prop(m1):
                def fget(self, handler, timeout=RobotRaconteurPython.RR_TIMEOUT_INFINITE): return stub_async_getproperty(
                    stub, m1.Name, m1, handler, timeout)

                def fset(self, value, handler, timeout=RobotRaconteurPython.RR_TIMEOUT_INFINITE): return stub_async_setproperty(
                    stub, m1.Name, m1, value, handler, timeout)
                return fget, fset
            p1_async = inner_async_prop(m)

            mdict['async_get_' + m.Name] = p1_async[0]
            mdict['async_set_' + m.Name] = p1_async[1]

        if (isinstance(m, RobotRaconteurPython.FunctionDefinition)):
            def inner_func(m1):
                if (not m1.IsGenerator()):
                    if (m1.ReturnType.Type == RobotRaconteurPython.DataTypes_void_t):
                        f = lambda self, * \
                            args: stub_functioncallvoid(
                                stub, m1.Name, m1, *args)
                    else:
                        f = lambda self, * \
                            args: stub_functioncall(stub, m1.Name, m1, *args)
                    return f
                else:
                    return lambda self, *args: stub_functioncallgenerator(stub, m1.Name, m1, *args)
            f1 = inner_func(m)
            mdict[m.Name] = f1

            def inner_async_func(m1):
                if not m1.IsGenerator():
                    if (m1.ReturnType.Type == RobotRaconteurPython.DataTypes_void_t):
                        f = lambda self, * \
                            args: stub_async_functioncallvoid(
                                stub, m1.Name, m1, *args)
                    else:
                        f = lambda self, * \
                            args: stub_async_functioncall(
                                stub, m1.Name, m1, *args)
                    return f
                else:
                    return lambda self, *args: stub_async_functioncallgenerator(stub, m1.Name, m1, *args)

            f1_async = inner_async_func(m)
            mdict['async_' + m.Name] = f1_async

        if (isinstance(m, RobotRaconteurPython.EventDefinition)):
            def new_evt_hook():
                evt = EventHook()
                def fget(self): return evt

                def fset(self, value):
                    if (value is not evt):
                        raise RuntimeError("Invalid operation")
                return property(fget, fset)
            mdict[m.Name] = new_evt_hook()

        if (isinstance(m, RobotRaconteurPython.ObjRefDefinition)):
            def inner_objref(m1):
                if (m1.ArrayType != RobotRaconteurPython.DataTypes_ArrayTypes_none or m1.ContainerType != RobotRaconteurPython.DataTypes_ContainerTypes_none):
                    def f(self, index): return stub_objref(
                        stub, m1.Name, index)
                else:
                    def f(self): return stub_objref(stub, m1.Name)
                return f
            f1 = inner_objref(m)
            mdict['get_%s' % m.Name] = f1

            def inner_async_objref(m1):
                if (m1.ArrayType != RobotRaconteurPython.DataTypes_ArrayTypes_none or m1.ContainerType != RobotRaconteurPython.DataTypes_ContainerTypes_none):
                    def f(self, index, handler, timeout=RobotRaconteurPython.RR_TIMEOUT_INFINITE): return stub_async_objref(
                        stub, m1.Name, index, handler, timeout)
                else:
                    def f(self, handler, timeout=RobotRaconteurPython.RR_TIMEOUT_INFINITE): return stub_async_objref(
                        stub, m1.Name, None, handler, timeout)
                return f
            f1 = inner_async_objref(m)
            mdict['async_get_%s' % m.Name] = f1

        if (isinstance(m, RobotRaconteurPython.PipeDefinition)):
            def inner_pipe(m1):
                innerp = stub.GetPipe(m1.Name)
                outerp = Pipe(innerp)
                def fget(self): return outerp
                return property(fget)
            p2 = inner_pipe(m)
            mdict[m.Name] = p2

        if (isinstance(m, RobotRaconteurPython.CallbackDefinition)):
            def new_cb_client():
                cb = CallbackClient()
                def fget(self): return cb
                return property(fget)
            mdict[m.Name] = new_cb_client()

        if (isinstance(m, RobotRaconteurPython.WireDefinition)):
            def inner_wire(m1):
                innerw = stub.GetWire(m1.Name)
                outerw = Wire(innerw)
                def fget(self): return outerw
                return property(fget)
            w = inner_wire(m)
            mdict[m.Name] = w

        if (isinstance(m, RobotRaconteurPython.MemoryDefinition)):
            if (RobotRaconteurPython.IsTypeNumeric(m.Type.Type)):
                def inner_memory(m1):
                    if (m.Type.ArrayType == RobotRaconteurPython.DataTypes_ArrayTypes_array):
                        outerm = ArrayMemoryClient(
                            stub.GetArrayMemory(m1.Name))
                    else:
                        outerm = MultiDimArrayMemoryClient(
                            stub.GetMultiDimArrayMemory(m1.Name))

                    def fget(self): return outerm
                    return property(fget)
                mem = inner_memory(m)
                mdict[m.Name] = mem
            else:
                memory_rr_type = RobotRaconteurPython._GetNamedTypeEntryType(
                    m.Type, stub, stub.RRGetNode())
                if (memory_rr_type == RobotRaconteurPython.DataTypes_pod_t):
                    def inner_memory(m1):
                        if (m.Type.ArrayType == RobotRaconteurPython.DataTypes_ArrayTypes_array):
                            pass
                            outerm = PodArrayMemoryClient(stub.GetPodArrayMemory(
                                m1.Name), m.Type, stub, stub.RRGetNode())
                        else:
                            outerm = PodMultiDimArrayMemoryClient(
                                stub.GetPodMultiDimArrayMemory(m1.Name), m.Type, stub, stub.RRGetNode())
                            pass

                        def fget(self): return outerm
                        return property(fget)
                    mem = inner_memory(m)
                    mdict[m.Name] = mem
                elif (memory_rr_type == RobotRaconteurPython.DataTypes_namedarray_t):
                    def inner_memory(m1):
                        if (m.Type.ArrayType == RobotRaconteurPython.DataTypes_ArrayTypes_array):
                            pass
                            outerm = NamedArrayMemoryClient(stub.GetNamedArrayMemory(
                                m1.Name), m.Type, stub, stub.RRGetNode())
                        else:
                            outerm = NamedMultiDimArrayMemoryClient(
                                stub.GetNamedMultiDimArrayMemory(m1.Name), m.Type, stub, stub.RRGetNode())
                            pass

                        def fget(self): return outerm
                        return property(fget)
                    mem = inner_memory(m)
                    mdict[m.Name] = mem
                else:
                    assert False

    mdict["__slots__"] = ["rrinnerstub", "rrlock"]
    outerstub_type = type(str(odef.Name), (ServiceStub,), mdict)
    outerstub = outerstub_type()

    for i in range(len(odef.Members)):
        m = odef.Members[i]

        if (isinstance(m, RobotRaconteurPython.PipeDefinition)):
            p = getattr(outerstub, m.Name)
            p._obj = outerstub

        if (isinstance(m, RobotRaconteurPython.WireDefinition)):
            w = getattr(outerstub, m.Name)
            w._obj = outerstub

    director = WrappedServiceStubDirectorPython(outerstub, stub)
    stub.SetRRDirector(director, 0)
    director.__disown__()

    stub.SetPyStub(outerstub)

    outerstub.rrinnerstub = stub
    outerstub.rrlock = threading.RLock()
    return outerstub


def check_member_args(name, param_types, args, isasync=False):
    expected_args_len = len(param_types)
    if isasync:
        expected_args_len += 1
    if len(param_types) > 0 and param_types[-1].ContainerType == RobotRaconteurPython.DataTypes_ContainerTypes_generator:
        expected_args_len -= 1
    if (expected_args_len != len(args)) and not (isasync and expected_args_len + 1 == len(args)):
        raise TypeError("%s() expects exactly %d arguments (%d given)" % (
            name, expected_args_len, len(args)))


def stub_getproperty(stub, name, type1):
    return UnpackMessageElement(stub.PropertyGet(name), type1.Type, stub, stub.RRGetNode())


def stub_setproperty(stub, name, type1, value):
    pvalue = PackMessageElement(value, type1.Type, stub, stub.RRGetNode())
    stub.PropertySet(name, pvalue)


def stub_functioncall(stub, name, type1, *args):
    check_member_args(name, type1.Parameters, args)
    m = RobotRaconteurPython.vectorptr_messageelement()
    i = 0
    for p in type1.Parameters:
        a = PackMessageElement(args[i], p, stub, stub.RRGetNode())
        a.ElementName = p.Name
        m.append(a)
        i += 1
    ret = stub.FunctionCall(name, m)
    return UnpackMessageElement(ret, type1.ReturnType, stub, stub.RRGetNode())


def stub_functioncallvoid(stub, name, type1, *args):
    check_member_args(name, type1.Parameters, args)
    m = RobotRaconteurPython.vectorptr_messageelement()
    i = 0
    for p in type1.Parameters:
        a = PackMessageElement(args[i], p, stub)
        a.ElementName = p.Name
        m.append(a)
        i += 1
    stub.FunctionCall(name, m)


def stub_functioncallgenerator(stub, name, type1, *args):
    check_member_args(name, type1.Parameters, args)
    m = RobotRaconteurPython.vectorptr_messageelement()
    i = 0
    param_type = None
    for p in type1.Parameters:
        if (p.ContainerType != RobotRaconteurPython.DataTypes_ContainerTypes_generator):
            a = PackMessageElement(args[i], p, stub)
            a.ElementName = p.Name
            m.append(a)
            i += 1
        else:
            param_type = p
    return GeneratorClient(stub.GeneratorFunctionCall(name, m), type1.ReturnType, param_type, stub, stub.RRGetNode())


class AsyncRequestDirectorImpl(RobotRaconteurPython.AsyncRequestDirector):
    def __init__(self, handler, isvoid, Type, stub, node):
        super(AsyncRequestDirectorImpl, self).__init__()
        self._handler = handler
        self._isvoid = isvoid
        self._Type = Type
        self._node = node
        self._stub = stub

    def handler(self, m, error_info):

        if (self._isvoid):
            if (error_info.error_code != 0):
                err = RobotRaconteurPythonError.RobotRaconteurExceptionUtil.ErrorInfoToException(
                    error_info)
                self._handler(err)
                return
            else:
                self._handler(None)
        else:
            if (error_info.error_code != 0):
                err = RobotRaconteurPythonError.RobotRaconteurExceptionUtil.ErrorInfoToException(
                    error_info)
                self._handler(None, err)
                return
            else:
                try:
                    a = UnpackMessageElement(
                        m, self._Type, self._stub, self._node)
                except Exception as err:
                    self._handler(None, err)
                    return
                self._handler(a, None)
                return


def stub_async_getproperty(stub, name, type1, handler, timeout=-1):
    return async_call(stub.async_PropertyGet, (name, adjust_timeout(timeout)), AsyncRequestDirectorImpl, handler, directorargs=(False, type1.Type, stub, stub.RRGetNode()))


def stub_async_setproperty(stub, name, type1, value, handler, timeout=-1):
    pvalue = PackMessageElement(value, type1.Type, stub)
    return async_call(stub.async_PropertySet, (name, pvalue, adjust_timeout(timeout)), AsyncRequestDirectorImpl, handler, directorargs=(True, type1.Type, stub, stub.RRGetNode()))


def stub_async_functioncall(stub, name, type1, *args):
    check_member_args(name, type1.Parameters, args, True)
    m = RobotRaconteurPython.vectorptr_messageelement()
    i = 0
    for p in type1.Parameters:
        a = PackMessageElement(args[i], p, stub)
        a.ElementName = p.Name
        m.append(a)
        i += 1
    handler = args[i]
    if (len(args) > i + 1):
        timeout = args[i + 1]
    else:
        timeout = RobotRaconteurPython.RR_TIMEOUT_INFINITE
    return async_call(stub.async_FunctionCall, (name, m, adjust_timeout(timeout)), AsyncRequestDirectorImpl, handler, directorargs=(False, type1.ReturnType, stub, stub.RRGetNode()))


def stub_async_functioncallvoid(stub, name, type1, *args):
    check_member_args(name, type1.Parameters, args, True)
    m = RobotRaconteurPython.vectorptr_messageelement()
    i = 0
    for p in type1.Parameters:
        a = PackMessageElement(args[i], p, stub)
        a.ElementName = p.Name
        m.append(a)
        i += 1
    handler = args[i]
    if (len(args) > i + 1):
        timeout = args[i + 1]
    else:
        timeout = -1

    return async_call(stub.async_FunctionCall, (name, m, adjust_timeout(timeout)), AsyncRequestDirectorImpl, handler, directorargs=(True, type1.ReturnType, stub, stub.RRGetNode()))


def stub_async_functioncallgenerator(stub, name, type1, *args):
    check_member_args(name, type1.Parameters, args, True)
    m = RobotRaconteurPython.vectorptr_messageelement()
    i = 0
    param_type = None
    for p in type1.Parameters:
        if (p.ContainerType != RobotRaconteurPython.DataTypes_ContainerTypes_generator):
            a = PackMessageElement(args[i], p, stub)
            a.ElementName = p.Name
            m.append(a)
            i += 1
        else:
            param_type = p
    handler = args[i]
    if (len(args) > i + 1):
        timeout = args[i + 1]
    else:
        timeout = RobotRaconteurPython.RR_TIMEOUT_INFINITE
    return async_call(stub.async_GeneratorFunctionCall, (name, m, adjust_timeout(timeout)), AsyncGeneratorClientReturnDirectorImpl, handler, directorargs=(type1.ReturnType, param_type, stub, stub.RRGetNode()))


_stub_objref_lock = threading.Lock()


def stub_objref(stub, name, index=None):
    if (index is None):
        s = stub.FindObjRef(name)
        with _stub_objref_lock:
            s2 = s.GetPyStub()
            if (s2 is not None):
                return s2
            return InitStub(s)

        return
    else:
        s = stub.FindObjRef(name, str(index))
        with _stub_objref_lock:
            s2 = s.GetPyStub()
            if (s2 is not None):
                return s2
            return InitStub(s)


def stub_async_objref(stub, name, index, handler, timeout=-1):
    if (index is None):
        return async_call(stub.async_FindObjRef, (name, adjust_timeout(timeout)), AsyncStubReturnDirectorImpl, handler)
    else:
        return async_call(stub.async_FindObjRef, (name, str(index), adjust_timeout(timeout)), AsyncStubReturnDirectorImpl, handler)


class WrappedServiceStubDirectorPython(RobotRaconteurPython.WrappedServiceStubDirector):
    def __init__(self, stub, innerstub):
        self.stub = stub
        self.innerstub = innerstub
        super(WrappedServiceStubDirectorPython, self).__init__()

    def DispatchEvent(self, name, args1):
        try:
            type1 = FindMemberByName(
                self.innerstub.RR_objecttype.Members, name)
            # type1=[e for e in self.innerstub.RR_objecttype.Members if e.Name == name][0]
            args = []
            type2 = RobotRaconteurPython.MemberDefinitionUtil.ToEvent(type1)
            for p in type2.Parameters:
                m = FindMessageElementByName(args1, p.Name)
                a = UnpackMessageElement(
                    m, p, self.stub, node=self.innerstub.RRGetNode())
                args.append(a)
            getattr(self.stub, name).fire(*args)
        except:
            traceback.print_exc()

    def CallbackCall(self, name, args1):
        try:
            type1 = FindMemberByName(
                self.innerstub.RR_objecttype.Members, name)
            # type1=[e for e in self.innerstub.RR_objecttype.Members if e.Name == name][0]
            args = []

            type2 = RobotRaconteurPython.MemberDefinitionUtil.ToCallback(type1)
            for p in type2.Parameters:
                m = FindMessageElementByName(args1, p.Name)

                a = UnpackMessageElement(
                    m, p, self.stub, self.innerstub.RRGetNode())
                args.append(a)
            ret = getattr(self.stub, name).Function(*args)

            if (ret is None):
                m = RobotRaconteurPython.MessageElement()
                m.ElementName = "return"
                m.ElementType = RobotRaconteurPython.DataTypes_void_t
                return m
            return PackMessageElement(ret, type2.ReturnType, self.stub, self.innerstub.RRGetNode())
        except:
            traceback.print_exc()


class ServiceStub(object):
    __slots__ = ["rrinnerstub", "rrlock", "__weakref__"]

    def RRGetNode(self):
        return self.rrinnerstub.RRGetNode()


class CallbackClient(object):
    __slots__ = ["Function", "__weakref__"]

    def __init__(self):
        self.Function = None


class PipeEndpoint(object):
    """
    PipeEndpoint()

    Pipe endpoint used to transmit reliable or unreliable data streams

    Pipe endpoints are used to communicate data between connected pipe members.
    See Pipe for more information on pipe members.

    Pipe endpoints are created by clients using the Pipe.Connect() or Pipe.AsyncConnect()
    functions. Services receive incoming pipe endpoint connection requests through a
    callback function specified using the Pipe.PipeConnectCallback property. Services
    may also use the PipeBroadcaster class to automate managing pipe endpoint lifecycles and
    sending packets to all connected client endpoints.

    Pipe endpoints are *indexed*, meaning that more than one pipe endpoint pair can be created
    using the same member. This means that multiple data streams can be created independent of
    each other between the client and service using the same member.

    Pipes send reliable packet streams between connected client/service endpoint pairs.
    Packets are sent using the SendPacket() or AsyncSendPacket() functions. Packets
    are read from the receive queue using the ReceivePacket(), ReceivePacketWait(),
    TryReceivePacketWait(), TryReceivePacketWait(), or PeekNextPacket(). The endpoint is closed
    using the Close() or AsyncClose() function.

    This class is instantiated by the Pipe class. It should not be instantiated
    by the user.
    """
    __slots__ = ["__innerpipe", "__type", "PipeEndpointClosedCallback",
                 "_PacketReceivedEvent", "_PacketAckReceivedEvent", "__obj", "__weakref__"]

    def __init__(self, innerpipe, type, obj=None):
        self.__innerpipe = innerpipe
        self.__type = type
        self.PipeEndpointClosedCallback = None
        """
        (Callable[[RobotRaconteur.PipeEndpoint],None]) The function to invoke when the pipe endpoint has been closed.
        """
        self._PacketReceivedEvent = EventHook()
        self._PacketAckReceivedEvent = EventHook()
        self.__obj = obj

    @property
    def Index(self):
        """
        The pipe endpoint index used when endpoint connected

        :rtype: int
        """
        return self.__innerpipe.GetIndex()

    @property
    def Endpoint(self):
        """
        the endpoint associated with the ClientContext or ServerEndpoint
        associated with the pipe endpoint.

        :rtype: int
        """
        return self.__innerpipe.GetEndpoint()

    @property
    def Available(self):
        """
        Return number of packets in the receive queue

        Invalid for *writeonly* pipes.

        :rtype: int
        """
        return self.__innerpipe.Available()

    @property
    def IsUnreliable(self):
        """
        Get if pipe endpoint is unreliable

        Pipe members may be declared as *unreliable* using member modifiers in the
        service definition. Pipes confirm unreliable operation when pipe endpoints are connected.

        :rtype: bool
        """
        return self.__innerpipe.IsUnreliable()

    @property
    def Direction(self):
        """
        The direction of the pipe

        Pipes may be declared "readonly" or "writeonly" in the service definition file. (If neither
        is specified, the pipe is assumed to be full duplex.) "readonly" pipes may only send packets from
        service to client. "writeonly" pipes may only send packets from client to service.

        See ``MemberDefinition_Direction`` constants for possible return values.

        :rtype: int
        """
        return self.__innerpipe.Direction()

    @property
    def RequestPacketAck(self):
        """
        Get if pipe endpoint should request packet acks

        Packet acks are generated by receiving endpoints to inform the sender that
        a packet has been received. The ack contains the packet index, the sequence number
        of the packet. Packet acks are used for flow control by PipeBroadcaster.

        :rtype: bool
        """
        return self.__innerpipe.GetRequestPacketAck()

    @RequestPacketAck.setter
    def RequestPacketAck(self, value):
        self.__innerpipe.SetRequestPacketAck(value)

    @property
    def IgnoreReceived(self):
        """
        Set if pipe endpoint is ignoring incoming packets

        If true, pipe endpoint is ignoring incoming packets and is not adding
        incoming packets to the receive queue.

        :rtype: bool
        """
        return self.__innerpipe.GetIgnoreReceived()

    @IgnoreReceived.setter
    def IgnoreReceived(self, value):
        self.__innerpipe.SetIgnoreReceived(value)

    def Close(self):
        """
        Close the pipe endpoint

        Close the pipe endpoint. Blocks until close complete. The peer endpoint is destroyed
        automatically.
        """
        return self.__innerpipe.Close()

    def AsyncClose(self, handler, timeout=2):
        """
        Asynchronously close the pipe endpoint

        Same as Close() but returns asynchronously

        If ``handler`` is None, returns an awaitable future.

        :param handler: A handler function to call on completion, possibly with an exception
        :type handler: Callable[[Exception],None]
        :param timeout: Timeout in seconds, or -1 for no timeout
        :type timeout: float
        """
        return async_call(self.__innerpipe.AsyncClose, (adjust_timeout(timeout),), AsyncVoidReturnDirectorImpl, handler)

    def SendPacket(self, packet):
        """
        Sends a packet to the peer endpoint

        Sends a packet to the peer endpoint. If the pipe is reliable, the packetsare  guaranteed to arrive
        in order. If the pipe is set to unreliable, "best effort" is made to deliver packets, and they are not
        guaranteed to arrive in order. This function will block until the packet has been transmitted by the
        transport. It will return before the peer endpoint has received the packet.

        :param packet: The packet to send
        :return: The packet number of the sent packet
        :rtype: int
        """
        m = PackMessageElement(packet, self.__type,
                               self.__obj, self.__innerpipe.GetNode())
        return self.__innerpipe.SendPacket(m)

    def AsyncSendPacket(self, packet, handler):
        """
        Send a packet to the peer endpoint asynchronously

        Same as SendPacket(), but returns asynchronously.

        If ``handler`` is None, returns an awaitable future.

        :param packet: The packet to send
        :param handler: A handler function to receive the sent packet number or an exception
        :type handler: Callable[[Exception],None]
        """
        m = PackMessageElement(packet, self.__type,
                               self.__obj, self.__innerpipe.GetNode())
        return async_call(self.__innerpipe.AsyncSendPacket, (m,), AsyncUInt32ReturnDirectorImpl, handler)

    def ReceivePacket(self):
        """
        Receive the next packet in the receive queue

        Receive the next packet from the receive queue. This function will throw an
        InvalidOperationException if there are no packets in the receive queue. Use
        ReceivePacketWait() to block until a packet has been received.

        :return: The received packet
        """
        m = self.__innerpipe.ReceivePacket()
        return UnpackMessageElement(m, self.__type, self.__obj, self.__innerpipe.GetNode())

    def PeekNextPacket(self):
        """
        Peeks the next packet in the receive queue

        Returns the first packet in the receive queue, but does not remove it from
        the queue. Throws an InvalidOperationException if there are no packets in the
        receive queue.

        :return: The next packet in the receive queue
        """
        m = self.__innerpipe.PeekNextPacket()
        return UnpackMessageElement(m, self.__type, self.__obj, self.__innerpipe.GetNode())

    def ReceivePacketWait(self, timeout=RobotRaconteurPython.RR_TIMEOUT_INFINITE):
        """
        Receive the next packet in the receive queue, block if queue is empty

        Same as ReceivePacket(), but blocks if queue is empty

        :param timeout: Timeout in seconds to wait for a packet, or -1 for infinite
        :type timeout: float
        :return: The received packet
        """
        m = self.__innerpipe.ReceivePacketWait(adjust_timeout(timeout))
        return UnpackMessageElement(m, self.__type, self.__obj, self.__innerpipe.GetNode())

    def PeekNextPacketWait(self, timeout=RobotRaconteurPython.RR_TIMEOUT_INFINITE):
        """
        Peek the next packet in the receive queue, block if queue is empty

        Same as PeekPacket(), but blocks if queue is empty

        :param timeout: Timeout in seconds to wait for a packet, or -1 for infinite
        :return: The received packet
        """
        m = self.__innerpipe.PeekNextPacketWait(adjust_timeout(timeout))
        return UnpackMessageElement(m, self.__type, self.__obj, self.__innerpipe.GetNode())

    def TryReceivePacketWait(self, timeout=RobotRaconteurPython.RR_TIMEOUT_INFINITE, peek=False):
        """
        Try receiving a packet, optionally blocking if the queue is empty

        Try receiving a packet with various options. Returns True if a packet has been
        received, or False if no packet is available instead of throwing an exception on failure.
        The timeout and peek parameters can be used to modify behavior to provide functionality
        similar to the various Receive and Peek functions.

        :param timeout: The timeout in seconds. Set to zero for non-blocking operation, an arbitrary value
          in seconds for a finite duration timeout, or -1 for infinite
        :type timeout: float
        :param peek: If true, the packet is not removed from the receive queue
        :type peek: bool
        :return: Tuple of success and received packet
        :rtype: Tuple[bool,T]
        """
        m = RobotRaconteurPython.MessageElement()
        r = self.__innerpipe.TryReceivePacketWait(
            m, adjust_timeout(timeout), peek)
        return (r, UnpackMessageElement(m, self.__type, self.__obj, self.__innerpipe.GetNode()))

    @property
    def PacketReceivedEvent(self):
        """
        Event hook for received packets. Use to add handlers to be called
        when packets are received by the endpoint.

        .. code-block:: python

           def my_handler(ep):
              # Receive packets
              while ep.Available > 0:
                  packet = ep.ReceivePacket()
                  # Do something with packet

           my_endpoint.PacketReceivedEvent += my_handler

        Handler must have signature ``Callable[[RobotRaconteur.PipeEndpoint],None]``

        """

        return self._PacketReceivedEvent

    @PacketReceivedEvent.setter
    def PacketReceivedEvent(self, evt):
        if (evt is not self._PacketReceivedEvent):
            raise RuntimeError("Invalid operation")

    @property
    def PacketAckReceivedEvent(self):
        """
        Event hook for received packets. Use to add handlers to be called
        when packets are received by the endpoint.

        .. code-block:: python

           def my_ack_handler(ep, packet_num):
              # Do something with packet_num info
              pass

           my_endpoint.PacketAckReceivedEvent += my_ack_handler

        Handler must have signature ``Callable[[RobotRaconteur.PipeEndpoint,T],None]``

        """
        return self._PacketAckReceivedEvent

    @PacketAckReceivedEvent.setter
    def PacketAckReceivedEvent(self, evt):
        if (evt is not self._PacketAckReceivedEvent):
            raise RuntimeError("Invalid operation")

    def GetNode(self):
        return self.__innerpipe.GetNode()


class PipeEndpointDirector(RobotRaconteurPython.WrappedPipeEndpointDirector):
    def __init__(self, endpoint):
        self.__endpoint = endpoint
        super(PipeEndpointDirector, self).__init__()

    def PipeEndpointClosedCallback(self):

        if (not self.__endpoint.PipeEndpointClosedCallback is None):
            self.__endpoint.PipeEndpointClosedCallback(self.__endpoint)

    def PacketReceivedEvent(self):

        self.__endpoint.PacketReceivedEvent.fire(self.__endpoint)

    def PacketAckReceivedEvent(self, packetnum):

        self.__endpoint.PacketAckReceivedEvent.fire(self.__endpoint, packetnum)


class PipeAsyncConnectHandlerImpl(RobotRaconteurPython.AsyncPipeEndpointReturnDirector):
    def __init__(self, handler, innerpipe, obj):
        super(PipeAsyncConnectHandlerImpl, self).__init__()
        self._handler = handler
        self.__innerpipe = innerpipe
        self.__obj = obj

    def handler(self, innerendpoint, error_info):
        if (error_info.error_code != 0):
            err = RobotRaconteurPythonError.RobotRaconteurExceptionUtil.ErrorInfoToException(
                error_info)
            self._handler(None, err)
            return
        try:
            outerendpoint = PipeEndpoint(
                innerendpoint, self.__innerpipe.Type, self.__obj)
            director = PipeEndpointDirector(outerendpoint)
            innerendpoint.SetRRDirector(director, 0)
            director.__disown__()
        except Exception as err2:
            self._handler(None, err2)
            return
        self._handler(outerendpoint, None)


class Pipe(object):
    """
    Pipe()

    "pipe" member type interface

    The Pipe class implements the "pipe" member type. Pipes are declared in service definition files
    using the "pipe" keyword within object declarations. Pipes provide reliable packet streaming between
    clients and services. They work by creating pipe endpoint pairs (peers), with one endpoint in the client,
    and one in the service. Packets are transmitted between endpoint pairs. Packets sent by one endpoint are received
    by the other, where they are placed in a receive queue. Received packets can then be retrieved from the receive queue.

    Pipe endpoints are created by the client using the Connect() or AsyncConnect() functions. Services receive
    incoming connection requests through a callback function. This callback is configured using the PipeConnectCallback
    property. Services may also use the PipeBroadcaster class to automate managing pipe endpoint lifecycles and
    sending packets to all connected client endpoints. If the PipeConnectCallback function is used, the service
    is responsible for keeping track of endpoints as the connect and disconnect. See PipeEndpoint for details
    on sending and receiving packets.

    Pipe endpoints are *indexed*, meaning that more than one endpoint pair can be created between the client and the service.

    Pipes may be *unreliable*, meaning that packets may arrive out of order or be dropped. Use IsUnreliable to check for
    unreliable pipes. The member modifier `unreliable` is used to specify that a pipe should be unreliable.

    Pipes may be declared *readonly* or *writeonly*. If neither is specified, the pipe is assumed to be full duplex. *readonly*
    pipes may only send packets from service to client. *writeonly* pipes may only send packets from client to service. Use
    Direction to determine the direction of the pipe.

    The PipeBroadcaster is often used to simplify the use of Pipes. See PipeBroadcaster for more information.

    This class is instantiated by the node. It should not be instantiated by the user.
    """
    __slots__ = ["_innerpipe", "_obj", "__weakref__"]

    def __init__(self, innerpipe, obj=None):
        self._innerpipe = innerpipe
        self._obj = obj

    def Connect(self, index):
        """
        Connect a pipe endpoint

        Creates a connected pipe endpoint pair, and returns the local endpoint. Use to create the streaming data
        connection to the service. Pipe endpoints are indexed, meaning that Connect() may be called multiple
        times for the same client connection to create multple pipe endpoint pairs. For most cases Pipe.ANY_INDEX
        (-1) can be used to automatically select an available index.

        Only valid on clients. Will throw InvalidOperationException on the service side.

        :param index: The index of the pipe endpoint, or -1 to automatically select an index
        :type index: int
        :return: The connected pipe endpoint
        :rtype: RobotRaconteur.PipeEndpoint
        """
        innerendpoint = self._innerpipe.Connect(index)
        outerendpoint = PipeEndpoint(
            innerendpoint, self._innerpipe.Type, self._obj)
        director = PipeEndpointDirector(outerendpoint)
        innerendpoint.SetRRDirector(director, 0)
        director.__disown__()
        return outerendpoint

    def AsyncConnect(self, *args):
        """
        AsyncConnect(index,handler,timeout=-1)

        Asynchronously connect a pipe endpoint.

        Same as Connect(), but returns asynchronously.

        Only valid on clients. Will throw InvalidOperationException on the service side.

        If ``handler`` is None, returns an awaitable future.

        :param index: The index of the pipe endpoint, or -1 to automatically select an index
        :type index: int
        :param handler: A handler function to receive the connected endpoint, or an exception
        :type handler: Callable[[PipeEndpoint,Exception],None]
        :param timeout: Timeout in seconds, or -1 for no timeout
        """
        if (isinstance(args[0], numbers.Number)):
            index = args[0]
            handler = args[1]
            if (len(args) >= 3):
                timeout = args[2]
            else:
                timeout = RobotRaconteurPython.RR_TIMEOUT_INFINITE
        else:
            index = -1
            handler = args[0]
            if (len(args) >= 2):
                timeout = args[1]
            else:
                timeout = RobotRaconteurPython.RR_TIMEOUT_INFINITE

        return async_call(self._innerpipe.AsyncConnect, (index, adjust_timeout(timeout)), PipeAsyncConnectHandlerImpl, handler, directorargs=(self._innerpipe, self._obj))

    @property
    def MemberName(self):
        """
        Get the member name of the pipe

        :rtype: str
        """
        return self._innerpipe.GetMemberName()

    @property
    def Direction(self):
        """
        The direction of the pipe

        Pipes may be declared "readonly" or "writeonly" in the service definition file. (If neither
        is specified, the pipe is assumed to be full duplex.) "readonly" pipes may only send packets from
        service to client. "writeonly" pipes may only send packets from client to service.

        See ``MemberDefinition_Direction`` constants for possible return values.

        :rtype: int
        """
        return self._innerpipe.Direction()

    @property
    def PipeConnectCallback(self):
        """
        Set the pipe endpoint connected callback function

        Callback function invoked when a client attempts to connect a pipe endpoint. The callback
        will receive the incoming pipe endpoint as a parameter. The service must maintain a reference to the
        pipe endpoint, but the pipe will retain ownership of the endpoint until it is closed.

        The callback may throw an exception to reject incoming connect request.

        Note: Connect callback is configured automatically by PipeBroadcaster

        Only valid for services. Will throw InvalidOperationException on the client side.

        :rtype: Callable[[PipeEndpoint],None]
        """
        raise Exception("Read only property")

    @PipeConnectCallback.setter
    def PipeConnectCallback(self, c):
        wrappedp = WrappedPipeServerConnectDirectorPython(
            self, self._innerpipe.Type, c)
        self._innerpipe.SetWrappedPipeConnectCallback(wrappedp, 0)
        wrappedp.__disown__()

    def GetNode(self):
        return self._innerpipe.GetNode()


class WrappedPipeServerConnectDirectorPython(RobotRaconteurPython.WrappedPipeServerConnectDirector):
    def __init__(self, pipe, type, callback):
        self.pipe = pipe
        self.type = type
        self.callback = callback
        super(WrappedPipeServerConnectDirectorPython, self).__init__()

    def PipeConnectCallback(self, innerendpoint):

        outerendpoint = PipeEndpoint(innerendpoint, self.type)
        director = PipeEndpointDirector(outerendpoint)
        innerendpoint.SetRRDirector(director, 0)
        director.__disown__()
        self.callback(outerendpoint)


class WireConnection(object):
    """
    WireConnection()

    Wire connection used to transmit "most recent" values

    Wire connections are used to transmit "most recent" values between connected
    wire members. See Wire for more information on wire members.

    Wire connections are created by clients using the Wire.Connect() or Wire.AsyncConnect()
    functions. Services receive incoming wire connection requests through a
    callback function specified using the Wire.WireConnectCallback property. Services
    may also use the WireBroadcaster class to automate managing wire connection lifecycles and
    sending values to all connected clients, or use WireUnicastReceiver to receive an incoming
    value from the most recently connected client.

    Wire connections are used to transmit "most recent" values between clients and services. Connection
    the wire creates a connection pair, one in the client, and one in the service. Each wire connection
    object has an InValue and an OutValue. Setting the OutValue of one will cause the specified value to
    be transmitted to the InValue of the peer. See Wire for more information.

    Values can optionally be specified to have a finite lifespan using InValueLifespan and
    OutValueLifespan. Lifespans can be used to prevent using old values that have
    not been recently updated.

    This class is instantiated by the Wire class. It should not be instantiated
    by the user.
    """
    __slots__ = ["__innerwire", "__type", "WireConnectionClosedCallback",
                 "_WireValueChanged", "__obj", "__weakref__"]

    def __init__(self, innerwire, type, obj=None):
        self.__innerwire = innerwire
        self.__type = type
        self.WireConnectionClosedCallback = None
        self._WireValueChanged = EventHook()
        self.__obj = obj

    @property
    def Endpoint(self):
        """
        Get the Robot Raconteur node Endpoint ID

        Gets the endpoint associated with the ClientContext or ServerEndpoint
        associated with the wire connection.

        :rtype: int
        """
        return self.__innerwire.GetEndpoint()

    @property
    def Direction(self):
        """
        The direction of the wire

        Wires may be declared "readonly" or "writeonly" in the service definition file. (If neither
        is specified, the wire is assumed to be full duplex.) "readonly" wires may only send packets from
        service to client. "writeonly" wires may only send packets from client to service.

        See ``MemberDefinition_Direction`` constants for possible return values.

        :rtype: int
        """
        return self.__innerwire.Direction()

    def Close(self):
        """
        Close the wire connection

        Close the wire connection. Blocks until close complete. The peer wire connection
        is destroyed automatically.
        """
        return self.__innerwire.Close()

    def AsyncClose(self, handler, timeout=2):
        """
        Asynchronously close the wire connection

        Same as Close() but returns asynchronously

        :param handler: A handler function to call on completion, possibly with an exception
        :type handler: Callable[[Exception],None]
        :param timeout: Timeout in seconds, or -1 for infinite
        :type timeout: float
        """
        return async_call(self.__innerwire.AsyncClose, (adjust_timeout(timeout),), AsyncVoidReturnDirectorImpl, handler)

    @property
    def InValue(self):
        """
        Get the current InValue

        Gets the current InValue that was transmitted from the peer. Throws
        ValueNotSetException if no value has been received, or the most
        recent value lifespan has expired.
        """
        m = self.__innerwire.GetInValue()
        return UnpackMessageElement(m, self.__type, self.__obj, self.__innerwire.GetNode())

    @property
    def OutValue(self):
        """
        Set the OutValue and transmit to the peer connection

        Sets the OutValue for the wire connection. The specified value will be
        transmitted to the peer, and will become the peers InValue. The transmission
        is unreliable, meaning that values may be dropped if newer values arrive.

        The most recent OutValue may also be read through this property.
        """
        m = self.__innerwire.GetOutValue()
        return UnpackMessageElement(m, self.__type, self.__obj, self.__innerwire.GetNode())

    @OutValue.setter
    def OutValue(self, value):
        m = PackMessageElement(
            value, self.__type, self.__obj, self.__innerwire.GetNode())
        return self.__innerwire.SetOutValue(m)

    @property
    def LastValueReceivedTime(self):
        """
        Get the timestamp of the last received value

        Returns the timestamp of the value in the *senders* clock

        :rtype: RobotRaconteur.TimeSpec
        """
        return self.__innerwire.GetLastValueReceivedTime()

    @property
    def LastValueSentTime(self):
        """
        Get the timestamp of the last sent value

        Returns the timestamp of the last sent value in the *local* clock

        :rtype: RobotRaconteur.TimeSpec
        """
        return self.__innerwire.GetLastValueSentTime()

    @property
    def InValueValid(self):
        """
        Get if the InValue is valid

        The InValue is valid if a value has been received and
        the value has not expired

        :rtype: bool
        """
        return self.__innerwire.GetInValueValid()

    @property
    def OutValueValid(self):
        """
        Get if the OutValue is valid

        The OutValue is valid if a value has been
        set using the OutValue property

        :rtype: bool
        """
        return self.__innerwire.GetOutValueValid()

    @property
    def IgnoreInValue(self):
        """
        Set whether wire connection should ignore incoming values

        Wire connections may optionally desire to ignore incoming values. This is useful if the connection
        is only being used to send out values, and received values may create a potential memory . If ignore is true,
        incoming values will be discarded.

        :rtype: bool
        """
        return self.__innerwire.GetIgnoreInValue()

    @IgnoreInValue.setter
    def IgnoreInValue(self, value):
        self.__innerwire.SetIgnoreInValue(value)

    def TryGetInValue(self):
        """
        Try getting the InValue, returning true on success or false on failure

        Get the current InValue and InValue timestamp. Return true or false on
        success or failure instead of throwing exception.

        :return: Tuple of success, in value, and timespec
        :rtype: Tuple[bool,T,RobotRaconteur.TimeSpec]
        """
        res = self.__innerwire.TryGetInValue()
        if not res.res:
            return (False, None, None)
        return (True, UnpackMessageElement(res.value, self.__type, self.__obj, self.__innerwire.GetNode()), res.ts)

    def TryGetOutValue(self):
        """
        Try getting the OutValue, returning true on success or false on failure

        Get the current OutValue and OutValue timestamp. Return true or false on
        success and failure instead of throwing exception.

        :return: Tuple of success, out value, and timespec
        :rtype: Tuple[bool,T,RobotRaconteur.TimeSpec]
        """
        res = self.__innerwire.TryGetOutValue()
        if not res.res:
            return (False, None, None)
        return (True, UnpackMessageElement(res.value, self.__type, self.__obj, self.__innerwire.GetNode()), res.ts)

    def WaitInValueValid(self, timeout=RobotRaconteurPython.RR_TIMEOUT_INFINITE):
        """
        Waits for InValue to be valid

        Blocks the current thread until InValue is valid,
        with an optional timeout. Returns true if InValue is valid,
        or false if timeout occurred.

        :param timeout: Timeout in seconds, or -1 for infinite
        :return: Value is valid
        :rtype: bool
        """
        return self.__innerwire.WaitInValueValid(adjust_timeout(timeout))

    def WaitOutValueValid(self, timeout=RobotRaconteurPython.RR_TIMEOUT_INFINITE):
        """
        Waits for OutValue to be valid

        Blocks the current thread until OutValue is valid,
        with an optional timeout. Returns true if OutValue is valid,
        or false if timeout occurred.

        :param timeout: Timeout in seconds, or -1 for infinite
        :return: Value is valid
        :rtype: bool
        """
        return self.__innerwire.WaitOutValueValid(adjust_timeout(timeout))

    @property
    def WireValueChanged(self):
        """
        Event hook for wire value change. Use to add handlers to be called
        when the InValue changes.

        .. code-block:: python

           def my_handler(con, value, ts):
              # Handle new value
              pass

           my_wire_connection.WireValueChanged += my_handler

        Handler must have signature ``Callable[[RobotRaconteur.WireConnection,T,RobotRaconteur.TimeSpec],None]``

        :rtype: RobotRaconteur.EventHook

        """
        return self._WireValueChanged

    @WireValueChanged.setter
    def WireValueChanged(self, evt):
        if (evt is not self._WireValueChanged):
            raise RuntimeError("Invalid operation")

    @property
    def InValueLifespan(self):
        """
        Set the lifespan of InValue

        InValue may optionally have a finite lifespan specified in seconds. Once
        the lifespan after reception has expired, the InValue is cleared and becomes invalid.
        Attempts to access InValue will result in ValueNotSetException.

        InValue lifespans may be used to avoid using a stale value received by the wire. If
        the lifespan is not set, the wire will continue to return the last received value, even
        if the value is old.

        Specify -1 for infinite lifespan.

        :rtype: float
        """
        t = self.__innerwire.GetInValueLifespan()
        if t < 0:
            return t
        return float(t) / 1000.0

    @InValueLifespan.setter
    def InValueLifespan(self, secs):
        if secs < 0:
            self.__innerwire.SetInValueLifespan(-1)
        else:
            self.__innerwire.SetInValueLifespan(int(secs * 1000.0))

    @property
    def OutValueLifespan(self):
        """
        Set the lifespan of OutValue

        OutValue may optionally have a finite lifespan specified in seconds. Once
        the lifespan after sending has expired, the OutValue is cleared and becomes invalid.
        Attempts to access OutValue will result in ValueNotSetException.

        OutValue lifespans may be used to avoid using a stale value sent by the wire. If
        the lifespan is not set, the wire will continue to return the last sent value, even
        if the value is old.

        Specify -1 for infinite lifespan.

        :rtype: float
        """
        t = self.__innerwire.GetOutValueLifespan()
        if t < 0:
            return t
        return float(t) / 1000.0

    @OutValueLifespan.setter
    def OutValueLifespan(self, secs):
        if secs < 0:
            self.__innerwire.SetOutValueLifespan(-1)
        else:
            self.__innerwire.SetOutValueLifespan(int(secs * 1000.0))

    def GetNode(self):
        return self.__innerwire.GetNode()


class WireConnectionDirector(RobotRaconteurPython.WrappedWireConnectionDirector):

    def __init__(self, endpoint, type, obj=None, innerep=None):
        self.__endpoint = endpoint
        self.__type = type
        self.__obj = obj
        self.__innerep = innerep
        super(WireConnectionDirector, self).__init__()

    def WireValueChanged(self, value, time):

        value2 = UnpackMessageElement(
            value, self.__type, self.__obj, self.__innerep.GetNode())
        self.__endpoint.WireValueChanged.fire(self.__endpoint, value2, time)

    def WireConnectionClosedCallback(self):

        if (not self.__endpoint.WireConnectionClosedCallback is None):
            self.__endpoint.WireConnectionClosedCallback(self.__endpoint)


class WireAsyncConnectHandlerImpl(RobotRaconteurPython.AsyncWireConnectionReturnDirector):
    def __init__(self, handler, innerpipe, obj):
        super(WireAsyncConnectHandlerImpl, self).__init__()
        self._handler = handler
        self.__innerpipe = innerpipe
        self.__obj = obj

    def handler(self, innerendpoint, error_info):
        if (error_info.error_code != 0):
            err = RobotRaconteurPythonError.RobotRaconteurExceptionUtil.ErrorInfoToException(
                error_info)
            self._handler(None, err)
            return
        try:
            outerendpoint = WireConnection(
                innerendpoint, self.__innerpipe.Type, self.__obj)
            director = WireConnectionDirector(
                outerendpoint, self.__innerpipe.Type, self.__obj, innerendpoint)
            innerendpoint.SetRRDirector(director, 0)
            director.__disown__()
        except Exception as err2:
            self._handler(None, err2)
            return
        self._handler(outerendpoint, None)


class WireAsyncPeekReturnDirectorImpl(RobotRaconteurPython.AsyncWirePeekReturnDirector):
    def __init__(self, handler, innerpipe, obj):
        super(WireAsyncPeekReturnDirectorImpl, self).__init__()
        self._handler = handler
        self.__innerpipe = innerpipe
        self.__obj = obj

    def handler(self, m, ts, error_info):
        if (error_info.error_code != 0):
            err = RobotRaconteurPythonError.RobotRaconteurExceptionUtil.ErrorInfoToException(
                error_info)
            self._handler((None, None), err)
            return
        value = UnpackMessageElement(
            m, self.__innerpipe.Type, self.__obj, self.__innerpipe.GetNode())
        self._handler((value, ts), None)


class WrappedWireServerPeekValueDirectorImpl(RobotRaconteurPython.WrappedWireServerPeekValueDirector):
    def __init__(self, cb, innerpipe, obj):
        super(WrappedWireServerPeekValueDirectorImpl, self).__init__()
        self._cb = cb
        self.__innerpipe = innerpipe
        self.__obj = obj

    def PeekValue(self, ep):
        value = self._cb(ep)
        m = PackMessageElement(value, self.__innerpipe.Type,
                               self.__obj, self.__innerpipe.GetNode())
        return m


class WrappedWireServerPokeValueDirectorImpl(RobotRaconteurPython.WrappedWireServerPokeValueDirector):
    def __init__(self, cb, innerpipe, obj):
        super(WrappedWireServerPokeValueDirectorImpl, self).__init__()
        self._cb = cb
        self.__innerpipe = innerpipe
        self.__obj = obj

    def PokeValue(self, m, ts, ep):
        value = UnpackMessageElement(
            m, self.__innerpipe.Type, self.__obj, self.__innerpipe.GetNode())
        self._cb(value, ts, ep)


class Wire(object):
    """
    Wire()

    \"wire\" member type interface

    The Wire class implements the \"wire\" member type. Wires are declared in service definition files
    using the \"wire\" keyword within object declarations. Wires provide "most recent" value streaming
    between clients and services. They work by creating "connection" pairs between the client and service.
    The wire streams the current value between the wire connection pairs using packets. Wires
    are unreliable; only the most recent value is of interest, and any older values
    will be dropped. Wire connections have an InValue and an OutValue. Users set the OutValue on the
    connection. The new OutValue is transmitted to the peer wire connection, and becomes the peer's
    InValue. The peer can then read the InValue. The client and service have their own InValue
    and OutValue, meaning that each direction, client to service or service to client, has its own
    value.

    Wire connections are created using the Connect() or AsyncConnect() functions. Services receive
    incoming connection requests through a callback function. Thes callback is configured using
    the SetWireConnectCallback() function. Services may also use the WireBroadcaster class
    or WireUnicastReceiver class to automate managing wire connection lifecycles. WireBroadcaster
    is used to send values to all connected clients. WireUnicastReceiver is used to receive the
    value from the most recent wire connection. See WireConnection for details on sending
    and receiving streaming values.

    Wire clients may also optionally "peek" and "poke" the wire without forming a streaming
    connection. This is useful if the client needs to read the InValue or set the OutValue
    instantaniously, but does not need continuous updating. PeekInValue() or
    AsyncPeekInValue() will retrieve the client's current InValue. PokeOutValue() or
    AsyncPokeOutValue() will send a new client OutValue to the service.
    PeekOutValue() or AsyncPeekOutValue() will retrieve the last client OutValue received by
    the service.

    "Peek" and "poke" operations initiated by the client are received on the service using
    callbacks. Use PeekInValueCallback, PeekOutValueCallback,
    and PokeOutValueCallback to configure the callbacks to handle these requests.
    WireBroadcaster and WireUnicastReceiver configure these callbacks automatically, so
    the user does not need to configure the callbacks when these classes are used.

    Wires can be declared *readonly* or *writeonly*. If neither is specified, the wire is assumed
    to be full duplex. *readonly* pipes may only send values from service to client, ie OutValue
    on service side and InValue on client side. *writeonly* pipes may only send values from
    client to service, ie OutValue on client side and InValue on service side. Use Direction()
    to determine the direction of the wire.

    Unlike pipes, wire connections are not indexed, so only one connection pair can be
    created per client connection.

    WireBroadcaster or WireUnicastReceiver are typically used to simplify using wires.
    See WireBroadcaster and WireUnicastReceiver for more information.

    This class is instantiated by the node. It should not be instantiated by the user.
    """
    __slots__ = ["_innerpipe", "_obj", "__weakref__"]

    def __init__(self, innerpipe, obj=None):
        self._innerpipe = innerpipe
        self._obj = obj

    def Connect(self):
        """
        Connect the wire

        Creates a connection between the wire, returning the client connection. Used to create
        a "most recent" value streaming connection to the service.

        Only valid on clients. Will throw InvalidOperationException on the service side.

        Note: If a streaming connection is not required, use PeekInValue(), PeekOutValue(),
        or PokeOutValue() instead of creating a connection.

        :return: The wire connection
        :rtype: RobotRaconteur.WireConnection
        """
        innerendpoint = self._innerpipe.Connect()
        outerendpoint = WireConnection(
            innerendpoint, self._innerpipe.Type, self._obj)
        director = WireConnectionDirector(
            outerendpoint, self._innerpipe.Type, self._obj, innerendpoint)
        innerendpoint.SetRRDirector(director, 0)
        director.__disown__()
        return outerendpoint

    def AsyncConnect(self, handler, timeout=RobotRaconteurPython.RR_TIMEOUT_INFINITE):
        """
        Asynchronously connect the wire

        Same as Connect(), but returns asynchronously

        Only valid on clients. Will throw InvalidOperationException on the service side.

        If ``handler`` is None, returns an awaitable future.

        :param handler: A handler function to receive the wire connection, or an exception
        :type handler: Callable[[RobotRaconteur.WireConnection,Exception],None]
        :param timeout: Timeout in seconds, or -1 for infinite
        """
        return async_call(self._innerpipe.AsyncConnect, (adjust_timeout(timeout),), WireAsyncConnectHandlerImpl, handler, directorargs=(self._innerpipe, self._obj))

    @property
    def MemberName(self):
        """
        Get the member name of the wire

        :rtype: str
        """
        return self._innerpipe.GetMemberName()

    @property
    def Direction(self):
        """
        The direction of the wire

        Wires may be declared "readonly" or "writeonly" in the service definition file. (If neither
        is specified, the wire is assumed to be full duplex.) "readonly" wires may only send packets from
        service to client. "writeonly" wires may only send packets from client to service.

        See ``MemberDefinition_Direction`` constants for possible return values.

        :rtype: int
        """
        return self._innerpipe.Direction()

    @property
    def WireConnectCallback(self):
        """
        Set wire connected callback function

        Callback function invoked when a client attempts to connect a the wire. The callback
        will receive the incoming wire connection as a parameter. The service must maintain a
        reference to the wire connection, but the wire will retain ownership of the wire connection
        until it is closed. Using  boost::weak_ptr to store the reference to the connection
        is recommended.

        The callback may throw an exception to reject incoming connect request.

        Note: Connect callback is configured automatically by WireBroadcaster or
        WireUnicastReceiver

        Only valid for services. Will throw InvalidOperationException on the client side.

        :rtype: Callable[[RobotRaconteur.WireConnection],None]
        """
        raise Exception("Read only property")

    @WireConnectCallback.setter
    def WireConnectCallback(self, c):
        wrappedp = WrappedWireServerConnectDirectorPython(
            self, self._innerpipe.Type, c)
        self._innerpipe.SetWrappedWireConnectCallback(wrappedp, 0)
        wrappedp.__disown__()

    def PeekInValue(self):
        """
        Peek the current InValue

        Peeks the current InValue using a "request" instead of a streaming value. Use
        if only the instantanouse value is required.

        Peek and poke are similar to "property" members. Unlike streaming,
        peek and poke are reliable operations.

        Throws ValueNotSetException if InValue is not valid.

        Only valid on clients. Will throw InvalidOperationException on the service side.

        :rtype: Tuple[T,RobotRaconteur.TimeSpec]
        """
        ts = RobotRaconteurPython.TimeSpec()
        m = self._innerpipe.PeekInValue(ts)
        return (UnpackMessageElement(m, self._innerpipe.Type, self._obj, self._innerpipe.GetNode()), ts)

    def PeekOutValue(self):
        """
        Peek the current OutValue

        Peeks the current OutValue using a "request" instead of a streaming value. Use
        if only the instantanouse value is required.

        Peek and poke are similar to "property" members. Unlike streaming,
        peek and poke are reliable operations.

        Throws ValueNotSetException if OutValue is not valid.

        Only valid on clients. Will throw InvalidOperationException on the service side.

        :rtype: Tuple[T,RobotRaconteur.TimeSpec]
        """
        ts = RobotRaconteurPython.TimeSpec()
        m = self._innerpipe.PeekOutValue(ts)
        return (UnpackMessageElement(m, self._innerpipe.Type, self._obj, self._innerpipe.GetNode()), ts)

    def PokeOutValue(self, value):
        """
        Poke the OutValue

        Pokes the OutValue using a "request" instead of a streaming value. Use
        to update the OutValue if the value is updated infrequently.

        Peek and poke are similar to `property` members. Unlike streaming,
        peek and poke are reliable operations.

        Only valid on clients. Will throw InvalidOperationException on the service side.

        :param value: The new OutValue
        """
        m = PackMessageElement(value, self._innerpipe.Type,
                               self._obj, self._innerpipe.GetNode())
        self._innerpipe.PokeOutValue(m)

    def AsyncPeekInValue(self, handler, timeout=RobotRaconteurPython.RR_TIMEOUT_INFINITE):
        """
        Asynchronously peek the current InValue

        Same as PeekInValue(), but returns asynchronously.

        Only valid on clients. Will throw InvalidOperationException on the service side.

        :param handler: A handler function to receive the InValue and timestamp, or an exception
        :type handler: Callable[[T,RobotRaconteur.TimeSpec,Exception],None]
        :param timeout: Timeout in seconds, or -1 for infinite
        :type timeout: float
        """
        return async_call(self._innerpipe.AsyncPeekInValue, (adjust_timeout(timeout),), WireAsyncPeekReturnDirectorImpl, handler, directorargs=(self._innerpipe, self._obj))

    def AsyncPeekOutValue(self, handler, timeout=RobotRaconteurPython.RR_TIMEOUT_INFINITE):
        """
        Asynchronously peek the current OutValue

        Same as PeekOutValue(), but returns asynchronously.

        Only valid on clients. Will throw InvalidOperationException on the service side.

        :param handler: A handler function to receive the OutValue and timestamp, or an exception
        :type handler: Callable[[T,RobotRaconteur.TimeSpec,Exception],None]
        :param timeout: Timeout in seconds, or -1 for infinite
        :type timeout: float
        """
        return async_call(self._innerpipe.AsyncPeekOutValue, (adjust_timeout(timeout),), WireAsyncPeekReturnDirectorImpl, handler, directorargs=(self._innerpipe, self._obj))

    def AsyncPokeOutValue(self, value, handler, timeout=RobotRaconteurPython.RR_TIMEOUT_INFINITE):
        """
        Asynchronously poke the OutValue

        Same as PokeOutValue(), but returns asynchronously

        Only valid on clients. Will throw InvalidOperationException on the service side.

        :param handler: A handler function to invoke on completion, with possible exception
        :type handler: Callable[[Exception],None]
        :param value: The new OutValue
        :param timeout: Timeout in seconds, or -1 for no timeout
        :type timeout: float
        """
        m = PackMessageElement(value, self._innerpipe.Type,
                               self._obj, self._innerpipe.GetNode())
        return async_call(self._innerpipe.AsyncPokeOutValue, (m, adjust_timeout(timeout)), AsyncVoidReturnDirectorImpl, handler)

    @property
    def PeekInValueCallback(self):
        """
        Set the PeekInValue callback function

        Peek and poke operations are used when a streaming connection of the most recent value
        is not required. Clients initiate peek and poke operations using PeekInValue(), PeekOutValue(),
        PokeOutValue(), or their asynchronous equivalents. Services receive the peek and poke
        requests through callbacks.

        PeekInValueCallback configures the service callback for PeekInValue() requests.

        The function receives the client endpoint ID, and returns the current InValue.

        Note: Callback is configured automatically by WireBroadcaster or
        WireUnicastReceiver

        Only valid for services. Will throw InvalidOperationException on the client side.

        :rtype: Callable[[int],T]
        """
        raise Exception("Read only property")

    @PeekInValueCallback.setter
    def PeekInValueCallback(self, c):
        cb = WrappedWireServerPeekValueDirectorImpl(
            c, self._innerpipe, self._obj)
        self._innerpipe.SetPeekInValueCallback(cb, 0)
        cb.__disown__()

    @property
    def PeekOutValueCallback(self):
        """
        Set the PeekOutValue callback function

        Peek and poke operations are used when a streaming connection of the most recent value
        is not required. Clients initiate peek and poke operations using PeekInValue(), PeekOutValue(),
        PokeOutValue(), or their asynchronous equivalents. Services receive the peek and poke
        requests through callbacks.

        PeekOutValueCallback configures the service callback for PeekOutValue() requests.

        The function receives the client endpoint ID, and returns the current OutValue.

        Note: Callback is configured automatically by WireBroadcaster or
        WireUnicastReceiver

        Only valid for services. Will throw InvalidOperationException on the client side.

        :rtype: Callable[[int],T]
        """
        raise Exception("Read only property")

    @PeekOutValueCallback.setter
    def PeekOutValueCallback(self, c):
        cb = WrappedWireServerPeekValueDirectorImpl(
            c, self._innerpipe, self._obj)
        self._innerpipe.SetPeekOutValueCallback(cb, 0)
        cb.__disown__()

    @property
    def PokeOutValueCallback(self):
        """
        Set the PokeOutValue callback function

        Peek and poke operations are used when a streaming connection of the most recent value
        is not required. Clients initiate peek and poke operations using PeekInValue(), PeekOutValue(),
        PokeOutValue(), or their asynchronous equivalents. Services receive the peek and poke
        requests through callbacks.

        PokeOutValueCallback configures the service callback for PokeOutValue() requests.

        The function receives the new out value, the new out value timestamp in the client's clock,
        and the client endpoint ID.

        Note: Callback is configured automatically by WireBroadcaster or
        WireUnicastReceiver

        Only valid for services. Will throw InvalidOperationException on the client side.

        :rtype: Callable[[T,RobotRaconteur.TimeSpec,int],None]
        """
        raise Exception("Read only property")

    @PokeOutValueCallback.setter
    def PokeOutValueCallback(self, c):
        cb = WrappedWireServerPokeValueDirectorImpl(
            c, self._innerpipe, self._obj)
        self._innerpipe.SetPokeOutValueCallback(cb, 0)
        cb.__disown__()

    def GetNode(self):
        return self._innerpipe.GetNode()


class WrappedWireServerConnectDirectorPython(RobotRaconteurPython.WrappedWireServerConnectDirector):
    def __init__(self, wire, type, callback):
        self.wire = wire
        self.type = type
        self.callback = callback
        super(WrappedWireServerConnectDirectorPython, self).__init__()

    def WireConnectCallback(self, innerendpoint):

        outerendpoint = WireConnection(innerendpoint, self.type)
        director = WireConnectionDirector(
            outerendpoint, self.type, innerep=innerendpoint)
        innerendpoint.SetRRDirector(director, 0)
        director.__disown__()
        self.callback(outerendpoint)


class ArrayMemoryClient(object):
    __slots__ = ["__innermemory", "__weakref__"]

    def __init__(self, innermemory):
        self.__innermemory = innermemory

    @property
    def Length(self):
        return self.__innermemory.Length()

    def Read(self, memorypos, buffer, bufferpos, count):
        dat = RobotRaconteurPython.WrappedArrayMemoryClientUtil.Read(
            self.__innermemory, memorypos, count)
        t = RobotRaconteurPython.TypeDefinition()
        t.Type = self.__innermemory.ElementTypeID()
        t.ArrayType = RobotRaconteurPython.DataTypes_ArrayTypes_array
        t.ArrayVarLength = True
        t.ArrayLength = RobotRaconteurPython.vectorint32([0])
        buffer[bufferpos:(bufferpos + count)] = UnpackFromRRArray(dat, t)

    def Write(self, memorypos, buffer, bufferpos, count):
        t = RobotRaconteurPython.TypeDefinition()
        t.Type = self.__innermemory.ElementTypeID()
        t.ArrayType = RobotRaconteurPython.DataTypes_ArrayTypes_array
        t.ArrayVarLength = True
        t.ArrayLength = RobotRaconteurPython.vectorint32([0])
        dat = PackToRRArray(buffer[bufferpos:(bufferpos + count)], t)
        RobotRaconteurPython.WrappedArrayMemoryClientUtil.Write(
            self.__innermemory, memorypos, dat, 0, count)


class MultiDimArrayMemoryClient(object):
    __slots__ = ["__innermemory", "__weakref__"]

    def __init__(self, innermemory):
        self.__innermemory = innermemory
        import RobotRaconteur

    @property
    def DimCount(self):
        return self.__innermemory.DimCount()

    @property
    def Dimensions(self):
        return list(self.__innermemory.Dimensions())

    def Read(self, memorypos, buffer, bufferpos, count):

        dat = RobotRaconteurPython.WrappedMultiDimArrayMemoryClientUtil.Read(
            self.__innermemory, memorypos, count)
        tdims = RobotRaconteurPython.TypeDefinition()
        tdims.Type = RobotRaconteurPython.DataTypes_uint32_t
        tdims.ArrayType = RobotRaconteurPython.DataTypes_ArrayTypes_array
        tdims.ArrayVarLength = True
        tdims.ArrayLength = RobotRaconteurPython.vectorint32([0])
        dims = UnpackFromRRArray(dat.Dims, tdims)

        t = RobotRaconteurPython.TypeDefinition()
        t.Type = self.__innermemory.ElementTypeID()
        t.ArrayType = RobotRaconteurPython.DataTypes_ArrayTypes_array
        t.ArrayVarLength = True
        t.ArrayLength = RobotRaconteurPython.vectorint32([0])
        array = UnpackFromRRArray(dat.Array, t)

        memind = [(slice(memorypos[i], (memorypos[i] + count[i])))
                  for i in range(len(count))]
        bufind = [(slice(bufferpos[i], (bufferpos[i] + count[i])))
                  for i in range(len(count))]

        buffer2 = array.reshape(dims, order="F")
        buffer[tuple(bufind)] = buffer2

    def Write(self, memorypos, buffer, bufferpos, count):

        tdims = RobotRaconteurPython.TypeDefinition()
        tdims.Type = RobotRaconteurPython.DataTypes_uint32_t
        tdims.ArrayType = RobotRaconteurPython.DataTypes_ArrayTypes_array
        tdims.ArrayVarLength = True
        tdims.ArrayLength = RobotRaconteurPython.vectorint32([0])

        t = RobotRaconteurPython.TypeDefinition()
        t.Type = self.__innermemory.ElementTypeID()
        t.ArrayType = RobotRaconteurPython.DataTypes_ArrayTypes_array
        t.ArrayVarLength = True
        t.ArrayLength = RobotRaconteurPython.vectorint32([0])
        elementcount = reduce(operator.mul, count, 1)
        dims = count

        memind = [slice(memorypos[i], (memorypos[i] + count[i]))
                  for i in range(len(count))]
        bufind = [slice(bufferpos[i], (bufferpos[i] + count[i]))
                  for i in range(len(count))]

        array = buffer[tuple(bufind)].flatten(order="F")

        dims2 = PackToRRArray(count, tdims)
        array2 = PackToRRArray(array, t)

        writedat2 = RobotRaconteurPython.RRMultiDimArrayUntyped()
        writedat2.Dims = dims2

        writedat2.Array = array2

        RobotRaconteurPython.WrappedMultiDimArrayMemoryClientUtil.Write(
            self.__innermemory, memorypos, writedat2, [0] * len(count), count)


class PodArrayMemoryClient_bufferdirector(RobotRaconteurPython.WrappedPodArrayMemoryClientBuffer):
    def __init__(self, buf, type1, obj, node):
        super(PodArrayMemoryClient_bufferdirector, self).__init__()
        self._buffer = buf
        self._obj = obj
        self._node = node
        self._type = type1

    def UnpackReadResult(self, res, bufferpos, count):
        res1 = RobotRaconteurPython.MessageElementDataUtil.ToMessageElementNestedElementList(
            res)
        m = RobotRaconteurPython.MessageElement()
        m.SetData(res)
        m.ElementTypeName = res1.TypeName
        m.DataCount = len(res1.Elements)

        res1 = UnpackMessageElement(m, self._type, self._obj, self._node)
        self._buffer[bufferpos:(bufferpos + count)] = res1

    def PackWriteRequest(self, bufferpos, count):
        buf1 = self._buffer[bufferpos:(bufferpos + count)]
        m_data = PackMessageElement(
            buf1, self._type, self._obj, self._node).GetData()
        return RobotRaconteurPython.MessageElementDataUtil.ToMessageElementNestedElementList(m_data)


class PodArrayMemoryClient(object):
    __slots__ = ["__innermemory", "_type", "_obj", "_node", "__weakref__"]

    def __init__(self, innermemory, type1, obj, node):
        self.__innermemory = innermemory
        self._type = type1
        self._obj = obj
        self._node = node

    @property
    def Length(self):
        return self.__innermemory.Length()

    def Read(self, memorypos, buf, bufferpos, count):
        b = PodArrayMemoryClient_bufferdirector(
            buf, self._type, self._obj, self._node)
        self.__innermemory.Read(memorypos, b, bufferpos, count)

    def Write(self, memorypos, buf, bufferpos, count):
        b = PodArrayMemoryClient_bufferdirector(
            buf, self._type, self._obj, self._node)
        self.__innermemory.Write(memorypos, b, bufferpos, count)


class PodMultiDimArrayMemoryClient_bufferdirector(RobotRaconteurPython.WrappedPodMultiDimArrayMemoryClientBuffer):
    def __init__(self, buf, type1, obj, node):
        super(PodMultiDimArrayMemoryClient_bufferdirector, self).__init__()
        self._buffer = buf
        self._obj = obj
        self._node = node
        self._type = type1

    def UnpackReadResult(self, res, bufferpos, count):
        res1 = RobotRaconteurPython.MessageElementDataUtil.ToMessageElementNestedElementList(
            res)
        m = RobotRaconteurPython.MessageElement()
        m.SetData(res)
        m.ElementTypeName = res1.TypeName
        m.DataCount = len(res1.Elements)

        res2 = UnpackMessageElement(m, self._type, self._obj, self._node)
        bufind = [(slice(bufferpos[i], (bufferpos[i] + count[i])))
                  for i in range(len(count))]
        self._buffer[tuple(bufind)] = res2

    def PackWriteRequest(self, bufferpos, count):
        bufind = [(slice(bufferpos[i], (bufferpos[i] + count[i])))
                  for i in range(len(count))]
        buf1 = self._buffer[tuple(bufind)]
        m_data = PackMessageElement(
            buf1, self._type, self._obj, self._node).GetData()
        return RobotRaconteurPython.MessageElementDataUtil.ToMessageElementNestedElementList(m_data)


class PodMultiDimArrayMemoryClient(object):
    __slots__ = ["__innermemory", "_type", "_obj", "_node", "__weakref__"]

    def __init__(self, innermemory, type1, obj, node):
        self.__innermemory = innermemory
        self._type = type1
        self._obj = obj
        self._node = node

    @property
    def DimCount(self):
        return self.__innermemory.DimCount()

    @property
    def Dimensions(self):
        return list(self.__innermemory.Dimensions())

    def Read(self, memorypos, buf, bufferpos, count):
        b = PodMultiDimArrayMemoryClient_bufferdirector(
            buf, self._type, self._obj, self._node)
        self.__innermemory.Read(memorypos, b, bufferpos, count)

    def Write(self, memorypos, buf, bufferpos, count):
        b = PodMultiDimArrayMemoryClient_bufferdirector(
            buf, self._type, self._obj, self._node)
        self.__innermemory.Write(memorypos, b, bufferpos, count)


class NamedArrayMemoryClient_bufferdirector(RobotRaconteurPython.WrappedNamedArrayMemoryClientBuffer):
    def __init__(self, buf, type1, obj, node):
        super(NamedArrayMemoryClient_bufferdirector, self).__init__()
        self._buffer = buf
        self._obj = obj
        self._node = node
        self._type = type1

    def UnpackReadResult(self, res, bufferpos, count):
        res1 = RobotRaconteurPython.MessageElementDataUtil.ToMessageElementNestedElementList(
            res)
        m = RobotRaconteurPython.MessageElement()
        m.SetData(res)
        m.ElementTypeName = res1.TypeName
        m.DataCount = len(res1.Elements)

        res1 = UnpackMessageElement(m, self._type, self._obj, self._node)
        self._buffer[bufferpos:(bufferpos + count)] = res1

    def PackWriteRequest(self, bufferpos, count):
        buf1 = self._buffer[bufferpos:(bufferpos + count)]
        m_data = PackMessageElement(
            buf1, self._type, self._obj, self._node).GetData()
        return RobotRaconteurPython.MessageElementDataUtil.ToMessageElementNestedElementList(m_data)


class NamedArrayMemoryClient(object):
    __slots__ = ["__innermemory", "_type", "_obj", "_node", "__weakref__"]

    def __init__(self, innermemory, type1, obj, node):
        self.__innermemory = innermemory
        self._type = type1
        self._obj = obj
        self._node = node

    @property
    def Length(self):
        return self.__innermemory.Length()

    def Read(self, memorypos, buf, bufferpos, count):
        b = NamedArrayMemoryClient_bufferdirector(
            buf, self._type, self._obj, self._node)
        self.__innermemory.Read(memorypos, b, bufferpos, count)

    def Write(self, memorypos, buf, bufferpos, count):
        b = NamedArrayMemoryClient_bufferdirector(
            buf, self._type, self._obj, self._node)
        self.__innermemory.Write(memorypos, b, bufferpos, count)


class NamedMultiDimArrayMemoryClient_bufferdirector(RobotRaconteurPython.WrappedNamedMultiDimArrayMemoryClientBuffer):
    def __init__(self, buf, type1, obj, node):
        super(NamedMultiDimArrayMemoryClient_bufferdirector, self).__init__()
        self._buffer = buf
        self._obj = obj
        self._node = node
        self._type = type1

    def UnpackReadResult(self, res, bufferpos, count):
        res1 = RobotRaconteurPython.MessageElementDataUtil.ToMessageElementNestedElementList(
            res)
        m = RobotRaconteurPython.MessageElement()
        m.SetData(res)
        m.ElementTypeName = res1.TypeName
        m.DataCount = len(res1.Elements)

        res2 = UnpackMessageElement(m, self._type, self._obj, self._node)
        bufind = [(slice(bufferpos[i], (bufferpos[i] + count[i])))
                  for i in range(len(count))]
        self._buffer[tuple(bufind)] = res2

    def PackWriteRequest(self, bufferpos, count):
        bufind = [(slice(bufferpos[i], (bufferpos[i] + count[i])))
                  for i in range(len(count))]
        buf1 = self._buffer[tuple(bufind)]
        m_data = PackMessageElement(
            buf1, self._type, self._obj, self._node).GetData()
        return RobotRaconteurPython.MessageElementDataUtil.ToMessageElementNestedElementList(m_data)


class NamedMultiDimArrayMemoryClient(object):
    __slots__ = ["__innermemory", "_type", "_obj", "_node", "__weakref__"]

    def __init__(self, innermemory, type1, obj, node):
        self.__innermemory = innermemory
        self._type = type1
        self._obj = obj
        self._node = node

    @property
    def DimCount(self):
        return self.__innermemory.DimCount()

    @property
    def Dimensions(self):
        return list(self.__innermemory.Dimensions())

    def Read(self, memorypos, buf, bufferpos, count):
        b = NamedMultiDimArrayMemoryClient_bufferdirector(
            buf, self._type, self._obj, self._node)
        self.__innermemory.Read(memorypos, b, bufferpos, count)

    def Write(self, memorypos, buf, bufferpos, count):
        b = NamedMultiDimArrayMemoryClient_bufferdirector(
            buf, self._type, self._obj, self._node)
        self.__innermemory.Write(memorypos, b, bufferpos, count)


class ServiceInfo2(object):
    """
    ServiceInfo2()

    Contains information about a service found using discovery

    ServiceInfo2 contains information about a service required to
    connect to the service, metadata, and the service attributes

    ServiceInfo2 structures are returned by RobotRaconteurNode.FindServiceByType()
    and ServiceInfo2Subscription
    """
    __slots__ = ["Name", "RootObjectType", "RootObjectImplements",
                 "ConnectionURL", "NodeID", "NodeName", "Attributes"]

    def __init__(self, info):
        self.Name = info.Name
        """(str) The name of the service"""
        self.RootObjectType = info.RootObjectType
        """(str) The fully qualified types the root object implements"""
        self.RootObjectImplements = list(info.RootObjectImplements)
        """(str) The fully qualified types the root object implements"""
        self.ConnectionURL = list(info.ConnectionURL)
        """(List[str]) Candidate URLs to connect to the service"""
        self.NodeID = RobotRaconteurPython.NodeID(str(info.NodeID))
        """(RobotRaconteur.NodeID) The NodeID of the node that owns the service"""
        self.NodeName = info.NodeName
        """(str) The NodeName of the node that owns the service"""
        self.Attributes = UnpackMessageElement(
            info.Attributes, "varvalue{string} value")
        """(Dict[str,Any]) Service attributes"""


class NodeInfo2(object):
    """
    NodeInfo2()

    Contains information about a node detected using discovery

    NodeInfo2 contains information about a node detected using discovery.
    Node information is typically not verified, and is used as a first
    step to detect available services.

    NodeInfo2 structures are returned by RobotRaconteurNode.FindNodeByName()
    and RobotRaconteurNode.FindNodeByID()
    """
    __slots__ = ["NodeID", "NodeName", "ConnectionURL"]

    def __init__(self, info):
        self.NodeID = RobotRaconteurPython.NodeID(str(info.NodeID))
        """(RobotRaconteur.NodeID) The NodeID of the detected node"""
        self.NodeName = info.NodeName
        """(str) The NodeName of the detected node"""
        self.ConnectionURL = list(info.ConnectionURL)
        """(List[str]) Candidate URLs to connect to the node

        The URLs for the node typically contain the node transport endpoint
        and the nodeid. A URL service parameter must be appended
        to connect to a service.
        """


class WrappedServiceSkelDirectorPython(RobotRaconteurPython.WrappedServiceSkelDirector):
    def __init__(self, obj):
        self.obj = obj
        super(WrappedServiceSkelDirectorPython, self).__init__()

    def Init(self, skel):

        self.skel = skel
        odef = skel.Type
        for i in range(len(odef.Members)):
            m = odef.Members[i]
            if (isinstance(m, RobotRaconteurPython.EventDefinition)):
                if (not m.Name in dir(self.obj)):
                    setattr(self.obj, m.Name, EventHook())

                def inner_event(m1):

                    f = lambda *args: skel_dispatchevent(
                        skel, m1.Name, m1, *args)
                    return f
                f1 = inner_event(m)
                evt = getattr(self.obj, m.Name)
                evt += f1
            if (isinstance(m, RobotRaconteurPython.CallbackDefinition)):
                def inner_callback(m1):
                    if (m1.ReturnType.Type == RobotRaconteurPython.DataTypes_void_t):
                        f = lambda endpoint, * \
                            args: skel_callbackcallvoid(
                                skel, m1.Name, m1, endpoint, *args)
                    else:
                        f = lambda endpoint, * \
                            args: skel_callbackcall(
                                skel, m1.Name, m1, endpoint, *args)
                    return f
                f1 = inner_callback(m)
                cs = CallbackServer(f1)

                setattr(self.obj, m.Name, cs)
            if (isinstance(m, RobotRaconteurPython.PipeDefinition)):
                p = skel.GetPipe(m.Name)
                outerp = Pipe(p)
                if (not m.Name in dir(self.obj)):
                    if ("readonly" in m.Modifiers):
                        setattr(self.obj, m.Name, PipeBroadcaster(outerp))
                    else:
                        setattr(self.obj, m.Name, outerp)
                else:
                    setattr(self.obj, m.Name, outerp)
            if (isinstance(m, RobotRaconteurPython.WireDefinition)):
                w = skel.GetWire(m.Name)
                outerw = Wire(w)
                if (not m.Name in dir(self.obj)):
                    if ("readonly" in m.Modifiers):
                        setattr(self.obj, m.Name, WireBroadcaster(outerw))
                    elif ("writeonly" in m.Modifiers):
                        setattr(self.obj, m.Name, WireUnicastReceiver(outerw))
                    else:
                        setattr(self.obj, m.Name, outerw)
                else:
                    setattr(self.obj, m.Name, outerw)
        if hasattr(self.obj, "RRServiceObjectInit"):
            self.obj.RRServiceObjectInit(
                skel.GetContext(), skel.GetServicePath())

    def _CallGetProperty(self, name, async_adapter):

        type1 = FindMemberByName(self.skel.Type.Members, name)
        # type1=[e for e in self.skel.Type.Members if e.Name == name][0]
        type2 = RobotRaconteurPython.MemberDefinitionUtil.ToProperty(type1)
        try:
            async_getter = getattr(self.obj, "async_get_" + name)
        except AttributeError:
            pass
        else:
            def async_getter_handler(ret, err):
                if err is not None:
                    async_adapter.End(
                        RobotRaconteurPythonError.RobotRaconteurExceptionUtil.ExceptionToErrorInfo(err))
                    _print_exc_traceback(err)
                    return
                if (ret is None):
                    m = RobotRaconteurPython.MessageElement()
                    m.ElementName = "value"
                    m.ElementType = RobotRaconteurPython.DataTypes_void_t
                else:
                    m = PackMessageElement(
                        ret, type2.Type, node=self.skel.RRGetNode())
                async_adapter.End(m, RobotRaconteurPython.HandlerErrorInfo())
                return
            async_adapter.MakeAsync()
            async_getter(async_getter_handler)
            return

        ret = getattr(self.obj, name)

        if (ret is None):
            m = RobotRaconteurPython.MessageElement()
            m.ElementName = "value"
            m.ElementType = RobotRaconteurPython.DataTypes_void_t
            return m
        return PackMessageElement(ret, type2.Type, node=self.skel.RRGetNode())

    def _CallSetProperty(self, name, value, async_adapter):

        type1 = FindMemberByName(self.skel.Type.Members, name)
        # type1=[e for e in self.skel.Type.Members if e.Name == name][0]
        type2 = RobotRaconteurPython.MemberDefinitionUtil.ToProperty(type1)
        a = UnpackMessageElement(value, type2.Type, node=self.skel.RRGetNode())

        try:
            async_setter = getattr(self.obj, "async_set_" + name)
        except AttributeError:
            pass
        else:
            def async_setter_handler(err):
                if err is not None:
                    async_adapter.End(
                        RobotRaconteurPythonError.RobotRaconteurExceptionUtil.ExceptionToErrorInfo(err))
                    _print_exc_traceback(err)
                    return
                async_adapter.End(RobotRaconteurPython.HandlerErrorInfo())
                return
            async_adapter.MakeAsync()
            async_setter(a, async_setter_handler)
            return

        setattr(self.obj, name, a)

    def _CallFunction(self, name, args1, async_adapter):

        type1 = FindMemberByName(self.skel.Type.Members, name)
        type2 = RobotRaconteurPython.MemberDefinitionUtil.ToFunction(type1)
        args = []
        for p in type2.Parameters:
            if p.ContainerType == RobotRaconteurPython.DataTypes_ContainerTypes_generator:
                continue
            m = FindMessageElementByName(args1, p.Name)
            a = UnpackMessageElement(m, p, node=self.skel.RRGetNode())
            args.append(a)
        is_generator = type2.IsGenerator()
        if not is_generator:
            try:
                async_func = getattr(self.obj, "async_" + name)
            except AttributeError:
                pass
            else:
                if type2.ReturnType.Type == RobotRaconteurPython.DataTypes_void_t:
                    def async_function_handler(err):
                        if err is not None:
                            async_adapter.End(
                                RobotRaconteurPythonError.RobotRaconteurExceptionUtil.ExceptionToErrorInfo(err))
                            _print_exc_traceback(err)
                            return
                        async_adapter.End(
                            None, RobotRaconteurPython.HandlerErrorInfo())
                        return
                else:
                    def async_function_handler(ret, err):
                        if err is not None:
                            async_adapter.End(
                                RobotRaconteurPythonError.RobotRaconteurExceptionUtil.ExceptionToErrorInfo(err))
                            _print_exc_traceback(err)
                            return
                        if (ret is None):
                            m = RobotRaconteurPython.MessageElement()
                            m.ElementName = "return"
                            m.ElementType = RobotRaconteurPython.DataTypes_void_t
                        else:
                            m = PackMessageElement(
                                ret, type2.ReturnType, node=self.skel.RRGetNode())
                        async_adapter.End(
                            m, RobotRaconteurPython.HandlerErrorInfo())
                        return
                async_adapter.MakeAsync()
                async_func(*args + [async_function_handler])
                return

        ret = getattr(self.obj, name)(*args)

        if is_generator:
            gen_return_type = None
            if type2.ReturnType.Type != RobotRaconteurPython.DataTypes_void_t:
                gen_return_type = type2.ReturnType.Clone()
                gen_return_type.RemoveContainers()
            gen_param_type = None
            if len(type2.Parameters) > 0 and type2.Parameters[-1].ContainerType == RobotRaconteurPython.DataTypes_ContainerTypes_generator:
                gen_param_type = type2.Parameters[-1]
            gen = WrappedGeneratorServerDirectorPython(
                ret, gen_return_type, gen_param_type, self.skel.RRGetNode())
            ind = self.skel.RegisterGeneratorServer(type2.Name, gen)
            gen.__disown__()
            return PackMessageElement(ind, "int32 index", node=self.skel.RRGetNode())

        if (ret is None):
            m = RobotRaconteurPython.MessageElement()
            m.ElementName = "return"
            m.ElementType = RobotRaconteurPython.DataTypes_void_t
            return m
        return PackMessageElement(ret, type2.ReturnType, node=self.skel.RRGetNode())

    def _GetSubObj(self, name, index):

        type1 = FindMemberByName(self.skel.Type.Members, name)
        # type1=[e for e in self.skel.Type.Members if e.Name == name][0]
        type2 = RobotRaconteurPython.MemberDefinitionUtil.ToObjRef(type1)
        if (type2.ArrayType == RobotRaconteurPython.DataTypes_ArrayTypes_array or (type2.ContainerType == RobotRaconteurPython.DataTypes_ContainerTypes_map_int32)):
            obj, objecttype = getattr(self.obj, 'get_' + name)(str(index))
        elif (type2.ContainerType == RobotRaconteurPython.DataTypes_ContainerTypes_map_string):
            obj, objecttype = getattr(self.obj, 'get_' + name)(index)
        else:
            obj, objecttype = getattr(self.obj, 'get_' + name)()

        director = WrappedServiceSkelDirectorPython(obj)
        rrobj = RobotRaconteurPython.WrappedRRObject(objecttype, director, 0)
        director.__disown__()
        return rrobj

    def _GetArrayMemory(self, name):

        m = getattr(self.obj, name)
        d = WrappedArrayMemoryDirectorPython(m)
        d.__disown__()
        return d

    def _GetMultiDimArrayMemory(self, name):
        m = getattr(self.obj, name)
        d = WrappedMultiDimArrayMemoryDirectorPython(m)
        d.__disown__()
        return d

    def _GetPodArrayMemory(self, name):
        type1 = FindMemberByName(self.skel.Type.Members, name)
        type2 = RobotRaconteurPython.MemberDefinitionUtil.ToMemory(type1)
        m = getattr(self.obj, name)
        d = WrappedPodArrayMemoryDirectorPython(
            m, type2.Type, self.skel.RRGetNode())
        d.__disown__()
        return d

    def _GetPodMultiDimArrayMemory(self, name):
        type1 = FindMemberByName(self.skel.Type.Members, name)
        type2 = RobotRaconteurPython.MemberDefinitionUtil.ToMemory(type1)
        m = getattr(self.obj, name)
        d = WrappedPodMultiDimArrayMemoryDirectorPython(
            m, type2.Type, self.skel.RRGetNode())
        d.__disown__()
        return d

    def _GetNamedArrayMemory(self, name):
        type1 = FindMemberByName(self.skel.Type.Members, name)
        type2 = RobotRaconteurPython.MemberDefinitionUtil.ToMemory(type1)
        m = getattr(self.obj, name)
        d = WrappedNamedArrayMemoryDirectorPython(
            m, type2.Type, self.skel.RRGetNode())
        d.__disown__()
        return d

    def _GetNamedMultiDimArrayMemory(self, name):
        type1 = FindMemberByName(self.skel.Type.Members, name)
        type2 = RobotRaconteurPython.MemberDefinitionUtil.ToMemory(type1)
        m = getattr(self.obj, name)
        d = WrappedNamedMultiDimArrayMemoryDirectorPython(
            m, type2.Type, self.skel.RRGetNode())
        d.__disown__()
        return d

    def MonitorEnter(self, timeout):

        self.obj.RobotRaconteurMonitorEnter(timeout)

    def MonitorExit(self):

        self.obj.RobotRaconteurMonitorExit()

    def ReleaseCastObject(self):
        self.obj = None
        self.skel = None


def skel_dispatchevent(skel, name, type1, *args):
    check_member_args(name, type1.Parameters, args)
    m = RobotRaconteurPython.vectorptr_messageelement()
    i = 0
    node = skel.RRGetNode()
    for p in type1.Parameters:
        a = PackMessageElement(args[i], p, node=node)
        a.ElementName = p.Name
        m.append(a)
        i += 1
    skel.WrappedDispatchEvent(name, m)


def skel_callbackcall(skel, name, type1, endpoint, *args):
    check_member_args(name, type1.Parameters, args)
    m = RobotRaconteurPython.vectorptr_messageelement()
    i = 0
    node = skel.RRGetNode()
    for p in type1.Parameters:
        a = PackMessageElement(args[i], p, node=node)
        a.ElementName = p.Name
        m.append(a)
        i += 1
    ret = skel.WrappedCallbackCall(name, endpoint, m)
    return UnpackMessageElement(ret, type1.ReturnType, node=node)


def skel_callbackcallvoid(skel, name, type1, endpoint, *args):
    check_member_args(name, type1.Parameters, args)
    m = RobotRaconteurPython.vectorptr_messageelement()
    i = 0
    node = skel.RRGetNode()
    for p in type1.Parameters:
        a = PackMessageElement(args[i], p, node=node)
        a.ElementName = p.Name
        m.append(a)
        i += 1
    skel.WrappedCallbackCall(name, endpoint, m)


class CallbackServer(object):
    __slots__ = ["func", "__weakref__"]

    def __init__(self, func):

        self.func = func

    def GetClientFunction(self, endpoint):
        func = self.func

        return lambda *args: func(endpoint, *args)


class WrappedArrayMemoryDirectorPython(RobotRaconteurPython.WrappedArrayMemoryDirector):
    def __init__(self, memory):
        self.memory = memory
        super(WrappedArrayMemoryDirectorPython, self).__init__()

    def Length(self):

        return self.memory.Length

    def Read(self, memorypos, buffer, bufferpos, count):

        buffer3 = numpy.zeros(
            (count,), dtype=RobotRaconteurPython._RRTypeIdToNumPyDataType(buffer.GetTypeID()))
        self.memory.Read(memorypos, buffer3, bufferpos, count)
        type1 = RobotRaconteurPython.TypeDefinition()
        type1.ArrayType = RobotRaconteurPython.DataTypes_ArrayTypes_array
        type1.ArrayVarLength = True
        type1.ArrayLength = RobotRaconteurPython.vectorint32([0])
        type1.Type = buffer.GetTypeID()
        PackToRRArray(buffer3, type1, buffer)

    def Write(self, memorypos, buffer, bufferpos, count):

        buffer3 = UnpackFromRRArray(buffer, None)
        self.memory.Write(memorypos, buffer3, bufferpos, count)


class WrappedMultiDimArrayMemoryDirectorPython(RobotRaconteurPython.WrappedMultiDimArrayMemoryDirector):
    def __init__(self, memory):
        self.memory = memory
        super(WrappedMultiDimArrayMemoryDirectorPython, self).__init__()
        import RobotRaconteur

    def Dimensions(self):

        d = self.memory.Dimensions
        d2 = RobotRaconteurPython.vector_uint64_t()
        for d_i in d:
            d2.push_back(d_i)
        return d2

    def DimCount(self):

        return self.memory.DimCount

    def Read(self, p):

        rrmultidim = p.buffer
        rrt = RobotRaconteurPython.TypeDefinition()
        rrt.Type = rrmultidim.Array.GetTypeID()
        rrt.ArrayType = RobotRaconteurPython.DataTypes_ArrayTypes_array
        rrt.ArrayVarLength = True
        rrt.ArrayLength = RobotRaconteurPython.vectorint32([0])
        dims = list(p.count)
        elementcount = reduce(operator.mul, dims, 1)

        readdat1 = numpy.zeros(dims, dtype=RobotRaconteurPython._RRTypeIdToNumPyDataType(
            rrmultidim.Array.GetTypeID()))
        self.memory.Read(list(p.memorypos), readdat1, list(p.bufferpos), dims)

        PackToRRArray(readdat1.flatten(order="F"), rrt, rrmultidim.Array)

    def Write(self, p):

        memorypos = list(p.memorypos)
        bufferpos = list(p.bufferpos)
        count = list(p.count)

        rrmultidim = p.buffer
        md_dims = UnpackFromRRArray(rrmultidim.Dims)
        md_array = UnpackFromRRArray(rrmultidim.Array)

        buffer = md_array.reshape(count, order="F")
        self.memory.Write(memorypos, buffer, bufferpos, count)


class WrappedPodArrayMemoryDirectorPython(RobotRaconteurPython.WrappedPodArrayMemoryDirector):
    def __init__(self, memory, type1, node):
        self.memory = memory
        self.node = node
        self.type = type1
        super(WrappedPodArrayMemoryDirectorPython, self).__init__()

    def Length(self):

        return self.memory.Length

    def Read(self, memorypos, bufferpos, count):
        dt = GetPodDType(self.type)
        buffer3 = numpy.zeros((count,), dtype=dt)
        self.memory.Read(memorypos, buffer3, bufferpos, count)
        m = PackMessageElement(buffer3, self.type, node=self.node)
        return RobotRaconteurPython.MessageElementDataUtil.ToMessageElementNestedElementList(m.GetData())

    def Write(self, memorypos, buffer_, bufferpos, count):
        m = RobotRaconteurPython.MessageElement()
        m.SetData(buffer_)
        m.ElementTypeName = buffer_.TypeName
        m.DataCount = len(buffer_.Elements)
        buffer3 = UnpackMessageElement(m, self.type, node=self.node)
        self.memory.Write(memorypos, buffer3, bufferpos, count)


class WrappedPodMultiDimArrayMemoryDirectorPython(RobotRaconteurPython.WrappedPodMultiDimArrayMemoryDirector):
    def __init__(self, memory, type1, node):
        super(WrappedPodMultiDimArrayMemoryDirectorPython, self).__init__()
        self.memory = memory
        self.type = type1
        self.node = node

    def Dimensions(self):

        d = self.memory.Dimensions
        d2 = RobotRaconteurPython.vector_uint64_t()
        for d_i in d:
            d2.push_back(d_i)
        return d2

    def DimCount(self):
        return self.memory.DimCount

    def Read(self, memorypos, bufferpos, count):

        dims = list(count)
        dt = GetPodDType(self.type)

        readdat1 = numpy.zeros(dims, dtype=dt)

        self.memory.Read(list(memorypos), readdat1, list(bufferpos), dims)

        m = PackMessageElement(readdat1, self.type, node=self.node)
        return RobotRaconteurPython.MessageElementDataUtil.ToMessageElementNestedElementList(m.GetData())

    def Write(self, memorypos_, buffer_, bufferpos_, count_):
        try:
            memorypos = list(memorypos_)
            bufferpos = list(bufferpos_)
            count = list(count_)

            buffer1 = RobotRaconteurPython.MessageElementDataUtil.ToMessageElementNestedElementList(
                buffer_)
            m = RobotRaconteurPython.MessageElement()
            m.SetData(buffer_)
            m.ElementTypeName = buffer1.TypeName
            m.DataCount = len(buffer1.Elements)
            buffer2 = UnpackMessageElement(m, self.type, node=self.node)
            self.memory.Write(memorypos, buffer2, bufferpos, count)
        except:
            traceback.print_exc()
            raise


class WrappedNamedArrayMemoryDirectorPython(RobotRaconteurPython.WrappedNamedArrayMemoryDirector):
    def __init__(self, memory, type1, node):
        self.memory = memory
        self.node = node
        self.type = type1
        super(WrappedNamedArrayMemoryDirectorPython, self).__init__()

    def Length(self):

        return self.memory.Length

    def Read(self, memorypos, bufferpos, count):
        dt = GetNamedArrayDType(self.type)
        buffer3 = numpy.zeros((count,), dtype=dt)
        self.memory.Read(memorypos, buffer3, bufferpos, count)
        m = PackMessageElement(buffer3, self.type, node=self.node)
        return RobotRaconteurPython.MessageElementDataUtil.ToMessageElementNestedElementList(m.GetData())

    def Write(self, memorypos, buffer_, bufferpos, count):
        m = RobotRaconteurPython.MessageElement()
        m.SetData(buffer_)
        m.ElementTypeName = buffer_.TypeName
        m.DataCount = len(buffer_.Elements)
        buffer3 = UnpackMessageElement(m, self.type, node=self.node)
        self.memory.Write(memorypos, buffer3, bufferpos, count)


class WrappedNamedMultiDimArrayMemoryDirectorPython(RobotRaconteurPython.WrappedNamedMultiDimArrayMemoryDirector):
    def __init__(self, memory, type1, node):
        super(WrappedNamedMultiDimArrayMemoryDirectorPython, self).__init__()
        self.memory = memory
        self.type = type1
        self.node = node

    def Dimensions(self):

        d = self.memory.Dimensions
        d2 = RobotRaconteurPython.vector_uint64_t()
        for d_i in d:
            d2.push_back(d_i)
        return d2

    def DimCount(self):
        return self.memory.DimCount

    def Read(self, memorypos, bufferpos, count):
        dims = list(count)
        dt = GetNamedArrayDType(self.type)
        readdat1 = numpy.zeros(dims, dtype=dt)
        self.memory.Read(list(memorypos), readdat1, list(bufferpos), dims)
        m = PackMessageElement(readdat1, self.type, node=self.node)
        return RobotRaconteurPython.MessageElementDataUtil.ToMessageElementNestedElementList(m.GetData())

    def Write(self, memorypos_, buffer_, bufferpos_, count_):
        try:
            memorypos = list(memorypos_)
            bufferpos = list(bufferpos_)
            count = list(count_)

            buffer1 = RobotRaconteurPython.MessageElementDataUtil.ToMessageElementNestedElementList(
                buffer_)
            m = RobotRaconteurPython.MessageElement()
            m.SetData(buffer_)
            m.ElementTypeName = buffer1.TypeName
            m.DataCount = len(buffer1.Elements)
            buffer2 = UnpackMessageElement(m, self.type, node=self.node)
            self.memory.Write(memorypos, buffer2, bufferpos, count)
        except:
            traceback.print_exc()
            raise


class WrappedClientServiceListenerDirector(RobotRaconteurPython.ClientServiceListenerDirector):
    def __init__(self, callback):
        self.callback = callback

        super(WrappedClientServiceListenerDirector, self).__init__()

    def Callback(self, code):

        self.callback(self.stub, code, None)

    def Callback2(self, code, p):

        self.callback(self.stub, code, p)


class WrappedServerServiceListenerDirector(RobotRaconteurPython.ServerServiceListenerDirector):
    def __init__(self, callback, context):
        self.callback = callback
        self.context = context

        super(WrappedServerServiceListenerDirector, self).__init__()

    def Callback(self, code, endpoint):

        self.callback(self.context, code, endpoint)


class RRConstants(object):
    pass


def convert_constant(const):

    if not const.VerifyValue():
        raise Exception("Invalid constant " + const.Name)
    t = const.Type

    if (t.Type == RobotRaconteurPython.DataTypes_string_t):
        return const.Name, const.ValueToString()

    if RobotRaconteurPython.IsTypeNumeric(t.Type):

        if (t.Type == RobotRaconteurPython.DataTypes_double_t or t.Type == RobotRaconteurPython.DataTypes_single_t):
            if (t.ArrayType == RobotRaconteurPython.DataTypes_ArrayTypes_array):
                s3 = const.Value.strip().lstrip('{').rstrip('}')
                return const.Name, [float(i) for i in s3.split(',')]
            else:
                return const.Name, float(const.Value)
        else:
            if (t.ArrayType == RobotRaconteurPython.DataTypes_ArrayTypes_array):
                s3 = const.Value.strip().lstrip('{').rstrip('}')
                return const.Name, [int(i, 0) for i in s3.split(',')]
            else:
                return const.Name, const.Value

    if t.Type == RobotRaconteurPython.DataTypes_namedtype_t:
        struct_fields = const.ValueToStructFields()
        f = dict()
        for struct_field in struct_fields:
            f[struct_field.Name] = struct_field.ConstantRefName
        return const.Name, f

    raise Exception("Unknown constant type")


def ServiceDefinitionConstants(servicedef, node, obj):
    o = dict()
    for s in servicedef.Options:
        s2 = s.split(None, 1)
        if (s2[0] == "constant"):
            c = RobotRaconteurPython.ConstantDefinition(servicedef)
            c.FromString(s)
            name, val = convert_constant(c)
            o[name] = val

    for c in servicedef.Constants:
        name, val = convert_constant(c)
        o[name] = val

    elem_o = dict()
    for e in itertools.chain(servicedef.NamedArrays, servicedef.Pods, servicedef.Structures, servicedef.Objects):
        o2 = dict()
        for s in e.Options:

            s2 = s.split(None, 1)
            if (s2[0] == "constant"):
                c = RobotRaconteurPython.ConstantDefinition(servicedef)
                c.FromString(s)
                name, val = convert_constant(c)
                o2[name] = val

        for c in e.Constants:
            name, val = convert_constant(c)
            o2[name] = val

        if (len(o2) > 0):
            elem_o[e.Name] = o2

    for _, c_value in o.items():
        if isinstance(c_value, dict):
            for f_name, f_value in c_value.items():
                if not f_value in o:
                    raise Exception(
                        "Invalid struct reference type: " + f_value)
                c_value[f_name] = o[f_value]

    for n, v in elem_o.items():
        o[n] = v

    for e in servicedef.Enums:
        o_enum = dict()
        for v in e.Values:
            o_enum[v.Name] = v.Value
        o[e.Name] = o_enum

    return o


def adjust_timeout(t):
    if (t < 0):
        return -1
    else:
        return int(t * 1000)


class AsyncStubReturnDirectorImpl(RobotRaconteurPython.AsyncStubReturnDirector):
    def __init__(self, handler):
        super(AsyncStubReturnDirectorImpl, self).__init__()
        self._handler = handler

    def handler(self, innerstub2, error_info):
        if (error_info.error_code != 0):
            err = RobotRaconteurPythonError.RobotRaconteurExceptionUtil.ErrorInfoToException(
                error_info)
            self._handler(None, err)
            return
        try:
            stub = innerstub2.GetPyStub()
            if (stub is None):
                stub = InitStub(innerstub2)
                innerstub2.SetPyStub(stub)
        except Exception as err2:
            self._handler(None, err2)
            return
        self._handler(stub, None)


class AsyncVoidReturnDirectorImpl(RobotRaconteurPython.AsyncVoidReturnDirector):
    def __init__(self, handler):
        super(AsyncVoidReturnDirectorImpl, self).__init__()
        self._handler = handler

    def handler(self, error_info):
        if (error_info.error_code != 0):
            err = RobotRaconteurPythonError.RobotRaconteurExceptionUtil.ErrorInfoToException(
                error_info)
            self._handler(err)
            return
        self._handler(None)


class AsyncVoidNoErrReturnDirectorImpl(RobotRaconteurPython.AsyncVoidNoErrReturnDirector):
    def __init__(self, handler):
        super(AsyncVoidNoErrReturnDirectorImpl, self).__init__()
        self._handler = handler

    def handler(self):
        self._handler()


class AsyncStringReturnDirectorImpl(RobotRaconteurPython.AsyncStringReturnDirector):
    def __init__(self, handler):
        super(AsyncStringReturnDirectorImpl, self).__init__()
        self._handler = handler

    def handler(self, istr, error_info):
        if (error_info.error_code != 0):
            err = RobotRaconteurPythonError.RobotRaconteurExceptionUtil.ErrorInfoToException(
                error_info)
            self._handler(None, err)
            return
        self._handler(istr, None)


class AsyncUInt32ReturnDirectorImpl(RobotRaconteurPython.AsyncUInt32ReturnDirector):
    def __init__(self, handler):
        super(AsyncUInt32ReturnDirectorImpl, self).__init__()
        self._handler = handler

    def handler(self, e, error_info):
        if (error_info.error_code != 0):
            err = RobotRaconteurPythonError.RobotRaconteurExceptionUtil.ErrorInfoToException(
                error_info)
            self._handler(None, err)
            return
        self._handler(e, None)


class AsyncTimerEventReturnDirectorImpl(RobotRaconteurPython.AsyncTimerEventReturnDirector):
    def __init__(self, handler):
        super(AsyncTimerEventReturnDirectorImpl, self).__init__()
        self._handler = handler

    def handler(self, ev, error_info):
        self._handler(ev)


def async_call(func, args, directorclass, handler, noerror=False, directorargs=()):
    d = None
    if (handler is None):
        if (sys.version_info > (3, 5)):
            loop = asyncio.get_event_loop()
            d = asyncio.Future()

            def handler3(*args):
                if noerror:
                    if len(args) == 0:
                        loop.call_soon_threadsafe(d.set_result, None)
                    else:
                        loop.call_soon_threadsafe(d.set_result, args[0])
                else:
                    ret = None
                    if len(args) == 2:
                        ret = args[0]
                    if args[-1] is None:
                        loop.call_soon_threadsafe(d.set_result, ret)
                    else:
                        loop.call_soon_threadsafe(d.set_exception, args[-1])
            handler = lambda *args1: handler3(*args1)
        else:
            raise Exception("handler must not be None")

    handler2 = directorclass(handler, *directorargs)
    args2 = list(args)
    args2.extend([handler2, 0])
    handler2.__disown__()
    func(*args2)
    return d


class ExceptionHandlerDirectorImpl(RobotRaconteurPython.AsyncVoidReturnDirector):
    def __init__(self, handler):
        super(ExceptionHandlerDirectorImpl, self).__init__()
        self._handler = handler

    def handler(self, error_info):
        if (error_info.error_code != 0):
            err = RobotRaconteurPythonError.RobotRaconteurExceptionUtil.ErrorInfoToException(
                error_info)
            self._handler(err)
            return


class WrappedPipeBroadcasterPredicateDirectorPython(RobotRaconteurPython.WrappedPipeBroadcasterPredicateDirector):
    def __init__(self, f):
        super(WrappedPipeBroadcasterPredicateDirectorPython, self).__init__()
        self.f = f

    def Predicate(self, client_endpoint, index_):
        try:
            return self.f(client_endpoint, index_)
        except:
            traceback.print_exc()
            return True


class PipeBroadcaster(object):
    """
    Broadcaster to send packets to all connected clients

    PipeBroadcaster is used by services to send packets to all connected
    client endpoints. It attaches to the pipe on the service side, and
    manages the lifecycle of connected endpoints. PipeBroadcaster should
    only be used with pipes that are declared *readonly*, since it has
    no provisions for receiving incoming packets from the client.

    PipeBroadcaster is initialized by the user, or by default if the service
    object has no attribute for *readonly* pipes. If default
    an attribute is specified for the pipe
    not used, the broadcaster must be instantiated manually. It is
    recommended this be done using the RRServiceObjectInit() function. If present
    in the service object, function is called after the pipes have been instantiated
    by the service.

    Use SendPacket() or AsyncSendPacket() to broadcast packets to all
    connected clients.

    PipeBroadcaster provides flow control by optionally tracking how many packets
    are in flight to each client pipe endpoint. (This is accomplished using packet acks.) If a
    maximum backlog is specified, pipe endpoints exceeding this count will stop sending packets.
    Specify the maximum backlog on construction or the MaxBacklog property.

    The rate that packets are sent can be regulated using a callback function configured
    with the SetPredicate() function, or using the BroadcastDownsampler class.

    :param pipe: The pipe to use for broadcasting. Must be a pipe from a service object.
     Specifying a client pipe will result in an exception.
    :type pipe: RobotRaconteur.Pipe
    :param maximum_backlog: The maximum number of packets in flight, or -1 for unlimited
    :type maximum_backlog: int

    """
    __slots__ = ["pipe", "_innerpipe", "_obj", "_type", "__weakref__"]

    def __init__(self, pipe, maximum_backlog=-1):
        self.pipe = pipe

        self._innerpipe = RobotRaconteurPython.WrappedPipeBroadcaster()
        self._innerpipe.Init(pipe._innerpipe, maximum_backlog)
        self._obj = pipe._obj
        self._type = pipe._innerpipe.Type

    def AsyncSendPacket(self, packet, handler):
        """
        Asynchronously send packet to all connected pipe endpoint clients

        Asynchronous version of SendPacket()

        :param packet: The packet to send
        :param handler: A handler function for when packet has been sent by all endpoints
        :type handler: Callable[[],None]
        """
        m = PackMessageElement(packet, self._type, self._obj)
        return async_call(self._innerpipe.AsyncSendPacket, (m,), AsyncVoidNoErrReturnDirectorImpl, handler)

    def SendPacket(self, packet):
        """
        Send a packet to all connected pipe endpoint clients

        Blocks until packet has been sent by all endpoints

        :param packet: The packet to send
        """
        m = PackMessageElement(packet, self._type, self._obj)
        self._innerpipe.SendPacket(m)

    @property
    def ActivePipeEndpointCount(self):
        """Get the number of active pipe endpoints"""
        return self._innerpipe.GetActivePipeEndpointCount()

    def SetPredicate(self, f):
        """
        Set the predicate callback function

        A predicate is optionally used to regulate when packets are sent to clients. This is used by the
        BroadcastDownsampler to regulate update rates of packets sent to clients.

        The predicate callback is invoked before the broadcaster sends a packet to an endpoint. If the predicate returns true,
        the packet will be sent. If it is false, the packet will not be sent to that endpoint.

        The predicate receives the broadcaster, the client endpoint ID, and the pipe endpoint index. It returns true to send the packet,
        or false to not send the packet.

        :param f: The predicate
        :type f: Callable[[RobotRaconteur.PipeBroadcaster,int,int],bool]
        """
        p = WrappedPipeBroadcasterPredicateDirectorPython(f)
        self._innerpipe.SetPredicateDirector(p, 0)
        p.__disown__()

    @property
    def MaxBacklog(self):
        """
        Set the maximum backlog

        PipeBroadcaster provides flow control by optionally tracking how many packets
        are in flight to each client pipe endpoint. (This is accomplished using packet acks.) If a
        maximum backlog is specified, pipe endpoints exceeding this count will stop sending packets.

        Set -1 for no flow control.

        :rtype: int
        """
        return self._innerpipe.GetMaxBacklog()

    @MaxBacklog.setter
    def MaxBacklog(self, maximum_backlog):
        self._innerpipe.SetMaxBacklog(maximum_backlog)


class WrappedWireBroadcasterPredicateDirectorPython(RobotRaconteurPython.WrappedWireBroadcasterPredicateDirector):
    def __init__(self, f):
        super(WrappedWireBroadcasterPredicateDirectorPython, self).__init__()
        self.f = f

    def Predicate(self, client_endpoint):
        try:
            return self.f(client_endpoint)
        except:
            traceback.print_exc()
            return True


class WrappedWireBroadcasterPeekValueDirectorImpl(RobotRaconteurPython.WrappedWireServerPeekValueDirector):
    def __init__(self, cb, value_type, node, obj):
        super(WrappedWireBroadcasterPeekValueDirectorImpl, self).__init__()
        self._cb = cb
        self.__node = node
        self.__obj = obj
        self.__value_type = value_type

    def PeekValue(self, ep):
        value = self._cb(ep)
        m = PackMessageElement(value, self.__value_type,
                               self.__obj, self.__node)
        return m


class WireBroadcaster(object):
    """
    Broadcaster to send values to all connected clients

    WireBroadcaster is used by services to send values to all
    connected client endpoints. It attaches to the wire on the service
    side, and manages the lifecycle of connections. WireBroadcaster
    should only we used with wires that are declared *readonly*, since
    it has no provisions for receiving incoming values from clients.

    WireBroadcaster is initialized by the user, or by default if the
    service object does not contain an attribute for the wire member. The default
    will automatically instantiate unicast receivers for
    wires marked *readonly*. If default the implementation is not
    not used, the broadcaster must be instantiated manually. It is recommended this
    be done using an RRServiceObjectInit() function in the service object.
    This function is called after the wires have been instantiated by the service.

    Use OutValue property to broadcast values to all connected clients.

    The rate that packets are sent can be regulated using a callback function configured
    with the SetPredicate() function, or using the BroadcastDownsampler class.

    :param wire: The wire to use for broadcasting. Must be a wire from a service object.
     Specifying a client wire will result in an exception.
    :type wire: RobotRaconteur.Wire
    """
    __slots__ = ["_wire", "_innerpipe", "_obj",
                 "_type", "__weakref__", "_node"]

    def __init__(self, wire):
        self._wire = wire
        self._innerpipe = RobotRaconteurPython.WrappedWireBroadcaster()
        self._innerpipe.Init(wire._innerpipe)
        self._obj = wire._obj
        self._type = wire._innerpipe.Type
        self._node = wire._innerpipe.GetNode()

    @property
    def OutValue(self):
        """
        Set the OutValue for all connections

        Sets the OutValue for all connections. This will transmit the value
        to all connected clients using packets. The value will become the clients'
        InValue.

        The value will be returned when clients call Wire.PeekInValue() or
        Wire.AsyncPeekInValue()

        """
        raise Exception("Write only property")

    @OutValue.setter
    def OutValue(self, value):
        m = PackMessageElement(value, self._type, self._obj)
        self._innerpipe.SetOutValue(m)

    @property
    def ActiveWireConnectionCount(self):
        """Get the number of active wire connections"""
        return self._innerpipe.GetActiveWireConnectionCount()

    def SetPredicate(self, f):
        """
        Set the predicate callback function

        A predicate is optionally used to regulate when values are sent to clients. This is used by the
        BroadcastDownsampler to regulate update rates of values sent to clients.

        The predicate callback is invoked before the broadcaster sends a value to an endpoint. If the predicate returns true,
        the value will be sent. If it is false, the value will not be sent to that endpoint.

        The predicate receives the broadcaster and the client endpoint ID. It returns true to send the value,
        or false to not send the value.

        :param f: The predicate
        :type f: Callable[[RobotRaconteur.WireBroadcaster,int],bool]
        """
        p = WrappedWireBroadcasterPredicateDirectorPython(f)
        self._innerpipe.SetPredicateDirector(p, 0)
        p.__disown__()

    @property
    def OutValueLifespan(self):
        """
        Set the lifespan of OutValue

        OutValue may optionally have a finite lifespan specified in seconds. Once
        the lifespan after sending has expired, the OutValue is cleared and becomes invalid.
        Attempts to access OutValue will result in ValueNotSetException.

        OutValue lifespans may be used to avoid using a stale value sent by the wire. If
        the lifespan is not set, the wire will continue to return the last sent value, even
        if the value is old.

        Specify -1 for no expiration.

        """
        t = self._innerpipe.GetOutValueLifespan()
        if t < 0:
            return t
        return float(t) / 1000.0

    @OutValueLifespan.setter
    def OutValueLifespan(self, secs):
        if secs < 0:
            self._innerpipe.SetOutValueLifespan(-1)
        else:
            self._innerpipe.SetOutValueLifespan(int(secs * 1000.0))

    @property
    def PeekInValueCallback(self):
        """
        Set the PeekInValue callback function for the WireBrodcaster.
        Overrides the default callback that returns the last set
        OutValue.

        :rtype: Callable[[int],T]
        """
        raise Exception("Write only property")

    @PeekInValueCallback.setter
    def PeekInValueCallback(self, c):
        cb = WrappedWireBroadcasterPeekValueDirectorImpl(
            c, self._type, self._node, self._obj)
        self._innerpipe.SetPeekInValueCallback(cb, 0)
        cb.__disown__()


class WrappedWireUnicastReceiverInValueChangedImpl(RobotRaconteurPython.WrappedWireServerPokeValueDirector):
    def __init__(self, rec):
        super(WrappedWireUnicastReceiverInValueChangedImpl, self).__init__()
        self._rec = weakref.ref(rec)

    def PokeValue(self, m, ts, ep):
        rec = self._rec()
        if (rec is None):
            return
        value = UnpackMessageElement(
            m, rec._wire._innerpipe.Type, None, rec._wire._innerpipe.GetNode())
        rec.InValueChanged.fire(value, ts, ep)


class WireUnicastReceiver(object):
    """
    Receive the InValue from the most recent connection

    WireUnicastReceiver is used by services to receive a value from a single client.
    When a client sets its OutValue, this value is transmitted to the service using
    packets, and becomes the service's InValue for that connection. Service wires
    can have multiple active clients, so the service needs to choose which connection
    is "active". The WireUnicastReceiver selects the "most recent" connection, and
    returns that connection's InValue. Any existing connections are closed.
    WireUnicastReceiver should only be used with wires that are declared *writeonly*.
    It is recommended that object locks be used to protect from concurrent
    access when unicast receivers are used.

    WireUnicastReceiver is initialized by the user, or by default if the
    service object does not contain an attribute for the wire member. The default
    will automatically instantiate unicast receivers for
    wires marked *writeonly*. If default the implementation is not
    not used, the unicast receiver must be instantiated manually. It is recommended this
    be done using an RRServiceObjectInit() function in the service object.
    This function is called after the wires have been instantiated by the service.

    The current InValue is received using the InValue property or TryGetInValue().
    The InValueChanged event hook can be used to monitor for changes to the InValue.

    Clients may also use PokeOutValue() or AsyncPokeOutValue() to update the
    unicast receiver's value.

    :param wire: The wire to use for receiving. Must be a wire from a service object.
     Specifying a client wire will result in an exception.
    :type wire: RobotRaconteur.Wire
    """
    __slots__ = ["_wire", "_innerpipe", "_obj",
                 "_type", "_InValueChanged", "__weakref__"]

    def __init__(self, wire):
        self._wire = wire
        self._innerpipe = RobotRaconteurPython.WrappedWireUnicastReceiver()
        self._innerpipe.Init(wire._innerpipe)
        self._obj = wire._obj
        self._type = wire._innerpipe.Type
        self._InValueChanged = EventHook()
        cb = WrappedWireUnicastReceiverInValueChangedImpl(self)
        self._innerpipe.AddInValueChangedListener(cb, 0)
        cb.__disown__()

    @property
    def InValue(self):
        """
        Get the current InValue

        Gets the current InValue that was received from the active connection.
        Throws ValueNotSetException if no value has been received, or
        the most recent value lifespan has expired.

        :rtype: Tuple[T,RobotRaconteur.TimeStamp]
        """
        ts = RobotRaconteurPython.TimeSpec()
        (m, ep) = self._innerpipe.GetInValue(ts)
        return (UnpackMessageElement(m, self._type, self._obj, self._wire._innerpipe.GetNode()), ts)

    def TryGetInValue(self):
        """
        Try getting the current InValue, returning true on success or false on failure

        Gets the current InValue, its timestamp, and the client that owns the connection.
        Returns True if value is valid, or False if value is invalid. Value will be invalid if no value has
        been received, or the value lifespan has expired.

        :rtype: Tuple[bool,T,RobotRaconteur.TimeSpec,object]
        """
        val = RobotRaconteurPython.WrappedService_typed_packet()
        ts = RobotRaconteurPython.TimeSpec()
        res = self._innerpipe.TryGetInValue(val, ts)
        if not res:
            return False, None, None, None
        return (True, UnpackMessageElement(val.packet, self._type, self._obj, self._wire._innerpipe.GetNode()), ts, val.client)

    @property
    def InValueChanged(self):
        """
        Event hook for wire value change. Use to add handlers to be called
        when the InValue changes.

        .. code-block:: python

           def my_handler(con, value, ts):
              # Handle new value
              pass

           my_wire_connection.WireValueChanged += my_handler

        Handler must have signature ``Callable[[T,RobotRaconteur.TimeSpec,int],None]``

        """
        return self._InValueChanged

    @InValueChanged.setter
    def InValueChanged(self, evt):
        if (evt is not self._InValueChanged):
            raise RuntimeError("Invalid operation")

    @property
    def InValueLifespan(self):
        """
        Set the lifespan of InValue

        InValue may optionally have a finite lifespan specified in seconds, or -1 for infinite.
        Once the lifespan after reception has expired, the InValue is cleared, and becomes invalid.
        Attempts to access InValue will result in a ValueNotSetException.

        :rtype: float
        """
        t = self._innerpipe.GetInValueLifespan()
        if t < 0:
            return t
        return float(t) / 1000.0

    @InValueLifespan.setter
    def InValueLifespan(self, secs):
        if secs < 0:
            self._innerpipe.SetInValueLifespan(-1)
        else:
            self._innerpipe.SetInValueLifespan(int(secs * 1000.0))


class BroadcastDownsamplerStep(object):
    """
    Class for automatic broadcast downsampler stepping

    Helper class to automate BroadcastDownsampler stepping.
    Calls BroadcastDownsampler.BeginStep() on construction,
    and BroadcastDownsampler.EndStep() on destruction.

    Use with a "with" block
    """
    __slots__ = ["downsampler"]

    def __init__(self, downsampler):
        self.downsampler = downsampler

    def __enter__(self):
        self.downsampler.BeginStep()

    def __exit__(self, etype, value, traceback):
        self.downsampler.EndStep()


class GeneratorClient(object):
    __slots__ = ["_inner_gen", "_obj", "_node",
                 "_return_type", "_param_type", "__weakref__"]

    def __init__(self, inner_gen, return_type, param_type, obj, node):
        self._inner_gen = inner_gen
        self._obj = obj
        self._node = node
        self._return_type = return_type
        self._param_type = param_type

    def _pack_param(self, param):
        param_type1 = RobotRaconteurPython.TypeDefinition()
        self._param_type.CopyTo(param_type1)
        param_type1.RemoveContainers()
        param1 = PackMessageElement(param, param_type1, self._obj, self._node)
        return param1

    def _unpack_return(self, ret):
        return_type1 = RobotRaconteurPython.TypeDefinition()
        self._return_type.CopyTo(return_type1)
        return_type1.RemoveContainers()
        return UnpackMessageElement(ret, return_type1, self._obj, self._node)

    def Next(self, param=None):
        if (self._param_type is not None and self._param_type.ContainerType == DataTypes_ContainerTypes_generator):
            param1 = self._pack_param(param)
        else:
            assert param is None
            param1 = None
        ret1 = self._inner_gen.Next(param1)
        return self._unpack_return(ret1)

    def TryNext(self, param=None):
        if (self._param_type is not None and self._param_type.ContainerType == DataTypes_ContainerTypes_generator):
            param1 = self._pack_param(param)
        else:
            assert param is None
            param1 = None
        gen_res = self._inner_gen.TryNext(param1)
        if not gen_res.res:
            return False, None
        return True, self._unpack_return(gen_res.value)

    def AsyncNext(self, param, handler, timeout=RobotRaconteurPython.RR_TIMEOUT_INFINITE):
        if (self._param_type is not None and self._param_type.ContainerType == DataTypes_ContainerTypes_generator):
            param1 = self._pack_param(param)
        else:
            assert param is None
            param1 = None

        return_type1 = RobotRaconteurPython.TypeDefinition()
        self._return_type.CopyTo(return_type1)
        return_type1.RemoveContainers()

        if (self._return_type.Type != RobotRaconteurPython.DataTypes_void_t):
            return async_call(self._inner_gen.AsyncNext, (param1, adjust_timeout(timeout)), AsyncRequestDirectorImpl, handler, directorargs=(False, return_type1, self._obj, self._node))
        else:
            return async_call(self._inner_gen.AsyncNext, (param1, adjust_timeout(timeout)), AsyncRequestDirectorImpl, handler, directorargs=(True, return_type1, self._obj, self._node))

    def Abort(self):
        self._inner_gen.Abort()

    def AsyncAbort(self, handler, timeout=RobotRaconteurPython.RR_TIMEOUT_INFINITE):
        return async_call(self._inner_gen.AsyncAbort, (adjust_timeout(timeout),), AsyncVoidReturnDirectorImpl, handler)

    def Close(self):
        self._inner_gen.Close()

    def AsyncClose(self, handler, timeout=RobotRaconteurPython.RR_TIMEOUT_INFINITE):
        return async_call(self._inner_gen.AsyncClose, (adjust_timeout(timeout),), AsyncVoidReturnDirectorImpl, handler)

    def NextAll(self):
        ret = []
        try:
            while True:
                ret.append(self.Next())
        except RobotRaconteurPythonError.StopIterationException:
            pass
        return ret

    # Add compatibility for iterator protocols
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
        super(AsyncGeneratorClientReturnDirectorImpl, self).__init__()
        self._handler = handler
        self._return_type = return_type
        self._param_type = param_type
        self._obj = obj
        self._node = node

    def handler(self, gen, error_info):
        if (error_info.error_code != 0):
            err = RobotRaconteurPythonError.RobotRaconteurExceptionUtil.ErrorInfoToException(
                error_info)
            self._handler(None, err)
            return

        gen2 = GeneratorClient(gen, self._return_type,
                               self._param_type, self._obj, self._node)
        self._handler(gen2, None)


class IteratorGenerator(object):
    """
    A generator that proxies Next() to Python iterators
    """
    __slots__ = ["_iter", "_lock", "_closed", "_aborted"]

    def __init__(self, obj):
        self._iter = iter(obj)
        self._lock = threading.Lock()
        self._closed = False
        self._aborted = False

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
            self._closed = True
            if hasattr(self._iter, "close"):
                self._iter.close()

    def Abort(self):
        with self._lock:
            self._aborted = True
            if hasattr(self._iter, "close"):
                self._iter.close()


class WrappedGeneratorServerDirectorPython(RobotRaconteurPython.WrappedGeneratorServerDirector):
    def __init__(self, gen, return_type, param_type, node):
        super(WrappedGeneratorServerDirectorPython, self).__init__()
        self._return_type = return_type
        self._param_type = param_type
        self._node = node
        if hasattr(gen, "Next") or hasattr(gen, "AsyncNext"):
            self._gen = gen
        else:
            if not hasattr(gen, "__iter__"):
                raise TypeError("Invalid generator")
            self._gen = IteratorGenerator(gen)

    def Next(self, m, async_adapter):

        try:
            async_next_func = getattr(self._gen, "AsyncNext")
        except AttributeError:
            pass
        else:
            if self._return_type is None:
                def async_next_handler(err):
                    if err is not None:
                        async_adapter.End(
                            RobotRaconteurPythonError.RobotRaconteurExceptionUtil.ExceptionToErrorInfo(err))
                        _print_exc_traceback(err)
                        return
                    async_adapter.End(
                        None, RobotRaconteurPython.HandlerErrorInfo())
                    return
            else:
                def async_next_handler(ret, err):
                    if err is not None:
                        async_adapter.End(
                            RobotRaconteurPythonError.RobotRaconteurExceptionUtil.ExceptionToErrorInfo(err))
                        _print_exc_traceback(err)
                        return
                    if (ret is None):
                        m = RobotRaconteurPython.MessageElement()
                        m.ElementName = "return"
                        m.ElementType = RobotRaconteurPython.DataTypes_void_t
                    else:
                        m = PackMessageElement(
                            ret, self._return_type, node=self._node)
                    async_adapter.End(
                        m, RobotRaconteurPython.HandlerErrorInfo())
                    return
            async_adapter.MakeAsync()
            if self._param_type is None:
                async_next_func(async_next_handler)
            else:
                param = UnpackMessageElement(
                    m, self._param_type, node=self._node)
                async_next_func(param, async_next_handler)
            return

        if self._param_type is None:
            ret = self._gen.Next()
        else:
            param = UnpackMessageElement(m, self._param_type, node=self._node)
            ret = self._gen.Next(param)

        if ret is None:
            mret = RobotRaconteurPython.MessageElement()
            mret.ElementName = "return"
            mret.ElementType = RobotRaconteurPython.DataTypes_void_t
            return mret

        return PackMessageElement(ret, self._return_type, node=self._node)

    def Abort(self, async_adapter):
        try:
            async_abort_func = getattr(self._gen, "AsyncAbort")
        except AttributeError:
            pass
        else:
            def async_abort_handler(err):
                if err is not None:
                    async_adapter.End(
                        RobotRaconteurPythonError.RobotRaconteurExceptionUtil.ExceptionToErrorInfo(err))
                    _print_exc_traceback(err)
                    return
                async_adapter.End(RobotRaconteurPython.HandlerErrorInfo())
                return
            async_adapter.MakeAsync()
            async_abort_func(async_abort_handler)
            return
        self._gen.Abort()

    def Close(self, async_adapter):
        try:
            async_close_func = getattr(self._gen, "AsyncClose")
        except AttributeError:
            pass
        else:
            def async_close_handler(err):
                if err is not None:
                    async_adapter.End(
                        RobotRaconteurPythonError.RobotRaconteurExceptionUtil.ExceptionToErrorInfo(err))
                    _print_exc_traceback(err)
                    return
                async_adapter.End(RobotRaconteurPython.HandlerErrorInfo())
                return
            async_adapter.MakeAsync()
            async_close_func(async_close_handler)
            return
        self._gen.Close()


_trace_hook = sys.gettrace()


class ServiceSubscriptionClientID(object):
    """
    ServiceSubscriptionClientID()

    ClientID for use with ServiceSubscription

    The ServiceSubscriptionClientID stores the NodeID
    and ServiceName of a connected service.
    """
    __slots__ = ["NodeID", "ServiceName"]

    def __init__(self, *args):
        self.NodeID = None
        """(RobotRaconteur.NodeID) The NodeID of the connected service"""
        self.ServiceName = None
        """(str) The ServiceName of the connected service"""

        if (len(args) == 1):
            self.NodeID = RobotRaconteurPython.NodeID(args[0].NodeID)
            self.ServiceName = args[0].ServiceName
        elif (len(args) == 2):
            self.NodeID = args[0]
            self.ServiceName = args[1]

    def __eq__(self, other):
        if (not hasattr(other, 'NodeID') or not hasattr(other, 'ServiceName')):
            return False
        return (self.NodeID == other.NodeID) and (self.ServiceName == other.ServiceName)

    def __neq__(self, other):
        return not self == other

    def __hash__(self):
        return hash((str(self.NodeID), self.ServiceName))

    def __str__(self):
        return str(self.NodeID) + "," + self.ServiceName


class ServiceSubscriptionFilterNode(object):
    """
    Subscription filter node information

    Specify a node by NodeID and/or NodeName. Also allows specifying
    username and password.

    When using username and credentials, secure transports and specified NodeID should
    be used. Using username and credentials without a transport that verifies the
    NodeID could result in credentials being leaked.
    """
    __slots__ = ["NodeID", "NodeName", "Username", "Credentials"]

    def __init__(self):
        self.NodeID = None
        """(RobotRaconteur.NodeID) The NodeID to match. All zero NodeID will match any NodeID."""
        self.NodeName = None
        """(str) The NodeName to match. Emtpy NodeName will match any NodeName."""
        self.Username = None
        """(str) The username to use for authentication. Should only be used with secure transports and verified NodeID"""
        self.Credentials = None
        """(Dict[str,Any]) The credentials to use for authentication. Should only be used with secure transports and verified NodeID"""


class ServiceSubscriptionFilter(object):
    """
    Subscription filter

    The subscription filter is used with RobotRaconteurNode.SubscribeServiceByType() and
    RobotRaconteurNode.SubscribeServiceInfo2() to decide which services should
    be connected. Detected services that match the service type are checked against
    the filter before connecting.
    """
    __slots__ = ["Nodes", "ServiceNames",
                 "TransportSchemes", "Predicate", "Attributes", "AttributesMatchOperation", "MaxConnections"]

    def __init__(self):
        self.Nodes = []
        """(List[RobotRaconteurServiceSubscriptionFilterNode]) List of nodes that should be connected. Empty means match any node."""
        self.ServiceNames = []
        """(List[str])  List of service names that should be connected. Empty means match any service name."""
        self.TransportSchemes = []
        """(List[str]) List of transport schemes. Empty means match any transport scheme."""
        self.Attributes = dict()
        """(Dict[str,RobotRaconteur.ServiceSubscriptionFilterAttributeGroup] Attributes to match)"""
        self.AttributesMatchOperation = RobotRaconteurPython.ServiceSubscriptionFilterAttributeGroupOperation_AND
        """(RobotRaconteur.ServiceSubscriptionFilterAttributeGroupOperation) The operation to use when matching attributes.
        Default is AND. Can be OR, AND, NOR, and NAND."""
        self.Predicate = None
        """(Callable[[RobotRaconteur.ServiceInfo2],bool]) A user specified predicate function. If nullptr, the predicate is not checked."""
        self.MaxConnections = 1000000
        """(int) The maximum number of connections the subscription will create. Zero means unlimited connections."""


class WrappedServiceInfo2SubscriptionDirectorPython(RobotRaconteurPython.WrappedServiceInfo2SubscriptionDirector):
    def __init__(self, subscription):
        super(WrappedServiceInfo2SubscriptionDirectorPython, self).__init__()
        self._subscription = weakref.ref(subscription)

    def ServiceDetected(self, subscription, id, info):
        s = self._subscription()
        if (s is None):
            return
        try:
            s.ServiceDetected.fire(
                s, ServiceSubscriptionClientID(id), ServiceInfo2(info))
        except:
            pass

    def ServiceLost(self, subscription, id, info):
        s = self._subscription()
        if (s is None):
            return
        try:
            s.ServiceLost.fire(
                s, ServiceSubscriptionClientID(id), ServiceInfo2(info))
        except:
            pass


class ServiceInfo2Subscription(object):
    """
    ServiceInfo2Subscription()

    Subscription for information about detected services

    Created using RobotRaconteurNode.SubscribeServiceInfo2()

    The ServiceInfo2Subscription class is used to track services with a specific service type as they are
    detected on the local network and when they are lost. The currently detected services can also
    be retrieved. The service information is returned using the ServiceInfo2 structure.
    """
    __slots__ = ["_subscription", "_ServiceDetected",
                 "_ServiceLost", "__weakref__"]

    def __init__(self, subscription):
        self._subscription = subscription
        self._ServiceDetected = EventHook()
        self._ServiceLost = EventHook()
        director = WrappedServiceInfo2SubscriptionDirectorPython(self)
        subscription.SetRRDirector(director, 0)
        director.__disown__()

    def GetDetectedServiceInfo2(self):
        """
        Returns a dict of detected services.

        The returned dict contains the detected nodes as ServiceInfo2. The dict
        is keyed with ServiceSubscriptionClientID.

        This function does not block.

        :return: The detected services.
        :rtype: Dict[RobotRaconteur.ServiceSubscriptionClientID,ServiceInfo2]
        """
        o = dict()
        c1 = self._subscription.GetDetectedServiceInfo2()
        for c2 in c1.items():
            id1 = ServiceSubscriptionClientID(c2[0])
            stub = ServiceInfo2(c2[1])
            o[id1] = stub
        return o

    def Close(self):
        """
        Close the subscription

        Closes the subscription. Subscriptions are automatically closed when the node is shut down.
        """
        self._subscription.Close()

    @property
    def ServiceDetected(self):
        """
        Event hook for service detected events. Use to add handlers to be called
        when a service is detected.

        .. code-block:: python

           def my_handler(sub, subscription_id, service_info2):
              # Process detected service
              pass

           my_serviceinfo2_sub.ServiceDetected += my_handler

        Handler must have signature ``Callable[[RobotRaconteur.ServiceInfo2Subscription,RobotRaconteur.ServiceSubscriptionClientID,RobotRaconteur.ServiceInfo2],None]``

        :rtype: RobotRaconteur.EventHook
        """
        return self._ServiceDetected

    @ServiceDetected.setter
    def ServiceDetected(self, evt):
        if (evt is not self._ServiceDetected):
            raise RuntimeError("Invalid operation")

    @property
    def ServiceLost(self):
        """
        Event hook for service lost events. Use to add handlers to be called
        when a service is lost.

        .. code-block:: python

           def my_handler(sub, subscription_id, service_info2):
              # Process lost service
              pass

           my_serviceinfo2_sub.ServiceLost += my_handler

        Handler must have signature ``Callable[[RobotRaconteur.ServiceInfo2Subscription,RobotRaconteur.ServiceSubscriptionClientID,RobotRaconteur.ServiceInfo2],None]``

        :rtype: RobotRaconteur.EventHook
        """
        return self._ServiceLost

    @ServiceLost.setter
    def ServiceLost(self, evt):
        if (evt is not self._ServiceLost):
            raise RuntimeError("Invalid operation")

    def GetNode(self):
        return self._subscription.GetNode()


class WrappedServiceSubscriptionDirectorPython(RobotRaconteurPython.WrappedServiceSubscriptionDirector):
    def __init__(self, subscription):
        super(WrappedServiceSubscriptionDirectorPython, self).__init__()
        self._subscription = weakref.ref(subscription)

    def ClientConnected(self, subscription, id, client):
        s = self._subscription()
        if (s is None):
            return

        client2 = s._GetClientStub(client)

        try:
            s.ClientConnected.fire(s, ServiceSubscriptionClientID(id), client2)
        except:
            traceback.print_exc()

    def ClientDisconnected(self, subscription, id, client):
        s = self._subscription()
        if (s is None):
            return

        client2 = s._GetClientStub(client)

        try:
            s.ClientDisconnected.fire(
                s, ServiceSubscriptionClientID(id), client2)
        except:
            traceback.print_exc()

    def ClientConnectFailed(self, subscription, id, url, error_info):
        s = self._subscription()
        if (s is None):
            return

        err = RobotRaconteurPythonError.RobotRaconteurExceptionUtil.ErrorInfoToException(
            error_info)

        try:
            s.ClientConnectFailed.fire(
                s, ServiceSubscriptionClientID(id), list(url), err)
        except:
            traceback.print_exc()


class ServiceSubscription(object):
    """
    ServiceSubscription()

    Subscription that automatically connects services and manages lifecycle of connected services

    Created using RobotRaconteurNode.SubscribeService() or RobotRaconteurNode.SubscribeServiceByType(). The ServiceSubscription
    class is used to automatically create and manage connections based on connection criteria. RobotRaconteur.SubscribeService()
    is used to create a robust connection to a service with a specific URL. RobotRaconteurNode.SubscribeServiceByType() is used
    to connect to services with a specified type, filtered with a ServiceSubscriptionFilter. Subscriptions will create connections
    to matching services, and will retry the connection if it fails or the connection is lost. This behavior allows subscriptions
    to be used to create robust connections. The retry delay for connections can be modified using the ConnectRetryDelay property.

    The currently connected clients can be retrieved using the GetConnectedClients() function. A single "default client" can be
    retrieved using the GetDefaultClient() function or TryGetDefaultClient() functions. Listeners for client connect and
    disconnect events can be added using the ClientConnectListener and ClientDisconnectListener properties. If
    the user wants to claim a client, the ClaimClient() and ReleaseClient() functions will be used. Claimed clients will
    no longer have their lifecycle managed by the subscription.

    Subscriptions can be used to create \"pipe\" and \"wire\" subscriptions. These member subscriptions aggregate
    the packets and values being received from all services. They can also act as a "reverse broadcaster" to
    send packets and values to all services that are actively connected. See PipeSubscription and WireSubscription.
    """

    def __init__(self, subscription):
        self._subscription = subscription
        self._ClientConnected = EventHook()
        self._ClientDisconnected = EventHook()
        self._ClientConnectFailed = EventHook()
        director = WrappedServiceSubscriptionDirectorPython(self)
        subscription.SetRRDirector(director, 0)
        director.__disown__()

    def _GetClientStub(self, innerstub):
        if (innerstub is None):
            return None
        s2 = innerstub.GetPyStub()
        if (s2 is not None):
            return s2
        return InitStub(innerstub)

    def GetConnectedClients(self):
        """
        Returns a dict of connected clients

        The returned dict contains the connect clients. The dict
        is keyed with ServiceSubscriptionClientID.

        Clients must be cast to a type, similar to the client returned by
        RobotRaconteurNode.ConnectService().

        Clients can be "claimed" using ClaimClient(). Once claimed, the subscription
        will stop managing the lifecycle of the client.

        This function does not block.

        :rtype: Dict[RobotRaconteur.ServiceSubscriptionClientID,Any]
        """
        o = dict()
        c1 = self._subscription.GetConnectedClients()
        for c2 in c1.items():
            id1 = ServiceSubscriptionClientID(c2[0])
            stub = self._GetClientStub(c2[1])
            o[id1] = stub
        return o

    def Close(self):
        """
        Close the subscription

        Closes the subscription. Subscriptions are automatically closed when the node is shut down.
        """
        self._subscription.Close()

    @property
    def ConnectRetryDelay(self):
        """
        Set the connect retry delay in seconds

        Default is 2.5 seconds

        :rtype: float
        """
        return self._subscription.GetConnectRetryDelay() / 1000.0

    @ConnectRetryDelay.setter
    def ConnectRetryDelay(self, value):
        if (value < 1):
            raise Exception("Invalid ConnectRetryDelay value")
        self._subscription.SetConnectRetryDelay(int(value * 1000.0))

    def SubscribeWire(self, wire_name, service_path=None):
        """
        Creates a wire subscription

        Wire subscriptions aggregate the value received from the connected services. It can also act as a
        "reverse broadcaster" to send values to clients. See WireSubscription.

        The optional service path may be an empty string to use the root object in the service. The first level of the
        service path may be "*" to match any service name. For instance, the service path "*.sub_obj" will match
        any service name, and use the "sub_obj" objref.

        :param membername: The member name of the wire
        :type membername: str
        :param servicepath: The service path of the object owning the wire member
        :type servicepath: str
        :return: The wire subscription
        :rtype: RobotRaconteur.WireSubscription
        """
        if service_path is None:
            service_path = ""
        s = self._subscription.SubscribeWire(wire_name, service_path)
        return WireSubscription(s)

    def SubscribePipe(self, pipe_name, service_path=None):
        """
        Creates a pipe subscription

        Pipe subscriptions aggregate the packets received from the connected services. It can also act as a
        "reverse broadcaster" to send packets to clients. See PipeSubscription.

        The optional service path may be an empty string to use the root object in the service. The first level of the
        service path may be "*" to match any service name. For instance, the service path "*.sub_obj" will match
        any service name, and use the "sub_obj" objref.

        :param membername: The member name of the pipe
        :type membername: str
        :param servicepath: The service path of the object owning the pipe member
        :type servicepath: str
        :return: The pipe subscription
        :rtype: RobotRaconteur.PipeSubscription
        """
        if service_path is None:
            service_path = ""
        s = self._subscription.SubscribePipe(pipe_name, service_path)
        return PipeSubscription(s)

    def SubscribeSubObject(self, service_path):
        """
        Creates a sub object subscription.

        Sub objects are objects within a service that are not the root object. Sub objects are typically
        referenced using objref members, however they can also be referenced using a service path.
        The SubObjectSubscription class is used to automatically access sub objects of the default client.

        The service path is broken up into segments using periods. See the Robot Raconter
        documentation for more information. The BuildServicePath() function can be used to assist
        building service paths. The first level of the* service path may be "*" to match any service name.
        For instance, the service path "*.sub_obj" will match any service name, and use the "sub_obj" objref

        :param service_path: The service path of the object
        :type service_path: str
        :return: The sub object subscription
        :rtype: SubObjectSubscription
        """

        s = self._subscription.SubscribeSubObject(service_path)
        return SubObjectSubscription(self, s)

    @property
    def ClientConnected(self):
        """
        Event hook for client connected events. Use to add handlers to be called
        when a client is connected.

        .. code-block:: python

           def my_handler(sub, subscription_id, connected_service):
              # Process lost service
              pass

           my_service_sub.ClientConnected += my_handler

        Handler must have signature ``Callable[[RobotRaconteur.ServiceInfo2Subscription,RobotRaconteur.ServiceSubscriptionClientID,T],None]``

        :rtype: RobotRaconteur.EventHook
        """
        return self._ClientConnected

    @ClientConnected.setter
    def ClientConnected(self, evt):
        if (evt is not self._ClientConnected):
            raise RuntimeError("Invalid operation")

    @property
    def ClientDisconnected(self):
        """
        Event hook for client disconnected events. Use to add handlers to be called
        when a client is disconnected.

        .. code-block:: python

           def my_handler(sub, subscription_id, connected_service):
              # Process lost service
              pass

           my_service_sub.ClientDisconnected += my_handler

        Handler must have signature ``Callable[[RobotRaconteur.ServiceInfo2Subscription,RobotRaconteur.ServiceSubscriptionClientID,T],None]``

        :rtype: RobotRaconteur.EventHook
        """
        return self._ClientDisconnected

    @ClientDisconnected.setter
    def ClientDisconnected(self, evt):
        if (evt is not self._ClientDisconnected):
            raise RuntimeError("Invalid operation")

    @property
    def ClientConnectFailed(self):
        """
        Event hook for client client connect failed events. Used to receive
        notification of when a client connection was not successful, including
        the urls and resulting exceptions.

        .. code-block:: python

           def my_handler(sub, subscription_id, candidate_urls, exceptions):
              # Process lost service
              pass

           my_service_sub.ClientDisconnected += my_handler

        Handler must have signature ``Callable[[RobotRaconteur.ServiceInfo2Subscription,RobotRaconteur.ServiceSubscriptionClientID,List[str],List[Exception]],None]``

        :rtype: RobotRaconteur.EventHook
        """
        return self._ClientConnectFailed

    @ClientConnectFailed.setter
    def ClientConnectFailed(self, evt):
        if (evt is not self._ClientConnectFailed):
            raise RuntimeError("Invalid operation")

    def GetDefaultClient(self):
        """
        Get the "default client" connection

        The "default client" is the "first" client returned from the connected clients map. This is effectively
        default, and is only useful if only a single client connection is expected. This is normally true
        for RobotRaconteurNode.SubscribeService()

        Clients using GetDefaultClient() should not store a reference to the client. It should instead
        call GetDefaultClient() right before using the client to make sure the most recenty connection
        is being used. If possible, SubscribePipe() or SubscribeWire() should be used so the lifecycle
        of pipes and wires can be managed automatically.

        :return: The client connection
        """
        return self._GetClientStub(self._subscription.GetDefaultClient())

    def TryGetDefaultClient(self):
        """
        Try getting the "default client" connection

        Same as GetDefaultClient(), but returns a bool success instead of throwing
        exceptions on failure.

        :return: Success and client (if successful) as a tuple
        :rtype: Tuple[bool,T]
        """
        res = self._subscription.TryGetDefaultClient()
        if not res.res:
            return False, None
        return True, self._GetClientStub(res.client)

    def GetDefaultClientWait(self, timeout=-1):
        """
        Get the "default client" connection, waiting with timeout if not connected

        The "default client" is the "first" client returned from the connected clients map. This is effectively
        default, and is only useful if only a single client connection is expected. This is normally true
        for RobotRaconteurNode.SubscribeService()

        Clients using GetDefaultClient() should not store a reference to the client. It should instead
        call GetDefaultClient() right before using the client to make sure the most recenty connection
        is being used. If possible, SubscribePipe() or SubscribeWire() should be used so the lifecycle
        of pipes and wires can be managed automatically.

        :param timeout: Timeout in seconds, or -1 for infinite
        :type timeout: float
        :return: The client connection
        """
        return self._GetClientStub(self._subscription.GetDefaultClientWait(adjust_timeout(timeout)))

    def TryGetDefaultClientWait(self, timeout=-1):
        """
        Try getting the "default client" connection, waiting with timeout if not connected

        Same as GetDefaultClientWait(), but returns a bool success instead of throwing
        exceptions on failure.

        :return: Success and client (if successful) as a tuple
        :rtype: Tuple[bool,T]
        """
        res = self._subscription.TryGetDefaultClientWait(
            adjust_timeout(timeout))
        if not res.res:
            return False, None
        return True, self._GetClientStub(res.client)

    def AsyncGetDefaultClient(self, handler, timeout=-1):
        """
        Asynchronously get the default client, with optional timeout

        Same as GetDefaultClientWait(), but returns asynchronously.

        If ``handler`` is None, returns an awaitable future.

        :param handler: The handler to call when default client is available, or times out
        :type handler: Callable[[bool,object],None]
        :param timeout: Timeout in seconds, or -1 for infinite
        :type timeout: float
        """
        return async_call(self._subscription.AsyncGetDefaultClient, (adjust_timeout(timeout),), AsyncStubReturnDirectorImpl, handler)

    def GetNode(self):
        return self._subscription.GetNode()

    @property
    def ServiceURL(self):
        """
        Get the service connection URL

                Returns the service connection URL. Only valid when subscription was created using
                RobotRaconteurNode.SubscribeService(). Will throw an exception if subscription
                was opened using RobotRaconteurNode.SubscribeServiceByType()
        """
        return list(self._subscription.GetServiceURL())

    def UpdateServiceURL(self, url, username=None, credentials=None, close_connected=False):
        """
        Update the service connection URL

                Updates the URL used to connect to the service. If close_connected is True,
                existing connections will be closed. If False, existing connections will not be closed.

        :param url: The new URL to use to connect to service
        :type url: Union[str,List[str]]
        :param username: An optional username for authentication
        :type username: str
        :param credentials: Optional credentials for authentication
        :type credentials: Dict[str,Any]
        :param close_connected: Optional, (default False) Close existing connections
        """

        if username is None:
            username = ""
        if credentials is not None:
            credentials = PackMessageElement(
                credentials, "varvalue{string}", None, self._subscription.GetNode()).GetData()
        self._subscription.UpdateServiceURL(
            url, username, credentials, "", close_connected)

    def UpdateServiceByType(self, service_types, filter_=None):
        """
        Update the service types and filter

        :param service_types: The new service types to use to connect to service
        :type service_types: Union[str,List[str]]
        :param filter_: Optional filter to use to connect to service
        :type filter_: Union[str,RobotRaconteurPython.ServiceSubscriptionFilter]
        """

        node = self._subscription.GetNode()
        filter2 = _SubscribeService_LoadFilter(node, filter_)

        service_types2 = RobotRaconteurPython.vectorstring()
        if (sys.version_info > (3, 0)):
            if (isinstance(service_types, str)):
                service_types2.append(service_types)
            else:
                for s in service_types:
                    service_types2.append(s)
        else:
            if (isinstance(service_types, (str, unicode))):
                service_types2.append(service_types)
            else:
                for s in service_types:
                    service_types2.append(s)

        self._subscription.UpdateServiceByType(service_types2, filter2)


class WrappedWireSubscriptionDirectorPython(RobotRaconteurPython.WrappedWireSubscriptionDirector):
    def __init__(self, subscription):
        super(WrappedWireSubscriptionDirectorPython, self).__init__()
        self._subscription = weakref.ref(subscription)

    def WireValueChanged(self, subscription, value, time):
        s = self._subscription()
        if (s is None):
            return

        try:
            v = RobotRaconteurPython._UnpackMessageElement(
                value.packet, value.type, value.stub, None)
            s.WireValueChanged.fire(s, v, time)
        except:
            traceback.print_exc()


class WireSubscription(object):
    """
    WireSubscription()

    Subscription for wire members that aggregates the values from client wire connections

    Wire subscriptions are created using the ServiceSubscription.SubscribeWire() function. This function takes the
    type of the wire value, the name of the wire member, and an optional service path of the service
    object that owns the wire member.

    Wire subscriptions aggregate the InValue from all active wire connections. When a client connects,
    the wire subscriptions will automatically create wire connections to the wire member specified
    when the WireSubscription was created using ServiceSubscription.SubscribeWire(). The InValue of
    all the active wire connections are collected, and the most recent one is used as the current InValue
    of the wire subscription. The current value, the timespec, and the wire connection can be accessed
    using GetInValue() or TryGetInValue().

    The lifespan of the InValue can be configured using the InValueLifeSpan property. It is recommended that
    the lifespan be configured, so that the value will expire if the subscription stops receiving
    fresh in values.

    The wire subscription can also be used to set the OutValue of all active wire connections. This behaves
    similar to a "reverse broadcaster", sending the same value to all connected services.
    """
    __slots__ = ["_subscription", "_WireValueChanged", "__weakref__"]

    def __init__(self, subscription):
        self._subscription = subscription
        director = WrappedWireSubscriptionDirectorPython(self)
        subscription.SetRRDirector(director, 0)
        director.__disown__()
        self._WireValueChanged = EventHook()

    def _UnpackValue(self, m):
        return RobotRaconteurPython._UnpackMessageElement(m.packet, m.type, m.stub, None)

    @property
    def InValue(self):
        """
        Get the current InValue

        Throws ValueNotSetException if no valid value is available

        :return: The current InValue
        """
        return self._UnpackValue(self._subscription.GetInValue())

    @property
    def InValueWithTimeSpec(self):
        """
        Get the current InValue and TimeSpec

        Throws ValueNotSetException if no valid value is available

        :return: The current InValue and TimeSpec
        :rtype: Tuple[T,RobotRaconteur.TimeSpec]
        """
        t = RobotRaconteurPython.TimeSpec()
        m = self._subscription.GetInValue(t)
        return (self._UnpackValue(m), t)

    def WaitInValueValid(self, timeout=-1):
        """
        Wait for a valid InValue to be received from a client

        Blocks the current thread until value is received or timeout

        :param timeout: The timeout in seconds
        :type timeout: float
        :return: True if value was received, otherwise False
        :rtype: bool
        """
        return self._subscription.WaitInValueValid(adjust_timeout(timeout))

    def TryGetInValue(self):
        """
        Try getting the current InValue and metadata

        Same as GetInValue(), but returns a bool for success or failure instead of throwing
        an exception.

        :return: Success and value (if successful)
        :rtype: Tuple[bool,T,RobotRaconteur.TimeSpec]
        """
        val = RobotRaconteurPython.WrappedService_typed_packet()
        t = RobotRaconteurPython.TimeSpec()
        res = self._subscription.TryGetInValue(val, t)
        if not res:
            return False, None, None
        return (True, self._UnpackValue(val), t)

    @property
    def ActiveWireConnectionCount(self):
        """
        Get the number of wire connections currently connected

        :rtype: int
        """
        return self._subscription.GetActiveWireConnectionCount()

    @property
    def IgnoreInValue(self):
        """
        Set if InValue should be ignored

        See WireConnection.IgnoreInValue

        If true, InValue will be ignored for all wire connections

        :rtype: bool
        """
        return self._subscription.GetIgnoreInValue()

    @IgnoreInValue.setter
    def IgnoreInValue(self, ignore):
        self._subscription.SetIgnoreInValue(ignore)

    @property
    def InValueLifespan(self):
        """
        Set the InValue lifespan in seconds

        Set the lifespan of InValue in seconds. The value will expire after
        the specified lifespan, becoming invalid. Use -1 for infinite lifespan.

        See also WireConnection.InValueLifespan

        :rtype: float
        """
        t = self._subscription.GetInValueLifespan()
        if t < 0:
            return t
        return float(t) / 1000.0

    @InValueLifespan.setter
    def InValueLifespan(self, secs):
        if secs < 0:
            self._subscription.SetInValueLifespan(-1)
        else:
            self._subscription.SetInValueLifespan(int(secs * 1000.0))

    def SetOutValueAll(self, value):
        """
        Set the OutValue for all active wire connections

        Behaves like a "reverse broadcaster". Calls WireConnection.OutValue
        for all connected wire connections.

        :param value: The value to send
        """
        iter = RobotRaconteurPython.WrappedWireSubscription_send_iterator(
            self._subscription)
        try:
            while iter.Next() is not None:
                m = PackMessageElement(value, iter.GetType(), iter.GetStub())
                iter.SetOutValue(m)
        finally:
            del iter

    def Close(self):
        """
        Closes the wire subscription

        Wire subscriptions are automatically closed when the parent ServiceSubscription is closed
        or when the node is shut down.
        """
        self._subscription.Close()

    @property
    def WireValueChanged(self):
        """
        Event hook for wire value change. Use to add handlers to be called
        when the InValue changes.

        .. code-block:: python

           def my_handler(sub, value, ts):
              # Handle new value
              pass

           my_wire_csub.WireValueChanged += my_handler

        Handler must have signature ``Callable[[RobotRaconteur.WireSubscription,T,RobotRaconteur.TimeSpec],None]``

        :rtype: RobotRaconteur.EventHook

        """
        return self._WireValueChanged

    @WireValueChanged.setter
    def WireValueChanged(self, evt):
        if (evt is not self._WireValueChanged):
            raise RuntimeError("Invalid operation")

    def GetNode(self):
        return self._subscription.GetNode()


class WrappedPipeSubscriptionDirectorPython(RobotRaconteurPython.WrappedPipeSubscriptionDirector):
    def __init__(self, subscription):
        super(WrappedPipeSubscriptionDirectorPython, self).__init__()
        self._subscription = weakref.ref(subscription)

    def PipePacketReceived(self, subscription):
        s = self._subscription()
        if (s is None):
            return

        try:
            s.PipePacketReceived.fire(s)
        except:
            traceback.print_exc()


class PipeSubscription(object):
    """
    PipeSubscription()

    Subscription for pipe members that aggregates incoming packets from client pipe endpoints

    Pipe subscriptions are created using the ServiceSubscription.SubscribePipe() function. This function takes the
    the type of the pipe packets, the name of the pipe member, and an optional service path of the service
    object that owns the pipe member.

    Pipe subscriptions collect all incoming packets from connect pipe endpoints. When a client connects,
    the pipe subscription will automatically connect a pipe endpoint the pipe endpoint specified when
    the PipeSubscription was created using ServiceSubscription.SubscribePipe(). The packets received
    from each of the collected pipes are collected and placed into a common receive queue. This queue
    is read using ReceivePacket(), TryReceivePacket(), or TryReceivePacketWait(). The number of packets
    available to receive can be checked using Available().

    Pipe subscriptions can also be used to send packets to all connected pipe endpoints. This is done
    with the AsyncSendPacketAll() function. This function behaves somewhat like a "reverse broadcaster",
    sending the packets to all connected services.

    If the pipe subscription is being used to send packets but not receive them, IgnoreInValue
    should be set to true to prevent packets from queueing.
    """
    __slots__ = ["_subscription", "_PipePacketReceived", "__weakref__"]

    def __init__(self, subscription):
        self._subscription = subscription
        director = WrappedPipeSubscriptionDirectorPython(self)
        subscription.SetRRDirector(director, 0)
        director.__disown__()
        self._PipePacketReceived = EventHook()

    def _UnpackValue(self, m):
        return RobotRaconteurPython._UnpackMessageElement(m.packet, m.type, m.stub, None)

    def ReceivePacket(self):
        """
        Dequeue a packet from the receive queue

        If the receive queue is empty, an InvalidOperationException() is thrown

        :return: The dequeued packet
        """
        return self._UnpackValue(self._subscription.ReceivePacket())

    def TryReceivePacket(self):
        """
        Try dequeuing a packet from the receive queue

        Same as ReceivePacket(), but returns a bool for success or failure instead of throwing
        an exception

        :return: Success and packet (if successful)
        :rtype: Tuple[bool,T]
        """
        return self.TryReceivePacketWait(0)

    def TryReceivePacketWait(self, timeout=-1, peek=False):
        """
        Try dequeuing a packet from the receive queue, optionally waiting or peeking the packet

        :param timeout: The time to wait for a packet to be received in seconds if the queue is empty, or -1 to wait forever
        :type timeout: float
        :param peek: If True, the packet is returned, but not dequeued. If False, the packet is dequeued
        :type peek: bool
        :return: Success and packet (if successful)
        :rtype: Tuple[bool,T]
        """
        val = RobotRaconteurPython.WrappedService_typed_packet()
        res = self._subscription.TryReceivePacketWait(
            val, adjust_timeout(timeout), peek)
        if (not res):
            return (False, None)
        else:
            return (True, self._UnpackValue(val))

    @property
    def Available(self):
        """
        Get the number of packets available to receive

        Use ReceivePacket(), TryReceivePacket(), or TryReceivePacketWait() to receive the packet

        :rtype: int
        """
        return self._subscription.Available()

    def AsyncSendPacketAll(self, packet):
        """
        Sends a packet to all connected pipe endpoints

        Calls AsyncSendPacket() on all connected pipe endpoints with the specified value.
        Returns immediately, not waiting for transmission to complete.

        :param packet: The packet to send
        """
        iter = RobotRaconteurPython.WrappedPipeSubscription_send_iterator(
            self._subscription)
        try:
            while iter.Next() is not None:
                m = PackMessageElement(packet, iter.GetType(), iter.GetStub())
                iter.AsyncSendPacket(m)
        finally:
            del iter

    @property
    def ActivePipeEndpointCount(self):
        """
        Get the number of pipe endpoints currently connected

        :rtype: int
        """
        return self._subscription.GetActivePipeEndpointCount()

    def Close(self):
        """
        Closes the pipe subscription

        Pipe subscriptions are automatically closed when the parent ServiceSubscription is closed
        or when the node is shut down.
        """
        return self._subscription.Close()

    @property
    def PipePacketReceived(self):
        """
        Event hook for packet received. Use to add handlers to be called
        when the subscription receives a new packet.

        .. code-block:: python

           def my_handler(pipe_sub):
              while pipe_sub.Available > 0:
                  packet = pipe_sub.ReceivePacket()
                  # Handle new packet
                  pass

           my_pipe_sub.PipePacketReceived+= my_handler

        Handler must have signature ``Callable[[RobotRaconteur.PipeSubscription],None]``

        :rtype: RobotRaconteur.EventHook

        """
        return self._PipePacketReceived

    @PipePacketReceived.setter
    def PipePacketReceived(self, evt):
        if (evt is not self._PipePacketReceived):
            raise RuntimeError("Invalid operation")

    def GetNode(self):
        return self._subscription.GetNode()


class SubObjectSubscription(object):
    """
    Subscription for sub objects of the default client.

    SubObjectSubscription is used to access sub objects of the default client. Sub objects are objects within a service
    that are not the root object. Sub objects are typically referenced using objref members, however they can also be
    referenced using a service path. The SubObjectSubscription class is used to automatically access sub objects of the
    default client.

    Use ServiceSubscription.SubscribeSubObject() to create a SubObjectSubscription.

    This class should not be used to access Pipe or Wire members. Use the ServiceSubscription.SubscribePipe() and
    ServiceSubscription.SubscribeWire() functions to access Pipe and Wire members.
    """

    def __init__(self, parent, subscription):
        self._parent = parent
        self._subscription = subscription

    def GetDefaultClient(self):
        """
        Get the "default client" sub object.

        The sub object is retrieved from the default client. The default client is the first client
        that connected to the service. If no clients are currently connected, an exception is thrown.

        Clients using GetDefaultClient() should not store a reference to the client. Call GetDefaultClient()
        each time the client is needed.

        :return: The sub object
        :rtype: T
        """
        return self._parent._GetClientStub(self._subscription.GetDefaultClient())

    def TryGetDefaultClient(self):
        """
        Try getting the "default client" sub object.

        Same as GetDefaultClient(), but returns a bool success instead of throwing
        exceptions on failure.

        :return: Success and client (if successful) as a tuple
        :rtype: Tuple[bool,T]
        """
        res = self._subscription.TryGetDefaultClient()
        if not res.res:
            return False, None
        return True, self._parent._GetClientStub(res.client)

    def GetDefaultClientWait(self, timeout=-1):
        """
        Get the "default client" sub object, waiting with timeout if not connected

        The sub object is retrieved from the default client. The default client is the first client
        that connected to the service. If no clients are currently connected, an exception is thrown.

        Clients using GetDefaultClient() should not store a reference to the client. Call GetDefaultClient()
        each time the client is needed.

        :param timeout: Timeout in seconds, or -1 for infinite
        :type timeout: float
        :return: The sub object
        """
        return self._parent._GetClientStub(self._subscription.GetDefaultClientWait(adjust_timeout(timeout)))

    def TryGetDefaultClientWait(self, timeout=-1):
        """
        Try getting the "default client" sub object, waiting with timeout if not connected

        Same as GetDefaultClientWait(), but returns a bool success instead of throwing
        exceptions on failure.

        :return: Success and client (if successful) as a tuple
        :rtype: Tuple[bool,T]
        """
        res = self._subscription.TryGetDefaultClientWait(adjust_timeout(timeout))
        if not res.res:
            return False, None
        return True, self._parent._GetClientStub(res.client)

    def AsyncGetDefaultClient(self, handler, timeout=-1):
        """
        Asynchronously get the default client, with optional timeout

        Same as GetDefaultClientWait(), but returns asynchronously.

        If ``handler`` is None, returns an awaitable future.

        :param handler: The handler to call when default client is available, or times out
        :type handler: Callable[[bool,object],None]
        :param timeout: Timeout in seconds, or -1 for infinite
        :type timeout: float
        """
        return async_call(self._subscription.AsyncGetDefaultClient, (adjust_timeout(timeout),), AsyncStubReturnDirectorImpl, handler)

    def GetNode(self):
        return self._subscription.GetNode()


class WrappedServiceSubscriptionFilterPredicateDirectorPython(RobotRaconteurPython.WrappedServiceSubscriptionFilterPredicateDirector):
    def __init__(self, f):
        super(WrappedServiceSubscriptionFilterPredicateDirectorPython, self).__init__()
        self._f = f

    def Predicate(self, info):
        info2 = ServiceInfo2(info)
        return self._f(info2)


def _SubscribeService_LoadFilter(node, filter_):
    filter2 = None
    if (filter_ is not None):
        filter2 = RobotRaconteurPython.WrappedServiceSubscriptionFilter()
        if (filter_.ServiceNames is not None):
            for s in filter_.ServiceNames:
                filter2.ServiceNames.append(s)
        if (filter_.TransportSchemes is not None):
            for s in filter_.TransportSchemes:
                filter2.TransportSchemes.append(s)
        filter2.MaxConnections = filter_.MaxConnections
        if (filter_.Attributes is not None):
            for n, v in filter_.Attributes.items():
                filter2.Attributes[n] = v
        filter2.AttributesMatchOperation = filter_.AttributesMatchOperation
        if (filter_.Nodes is not None):
            nodes2 = RobotRaconteurPython.vectorptr_wrappedservicesubscriptionnode()
            for n1 in filter_.Nodes:
                if (n1 is None):
                    continue
                n2 = RobotRaconteurPython.WrappedServiceSubscriptionFilterNode()
                if (n1.NodeID is not None):
                    n2.NodeID = n1.NodeID
                if (n1.NodeName is not None):
                    n2.NodeName = n1.NodeName
                if (n1.Username is not None):
                    n2.Username = n1.Username
                if (n1.Credentials is not None):
                    n2.Credentials = PackMessageElement(
                        n1.Credentials, "varvalue{string}", None, node).GetData()

                nodes2.append(n2)
            filter2.Nodes = nodes2

        if (filter_.Predicate is not None):
            director = WrappedServiceSubscriptionFilterPredicateDirectorPython(
                filter_.Predicate)
            filter2.SetRRPredicateDirector(director, 0)
            director.__disown__()
    return filter2


def SubscribeServiceInfo2(node, service_types, filter_=None):

    filter2 = _SubscribeService_LoadFilter(node, filter_)

    service_types2 = RobotRaconteurPython.vectorstring()
    if (sys.version_info > (3, 0)):
        if (isinstance(service_types, str)):
            service_types2.append(service_types)
        else:
            for s in service_types:
                service_types2.append(s)
    else:
        if (isinstance(service_types, (str, unicode))):
            service_types2.append(service_types)
        else:
            for s in service_types:
                service_types2.append(s)

    sub1 = RobotRaconteurPython.WrappedSubscribeServiceInfo2(
        node, service_types2, filter2)
    return ServiceInfo2Subscription(sub1)


def SubscribeServiceByType(node, service_types, filter_=None):

    filter2 = _SubscribeService_LoadFilter(node, filter_)

    service_types2 = RobotRaconteurPython.vectorstring()
    if (sys.version_info > (3, 0)):
        if (isinstance(service_types, str)):
            service_types2.append(service_types)
        else:
            for s in service_types:
                service_types2.append(s)
    else:
        if (isinstance(service_types, (str, unicode))):
            service_types2.append(service_types)
        else:
            for s in service_types:
                service_types2.append(s)

    sub1 = RobotRaconteurPython.WrappedSubscribeServiceByType(
        node, service_types2, filter2)
    return ServiceSubscription(sub1)


def SubscribeService(node, *args):
    args2 = list(args)
    if (len(args) >= 3):
        if (args[1] == None):
            args2[1] = ""
        args2[2] = PackMessageElement(
            args[2], "varvalue{string}", None, node).GetData()
    sub1 = RobotRaconteurPython.WrappedSubscribeService(node, *args2)
    return ServiceSubscription(sub1)


class ServiceSubscriptionManagerDetails(object):
    """
    ServiceSubscriptionManager subscription connection information

    Contains the connection information for a ServiceSubscriptionManager subscription
    and the local name of the subscription
    """
    __slots__ = ["Name", "ConnectionMethod", "Urls", "UrlUsername",
                 "UrlCredentials", "ServiceTypes", "Filter", "Enabled"]

    def __init__(self,
                 Name=None,
                 ConnectionMethod=None,
                 Urls=None,
                 UrlUsername=None,
                 UrlCredentials=None,
                 ServiceTypes=None,
                 Filter=None,
                 Enabled=True
                 ):
        self.Name = Name
        """(str) The local name of the subscription"""
        self.ConnectionMethod = ConnectionMethod
        """(ServiceSubscriptionManager_CONNECTION_METHOD) The connection method used to connect to the service"""
        self.Urls = Urls
        """(List[str]) The URLs used to connect to the service"""
        self.UrlUsername = UrlUsername
        """(str) The username used to connect to the service"""
        self.UrlCredentials = UrlCredentials
        """(Dict[str,Any]) The credentials used to connect to the service"""
        self.ServiceTypes = ServiceTypes
        """(List[str]) The service types used to connect to the service"""
        self.Filter = Filter
        """(ServiceSubscriptionFilter) The filter used to connect to the service"""
        self.Enabled = Enabled
        """(bool) True if the subscription is enabled"""


def _ServiceSubscriptionManager_LoadDetails(node, details):
    if details is None:
        return None
    details2 = RobotRaconteurPython.WrappedServiceSubscriptionManagerDetails()
    if details.Name is not None:
        details2.Name = details.Name
    if details.ConnectionMethod is not None:
        details2.ConnectionMethod = details.ConnectionMethod
    if details.Urls is not None:
        for s in details.Urls:
            details2.Urls.append(s)
    if details.UrlUsername is not None:
        details2.UrlUsername = details.UrlUsername
    if details.UrlCredentials is not None:
        details2.UrlCredentials = PackMessageElement(
            details.UrlCredentials, "varvalue{string}", None, node).GetData()
    if details.ServiceTypes is not None:
        if isinstance(details.ServiceTypes, str):
            details2.ServiceTypes.append(details.ServiceTypes)
        else:
            for s in details.ServiceTypes:
                details2.ServiceTypes.append(s)
    if details.Filter is not None:
        details2.Filter = _SubscribeService_LoadFilter(node, details.Filter)
    details2.Enabled = details.Enabled
    return details2


class ServiceSubscriptionManager(object):
    """
    Class to manage multiple subscriptions to services

    ServiceSubscriptionManager is used to manage multiple subscriptions to services. Subscriptions
    are created using information contained in ServiceSubscriptionManagerDetails structures. The subscriptions
    can connect using URLs or service types. The subscriptions can be enabled or disabled, and can be
    closed.

    :param details: (optional) A list of ServiceSubscriptionManagerDetails structures
    :type details: List[ServiceSubscriptionManagerDetails]
    :param node: (optional) The RobotRaconteurNode to use
    :type node: RobotRaconteur.RobotRaconteurNode
    """

    def __init__(self, details=None, node=None):
        if node is None:
            node = RobotRaconteurPython.RobotRaconteurNode.s

        details2 = RobotRaconteurPython.vector_wrappedservicesubscriptionmanagerdetails()
        if details is not None:
            for d in details:
                details2.append(_ServiceSubscriptionManager_LoadDetails(node, d))

        self._subscription_manager = RobotRaconteurPython.WrappedServiceSubscriptionManager(details2, node)

        self._subscriptions = {}
        self._lock = threading.Lock()

    def AddSubscription(self, details):
        """
        Add a subscription to the manager

        :param details: The subscription to add
        :type details: ServiceSubscriptionManagerDetails
        """
        details2 = _ServiceSubscriptionManager_LoadDetails(self._subscription_manager.GetNode(), details)
        self._subscription_manager.AddSubscription(details2)

    def RemoveSubscription(self, name, close=True):
        """
        Remove a subscription from the manager

        :param name: The name of the subscription to remove
        :type name: str
        :param close: (default True) Close the subscription
        :type close: bool
        """
        with self._lock:
            self._subscription_manager.RemoveSubscription(name)
            if close:
                del self._subscriptions[name]

    def EnableSubscription(self, name):
        """
        Enable a subscription

        :param name: The name of the subscription to enable
        :type name: str
        """
        self._subscription_manager.EnableSubscription(name)

    def DisableSubscription(self, name, close=True):
        """
        Disable a subscription

        :param name: The name of the subscription to disable
        :type name: str
        """
        self._subscription_manager.DisableSubscription(name, close)

    def GetSubscription(self, name, force_create=False):
        """
        Get a subscription by name

        :param name: The name of the subscription to get
        :type name: str
        :param force_create: (default False) Create the subscription if it does not exist
        :type force_create: bool
        :return: The subscription
        :rtype: ServiceSubscription
        """
        with self._lock:
            sub = self._subscriptions.get(name, None)
            if sub is None:
                sub1 = self._subscription_manager.GetSubscription(name, force_create)
                if sub1 is None:
                    return None
                sub = ServiceSubscription(sub1)
                self._subscriptions[name] = sub
            return sub

    def IsConnected(self, name):
        """
        Check if a subscription is connected

        :param name: The name of the subscription to check
        :type name: str
        :return: True if the subscription is connected
        :rtype: bool
        """
        return self._subscription_manager.IsConnected(name)

    def IsEnabled(self, name):
        """
        Get if a subscription is enabled

        :param name: The name of the subscription to check
        :type name: str
        :return: True if the subscription is enabled
        :rtype: bool
        """
        return self._subscription_manager.IsEnabled(name)

    def Close(self, close_subscriptions=True):
        """
        Close the subscription manager

        :param close_subscriptions: (default True) Close all subscriptions
        :type close_subscriptions: bool
        """
        self._subscription_manager.Close(close_subscriptions)
        with self._lock:
            self._subscriptions.clear()

    @property
    def SubscriptionNames(self):
        """
        Get the names of all subscriptions

        :rtype: List[str]
        """
        return list(self._subscription_manager.GetSubscriptionNames())

    @property
    def SubscriptionDetails(self):
        """
        Get the details of all subscriptions

        :rtype: List[ServiceSubscriptionManagerDetails]
        """
        details = []
        for d in self._subscription_manager.GetSubscriptionDetails():
            d2 = ServiceSubscriptionManagerDetails()
            d2.Name = d.Name
            d2.ConnectionMethod = d.ConnectionMethod
            d2.Urls = list(d.Urls)
            d2.ServiceTypes = list(d.ServiceTypes)
            d2.Enabled = d.Enabled
            details.append(d2)
        return details


class WrappedUserAuthenticatorDirectorPython(RobotRaconteurPython.WrappedUserAuthenticatorDirector):
    def __init__(self, target):
        super(WrappedUserAuthenticatorDirectorPython, self).__init__()
        self.target = target

    def AuthenticateUser(self, username, credentials, context):

        c2 = UnpackMessageElement(credentials, "varvalue{string}")
        ret = self.target.AuthenticateUser(username, c2, context)
        return ret


def _UserAuthenticator_PackCredentials(credentials):
    return PackMessageElement(credentials, "varvalue{string}")


def ReadServiceDefinitionFile(servicedef_name):
    f_name = None
    if (os.path.isfile(servicedef_name)):
        f_name = servicedef_name
    elif (os.path.isfile(servicedef_name + '.robdef')):
        f_name = servicedef_name + '.robdef'
    elif not os.path.isabs(servicedef_name):
        p = os.getenv("ROBOTRACONTEUR_ROBDEF_PATH", None)
        if p is not None:
            p1 = p.split(os.pathsep)
            for p2 in p1:
                p3 = p2.strip()
                if (os.path.isfile(os.path.join(p3, servicedef_name))):
                    f_name = os.path.join(p3, servicedef_name)
                if (os.path.isfile(os.path.join(p3, servicedef_name + '.robdef'))):
                    f_name = os.path.join(p3, servicedef_name + '.robdef')

    if f_name is None:
        raise IOError("Service definition file %s not found" % servicedef_name)

    with codecs.open(f_name, 'r', 'utf-8-sig') as f:
        return f.read()


def ReadServiceDefinitionFiles(servicedef_names, auto_import=False):

    d = []
    for servicedef_name in servicedef_names:
        d1 = RobotRaconteurPython.ServiceDefinition()
        d1.FromString(str(ReadServiceDefinitionFile(servicedef_name)))
        d.append(d1)

    if auto_import:
        missing_imports = set()
        d2 = {d3.Name: d3 for d3 in d}
        for k, v in d2.items():
            for imported in v.Imports:
                if imported not in d2:
                    missing_imports.add(imported)

        attempted_imports = set()
        while len(missing_imports) != 0:
            e = missing_imports.pop()
            d1 = RobotRaconteurPython.ServiceDefinition()
            d1.FromString(str(ReadServiceDefinitionFile(e)))
            d.append(d1)
            d2[d1.Name] = d1
            for imported in d1.Imports:
                if imported not in d2:
                    missing_imports.add(imported)
    return d


class RobotRaconteurNodeSetup(object):
    """
    Setup a node using specified options and manage node lifecycle

    RobotRaconteurNodeSetup and its subclasses ClientNodeSetup, ServerNodeSetup,
    and SecureServerNodeSetup are designed to help configure nodes and manage
    node lifecycles. The node setup classes use the "with" statement to configure the node
    on construction, and call RobotRaconteurNode.Shutdown() when the instance
    is destroyed.

    The node setup classes execute the following operations to configure the node:

    1. Set log level and tap options from flags, command line options, or environmental variables
    2. Register specified service factory types
    3. Initialize transports using flags specified in flags or from command line options
    4. Configure timeouts

    See Command Line Options for more information on available command line options.

    Logging level is configured using the environmental variable ``ROBOTRACONTEUR_LOG_LEVEL``
    or the command line option ``--robotraconteur-log-level``. See Logging for more information.

    See Taps for more information on using taps.

    The node setup classes optionally initialize LocalTransport,
    TcpTransport, HardwareTransport, and/or IntraTransport.
    Transports for more information.

    The LocalTransport.StartServerAsNodeName() or
    LocalTransport.StartClientAsNodeName() are used to load the NodeID.
    See LocalTransport for more information on this procedure.

    :param node_name: (optional) The NodeName
    :type node_name: str
    :param tcp_port: (optional) The port to listen for incoming TCP clients
    :type tcp_port: int
    :param flags: (optional) The configuration flags
    :type flags: int
    :param allowed_overrides: (optional) The allowed override flags
    :type allowed_overrides: int
    :param node: (optional) The node to configure and manage lifecycle
    :type node: RobotRaconteur.RobotRaconteurNode
    :param argv: (optional) The command line argument vector. Default is ``sys.argv``
    """
    __slots__ = ["__setup", "__node"]

    def __init__(self, node_name=None, tcp_port=None, flags=None, allowed_overrides=None, node=None, argv=None, config=None):
        if (config is not None):
            assert node_name is None and tcp_port is None and flags is None and allowed_overrides is None and argv is None
            self.__setup = RobotRaconteurPython.WrappedRobotRaconteurNodeSetup(
                config)
        else:
            if node_name is None:
                node_name = ""
            if tcp_port is None:
                tcp_port = 0
            if flags is None:
                flags = 0
            if node is None:
                node = RobotRaconteurPython.RobotRaconteurNode.s
            if allowed_overrides is None:
                allowed_overrides = 0
            if argv is None:
                argv = sys.argv
            self.__setup = RobotRaconteurPython.WrappedRobotRaconteurNodeSetup(node, node_name, tcp_port, flags, allowed_overrides,
                                                                               RobotRaconteurPython.vectorstring(argv))
        self.__node = node

    @property
    def tcp_transport(self):
        """The TcpTransport, will be None if TcpTransport is not specified in flags"""
        return self.__setup.GetTcpTransport()

    @property
    def local_transport(self):
        """The LocalTransport, will be None if LocalTransport is not specified in flags"""
        return self.__setup.GetLocalTransport()

    @property
    def hardware_transport(self):
        """The HardwareTransport, will be None if HardwareTransport is not specified in flags, Note: Hardware transport is not enabled by default"""
        return self.__setup.GetHardwareTransport()

    @property
    def intra_transport(self):
        """The IntraTransport, will be None if IntraTransport is not specified in flags"""
        return self.__setup.GetIntraTransport()

    @property
    def command_line_config(self):
        """The command line config parser object used to configure node"""
        return self.__setup.GetCommandLineConfig()

    def __enter__(self):
        return self

    def __exit__(self, etype, value, traceback):
        self.close()

    def close(self):
        """Shutdown the node and release the node from lifecycle management"""
        if self.__node is not None:
            self.__node.Shutdown()
            self.__node = None
        if self.__setup is not None:
            self.__setup.ReleaseNode()
            self.__setup = None

    def ReleaseNode(self):
        """
        Release the node from lifecycle management

        If called, RobotRaconteurNode.Shutdown() will not
        be called when the node setup instance is destroyed
        """
        if self.__setup is None:
            return
        self.__node = None
        self.__setup.ReleaseNode()


class ClientNodeSetup(RobotRaconteurNodeSetup):
    """
    Initializes a RobotRaconteurNode instance to default configuration for a client only node

    ClientNodeSetup is a subclass of RobotRaconteurNodeSetup providing default configuration for a
    RobotRaconteurNode instance that is used only to create outgoing client connections.

    See CommandLineOptions for more information on available command line options.

    Note: String table and HardwareTransport are disabled by default. They can be enabled
    using command line options.

    By default, the configuration will do the following:

    1. Configure logging level from environmental variable or command line options. Defaults to `INFO` if
       not specified
    2. Configure tap if specified in command line options
    3. Register service types passed to service_types
    4. Start LocalTransport (default enabled)

       1. If `RobotRaconteurNodeSetupFlags_LOCAL_TRANSPORT_START_CLIENT` flag is specified, call
          LocalTransport.StartServerAsNodeName() with the specified node_name
       2. Start LocalTransport discovery listening if specified in flags or on command line (default enabled)
       3. Disable Message Format Version 4 (default enabled) and/or String Table (default disabled) if
          specified on command line
    5. Start TcpTransport (default enabled)

       1. Disable Message Format Version 4 (default enabled) and/or String Table
          (default disabled) if specified in flags or command line
       2. Start TcpTranport discovery listening (default enabled)
       3. Load TLS certificate and set if TLS is specified on command line (default disabled)
       4. Process WebSocket origin command line options
    6. Start HardwareTransport (default disabled)

       1. Disable Message Format Version 4 (default enabled) and/or String Table
          (default disabled) if specified in flags or command line
    7. Start IntraTransport (default enabled)

       1. Disable Message Format Version 4 (default enabled) and/or String Table
          (default disabled) if specified in flags or command line
    8. Disable timeouts if specified in flags or command line (default timeouts normal)

    Most users will not need to be concerned with these details, and can simply
    use the default configuration.

    :param node_name: (optional) The NodeName
    :type node_name: str
    :param node: (optional) The node to configure and manage lifecycle
    :type node: RobotRaconteur.RobotRaconteurNode
    :param argv: (optional) The command line argument vector. Default is ``sys.argv``
    """

    def __init__(self, node_name=None, node=None, argv=None):
        super(ClientNodeSetup, self).__init__(node_name, 0, RobotRaconteurPython.RobotRaconteurNodeSetupFlags_CLIENT_DEFAULT,
                                              RobotRaconteurPython.RobotRaconteurNodeSetupFlags_CLIENT_DEFAULT_ALLOWED_OVERRIDE, node, argv)


class ServerNodeSetup(RobotRaconteurNodeSetup):
    """
    Initializes a RobotRaconteurNode instance to default configuration for a server and client node

    ServerNodeSetup is a subclass of RobotRaconteurNodeSetup providing default configuration for a
    RobotRaconteurNode instance that is used as a server to accept incoming client connections
    and to initiate client connections.

    ServerNodeSetup requires a NodeName, and a TCP port if LocalTransport and TcpTransport
    are enabled (default behavior).

    See Command Line Options for more information on available command line options.

    Note: String table and HardwareTransport are disabled by default. They can be enabled
    using command line options.

    By default, the configuration will do the following:

    1. Configure logging level from environmental variable or command line options. Defaults to `INFO` if
    not specified
    2. Configure tap if specified in command line options
    3. Register service types passed to service_types
    4. Start LocalTransport (default enabled)

       1. Configure the node to use the specified NodeName, and load the NodeID from the filesystem based
          based on the NodeName. NodeID will be automatically generated if not previously used.

          1. If "public" option is set, the transport will listen for all local users (default disabled)
       2. Start the LocalTransport server to listen for incoming connections with the specified NodeName and NodeID
       3. Start LocalTransport discovery announce and listening (default enabled)
       4. Disable Message Format Version 4 (default enabled) and/or String Table (default disabled) if
          specified on command line
    5. Start TcpTransport (default enabled)

       1. Start the TcpTransport server to listen for incoming connections on specified port
          or using the port sharer (default enabled using specified port)
       2. Disable Message Format Version 4 (default enabled) and/or String Table
          (default disabled) if specified in flags or command line
       3. Start TcpTranport discovery announce and listening (default enabled)
       4. Load TLS certificate and set if TLS is specified on command line (default disabled)
       5. Process WebSocket origin command line options
    6. Start HardwareTransport (default disabled)

       1. Disable Message Format Version 4 (default enabled) and/or String Table
          (default disabled) if specified in flags or command line
    7. Start IntraTransport (default enabled)

       1. Enable IntraTransport server to listen for incoming clients (default enabled)
       2. Disable Message Format Version 4 (default enabled) and/or String Table
          (default disabled) if specified in flags or command line
    8. Disable timeouts if specified in flags or command line (default timeouts normal)

    Most users will not need to be concerned with these details, and can simply
    use the default configuration.

    :param node_name: The NodeName
    :type node_name: str
    :param tcp_port: The port to listen for incoming TCP clients
    :type tcp_port: int
    :param node: (optional) The node to configure and manage lifecycle
    :type node: RobotRaconteur.RobotRaconteurNode
    :param argv: (optional) The command line argument vector. Default is ``sys.argv``
    """

    def __init__(self, node_name, tcp_port, node=None, argv=None):
        super(ServerNodeSetup, self).__init__(node_name, tcp_port, RobotRaconteurPython.RobotRaconteurNodeSetupFlags_SERVER_DEFAULT,
                                              RobotRaconteurPython.RobotRaconteurNodeSetupFlags_SERVER_DEFAULT_ALLOWED_OVERRIDE, node, argv)


class SecureServerNodeSetup(RobotRaconteurNodeSetup):
    """
    Initializes a RobotRaconteurNode instance to default configuration for a secure server and client node

    SecureServerNodeSetup is a subclass of RobotRaconteurNodeSetup providing default configuration for a
    secure RobotRaconteurNode instance that is used as a server to accept incoming client connections
    and to initiate client connections. SecureServerNodeSetup is identical to ServerNodeSetup,
    except that it requires TLS for all network communication.

    ServerNodeSetup requires a NodeName, and a TCP port if LocalTransport and TcpTransport
    are enabled (default behavior).

    See Command Line Options for more information on available command line options.

    Note: String table and HardwareTransport are disabled by default. They can be enabled
    using command line options.

    By default, the configuration will do the following:

    1. Configure logging level from environmental variable or command line options. Defaults to `INFO` if
    not specified
    2. Configure tap if specified in command line options
    3. Register service types passed to service_types
    4. Start LocalTransport (default enabled)

      1. Configure the node to use the specified NodeName, and load the NodeID from the filesystem based
         based on the NodeName. NodeID will be automatically generated if not previously used.

         1. If "public" option is set, the transport will listen for all local users (default disabled)
      2. Start the LocalTransport server to listen for incoming connections with the specified NodeName and NodeID
      3. Start LocalTransport discovery announce and listening (default enabled)
      4. Disable Message Format Version 4 (default enabled) and/or String Table (default disabled) if
         specified on command line
    5. Start TcpTransport (default enabled)

       1. Start the TcpTransport server to listen for incoming connections on specified port
          or using the port sharer (default enabled using specified port)
       2. Disable Message Format Version 4 (default enabled) and/or String Table
          (default disabled) if specified in flags or command line
       3. Start TcpTranport discovery announce and listening (default enabled)
       4. Load TLS certificate and set if TLS is specified on command line (default enabled, required)
       5. Process WebSocket origin command line options
    6. Start HardwareTransport (default disabled)

       1. Disable Message Format Version 4 (default enabled) and/or String Table
          (default disabled) if specified in flags or command line
    7. Start IntraTransport (default enabled)

       1. Enable IntraTransport server to listen for incoming clients (default enabled)
       2. Disable Message Format Version 4 (default enabled) and/or String Table
          (default disabled) if specified in flags or command line
    8. Disable timeouts if specified in flags or command line (default timeouts normal)

    Most users will not need to be concerned with these details, and can simply
    use the default configuration.

    :param node_name: The NodeName
    :type node_name: str
    :param tcp_port: The port to listen for incoming TCP clients
    :type tcp_port: int
    :param node: (optional) The node to configure and manage lifecycle
    :type node: RobotRaconteur.RobotRaconteurNode
    :param argv: (optional) The command line argument vector. Default is ``sys.argv``
    """

    def __init__(self, node_name, tcp_port, node=None, argv=None):
        super(SecureServerNodeSetup, self).__init__(node_name, tcp_port, RobotRaconteurPython.RobotRaconteurNodeSetupFlags_SECURE_SERVER_DEFAULT,
                                                    RobotRaconteurPython.RobotRaconteurNodeSetupFlags_SECURE_SERVER_DEFAULT_ALLOWED_OVERRIDE, node, argv)


class UserLogRecordHandlerDirectorPython(RobotRaconteurPython.UserLogRecordHandlerDirector):
    def __init__(self, handler):
        super(UserLogRecordHandlerDirectorPython, self).__init__()
        self.handler = handler

    def HandleLogRecord(self, record):
        if self.handler is not None:
            self.handler(record)


class UserLogRecordHandler(RobotRaconteurPython.UserLogRecordHandlerBase):
    def __init__(self, handler):
        super(UserLogRecordHandler, self).__init__()
        director = UserLogRecordHandlerDirectorPython(handler)
        self._SetHandler(director, 0)
        director.__disown__()


class TapFileReader(object):
    __slots__ = ["_fileobj"]

    def __init__(self, fileobj):
        self._fileobj = fileobj

    def ReadNextMessage(self):
        len_bytes = self._fileobj.read(8)
        if (len(len_bytes) < 8):
            return None
        message_len = RobotRaconteurPython.MessageLengthFromBytes(len_bytes)
        message_bytes = len_bytes + self._fileobj.read(message_len - 8)
        if (len(message_bytes) < message_len):
            return None
        m = RobotRaconteurPython.MessageFromBytes(message_bytes)
        return m

    def UnpackMessageElement(self, el, node=None):
        if node is None:
            node = RobotRaconteurPython.RobotRaconteurNode.s
        return UnpackMessageElement(el, "varvalue value", None, node)


def settrace():

    enable_debugpy = os.environ.get(
        "ROBOTRACONTEUR_PYTHON_ENABLE_DEBUGPY", None)
    if enable_debugpy is not None:
        enable_debugpy = enable_debugpy.strip().lower()
        if enable_debugpy == "1" or enable_debugpy == "true":
            import debugpy
            debugpy.debug_this_thread()
            return

    enable_pydevd = os.environ.get("ROBOTRACONTEUR_PYTHON_ENABLE_PYDEVD", None)
    if enable_pydevd is not None:
        enable_pydevd = enable_pydevd.strip().lower()
        if enable_pydevd == "1" or enable_pydevd == "true":
            import pydevd
            pydevd.settrace(suspend=False)
