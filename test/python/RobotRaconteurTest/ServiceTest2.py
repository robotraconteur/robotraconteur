from __future__ import absolute_import

import copy
import RobotRaconteur as RR
import numpy as np
import numpy.testing as nptest

from .testvectors import get_test_vector

try:
    xrange  # type: ignore
except NameError:
    xrange = range

ca = nptest.assert_allclose


class testroot3_impl(object):
    def __init__(self):
        self._peekwire = None
        self._pokewire = None
        self._peekwire_b = None
        self._pokewire_r = None

        self._timer = None

        self._o5 = obj5_impl()

        self.c_m1 = RR.ArrayMemory(np.zeros((100,), np.complex128))
        self.c_m2 = RR.MultiDimArrayMemory(np.zeros((3, 3), np.complex128))

        self.c_m5 = RR.ArrayMemory(np.zeros((512,), np.bool_))
        self.c_m6 = RR.MultiDimArrayMemory(np.zeros((10, 10), np.bool_))

    def RRServiceObjectInit(self, ctx, service_path):
        self._node = ctx.GetNode()
        self._const = self._node.GetConstants(
            "com.robotraconteur.testing.TestService3")
        pod_dtype = self._node.GetPodDType(
            'com.robotraconteur.testing.TestService3.testpod2')
        self._pod_m1 = RR.ArrayMemory(np.zeros((1024,), dtype=pod_dtype))
        self._pod_m2 = RR.MultiDimArrayMemory(
            np.zeros((3, 3), dtype=pod_dtype))
        transform_dtype = self._node.GetNamedArrayDType(
            'com.robotraconteur.testing.TestService3.transform')
        self._namedarray_m1 = RR.ArrayMemory(
            np.zeros((512,), dtype=transform_dtype))
        self._namedarray_m2 = RR.MultiDimArrayMemory(
            np.zeros((10, 10), dtype=transform_dtype))
        self._timer = self._node.CreateTimer(.1, self._timer_handler)
        self._timer.Start()

    @property
    def peekwire(self):
        return self._peekwire

    @peekwire.setter
    def peekwire(self, v):
        self._peekwire = v
        self._peekwire_b = RR.WireBroadcaster(v)
        self._peekwire_b.OutValue = 56295674

    def _timer_handler(self, evt):
        self._peekwire_b.OutValue = 56295674

    @property
    def pokewire(self):
        return self._pokewire

    @pokewire.setter
    def pokewire(self, v):
        self._pokewire = v
        self._pokewire_r = RR.WireUnicastReceiver(v)

        def in_value_changed(val, ts, ep):
            print("In value changed: " + str(val) + " ep: " + str(ep))
            # print(self._pokewire_r.InValue)
            # print(self._pokewire_r.TryGetInValue())

        self._pokewire_r.InValueChanged += in_value_changed

    @property
    def testenum1_prop(self):
        return self._const['testenum1']['anothervalue']

    @testenum1_prop.setter
    def testenum1_prop(self, value):
        assert value == self._const['testenum1']['hexval1']

    @property
    def testpod1_prop(self):
        return ServiceTest2_fill_testpod1(self._node, 563921043, None)

    @testpod1_prop.setter
    def testpod1_prop(self, value):
        ServiceTest2_verify_testpod1(value[0], 85932659)

    def testpod1_func1(self, s):
        ServiceTest2_verify_testpod1(s[0], 29546592)

    def testpod1_func2(self):
        return ServiceTest2_fill_testpod1(self._node, 95836295, None)

    @property
    def teststruct3_prop(self):
        return ServiceTest2_fill_teststruct3(16483675, None)

    @teststruct3_prop.setter
    def teststruct3_prop(self, value):
        ServiceTest2_verify_teststruct3(value, 858362)

    def gen_func1(self):
        return xrange(16)

    def gen_func2(self, name):
        assert name == "gen_func2_a_param"
        return [[i] for i in xrange(16)]

    def gen_func4(self):
        return func4_gen()

    def get_o4(self):
        return obj4_impl(), "com.robotraconteur.testing.TestService3.obj4"

    @property
    def pod_m1(self):
        return self._pod_m1

    @property
    def pod_m2(self):
        return self._pod_m2

    @property
    def testnamedarray1(self):
        return ServiceTest2_fill_transform(self._node, 74637, None)['translation']

    @testnamedarray1.setter
    def testnamedarray1(self, value):
        a1 = ServiceTest2_fill_transform(self._node, 3956378, None)
        a1['translation'] = value
        ServiceTest2_verify_transform(a1, 3956378)

    @property
    def testnamedarray2(self):
        return ServiceTest2_fill_transform(self._node, 1294, None)

    @testnamedarray2.setter
    def testnamedarray2(self, value):
        ServiceTest2_verify_transform(value, 827635)

    @property
    def testnamedarray3(self):
        return ServiceTest2_create_transform_array(self._node, 8, 837512, None)

    @testnamedarray3.setter
    def testnamedarray3(self, value):
        ServiceTest2_verify_transform_array(value, 6, 19274)

    @property
    def testnamedarray4(self):
        return ServiceTest2_create_transform_multidimarray(self._node, 7, 2, 66134, None)

    @testnamedarray4.setter
    def testnamedarray4(self, value):
        ServiceTest2_verify_transform_multidimarray(value, 5, 2, 6385)

    @property
    def testnamedarray5(self):
        return ServiceTest2_create_transform_multidimarray(self._node, 3, 2, 773142, None)

    @testnamedarray5.setter
    def testnamedarray5(self, value):
        ServiceTest2_verify_transform_multidimarray(value, 3, 2, 7732)

    @property
    def namedarray_m1(self):
        return self._namedarray_m1

    @property
    def namedarray_m2(self):
        return self._namedarray_m2

    @property
    def c1(self):
        return complex(5.708705e+01, -2.328294e-03)

    @c1.setter
    def c1(self, value):
        nptest.assert_allclose(complex(5.708705e+01, -2.328294e-03), value)

    @property
    def c2(self):
        return np.array([1.968551e+07, 2.380643e+18, 3.107374e-16, 7.249542e-16, -4.701135e-19, -6.092764e-17, 2.285854e+14, 2.776180e+05, -1.436152e-12, 3.626609e+11, 3.600952e-02, -3.118123e-16, -1.312210e-10, -1.738940e-07, -1.476586e-12, -2.899781e-20, 4.806642e+03, 4.476869e-05, -2.935084e-16, 3.114019e-20, -3.675955e+01, 3.779796e-21, 2.190594e-11, 4.251420e-06, -9.715221e+11, -3.483924e-01, 7.606428e+05, 5.418088e+15, 4.786378e+16, -1.202581e+08, -1.662061e+02, -2.392954e+03]).view(np.complex128)

    @c2.setter
    def c2(self, value):
        nptest.assert_allclose(np.array([4.925965e-03, 5.695254e+13, -4.576890e-14, -6.056342e-07, -4.918571e-08, -1.940684e-10, 1.549104e-02, -1.954145e+04, -2.499019e-16, 4.010614e+09, -1.906811e-08, 3.297924e-10, 2.742399e-02, -4.372839e-01, -3.093171e-10, 4.311755e-01, -
                               2.218220e-14, 5.399758e+10, 3.360304e+17, 1.340681e-18, -4.441140e+11, -1.845055e-09, -3.074586e-10, -1.754926e+01, -2.766799e+04, -2.307577e+10, 2.754875e+14, 1.179639e+15, 6.976204e-10, 1.901856e+08, -3.824351e-02, -1.414167e+08]).view(np.complex128), value)

    @property
    def c3(self):
        return np.array([5.524802e+18, -2.443857e-05, 3.737932e-02, -4.883553e-03, -1.184347e+12, 4.537366e-08, -4.567913e-01, -1.683542e+15, -1.676517e+00, -8.911085e+12, -2.537376e-17, 1.835687e-10, -9.366069e-22, -5.426323e-12, -7.820969e-10, -1.061541e+12, -3.660854e-12, -4.969930e-03, 1.988428e+07, 1.860782e-16]).view(np.complex128).reshape((2, 5), order="F")

    @c3.setter
    def c3(self, value):
        nptest.assert_allclose(np.array([4.435180e+04, 5.198060e-18, -1.316737e-13, -4.821771e-03, -4.077550e-19, -1.659105e-09, -6.332363e-11, -1.128999e+16, 4.869912e+16, 2.680490e-04, -8.880119e-04, 3.960452e+11, 4.427784e-09, -
                               2.813742e-18, 7.397516e+18, 1.196394e+13, 3.236906e-14, -4.219297e-17, 1.316282e-06, -2.771084e-18, -1.239118e-09, 2.887453e-08, -1.746515e+08, -2.312264e-11]).view(np.complex128).reshape((3, 4), order="F"), value)

    @property
    def c5(self):
        return [np.array([1.104801e+00, 4.871266e-10, -2.392938e-03, 4.210339e-07, 1.474114e-19, -1.147137e-01, -2.026434e+06, 4.450447e-19, 3.702953e-21, 9.722025e+12, 3.464073e-14, 4.628110e+15, 2.345453e-19, 3.730012e-04, 4.116650e+16, 4.380220e+08]).view(np.complex128)]

    @c5.setter
    def c5(self, value):
        nptest.assert_allclose(np.array([2.720831e-20, 2.853037e-16, -7.982497e+16, -2.684318e-09, -2.505796e+17, -4.743970e-12, -3.657056e+11, 2.718388e+15,
                               1.597672e+03, 2.611859e+14, 2.224926e+06, -1.431096e-09, 3.699894e+19, -5.936706e-01, -1.385395e-09, -4.248415e-13]).view(np.complex128), value[0])

    @property
    def c7(self):
        return complex(-5.527021e-18, -9.848457e+03)

    @c7.setter
    def c7(self, value):
        nptest.assert_allclose(complex(9.303345e-12, -3.865684e-05), value)

    @property
    def c8(self):
        return np.array([-3.153395e-09, 3.829492e-02, -2.665239e+12, 1.592927e-03, 3.188444e+06, -3.595015e-11, 2.973887e-18, -2.189921e+17, 1.651567e+10, 1.095838e+05, 3.865249e-02, 4.725510e+10, -2.334376e+03, 3.744977e-05, -1.050821e+02, 1.122660e-22, 3.501520e-18, -2.991601e-17, 6.039622e-17, 4.778095e-07, -4.793136e-05, 3.096513e+19, 2.476004e+18, 1.296297e-03, 2.165336e-13, 4.834427e+06, 4.675370e-01, -2.942290e-12, -2.090883e-19, 6.674942e+07, -4.809047e-10, -4.911772e-13], np.float32).view(np.complex64)

    @c8.setter
    def c8(self, value):
        nptest.assert_allclose(np.array([1.324498e+06, 1.341746e-04, 4.292993e-04, -3.844509e+15, -3.804802e+10, 3.785305e-12, 2.628285e-19, -1.664089e+15, -4.246472e-10, -3.334943e+03, -3.305796e-01, 1.878648e-03, 1.420880e-05, -3.024657e+14, 2.227031e-21, 2.044653e+17, 9.753609e-20, -
                               6.581817e-03, 3.271063e-03, -1.726081e+06, -1.614502e-06, -2.641638e-19, -2.977317e+07, -1.278224e+03, -1.760207e-05, -4.877944e-07, -2.171524e+02, 1.620645e+01, -4.334168e-02, 1.871011e-09, -3.066163e+06, -3.533662e+07], np.float32).view(np.complex64), value)

    @property
    def c9(self):
        return np.array([1.397743e+15, 3.933042e+10, -3.812329e+07, 1.508109e+16, -2.091397e-20, 3.207851e+12, -3.640702e+02, 3.903769e+02, -2.879727e+17, -4.589604e-06, 2.202769e-06, 2.892523e+04, -3.306489e-14, 4.522308e-06, 1.665807e+15, 2.340476e+10], np.float32).view(np.complex64).reshape((2, 4), order="F")

    @c9.setter
    def c9(self, value):
        nptest.assert_allclose(np.array([2.138322e-03, 4.036979e-21, 1.345236e+10, -1.348460e-12, -3.615340e+12, -2.911340e-21, 3.220362e+09, 3.459909e-04, 4.276259e-08, -3.199451e+18,
                               3.468308e+07, -2.928506e-09, -3.154288e+17, -2.352920e-02, 6.976385e-21, 2.435472e+12], np.float32).view(np.complex64).reshape((2, 2, 2), order="F"), value)

    def get_nolock_test(self):
        return self._o5, "com.robotraconteur.testing.TestService3.obj5"

    @property
    def b1(self):
        return True

    @b1.setter
    def b1(self, value):
        assert value == True

    @property
    def b2(self):
        return np.array([True, False, True, True, False, True, False])

    @b2.setter
    def b2(self, value):
        ca(value, [True, False, False, True, True, True, False, True])

    @property
    def b3(self):
        return np.array([False, True, True, False]).reshape(2, 2, order='F')

    @b3.setter
    def b3(self, value):
        ca(value, np.array([True, False]).reshape(2, 1))

    @property
    def b4(self):
        return [True]

    @b4.setter
    def b4(self, value):
        assert value[0] == True

    @property
    def b5(self):
        return [np.array([False, True, False, False])]

    @b5.setter
    def b5(self, value):
        ca(value[0], np.array([True, False]))

    @property
    def b6(self):
        return [np.array([False, True, True, False]).reshape(2, 2, order='F')]

    @b6.setter
    def b6(self, value):
        ca(value[0], np.array([True, False]).reshape(2, 1))

    def test_exception_params1(self):
        params_dict = {}
        params_dict["param1"] = RR.VarValue(10, "int32")
        params_dict["param2"] = RR.VarValue("20", "string")
        params_ = RR.VarValue(params_dict, "varvalue{string}")
        err = RR.InvalidOperationException("test error", "my_error", params_)
        raise err

    def test_exception_params2(self):
        params_dict = {}
        params_dict["param1"] = RR.VarValue(30, "int32")
        params_dict["param2"] = RR.VarValue("40", "string")
        params_ = RR.VarValue(params_dict, "varvalue{string}")
        e4 = self._node.GetExceptionType(
            "com.robotraconteur.testing.TestService3.test_exception4")
        err = e4("test error2", "my_error2", params_)
        raise err


