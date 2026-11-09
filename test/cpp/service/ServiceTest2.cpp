#ifdef ROBOTRACONTEUR_USE_STDAFX
#include "stdafx.h"
#endif

#include "ServiceTest2.h"
#include "array_compare.h"
#include "service_test_utils.h"
#include "test_vectors.h"

using namespace RobotRaconteur::test;

namespace RobotRaconteurTest
{

void RobotRaconteurTestService2Support::RegisterServices(RR_SHARED_PTR<TcpTransport> transport,
                                                         RR_SHARED_PTR<RobotRaconteurNode> node)
{
    if (!node)
    {
        node = RobotRaconteurNode::sp();
    }
    testservice2 = RR_MAKE_SHARED<testroot3_impl>();
    RR_SHARED_PTR<ServerContext> c =
        node->RegisterService("RobotRaconteurTestService2", "com.robotraconteur.testing.TestService3", testservice2);
    c->RequestObjectLock("RobotRaconteurTestService2.nolock_test", "server");
}

void RobotRaconteurTestService2Support::UnregisterServices()
{
    RobotRaconteurNode::s()->CloseService("RobotRaconteurTestService2");
}

testroot3_impl::testroot3_impl()
{
    RR_INTRUSIVE_PTR<RRPodArray<com::robotraconteur::testing::TestService3::testpod2> > m1 =
        AllocateEmptyRRPodArray<com::robotraconteur::testing::TestService3::testpod2>(1024);

    pod_m1 = RR_MAKE_SHARED<PodArrayMemory<com::robotraconteur::testing::TestService3::testpod2> >(m1);

    RR_INTRUSIVE_PTR<RRPodMultiDimArray<com::robotraconteur::testing::TestService3::testpod2> > m2 =
        AllocateEmptyRRPodMultiDimArray<com::robotraconteur::testing::TestService3::testpod2>();
    uint32_t m2_dims[] = {6, 6};
    m2->Dims = AttachRRArrayCopy(m2_dims, 2);
    m2->PodArray = AllocateEmptyRRPodArray<com::robotraconteur::testing::TestService3::testpod2>(36);

    pod_m2 = RR_MAKE_SHARED<PodMultiDimArrayMemory<com::robotraconteur::testing::TestService3::testpod2> >(m2);

    namedarray_m1 = RR_MAKE_SHARED<NamedArrayMemory<com::robotraconteur::testing::TestService3::transform> >(
        AllocateEmptyRRNamedArray<com::robotraconteur::testing::TestService3::transform>(512));
    std::vector<uint32_t> namedarray_m2_dims = boost::assign::list_of(10)(20);
    namedarray_m2 = RR_MAKE_SHARED<NamedMultiDimArrayMemory<com::robotraconteur::testing::TestService3::transform> >(
        AllocateEmptyRRNamedMultiDimArray<com::robotraconteur::testing::TestService3::transform>(namedarray_m2_dims));

    c_m1 = RR_MAKE_SHARED<ArrayMemory<cdouble> >(AllocateRRArray<cdouble>(512));

    uint32_t c_m2_1[] = {10, 10};

    c_m2 = RR_MAKE_SHARED<MultiDimArrayMemory<cdouble> >(
        AllocateRRMultiDimArray<cdouble>(AttachRRArrayCopy(c_m2_1, 2), AllocateRRArray<cdouble>(100)));

    c_m5 = RR_MAKE_SHARED<ArrayMemory<rr_bool> >(AllocateRRArray<rr_bool>(512));

    c_m6 = RR_MAKE_SHARED<MultiDimArrayMemory<rr_bool> >(
        AllocateRRMultiDimArray<rr_bool>(AttachRRArrayCopy(c_m2_1, 2), AllocateRRArray<rr_bool>(100)));

    obj5_ = RR_MAKE_SHARED<obj5_impl>();
}

void testroot3_impl::set_peekwire(const RR_SHARED_PTR<Wire<int32_t> >& value)
{
    testroot3_default_impl::set_peekwire(value);

    peekwire_timer = ServerContext::GetCurrentServerContext()->GetNode()->CreateTimer(
        boost::posix_time::milliseconds(100),
        boost::bind(&testroot3_impl::peekwire_timer_handler, shared_from_this(), RR_BOOST_PLACEHOLDERS(_1)));
    peekwire_timer->Start();
    rrvar_peekwire->SetOutValue(56295674);
}

void testroot3_impl::peekwire_timer_handler(const TimerEvent& e) { rrvar_peekwire->SetOutValue(56295674); }

int32_t testroot3_impl::get_unknown_modifier() { return 0; }
void testroot3_impl::set_unknown_modifier(int32_t value) {}

int32_t testroot3_impl::get_readme() { return 0; }

void testroot3_impl::set_writeme(int32_t value) {}

RR_SHARED_PTR<Pipe<int32_t> > testroot3_impl::get_unreliable1() { return unreliable1; }
void testroot3_impl::set_unreliable1(const RR_SHARED_PTR<Pipe<int32_t> >& value) { unreliable1 = value; }

RR_SHARED_PTR<ArrayMemory<double> > testroot3_impl::get_readmem() { return RR_SHARED_PTR<ArrayMemory<double> >(); }

RR_SHARED_PTR<com::robotraconteur::testing::TestService3::obj4> testroot3_impl::get_o4()
{
    return RR_MAKE_SHARED<obj4_impl>();
}

com::robotraconteur::testing::TestService3::testenum1::testenum1 testroot3_impl::get_testenum1_prop()
{
    return com::robotraconteur::testing::TestService3::testenum1::anothervalue;
}
void testroot3_impl::set_testenum1_prop(com::robotraconteur::testing::TestService3::testenum1::testenum1 value)
{
    ThrowIfFailures thrower;
    EXPECT_EQ(value, com::robotraconteur::testing::TestService3::testenum1::hexval1);
    thrower.Throw();
}

com::robotraconteur::testing::TestService3::testpod1 testroot3_impl::get_testpod1_prop()
{
    com::robotraconteur::testing::TestService3::testpod1 o;
    ServiceTest2_fill_testpod1(o, 563921043);
    return o;
}
void testroot3_impl::set_testpod1_prop(const com::robotraconteur::testing::TestService3::testpod1& value)
{
    ThrowIfFailures thrower;
    ServiceTest2_verify_testpod1(value, 85932659);
    thrower.Throw();
}

void testroot3_impl::testpod1_func1(const com::robotraconteur::testing::TestService3::testpod1& s)
{
    ThrowIfFailures thrower;
    ServiceTest2_verify_testpod1(s, 29546592);
    thrower.Throw();
}

com::robotraconteur::testing::TestService3::testpod1 testroot3_impl::testpod1_func2()
{
    com::robotraconteur::testing::TestService3::testpod1 o;
    ServiceTest2_fill_testpod1(o, 95836295);
    return o;
}

RR_INTRUSIVE_PTR<com::robotraconteur::testing::TestService3::teststruct3> testroot3_impl::get_teststruct3_prop()
{
    return ServiceTest2_fill_teststruct3(16483675);
}
void testroot3_impl::set_teststruct3_prop(
    const RR_INTRUSIVE_PTR<com::robotraconteur::testing::TestService3::teststruct3>& value)
{
    ThrowIfFailures thrower;
    ServiceTest2_verify_teststruct3(value, 858362);
    thrower.Throw();
}

RR_SHARED_PTR<PodArrayMemory<com::robotraconteur::testing::TestService3::testpod2> > testroot3_impl::get_pod_m1()
{
    return pod_m1;
}

RR_SHARED_PTR<PodMultiDimArrayMemory<com::robotraconteur::testing::TestService3::testpod2> > testroot3_impl::
    get_pod_m2()
{
    return pod_m2;
}

class func4_gen : public SyncGenerator<RR_INTRUSIVE_PTR<RRArray<uint8_t> >, RR_INTRUSIVE_PTR<RRArray<uint8_t> > >
{
  public:
    size_t j;
    bool aborted;

