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
        return [2.047398e-20, 2.091541e-20, 9.084241e+14, 1.583413e+01, 5.168067e-02, 1.360920e-11, 9.818531e-21, 6.293083e+07, 4.406956e-14, 8.540213e-09, 7.329310e-03, 5.566796e+00, 3.968358e-08, 4.928656e-08, 5.994301e-20, 8.281551e-21]

    @d3.setter
    def d3(self, value):
        ca(value, [9.025110e-18, 3.567231e+17, 2.594489e+01, 2.311708e-04, 7.345164e+13, 6.550284e-01, 1.969554e+12, 9.451979e-05,
           5.900637e-09, 9.975667e+03, 6.549533e-17, 2.227145e-13, 2.822132e+18, 4.332600e+18, 1.485466e+05, 5.844952e-14])

    @property
    def d4(self):
        return [2.864760e-08, 3.900663e+13, 9.105789e+11, 2.943743e-15, -2.823159e-16, -3.481261e+19]

    @d4.setter
    def d4(self, value):
        ca(value, [-4.207179e-09, -3.611333e+11, -4.155626e-06, -2.458459e+10, 2.826045e-11, 3.511191e-08,
           4.759250e-07, 2.455883e+09, 4.182578e+11, 4.732337e-14, -2.967313e+02, -4.139188e+14, 6.287269e+03])

    @property
    def d5(self):
        return np.array(get_test_vector("ts1.d5.to_client", "double")).reshape([5, 6, 5], order="F")

    @d5.setter
    def d5(self, value):
        np.testing.assert_allclose(value, np.array(get_test_vector(
            "ts1.d5.to_service", "double")).reshape([8, 6, 2], order="F"))

    @property
    def d6(self):
        return np.array([4.427272e-10, -1.149547e-08, -1.134096e+16, -4.932974e-03, -7.702447e-01, -3.468374e-03, -5.037849e-14, -4.140513e-08, 4.553774e+03]).reshape([3, 3], order="F")

    @d6.setter
    def d6(self, value):
        np.testing.assert_allclose(value, np.array([-5.528040e-08, 3.832644e-01, -9.139211e-22, -4.919312e-05,
                                   3.809620e-11, 1.751983e-09, 2.207872e-21, 1.432794e+09, -1.970313e+11]).reshape([3, 3], order="F"))

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
        return np.array(get_test_vector("ts1.u8_3.to_client", "uint8"), dtype=np.uint8).reshape([10, 20], order="F")

    @u8_3.setter
    def u8_3(self, value):

        nptest.assert_array_equal(value, np.array(get_test_vector(
            "ts1.u8_3.to_service", "uint8")).reshape([30, 10], order="F"))

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
        nptest.assert_array_equal(value, get_test_vector("ts1.u64_2.to_service", "uint64"))

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
        s1_dstruct2_2.mydat = [4.856615e+15, 5.981566e-22, 1.433616e+14, 1.747102e-09, 2.850376e+06, -3.748685e-08, -4.969544e-21, 2.530419e-01,
                               4.393913e-09, 3.837331e+04, -4.315065e-04, -1.073834e-17, 1.244057e-15, 3.901853e-10, -2.725237e+10, 2.896243e-18, 3.609897e-13, -1.937982e+02]
        s1.dstruct2 = {"test1": s1_dstruct2_1, "anothertest": s1_dstruct2_2}
        s1_lstruct3_1 = self._teststruct2_type()
        s1_lstruct3_1.mydat = get_test_vector("ts1.struct1.dstruct2_1.to_client", "double")
        s1_lstruct3_2 = self._teststruct2_type()
        s1_lstruct3_2.mydat = [4.856615e+15, 5.981566e-22, 1.433616e+14, 1.747102e-09, 2.850376e+06, -3.748685e-08, -4.969544e-21, 2.530419e-01,
                               4.393913e-09, 3.837331e+04, -4.315065e-04, -1.073834e-17, 1.244057e-15, 3.901853e-10, -2.725237e+10, 2.896243e-18, 3.609897e-13, -1.937982e+02]
        s1.lstruct3 = [s1_dstruct2_1, s1_dstruct2_2]
        s1.multidimarray = np.array(get_test_vector("ts1.struct1.multidimarray.to_client",
                                    "double")).reshape([10, 10], order="F")
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
            "ts1.struct1.multidimarray.to_service", "double")).reshape([10, 10], order="F"))
        ca(value.var3.data, [6.404176e-12, 9.258110e-03, 8.657620e-03, -2.064381e+00, 5.182360e-16, 4.167658e-16, -
           4.533051e-19, 5.357520e+18, -4.990383e-13, 2.286982e+08, -4.727256e-18, 1.465299e-17, 3.000340e-10, -2.304453e-04])

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
        out.a1 = [-8.483090e-19, -4.401548e-08, 3.908118e+00, 2.063513e-18, 4.237047e+18, -1.124681e-16, 3.924541e-01, -2.184335e-10, -1.978950e+11, 1.586365e+18,
                  1.712393e+00, -6.314723e+00, 1.196777e-16, -2.748704e-08, -1.289967e+02, -4.051137e+17, -1.902860e+10, -2.070486e+08, 3.622651e+06, 1.315398e+17]
        return out

    @struct3.setter
    def struct3(self, value):
        ca(value.a1, [-2.426765e+05, -9.410735e+01, -1.667915e+12, -4.084240e-05, 3.199460e+03, 8.256717e-12, -4.772119e-11, -1.061407e-13, 2.759750e+02, -
           1.212549e+10, 7.012690e+15, 3.953354e+04, -2.617985e-07, 1.104408e-21, -3.889366e+00, 4.549493e+16, -1.376791e+15, -3.445205e-21, 2.137830e-14, 4.620179e+18])

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
        out[12] = [8.609080e-13, 3.946603e+03, 2.994203e-10, 3.200877e+14, 1.747361e-09, 2.827056e-16, -3.676613e-18, 1.886901e-14, -9.970511e-12, 1.932468e-18, -
                   3.629253e-05, 4.903023e-12, -3.919949e-10, 4.982164e+07, 3.823096e-20, -4.044068e-13, 3.114078e+09, 7.572697e-12, -2.619929e+04, -3.882046e+01]
        out[832] = [4.750899e+00, 3.924377e+18, -2.735066e+17, 4.095362e-21, -2.407932e+09, 4.059499e+10, 1.376975e-10, -8.547220e-21, -1.344568e-20, 2.809398e+03,
                    2.118944e-06, 2.435328e-03, -1.410999e-12, 9.907226e-04, -9.745948e-20, 1.270118e+15, -2.833333e+05, 1.032636e-10, 5.312574e+13, -2.651512e+02]
        return out

    @is_d3.setter
    def is_d3(self, value):
        assert len(value) == 2
        ca(value[47], [4.335907e-08, -3.270294e-03, 1.752801e-01, 1.235219e-20, -4.348647e+02, -4.503864e-21, -3.316231e+15, -2.080056e+17, 1.813854e+13, -
           3.380846e-05, 4.350998e+03, 4.539570e+11, 8.981827e+09, 3.326114e+01, 2.975688e+06, -1.017456e-12, 2.989498e-03, 2.842392e-03, -1.258677e-21, 1.068563e-15])
        ca(value[324], [3.239279e+12, 1.047689e+17, -1.236114e+17, -4.002822e-17, 2.657374e-03, 7.383907e-19, -5.067889e-13, -4.195122e-12, 3.642885e-01, -
           2.946040e+14, 5.522403e-08, 6.603132e+04, 1.464154e+05, -1.851534e-08, 2.808294e-13, -2.702278e-11, 3.850704e-06, -2.453957e+02, -3.015401e-02, 1.654070e+05])

    @property
    def is_d4(self):
        out = {}
        out["testval1"] = [1.113851e-04, 3.830104e+07, 4.571169e-21, -4.064180e-05, 2.889736e+01, -1.790060e-06, 4.608538e+00, 4.687713e-04, 1.387717e-08,
                           3.914187e-18, -5.618118e-06, 1.530811e+05, -5.848922e-11, -3.397558e-20, -6.597368e-08, -3.779049e-06, 2.406033e-19, 2.507939e-10, 3.246113e-20, 1.341205e+16]
        out["testval2"] = [-3.088190e-13, -4.033334e-20, 4.150103e-21, -6.610855e+17, 3.688824e-13, -3.208025e+13, -5.034888e-11, -4.098363e-06, -1.272830e-03,
                           2.748392e-03, -2.644272e-06, -4.810065e-18, 4.629861e-19, -5.444015e-03, 4.046008e+17, -3.548079e+12, -3.455290e+16, -3.668946e-12, -3.522178e-01, -1.537583e+14]
        return out

    @is_d4.setter
    def is_d4(self, value):
        assert len(value) == 2
        ca(value["testval3"], [1.771838e+06, 3.037284e-01, -1.739742e-02, 1.399508e-20, 3.605232e-21, 3.517522e+14, 4.887514e+14, 3.505442e-03, -3.968972e+18,
           1.422037e-20, 2.596937e-21, 4.852833e-11, 6.852955e-17, 4.765526e-12, -3.445954e+16, 2.322531e-14, -1.755122e-12, 3.941875e+00, 8.877046e-13, 2.818923e-02])
        ca(value["testval4"], [4.146439e+16, 2.923439e-07, 3.549608e+16, -1.664891e-01, -4.192309e-15, 3.857317e+05, -1.101076e+00, 1.213105e+19, 3.237584e-14, -
           2.421219e-06, -4.603196e-05, -3.719535e-10, 1.124961e+06, 2.032849e+10, 4.639704e-22, 3.946835e+01, -9.267263e+01, -4.456188e+11, 3.470487e+08, 7.918764e+10])

    @property
    def is_d5(self):
        out = {}
        out1 = np.array(get_test_vector("ts1.is_d5.to_client", "double")).reshape([10, 10], order="F")
        out[564] = out1
        return out

    @is_d5.setter
    def is_d5(self, value):
        assert len(value) == 1
        in1 = value[328]
        np.testing.assert_allclose(in1, np.array(get_test_vector(
            "ts1.is_d5.to_service", "double")).reshape((10, 20), order="F"))

    @property
    def is_d6(self):
        out = {}
        out1 = np.array(get_test_vector("ts1.is_d6.to_client", "double")).reshape([5, 10], order="F")
        out["testval1"] = out1
        return out

    @is_d6.setter
    def is_d6(self, value):
        if (len(value) != 1):
            return Exception()
        in1 = value["testval2"]
        np.testing.assert_allclose(in1, np.array(get_test_vector(
            "ts1.is_d6.to_service", "double")).reshape([8, 10], order="F"))

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
        out1.mydat = [-9.692618e+00, -1.944240e+03, -2.456327e+16, 4.673405e-20, 5.147581e-14, -3.773975e+15, 2.336430e-21, 1.597144e-18, -2.609059e-03,
                      3.557639e-21, -1.666575e-16, -4.242788e-07, 2.686206e+07, -3.200902e-05, -1.549754e-06, -3.010796e-12, 4.638418e+01, 2.664397e-14, -2.689174e+01, 4.564584e-21]
        out[748] = out1
        return out

    @is_struct1.setter
    def is_struct1(self, value):
        assert len(value) == 1
        ca(value[372].mydat, [-2.101948e-07, -2.594836e-08, 2.515710e+01, -3.834127e-14, -3.088095e+06, -3.256612e-02, -1.855481e-19, 3.801916e+07, 2.145894e+09,
           4.487676e+12, 1.351202e-02, -1.125124e-16, 1.369826e-20, -2.290673e+00, 1.786029e-20, -4.991515e+08, 4.006107e-10, -4.947871e-11, -2.737020e-08, 4.123759e-20])

    @property
    def is_struct2(self):
        out = {}
        out1 = self._teststruct2_type()
        out1.mydat = [-4.489570e+13, 9.574895e-05, 4.081711e+06, 5.612839e-18, -1.078604e+05, 3.658139e+08, -4.748975e+05, -2.606481e+01, 3.016739e+15,
                      3.174709e+19, -4.572549e+17, 1.980389e-04, -3.551911e-10, 3.598401e-07, 2.659416e-12, -3.606157e+06, 2.059674e+17, -9.362336e-20, -3.299256e+17, -2.245745e+16]
        out["testval1"] = out1
        return out

    @is_struct2.setter
    def is_struct2(self, value):
        assert len(value) == 1
        ca(value["testval2"].mydat, [6.931327e-21, 4.527137e-02, 1.260822e-18, 3.592805e-12, 1.088317e-05, 3.305865e+03, -9.798828e-20, 1.497504e+18, -3.653592e+01,
           1.473952e+10, -1.003612e-20, 1.302159e+18, -8.544326e+05, 1.038521e+16, -2.845746e-18, -3.899909e-04, 4.785560e-02, -7.203365e-12, -1.500022e-14, -1.892753e-17])

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
        out.append([8.609080e-13, 3.946603e+03, 2.994203e-10, 3.200877e+14, 1.747361e-09, 2.827056e-16, -3.676613e-18, 1.886901e-14, -9.970511e-12, 1.932468e-18, -
                   3.629253e-05, 4.903023e-12, -3.919949e-10, 4.982164e+07, 3.823096e-20, -4.044068e-13, 3.114078e+09, 7.572697e-12, -2.619929e+04, -3.882046e+01])
        out.append([4.750899e+00, 3.924377e+18, -2.735066e+17, 4.095362e-21, -2.407932e+09, 4.059499e+10, 1.376975e-10, -8.547220e-21, -1.344568e-20, 2.809398e+03,
                   2.118944e-06, 2.435328e-03, -1.410999e-12, 9.907226e-04, -9.745948e-20, 1.270118e+15, -2.833333e+05, 1.032636e-10, 5.312574e+13, -2.651512e+02])
        return out

    @list_d3.setter
    def list_d3(self, value):
        assert len(value) == 2
        ca(value[0], [4.335907e-08, -3.270294e-03, 1.752801e-01, 1.235219e-20, -4.348647e+02, -4.503864e-21, -3.316231e+15, -2.080056e+17, 1.813854e+13, -
           3.380846e-05, 4.350998e+03, 4.539570e+11, 8.981827e+09, 3.326114e+01, 2.975688e+06, -1.017456e-12, 2.989498e-03, 2.842392e-03, -1.258677e-21, 1.068563e-15])
        ca(value[1], [3.239279e+12, 1.047689e+17, -1.236114e+17, -4.002822e-17, 2.657374e-03, 7.383907e-19, -5.067889e-13, -4.195122e-12, 3.642885e-01, -2.946040e+14,
           5.522403e-08, 6.603132e+04, 1.464154e+05, -1.851534e-08, 2.808294e-13, -2.702278e-11, 3.850704e-06, -2.453957e+02, -3.015401e-02, 1.654070e+05])

    @property
    def list_d5(self):
        out = []
        out1 = np.array(get_test_vector("ts1.is_d5.to_client", "double")).reshape([10, 10], order="F")
        out.append(out1)
        return out

    @list_d5.setter
    def list_d5(self, value):
        assert len(value) == 1
        in1 = value[0]
        np.testing.assert_allclose(in1, np.array(get_test_vector(
            "ts1.is_d5.to_service", "double")).reshape([10, 20], order="F"))

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
        out1.mydat = [-9.692618e+00, -1.944240e+03, -2.456327e+16, 4.673405e-20, 5.147581e-14, -3.773975e+15, 2.336430e-21, 1.597144e-18, -2.609059e-03,
                      3.557639e-21, -1.666575e-16, -4.242788e-07, 2.686206e+07, -3.200902e-05, -1.549754e-06, -3.010796e-12, 4.638418e+01, 2.664397e-14, -2.689174e+01, 4.564584e-21]
        out.append(out1)
        return out

    @list_struct1.setter
    def list_struct1(self, value):
        assert len(value) == 1
        ca(value[0].mydat, [-2.101948e-07, -2.594836e-08, 2.515710e+01, -3.834127e-14, -3.088095e+06, -3.256612e-02, -1.855481e-19, 3.801916e+07, 2.145894e+09,
           4.487676e+12, 1.351202e-02, -1.125124e-16, 1.369826e-20, -2.290673e+00, 1.786029e-20, -4.991515e+08, 4.006107e-10, -4.947871e-11, -2.737020e-08, 4.123759e-20])

    @property
    def is_struct3(self):
        out = self._ostruct2_type()
        out.a1 = [-8.483090e-19, -4.401548e-08, 3.908118e+00, 2.063513e-18, 4.237047e+18, -1.124681e-16, 3.924541e-01, -2.184335e-10, -1.978950e+11, 1.586365e+18,
                  1.712393e+00, -6.314723e+00, 1.196777e-16, -2.748704e-08, -1.289967e+02, -4.051137e+17, -1.902860e+10, -2.070486e+08, 3.622651e+06, 1.315398e+17]
        return out

    @is_struct3.setter
    def is_struct3(self, value):
        ca(value.a1, [-2.426765e+05, -9.410735e+01, -1.667915e+12, -4.084240e-05, 3.199460e+03, 8.256717e-12, -4.772119e-11, -1.061407e-13, 2.759750e+02, -
           1.212549e+10, 7.012690e+15, 3.953354e+04, -2.617985e-07, 1.104408e-21, -3.889366e+00, 4.549493e+16, -1.376791e+15, -3.445205e-21, 2.137830e-14, 4.620179e+18])

    @property
    def var_num(self):
        return RR.VarValue([-1680284833, -54562307, 732107275, 1470526962, -1389452949, 256801409, 261288152, 1728150828, 1322531658, -1640628174, 1036878614, 511108054, 2057847386, 288780916, 996595759], "int32[]")

    @var_num.setter
    def var_num(self, value):
        ca(value.data, [-1046369769, 1950632347, 1140727074, -1277424443,
           163999900, 970815027, 545593183, 514305170, 1896372264, 1385916382])

    @property
    def var_str(self):
        return RR.VarValue("Hello Client!", "string")

    @var_str.setter
    def var_str(self, value):
        assert value.data == "Hello Server!"

    @property
    def var_struct(self):
        out = self._teststruct2_type()
        out.mydat = [-9.052731e+13, 4.151705e-17, -4.004463e+19, -2.838274e+03, 9.983314e+12, 2.764122e+10, -1.131486e+03, 2.418899e+12, 1.323675e-05, -4.602174e+13,
                     2.717530e+01, 1.193887e-10, -4.137578e+16, -1.246990e-19, 4.244315e-18, -2.833005e-08, 1.956266e-04, 4.130129e-21, 1.641708e-11, -4.488158e-19]
        return RR.VarValue(out, "com.robotraconteur.testing.TestService1.teststruct2")

    @var_struct.setter
    def var_struct(self, value):
        ca(value.data.mydat, [-4.945426e-20, 1.763386e+13, 3.431578e-04, 4.411409e+17, -2.690201e+03, 3.025939e-10, -3.659846e+11, -4.780435e-10, -3.246816e+14, -
           1.815578e+04, 2.236455e+10, -4.639041e+14, 1.767930e+10, -1.636094e+05, -4.392462e-01, 2.225260e+04, -5.250245e+18, 8.755282e-12, 2.005819e-10, 2.702210e+04])

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
        return RR.VarValue(np.array([-4.915597e-01, 3.892823e+00, 2.622325e+08, -7.150935e+04, 9.418756e+00, 3.633879e+18, 3.522383e-03, -4.989811e+05, 2.027383e-03, -3.153241e+12, -6.948245e-21, -3.198577e+14, 6.172905e+09, 3.849430e+15, 8.600383e+13, 4.079437e-17, 3.194775e+06, 4.222550e-18, 1.758122e+17, -1.018308e+03]).reshape([5, 4], order="F"), "double[*]")

    @var_multidimarray.setter
    def var_multidimarray(self, value):
        np.testing.assert_allclose(value.data, np.array([3.792953e+00, 2.968121e-17, -3.976413e-15, 4.392986e+19, 2.197463e+10, -2.627743e-14, -2.184665e+17, 1.972257e-17, 9.929684e-03, -
                                   3.096821e+17, 3.598051e+11, -6.266015e-18, 1.811985e-11, 2.815232e-07, 7.469467e-06, 6.141798e+13, 3.105763e+09, -1.697809e-10, -4.141707e-17, 4.391634e+13]).reshape([5, 4], order="F"))

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