class func4_gen(object):
    def __init__(self):
        self._j = 0
        self._aborted = False

    def Next(self, v):
        if self._aborted:
            raise RR.OperationAbortedException()

        if (self._j >= 8):
            raise RR.StopIterationException()

        a = copy.copy(v)
        for i in xrange(len(a)):
            a[i] += self._j

        self._j += 1

        return a

    def Abort(self):
        self._aborted = True

    def Close(self):
        self._j = 1000


class obj4_impl(object):
    def __init__(self):
        self.s_ind = ""
        self.i_ind = 0
        self.data = ""


class obj5_impl(object):
    def __init__(self):
        self.m1 = RR.ArrayMemory(np.zeros((100,), np.int32))
        self.m2 = RR.ArrayMemory(np.zeros((100,), np.int32))
        self.m3 = RR.ArrayMemory(np.zeros((100,), np.int32))
        self.p1 = 0
        self.p2 = 0
        self.p3 = 0
        self.q1 = None
        self.q2 = None
        self.w1 = None
        self.w2 = None

    def f1(self):
        return 10

    def f2(self):
        return 11


class ServiceTest2_test_sequence_gen(object):

    double_constants = get_test_vector("seq.double_constants", "double")
    uint32_constants = get_test_vector("seq.uint32_constants", "uint32")

    def __init__(self, seed):
        self.counter = seed % 7823
        self.step = (seed % 17) + 7

    def get_double(self):
        self.counter += self.step
        return self.double_constants[self.counter % len(self.double_constants)]

    def get_uint32(self):
        self.counter += self.step
        return self.uint32_constants[self.counter % len(self.uint32_constants)]

    def get_int8(self):
        self.counter += self.step
        return (self.counter % 0xFF) - 128


