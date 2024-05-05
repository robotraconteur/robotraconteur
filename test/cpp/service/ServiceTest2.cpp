#ifdef ROBOTRACONTEUR_USE_STDAFX
#include "stdafx.h"
#endif

#include "ServiceTest2.h"
#include "array_compare.h"
#include "service_test_utils.h"

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
        const double double_constants[] = {
            0.11808806176314335,    0.39707350606301617,    0.27114383032566935,   0.1816549980743133,
            0.9745821790407598,     0.3871963990399544,     0.23019466875895944,   0.9750733084525263,
            0.9082211077758257,     0.3458578243892674,     0.2145223031819451,    0.050577890701211214,
            0.6859014835404248,     0.13346491631693647,    0.31130161297472747,   0.4427049020013023,
            0.6668933913989237,     0.27386895592679106,    0.7935266595399671,    0.1801039408397278,
            0.24687809555578377,    0.14371795298956047,    0.6919432557030775,    0.26342660961205466,
            0.8297390075495092,     0.590707127717445,      0.31363110564252494,   0.9347078770102211,
            0.9092858094676379,     0.05007361982869851,    0.6839993121920985,    0.04537519346436425,
            0.9596513618649168,     0.014192218318528238,   0.3461518994899867,    0.2892061109297216,
            0.7852209512734595,     0.5512898498515767,     0.23876171449479588,   0.18603170199864816,
            0.9332549148364775,     0.7196381585823254,     0.11328783534529274,   0.28718907679131567,
            0.8943265682808665,     0.09528306977661194,    0.12076041011075966,   0.9093613227704904,
            0.6159424378241243,     0.46040246050201283,    0.10114403603396294,   0.3256022559352807,
            0.4345689432015529,     0.16526053043685363,    0.921377210764023,     0.047316086921034506,
            0.8124471820976614,     0.5549322662743057,     0.6372044012956544,    0.7384490314500013,
            0.08785073630126417,    0.34009451753816633,    0.1128188235298141,    0.7570689642925523,
            0.8888240249867422,     0.4793558825392765,     0.2873446224140068,    0.5812977408955284,
            0.12115017049089272,    0.027781953854400854,   0.2261478033219121,    0.5215829399443781,
            0.6689715427971396,     0.037255766439576954,   0.027369544851444938,  0.0012655162592882796,
            0.057718580810741105,   0.768571577589763,      0.40663466239871804,   0.37132551049394413,
            0.35689602073641835,    0.999288862894407,      0.45921716865448736,   0.8858162825940926,
            0.5256617928380493,     0.10987414986308519,    0.2282318164593513,    0.9447877860501251,
            0.3916195407188612,     0.3162722195101684,     0.6022681762024559,    0.8968606183295518,
            0.9660981389203961,     0.8259051566799759,     0.9567596329789867,    0.10899477146391134,
            0.8686975579744052,     0.6460925454900462,     0.014139153158731599,  0.8236465472145591,
            0.14070749413750772,    0.45029851995863457,    0.49760400143002703,   0.4269219371696674,
            0.6497722752581501,     0.18891142683143125,    0.7842553099501023,    0.4567336175118104,
            0.41557010767259617,    0.5409386156760537,     0.8569918583692724,    0.6404373935353948,
            0.3979570017461407,     0.7638872113303937,     0.3143786258820297,    0.4124363084260697,
            0.7511608198176333,     0.9309621020613599,     0.3510817626656133,    0.4129906358226556,
            0.160132289651869,      0.9034249772852133,     0.29307932554939686,   0.6648217626783984,
            0.57370441847376,       0.25866833134255185,    0.9868393298504949,    0.7519952534740356,
            0.9125793884741256,     0.09414441028013487,    0.3698912686054433,    0.7697425935080573,
            0.818433838006258,      0.45387442546859913,    0.9540623871808379,    0.6546622392518656,
            0.33175597948205304,    0.3650812315404032,     0.2204262671755911,    0.2896249886230644,
            0.4359348975163234,     0.6874635995532687,     0.9898862923022991,    0.8447030875007443,
            0.556631665555674,      0.22535633122318266,    0.2962015363488314,    0.40919811529893524,
            0.35277813955676585,    0.9135649109049062,     0.1362136204679547,    0.014832030307457944,
            0.9786997284639897,     0.8831443107570999,     0.18693034847662915,   0.6306544158593178,
            0.7195346571103611,     0.12280632826845916,    0.09598120028079893,   0.41628580769894785,
            0.4563945117320106,     0.5453569722070474,     0.5409708243628841,    0.9548826130433853,
            0.73826674029996,       0.7606706691331442,     0.7451510370045543,    0.46905357694223615,
            0.38604069300230726,    0.24227549467990406,    0.36888260743435897,   0.7736266009784566,
            0.5599726292985049,     0.1638200476964048,     0.5231363926268443,    0.46476816047428493,
            0.7354540829476179,     0.7592160523397236,     0.904926665320885,     0.13659567661634864,
            0.3605347480789368,     0.8559898787924197,     0.05806361089284218,   0.3514516073731695,
            0.3200035963730826,     0.8130816300220727,     0.2441806071247825,    0.5094333382670222,
            0.3085154105289487,     0.8016697912649855,     0.489223980669399,     0.1271196388708592,
            0.45361959574589095,    0.4076021039211315,     0.21008200776692043,   0.6492824809428919,
            0.36355899831102345,    0.3700493597456158,     0.8804732140356909,    0.6838347468524664,
            0.6251817013921166,     0.8129669218885914,     0.5802559767397518,    0.7612778764685251,
            0.7743182342054986,     0.8813454629585609,     0.13901937602535086,   0.6441396609146538,
            0.0669814086333228,     0.7988834918175571,     0.21342150853610775,   0.48912606983409,
            0.6268962979617757,     0.3420705951554025,     0.644355248574381,     0.02474027233115894,
            0.9629261752485292,     0.32756595461616556,    0.6234896183759103,    0.8694620695988075,
            0.5339106063143038,     0.8138209300770278,     0.4605248816782116,    0.22035927729293459,
            0.46230821418688905,    0.040888157819212734,   0.9342201664439329,    0.49451931633162305,
            0.7828481989036491,     0.6308540812254672,     0.787338250006729,     0.8242296298141132,
            0.37842153129646017,    0.4037533058169085,     0.29363853429695086,   0.2616333890346294,
            0.021747079536094893,   0.07159932403819824,    0.0836252556663275,    0.6508931436148896,
            0.4696851495094455,     0.7780320192599322,     0.9056810238799952,    0.16581571523410876,
            0.07477643334310968,    0.08918832273632304,    0.3510030518197498,    0.8208879098716282,
            0.6730030327410184,     0.2056228302092813,     0.8589526402816096,    0.7661103309406245,
            0.10410779894694244,    0.2980240916305603,     0.08309278020221389,   0.16892923264993776,
            0.1673649190856814,     0.9902665970726163,     0.3958281017993982,    0.6357761365697472,
            0.0004985278107018098,  0.8934719592462411,     0.4433264015781644,    0.3060269838972648,
            0.5961031105262977,     0.19337740613989174,    0.6601304530774094,    0.3188815478943351,
            0.9591385952061902,     0.7487404392909567,     0.021452192187004893,  0.09677090814242906,
            0.9443022122893349,     0.06906514980446588,    0.47150949773110995,   0.5254661986191255,
            0.35885196872594927,    0.6149642641579985,     0.43424327780540106,   0.22788025977293125,
            0.49038872275875656,    0.7629112500519988,     0.39008097446204415,   0.6305574278747439,
            0.8660069529571268,     0.09187729927486732,    0.10263894893766723,   0.9867782984008415,
            0.6162178104927842,     0.5865318376745987,     0.7492617148781313,    0.3109228268448453,
            0.4066052601781911,     0.8077848008927694,     0.2339009135047284,    0.8946378393181584,
            0.46618372347474724,    0.8228301884287071,     0.4963502528255559,    0.10742931804428801,
            0.3790151347468922,     0.3556483613732718,     0.7454368606830942,    0.29581192451899996,
            0.23435803066694416,    0.5567984286403972,     0.9944409203657257,    0.40931036913420094,
            0.6117874287675803,     0.6747592372520032,     0.5883710356352353,    0.5879986008137923,
            0.9894877844299926,     0.24498970716199964,    0.43472097669493615,   0.5658599957342948,
            0.6785955276344691,     0.32468469937473055,    0.8120952606970443,    0.9665109134432694,
            0.3633211690155298,     0.8776842975299799,     0.6166943967353135,    0.5264436393190735,
            0.43461378380896143,    0.31963622439415607,    0.5661547056518071,    0.2799550324270562,
            0.5225294051645226,     0.21965894465606783,    0.8745496251589806,    0.355155931673077,
            0.9785028143409555,     0.549180958773751,      0.04728436109850853,   0.4408505961142082,
            0.14266589754619374,    0.12806936439311334,    0.8422508564579877,    0.30560080277060553,
            0.28786165870961034,    0.5910301159521268,     0.04620033959431569,   0.26425608337438955,
            0.7018316541730308,     0.5339492648974965,     0.6430291468345456,    0.6907932037900721,
            0.6245286197249267,     0.37094536018183155,    0.2396910270788618,    0.39124245712771666,
            0.5541688454847093,     0.8713377215893969,     0.05938742665581742,   0.01241460136353345,
            0.879517562854424,      0.37477715983213533,    0.07192681026285175,   0.9241059349680454,
            0.9385300558643621,     0.4421320115952644,     0.7237581457744204,    0.5911461221297237,
            0.5052348293128005,     0.768573703207428,      0.5428363230360528,    0.8136325092440324,
            0.08178473157166011,    0.08127441625255183,    0.16118756878439533,   0.5324614938478965,
            0.7504733592403365,     0.8395312059157386,     0.006502769647036333,  0.681805185167828,
            0.55062774394151,       0.7720599468655934,     0.7108274856720785,    0.5029284824736332,
            0.7296136311360604,     0.18611079483260184,    0.3214894555659986,    0.5497521448934527,
            0.612312546478986,      0.2567665793415739,     0.18063694335767433,   0.6529415469066755,
            0.428220857347304,      0.27033865701859017,    0.6301539033537722,    0.8954399656398911,
            0.8620791238652082,     0.358883885983992,      0.6565637660364451,    0.03899031230829075,
            0.5992254226976144,     0.676891025853718,      0.2762482994500366,    0.7702445697240418,
            0.7883253815379588,     0.20393956323595241,    0.9626316053331848,    0.6335084312247247,
            0.2576140594017752,     0.05256518359732176,    0.9159962132008892,    0.2513156443590282,
            0.582613297989311,      0.09486499892714495,    0.5089995503110107,    0.023105697168018247,
            0.6934221576913613,     0.6447280482700932,     0.9222950845931368,    0.4908271777816723,
            0.4012739483720401,     0.5949023418177657,     0.40571994486797613,   0.6467215135683724,
            0.09603587913633727,    0.5880987073700991,     0.9475098122497913,    0.17017882359144798,
            0.7611288126744469,     0.7800004705344918,     0.9805157090633413,    0.49239767506511134,
            0.525621028628471,      0.5955097911630189,     0.8382565015317662,    0.8521067587896233,
            0.016174722075197212,   0.9622504576974049,     0.41305828807482736,   0.9696375267344141,
            0.9570049439265458,     0.3725469311180356,     0.7217083924350551,    0.13550047029202483,
            0.048554001245699197,   0.611121478355831,      0.11699009554859885,   0.2549105305320696,
            0.8353167504742361,     0.00855534601782959,    0.7420952212399781,    0.9306301641377172,
            0.2582575255846541,     0.40373052563148093,    0.4724456211978053,    0.48473950091287354,
            0.13641662775945151,    0.892900261781099,      0.9134971038444577,    0.8124550194246825,
            0.021377019307288747,   0.5371219453187059,     0.06616515667907086,   0.8392982081481858,
            0.7878164775264851,     0.6038019826468104,     0.5407279567805809,    0.8334685679598667,
            0.8828200130301792,     0.2892161433059288,     0.6363827534826434,    0.9111080426139715,
            0.2216791093450039,     0.8706828446080086,     0.058320171542013854,  0.0017885147201338603,
            0.5374100491309518,     0.9191921406673188,     0.9004840238169437,    0.21546548822697664,
            0.21521643757150222,    0.4135650192826279,     0.41735904098918497,   0.3680566475553957,
            0.5361010291026598,     0.2060745133027586,     0.6466786414844194,    0.2555059150169803,
            0.3042398408694681,     0.13599973293246204,    0.5643396250890071,    0.49022960983678765,
            0.886775268876995,      0.6808574378306889,     0.34886912190621966,   0.05910306999475168,
            0.2644985648004752,     0.43248727337474,       0.4338321881250695,    0.3379412208548226,
            0.7314920530536205,     0.38747717497997813,    0.43788062299958863,   0.6252405062326328,
            0.4228607506564407,     0.11043479369103382,    0.8600310571909089,    0.3238100345473882,
            0.6118121488628919,     0.5136309395371775,     0.7134454351295275,    0.36297050974034517,
            0.7863582181437382,     0.9394021060565931,     0.19272040719334027,   0.8892728719954666,
            0.3739539578009009,     0.40828732660228084,    0.7710108251456727,    0.3139143928322433,
            0.5231143486040469,     0.5884727079237009,     0.40292448955304494,   0.31229128739915457,
            0.008103390161475987,   0.5951526889855415,     0.08913610806878203,   0.9945373885500453,
            0.9252330715818449,     0.02261280756297268,    0.7055952792909543,    0.011887107429877974,
            0.06471394323665669,    0.3073821113611167,     0.7209790243952364,    0.9890927427895614,
            0.2829090159211163,     0.38987754724421464,    0.4054688299855227,    0.020066476289379187,
            0.2626615925901822,     0.5590002234109553,     0.9498023765681355,    0.5973420301849269,
            0.8980548256172753,     0.5616962292771038,     0.04230268859383657,   0.30308051212676623,
            0.35930920515841924,    0.6036950051401851,     0.384968274852718,     0.312308774242101,
            0.3692273904078601,     0.3172006270594643,     0.7226276523499817,    0.20439047525077614,
            0.6357508706809525,     0.3577069534295817,     0.9492404541407644,    0.3735290617972227,
            0.5119756913679082,     0.9664339494183619,     0.6364328990394912,    0.2846721192618006,
            0.010964760898709014,   0.49772356258980777,    0.46988334199167014,   0.4279494746262865,
            0.22623302281987934,    0.3187704225374557,     0.2093231719134918,    0.7593963353959611,
            0.4625524803450076,     0.8468487813319907,     0.30206910755651506,   0.3380613633549854,
            0.39455415467782573,    0.7019645750419382,     0.947065639026993,     0.605333850442553,
            0.11292456377093496,    0.4721597294029506,     0.6477029655985921,    0.011172089213592806,
            0.44983047519810904,    0.38910305768154374,    0.18975209868914733,   0.8165156644762526,
            0.16011293264660797,    0.8835426554822109,     0.5429230439593155,    0.8489041844295836,
            0.12524355886296823,    0.3400126851146301,     0.5384281394899175,    0.4771459907557558,
            0.06033129106902324,    0.38125744632632086,    0.7131492958924768,    0.8136329955769709,
            0.16376611177446598,    0.0022631081640877015,  0.18366496166829394,   0.11032363663544142,
            0.6709984131227905,     0.591537150789633,      0.3965894462978884,    0.4462240652526217,
            0.6489369153602685,     0.15627792060804246,    0.5828561460304416,    0.8021362221478929,
            0.46969278894375666,    0.9270688928839373,     0.48232595220072527,   0.3030139868257481,
            0.26417159530156886,    0.4771428897714962,     0.03133356850609259,   0.8009407727149894,
            0.9277491659668051,     0.532200504678513,      0.6829142967963835,    0.09841692883850661,
            0.26374551811487834,    0.9018313727579701,     0.11942764786327098,   0.7166639736889414,
            0.5132679723575806,     0.5338325173566333,     0.7805635679525501,    0.7656351819693812,
            0.6204226093833011,     0.1850875995156398,     0.9073688196259678,    0.8233469506736163,
            0.2658230025797421,     0.13501660774180047,    0.026158102821365548,  0.7538369816313552,
            0.4373271076245152,     0.2559079732442736,     0.44318061196793557,   0.18874478556217944,
            0.9001217870246204,     0.02510911772630009,    0.629531681380815,     0.27491250373270626,
            0.7652802588216262,     0.2342039700077978,     0.1905300339535071,    0.9241302782770972,
            0.04821274718469781,    0.2788030830433196,     0.7247337501517115,    0.6850118669022752,
            0.9866938645400655,     0.1446727020334575,     0.2597335525496386,    0.7915445798552591,
            0.863576504277694,      0.3511934296903042,     0.7049169116788868,    0.6914137749844148,
            0.92883026483981,       0.11672137780963687,    0.048423681326212,     0.18739398181964795,
            0.3717443471715435,     0.3289046003213417,     0.9558190128626967,    0.20714383936144587,
            0.3714676810861953,     0.4392060538337378,     0.984907109132837,     0.9751413322207008,
            0.015264435061731918,   0.8991052688073077,     0.7580255717541848,    0.28082798296481626,
            0.6179452769767477,     0.9311574279329193,     0.5114546595067491,    0.25643598931795397,
            0.38961615906891567,    0.8845533119972361,     0.4662208803328106,    0.8969664884345281,
            0.2010493976166119,     0.32510086587086084,    0.4860247268592096,    0.18103893281063999,
            0.8318100859577969,     0.7508416834751045,     0.44478094716908256,   0.49254435499316973,
            0.8482475369094777,     0.7819299413149919,     0.5624486431318817,    0.8643988258521559,
            0.7397756253502105,     0.415206714651393,      0.6352217336497741,    0.933368986711593,
            0.7749550979908477,     0.36235331034037,       0.5998729853921307,    0.20554147883025997,
            0.4796964376513627,     0.5430472244564134,     0.6611784861824505,    0.649118006557437,
            0.0894991374510038,     0.9332151757938915,     0.8518028707789924,    0.6875168603250106,
            0.40224345420985697,    0.43613323910022517,    0.5764611501317158,    0.5110203090553879,
            0.11778146782866938,    0.2750499555771603,     0.9204412228511986,    0.5982084604806762,
            0.49465927285377154,    0.23843873632115298,    0.14799321572142765,   0.5249347049449488,
            0.676157317555957,      0.07035946204215415,    0.7955851364803835,    0.31302775703764785,
            0.8603218104066013,     0.5653696525722016,     0.045118422639226274,  0.22217852731666943,
            0.04206878707044237,    0.9267416745378635,     0.4484419159416485,    0.772649961781817,
            0.04313723247025614,    0.5833126281497195,     0.8920846214147803,    0.32629738035933775,
            0.834026700320939,      0.8515113953422444,     0.34647394864933556,   0.9312457566253319,
            0.9288110303011535,     0.911460819066421,      0.5061654217295981,    0.22126802227839937,
            0.11011894041989267,    0.07982945058915258,    0.6014129884740537,    0.21137697706366954,
            0.5050947780080455,     0.9368090455974944,     0.8095629063653647,    0.41897458881848004,
            0.18376286120576935,    0.3746585886118071,     0.0514557303230222,    0.6149285340353486,
            0.7175000558317168,     0.24323239233243676,    0.8995782262049171,    0.4382141996921126,
            0.9689105908967175,     0.5567083081333106,     0.21824050358884228,   0.3041010071253871,
            0.8578257860572073,     0.6874809725039885,     0.2567360177759136,    0.09289556655791065,
            0.601394757441654,      0.3905707847343465,     0.6269622665451811,    0.18548477615937786,
            0.9831751317670202,     0.9293763674710948,     0.2883425128582334,    0.9447685757268528,
            0.6668718639718363,     0.08860781220164293,    0.27866294123711366,   0.018152249861373426,
            0.7769442305211107,     0.22290755311883403,    0.7110899525064801,    0.29545667903194384,
            0.21042804961990236,    0.6696809283826698,     0.9429756296173233,    0.9858086538513796,
            0.3365876016010937,     0.09820421262945267,    0.9721375652021342,    0.14524893899638558,
            0.32051082913121176,    0.3790811793383936,     0.8081682727177745,    0.9041790482728926,
            0.9072273155669196,     0.7361854451937497,     0.01700535777444878,   0.3462198523221117,
            0.20133171171444586,    0.6552903602781011,     0.9849045806546715,    0.10300029387803,
            0.0343363381484898,     0.29587874639823764,    0.27247339500700607,   0.8122815814942925,
            0.697282148600555,      0.8019014337248156,     0.32110392002997645,   0.5435989135525243,
            0.25336273564042755,    0.07511506630216169,    0.5771842509539693,    0.9494621082892432,
            0.05181408451256031,    0.2255346787482062,     0.4052327551356675,    0.07002191149959391,
            0.7497381003217017,     0.5287801105987655,     0.08701267341844254,   0.05728977368575816,
            0.8453833084072174,     0.7370291032527608,     0.1974931159403368,    0.4410433468592112,
            0.6561633530650004,     0.961638637866387,      0.400232474605695,     0.7881918099093638,
            0.971989792765586,      0.3421476666378508,     0.21695082711274027,   0.9662372712701265,
            0.5587525738781545,     0.23411049934669048,    0.21927272214165738,   0.25875228093183666,
            0.39125949901194423,    0.3722976464384721,     0.5799971412857879,    0.7924928130616936,
            0.34733016920823634,    0.39825231686321305,    0.47353011301205017,   0.8665153254160903,
            0.8170832171132127,     0.7186757014161756,     0.36797525340135495,   0.7202421373604678,
            0.22742374060848236,    0.05237345131925608,    0.7815250696637724,    0.537865375063918,
            0.05877508249063901,    0.3388461477231621,     0.010378113246003129,  0.7635891851459585,
            0.26189296267345497,    0.8921563062382811,     0.5704929121896244,    0.8921102439856884,
            0.8631244998126665,     0.21437254249809667,    0.17325378430506577,   0.4413631319356942,
            0.16723067628477095,    0.05844671650583211,    0.40713674036451775,   0.5478318535374214,
            0.7507637054462191,     0.5518810990780751,     0.3531061208308618,    0.18367405870536502,
            0.2128763730880997,     0.5545834436303758,     0.05712173937328735,   0.9396878932616111,
            0.8429071406736056,     0.4799604112179615,     0.19740273089087257,   0.9598896492166362,
            0.5385708537121373,     0.3020694526890365,     0.5970228839519559,    0.03311460687787404,
            0.21582178251860484,    0.2723664867630228,     0.73377974515052,      0.8759617868844667,
            0.8232976308651004,     0.03700306133178122,    0.38716654259545924,   0.771694669657325,
            0.7642610346956655,     0.8731197874627835,     0.4189921573136275,    0.7403621716160607,
            0.591107629855735,      0.4963269450394223,     0.9929002976618904,    0.5051598559514098,
            0.8762214154952497,     0.26345798260901243,    0.8734010724298948,    0.33930779461274674,
            0.20455454107155213,    0.27202864323058396,    0.8379340605826956,    0.1577136422613713,
            0.3497420070274553,     0.6047774698098963,     0.1346439907993906,    0.032487045996263286,
            0.42192915900170036,    0.5819629347562012,     0.5583299171311906,    0.9465277347015835,
            0.16478091262264427,    0.5694667944642635,     0.30707535620818993,   0.49819159245955047,
            0.792023510165612,      0.13850939867398881,    0.7765136993634244,    0.7757240424754629,
            0.7027089483498431,     0.7576024695597564,     0.6780908097829627,    0.8115402968381701,
            0.04230081680416986,    0.598117466699103,      0.25247094709709106,   0.21789026412046208,
            0.5596450902785303,     0.27924138534471954,    0.5836857352686641,    0.3101052990547787,
            0.8082219755862983,     0.4002538473011129,     0.4418030617217851,    0.3355220622939905,
            0.6418736967123997,     0.04002086572356123,    0.2697100442778948,    0.05219412234533882,
            0.9682244357590644,     0.7524399392871339,     0.3150178009344061,    0.6841813700641486,
            0.16121920066904383,    0.7166309590400565,     0.8239736649769723,    0.13056426951089095,
            0.5382711196638462,     0.2918655247296591,     0.06361986731678793,   0.4997101653793863,
            0.7160735980905536,     0.6883210348229364,     0.29028961949956644,   0.32840692839408236,
            0.9867303846619755,     0.682667858295624,      0.9999344111044323,    0.9093754505051961,
            0.13987442946833872,    0.6717610673233682,     0.870856346750566,     0.789550023057975,
            0.8912727758158918,     0.8620626807962146,     0.6647804690817651,    0.7203747981083668,
            0.9445149767375695,     0.3485298532125519,     0.7475414127294949,    0.35630027486927696,
            0.45177948270215307,    0.40537211735323664,    0.289655671324311,     0.3523591255345878,
            0.658299152446759,      0.5723421077235467,     0.32340677976336263,   0.608024402599978,
            0.15065571536169664,    0.9454653804613852,     0.6639779299499051,    0.9448532211731823,
            0.16446005174859324,    0.8134121698691537,     0.5902108167131438,    0.17269140068719557,
            0.47092834327354394,    0.8024210549243069,     0.1835032794252719,    0.22855832354810224,
            0.2956629333405023,     0.11277119125261792,    0.3708014084241472,    0.1771214316910451,
            0.0613256329734867,     0.15535562508038425,    0.14643768124284207,   0.7113786632051002,
            0.6816121941010977,     0.25427440589044625,    0.9105039942575283,    0.2626275949650906,
            0.8743248467589826,     0.17775109798408306,    0.8233144213044139,    0.4493045862148761,
            0.9289574215563192,     0.41246605757394894,    0.46305006898485435,   0.09465486382456179,
            0.14138710454789294,    0.5995265742175495,     0.1895915026442926,    0.7703539240047068,
            0.39460296525219274,    0.09298348697370684,    0.05572666816234695,   0.5768972850553533,
            0.12143304436502067,    0.22177355070763383,    0.8456161476367566,    0.9950862202917009,
            0.8762989615513064,     0.12973237372807023,    0.5096093345704372,    0.29856204365830596,
            0.13405184341373155,    0.3508844672598084,     0.2715606876631014,    0.5920439335131269,
            0.5203008049176737,     0.2645334446296833,     0.9961756572030332,    0.6947528615717281,
            0.2656923389499519,     0.800219418664097,      0.5253090362395398,    0.9331940049262183,
            0.20702393231987304,    0.5133355601340401,     0.30525838587897336,   0.12777611264004018,
            0.5699751271320035,     0.2552404635084413,     0.07162322795740794,   0.7647529845336876,
            0.672224159186855,      0.7739403827398212,     0.1595963822742964,    0.16503380393425038,
            0.2341066103208712,     0.25413513980031965,    0.3201946129772437,    0.7886082407666994,
            0.30279949786360616,    0.5458688386567395,     0.27593536647410677,   0.6199644196256926,
            0.9769056462226836,     0.0690346955004767,     0.7356449880772845,    0.5680675438627516,
            0.7885194500613456,     0.5745253694880936,     0.09516979118464852,   0.41095363718596567,
            0.5247806528166639,     0.06889327257344358,    0.10220677524749744,   0.46156592611062375,
            0.9941755888582899,     0.6575323541511688,     0.8903014197576481,    0.021566699717169846,
            0.5854098813021632,     0.024849866692510547,   0.7321832185208413,    0.9398908189062314,
            0.161605146097898,      0.2923692981350924,     0.29683177201663824,   0.888225972664728,
            0.4952168768403802,     0.9347639362844131,     0.8957212359907937,    0.33057629726945703,
            0.6980354931964999,     0.4623267600384875,     0.4295381149626404,    0.763010526632678,
            0.603341430953062,      0.7821487281285144,     0.9834613693375158,    0.12016505491050056,
            0.9589619378542033,     0.4665205603498396,     0.2331460897326887,    0.5503369340809909,
            0.32026322926348005,    0.1281583012900599,     0.24177065071469128,   0.8703669385484409,
            0.014422433798426337,   0.793121808695591,      0.9887371052863829,    0.27674894683934126,
            0.3601826273412466,     0.6612945952139901,     0.7978130570800702,    0.7680528221246946,
            0.9683709178140776,     0.48495751622276795,    0.03602089758790905,   0.6294163266719937,
            0.9471301945981717,     0.710097092302267,      0.12870016302295506,   0.023377190448314322,
            0.10137820933379493,    0.8781540719257389,     0.8141122873538388,    0.11184975599118285,
            0.08318427795620031,    0.28165385046275526,    0.8517555579123011,    0.8221724831015889,
            0.45468896351717525,    0.613370628079813,      0.7416285770432711,    0.6578158745550554,
            0.9242826184836967,     0.19823000884358288,    0.1946117703636946,    0.15748670161390943,
            0.041554014102250125,   0.17467695217063406,    0.536890983485062,     0.6051092503685662,
            0.22638709958870196,    0.2577118066365772,     0.47789381671870235,   0.40216971144124936,
            0.9081173281300887,     0.4323062424734797,     0.33331616108164186,   0.6387841843184672,
            0.34771928976438904,    0.25238005084111514,    0.0673507596143672,    0.7827784228217279,
            0.5486749171461384,     0.2686786993773266,     0.7216673320831933,    0.8521385622030841,
            0.4036556213101784,     0.05389955361271492,    0.8605727495121023,    0.2620183453126005,
            0.7428276631488362,     0.7499868891698707,     0.4751417615162302,    0.08078468464713362,
            0.4833549981351992,     0.29646612965330355,    0.7230809728254035,    0.0013587526739268219,
            0.6485697032808296,     0.8236726896984554,     0.48605546526841714,   0.5886948874045671,
            0.039866344675153154,   0.8772918848628517,     0.9278577278780504,    0.3533904891796865,
            0.526476660178618,      0.9840169017457732,     0.3914102913105054,    0.09552078871295433,
            0.4263214500389727,     0.4160116237030925,     0.4775880417453596,    0.7728455635232607,
            0.13485017351503592,    0.39924559821712424,    0.9352737505809616,    0.4797889211413007,
            0.34357765474066104,    0.8648071336289281,     0.813513159951602,     0.37306270264629016,
            0.4990443401952108,     0.9014805016325693,     0.32956878239317655,   0.2053848292855347,
            0.3525575775160579,     0.5176633807445933,     0.7516770606798547,    0.04562244478478217,
            0.1697806508231906,     0.593943897397884,      0.7363436395713012,    0.594900843719289,
            0.6911625891709269,     0.6526154357662239,     0.05973222546491319,   0.4538110117378529,
            0.9660242907014512,     0.38756108341096485,    0.43861132953360515,   0.06521149107238222,
            0.639911222345425,      0.7110133731361981,     0.33737462182563505,   0.920827379705587,
            0.4289816371072974,     0.07185369345591985,    0.06472721805646531,   0.9524089639989497,
            0.44624432954810545,    0.4963945823468069,     0.8395184978194854,    0.45481361137329523,
            0.23145947579975878,    0.3836691793079763,     0.041253631016087144,  0.7689487771662921,
            0.7843263106396721,     0.5921029375626075,     0.178462024863297,     0.38195102635650247,
            0.18393464903080758,    0.853978220418008,      0.16226466739788248,   0.6028903173288414,
            0.9144495364343216,     0.8495834813249012,     0.9989565942210943,    0.3982606756919106,
            0.49710529158564365,    0.8058155133515219,     0.6995545318333856,    0.8046146561850005,
            0.21531481161005384,    0.2660918678310139,     0.4963448864050486,    0.011219916681143527,
            0.7814034093507488,     0.9698350708617531,     0.8206367554151386,    0.0811353783080675,
            0.3048635443295752,     0.34035895649236947,    0.4741623827096034,    0.4755925001741895,
            0.23599971348225468,    0.5972830659522512,     0.44446389036036027,   0.18781147593917435,
            0.2852150593091246,     0.9247682052276058,     0.39022873535731395,   0.29999547645337565,
            0.23261722839790344,    0.4726943541336024,     0.32999969901504733,   0.4106989605368676,
            0.25987594226771926,    0.5733237991090476,     0.46743347859059914,   0.20734017822547657,
            0.8250933905245337,     0.8950083703449331,     0.9826330335178797,    0.2588455973558593,
            0.8848482397545162,     0.22425334548902476,    0.007114102341946094,  0.6178306237171123,
            0.2882679935056218,     0.004110606721978272,   0.3578514370324011,    0.937117502373018,
            0.8191362478327489,     0.5264503774680637,     0.7126310314599116,    0.725033035599427,
            0.6836460833678726,     0.9489555706663746,     0.24217381582170605,   0.9981285947519442,
            0.6885491304249269,     0.4083676847227724,     0.41476349129012857,   0.19828809994981234,
            0.08554267342931854,    0.2724756424933251,     0.6938317918015329,    0.14644679680150752,
            0.9188312971572159,     0.49378911499433087,    0.582784427728424,     0.8372117508389326,
            0.24040536728651163,    0.294899190156918,      0.7600285165968004,    0.1306238194294148,
            0.0475468072376124,     0.2974487598308012,     0.29112882855008715,   0.42002893918675976,
            0.2857520673654842,     0.47201924256805516,    0.2203772263861664,    0.5649892518642434,
            0.41960301968761426,    0.021473881707026243,   0.8267686083066739,    0.10934400430896418,
            0.2688685389783312,     0.27425166388781275,    0.5020029909044462,    0.2474929645284265,
            0.09668173589614204,    0.48362150833826734,    0.5214144537225854,    0.05779557946888281,
            0.34711514861747206,    0.44103471585161436,    0.19257946498362577,   0.06857816020547614,
            0.7895372691077953,     0.04804763170078852,    0.7658751154176736,    0.74824490448168,
            0.6560107291125247,     0.6299841841489916,     0.12680992071631447,   0.8980663961263227,
            0.13756442813749192,    0.6043579779445404,     0.15684452277827166,   0.06156868078255817,
            0.7600416796273484,     0.0041341338186680066,  0.46616097834364856,   0.353338545534724,
            0.5360763505364982,     0.23993508958618148,    0.46383924606953497,   0.43949438112491745,
            0.11406731638782297,    0.045103838284834885,   0.7981253345694256,    0.6012632962924179,
            0.23569683055647783,    0.13351735962171918,    0.6005229376866086,    0.86711390221398,
            0.5268355039292378,     0.16525694371415878,    0.6182720043990849,    0.5888447353429345,
            0.2615255713038652,     0.7149559980228649,     0.7500650589278361,    0.732435553882271,
            0.8498340899577493,     0.4323993025519053,     0.7524203478625255,    0.00926228750802971,
            0.43384304529061035,    0.466847231825109,      0.46020945246307177,   0.44590941202823486,
            0.7852062253288729,     0.06713230092025946,    0.7689787442422019,    0.031521215638755984,
            0.6856683373579755,     0.11715023177116335,    0.9477129117060692,    0.6103592592393331,
            0.9726436692624316,     0.8719805409274874,     0.9600124902875928,    0.7633157640954337,
            0.9031194006984042,     0.4117097655798424,     0.5917728902647283,    0.880696780078381,
            0.10076849583773773,    0.6440486856072943,     0.5578275563873039,    0.19923344200809012,
            0.729868957904753,      0.022465466357680097,   0.555717066085314,     0.04111507661864633,
            0.03990711913222722,    0.04232565979515879,    0.9913994787497808,    0.4237501411829311,
            0.472725602110709,      0.6500451579043571,     0.7974431296125448,    0.7921010448281448,
            0.75532316597476,       0.443662947185513,      0.868632168915875,     0.637953815996993,
            0.27687102768226457,    0.4844189010822929,     0.7619495590109543,    0.4988385920081573,
            0.5733691945233387,     0.594937884768482,      0.9796855921456933,    0.8707838701595241,
            0.13988820411508085,    0.8029451472278305,     0.836685754190935,     0.021052858917557704,
            0.2992704689336185,     0.3022074571287551,     0.7572899905699652,    0.4300839916838026,
            0.7707864486268577,     0.8673409467843307,     0.9434105103438207,    0.6326363671871539,
            0.6667845795407594,     0.34096992156059946,    0.707632243060371,     0.950964921015516,
            0.5800677495952639,     0.7511405441957287,     0.6516326902418639,    0.6007137599408454,
            0.4490037841618797,     0.8685940002941004,     0.23347001254834931,   0.2786319207646102,
            0.07699077865421378,    0.9058912618889178,     0.8019059430515363,    0.0221649395179927,
            0.7342096818030527,     0.10661336410019429,    0.6833093656955919,    0.323633706564883,
            0.821913546511289,      0.4876412538229209,     0.8663555957631257,    0.9784109123396604,
            0.494848107427869,      0.9485381134044084,     0.23466042866463965,   0.26879330230348275,
            0.40924541255648617,    0.024547492477307475,   0.8684653423970778,    0.40061932541866885,
            0.1331811266667069,     0.030309176671732496,   0.07872555954011373,   0.4144064580934682,
            0.13511750669157718,    0.17107649830858374,    0.7071731489900208,    0.7625363329695407,
            0.5164087569034105,     0.5158360046777561,     0.28535790612195233,   0.9879664343944952,
            0.67280256912603,       0.8890291546480457,     0.9160230021859649,    0.3870806889253373,
            0.856090460117206,      0.5768102059551495,     0.9223337174375985,    0.23721869407349916,
            0.4069096715991599,     0.7796022542728966,     0.5193936684525793,    0.1748902593527093,
            0.09086477172395413,    0.054490239252921735,   0.1881730383889063,    0.7110632070770782,
            0.8303308295384368,     0.8334735451402168,     0.5379791047283308,    0.16302925152042347,
            0.7727351062557225,     0.3388189644788434,     0.6973041144352392,    0.4733588987442421,
            0.8402609835393424,     0.49625970826659327,    0.6378136894404014,    0.2856951819363074,
            0.4413238986227448,     0.6525680894378554,     0.9983963327111249,    0.48752627131731285,
            0.906506038704346,      0.01932347244522925,    0.8121950275380121,    0.9255041756709821,
            0.5995564672674958,     0.4205531648080212,     0.033335219953896544,  0.5912868361012278,
            0.3196128167231035,     0.221903568953581,      0.6908879786020649,    0.3787754104609753,
            0.8522534041955377,     0.03873285724146169,    0.43482947815592,      0.919129126644546,
            0.667354082627777,      0.31783226395103603,    0.8001375957424187,    0.17310362640226762,
            0.5233480284832298,     0.2913233109609661,     0.8792650757288446,    0.07868718804987207,
            0.9509021711701667,     0.4108003288622233,     0.5934766628579128,    0.34995291077153867,
            0.41854717950418097,    0.6941658891238749,     0.12557733400099758,   0.3281062412202753,
            0.05141635826312552,    0.7419427766883007,     0.5974167460502979,    0.11843824474324205,
            0.41624570357145785,    0.5241978143519431,     0.6088797415457056,    0.07468968176757884,
            0.5023265176344017,     0.9090329039918261,     0.4732874239558371,    0.0873343552445005,
            0.6764967429865865,     0.2685285522329034,     0.17987348708361062,   0.21794025543419937,
            0.04506667459049274,    0.5018862569659501,     0.5078747135732997,    0.43021565967376363,
            0.6523140326710496,     0.28274802785807285,    0.07106725904517608,   0.28412045938179287,
            0.9880686559758622,     0.15413418168040405,    0.6816532780414968,    0.10817525221350599,
            0.24761260716111821,    0.8649020852886178,     0.411873463400273,     0.17058153589762415,
            0.9163691971503306,     0.007196092301011703,   0.1936665602549229,    0.46466627431828744,
            0.44295911943086863,    0.1172971321827675,     0.4400844880916466,    0.5368268361835457,
            0.8684164555968115,     0.2812307653966326,     0.06851445918914023,   0.7132782777678082,
            0.09576820129809371,    0.8995898428580046,     0.10543798768004409,   0.4614566156221068,
            0.6331937101571287,     0.4807961686524217,     0.4609565296429643,    0.5836532376147622,
            0.5807543463780841,     0.8162136284856114,     0.8132769016139306,    0.16919281057255597,
            0.6922889722501279,     0.6638165028617848,     0.44427877700540563,   0.24612130969874924,
            0.2288978442462356,     0.4196732099844862,     0.6068876369911855,    0.8538412466282072,
            0.5377958438136206,     0.0001820101730740653,  0.06012719703223002,   0.7839390339179111,
            0.8661168961580733,     0.7862900458227231,     0.775939700793733,     0.04398179954449821,
            0.35386997226032924,    0.38318008423155825,    0.8753253311427119,    0.5387594663326629,
            0.11380706765344151,    0.379221151136929,      0.3369951011284258,    0.3586380278000254,
            0.24562341723876546,    0.7498769304062437,     0.24873804182437853,   0.05127831463752808,
            0.998994870524362,      0.775559809580817,      0.6384471957165043,    0.012789066888993506,
            0.20010407278711184,    0.18521892179341282,    0.21464298369620527,   0.34693695168648864,
            0.4316773585977651,     0.7126073849598474,     0.7548406602482975,    0.9296337158718612,
            0.12077500898366889,    0.6245571106285978,     0.583836571235023,     0.9160327609345381,
            0.922492499533602,      0.9420495998387024,     0.20225159488872646,   0.33422669541135364,
            0.739250921818253,      0.1884582293852114,     0.8695566511000842,    0.7363479454729783,
            0.23805479899600146,    0.7407899518410447,     0.2755179071645256,    0.6912781729057249,
            0.04029952914467361,    0.9398317582837065,     0.9261184105857462,    0.6223123077462753,
            0.2650908892091116,     0.3408005108816735,     0.9965574809953514,    0.46445559413888493,
            0.418762462623965,      0.24059957173932311,    0.5364698219203713,    0.17213550136081102,
            0.41049021521591167,    0.5401304007079931,     0.7357920834555868,    0.11236329476922269,
            0.6067416559966118,     0.30045453801629995,    0.5362729073390462,    0.6126397865724198,
            0.8408852964127183,     0.9395203879546513,     0.8029151136285452,    0.43569243314527684,
            0.31820688821012033,    0.247134880790028,      0.8186121927712436,    0.32669531958500064,
            0.667774009341066,      0.1425393689196467,     0.3150549477793537,    0.006931651342571454,
            0.37126719317107526,    0.7684911017700815,     0.12496740176870935,   0.29317902632472626,
            0.20034287744146184,    0.5340688691066464,     0.6159440609913173,    0.04264050938256159,
            0.28411421975240747,    0.16536872596589292,    0.37402454445497735,   0.27979702387612193,
            0.29424633772813824,    0.1341630729328499,     0.07763118770442157,   0.8294037178312093,
            0.6445986349739792,     0.3573744028172974,     0.3359609212464857,    0.3662960055336121,
            0.7267007076504431,     0.5282700898845214,     0.23177774254702455,   0.470034327964415,
            0.42530540953223683,    0.1855509777931731,     0.26908806206556557,   0.1058875659598546,
            0.2918959111214987,     0.5545413381101905,     0.567650111225151,     0.6251358005929395,
            0.5933163913435103,     0.4060824049027698,     0.3563463664492532,    0.35579322788767087,
            0.6731741839779117,     0.6030411395318556,     0.539902239255278,     0.41469875627509156,
            0.8169267616188663,     0.7481953395321682,     0.04745545889242653,   0.9136849896008767,
            0.23983362418774523,    0.36534185129051266,    0.8244963402964302,    0.2655065350921111,
            0.08034038945972422,    0.39861362533490385,    0.2103281730057336,    0.5606626584279822,
            0.3761745209390953,     0.3521432404037028,     0.7230344739471675,    0.24479933799615217,
            0.130564383077452,      0.23182891228666191,    0.13456639528662862,   0.14881261821245917,
            0.5215527736126426,     0.6059596476399376,     0.5008135126734993,    0.5073085084774656,
            0.16221552331119804,    0.6569892805853472,     0.023823951884095362,  0.24088471492098207,
            0.9147818962823043,     0.8693736909602656,     0.3294903103332074,    0.4000389448166144,
            0.8559660814456523,     0.20719352722991435,    0.20586435039089146,   0.4929536358123232,
            0.0284778463489761,     0.30467746930710504,    0.6328536928316043,    0.7691416172430684,
            0.9637769150111929,     0.3767251645700964,     0.5119265629781609,    0.5462630373901048,
            0.8288551022692897,     0.4005909943239402,     0.20637488276832783,   0.6035790621789057,
            0.6395792658562701,     0.42420700165424496,    0.0022867360860429065, 0.17061920599563474,
            0.09465517020546466,    0.9655750476934547,     0.06734395045139452,   0.43144153648859573,
            0.8167948544785013,     0.2868153926147301,     0.7381193628197686,    0.908682969271311,
            0.17655464918497465,    0.7443308117004017,     0.3300566213058971,    0.21146004717299705,
            0.29257045439475937,    0.661600674725096,      0.5293186076596803,    0.3855758051123559,
            0.32653997627207876,    0.9399809592108904,     0.6717696795741708,    0.8890848140281892,
            0.8892205579650635,     0.42105114843140046,    0.5967477733647794,    0.24103976527527382,
            0.10398362871060707,    0.0590543439899619,     0.5312719269231081,    0.16846601546263862,
            0.2721027330357856,     0.9269754095126502,     0.2457055747937893,    0.48265466231188936,
            0.4668849041781735,     0.5141109693686958,     0.6234964068512966,    0.28330092949879404,
            0.5762229615596921,     0.8515478253312639,     0.6834088928467109,    0.708220890090973,
            0.3269988295645756,     0.7524749744744885,     0.13811343367924422,   0.5801666070348239,
            0.9957700676641733,     0.8831522987220461,     0.40343267550789286,   0.5205365089412439,
            0.9539164251395428,     0.754499986986482,      0.4467291942830106,    0.5936160462266381,
            0.5710205352468797,     0.5270047257938951,     0.45164915195739164,   0.2691168623386204,
            0.6750068715192817,     0.11957620398471724,    0.6199395121547603,    0.9313905823070637,
            0.26675434276491106,    0.09100085646490552,    0.09246017788786476,   0.5948023039352007,
            0.1701221210699292,     0.6842521824606033,     0.30698034058199875,   0.19742432338007554,
            0.6024877191469418,     0.8902628719182986,     0.06047871106563152,   0.8631127790901112,
            0.5077809457414237,     0.7728702230516774,     0.5028895064850561,    0.7750250102673119,
            0.8957417305842154,     0.10733555728450861,    0.7478021739851379,    0.0899010730440053,
            0.44660696413342926,    0.15137624315723375,    0.5913262760232515,    0.9560738958091881,
            0.7874244726408918,     0.8253902953367841,     0.9418138202926816,    0.007836847312340023,
            0.4839496639431803,     0.024038625585567686,   0.1851074871157825,    0.12281028550110629,
            0.8064402324522449,     0.15612839722975447,    0.7060641037996749,    0.4673448736786716,
            0.011894829080457359,   0.39272168726970946,    0.7161259792323178,    0.6290052734397343,
            0.7772797803099432,     0.1574463433421165,     0.8030899813503648,    0.7363431400338964,
            0.45879823345308324,    0.0037917753764487028,  0.28447976376466977,   0.5921853608258534,
            0.7245782648400525,     0.8096837273927996,     0.7932210447765462,    0.15080502500483772,
            0.9259156010110698,     0.15669259858340023,    0.05376718762761912,   0.17555446073882497,
            0.556873070039762,      0.88559663950969,       0.18637987027397696,   0.7700255428942459,
            0.8164845822377289,     0.9138337355820165,     0.14901577192750448,   0.34358497671230726,
            0.7940872150147961,     0.6041870780033277,     0.9520589780511517,    0.4337068157339632,
            0.9677477722371496,     0.5392420826655399,     0.07939819567247197,   0.22847717117027255,
            0.7655723588101238,     0.041164715874710156,   0.8285532626279466,    0.21536398847890037,
            0.583061000667976,      0.0062261609347386004,  0.03221266681545243,   0.0861970443208151,
            0.8763050548197815,     0.09103933223536842,    0.6737024438959067,    0.7405594875601649,
            0.8125159677470202,     0.9658410265388896,     0.38767817859684217,   0.24770028846362457,
            0.8356555369302041,     0.14644493140939052,    0.1400698032811769,    0.11292835074344476,
            0.8292038135029747,     0.8195253443641697,     0.4305707875447039,    0.008768925103462055,
            0.9418116386213089,     0.3700537696460441,     0.7281458705441715,    0.3525597878986072,
            0.4706622356938852,     0.07597023166061723,    0.7263845942968795,    0.7926957856529625,
            0.20708854189893933,    0.8550994281361883,     0.4921826901475681,    0.731291818473225,
            0.7581965712856973,     0.9847295665065082,     0.35840503352716013,   0.8036214506614977,
            0.6721274753851932,     0.8500341541503577,     0.05622160200809767,   0.9192843691149019,
            0.027933283911725004,   0.9280325658207272,     0.5617424927797804,    0.5642222892605603,
            0.9011999782137274,     0.3167170964934296,     0.5419148325336923,    0.3651261427058342,
            0.0159896116887146,     0.7018674495198965,     0.8360808351884182,    0.7375427821032601,
            0.7412603958012424,     0.5150371220928276,     0.7748984109571144,    0.6296323711967572,
            0.6234499015258101,     0.024127763826156445,   0.989420995145785,     0.5510541799966217,
            0.016152307255765885,   0.9004863713616409,     0.901535273772316,     0.5337852001885921,
            0.8199119894702028,     0.9602062266495569,     0.02635737140901462,   0.5152671357593348,
            0.11413472626947263,    0.7271220524232084,     0.04669851962449345,   0.973215206797113,
            0.06783606544762733,    0.5844296822524406,     0.42134165886829256,   0.825224107446682,
            0.21615617130344023,    0.4605835313838086,     0.7152292017218791,    0.39286153037332894,
            0.2539815252546994,     0.22192389250490863,    0.8671251111429498,    0.9326793274770836,
            0.4985496220390869,     0.2884633592216248,     0.2583217015191792,    0.940757553166467,
            0.1604329513170739,     0.6821676702342997,     0.5233404828416436,    0.0775679753161076,
            0.7760026334993639,     0.9614881246947393,     0.6950328669480749,    0.8515268490542989,
            0.09479128969214268,    0.39494728277981694,    0.4891769476639597,    0.41815752427950004,
            0.002589580520870216,   0.3521693755960913,     0.6173911460923495,    0.8436265316336606,
            0.4357535838448764,     0.6043833408130997,     0.948749730572852,     0.5456556522329518,
            0.7662402976927806,     0.053064650631492793,   0.3742569671467624,    0.5827552867093838,
            0.4889867387806579,     0.3820360142557956,     0.041034126282015415,  0.03371107995460243,
            0.17335834127993832,    0.5371189738919894,     0.29752399365933646,   0.9286286264166171,
            0.10053376819026816,    0.9519555140547626,     0.5380222669463706,    0.5360900152115715,
            0.6896312366373103,     0.23960618855662463,    0.5001524977804008,    0.9410407030719887,
            0.5546259450199411,     0.7384377027597124,     0.8867532014071955,    0.5532096254824155,
            0.3888227482273181,     0.9584850375383954,     0.22411777454756432,   0.06716569034802933,
            0.03624978672089152,    0.2449932378264459,     0.8147438662205808,    0.8235595013273971,
            0.2893755968545869,     0.6549066969067938,     0.5884037291461175,    0.6833479121086908,
            0.3258673953587694,     0.14899594581568953,    0.722285317207751,     0.9387781974451291,
            0.5956209233022104,     0.32995424803264606,    0.513543699170583,     0.6856517421406892,
            0.3484620158608389,     0.08591173217023884,    0.7216905568962927,    0.572228776676013,
            0.13500720071578665,    0.0050531610163068175,  0.6305800108483972,    0.27613953256055224,
            0.3113362409469934,     0.9481644323543554,     0.09548186749721632,   0.19042100198379142,
            0.6501161284002616,     0.32604043008100503,    0.45583282030161676,   0.3092296984805829,
            0.5981846050269867,     0.8786203326873409,     0.4211951023483903,    0.25893027667094404,
            0.5820861663660312,     0.16588048611517814,    0.9909001330674879,    0.5146467776521106,
            0.6890849695525968,     0.24123070427349003,    0.1645764521259283,    0.08324399422537854,
            0.22221536783269424,    0.7318393486409069,     0.5270003280355477,    0.03360274379553796,
            0.6139566592431733,     0.8015177461520704,     0.7291459651181122,    0.04219298921856329,
            0.7812088799573482,     0.038004761877105664,   0.15169045145630933,   0.7336080796610778,
            0.4321421952876542,     0.24856079593973557,    0.6081573338404965,    0.8733286351828925,
            0.7890779557070375,     0.04189005311550542,    0.27637014102896573,   0.6204954038575486,
            0.5653786728700275,     0.3732710478564275,     0.849386101873059,     0.9653013944564804,
            0.0807368816315579,     0.04608995311394204,    0.15113810120034965,   0.990384502134036,
            0.7495425194796209,     0.07615960014862666,    0.3820918759585902,    0.757002544984481,
            0.37983058080434107,    0.6084548349806261,     0.7592175264980374,    0.6377560264624994,
            0.014791085098256329,   0.5036562051824344,     0.9543143183435266,    0.2604842540819292,
            0.6173257441758949,     0.8261649801252334,     0.8013303320945281,    0.14527962650796356,
            0.22448127924488914,    0.32509376978289195,    0.5098732051987036,    0.19046104219183801,
            0.6392980519244734,     0.21859424523447457,    0.26278855474731755,   0.18072378418074853,
            0.9591595406028821,     0.9190787106943422,     0.48302761425224616,   0.7422902417466823,
            0.24995642067620194,    0.12466455199951032,    0.8272258519851345,    0.4114210682179176,
            0.32062426361257945,    0.30458700001408,       0.25667551160023694,   0.8900286474799964,
            0.42455454617674293,    0.45666880150266953,    0.0693183413941536,    0.9454989347857441,
            0.35165264478129843,    0.3498666628649668,     0.01456774626142987,   0.31515040663895133,
            0.10346516575790154,    0.7578485962534294,     0.3309948092820333,    0.534297688397666,
            0.2857557785794663,     0.4193694879451202,     0.08756225558562658,   0.19137192615143683,
            0.8954386627589025,     0.9591420819176131,     0.993492948252887,     0.36936988473957844,
            0.17461262186922533,    0.7392188932477525,     0.752907274396299,     0.9291615441767478,
            0.5512000308603481,     0.381139811279397,      0.5865361829032815,    0.611593134511888,
            0.22338528282159997,    0.1605048387912127,     0.9035349998127344,    0.10685187281330522,
            0.04950599766051966,    0.7713245856983733,     0.40065711193552667,   0.9119457079044073,
            0.7262105842833073,     0.6553078510892713,     0.15560506127053697,   0.1551553551941499,
            0.03542722557158384,    0.39459663453385474,    0.17302792860073513,   0.04743024311252164,
            0.45222697483459573,    0.09727342383121718,    0.37070121288662183,   0.7717157479636357,
            0.6526827338596336,     0.9597813263941959,     0.3506004159048033,    0.4120935734358421,
            0.28746676879539,       0.40846442705034247,    0.34042521125650027,   0.9998776398159358,
            0.3199212645260523,     0.3787052054488068,     0.7954085859068571,    0.24121975389211103,
            0.5007010982589831,     0.5557316216194076,     0.7120093450390624,    0.7757483338262458,
            0.15679902985768457,    0.5115840422955561,     0.4683384901513723,    0.9710901626736105,
            0.531114877023973,      0.11944046760628668,    0.28810598823138733,   0.49737931173820227,
            0.7630338973591214,     0.6297796776608683,     0.982918177966818,     0.9754048510738085,
            0.30894769342950446,    0.1408139794098764,     0.1421447645088545,    0.7805266338360533,
            0.24009724933284315,    0.11350093545615292,    0.17353948867266333,   0.19109267959865261,
            0.7667167687284862,     0.09969916744372154,    0.7189765042250021,    0.9386198919957126,
            0.1794141209968937,     0.17547456761587,       0.5300011606306563,    0.0942936592224356,
            0.043386635042763966,   0.9714582340334335,     0.9914623205017665,    0.06662655613515922,
            0.15741300969346605,    0.5879464987497366,     0.042086381233854,     0.3562158512248824,
            0.443417272397133,      0.07566634837683883,    0.89634056982892,      0.022100208338563188,
            0.6361340178435013,     0.9620599959216278,     0.2713322161356465,    0.8488416767279183,
            0.1498237166844839,     0.37875305290250194,    0.4773676271281221,    0.8938920543568624,
            0.3843629822974356,     0.09950524502221147,    0.20396081180396708,   0.3077922050083325,
            0.5615088176538799,     0.918773298750677,      0.40293770414876373,   0.918592120395035,
            0.019895001735154794,   0.43764829173947684,    0.6887578842986972,    0.8228543176726193,
            0.5226371799728504,     0.4682092578022833,     0.6446047763275783,    0.27680698885994237,
            0.5935186026217055,     0.45485018935797084,    0.8418826034312816,    0.9767222143976066,
            0.5117145979721889,     0.566149567436225,      0.8763309511866845,    0.499923718246694,
            0.7161767899432988,     0.9099114594312094,     0.29641050214914477,   0.0954943857066205,
            0.5936248330113836,     0.11020524057556624,    0.5175859520520134,    0.42662706190829447,
            0.13726487467797988,    0.8011462974433778,     0.7703060716926005,    0.30144372606039793,
            0.7224483265482576,     0.21876759896010178,    0.9199722993473572,    0.17055163283414543,
            0.2815264904912502,     0.5830296466816132,     0.07820325563111274,   0.8343410830887459,
            0.6098400806799412,     0.1139511840093036,     0.8982717680510702,    0.27792006001022285,
            0.07470992041645985,    0.29259309942416034,    0.46252455578479323,   0.009096850030739678,
            0.9456742603747124,     0.582154355738705,      0.49120756461815784,   0.4281452027122896,
            0.9144348881913614,     0.6908781650908771,     0.16718051131447043,   0.5368850490327784,
            0.05347617173396779,    0.04521314971280177,    0.8961707730083248,    0.9798855033702991,
            0.16480904255871043,    0.37301216634895107,    0.2524925851605131,    0.5232767842087098,
            0.7691593448330488,     0.9536964916877203,     0.9848775214497036,    0.546559794412735,
            0.6406298013606235,     0.9114179316381104,     0.41169760443203196,   0.050971120079208565,
            0.9613464804886003,     0.9356312388442105,     0.9517782951791955,    0.4062152467312564,
            0.4296848295237362,     0.6626379387038296,     0.15413405458482943,   0.9391679348537739,
            0.63801076840913,       0.19770649294628073,    0.14329257523214267,   0.24694895980913123,
            0.7753486540336143,     0.7404694927364381,     0.20774376083936952,   0.5578868313750754,
            0.47477915987055463,    0.6596469680927798,     0.1963596193492394,    0.6051300750975935,
            0.7234991160038934,     0.04905759995353154,    0.7773179075013043,    0.49347551827551184,
            0.6188762345745582,     0.14319769591362674,    0.5058206446025271,    0.214621999730513,
            0.8030536961046402,     0.5097326026525754,     0.573781625100325,     0.23586093188786506,
            0.01756712760874246,    0.028441160053459424,   0.17403124536830727,   0.6463542000447045,
            0.3276111952616175,     0.07432614797139292,    0.10385318009891331,   0.3167305499719085,
            0.06311345757646147,    0.18962033979741688,    0.8814844946531515,    0.15491351069263926,
            0.10852109814295696,    0.3240637235044723,     0.36559071047741976,   0.2564973944069866,
            0.6826292614794567,     0.24747784083097324,    0.5746978029366447,    0.06891287393783507,
            0.600125175206528,      0.38191339337927643,    0.8627395667345134,    0.6230874782216934,
            0.9210343883110211,     0.9724284063784515,     0.7431049195537048,    0.9696098011907885,
            0.04618012856063358,    0.13673976409545696,    0.2281827149464647,    0.07016593291139372,
            0.9389972369382291,     0.38973125312917256,    0.0013417138651293792, 0.049013252619224734,
            0.3435698928387867,     0.3842870570793231,     0.43269632772394195,   0.8522927514527636,
            0.755969298158086,      0.9339258507139604,     0.9136241944280807,    0.11522556842570664,
            0.78335990296543,       0.611482297547138,      0.5162057004111185,    0.5143283856491905,
            0.5482974399405539,     0.5897777047423666,     0.45219424036166156,   0.5758191701388371,
            0.6984615264093182,     0.598111605345339,      0.7432743938310424,    0.20560371186312243,
            0.3402964803568025,     0.17336099556730689,    0.8669935435193754,    0.8967247415048393,
            0.11265689079018526,    0.10030682162467541,    0.4108619107229994,    0.24060839367580422,
            0.9393915493941769,     0.9457855092079419,     0.17875232120634932,   0.42578326188448756,
            0.6698869198498638,     0.5824992597630952,     0.25467239643420037,   0.3049903314837056,
            0.048010554652256454,   0.3861953542566655,     0.5345273410223993,    0.02624265892337896,
            0.6848512549823775,     0.002514097826813111,   0.9396724066630506,    0.6219813242597629,
            0.9995311930688077,     0.23698645667834384,    0.1705093220131445,    0.3017230951321942,
            0.4143836311475525,     0.1767556976050806,     0.7084464121933092,    0.07801558589512991,
            0.40284553390334454,    0.519412074409634,      0.15793171509248938,   0.18495561278166328,
            0.8193602367999603,     0.49194064014969774,    0.6200423767761295,    0.9162033188748976,
            0.8289827810984656,     0.9673295648421363,     0.6512193643108418,    0.16411888147926568,
            0.23383804059763114,    0.11964701561830104,    0.6290278355052964,    0.9950103755953448,
            0.29995291760004783,    0.2983580309672912,     0.4568280505317853,    0.12036860501427671,
            0.7655538601568828,     0.20217928748091718,    0.22390669334174584,   0.7018462032438008,
            0.8951049333769487,     0.6408536578064189,     0.11923325800797147,   0.5788651270556688,
            0.28746660158034365,    0.33777613814759233,    0.6113074414616786,    0.7337042150149125,
            0.41170069084718797,    0.6939031555748576,     0.6775056563440938,    0.8529427257187413,
            0.3652924229440403,     0.048099343782390114,   0.21789259382300408,   0.7367935457485177,
            0.23088031866698078,    0.9133046070597758,     0.642658696321858,     0.06871831747188573,
            0.3511510958290064,     0.24886822958384247,    0.36759300858275834,   0.5355212813587565,
            0.6234508362469428,     0.5433610213100648,     0.7909494857889581,    0.44185026695052365,
            0.7152703888002844,     0.9788794367484707,     0.3384594262355374,    0.51451618182596,
            0.7942183252400027,     0.08613796561292286,    0.2294362673417849,    0.05194408733084144,
            0.1721897358594271,     0.7832719681111163,     0.48639651559051134,   0.39585135910388103,
            0.7521834990319302,     0.07072008710945821,    0.17760798159525137,   0.23718053105526427,
            0.6331239258299188,     0.6347519205097168,     0.8765830782746383,    0.9565809403047149,
            0.9998768782909935,     0.4709686054539567,     0.7073010926730097,    0.3052706870648968,
            0.5197766682135851,     0.5565881174458365,     0.17969173133250826,   0.3681515934629004,
            0.3980140261931231,     0.023802569996914102,   0.21145440202098342,   0.9195942258593471,
            0.24079018878434544,    0.9733819174048696,     0.9692146455721969,    0.09854074135406643,
            0.26894464767709325,    0.8179284951769817,     0.7160063133938216,    0.4510908896392407,
            0.276610298188953,      0.21322624706619175,    0.16941021800105205,   0.48736035905853936,
            0.7687449190483973,     0.7557739944253864,     0.8610213881987214,    0.22032491479003957,
            0.24606133861881097,    0.41051472856216564,    0.5899261917913632,    0.7244586739830227,
            0.08690816631651976,    0.8850744546274172,     0.5602508931288084,    0.013340055963316555,
            0.4230390048200161,     0.3204265511001234,     0.004092006956385674,  0.9109879206252187,
            0.8366373131822703,     0.4215902658572529,     0.8532773718403144,    0.02693733574196866,
            0.4360766069594092,     0.16348845075398621,    0.028887159524982553,  0.5416161679445444,
            0.7700477093487886,     0.43007659807676757,    0.04542086297836534,   0.7804761916975351,
            0.4305110191806777,     0.0015512806231195997,  0.5234473013503755,    0.7017270620543213,
            0.49084484702102116,    0.2111339130003579,     0.8174819372777837,    0.3029583706565877,
            0.6699701589858571,     0.47389629250119514,    0.5251702701872705,    0.8440908253660291,
            0.5001314273320203,     0.196807061896765,      0.15564503929895857,   0.4480700445464203,
            0.5760380254928401,     0.034729318595858594,   0.3215007866362165,    0.4623789796270176,
            0.1018846860259609,     0.03628207673368378,    0.7568326501476224,    0.2615733574631275,
            0.9982413707898519,     0.37311324212027275,    0.8561193908808279,    0.6985262701519291,
            0.0899219195496902,     0.4837180436167945,     0.40931109524891485,   0.22793212592605983,
            0.35985242580268006,    0.7354309456205813,     0.3522648755390645,    0.5684831309986088,
            0.21578915801626508,    0.2653202226551683,     0.3823056920719937,    0.7055507309431205,
            0.00039283548935353796, 0.17579774816140614,    0.2750680916869609,    0.5254263808147814,
            0.9378144255303508,     0.9285071576418966,     0.8385333568244282,    0.4518907236845362,
            0.5640029082059482,     0.7814762954988036,     0.36085253207849677,   0.8035042172458713,
            0.5768605741417565,     0.21313087796513386,    0.1816584887933913,    0.3036087738755773,
            0.5815654004960356,     0.610553741473695,      0.9004794621544634,    0.11064083867461294,
            0.32881077186069474,    0.8766984853201764,     0.8094416939313411,    0.8924390241659413,
            0.3537515191885554,     0.8970451960201312,     0.9565357069340682,    0.64131042963527,
            0.18620896307163504,    0.6391350610059358,     0.08947240905703113,   0.621455764104525,
            0.0063786285874778414,  0.927569073578716,      0.5687799650578784,    0.046313305070152855,
            0.6693345003244959,     0.22991393995842757,    0.45084180336885504,   0.6899796672357308,
            0.47256239674648803,    0.598402725605706,      0.3047883124484012,    0.25850166399116825,
            0.47737494279846127,    0.12132526694305201,    0.36962074720142946,   0.4111950813186772,
            0.4896215731020557,     0.8113048965602441,     0.2500256691452587,    0.5264701251062012,
            0.5818177209128076,     0.1478592444256308,     0.5695929357243108,    0.18625808885589867,
            0.8757877582595698,     0.7724253817374587,     0.4302265969464366,    0.36062457071904863,
            0.9940435342813795,     0.4518495055766967,     0.26069230903720797,   0.8348720887403068,
            0.1852421456711113,     0.40954356501913003,    0.009846208686320135,  0.2013446314375591,
            0.7531626998398254,     0.3337308808689585,     0.0424075241452927,    0.37285349750255437,
            0.32485676082432047,    0.27313119313295675,    0.2331322656040885,    0.6968003623423193,
            0.04536761461418082,    0.9013468463257183,     0.8552763171110279,    0.8595261054382751,
            0.5316549983947071,     0.36718507477339213,    0.9606463362954432,    0.8456418045085219,
            0.2857828637319778,     0.6003060048777412,     0.08463009106018893,   0.7806141379032902,
            0.830984058287618,      0.231469481698801,      0.10423487162408762,   0.8050470791979834,
            0.7877431848646022,     0.2517284831416057,     0.3548574157225389,    0.8550966698736117,
            0.5453006595029133,     0.2858451187055142,     0.7660226750736991,    0.8015646354579128,
            0.7487255142692215,     0.9847635880922488,     0.06803864911442081,   0.08273172290801212,
            0.9210635594984841,     0.037878372508300395,   0.060072689142463886,  0.37543033638679746,
            0.6106956528487804,     0.9916753250349124,     0.23549258351538327,   0.2564632229388758,
            0.5990116249314974,     0.812927923318155,      0.2825451472016566,    0.5413038847070728,
            0.3877899579242009,     0.5302479062841088,     0.5424864033544462,    0.9713521856885804,
            0.5840132545453635,     0.547258891069285,      0.3835380680486892,    0.7120721435359034,
            0.3033880848614229,     0.13641415536155488,    0.016011872494738122,  0.578320115952207,
            0.04023854116224801,    0.3940698935357704,     0.38274045520347344,   0.7332539866978104,
            0.971344368425348,      0.9187412911009326,     0.5047603892165965,    0.3226403207371059,
            0.9423276074358952,     0.2290268121416097,     0.34985762876172233,   0.07874494740012916,
            0.5052065343777334,     0.18287239440963776,    0.12318171253294807,   0.7287573239278213,
            0.14142599434308634,    0.24927807660319856,    0.9158551192749332,    0.28919157438641874,
            0.2348996035031694,     0.40855557787810026,    0.45504893094125265,   0.767227224797335,
            0.5818291926517437,     0.014308915042779868,   0.37488754824549486,   0.32294853493992004,
            0.05928427192516994,    0.35428716996761545,    0.41220527003171226,   0.26859108957429334,
            0.8741139833814935,     0.6075051236790235,     0.2890899467490916,    0.5491615886776077,
            0.7537100073947047,     0.5694917927351275,     0.6055449214915447,    0.8735338219121755,
            0.4852041915381974,     0.06509190534510179,    0.9009886788572773,    0.3858912408829892,
            0.12176496350515043,    0.08075775573754607,    0.16755746300805785,   0.8917133036370205,
            0.9139204928860818,     0.09911876876935721,    0.720950732430429,     0.7715181263698246,
            0.9573423658626958,     0.6627741999275679,     0.2029475297654838,    0.3083553828720149,
            0.1294632871304532,     0.1678302617661922,     0.8270122819468562,    0.4578102194378917,
            0.7581038461490538,     0.7675286876755522,     0.9733562569635608,    0.8270275598678809,
            0.0878258390387785,     0.6482514031181247,     0.4379031753221688,    0.5157032142600425,
            0.36618388156620674,    0.9160852349555045,     0.7067118789314688,    0.8624931676681007,
            0.9792074096394295,     0.8133448211736258,     0.24650987150070136,   0.4457553787815328,
            0.47544500486312746,    0.44439692115601104,    0.31194349248963427,   0.8257979887535002,
            0.30855857157905464,    0.3717779619342073,     0.21800748653944135,   0.5647449378235687,
            0.6104580210586518,     0.45906609083541106,    0.529641511186618,     0.4738460026771928,
            0.6810308827738719,     0.8306726087078727,     0.345711611576077,     0.1844146733812414,
            0.2000148908942101,     0.06727263359783153,    0.7631349960207461,    0.03566717580710854,
            0.7098769233442549,     0.6325189026776282,     0.24888346983992704,   0.7180223025854079,
            0.7234712353519344,     0.8246172456954701,     0.0989113139270189,    0.5841537473068414,
            0.3868411059954022,     0.9393851576693042,     0.7853045760049937,    0.2662147804636781,
            0.7069128842662119,     0.22583763183563044,    0.7035861420911306,    0.2465858641670633,
            0.4636881220638156,     0.2330311819885833,     0.3154514405521681,    0.2924539172877595,
            0.9857373546877803,     0.9598018735808685,     0.13575284561748824,   0.2999720153998805,
            0.21735079598214113,    0.22394402103279887,    0.9530454820173864,    0.1596308591719282,
            0.6290995579405391,     0.07577595411215055,    0.8616695446195058,    0.23481776802675658,
            0.3928044534951387,     0.9104582959167222,     0.434877976301968,     0.6542473849483609,
            0.9026424568777447,     0.7007405674255236,     0.7655756914680999,    0.437146275113413,
            0.7273488271126418,     0.8149340957450891,     0.3869619231869017,    0.03089143649103754,
            0.48785308907695957,    0.7830515366972868,     0.13273672388515678,   0.9444151761360882,
            0.06805854212477513,    0.35796647799735115,    0.6498833838548614,    0.8021894946359983,
            0.8472836493166831,     0.8058989848143164,     0.465086451404091,     0.12637369648776653,
            0.5195536848373707,     0.5266373161514829,     0.977873650203974,     0.9707535092294907,
            0.9772362872539798,     0.6275848301347967,     0.9785159662583963,    0.5538975175363018,
            0.7951489306186488,     0.3767326540285659,     0.729647946582455,     0.3875675644198022,
            0.827688228015073,      0.7788524079235603,     0.8736410066062794,    0.8081380737419247,
            0.6208526669675403,     0.6760280928312599,     0.33165825327337195,   0.0014318939145854248,
            0.13220524685383483,    0.8679297767642343,     0.890392909646724,     0.1955180196691393,
            0.1201456634007011,     0.6106218094686656,     0.010727331788318528,  0.1909361435623047,
            0.5999608041139243,     0.17542741083855917,    0.8041087864392908,    0.49464454554882287,
            0.13612500781123194,    0.23992200980645306,    0.7655912936873249,    0.9057723681028971,
            0.13177587254865497,    0.5983906669744957,     0.43185843805809676,   0.9826855163007238,
            0.08986826076477294,    0.13329998689043498,    0.5877846423757153,    0.5556100147280302,
            0.22137440681223297,    0.21165741625821122,    0.36387706409732024,   0.23563285844210113,
            0.05511754106973754,    0.7371865362386334,     0.2793195336031903,    0.9703697925632194,
            0.8796548179271865,     0.9039982362710011,     0.17160405246806798,   0.32270715806353223,
            0.4796812565347358,     0.16333637340573715,    0.8893759794711299,    0.8411216732320622,
            0.8828990404514035,     0.6371220283370423,     0.4651488463104875,    0.7010907054828708,
            0.6920317041886856,     0.7927734617239862,     0.758488560267538,     0.6812219957974719,
            0.8478475443388092,     0.5038845583805129,     0.23931042595547192,   0.45262291951936995,
            0.10511934336609974,    0.43597758587988633,    0.258454359749239,     0.6770128487513555,
            0.9006769542665601,     0.021667224418262077,   0.1417136002554985,    0.8606306849960799,
            0.20725345597806932,    0.34173699239732214,    0.08410784422357265,   0.46759375514271095,
            0.040022592625419295,   0.6776843564696649,     0.3668475339363545,    0.583494086741944,
            0.10728872676544443,    0.5701126281443447,     0.9389818415318578,    0.183733763847036,
            0.6815518661810608,     0.614932533487753,      0.7362828058663401,    0.09492427229668066,
            0.3848274650306498,     0.6503859515045798,     0.25197801778951934,   0.06673369641552629,
            0.24957362684278928,    0.2537500351950823,     0.1385896740620981,    0.7295932916950076,
            0.7210174245341036,     0.8502610746350925,     0.16497072803320123,   0.9267158528222813,
            0.2804173352164233,     0.4583101844652592,     0.4915607873900022,    0.7340181110445808,
            0.7433386672138056,     0.8422317638570351,     0.3286462336352586,    0.8127611537764468,
            0.12458201524224322,    0.5692742568470338,     0.29395757955104473,   0.42746217504695994,
            0.25865985479099796,    0.04720807062660748,    0.9192320711250647,    0.2993476340142658,
            0.007142688301701505,   0.2204812062903747,     0.5200916330793901,    0.6489104946179726,
            0.8204106149844391,     0.06264511065207623,    0.8671491396485991,    0.6074139071984634,
            0.5262128963098944,     0.2958716745812183,     0.7813396873303533,    0.873180571690271,
            0.18225790991450785,    0.6462921511388142,     0.06376474836533608,   0.7248020594800143,
            0.7079579906584446,     0.9052977895485574,     0.25984885646868994,   0.15606958485531963,
            0.8988677091539751,     0.6473901414847865,     0.6600509898475924,    0.40518297286617166,
            0.41929155257565276,    0.02064144996989603,    0.7022142710202426,    0.3838894818057821,
            0.10985220181206967,    0.868250184425854,      0.09582341656945681,   0.48401269391839885,
            0.4832365079598986,     0.020781892250526623,   0.15001142546445267,   0.24067638111054324,
            0.440607801813774,      0.9127505302223127,     0.21817491658350352,   0.2465725054507102,
            0.2812381104777337,     0.6557124107282353,     0.38928447419289036,   0.6181616723806028,
            0.7995073278614203,     0.7401888309791503,     0.13117463757275383,   0.7774046984010252,
            0.26245372666065214,    0.9568290244531503,     0.1277554271028044,    0.29653963508741477,
            0.9611149520591701,     0.5160454663488208,     0.9250601226273969,    0.8726247639115782,
            0.5518053231768054,     0.6914251582344108,     0.4912397711655774,    0.8115252415332865,
            0.5218342197537854,     0.18236587918458325,    0.039501655165854355,  0.7294998023946525,
            0.20660520422254613,    0.8313271371854392,     0.13069804480624958,   0.6217335884413676,
            0.633157092642952,      0.7359265096602418,     0.5453449544360572,    0.5631001652584222,
            0.14963075714136687,    0.5268886128924611,     0.2836812243221698,    0.7357656090263957,
            0.5775445817868133,     0.5184293412962692,     0.6486845474479586,    0.6882335261966438,
            0.25025640400915705,    0.8952458349480324,     0.9452513468635373,    0.9633228320603805,
            0.10916311314862048,    0.9751227719466589,     0.30153449696639734,   0.8600021987263843,
            0.551623508554851,      0.1216755688977923,     0.9454378111712867,    0.8648640640746488,
            0.75460226001643,       0.8021674699568363,     0.2885961154639236,    0.5892655389175749,
            0.5820366047050023,     0.6638692140695065,     0.6698557569948149,    0.04500805503346872,
            0.5069740877616361,     0.8957017090827417,     0.8163075654907458,    0.009484964441270227,
            0.20799557699400784,    0.6248598099190438,     0.8268831433609406,    0.24495511131665426,
            0.9093102929274715,     0.6827487190995692,     0.6517748250094413,    0.685101378687588,
            0.8420522932839575,     0.7830650199365697,     0.46864696841639675,   0.37773773598466365,
            0.22103976661591662,    0.30948427037099635,    0.47974246876395077,   0.7898209564122577,
            0.03500981065136166,    0.7668498769620313,     0.07081464978841678,   0.5410053019324684,
            0.056809041781473835,   0.24741769991887064,    0.3246980719811503,    0.788445509784719,
            0.7048449564252207,     0.5979729680175447,     0.021114046175444212,  0.3738050064348668,
            0.34621015754869533,    0.048927153500845755,   0.523225995711705,     0.7220186048562133,
            0.3580536686832083,     0.8451572964908878,     0.857427487948765,     0.23169407068989867,
            0.38110578222122216,    0.8631619157574336,     0.8549071089253937,    0.5280198234883616,
            0.727625447794741,      0.11514883793606923,    0.8139348606835712,    0.6860375793755535,
            0.05294966937366408,    0.6589654624144575,     0.6021982750753707,    0.04476535837224083,
            0.24343046777977528,    0.02005167285774956,    0.7850595228766827,    0.4059349951718122,
            0.8944737192082901,     0.7788062086981887,     0.056590431798021035,  0.7523798021122023,
            0.8977746111332048,     0.22788861502085933,    0.3657699507278147,    0.412687120947112,
            0.056209988661683696,   0.17489934962869214,    0.3312024131975225,    0.356841273518706,
            0.07745506210333619,    0.35883944910388454,    0.842713949632148,     0.5593598406439976,
            0.995450779975233,      0.6607821288236504,     0.971357812661321,     0.23452596217137522,
            0.6763995783187896,     0.20370294212333973,    0.7681986976304094,    0.34286632198852907,
            0.649486023788997,      0.6958658790972774,     0.5324032657247066,    0.4423497942246497,
            0.5244757693923124,     0.7259592672527376,     0.06328671709877087,   0.13796135768879303,
            0.11627599126708488,    0.5602872813311196,     0.7724463171596005,    0.7402127274369983,
            0.23173760273237543,    0.5014054340296494,     0.35581301026952705,   0.8016535823378489,
            0.0765808508471093,     0.4705669442118541,     0.08987757030370458,   0.8765624968703016,
            0.5096554481647972,     0.12861693235155358,    0.5033152272569512,    0.2869821112917659,
            0.06276434146725585,    0.5460019010715733,     0.6804293325861408,    0.17077149115777335,
            0.6252185561757618,     0.6847202326458784,     0.6340822911234761,    0.37992469307660226,
            0.1310546588488768,     0.7529301677887328,     0.05818395888784089,   0.8005648840817398,
            0.1450150422854155,     0.05344185972836257,    0.7682453440171657,    0.8984348507737858,
            0.3994383796935356,     0.4670588072276147,     0.49027935151702107,   0.9881796187507176,
            0.16810131075029355,    0.8455214645571316,     0.5361018503554209,    0.5190000373950252,
            0.6624626931972518,     0.3017037390070031,     0.44069031856867,      0.9167781154821643,
            0.04841865655290367,    0.6356951423655655,     0.4654591632635602,    0.05771375662133971,
            0.8119703246206331,     0.8420305348811182,     0.9704548574825884,    0.1841781286534374,
            0.4492837745761632,     0.07560534339333358,    0.43253538903013855,   0.13803103485403212,
            0.35224203954420175,    0.8757658631262869,     0.15661733251981524,   0.2628912691386208,
            0.6093001302585389,     0.6214514660998613,     0.34215896727147865,   0.850358073124269,
            0.15742953063415577,    0.8326910081198957,     0.8036572985553427,    0.559250266099208,
            0.5463677058033787,     0.3733665986030992,     0.4445747091705091,    0.14575674092864044,
            0.9524490846759229,     0.3035830756814668,     0.8907864492893807,    0.8014531860175,
            0.6606793575692113,     0.1258716435479723,     0.48217387047038396,   0.4774172734020168,
            0.77099502057293,       0.04185936348086905,    0.37296062225255944,   0.9942133728554116,
            0.8773592061759301,     0.5739843160124485,     0.05913781114276451,   0.2116335763409588,
            0.48448498360275316,    0.18725617569109765,    0.660722171397489,     0.4790176750874734,
            0.6789972124194318,     0.2759917826852536,     0.9854712951586065,    0.2750656661149097,
            0.6957795047177117,     0.4056926043217185,     0.6048068813711487,    0.7413544514939397,
            0.16212174676373936,    0.37050416942316233,    0.22621699933466966,   0.7400033811583572,
            0.42686525547687015,    0.9697221280268933,     0.847976796655453,     0.5027407065016054,
            0.7016645203192741,     0.6762263378589377,     0.07782895800404754,   0.4973343605855627,
            0.5024993403149735,     0.9709257689357993,     0.7501038834155871,    0.7121992877599042,
            0.35367386196162354,    0.3391267771165555,     0.28014695716341564,   0.24087553553881347,
            0.5615797695981954,     0.9698559572852622,     0.5972661355434566,    0.19294176804827368,
            0.9646521485234241,     0.14557982515515344,    0.34009276038258884,   0.3022193822174213,
            0.1512788334569477,     0.9610585413236644,     0.21532147428744064,   0.019987991894313106,
            0.43938670104992994,    0.14721788841742778,    0.058690849548663326,  0.4346965278714716,
            0.35251851946336676,    0.7835813536122155,     0.6760653479386179,    0.6220431311477301,
            0.2528196140742368,     0.000533959017523622,   0.894535870484847,     0.8492038880576421,
            0.38584082932288033,    0.2884756729814787,     0.5492959390611992,    0.8017052722645179,
            0.9698341074093364,     0.17679641372360777,    0.37850671471819664,   0.18521024429108068,
            0.29242504625332066,    0.9902179706093583,     0.6009030722364157,    0.9543115655949146,
            0.8052138837075908,     0.35885131314543905,    0.826698813731397,     0.23472943644304167,
            0.7239904300116352,     0.2528411756865797,     0.9907365576775006,    0.6431339519213621,
            0.7098248144745495,     0.017563394381231223,   0.2815643451598231,    0.17719456011547463,
            0.8793359589476787,     0.16890251517331234,    0.03165094546018765,   0.6128364948691476,
            0.048317563465782976,   0.1457891705597023,     0.958321841621704,     0.6416285373723682,
            0.49774456764256825,    0.6727845817695177,     0.15352816461969254,   0.4439653279218745,
            0.39448707551900963,    0.13456503199038383,    0.40250212649214834,   0.5164340130185989,
            0.15652878620929878,    0.6051693965603501,     0.8639195616874488,    0.8639096596071155,
            0.17127823137236653,    0.510855518917277,      0.4924797093273501,    0.33840311400076073,
            0.5326024559457917,     0.1553291294271849,     0.6294304337632686,    0.9477040569348971,
            0.40627235659717664,    0.9285678971514644,     0.31334293951594727,   0.31362824231638153,
            0.4502801268279887,     0.43859159594574626,    0.09937237324211612,   0.7054532175727435,
            0.7753889535366468,     0.5861858956790452,     0.9500288997523375,    0.49494136036292513,
            0.08551923672392192,    0.2953832128564374,     0.358815848412364,     0.7999766996401263,
            0.2535075252881923,     0.709208934072963,      0.9986422801896068,    0.5341064948385097,
            0.06873813351617386,    0.32494143314782553,    0.5612590070724911,    0.38135270121565146,
            0.3578925062065128,     0.9690429417434706,     0.4795512500636956,    0.07870280725666035,
            0.9082116049561345,     0.18747127227801885,    0.7984100268726031,    0.20306850164878976,
            0.23033397499655872,    0.3496075587760975,     0.9050194173979551,    0.5993050147422377,
            0.08476893729639334,    0.39451639252751014,    0.12818913647973307,   0.05214553641964548,
            0.7771611533497547,     0.9029496034331609,     0.37999809354136216,   0.893461277271767,
            0.5612955556042161,     0.5010305335932177,     0.47018791099901425,   0.7502993429119359,
            0.48090654094183216,    0.3130844076383874,     0.1449483159538807,    0.9732559879302132,
            0.7024840598455233,     0.5653970243727656,     0.7152470131738804,    0.343701874839517,
            0.8320915198545262,     0.5002502568983627,     0.03307604441020673,   0.8126455203432832,
            0.8229732106738592,     0.03136039392274759,    0.2720364109029123,    0.7301508107518284,
            0.3299319102803394,     0.9554750560532498,     0.017237000276937642,  0.42919080385716035,
            0.78005512031634,       0.5634771660930555,     0.7626499454832276,    0.20983575343207828,
            0.3353795873079396,     0.8460419427766125,     0.23459411670206154,   0.10143977413056626,
            0.2540198303338761,     0.16254531179501208,    0.8510877229282166,    0.8603900589754762,
            0.5863417808980522,     0.3651375649331312,     0.29219396354616056,   0.6399725148968649,
            0.6166369188684203,     0.7025055322259772,     0.945320677587678,     0.5709421494658934,
            0.6491879389472301,     0.22438970496628063,    0.9992061457403759,    0.5991961295332436,
            0.4713474695799226,     0.3451558324576989,     0.06977221898922714,   0.6925857873300553,
            0.31114840203074745,    0.5895698797598925,     0.5651384740091681,    0.4810231124728358,
            0.23811355814026414,    0.2881941328093329,     0.7059334242709647,    0.3862342747127788,
            0.6160680349594981,     0.8745689838607825,     0.7249998063143809,    0.8260889976654063,
            0.9461138156932158,     0.9232592676394705,     0.7002892904118151,    0.32710601094386527,
            0.7609746143683473,     0.9078564121681063,     0.5631487611378525,    0.9337899770870097,
            0.48063525918471484,    0.446305670815657,      0.3212179171227414,    0.28954292649182123,
            0.2989083340288635,     0.9076999853287178,     0.44323555788606805,   0.6137570359274979,
            0.05957581469620821,    0.3366000856124568,     0.08626338501753195,   0.1981234472921627,
            0.015468607014190394,   0.711428938259771,      0.6272950291743647,    0.7586996610501318,
            0.03538306347582876,    0.9175646846602493,     0.050266955519039924,  0.4899910602560694,
            0.9320885678788006,     0.15126804440615194,    0.48185592111675335,   0.8867781217033445,
            0.7613221003785055,     0.47895124104726594,    0.9694583855463286,    0.4380567917172483,
            0.5515965411048586,     0.7090355605932342,     0.020082172176081325,  0.4664782083319404,
            0.37255000056190546,    0.6390226487338151,     0.7741287103550385,    0.6713249469103847,
            0.46180421099794466,    0.9369123295076744,     0.8930325889057228,    0.06830040711617325,
            0.10450093318836706,    0.8511348984748487,     0.36857198720172757,   0.46841500673112524,
            0.7763205487419618,     0.41672253011437543,    0.49546416733663057,   0.23408803714483795,
            0.18046233402362,       0.9647707995970993,     0.2510883664168052,    0.4268286869585586,
            0.48451540528216774,    0.4106897791256524,     0.4748865054377208,    0.9434726709393102,
            0.6729252617229817,     0.20310850702267869,    0.15682597663649578,   0.62938621985971,
            0.15725016939867842,    0.09316966247617287,    0.549807658252104,     0.5684581247127587,
            0.20500962025762381,    0.9915916240040688,     0.5531838019327445,    0.3852681875538304,
            0.6775374708647425,     0.7392698286551905,     0.7561965802565661,    0.7567310776187554,
            0.04107195111209694,    0.3445470410752436,     0.6832570305869778,    0.48694540992900726,
            0.15771988453181485,    0.6835160196635619,     0.0036545237159456567, 0.7687750587858643,
            0.9093054397902908,     0.9000873869222972,     0.04612756479018432,   0.12024135939338387,
            0.38355819135720626,    0.5236485737785965,     0.46217373634580383,   0.6609297018983059,
            0.8472810383305002,     0.5699403920894351,     0.11094595650523131,   0.06599277825960193,
            0.6945660078349536,     0.08000264493762377,    0.9827103790893303,    0.28084947143076155,
            0.6886879834680154,     0.9375709894211569,     0.37119994684545343,   0.5597294497456566,
            0.18023997510214618,    0.688570379150127,      0.4639876785472594,    0.5926089871510987,
            0.9232878153550489,     0.5725783530978408,     0.6253201890717913,    0.014828846321437661,
            0.7737956195988298,     0.2256510137083334,     0.4048140687087728,    0.6107685042002363,
            0.8479585538864394,     0.8459368003615639,     0.5526429704526007,    0.07981021620041762,
            0.3838978361979044,     0.7380890240924678,     0.030016701710504856,  0.8231132870279131,
            0.5552248637279988,     0.5783449981540453,     0.583719140387268,     0.9547676329852879,
            0.23881420993413582,    0.778512530403879,      0.05354384989684169,   0.8847758901423215,
            0.0934244491960089,     0.774238663844377,      0.5374889578637255,    0.48204633511148753,
            0.8210247286258077,     0.38115910359161664,    0.4930872626732372,    0.7373114125949569,
            0.749071600404724,      0.9525310601901555,     0.17239367171790387,   0.6524983150843158,
            0.1883390016394142,     0.7664796972533965,     0.5886059333461698,    0.3726485840482904,
            0.1982230480138164,     0.053283265509501354,   0.5398565851751425,    0.920321661414938,
            0.6735057064793921,     0.7910033010070866,     0.8034306781765964,    0.34277323016501215,
            0.2392219276306432,     0.7395680154657107,     0.2831464084076808,    0.51880802069894,
            0.4433954778399597,     0.875451260554618,      0.9818288893780782,    0.04268041015526758,
            0.8377873472053925,     0.3428193910756936,     0.909474114340329,     0.42493221730642494,
            0.21916978989867908,    0.20703643858326093,    0.8649183337398252,    0.5239684532528621,
            0.9620475523921062,     0.19871651067512663,    0.245484795900776,     0.5030573450324527,
            0.8917145830169195,     0.5435086448826065,     0.38073496719614963,   0.30738859751223413,
            0.5737258816002603,     0.5838885804555886,     0.6644898987869697,    0.03517234746272013,
            0.0779590648010553,     0.6367520271705688,     0.023808817988623887,  0.5318755407449008,
            0.4501402644966118,     0.6380617707736033,     0.5195017737746633,    0.3697355227912761,
            0.06308657862797962,    0.5437725978434391,     0.14348257182809443,   0.06706542038454377,
            0.6040834989081485,     0.7237275679084376,     0.8271277268879631,    0.9728583952899327,
            0.19911747044977235,    0.34662098268578245,    0.7896168550923706,    0.6435219209311089,
            0.22885469139976344,    0.27319066103448,       0.8583138529676103,    0.6530818991518622,
            0.9677679679696225,     0.6340184685096437,     0.34358485217302726,   0.6642884973031035,
            0.23606262960647395,    0.752463719835488,      0.5554193759757942,    0.6280018159939361,
            0.636714735330071,      0.10232072322268781,    0.8031954240245469,    0.3247671057065975,
            0.16757767946973934,    0.6131604919342938,     0.3545136595911871,    0.6423854308054646,
            0.09010831777383399,    0.7379472997811976,     0.9831156431921086,    0.09468959884560968,
            0.9250640861884252,     0.18821854382262215,    0.18658511981815096,   0.3031526078882073,
            0.2599814779348334,     0.7679035779418721,     0.7811510947555005,    0.07002568323764746,
            0.5057485754046723,     0.6455864546558661,     0.5639153676344986,    0.9598886242996483,
            0.6590570558986457,     0.4720045565622838,     0.3979933888060523,    0.6354128243382362,
            0.853369168406155,      0.13883221109673105,    0.6012323448672612,    0.2692879179933705,
            0.015932180529431128,   0.2799709394365537,     0.41923148783036557,   0.761373284300726,
            0.4438616719837132,     0.47117551223596643,    0.2134267740572383,    0.019986438589841726,
            0.946450191651161,      0.6493837671450003,     0.5890143809234979,    0.23465768525359387,
            0.8882229211011063,     0.9636191409761834,     0.11640288549105582,   0.946324727090255,
            0.7832061418598538,     0.6639112603372258,     0.7029478771989037,    0.741496242989764,
            0.1934853793228457,     0.9204462630770868,     0.3817481222895054,    0.4325391502898268,
            0.966715327641235,      0.3413768474439476,     0.7590968313541181,    0.6302446925592264,
            0.12668193372517922,    0.8366680209877133,     0.06383146344003943,   0.9345727159916446,
            0.5317987385390454,     0.42603907321225354,    0.2697751953341604,    0.8513654833620978,
            0.2256362970349669,     0.12039615374778923,    0.8441887167171969,    0.15540089962213055,
            0.4748619138165948,     0.33477728403164086,    0.660091990030405,     0.3522092487374039,
            0.9605347549769048,     0.891399753960918,      0.2667574128603796,    0.3847356369384093,
            0.2662946342494672,     0.06700601468860212,    0.461432738334131,     0.34271726924061674,
            0.1228510557956567,     0.4760640520035494,     0.9034353000751758,    0.13489956166553319,
            0.3102163593063858,     0.3935082851993792,     0.158884209120577,     0.3297299781212023,
            0.08195531804747669,    0.2845637361857394,     0.04061419519436171,   0.8251679044404158,
            0.610107184515523,      0.6904074215497966,     0.14416008859316432,   0.9752875326476546,
            0.7870931714812182,     0.2857860169524743,     0.0033525203769441125, 0.21291862412918294,
            0.8326500906680618,     0.09167280498858721,    0.4970430300918708,    0.3296522732465922,
            0.41693139177961525,    0.2870831433408576,     0.03441309187131092,   0.727453316826077,
            0.8703418572628424,     0.11777039524549027,    0.5395813068325068,    0.02091846823133625,
            0.04047698387022891,    0.6807515712419521,     0.9479338946535115,    0.9524131443818261,
            0.2890352880519952,     0.07238105417765683,    0.43137329060552376,   0.6717730313796278,
            0.33044131834912516,    0.9236197373166113,     0.4070099273863388,    0.5037609532692949,
            0.5248114317054952,     0.5545078754405889,     0.7920784213773897,    0.36187902535390026,
            0.7080061896039107,     0.76656569470362,       0.9562497687315548,    0.6156124430633811,
            0.7816804933201404,     0.5776656571950493,     0.39959490625487715,   0.7196095213653716,
            0.8118950018803841,     0.2649043135306659,     0.8019486090309614,    0.6722921313994507,
            0.33993547182519346,    0.23104529434742094,    0.35958716954675896,   0.24459953664727552,
            0.4908977644337984,     0.16909537785775464,    0.04424793368768476,   0.4885499036923432,
            0.30915635275400666,    0.4537333984846085,     0.5333048616667747,    0.9514913923577094,
            0.5439305964434474,     0.036786459912506286,   0.9884859311579072,    0.5218434362636359,
            0.019965015556903443,   0.20086310447687517,    0.6296270725895466,    0.9842048731080741,
            0.40286398653342537,    0.3830961321320234,     0.7719466396744851,    0.047165606537411975,
            0.789320231199929,      0.5052885078366918,     0.9129221129500353,    0.3319239897534646,
            0.8236419951534456,     0.9403590884242866,     0.6158362098139302,    0.7584830889199611,
            0.8522707703972959,     0.0906376785865346,     0.8140780830136866,    0.5054216376335434,
            0.6800525887899218,     0.6888351575201161,     0.4272965708307387,    0.35309087474123146,
            0.7630276443193669,     0.1585598683870718,     0.7491552790187822,    0.4932155468413121,
            0.9257005501844843,     0.2741664603809407,     0.4506298584279679,    0.8335201848903918,
            0.5465767490059674,     0.5602912735981056,     0.49190857235443075,   0.45622390698234816,
            0.7802158188871214,     0.499155492580064,      0.09103123865265628,   0.8805914898774542,
            0.9098787481207782,     0.9765332208976073,     0.7821948171762165,    0.13224731365814824,
            0.38179226430430935,    0.19624872266741666,    0.7348156848191983,    0.7505367338615669,
            0.9173916431437428,     0.38373456776660575,    0.21730126096712377,   0.7084627514531115,
            0.8226960992168382,     0.11244309088575566,    0.24980703036524965,   0.3833871399394828,
            0.1107681331784337,     0.28990869260993357,    0.23023324059529593,   0.0824186734981005,
            0.40069946794407274,    0.004836216027325313,   0.860694215568036,     0.1967855834010267,
            0.8540660880593619,     0.09488824279522046,    0.9316356273318933,    0.24002919622213992,
            0.23694014325207413,    0.07660790438716625,    0.7346414333313286,    0.8166020264437737,
            0.5322892269551114,     0.8167017330574166,     0.9071014451653882,    0.9403203305035448,
            0.2911236155631133,     0.6740891711622454,     0.7381132217125792,    0.20559227776763078,
            0.14708226828706616,    0.12494659941696551,    0.4910773994830627,    0.09984819286125579,
            0.19328817936099052,    0.01978829113749081,    0.41301529103022916,   0.7854914635679247,
            0.9991389409218714,     0.908953964758846,      0.5225819662845984,    0.32782875443955517,
            0.7517635620466835,     0.2662017485341044,     0.25924197193203635,   0.4945575810550099,
            0.06597070182866505,    0.676940856183024,      0.7789758123751029,    0.9695066811579602,
            0.2914529432070131,     0.19217873712155265,    0.3570129706073306,    0.7723774765477364,
            0.5805704352706412,     0.7953107782357461,     0.057878873393940045,  0.7403069874803593,
            0.37911141227174894,    0.8931220427257193,     0.023619975552459538,  0.5398322405277811,
            0.7698849312170793,     0.8447166821910701,     0.7485756073717695,    0.5099173191007011,
            0.5229212989954363,     0.5810602605117828,     0.7282652506332697,    0.18842457574228533,
            0.15250920093124964,    0.9543321216637011,     0.6858381185157816,    0.9182655334384784,
            0.8450976592784886,     0.12368686118131222,    0.6888867123142767,    0.3062447694584498,
            0.2991486576902811,     0.1884960737501532,     0.010203016875596438,  0.53974930460702,
            0.08590045224740495,    0.4530392360955344,     0.32656556864336506,   0.4673861133086109,
            0.713838543006717,      0.43352356016341,       0.7607461135837655,    0.1507712055834748,
            0.5388510625547493,     0.4912068297341492,     0.1693003550119253,    0.2705396304911616,
            0.9355586536795282,     0.3260608590055567,     0.04816464436091039,   0.7047534238822833,
            0.6421228811682665,     0.5225194021575739,     0.8313096205048437,    0.3181132630854606,
            0.31579401277335073,    0.500740442377846,      0.446340062394746,     0.12883963860120773,
            0.7960679424504243,     0.3293462959898794,     0.6827864548119956,    0.5450114666824731,
            0.7810496036770709,     0.903025450466452,      0.4446943992479814,    0.27543489167922697,
            0.7225159421831087,     0.8184770722851453,     0.2619256419896915,    0.48501725546838426,
            0.09625144295563248,    0.9377218936533477,     0.6269704806135408,    0.25251367520544277,
            0.5379050294431125,     0.9777390088992477,     0.8922712611535861,    0.43061620953887836,
            0.18220002948565273,    0.5871470819511885,     0.3906010318573543,    0.7562445920564482,
            0.4632623124636699,     0.6441053388678187,     0.7786547704763063,    0.3639719457921242,
            0.03227375544695266,    0.6522258538744792,     0.9355362919340011,    0.2068183301780403,
            0.21000894169481688,    0.8749400902645131,     0.25256883320256773,   0.4356078850766878,
            0.09750260363736296,    0.550752586854734,      0.70355992438061,      0.7512486339369331,
            0.1845058005228739,     0.13945887954954372,    0.6093130377717191,    0.46420353569880046,
            0.6835199564235261,     0.30738452590033893,    0.8742105783565188,    0.21457207406794732,
            0.018167347462280214,   0.31261461146762204,    0.6315950660091322,    0.5729616651969766,
            0.8629945496969306,     0.3017522992670638,     0.18821416638211375,   0.8528020237511038,
            0.06370803555111781,    0.6660838032458873,     0.6378639794749231,    0.3334419280741796,
            0.8322912189829458,     0.280026944926816,      0.9520300354363374,    0.12791030845510665,
            0.2534077505432255,     0.15194280729939358,    0.13421816561244282,   0.38613124073213934,
            0.18651141998270038,    0.010402589079757885,   0.16171435952712698,   0.9368518641763011,
            0.7839659530504567,     0.1755627210158469,     0.055370312290232815,  0.74607682373267,
            0.21566443454913187,    0.7733302254016793,     0.28713023864838905,   0.34537634496983916,
            0.11867444635225599,    0.24422446758200433,    0.6058777545966703,    0.7375392464212742,
            0.6298138841112356,     0.4320743309455569,     0.5245113168857385,    0.5073628481028901,
            0.10090946852275928,    0.682776976496594,      0.9179055782451095,    0.25439855333827033,
            0.4096257747680766,     0.4727131165157603,     0.9115103389793892,    0.15292489841876844,
            0.15081671692177911,    0.27285129646047157,    0.13746358292333571,   0.28096820730540806,
            0.6235755869311604,     0.5133999369107294,     0.9954347732150087,    0.13902084460160313,
            0.8492414381091979,     0.9092769614503873,     0.7321501874604914,    0.8703118390759484,
            0.009828954589244265,   0.7127149585835475,     0.12254833940961174,   0.6841838706090675,
            0.6797898678564891,     0.7617711949099634,     0.8867391934944756,    0.7510802090879748,
            0.2182687354342141,     0.5996413031330944,     0.2495219488317163,    0.5074304285500727,
            0.9630305833717229,     0.5003879980597904,     0.3660156716704823,    0.7373757695789731,
            0.6122959717204958,     0.35009501397735077,    0.5348681087680487,    0.10878172778354811,
            0.9722622130233555,     0.6475127320547724,     0.4843536830262407,    0.3829121220739766,
            0.694985640799419,      0.4943348703751752,     0.44104112402752393,   0.8112753520850255,
            0.015051745929245297,   0.23464567779627732,    0.5998022090031062,    0.939770680156893,
            0.533962078997285,      0.39823394015738334,    0.4224836445078354,    0.3355259195981163,
            0.04567579881080175,    0.17664618939821342,    0.4124880459451128,    0.99019933361313,
            0.4086955523216489,     0.4953086452271196,     0.3101415459625855,    0.7353574057188267,
            0.5057581829754964,     0.9067631351878555,     0.857896372901816,     0.5259515699425463,
            0.7907037623687696,     0.24633864983957798,    0.958613383922992,     0.9536652702291899,
            0.12375162740685253,    0.897529159644056,      0.7522813231613783,    0.1815177520813216,
            0.519955178358722,      0.5652852399637138,     0.8131506573398954,    0.2997147074588975,
            0.9028274140651887,     0.3077460655698906,     0.9707223948908753,    0.2508338100704173,
            0.7942044110543858,     0.7331530249159441,     0.4136871809355265,    0.08242300808783987,
            0.4398627661500544,     0.9039317408982772,     0.9366343786349624,    0.5587802596227949,
            0.5153129805648125,     0.6462556860488188,     0.5775456450004183,    0.4707109021904742,
            0.6270548880780359,     0.9350700949289592,     0.7715195681069944,    0.6619214085798729,
            0.5369652107270392,     0.4585399444105942,     0.9331567039223996,    0.8997969906972278,
            0.8013872239286973,     0.7991632835150955,     0.751574676725665,     0.850128778616071,
            0.013911508478306245,   0.9827400167059904,     0.9089085891503617,    0.09058671152964304,
            0.3661973962815184,     0.244121820766233,      0.8997529619424489,    0.3693548470994905,
            0.594102047657788,      0.09752157303401265,    0.02117650643721969,   0.18986970609553888,
            0.07777350856700349,    0.01904573908337026,    0.9604118276595635,    0.38843712631739946,
            0.08297596525163997,    0.04035755694223486,    0.3145819492442985,    0.1055976227385077,
            0.054878105918033215,   0.3956323792204476,     0.8004877339674538,    0.03238620302347428,
            0.35131899136962963,    0.7795753260195983,     0.7572307446991025,    0.49778067799816084,
            0.05733556369949788,    0.8395814334948822,     0.8136871182676774,    0.7117252857825732,
            0.09569503688724135,    0.5123443211159915,     0.7671663718210584,    0.9455302466271156,
            0.7403784074209128,     0.5241201235386309,     0.6487635799916047,    0.987967924849385,
            0.7756042452359231,     0.5374466568988294,     0.17642898129508988,   0.08897407119910983,
            0.8312141647459399,     0.019699242017796048,   0.0627663071534148,    0.09886805827280087,
            0.8948561515283756,     0.6166313182506137,     0.7007348882992398,    0.13157354849063208,
            0.400328020714128,      0.38279627065271804,    0.5063261002940321,    0.26141926300809104,
            0.22626615071790923,    0.9566182719807035,     0.18824446159319685,   0.37836467510653504,
            0.7556814884088183,     0.9131855356489142,     0.49645662437786187,   0.6455061132934143,
            0.07170009230196261,    0.05016726960451312,    0.246136844714963,     0.7801616234699119,
            0.06621854362724056,    0.299192622229307,      0.47863674955616087,   0.054847732663966364,
            0.668997577900571,      0.938737489502215,      0.862391187970208,     0.15444142171095632,
            0.03558629581172246,    0.3233858264378089,     0.06603821928005082,   0.4650165793341101,
            0.5600925753097075,     0.11632169846131202,    0.035942986952755884,  0.7086663205896244,
            0.4051267040626767,     0.3336461399304169,     0.11062929889736461,   0.6073381096757693,
            0.9481483793877753,     0.549008197552151,      0.23057468652252244,   0.37727255424179507,
            0.6168165812004855,     0.9771588132605955,     0.5855561243613178,    0.17422574674814018,
            0.5521098006724932,     0.45393061629285436,    0.3442053516329908,    0.7113666247274623,
            0.22069255640809038,    0.9293921496134889,     0.7251512337662525,    0.19528833682522828,
            0.5925868613854349,     0.9224096624894336,     0.7930540132486785,    0.053977556374531166,
            0.6173659424238513,     0.3166615457215467,     0.2760120664306902,    0.651412265932223,
            0.2954147698615751,     0.7279642492491684,     0.9500540310842736,    0.19541987549040896,
            0.030315060545994976,   0.10381241789090656,    0.26109155712713694,   0.16743343918145503,
            0.3660054619794624,     0.5086532783785686,     0.3031691182421813,    0.9559312010085553,
            0.745857449017812,      0.2917449824444206,     0.8577683453452225,    0.8954180812193946,
            0.32416973847955133,    0.9091882509712734,     0.10389000119727343,   0.2084147111817648,
            0.7251843233807603,     0.8872829326542705,     0.9416226842452367,    0.47454810889606125,
            0.03708508106659658,    0.5612807944860695,     0.9207934982448776,    0.3690793914944117,
            0.9353759280516082,     0.5535798540475436,     0.2980243748243394,    0.39153791581624053,
            0.5924366962050152,     0.7452724291560255,     0.8942833573498825,    0.6237508355554183,
            0.9160079480828767,     0.20860605914559727,    0.7962194634630373,    0.2964313431680926,
            0.08225388546525936,    0.02404126472138579,    0.027414830160852,     0.701091972176009,
            0.845402897055724,      0.9623105696795856,     0.7221929001820542,    0.07694316931802314,
            0.6148512833513421,     0.6242010634659172,     0.14108133081161,      0.6739770687966241,
            0.7434124504626559,     0.17692730499742315,    0.35054310800005617,   0.25598689434626665,
            0.840436004348755,      0.32811108039111647,    0.5335790001265612,    0.0367023334215697,
            0.11434700924632879,    0.02830433776542396,    0.8786642491666433,    0.7415723930220833,
            0.7029743248725108,     0.6885049939678209,     0.47410612766607174,   0.451332581907825,
            0.2471246483800431,     0.705495973341043,      0.5595049075229553,    0.5428842741209942,
            0.27563765922292094,    0.5121487865852304,     0.2038375556652976,    0.6086603900392263,
            0.8678569402224703,     0.3322341570669234,     0.8860578777712762,    0.15032365906523515,
            0.5220672022410702,     0.6721320659029598,     0.7179995563835297,    0.9863298822247657,
            0.14611327394893148,    0.636158167795051,      0.15016790490419085,   0.9293195820487136,
            0.20823760292228488,    0.39118356255561604,    0.8478325466306105,    0.6238665869776472,
            0.38892477643691836,    0.1947909169703793,     0.8193140109470651,    0.5916604624684888,
            0.6617957218101335,     0.9523344606762081,     0.13791785859478323,   0.42166564581206156,
            0.06395650815946285,    0.8547168218029992,     0.811467471942253,     0.956662340109981,
            0.21135025812700725,    0.22776355710904816,    0.32873639923759523,   0.8225828581220365,
            0.8074366260503684,     0.9253702522252192,     0.2870783243664534,    0.7482535627860125,
            0.7419726164577817,     0.6800647655401266,     0.9306967856660886,    0.8828274999806435,
            0.04603333283775202,    0.3976081728051163,     0.15712718412831284,   0.38232962704820084,
            0.0014683820835882377,  0.24999329698617978,    0.09341463590013122,   0.44004159788156194,
            0.8826032379171254,     0.2595692013354478,     0.9323031954938713,    0.9222566995715802,
            0.8263635903912544,     0.19333393658373543,    0.7785983346877806,    0.2135325812843324,
            0.8868077223754203,     0.491788787841855,      0.5116456034509074,    0.5159231461833037,
            0.10975820478379894,    0.20576944288946197,    0.10877963016060477,   0.33408070142868684,
            0.8855755648422882,     0.05980323401628351,    0.8637078748398969,    0.5638198307471801,
            0.21125906686561158,    0.9930186890004368,     0.36552484848753086,   0.6227989212622395,
            0.6409356312529642,     0.7715939125793486,     0.49335270791885677,   0.30916962880672894,
            0.9558466760608072,     0.7157582382317161,     0.8479859401297832,    0.7436472908268735,
            0.8374501847853362,     0.2687173784642233,     0.3719318154071177,    0.16145721576903904,
            0.4786753717548976,     0.17658818617987182,    0.7253782046989563,    0.645246128489611,
            0.9466051206847397,     0.640137228021984,      0.5722841875990824,    0.20389663712145034,
            0.8545298024337249,     0.7380793932715825,     0.8215149966764985,    0.6104461557670458,
            0.23442940460882034,    0.6123691691031152,     0.7437144857118246,    0.17825602719378109,
            0.9357111995980801,     0.5216438852971895,     0.9326346419795244,    0.7769113405672327,
            0.8671915782708538,     0.7101426036117587,     0.20192446126559493,   0.667684595450286,
            0.7561302989575927,     0.5299678589610611,     0.4740460810184325,    0.6678976506603213,
            0.18982740325987535,    0.7754815258209162,     0.8912418547340085,    0.8582391712715519,
            0.5234959384184952,     0.5356656573964478,     0.08824207817225227,   0.2546587872233984,
            0.06400271843996796,    0.14795991786070883,    0.7223041061317048,    0.45461459835409834,
            0.23739663595483806,    0.46533302061863,       0.8797590379500149,    0.9679415151155805,
            0.4950826473640618,     0.6364019081732267,     0.030230016743837962,  0.8385725904223664,
            0.5684519872302574,     0.8072602201092485,     0.5248618191856367,    0.8852925311548213,
            0.7482260141785877,     0.9713616877171397,     0.1420937958034003,    0.8017407154294148,
            0.5106911663782366,     0.6098164389053784,     0.23115798242299546,   0.8509385790786431,
            0.7665772983399602,     0.9393098407465904,     0.04209603234663706,   0.9771297635499196,
            0.15358359790476628,    0.39352418017453883,    0.6202102266129389,    0.1335429286194626,
            0.63193489864132,       0.5831564534087365,     0.8776136257771191,    0.9039207390994624,
            0.8464470187024711,     0.22015445409166434,    0.8758007877722682,    0.5851239989684559,
            0.5898595680926785,     0.04071547690235877,    0.8253457187844906,    0.24325808257077552,
            0.9996262268885948,     0.3691614600340126,     0.5643614269762219,    0.1630823145336715,
            0.9588420377385642,     0.4804879795691205,     0.8226960511587661,    0.956060073252254,
            0.01659088473297099,    0.22739377608187905,    0.6706579158019068,    0.6063368383867996,
            0.11763113616922216,    0.12242279151794855,    0.9985661462571744,    0.36903342899318914,
            0.39869687250042274,    0.7187090588440607,     0.8475847475235313,    0.6553391141005228,
            0.12863123150038558,    0.5836869686536438,     0.3522864599361549,    0.40955837698338127,
            0.30611366257419803,    0.08414039290708708,    0.8336975233383931,    0.6738773074893865,
            0.6428385059166033,     0.8907033084159207,     0.22959797284571337,   0.7369959079185738,
            0.764559332056781,      0.8460897600611726,     0.9342745878169123,    0.6797930366609867,
            0.2590611993749421,     0.18200367952682905,    0.5398052073548281,    0.6135052126501889,
            0.9629087642347781,     0.3502142894623047,     0.8565328095187749,    0.30366728820722133,
            0.8649565171904231,     0.30627643129486304,    0.5097112506135479,    0.5395615475838216,
            0.30123127472496636,    0.3152643339579706,     0.623404055585028,     0.9681245535183327,
            0.6878256269558346,     0.7011734214993312,     0.5586441073144988,    0.0379074017111003,
            0.41196503601278955,    0.17326048618444756,    0.08967405677495122,   0.16327890416213398,
            0.6067354530243341,     0.05574386708342938,    0.30702773695791663,   0.3591315779204586,
            0.8485003205103784,     0.7595345211816413,     0.4856754427037272,    0.8172224268993479,
            0.9877879242491928,     0.5924825579327764,     0.3618245110082776,    0.3548864109202763,
            0.9762786876976958,     0.36117544164029625,    0.9384918355587047,    0.44482421907262093,
            0.4512444243209993,     0.2942412676670435,     0.3520183660479801,    0.59524292074513,
            0.060313488971640905,   0.5664757004758704,     0.8004140890203537,    0.049739496565150754,
            0.8326238008556021,     0.5231451049952724,     0.9589558280589826,    0.4688736120635163,
            0.6457270091130933,     0.2743253316453027,     0.9817226612551712,    0.19214408906157499,
            0.5365377233133565,     0.21937798392084207,    0.4474268422020008,    0.21332809147586096,
            0.45152629216338536,    0.3954384470090013,     0.08389248957965645,   0.9456696894285853,
            0.5455269272443676,     0.7162977033121736,     0.9921059770406268,    0.009140458313235733,
            0.3068422204833988,     0.979535754019835,      0.39829763785135963,   0.9535422055318536,
            0.5234518862510418,     0.45524982399842284,    0.20405093144406117,   0.07696699410493557,
            0.9804917453968658,     0.7825096762276561,     0.6844391970560416,    0.06298248031832787,
            0.7065637824445677,     0.8624628480867429,     0.39673398563242634,   0.8039480405258369,
            0.11194534090382247,    0.3383757894318189,     0.4474095215262046,    0.45858194942102426,
            0.614589845977242,      0.4245550119524921,     0.8932963158388915,    0.018992805669430957,
            0.273897431447773,      0.9056057865446592,     0.13748729687735983,   0.703547358197167,
            0.4175471719258016,     0.0891231092090986,     0.521908149470029,     0.6181832510616906,
            0.5162935080620106,     0.006874159381718203,   0.17212747826130959,   0.9220698879129372,
            0.3572613445519437,     0.013537004339399195,   0.00332288167478878,   0.9250367428697359,
            0.5502732509918479,     0.4397655101560085,     0.976601928543793,     0.06587254795860342,
            0.9967676011516247,     0.6905977258713558,     0.26360736296776377,   0.7377790501676503,
            0.58949493561779,       0.6804985950100816,     0.5981458303119022,    0.7703354565435662,
            0.6627016653960298,     0.17224546603828572,    0.7082205562153289,    0.9359161836117396,
            0.19886404832129612,    0.29836885703887706,    0.6473082353319027,    0.19284360062511752,
            0.24025878824616786,    0.4614850648177867,     0.11502873683838533,   0.7526668882653277,
            0.22760724539997257,    0.9202846102044087,     0.25550938435415715,   0.35544012534438074,
            0.6126980217670746,     0.7890785075351323,     0.43009962728385065,   0.3086886946583104,
            0.893588207570993,      0.17696739805360007,    0.05815134452806059,   0.4915140236220875,
            0.1964501214583112,     0.39365076296370527,    0.9805756394782623,    0.04868204736327009,
            0.7852713192874631,     0.5662294612024402,     0.1438711221710759,    0.30549087894587446,
            0.83700883167725,       0.7583273991302517,     0.8360132064134883,    0.5700849208188266,
            0.5481702973600299,     0.7511787034407322,     0.36120915262371334,   0.132261472961134,
            0.6686710290333494,     0.43826121820220365,    0.27632276417904833,   0.9008174856418658,
            0.3132578975392324,     0.13788308650145287,    0.33012548777667416,   0.06369117167103067,
            0.5615609397503767,     0.8469336604603973,     0.07515128644039626,   0.7282956188244899,
            0.6062687977350211,     0.19154058595916068,    0.051760414269677146,  0.9321707816408611,
            0.6388059391959006,     0.16067973027254356,    0.49671002197682934,   0.810464225248854,
            0.6242380199031319,     0.05822510543252912,    0.5859719405339411,    0.5369842479928887,
            0.619865713972886,      0.9469581122505808,     0.4840236543656491,    0.2446590918317264,
            0.3734040462532573,     0.41153241114773353,    0.6722644245654192,    0.9438265055904,
            0.6151662338440665,     0.3197496864519831,     0.6445709776773069,    0.056224428769436674,
            0.10088475920050821,    0.24889628578565504,    0.7738240305795522,    0.03380264814176548,
            0.36631771582407024,    0.5995615466962256,     0.7315869652859625,    0.012710303153376956,
            0.08364010672389866,    0.3706441905455059,     0.9373563883673417,    0.9875599034399782,
            0.7509701583207454,     0.02864431810531165,    0.3335893834931921,    0.053975729619853974,
            0.7492542388599975,     0.4236636780056169,     0.6489993496635825,    0.4757128477406488,
            0.8501970384373653,     0.1268046479893734,     0.11640206411035037,   0.1802986721722314,
            0.8453208273446001,     0.8275576269506577,     0.6147153087971005,    0.1175291803443439,
            0.41477625112760397,    0.7704409147534934,     0.3533472246534449,    0.7043999684517056,
            0.775247294967955,      0.9680923381195076,     0.07249110207318155,   0.7134446243960051,
            0.2112924294650559,     0.5601033124031858,     0.8351535085778684,    0.6651260922739445,
            0.849093636443096,      0.20523992884744047,    0.6985207565901812,    0.07930679838337618,
            0.43241274678741615,    0.21381459398456937,    0.0519328953800301,    0.05624972539580908,
            0.05839395149548465,    0.4433522299538598,     0.3895892892400896,    0.6261753770716189,
            0.892302431029592,      0.13970306225988627,    0.1456114428839015,    0.4950650258719298,
            0.545653270903222,      0.006004480737930917,   0.4208035646201753,    0.3573139364844512,
            0.7434131064312294,     0.06236410776831858,    0.6221567609082567,    0.9748682253215738,
            0.07931212068682203,    0.3545797843007681,     0.014486999616720397,  0.952942242408267,
            0.702288586815865,      0.9409255416841286,     0.24827063949386452,   0.8583481708963405,
            0.8646466524926354,     0.9449436184654777,     0.1379156671442492,    0.39301636473189094,
            0.6940708441909698,     0.05064455415602809,    0.30932198343863126,   0.12439893551457126,
            0.9725969293678293,     0.22981000064090706,    0.42793450167504743,   0.6240132929133849,
            0.41274173218145593,    0.10829569501663205,    0.07595924640852625,   0.7891772556987178,
            0.2029382856735208,     0.9504847151745953,     0.25946997333986954,   0.1848810682149351,
            0.6815257687996995,     0.2822698882235799,     0.6119945853180316,    0.032081347455592435,
            0.740984443711812,      0.38272402393289806,    0.21738260099766638,   0.8275875488973528,
            0.8553262745989765,     0.5822720377847401,     0.5554500638376147,    0.7136089747266824,
            0.8617301311847095,     0.11174941727880727,    0.10998531598519001,   0.11579210053669431,
            0.5112942412426882,     0.40233180692142756,    0.48360045805782803,   0.28324739831875256,
            0.590023394006501,      0.6283584737073288,     0.6101317601514166,    0.6823463906868024,
            0.6878986835866159,     0.055622664124902355,   0.5721668333725881,    0.8310677523242348,
            0.2354956414595888,     0.3921367771656503,     0.301710577021835,     0.07982569798898997,
            0.5844595931256942,     0.010606427673007457,   0.6792236211085227,    0.48711598100376485,
            0.4186848389719604,     0.1520205731322931,     0.7038373585767068,    0.5786547828151765,
            0.4501356701602913,     0.5940159476490812,     0.054130408441241396,  0.5340508484006485,
            0.14563077352160447,    0.3467809498492218,     0.015245119191614198,  0.11916804616842058,
            0.6442054337019893,     0.22110456957047342,    0.45760836940150784,   0.40035833791514097,
            0.39736125725439,       0.8406222554440334,     0.7480051824691096,    0.39607450326184934,
            0.6169601784227084,     0.9147752952196849,     0.528942392721163,     0.19671595910585504,
            0.7689629632964102,     0.5857627989010331,     0.7281758790847451,    0.47549678474268886,
            0.9771515487528412,     0.774362181908109,      0.5792613657580393,    0.2915181063192954,
            0.48129611722341215,    0.16842684750387993,    0.4322435813410115,    0.37706028162465177,
            0.5275190748603705,     0.19388516254113952,    0.898894215990235,     0.4105507981093036,
            0.10183344441151243,    0.207244497511354,      0.9936018501009102,    0.6822066970703475,
            0.6097103669992625,     0.8596785426464303,     0.9737261311085316,    0.23384606795789475,
            0.469228370203129,      0.3150856919691406,     0.45325584290765897,   0.3021481446971995,
            0.7359263296519397,     0.1041928137278838,     0.5392024266393926,    0.06796476537924911,
            0.7553123488967057,     0.2747219889480731,     0.7448023725235665,    0.7267290806878393,
            0.154584743104079,      0.1426717670246317,     0.8522167288462654,    0.23101453763592739,
            0.5324589078725956,     0.6001934763867139,     0.2841668620399004,    0.020418692597819477,
            0.4722662627404567,     0.036155325406367345,   0.9978720177345408,    0.9098451173551634,
            0.5419285071156141,     0.11797572280199964,    0.048379473113458804,  0.4672711193136607,
            0.9976039161475324,     0.9060066652367704,     0.11916027567553478,   0.35622097880083825,
            0.5837853249886084,     0.8673768649987554,     0.032129972124176565,  0.3178381599419763,
            0.031188770237290275,   0.6740117489211176,     0.3874682439750927,    0.6034438219410512,
            0.6976344752830447,     0.7235262307756359,     0.04479657754166555,   0.911151772773858,
            0.15080123773789078,    0.589525284901,         0.17840564644069878,   0.9373110675973576,
            0.21559324070959918,    0.10706404774727907,    0.5898301805791695,    0.71684443995085,
            0.3955748642340061,     0.7195496822110167,     0.012350906723061605,  0.9904757588978808,
            0.6105136929764267,     0.01682024403986948,    0.42501736700329673,   0.21419171941390525,
            0.5487351165612928,     0.0023875126283702253,  0.6827750954806542,    0.10490738049003712,
            0.18497201371089644,    0.670811504068588,      0.2927928640755091,    0.4998383395178182,
            0.22808468769155055,    0.33078188807354403,    0.12880091194857413,   0.02885265498228462,
            0.47935276328861187,    0.0273501250248277,     0.6184719965891724,    0.4484981019599926,
            0.16317110109496624,    0.20872944653302772,    0.95201336175559,      0.7963354270919263,
            0.7715986969599571,     0.403482508281638,      0.9297256200098162,    0.29986859030807567,
            0.45218289423576097,    0.9159675492849336,     0.7797312318801557,    0.3292487117573706,
            0.8932268863243941,     0.5440300092785846,     0.09194090420757495,   0.07879232640967515,
            0.040625433564198565,   0.9503034730149118,     0.15328940647971256,   0.968374806319513,
            0.9130639483136002,     0.8305563280949961,     0.7430223959392527,    0.8468642149605811,
            0.9441514991775032,     0.059431005272214454,   0.9445956362139237,    0.8470158973052053,
            0.41664160482788337,    0.8994160293188886,     0.6070162264314966,    0.36164423155689573,
            0.31625664936558784,    0.6799843043260322,     0.6136959611617626,    0.1301268735001968,
            0.02377007352486482,    0.7932053225469253,     0.2836196329545091,    0.0033773633064905217,
            0.9345825605764154,     0.19414282257909432,    0.9248060469917657,    0.7165778920127337,
            0.8753653449773379,     0.643516949277003,      0.7382024233299749,    0.7680237323978785,
            0.9901440256760623,     0.6226630404604593,     0.8449432524291013,    0.6923742172151705,
            0.2781587066872502,     0.4957407497415651,     0.4855832772459078,    0.31708832143366916,
            0.058436697318391184,   0.0683478293038251,     0.5750520306533216,    0.28948034703034453,
            0.5765627757815632,     0.4715704872547748,     0.22788663761967987,   0.7080665572063077,
            0.2794882968516803,     0.20370161645442797,    0.03021991812820024,   0.4876444400214619,
            0.9468853854088918,     0.8320045949627904,     0.7745298637300877,    0.2703193784452338,
            0.19852261953924477,    0.6410566228896608,     0.4778123457137041,    0.545476392499247,
            0.06642829785014648,    0.22189436858071054,    0.138348616122402,     0.1992553717660479,
            0.6675765820845962,     0.7938402926354475,     0.1845485694491109,    0.5220038702812361,
            0.9156628087827617,     0.5171059903078549,     0.7682320826737409,    0.5353184044875635,
            0.2753276155492411,     0.8536998274382601,     0.9574612439879333,    0.8443201396244076,
            0.452736414538202,      0.09814834076597412,    0.7831688382370738,    0.23053173360813417,
            0.2872843483053551,     0.5464667272055969,     0.8339728010161706,    0.8737974051832735,
            0.5791460745041196,     0.035061872524816406,   0.15330405242539535,   0.8833427264594675,
            0.019455499177527202,   0.7765179596489821,     0.4246213814504972,    0.6018063770500839,
            0.47550462640023883,    0.25401536848013795,    0.5276980323620261,    0.9761363781187026,
            0.1067498076808111,     0.9685086403819209,     0.8698169765639949,    0.08316789304990158,
            0.7638977882293679,     0.5973581096783714,     0.6938183139100992,    0.4195666743717207,
            0.06693056686596344,    0.9028690484818521,     0.9702734767918807,    0.6764342486984665,
            0.45625768583112336,    0.9648986798783104,     0.9396506860051668,    0.9538360653045057,
            0.25325193102193755,    0.36031844206186947,    0.3977997550597844,    0.5572149144626898,
            0.9415146703812277,     0.6501691840398597,     0.0810482876250247,    0.46703641936801854,
            0.04871768527153719,    0.9957216498980977,     0.6893381973344791,    0.8880136437107692,
            0.5497225509802108,     0.7422844449685458,     0.6941851504106487,    0.9593676896692689,
            0.5606399742300912,     0.6421050338498311,     0.6233991086362393,    0.012802969408392428,
            0.5121379506441427,     0.6445479385963687,     0.2808629709144951,    0.9134059284125124,
            0.241129912522295,      0.394455873040213,      0.6446088672758098,    0.5904555718899333,
            0.10540634673485016,    0.25908661804810684,    0.7728241403998817,    0.6358154054664722,
            0.04101646728837749,    0.8516622868856863,     0.7702489121694064,    0.5358501365373546,
            0.5980355452050076,     0.6381225444135599,     0.0005730689961515045, 0.970167631347459,
            0.4139198325451804,     0.31278243934984296,    0.47495283600091587,   0.1506419275618026,
            0.606656160038374,      0.7674287071193997,     0.24208578546375847,   0.05657336308366978,
            0.7296473273305218,     0.2800903221949176,     0.3737098785122176,    0.5816524263942958,
            0.7398790543278544,     0.8939526932421885,     0.5351544556658304,    0.563915725881967,
            0.13263723512796788,    0.07480840017856882,    0.888439574882212,     0.26115699832587147,
            0.08869035589692076,    0.31212028688447613,    0.656403249920012,     0.17757508468680894,
            0.1818755206457191,     0.14906764949964846,    0.8318883046189531,    0.8662107933319225,
            0.3764187147705156,     0.6696546881748311,     0.8458197048945892,    0.3110623979033247,
            0.7437629057558237,     0.9027797770544782,     0.8209307165929335,    0.23092644666423678,
            0.8242236924078323,     0.1056270838541351,     0.22714477549988266,   0.20030944013001561,
            0.22947690294329104,    0.696107896913141,      0.1283109954762095,    0.44283684026157966,
            0.9870057925549655,     0.38010663867098504,    0.4995866716824585,    0.05815074830759892,
            0.830644135939699,      0.901107268640793,      0.6674748299363391,    0.8742574761980677,
            0.5410088553711261,     0.41808675211638335,    0.3026323472134641,    0.6228425912179886,
            0.8813533904099847,     0.2014708147991391,     0.18056524943067742,   0.27298410151262853,
            0.5845715257984936,     0.1427309327353672,     0.19563483007643578,   0.6604895077724356,
            0.4328040134627058,     0.5535861275702852,     0.43729116466734064,   0.5785703891701747,
            0.8584977951034565,     0.9287300149661745,     0.3181366808854369,    0.018292526012644306,
            0.8952263154294313,     0.44756393222725244,    0.8828372510820739,    0.7920190417259078,
            0.6316487285191849,     0.47214678451975034,    0.1217236120656986,    0.06956671644049939,
            0.8685768661626136,     0.6366666715394657,     0.7665535561264174,    0.2942000153076372,
            0.0021384733335659822,  0.2705189531723111,     0.14852579879255556,   0.0031594701312605,
            0.8020540104483613,     0.23825907633485832,    0.058572212572497784,  0.7212883579835159,
            0.5100074611405732,     0.722192976008997,      0.819647715163588,     0.7612228919413981,
            0.6876784548058867,     0.753905786906214,      0.39282539072319744,   0.004182904604925652,
            0.48721894979930347,    0.5580422170120712,     0.8774420726435828,    0.8975977440531511,
            0.9451131350702355,     0.41383061060177884,    0.817770323637275,     0.6039487832761105,
            0.987517253890432,      0.05015786167681868,    0.04723660246617467,   0.12377526334074984,
            0.21124698750240423,    0.1322116018152607,     0.7817994842033689,    0.03028219241051744,
            0.3428835864849662,     0.03199451074338855,    0.2973095452202774,    0.49511220520603216,
            0.9018201769429584,     0.0006354099343498509,  0.19421708019059825,   0.6450231697732115,
            0.5296240807773047,     0.5105340545040825,     0.9386296804923623,    0.3161713115563347,
            0.14028515886916293,    0.5387406003641704,     0.9110577932539954,    0.06657870021944057,
            0.5912929046820949,     0.5179400632837544,     0.609261071478327,     0.9117110959165275,
            0.8703690153188935,     0.5497902950456989,     0.9350579496703919,    0.5974582803716901,
            0.13403848834280496,    0.6629294252295096,     0.47010691696034035,   0.9761243559009092,
            0.34289882813318207,    0.8930139064505231,     0.19040777260393993,   0.051124774360734704,
            0.5294072929890149,     0.19410419502989018,    0.9440421479442738,    0.4166782766470989,
            0.3160191740816636,     0.5422214465437505,     0.949524592828134,     0.7619764670221031,
            0.19813663447896934,    0.9358447108348422,     0.2258479781524465,    0.8369816032225623,
            0.42371677122009965,    0.4927493002445543,     0.6460682076138372,    0.39918844597280256,
            0.18909188427557178,    0.3883802426788604,     0.532017099455421,     0.7441909286934728,
            0.23403353559202356,    0.061046748830294706,   0.5961221506465061,    0.8395904615929172,
            0.6457084544821122,     0.15311372620329644,    0.4373881068440266,    0.1663769055446913,
            0.7126706591764523,     0.4388229790700475,     0.6019044757231076,    0.3174900094703226,
            0.7373903377263857,     0.3696894074493522,     0.8631994093593957,    0.10456056343761588,
            0.5682302022515529,     0.11036687014440916,    0.1853534915477586,    0.4714290617030107,
            0.021146922089727016,   0.4992025281859396,     0.853745244650295,     0.029389637234443766,
            0.2376360327147291,     0.8675121158430259,     0.518866279317268,     0.9581510765966711,
            0.3061391874518814,     0.7178121131464243,     0.42959687668058044,   0.8030624178476544,
            0.1779311926645154,     0.9406148630475923,     0.5988386283208718,    0.21336034177209195,
            0.8458220171084456,     0.6518563580968002,     0.8151656534406012,    0.11530676312167254,
            0.8549342316974974,     0.12026560805600395,    0.8825419742135456,    0.25306197060654323,
            0.6246537536440278,     0.9810507533303324,     0.08198675254978616,   0.18356708081737294,
            0.840212309029986,      0.34978738639472473,    0.5811133514977767,    0.12108801074379638,
            0.29279170621473194,    0.6211657218385176,     0.26614473502557867,   0.5215857463382015,
            0.8374212992213292,     0.4503314494895746,     0.41182033387137573,   0.5294716972687585,
            0.4490763273822429,     0.715512372665164,      0.7547233919025684,    0.5787696411606202,
            0.9161390478144067,     0.04321560667689239,    0.5935981448998804,    0.3513088380775903,
            0.1999436890474422,     0.7441935839647646,     0.9007133644535372,    0.8835327361660775,
            0.6168277965883692,     0.5728661998859953,     0.29383979076085,      0.67588620149184,
            0.780627164900737,      0.22948824232609555,    0.8087087491200404,    0.9517699074642698,
            0.4837326541710554,     0.2649569323583517,     0.3893779848611677,    0.38993205745783954,
            0.9995700115351378,     0.6854742123212844,     0.5223064599310434,    0.5965334844686394,
            0.026911927555466453,   0.5564166288938609,     0.36077705590427533,   0.7389534773751507,
            0.13404086412429939,    0.11630315014743342,    0.9315752021049897,    0.9403315245386192,
            0.49533689452191676,    0.525325058494812,      0.47127611656533197,   0.0011261634158155065,
            0.40116422984841105,    0.7810839083387513,     0.14908600010120387,   0.5835900308371406,
            0.08424615664036961,    0.08954714530749353,    0.0042306551068308496, 0.3549308104413461,
            0.9688357347342783,     0.805803674781628,      0.06464464059894703,   0.6492576322174521,
            0.4603415023926454,     0.809980000381188,      0.4467787853548748,    0.8250527789304927,
            0.5015847298091055,     0.3217176870635923,     0.6815068170860141,    0.9307020801316691,
            0.5108938178855553,     0.22472536844261826,    0.3205189185569852,    0.8123237301607058,
            0.7853530489145921,     0.17261391570358375,    0.20618152457728678,   0.07489774899267887,
            0.6841235958896653,     0.8245657783242767,     0.6127687578195344,    0.21417887046334738,
            0.5898995482446496,     0.43214776039847524,    0.287685658904389,     0.966668910907153,
            0.6265879362570596,     0.5567067585752324,     0.9255099245349717,    0.15251016852114685,
            0.07504678340158943,    0.6602504093047964,     0.037377946354966496,  0.30314961993664213,
            0.028654271478108395,   0.13360344298379012,    0.24946821435783817,   0.36605019991506693,
            0.9666551630999553,     0.2929026084461347,     0.7007829308242938,    0.3984103488422228,
            0.7077815244254492,     0.20457366847316405,    0.3591135250823603,    0.782302855146139,
            0.43044850338352525,    0.3279348118907016,     0.942806426429796,     0.5487491735780083,
            0.8070770045965476,     0.8850820557086818,     0.4686457261050633,    0.9386256214555576,
            0.04981444788221934,    0.45119441894347345,    0.3755329696980041,    0.6072001020632831,
            0.04685556555031334,    0.36463888950011303,    0.1292736722422273,    0.5559461390331101,
            0.448137606634192,      0.04307902521305429,    0.9915888468749282,    0.9937972790160576,
            0.12683585302018174,    0.9190781144931423,     0.005173955568690269,  0.3789608860441951,
            0.23225393791227356,    0.5197235209850033,     0.06068506165754006,   0.3328722232602167,
            0.7144951467890664,     0.011772266936242448,   0.8089600890513293,    0.5584621751241723,
            0.48685441469910484,    0.30140044336456673,    0.7384330273534823,    0.8838860911433283,
            0.7655045666830963,     0.9655461107163853,     0.048934334553572434,  0.8638767051770332,
            0.5627714547285853,     0.931310644602255,      0.41220090849492275,   0.7082542696207274,
            0.6084347576669571,     0.20456104258164465,    0.477029268698448,     0.44844127675336487,
            0.5787990745548962,     0.3770735574900834,     0.3735618947148749,    0.6876613663958867,
            0.9064909516707507,     0.8821015134749218,     0.7932978766680677,    0.5459485896276415,
            0.723823901752216,      0.7328258589747866,     0.19605109852844538,   0.024165035352742503,
            0.7454203375687221,     0.18652268736533506,    0.02456337606947112,   0.5545478651158708,
            0.7791745915654774,     0.22338557964806327,    0.6161725047861623,    0.4841106710643994,
            0.5203647330718995,     0.9735371638682989,     0.6220421470403724,    0.936698459323376,
            0.6061502892210182,     0.9335847299440825,     0.6633445705503808,    0.96514758068251,
            0.6861018984601583,     0.3406344779441254,     0.6134172702116162,    0.8437599560787655,
            0.13903574485505588,    0.15598235134828065,    0.5996357044099047,    0.7778885314496502,
            0.845182037284684,      0.5644977593981221,     0.22574113472727053,   0.14143922421302713,
            0.3459062599595004,     0.5467997698965403,     0.27877753281044704,   0.7719785301145675,
            0.6558884344576283,     0.9484641184766361,     0.05356603962708251,   0.31347229890306105,
            0.1927929374287186,     0.04700060806397721,    0.7777934067068457,    0.9428170789075296,
            0.13788884083324016,    0.09225740257930115,    0.39538186666138386,   0.6426671597558447,
            0.6645101669603284,     0.8516079960750027,     0.46352144150671604,   0.6821217471212255,
            0.3166835374075665,     0.7565640234568531,     0.4572110580211288,    0.7091424258819078,
            0.8284542114051713,     0.5954578784783009,     0.600818862321135,     0.20454027752451676,
            0.8088374839902512,     0.7958778621997168,     0.7235778614070212,    0.12146761738117173,
            0.9856505515985629,     0.12806218368711997,    0.6468565460652339,    0.9365945292512512,
            0.11868796418600469,    0.22659383990738313,    0.3355705057867203,    0.8305069446356678,
            0.9886186136039321,     0.8832738087870601,     0.8717170289090703,    0.6548832673263553,
            0.3417799358955971,     0.8756836631678206,     0.9343578065576121,    0.6226707188188723,
            0.2795650148316504,     0.15666076112472216,    0.3604682262499933,    0.6016462277742775,
            0.011908818549332678,   0.8244967331339306,     0.5492979740004873,    0.08471502647339568,
            0.9649886680574392,     0.4739467634309569,     0.9142314938199682,    0.9738370904225634,
            0.6821333536605566,     0.40104361871173344,    0.6025757666988332,    0.29934741193327663,
            0.36008701386172837,    0.9097456482328089,     0.5581538052997621,    0.735148341625817,
            0.3118239423743223,     0.40755094388626223,    0.09849728258789037,   0.7571048153460735,
            0.8181732279684203,     0.10281710395431065,    0.661738919058682,     0.6743533320839143,
            0.33257752896774206,    0.5254200625539256,     0.16188812739496083,   0.5760914997424635,
            0.39048671844780003,    0.6046862323244631,     0.21539773760842906,   0.45228997288386763,
            0.7598499476723125,     0.25906168590022816,    0.13171301366244959,   0.9380895129310859,
            0.24464236000193362,    0.6756528896294898,     0.8010293651258765,    0.11112497287813083,
            0.15889180461314412,    0.6426914329245296,     0.08012933742817208,   0.7024438475025708,
            0.7800406052437331,     0.6976349048033528,     0.2655752601089888,    0.11894263185853304,
            0.9726891905476274,     0.958686883069459,      0.4614238216532637,    0.825206961294047,
            0.00899802724395804,    0.10557162167042611,    0.2862468587516397,    0.4007705146493502,
            0.36788320071433955,    0.6256519985464207,     0.6547717170923529,    0.01619095080673516,
            0.6428985375774177,     0.37440334217310656,    0.9510114809838487,    0.7423930113498056,
            0.3748510089200686,     0.3021426742837763,     0.27677748421735315,   0.4726216365748753,
            0.35347246406955923,    0.47263562317504193,    0.8904053126100108,    0.993414980906022,
            0.2108959975549224,     0.40179737370660873,    0.5413856702618038,    0.9837722202857723,
            0.8392864405093642,     0.7804332664011326,     0.7599170086552804,    0.6616335657364181,
            0.5060321394194944,     0.2645028217449016,     0.9021606622505445,    0.5011482781203392,
            0.9161768880475764,     0.3521801941971522,     0.3645557372854108,    0.9212675626530059,
            0.2481742895188387,     0.2597225757078925,     0.29922477348662346,   0.5555225525115886,
            0.15510385590994913,    0.3917931411067034,     0.21419293874028134,   0.2123210702681928,
            0.5423422640669667,     0.6022056003123867,     0.9852082112019599,    0.9408668231380903,
            0.007452175377153258,   0.9918775989786723,     0.11968267433412427,   0.4171504253040472,
            0.8990178806960634,     0.8204545641663439,     0.12737421210472932,   0.7767068637762939,
            0.9543759938976861,     0.6539866469075626,     0.15083598527341247,   0.9105997857285306,
            0.6899109893566384,     0.9330765131081907,     0.11471514165595975,   0.9830417643736598,
            0.7915550882449488,     0.9792404144480158,     0.4427754250187914,    0.218546800472755,
            0.38907061080875616,    0.08417536584888896,    0.4386027967603151,    0.15799180331449725,
            0.768316164567885,      0.15825409957639613,    0.20959162724058022,   0.28239526818846694,
            0.5805957186323695,     0.2990751101856757,     0.4261014875801308,    0.04983369279912864,
            0.9740752200682488,     0.00032215374705446553, 0.6303506391626595,    0.10090378299958092,
            0.5324431397683922,     0.10118549166252622,    0.9330794162946814,    0.28224380396207993,
            0.26764062330390836,    0.11600917673683364,    0.177114301538137,     0.5889084764708228,
            0.49035737310134797,    0.4193138969522666,     0.5482991153071438,    0.061978536212031976,
            0.03648561086635893,    0.9595424045917216,     0.016561138083179938,  0.40419553893731397,
            0.16071238740942773,    0.9053023052530427,     0.6611494893690456,    0.09368948959019074,
            0.7537397509437487,     0.10984720438672857,    0.07435475772148714,   0.4363308394820318,
            0.9783724151057756,     0.6842537483403482,     0.16593259214075062,   0.4283230180626766,
            0.9656437260267741,     0.993918048460032,      0.7504252748810978,    0.3171313513657953,
            0.6899439690944335,     0.19673981360546244,    0.7933607547701728,    0.6568330264840634,
            0.0366604850596034,     0.586676094667199,      0.9002159750320393,    0.974587691501134,
            0.39708846807912035,    0.018230579141917058,   0.8390032397927519,    0.7250606025155855,
            0.707740834187484,      0.5897591282841996,     0.03204849377328489,   0.6249782190680675,
            0.6121534063205292,     0.941684997455199,      0.40933037433017183,   0.7331045194725069,
            0.9616559451212776,     0.05484791845088777,    0.14215545738865076,   0.7218411192012832,
            0.7245761149461467,     0.11267407940931673,    0.7167815870902152,    0.7718592073731969,
            0.1708343093299829,     0.006134529546718226,   0.3285773804069311,    0.8234402522217823,
            0.20131489118473334,    0.9571295273757158,     0.9029364686662202,    0.7337951081119427,
            0.6193750746550172,     0.5697903555297493,     0.04187276374112481,   0.05116979980185532,
            0.8314816091403153,     0.13646266833628506,    0.1827819361273202,    0.08251065658619616,
            0.22930002967559882,    0.9847498288427354,     0.4854014665012084,    0.9371163726291158,
            0.43948179218112915,    0.7696563960696302,     0.9406734438198084,    0.0014140890580686127,
            0.5828788152389587,     0.8115742064972556,     0.5336101774408298,    0.09703523482800858,
            0.8857790891852446,     0.019260078528706148,   0.9288314028688027,    0.9860862022233942,
            0.5219878989947618,     0.6919798726880586,     0.487644912548607,     0.1274325393669612,
            0.2810803217475317,     0.5021660332039558,     0.13843448921554957,   0.5669820000670358,
            0.9826325525752924,     0.040725980460798894,   0.41312061048452786,   0.7517449158380617,
            0.8441372415542526,     0.5406017150189717,     0.5517473795646475,    0.5117995027340629,
            0.21912236023425102,    0.15633221887458293,    0.8013934472183825,    0.7030608868768208,
            0.3387838519224313,     0.06439131494485617,    0.6072484045014047,    0.7009052268738848,
            0.4718957498453151,     0.7900346277920608,     0.44921073510498977,   0.25851607859383297,
            0.6474009514787405,     0.45306798039003426,    0.2554132582319184,    0.10960580060874014,
            0.47912881940191787,    0.027868363893438164,   0.006106173386905778,  0.7259626007588088,
            0.016583008867730453,   0.8601740028990159,     0.20970860724401075,   0.4377378927082777,
            0.34745035016919745,    0.7402309045176597,     0.09126785161894224,   0.9419552432406452,
            0.36258247608941263,    0.8444264198665146,     0.9694907666031837,    0.33240556983912317,
            0.2841548142222666,     0.9293360021847514,     0.17737241967651574,   0.8129789767923287,
            0.43838646638112655,    0.044352029260083414,   0.42032070140515854,   0.9457038930907485,
            0.8937664574663913,     0.9807780739109321,     0.86940403023997,      0.7985926255993681,
            0.767649657289681,      0.2715392010357749,     0.5638233592226446,    0.5429260016497375,
            0.6281534110495334,     0.6941281957059903,     0.2625504404760831,    0.574385438188128,
            0.8166393028206065,     0.20953862956384983,    0.07813020231811607,   0.6591245871100481,
            0.33077748352026937,    0.14595567132572362,    0.8024891767383944,    0.8909560685053818,
            0.7373554637900283,     0.8388269948540654,     0.399190349529548,     0.6305751690917523,
            0.5477434981710652,     0.42082606303815506,    0.9284524943968359,    0.07846200614647458,
            0.08768393732336088,    0.9835584157484129,     0.22458938497257142,   0.7234381686683174,
            0.8071042934264052,     0.38373858729637356,    0.6314764300376291,    0.17037297131760476,
            0.26354482931810785,    0.30605507789000386,    0.7371997188797301,    0.9776135934677442,
            0.008512820063818594,   0.10869869608049121,    0.9179058490408968,    0.10325024249521753,
            0.8658809504227689,     0.4338200486094673,     0.4482240224907943,    0.32665629882468983,
            0.11785058947074212,    0.8801195994988438,     0.21399401198667523,   0.8853200395578502,
            0.3511490525481342,     0.9802061324552901,     0.5655375342681864,    0.7948142734645098,
            0.5675696058794935,     0.6383856420960282,     0.8478230332215296,    0.9703696830313749,
            0.4671297543772188,     0.06983129704605762,    0.6525838929635193,    0.04457626977989637,
            0.46527766222973477,    0.869884077385513,      0.8713504397191266,    0.48174019199406026,
            0.8786871247661204,     0.8444848915406978,     0.7909268831186177,    0.22938936649959962,
            0.5111027984669417,     0.748651170513814,      0.37737539989049185,   0.8840334371857501,
            0.5510733006826646,     0.10648063450738887,    0.21886789617142666,   0.16317662717280335,
            0.7629352833426475,     0.12909066834194638,    0.8555171416585637,    0.8129929404099899,
            0.16597345469807878,    0.2630470053173972,     0.12001620008038238,   0.2980015357381648,
            0.8899244737630556,     0.44787156559219266,    0.4196703118908268,    0.5759109349541586,
            0.4164581898027886,     0.6546265571009381,     0.684724154575046,     0.5590946252074627,
            0.49168621314215666,    0.3058928221458628,     0.9544390401499846,    0.853658458874726,
            0.21275808841632982,    0.4666348536818804,     0.7129036867018201,    0.6768287504098665,
            0.4649588379702999,     0.5147326818148316,     0.03674789057197436,   0.4019630892602475,
            0.6712487501813974,     0.6542485793652907,     0.6440612497487972,    0.8590372219447355,
            0.008495614336447743,   0.08473985821507257,    0.6480516205261334,    0.07597894893155033,
            0.05090009686790542,    0.26354064580002734,    0.6071189359327064,    0.8981093334339811,
            0.9873705826957054,     0.2346938414790437,     0.5744909069047073,    0.13320613456206354,
            0.2829557721335417,     0.6303316606235478,     0.10323905458911298,   0.05980034324224026,
            0.6276329029724647,     0.7567814781709326,     0.8740605964980697,    0.7840824582989661,
            0.2233471473003188,     0.05855824778786234,    0.2381236129766915,    0.1836136989133872,
            0.7653580622360917,     0.9339037006205303,     0.5816951874334918,    0.7088541970400036,
            0.4782210621723998,     0.6320182406635941,     0.8379102189796891,    0.4753302203434747,
            0.5729176996679675,     0.5268155431449816,     0.11026388398003484,   0.13171219495346664,
            0.9704246054166432,     0.6200459243536022,     0.8145628229831698,    0.23223366662359457,
            0.5196496780277942,     0.4767948586098145,     0.8312713734556832,    0.5662305287300955,
            0.7593409467477318,     0.004558678330644272,   0.298406011857943,     0.48730116497590836,
            0.7239704676587752,     0.35959323344515326,    0.05126451485350014,   0.9064886768048788,
            0.2692384607413335,     0.5589198081882547,     0.12912482259957514,   0.5147901213332146,
            0.33145842966242545,    0.20871738123377692,    0.03541979678253637,   0.4098084717305672,
            0.8554333210416315,     0.9267536963629067,     0.6083276773134011,    0.7916088354402241,
            0.7813625572896977,     0.9170870928262315,     0.6282698742151044,    0.9343836859351446,
            0.8886709101828077,     0.23626366539220522,    0.004105794434200116,  0.2879082699350165,
            0.36567169973679237,    0.955256193504928,      0.6294458713305562,    0.09103900758001693,
            0.8469312473693905,     0.0014838724188969143,  0.0730702896986335,    0.8364985398778184,
            0.004808171586357379,   0.9302304023298917,     0.3793888885770289,    0.47793120630931707,
            0.6178484131341514,     0.04348012623334574,    0.16919156084524,      0.1580940570587458,
            0.33432653785086464,    0.446070816449764,      0.7456699751042108,    0.027248434098246066,
            0.5130379707852901,     0.9124654482672971,     0.20776673925643374,   0.2636170738421506,
            0.15322000146224457,    0.1241009866844297,     0.35578720492164695,   0.5211235421346518,
            0.3164195130719253,     0.5299069809663779,     0.0030222633286991485, 0.29299764689615815,
            0.34169217165685095,    0.09619199583064919,    0.7201185155794164,    0.523836365618791,
            0.09422538430473615,    0.6894251441859315,     0.8109289188418026,    0.5507676267128784,
            0.7875889700149205,     0.7460708818443026,     0.2674186102862097,    0.8880648493901172,
            0.858649134282793,      0.6731546082956901,     0.358301666422131,     0.8638837182160444,
            0.07775697534964099,    0.24497582154927666,    0.9593068764206688,    0.925761948926003,
            0.32034453042173405,    0.6603319911009783,     0.4150325675390295,    0.778364300975389,
            0.2636234943309784,     0.33923950173924644,    0.9218773376248665,    0.2889111478583931,
            0.8770656737277066,     0.8621685222181902,     0.6723311758022619,    0.2605153243045394,
            0.6097867896803811,     0.0009594047833868524,  0.4519505521431685,    0.6497667064297269,
            0.453976342469788,      0.3993842862978997,     0.323114244725039,     0.8996004342586513,
            0.9610662041067752,     0.5154665171363502,     0.6884889037472333,    0.2969198384398505,
            0.6490400491756882,     0.05853874728565345,    0.7739308349734073,    0.3293458724575423,
            0.9570197495433028,     0.17283147059396897,    0.28930996504732176,   0.3863223570947747,
            0.15320749006711865,    0.23779080369512873,    0.8912304760868286,    0.47631078090237944,
            0.7554738886093284,     0.90041004023757,       0.684442709249358,     0.3724315620337597,
            0.25537990805579114,    0.9390953782390274,     0.6881598059238712,    0.41060365643565144,
            0.8113060334062302,     0.5822406615965725,     0.6950494704883114,    0.8685724471069483,
            0.6869400639375701,     0.7868680256982854,     0.8636048049648265,    0.044671237035904476,
            0.3854928304185423,     0.992016344242109,      0.6607963854423865,    0.10006921197913154,
            0.21217674304853384,    0.733714216367171,      0.5122882249286589,    0.25262671684003757,
            0.7458952631238392,     0.4896060518251806,     0.06524300210801981,   0.15996823719534792,
            0.9572921921652023,     0.21153701149568171,    0.3631876671228319,    0.10763292539292857,
            0.3968451986744491,     0.009028588125270076,   0.5040465952864235,    0.18862980100115,
            0.0009805060182767056,  0.20572530379515364,    0.5194566405462543,    0.17603804090113906,
            0.7852113384431475,     0.3455382401713297,     0.6669634483788133,    0.038659822124262555,
            0.9724654513697798,     0.6518917785234513,     0.6451266337602618,    0.5165842668692634,
            0.6854582863536932,     0.36662112881860465,    0.19783858055409853,   0.05209320862630096,
            0.06848818232131315,    0.5620488897349047,     0.7814340593863996,    0.0015579886364495144,
            0.06801034182744226,    0.35691965425128225,    0.4090556670508517,    0.5950249280592846,
            0.17296012453967602,    0.9808186822058274,     0.6436863887760205,    0.17719172988316012,
            0.3122834597142039,     0.18286887466427793,    0.46885879849994994,   0.8897053245055813,
            0.21141486050979375,    0.8499188869077299,     0.3847450841790877,    0.5133694204353769,
            0.05265969096711354,    0.3455522195496137,     0.7217475042024573,    0.3424069753314998,
            0.703047185039163,      0.7138267365136245,     0.8309582196073724,    0.48830636927457494,
            0.2977454794617652,     0.8147915021967428,     0.9546752875251681,    0.47394632155077965,
            0.9187583027266814,     0.5199272297544021,     0.7351357157352518,    0.24241224252407434,
            0.8882538524878565,     0.5091420004969103,     0.6013326660222763,    0.3883364996654842,
            0.33419583928849395,    0.6088957250977964,     0.2512737647019182,    0.1440776830337569,
            0.13152661362709495,    0.027209246542848398,   0.01531794977564005,   0.240542598102776,
            0.7596825260658513,     0.6914039379483994,     0.7173036065320775,    0.5317048926357799,
            0.7900798940346032,     0.47558250705400873,    0.43140919042029446,   0.006624543162651486,
            0.10685220449336275,    0.02099659947815813,    0.5417995639525759,    0.8131460186438586,
            0.10122039633826663,    0.47316976394079624,    0.08650257713109899,   0.5785365363729024,
            0.8921250150145295,     0.03926862909827877,    0.5963595868324929,    0.5525448237680127,
            0.32775234214947824,    0.5580311564883518,     0.5647264840758505,    0.9637602690673241,
            0.5004588681439911,     0.008233208197315878,   0.9131174631834843,    0.7228242118599889,
            0.46011176280499877,    0.6832861096429752,     0.36761886948973943,   0.9888457755905448,
            0.32008849008322426,    0.12964271829216167,    0.834250831696058,     0.7921133763199811,
            0.7088284502442913,     0.6445253845709896,     0.9757803908793508,    0.5958956780884561,
            0.8517141963333277,     0.3867321041619475,     0.933646823188871,     0.5517153496071622,
            0.6064114202369612,     0.05381169728787194,    0.1665464501399011,    0.7188102443965307,
            0.03341989673273382,    0.49499423099274154,    0.24257111116569807,   0.6835386035994174,
            0.2538157229731184,     0.15753291274024483,    0.34072935511982405,   0.3195146633356625,
            0.8511823281209047,     0.4461198295997485,     0.9259272250881303,    0.4691039267989945,
            0.2408756672628607,     0.9411203555036771,     0.5540495686734663,    0.9088247039656665,
            0.24239076588000175,    0.5785070541835289,     0.3334163393503512,    0.8976127973908765,
            0.8397523142250739,     0.3761085760990982,     0.05313095725491579,   0.2607452849893952,
            0.9392688961636607,     0.5013323863257974,     0.9046928310275727,    0.44188844270745653,
            0.8359912018845598,     0.0822785264788175,     0.8737461017188949,    0.5461114455167173,
            0.23458332243804225,    0.354598828580427,      0.17074144352596876,   0.652880039149025,
            0.08674833780617708,    0.9433128679469707,     0.688610349741941,     0.29791472890076676,
            0.05702280238702284,    0.6504621333364767,     0.6272505494004752,    0.6025390029659357,
            0.40241964671223396,    0.9457134788906609,     0.8499828828747102,    0.18874303465998554,
            0.38265519286797,       0.3839713919094293,     0.39623883311327335,   0.507783171844135,
            0.2159221921031832,     0.3383099360285574,     0.009235637470865199,  0.4350184977393765,
            0.2827485430683232,     0.21949116903208565,    0.5887193767478403,    0.3235558510240778,
            0.137034251626184,      0.4324954521609413,     0.5063480657717252,    0.7087781196894043,
            0.9695436451232501,     0.6244514168327856,     0.534911067305013,     0.11123425487237404,
            0.4874933865585298,     0.5869751368421979,     0.9886124565976541,    0.895544757837733,
            0.8736215098206865,     0.8338167978213771,     0.7889393112040557,    0.38017432689403385,
            0.38589852409526315,    0.5050591878790863,     0.25109960441872625,   0.3513171415203168,
            0.8613864643311907,     0.24625770971407268,    0.20160456083689404,   0.41129704655919896,
            0.019888400775907922,   0.875684711714011,      0.26178602387477823,   0.27146811211289,
            0.20894847792573257,    0.9532003942984517,     0.6971137063522846,    0.9059634199855195,
            0.4401591098507093,     0.01795931575255738,    0.3156566676032445,    0.020849147083099928,
            0.5324972298270672,     0.9825763272511439,     0.29102036181761004,   0.03851503672095169,
            0.9700768884098047,     0.2928766518767334,     0.7396726161195039,    0.11705518788264502,
            0.2064290881156181,     0.885448570732786,      0.3608795153631795,    0.926213206265405,
            0.8005651607912058,     0.7468219392837232,     0.4745352229453357,    0.4295922286919248,
            0.3918061341334991,     0.21617368663594672,    0.10724936361729775,   0.7703147080282404,
            0.22351682233434744,    0.6641192981124071,     0.0414143720617377,    0.9640355474055853,
            0.5963444792374569,     0.00032576302701692317, 0.05759281353769996,   0.1348350085635298,
            0.9377182381070673,     0.3509626613109743,     0.611345075498055,     0.19680647255566963,
            0.884740330637731,      0.09306291499684138,    0.3184543304085907,    0.22669912803822467,
            0.2313644701578511,     0.5166488952792366,     0.8436555064781269,    0.621279780741898,
            0.6363668512929629,     0.8783271403907555,     0.8514132706625926,    0.827000147149191,
            0.8190674600844177,     0.4174407512821495,     0.47732917335211555,   0.34164224909308405,
            0.11176172250866745,    0.5619264977667451,     0.2998048892748941,    0.8782580113637876,
            0.028709689237371938,   0.5737998865292607,     0.29895815732751985,   0.5210463785967583,
            0.2629070567007913,     0.4034693081517102,     0.08692303941134549,   0.6775110833178721,
            0.9728437083447087,     0.3315476186337849,     0.29365418449414804,   0.08257032324159874,
            0.9839761032407703,     0.964596905069,         0.6352606890928622,    0.0779268074873748,
            0.4804084418422565,     0.2859898228427621,     0.6882206128169568,    0.06794141790154729,
            0.5459718775781492,     0.684403524954966,      0.8977701651948777,    0.23049289195691436,
            0.3121148858081436,     0.7013666863671739,     0.09449449022911982,   0.173557023555454,
            0.7612665270325496,     0.5007091800644236,     0.19048201710756119,   0.1369013382062383,
            0.542581138677758,      0.5473320075870957,     0.7355024033867158,    0.8952954213484605,
            0.4290925298134547,     0.08528902496775348,    0.5151435907985005,    0.2515577179564804,
            0.6380966397994302,     0.1539732886463263,     0.1857156364463718,    0.4475982103370326,
            0.703556244196847,      0.7668627495264866,     0.7771115684144717,    0.19842515945648576,
            0.8524290693121767,     0.6032880069677579,     0.6096275954768208,    0.7817415707442399,
            0.3641611905873149,     0.8906932714831536,     0.8124175227970734,    0.7164991708784773,
            0.9581985445955595,     0.60220506974975,       0.6970748647959257,    0.29345481682203756,
            0.032845128302094295,   0.14508293497014635,    0.23059118521943367,   0.996903368983305,
            0.7140268534044998,     0.5464022577479941,     0.6075985633909603,    0.5243844313750604,
            0.22751203875198844,    0.9875323576741843,     0.4764127368024498,    0.0815070809410211,
            0.07516088937063392,    0.08436110819767584,    0.4069514509177412,    0.14827279891330147,
            0.4554411973237368,     0.5026695518808091,     0.36792917904223554,   0.6206544293634522,
            0.015440128666228126,   0.10269067329161896,    0.015163468995910256,  0.2969412928298417,
            0.6085918034852696,     0.08025312949222818,    0.15179257293992798,   0.04315776888042311,
            0.7677755031904019,     0.9381448304518312,     0.5140876763299803,    0.9514443011555282,
            0.6407757657079132,     0.0765698599268878,     0.8476496982444982,    0.29742023288486263,
            0.4278682864122032,     0.9807213380432217,     0.07911917475472163,   0.8512230995302364,
            0.10464574549303729,    0.29640579653429244,    0.5330302554416567,    0.26513319106603617,
            0.5904796810764913,     0.8045767818618806,     0.8366111498182954,    0.5803132778233024,
            0.33546911180316286,    0.19755405967272732,    0.7739232168523782,    0.3431428162209841,
            0.06943880657672563,    0.8424212560717366,     0.1792541652464148,    0.23923891139115705,
            0.2695174630337248,     0.5983688408671621,     0.2166853566314202,    0.9555002941610525,
            0.3733927328005682,     0.06773668612539918,    0.5897664723661606,    0.16220980649511318,
            0.8217097211780661,     0.013322815612317407,   0.19994234068611894,   0.23980826687700363,
            0.16567772272135473,    0.9225760341307798,     0.8495754373962656,    0.3747780614528954,
            0.6370432578151596,     0.9768984204125892,     0.4204515151680184,    0.7005843204770014,
            0.6892847660959509,     0.9545148748016049,     0.6143830077940992,    0.2094946748069053,
            0.6137540635212229,     0.7907730339684662,     0.43928047555035843,   0.2404702909941716,
            0.03525245664088905,    0.9468549780596774,     0.7018444589043075,    0.4233097335254806,
            0.17055600084252887,    0.21479511489577663,    0.015803981578659143,  0.8413223905651063,
            0.519348093484841,      0.677328794875255,      0.04102853537158957,   0.5973099425200125,
            0.1961516047773859,     0.0062697185169774405,  0.12995538953415364,   0.5024906873648372,
            0.9366413539188541,     0.2005732471901741,     0.2845429314469091,    0.7420525443666908,
            0.32663474803099335,    0.41817090842663707,    0.34679822715136543,   0.6254450782613655,
            0.5617918392055117,     0.8493672841886342,     0.5678823903181036,    0.5414854471001395,
            0.07595259068167515,    0.05992393753186531,    0.43950385738819386,   0.22426024207450634,
            0.3440795103112836,     0.543614736931006,      0.39840127677842285,   0.8380022094986134,
            0.42735989849616896,    0.21116213301339326,    0.10688135855039405,   0.8122754674795016,
            0.24349142326505413,    0.7548582735244727,     0.32867610838513206,   0.9288904160700249,
            0.9209177539290534,     0.8842602740791992,     0.8279493007772021,    0.30075977066974824,
            0.07280702774796677,    0.11864651501493295,    0.6688470229861069,    0.03724364818799586,
            0.48612155851759253,    0.6309638611431445,     0.4942769924580389,    0.7984005800249495,
            0.6556850452872549,     0.6973321309187377,     0.8105546372590183,    0.5271401700450064,
            0.31988416310121603,    0.11292485612467362,    0.6897767445279341,    0.18083999759778802,
            0.10577005826134867,    0.02511931159870806,    0.6547138599790717,    0.8680967675529965,
            0.5719830697041671,     0.7644132002714386,     0.7696055721073414,    0.4588595172180936,
            0.03628478642533983,    0.8715479674408582,     0.5279191738220554,    0.7464273424343992,
            0.34195259909108366,    0.47625196882372234,    0.9063949033141615,    0.6451078418202955,
            0.010016678965625081,   0.5633772179641451,     0.3040492111100356,    0.04602006615957521,
            0.3639265456872167,     0.13654018444855898,    0.6480557055009035,    0.4445476879765782,
            0.17204731875768708,    0.7040595552109383,     0.3198017362250478,    0.82812696432128,
            0.7595605039974195,     0.218581275194754,      0.9964171235689756,    0.5783764464966217,
            0.7050543348500834,     0.12082311647408495,    0.1617173102063515,    0.21358993038707508,
            0.3094097996762023,     0.7661823190831037,     0.8518447945034651,    0.2964770449576728,
            0.09779437868900487,    0.3815727848325584,     0.6554275411064775,    0.29164952428811597,
            0.6161379450466672,     0.35509282781577445,    0.22558271275348163,   0.7371844681372586,
            0.5944039305145205,     0.5003711575045766,     0.47049432476265973,   0.730610739466964,
            0.0988134281342925,     0.16190754148570252,    0.8531650549659385,    0.7910486502112972,
            0.7654902688364232,     0.7264305760700777,     0.8622773323691552,    0.6651306024752726,
            0.38696232353989524,    0.6380258212742878,     0.31873156785888435,   0.4903899038977766,
            0.17016070510187253,    0.08357688777509897,    0.2898204855944452,    0.39949727107717015,
            0.18313485335274593,    0.3894034077267524,     0.18318318558418012,   0.6009342776632394,
            0.34872036976469056,    0.6581115690298905,     0.33150890354645746,   0.0496414017443747,
            0.6261616029903583,     0.9412282007497498,     0.9354433265928807,    0.3343757259527984,
            0.17110105275028809,    0.5743773228256834,     0.631282326851356,     0.1265223154947448,
            0.05956666006745148,    0.29275659742335025,    0.9638479862044448,    0.8835168473733069,
            0.44233475395862565,    0.776302788765977,      0.960790521074214,     0.6418119681054083,
            0.5297743900364406,     0.20908914378335974,    0.2112387646466073,    0.2409200625286152,
            0.9658210624840681,     0.7194427745114023,     0.8961510058897204,    0.7158726271323808,
            0.9709478427129915,     0.71641934291295,       0.6978598889198826,    0.562433685892602,
            0.7584211186028187,     0.5285173957402748,     0.5281476200616488,    0.2433402131975656,
            0.6416443509917007,     0.9919156121101268,     0.08325553077154091,   0.844919711577578,
            0.21736855876623884,    0.5134126025348843,     0.649945492330081,     0.507371406922281,
            0.9612080916546023,     0.799559489036482,      0.38784886231871973,   0.2514257805169353,
            0.5147929132519722,     0.7359179457042792,     0.393962295971405,     0.7215352480207046,
            0.04692795757302448,    0.5262940246059401,     0.965670867852144,     0.5821420648305368,
            0.6541083154130035,     0.28200537946886706,    0.8515727218320565,    0.23394206114530003,
            0.8440952963960885,     0.3420720114566579,     0.8598142741233008,    0.06844682464594498,
            0.7764638527281771,     0.6891653983619935,     0.5906360377441091,    0.693767863223038,
            0.501631277360539,      0.1259357782951196,     0.8811637311755608,    0.9296187432293417,
            0.03430909767154722,    0.5885428138086796,     0.3523586217223946,    0.5141633202186399,
            0.01478331280322187,    0.07374690088781022,    0.6008897850962397,    0.45508763695770293,
            0.366143275617999,      0.4007635264016053,     0.6256287424398075,    0.3876117061535449,
            0.12617695049817323,    0.32648577285849867,    0.2393650854928122,    0.14525875946788858,
            0.9304786362838342,     0.22634427890693198,    0.5872569341864483,    0.49786461824300887,
            0.809109922707037,      0.4908787429728545,     0.2971558348741945,    0.9202443725906043,
            0.4708581261693834,     0.05047996039399072,    0.8085789561361016,    0.0033775962984968766,
            0.8378561958865722,     0.8831252690408297,     0.8832717731022176,    0.7446755804075358,
            0.33093560300141756,    0.20094491008482307,    0.3588281454786427,    0.32285719792862544,
            0.7451138552440213,     0.14248586173611089,    0.7926058404533078,    0.8841408961254881,
            0.43399243441758395,    0.050645097031216224,   0.1338309253389658,    0.3166573015111728,
            0.6844013834047146,     0.5582398424106365,     0.19639199836219712,   0.8806278774744208,
            0.04775053841211363,    0.6207222172034218,     0.2126533800165451,    0.6985288696086546,
            0.4872620844632072,     0.38223701361720486,    0.609888972221702,     0.7026030762052993,
            0.8950892592987694,     0.9686700539177381,     0.42344736369788927,   0.995783156210629,
            0.5751781192031303,     0.44121798802045287,    0.7205876863304265,    0.9391234975151154,
            0.2102330928534104,     0.5232897972052771,     0.3526268668321072,    0.9424589210795762,
            0.4684433770462869,     0.2938901162459294,     0.5546783052046197,    0.17987847012488312,
            0.9458445471073877,     0.7989491359988972,     0.5850240381583725,    0.48744171318427953,
            0.6221944870992951,     0.46489192369226817,    0.4185053947197217,    0.13874959872689918,
            0.9837782839018144,     0.15547885847650267,    0.031368698829014496,  0.14915857339116922,
            0.1264875125442777,     0.6197256944016849,     0.8282624133666818,    0.7391256673104295,
            0.7740924967553946,     0.2641937528674697,     0.3215436705782915,    0.5341497193875369,
            0.0040252106313000136,  0.7983273280819632,     0.42029374880025616,   0.8780664298021492,
            0.3402349161445176,     0.1898755944354369,     0.6001491642575865,    0.6778463508273469,
            0.8305032695240993,     0.7387814919440702,     0.7190797644683271,    0.8835005631864289,
            0.8054966845612681,     0.8854776989211987,     0.49612141649228214,   0.35216950999292207,
            0.23621763753759806,    0.35939180325653164,    0.836430640027048,     0.2685592614020714,
            0.8500592818896662,     0.5519154210321693,     0.1281922490422176,    0.06626489412482861,
            0.4943884264671894,     0.9340699457518234,     0.8799353758955039,    0.22460624607326451,
            0.8937286575037703,     0.23008378783339944,    0.7386036495961216,    0.40421863803923963,
            0.795152586222173,      0.055833935043233085,   0.5845715858837066,    0.9897177031962151,
            0.9905302982165827,     0.9596811901141332,     0.6979122440459229,    0.29108276423325186,
            0.6682494989534188,     0.5499315391981521,     0.6224334711333187,    0.11226274018948956,
            0.48733213291776956,    0.887085706303916,      0.7056629698221989,    0.5655546768425956,
            0.4105407543749683,     0.12895505212239988,    0.5890961544353817,    0.5778132175626944,
            0.4409406759190033,     0.75867006830884,       0.7523397188233624,    0.527472315336094,
            0.969672806413678,      0.2394692485901424,     0.41812898416021005,   0.8677863194061313,
            0.15590500040576416,    0.831963739937221,      0.8896851243689841,    0.48713367625648896,
            0.8130921353370143,     0.20963918051728903,    0.9244678891191762,    0.46966604449107063,
            0.579149925671282,      0.6743628448529819,     0.23451845746220412,   0.7466847660417775,
            0.391001983821309,      0.6538754715696976,     0.4615012769017248,    0.9377676181245355,
            0.9491675354663268,     0.05052363368814217,    0.28390908597093845,   0.8183874505567702,
            0.38543395956917315,    0.26858281977863985,    0.6392153388365647,    0.22887880767960878,
            0.13644806044088376,    0.167587051612752,      0.8718839872689222,    0.6559717724307271,
            0.9251067684415579,     0.06990914454920516,    0.5034640312497337,    0.5013076850206055,
            0.7717294275008427,     0.3306092938951103,     0.9986186396878293,    0.2035067252166245,
            0.4857108461400149,     0.7301867962151891,     0.8603827869142012,    0.41607976024504234,
            0.14543979623993353,    0.24986808624527845,    0.8518435288618611,    0.16583027167137498,
            0.26630399296914486,    0.2094887667292088,     0.2698471225304603,    0.4294969362240938,
            0.12421028850091864,    0.5172828650569337,     0.2792594547636793,    0.8039707526453396,
            0.8027059302843155,     0.39949250067778885,    0.04208102326762064,   0.6493345143904136,
            0.15722527973254075,    0.8840554396535564,     0.3728498153822032,    0.9729287610957722,
            0.7359173630256405,     0.48074366583962336,    0.6422662945681412,    0.04312589553324786,
            0.20546291298784636,    0.22943639894890788,    0.7244921625489029,    0.7032849770842924,
            0.9865104182765432,     0.9103658416923569,     0.7639586222414887,    0.19371354249069217,
            0.8874634147020038,     0.519538039677244,      0.10538261162636964,   0.5398822865493209,
            0.6241623772444894,     0.7589325100642482,     0.48214847884172085,   0.4724791737229119,
            0.48800175782053024,    0.5833304396342864,     0.7006271893758288,    0.7605323272374044,
            0.806455429269515,      0.9493614975208688,     0.1413773857944206,    0.07714424812896614,
            0.6638952078911184,     0.3857321163325369,     0.08046988083065976,   0.991204058533467,
            0.9374056560421572,     0.556728157190655,      0.9771817120283296,    0.27541630326710465,
            0.005146815194777221,   0.16841371636410674,    0.2503051083358384,    0.023120626516520915,
            0.449515835483693,      0.1455111998190729,     0.9237147951376047,    0.7549402473173545,
            0.8910507255063151,     0.2181033764675262,     0.6695649417369641,    0.39023252436127176,
            0.3056219734968382,     0.7830062743808217,     0.2211189480527077,    0.5275464326323508,
            0.6825488169507075,     0.5540142155206008,     0.9676159603279827,    0.2588339382028777,
            0.3415262264678389,     0.8268978189146996,     0.739718656690241,     0.6186378232095143,
            0.23993343238269738,    0.46240402745559617,    0.131666449657584,     0.5515999590910257,
            0.4954134937877559,     0.10504577970245632,    0.22783547119025505,   0.07603704088554908,
            0.13459028458226674,    0.41422163791855326,    0.15114803866664073,   0.34779088953069504,
            0.5497424103806892,     0.6299660213595425,     0.3945009204561891,    0.6218681857283719,
            0.49805165210862345,    0.5483917613969087,     0.563114391006557,     0.214282079860027,
            0.3257468566357794,     0.6687235569951967,     0.6567727844592294,    0.8655073384906574,
            0.45553984700746053,    0.8484727547627134,     0.12745038797870378,   0.7570133137494108,
            0.7866880419140133,     0.6561719210467561,     0.6115834726324461,    0.19268027491263107,
            0.21949169943589864,    0.0021835569105005392,  0.4836238089724795,    0.3132379788030558,
            0.2755510112780075,     0.5367570544631642,     0.37898054086881505,   0.25231798687035223,
            0.7356090701612248,     0.3855546751882303,     0.05419937161400601,   0.5771402420930211,
            0.06901883372463047,    0.7389251498611187,     0.2354254979231436,    0.12715140226240396,
            0.597647547746675,      0.9257713493525752,     0.014222004234462626,  0.18027176272878798,
            0.17750742865196356,    0.48847818687235345,    0.89605781077376,      0.9674192283326282,
            0.8702891571510301,     0.17140908918377384,    0.3489816474812989,    0.21634035020867626,
            0.6526093819618123,     0.23974249012825755,    0.6645119754125621,    0.6158025842993531,
            0.34728413784387224,    0.614104079623674,      0.9223348436311207,    0.1691604750813117,
            0.4644488648277778,     0.5433276173770782,     0.5774792042703888,    0.37503896462277264,
            0.6698841424708741,     0.1655345874062174,     0.20064189172088998,   0.8787630592009681,
            0.740963351191044,      0.18738418238312793,    0.30674530123907895,   0.5493610555830722,
            0.8973843163827346,     0.617142252785327,      0.8940984297603277,    0.7516765245950261,
            0.06261524901748194,    0.8811387950890462,     0.205881158051069,     0.5166067992948751,
            0.5796213990184897,     0.012784999546325837,   0.6103794639880136,    0.005096072042229283,
            0.16360686065783658,    0.21291828903343213,    0.8278661975212714,    0.668681077497476,
            0.8837657933192906,     0.37153073497376876,    0.8321645486333105,    0.13638622192474203,
            0.25566130756854,       0.6473606209411075,     0.7377522715023068,    0.7541021968364529,
            0.8357749535235386,     0.7456231642603187,     0.3338486673371397,    0.8862421825817257,
            0.7583987760311064,     0.19064969356828676,    0.242924315508393,     0.3534597682176439,
            0.0918961543561223,     0.5954630160842695,     0.39985144716215026,   0.0985583146578195,
            0.29166877468564134,    0.7652616441449321,     0.0959772267667961,    0.7546602672158454,
            0.2636849154411278,     0.06956636809013739,    0.4095461896828313,    0.3822624480902461,
            0.28907049079939007,    0.4424939406510091,     0.8412601179142394,    0.29166021210179627,
            0.5405706157448056,     0.8873811396705715,     0.7076703541511937,    0.8959490233081647,
            0.7804495718442109,     0.9209043193280484,     0.19566557446183153,   0.8085961911859586,
            0.32057679670364725,    0.12189096673230837,    0.26473675989398004,   0.996629730927226,
            0.652548786334173,      0.9075290277802945,     0.4802252392747707,    0.10779965292920712,
            0.4054066761800629,     0.17330367182750972,    0.6096458664729524,    0.5798044779596909,
            0.872703786037611,      0.17769553180704267,    0.6712507817496286,    0.5816981470279134,
            0.07448701842743422,    0.7742141516056278,     0.5609798783301355,    0.9212181017004285,
            0.27354334468042285,    0.060049799538552606,   0.5807922937993397,    0.9027411453522435,
            0.7936661335019406,     0.09273493131472499,    0.6200412702613728,    0.35529556980971366,
            0.0933884829245043,     0.8011910925342979,     0.8856129084246106,    0.5982123576259409,
            0.028053739675490408,   0.36801177677912855,    0.15324761311259505,   0.19625512528600997,
            0.3490375679764771,     0.8954704199061206,     0.4992270415928295,    0.3855763822318714,
            0.8701777267092509,     0.8957514762731625,     0.8260499455480201,    0.5787364801339311,
            0.9455651034940513,     0.4236325954612683,     0.7885191319871936,    0.11265470628235752,
            0.9373392045772332,     0.7522189865280404,     0.5814389968184337,    0.33740972281117654,
            0.3898500726198344,     0.8187579391512357,     0.464228690644809,     0.7386098231187797,
            0.7870214398333207,     0.6096311176459468,     0.38708134964679464,   0.07205113130750673,
            0.6338669142030066,     0.5699560960020534,     0.689485551532759,     0.2823074432983642,
            0.6364356587618056,     0.16526808895445344,    0.08565142875036769,   0.645794132217499,
            0.22340577154801444,    0.6703925461586239,     0.8368333338080013,    0.6443011912694613,
            0.6697007632746084,     0.9476557015003222,     0.4444850665642158,    0.5241759847527117,
            0.1561431110825181,     0.07550122041145502,    0.3570740774708877,    0.9655642633584276,
            0.7768888645136565,     0.9542591993094416,     0.06537037259879463,   0.6244902707110851,
            0.619335704927162,      0.9024727399270388,     0.3363927692719417,    0.8340553635826755,
            0.39458753751179954,    0.29280597218899285,    0.29280151563985113,   0.0023619134490885374,
            0.036164389869845937,   0.6338880628939046,     0.08180223658834529,   0.8097360023824277,
            0.2948129029100992,     0.6745822965184326,     0.00668946935004,      0.2141593341913347,
            0.10834281616655295,    0.20028147367340476,    0.02919486252302861,   0.8275058057243686,
            0.14220912348912718,    0.6494289915814013,     0.16866692031776653,   0.8467359167534151,
            0.00953142527471984,    0.649615243939638,      0.8111062048998361,    0.0041109823210867535,
            0.2766170683786505,     0.9425521933946843,     0.45022914524978563,   0.6439886356559811,
            0.9359720802259758,     0.8091212583918839,     0.2394861050549778,    0.19216059728221646,
            0.08833106056260387,    0.3967525844210681,     0.660530811057208,     0.9028932778584909,
            0.038576696949704226,   0.2881754330313526,     0.377862615838773,     0.34666458169725156,
            0.9384985944515373,     0.01674451817158873,    0.4730875502376053,    0.9736795684317692,
            0.9615005865111602,     0.18419355085875677,    0.1015017264929916,    0.729049027418257,
            0.7912470031735453,     0.9732974539737105,     0.3024315499162922,    0.3047123131178312,
            0.07167484786396039,    0.08329515475062188,    0.012203199038870904,  0.5608236383142611,
            0.6920618822140845,     0.8095692585838979,     0.6456886089041037,    0.8956088161237685,
            0.3710937215887009,     0.6404408266676882,     0.5390560861940349,    0.840548664985671,
            0.5393434220055948,     0.8483093847917064,     0.6964580612142962,    0.2787613190958763,
            0.07836622434053908,    0.5527624513089565,     0.4511954710934898,    0.6322303115731569,
            0.5293147692553649,     0.4797456112485896,     0.022772621439471563,  0.4315028392758812,
            0.8131063302519922,     0.835499462708823,      0.5954735929903859,    0.6499520035504176,
            0.05338546770130992,    0.25987643775558844,    0.7854030318948674,    0.0453186845636645,
            0.37732247264323415,    0.24730619642092388,    0.9832938258449161,    0.9595452798656801,
            0.9670633194992989,     0.04985201664670402,    0.49379417267049563,   0.32464719033751355,
            0.05908443060987556,    0.8605731642912389,     0.7588451327435463,    0.13682080278644893,
            0.9238049022655254,     0.36103458382292253,    0.5691471759361028,    0.07679617119683702,
            0.22554866316988564,    0.09019253585688614,    0.6829468282654838,    0.5322399347649318,
            0.7855180040164972,     0.551441074662128,      0.37267763228484374,   0.2614767444463608,
            0.9845275285642371,     0.4809313904914674,     0.9735731762954789,    0.17294585753831992,
            0.24838795395334812,    0.023193114474997056,   0.1677125151942439,    0.5980536563961926,
            0.5094414515754091,     0.5848334887699315,     0.8434228252082491,    0.5081123729304549,
            0.8334310703822422,     0.576281011375385,      0.727398688422103,     0.24250344784585076,
            0.26248952658702684,    0.7857180015568903,     0.9586489749249053,    0.6242176521314566,
            0.3125571475935296,     0.904140994234653,      0.0248786103272427,    0.5120182640408835,
            0.29296668216215205,    0.018175546421433597,   0.5830810683650586,    0.31239383484399397,
            0.5133045603102073,     0.8239687413547585,     0.46903947330926243,   0.3068865751767762,
            0.8957272222664127,     0.82794151092707,       0.921864906207686,     0.6147469688809836,
            0.6070393815969863,     0.9125018173596807,     0.7180590457979998,    0.5221858544982311,
            0.7340443908857213,     0.6038863399482572,     0.8610766999390486,    0.061324527421735886,
            0.341826990505229,      0.9005398881117763,     0.7230753909523456,    0.14739802123142443,
            0.14209125041580228,    0.5358758818702276,     0.7189664771481943,    0.9300285770261746,
            0.7693915123560771,     0.019251287073499546,   0.9248839830120014,    0.12621894400507894,
            0.6867737288479482,     0.032680230880641314,   0.4069136584337537,    0.6253620375462905,
            0.484710004813795,      0.8819106166842692,     0.20096370427907773,   0.8767716072491476,
            0.15960265132057694,    0.4072239661601368,     0.18500455513199598,   0.9876394034684657,
            0.7732073919080598,     0.9691301348452812,     0.8316759940377845,    0.3390482084822115,
            0.3068363628806766,     0.89429028538344,       0.3507965907102011,    0.8025229912756378,
            0.32891518374725437,    0.1995050643077776,     0.11345689929296021,   0.17715025247364635,
            0.6137519414216935,     0.7505328581926496,     0.6206097253399693,    0.5581998354828086,
            0.3890985099875842,     0.6041862610103657,     0.6887317822347351,    0.23649827137390378,
            0.4035361076165067,     0.907810064530496,      0.5512149530609041,    0.6006771364583976,
            0.5364266948952668,     0.18812340634434443,    0.24008019293269933,   0.12330343847850334,
            0.7923882639552893,     0.788794433348703,      0.09001757626753426,   0.963776828369856,
            0.38997933298417775,    0.06913317435072419,    0.46540111757705316,   0.0672872355426023,
            0.11970712197334388,    0.4837497438343644,     0.716006175723687,     0.9196124482785273,
            0.2806521491767219,     0.4924204469842667,     0.575221183532452,     0.7082377001353426,
            0.3893620091624832,     0.768575683521727,      0.1666110102015781,    0.8217487545712984,
            0.6846048419441862,     0.6484204249873066,     0.2887921814417467,    0.8074589850504662,
            0.5721180734009176,     0.4948531394053586,     0.8266828036249912,    0.6002156077046021,
            0.8701953052395094,     0.35966990082375916,    0.1391258762896067,    0.16570526960164822,
            0.28651830362544173,    0.4209468159672606,     0.8635118930924496,    0.002105787324557462,
            0.2986969731657273,     0.6324681441508591,     0.5144504246929688,    0.9839651495710753,
            0.925487354312196,      0.38684611218736664,    0.8108566785379234,    0.12860817956540105,
            0.35021553632084623,    0.22879031476930356,    0.9327602696109594,    0.1171975798340954,
            0.3749003812688314,     0.5585917445219102,     0.47648012276778173,   0.14792193396090836,
            0.5579323016162466,     0.9257747733243183,     0.6454751055754582,    0.08902832364082969,
            0.41407476566965784,    0.5030668155257981,     0.22392580321263844,   0.7043137117556717,
            0.5889609256421384,     0.07784725750424648,    0.21837353609550036,   0.9775765306028937,
            0.7514452960820399,     0.21500455747244895,    0.03290953413354536,   0.7309979480137581,
            0.6631765136220552,     0.11501792606472094,    0.0638901299986917,    0.09142430319850059,
            0.04663221593976219,    0.4325415576671975,     0.20250946390595048,   0.7764686233338783,
            0.4323002886695487,     0.30075383252854215,    0.4316586449563681,    0.059885683881701346,
            0.044153467557825454,   0.8935041504972123,     0.0621708142544537,    0.6732993069806318,
            0.2774134661046056,     0.8541275762303439,     0.8216988069874015,    0.11339178565233443,
            0.004412879002477221,   0.8318428611376332,     0.8123648883815588,    0.6370638647189433,
            0.10654390706417594,    0.08015229654888001,    0.3788005682981507,    0.8812993715788957,
            0.5619597016662584,     0.21920900354534312,    0.6483033859144742,    0.10851351594888248,
            0.2679636649610744,     0.7775006389844328,     0.10223863685007828,   0.8233178820297202,
            0.9015206721860061,     0.7421818127428116,     0.89120891843561,      0.8662696959482991,
            0.5238487920744104,     0.13031015249773115,    0.8122146489924141,    0.519649573401864,
            0.6062902919061554,     0.9289469874268724,     0.9154548321372453,    0.34181309802928206,
            0.22391040221784797,    0.07534484858417001,    0.6439959087282352,    0.9557479018614742,
            0.6948757339995211,     0.049963193467885914,   0.60894336917863,      0.8375203024627258,
            0.6308213194733441,     0.7411289925447127,     0.48738162529546436,   0.4487463792772949,
            0.09463960166913454,    0.4917098078464722,     0.084679545424733,     0.8981172106185014,
            0.5643407456087915,     0.10280956965060795,    0.8446685996472796,    0.005672569156133678,
            0.19430454370552486,    0.41542539803568546,    0.16139040859770426,   0.8148232444632695,
            0.00219476245394179,    0.558126251727518,      0.025490801311942368,  0.6243556794150557,
            0.15263981875604826,    0.46113324845654036,    0.09439774163074055,   0.43338363177681083,
            0.8735631903092378,     0.548737972500742,      0.356577731866747,     0.2216284515060788,
            0.35808441524217993,    0.19946198866894305,    0.7972599484010688,    0.3969494681372636,
            0.41303112591424085,    0.3629446841370786,     0.43815260135519096,   0.6081897376303848,
            0.06061842189676503,    0.45448279770904665,    0.8651442426808169,    0.875784558896079,
            0.26619078665105345,    0.6687528123676825,     0.5293380989606575,    0.3606973326886752,
            0.644668779378844,      0.5283358938106392,     0.3761013949497658,    0.1444075025269388,
            0.2964504848233692,     0.8260597276896579,     0.0010408844952033425, 0.6327646232036179,
            0.25519377871235494,    0.7334459062910789,     0.8352962334396864,    0.3712143547332777,
            0.3857487965639541,     0.8908591275975474,     0.1632486684334511,    0.05585055487883728,
            0.29876702859262105,    0.9212755251623836,     0.4877536995687354,    0.16312621646167935,
            0.06198923668777623,    0.3408401771661087,     0.3437149336859253,    0.965569472171775,
            0.4071605953291859,     0.1319173782542049,     0.9210643219714407,    0.33341408227859104,
            0.26794225996066756,    0.7271841327337942,     0.500512280771342,     0.532695588261878,
            0.5315885920567364,     0.04016188132390264,    0.6567369668594921,    0.6876916189734837,
            0.04370932251268134,    0.6237993092787643,     0.10145884299695607,   0.7961359183941386,
            0.6987872082606429,     0.35586250958042254,    0.035145325217329826,  0.060568669216712334,
            0.5468054342866828,     0.7393761292911065,     0.15670465181461124,   0.8885322749284394,
            0.6505310431462754,     0.08900138402539315,    0.517909975243018,     0.510026910570548,
            0.9996659700671005,     0.6598015799757174,     0.26010373975449186,   0.6553309858599665,
            0.7059001111288336,     0.03451288281714926,    0.564822482703087,     0.7950942413110569,
            0.9718398887831793,     0.38896727150691845,    0.9549654767605698,    0.7610265087207583,
            0.2466482848656565,     0.9826401165829733,     0.6724416529809022,    0.8553310159247016,
            0.59012229030453,       0.7893202985537364,     0.3880540541277854,    0.5397899758006192,
            0.44891113982799047,    0.7940119733291549,     0.335713647962803,     0.1255635719357745,
            0.2993241079455562,     0.8339833103857486,     0.8587951659765223,    0.5168356364606572,
            0.76857854596841,       0.13921789802027074,    0.8792955541141682,    0.3147070194056061,
            0.16839495249502945,    0.9833910146930196,     0.5527135313737833,    0.7535831659267808,
            0.1206764827757475,     0.5817707669612704,     0.75484924910893,      0.8564088753374455,
            0.10027374478199413,    0.21901293916668552,    0.21372659786785808,   0.2610308811511607,
            0.2077100158056363,     0.6808735502500124,     0.9364906709807392,    0.3994579834368802,
            0.7920285370885659,     0.8767103114330838,     0.6662238298581796,    0.7754728851504256,
            0.9800903040300273,     0.7277460129131508,     0.3832250915755193,    0.15417868149594816,
            0.19094404961675926,    0.5087669646008749,     0.7602254425791766};
        counter += step;
        return double_constants[counter % (sizeof(double_constants) / sizeof(double))];
    }
    uint32_t get_uint32()
    {
        const uint32_t uint32_constants[] = {
            2158666988, 41566169,   2242813059, 1713905491, 2606451716, 3868193376, 953481666,  598543991,  1620112842,
            1783380745, 702486269,  3481835270, 886505658,  3711857897, 1912586610, 279198896,  2990750274, 2293739987,
            3457210255, 4018112145, 2763371318, 3726406775, 1096023021, 1186046627, 1116027162, 3883451390, 1413212609,
            1922870598, 2144350207, 2531522485, 3583052253, 297082084,  1346016468, 1498830614, 2975560587, 3864573242,
            2513414439, 1926180840, 1507012395, 3810784083, 2219900517, 2630495414, 3443916083, 1257987869, 1455772753,
            3493511787, 3565932205, 3654154348, 713252439,  1231100340, 2507469338, 1255110123, 1473099086, 2898961517,
            3329252557, 3210151383, 4101284308, 1045422807, 3689845597, 3559599122, 762352229,  992604139,  3516511433,
            310565989,  2350549856, 1787529558, 191559108,  2969207420, 1959453500, 2869200955, 253142935,  83104345,
            1326000413, 3127635973, 916430015,  502338120,  1279770172, 2684782390, 6410385,    301927079,  672926313,
            1327095627, 30026292,   4272216602, 713858743,  1020363043, 357184972,  1066014934, 2467663112, 2391201341,
            3047502643, 3754235121, 3028204666, 863098688,  504881819,  4257903901, 3936895409, 3484823576, 2920531816,
            913607873,  1851335489, 2729323520, 430812535,  454713344,  3606777711, 323269189,  228087775,  3715774926,
            87304774,   2724174865, 2741710073, 2363411441, 2984351010, 3122302154, 3761742840, 2637795393, 1498606255,
            365444637,  4018582963, 616621843,  536217682,  3526534505, 2151710982, 4288448152, 321129786,  3618224247,
            1760120453, 1319764869, 923677380,  1980023747, 4142526672, 1171973909, 80415349,   1504514183, 1815926859,
            1098670025, 4286748890, 4199827595, 1735659228, 1598829280, 2720631255, 1366940606, 2727212117, 3766845735,
            527239947,  3493602309, 625302532,  39812166,   870083950,  1786903320, 1880636901, 4093266999, 3282335302,
            357434705,  1328655766, 1547886936, 318515014,  2314429322, 2141252918, 2976923311, 369989193,  3283646902,
            287922603,  3777945103, 300919787,  3584241478, 131942094,  1934539312, 1935955185, 2520746479, 1019305967,
            1793998806, 2609753108, 436646368,  240056312,  2228855878, 4038353379, 3584669790, 1651887006, 2404125598,
            3157825810, 3221667529, 868172354,  66146997,   4022808626, 475819631,  4125130933, 1469148787, 904808754,
            2163561042, 2071071076, 2424735571, 3309468947, 2563037901, 15047719,   4225844177, 85031320,   3153777246,
            977619786,  3283632453, 578076009,  2853928610, 1041016160, 3196664503, 1496700517, 3311367586, 3675589745,
            396535749,  2068790523, 394454707,  1187963860, 2943363568, 3636043126, 140250703,  2835280382, 3545386937,
            2517461116, 2351103394, 2218750624, 638891508,  1510935278, 4238333939, 481858224,  123947157,  1193350485,
            90689506,   3308396371, 1966200518, 947655450,  731953131,  4123495735, 2598767721, 380288531,  775781230,
            3325334231, 3831585271, 934900884,  722596038,  2743507613, 986030440,  4056516429, 1473215144, 4256082289,
            3320633226, 3152690777, 2340508503, 1592677695, 1294814188, 2248830908, 2165980709, 1115104845, 4065450736,
            2887934294, 1599817958, 3283096764, 426517169,  1909580759, 944023695,  3547361356, 970009285,  376807911,
            2201401838, 3057162944, 1180624372, 1893697363, 3011229047, 3926971963, 3741420086, 3789566068, 2069995971,
            447946484,  1708268297, 2571646232, 1320924626, 2420220487, 273213642,  63841886,   854214527,  1419911288,
            2643013425, 1021217668, 3769479382, 1421863155, 2086393539, 1903914849, 305952136,  3620996500, 1730535425,
            3041968399, 2543371796, 2427288063, 2474673125, 720981692,  1775148986, 1140782432, 1240271140, 1401874616,
            1692851142, 2794166075, 826915029,  735946224,  3787551975, 271025114,  1148636114, 3662011708, 2254838561,
            195264874,  1642571153, 2628864601, 287639927,  4247575144, 3325145638, 2626612274, 1776447500, 4136639730,
            1936450687, 1972169521, 860985845,  2925308600, 1043353575, 1634823346, 62013364,   3069122110, 4294393969,
            3311229445, 1160185753, 2289687531, 3992653060, 2038702459, 1141845983, 4110130152, 895487168,  4205193680,
            51526245,   2442642029, 1017197722, 2778708520, 781084184,  3313537175, 2924213803, 1429883128, 2328546392,
            2223325536, 641884532,  1111922560, 2372666392, 368467430,  254384864,  2340889844, 960041935,  2388775929,
            1477371842, 2305086875, 4271566542, 2911585819, 3783293085, 1388560321, 2381324061, 2879313423, 2201311855,
            149670241,  1543000024, 3127174901, 101484676,  2263444653, 3856713647, 132620514,  1414705702, 3957504614,
            10575878,   2134237362, 246539754,  2613514110, 2132075163, 3385294990, 840125174,  2528932933, 3925737388,
            2443832533, 291967975,  2388784841, 938345541,  2158137238, 376923552,  2391755569, 3183922158, 2514008342,
            1090063300, 2894045126, 2700022623, 1654621821, 2948785471, 1136077732, 220626925,  2540745933, 375321608,
            2887774569, 1313420670, 472519803,  2920185888, 739350232,  574202953,  128585609,  2849069346, 3131161054,
            3285949925, 661841087,  2342952145, 1606250822, 3058592130, 1414805631, 2666503285, 2385627271, 3861465726,
            4229920348, 3703827984, 1496762829, 3326377974, 1978482607, 1167568398, 2491837766, 3627036399, 3717137080,
            1414397933, 3698178328, 1636055405, 2246084815, 1533233764, 322578868,  4226203633, 1402580679, 127206521,
            3634617231, 3559375418, 1618342189, 606523218,  621521914,  745863931,  3915798968, 1960416985, 1802163069,
            1163417280, 349697540,  271961308,  4204276959, 1284440414, 1222311033, 2109153000, 3055887417, 4191198224,
            3408167108, 1421633153, 2888797160, 3658059744, 95625730,   1427552733, 1056998492, 315284296,  566470416,
            136890407,  4272337730, 1206540696, 2707458537, 3816668534, 3775659601, 1946360202, 2029191337, 2543085758,
            3386524130, 3072383890, 1392653552, 839710844,  2667859584, 1434661193, 929671936,  2273163907, 3004864078,
            1890026722, 1909212287, 3046571464, 2162748519, 2499462514, 2969929317, 214958837,  2750074569, 638119832,
            3441133351, 2723485457, 3277269270, 3333715478, 3766535300, 3005043891, 2477286381, 375621329,  2877582561,
            2047350808, 321455664,  1409879168, 1909809997, 2702241041, 4079275454, 2154649136, 3454715810, 3411931313,
            1600074000, 3339798888, 322090109,  3170848896, 2972648202, 750136610,  793236930,  1904573536, 769191417,
            2690323228, 3798140459, 1797714322, 4140517412, 775591934,  2713776145, 1920425104, 1552784780, 1431152812,
            2835349519, 3715009506, 1572569530, 537975892,  2030190502, 3287012384, 3407729402, 1369006797, 2495698181,
            1126974116, 4185099956, 2355779643, 809258994,  1907987615, 2857521463, 3790651456, 3186753495, 3456652606,
            2788203457, 3134393715, 2450711152, 3739373620, 3914792714, 2219173001, 1192274059, 1467334073, 449441582,
            3482726560, 2645016244, 4084213005, 1126485021, 1973888613, 3288340636, 4161824387, 415876046,  909194328,
            2073740706, 748804308,  2571712981, 351609572,  1755708289, 1922853881, 1763344113, 169387514,  1354753129,
            1121519667, 3947609031, 3179353132, 145048546,  1750424928, 1171152050, 1640962698, 2118876960, 3402840592,
            1966991108, 850008765,  1841820888, 1145712361, 1297734395, 4063987854, 3158587025, 2630971506, 4158595949,
            2750348381, 2091590722, 4067056253, 83283098,   3785170991, 2183827963, 274811911,  4014308921, 3646693204,
            2819672198, 3430089944, 3505959203, 1175307952, 2923477860, 868978798,  3656546523, 3527376019, 662159573,
            2956021134, 1882909114, 3521514112, 2794700378, 2793450123, 1377525281, 900992029,  3802075154, 1030766716,
            334651238,  130152437,  3965336362, 3712928372, 293885426,  2527735393, 3749222267, 2163624535, 228133853,
            1408566025, 1033977817, 3422834629, 3224939976, 1768370140, 3292433792, 3469081293, 3241409763, 3523922323,
            2527405560, 2829759019, 445243388,  2297476307, 2263622072, 3828717731, 61331938,   932674048,  1896114890,
            1608794907, 1714948940, 2095632877, 3667353762, 1243227339, 4243716586, 2849681726, 4146720687, 4131223740,
            615933150,  2313039170, 2651049422, 2094377459, 3914650399, 3482443014, 3120835440, 1483512517, 981852104,
            2978180335, 1312664588, 250297655,  3751554489, 2262627489, 2382270816, 848980143,  4281696160, 28744765,
            2757119211, 3139452831, 1098105073, 2899751242, 3273912223, 3777797513, 1016280294, 1761734252, 1121133524,
            1834556081, 3990715957, 831334863,  1407054552, 1561848194, 3885311781, 1100286040, 1729800347, 2702792271,
            1240039449, 3696120292, 1953333085, 132060646,  883238136,  1908364952, 3974297904, 328580209,  1263185644,
            3097261595, 3598253374, 1118191323, 4241105895, 1860286212, 2960050743, 4161486300, 2910270767, 2664872994,
            2091639379, 794667240,  732930159,  652891413,  2744174160, 1451988725, 1735577106, 4032670716, 1939364797,
            637321123,  4082700363, 2752566145, 2317883847, 3805352069, 362028889,  2440982516, 1068285473, 1488586852,
            2283142646, 3188631285, 2710105631, 593658761,  3518900088, 1790648401, 1357055687, 2616680098, 81398103,
            236114240,  1590240213, 1956597413, 2682758294, 3596038133, 1388641188, 2578546944, 4128922543, 2604973361,
            2834769202, 1403490523, 3866866602, 3856236837, 213082863,  2598051127, 3215630888, 2702323815, 1825471041,
            1475220004, 872539704,  915821884,  2206034168, 1860694830, 4227809351, 1779205456, 3823542226, 137675546,
            3756600942, 435046124,  2842357559, 794207264,  3280117767, 1956525338, 1699475450, 1693192767, 2329514035,
            552602476,  2864055387, 1529856935, 2525302499, 2847224691, 1232855034, 2988132380, 3802051754, 259846817,
            2242868322, 4184014163, 2825321620, 13534986,   1685503649, 384385435,  1304163609, 1141547320, 1348176993,
            1359003024, 1366067306, 1655431191, 2340918079, 1136617961, 2221954843, 155659301,  1885664703, 2545577293,
            2067440331, 758485997,  1127703486, 1112906866, 2946885148, 2372018278, 780273444,  1459029787, 3534448768,
            2886845175, 619835831,  537701446,  1262187453, 2403120953, 3467810099, 1381500954, 2667838332, 2696363531,
            1694065466, 3886560532, 1742556582, 3803079711, 533031983,  2432838223, 1081056868, 3544141361, 3137627459,
            1843582974, 3854340981, 1909579154, 115295846,  1658305106, 1100571719, 3750969320, 1879947804, 3880820583,
            3585742593, 348290146,  2395402028, 4018307593, 307554892,  1109247882, 480663901,  3841337215, 3969132665,
            1249511307, 247848238,  1226074025, 1158848660, 3367293446, 2356329539, 1998125859, 2892161465, 418274456,
            3512426131, 4179931644, 1083763791, 2448369157, 3604925494, 1384264685, 399314487,  329491597,  2141135546,
            1282874215, 103046312,  1406865802, 3078408480, 1335164879, 3164442880, 1367494749, 3729855894, 1034138485,
            1193373705, 1295460939, 277998263,  3283810292, 1398615835, 1648971479, 342377357,  1404841455, 2793768216,
            329677284,  3014874788, 3605511596, 1139145812, 3185034003, 4027818039, 1827030370, 2649145172, 2174231519,
            1983896064, 2672618830, 3572126180, 3024656566, 3928770295, 3713579450, 4192432122, 3393072710, 3930455523,
            1540248472, 4210019567, 1528184414, 2489059390, 2382921539, 2284746314, 2440433121, 2274431747, 3477784399,
            3486495613, 135167947,  1791732668, 1256311850, 1615540681, 3658308418, 1521310150, 4075157030, 3478497250,
            2648749653, 1904340621, 800591401,  2337381395, 1444719659, 481948852,  2568386652, 3578247243, 257893273,
            1990952984, 4049848971, 1822812899, 2771185582, 3906214541, 2847954351, 225840234,  3350282473, 2179161857,
            3141962805, 1757701555, 3006637500, 2158751912, 284714304,  4121460627, 2599487248, 2019352924, 2576609378,
            2235276959, 1788680407, 3285349178, 1524848012, 3381160503, 2843162695, 2619261578, 1951237336, 2394879713,
            2356170324, 2792701016, 3404008547, 1002368440, 1419997935, 4274316820, 4272792312, 2600711761, 2924208006,
            3799196687, 696950966,  2896574900, 1320867021, 1371125429, 458156178,  2490470440, 2219426620, 973212528,
            962603818,  3028036675, 1092766318, 700077671,  1961668807, 1413467709, 3551952291, 2243578803, 124518171,
            2990223204, 2950073079, 3732478177, 3288060071, 1919120565, 3812014614, 3608862831, 431424612,  1131510838,
            1974205493, 2177660085, 3387256777, 1464414294, 3144390273, 170625084,  2670108972, 322520664,  407651574,
            3367433081, 1023268981, 4294542649, 1648604185, 2282100174, 848163558,  2648686607, 3586320674, 1942931408,
            4181145468, 2084670734, 4055061939, 321659295,  2205837499, 1179724475, 2633022175, 3579309098, 1108502074,
            2058030166, 3314941730, 3818763523, 4282431620, 1146563866, 555631220,  210570370,  2184821613, 2272729339,
            2413880982, 3638531794, 3361320941, 2401767708, 1529464072, 2466734206, 2243057527, 2980199868, 2760710742,
            483732721,  1704383348, 2325064790, 1848981566, 1424373545, 752210258,  1368884955, 1406336094, 1181447224,
            544293349,  1292965073, 2805498791, 1820046192, 700731872,  4049120088, 2684807093, 3593706742, 1357350837,
            1557310514, 2737286502, 1779747765, 2809114953, 2903685924, 2909304208, 2740902951, 274591554,  677369814,
            3606800113, 2822291002, 2897175326, 3215594664, 3603579319, 1575627831, 567987898,  3848728359, 624094145,
            919703489,  754070899,  414406002,  2869059975, 3046079982, 1339376144, 1559336480, 3496687851, 2087336061,
            1128673960, 1287181558, 6801363,    2206476874, 2270106891, 3776867137, 3083213895, 664755430,  1045570718,
            480207222,  416593112,  3695211324, 1478672597, 2113676978, 1968648315, 3769994202, 1651225661, 2162955146,
            596615403,  221244731,  1567173201, 131815261,  1905585902, 1525265951, 2833077531, 4254008891, 243311322,
            1797634919, 3818580251, 245397321,  172381663,  3367885612, 3038857154, 1317817258, 2553131875, 4164019646,
            50317131,   2414368718, 2895502227, 2596980644, 2678300439, 3009869408, 3613859575, 1851711752, 3168922032,
            3166142976, 2929184727, 3374941605, 2459483718, 1080087746, 61398623,   439523566,  944147042,  520666539,
            4075307057, 419618599,  488558627,  598073463,  2111765089, 2909341655, 1514592426, 4195923145, 930569264,
            1840813304, 947863014,  823083115,  2405319331, 768413657,  2396078648, 909652819,  3152305242, 1688520310,
            2429923569, 11969372,   2792921291, 3595735387, 645583735,  1763222155, 406035086,  502382158,  2286036272,
            604883859,  93252416,   973498843,  3589122701, 3759485421, 1921560734, 2310360674, 3648192948, 1681279423,
            2372135375, 2402107098, 730662941,  696917368,  2093078477, 1278935804, 1472617163, 1498514050, 2649616273,
            232019852,  1095401883, 4172674766, 1874215631, 510878190,  2452331703, 4284764625, 586391767,  3208727113,
            3898750327, 3295423717, 2560283486, 708895219,  2170682566, 477352602,  4051489561, 3562386811, 705265709,
            1650521096, 1202519480, 489140025,  1969535439, 58606358,   93891914,   1841522806, 140685194,  3546428899,
            3832794904, 455507987,  3720870270, 180443184,  3540558889, 3308294696, 2765417614, 1384336507, 1436383559,
            3339787431, 1106706963, 932556649,  2743889112, 1589032423, 3823186543, 943259448,  2501729856, 2881943698,
            1588826062, 3022507748, 711020356,  4235526837, 2730014119, 3400977460, 2436065017, 2540234136, 584958300,
            2547355104, 97484500,   869793999,  373708060,  3740163416, 2234476329, 2379585292, 1412978683, 2938563218,
            2150151462, 4223150016, 3106644146, 1213466128, 950688665,  3314775082, 1776896636, 2923356624, 2444208017,
            1167632176, 1833074594, 1227825631, 2552435445, 3966806405, 2523342892, 2057618336, 3528260322, 2414341212,
            3917418837, 1915630902, 570704484,  4033976909, 53600857,   2643690281, 2227184609, 1577165305, 1671824914,
            1839206768, 2962846209, 384523954,  1491795207, 3289639366, 3899575807, 3016198223, 3513611710, 206164226,
            1226423815, 2787287316, 2431131724, 2949124558, 192537088,  2589742310, 4241132407, 3454706241, 2272299924,
            2343268121, 2007664022, 3486236986, 1307823360, 3815894767, 1706356681, 2286333014, 1172330234, 495453859,
            1046648281, 3554171162, 2485118989, 2812329861, 3607384768, 1544157683, 3992580504, 848079470,  3041542447,
            4191338899, 2535200345, 656156612,  798092116,  3658473430, 571201790,  2441096018, 3438206813, 1410565612,
            154709992,  3000801529, 1645448996, 462508027,  3752855082, 3361294979, 2870413033, 3186126758, 3923983810,
            1941850370, 4188429895, 273242189,  1234068746, 1114370913, 3054592005, 811913349,  821035974,  397564902,
            4126316445, 3844378680, 568208165,  2320060208, 3320099290, 1271423531, 1417324103, 2282105257, 1190417363,
            3034381859, 1109805343, 370453535,  979626402,  3474549307, 4291146241, 3377129404, 3291758307, 774000051,
            995035488,  585172188,  2893340736, 2715811960, 2086085581, 1107324609, 4127015079, 1485151586, 2201091928,
            250469230,  1046502600, 3254239709, 2755089305, 1009697895, 2528081218, 175600315,  3501448994, 901728738,
            3792338128, 676379456,  914862393,  2487801970, 2509611351, 1348159682, 2405746514, 3927344777, 2520198449,
            1858031069, 1267233997, 693640692,  2552228259, 1176133014, 691074585,  3487438997, 937358127,  2850322587,
            3606231003, 3315787338, 3528575623, 3792904805, 3340389738, 3871976702, 1561677598, 1196983476, 3835201149,
            1493591339, 3619973128, 1568693526, 3397453130, 2024517474, 941427061,  868472907,  1670534029, 2109215696,
            732821883,  78723504,   2165599025, 3692799882, 2155894668, 1499816103, 2229107099, 2640254266, 2982369738,
            3441086384, 1946967080, 3019637624, 3329989489, 3687311326, 1640598327, 3808087146, 269142855,  1632664872,
            3971707738, 2595313115, 373724739,  1093408840, 3051853113, 2584567206, 103558321,  3430090342, 792289162,
            3873977595, 3598337913, 767197210,  1460432796, 1886348351, 3876026993, 4149481356, 3182221952, 4149477985,
            1535479140, 3221741234, 2088144508, 2891186820, 192364089,  2457101308, 2560921925, 1874540478, 2103951833,
            2037245347, 305042345,  1192631085, 822111081,  2898851741, 3090755911, 4001741650, 3404041017, 2229679043,
            1501885861, 2746117072, 105272378,  4000328722, 1730967292, 2879040406, 2228889849, 3935773512, 2743499755,
            1015108212, 1902739887, 3548958212, 696646080,  51845250,   618062993,  4161267246, 3250756120, 3530821289,
            1872608424, 1327674787, 1049501143, 2604703217, 4282993202, 814117162,  4286943736, 1679860498, 2716783833,
            3820210854, 3551989926, 310556439,  1161723426, 2331730604, 2265791844, 2232838156, 2309910773, 3273385984,
            402061132,  4055947717, 2919809338, 39737082,   728397586,  3368287744, 1913781737, 356739286,  320426938,
            2854675109, 4170900323, 2135551061, 1408689164, 3551724697, 1428803680, 2473143223, 2778970560, 2044748953,
            675097387,  3029388016, 2523777647, 1763435060, 1885071665, 1036444383, 3186560831, 3114038268, 1180129171,
            130714909,  3208725601, 4216192743, 3616366956, 417664759,  3926591346, 1738904961, 4211154712, 912944107,
            1629830711, 871221953,  418125851,  4071702998, 1426552588, 3778594430, 180628425,  2476787581, 1172160691,
            3804443737, 2764856832, 3069288908, 185423547,  3484350312, 3648696802, 2342856441, 399914340,  3055160038,
            3836607482, 3255985104, 1118320215, 1800683547, 4207165378, 302057947,  2117451818, 3599110671, 2090826921,
            2545297052, 2337434219, 801687905,  3585807895, 2490547894, 4098575672, 3562147265, 1042825839, 1983600385,
            2882927473, 3703873500, 1062240801, 4234476471, 2316622712, 1226362031, 1959943587, 2234381439, 769453343,
            3612386847, 1368156543, 3714930252, 1684872084, 1995417666, 3075331313, 3070227268, 1232161167, 2698899334,
            92292651,   540999128,  4198132946, 2133994742, 3338783854, 3239937259, 1102454444, 2805328653, 3237282504,
            2775707532, 1855321241, 4100055610, 821312910,  4250038806, 2417481505, 1919116829, 807740557,  2784193945,
            317410769,  1107864,    4106569599, 2668922018, 1252276797, 4165875163, 1262425792, 1181202529, 1204492292,
            1091213975, 547422582,  1667091094, 3626891978, 3188901382, 1670312384, 852381115,  873164949,  1225723946,
            3024021999, 2333634790, 1121998456, 2606044480, 658476826,  1998781478, 3460417791, 4118344782, 3842479887,
            8729057,    3693800364, 3081254562, 292234496,  1705338491, 814275332,  4062806813, 4236863682, 1743214436,
            1719955450, 565231168,  2540025880, 179148131,  975806207,  3177793110, 3634924939, 2810229628, 2852242573,
            348883471,  3726673895, 4205680801, 1237876094, 3727439460, 2857556789, 2326582979, 2541482340, 2812726871,
            310417785,  653618250,  4062576739, 907492906,  3391207180, 3648164122, 1057470372, 3348198382, 937233976,
            806383731,  680128455,  864251865,  3500284945, 4023961571, 2390935430, 1806389126, 1697546596, 740341438,
            3671279504, 1938647717, 1760034242, 897323370,  2166361656, 2666954004, 854163493,  2289629179, 715574306,
            2624834367, 2891155278, 2670730351, 4154407366, 1184288084, 788038744,  1086874695, 258685701,  263608388,
            2890830088, 4237171759, 2144648923, 1264350348, 3792453194, 2177861553, 425834265,  3231381881, 2680532761,
            2723748038, 412636769,  446860443,  2129492853, 108888206,  3010940704, 2052769050, 902830204,  261347494,
            2096151215, 3255146790, 1190113744, 3779823974, 3307502753, 3836881007, 1361451517, 1114493739, 1107418610,
            4049121627, 1423854880, 3070549565, 460774681,  2255609896, 684993156,  435683891,  1462359457, 157554082,
            916291012,  2973071116, 460497646,  550166729,  2082590399, 2258337337, 3381630551, 131289371,  623708770,
            1200952313, 649523733,  315603821,  3320198398, 4059943705, 1913914549, 399696933,  2011926856, 1771562550,
            291774797,  3374823179, 2988580785, 2046729079, 3901782055, 98246793,   2744219335, 2488774582, 1572936160,
            4275986582, 4225514994, 1143146995, 1116718058, 3990493156, 2888075196, 2088935215, 4189017949, 1553375433,
            2698493767, 407434025,  3779073778, 3804782452, 1165223537, 295758051,  490236184,  2738060982, 3087660405,
            1841549113, 4180083526, 2749239620, 3714751679, 1672025085, 3140215530, 3009632415, 1209108272, 570308889,
            2802884211, 3030503551, 2116750586, 293015859,  3290404025, 1770348074, 2671919246, 3116355602, 3452673120,
            1931588313, 2102625355, 3132825505, 3175902582, 3464288220, 3480074360, 745212889,  1835459712, 4277081137,
            2714763947, 3237994475, 4044967160, 1647749244, 1374837907, 2582877611, 1201004268, 921634247,  2220869864,
            3027062644, 1054796739, 905830666,  3521669019, 3284878354, 937944827,  761915506,  971846036,  528166716,
            2850153542, 1059398882, 902843557,  1495645084, 2160845409, 1158895420, 1019405983, 611283040,  3569640190,
            2707285971, 4187363037, 1425830687, 700469064,  2707046806, 3082933918, 4234168121, 2097093701, 396294844,
            2639258447, 1334341965, 30434512,   3792466979, 2584859082, 557326107,  2443283535, 353463532,  3534700728,
            2261680231, 352605913,  672321308,  4087006832, 3908938513, 3066183096, 307866759,  1701861111, 3328534252,
            2318093938, 221450551,  3956223792, 608052154,  113632748,  845599116,  1512064043, 802121572,  1660895572,
            480799984,  2953814970, 96040099,   3232765534, 2506507236, 2107024367, 1116268238, 644977591,  2459624397,
            1836110127, 2795351966, 1953160332, 3093663887, 1595447375, 3598507437, 394763339,  3318090570, 2229066651,
            1324108080, 3603332253, 3657268899, 450031508,  327209329,  3625706177, 3316400357, 3969755726, 2380028787,
            1362781297, 1668799974, 3193538468, 1736866519, 1550621094, 1827667605, 1175662960, 1228207257, 983409684,
            171357785,  2527567966, 1030068253, 4127402632, 3293125659, 1050084238, 3520716777, 1390168934, 2554184178,
            1534715750, 4180983266, 3436179552, 3787329602, 2841499792, 1607590379, 1967261304, 3954455056, 1720176254,
            2650496224, 1062864095, 307369638,  2614533958, 3099524173, 1864555180, 169444038,  3528109345, 138525419,
            266176654,  4252526977, 795024827,  2482298300, 58113327,   1539210032, 3688183310, 2077198384, 1766547342,
            3613151084, 1117122266, 1161268109, 3046447660, 2045364360, 3737405386, 1712515142, 1537326753, 3048303239,
            2243753905, 1392785134, 2788611894, 853306679,  200327078,  4087771536, 1846518339, 3899730276, 3931283678,
            1571846451, 788063881,  3487960746, 2811845215, 1087251553, 3793476453, 2802083246, 3403453517, 335973593,
            2959741217, 2996421233, 72481483,   3813313057, 2728109877, 3484703806, 14273447,   3682550110, 3367944329,
            1854597479, 1373450928, 363870760,  122952613,  4120955894, 873241611,  1771185986, 869868137,  3570824371,
            2490309337, 3109464591, 3887570918, 3060237949, 3713580925, 473123855,  1894373719, 2848143089, 184018536,
            395971998,  3236963542, 151430042,  1188183148, 1737870603, 1272861569, 3403483907, 1555936647, 3092113289,
            1728890358, 581794434,  2202026784, 4282240350, 1426739104, 837691724,  1873143743, 4204050927, 2688946535,
            988743082,  1751923340, 1452333792, 2726360806, 3841074380, 3343082695, 3912861862, 1499405555, 2894517394,
            1592536766, 435257859,  4017288440, 764404416,  4088651793, 1407410401, 1468627205, 1009212307, 415315239,
            573418150,  2454463686, 2778024763, 1586512190, 3004428681, 3812471857, 2248470257, 2804161761, 1749517575,
            718381758,  3813803894, 388965471,  3457108715, 2255002604, 840340435,  1254745861, 3141172526, 177319654,
            3393919903, 2958147073, 1052552674, 3131511984, 388462961,  2146014958, 211422549,  907976493,  3993853304,
            2399488462, 1531574847, 397175238,  4274905841, 2467401495, 352405655,  2549545263, 1601303587, 399921336,
            2155413841, 2090616815, 1008308945, 3720510223, 2253616576, 2651339690, 1636736278, 117591397,  3184690661,
            1574250940, 67202842,   904525851,  4294668151, 270793828,  4002238376, 3905604747, 1187315293, 3159841725,
            1418670849, 1323987250, 3111622414, 2428481088, 2793638719, 4034362607, 3407645889, 1225774062, 850266927,
            1572371206, 1105993877, 2238211171, 3786679406, 1866345201, 3177363642, 439803347,  803306985,  728250721,
            2419782417, 2712141592, 39370187,   4234872735, 2796092422, 3060298645, 1452562652, 3052772106, 661873466,
            86489014,   880178217,  1825417245, 2237276118, 1786332761, 3385759141, 2897645436, 3076631493, 2058173377,
            1412980193, 630369411,  4224380572, 3642562991, 1666967992, 1814063688, 1139541483, 154144250,  2997702880,
            1156183524, 3456397196, 1913196553, 1397329607, 3221735951, 921938885,  3516033374, 2776338272, 3739350936,
            788685068,  2985815728, 2725118129, 1005477623, 2338963002, 4199686985, 1067019890, 1080962074, 2259381993,
            4002647069, 4209680047, 2530541980, 2580337890, 2522222580, 1885408812, 2200474118, 2878409080, 3430332448,
            3865678220, 3522200575, 2063735558, 1483045394, 4111936566, 1718474670, 2212743205, 1217124780, 800146347,
            1476572233, 3064787349, 1755315880, 3115812683, 57229346,   3959929460, 3655762442, 367811550,  1623486459,
            3816380886, 238817236,  3204320329, 2391833889, 273675414,  3867840908, 3418813394, 3505425192, 198297710,
            1196731827, 1080266588, 2238244788, 3215387457, 3066142755, 2420600605, 1353896034, 2842799203, 3431102053,
            3328982426, 1467477451, 3598811481, 468755741,  3070152154, 3409562602, 2115593442, 2612915009, 3604090599,
            2900163372, 2768201188, 352140182,  4170954278, 3528187549, 228347097,  2428797274, 3319272559, 3946050647,
            1780469041, 2369460249, 2507185330, 177514964,  1094683242, 2721764983, 1453001434, 2620606992, 2987338631,
            3385207513, 322417003,  3743869145, 4028254011, 2521047899, 3411779166, 1961279478, 2121068159, 1818205857,
            2626957519, 2392491079, 3089431296, 4148137057, 2054803913, 693700093,  1852796979, 418153377,  2993989189,
            1118759508, 2462265844, 4252676396, 1827422154, 2853584033, 4263232169, 3526442301, 3396238301, 676400433,
            372384106,  4262374413, 3085212613, 2389018358, 2518874572, 4154449287, 4188615869, 1898755180, 2925336142,
            665681401,  356394422,  2882057590, 2073712992, 2832515206, 427704737,  3339240143, 2298514605, 3133714048,
            2380206310, 1208168216, 3074973564, 3198370332, 916435603,  165025200,  702451762,  3334036350, 2801802414,
            3102545301, 3350875114, 3356386457, 1689206656, 1300914858, 4099436937, 1079522713, 2832156414, 469527224,
            2892988268, 730638094,  81568989,   1138345577, 3141886985, 422513438,  3163550331, 935761740,  147209105,
            2917755136, 2563667472, 2335572089, 1221189960, 2797845746, 2030922336, 3222268500, 3350222423, 1875012915,
            913188073,  3567103213, 649728956,  3969843102, 2413407459, 1271772265, 1864515029, 3024387086, 1978738762,
            2189306248, 701767000,  4022539850, 1244508846, 3037860943, 3565142121, 4055972254, 4039612409, 3950871459,
            3361569301, 3931672015, 1861671761, 287990818,  2054985720, 3293981435, 1142436636, 3663004182, 548830099,
            860056494,  1427050437, 3917853727, 3476367382, 1324861241, 292863807,  1353344334, 3790692228, 564501663,
            2726953240, 3788978939, 1870949982, 2443187462, 1598704649, 869102240,  2747345603, 2949652746, 882557613,
            626721427,  3695475979, 3761275500, 3081065612, 953290733,  2427634287, 275675311,  3489237999, 1409364854,
            22217310,   3874277252, 1919987194, 3902061512, 499875881,  3522873046, 1548371975, 2916528714, 2063113374,
            3254579903, 1392044035, 180743283,  2367877209, 910252700,  1181472732, 3554936362, 4290901884, 4168855147,
            1188172325, 2727259401, 1739795606, 3080265682, 1523028493, 755772959,  1288302043, 3958780095, 3349284085,
            1735981011, 3678391283, 3239250635, 2600989056, 2036489834, 4212989537, 1955851223, 3606017501, 1120160994,
            4028423131, 200588549,  2366817149, 400871322,  769262945,  2059073003, 2356801840, 3524924991, 2024955325,
            2611073008, 3316635354, 2586877408, 2574570817, 3288285968, 3316540252, 79958527,   3493504889, 2494984173,
            3604286779, 3598216062, 2929295835, 794042362,  2532914920, 4047765126, 2491728548, 2186423348, 2253477564,
            604589379,  2541876140, 4106404473, 2814291199, 2560278225, 2153221030, 2527102764, 1288005380, 1549478298,
            2560147492, 952112495,  1046208290, 2391816412, 1535338719, 3657759417, 2755788456, 3677822993, 3324252966,
            1093184621, 3670757654, 3026219609, 1803459579, 859745042,  2730960697, 4068091083, 3509813172, 3616032323,
            2745598914, 47676937,   3644965991, 2751422476, 1097690794, 269494746,  449697168,  3024502437, 1415350991,
            1996683536, 2261977964, 1085166666, 1877967942, 2715095845, 4166792300, 1743341316, 381857324,  3821179457,
            1723896926, 1247122208, 841729074,  4169012172, 2813574348, 1572879825, 275757080,  1041062159, 1368580102,
            2932653601, 542459056,  4106763326, 1849102470, 2488382884, 2087183037, 3937055408, 1172384853, 3542358815,
            2985225033, 1651176377, 3760113257, 1726765558, 2304291514, 2377492920, 764618425,  3732778722, 1784727556,
            2182102498, 4098181398, 2817350407, 2635941002, 3500518689, 1589605794, 1510884064, 1758425907, 2678316955,
            4284769685, 3912417566, 967620981,  1751079430, 2264298092, 3461692092, 2928904353, 1427120450, 2118753864,
            2943198082, 2080830041, 1516990312, 3883727268, 1268445995, 1694692175, 3532738804, 3120289928, 3434046077,
            3576154697, 2184913964, 640584551,  227663130,  259848690,  3336027860, 2415066272, 3082535578, 2301394991,
            1588650804, 375226244,  3699352654, 2718641779, 3183377046, 874447418,  2842724036, 2471767194, 3147413748,
            3877376880, 1543146870, 1389267320, 2735683577, 541110467,  2003402921, 2323261055, 1995585029, 2078957184,
            1722646036, 4108532331, 1121473970, 1039665322, 3414434525, 906460656,  3117668002, 225845505,  3409232119,
            2461929525, 1724951223, 1963726472, 3394880502, 585381174,  2591220974, 386213493,  3888854262, 263404412,
            2940814018, 3970267185, 273816627,  1475532173, 3548757054, 2548378170, 3153240798, 3672329238, 211278910,
            104001302,  3241280754, 3545740470, 16820164,   3261213740, 1374371295, 155978151,  1424468358, 335271082,
            2119870322, 1003124001, 2110005140, 3764914231, 1539971252, 1382834410, 3573508484, 3223808912, 3565385154,
            3404514425, 3479535893, 1602777654, 1954338368, 4240079299, 47465300,   677186395,  3109052018, 3868633854,
            3512324627, 1769082803, 4110833394, 1939263141, 3036644270, 1849505923, 2546805449, 388638738,  794064742,
            2122155858, 3625992790, 3073047023, 670005027,  2225552682, 3627587828, 4179871718, 13559252,   1708029046,
            2175593997, 4005385631, 1338521947, 2733942192, 2136341441, 1823237869, 1824920472, 1310150834, 2076174418,
            448989204,  3947179076, 1006675266, 4100366005, 733281401,  704513131,  1653636794, 579085773,  1576941907,
            2165874708, 3058837103, 94571879,   3585387343, 2256365577, 1610131515, 833206413,  3056541718, 1302315231,
            282161551,  6001006,    2381565878, 2248935472, 2764634345, 3811890309, 1363389136, 539639285,  3330861532,
            2829911450, 2892949980, 2065749461, 1828402443, 4207168170, 506185334,  2024407027, 1891970038, 1709318193,
            3836575075, 48277532,   2283948714, 4228381373, 3101512181, 3246162054, 1701183039, 767347589,  2447138416,
            479840359,  1765112324, 3330783749, 3402276932, 984697268,  4147301491, 2170080507, 516140004,  1465842957,
            1192146667, 556677166,  1774938725, 1064094261, 2457567145, 955985826,  2776909393, 1664365780, 1245955022,
            4139315012, 3332423935, 3584383456, 959550365,  2851581095, 2765081174, 312015845,  2925122154, 831150802,
            2225638909, 2164098389, 1191447415, 1117010628, 135561721,  4276963837, 845458518,  2851510357, 969679932,
            1796734039, 2511313129, 2522970427, 448789704,  4073504765, 659076456,  30688278,   678042678,  988391597,
            2657818481, 3122219921, 2252457867, 381833280,  39664823,   798312169,  1342169064, 685136713,  343784474,
            1999891332, 2648989534, 3121328072, 2094704341, 875054259,  1539951585, 3426006595, 2948462920, 4241999564,
            480237018,  76339523,   993214506,  212508648,  1761289874, 1458847695, 2815784859, 3828413800, 2112473099,
            1970671077, 3585191439, 3453953161, 1715148319, 782577083,  3559195779, 666863548,  3269449590, 1738497402,
            3327440058, 2235043279, 763967509,  2723962306, 1300681724, 1644634995, 1020883713, 3134902007, 167254450,
            3835378367, 2890825816, 1882600156, 3842475942, 1580439250, 3304510024, 3564220150, 585616996,  1124865185,
            4039703682, 3346376390, 2926107577, 1428383569, 1403186104, 3499989938, 1755509293, 1722103947, 331432201,
            2568758684, 1646728996, 4243446314, 1526164470, 3471547995, 2444662401, 1294209014, 2516383774, 2221663104,
            3846549522, 1159773243, 3094344486, 2618544842, 3939291411, 2606083838, 1226639076, 851733441,  2230701030,
            2250669365, 3466438617, 1644286935, 2339513487, 1809184493, 2145665846, 759905197,  1355986255, 1263473014,
            95463257,   2059590240, 2375277619, 3763920231, 1647395486, 2208600025, 1743033782, 1070898946, 559884107,
            2246218814, 4107598716, 1163859100, 1747538253, 600091417,  555425760,  1717835270, 3807124353, 2597656066,
            365296128,  113127124,  2236845020, 1660665852, 1952113073, 1994841776, 3920008566, 261384886,  1775714106,
            3369232672, 579093206,  2485710549, 3177362285, 3814071502, 3332073094, 2727071987, 4079756356, 74907460,
            182436247,  457222487,  2657753313, 543162418,  779804662,  2792813646, 2080884976, 2097955208, 2900604543,
            103301967,  3930415991, 308718593,  1552419335, 749043430,  2411193439, 1098424808, 2570118989, 2526694181,
            3400415626, 826639960,  3742302823, 2133662427, 620256904,  2591646669, 2532798009, 505294613,  318811638,
            1036220366, 1873158787, 3081676522, 1897558064, 4139471411, 1934597655, 184383759,  3377149482, 1315956985,
            3503258655, 2995273328, 3760695601, 811451116,  826293446,  2369821295, 4075827518, 2893132959, 2626882418,
            2631502734, 343778694,  3078185303, 4109408861, 2364496718, 2398698968, 833238139,  2426477338, 2544790438,
            3418780531, 1729437941, 1426848541, 703366009,  1593001628, 3832885374, 121887589,  3714704746, 1887598275,
            2635541559, 645673967,  2633766892, 2138371399, 3489491357, 860025592,  2858956168, 3707495752, 988417647,
            3143123665, 403011270,  361217919,  1292362439, 3072108637, 3266764040, 3135891208, 4255030170, 1819294900,
            3774725854, 704946905,  3914456762, 2548645109, 2835934260, 1917802734, 72414571,   3545740638, 3036114677,
            328334211,  2279999972, 1684556682, 4115980801, 4158737999, 2349331837, 1184362158, 1377163853, 1447406213,
            251946304,  2054699319, 4162300527, 3154732537, 800830595,  2345623351, 4186958457, 2597505670, 1622655415,
            4094370742, 3495169030, 3050889288, 2811297024, 2100924914, 414823337,  1155175373, 588370150,  3539521602,
            1785268161, 1238354232, 1999419243, 1956013442, 2727088474, 2772169915, 1034439403, 3016448032, 644479533,
            20455836,   3152088005, 141608454,  3167589853, 108693017,  165857307,  1677180682, 1912847266, 163072558,
            3296918496, 1238554706, 2913139339, 3250494328, 2656026782, 1814001636, 983005525,  3581042682, 1273295181,
            559488001,  1513853211, 2674226464, 1496174110, 2676038553, 2913291535, 1553836470, 397263220,  2255512214,
            4154190644, 595917305,  261247893,  2624933658, 2121607555, 1122170555, 2052329115, 2719924412, 2564565254,
            1268524644, 3877295800, 3658790481, 2358266462, 1007723483, 1447765616, 3423692597, 265739651,  901379688,
            3385164778, 1406671308, 131685138,  3713756304, 3654640221, 2736543153, 2472622330, 296454307,  852438478,
            7174775,    1578146007, 2337741740, 3285588755, 4121241537, 2506803918, 1111804407, 2993267143, 3847761998,
            4003822560, 3451790762, 3677812854, 4098046969, 3705675793, 1671683662, 3986647063, 3787473835, 1085779680,
            63990554,   3052414141, 4084321971, 1680156006, 3808683942, 2589092699, 2363032620, 2314644674, 2410842488,
            3482027906, 2194963213, 2213653516, 1545508186, 1257471342, 1898594301, 56389295,   4031070285, 1283603427,
            3223663229, 1352935921, 2738160650, 2000719221, 2676319895, 3353571606, 2387345702, 3369949082, 4206766813,
            1997790391, 3247095723, 1895752266, 3138635348, 2024797579, 727824971,  3740942574, 3691088316, 3523830306,
            1083845175, 3611575859, 96129162,   1426405506, 3576582890, 1324911621, 139050613,  1381515152, 1350361631,
            3260093458, 4009561124, 2902641247, 3421888551, 2157764063, 1677483974, 3548504518, 3149100719, 950890023,
            1000765812, 1798951241, 234444169,  1840745749, 3227267033, 601588165,  4270830694, 1265160837, 2245119744,
            1832861096, 2441788779, 291223082,  1354813178, 3108244631, 220388565,  4210917215, 4010444702, 2886283367,
            82311693,   4239286067, 3456441667, 1629365021, 2963465439, 1481069014, 2097168120, 2243459591, 2045960661,
            173328679,  3482434896, 4284957309, 484819561,  1740171710, 2555325273, 2661539433, 2396132170, 36487093,
            3114330075, 2734356016, 3976205591, 2505500864, 2126002765, 3266217676, 1302423724, 1562426791, 3510406517,
            348852671,  1734927396, 300810933,  1937707904, 613817860,  2938633657, 1159075264, 2784013920, 2444220279,
            727962070,  3989787881, 943588331,  368304548,  1020941872, 1481150501, 2674628368, 3420429396, 1684097634,
            959541097,  2941757332, 3236364940, 1880542357, 2764924294, 3213027040, 2726761407, 877837400,  832388117,
            1170376228, 2599645276, 3592924439, 2127941493, 2180317916, 3401584289, 1283238252, 2111779714, 1312217909,
            880488523,  3747191761, 3711419005, 1565267457, 245583610,  2584971161, 3274236273, 2816685199, 2298120534,
            1736377983, 3433000851, 3570653415, 1300835749, 3494473256, 1405825069, 782120632,  3333110654, 3015387134,
            4240837245, 298089611,  3437707558, 4195547439, 2116756321, 2127250135, 2657834510, 146019804,  871958202,
            3619635538, 1886354488, 2191743571, 1673640959, 2801677351, 1855160805, 2825509812, 1076198034, 1092011304,
            2923986417, 1440398202, 1366248429, 999842069,  2853432030, 887296075,  745344340,  3915444423, 137214069,
            2109947701, 593011704,  3709740023, 72054388,   594605309,  3120971686, 1754070837, 469577850,  1123863468,
            422939802,  2499811560, 2041182159, 3663197764, 699866274,  2992511944, 2023300152, 2818667039, 2554586998,
            16195633,   1464495199, 1878983070, 3221705578, 3894377975, 2206566824, 3621602204, 4112584541, 1654655134,
            2046462885, 1199949442, 4028644735, 1342750738, 715825979,  2473867245, 2481829200, 1427831000, 3897677309,
            4273940493, 304354183,  3863817587, 4269371494, 1564245555, 2690956328, 288229216,  2908021585, 3808602115,
            837771623,  2087448898, 240864044,  4075189389, 1540749110, 601012683,  1956916011, 7208633,    292882766,
            739164803,  1310072295, 3748433258, 900592441,  2286857773, 258207382,  2077585386, 791503051,  504679761,
            366569024,  1987480819, 677624392,  2756754647, 1824926503, 1397874406, 2300475673, 1607388792, 621827211,
            685564454,  3229937113, 2189260300, 1536150535, 2703653614, 1454082497, 383977876,  3206916677, 3326425559,
            3234811301, 1391974329, 3418166122, 2128451694, 1067074505, 4143057672, 3276489313, 2067297467, 402789558,
            685511549,  574592745,  3436352270, 56094572,   3027611471, 2499065862, 3669703867, 1412618951, 2879910062,
            1711740277, 2589693067, 2451752439, 1639484565, 1006926125, 2330433969, 3269248718, 593755649,  493557873,
            2040814187, 4042466278, 703016478,  3817283497, 1327040299, 2339650788, 1088223135, 2890879770, 894916164,
            3525926793, 687925230,  4071461475, 2809678788, 3326724253, 2497398932, 2944824582, 619786,     2693209883,
            1104276458, 1969261178, 3799324683, 3082249155, 3380797409, 3309498417, 3775231428, 677009500,  2430127824,
            3663062776, 2346270068, 3211924049, 1075408881, 3144851064, 3402727872, 988367174,  1033547960, 3541629365,
            3398709809, 132394811,  3972795362, 549601471,  3690251852, 1264257864, 1015058312, 31128330,   2654982371,
            2307969937, 3805637799, 3191398359, 2330889832, 875573722,  2815460038, 478427945,  1605726371, 1993862125,
            3558529989, 842135129,  3201338141, 4027924584, 1355673831, 1758058840, 1681506452, 4233951981, 510811326,
            3307971425, 424881303,  732090852,  2369116896, 1265883795, 928486672,  3808814209, 538866645,  1636235088,
            2781732247, 2155725650, 77162846,   594984709,  174940515,  1835663652, 3361010167, 952506899,  1212290429,
            4255703460, 1527478375, 3610266436, 191984657,  2547818163, 1591637854, 2813508431, 1938555296, 2574817744,
            4894775,    2728378906, 3094485006, 3136258558, 3330331604, 561510431,  16764632,   3662304336, 867446159,
            2435391430, 1298662597, 1741538909, 2542408617, 831388915,  4056519045, 4230962771, 3119545068, 3094838557,
            2921185774, 2889755884, 2765070650, 3885024396, 3820970037, 3033503395, 1507983318, 671096937,  4163926604,
            2563597560, 2964706945, 798522642,  3285315751, 3987869649, 2034843484, 3264133905, 2715105430, 4087933875,
            4000927090, 3267758277, 483124591,  2550465362, 561317022,  3922611377, 2261254576, 596368429,  1048233448,
            1046712137, 558454545,  2595715704, 3013690214, 1787016949, 2189250401, 2316157454, 1818265720, 2185421123,
            4165658347, 1907713649, 1893710286, 4099664463, 1276819316, 425702805,  445203476,  2348051421, 1527034417,
            2536840666, 2457414079, 3212465580, 3844283979, 149172290,  3840127048, 2066035182, 3306976031, 275322554,
            4205168228, 2104640461, 2406877783, 2430950173, 926834869,  2804653836, 3063999404, 2447664420, 929775433,
            2245760161, 1420868878, 99771882,   867297838,  231947609,  3067947096, 412334033,  1294146064, 3712546117,
            2589442708, 1762494069, 725370604,  3470500765, 569187887,  1813554485, 3500757007, 2339274330, 441804163,
            1112526664, 1967412984, 2095036263, 2812277198, 2082442107, 1025233315, 1347449706, 3830746182, 3336428929,
            1866116627, 2271859044, 3625797601, 2512906061, 2016994923, 1622547312, 3593915411, 1742821464, 2704347638,
            3811537740, 927850246,  1151650537, 2840337814, 1969336484, 4102200979, 1631086872, 1638625162, 2316727680,
            4235247593, 917482552,  636357622,  3068660418, 4056094456, 3834156503, 922027306,  4182579749, 2274988128,
            3499595696, 4289786147, 2231886640, 2517163615, 234826320,  3037868198, 4027255229, 331541751,  3632090866,
            2286914942, 3797088527, 2560327205, 3280688017, 3259387116, 1235033225, 4147077682, 1593860297, 2243793157,
            2157850550, 1897571121, 2859168606, 4219683906, 2666409010, 2531913135, 3842064031, 2975299127, 2990270975,
            305620767,  782872114,  1873339555, 4232276158, 419317474,  2285813026, 3850705968, 2337077076, 3322746111,
            2996510097, 1190051226, 1155768460, 2734815708, 2501355088, 124287211,  9906142,    2128580241, 3592547922,
            2519942625, 1063655900, 1649654785, 2244418032, 3260516,    3268324395, 297708159,  1874512676, 486287750,
            3564149704, 774430984,  3739444880, 732001975,  2445402284, 44869036,   1494438124, 1765962633, 719773026,
            869740160,  109440719,  3468536612, 4037880420, 2289055777, 2983957841, 311658285,  3670474165, 921767218,
            1794587058, 627661050,  2640562493, 999822045,  2283399773, 3959369811, 3099450161, 1622663970, 2194124866,
            836397807,  1433566075, 1268352364, 77669776,   828388331,  2728941389, 2243554679, 1167640573, 3141548770,
            2672572925, 914513872,  2271688138, 3873562732, 1893212049, 3344967591, 3983047802, 1758604789, 1303308479,
            2245928210, 1964407979, 2162974362, 4129173562, 1964997693, 480018193,  2867706367, 2085006716, 2004126013,
            4184736742, 4047008801, 1661037463, 3689702840, 3692462075, 3570495403, 1364865014, 2233331184, 408350207,
            3913046713, 611728663,  1034456977, 4167979042, 2582216639, 1543272331, 2243036145, 993620955,  3292434817,
            3705703669, 4204017684, 1197931279, 1537885298, 598587484,  1496557380, 1407378688, 299297600,  810870084,
            3310078786, 3404328565, 2207377669, 2173812170, 3707305176, 2712573869, 3193314832, 3843972985, 4252198192,
            1484224291, 402231860,  395437141,  3950444976, 3792984519, 2485969210, 1261817135, 510506820,  4081667294,
            2576563015, 10291870,   1046774861, 1912347418, 680112214,  413007049,  2846628588, 3071404904, 2794759569,
            2198339192, 3592853039, 3486234033, 3576305809, 728252578,  3902444094, 3304912747, 2141705279, 3455300755,
            4181535485, 1958004100, 648885967,  3167202263, 3865398392, 3204727215, 3556781006, 3998007739, 3637337910,
            2258548376, 610222066,  4137380157, 3485899723, 1481440721, 3809293452, 2331809817, 55941516,   696960978,
            4155520153, 1513505295, 1303646374, 3348860949, 2169539231, 4233004251, 2650791723, 184420991,  2181840404,
            616602012,  3493535598, 2821415559, 2017490208, 3988940925, 771484360,  1930210967, 3096419012, 332214671,
            631623991,  509541241,  3530826069, 170460777,  3332561039, 331027469,  4163183980, 950885024,  2450675408,
            4209613469, 4168930708, 255471420,  3341915739, 3242872109, 3952466929, 2322949398, 3738826136, 2023292500,
            341901055,  1617070570, 3365510440, 2318847851, 273057587,  2433762968, 162431070,  636937558,  3054507389,
            2161972447, 2248138027, 3969671081, 864771593,  1650436443, 602518505,  923483622,  2407318064, 2079913399,
            3813197025, 2737361148, 3618309336, 1062375977, 4216115599, 385810045,  3640007380, 2395638560, 1640053682,
            669939115,  1122818174, 2107985506, 3426817490, 840389093,  829620248,  1887763809, 1521081185, 3642058843,
            1973844100, 12661133,   2162593761, 3891609704, 1784494060, 2044257407, 3329745268, 2903593166, 2970080880,
            3249141492, 3503717325, 4116310565, 2391911792, 3681281723, 3445537681, 1315925110, 3124580215, 3133346295,
            3192495004, 2051667604, 2300354402, 2658048799, 4163663921, 2878138572, 3677619900, 3717510482, 293400592,
            2343381949, 285263469,  430409086,  793442475,  1259436382, 2932738694, 1853719227, 3080593626, 1604887400,
            2760813932, 1333186387, 112535360,  2480079744, 3056839474, 4088398262, 102978420,  2214938050, 1102864894,
            3840953340, 1518922283, 2478245435, 2255237690, 2557202857, 754791355,  2489061397, 1180224149, 2887214313,
            1300933882, 3428131516, 854252016,  2462003431, 2847774286, 1557242088, 2795302849, 3343497084, 3158916462,
            2861567530, 3483133831, 4159573894, 3729071224, 4244304879, 1302007918, 1442957325, 2677160330, 3091005783,
            2866806232, 473287500,  1276042544, 2344323501, 2287275222, 1644192473, 3124433688, 2127392883, 2688550961,
            3791582800, 472767457,  1504801370, 924906298,  2585327461, 2387702605, 1244201168, 2025773180, 3558722261,
            1747077076, 2235290026, 3334906670, 2212094521, 2104290540, 425565095,  3063698898, 2984604904, 3526191125,
            1273745866, 4130897585, 664565494,  1101389402, 1867294962, 584401257,  2308622611, 84275,      3886843768,
            2952936625, 2182803273, 945840420,  564456040,  1231430250, 1116367146, 3977401809, 3856714515, 3438513166,
            1962845594, 1462408276, 4038556971, 2717262142, 2071338205, 2378801035, 2471009862, 453380923,  90608020,
            256190225,  1772374718, 3904593901, 1085689797, 3449842973, 951219530,  1836802864, 2520977420, 3787372871,
            1092477428, 1258530611, 1354179102, 2661601572, 3850874358, 1770282275, 1590227301, 322935988,  140904195,
            2406912248, 2558481307, 1355306572, 1520189205, 2564789704, 3954274834, 2912192681, 706547152,  794514058,
            3125366095, 319626862,  3514855443, 712083526,  872135422,  822924910,  1989432487, 810528555,  2010240875,
            1771372105, 3853827648, 97206440,   238686950,  2637770821, 3349966249, 954870885,  2150207567, 2505394687,
            1351977645, 86453836,   3981251342, 3812701846, 497142721,  1320366792, 2329111234, 712386168,  266511744,
            486503178,  4022135565, 406620050,  853384444,  2039851954, 109110430,  3458557213, 85195081,   3047334318,
            22683743,   2043724899, 2199939527, 2264042797, 1718499784, 2805416797, 757419777,  915604228,  1576803591,
            3852258097, 3639831508, 2670143421, 213154520,  3043602459, 647838611,  1905437958, 4072757889, 2787390148,
            2039854819, 1939729526, 3803121642, 1094674591, 2036748830, 3463807372, 1281241527, 1015866462, 3281782143,
            2113298487, 3127998376, 1433483243, 3628963213, 471288138,  2690609594, 164778934,  4030450999, 865446964,
            1328914535, 1198367947, 1798966897, 613291465,  981756857,  917374869,  488654345,  336380642,  1590901230,
            2530500574, 1335206373, 1896228377, 1804652122, 2338563765, 4196093694, 1983300845, 2465539904, 2230106746,
            1927901962, 3154062705, 2019911523, 297187162,  4133401023, 587417529,  2182479442, 2783086550, 4077929643,
            2408484000, 4118084402, 1663858033, 2153330285, 1893288978, 2227570337, 2792183071, 2478571924, 971816454,
            1417895397, 3822012116, 9635357,    4237484688, 225829342,  2182755538, 1860897701, 3602842388, 1703972432,
            2551822690, 3581820021, 198865239,  1509023965, 1824806788, 1944049937, 2929016700, 1613971390, 3746296790,
            4074709837, 763209171,  3979998442, 3540469807, 2459871768, 3275144041, 2371146189, 4121830358, 388143676,
            548004372,  2118694623, 2908599826, 1294302384, 3985451175, 2123567685, 2933179033, 3259804451, 3250590638,
            2701132546, 2989597316, 772400258,  1999829893, 2563787019, 2883221287, 3666061079, 3755700741, 894587584,
            2621054240, 1267104256, 4215384733, 2206234066, 3316466326, 3492273235, 4278029635, 3351867221, 1894616027,
            171884857,  1054127276, 2829690294, 1263400165, 2266676616, 3007581899, 2348850132, 2787624864, 2086021499,
            1692430249, 181239663,  3508161627, 3214934339, 3378351470, 96996117,   2582528612, 1297906160, 1047551903,
            2271168141, 1253922727, 4094829083, 2565553194, 160987488,  3580751373, 3864612055, 3872645531, 801537067,
            1681400757, 2786944744, 486903470,  3910046398, 4243547101, 786865425,  3745925990, 1262793521, 1434567391,
            3018602424, 3447405592, 1139784776, 188728849,  108704018,  2455363979, 3345223585, 2808532172, 879162620,
            54488993,   1690104845, 382951565,  1175914392, 1963926337, 153618720,  607958406,  1257710478, 3743942146,
            1772749467, 3009645812, 3229481093, 538807495,  2303716588, 3003260226, 14968305,   3590636424, 2167055731,
            2215291096, 1877312016, 876083217,  3396799051, 3280043064, 1582432415, 1596908623, 3318284124, 2138145009,
            2755018628, 788499089,  3052718417, 896264690,  1178427313, 3253920579, 862780343,  1677747092, 329979156,
            2320067741, 274638595,  530233724,  1081447425, 3785467375, 3355218487, 2315820968, 446079568,  3195316438,
            1550806948, 2360613944, 2062562998, 3818832455, 2567415199, 3197903852, 582305102,  518937984,  885775847,
            2173785541, 821730276,  2531963536, 2777589429, 2002149639, 57485221,   1904470877, 4152865147, 4237501809,
            2961981999, 248308655,  769807700,  2455568897, 3550752081, 2183411410, 3722010400, 4280053225, 1230709424,
            991513717,  4230841729, 3023533516, 386479491,  3575779634, 1554861350, 1627686123, 3232693029, 779458098,
            859077623,  2007247233, 3257970051, 2778512329, 4218577511, 3115771622, 4022412966, 3975139820, 2603136324,
            363568094,  3336979128, 3620765897, 1119391878, 3109917729, 2489742672, 471269191,  2320178771, 1964761822,
            3092412188, 1519071250, 162153331,  2842166372, 2873239448, 2644995328, 120613276,  3600654605, 2701049012,
            3160886418, 4064728480, 2818119120, 455612595,  2201436309, 2433273364, 2511515835, 2635382963, 4288496041,
            1537509013, 3060174282, 3336717897, 1125128551, 582760872,  225958707,  3239484867, 1890747215, 3138471502,
            892335098,  3216849692, 2566414941, 2410238677, 1333297221, 679219118,  32388963,   86740378,   882776516,
            2355943987, 2701355733, 1963621166, 1750723739, 1536625853, 1768049582, 90473686,   2480131953, 2759897394,
            2355182133, 3915991656, 1218848133, 265360925,  1323655301, 3987764797, 382004015,  3767740561, 2223785162,
            38405833,   2567493113, 1236409859, 3619143925, 1726964061, 1287783995, 2188453419, 4120310899, 1668016245,
            2689772731, 3962734209, 2643081845, 2623816715, 3608422692, 3554559450, 1199196125, 3920552703, 190263297,
            1255130923, 1190461783, 2404346477, 3461474856, 4102273686, 372943695,  2100321096, 2791914046, 3397828282,
            1004348740, 1277133932, 2535134566, 365291795,  1410872343, 826323707,  669074846,  228169371,  2109343264,
            3827612293, 1326788467, 1982272226, 446801148,  2343046098, 3936458810, 3451374065, 618855410,  2511251550,
            4228297871, 327584544,  1345196551, 1035293340, 1545828440, 2380087551, 1559766270, 4183128152, 3640266581,
            1767178697, 682782465,  285872671,  73850056,   436729226,  4034530898, 542460320,  970655855,  1263508118,
            2473209791, 2283657618, 1458173226, 2383878598, 3549213353, 3605161837, 948906137,  3702409148, 1219566104,
            2011481394, 508990153,  3159647304, 4266780367, 3798671085, 1640956189, 1884552782, 2082129227, 3653841896,
            2842792179, 2828127897, 3958669432, 1999328972, 1187323813, 1982343423, 1559326821, 3877594181, 2447429794,
            528223656,  2757983372, 1180114911, 2576423775, 925248716,  4283503804, 454060210,  2607868415, 1962926223,
            904160983,  736195764,  1901535476, 294842472,  1140451806, 3610997383, 423324553,  13124655,   974575945,
            3650695070, 3504429568, 1445146673, 1166848672, 3337124229, 1337486968, 1498330336, 1235564972, 255630232,
            578580783,  2085701895, 2204716003, 669956187,  3895494015, 1069614906, 4195511594, 1819785212, 1916385243,
            457917788,  2700002244, 4182459709, 44917490,   3092745965, 481633008,  2568779016, 4233987575, 630329192,
            2729385379, 4098124963, 596271756,  1811048447, 4233302074, 3763608447, 1547886565, 1792962870, 3637304311,
            3276015905, 2219359868, 490267475,  1992441268, 558160550,  3505528718, 1634797488, 4289002112, 1395116372,
            64252250,   564128064,  1466723201, 2892870718, 3867268365, 4280105392, 1963729832, 2573502169, 2788735888,
            3493201618, 578831832,  1240512260, 1047750976, 2647293219, 3278318293, 599344328,  2601769059, 3525949051,
            750705977,  212188614,  3405024571, 1649740821, 2731036117, 559999954,  3366014280, 1491770839, 817110706,
            789077142,  3816405680, 2280247302, 1685988942, 3541058355, 2030544217, 125298212,  1879506747, 3076014922,
            785383148,  4121787253, 1345265641, 926055274,  4134008307, 465980587,  2246582348, 1777642358, 793132026,
            2879272740, 2002386485, 3985743419, 20316293,   2176974293, 797663027,  1912893000, 1918598806, 246363593,
            1239889569, 2666510487, 1974639765, 1472581234, 415382420,  2565151597, 2533428808, 4256721471, 839811574,
            2794785401, 865016369,  1187777670, 1008770132, 2022265616, 4173115343, 4119493255, 3763808640, 358943876,
            2534110338, 2568212660, 918445798,  4191632840, 1144061024, 2231465328, 4098068927, 3458786493, 978691361,
            1172546067, 2445695153, 2730495615, 1368757315, 3475492095, 3583312259, 4005770539, 12828775,   1930751857,
            2335311254, 2602572936, 1125478724, 2444276415, 1449082553, 186294652,  1468358108, 3751864840, 1017051329,
            3859339995, 1122373606, 3252628246, 4253119433, 3468552953, 2264249734, 3324330478, 217523305,  2141562400,
            190690312,  3297300930, 1990897601, 4012332576, 1010093014, 3301795466, 3026848363, 1628241543, 4231034072,
            955692066,  3088663484, 3606431013, 4193930784, 2938632616, 1428690959, 158356224,  1221499475, 1431831161,
            2280610781, 3014483484, 3590701401, 3333595715, 1277981041, 3901583697, 1750782528, 899872406,  2900611894,
            709559225,  596450003,  278435865,  2317114538, 4009418400, 252297572,  3725278947, 685729789,  1680303280,
            2223803634, 3417525563, 3973174203, 1497767567, 3448192379, 850298872,  3297260476, 2029688344, 4208553672,
            3035191335, 2889430952, 146986036,  47572917,   1776434631, 1775887747, 466903880,  1751890805, 2228766302,
            2524702532, 3923882564, 3152925007, 314158386,  2543502412, 2141760185, 1036119263, 1096271477, 4103039493,
            840551259,  2442818131, 751636408,  298752933,  756907572,  3588846905, 2486258379, 2963771126, 2805079146,
            1072040561, 3034070738, 4173611989, 853813568,  1918058162, 137991992,  2780698588, 699902509,  3511227208,
            2930981849, 2996516700, 381922492,  2495326781, 2230556256, 805054665,  3057150347, 391162146,  1126998684,
            3015150075, 3068816966, 2743494142, 1608522816, 2788143833, 2668930190, 383445850,  4055024205, 2933282584,
            2598909892, 1213272559, 1134166671, 3231660898, 964312685,  4136302428, 3098637657, 3447941058, 701008012,
            4291030853, 1342676155, 1512656657, 1107020938, 2725951126, 2936801309, 3224487475, 2608468550, 3318842199,
            1277116915, 1136839671, 3663994061, 3135931272, 3534679592, 3793388026, 2228545154, 4282098378, 979545695,
            790528343,  1975236923, 3190831975, 2175946267, 510078003,  303818157,  970836976,  1348936391, 2099187245,
            2331249485, 2720503938, 403373506,  3207422347, 519492240,  1533426860, 3551034406, 1528102582, 4042292938,
            592279471,  3005831738, 3831365163, 81039638,   1363707751, 886088776,  1302391689, 2539447399, 2566603996,
            1602805672, 3804350895, 3498967202, 675526007,  3065961771, 10883222,   881074677,  1589405713, 3129199592,
            2573939138, 3156782289, 4091279399, 1872368768, 3453629671, 360328772,  1949846085, 1459257771, 2702390247,
            3202519788, 1590883880, 1564269014, 3526997253, 1202751835, 2900376167, 2077050306, 4258130845, 1462970763,
            1130723611, 3950952299, 3212394086, 616693094,  2551861628, 3822264992, 1065479064, 2021472212, 3802576300,
            3913956185, 3059903272, 133076079,  861283410,  1038095439, 1614841818, 1513339226, 383922908,  3307187827,
            1218112993, 868745561,  517479669,  145795279,  1240591914, 2790304532, 1166251792, 966049101,  2592025400,
            2094385320, 1572651369, 99163486,   2946235134, 2784007713, 213511930,  3666171719, 3713735824, 570870769,
            2231830776, 1554341218, 2985036371, 2120289432, 171445355,  3422574720, 1506092472, 1595092727, 846675486,
            3276446091, 2719705818, 4119247827, 511210442,  2416191972, 2960932180, 1406812737, 473709555,  3437484482,
            1201379765, 3375106560, 1011203594, 3843033248, 2090942959, 4274492859, 3858339017, 305401239,  141075641,
            3085549008, 1609507042, 3189704239, 2024844568, 51810256,   238191140,  4045931642, 1835233742, 4255331426,
            1768730271, 2343168385, 1569435606, 1304984456, 4053972178, 3042510379, 1715183030, 522056580,  531899892,
            2565971810, 3729285704, 679236958,  3155324862, 2866484325, 3008763488, 3697141421, 1002610389, 4185620311,
            3571933138, 2299513431, 153326618,  502643612,  3735603128, 3263586666, 4144396962, 2651337427, 4100454671,
            1102352789, 3868000056, 1233878305, 3467462226, 3587417001, 1673186395, 3167731748, 3237808426, 3625038718,
            3899399541, 116838796,  366982583,  2856877715, 3954053415, 1808150713, 2186966904, 4135046162, 513717067,
            3523704565, 560513266,  3836534521, 4230182188, 739328318,  3532675401, 3197816123, 3431120498, 3285235631,
            2906129710, 3309473158, 268990707,  3153944340, 420298442,  2655040028, 769190542,  3199046121, 2552862574,
            3503402846, 2902722266, 2884348832, 1576048913, 1848754436, 1583492956, 4113039924, 4168616119, 2764495094,
            618802219,  3134990836, 4246174469, 1084127029, 751348650,  3907168293, 3219829699, 4190765371, 551642126,
            3626522073, 4162988083, 324399665,  380967854,  2585093615, 2180906141, 3625283657, 3800137903, 13991119,
            2155472622, 2155774093, 622030628,  1442736754, 1970806076, 759204141,  209491449,  3964065558, 481826519,
            256453204,  3554347505, 1672127205, 3016026484, 2555221351, 1373994483, 2912362776, 3687679280, 1466390488,
            915127910,  3748219908, 1443695518, 2281153771, 4060149943, 675478236,  32867818,   248988549,  2892642308,
            2914989918, 3758841386, 1312485932, 2167184500, 222801702,  3744270248, 2557965905, 3015103559, 3939879317,
            3842290107, 1491458225, 3429070238, 163791770,  2656636673, 3355171342, 3334322745, 581864061,  437061973,
            733582722,  473333896,  1860425017, 1245771639, 477210324,  2734868759, 828222023,  2173721835, 2895365084,
            1832065255, 161124779,  2188798022, 3878120334, 3388801939, 3712250608, 1458897119, 3111329667, 1351718747,
            2088010262, 4216531273, 1286954664, 2694868853, 1441728197, 1857210379, 1057640025, 3676175703, 902660331,
            1154923192, 3265137822, 3258977308, 3810914534, 2053726249, 234557777,  4103891484, 4083908223, 1102808939,
            1536919246, 3298594360, 102979572,  1732671929, 3920164193, 157877592,  1851765860, 2936750113, 4255634317,
            1547606444, 3250545051, 3632110887, 3501009098, 1247513365, 3259673774, 3322945570, 127219974,  3687802865,
            3153418589, 2945847000, 502594131,  1173398569, 3390392495, 1287139684, 3143475768, 3986612820, 3501834251,
            2357122157, 2882229916, 675040996,  75992247,   191304654,  3169757490, 2627309128, 1037489034, 831694113,
            2289157368, 814588232,  412944234,  1523548382, 1280062488, 912279418,  4134524876, 3561331126, 2952836061,
            391973763,  2171947345, 2803058962, 4219581157, 749403166,  1765714010, 3635570095, 2189556291, 2833029847,
            50691004,   4031401715, 988495855,  1975867859, 1285990152, 1906787395, 1657058162, 2623303110, 4460855,
            1873551768, 1134531092, 649425076,  3186119327, 2357554526, 2480878696, 1986318134, 2334777297, 2516941077,
            3110702662, 775637121,  691059235,  3293158751, 916611588,  2102470845, 440543381,  2122043945, 3299872629,
            2447197350, 345838490,  952056422,  3737380636, 3836073069, 2480051526, 2696617729, 1487564431, 1524357662,
            1066989902, 1854070813, 3606933446, 2504505722, 1027052576, 886713004,  3966384777, 629819544,  2502533208,
            4091395384, 183499480,  2935580005, 4071185399, 2414288862, 4106219877, 1474663394, 547416515,  2156574821,
            911188763,  818435665,  1754421967, 684500251,  566140820,  2313681362, 3894954480, 3082332455, 101569154,
            4267416930, 1411295468, 2034826130, 2135834550, 2568626914, 2747668541, 3787752642, 1977117047, 4237851309,
            2089227413, 3654418612, 1432403748, 2422179291, 1777509197, 3983645652, 1543588653, 2933360645, 3050055960,
            3074960094, 1871857415, 2409286493, 2302756883, 4236229597, 1879697196, 4024884993, 1485075691, 590309283,
            799266734,  2152390489, 2788575505, 3098150958, 3863364429, 2286350365, 1849538934, 1429560474, 3150859729,
            2517189156, 721952961,  2427010059, 4018105464, 239341325,  1090111765, 2847974996, 3963402044, 2078502455,
            819767095,  3002777361, 26626817,   1878225427, 3477971299, 556700810,  482097639,  3168172513, 2971104837,
            1731824364, 1674077823, 1052685035, 14531894,   1621372517, 780631565,  4004835464, 1577668955, 1844496573,
            4248441359, 1120215554, 123325953,  752885209,  1088005680, 2639739158, 213252013,  120914473,  4168629462,
            1685359931, 1494535849, 319703818,  4262270752, 235531706,  203791795,  4291818073, 2986617739, 3180977612,
            3472448722, 2665256610, 1413942620, 3578346406, 71630198,   3382270940, 1572032557, 4052306710, 979137549,
            879471598,  2242835198, 3345416532, 1295638158, 3217276473, 1628940286, 3846385367, 725462666,  1815147120,
            1741721172, 1748039107, 3455200354, 2587737613, 332473181,  2109589367, 1311457412, 4185653248, 2370700739,
            3480580359, 489778638,  3304822794, 341196562,  1243096869, 3175307630, 1237270306, 4029884052, 683694387,
            1913742411, 1587403926, 350837880,  1057382743, 843469358,  1232005660, 2445929269, 3085140141, 2411643855,
            1778610160, 1365845422, 2324466381, 2935597392, 632507145,  2537420371, 2912868891, 223052706,  4095548223,
            2824493650, 3444734348, 3733394750, 1260169143, 861540270,  835465259,  3623741940, 533822173,  3351246303,
            2790966064, 4161566037, 946363984,  307395308,  1913408002, 1113614807, 1360601213, 1390202548, 663998681,
            2597539516, 637481144,  2820556439, 824186294,  365295388,  944249314,  903722905,  155662175,  672440150,
            1408896573, 3282797134, 610144857,  2515642080, 3442055065, 1516695922, 1173225149, 3884181656, 1363569049,
            3963408236, 3627064968, 1570421278, 2739402460, 78723962,   3367629662, 199426279,  3468347165, 3608509525,
            3931279583, 2564640181, 868691099,  2581622019, 4260522798, 3465898762, 4073416647, 3369295138, 4179476664,
            3805127324, 1527338084, 3325976845, 2906707248, 222431837,  2569466070, 1630714380, 246809401,  559496431,
            3668144193, 1615013225, 2862980402, 303506994,  3989332257, 1943313095, 3995932718, 535819414,  1707271089,
            3307416185, 2991471979, 4039957684, 1242618052, 2827581004, 666854473,  1371078741, 986025959,  3267739872,
            2355522589, 1422744897, 698095966,  3585360514, 2165665102, 2480925110, 174364340,  3462141199, 3486995817,
            1314709415, 3792762750, 1323979760, 4147237398, 3552881382, 1750358689, 25370654,   1300310546, 2090406636,
            2373606279, 604882297,  867083955,  3252657144, 545765469,  2400383282, 29046100,   3218049764, 1349806938,
            1760488404, 1564324224, 2554973906, 214584060,  2423582663, 1842936399, 77666091,   3329399743, 3526188639,
            625214673,  1021014022, 1098762163, 1353121452, 4146719507, 1660337852, 743367965,  660842171,  1296213304,
            1905952326, 3549231138, 3108395231, 958532497,  1848239991, 134048954,  865953606,  3969874470, 3812383447,
            3076548046, 704863629,  4155111087, 2104460204, 1643412691, 3199423424, 3632075197, 97220511,   108174655,
            2041700163, 3449831099, 405827140,  508442379,  907483369,  2908229295, 1159974063, 4189530753, 2161194025,
            1860914423, 70000089,   1239041264, 1030379370, 313412718,  210876745,  2626252419, 4194607912, 3154233274,
            4043050737, 3570338371, 1708032128, 772032136,  1836030671, 2309101058, 1755397995, 51366841,   1268433825,
            2991840199, 1373481493, 1635091092, 373555370,  3251606190, 2734152746, 2298107042, 3992922784, 3660874270,
            4003491746, 1920760497, 1515733131, 3832202858, 2771494284, 1964991548, 1954015038, 1133935323, 3102778016,
            2146840316, 517169168,  366184200,  4182978135, 2874132761, 1279303432, 820840792,  3081439343, 3241059860,
            1857904754, 3989441966, 3201435148, 3834105558, 3080811910, 3728986147, 2787079417, 4016533584, 4178500882,
            1346507040, 1402054626, 2316227427, 182294507,  373479035,  510963871,  3838176295, 3663912645, 2410375408,
            985383593,  344663494,  2542468391, 4157483076, 2196854651, 1678335452, 2492668872, 3632700824, 3611970198,
            3536068576, 3074701611, 1309379187, 3125281735, 2307442588, 141971921,  782304183,  9198433,    2682436849,
            2066173795, 1121667771, 1555718902, 3167952954, 3005191405, 1198941669, 3026198399, 638154217,  735931579,
            1839836455, 3235012406, 1783484065, 2682160941, 2174459606, 2721925162, 3976508920, 1236057956, 2735597091,
            4202155251, 914403230,  2130435844, 3662297748, 1475606979, 2229835738, 572187928,  4132958648, 1611004731,
            103336474,  4219191972, 4164312923, 3477281808, 3474776027, 1762881398, 3047414394, 2594447346, 3954159490,
            2201555488, 204519158,  2197814461, 4209714777, 1423738749, 2155027566, 3800701116, 3947957813, 2199375013,
            2117323319, 2754767976, 4003841199, 2589169318, 1586857301, 3829950643, 4195405500, 4178439012, 1361118254,
            1904599346, 1221226815, 1151043767, 2913493722, 4004783878, 1878307525, 833330113,  3091321948, 2739566980,
            1538811657, 313699025,  1399009251, 1559178441, 233479994,  410794644,  3944560474, 2293255814, 182824700,
            150789889,  2834717484, 2569868516, 4085868982, 2173540628, 456991586,  2532307490, 2490868026, 2470765246,
            909922886,  347142902,  471165293,  2408404749, 3214975674, 3134763699, 3137834596, 2047159181, 4256592764,
            1570273985, 442265476,  3869653389, 669702029,  3245742278, 2061397325, 650389574,  1710659386, 381325535,
            3453106113, 3842356333, 697219187,  3870268902, 1051328618, 556056573,  3209970923, 2592924337, 1069292461,
            1918262392, 1089122962, 2687888536, 26699763,   1538322861, 1180939959, 904804948,  99817636,   2162936140,
            3835727405, 921228004,  1179480255, 1897384771, 28853291,   3907566660, 1613430952, 3564468299, 1239686613,
            555206796,  3864734731, 3095950002, 3269088743, 2720336323, 463361991,  29284184,   3952600670, 3974663820,
            374869536,  1015147528, 1406932428, 2224101048, 12377689,   3588421789, 3030990595, 3845851903, 3569509358,
            895427011,  948557133,  4004116031, 3016306835, 1523243432, 1904281385, 1447154004, 1025242677, 829420000,
            3543398557, 245487194,  2418984213, 286167460,  1179631479, 1019309523, 1872820896, 3971354338, 4232365541,
            2234444392, 2205742978, 788991714,  1122175341, 3172175672, 3805847238, 1906398206, 2855845213, 3695843824,
            751003984,  2739321248, 4170817630, 2336811657, 110654228,  2545690857, 1028124884, 1339728437, 825811919,
            1744809042, 643844835,  3898054974, 4151488510, 1042865492, 1101588267, 95409211,   3388807012, 3125560049,
            3308139574, 2455294649, 1547217965, 3116952791, 289062940,  1292725861, 1725294274, 2379592986, 2000110994,
            3178714620, 3069336247, 2814512441, 765093337,  1683107896, 1662183978, 156191559,  803333180,  1899592838,
            2710592116, 76464499,   1170070831, 247477850,  4177285700, 2102278347, 4144698579, 18764388,   1052693064,
            3496752313, 822368361,  1904821842, 2838476975, 2824064200, 1084816425, 830380874,  1866693701, 1873949933,
            2583534950, 2560271922, 2507469546, 563787708,  72691960,   1495083733, 3292478931, 820973390,  3693515822,
            1707076656, 3307589900, 2224673541, 2111332812, 558854092,  3977807931, 539808801,  2097634312, 3565313108,
            984386449,  2931005615, 2489185652, 2670806346, 2235750751, 856039251,  1005123064, 902372999,  2315083733,
            4136346539, 1823777950, 4220524814, 1479096588, 916090384,  628953069,  3988728168, 4199113414, 185575240,
            742926345,  915667656,  2378561861, 576820738,  946379302,  3562376208, 554467224,  4225495744, 3699478632,
            2443619664, 94492669,   671172724,  3768491707, 489248152,  1697053139, 796027853,  2627737559, 426752031,
            1806703463, 263399799,  3274762101, 2459606407, 1673150840, 2138850413, 2997873959, 2549166475, 3501813433,
            2184445602, 703264628,  1111649901, 2355935399, 472225890,  3795203109, 1935701663, 3774174634, 2833148088,
            2027427909, 1584580407, 783336210,  2899677050, 1508019500, 848016577,  1665090784, 3988755883, 226753437,
            1471254433, 2747438806, 2747332136, 4251975303, 2575209180, 2370190390, 3388192008, 1001231473, 4202599144,
            1480430639, 264872714,  1150753378, 1322072688, 475903256,  1193609671, 132392616,  2419080542, 1426144190,
            2747435788, 565619832,  269915754,  8706171,    1898187956, 3210527298, 2567374804, 4032850033, 2347111131,
            2748231652, 2427401461, 2972811799, 1958314572, 723166844,  3667082753, 3815656736, 1287744578, 3643279308,
            1294134314, 3090942973, 2080392831, 329447228,  297806099,  4169236422, 2337155963, 1497028196, 1325143127,
            4165813614, 1968500238, 1927806880, 210757072,  717102128,  1537122167, 1048190985, 3437846367, 3615099630,
            987835813,  3824400898, 3951469754, 2138794295, 1241942560, 1862190505, 1607506835, 4194930865, 1944971572,
            227842456,  3874104869, 4105398614, 3789646543, 2307331029, 3803002369, 1343682944, 3947542833, 2337383738,
            3955406112, 1795727209, 1420181440, 3323986304, 714896199,  1260617688, 4035502077, 3956081047, 165332608,
            546292116,  1212577785, 789396619,  2246586883, 3735135748, 37822461,   632103394,  691054096,  2779413872,
            1933936799, 673303282,  1555234006, 3716678513, 1466066508, 83880536,   983979803,  709926844,  1644725395,
            474558041,  2422098504, 637798440,  276597319,  4109798494, 1368782648, 4041420432, 4122000853, 1652948047,
            3372226837, 4188387627, 3778617490, 3616270038, 1943862259, 270885474,  2816617219, 1819908164, 3372195426,
            3248493243, 2460608344, 3869168830, 1207505847, 1362554737, 2165134835, 1574593464, 2104643850, 3680754435,
            3228084551, 3164442407, 987024393,  3424514911, 3020012732, 2430705126, 1883675530, 2715844414, 2642283517,
            2393408386, 4193834095, 216412677,  2792759343, 2917248192, 1052637819, 553016942,  2364449207, 3243239014,
            1093830618, 1285914493, 2065440490, 548585357,  4234311586, 2977173276, 3278425495, 3473708417, 2382428110,
            3719935213, 1029554512, 3770166441, 3032398230, 3643891877, 998101004,  3587125266, 3592706471, 859022194,
            3851893326, 885827631,  2618082491, 3465230360, 504915824,  2442274005, 1887576682, 1792297614, 2934158316,
            1590899469, 4105950922, 3616784885, 1912256688, 766167404,  1173863702, 1905836114, 2155490294, 3971720342,
            1162893424, 3409535264, 4165678373, 1165281733, 724255899,  760021454,  1787710369, 3817734228, 203268801,
            3720432653, 1819223929, 1839162912, 2037779055, 2310566195, 579808785,  3502416582, 2028921469, 1441598434,
            1526501080, 878588729,  599767789,  1010882067, 3649532718, 1221397081, 1462940534, 3619626269, 729529813,
            3374668806, 422562465,  2211147397, 2963370575, 3221247034, 4015126212, 297381234,  4183564372, 939740318,
            2611830517, 1096574932, 541011959,  2188242879, 1783580367, 447574367,  2003934559, 2279533547, 2792460002,
            2255812646, 3413675186, 580898548,  2229999740, 1096105952, 4244423343, 2935245960, 2254021892, 4163296007,
            559478003,  3942857209, 1119963254, 2544484347, 4036705698, 3589160096, 1630423599, 2170477615, 3591187211,
            1655689764, 1128785650, 533564648,  995584376,  2386010587, 646223679,  637975734,  3125658035, 4086835803,
            881734974,  3965044509, 503517182,  1780731367, 2324255197, 264762062,  3051851940, 1822570831, 2092856474,
            3930344837, 2910733392, 690151343,  3083134941, 3525927867, 2216036,    2588605841, 2009111659, 3272096242,
            1605450256, 2919537262, 4230888630, 76818604,   3744783779, 2801690509, 1238992268, 2878398840, 3746882099,
            3985998978, 3280970869, 3491179263, 380348730,  1657678717, 2934831977, 3071989145, 1133814947, 3483343350,
            3087033783, 1146696824, 3954064327, 3779313612, 682331113,  3762102367, 1105315248, 744541178,  3180771490,
            4198705492, 1120248603, 3850137932, 2338577224, 1819253145, 649440247,  654987080,  3470072835, 712134453,
            3298281108, 1139063469, 23887181,   1569735793, 2333692031, 3611594763, 129452663,  2094749885, 883065966,
            3954917082, 578200586,  3500782968, 1934656801, 2883345064, 1931669212, 3147324590, 1464802785, 2965853552,
            790501402,  258009254,  2503160616, 2649046567, 3904964681, 1599077734, 3919935570, 1303529763, 3991975809,
            3189086331, 1057043550, 3395103804, 2877354301, 2306930273, 3982880750, 2924855815, 2487820297, 2652911164,
            3999703772, 2223059040, 1740258037, 803351223,  3981488998, 1064648509, 1360021823, 2337841535, 2146948154,
            2440954468, 3130762432, 1813182439, 3074850533, 4283503593, 4270484299, 949490550,  2266372770, 2295246887,
            1861688194, 4040174575, 1449464748, 1366853117, 2068858018, 254544809,  3877674561, 3224793088, 3267498751,
            2107034611, 563498979,  3449171610, 1195453762, 273177086,  289471273,  616927831,  2074092853, 3899999301,
            286740001,  3420933845, 3592214327, 884189525,  222835649,  2225635747, 2188257362, 3190419420, 4025559130,
            1478386940, 1558547116, 3231059985, 3442723480, 1257988497, 1230169668, 4204920613, 1680720428, 872302704,
            3072235055, 690014443,  2522917836, 514713378,  3021775723, 1321394624, 3884661892, 926882735,  3080333504,
            326751408,  3303976139, 4235779168, 2429481693, 1217466369, 3791218516, 2222358465, 1623611345, 2787041527,
            4078544859, 1365988909, 3306328170, 3344521067, 4143717872, 2986806940, 761212906,  4154456262, 2789197670,
            1624539898, 57869540,   1207526886, 2578669846, 1958234150, 1777451133, 2129169500, 335914812,  4078757930,
            3345953932, 150543374,  735284016,  3961834956, 845377072,  2144594832, 1614985729, 1054820689, 2991989080,
            3174679142, 2236995668, 62294231,   2873837462, 3142910821, 1199872045, 2601892540, 552738648,  1021074598,
            2337595165, 3159932723, 1421373831, 2852138360, 3106767941, 1157694337, 2750128317, 1772017733, 154079587,
            141054736,  262862859,  232757713,  221025285,  706028672,  1875436250, 956312777,  2201277682, 3590829191,
            1848993676, 4083454028, 3566742977, 3503063328, 1864869341, 3423684506, 1393864104, 2416690788, 2766585002,
            2130688952, 3844258502, 3337815280, 3091937789, 349123157,  3807721080, 2444073514, 2451066824, 3998237698,
            1140469168, 2919152242, 2217355879, 3278465837, 1742719589, 2157539246, 2811674180, 388712428,  1047685661,
            3397021293, 2384054399, 4233122871, 343468428,  1928701525, 3579480071, 905080500,  3365249726, 884089982,
            926035997,  2250824413, 3674743698, 1103287857, 2440105975, 616634619,  1923570126, 212857251,  3699939407,
            1494175986, 1774922124, 2114576677, 3035124497, 3346065309, 3287177393, 604624215,  2867504980, 634469686,
            873242186,  951438535,  4137621355, 2570510799, 2802795193, 2843455931, 767579718,  1890503101, 823769604,
            2735068143, 2707525515, 3990479894, 2406919069, 3525918157, 2886294828, 1219864314, 678623495,  2898087551,
            3411564190, 3940664382, 652501256,  714531640,  1473865718, 1431021514, 4148189552, 2245159337, 1076941098,
            816707811,  1337624501, 297487765,  38600646,   864558622,  2247687860, 4194578366, 1986070170, 2421609970,
            1965441963, 145830434,  3241118120, 3157751963, 2400447902, 384736236,  995447386,  1066230612, 1590936359,
            2619286511, 2670409058, 1369006695, 4127910546, 3550325184, 3593082767, 3865524409, 2864690582, 2625701841,
            2036541147, 3233311212, 4261833975, 2422410440, 2832148170, 1556052639, 215989597,  2416711777, 3463070183,
            4177850045, 128323673,  2207385607, 4129145942, 2591595665, 1127555689, 3309249433, 233605789,  4195716420,
            1493914608, 450292440,  3106847448, 3325965005, 3474065831, 633209133,  2256155961, 537355112,  562349960,
            2920130469, 4257823976, 3194115618, 938036945,  2709711086, 688217007,  2854268270, 570525277,  1074448737,
            3022790230, 1408299668, 4264456841, 3634044527, 2475248905, 3407882491, 463826284,  401413402,  2633851237,
            2154372447, 2369213365, 3606409779, 555916408,  554764341,  413102912,  268173915,  1243956163, 2236116935,
            4256678080, 583906087,  1045370605, 68579465,   3068148389, 1372194493, 3737570643, 1069484043, 4035970320,
            3137858625, 2632533173, 1131259704, 1956314450, 2139036495, 1693635173, 1347101027, 297670136,  443676587,
            45078526,   9644821,    13024437,   3947779661, 2636099762, 2988059012, 4293595183, 1837930809, 20717789,
            2436064584, 2641256380, 1788079499, 487367235,  3952405086, 4293333151, 2001126465, 325450887,  4274005052,
            3283754653, 3171965309, 3733966652, 2677265288, 1415645422, 2865801289, 4243528708, 517320369,  2848973825,
            4246368793, 4008131504, 3912315722, 75263429,   3901564263, 3163709089, 1915955163, 814314600,  2851014790,
            535576064,  2115465923, 791863217,  1513138018, 1817851661, 1430189011, 4026283369, 1273775110, 51363969,
            1690706294, 955659596,  1256032279, 4155022186, 3556701285, 2094838304, 2555108598, 3900795263, 2316156865,
            2481324589, 2127484379, 3069769195, 3081364624, 4051550903, 3349894137, 4028203007, 2706441518, 1657726911,
            2050548647, 3948506061, 514733067,  1245547062, 1253380285, 2573637166, 4106465896, 412749972,  2553016818,
            2377954964, 205524185,  1285259001, 3177583366, 1500657131, 2171117540, 3112088774, 4046115004, 3358847120,
            3880379733, 1026439335, 2964156688, 1402633360, 832806998,  2662483319, 4181324334, 1074028406, 2451941658,
            3177225652, 3621760189, 1874548982, 810317789,  2967519315, 1208802636, 1791699236, 3571146477, 3675119985,
            1535945126, 4206799040, 1352166344, 1842547102, 4293941614, 3876325997, 2961520588, 4291024947, 2377690140,
            3029882842, 3385028522, 1769584996, 707012955,  1874347097, 1177298139, 567127784,  2932672318, 1359658876,
            747935864,  1237299557, 2776872490, 1614516459, 3790384341, 105380764,  60403570,   2028064064, 3360197661,
            3778818867, 2554961043, 1815178949, 3990928553, 830885401,  211270252,  3525350720, 3323467237, 3917205905,
            1019215603, 4044912638, 531260692,  2490226642, 1538942872, 3984885162, 2095909331, 3397624047, 3141642419,
            2359511874, 2416257464, 2152558510, 3304435559, 409172760,  1227269822, 2477906248, 1504972756, 1896065843,
            4239329455, 2588059069, 4242041383, 1893351626, 191065276,  491303559,  550803364,  2336149361, 205575547,
            2277235038, 3316266813, 3413440406, 592682965,  1167521509, 603267836,  3677750629, 1997620720, 662924749,
            2975910429, 399326526,  3566325804, 3744648502, 2769380438, 99343152,   3784246318, 513448785,  2393415065,
            422495707,  3451996124, 3755752758, 4238139976, 1256137919, 1929038182, 416741414,  1033632768, 4098162224,
            812283983,  2775687609, 473061527,  884032780,  1645375283, 2039912638, 3912362121, 2647404275, 1522690301,
            2445104256, 3224841505, 806162779,  2057840740, 3837885470, 378821814,  1838465422, 1718657914, 4119552540,
            1297458390, 95059192,   3054019901, 2286395664, 4275269216, 3438586873, 42672948,   3604182870, 3049842594,
            1051093617, 3399787737, 4078751625, 1889709174, 1197658687, 2837439980, 4085378267, 2733607213, 2451764203,
            1688909613, 4069575565, 326625990,  4253280853, 966786581,  421604840,  327254563,  1528071832, 10462300,
            3818080004, 2940970643, 3013330465, 2660097098, 2463359144, 1171642467, 1276891436, 1724827619, 2703108575,
            894837839,  3269310186, 475382424,  3662889370, 3594355086, 1516528440, 2451013773, 997304248,  1968670999,
            1829941902, 1059902154, 17571430,   576860691,  3670314058, 3602361151, 2098948197, 883466310,  4216409196,
            2452232035, 3264668432, 403473066,  2481805251, 3194270322, 2298754401, 2194644105, 3518078865, 873240945,
            1134305003, 2718823138, 1935072880, 2982454869, 2259840652, 1962997313, 571550175,  3923923650, 4071690436,
            2567657999, 3524990489, 2587473858, 1226545156, 3625555288, 3556467719, 2110084229, 3461707755, 2418321085,
            374546082,  3957523870, 224948585,  3030724537, 1587458094, 1553112615, 1495144888, 2084754285, 2082453883,
            637245269,  3696621892, 1688129650, 730279191,  3465268303, 1255405508, 1040615754, 810568624,  1668799184,
            3442525021, 4140082195, 3028386449, 1523702039, 3605655316, 3570577573, 1507075394, 29511801,   3939152850,
            3144976359, 1435969637, 3397347544, 4221009679, 2401616085, 3596889904, 3609628987, 2458799004, 4161626088,
            3403711934, 1545958552, 166167265,  3499076534, 3755799521, 1869789413, 2799182131, 1475941476, 4183337562,
            1553699990, 551886344,  1396622316, 3796954922, 2471897525, 2588921881, 491219037,  1936711795, 1800777674,
            549515442,  2780471991, 1761940382, 1107960962, 3258460594, 4101167737, 762584396,  307636269,  3918528963,
            3414785149, 2104232568, 162750571,  2954961311, 1923932531, 1284681664, 3352410700, 1439431743, 2629669571,
            2238836884, 4210177312, 3494174832, 3211653097, 912552573,  2474336253, 3573978208, 1057364555, 614776391,
            1324746145, 249066267,  3837686961, 2193966233, 340924547,  1102806576, 2983844361, 3529781279, 3367834808,
            3382950113, 1309667299, 1090323269, 775712531,  2668006310, 2725521046, 329586582,  2414768443, 1172259265,
            2070444509, 3748531265, 3624429180, 3822756442, 2182937819, 626914561,  683265097,  183781819,  3115014037,
            1206457943, 261184948,  172603504,  963603907,  116297718,  2855743306, 1783971075, 2293162088, 3030524182,
            574274926,  2033701704, 55325736,   1488183704, 581219210,  3449783338, 1279704141, 3006648174, 2991302601,
            2132112976, 3234112694, 1817904286, 1936121269, 2727406315, 1183301155, 2903602018, 659536622,  2944731992,
            2409868691, 3373266127, 495103010,  1210740340, 2540241558, 2886449017, 3382933128, 775211966,  419822592,
            2270163620, 1402243599, 2527473510, 3749362357, 1218685709, 3870335114, 1673723750, 355117338,  1126900312,
            3015437466, 992101659,  1472130679, 957991740,  4239133855, 3887118331, 2261380374, 2028837654, 3141065475,
            4196104099, 453642977,  69491917,   1330057718, 4179152562, 4107857789, 2125396131, 3765994908, 3681378168,
            2598111108, 810493802,  2997165166, 3440220100, 2635968886, 3326491203, 2532338921, 2746426942, 2925062700,
            3179436451, 3087566931, 3381995974, 3645951228, 3081265279, 3082214755, 3570053390, 3119031418, 2034695101,
            192374580,  912198837,  2109352261, 190830525,  1115967532, 237726305,  4162738665, 1428719284, 2233358430,
            1155575897, 2972610565, 3172053941, 2542146091, 1455731394, 3175915255, 2482299510, 3945335119, 2414101673,
            310920545,  1312020502, 2335555692, 1250477183, 2678735145, 637556140,  410285385,  1245080894, 329664169,
            2363986305, 777907294,  1264914378, 2591577697, 1139226117, 1376439008, 3663618218, 3735999941, 148014699,
            1937786323, 2003204549, 3463503815, 4216261183, 2183424674, 4108770158, 1207294525, 4188228135, 2090207882,
            1679615060, 4001281264, 1127387594, 3403896653, 2016082475, 840037585,  820318947,  1325022434, 2681082158,
            4163024045, 2753175409, 3141650736, 2111643474, 2622216667, 1605419853, 3036795146, 2076521666, 3607996787,
            1860969824, 2951256410, 324322002,  3005572564, 2217489733, 841376491,  76563819,   2306858493, 1762304873,
            644859256,  2724316491, 968041963,  354106038,  3443116013, 3785347795, 3290263341, 1804296963, 4082496485,
            1730333610, 535336840,  344131239,  2921172007, 3212976443, 2178910760, 2089728973, 1551074445, 2595889152,
            660413765,  2707363048, 3560363679, 367334559,  1650352065, 181757844,  2757957077, 2465727027, 3526882374,
            3051342907, 3974097517, 3799215321, 1727326675, 3606827693, 1251858021, 3644022554, 1324264289, 3523694595,
            542893628,  1130017099, 2213365908, 1856876106, 2736148460, 1437757754, 1699569611, 1908229781, 175145590,
            88560034,   403821454,  2169375972, 1953512580, 1067810864, 2498758412, 3059308848, 1084385356, 3609111146,
            154074856,  2670076756, 3832860747, 3488195097, 3793615554, 3154233107, 829148915,  972017054,  3532896099,
            1394043326, 1745342052, 3723773043, 2432449259, 1684610844, 2334315870, 2015323155, 119355700,  1946172831,
            902171516,  2666973977, 3280764141, 3863911963, 976100962,  3954221042, 4125369742, 301614008,  873457727,
            3054287469, 1474295881, 439505646,  628351466,  3044761275, 4178959217, 3576883954, 2225559639, 3119799456,
            4293932528, 294598327,  2636014891, 3151984741, 844906457,  756951325,  1565061935, 2759135208, 2914750231,
            335946686,  4053591911, 1306644076, 2867088497, 1925621689, 636330657,  901456329,  2633220805, 1433254352,
            2629148402, 4055932742, 702519613,  2731770932, 537755899,  2041341758, 3636947088, 4265420206, 719067811,
            1450597050, 1499255908, 698102457,  1867572406, 1247140116, 3796524001, 2241287708, 1597407192, 1676723555,
            259493846,  684438679,  4099756632, 1842834018, 3941133149, 1350204237, 2889075141, 1969131420, 1771438289,
            1151231640, 2209445833, 3722576031, 3803176863, 953522795,  3347742768, 1061887211, 3708311915, 3593990004,
            1167977765, 828280664,  119038264,  302183797,  2981101993, 3970121510, 1231804061, 1707740906, 1274273740,
            112377943,  3829779962, 2911412488, 1625730724, 2427113564, 1339177580, 740640082,  109465063,  2804376926,
            224774876,  14495342,   3746166469, 3810785443, 4216866731, 3227295475, 843133023,  3860962891, 1282157019,
            834330310,  967916332,  2608679422, 3338821092, 2232583570, 2979089228, 3191234183, 2439044458, 58880587,
            2140149311, 76254482,   1384425063, 3368409820, 4197084109, 2189549695, 1876658776, 370709836,  4167789283,
            1122198992, 3022480384, 862501966,  3109149517, 1288484100, 675690118,  4102231260, 2263994084, 3206960835,
            1416626598, 3126235316, 1092247778, 2941811129, 3591527974, 3900742964, 3162371586, 2481483851, 1383268331,
            3610895658, 2644555154, 2121256345, 4205854983, 2313559087, 3714845459, 1162578592, 1152343910, 21974893,
            1877613671, 3503641460, 1157125657, 3897792353, 1682257288, 1278312892, 2811216685, 2238711941, 2756442767,
            1746694256, 2380851297, 455009711,  1559876155, 2714979643, 189725460,  2333833303, 2007575997, 1886996924,
            97739722,   2082309058, 1836652900, 1333223437, 1565591222, 1292115745, 1314839493, 4138268668, 3610391999,
            3396588894, 1819104170, 1069832449, 4019882623, 1129202117, 1018322743, 2032102993, 2228926898, 1362593766,
            988655300,  3625813269, 3797092697, 3541989067, 1136087432, 600961005,  3515063614, 2288318519, 1730805276,
            584192093,  2332905361, 3135492088, 2665569047, 1402323662, 3648584422, 4242681354, 868040468,  3687090251,
            3891712654, 3741674500, 2414400190, 589221778,  2295398758, 1656931974, 2416678277, 1811181700, 1935067279,
            2287397835, 4019946301, 1045675391, 4061487697, 396390504,  981089981,  634459867,  4009720711, 910487098,
            1281771789, 737021213,  1350564545, 108626627,  3556994601, 2839574827, 127855259,  14716402,   673997195,
            3149898583, 997124220,  3429385172, 537469499,  4283331929, 3568681014, 3005077655, 3625610952, 3831820879,
            2396703831, 2351693186, 1718045063, 22515266,   1051945508, 934319527,  1161318636, 1031818079, 1724854168,
            2038307849, 3813735308, 446520486,  3906272911, 498217935,  129005616,  3940886270, 3280188771, 2565017665,
            2397473022, 1919399688, 3596433229, 3659588424, 192215451,  4114040320, 3639266447, 3730864011, 2338916453,
            1477650690, 4091610415, 2785071595, 857115497,  2983851304, 3115229758, 376806224,  2553158862, 1146002735,
            2625462473, 4189485248, 237281249,  3598197469, 1027181259, 3784411924, 67203160,   2783469513, 2371642634,
            1237235694, 2789060461, 3614521009, 2552957984, 3496002289, 942112758,  663313450,  3668195471, 2422328162,
            1690231058, 1241952550, 4235757754, 3280708084, 3608811512, 1364624224, 3910843804, 1278838429, 381458912,
            2434384984, 781524834,  4278579487, 802000605,  1241015146, 2292443375, 1324961156, 2548652549, 3467158728,
            1271091130, 3202425550, 2191091804, 2179605897, 4099013039, 2127824441, 319480106,  3114849331, 3147437286,
            3253695609, 2950724575, 2914122484, 3120761616, 2351082421, 177721330,  2197810508, 709140833,  1989253851,
            2493634740, 336779249,  1413220589, 949473005,  1494392105, 2740263730, 2501784403, 4220806161, 3933294228,
            1541933957, 3566262720, 434009917,  2279934609, 912550215,  245951768,  218302647,  325459907,  2364228626,
            1320321147, 1975318233, 4082548789, 3773543894, 101889398,  1356659502, 4081585714, 3033930576, 1932335743,
            4177215091, 2350570326, 1264349611, 1940963484, 2744982985, 2058902147, 2286043998, 1532155570, 2232714445,
            623971666,  3500845538, 4001461152, 3102804289, 3747985323, 2632278338, 2623572725, 1099181105, 2129095122,
            1396958609, 930365652,  2727081080, 1981259419, 2537562552, 498517317,  3482014832, 1185740029, 3121480370,
            1765478484, 3648549704, 1724933006, 3335178704, 3115568496, 961874263,  3970118036, 4122031655, 302743333,
            1778266952, 125030821,  2519078260, 3268083827, 3693609103, 43434602,   2113380600, 433971644,  3103058389,
            2542373815, 1434919268, 3057243539, 3358332123, 1193637761, 193029813,  2702563964, 339772968,  2738925197,
            2784638290, 1899385136, 603938617,  1345134811, 486991224,  2874485578, 4204219025, 1597542264, 1074248689,
            984509844,  4167941609, 2381848404, 313653044,  384032373,  3077881229, 3844827855, 1122069313, 2242265217,
            3635392782, 1344943788, 1825639354, 2208066605, 3663468475, 692796401,  2331186689, 2016490183, 1088810631,
            387839534,  703992647,  572282146,  3674470894, 1383360408, 2613416729, 508217280,  2531182030, 788333147,
            458477965,  3015354309, 2399569135, 2736687937, 271512345,  2710115586, 2918699463, 1575429833, 728169406,
            212804919,  2426496014, 926502308,  3006915768, 3763410744, 2424022772, 4183352559, 2746319890, 921881192,
            3152256318, 1973955117, 2095615106, 3708147068, 1818018474, 948982704,  194925950,  3304721749, 3755537143,
            4087860031, 2259098591, 558834084,  2180993286, 3030542193, 585714717,  432440337,  742260317,  2914692478,
            2354439062, 1388915005, 3279070310, 2786565964, 499639095,  2414383430, 1778892993, 742869280,  3197848109,
            3007221226, 1919446761, 139662818,  2352148533, 2635192414, 816985516,  2882526420, 1209118595, 2974286931,
            2509489955, 3728148816, 2067867943, 3093424632, 2158904659, 2015983463, 2954136097, 2619172392, 330943573,
            3499812037, 2568442168, 3310969227, 1033699176, 2080514925, 2130560355, 2972087518, 877649428,  237195294,
            1628077779, 268278634,  3996156904, 3317657743, 1740192238, 4170517573, 88223419,   3905481708, 3363698028,
            498519120,  3625904479, 293203497,  3670985034, 2054541632, 3298725626, 2480593659, 2223776398, 1634347204,
            2215844949, 2104841048, 3670995618, 3601255277, 3912366406, 1847441190, 4008054822, 1360293849, 1701300061,
            3867389801, 2215311706, 3052888786, 1980513524, 1702529541, 2493432185, 2255860359, 2076780893, 1667830663,
            4003910209, 2833684755, 1531188732, 1987865923, 3415289420, 61613019,   3051246731, 2568867322, 2598191743,
            394030802,  4091929449, 1887531246, 4187444081, 662091924,  392970282,  165453901,  2704274271, 1493869323,
            658786826,  785945344,  510147204,  2901693204, 3423997925, 1198026053, 2692693903, 2059283302, 1742382128,
            3424436781, 1390863016, 1044020012, 3994462897, 1961682213, 3190572805, 604285798,  3995695119, 1771261768,
            3245100441, 3681080538, 971437554,  3860685960, 279123094,  4238842596, 3405331537, 552167914,  1055008025,
            1950134434, 2961646271, 1955302803, 1722485679, 1758190963, 1729359382, 151732431,  2975671854, 247920624,
            3388548947, 1553852617, 3655966648, 3293475595, 4136986292, 4125196386, 1761868919, 2084292805, 3742433573,
            1850244386, 2206452580, 4156282532, 1346814547, 2820323253, 975032677,  46817533,   2438802842, 2717448888,
            816496145,  496096515,  1509137007, 3202912676, 3124438109, 294850928,  76707672,   3576972823, 1434515170,
            3797215694, 1139943743, 3191526556, 2676565129, 3057535224, 2781090099, 2408969255, 3940713661, 1925278427,
            2418772170, 844586498,  357917292,  1502145715, 3756405733, 2629458815, 2233165797, 624925874,  2292858577,
            3949009490, 709906505,  1246333736, 3863077887, 1796645313, 3268312861, 3633646221, 2692509080, 4149887921,
            4182468972, 3843101055, 311929642,  400872849,  813964776,  4184855164, 1566874656, 1360324679, 1721466596,
            1809473503, 2516710439, 2822530636, 3698913772, 146246892};
        counter += step;
        return uint32_constants[counter % (sizeof(uint32_constants) / sizeof(uint32_t))];
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