    func4_gen()
    {
        j = 0;
        aborted = false;
    }

    virtual RR_INTRUSIVE_PTR<RRArray<uint8_t> > Next(const RR_INTRUSIVE_PTR<RRArray<uint8_t> >& v)
    {
        if (aborted)
            throw OperationAbortedException("");
        if (j >= 8)
        {
            throw StopIterationException("");
        }

        RR_INTRUSIVE_PTR<RRArray<uint8_t> > a = AllocateRRArray<uint8_t>(v->size());
        for (size_t i = 0; i < v->size(); i++)
        {
            (*a)[i] = (*v)[i] + j;
        }
        j++;
        return a;
    }

    virtual void Abort() { aborted = true; }

    virtual void Close() { j = 1000; }
};

RR_SHARED_PTR<Generator<RR_INTRUSIVE_PTR<RRArray<uint8_t> >, RR_INTRUSIVE_PTR<RRArray<uint8_t> > > > testroot3_impl::
    gen_func4()
{
    return RR_MAKE_SHARED<func4_gen>();
}

class func1_gen : public SyncGenerator<double, void>
{
  public:
    double j;
    bool aborted;

    func1_gen()
    {
        j = 0;
        aborted = false;
    }

    virtual double Next()
    {
        if (aborted)
            throw OperationAbortedException("");
        if (j >= 16)
        {
            throw StopIterationException("");
        }

        return j++;
    }

    virtual void Abort() { aborted = true; }