def ServiceTest2_create_double_array(gen, len_):
    return np.array([gen.get_double() for _ in xrange(len_)])


def ServiceTest2_verify_double_array(gen, a, len_):
    a2 = np.reshape(a, (a.size,), order="F")
    assert len(a2) == len_
    for d in a2:
        assert gen.get_double() == d


def ServiceTest2_create_uint32_array(gen, len_):
    return [gen.get_uint32() for _ in xrange(len_)]


def ServiceTest2_verify_uint32_array(gen, a, len_):
    assert len(a) == len_
    for d in a:
        assert gen.get_uint32() == d


def ServiceTest2_create_int8_array(gen, len_):
    return np.array([gen.get_int8() for _ in xrange(len_)], dtype=np.int8)


def ServiceTest2_verify_int8_array(gen, a, len_):
    assert len(a) == len_
    for d in a:
        assert gen.get_int8() == d


def ServiceTest2_fill_testpod1(node, seed, obj):
    gen = ServiceTest2_test_sequence_gen(seed)
    pod_dtype = node.GetPodDType(
        'com.robotraconteur.testing.TestService3.testpod1', obj)
    s = np.zeros((1,), dtype=pod_dtype)
    s['d1'] = gen.get_double()
    s['d2'] = ServiceTest2_create_double_array(gen, 6)
    d3 = ServiceTest2_create_double_array(gen, (gen.get_uint32() % 6))
    s['d3']['len'] = len(d3)
    s['d3']['array'][0][0:len(d3)] = d3
    s['d4'] = ServiceTest2_create_double_array(
        gen, 9).reshape((3, 3), order="F")

    s['s1'] = ServiceTest2_fill_testpod2(node, gen.get_uint32(), obj)
    s['s2'] = ServiceTest2_create_testpod2_array(node, gen, 8, obj)
    s3 = ServiceTest2_create_testpod2_array(
        node, gen, (gen.get_uint32() % 9), obj)
    s['s3']['len'] = len(s3)
    s['s3']['array'][0][0:len(s3)] = s3
    s['s4'] = ServiceTest2_create_testpod2_array(
        node, gen, 8, obj).reshape((2, 4), order="F")

    s['t1'] = ServiceTest2_fill_transform(node, gen.get_uint32(), obj)
    for i in xrange(4):
        s[0]['t2'][i] = ServiceTest2_fill_transform(
            node, gen.get_uint32(), obj)
    t3_len = gen.get_uint32() % 15
    s['t3']['len'] = t3_len
    for i in xrange(t3_len):
        s[0]['t3']['array'][i] = ServiceTest2_fill_transform(
            node, gen.get_uint32(), obj)
    n_dtype = node.GetNamedArrayDType(
        'com.robotraconteur.testing.TestService3.transform', obj)
    t4 = np.ndarray((8,), dtype=n_dtype)
    for i in xrange(8):
        t4[i] = ServiceTest2_fill_transform(node, gen.get_uint32(), obj)
    s['t4'] = t4.reshape((2, 4), order="F")

    return s


