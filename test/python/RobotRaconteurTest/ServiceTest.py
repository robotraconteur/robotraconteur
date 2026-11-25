from __future__ import absolute_import

import time
import traceback
import RobotRaconteur as RR
import pytest
import threading
import numpy as np
import numpy.testing as nptest
from .testvectors import get_test_vector

try:
    xrange  # type: ignore
except NameError:
    xrange = range

ca = nptest.assert_allclose

# cSpell: ignore mdat, mdat, lstruct


class testroot_impl(object):

    def __init__(self, tcptransport):
        self._o1 = sub1_impl()
        self._o2 = {}
        self._o2_lock = threading.RLock()
        self._o3 = {}
        self._o3_lock = threading.RLock()
        self._o4 = {}
        self._o4_lock = threading.RLock()
        self._o5 = sub1_impl()
        self._o6 = sub1_impl()

        self._packet_sent = False
        self._p1_lock = threading.RLock()
        self._ack_recv = False
        self._p1 = None

        self._p2 = None
        self._p2_lock = threading.RLock()

        self.ev1 = RR.EventHook()
        # self.ev2=EventHook()
        # self.ev3=EventHook()

        mdat1 = get_test_vector("ts1.m1.initial", "double")
        self.m1 = RR.ArrayMemory(mdat1)
        self.m2 = RR.MultiDimArrayMemory(np.zeros([10, 10, 10, 10, 10]))
        self.m3 = RR.MultiDimArrayMemory(
            np.zeros([1024, 1024], dtype=np.uint8))

        self._broadcastpipe = None
        self._broadcastwire = None
        self.tcptransport = tcptransport

    def RRServiceObjectInit(self, ctx, service_path):
        self._node = ctx.GetNode()
        self._teststruct1_type = self._node.GetStructureType(
            "com.robotraconteur.testing.TestService1.teststruct1")
        self._teststruct2_type = self._node.GetStructureType(
            "com.robotraconteur.testing.TestService1.teststruct2")
        self._ostruct2_type = self._node.GetStructureType(
            "com.robotraconteur.testing.TestService2.ostruct2")

        def timer_func(ev):
            try:

                for i in xrange(100):
                    self._broadcastpipe.AsyncSendPacket(i, lambda: None)

            except:
                pass
        t = self._node.CreateTimer(.025, timer_func, oneshot=False)
        t.Start()

        def timer_func2(ev):
            try:
                for i in xrange(100):
                    self._wirebroadcaster.OutValue = i
            except:
                pass

        t2 = self._node.CreateTimer(.025, timer_func2, oneshot=False)
        t2.Start()

    @property
    def d1(self):
        return 12.345

    @d1.setter
    def d1(self, value):
        assert value == 3.456

    @property
    def d2(self):
        return get_test_vector("ts1.d2.to_client", "double")

    @d2.setter
    def d2(self, value):
        ca(value, get_test_vector("ts1.d2.to_service", "double"))

    @property
    def d3(self):
        return get_test_vector("ts1.d3.to_client", "double")

    @d3.setter
    def d3(self, value):
        ca(value, get_test_vector("ts1.d3.to_service", "double"))

    @property
    def d4(self):
        return get_test_vector("ts1.d4.to_client", "double")

    @d4.setter
    def d4(self, value):
        ca(value, get_test_vector("ts1.d4.to_service", "double"))

    @property
    def d5(self):
        return np.array(get_test_vector("ts1.d5.to_client", "double")).reshape(get_test_vector("ts1.d5.to_client.dims", "uint32"), order="F")

    @d5.setter
    def d5(self, value):
        np.testing.assert_allclose(value, np.array(get_test_vector(
            "ts1.d5.to_service", "double")).reshape(get_test_vector("ts1.d5.to_service.dims", "uint32"), order="F"))

    @property
    def d6(self):
        return np.array(get_test_vector("ts1.d6.to_client", "double")).reshape(get_test_vector("ts1.d6.to_client.dims", "uint32"), order="F")

    @d6.setter
    def d6(self, value):
        np.testing.assert_allclose(value, np.array(get_test_vector("ts1.d6.to_service", "double")).reshape(
            get_test_vector("ts1.d6.to_service.dims", "uint32"), order="F"))

    @property
    def s1(self):
        return 7.8573

    @s1.setter
    def s1(self, value):
        pass

    @property
    def s2(self):
        return get_test_vector("ts1.s2.to_client", "single")

    @s2.setter
    def s2(self, value):
        pass

    @property
    def i8_1(self):
        return -66

    @i8_1.setter
    def i8_1(self, value):
        assert value == 45

    @property
    def i8_2(self):
        return get_test_vector("ts1.i8_2.to_client", "int8")

    @i8_2.setter
    def i8_2(self, value):
        nptest.assert_array_equal(value, get_test_vector("ts1.i8_2.to_service", "int8"))

    @property
    def u8_1(self):
        return 222

    @u8_1.setter
    def u8_1(self, value):
        assert value == 232

    @property
    def u8_2(self):
        return get_test_vector("ts1.u8_2.to_client", "uint8")

    @u8_2.setter
    def u8_2(self, value):
        nptest.assert_array_equal(value, get_test_vector("ts1.u8_2.to_service", "uint8"))

    @property
    def u8_3(self):
        return np.array(get_test_vector("ts1.u8_3.to_client", "uint8"), dtype=np.uint8).reshape(get_test_vector("ts1.u8_3.to_client.dims", "uint32"), order="F")

    @u8_3.setter
    def u8_3(self, value):

        nptest.assert_array_equal(value, np.array(get_test_vector(
            "ts1.u8_3.to_service", "uint8")).reshape(get_test_vector("ts1.u8_3.to_service.dims", "uint32"), order="F"))

    @property
    def i16_1(self):
        return -13428

    @i16_1.setter
    def i16_1(self, value):
        assert value == 2387

    @property
    def i16_2(self):
        return get_test_vector("ts1.i16_2.to_client", "int16")

    @i16_2.setter
    def i16_2(self, value):
        nptest.assert_array_equal(value, get_test_vector("ts1.i16_2.to_service", "int16"))

    @property
    def u16_1(self):
        return 60981

    @u16_1.setter
    def u16_1(self, value):
        assert value == 54732

    @property
    def u16_2(self):
        return get_test_vector("ts1.u16_2.to_client", "uint16")

    @u16_2.setter
    def u16_2(self, value):
        nptest.assert_array_equal(value, get_test_vector("ts1.u16_2.to_service", "uint16"))

    @property
    def i32_1(self):
        return 898734

    @i32_1.setter
    def i32_1(self, value):
        assert value == -9837284

    @property
    def i32_2(self):
        return get_test_vector("ts1.i32_2.to_client", "int32")

    @i32_2.setter
    def i32_2(self, value):
        nptest.assert_array_equal(value, get_test_vector("ts1.i32_2.to_service", "int32"))

    @property
    def i32_huge(self):
        o = [0] * 2621440
        for i in xrange(2621440):
            o[i] = i
        return o

    @i32_huge.setter
    def i32_huge(self, value):
        assert len(value) == 2621440
        for i in xrange(2621440):
            assert value[i] == -i

    @property
    def u32_1(self):
        return 547919123

    @u32_1.setter
    def u32_1(self, value):
        assert value == 1550099195

    @property
    def u32_2(self):
        return get_test_vector("ts1.u32_2.to_client", "uint32")

    @u32_2.setter
    def u32_2(self, value):
        nptest.assert_array_equal(value, get_test_vector("ts1.u32_2.to_service", "uint32"))

    @property
    def i64_1(self):
        return -1357833931563696072

    @i64_1.setter
    def i64_1(self, value):
        assert value == 8621740821050813024

    @property
    def i64_2(self):
        return get_test_vector("ts1.i64_2.to_client", "int64")

    @i64_2.setter
    def i64_2(self, value):
        nptest.assert_array_equal(value, get_test_vector("ts1.i64_2.to_service", "int64"))

    @property
    def u64_1(self):
        return 13389861970863644378

    @u64_1.setter
    def u64_1(self, value):
        assert value == 1465640522145789825

    @property
    def u64_2(self):
        return get_test_vector("ts1.u64_2.to_client", "uint64")

    @u64_2.setter
    def u64_2(self, value):
        nptest.assert_array_equal(value, np.array(get_test_vector("ts1.u64_2.to_service", "uint64"), dtype=np.uint64))

    @property
    def str1(self):
        return "Hello Client!"

    @str1.setter
    def str1(self, value):
        assert value == "Hello Server!"

    @property
    def struct1(self):
        s1 = self._teststruct1_type()
        s1.dat1 = get_test_vector("ts1.struct1.dat1.to_client", "double")
        s1.str2 = "Hello world!"
        s1.vec3 = {1: "Hello Client!", 2: "Hello Client, again",
                   4372: "This is yet another test string"}
        s1.dict4 = {"teststring1": "Hello Client!", "teststring2": "Hello Client, again",
                    "anotherstr": "This is yet another test string"}
        s1.list5 = ["Hello Client!", "Hello Client, again",
                    "This is yet another test string"]
        s1.struct1 = self._teststruct2_type()
        s1.struct1.mydat = get_test_vector("ts1.struct1.struct1_mydat.to_client", "double")
        s1_dstruct2_1 = self._teststruct2_type()
        s1_dstruct2_1.mydat = get_test_vector("ts1.struct1.dstruct2_1.to_client", "double")
        s1_dstruct2_2 = self._teststruct2_type()
        s1_dstruct2_2.mydat = get_test_vector("ts1.struct1.dstruct2_3.to_client", "double")
        s1.dstruct2 = {"test1": s1_dstruct2_1, "anothertest": s1_dstruct2_2}
        s1_lstruct3_1 = self._teststruct2_type()
        s1_lstruct3_1.mydat = get_test_vector("ts1.struct1.dstruct2_1.to_client", "double")
        s1_lstruct3_2 = self._teststruct2_type()
        s1_lstruct3_2.mydat = get_test_vector("ts1.struct1.dstruct2_3.to_client", "double")
        s1.lstruct3 = [s1_dstruct2_1, s1_dstruct2_2]
        s1.multidimarray = np.array(get_test_vector("ts1.struct1.multidimarray.to_client",
                                    "double")).reshape(get_test_vector("ts1.struct1.multidimarray.to_client.dims", "uint32"), order="F")
        s1.var3 = RR.VarValue("This is a vartype string", "string")
        return s1

    @struct1.setter
    def struct1(self, value):
        ca(value.dat1, get_test_vector("ts1.struct1.dat1.to_service", "double"))
        assert value.str2 == "Hello world 2!"
        assert len(value.vec3) == 4
        assert value.vec3[10] == "Hello Server!"
        assert value.vec3[11] == "Hello Server, again"
        assert value.vec3[46372] == "Test string!"
        assert value.vec3[46373] == "Test string again"
        assert len(value.dict4) == 2
        assert value.dict4["cteststring1"] == "Hello Server!"
        assert value.dict4["cteststring2"] == "Hello Server, again"
        ca(value.struct1.mydat, get_test_vector("ts1.struct1.struct1_mydat.to_service", "double"))
        assert len(value.dstruct2) == 2
        ca(value.dstruct2["ctest1"].mydat, get_test_vector("ts1.struct1.dstruct2_1.to_service", "double"))
        ca(value.dstruct2["anothertest"].mydat, get_test_vector("ts1.struct1.dstruct2_2.to_service", "double"))
        assert len(value.lstruct3) == 2
        ca(value.lstruct3[0].mydat, get_test_vector("ts1.struct1.dstruct2_1.to_service", "double"))
        ca(value.lstruct3[1].mydat, get_test_vector("ts1.struct1.dstruct2_2.to_service", "double"))

        np.testing.assert_allclose(value.multidimarray, np.array(get_test_vector(
            "ts1.struct1.multidimarray.to_service", "double")).reshape(get_test_vector("ts1.struct1.multidimarray.to_service.dims", "uint32"), order="F"))
        ca(value.var3.data, get_test_vector("ts1.struct1.var3.to_service", "double"))

    @property
    def struct2(self):
        out = self._teststruct2_type()
        out.mydat = get_test_vector("ts1.struct2.to_client", "double")
        return out

    @struct2.setter
    def struct2(self, value):
        ca(value.mydat, get_test_vector("ts1.struct2.to_service", "double"))

    @property
    def struct3(self):
        out = self._ostruct2_type()
        out.a1 = get_test_vector("ts1.struct3.to_client", "double")
        return out

    @struct3.setter
    def struct3(self, value):
        ca(value.a1, get_test_vector("ts1.struct3.to_service", "double"))

    @property
    def is_d1(self):
        return {9285: 1.643392e-01, 74822: 1.537133e+09, 4: 1.369505e-03}

    @is_d1.setter
    def is_d1(self, value):
        assert len(value) == 3
        assert value[928] == 4.074501e-07
        assert value[394820] == -4.535303e+05
        assert value[623] == -2.956241e-20

    @property
    def is_d2(self):
        return {"testval1": -1.079664e+16, "testval2": 2.224846e+00}

    @is_d2.setter
    def is_d2(self, value):
        assert len(value) == 2
        assert value["testval3"] == 5.242474e+10
        assert value["testval4"] == 2.208636e+08

    @property
    def is_d3(self):
        out = {}
        out[12] = get_test_vector("ts1.is_d3.to_client.a", "double")
        out[832] = get_test_vector("ts1.is_d3.to_client.b", "double")
        return out

    @is_d3.setter
    def is_d3(self, value):
        assert len(value) == 2
        ca(value[47], get_test_vector("ts1.is_d3.to_service.a", "double"))
        ca(value[324], get_test_vector("ts1.is_d3.to_service.b", "double"))

    @property
    def is_d4(self):
        out = {}
        out["testval1"] = get_test_vector("ts1.is_d4.to_client.a", "double")
        out["testval2"] = get_test_vector("ts1.is_d4.to_client.b", "double")
        return out

    @is_d4.setter
    def is_d4(self, value):
        assert len(value) == 2
        ca(value["testval3"], get_test_vector("ts1.is_d4.to_service.a", "double"))
        ca(value["testval4"], get_test_vector("ts1.is_d4.to_service.b", "double"))

    @property
    def is_d5(self):
        out = {}
        out1 = np.array(get_test_vector("ts1.is_d5.to_client", "double")).reshape(
            get_test_vector("ts1.is_d5.to_client.dims", "uint32"), order="F")
        out[564] = out1
        return out

    @is_d5.setter
    def is_d5(self, value):
        assert len(value) == 1
        in1 = value[328]
        np.testing.assert_allclose(in1, np.array(get_test_vector(
            "ts1.is_d5.to_service", "double")).reshape(get_test_vector("ts1.is_d5.to_service.dims", "uint32"), order="F"))

    @property
    def is_d6(self):
        out = {}
        out1 = np.array(get_test_vector("ts1.is_d6.to_client", "double")).reshape(
            get_test_vector("ts1.is_d6.to_client.dims", "uint32"), order="F")
        out["testval1"] = out1
        return out

    @is_d6.setter
    def is_d6(self, value):
        if (len(value) != 1):
            return Exception()
        in1 = value["testval2"]
        np.testing.assert_allclose(in1, np.array(get_test_vector(
            "ts1.is_d6.to_service", "double")).reshape(get_test_vector("ts1.is_d6.to_service.dims", "uint32"), order="F"))

    @property
    def is_str1(self):
        return {23: "Hello server"}

    @is_str1.setter
    def is_str1(self, value):
        assert len(value) == 1
        assert value[24] == "Hello client"

    @property
    def is_str2(self):
        return {"testval1": "Hello server"}

    @is_str2.setter
    def is_str2(self, value):
        assert len(value) == 1
        assert value["testval2"] == "Hello client"

    @property
    def is_struct1(self):
        out = {}
        out1 = self._teststruct2_type()
        out1.mydat = get_test_vector("ts1.is_struct1.to_client", "double")
        out[748] = out1
        return out

    @is_struct1.setter
    def is_struct1(self, value):
        assert len(value) == 1
        ca(value[372].mydat, get_test_vector("ts1.is_struct1.to_service", "double"))

    @property
    def is_struct2(self):
        out = {}
        out1 = self._teststruct2_type()
        out1.mydat = get_test_vector("ts1.is_struct2.to_client", "double")
        out["testval1"] = out1
        return out

    @is_struct2.setter
    def is_struct2(self, value):
        assert len(value) == 1
        ca(value["testval2"].mydat, get_test_vector("ts1.is_struct2.to_service", "double"))

    @property
    def list_d1(self):
        return [1.643392e-01, 1.537133e+09, 1.369505e-03]

    @list_d1.setter
    def list_d1(self, value):
        assert len(value) == 3
        assert value[0] == 4.074501e-07
        assert value[1] == -4.535303e+05
        assert value[2] == -2.956241e-20

    @property
    def list_d3(self):
        out = []
        out.append(get_test_vector("ts1.is_d3.to_client.a", "double"))
        out.append(get_test_vector("ts1.is_d3.to_client.b", "double"))
        return out

    @list_d3.setter
    def list_d3(self, value):
        assert len(value) == 2
        ca(value[0], get_test_vector("ts1.is_d3.to_service.a", "double"))
        ca(value[1], get_test_vector("ts1.is_d3.to_service.b", "double"))

    @property
    def list_d5(self):
        out = []
        out1 = np.array(get_test_vector("ts1.is_d5.to_client", "double")).reshape(
            get_test_vector("ts1.is_d5.to_client.dims", "uint32"), order="F")
        out.append(out1)
        return out

    @list_d5.setter
    def list_d5(self, value):
        assert len(value) == 1
        in1 = value[0]
        np.testing.assert_allclose(in1, np.array(get_test_vector(
            "ts1.is_d5.to_service", "double")).reshape(get_test_vector("ts1.is_d5.to_service.dims", "uint32"), order="F"))

    @property
    def list_str1(self):
        return ["Hello server"]

    @list_str1.setter
    def list_str1(self, value):
        assert len(value) == 1
        assert value[0] == "Hello client"

    @property
    def list_struct1(self):
        out = []
        out1 = self._teststruct2_type()
        out1.mydat = get_test_vector("ts1.is_struct1.to_client", "double")
        out.append(out1)
        return out

    @list_struct1.setter
    def list_struct1(self, value):
        assert len(value) == 1
        ca(value[0].mydat, get_test_vector("ts1.is_struct1.to_service", "double"))

    @property
    def is_struct3(self):
        out = self._ostruct2_type()
        out.a1 = get_test_vector("ts1.struct3.to_client", "double")
        return out

    @is_struct3.setter
    def is_struct3(self, value):
        ca(value.a1, get_test_vector("ts1.struct3.to_service", "double"))

    @property
    def var_num(self):
        return RR.VarValue(get_test_vector("ts1.var_num.to_client", "int32"), "int32[]")

    @var_num.setter
    def var_num(self, value):
        nptest.assert_array_equal(value.data, get_test_vector("ts1.var_num.to_service", "int32"))

    @property
    def var_str(self):
        return RR.VarValue("Hello Client!", "string")

    @var_str.setter
    def var_str(self, value):
        assert value.data == "Hello Server!"

    @property
    def var_struct(self):
        out = self._teststruct2_type()
        out.mydat = get_test_vector("ts1.var_struct.to_client", "double")
        return RR.VarValue(out, "com.robotraconteur.testing.TestService1.teststruct2")

    @var_struct.setter
    def var_struct(self, value):
        ca(value.data.mydat, get_test_vector("ts1.var_struct.to_service", "double"))

    @property
    def var_vector(self):
        out = {}
        out[10] = "Hello Client!"
        return RR.VarValue(out, "string{int32}")

    @var_vector.setter
    def var_vector(self, value):
        assert len(value.data) == 1
        assert value.data[11].data == "Hello Server!"

    @property
    def var_dictionary(self):
        out = {}
        out["test1"] = "Hello Client!"
        return RR.VarValue(out, "string{string}")

    @var_dictionary.setter
    def var_dictionary(self, value):
        assert len(value.data) == 1
        assert value.data["test2"].data == "Hello Server!"

    @property
    def var_list(self):
        out = ["Hello Client!"]
        return RR.VarValue(out, "string{list}")

    @var_list.setter
    def var_list(self, value):
        assert len(value.data) == 1
        assert value.data[0].data == "Hello Server!"

    @property
    def var_multidimarray(self):
        return RR.VarValue(np.array(get_test_vector("ts1.var_multidimarray.to_client", "double")).reshape(get_test_vector("ts1.var_multidimarray.to_client.dims", "uint32"), order="F"), "double[*]")

    @var_multidimarray.setter
    def var_multidimarray(self, value):
        np.testing.assert_allclose(value.data, np.array(get_test_vector("ts1.var_multidimarray.to_service", "double")).reshape(
            get_test_vector("ts1.var_multidimarray.to_service.dims", "uint32"), order="F"))

    @property
    def errtest(self):
        raise Exception()

    @errtest.setter
    def errtest(self, value):
        raise Exception()

    @property
    def nulltest(self):
        return None

    @nulltest.setter
    def nulltest(self, value):
        assert value is None

    # functions
    def func1(self):
        def timer_handler(ev):
            try:
                self.ev1.fire()
            except:
                traceback.print_exc()
        t = self._node.CreateTimer(1, timer_handler, True)
        t.Start()

    def func2(self, d1, d2):
        def timer_handler(ev):
            try:
                s = self._teststruct2_type()
                s.mydat = [d2]
                self.ev2.fire(d1, s)
            except:
                traceback.print_exc()
        t = self._node.CreateTimer(1, timer_handler, True)
        t.Start()

    def func3(self, d1, d2):
        try:
            user = RR.ServerEndpoint.GetCurrentAuthenticatedUser()
            if (user is None):
                print("No user")
            else:
                print("User is: " + user.Username)
                print("Login time is " + str(user.LoginTime))
                print("Last access time is " + str(user.LastAccessTime))
                print("Privileges " + str(user.Privileges))
        except:
            # traceback.print_exc()
            print("No user")
        print(str(self._node.NowUTC()))

        try:

            isconnectionsecure = self.tcptransport.IsTransportConnectionSecure(
                RR.ServerEndpoint.GetCurrentEndpoint())
            if (isconnectionsecure):
                ispeerverified = self.tcptransport.IsSecurePeerIdentityVerified(
                    RR.ServerEndpoint.GetCurrentEndpoint())
                if (not ispeerverified):
                    print("Peer identity is not verified")
                else:
                    print("Peer identity is: " + self.tcptransport.GetSecurePeerIdentity(
                        RR.ServerEndpoint.GetCurrentEndpoint()))
        except:
            traceback.print_exc()
            raise

        return d1 + d2

    def meaning_of_life(self):
        return 42

    def func_errtest(self):
        raise Exception("This is a test")

    def func_errtest1(self):
        raise RR.DataTypeException("This is a test 1")

    def func_errtest2(self):
        e = self._node.GetExceptionType(
            "com.robotraconteur.testing.TestService1.testexception1")
        raise e("This is a test 2")

    def func_errtest3(self):
        e = self._node.GetExceptionType(
            "com.robotraconteur.testing.TestService2.testexception3")
        raise e("This is a test 3")

    # objrefs
    def get_o1(self):
        return self._o1, "com.robotraconteur.testing.TestService1.sub1"

    def get_o2(self, ind):
        # cSpell: ignore iind
        iind = int(ind)
        with self._o2_lock:
            if (not iind in self._o2):
                self._o2[iind] = sub1_impl()
                self._o2[iind].i_ind = iind
            return self._o2[iind], "com.robotraconteur.testing.TestService1.sub1"

    def get_o3(self, ind):
        iind = int(ind)
        with self._o3_lock:
            if (not iind in self._o3):
                self._o3[iind] = sub1_impl()
                self._o3[iind].i_ind = iind
            return self._o3[iind], "com.robotraconteur.testing.TestService1.sub1"

    def get_o4(self, ind):
        iind = str(ind)
        with self._o4_lock:
            if (not iind in self._o4):
                self._o4[iind] = sub1_impl()
                self._o4[iind].s_ind = iind
            return self._o4[iind], "com.robotraconteur.testing.TestService1.sub1"

    def get_o5(self, ind):
        return self._o5, "com.robotraconteur.testing.TestService1.sub1"

    def get_o6(self):
        if (isinstance(self._o6, sub1_impl)):
            return self._o6, "com.robotraconteur.testing.TestService1.sub1"
        if (isinstance(self._o6, sub2_impl)):
            return self._o6, "com.robotraconteur.testing.TestService1.sub2"
        if (isinstance(self._o6, subobj_impl)):
            return self._o6, "com.robotraconteur.testing.TestService2.subobj"

    def o6_op(self, op):
        try:
            # print "op " + ServerContext.GetCurrentServicePath() + ".o6"
            RR.ServerContext.GetCurrentServerContext().ReleaseServicePath(
                RR.ServerContext.GetCurrentServicePath() + ".o6")
        except:
            traceback.print_exc()

        if (op == 0):
            self._o6 = sub1_impl()
        elif (op == 1):
            self._o6 = sub2_impl()
        elif (op == 2):
            self._o6 = subobj_impl()
        else:
            assert False, "Unknown op number"

    # pipes
    @property
    def p1(self):
        return self._p1

    @p1.setter
    def p1(self, value):
        self._p1 = value
        value.PipeConnectCallback = (self.p1_connect_callback)

    def p1_connect_callback(self, p):
        p.RequestPacketAck = True
        p.PacketReceivedEvent += self.p1_packet_received
        p.PacketAckReceivedEvent += self.p1_packet_ack_received

    def p1_packet_received(self, p):
        def p1_pr():
            try:
                time.sleep(.5)
                with self._p1_lock:
                    while p.Available:
                        dat = p.ReceivePacket()
                        pnum = p.SendPacket(dat)
                        if (not self._packet_sent):
                            self._packetnum = pnum
                            self._packet_sent = True
            except:
                pass
        self._node.PostToThreadPool(p1_pr)

    def p1_packet_ack_received(self, p, packetnum):
        if (packetnum == self._packetnum):
            self._ack_recv = True

    @property
    def p2(self):
        return self._p2

    @p2.setter
    def p2(self, value):
        self._p2 = value
        value.PipeConnectCallback = self.p2_connect_callback

    def p2_connect_callback(self, p):
        p.PacketReceivedEvent += self.p2_packet_received

    def p2_packet_received(self, p):
        try:
            time.sleep(.5)
            with self._p2_lock:
                while p.Available > 0:
                    dat = p.ReceivePacket()
                    p.SendPacket(dat)
        except:
            pass

    def pipe_check_error(self):
        assert self._ack_recv

    # callbacks

    def test_callbacks(self):
        ep = RR.ServerEndpoint.GetCurrentEndpoint()
        self.cb1.GetClientFunction(ep)()
        self.cb2.GetClientFunction(ep)(739.2, 0.392)
        res = self.cb3.GetClientFunction(ep)(34, 45)
        assert res == (34 + 45 + 3.14)
        assert self.cb_meaning_of_life.GetClientFunction(ep)() == 42

        with pytest.raises(Exception):
            self.cb_errtest.GetClientFunction(ep)()

    @property
    def broadcastpipe(self):
        return self._broadcastpipe

    @broadcastpipe.setter
    def broadcastpipe(self, value):
        assert self._broadcastpipe is None, "Pipe already set"
        self._broadcastpipe = RR.PipeBroadcaster(value, 3)
        assert self._broadcastpipe.MaxBacklog == 3
        self._broadcastpipe.MaxBacklog = 3

    # wires

    @property
    def w1(self):
        return self._w1

    @w1.setter
    def w1(self, value):
        self._w1 = value
        value.WireConnectCallback = self.w1_connect_callback

    def w1_connect_callback(self, w):
        # print "connect"
        w.WireValueChanged += self.w1_value_changed

    def w1_value_changed(self, w, value, time):
        # print "change"
        w.OutValue = w.InValue

    @property
    def w2(self):
        return self._w2

    @w2.setter
    def w2(self, value):
        self._w2 = value
        value.WireConnectCallback = self.w2_connect_callback

    def w2_connect_callback(self, w):
        w.WireValueChanged += self.w2_value_changed

    def w2_value_changed(self, w, value, time):
        w.OutValue = w.InValue

    @property
    def w3(self):
        return self._w1

    @w3.setter
    def w3(self, value):
        self._w3 = value
        value.WireConnectCallback = self.w3_connect_callback

    def w3_connect_callback(self, w):
        w.WireValueChanged += self.w3_value_changed

    def w3_value_changed(self, w, value, time):
        w.OutValue = w.InValue

    @property
    def broadcastwire(self):
        return self._broadcastwire

    @broadcastwire.setter
    def broadcastwire(self, value):

        assert self._broadcastwire is None, "Value already set"
        self._broadcastwire = value

        self._wirebroadcaster = RR.WireBroadcaster(value)


