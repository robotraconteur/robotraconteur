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

        #ServiceTest2_verify_teststruct3(self._r.teststruct3_prop, 16483675);
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

        c2_1_1 = np.array([1.968551e+07, 2.380643e+18, 3.107374e-16, 7.249542e-16, -4.701135e-19, -6.092764e-17, 2.285854e+14, 2.776180e+05, -1.436152e-12, 3.626609e+11, 3.600952e-02, -3.118123e-16, -1.312210e-10, -1.738940e-07, -1.476586e-12, -2.899781e-20,
                          4.806642e+03, 4.476869e-05, -2.935084e-16, 3.114019e-20, -3.675955e+01, 3.779796e-21, 2.190594e-11, 4.251420e-06, -9.715221e+11, -3.483924e-01, 7.606428e+05, 5.418088e+15, 4.786378e+16, -1.202581e+08, -1.662061e+02, -2.392954e+03]).view(np.complex128)
        nptest.assert_allclose(self._r.c2, c2_1_1)
        c2_2_1 = np.array([4.925965e-03, 5.695254e+13, -4.576890e-14, -6.056342e-07, -4.918571e-08, -1.940684e-10, 1.549104e-02, -1.954145e+04, -2.499019e-16, 4.010614e+09, -1.906811e-08, 3.297924e-10, 2.742399e-02, -4.372839e-01, -3.093171e-10, 4.311755e-01, -
                          2.218220e-14, 5.399758e+10, 3.360304e+17, 1.340681e-18, -4.441140e+11, -1.845055e-09, -3.074586e-10, -1.754926e+01, -2.766799e+04, -2.307577e+10, 2.754875e+14, 1.179639e+15, 6.976204e-10, 1.901856e+08, -3.824351e-02, -1.414167e+08]).view(np.complex128)
        self._r.c2 = c2_2_1

        c3_1_2 = np.array([5.524802e+18, -2.443857e-05, 3.737932e-02, -4.883553e-03, -1.184347e+12, 4.537366e-08, -4.567913e-01, -1.683542e+15, -1.676517e+00, -8.911085e+12, -2.537376e-17,
                          1.835687e-10, -9.366069e-22, -5.426323e-12, -7.820969e-10, -1.061541e+12, -3.660854e-12, -4.969930e-03, 1.988428e+07, 1.860782e-16]).view(np.complex128).reshape((2, 5), order="F")
        nptest.assert_allclose(c3_1_2, self._r.c3)
        c3_2_2 = np.array([4.435180e+04, 5.198060e-18, -1.316737e-13, -4.821771e-03, -4.077550e-19, -1.659105e-09, -6.332363e-11, -1.128999e+16, 4.869912e+16, 2.680490e-04, -8.880119e-04, 3.960452e+11, 4.427784e-09, -
                          2.813742e-18, 7.397516e+18, 1.196394e+13, 3.236906e-14, -4.219297e-17, 1.316282e-06, -2.771084e-18, -1.239118e-09, 2.887453e-08, -1.746515e+08, -2.312264e-11]).view(np.complex128).reshape((3, 4), order="F")
        self._r.c3 = c3_2_2

        c5_1_1 = np.array([1.104801e+00, 4.871266e-10, -2.392938e-03, 4.210339e-07, 1.474114e-19, -1.147137e-01, -2.026434e+06, 4.450447e-19,
                          3.702953e-21, 9.722025e+12, 3.464073e-14, 4.628110e+15, 2.345453e-19, 3.730012e-04, 4.116650e+16, 4.380220e+08]).view(np.complex128)
        nptest.assert_allclose(c5_1_1, (self._r.c5)[0])
        c5_2_1 = np.array([2.720831e-20, 2.853037e-16, -7.982497e+16, -2.684318e-09, -2.505796e+17, -4.743970e-12, -3.657056e+11, 2.718388e+15,
                          1.597672e+03, 2.611859e+14, 2.224926e+06, -1.431096e-09, 3.699894e+19, -5.936706e-01, -1.385395e-09, -4.248415e-13]).view(np.complex128)
        self._r.c5 = [c5_2_1]

        nptest.assert_allclose(
            self._r.c7, complex(-5.527021e-18, -9.848457e+03))
        self._r.c7 = complex(9.303345e-12, -3.865684e-05)

        c8_1_1 = np.array([-3.153395e-09, 3.829492e-02, -2.665239e+12, 1.592927e-03, 3.188444e+06, -3.595015e-11, 2.973887e-18, -2.189921e+17, 1.651567e+10, 1.095838e+05, 3.865249e-02, 4.725510e+10, -2.334376e+03, 3.744977e-05, -1.050821e+02, 1.122660e-22, 3.501520e-18, -
                          2.991601e-17, 6.039622e-17, 4.778095e-07, -4.793136e-05, 3.096513e+19, 2.476004e+18, 1.296297e-03, 2.165336e-13, 4.834427e+06, 4.675370e-01, -2.942290e-12, -2.090883e-19, 6.674942e+07, -4.809047e-10, -4.911772e-13], np.float32).view(np.complex64)
        nptest.assert_allclose(c8_1_1, self._r.c8)
        c8_2_1 = np.array([1.324498e+06, 1.341746e-04, 4.292993e-04, -3.844509e+15, -3.804802e+10, 3.785305e-12, 2.628285e-19, -1.664089e+15, -4.246472e-10, -3.334943e+03, -3.305796e-01, 1.878648e-03, 1.420880e-05, -3.024657e+14, 2.227031e-21, 2.044653e+17, 9.753609e-20, -
                          6.581817e-03, 3.271063e-03, -1.726081e+06, -1.614502e-06, -2.641638e-19, -2.977317e+07, -1.278224e+03, -1.760207e-05, -4.877944e-07, -2.171524e+02, 1.620645e+01, -4.334168e-02, 1.871011e-09, -3.066163e+06, -3.533662e+07], np.float32).view(np.complex64)
        self._r.c8 = c8_2_1

        c9_1_1 = np.array([1.397743e+15, 3.933042e+10, -3.812329e+07, 1.508109e+16, -2.091397e-20, 3.207851e+12, -3.640702e+02, 3.903769e+02, -2.879727e+17, -
                          4.589604e-06, 2.202769e-06, 2.892523e+04, -3.306489e-14, 4.522308e-06, 1.665807e+15, 2.340476e+10], np.float32).view(np.complex64).reshape((2, 4), order="F")
        nptest.assert_allclose(c9_1_1, self._r.c9)
        c9_2_1 = np.array([2.138322e-03, 4.036979e-21, 1.345236e+10, -1.348460e-12, -3.615340e+12, -2.911340e-21, 3.220362e+09, 3.459909e-04, 4.276259e-08, -3.199451e+18,
                          3.468308e+07, -2.928506e-09, -3.154288e+17, -2.352920e-02, 6.976385e-21, 2.435472e+12], np.float32).view(np.complex64).reshape((2, 2, 2), order="F")
        self._r.c9 = c9_2_1

    def TestComplexMemories(self):
        c_m1_1 = np.array([8.952764e-05, 4.348213e-04, -1.051215e+08, 1.458626e-09, -2.575954e+10, 2.118740e+03, -2.555026e-02, 2.192576e-18, -
                          2.035082e+18, 2.951834e-09, -1.760731e+15, 4.620903e-11, -3.098798e+05, -8.883556e-07, 2.472289e+17, 7.059075e-12]).view(np.complex128)
        self._r.c_m1.Write(10, c_m1_1, 0, 8)
        c_m1_3 = np.zeros((8,), np.complex128)
        self._r.c_m1.Read(10, c_m1_3, 0, 8)
        nptest.assert_allclose(c_m1_1[0:8], c_m1_3)

        c_m2_3 = np.array([-4.850043e-03, 3.545429e-07, 2.169430e+12, 1.175943e-09, 2.622300e+08, -4.439823e-11, -1.520489e+17, 8.250078e-14, 3.835439e-07, -1.424709e-02,
                          3.703099e+08, -1.971111e-08, -2.805354e+01, -2.093850e-17, -4.476148e+19, 9.914350e+11, 2.753067e+08, -1.745041e+14]).view(np.complex128).reshape((3, 3), order="F")
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