def ServiceTest2_verify_testpod1(s, seed):

    gen = ServiceTest2_test_sequence_gen(seed)
    assert s['d1'] == gen.get_double()
    ServiceTest2_verify_double_array(gen, s['d2'], 6)

    ServiceTest2_verify_double_array(
        gen, s['d3']['array'][0:s['d3']['len']], (gen.get_uint32() % 6))

    ServiceTest2_verify_double_array(gen, s['d4'], 9)
    ServiceTest2_verify_testpod2(s['s1'], gen.get_uint32())
    ServiceTest2_verify_testpod2_array(gen, s['s2'], 8)
    ServiceTest2_verify_testpod2_array(
        gen, s['s3']['array'][0:s['s3']['len']], (gen.get_uint32() % 9))
    ServiceTest2_verify_testpod2_array(
        gen, s['s4'].reshape((8,), order="F"), 8)

    ServiceTest2_verify_transform(s['t1'], gen.get_uint32())
    for i in xrange(4):
        ServiceTest2_verify_transform(s['t2'][i], gen.get_uint32())
    t3_len = gen.get_uint32() % 15
    assert s['t3']['len'] == t3_len
    for i in xrange(t3_len):
        ServiceTest2_verify_transform(s['t3']['array'][i], gen.get_uint32())

    t4 = s['t4'].flatten(order="F")
    for i in xrange(8):
        ServiceTest2_verify_transform(t4[i], gen.get_uint32())