class sub1_impl(object):
    def __init__(self):
        self.d1 = None
        self.d2 = None
        self.s_ind = ""
        self.i_ind = 0

        self._o2_1 = sub2_impl()
        self._o2_2 = {}
        self._o2_2_lock = threading.RLock()
        self._o2_3 = {}
        self._o2_3_lock = threading.RLock()

        self._lock = threading.RLock()

    def RobotRaconteurMonitorEnter(self, timeout):
        print("Monitor enter")
        self._lock.acquire()

    def RobotRaconteurMonitorExit(self):
        print("Monitor exit")
        self._lock.release()

    def get_o2_1(self):
        return self._o2_1, "com.robotraconteur.testing.TestService1.sub2"

    def get_o2_2(self, ind):
        iind = int(ind)
        with self._o2_2_lock:
            if (not iind in self._o2_2):
                self._o2_2[iind] = sub2_impl()
                self._o2_2[iind].i_ind = iind
            return self._o2_2[iind], "com.robotraconteur.testing.TestService1.sub2"

    def get_o2_3(self, ind):
        iind = str(ind)
        with self._o2_3_lock:
            if (not iind in self._o2_3):
                self._o2_3[iind] = sub2_impl()
                self._o2_3[iind].s_ind = iind
            return self._o2_3[iind], "com.robotraconteur.testing.TestService1.sub2"


class sub2_impl(object):
    def __init__(self):
        self.s_ind = ""
        self.i_ind = 0
        self.data = ""

        self._o3_1 = {}
        self._o3_1_lock = threading.RLock()

    def get_o3_1(self, ind):
        iind = str(ind)
        with self._o3_1_lock:
            if (not iind in self._o3_1):
                self._o3_1[iind] = sub3_impl()
                self._o3_1[iind].ind = iind
            return self._o3_1[iind], "com.robotraconteur.testing.TestService1.sub3"

    def RRServiceObjectInit(self, context, service_path):
        print("Got RRServiceObjectInit: " + str(context) +
              " service_path: " + service_path)


class sub3_impl(object):
    def __init__(self):
        self.ind = ""
        self.data2 = ""
        self.data3 = 0

    def add(self, d):
        return d + 42


class subobj_impl(object):
    def add_val(self, v):
        return v + 1
