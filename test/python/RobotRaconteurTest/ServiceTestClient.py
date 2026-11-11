from __future__ import absolute_import

import RobotRaconteur as RR
RRN = RR.RobotRaconteurNode.s
import functools
import numpy as np
import numpy.testing as nptest
import threading
import time
import traceback
import pytest
from . import multidim_array_util
from . import lfsr
from .testvectors import get_test_vector

ca = nptest.assert_allclose

# cSpell: ignore lstruct, myind


class ServiceTestClient:
    def __init__(self):

        self._packetnum = 0
        self._ack_recv = False

    def RunFullTest(self, url, authurl):
        self.ConnectService(url)
        self.TestProperties()
        self.TestFunctions()
        self.TestEvents()
        self.TestObjRefs()
        self.TestPipes()
        self.TestCallbacks()
        self.TestWires()
        self.TestMemories()
        self.DisconnectService()
        self.TestAuthentication(authurl)
        self.TestObjectLock(authurl)
        self.TestMonitorLock(url)
        self.TestAsync(authurl)

    def ConnectService(self, url):
        def client_listener(c, event_type, p):
            print("Client event: " + str(event_type) + " param: " + str(p))
        self._r = RRN.ConnectService(url, None, None, client_listener)
        attributes = RRN.GetServiceAttributes(self._r)
        print(attributes)

    def DisconnectService(self):
        RRN.DisconnectService(self._r)

    def TestProperties(self):
        self._r.d1 = 3.456
        assert self._r.d1 == 12.345
        self._r.d2 = get_test_vector("ts1.d2.to_service", "double")
        ca(self._r.d2, get_test_vector("ts1.d2.to_client", "double"))
        self._r.d3 = get_test_vector("ts1.d3.to_service", "double")
        ca(self._r.d3, get_test_vector("ts1.d3.to_client", "double"))
        self._r.d4 = get_test_vector("ts1.d4.to_service", "double")
        ca(self._r.d4, get_test_vector("ts1.d4.to_client", "double"))
        d5_1 = np.array(get_test_vector("ts1.d5.to_service", "double")).reshape(
            get_test_vector("ts1.d5.to_service.dims", "uint32"), order="F")
        self._r.d5 = d5_1
        d5_2 = self._r.d5
        nptest.assert_allclose(d5_2, np.array(get_test_vector(
            "ts1.d5.to_client", "double")).reshape(get_test_vector("ts1.d5.to_client.dims", "uint32"), order="F"))

        d6_1 = np.array(get_test_vector("ts1.d6.to_service", "double")).reshape(
            get_test_vector("ts1.d6.to_service.dims", "uint32"), order="F")
        self._r.d6 = d6_1
        d6_2 = self._r.d6
        nptest.assert_allclose(d6_2, np.array(get_test_vector("ts1.d6.to_client", "double")).reshape(
            get_test_vector("ts1.d6.to_client.dims", "uint32"), order="F"))

        self._r.s1 = 3847.9283
        val = self._r.s1
        nptest.assert_allclose(self._r.s1, 7.8573)
        self._r.s2 = get_test_vector("ts1.s2.to_service", "single")
        # ca(self._r.s2, get_test_vector("ts1.s2.to_client", "single"))
        self._r.i8_1 = 45
        assert self._r.i8_1 == -66
        self._r.i8_2 = get_test_vector("ts1.i8_2.to_service", "int8")
        nptest.assert_array_equal(self._r.i8_2, get_test_vector("ts1.i8_2.to_client", "int8"))
        self._r.u8_1 = 232
        assert self._r.u8_1 == 222
        self._r.u8_2 = get_test_vector("ts1.u8_2.to_service", "uint8")
        nptest.assert_array_equal(self._r.u8_2, get_test_vector("ts1.u8_2.to_client", "uint8"))
        u8_3_1 = np.array(get_test_vector("ts1.u8_3.to_service", "uint8"), dtype=np.uint8).reshape(
            get_test_vector("ts1.u8_3.to_service.dims", "uint32"), order="F")
        self._r.u8_3 = u8_3_1
        u8_3_2 = self._r.u8_3

        nptest.assert_array_equal(u8_3_2, np.array(get_test_vector("ts1.u8_3.to_client",
                                  "uint8"), dtype=np.uint8).reshape(get_test_vector("ts1.u8_3.to_client.dims", "uint32"), order="F"))

        self._r.i16_1 = 2387
        assert self._r.i16_1 == -13428
        self._r.i16_2 = get_test_vector("ts1.i16_2.to_service", "int16")
        nptest.assert_array_equal(self._r.i16_2, get_test_vector("ts1.i16_2.to_client", "int16"))
        self._r.u16_1 = 54732
        assert self._r.u16_1 == 60981
        self._r.u16_2 = get_test_vector("ts1.u16_2.to_service", "uint16")
        nptest.assert_array_equal(self._r.u16_2, get_test_vector("ts1.u16_2.to_client", "uint16"))
        self._r.i32_1 = -9837284
        assert self._r.i32_1 == 898734
        self._r.i32_2 = get_test_vector("ts1.i32_2.to_service", "int32")
        nptest.assert_array_equal(self._r.i32_2, get_test_vector("ts1.i32_2.to_client", "int32"))

        """i32_huge_1=[0]*2621440
        for i in xrange(2621440):
            i32_huge_1[i]=-i
        self._r.i32_huge=i32_huge_1
        del i32_huge_1
        i32_huge_2=self._r.i32_huge
        assert len(i32_huge_2)==2621440
        for i in xrange(2621440):
            assert i32_huge_2[i]==i
        del i32_huge_2"""

        self._r.u32_1 = 1550099195
        assert self._r.u32_1 == 547919123
        self._r.u32_2 = get_test_vector("ts1.u32_2.to_service", "uint32")
        nptest.assert_array_equal(self._r.u32_2, get_test_vector("ts1.u32_2.to_client", "uint32"))
        self._r.i64_1 = 8621740821050813024
        assert self._r.i64_1 == -1357833931563696072
        self._r.i64_2 = get_test_vector("ts1.i64_2.to_service", "int64")
        nptest.assert_array_equal(self._r.i64_2, get_test_vector("ts1.i64_2.to_client", "int64"))
        self._r.u64_1 = 1465640522145789825
        assert self._r.u64_1 == 13389861970863644378
        self._r.u64_2 = get_test_vector("ts1.u64_2.to_service", "uint64")
        nptest.assert_array_equal(self._r.u64_2, np.array(
            get_test_vector("ts1.u64_2.to_client", "uint64"), dtype=np.uint64))
        self._r.str1 = "Hello Server!"
        assert self._r.str1 == "Hello Client!"

        # More complex stuff in the testing of properties
        s1 = self._r.struct1
        ca(s1.dat1, get_test_vector("ts1.struct1.dat1.to_client", "double"))
        assert s1.str2 == "Hello world!"
        assert len(s1.vec3) == 3
        assert s1.vec3[1] == "Hello Client!"
        assert s1.vec3[2] == "Hello Client, again"
        assert s1.vec3[4372] == "This is yet another test string"
        assert len(s1.dict4) == 3
        assert s1.dict4["teststring1"] == "Hello Client!"
        assert s1.dict4["teststring2"] == "Hello Client, again"
        assert s1.dict4["anotherstr"] == "This is yet another test string"
        ca(s1.struct1.mydat, get_test_vector("ts1.struct1.struct1_mydat.to_client", "double"))
        assert len(s1.dstruct2) == 2
        ca(s1.dstruct2["test1"].mydat, get_test_vector("ts1.struct1.dstruct2_1.to_client", "double"))
        ca(s1.dstruct2["anothertest"].mydat, get_test_vector("ts1.struct1.dstruct2_3.to_client", "double"))
        nptest.assert_allclose(s1.multidimarray, np.array(get_test_vector(
            "ts1.struct1.multidimarray.to_client", "double")).reshape(get_test_vector("ts1.struct1.multidimarray.to_client.dims", "uint32"), order="F"))
        assert s1.var3.data == "This is a vartype string"
        print(s1)
        s2 = RRN.NewStructure(
            "com.robotraconteur.testing.TestService1.teststruct1", self._r)
        s2.dat1 = get_test_vector("ts1.struct1.dat1.to_service", "double")
        s2.str2 = "Hello world 2!"
        s2_3 = {}
        s2_3[10] = "Hello Server!"
        s2_3[11] = "Hello Server, again"
        s2_3[46372] = "Test string!"
        s2_3[46373] = "Test string again"
        s2.vec3 = s2_3
        s2_4 = {}
        s2_4["cteststring1"] = "Hello Server!"
        s2_4["cteststring2"] = "Hello Server, again"
        s2.dict4 = s2_4
        s2.list5 = ["Hello Server!", "Hello Server, again"]
        s2.struct1 = RRN.NewStructure(
            "com.robotraconteur.testing.TestService1.teststruct2", self._r)
        s2.struct1.mydat = get_test_vector("ts1.struct1.struct1_mydat.to_service", "double")
        s2_ds2 = {}
        s2_ds2_1 = RRN.NewStructure(
            "com.robotraconteur.testing.TestService1.teststruct2", self._r)
        s2_ds2_1.mydat = get_test_vector("ts1.struct1.dstruct2_1.to_service", "double")
        s2_ds2["ctest1"] = s2_ds2_1
        s2_ds2_2 = RRN.NewStructure(
            "com.robotraconteur.testing.TestService1.teststruct2", self._r)
        s2_ds2_2.mydat = get_test_vector("ts1.struct1.dstruct2_2.to_service", "double")
        s2_ds2["anothertest"] = s2_ds2_2
        s2.dstruct2 = s2_ds2
        s2_ls3_1 = RRN.NewStructure(
            "com.robotraconteur.testing.TestService1.teststruct2", self._r)
        s2_ls3_1.mydat = get_test_vector("ts1.struct1.dstruct2_1.to_service", "double")
        s2_ls3_2 = RRN.NewStructure(
            "com.robotraconteur.testing.TestService1.teststruct2", self._r)
        s2_ls3_2.mydat = get_test_vector("ts1.struct1.dstruct2_2.to_service", "double")
        s2.lstruct3 = [s2_ls3_1, s2_ls3_2]
        s2.multidimarray = np.array(get_test_vector("ts1.struct1.multidimarray.to_service",
                                    "double")).reshape(get_test_vector("ts1.struct1.multidimarray.to_service.dims", "uint32"), order="F")
        s2.var3 = RR.VarValue(get_test_vector("ts1.struct1.var3.to_service", "double"), "double[]")
        self._r.struct1 = s2
        s3 = RRN.NewStructure(
            "com.robotraconteur.testing.TestService1.teststruct2", self._r)
        s3.mydat = get_test_vector("ts1.struct2.to_service", "double")
        self._r.struct2 = s3
        s4 = self._r.struct2
        ca(s4.mydat, get_test_vector("ts1.struct2.to_client", "double"))
        is_d1_1 = self._r.is_d1
        assert len(is_d1_1) == 3
        assert is_d1_1[9285] == 1.643392e-01
        assert is_d1_1[74822] == 1.537133e+09
        assert is_d1_1[4] == 1.369505e-03
        is_d1_2 = {}
        is_d1_2[928] = 4.074501e-07
        is_d1_2[394820] = -4.535303e+05
        is_d1_2[623] = -2.956241e-20
        self._r.is_d1 = is_d1_2
        is_d2_1 = self._r.is_d2
        assert len(is_d2_1) == 2
        assert is_d2_1["testval1"] == -1.079664e+16
        assert is_d2_1["testval2"] == 2.224846e+00
        is_d2_2 = {}
        is_d2_2["testval3"] = 5.242474e+10
        is_d2_2["testval4"] = 2.208636e+08
        self._r.is_d2 = is_d2_2
        is_d3_1 = self._r.is_d3
        assert len(is_d3_1) == 2
        ca(is_d3_1[12], get_test_vector("ts1.is_d3.to_client.a", "double"))
        ca(is_d3_1[832], get_test_vector("ts1.is_d3.to_client.b", "double"))
        is_d3_2 = {}
        is_d3_2[47] = get_test_vector("ts1.is_d3.to_service.a", "double")
        is_d3_2[324] = get_test_vector("ts1.is_d3.to_service.b", "double")
        self._r.is_d3 = is_d3_2
        is_d4_1 = self._r.is_d4
        assert len(is_d4_1) == 2
        ca(is_d4_1["testval1"], get_test_vector("ts1.is_d4.to_client.a", "double"))
        ca(is_d4_1["testval2"], get_test_vector("ts1.is_d4.to_client.b", "double"))
        is_d4_2 = {}
        is_d4_2["testval3"] = get_test_vector("ts1.is_d4.to_service.a", "double")
        is_d4_2["testval4"] = get_test_vector("ts1.is_d4.to_service.b", "double")
        self._r.is_d4 = is_d4_2
        is_d5_1 = self._r.is_d5
        assert len(is_d5_1) == 1
        is_d5_1_1 = is_d5_1[564]
        nptest.assert_allclose(is_d5_1_1, np.array(get_test_vector(
            "ts1.is_d5.to_client", "double")).reshape(get_test_vector("ts1.is_d5.to_client.dims", "uint32"), order="F"))
        is_d5_2_1 = np.array(get_test_vector("ts1.is_d5.to_service", "double")).reshape(
            get_test_vector("ts1.is_d5.to_service.dims", "uint32"), order="F")
        is_d5_2 = {}
        is_d5_2[328] = is_d5_2_1
        self._r.is_d5 = is_d5_2
        is_d6_1 = self._r.is_d6
        assert len(is_d6_1) == 1
        is_d6_1_1 = is_d6_1["testval1"]

        nptest.assert_allclose(is_d6_1_1, np.array(get_test_vector(
            "ts1.is_d6.to_client", "double")).reshape(get_test_vector("ts1.is_d6.to_client.dims", "uint32"), order="F"))
        is_d6_2_1 = np.array(get_test_vector("ts1.is_d6.to_service", "double")).reshape(
            get_test_vector("ts1.is_d6.to_service.dims", "uint32"), order="F")
        is_d6_2 = {}
        is_d6_2["testval2"] = is_d6_2_1
        self._r.is_d6 = is_d6_2
        is_str1_1 = self._r.is_str1
        assert len(is_str1_1) == 1
        assert is_str1_1[23] == "Hello server"
        is_str1_2 = {}
        is_str1_2[24] = "Hello client"
        self._r.is_str1 = is_str1_2
        is_str2_1 = self._r.is_str2
        assert len(is_str2_1) == 1
        assert is_str2_1["testval1"] == "Hello server"
        is_str2_2 = {}
        is_str2_2["testval2"] = "Hello client"
        self._r.is_str2 = is_str2_2
        is_struct1_1 = self._r.is_struct1
        ca(is_struct1_1[748].mydat, get_test_vector("ts1.is_struct1.to_client", "double"))
        is_struct1_2 = {}
        is_struct1_2_1 = RRN.NewStructure(
            "com.robotraconteur.testing.TestService1.teststruct2", self._r)
        is_struct1_2_1.mydat = get_test_vector("ts1.is_struct1.to_service", "double")
        is_struct1_2[372] = is_struct1_2_1
        self._r.is_struct1 = is_struct1_2
        is_struct2_1 = self._r.is_struct2
        ca(is_struct2_1["testval1"].mydat, get_test_vector("ts1.is_struct2.to_client", "double"))
        is_struct2_2 = {}
        is_struct2_2_1 = RRN.NewStructure(
            "com.robotraconteur.testing.TestService1.teststruct2", self._r)
        is_struct2_2_1.mydat = get_test_vector("ts1.is_struct2.to_service", "double")
        is_struct2_2["testval2"] = is_struct2_2_1
        self._r.is_struct2 = is_struct2_2

        list_d1_1 = self._r.list_d1
        assert len(list_d1_1) == 3
        assert list_d1_1[0] == 1.643392e-01
        assert list_d1_1[1] == 1.537133e+09
        assert list_d1_1[2] == 1.369505e-03
        list_d1_2 = [4.074501e-07, -4.535303e+05, -2.956241e-20]
        self._r.list_d1 = list_d1_2

        ca(self._r.struct3.a1, get_test_vector("ts1.struct3.to_client", "double"))
        struct3_1 = RRN.NewStructure(
            "com.robotraconteur.testing.TestService2.ostruct2", self._r)
        struct3_1.a1 = get_test_vector("ts1.struct3.to_service", "double")
        self._r.struct3 = struct3_1

        list_d3_1 = self._r.list_d3
        assert len(list_d3_1) == 2
        ca(list_d3_1[0], get_test_vector("ts1.is_d3.to_client.a", "double"))
        ca(list_d3_1[1], get_test_vector("ts1.is_d3.to_client.b", "double"))
        list_d3_2 = [
            get_test_vector("ts1.is_d3.to_service.a", "double"), get_test_vector("ts1.is_d3.to_service.b", "double")
        ]
        self._r.list_d3 = list_d3_2

        list_d5_1 = self._r.list_d5
        assert len(list_d5_1) == 1
        list_d5_1_1 = list_d5_1[0]
        nptest.assert_allclose(list_d5_1_1, np.array(get_test_vector(
            "ts1.is_d5.to_client", "double")).reshape(get_test_vector("ts1.is_d5.to_client.dims", "uint32"), order="F"))
        list_d5_2_1 = np.array(get_test_vector("ts1.is_d5.to_service", "double")).reshape(
            get_test_vector("ts1.is_d5.to_service.dims", "uint32"), order="F")
        list_d5_2 = [list_d5_2_1]
        self._r.list_d5 = list_d5_2

        list_struct2_1 = self._r.list_struct1
        ca(list_struct2_1[0].mydat, get_test_vector("ts1.is_struct1.to_client", "double"))
        list_struct2_2 = []
        list_struct2_2_1 = RRN.NewStructure(
            "com.robotraconteur.testing.TestService1.teststruct2", self._r)
        list_struct2_2_1.mydat = get_test_vector("ts1.is_struct1.to_service", "double")
        list_struct2_2.append(list_struct2_2_1)
        self._r.list_struct1 = list_struct2_2

        # varvalue types (TODO: finish these...)

        nptest.assert_array_equal(self._r.var_num.data, get_test_vector("ts1.var_num.to_client", "int32"))
        self._r.var_num = RR.VarValue(get_test_vector("ts1.var_num.to_service", "int32"), "int32[]")
        assert self._r.var_str.data == "Hello Client!"
        self._r.var_str = RR.VarValue("Hello Server!", "string")
        ca((self._r.var_struct.data).mydat, get_test_vector("ts1.var_struct.to_client", "double"))
        var_struct_1 = RRN.NewStructure(
            "com.robotraconteur.testing.TestService1.teststruct2", self._r)
        var_struct_1.mydat = get_test_vector("ts1.var_struct.to_service", "double")
        self._r.var_struct = RR.VarValue(
            var_struct_1, "com.robotraconteur.testing.TestService1.teststruct2")
        assert ((self._r.var_vector.data)[10].data) == "Hello Client!"
        var_vector_1 = {}
        var_vector_1[11] = RR.VarValue("Hello Server!", "string")
        self._r.var_vector = RR.VarValue(var_vector_1, "varvalue{int32}")
        assert ((self._r.var_dictionary.data)["test1"].data) == "Hello Client!"
        var_dictionary_1 = {}
        var_dictionary_1["test2"] = RR.VarValue("Hello Server!", "string")
        self._r.var_dictionary = RR.VarValue(
            var_dictionary_1, "varvalue{string}")
        assert ((self._r.var_list.data)[0].data) == "Hello Client!"
        var_list_1 = [RR.VarValue("Hello Server!", "string")]
        self._r.var_list = RR.VarValue(var_list_1, "varvalue{list}")
        var_a_1 = self._r.var_multidimarray.data
        nptest.assert_allclose(var_a_1, np.array(get_test_vector("ts1.var_multidimarray.to_client", "double")).reshape(
            get_test_vector("ts1.var_multidimarray.to_client.dims", "uint32"), order="F"))
        self._r.var_multidimarray = RR.VarValue(np.array(get_test_vector("ts1.var_multidimarray.to_service", "double")).reshape(
            get_test_vector("ts1.var_multidimarray.to_service.dims", "uint32"), order="F"), "double[*]")

        # Test error passing
        with pytest.raises(Exception):
            self._r.errtest = 1

        with pytest.raises(Exception):

            d = self._r.errtest

        assert self._r.nulltest is None
        self._r.nulltest = None

    def TestFunctions(self):
        self._r.func1()
        self._r.func2(10, 20.34)
        assert self._r.func3(2, 3.45) == 5.45
        assert self._r.meaning_of_life() == 42

        with pytest.raises(Exception):
            self._r.func_errtest()

        with pytest.raises(RR.DataTypeException):
            self._r.func_errtest1()

        e1 = RRN.GetExceptionType(
            "com.robotraconteur.testing.TestService1.testexception1", self._r)
        e3 = RRN.GetExceptionType(
            "com.robotraconteur.testing.TestService2.testexception3", self._r)

        # TODO: catch service definition exception types
        with pytest.raises(Exception):
            self._r.func_errtest2()

        with pytest.raises(Exception):
            self._r.func_errtest3()

    def TestEvents(self):
        ev1 = self._r.ev1
        ev1 += self.ev1_cb
        ev2 = self._r.ev2
        ev2 += self.ev2_cb
        try:
            self._ev1_event = threading.Event()
            self._ev2_event = threading.Event()
            self._r.func1()
            self._r.func2(27.3, 98.23)
            assert self._ev1_event.wait(2)
            assert self._ev2_event.wait(2)
        finally:
            ev1 = self._r.ev1
            ev1 -= self.ev1_cb
            ev2 = self._r.ev2
            ev2 -= self.ev2_cb
            self._ev1_event = None
            self._ev2_event = None

    def ev1_cb(self):
        self._ev1_event.set()

    def ev2_cb(self, d, s):
        if d == 27.3 and s.mydat[0] == 98.23:
            self._ev2_event.set()

    def TestObjRefs(self):
        o1 = self._r.get_o1()
        o2_10 = self._r.get_o2(10)
        o2_34 = self._r.get_o2(34)
        o3_1 = self._r.get_o3(1)
        o4_myind = self._r.get_o4("myind")
        o4_specialind = self._r.get_o4("ind!@#$%^&*().<>     ")
        o2_10_o2_1_32 = o2_10.get_o2_2(32)
        o2_10_o2_1_32_o3_ind1 = o2_10_o2_1_32.get_o3_1("ind1")
        o2_10_o2_1_32_o3_ind2 = o2_10_o2_1_32.get_o3_1("ind2")
        o1.d1 = get_test_vector("ts1.client.o_d1a", "double")
        o2_10.d1 = get_test_vector("ts1.client.o_d1b", "double")
        o2_34.d1 = get_test_vector("ts1.client.o_d1c", "double")
        o3_1.d1 = get_test_vector("ts1.client.o_d1d", "double")
        o4_myind.d1 = get_test_vector("ts1.client.o_d1e", "double")
        o4_specialind.d1 = get_test_vector("ts1.client.o_d1f", "double")
        o2_10_o2_1_32.data = "Hello world!"
        o2_10_o2_1_32_o3_ind1.data2 = "Test string 1"
        o2_10_o2_1_32_o3_ind2.data2 = "Test string 2"
        self._r.o6_op(0)
        time.sleep(0.1)
        o6_1 = self._r.get_o6()
        o6_1_1 = o6_1.get_o2_1()
        o6_1.d1 = [0.0]
        o6_1_1.data = "Hello world!"
        self._r.o6_op(1)
        time.sleep(0.1)
        with pytest.raises(Exception):
            o6_1.d1 = [0.0]
        with pytest.raises(Exception):
            o6_1_1.data = "Hello world!"

        o6_2 = self._r.get_o6()
        o6_2.data = "Hello world!"
        self._r.o6_op(2)
        time.sleep(0.1)
        with pytest.raises(Exception):
            o6_2.data = "Hello world!"
        o6_3 = self._r.get_o6()
        o6_3.add_val(2)

    def TestPipes(self):

        self._pipe_ack_lock = threading.Lock()

        self._ee1 = threading.Event()
        self._ee2 = threading.Event()
        self._ee3 = threading.Event()
        e1 = self._r.p1.Connect(-1)
        e2 = self._r.p1.Connect(3432)
        e3 = self._r.p2.Connect(-1)

        e1.RequestPacketAck = True
        e1.PacketReceivedEvent += self.ee1_cb
        e1.PacketAckReceivedEvent += self.ee1_ack_cb
        e2.PacketReceivedEvent += self.ee2_cb
        e3.PacketReceivedEvent += self.ee3_cb
        with self._pipe_ack_lock:
            self._packetnum = e1.SendPacket([1, 2, 3, 4])
        e1.SendPacket([5, 6, 7, 8])
        e1.SendPacket(get_test_vector("ts1.client.d3", "double"))
        e2.SendPacket([3.21])
        e2.SendPacket([4.72])
        e2.SendPacket([72.34])
        s1 = RRN.NewStructure(
            "com.robotraconteur.testing.TestService1.teststruct2", self._r)
        s1.mydat = [738.29]
        s2 = RRN.NewStructure(
            "com.robotraconteur.testing.TestService1.teststruct2", self._r)
        s2.mydat = [89.83]
        e3.SendPacket(s1)
        e3.SendPacket(s2)
        assert self._ee1.wait(5)
        assert self._ee2.wait(5)
        # time.sleep(1)
        assert self._ee3.wait(5)

        ca(e1.ReceivePacket(), [1, 2, 3, 4])
        ca(e1.ReceivePacketWait(1), [5, 6, 7, 8])
        ca(e1.ReceivePacketWait(1), get_test_vector("ts1.client.d3", "double"))
        ca(e2.ReceivePacket(), [3.21])
        ca(e2.ReceivePacketWait(1), [4.72])
        ca(e2.ReceivePacketWait(1), [72.34])
        ca(e3.ReceivePacket().mydat, [738.29])
        ca(e3.ReceivePacketWait(1).mydat, [89.83])
        time.sleep(.5)
        # assert self._ack_recv:
        self._r.pipe_check_error()

        assert self._r.p1.Direction == RR.MemberDefinition_Direction_both

        assert e1.Direction == RR.MemberDefinition_Direction_both

        e1.Close()
        e2.Close()
        e3.Close()

    def ee1_cb(self, p):
        try:
            self._ee1.set()

        finally:
            pass

    def ee1_ack_cb(self, p, packetnum):
        with self._pipe_ack_lock:
            if packetnum == self._packetnum:
                self._ack_recv = True

    def ee2_cb(self, p):
        try:
            self._ee2.set()

        finally:
            pass

    def ee3_cb(self, p):
        try:
            self._ee3.set()

        finally:
            pass

    def TestCallbacks(self):
        self._cb1_called = False
        self._cb2_called = False
        self._cb3_called = False
        self._cb4_called = False
        self._cb5_called = False
        self._r.cb1.Function = self.cb1_func
        self._r.cb2.Function = self.cb2_func
        self._r.cb3.Function = self.cb3_func
        self._r.cb_meaning_of_life.Function = self.cb_meaning_of_life_func
        self._r.cb_errtest.Function = self.cb_errtest
        self._r.test_callbacks()
        assert self._cb1_called
        assert self._cb2_called
        assert self._cb3_called
        assert self._cb4_called
        assert self._cb5_called

    def cb1_func(self):
        self._cb1_called = True

    def cb2_func(self, d1, d2):
        assert d1 == 739.2 and d2 == 0.392
        self._cb2_called = True

    def cb3_func(self, d1, d2):
        self._cb3_called = True
        return d1 + d2 + 3.14

    def cb_meaning_of_life_func(self):
        self._cb4_called = True
        return 42

    def cb_errtest(self):
        self._cb5_called = True
        raise Exception("This is a test")

    def TestWires(self):
        self._w1_called = False
        self._w2_called = False
        self._w3_called = False
        w1 = self._r.w1.Connect()
        w2 = self._r.w2.Connect()
        w3 = self._r.w3.Connect()

        w1.InValueLifespan = 10

        w11 = w1.WireValueChanged
        w11 += self.w1_changed
        w21 = w2.WireValueChanged
        w21 += self.w2_changed
        w31 = w3.WireValueChanged
        w31 += self.w3_changed
        w1.OutValue = [0]
        s1 = RRN.NewStructure(
            "com.robotraconteur.testing.TestService1.teststruct2", self._r)
        s1.mydat = [0]
        w2.OutValue = s1
        a1 = np.zeros((1, 1), np.int32)
        w3.OutValue = a1
        w1.OutValue = get_test_vector("ts1.client.d1", "double")
        s2 = RRN.NewStructure(
            "com.robotraconteur.testing.TestService1.teststruct2", self._r)
        s2.mydat = get_test_vector("ts1.client.d2", "double")
        w2.OutValue = s2
        a2 = np.array(get_test_vector("ts1.client.i3", "int32"), np.int32).reshape(
            get_test_vector("ts1.client.i3.dims", "uint32"), order="F")
        w3.OutValue = a2
        time.sleep(.5)
        in1 = w1.InValue
        ca(in1, get_test_vector("ts1.client.d1", "double"))
        in2 = w2.InValue
        ca(in2.mydat, get_test_vector("ts1.client.d2", "double"))
        in3 = w3.InValue
        nptest.assert_array_equal(in3, np.array(get_test_vector("ts1.client.i3", "int32")).reshape(
            get_test_vector("ts1.client.i3.dims", "uint32"), order="F"))
        assert self._w1_called
        assert self._w2_called
        assert self._w3_called

        (res, in1_2, in1_2_ts) = w1.TryGetInValue()
        assert res
        ca(in1_2, get_test_vector("ts1.client.d1", "double"))

        (res, out1_2, out1_2_ts) = w1.TryGetOutValue()
        assert res
        ca(out1_2, get_test_vector("ts1.client.d1", "double"))

        assert self._r.w1.Direction == RR.MemberDefinition_Direction_both

        assert w1.Direction == RR.MemberDefinition_Direction_both

        w1.InValueLifespan = 0.001
        time.sleep(0.01)
        with pytest.raises(Exception):
            in1_2 = w1.InValue

        w1.Close()
        w2.Close()
        w3.Close()

    def w1_changed(self, c, value, t):
        self._w1_called = True

    def w2_changed(self, c, value, t):
        self._w2_called = True

    def w3_changed(self, c, value, t):
        self._w3_called = True

    def TestMemories(self):
        self.test_m1()
        self.test_m2()
        self.test_m3()

    def test_m1(self):
        assert self._r.m1.Length == 100
        m1_1 = [0] * 11
        self._r.m1.Read(10, m1_1, 1, 10)
        ca(m1_1, get_test_vector("ts1.client.m1_1a", "double"))
        m1_2 = get_test_vector("ts1.client.m1_2a", "double")
        self._r.m1.Write(20, m1_2, 2, 8)
        m1_3 = [0] * 100
        self._r.m1.Read(0, m1_3, 0, 100)
        m1_4 = get_test_vector("ts1.m1.initial", "double")
        # Array.Copy(m1_2, 2, m1_4, 20, 8)
        m1_4[20:(20 + 8)] = m1_2[2:(2 + 8)]
        ca(m1_3, m1_4)

    def test_m2(self):
        rng = lfsr.LFSRSeqGen(1234, "multidimarray_test_double")
        assert self._r.m2.DimCount == 5
        m2_dims = self._r.m2.Dimensions
        ca(m2_dims, [10, 10, 10, 10, 10])
        m1 = multidim_array_util.fill_double_multidim_array(
            rng, [10, 10, 10, 10, 10])
        m2 = multidim_array_util.fill_double_multidim_array(
            rng, [2, 8, 8, 2, 2])
        m2_1 = multidim_array_util.get_multidim_array_sub(
            m2, [0, 2, 0, 0, 0], [1, 5, 5, 2, 1])
        m3 = np.copy(m1)
        multidim_array_util.set_multidim_array_sub(m3, [2, 2, 3, 3, 4], m2_1)
        m4 = multidim_array_util.get_multidim_array_sub(
            m3, [4, 2, 2, 8, 0], [2, 2, 1, 1, 10])
        m5 = np.zeros((4, 4, 4, 4, 10), dtype=np.float64)
        multidim_array_util.set_multidim_array_sub(m5, [2, 1, 2, 1, 0], m4)

        self._r.m2.Write([0, 0, 0, 0, 0], m1, [0, 0, 0, 0, 0], m1.shape)
        m1_2 = np.zeros([10, 10, 10, 10, 10])
        self._r.m2.Read([0, 0, 0, 0, 0], m1_2, [0, 0, 0, 0, 0], m1.shape)
        nptest.assert_allclose(m1, m1_2)
        self._r.m2.Write([2, 2, 3, 3, 4], m2, [0, 2, 0, 0, 0], [1, 5, 5, 2, 1])
        m2_2 = np.zeros([10, 10, 10, 10, 10])
        self._r.m2.Read([0, 0, 0, 0, 0], m2_2, [0, 0, 0, 0, 0], m1.shape)
        nptest.assert_allclose(m2_2, m3)
        m6 = np.zeros([2, 2, 1, 1, 10])
        self._r.m2.Read([4, 2, 2, 8, 0], m6, [0, 0, 0, 0, 0], [2, 2, 1, 1, 10])
        nptest.assert_allclose(m4, m6)
        m7 = np.zeros([4, 4, 4, 4, 10])
        self._r.m2.Read([4, 2, 2, 8, 0], m7, [2, 1, 2, 1, 0], [2, 2, 1, 1, 10])
        nptest.assert_allclose(m5, m7)

    def test_m3(self):
        rng = lfsr.LFSRSeqGen(1234, "multidimarray_test_byte")
        m1 = multidim_array_util.fill_uint8_multidim_array(rng, [1024, 1024])
        m2 = multidim_array_util.fill_uint8_multidim_array(rng, [450, 450])
        m2_1 = multidim_array_util.get_multidim_array_sub(
            m2, [20, 25], [200, 200])
        m3 = np.copy(m1)
        multidim_array_util.set_multidim_array_sub(m3, [50, 100], m2_1)
        m4 = multidim_array_util.get_multidim_array_sub(
            m3, [65, 800], [200, 200])
        m5 = np.zeros((512, 512), dtype=np.uint8)
        multidim_array_util.set_multidim_array_sub(m5, [100, 230], m4)

        self._r.m3.Write([0, 0], m1, [0, 0], [1024, 1024])
        m1_2 = np.zeros([1024, 1024], dtype=np.uint8)
        self._r.m3.Read([0, 0], m1_2, [0, 0], [1024, 1024])
        nptest.assert_allclose(m1, m1_2)
        self._r.m3.Write([50, 100], m2, [20, 25], [200, 200])
        m2_2 = np.zeros([1024, 1024], dtype=np.uint8)
        self._r.m3.Read([0, 0], m2_2, [0, 0], [1024, 1024])
        nptest.assert_allclose(m3, m2_2)
        m6 = np.zeros([200, 200], np.uint8)
        self._r.m3.Read([65, 800], m6, [0, 0], [200, 200])
        nptest.assert_allclose(m4, m6)
        m7 = np.zeros([512, 512], dtype=np.uint8)
        self._r.m3.Read([65, 800], m7, [100, 230], [200, 200])
        nptest.assert_allclose(m5, m7)

    def TestAuthentication(self, url):
        # Test two different logins

        cred1 = {"password": RR.VarValue("testpass1", "string")}
        r1 = RRN.ConnectService(url, "testuser1", cred1)
        r1.func3(2.2, 3.3)
        RRN.DisconnectService(r1)

        cred2 = {"password": RR.VarValue("testpass2", "string")}
        r2 = RRN.ConnectService(url, "testuser2", cred2)
        r2.func3(2.2, 3.3)
        RRN.DisconnectService(r2)
        # Check an invalid password

        r3 = None

        with pytest.raises(Exception):
            r3 = RRN.ConnectService(url, "testuser2", cred1)
            r3.func3(2.2, 3.3)

        try:
            if (r3 != None):
                RRN.DisconnectService(r3)
        except:
            pass

        # Check no password
        r4 = None
        with pytest.raises(Exception):
            r4 = RRN.ConnectService(url)
            r4.func3(2.2, 3.3)

        try:
            RRN.DisconnectService(r4)
        except:
            pass

    def TestObjectLock(self, url):
        # Run a test, check 6 things:
        # 1. Exclusive username lock works as expected (user with lock can access, other user can't)
        # 2. Check that session-level lock works as expected (only one session can access the locked object)
        # 3. Check that "sub-tree" objects lock as expected
        # 5. Object lock release works
        # 6. Object lock override works
        # Log in twice

        cred1 = {"password": RR.VarValue("testpass1", "string")}
        r1 = RRN.ConnectService(url, "testuser1", cred1)
        # Log in again with "testuser1" username

        cred2 = {"password": RR.VarValue("testpass2", "string")}
        r2 = RRN.ConnectService(url, "testuser1", cred1)
        r1_o = r1.get_o1()
        r1_o_o2 = r1_o.get_o2_1()
        r2_o = r2.get_o1()
        r2_o_o2 = r2_o.get_o2_1()
        r3 = RRN.ConnectService(url, "testuser2", cred2)
        r3_o = r3.get_o1()
        r3_o_o2 = r3_o.get_o2_1()
        # Make sure all the objects work
        r1.func3(2.2, 3.3)
        r1_o.d1 = [1.0]
        r1_o_o2.data = "Hello world"
        r2.func3(2.2, 3.3)
        r2_o.d1 = [1.0]
        r2_o_o2.data = "Hello world"
        r3.func3(2.2, 3.3)
        r3_o.d1 = [1.0]
        r3_o_o2.data = "Hello world"
        # Lock the object by username
        RRN.RequestObjectLock(r1_o, RR.RobotRaconteurObjectLockFlags_USER_LOCK)
        # Check that all users that should access the objects can
        r1.func3(2.2, 3.3)
        r1_o.d1 = [1.0]
        r1_o_o2.data = "Hello world"
        r2.func3(2.2, 3.3)
        r2_o.d1 = [1.0]
        r2_o_o2.data = "Hello world"
        r3.func3(2.2, 3.3)
        # Check that objects that shouldn't be able to access the objects can't

        with pytest.raises(Exception):
            r3_o.d1 = [1.0]
        with pytest.raises(Exception):
            r3_o_o2.data = "Hello world"

        # Unlock and recheck all
        RRN.ReleaseObjectLock(r1_o)
        r1.func3(2.2, 3.3)
        r1_o.d1 = [1.0]
        r1_o_o2.data = "Hello world"
        r2.func3(2.2, 3.3)
        r2_o.d1 = [1.0]
        r2_o_o2.data = "Hello world"
        r3.func3(2.2, 3.3)
        r3_o.d1 = [1.0]
        r3_o_o2.data = "Hello world"
        # Relock, test that the lock is active, and then close the connection.  The lock should release.  The
        # second session is closed first, and should not release the lock.
        RRN.RequestObjectLock(r1_o, RR.RobotRaconteurObjectLockFlags_USER_LOCK)
        with pytest.raises(Exception):
            r3_o.d1 = [1.0]
        r2_o.d1 = [1.0]
        RRN.DisconnectService(r2)
        # Object still should be locked

        with pytest.raises(Exception):
            r3_o.d1 = [1.0]

        # Now close the session and lock should be released
        RRN.DisconnectService(r1)
        r3_o.d1 = [1.0]
        # Reconnect the first two test sessions
        r1 = RRN.ConnectService(url, "testuser1", cred1)
        r2 = RRN.ConnectService(url, "testuser2", cred2)
        r1_o = r1.get_o1()
        r1_o_o2 = r1_o.get_o2_1()
        r2_o = r2.get_o1()
        r2_o_o2 = r2_o.get_o2_1()
        # Test the exclusive client lock
        RRN.RequestObjectLock(
            r1_o, RR.RobotRaconteurObjectLockFlags_CLIENT_LOCK)
        r1_o.d1 = [1.0]
        with pytest.raises(Exception):
            r2_o.d1 = [1.0]

        with pytest.raises(Exception):
            r3_o.d1 = [1.0]

        # Test the lock override by testsuperpass

        cred5 = {"password": RR.VarValue("superpass1", "string")}
        r5 = RRN.ConnectService(url, "testsuperuser", cred5)
        r5_o = r5.get_o1()
        RRN.ReleaseObjectLock(r5_o)
        # Make sure the lock is released
        r2_o.d1 = [1.0]
        r3_o.d1 = [1.0]
        # Close all connections
        RRN.DisconnectService(r1)
        RRN.DisconnectService(r2)
        RRN.DisconnectService(r3)
        RRN.DisconnectService(r5)

    def TestMonitorLock(self, url):
        # The monitor lock acquires an exclusive sock to a single thread on the client.
        # This lock is for a single thread, and works for all clients and the service if
        # it is checking for monitor locks.
        r1 = RRN.ConnectService(url)
        r2 = RRN.ConnectService(url)
        r1_o = r1.get_o1()
        r1_o_o2 = r1_o.get_o2_1()
        r2_o = r2.get_o1()
        r2_o_o2 = r2_o.get_o2_1()
        # Make sure everything works
        r1.func3(2.2, 3.3)
        r1_o.d1 = [1.0]
        r1_o_o2.data = "Hello world"
        r2.func3(2.2, 3.3)
        r2_o.d1 = [1.0]
        r2_o_o2.data = "Hello world"
        # Make sure that locks work as expected
        t1 = False
        t2 = False
        e1 = threading.Event()
        # AutoResetEvent e2 = new AutoResetEvent(false);
        threaderr = False

        def f1():
            try:
                r2.func3 = (2.2, 3.3)
                r2_o_o2.data = "Hello world"

                e1.set()
                RRN.MonitorEnter(r2_o)
                assert not t1
                t2 = True

                r2_o.d1 = [0.0]
                RRN.MonitorExit(r2_o)
            except:
                import traceback
                traceback.print_exc()
                threaderr = True

        with RRN.ScopedMonitorLock(r1_o):
            RRN.PostToThreadPool(f1)
            t1 = True

            time.sleep(0.01)
            r1.func3(2.2, 3.3)
            r1_o.d1 = [0.0]
            assert not t2
            t1 = False

        time.sleep(0.5)
        assert not threaderr

    def TestAsync(self, url):
        async_wait = threading.Event()
        async_err = [None]

        def TestAsync_err(err):
            async_err[0] = err
            async_wait.set()
            print(err)
            traceback.print_stack()

        def TestAsync1(r, err):

            if (err is not None):
                TestAsync_err(err)
                return
            try:
                r.async_get_d2(functools.partial(TestAsync2, r))
            except Exception as e:
                TestAsync_err(e)

        def TestAsync2(r, value, err):

            if (err is not None):
                TestAsync_err(err)
                return
            try:
                ca(value, get_test_vector("ts1.d2.to_client", "double"))
                r.async_set_d2(get_test_vector("ts1.d2.to_service", "double"), functools.partial(TestAsync3, r))
            except Exception as e:
                TestAsync_err(e)

        def TestAsync3(r, err):
            if (err is not None):
                TestAsync_err(err)
                return
            try:
                r.async_func1(functools.partial(TestAsync4, r))
            except Exception as e:
                TestAsync_err(e)

        def TestAsync4(r, err):
            if (err is not None):
                TestAsync_err(err)
                return
            try:
                r.async_func3(2, 3.45, functools.partial(TestAsync5, r))
            except Exception as e:
                TestAsync_err(e)

        def TestAsync5(r, ret, err):
            if (err is not None):
                TestAsync_err(err)
                return
            try:
                assert ret == 5.45
                r.async_func_errtest(functools.partial(TestAsync6, r))
            except Exception as e:
                TestAsync_err(e)

        def TestAsync6(r, err):
            try:
                assert err is not None
                r.async_get_o1(functools.partial(TestAsync7, r))
            except Exception as e:
                TestAsync_err(e)

        def TestAsync7(r, o1, err):
            if (err is not None):
                TestAsync_err(err)
                return
            try:
                assert o1 is not None
                o1.d1 = 10.2
                dd = o1.d1

                r.p1.AsyncConnect(-1, functools.partial(TestAsync8, r))
            except Exception as e:
                TestAsync_err(e)

        def TestAsync8(r, e1, err):
            if (err is not None):
                TestAsync_err(err)
                return
            try:
                e1.AsyncSendPacket(
                    [1, 2, 3, 4], functools.partial(TestAsync9, r, e1))
            except Exception as e:
                TestAsync_err(e)

        def TestAsync9(r, e1, pnum, err):
            if (err is not None):
                TestAsync_err(err)
                return
            try:
                e1.AsyncClose(functools.partial(TestAsync10, r, e1))
            except Exception as e:
                TestAsync_err(e)
                return

        def TestAsync10(r, e1, err):
            if (err is not None):
                TestAsync_err(err)
                return
            try:
                r.w1.AsyncConnect(functools.partial(TestAsync11, r))
            except Exception as e:
                TestAsync_err(e)
                return

        def TestAsync11(r, w1, err):
            if (err is not None):
                TestAsync_err(err)
                return
            try:
                w1.OutValue = 0.0
                w1.AsyncClose(functools.partial(TestAsync12, r, w1))
            except Exception as e:
                TestAsync_err(e)

        def TestAsync12(r, w1, err):
            if (err is not None):
                TestAsync_err(err)
                return
            try:
                RRN.AsyncRequestObjectLock(
                    r, RR.RobotRaconteurObjectLockFlags_CLIENT_LOCK, functools.partial(TestAsync13, r))
            except Exception as e:
                TestAsync_err(e)

        def TestAsync13(r, res, err):
            if (err is not None):
                TestAsync_err(err)
                return
            if (res != "OK"):
                TestAsync_err(Exception(""))
                return
            try:
                RRN.AsyncReleaseObjectLock(
                    r, functools.partial(TestAsync14, r))
            except Exception as e:
                TestAsync_err(e)

        def TestAsync14(r, res, err):
            if (err is not None):
                TestAsync_err(err)
                return
            if (res != "OK"):
                TestAsync_err(Exception(""))
                return
            try:
                RRN.AsyncDisconnectService(r, TestAsync15)
            except Exception as e:
                TestAsync_err(e)

        def TestAsync15():

            async_wait.set()

        cred1 = {"password": RR.VarValue("testpass1", "string")}
        RRN.AsyncConnectService(url, "testuser1", cred1, None, TestAsync1)

        async_wait.wait()

        if (async_err[0]):
            raise async_err[0]