def ServiceTest2_fill_testpod2(node, seed, obj):
    gen = ServiceTest2_test_sequence_gen(seed)
    pod_dtype = node.GetPodDType(
        'com.robotraconteur.testing.TestService3.testpod2', obj)
    s = np.zeros((1,), dtype=pod_dtype)
    s['i1'] = gen.get_int8()
    s['i2'] = ServiceTest2_create_int8_array(gen, 15)
    i3 = ServiceTest2_create_int8_array(gen, (gen.get_uint32() % 15))
    s['i3']['len'] = len(i3)
    s['i3']['array'][0][0:len(i3)] = i3
    return s


def ServiceTest2_verify_testpod2(s, seed):
    gen = ServiceTest2_test_sequence_gen(seed)
    assert (s['i1'] == gen.get_int8())
    ServiceTest2_verify_int8_array(gen, s['i2'], 15)
    ServiceTest2_verify_int8_array(
        gen, s['i3']['array'][0:s['i3']['len']], (gen.get_uint32() % 15))


def ServiceTest2_create_testpod1_array(node, len_, seed, obj):
    pod_dtype = node.GetPodDType(
        'com.robotraconteur.testing.TestService3.testpod1', obj)
    s = np.zeros((len_,), dtype=pod_dtype)
    gen = ServiceTest2_test_sequence_gen(seed)
    for i in xrange(len_):
        s[i] = ServiceTest2_fill_testpod1(node, gen.get_uint32(), obj)
    return s


def ServiceTest2_verify_testpod1_array(v, len_, seed):

    gen = ServiceTest2_test_sequence_gen(seed)
    assert (len(v) == len_)
    for i in xrange(len_):
        ServiceTest2_verify_testpod1(v[i], gen.get_uint32())