    virtual void Close() { j = 1000; }
};

RR_SHARED_PTR<Generator<double, void> > testroot3_impl::gen_func1() { return RR_MAKE_SHARED<func1_gen>(); }

RR_SHARED_PTR<Generator<RR_INTRUSIVE_PTR<RRArray<uint8_t> >, void> > testroot3_impl::gen_func2(const std::string& name)
{
    std::vector<RR_INTRUSIVE_PTR<RRArray<uint8_t> > > range;
    for (uint8_t i = 0; i < 16; i++)
    {
        range.push_back(ScalarToRRArray(i));
    }

    return CreateRangeGenerator(range);
}

RR_SHARED_PTR<Generator<void, RR_INTRUSIVE_PTR<RRArray<uint8_t> > > > testroot3_impl::gen_func3(const std::string& name)
{
    return RR_SHARED_PTR<Generator<void, RR_INTRUSIVE_PTR<RRArray<uint8_t> > > >();
}

RR_SHARED_PTR<Generator<RR_INTRUSIVE_PTR<com::robotraconteur::testing::TestService1::teststruct2>,
                        RR_INTRUSIVE_PTR<com::robotraconteur::testing::TestService1::teststruct2> > >
testroot3_impl::gen_func5()
{
    return RR_SHARED_PTR<Generator<RR_INTRUSIVE_PTR<com::robotraconteur::testing::TestService1::teststruct2>,
                                   RR_INTRUSIVE_PTR<com::robotraconteur::testing::TestService1::teststruct2> > >();
}

com::robotraconteur::testing::TestService3::vector3 testroot3_impl::get_testnamedarray1()
{
    com::robotraconteur::testing::TestService3::transform a1;
    ServiceTest2_fill_transform(a1, 74637);
    return a1.s.translation;
}
void testroot3_impl::set_testnamedarray1(const com::robotraconteur::testing::TestService3::vector3& value)
{
    ThrowIfFailures thrower;
    com::robotraconteur::testing::TestService3::transform a1;
    ServiceTest2_fill_transform(a1, 3956378);
    a1.s.translation = value;
    ServiceTest2_verify_transform(a1, 3956378);
    thrower.Throw();
}

com::robotraconteur::testing::TestService3::transform testroot3_impl::get_testnamedarray2()
{
    com::robotraconteur::testing::TestService3::transform a2;
    ServiceTest2_fill_transform(a2, 1294);
    return a2;
}
void testroot3_impl::set_testnamedarray2(const com::robotraconteur::testing::TestService3::transform& value)
{
    ThrowIfFailures thrower;
    ServiceTest2_verify_transform(value, 827635);
    thrower.Throw();
}

RR_INTRUSIVE_PTR<RRNamedArray<com::robotraconteur::testing::TestService3::transform> > testroot3_impl::
    get_testnamedarray3()
{
    return ServiceTest2_fill_transform_array(8, 837512);
}
void testroot3_impl::set_testnamedarray3(
    const RR_INTRUSIVE_PTR<RRNamedArray<com::robotraconteur::testing::TestService3::transform> >& value)
{
    ThrowIfFailures thrower;
    ServiceTest2_verify_transform_array(value, 6, 19274);
    thrower.Throw();
}

RR_INTRUSIVE_PTR<RRNamedMultiDimArray<com::robotraconteur::testing::TestService3::transform> > testroot3_impl::
    get_testnamedarray4()
{
    return ServiceTest2_fill_transform_multidimarray(7, 2, 66134);
}
void testroot3_impl::set_testnamedarray4(
    const RR_INTRUSIVE_PTR<RRNamedMultiDimArray<com::robotraconteur::testing::TestService3::transform> >& value)
{
    ThrowIfFailures thrower;
    ServiceTest2_verify_transform_multidimarray(value, 5, 2, 6385);
    thrower.Throw();
}

RR_INTRUSIVE_PTR<RRNamedMultiDimArray<com::robotraconteur::testing::TestService3::transform> > testroot3_impl::
    get_testnamedarray5()
{
    return ServiceTest2_fill_transform_multidimarray(3, 2, 773142);
}
void testroot3_impl::set_testnamedarray5(
    const RR_INTRUSIVE_PTR<RRNamedMultiDimArray<com::robotraconteur::testing::TestService3::transform> >& value)
{
    ThrowIfFailures thrower;
    ServiceTest2_verify_transform_multidimarray(value, 3, 2, 7732);
    thrower.Throw();
}

RR_SHARED_PTR<NamedArrayMemory<com::robotraconteur::testing::TestService3::transform> > testroot3_impl::
    get_namedarray_m1()
{
    return namedarray_m1;
}
RR_SHARED_PTR<NamedMultiDimArrayMemory<com::robotraconteur::testing::TestService3::transform> > testroot3_impl::
    get_namedarray_m2()
{
    return namedarray_m2;
}

cdouble testroot3_impl::get_c1() { return cdouble(5.708705e+01, -2.328294e-03); }
void testroot3_impl::set_c1(cdouble value)
{
    ThrowIfFailures thrower;
    cdouble value1(5.708705e+01, -2.328294e-03);
    EXPECT_EQ(value, value1);
    thrower.Throw();
}
RR_INTRUSIVE_PTR<RRArray<cdouble> > testroot3_impl::get_c2()
{
    double c2_1_1[] = {1.968551e+07,  2.380643e+18,  3.107374e-16,  7.249542e-16,  -4.701135e-19, -6.092764e-17,
                       2.285854e+14,  2.776180e+05,  -1.436152e-12, 3.626609e+11,  3.600952e-02,  -3.118123e-16,
                       -1.312210e-10, -1.738940e-07, -1.476586e-12, -2.899781e-20, 4.806642e+03,  4.476869e-05,
                       -2.935084e-16, 3.114019e-20,  -3.675955e+01, 3.779796e-21,  2.190594e-11,  4.251420e-06,
                       -9.715221e+11, -3.483924e-01, 7.606428e+05,  5.418088e+15,  4.786378e+16,  -1.202581e+08,
                       -1.662061e+02, -2.392954e+03};
    return AttachRRArrayCopy<cdouble>((cdouble*)c2_1_1, 16);
}
void testroot3_impl::set_c2(const RR_INTRUSIVE_PTR<RRArray<cdouble> >& value)
{
    ThrowIfFailures thrower;
    double c2_2_1[] = {4.925965e-03,  5.695254e+13,  -4.576890e-14, -6.056342e-07, -4.918571e-08, -1.940684e-10,
                       1.549104e-02,  -1.954145e+04, -2.499019e-16, 4.010614e+09,  -1.906811e-08, 3.297924e-10,
                       2.742399e-02,  -4.372839e-01, -3.093171e-10, 4.311755e-01,  -2.218220e-14, 5.399758e+10,
                       3.360304e+17,  1.340681e-18,  -4.441140e+11, -1.845055e-09, -3.074586e-10, -1.754926e+01,
                       -2.766799e+04, -2.307577e+10, 2.754875e+14,  1.179639e+15,  6.976204e-10,  1.901856e+08,
                       -3.824351e-02, -1.414167e+08};
    RR_INTRUSIVE_PTR<RRArray<cdouble> > c2_2 = AttachRRArrayCopy((cdouble*)c2_2_1, 16);
    EXPECT_RRARRAY_EQ(value, c2_2);
    thrower.Throw();
}

RR_INTRUSIVE_PTR<RRMultiDimArray<cdouble> > testroot3_impl::get_c3()
{
    uint32_t c3_1_1[] = {2, 5};
    double c3_1_2[] = {5.524802e+18,  -2.443857e-05, 3.737932e-02,  -4.883553e-03, -1.184347e+12,
                       4.537366e-08,  -4.567913e-01, -1.683542e+15, -1.676517e+00, -8.911085e+12,
                       -2.537376e-17, 1.835687e-10,  -9.366069e-22, -5.426323e-12, -7.820969e-10,
                       -1.061541e+12, -3.660854e-12, -4.969930e-03, 1.988428e+07,  1.860782e-16};
    return AllocateRRMultiDimArray<cdouble>(AttachRRArrayCopy(c3_1_1, 2), AttachRRArrayCopy((cdouble*)c3_1_2, 10));
}
void testroot3_impl::set_c3(const RR_INTRUSIVE_PTR<RRMultiDimArray<cdouble> >& value)
{
    ThrowIfFailures thrower;
    uint32_t c3_2_1[] = {3, 4};
    double c3_2_2[] = {4.435180e+04,  5.198060e-18,  -1.316737e-13, -4.821771e-03, -4.077550e-19, -1.659105e-09,
                       -6.332363e-11, -1.128999e+16, 4.869912e+16,  2.680490e-04,  -8.880119e-04, 3.960452e+11,
                       4.427784e-09,  -2.813742e-18, 7.397516e+18,  1.196394e+13,  3.236906e-14,  -4.219297e-17,
                       1.316282e-06,  -2.771084e-18, -1.239118e-09, 2.887453e-08,  -1.746515e+08, -2.312264e-11};

    EXPECT_RRARRAY_EQ(value->Dims, AttachRRArray(c3_2_1, 2, false));
    EXPECT_RRARRAY_EQ(value->Array, AttachRRArray((cdouble*)c3_2_2, 12, false));
    thrower.Throw();
}

RR_INTRUSIVE_PTR<RRList<RRArray<cdouble> > > testroot3_impl::get_c5()
{
    RR_INTRUSIVE_PTR<RRList<RRArray<cdouble> > > c5_1 = AllocateEmptyRRList<RRArray<cdouble> >();
    double c5_1_1[] = {1.104801e+00,  4.871266e-10, -2.392938e-03, 4.210339e-07, 1.474114e-19, -1.147137e-01,
                       -2.026434e+06, 4.450447e-19, 3.702953e-21,  9.722025e+12, 3.464073e-14, 4.628110e+15,
                       2.345453e-19,  3.730012e-04, 4.116650e+16,  4.380220e+08};
    c5_1->push_back(AttachRRArrayCopy((cdouble*)c5_1_1, 8));
    return c5_1;
}
void testroot3_impl::set_c5(const RR_INTRUSIVE_PTR<RRList<RRArray<cdouble> > >& value)
{
    ThrowIfFailures thrower;
    EXPECT_TRUE(value);
    thrower.Throw();
    double c5_2_1[] = {2.720831e-20,  2.853037e-16,  -7.982497e+16, -2.684318e-09, -2.505796e+17, -4.743970e-12,
                       -3.657056e+11, 2.718388e+15,  1.597672e+03,  2.611859e+14,  2.224926e+06,  -1.431096e-09,
                       3.699894e+19,  -5.936706e-01, -1.385395e-09, -4.248415e-13};
    ca(value->front(), AttachRRArray<cdouble>((cdouble*)c5_2_1, 8, false));
    thrower.Throw();
}

cfloat testroot3_impl::get_c7() { return cfloat(-5.527021e-18, -9.848457e+03); }
void testroot3_impl::set_c7(cfloat value)
{
    ThrowIfFailures thrower;
    cfloat value1(9.303345e-12, -3.865684e-05);
    EXPECT_EQ(value, value1);
    thrower.Throw();
}
RR_INTRUSIVE_PTR<RRArray<cfloat> > testroot3_impl::get_c8()
{
    float c8_1_1[] = {-3.153395e-09, 3.829492e-02,  -2.665239e+12, 1.592927e-03,  3.188444e+06,  -3.595015e-11,
                      2.973887e-18,  -2.189921e+17, 1.651567e+10,  1.095838e+05,  3.865249e-02,  4.725510e+10,
                      -2.334376e+03, 3.744977e-05,  -1.050821e+02, 1.122660e-22,  3.501520e-18,  -2.991601e-17,
                      6.039622e-17,  4.778095e-07,  -4.793136e-05, 3.096513e+19,  2.476004e+18,  1.296297e-03,
                      2.165336e-13,  4.834427e+06,  4.675370e-01,  -2.942290e-12, -2.090883e-19, 6.674942e+07,
                      -4.809047e-10, -4.911772e-13};
    return AttachRRArrayCopy<cfloat>((cfloat*)c8_1_1, 16);
}
void testroot3_impl::set_c8(const RR_INTRUSIVE_PTR<RRArray<cfloat> >& value)
{
    ThrowIfFailures thrower;
    float c8_2_1[] = {1.324498e+06,  1.341746e-04,  4.292993e-04,  -3.844509e+15, -3.804802e+10, 3.785305e-12,
                      2.628285e-19,  -1.664089e+15, -4.246472e-10, -3.334943e+03, -3.305796e-01, 1.878648e-03,
                      1.420880e-05,  -3.024657e+14, 2.227031e-21,  2.044653e+17,  9.753609e-20,  -6.581817e-03,
                      3.271063e-03,  -1.726081e+06, -1.614502e-06, -2.641638e-19, -2.977317e+07, -1.278224e+03,
                      -1.760207e-05, -4.877944e-07, -2.171524e+02, 1.620645e+01,  -4.334168e-02, 1.871011e-09,
                      -3.066163e+06, -3.533662e+07};
    RR_INTRUSIVE_PTR<RRArray<cfloat> > c8_2 = AttachRRArrayCopy((cfloat*)c8_2_1, 16);
    EXPECT_RRARRAY_EQ(c8_2, value);
    thrower.Throw();
}

RR_INTRUSIVE_PTR<RRMultiDimArray<cfloat> > testroot3_impl::get_c9()
{
    uint32_t c9_1_1[] = {2, 4};
    float c9_1_2[] = {1.397743e+15,  3.933042e+10, -3.812329e+07, 1.508109e+16,  -2.091397e-20, 3.207851e+12,
                      -3.640702e+02, 3.903769e+02, -2.879727e+17, -4.589604e-06, 2.202769e-06,  2.892523e+04,
                      -3.306489e-14, 4.522308e-06, 1.665807e+15,  2.340476e+10};

    return AllocateRRMultiDimArray<cfloat>(AttachRRArrayCopy(c9_1_1, 2), AttachRRArrayCopy((cfloat*)c9_1_2, 8));
}
void testroot3_impl::set_c9(const RR_INTRUSIVE_PTR<RRMultiDimArray<cfloat> >& value)
{
    ThrowIfFailures thrower;
    uint32_t c9_2_1[] = {2, 2, 2};
    float c9_2_2[] = {2.138322e-03,  4.036979e-21,  1.345236e+10, -1.348460e-12, -3.615340e+12, -2.911340e-21,
                      3.220362e+09,  3.459909e-04,  4.276259e-08, -3.199451e+18, 3.468308e+07,  -2.928506e-09,
                      -3.154288e+17, -2.352920e-02, 6.976385e-21, 2.435472e+12};
    EXPECT_RRARRAY_EQ(value->Dims, AttachRRArray(c9_2_1, 3, false));
    EXPECT_RRARRAY_EQ(value->Array, AttachRRArray((cfloat*)c9_2_2, 8, false));
    thrower.Throw();
}

RR_SHARED_PTR<ArrayMemory<cdouble> > testroot3_impl::get_c_m1() { return c_m1; }

RR_SHARED_PTR<MultiDimArrayMemory<cdouble> > testroot3_impl::get_c_m2() { return c_m2; }

RR_SHARED_PTR<com::robotraconteur::testing::TestService3::obj5> testroot3_impl::get_nolock_test() { return obj5_; }

rr_bool testroot3_impl::get_b1() { return 1; }
void testroot3_impl::set_b1(rr_bool value)
{
    ThrowIfFailures thrower;
    EXPECT_NE(value, 0);
    thrower.Throw();
}

RR_INTRUSIVE_PTR<RRArray<rr_bool> > testroot3_impl::get_b2()
{
    rr_bool v[] = {1, 0, 1, 1, 0, 1, 0};
    return AttachRRArrayCopy<rr_bool>(v, 7);
}
void testroot3_impl::set_b2(const RR_INTRUSIVE_PTR<RRArray<rr_bool> >& value)
{
    ThrowIfFailures thrower;
    EXPECT_EQ(value->size(), 8);
    thrower.Throw();
    rr_bool v[] = {1, 0, 0, 1, 1, 1, 0, 1};
    for (size_t i = 0; i < 8; i++)
    {
        EXPECT_EQ(value->at(i), v[i]);
        if (value->at(i) != v[i])
            ;
        {
            break;
        }
    }
    thrower.Throw();
}

RR_INTRUSIVE_PTR<RRMultiDimArray<rr_bool> > testroot3_impl::get_b3()
{
    rr_bool v[] = {0, 1, 1, 0};
    uint32_t dims[] = {2, 2};

    return AllocateRRMultiDimArray(AttachRRArrayCopy(dims, 2), AttachRRArrayCopy(v, 4));
}
void testroot3_impl::set_b3(const RR_INTRUSIVE_PTR<RRMultiDimArray<rr_bool> >& value)
{
    ThrowIfFailures thrower;
    EXPECT_TRUE(value);
    thrower.Throw();
    EXPECT_EQ(value->Dims->size(), 2);
    EXPECT_EQ(value->Array->size(), 2);
    EXPECT_EQ(value->Dims->at(0), 2);
    EXPECT_EQ(value->Dims->at(1), 1);
    EXPECT_NE(value->Array->at(0), 0);
    EXPECT_EQ(value->Array->at(1), 0);
    thrower.Throw();
}

RR_INTRUSIVE_PTR<RRList<RRArray<rr_bool> > > testroot3_impl::get_b4()
{
    RR_INTRUSIVE_PTR<RRList<RRArray<rr_bool> > > o = AllocateEmptyRRList<RRArray<rr_bool> >();
    o->push_back(ScalarToRRArray<rr_bool>(1));
    return o;
}
void testroot3_impl::set_b4(const RR_INTRUSIVE_PTR<RRList<RRArray<rr_bool> > >& value)
{
    ThrowIfFailures thrower;
    EXPECT_TRUE(value);
    thrower.Throw();
    EXPECT_EQ(value->size(), 1);
    EXPECT_EQ(value->front()->size(), 1);
    EXPECT_NE(value->front()->at(0), 0);
    thrower.Throw();
}

RR_INTRUSIVE_PTR<RRList<RRArray<rr_bool> > > testroot3_impl::get_b5()
{
    rr_bool v[] = {0, 1, 0, 0};
    RR_INTRUSIVE_PTR<RRList<RRArray<rr_bool> > > o = AllocateEmptyRRList<RRArray<rr_bool> >();
    o->push_back(AttachRRArrayCopy(v, 4));
    return o;
}
void testroot3_impl::set_b5(const RR_INTRUSIVE_PTR<RRList<RRArray<rr_bool> > >& value)
{
    ThrowIfFailures thrower;
    EXPECT_TRUE(value);
    thrower.Throw();
    EXPECT_EQ(value->size(), 1);
    EXPECT_TRUE(value->front());
    EXPECT_EQ(value->front()->size(), 2);
    EXPECT_NE(value->front()->at(0), 0);
    EXPECT_EQ(value->front()->at(1), 0);
    thrower.Throw();
}

RR_INTRUSIVE_PTR<RRList<RRMultiDimArray<rr_bool> > > testroot3_impl::get_b6()
{
    rr_bool v[] = {0, 1, 1, 0};
    uint32_t dims[] = {2, 2};

    RR_INTRUSIVE_PTR<RRList<RRMultiDimArray<rr_bool> > > o = AllocateEmptyRRList<RRMultiDimArray<rr_bool> >();

    o->push_back(AllocateRRMultiDimArray(AttachRRArrayCopy(dims, 2), AttachRRArrayCopy(v, 4)));
    return o;
}
void testroot3_impl::set_b6(const RR_INTRUSIVE_PTR<RRList<RRMultiDimArray<rr_bool> > >& value)
{
    ThrowIfFailures thrower;
    RR_INTRUSIVE_PTR<RRMultiDimArray<rr_bool> > value1;

    EXPECT_TRUE(value);
    thrower.Throw();
    EXPECT_EQ(value->size(), 1);
    value1 = value->front();

    EXPECT_EQ(value1->Dims->size(), 2);
    EXPECT_EQ(value1->Array->size(), 2);
    EXPECT_EQ(value1->Dims->at(0), 2);
    EXPECT_EQ(value1->Dims->at(1), 1);
    EXPECT_NE(value1->Array->at(0), 0);
    EXPECT_EQ(value1->Array->at(1), 0);
    thrower.Throw();
}

RR_SHARED_PTR<ArrayMemory<rr_bool> > testroot3_impl::get_c_m5() { return c_m5; }

RR_SHARED_PTR<MultiDimArrayMemory<rr_bool> > testroot3_impl::get_c_m6() { return c_m6; }

void testroot3_impl::test_exception_params1()
{
    RR_INTRUSIVE_PTR<RRMap<std::string, RRValue> > params = AllocateEmptyRRMap<std::string, RRValue>();
    params->insert(std::make_pair("param1", ScalarToRRArray<int32_t>(10)));
    params->insert(std::make_pair("param2", stringToRRArray("20")));
    throw InvalidOperationException("test error", "my_error", params);
}

void testroot3_impl::test_exception_params2()
{
    RR_INTRUSIVE_PTR<RRMap<std::string, RRValue> > params = AllocateEmptyRRMap<std::string, RRValue>();
    params->insert(std::make_pair("param1", ScalarToRRArray<int32_t>(30)));
    params->insert(std::make_pair("param2", stringToRRArray("40")));
    throw com::robotraconteur::testing::TestService3::test_exception4("test error2", "my_error2", params);
}

obj5_impl::obj5_impl()
{
    _m1 = RR_MAKE_SHARED<ArrayMemory<int32_t> >(AllocateRRArray<int32_t>(100));
    _m2 = RR_MAKE_SHARED<ArrayMemory<int32_t> >(AllocateRRArray<int32_t>(100));
    _m3 = RR_MAKE_SHARED<ArrayMemory<int32_t> >(AllocateRRArray<int32_t>(100));
}

RR_SHARED_PTR<ArrayMemory<int32_t> > obj5_impl::get_m1() { return _m1; }

RR_SHARED_PTR<ArrayMemory<int32_t> > obj5_impl::get_m2() { return _m2; }

RR_SHARED_PTR<ArrayMemory<int32_t> > obj5_impl::get_m3() { return _m3; }

int32_t obj5_impl::f1() { return 10; }

int32_t obj5_impl::f2() { return 11; }

namespace
{
const std::vector<double> double_constants = GetTestVector<double>("seq.double_constants");
const std::vector<uint32_t> uint32_constants = GetTestVector<uint32_t>("seq.uint32_constants");
} // namespace

class ServiceTest2_test_sequence_gen
{
    uint32_t counter;
    uint32_t step;

