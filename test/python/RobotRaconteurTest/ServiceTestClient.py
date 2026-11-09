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
        self._r.d3 = [9.025110e-18, 3.567231e+17, 2.594489e+01, 2.311708e-04, 7.345164e+13, 6.550284e-01, 1.969554e+12, 9.451979e-05,
                      5.900637e-09, 9.975667e+03, 6.549533e-17, 2.227145e-13, 2.822132e+18, 4.332600e+18, 1.485466e+05, 5.844952e-14]
        ca(self._r.d3, [2.047398e-20, 2.091541e-20, 9.084241e+14, 1.583413e+01, 5.168067e-02, 1.360920e-11, 9.818531e-21,
           6.293083e+07, 4.406956e-14, 8.540213e-09, 7.329310e-03, 5.566796e+00, 3.968358e-08, 4.928656e-08, 5.994301e-20, 8.281551e-21])
        self._r.d4 = [-4.207179e-09, -3.611333e+11, -4.155626e-06, -2.458459e+10, 2.826045e-11, 3.511191e-08,
                      4.759250e-07, 2.455883e+09, 4.182578e+11, 4.732337e-14, -2.967313e+02, -4.139188e+14, 6.287269e+03]
        ca(self._r.d4, [2.864760e-08, 3.900663e+13, 9.105789e+11,
           2.943743e-15, -2.823159e-16, -3.481261e+19])
        d5_1 = np.array(get_test_vector("ts1.d5.to_service", "double")).reshape([8, 6, 2], order="F")
        self._r.d5 = d5_1
        d5_2 = self._r.d5
        nptest.assert_allclose(d5_2, np.array(get_test_vector(
            "ts1.d5.to_client", "double")).reshape((5, 6, 5), order="F"))

        d6_1 = np.array([-5.528040e-08, 3.832644e-01, -9.139211e-22, -4.919312e-05, 3.809620e-11,
                        1.751983e-09, 2.207872e-21, 1.432794e+09, -1.970313e+11]).reshape((3, 3), order="F")
        self._r.d6 = d6_1
        d6_2 = self._r.d6
        nptest.assert_allclose(d6_2, np.array([4.427272e-10, -1.149547e-08, -1.134096e+16, -4.932974e-03, -
                               7.702447e-01, -3.468374e-03, -5.037849e-14, -4.140513e-08, 4.553774e+03]).reshape((3, 3), order="F"))

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
        u8_3_1 = np.array(get_test_vector("ts1.u8_3.to_service", "uint8"), dtype=np.uint8).reshape([30, 10], order="F")
        self._r.u8_3 = u8_3_1
        u8_3_2 = self._r.u8_3

        nptest.assert_array_equal(u8_3_2, np.array(get_test_vector("ts1.u8_3.to_client",
                                  "uint8"), dtype=np.uint8).reshape([10, 20], order="F"))

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
        nptest.assert_array_equal(self._r.u64_2, get_test_vector("ts1.u64_2.to_client", "uint64"))
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
        ca(s1.dstruct2["anothertest"].mydat, [4.856615e+15, 5.981566e-22, 1.433616e+14, 1.747102e-09, 2.850376e+06, -3.748685e-08, -4.969544e-21, 2.530419e-01,
           4.393913e-09, 3.837331e+04, -4.315065e-04, -1.073834e-17, 1.244057e-15, 3.901853e-10, -2.725237e+10, 2.896243e-18, 3.609897e-13, -1.937982e+02])
        nptest.assert_allclose(s1.multidimarray, np.array(get_test_vector(
            "ts1.struct1.multidimarray.to_client", "double")).reshape((10, 10), order="F"))
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
                                    "double")).reshape([10, 10], order="F")
        s2.var3 = RR.VarValue([6.404176e-12, 9.258110e-03, 8.657620e-03, -2.064381e+00, 5.182360e-16, 4.167658e-16, -4.533051e-19,
                              5.357520e+18, -4.990383e-13, 2.286982e+08, -4.727256e-18, 1.465299e-17, 3.000340e-10, -2.304453e-04], "double[]")
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
        ca(is_d3_1[12], [8.609080e-13, 3.946603e+03, 2.994203e-10, 3.200877e+14, 1.747361e-09, 2.827056e-16, -3.676613e-18, 1.886901e-14, -9.970511e-12,
           1.932468e-18, -3.629253e-05, 4.903023e-12, -3.919949e-10, 4.982164e+07, 3.823096e-20, -4.044068e-13, 3.114078e+09, 7.572697e-12, -2.619929e+04, -3.882046e+01])
        ca(is_d3_1[832], [4.750899e+00, 3.924377e+18, -2.735066e+17, 4.095362e-21, -2.407932e+09, 4.059499e+10, 1.376975e-10, -8.547220e-21, -1.344568e-20,
           2.809398e+03, 2.118944e-06, 2.435328e-03, -1.410999e-12, 9.907226e-04, -9.745948e-20, 1.270118e+15, -2.833333e+05, 1.032636e-10, 5.312574e+13, -2.651512e+02])
        is_d3_2 = {}
        is_d3_2[47] = [4.335907e-08, -3.270294e-03, 1.752801e-01, 1.235219e-20, -4.348647e+02, -4.503864e-21, -3.316231e+15, -2.080056e+17, 1.813854e+13, -
                       3.380846e-05, 4.350998e+03, 4.539570e+11, 8.981827e+09, 3.326114e+01, 2.975688e+06, -1.017456e-12, 2.989498e-03, 2.842392e-03, -1.258677e-21, 1.068563e-15]
        is_d3_2[324] = [3.239279e+12, 1.047689e+17, -1.236114e+17, -4.002822e-17, 2.657374e-03, 7.383907e-19, -5.067889e-13, -4.195122e-12, 3.642885e-01, -
                        2.946040e+14, 5.522403e-08, 6.603132e+04, 1.464154e+05, -1.851534e-08, 2.808294e-13, -2.702278e-11, 3.850704e-06, -2.453957e+02, -3.015401e-02, 1.654070e+05]
        self._r.is_d3 = is_d3_2
        is_d4_1 = self._r.is_d4
        assert len(is_d4_1) == 2
        ca(is_d4_1["testval1"], [1.113851e-04, 3.830104e+07, 4.571169e-21, -4.064180e-05, 2.889736e+01, -1.790060e-06, 4.608538e+00, 4.687713e-04, 1.387717e-08,
           3.914187e-18, -5.618118e-06, 1.530811e+05, -5.848922e-11, -3.397558e-20, -6.597368e-08, -3.779049e-06, 2.406033e-19, 2.507939e-10, 3.246113e-20, 1.341205e+16])
        ca(is_d4_1["testval2"], [-3.088190e-13, -4.033334e-20, 4.150103e-21, -6.610855e+17, 3.688824e-13, -3.208025e+13, -5.034888e-11, -4.098363e-06, -1.272830e-03,
           2.748392e-03, -2.644272e-06, -4.810065e-18, 4.629861e-19, -5.444015e-03, 4.046008e+17, -3.548079e+12, -3.455290e+16, -3.668946e-12, -3.522178e-01, -1.537583e+14])
        is_d4_2 = {}
        is_d4_2["testval3"] = [1.771838e+06, 3.037284e-01, -1.739742e-02, 1.399508e-20, 3.605232e-21, 3.517522e+14, 4.887514e+14, 3.505442e-03, -3.968972e+18,
                               1.422037e-20, 2.596937e-21, 4.852833e-11, 6.852955e-17, 4.765526e-12, -3.445954e+16, 2.322531e-14, -1.755122e-12, 3.941875e+00, 8.877046e-13, 2.818923e-02]
        is_d4_2["testval4"] = [4.146439e+16, 2.923439e-07, 3.549608e+16, -1.664891e-01, -4.192309e-15, 3.857317e+05, -1.101076e+00, 1.213105e+19, 3.237584e-14, -
                               2.421219e-06, -4.603196e-05, -3.719535e-10, 1.124961e+06, 2.032849e+10, 4.639704e-22, 3.946835e+01, -9.267263e+01, -4.456188e+11, 3.470487e+08, 7.918764e+10]
        self._r.is_d4 = is_d4_2
        is_d5_1 = self._r.is_d5
        assert len(is_d5_1) == 1
        is_d5_1_1 = is_d5_1[564]
        nptest.assert_allclose(is_d5_1_1, np.array(get_test_vector(
            "ts1.is_d5.to_client", "double")).reshape([10, 10], order="F"))
        is_d5_2_1 = np.array(get_test_vector("ts1.is_d5.to_service", "double")).reshape([10, 20], order="F")
        is_d5_2 = {}
        is_d5_2[328] = is_d5_2_1
        self._r.is_d5 = is_d5_2
        is_d6_1 = self._r.is_d6
        assert len(is_d6_1) == 1
        is_d6_1_1 = is_d6_1["testval1"]

        nptest.assert_allclose(is_d6_1_1, np.array(get_test_vector(
            "ts1.is_d6.to_client", "double")).reshape([5, 10], order="F"))
        is_d6_2_1 = np.array(get_test_vector("ts1.is_d6.to_service", "double")).reshape([8, 10], order="F")
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
        ca(is_struct1_1[748].mydat, [-9.692618e+00, -1.944240e+03, -2.456327e+16, 4.673405e-20, 5.147581e-14, -3.773975e+15, 2.336430e-21, 1.597144e-18, -2.609059e-03,
           3.557639e-21, -1.666575e-16, -4.242788e-07, 2.686206e+07, -3.200902e-05, -1.549754e-06, -3.010796e-12, 4.638418e+01, 2.664397e-14, -2.689174e+01, 4.564584e-21])
        is_struct1_2 = {}
        is_struct1_2_1 = RRN.NewStructure(
            "com.robotraconteur.testing.TestService1.teststruct2", self._r)
        is_struct1_2_1.mydat = [-2.101948e-07, -2.594836e-08, 2.515710e+01, -3.834127e-14, -3.088095e+06, -3.256612e-02, -1.855481e-19, 3.801916e+07, 2.145894e+09,
                                4.487676e+12, 1.351202e-02, -1.125124e-16, 1.369826e-20, -2.290673e+00, 1.786029e-20, -4.991515e+08, 4.006107e-10, -4.947871e-11, -2.737020e-08, 4.123759e-20]
        is_struct1_2[372] = is_struct1_2_1
        self._r.is_struct1 = is_struct1_2
        is_struct2_1 = self._r.is_struct2
        ca(is_struct2_1["testval1"].mydat, [-4.489570e+13, 9.574895e-05, 4.081711e+06, 5.612839e-18, -1.078604e+05, 3.658139e+08, -4.748975e+05, -2.606481e+01, 3.016739e+15,
           3.174709e+19, -4.572549e+17, 1.980389e-04, -3.551911e-10, 3.598401e-07, 2.659416e-12, -3.606157e+06, 2.059674e+17, -9.362336e-20, -3.299256e+17, -2.245745e+16])
        is_struct2_2 = {}
        is_struct2_2_1 = RRN.NewStructure(
            "com.robotraconteur.testing.TestService1.teststruct2", self._r)
        is_struct2_2_1.mydat = [6.931327e-21, 4.527137e-02, 1.260822e-18, 3.592805e-12, 1.088317e-05, 3.305865e+03, -9.798828e-20, 1.497504e+18, -3.653592e+01,
                                1.473952e+10, -1.003612e-20, 1.302159e+18, -8.544326e+05, 1.038521e+16, -2.845746e-18, -3.899909e-04, 4.785560e-02, -7.203365e-12, -1.500022e-14, -1.892753e-17]
        is_struct2_2["testval2"] = is_struct2_2_1
        self._r.is_struct2 = is_struct2_2

        list_d1_1 = self._r.list_d1
        assert len(list_d1_1) == 3
        assert list_d1_1[0] == 1.643392e-01
        assert list_d1_1[1] == 1.537133e+09
        assert list_d1_1[2] == 1.369505e-03
        list_d1_2 = [4.074501e-07, -4.535303e+05, -2.956241e-20]
        self._r.list_d1 = list_d1_2

        ca(self._r.struct3.a1, [-8.483090e-19, -4.401548e-08, 3.908118e+00, 2.063513e-18, 4.237047e+18, -1.124681e-16, 3.924541e-01, -2.184335e-10, -1.978950e+11,
           1.586365e+18, 1.712393e+00, -6.314723e+00, 1.196777e-16, -2.748704e-08, -1.289967e+02, -4.051137e+17, -1.902860e+10, -2.070486e+08, 3.622651e+06, 1.315398e+17])
        struct3_1 = RRN.NewStructure(
            "com.robotraconteur.testing.TestService2.ostruct2", self._r)
        struct3_1.a1 = [-2.426765e+05, -9.410735e+01, -1.667915e+12, -4.084240e-05, 3.199460e+03, 8.256717e-12, -4.772119e-11, -1.061407e-13, 2.759750e+02, -
                        1.212549e+10, 7.012690e+15, 3.953354e+04, -2.617985e-07, 1.104408e-21, -3.889366e+00, 4.549493e+16, -1.376791e+15, -3.445205e-21, 2.137830e-14, 4.620179e+18]
        self._r.struct3 = struct3_1

        list_d3_1 = self._r.list_d3
        assert len(list_d3_1) == 2
        ca(list_d3_1[0], [8.609080e-13, 3.946603e+03, 2.994203e-10, 3.200877e+14, 1.747361e-09, 2.827056e-16, -3.676613e-18, 1.886901e-14, -9.970511e-12,
           1.932468e-18, -3.629253e-05, 4.903023e-12, -3.919949e-10, 4.982164e+07, 3.823096e-20, -4.044068e-13, 3.114078e+09, 7.572697e-12, -2.619929e+04, -3.882046e+01])
        ca(list_d3_1[1], [4.750899e+00, 3.924377e+18, -2.735066e+17, 4.095362e-21, -2.407932e+09, 4.059499e+10, 1.376975e-10, -8.547220e-21, -1.344568e-20,
           2.809398e+03, 2.118944e-06, 2.435328e-03, -1.410999e-12, 9.907226e-04, -9.745948e-20, 1.270118e+15, -2.833333e+05, 1.032636e-10, 5.312574e+13, -2.651512e+02])
        list_d3_2 = [
            [4.335907e-08, -3.270294e-03, 1.752801e-01, 1.235219e-20, -4.348647e+02, -4.503864e-21, -3.316231e+15, -2.080056e+17, 1.813854e+13, -3.380846e-05, 4.350998e+03, 4.539570e+11, 8.981827e+09, 3.326114e+01, 2.975688e+06, -1.017456e-12, 2.989498e-03, 2.842392e-03, -1.258677e-21, 1.068563e-15], [
                3.239279e+12, 1.047689e+17, -1.236114e+17, -4.002822e-17, 2.657374e-03, 7.383907e-19, -5.067889e-13, -4.195122e-12, 3.642885e-01, -2.946040e+14, 5.522403e-08, 6.603132e+04, 1.464154e+05, -1.851534e-08, 2.808294e-13, -2.702278e-11, 3.850704e-06, -2.453957e+02, -3.015401e-02, 1.654070e+05]
        ]
        self._r.list_d3 = list_d3_2

        list_d5_1 = self._r.list_d5
        assert len(list_d5_1) == 1
        list_d5_1_1 = list_d5_1[0]
        nptest.assert_allclose(list_d5_1_1, np.array(get_test_vector(
            "ts1.is_d5.to_client", "double")).reshape([10, 10], order="F"))
        list_d5_2_1 = np.array(get_test_vector("ts1.is_d5.to_service", "double")).reshape([10, 20], order="F")
        list_d5_2 = [list_d5_2_1]
        self._r.list_d5 = list_d5_2

        list_struct2_1 = self._r.list_struct1
        ca(list_struct2_1[0].mydat, [-9.692618e+00, -1.944240e+03, -2.456327e+16, 4.673405e-20, 5.147581e-14, -3.773975e+15, 2.336430e-21, 1.597144e-18, -2.609059e-03,
           3.557639e-21, -1.666575e-16, -4.242788e-07, 2.686206e+07, -3.200902e-05, -1.549754e-06, -3.010796e-12, 4.638418e+01, 2.664397e-14, -2.689174e+01, 4.564584e-21])
        list_struct2_2 = []
        list_struct2_2_1 = RRN.NewStructure(
            "com.robotraconteur.testing.TestService1.teststruct2", self._r)
        list_struct2_2_1.mydat = [-2.101948e-07, -2.594836e-08, 2.515710e+01, -3.834127e-14, -3.088095e+06, -3.256612e-02, -1.855481e-19, 3.801916e+07, 2.145894e+09,
                                  4.487676e+12, 1.351202e-02, -1.125124e-16, 1.369826e-20, -2.290673e+00, 1.786029e-20, -4.991515e+08, 4.006107e-10, -4.947871e-11, -2.737020e-08, 4.123759e-20]
        list_struct2_2.append(list_struct2_2_1)
        self._r.list_struct1 = list_struct2_2

        # varvalue types (TODO: finish these...)

        ca(self._r.var_num.data, [-1680284833, -54562307, 732107275, 1470526962, -1389452949, 256801409,
           261288152, 1728150828, 1322531658, -1640628174, 1036878614, 511108054, 2057847386, 288780916, 996595759])
        self._r.var_num = RR.VarValue([-1046369769, 1950632347, 1140727074, -1277424443,
                                      163999900, 970815027, 545593183, 514305170, 1896372264, 1385916382], "int32[]")
        assert self._r.var_str.data == "Hello Client!"
        self._r.var_str = RR.VarValue("Hello Server!", "string")
        ca((self._r.var_struct.data).mydat, [-9.052731e+13, 4.151705e-17, -4.004463e+19, -2.838274e+03, 9.983314e+12, 2.764122e+10, -1.131486e+03, 2.418899e+12, 1.323675e-05, -
           4.602174e+13, 2.717530e+01, 1.193887e-10, -4.137578e+16, -1.246990e-19, 4.244315e-18, -2.833005e-08, 1.956266e-04, 4.130129e-21, 1.641708e-11, -4.488158e-19])
        var_struct_1 = RRN.NewStructure(
            "com.robotraconteur.testing.TestService1.teststruct2", self._r)
        var_struct_1.mydat = [-4.945426e-20, 1.763386e+13, 3.431578e-04, 4.411409e+17, -2.690201e+03, 3.025939e-10, -3.659846e+11, -4.780435e-10, -3.246816e+14, -
                              1.815578e+04, 2.236455e+10, -4.639041e+14, 1.767930e+10, -1.636094e+05, -4.392462e-01, 2.225260e+04, -5.250245e+18, 8.755282e-12, 2.005819e-10, 2.702210e+04]
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
        nptest.assert_allclose(var_a_1, np.array([-4.915597e-01, 3.892823e+00, 2.622325e+08, -7.150935e+04, 9.418756e+00, 3.633879e+18, 3.522383e-03, -4.989811e+05, 2.027383e-03, -3.153241e+12, -
                               6.948245e-21, -3.198577e+14, 6.172905e+09, 3.849430e+15, 8.600383e+13, 4.079437e-17, 3.194775e+06, 4.222550e-18, 1.758122e+17, -1.018308e+03]).reshape([5, 4], order="F"))
        self._r.var_multidimarray = RR.VarValue(np.array([3.792953e+00, 2.968121e-17, -3.976413e-15, 4.392986e+19, 2.197463e+10, -2.627743e-14, -2.184665e+17, 1.972257e-17, 9.929684e-03, -3.096821e+17,
                                                3.598051e+11, -6.266015e-18, 1.811985e-11, 2.815232e-07, 7.469467e-06, 6.141798e+13, 3.105763e+09, -1.697809e-10, -4.141707e-17, 4.391634e+13]).reshape([5, 4], order="F"), "double[*]")

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
        o1.d1 = [-2.086627e+06, 3.092642e+04, -1.981667e+02, 1.963286e-20, 4.264052e-08,
                 3.594320e+12, -4.820517e-02, -3.629590e+06, 6.037089e-07, 3.328419e+06]
        o2_10.d1 = [4.978178e-14, 2.867603e-17, 4.471047e-21, -2.002902e+15, -2.910881e-03, -
                    2.601092e-03, -3.043199e+16, -3.257109e-12, 1.834255e-11, -3.383015e+00]
        o2_34.d1 = [4.661927e-02, 2.334444e+02, 3.985567e+12, -2.324843e+01, -
                    3.315866e+03, -4.442404e+10, 3.280626e-02, 2.334668e-12, -3.374202e-14, 4.809260e+02]
        o3_1.d1 = [-1.882441e-04, 2.065458e+14, -6.309214e-16, -3.181637e-07, -9.906616e+02,
                   1.684926e-14, 1.672252e+15, -3.950901e+01, -3.295950e-17, -3.080902e+13]
        o4_myind.d1 = [2.997950e-14, -1.077977e+17, 3.721399e-09, -1.289619e+18,
                       4.494844e+06, -4.918719e-15, 2.194759e+13, 2.554572e-09, 4.166299e-06, -1.409589e+04]
        o4_specialind.d1 = [3.404179e-08, -1.749189e-18, -3.219593e-09, 1.313794e+01, -
                            4.193673e+10, -2.479829e+07, -2.617068e+04, 8.181730e+15, -2.003653e+18, -1.833401e+19]
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
        e1.SendPacket([-1, -2, -3, -5.32])
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
        ca(e1.ReceivePacketWait(1), [-1, -2, -3, -5.32])
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
        w1.OutValue = [-2.377683e+02, -6.760080e-08, 4.191315e-18, -4.621977e+07, -
                       1.570323e+03, -4.163378e+03, -2.506701e+13, -4.755701e+18, -1.972380e-19, 1.791593e-11]
        s2 = RRN.NewStructure(
            "com.robotraconteur.testing.TestService1.teststruct2", self._r)
        s2.mydat = [-1.014645e-21, 4.743740e+11, 5.804886e-04, 2.963852e-20, 4.277621e-21, -
                    1.168151e+13, -2.638708e-18, -5.123312e+14, 1.261123e-05, 2.552626e-10]
        w2.OutValue = s2
        a2 = np.array([2058500854, -611248192, 197490486, -517717939, -513450368, 296469979,
                      645365194, 2043654604, -1672941174, 710030901], np.int32).reshape([2, 5], order="F")
        w3.OutValue = a2
        time.sleep(.5)
        in1 = w1.InValue
        ca(in1, [-2.377683e+02, -6.760080e-08, 4.191315e-18, -4.621977e+07, -1.570323e+03, -
           4.163378e+03, -2.506701e+13, -4.755701e+18, -1.972380e-19, 1.791593e-11])
        in2 = w2.InValue
        ca(in2.mydat, [-1.014645e-21, 4.743740e+11, 5.804886e-04, 2.963852e-20,
           4.277621e-21, -1.168151e+13, -2.638708e-18, -5.123312e+14, 1.261123e-05, 2.552626e-10])
        in3 = w3.InValue
        nptest.assert_allclose(in3, np.array([2058500854, -611248192, 197490486, -517717939, -513450368,
                               296469979, 645365194, 2043654604, -1672941174, 710030901]).reshape([2, 5], order="F"))
        assert self._w1_called
        assert self._w2_called
        assert self._w3_called

        (res, in1_2, in1_2_ts) = w1.TryGetInValue()
        assert res
        ca(in1_2, [-2.377683e+02, -6.760080e-08, 4.191315e-18, -4.621977e+07, -1.570323e+03, -
           4.163378e+03, -2.506701e+13, -4.755701e+18, -1.972380e-19, 1.791593e-11])

        (res, out1_2, out1_2_ts) = w1.TryGetOutValue()
        assert res
        ca(out1_2, [-2.377683e+02, -6.760080e-08, 4.191315e-18, -4.621977e+07, -
           1.570323e+03, -4.163378e+03, -2.506701e+13, -4.755701e+18, -1.972380e-19, 1.791593e-11])

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
        ca(m1_1, [0, -1.478723e-16, 1.507042e-05, -2.046271e+13, 4.014775e+06, 4.140740e+10,
           1.318907e+16, -2.312403e+17, 4.463696e-13, 9.173421e-04, 6.169183e-21])
        m1_2 = [3.462892e+10, -1.149841e-18, 4.649317e-15, -3.632280e-19, -5.252280e+07, 1.800453e-07,
                3.772468e-04, -1.911891e+09, -3.018967e-14, 4.835062e-06, 5.269663e+13, 4.946221e+03]
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