def ServiceTest2_create_testpod2_array(node, gen, len_, obj):
    pod_dtype = node.GetPodDType(
        'com.robotraconteur.testing.TestService3.testpod2', obj)
    s = np.zeros((len_,), dtype=pod_dtype)
    for i in xrange(len_):
        s[i] = ServiceTest2_fill_testpod2(node, gen.get_uint32(), obj)
    return s


def ServiceTest2_verify_testpod2_array(gen, v, len_):
    assert (len(v) == len_)
    for i in xrange(len_):
        ServiceTest2_verify_testpod2(v[i], gen.get_uint32())


def ServiceTest2_create_testpod1_multidimarray(node, m, n, seed, obj):
    gen = ServiceTest2_test_sequence_gen(seed)
    pod_dtype = node.GetPodDType(
        'com.robotraconteur.testing.TestService3.testpod1', obj)
    s = np.zeros((m * n,), dtype=pod_dtype)
    for i in xrange(m * n):
        s[i] = ServiceTest2_fill_testpod1(node, gen.get_uint32(), obj)

    return s.reshape((m, n), order="F")


def ServiceTest2_verify_testpod1_multidimarray(v, m, n, seed):
    assert v.shape == (m, n)
    v2 = v.reshape((m * n,), order="F")
    ServiceTest2_verify_testpod1_array(v2, m * n, seed)


def ServiceTest2_fill_teststruct3(node, seed, obj):
    o = node.NewStructure(
        'com.robotraconteur.testing.TestService3.teststruct3', obj)
    gen = ServiceTest2_test_sequence_gen(seed)
    o.s1 = ServiceTest2_fill_testpod1(node, gen.get_uint32(), obj)
    s2_seed = gen.get_uint32()
    o.s2 = ServiceTest2_create_testpod1_array(
        node, (s2_seed % 17), s2_seed, obj)
    o.s3 = ServiceTest2_create_testpod1_array(node, 11, gen.get_uint32(), obj)
    s4_seed = gen.get_uint32()
    o.s4 = ServiceTest2_create_testpod1_array(
        node, (s4_seed % 16), s4_seed, obj)
    o.s5 = ServiceTest2_create_testpod1_multidimarray(
        node, 3, 3, gen.get_uint32(), obj)
    s6_seed = gen.get_uint32()
    o.s6 = ServiceTest2_create_testpod1_multidimarray(
        node, (s6_seed % 6), (s6_seed % 3), s6_seed, obj)
    o.s7 = []
    o.s7.append(ServiceTest2_fill_testpod1(node, gen.get_uint32(), obj))

    o.s8 = []
    o.s8.append(ServiceTest2_create_testpod1_array(
        node, 2, gen.get_uint32(), obj))
    o.s8.append(ServiceTest2_create_testpod1_array(
        node, 4, gen.get_uint32(), obj))

    o.s9 = []
    o.s9.append(ServiceTest2_create_testpod1_multidimarray(
        node, 2, 3, gen.get_uint32(), obj))
    o.s9.append(ServiceTest2_create_testpod1_multidimarray(
        node, 4, 5, gen.get_uint32(), obj))

    s10 = ServiceTest2_fill_testpod1(node, gen.get_uint32(), obj)
    o.s10 = RR.VarValue(
        s10, 'com.robotraconteur.testing.TestService3.testpod1[]')

    o.s11 = RR.VarValue(ServiceTest2_create_testpod1_array(node, 3, gen.get_uint32(
    ), obj), 'com.robotraconteur.testing.TestService3.testpod1[]')
    o.s12 = RR.VarValue(ServiceTest2_create_testpod1_multidimarray(
        node, 2, 2, gen.get_uint32(), obj), 'com.robotraconteur.testing.TestService3.testpod1[*]')

    s13 = ServiceTest2_fill_testpod1(node, gen.get_uint32(), obj)
    o.s13 = RR.VarValue(
        [s13], 'com.robotraconteur.testing.TestService3.testpod1[]{list}')

    s14 = RR.VarValue(
        [], 'com.robotraconteur.testing.TestService3.testpod1[]{list}')
    s14.data.append(ServiceTest2_create_testpod1_array(
        node, 3, gen.get_uint32(), obj))
    s14.data.append(ServiceTest2_create_testpod1_array(
        node, 5, gen.get_uint32(), obj))
    o.s14 = s14

    s15 = RR.VarValue(
        [], 'com.robotraconteur.testing.TestService3.testpod1[*]{list}')
    s15.data.append(ServiceTest2_create_testpod1_multidimarray(
        node, 7, 2, gen.get_uint32(), obj))
    s15.data.append(ServiceTest2_create_testpod1_multidimarray(
        node, 5, 1, gen.get_uint32(), obj))
    o.s15 = s15

    o.t1 = ServiceTest2_fill_transform(node, gen.get_uint32(), obj)
    o.t2 = ServiceTest2_create_transform_array(node, 4, gen.get_uint32(), obj)
    o.t3 = ServiceTest2_create_transform_multidimarray(
        node, 2, 4, gen.get_uint32(), obj)

    o.t4 = RR.VarValue(ServiceTest2_create_transform_array(node, 10, gen.get_uint32(
    ), obj), 'com.robotraconteur.testing.TestService3.transform[]')
    o.t5 = RR.VarValue(ServiceTest2_create_transform_multidimarray(
        node, 6, 5, gen.get_uint32(), obj), 'com.robotraconteur.testing.TestService3.transform[*]')

    o.t6 = [ServiceTest2_fill_transform(node, gen.get_uint32(), obj)]
    o.t7 = [ServiceTest2_create_transform_array(node, 4, gen.get_uint32(), obj),
            ServiceTest2_create_transform_array(node, 4, gen.get_uint32(), obj)]

    o.t8 = [ServiceTest2_create_transform_multidimarray(node, 2, 4, gen.get_uint32(), obj),
            ServiceTest2_create_transform_multidimarray(node, 2, 4, gen.get_uint32(), obj)]

    o.t9 = RR.VarValue([ServiceTest2_fill_transform(node, gen.get_uint32(
    ), obj)], 'com.robotraconteur.testing.TestService3.transform{list}')

    o.t10 = RR.VarValue([ServiceTest2_create_transform_array(node, 3, gen.get_uint32(), obj),
                         ServiceTest2_create_transform_array(node, 5, gen.get_uint32(), obj)], 'com.robotraconteur.testing.TestService3.transform[]{list}')

    o.t11 = RR.VarValue([ServiceTest2_create_transform_multidimarray(node, 7, 2, gen.get_uint32(), obj),
                         ServiceTest2_create_transform_multidimarray(node, 5, 1, gen.get_uint32(), obj)], 'com.robotraconteur.testing.TestService3.transform[*]{list}')

    return o