  public:
    ServiceTest2_test_sequence_gen(uint32_t seed)
    {
        counter = seed % 7823;
        step = seed % 17 + 7;
    }
    double get_double()
    {
        counter += step;
        return double_constants[counter % double_constants.size()];
    }
    uint32_t get_uint32()
    {
        counter += step;
        return uint32_constants[counter % uint32_constants.size()];
    }
    int8_t get_int8()
    {
        counter += step;
        return (int8_t)(((int32_t)(counter % 0xFF)) - 128);
    }
};

void ServiceTest2_fill_testpod1(com::robotraconteur::testing::TestService3::testpod1& v, uint32_t seed)
{
    ServiceTest2_test_sequence_gen gen(seed);

    v.d1 = gen.get_double();
    for (size_t i = 0; i < v.d2.size(); i++)
        v.d2[i] = gen.get_double();
    v.d3.clear();
    size_t d3_len = gen.get_uint32() % 6;
    v.d3.resize(d3_len);
    for (size_t i = 0; i < d3_len; i++)
        v.d3.at(i) = (gen.get_double());
    for (size_t i = 0; i < v.d4.size(); i++)
        v.d4[i] = gen.get_double();
    ServiceTest2_fill_testpod2(v.s1, gen.get_uint32());
    for (size_t i = 0; i < v.s2.size(); i++)
        ServiceTest2_fill_testpod2(v.s2[i], gen.get_uint32());
    size_t s3_len = gen.get_uint32() % 9;
    v.s3.resize(s3_len);
    for (size_t i = 0; i < s3_len; i++)
    {
        com::robotraconteur::testing::TestService3::testpod2 v2;
        ServiceTest2_fill_testpod2(v2, gen.get_uint32());
        v.s3.at(i) = (v2);
    }
    for (size_t i = 0; i < v.s4.size(); i++)
        ServiceTest2_fill_testpod2(v.s4[i], gen.get_uint32());

    ServiceTest2_fill_transform(v.t1, gen.get_uint32());

    for (size_t i = 0; i < 4; i++)
        ServiceTest2_fill_transform(v.t2[i], gen.get_uint32());

    size_t t3_len = gen.get_uint32() % 15;
    v.t3.resize(t3_len);
    for (size_t i = 0; i < t3_len; i++)
        ServiceTest2_fill_transform(v.t3[i], gen.get_uint32());

    for (size_t i = 0; i < 8; i++)
        ServiceTest2_fill_transform(v.t4[i], gen.get_uint32());
}
void ServiceTest2_verify_testpod1(const com::robotraconteur::testing::TestService3::testpod1& v, uint32_t seed)
{
    ServiceTest2_test_sequence_gen gen(seed);

    EXPECT_EQ(v.d1, gen.get_double());
    for (size_t i = 0; i < v.d2.size(); i++)
        EXPECT_EQ(v.d2[i], gen.get_double()) << " at index " << i;
    size_t d3_len = gen.get_uint32() % 6;
    EXPECT_EQ(v.d3.size(), d3_len);
    for (size_t i = 0; i < d3_len; i++)
        EXPECT_EQ(v.d3[i], gen.get_double()) << " at index " << i;
    for (size_t i = 0; i < v.d4.size(); i++)
        EXPECT_EQ(v.d4[i], gen.get_double()) << " at index " << i;
    ServiceTest2_verify_testpod2(v.s1, gen.get_uint32());
    for (size_t i = 0; i < v.s2.size(); i++)
        ServiceTest2_verify_testpod2(v.s2[i], gen.get_uint32());
    size_t s3_len = gen.get_uint32() % 9;
    EXPECT_EQ(v.s3.size(), s3_len);
    for (size_t i = 0; i < s3_len; i++)
        ServiceTest2_verify_testpod2(v.s3[i], gen.get_uint32());
    for (size_t i = 0; i < v.s4.size(); i++)
        ServiceTest2_verify_testpod2(v.s4[i], gen.get_uint32());

    ServiceTest2_verify_transform(v.t1, gen.get_uint32());

    for (size_t i = 0; i < 4; i++)
        ServiceTest2_verify_transform(v.t2[i], gen.get_uint32());

    size_t t3_len = gen.get_uint32() % 15;
    EXPECT_EQ(v.t3.size(), t3_len);
    for (size_t i = 0; i < t3_len; i++)
        ServiceTest2_verify_transform(v.t3[i], gen.get_uint32());

    for (size_t i = 0; i < 8; i++)
        ServiceTest2_verify_transform(v.t4[i], gen.get_uint32());
}
void ServiceTest2_fill_testpod2(com::robotraconteur::testing::TestService3::testpod2& v, uint32_t seed)
{
    ServiceTest2_test_sequence_gen gen(seed);

    v.i1 = gen.get_int8();
    for (size_t i = 0; i < v.i2.size(); i++)
        v.i2[i] = gen.get_int8();
    v.i3.clear();
    size_t i3_len = gen.get_uint32() % 15;
    v.i3.resize(i3_len);
    for (size_t i = 0; i < i3_len; i++)
        v.i3.at(i) = (gen.get_int8());
}
void ServiceTest2_verify_testpod2(const com::robotraconteur::testing::TestService3::testpod2& v, uint32_t seed)
{
    ServiceTest2_test_sequence_gen gen(seed);

    EXPECT_EQ(v.i1, gen.get_int8());
    for (size_t i = 0; i < v.i2.size(); i++)
        EXPECT_EQ(v.i2[i], gen.get_int8()) << " at index " << i;

    size_t i3_len = gen.get_uint32() % 15;
    EXPECT_EQ(v.i3.size(), i3_len);

    for (size_t i = 0; i < i3_len; i++)
        EXPECT_EQ(v.i3[i], gen.get_int8()) << " at index " << i;
}

RR_INTRUSIVE_PTR<com::robotraconteur::testing::TestService3::teststruct3> ServiceTest2_fill_teststruct3(uint32_t seed)
{
    ServiceTest2_test_sequence_gen gen(seed);

    RR_INTRUSIVE_PTR<com::robotraconteur::testing::TestService3::teststruct3> o(
        new com::robotraconteur::testing::TestService3::teststruct3());

    ServiceTest2_fill_testpod1(o->s1, gen.get_uint32());
    uint32_t s2_seed = gen.get_uint32();
    o->s2 = ServiceTest2_fill_testpod1_array(s2_seed % 17, s2_seed);
    o->s3 = ServiceTest2_fill_testpod1_array(11, gen.get_uint32());
    uint32_t s4_seed = gen.get_uint32();
    o->s4 = ServiceTest2_fill_testpod1_array(s4_seed % 16, s4_seed);
    o->s5 = ServiceTest2_fill_testpod1_multidimarray(3, 3, gen.get_uint32());
    uint32_t s6_seed = gen.get_uint32();
    o->s6 = ServiceTest2_fill_testpod1_multidimarray(s6_seed % 6, s6_seed % 3, s6_seed);

    o->s7 = AllocateEmptyRRList<RRPodArray<com::robotraconteur::testing::TestService3::testpod1> >();
    com::robotraconteur::testing::TestService3::testpod1 s7_1;
    ServiceTest2_fill_testpod1(s7_1, gen.get_uint32());
    o->s7->push_back(ScalarToRRPodArray(s7_1));

    o->s8 = AllocateEmptyRRList<RRPodArray<com::robotraconteur::testing::TestService3::testpod1> >();
    o->s8->push_back(ServiceTest2_fill_testpod1_array(2, gen.get_uint32()));
    o->s8->push_back(ServiceTest2_fill_testpod1_array(4, gen.get_uint32()));

    o->s9 = AllocateEmptyRRList<RRPodMultiDimArray<com::robotraconteur::testing::TestService3::testpod1> >();
    o->s9->push_back(ServiceTest2_fill_testpod1_multidimarray(2, 3, gen.get_uint32()));
    o->s9->push_back(ServiceTest2_fill_testpod1_multidimarray(4, 5, gen.get_uint32()));

    com::robotraconteur::testing::TestService3::testpod1 s10;
    ServiceTest2_fill_testpod1(s10, gen.get_uint32());
    o->s10 = ScalarToRRPodArray(s10);

    o->s11 = ServiceTest2_fill_testpod1_array(3, gen.get_uint32());

    o->s12 = ServiceTest2_fill_testpod1_multidimarray(2, 2, gen.get_uint32());

    RR_INTRUSIVE_PTR<RRList<RRValue> > s13 = AllocateEmptyRRList<RRValue>();
    com::robotraconteur::testing::TestService3::testpod1 s13_1;
    ServiceTest2_fill_testpod1(s13_1, gen.get_uint32());
    s13->push_back(ScalarToRRPodArray(s13_1));
    o->s13 = s13;

    RR_INTRUSIVE_PTR<RRList<RRValue> > s14 = AllocateEmptyRRList<RRValue>();
    s14->push_back(ServiceTest2_fill_testpod1_array(3, gen.get_uint32()));
    s14->push_back(ServiceTest2_fill_testpod1_array(5, gen.get_uint32()));
    o->s14 = s14;

    RR_INTRUSIVE_PTR<RRList<RRValue> > s15 = AllocateEmptyRRList<RRValue>();
    s15->push_back(ServiceTest2_fill_testpod1_multidimarray(7, 2, gen.get_uint32()));
    s15->push_back(ServiceTest2_fill_testpod1_multidimarray(5, 1, gen.get_uint32()));
    o->s15 = s15;

    ServiceTest2_fill_transform(o->t1, gen.get_uint32());

    o->t2 = ServiceTest2_fill_transform_array(4, gen.get_uint32());
    o->t3 = ServiceTest2_fill_transform_multidimarray(2, 4, gen.get_uint32());

    o->t4 = ServiceTest2_fill_transform_array(10, gen.get_uint32());
    o->t5 = ServiceTest2_fill_transform_multidimarray(6, 5, gen.get_uint32());

    o->t6 = AllocateEmptyRRList<RRNamedArray<com::robotraconteur::testing::TestService3::transform> >();
    com::robotraconteur::testing::TestService3::transform t6_1;
    ServiceTest2_fill_transform(t6_1, gen.get_uint32());
    o->t6->push_back(ScalarToRRNamedArray(t6_1));

    o->t7 = AllocateEmptyRRList<RRNamedArray<com::robotraconteur::testing::TestService3::transform> >();
    o->t7->push_back(ServiceTest2_fill_transform_array(4, gen.get_uint32()));
    o->t7->push_back(ServiceTest2_fill_transform_array(4, gen.get_uint32()));

    o->t8 = AllocateEmptyRRList<RRNamedMultiDimArray<com::robotraconteur::testing::TestService3::transform> >();
    o->t8->push_back(ServiceTest2_fill_transform_multidimarray(2, 4, gen.get_uint32()));
    o->t8->push_back(ServiceTest2_fill_transform_multidimarray(2, 4, gen.get_uint32()));

    RR_INTRUSIVE_PTR<RRList<RRValue> > t9 = AllocateEmptyRRList<RRValue>();
    com::robotraconteur::testing::TestService3::transform t9_1;
    ServiceTest2_fill_transform(t9_1, gen.get_uint32());
    t9->push_back(ScalarToRRNamedArray(t9_1));
    o->t9 = t9;

    RR_INTRUSIVE_PTR<RRList<RRValue> > t10 = AllocateEmptyRRList<RRValue>();
    t10->push_back(ServiceTest2_fill_transform_array(3, gen.get_uint32()));
    t10->push_back(ServiceTest2_fill_transform_array(5, gen.get_uint32()));
    o->t10 = t10;

    RR_INTRUSIVE_PTR<RRList<RRValue> > t11 = AllocateEmptyRRList<RRValue>();
    t11->push_back(ServiceTest2_fill_transform_multidimarray(7, 2, gen.get_uint32()));
    t11->push_back(ServiceTest2_fill_transform_multidimarray(5, 1, gen.get_uint32()));
    o->t11 = t11;

    return o;
}
void ServiceTest2_verify_teststruct3(const RR_INTRUSIVE_PTR<com::robotraconteur::testing::TestService3::teststruct3> v,
                                     uint32_t seed)
{
    ServiceTest2_test_sequence_gen gen(seed);

    ASSERT_TRUE(v);
    ServiceTest2_verify_testpod1(v->s1, gen.get_uint32());
    uint32_t s2_seed = gen.get_uint32();
    ServiceTest2_verify_testpod1_array(v->s2, s2_seed % 17, s2_seed);
    ServiceTest2_verify_testpod1_array(v->s3, 11, gen.get_uint32());
    uint32_t s4_seed = gen.get_uint32();
    ServiceTest2_verify_testpod1_array(v->s4, s4_seed % 16, s4_seed);
    ServiceTest2_verify_testpod1_multidimarray(v->s5, 3, 3, gen.get_uint32());
    uint32_t s6_seed = gen.get_uint32();
    ServiceTest2_verify_testpod1_multidimarray(v->s6, s6_seed % 6, s6_seed % 3, s6_seed);

    ASSERT_TRUE(v->s7);
    ASSERT_EQ(v->s7->size(), 1);
    com::robotraconteur::testing::TestService3::testpod1 s7_1 = RRPodArrayToScalar(v->s7->front());
    ServiceTest2_verify_testpod1(s7_1, gen.get_uint32());

    ASSERT_TRUE(v->s8);
    ASSERT_EQ(v->s8->size(), 2);
    ServiceTest2_verify_testpod1_array(v->s8->front(), 2, gen.get_uint32());
    ServiceTest2_verify_testpod1_array(*(++v->s8->begin()), 4, gen.get_uint32());

    ASSERT_TRUE(v->s9);
    ASSERT_EQ(v->s9->size(), 2);
    ServiceTest2_verify_testpod1_multidimarray(v->s9->front(), 2, 3, gen.get_uint32());
    ServiceTest2_verify_testpod1_multidimarray(*(++v->s9->begin()), 4, 5, gen.get_uint32());

    com::robotraconteur::testing::TestService3::testpod1 v10 =
        RRPodArrayToScalar(rr_cast<RRPodArray<com::robotraconteur::testing::TestService3::testpod1> >(v->s10));
    ServiceTest2_verify_testpod1(v10, gen.get_uint32());

    ServiceTest2_verify_testpod1_array(
        rr_cast<RRPodArray<com::robotraconteur::testing::TestService3::testpod1> >(v->s11), 3, gen.get_uint32());

    ServiceTest2_verify_testpod1_multidimarray(v->s12, 2, 2, gen.get_uint32());

    ASSERT_TRUE(v->s13);
    RR_INTRUSIVE_PTR<RRList<RRValue> > s13 = rr_cast<RRList<RRValue> >(v->s13);
    ASSERT_EQ(s13->size(), 1);
    ServiceTest2_verify_testpod1(
        rr_cast<RRPodArray<com::robotraconteur::testing::TestService3::testpod1> >(s13->front())->at(0),
        gen.get_uint32());

    ASSERT_TRUE(v->s14);
    RR_INTRUSIVE_PTR<RRList<RRValue> > s14 = rr_cast<RRList<RRValue> >(v->s14);
    ASSERT_EQ(s14->size(), 2);
    ServiceTest2_verify_testpod1_array(
        rr_cast<RRPodArray<com::robotraconteur::testing::TestService3::testpod1> >(s14->front()), 3, gen.get_uint32());
    ServiceTest2_verify_testpod1_array(
        rr_cast<RRPodArray<com::robotraconteur::testing::TestService3::testpod1> >(*(++s14->begin())), 5,
        gen.get_uint32());

    ASSERT_TRUE(v->s15);
    RR_INTRUSIVE_PTR<RRList<RRValue> > s15 = rr_cast<RRList<RRValue> >(v->s15);
    ASSERT_EQ(s15->size(), 2);
    ServiceTest2_verify_testpod1_multidimarray(
        rr_cast<RRPodMultiDimArray<com::robotraconteur::testing::TestService3::testpod1> >(s15->front()), 7, 2,
        gen.get_uint32());

    ServiceTest2_verify_testpod1_multidimarray(*(++s15->begin()), 5, 1, gen.get_uint32());

    ServiceTest2_verify_transform(v->t1, gen.get_uint32());

    ServiceTest2_verify_transform_array(v->t2, 4, gen.get_uint32());
    ServiceTest2_verify_transform_multidimarray(v->t3, 2, 4, gen.get_uint32());

    /*ServiceTest2_verify_transform_array(rr_cast<RRNamedArray< com::robotraconteur::testing::TestService3::transform>
    >(v->t4), 10, gen.get_uint32()); ServiceTest2_verify_transform_multidimarray(rr_cast<RRNamedMultiDimArray<
    com::robotraconteur::testing::TestService3::transform> >(v->t5), 6, 5, gen.get_uint32());
    */

    gen.get_uint32();
    gen.get_uint32();

    ASSERT_TRUE(v->t6);
    ASSERT_EQ(v->t6->size(), 1);
    com::robotraconteur::testing::TestService3::transform t6_1 = RRNamedArrayToScalar(v->t6->front());
    ServiceTest2_verify_transform(t6_1, gen.get_uint32());

    ASSERT_TRUE(v->t7);
    ASSERT_EQ(v->t7->size(), 2);
    ServiceTest2_verify_transform_array(v->t7->front(), 4, gen.get_uint32());
    ServiceTest2_verify_transform_array(*(++v->t7->begin()), 4, gen.get_uint32());

    ASSERT_TRUE(v->t8);
    ASSERT_EQ(v->t8->size(), 2);
    ServiceTest2_verify_transform_multidimarray(v->t8->front(), 2, 4, gen.get_uint32());
    ServiceTest2_verify_transform_multidimarray(*(++v->t8->begin()), 2, 4, gen.get_uint32());

    /*if (!v->t9) throw std::runtime_error("");
    RR_INTRUSIVE_PTR<RRList<RRValue> > t9 = rr_cast<RRList<RRValue> >(v->t9);
    if (t9->list.size() != 1) throw std::runtime_error("");
    ServiceTest2_verify_transform((*rr_cast<RRNamedArray<com::robotraconteur::testing::TestService3::transform>
    >(t9->list[0]))[0], gen.get_uint32());

    if (!v->t10) throw std::runtime_error("");
    RR_INTRUSIVE_PTR<RRList<RRValue> > t10 = rr_cast<RRList<RRValue> >(v->t10);
    if (t10->list.size() != 2) throw std::runtime_error("");
    ServiceTest2_verify_transform_array(rr_cast<RRNamedArray<com::robotraconteur::testing::TestService3::transform>
    >(t10->list[0]), 3, gen.get_uint32());
    ServiceTest2_verify_transform_array(rr_cast<RRNamedArray<com::robotraconteur::testing::TestService3::transform>
    >(t10->list[1]), 5, gen.get_uint32());

    if (!v->t11) throw std::runtime_error("");
    RR_INTRUSIVE_PTR<RRList<RRValue> > t11 = rr_cast<RRList<RRValue> >(v->t11);
    if (t11->list.size() != 2) throw std::runtime_error("");
    ServiceTest2_verify_transform_multidimarray(rr_cast<RRNamedMultiDimArray<com::robotraconteur::testing::TestService3::transform>
    >(t11->list[0]), 7, 2, gen.get_uint32());
    ServiceTest2_verify_transform_multidimarray(rr_cast<RRNamedMultiDimArray<com::robotraconteur::testing::TestService3::transform>
    >(t11->list[1]), 5, 1, gen.get_uint32());*/
}

RR_INTRUSIVE_PTR<RRPodArray<com::robotraconteur::testing::TestService3::testpod1> > ServiceTest2_fill_testpod1_array(
    size_t s, uint32_t seed)
{
    ServiceTest2_test_sequence_gen gen(seed);

    RR_INTRUSIVE_PTR<RRPodArray<com::robotraconteur::testing::TestService3::testpod1> > o =
        AllocateEmptyRRPodArray<com::robotraconteur::testing::TestService3::testpod1>(0);
    for (size_t i = 0; i < s; i++)
    {
        com::robotraconteur::testing::TestService3::testpod1 s1;
        ServiceTest2_fill_testpod1(s1, gen.get_uint32());
        o->GetStorageContainer().push_back(s1);
    }
    return o;
}

void ServiceTest2_verify_testpod1_array(
    const RR_INTRUSIVE_PTR<RRPodArray<com::robotraconteur::testing::TestService3::testpod1> > v, size_t s,
    uint32_t seed)
{
    ASSERT_TRUE(v);

    ServiceTest2_test_sequence_gen gen(seed);

    ASSERT_EQ(v->size(), s);

    for (size_t i = 0; i < s; i++)
    {
        ServiceTest2_verify_testpod1(v->at(i), gen.get_uint32());
    }
}

RR_INTRUSIVE_PTR<RRPodMultiDimArray<com::robotraconteur::testing::TestService3::testpod1> >
ServiceTest2_fill_testpod1_multidimarray(size_t m, size_t n, uint32_t seed)
{
    RR_INTRUSIVE_PTR<RRPodMultiDimArray<com::robotraconteur::testing::TestService3::testpod1> > o =
        AllocateEmptyRRPodMultiDimArray<com::robotraconteur::testing::TestService3::testpod1>();
    std::vector<uint32_t> dims;
    dims.push_back(m);
    dims.push_back(n);
    o->Dims = VectorToRRArray<uint32_t>(dims);
    o->PodArray = ServiceTest2_fill_testpod1_array(m * n, seed);
    return o;
}
void ServiceTest2_verify_testpod1_multidimarray(
    const RR_INTRUSIVE_PTR<RRPodMultiDimArray<com::robotraconteur::testing::TestService3::testpod1> > v, size_t m,
    size_t n, uint32_t seed)
{
    ASSERT_TRUE(v);
    ASSERT_EQ(v->Dims->size(), 2);
    ASSERT_EQ((*v->Dims)[0], m);
    ASSERT_EQ((*v->Dims)[1], n);
    ServiceTest2_verify_testpod1_array(v->PodArray, m * n, seed);
}

void ServiceTest2_verify_testpod1_multidimarray(const RR_INTRUSIVE_PTR<RRValue> v, size_t m, size_t n, uint32_t seed)
{
    RR_INTRUSIVE_PTR<RRPodArray<com::robotraconteur::testing::TestService3::testpod1> > v1 =
        RR_DYNAMIC_POINTER_CAST<RRPodArray<com::robotraconteur::testing::TestService3::testpod1> >(v);
    if (v1 && n == 1)
    {
        ServiceTest2_verify_testpod1_array(v1, m, seed);
    }
    else
    {
        ServiceTest2_verify_testpod1_multidimarray(
            rr_cast<RRPodMultiDimArray<com::robotraconteur::testing::TestService3::testpod1> >(v), m, n, seed);
    }
}

void ServiceTest2_fill_transform(com::robotraconteur::testing::TestService3::transform& v, uint32_t seed)
{
    ServiceTest2_test_sequence_gen gen(seed);
    for (size_t i = 0; i < 7; i++)
        v.a[i] = gen.get_double();
}

void ServiceTest2_verify_transform(const com::robotraconteur::testing::TestService3::transform& v, uint32_t seed)
{
    ServiceTest2_test_sequence_gen gen(seed);
    for (size_t i = 0; i < 7; i++)
    {
        EXPECT_EQ(v.a[i], gen.get_double()) << " at index " << i;
    }
}

RR_INTRUSIVE_PTR<RRNamedArray<com::robotraconteur::testing::TestService3::transform> >
ServiceTest2_fill_transform_array(size_t s, uint32_t seed)
{
    ServiceTest2_test_sequence_gen gen(seed);

    RR_INTRUSIVE_PTR<RRNamedArray<com::robotraconteur::testing::TestService3::transform> > o =
        AllocateEmptyRRNamedArray<com::robotraconteur::testing::TestService3::transform>(s);
    for (size_t i = 0; i < s; i++)
    {
        ServiceTest2_fill_transform((*o)[i], gen.get_uint32());
    }
    return o;
}

void ServiceTest2_verify_transform_array(
    const RR_INTRUSIVE_PTR<RRNamedArray<com::robotraconteur::testing::TestService3::transform> > v, size_t s,
    uint32_t seed)
{
    ASSERT_TRUE(v);

    ServiceTest2_test_sequence_gen gen(seed);

    ASSERT_EQ(v->size(), s);

    for (size_t i = 0; i < s; i++)
    {
        ServiceTest2_verify_transform((*v)[i], gen.get_uint32());
    }
}

RR_INTRUSIVE_PTR<RRNamedMultiDimArray<com::robotraconteur::testing::TestService3::transform> >
ServiceTest2_fill_transform_multidimarray(size_t m, size_t n, uint32_t seed)
{
    RR_INTRUSIVE_PTR<RRNamedMultiDimArray<com::robotraconteur::testing::TestService3::transform> > o =
        AllocateEmptyRRNamedMultiDimArray<com::robotraconteur::testing::TestService3::transform>();
    std::vector<uint32_t> dims;
    dims.push_back(m);
    dims.push_back(n);
    o->Dims = VectorToRRArray<uint32_t>(dims);
    o->NamedArray = ServiceTest2_fill_transform_array(m * n, seed);
    return o;
}

void ServiceTest2_verify_transform_multidimarray(
    const RR_INTRUSIVE_PTR<RRNamedMultiDimArray<com::robotraconteur::testing::TestService3::transform> > v, size_t m,
    size_t n, uint32_t seed)
{
    ASSERT_TRUE(v);
    ASSERT_EQ(v->Dims->size(), 2);
    ASSERT_EQ((*v->Dims)[0], m);
    ASSERT_EQ((*v->Dims)[1], n);
    ServiceTest2_verify_transform_array(v->NamedArray, m * n, seed);
}

void ServiceTest2_verify_transform_multidimarray(const RR_INTRUSIVE_PTR<RRValue> v, size_t m, size_t n, uint32_t seed)
{
    RR_INTRUSIVE_PTR<RRNamedArray<com::robotraconteur::testing::TestService3::transform> > v1 =
        RR_DYNAMIC_POINTER_CAST<RRNamedArray<com::robotraconteur::testing::TestService3::transform> >(v);
    if (v1 && n == 1)
    {
        ServiceTest2_verify_transform_array(v1, m, seed);
    }
    else
    {
        ServiceTest2_verify_transform_multidimarray(
            rr_cast<RRNamedMultiDimArray<com::robotraconteur::testing::TestService3::transform> >(v), m, n, seed);
    }
}

RR_SHARED_PTR<Pipe<RR_INTRUSIVE_PTR<RRArray<int32_t> > > > testroot3_impl::get_p1() { return p1; }
void testroot3_impl::set_p1(const RR_SHARED_PTR<Pipe<RR_INTRUSIVE_PTR<RRArray<int32_t> > > >& value) { p1 = value; }

RR_SHARED_PTR<Pipe<RR_INTRUSIVE_PTR<RRArray<int32_t> > > > testroot3_impl::get_p2() { return p2; }
void testroot3_impl::set_p2(const RR_SHARED_PTR<Pipe<RR_INTRUSIVE_PTR<RRArray<int32_t> > > >& value) { p2 = value; }

RR_SHARED_PTR<Pipe<RR_INTRUSIVE_PTR<RRMultiDimArray<int32_t> > > > testroot3_impl::get_p3() { return p3; }
void testroot3_impl::set_p3(const RR_SHARED_PTR<Pipe<RR_INTRUSIVE_PTR<RRMultiDimArray<int32_t> > > >& value)
{
    p3 = value;
}

RR_SHARED_PTR<Wire<RR_INTRUSIVE_PTR<RRArray<int32_t> > > > testroot3_impl::get_w1() { return w1; }
void testroot3_impl::set_w1(const RR_SHARED_PTR<Wire<RR_INTRUSIVE_PTR<RRArray<int32_t> > > >& value) { w1 = value; }

RR_SHARED_PTR<Wire<RR_INTRUSIVE_PTR<RRArray<int32_t> > > > testroot3_impl::get_w2() { return w2; }
void testroot3_impl::set_w2(const RR_SHARED_PTR<Wire<RR_INTRUSIVE_PTR<RRArray<int32_t> > > >& value) { w2 = value; }

RR_SHARED_PTR<Wire<RR_INTRUSIVE_PTR<RRMultiDimArray<int32_t> > > > testroot3_impl::get_w3() { return w3; }
void testroot3_impl::set_w3(const RR_SHARED_PTR<Wire<RR_INTRUSIVE_PTR<RRMultiDimArray<int32_t> > > >& value)
{
    w3 = value;
}

} // namespace RobotRaconteurTest
