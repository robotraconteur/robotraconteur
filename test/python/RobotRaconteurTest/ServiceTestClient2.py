from __future__ import absolute_import

import RobotRaconteur as RR
RRN = RR.RobotRaconteurNode.s
import numpy as np
import numpy.testing as nptest
import traceback
import time
import threading
import sys
import pytest

from .ServiceTest2 import ServiceTest2_fill_testpod1, ServiceTest2_verify_testpod1, \
    ServiceTest2_fill_teststruct3, ServiceTest2_verify_teststruct3, ServiceTest2_fill_transform, \
    ServiceTest2_verify_transform, ServiceTest2_create_transform_array, ServiceTest2_verify_transform_array, \
    ServiceTest2_create_transform_multidimarray, ServiceTest2_verify_transform_multidimarray, \
    ServiceTest2_fill_testpod2, ServiceTest2_verify_testpod2

from RobotRaconteur import RobotRaconteurPythonUtil
from .testvectors import get_test_vector

try:
    xrange  # type: ignore
except NameError:
    xrange = range

try:
    cmp  # type: ignore
except NameError:
    def cmp(a, b): return (a > b) - (a < b)

ca = nptest.assert_allclose


class ServiceTestClient2:
    def __init__(self):
        pass

    def RunFullTest(self, url):
        self.ConnectService(url)

        self.TestWirePeekPoke()
        self.AsyncTestWirePeekPoke()
        self.TestEnums()
        self.TestPod()
        self.TestGenerators()
        self.TestMemories()

        self.TestNamedArrays()
        self.TestNamedArrayMemories()

        self.TestComplex()
        self.TestComplexMemories()

        self.TestNoLock()

        self.TestBool()
        self.TestBoolMemories()

        self.TestExceptionParams()

        self.DisconnectService()

    def ConnectService(self, url):
        RRN.RequestTimeout = 30
        self._r = RRN.ConnectService(url)

    def DisconnectService(self):
        RRN.DisconnectService(self._r)

    def TestWirePeekPoke(self):
        (v, ts) = self._r.peekwire.PeekInValue()
        assert v == 56295674

        self._r.pokewire.PokeOutValue(75738265)
        (v2, ts2) = self._r.pokewire.PeekOutValue()
        assert v2 == 75738265

        w = self._r.pokewire.Connect()
        for i in xrange(3):
            w.OutValue = 8638356

        time.sleep(.1)
        (v3, ts3) = self._r.pokewire.PeekOutValue()
        assert v3 == 8638356

    def AsyncTestWirePeekPoke(self):
        async_wait = threading.Event()
        async_err = [None]

        def TestAsync_err(err):
            async_err[0] = err
            async_wait.set()
            print(err)
            traceback.print_stack()

        def TestAsync1(val, ts, err):
            try:
                if (err is not None):
                    TestAsync_err(err)
                    return
                if val != 56295674:
                    TestAsync_err(Exception())
                    return

                self._r.pokewire.AsyncPokeOutValue(75738261, TestAsync2)
            except:
                traceback.print_exc()

        def TestAsync2(err):
            if (err is not None):
                TestAsync_err(err)
                return

            self._r.pokewire.AsyncPeekOutValue(TestAsync3)

        def TestAsync3(val, ts, err):
            if (err is not None):
                TestAsync_err(err)
                return
            if val != 75738261:
                TestAsync_err(Exception())
                return

            async_wait.set()

        self._r.peekwire.AsyncPeekInValue(TestAsync1)

        """if not async_wait.wait(1):
            raise Exception()

        if (async_err[0]):
            raise async_err[0]"""

    def TestEnums(self):

        c = RRN.GetConstants(
            "com.robotraconteur.testing.TestService3", self._r)

        assert self._r.testenum1_prop == c['testenum1']['anothervalue']
        self._r.testenum1_prop = c['testenum1']['hexval1']

    def TestPods(self):

        if sys.platform == "darwin" and sys.version_info[0] < 3:
            return

        s1 = ServiceTest2_fill_testpod1(RRN, 563921043, self._r)
        ServiceTest2_verify_testpod1(s1[0], 563921043)

        s1_m = RobotRaconteurPythonUtil.PackMessageElement(
            s1, 'com.robotraconteur.testing.TestService3.testpod1', self._r)
        s1_m.UpdateData()
        s1_1 = RobotRaconteurPythonUtil.UnpackMessageElement(
            s1_m, 'com.robotraconteur.testing.TestService3.testpod1', self._r)
        ServiceTest2_verify_testpod1(s1_1[0], 563921043)

        s3 = ServiceTest2_fill_teststruct3(RRN, 858362, self._r)
        # ServiceTest2_verify_teststruct3(s3,858362)
        s3_m = RobotRaconteurPythonUtil.PackMessageElement(
            s3, 'com.robotraconteur.testing.TestService3.teststruct3', self._r)
        s3_m.UpdateData()
        s3_1 = RobotRaconteurPythonUtil.UnpackMessageElement(
            s3_m, 'com.robotraconteur.testing.TestService3.teststruct3', self._r)
        ServiceTest2_verify_teststruct3(s3_1, 858362)

        p1 = self._r.testpod1_prop
        ServiceTest2_verify_testpod1(p1[0], 563921043)

        p2 = ServiceTest2_fill_testpod1(RRN, 85932659, self._r)
        self._r.testpod1_prop = p2

        f1 = self._r.testpod1_func2()
        ServiceTest2_verify_testpod1(f1[0], 95836295)

        f2 = ServiceTest2_fill_testpod1(RRN, 29546592, self._r)
        self._r.testpod1_func1(f2)

        # ServiceTest2_verify_teststruct3(self._r.teststruct3_prop, 16483675);
        self._r.teststruct3_prop = (
            ServiceTest2_fill_teststruct3(RRN, 858362, self._r))

    def TestGenerators(self):
        assert cmp(self._r.gen_func1().NextAll(), list(xrange(16))) == 0
        assert cmp(list(self._r.gen_func1()), list(xrange(16))) == 0

        g = self._r.gen_func4()
        for _ in xrange(3):
            g.Next([])
        b = g.Next([2, 3, 4])
        print(list(b))
        g.Abort()
        try:
            g.Next([])
        except RR.OperationAbortedException:
            pass

        g2 = self._r.gen_func4()
        g2.Next([2, 3, 4])
        g2.Close()
        try:
            g2.Next([])
        except RR.StopIterationException:
            pass
        assert cmp(list(self._r.gen_func2("gen_func2_a_param").NextAll()), [
                   bytearray([i]) for i in xrange(16)]) == 0

        g3 = self._r.gen_func4()
        for _ in xrange(3):
            assert g3.TryNext([])[0]
        res, b = g3.TryNext([2, 3, 4])
        assert res
        print(list(b))
        g3.Abort()
        try:
            g3.TryNext([])
        except RR.OperationAbortedException:
            pass

        g4 = self._r.gen_func4()
        assert g4.TryNext([2, 3, 4])[0]
        g4.Close()
        assert not g4.TryNext([])[0]

    def TestMemories(self):

        if sys.platform == "darwin" and sys.version_info[0] < 3:
            return

        self.test_m1()
        self.test_m2()

    def test_m1(self):
        pod_dtype = RRN.GetPodDType(
            'com.robotraconteur.testing.TestService3.testpod2', self._r)
        o1 = np.zeros((32,), dtype=pod_dtype)
        for i in xrange(32):
            o1[i] = ServiceTest2_fill_testpod2(RRN, 59174 + i, self._r)
        assert self._r.pod_m1.Length == 1024
        self._r.pod_m1.Write(52, o1, 3, 17)
        o2 = np.zeros((32,), dtype=pod_dtype)
        self._r.pod_m1.Read(53, o2, 2, 16)

        for i in xrange(2, 16):
            ServiceTest2_verify_testpod2(o2[i], 59174 + i + 2)

    def test_m2(self):
        pod_dtype = RRN.GetPodDType(
            'com.robotraconteur.testing.TestService3.testpod2', self._r)
        s1_1 = np.zeros((9,), dtype=pod_dtype)
        for i in xrange(9):
            s1_1[i] = ServiceTest2_fill_testpod2(RRN, 75721 + i, self._r)

        s = s1_1.reshape((3, 3), order="F")
        self._r.pod_m2.Write([0, 0], s, [0, 0], [3, 3])

        s2 = np.zeros((3, 3), dtype=pod_dtype)
        self._r.pod_m2.Read([0, 0], s2, [0, 0], [3, 3])

        s2_1 = s2.reshape((9,), order="F")

        for i in xrange(9):
            ServiceTest2_verify_testpod2(s2_1[i], 75721 + i)

    def TestNamedArrays(self):

        self._r.testnamedarray1 = ServiceTest2_fill_transform(RRN, 3956378, self._r)[
            'translation']
        a1_2 = ServiceTest2_fill_transform(RRN, 74637, self._r)
        a1_2['translation'] = self._r.testnamedarray1
        ServiceTest2_verify_transform(a1_2, 74637)

        self._r.testnamedarray2 = ServiceTest2_fill_transform(
            RRN, 827635, self._r)
        ServiceTest2_verify_transform((self._r.testnamedarray2)[0], 1294)

        self._r.testnamedarray3 = ServiceTest2_create_transform_array(
            RRN, 6, 19274, self._r)
        ServiceTest2_verify_transform_array(
            (self._r.testnamedarray3), 8, 837512)

        self._r.testnamedarray4 = (
            ServiceTest2_create_transform_multidimarray(RRN, 5, 2, 6385, self._r))
        ServiceTest2_verify_transform_multidimarray(
            self._r.testnamedarray4, 7, 2, 66134)

        self._r.testnamedarray5 = (
            ServiceTest2_create_transform_multidimarray(RRN, 3, 2, 7732, self._r))
        ServiceTest2_verify_transform_multidimarray(
            self._r.testnamedarray5, 3, 2, 773142)

        a1 = ServiceTest2_create_transform_array(RRN, 6, 174, self._r)
        a2 = RRN.NamedArrayToArray(a1)
        a3 = RRN.ArrayToNamedArray(a2, a1.dtype)
        nptest.assert_equal(a1, a3)

    def TestNamedArrayMemories(self):
        self.test_named_array_m1()
        self.test_named_array_m2()

    def test_named_array_m1(self):
        n_dtype = RRN.GetNamedArrayDType(
            'com.robotraconteur.testing.TestService3.transform', self._r)

        s = np.zeros((32,), dtype=n_dtype)
        for i in xrange(32):
            s[i] = ServiceTest2_fill_transform(RRN, 79174 + i, self._r)
        assert self._r.namedarray_m1.Length == 512
        self._r.namedarray_m1.Write(23, s, 3, 21)

        s2 = np.zeros((32,), dtype=n_dtype)
        self._r.namedarray_m1.Read(24, s2, 2, 18)

        for i in xrange(2, 16):
            ServiceTest2_verify_transform(s2[i], 79174 + i + 2)

    def test_named_array_m2(self):
        n_dtype = RRN.GetNamedArrayDType(
            'com.robotraconteur.testing.TestService3.transform', self._r)

        s = np.zeros((9,), dtype=n_dtype)
        for i in xrange(9):
            s[i] = ServiceTest2_fill_transform(RRN, 15721 + i, self._r)
        s = s.reshape((3, 3), order="F")
        self._r.namedarray_m2.Write([0, 0], s, [0, 0], [3, 3])

        s2 = np.zeros((3, 3), dtype=n_dtype)
        self._r.namedarray_m2.Read([0, 0], s2, [0, 0], [3, 3])
        s2_1 = s2.reshape((9,), order="F")
        for i in xrange(9):
            ServiceTest2_verify_transform(s2_1[i], 15721 + i)

    def TestComplex(self):
        assert self._r.c1 == complex(5.708705e+01, -2.328294e-03)
        self._r.c1 = complex(5.708705e+01, -2.328294e-03)

        c2_1_1 = np.array(get_test_vector("ts3.c2.to_client", "double")).view(np.complex128)
        nptest.assert_allclose(self._r.c2, c2_1_1)
        c2_2_1 = np.array(get_test_vector("ts3.c2.to_service", "double")).view(np.complex128)
        self._r.c2 = c2_2_1

        c3_1_2 = np.array(get_test_vector("ts3.c3.to_client", "double")).view(
            np.complex128).reshape(get_test_vector("ts3.c3.to_client.dims", "uint32"), order="F")
        nptest.assert_allclose(c3_1_2, self._r.c3)
        c3_2_2 = np.array(get_test_vector("ts3.c3.to_service", "double")).view(
            np.complex128).reshape(get_test_vector("ts3.c3.to_service.dims", "uint32"), order="F")
        self._r.c3 = c3_2_2

        c5_1_1 = np.array(get_test_vector("ts3.c5.to_client", "double")).view(np.complex128)
        nptest.assert_allclose(c5_1_1, (self._r.c5)[0])
        c5_2_1 = np.array(get_test_vector("ts3.c5.to_service", "double")).view(np.complex128)
        self._r.c5 = [c5_2_1]

        nptest.assert_allclose(
            self._r.c7, complex(-5.527021e-18, -9.848457e+03))
        self._r.c7 = complex(9.303345e-12, -3.865684e-05)

        c8_1_1 = np.array(get_test_vector("ts3.c8.to_client", "single"), np.float32).view(np.complex64)
        nptest.assert_allclose(c8_1_1, self._r.c8)
        c8_2_1 = np.array(get_test_vector("ts3.c8.to_service", "single"), np.float32).view(np.complex64)
        self._r.c8 = c8_2_1

        c9_1_1 = np.array(get_test_vector("ts3.c9.to_client", "single"), np.float32).view(
            np.complex64).reshape(get_test_vector("ts3.c9.to_client.dims", "uint32"), order="F")
        nptest.assert_allclose(c9_1_1, self._r.c9)
        c9_2_1 = np.array(get_test_vector("ts3.c9.to_service", "single"), np.float32).view(
            np.complex64).reshape(get_test_vector("ts3.c9.to_service.dims", "uint32"), order="F")
        self._r.c9 = c9_2_1

    def TestComplexMemories(self):
        c_m1_1 = np.array(get_test_vector("ts3.client.c_m1_1", "double")).view(np.complex128)
        self._r.c_m1.Write(10, c_m1_1, 0, 8)
        c_m1_3 = np.zeros((8,), np.complex128)
        self._r.c_m1.Read(10, c_m1_3, 0, 8)
        nptest.assert_allclose(c_m1_1[0:8], c_m1_3)

        c_m2_3 = np.array(get_test_vector("ts3.client.c_m2_2", "double")).view(
            np.complex128).reshape(get_test_vector("ts3.client.c_m2_2.dims", "uint32"), order="F")
        self._r.c_m2.Write([0, 0], c_m2_3, [0, 0], [3, 3])
        c_m2_4 = np.zeros((3, 3), np.complex128)
        self._r.c_m2.Read([0, 0], c_m2_4, [0, 0], [3, 3])
        nptest.assert_allclose(c_m2_3, c_m2_4)

    def TestNoLock(self):
        o5 = self._r.get_nolock_test()

        with pytest.raises(RR.ObjectLockedException):
            a = o5.p1

        a = o5.p2
        o5.p2 = 0
        a = o5.p3
        with pytest.raises(RR.ObjectLockedException):
            o5.p3 = 0

        with pytest.raises(RR.ObjectLockedException):
            a = o5.f1()

        o5.f2()

        with pytest.raises(RR.ObjectLockedException):
            o5.q1.Connect(-1).Close()
        o5.q2.Connect(-1).Close()

        with pytest.raises(RR.ObjectLockedException):
            o5.w1.Connect().Close()

        o5.w2.Connect().Close()

        with pytest.raises(RR.ObjectLockedException):
            o5.m1.Length

        b1 = np.zeros((100,), np.int32)

        a = o5.m2.Length
        o5.m2.Read(0, b1, 0, 10)
        o5.m2.Write(0, b1, 0, 10)

        a = o5.m3.Length
        o5.m3.Read(0, b1, 0, 10)
        errthrown = False
        with pytest.raises(RR.ObjectLockedException):
            o5.m3.Write(0, b1, 0, 10)

    def TestBools(self):

        self._r.b1 = True
        assert self._r.b1

        self._r.b2 = [True, False, False, True, True, True, False, True]
        ca(self._r.b2, [True, False, True, True, False, True, False])

        self._r.b3 = np.array([True, False]).reshape(2, 1)
        ca(self._r.b3, np.array(
            [False, True, True, False]).reshape(2, 2, order='F'))

        self._r.b4 = [True]
        assert self._r.b4[0]

        self._r.b5 = [np.array([True, False])]
        ca(self._r.b5[0], np.array([False, True, False, False]))

        self._r.b6 = [np.array([True, False]).reshape(2, 1)]
        ca(self._r.b6[0], np.array(
            [False, True, True, False]).reshape(2, 2, order='F'))

    def TestBoolMemories(self):
        v1_1 = np.array([True, False, False, True, True,
                        False, False, False, True, True])
        self._r.c_m5.Write(100, v1_1, 1, 8)
        v1_2 = np.zeros((10,), np.bool_)
        self._r.c_m5.Read(99, v1_2, 0, 10)
        ca(v1_1[1:9], v1_2[1:9])

        v2_1 = v1_1.reshape(2, 5, order='F')
        self._r.c_m6.Write([0, 0], v2_1, [0, 0], [2, 5])
        v2_2 = np.zeros((2, 5), np.bool_)
        self._r.c_m6.Read([0, 0], v2_2, [0, 0], [2, 5])
        ca(v2_1, v2_2)

    def TestExceptionParams(self):

        exp1_caught = False
        try:
            self._r.test_exception_params1()
        except Exception as e:
            exp1_caught = True
            assert e.errorname == "RobotRaconteur.InvalidOperation"
            assert e.message == "test error"
            assert e.errorsubname == "my_error"
            assert len(e.errorparam.data) == 2
            assert e.errorparam.data["param1"].data[0] == 10
            assert e.errorparam.data["param2"].data == "20"

        assert exp1_caught == True

        exp2_caught = False
        try:
            self._r.test_exception_params2()
        except Exception as e:
            exp2_caught = True
            assert e.errorname == "com.robotraconteur.testing.TestService3.test_exception4"
            assert e.message == "test error2"
            assert e.errorsubname == "my_error2"
            assert len(e.errorparam.data) == 2
            assert e.errorparam.data["param1"].data[0] == 30
            assert e.errorparam.data["param2"].data == "40"

        assert exp2_caught == True