def ServiceTest2_verify_teststruct3(v, seed):
    assert v is not None
    gen = ServiceTest2_test_sequence_gen(seed)

    ServiceTest2_verify_testpod1(v.s1[0], gen.get_uint32())
    s2_seed = gen.get_uint32()
    ServiceTest2_verify_testpod1_array(v.s2, (s2_seed % 17), s2_seed)
    ServiceTest2_verify_testpod1_array(v.s3, 11, gen.get_uint32())
    s4_seed = gen.get_uint32()
    ServiceTest2_verify_testpod1_array(v.s4, (s4_seed % 16), s4_seed)
    ServiceTest2_verify_testpod1_multidimarray(v.s5, 3, 3, gen.get_uint32())
    s6_seed = gen.get_uint32()
    ServiceTest2_verify_testpod1_multidimarray(
        v.s6, (s6_seed % 6), (s6_seed % 3), s6_seed)

    assert v.s7 is not None
    assert len(v.s7) == 1
    s7_0 = v.s7[0]
    ServiceTest2_verify_testpod1(s7_0[0], gen.get_uint32())

    assert v.s8 is not None
    assert len(v.s8) == 2
    ServiceTest2_verify_testpod1_array(v.s8[0], 2, gen.get_uint32())
    ServiceTest2_verify_testpod1_array(v.s8[1], 4, gen.get_uint32())

    assert v.s9 is not None
    assert len(v.s9) == 2
    ServiceTest2_verify_testpod1_multidimarray(v.s9[0], 2, 3, gen.get_uint32())
    ServiceTest2_verify_testpod1_multidimarray(v.s9[1], 4, 5, gen.get_uint32())

    s10 = v.s10.data[0]
    ServiceTest2_verify_testpod1(s10, gen.get_uint32())

    ServiceTest2_verify_testpod1_array(v.s11.data, 3, gen.get_uint32())
    ServiceTest2_verify_testpod1_multidimarray(
        v.s12.data, 2, 2, gen.get_uint32())

    assert v.s13 is not None
    s13 = v.s13.data[0]
    ServiceTest2_verify_testpod1(s13.data[0], gen.get_uint32())

    assert (v.s14 is not None)
    v14 = v.s14.data
    assert len(v14) == 2
    ServiceTest2_verify_testpod1_array(v14[0].data, 3, gen.get_uint32())
    ServiceTest2_verify_testpod1_array(v14[1].data, 5, gen.get_uint32())

    assert v.s15 is not None
    v15 = v.s15.data
    assert (len(v15) == 2)
    ServiceTest2_verify_testpod1_multidimarray(
        v15[0].data, 7, 2, gen.get_uint32())
    ServiceTest2_verify_testpod1_multidimarray(
        v15[1].data, 5, 1, gen.get_uint32())

    ServiceTest2_verify_transform(v.t1[0], gen.get_uint32())
    ServiceTest2_verify_transform_array(v.t2, 4, gen.get_uint32())
    ServiceTest2_verify_transform_multidimarray(v.t3, 2, 4, gen.get_uint32())

    ServiceTest2_verify_transform_array(v.t4.data, 10, gen.get_uint32())
    ServiceTest2_verify_transform_multidimarray(
        v.t5.data, 6, 5, gen.get_uint32())

    assert len(v.t6) == 1
    ServiceTest2_verify_transform(v.t6[0], gen.get_uint32())

    assert len(v.t7) == 2
    ServiceTest2_verify_transform_array(v.t7[0], 4, gen.get_uint32())
    ServiceTest2_verify_transform_array(v.t7[1], 4, gen.get_uint32())

    assert len(v.t8) == 2
    ServiceTest2_verify_transform_multidimarray(
        v.t8[0], 2, 4, gen.get_uint32())
    ServiceTest2_verify_transform_multidimarray(
        v.t8[1], 2, 4, gen.get_uint32())

    t9 = v.t9.data
    assert len(t9) == 1
    ServiceTest2_verify_transform(t9[0].data[0], gen.get_uint32())

    t10 = v.t10.data
    assert len(t10) == 2
    ServiceTest2_verify_transform_array(t10[0].data, 3, gen.get_uint32())
    ServiceTest2_verify_transform_array(t10[1].data, 5, gen.get_uint32())

    t11 = v.t11.data
    assert len(t11) == 2
    ServiceTest2_verify_transform_multidimarray(
        t11[0].data, 7, 2, gen.get_uint32())
    ServiceTest2_verify_transform_multidimarray(
        t11[1].data, 5, 1, gen.get_uint32())


def ServiceTest2_fill_transform(node, seed, obj):
    gen = ServiceTest2_test_sequence_gen(seed)
    n_dtype = node.GetNamedArrayDType(
        'com.robotraconteur.testing.TestService3.transform', obj)
    s = np.zeros((1,), dtype=n_dtype)
    s1 = s.view(np.float64)
    s1[:] = np.array([gen.get_double() for _ in xrange(7)])
    return s


def ServiceTest2_verify_transform(s, seed):
    gen = ServiceTest2_test_sequence_gen(seed)
    s1 = np.atleast_1d(s).view(np.float64)
    nptest.assert_allclose(s1, np.array([gen.get_double() for _ in xrange(7)]))


def ServiceTest2_create_transform_array(node, len_, seed, obj):
    gen = ServiceTest2_test_sequence_gen(seed)
    n_dtype = node.GetNamedArrayDType(
        'com.robotraconteur.testing.TestService3.transform', obj)
    s = np.zeros((len_,), dtype=n_dtype)
    for i in xrange(len_):
        s[i] = ServiceTest2_fill_transform(node, gen.get_uint32(), obj)
    return s


def ServiceTest2_verify_transform_array(v, len_, seed):
    gen = ServiceTest2_test_sequence_gen(seed)
    assert (len(v) == len_)
    for i in xrange(len_):
        ServiceTest2_verify_transform(v[i], gen.get_uint32())


def ServiceTest2_create_transform_multidimarray(node, m, n, seed, obj):
    gen = ServiceTest2_test_sequence_gen(seed)
    n_dtype = node.GetNamedArrayDType(
        'com.robotraconteur.testing.TestService3.transform', obj)
    s = np.zeros((m * n,), dtype=n_dtype)
    for i in xrange(m * n):
        s[i] = ServiceTest2_fill_transform(node, gen.get_uint32(), obj)

    return s.reshape((m, n), order="F")


def ServiceTest2_verify_transform_multidimarray(v, m, n, seed):
    assert v.shape == (m, n)
    v2 = v.reshape((m * n,), order="F")
    ServiceTest2_verify_transform_array(v2, m * n, seed)
