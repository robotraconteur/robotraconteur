#ifdef ROBOTRACONTEUR_USE_STDAFX
#include "stdafx.h"
#endif

#include "ServiceTestClient2.h"
#include "test_vectors.h"

namespace RobotRaconteurTest
{

void ServiceTestClient2::RunFullTest(const std::string& url)
{
    Connect(url);

    r->get_o4();
    TestWirePeekPoke();
    AsyncTestWirePeekPoke();

    TestEnums();
    TestPods();
    TestMemories();

    TestGenerators();

    TestNamedArrays();
    TestNamedArrayMemories();

    TestComplex();
    TestComplexMemories();

    TestNoLock();

    TestBools();
    TestBoolMemories();

    TestExceptionParams();

    Disconnect();
}

void ServiceTestClient2::Connect(const std::string& url)
{
    r = rr_cast<testroot3>(RobotRaconteurNode::s()->ConnectService(url));
}

void ServiceTestClient2::Disconnect() { RobotRaconteurNode::s()->DisconnectService(r); }

void ServiceTestClient2::TestWirePeekPoke()
{

    // Test Peek

    TimeSpec ts;
    int32_t v;
    EXPECT_NO_THROW(v = r->get_peekwire()->PeekInValue(ts));
    EXPECT_EQ(v, 56295674);

    // Test Poke

    EXPECT_NO_THROW(r->get_pokewire()->PokeOutValue(75738265));
    TimeSpec ts2;
    int32_t v2;
    EXPECT_NO_THROW(v2 = r->get_pokewire()->PeekOutValue(ts2));
    EXPECT_EQ(v2, 75738265);

    // Test WireUnicastReceiver

    RR_SHARED_PTR<WireConnection<int32_t> > w;
    ASSERT_NO_THROW(w = r->get_pokewire()->Connect());
    for (size_t i = 0; i < 3; i++)
    {
        EXPECT_NO_THROW(w->SetOutValue(8638356));
    }
    boost::this_thread::sleep(boost::posix_time::milliseconds(100));
    TimeSpec ts3;
    int32_t v3;
    EXPECT_NO_THROW(v3 = r->get_pokewire()->PeekOutValue(ts2));
    EXPECT_EQ(v3, 8638356);
}

void ServiceTestClient2::AsyncTestWirePeekPoke()
{
    EXPECT_NO_THROW(r->get_peekwire()->AsyncPeekInValue(
        boost::bind(&ServiceTestClient2::AsyncTestWirePeekPoke1, this, RR_BOOST_PLACEHOLDERS(_1),
                    RR_BOOST_PLACEHOLDERS(_2), RR_BOOST_PLACEHOLDERS(_3))));

    async_peekpoke_evt.WaitOne(5000);

    EXPECT_FALSE(async_peekpoke_err);
}

void ServiceTestClient2::AsyncTestWirePeekPoke1(const int32_t& value, const TimeSpec& ts,
                                                RR_SHARED_PTR<RobotRaconteurException> err)
{
    EXPECT_FALSE(err) << err->what();
    if (err)
    {
        async_peekpoke_err = err;
        async_peekpoke_evt.Set();
        return;
    }

    EXPECT_EQ(value, 56295674);
    if (value != 56295674)
    {
        async_peekpoke_err = RR_MAKE_SHARED<RobotRaconteurRemoteException>("std::runtime_error", "");
        async_peekpoke_evt.Set();
        return;
    }

    EXPECT_NO_THROW(r->get_pokewire()->AsyncPokeOutValue(
        75738261, boost::bind(&ServiceTestClient2::AsyncTestWirePeekPoke2, this, RR_BOOST_PLACEHOLDERS(_1))));
}

void ServiceTestClient2::AsyncTestWirePeekPoke2(RR_SHARED_PTR<RobotRaconteurException> err)
{
    EXPECT_FALSE(err) << err->what();
    if (err)
    {
        async_peekpoke_err = err;
        async_peekpoke_evt.Set();
        return;
    }

    EXPECT_NO_THROW(r->get_pokewire()->AsyncPeekOutValue(
        boost::bind(&ServiceTestClient2::AsyncTestWirePeekPoke3, this, RR_BOOST_PLACEHOLDERS(_1),
                    RR_BOOST_PLACEHOLDERS(_2), RR_BOOST_PLACEHOLDERS(_3))));
}

void ServiceTestClient2::AsyncTestWirePeekPoke3(const int32_t& value, const TimeSpec& ts,
                                                RR_SHARED_PTR<RobotRaconteurException> err)
{
    EXPECT_FALSE(err) << err->what();
    if (err)
    {
        async_peekpoke_err = err;
        async_peekpoke_evt.Set();
        return;
    }

    EXPECT_EQ(value, 75738261);
    if (value != 75738261)
    {
        async_peekpoke_err = RR_MAKE_SHARED<RobotRaconteurRemoteException>("std::runtime_error", "");
        async_peekpoke_evt.Set();
        return;
    }

    async_peekpoke_evt.Set();
}

void ServiceTestClient2::TestEnums()
{
    EXPECT_NO_THROW(
        EXPECT_EQ(r->get_testenum1_prop(), com::robotraconteur::testing::TestService3::testenum1::anothervalue));
    EXPECT_NO_THROW(r->set_testenum1_prop(com::robotraconteur::testing::TestService3::testenum1::hexval1));
}

void ServiceTestClient2::TestPods()
{
    SCOPED_TRACE("s0");
    com::robotraconteur::testing::TestService3::testpod1 s0;
    ServiceTest2_fill_testpod1(s0, 563921043);
    ServiceTest2_verify_testpod1(s0, 563921043);

    com::robotraconteur::testing::TestService3::testpod1 s1;
    EXPECT_NO_THROW(s1 = r->get_testpod1_prop());
    SCOPED_TRACE("s1");
    ServiceTest2_verify_testpod1(s1, 563921043);
    com::robotraconteur::testing::TestService3::testpod1 s2;
    ServiceTest2_fill_testpod1(s2, 85932659);
    EXPECT_NO_THROW(r->set_testpod1_prop(s2));
    com::robotraconteur::testing::TestService3::testpod1 s3;
    EXPECT_NO_THROW(s3 = r->testpod1_func2());
    SCOPED_TRACE("s3");
    ServiceTest2_verify_testpod1(s3, 95836295);
    com::robotraconteur::testing::TestService3::testpod1 s4;
    ServiceTest2_fill_testpod1(s4, 29546592);
    EXPECT_NO_THROW(r->testpod1_func1(s4));
    SCOPED_TRACE("s4");
    ServiceTest2_verify_teststruct3(r->get_teststruct3_prop(), 16483675);
    EXPECT_NO_THROW(r->set_teststruct3_prop(ServiceTest2_fill_teststruct3(858362)));
}

void ServiceTestClient2::TestMemories()
{
    test_m1();
    test_m2();
}

void ServiceTestClient2::test_m1()
{
    RR_INTRUSIVE_PTR<RRPodArray<com::robotraconteur::testing::TestService3::testpod2> > s =
        AllocateEmptyRRPodArray<com::robotraconteur::testing::TestService3::testpod2>(32);

    for (size_t i = 0; i < s->size(); i++)
    {
        ServiceTest2_fill_testpod2(s->at(i), 59174 + i);
    }

    EXPECT_NO_THROW(EXPECT_EQ(r->get_pod_m1()->Length(), 1024));
    EXPECT_NO_THROW(r->get_pod_m1()->Write(52, s, 3, 17));

    RR_INTRUSIVE_PTR<RRPodArray<com::robotraconteur::testing::TestService3::testpod2> > s2 =
        AllocateEmptyRRPodArray<com::robotraconteur::testing::TestService3::testpod2>(32);

    EXPECT_NO_THROW(r->get_pod_m1()->Read(53, s2, 2, 16));

    for (size_t i = 2; i < 16; i++)
    {
        SCOPED_TRACE("s2");
        ServiceTest2_verify_testpod2(s2->at(i), 59174 + i + 2);
    }
}

void ServiceTestClient2::test_m2()
{
    RR_INTRUSIVE_PTR<RRPodMultiDimArray<com::robotraconteur::testing::TestService3::testpod2> > s =
        AllocateEmptyRRPodMultiDimArray<com::robotraconteur::testing::TestService3::testpod2>();
    uint32_t s_dims[] = {3, 3};
    s->Dims = AttachRRArrayCopy<uint32_t>(s_dims, 2);
    s->PodArray = AllocateEmptyRRPodArray<com::robotraconteur::testing::TestService3::testpod2>(9);

    for (size_t i = 0; i < s->PodArray->size(); i++)
    {
        ServiceTest2_fill_testpod2(s->PodArray->at(i), 75721 + i);
    }

    std::vector<uint64_t> z;
    z.push_back(0);
    z.push_back(0);
    std::vector<uint64_t> c;
    c.push_back(3);
    c.push_back(3);
    EXPECT_NO_THROW(r->get_pod_m2()->Write(z, s, z, c));

    RR_INTRUSIVE_PTR<RRPodMultiDimArray<com::robotraconteur::testing::TestService3::testpod2> > s2 =
        AllocateEmptyRRPodMultiDimArray<com::robotraconteur::testing::TestService3::testpod2>();
    s2->Dims = AttachRRArrayCopy<uint32_t>(s_dims, 2);
    s2->PodArray = AllocateEmptyRRPodArray<com::robotraconteur::testing::TestService3::testpod2>(9);

    EXPECT_NO_THROW(r->get_pod_m2()->Read(z, s2, z, c));

    for (size_t i = 0; i < 9; i++)
    {
        SCOPED_TRACE("s2");
        ServiceTest2_verify_testpod2(s2->PodArray->at(i), 75721 + i);
    }
}

void ServiceTestClient2::TestGenerators()
{
    RR_SHARED_PTR<Generator<RR_INTRUSIVE_PTR<RRArray<uint8_t> >, RR_INTRUSIVE_PTR<RRArray<uint8_t> > > > gen;
    ASSERT_NO_THROW(gen = r->gen_func4());
    uint8_t a[] = {2, 3, 4};
    for (size_t i = 0; i < 6; i++)
    {
        EXPECT_NO_THROW(gen->Next(AttachRRArrayCopy(a, 3)));
    }

    RR_INTRUSIVE_PTR<RRArray<uint8_t> > b;
    ASSERT_NO_THROW(b = gen->Next(AttachRRArrayCopy(a, 3)));
    EXPECT_NO_THROW(gen->Abort());
    EXPECT_THROW(gen->Next(AttachRRArrayCopy(a, 3)), OperationAbortedException);

    std::vector<double> f1;
    EXPECT_NO_THROW(f1 = r->gen_func1()->NextAll());

    RR_SHARED_PTR<Generator<RR_INTRUSIVE_PTR<RRArray<uint8_t> >, RR_INTRUSIVE_PTR<RRArray<uint8_t> > > > gen2;
    ASSERT_NO_THROW(gen2 = r->gen_func4());

    ASSERT_NO_THROW(gen2->Next(AttachRRArrayCopy(a, 3)));
    EXPECT_NO_THROW(gen2->Close());

    EXPECT_THROW(gen2->Next(AttachRRArrayCopy(a, 3)), StopIterationException);

    RR_SHARED_PTR<Generator<RR_INTRUSIVE_PTR<RRArray<uint8_t> >, RR_INTRUSIVE_PTR<RRArray<uint8_t> > > > gen3;
    ASSERT_NO_THROW(gen3 = r->gen_func4());
    for (size_t i = 0; i < 6; i++)
    {
        RR_INTRUSIVE_PTR<RRArray<uint8_t> > gen3_ret;
        EXPECT_NO_THROW(EXPECT_TRUE(gen3->TryNext(AttachRRArrayCopy(a, 3), gen3_ret)));
    }

    RR_INTRUSIVE_PTR<RRArray<uint8_t> > b3;
    EXPECT_NO_THROW(ASSERT_TRUE(gen3->TryNext(AttachRRArrayCopy(a, 3), b3)));
    EXPECT_NO_THROW(gen3->Abort());
    EXPECT_THROW(gen3->TryNext(AttachRRArrayCopy(a, 3), b3), OperationAbortedException);

    RR_SHARED_PTR<Generator<RR_INTRUSIVE_PTR<RRArray<uint8_t> >, RR_INTRUSIVE_PTR<RRArray<uint8_t> > > > gen4;
    ASSERT_NO_THROW(gen4 = r->gen_func4());

    RR_INTRUSIVE_PTR<RRArray<uint8_t> > gen4_ret;
    EXPECT_NO_THROW(ASSERT_TRUE(gen4->TryNext(AttachRRArrayCopy(a, 3), gen4_ret)));
    EXPECT_NO_THROW(gen4->Close());

    EXPECT_NO_THROW(EXPECT_FALSE(gen4->TryNext(AttachRRArrayCopy(a, 3), gen4_ret)));
}

void ServiceTestClient2::TestNamedArrays()
{
    com::robotraconteur::testing::TestService3::transform a1;
    ServiceTest2_fill_transform(a1, 3956378);
    EXPECT_NO_THROW(r->set_testnamedarray1(a1.s.translation));

    com::robotraconteur::testing::TestService3::transform a1_1;
    a1_1.s.rotation = a1.s.rotation;
    EXPECT_NO_THROW(a1_1.s.translation = r->get_testnamedarray1());
    com::robotraconteur::testing::TestService3::transform a1_2;
    ServiceTest2_fill_transform(a1_2, 74637);
    a1_1.s.rotation = a1_2.s.rotation;
    SCOPED_TRACE("a1_1");
    ServiceTest2_verify_transform(a1_1, 74637);

    com::robotraconteur::testing::TestService3::transform a2;
    ServiceTest2_fill_transform(a2, 827635);
    EXPECT_NO_THROW(r->set_testnamedarray2(a2));

    com::robotraconteur::testing::TestService3::transform a2_1;
    EXPECT_NO_THROW(a2_1 = r->get_testnamedarray2());
    SCOPED_TRACE("a2_1");
    ServiceTest2_verify_transform(a2_1, 1294);

    EXPECT_NO_THROW(r->set_testnamedarray3(ServiceTest2_fill_transform_array(6, 19274)));
    SCOPED_TRACE("get_testnamedarray3");
    EXPECT_NO_THROW(ServiceTest2_verify_transform_array(r->get_testnamedarray3(), 8, 837512));

    EXPECT_NO_THROW(r->set_testnamedarray4(ServiceTest2_fill_transform_multidimarray(5, 2, 6385)));
    SCOPED_TRACE("get_testnamedarray4");
    EXPECT_NO_THROW(ServiceTest2_verify_transform_multidimarray(r->get_testnamedarray4(), 7, 2, 66134));

    EXPECT_NO_THROW(r->set_testnamedarray5(ServiceTest2_fill_transform_multidimarray(3, 2, 7732)));
    SCOPED_TRACE("get_testnamedarray5");
    EXPECT_NO_THROW(ServiceTest2_verify_transform_multidimarray(r->get_testnamedarray5(), 3, 2, 773142));
}

void ServiceTestClient2::TestNamedArrayMemories()
{
    test_namedarray_m1();
    test_namedarray_m2();
}

void ServiceTestClient2::test_namedarray_m1()
{
    RR_INTRUSIVE_PTR<RRNamedArray<com::robotraconteur::testing::TestService3::transform> > s =
        AllocateEmptyRRNamedArray<com::robotraconteur::testing::TestService3::transform>(32);

    for (size_t i = 0; i < s->size(); i++)
    {
        ServiceTest2_fill_transform((*s)[i], 79174 + i);
    }

    EXPECT_NO_THROW(EXPECT_EQ(r->get_namedarray_m1()->Length(), 512));
    EXPECT_NO_THROW(r->get_namedarray_m1()->Write(23, s, 3, 21));

    RR_INTRUSIVE_PTR<RRNamedArray<com::robotraconteur::testing::TestService3::transform> > s2 =
        AllocateEmptyRRNamedArray<com::robotraconteur::testing::TestService3::transform>(32);

    EXPECT_NO_THROW(r->get_namedarray_m1()->Read(24, s2, 2, 18));

    for (size_t i = 2; i < 18; i++)
    {
        SCOPED_TRACE("s2");
        ServiceTest2_verify_transform((*s2)[i], 79174 + i + 2);
    }
}

void ServiceTestClient2::test_namedarray_m2()
{
    std::vector<uint32_t> s_dims = boost::assign::list_of(3)(3);
    RR_INTRUSIVE_PTR<RRNamedMultiDimArray<com::robotraconteur::testing::TestService3::transform> > s =
        AllocateEmptyRRNamedMultiDimArray<com::robotraconteur::testing::TestService3::transform>(s_dims);

    for (size_t i = 0; i < s->NamedArray->size(); i++)
    {
        ServiceTest2_fill_transform((*s->NamedArray)[i], 15721 + i);
    }

    std::vector<uint64_t> z = boost::assign::list_of(0)(0);
    std::vector<uint64_t> c = boost::assign::list_of(3)(3);
    EXPECT_NO_THROW(r->get_namedarray_m2()->Write(z, s, z, c));

    RR_INTRUSIVE_PTR<RRNamedMultiDimArray<com::robotraconteur::testing::TestService3::transform> > s2 =
        AllocateEmptyRRNamedMultiDimArray<com::robotraconteur::testing::TestService3::transform>(s_dims);

    EXPECT_NO_THROW(r->get_namedarray_m2()->Read(z, s2, z, c));

    for (size_t i = 0; i < 9; i++)
    {
        SCOPED_TRACE("s2->NamedArray");
        ServiceTest2_verify_transform((*s2->NamedArray)[i], 15721 + i);
    }
}

void ServiceTestClient2::TestComplex()
{
    cdouble c1_1(5.708705e+01, -2.328294e-03);
    EXPECT_EQ(r->get_c1(), c1_1);

    cdouble c1_2(5.708705e+01, -2.328294e-03);
    EXPECT_NO_THROW(r->set_c1(c1_2));

    RR_INTRUSIVE_PTR<RRArray<cdouble> > c2_1;
    ASSERT_NO_THROW(c2_1 = r->get_c2());
    std::vector<double> c2_1_1 = GetTestVector<double>("ts3.c2.to_client");
    EXPECT_RRARRAY_EQ(c2_1, AttachRRArrayCopy<cdouble>((cdouble*)&c2_1_1[0], 16));

    std::vector<double> c2_2_1 = GetTestVector<double>("ts3.c2.to_service");
    RR_INTRUSIVE_PTR<RRArray<cdouble> > c2_2 = AttachRRArrayCopy((cdouble*)&c2_2_1[0], 16);
    EXPECT_NO_THROW(r->set_c2(c2_2));

    RR_INTRUSIVE_PTR<RRMultiDimArray<cdouble> > c3_1;
    ASSERT_NO_THROW(c3_1 = r->get_c3());
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<uint32_t> > c3_1_1 = GetTestRRArray<uint32_t>("ts3.c3.to_client.dims");
    std::vector<double> c3_1_2 = GetTestVector<double>("ts3.c3.to_client");
    EXPECT_RRARRAY_EQ(c3_1->Dims, c3_1_1);
    EXPECT_RRARRAY_EQ(c3_1->Array, AttachRRArray((cdouble*)&c3_1_2[0], 10, false));

    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<uint32_t> > c3_2_1 = GetTestRRArray<uint32_t>("ts3.c3.to_service.dims");
    std::vector<double> c3_2_2 = GetTestVector<double>("ts3.c3.to_service");
    EXPECT_NO_THROW(r->set_c3(AllocateRRMultiDimArray<cdouble>(c3_2_1, AttachRRArrayCopy((cdouble*)&c3_2_2[0], 12))));

    RR_INTRUSIVE_PTR<RRList<RRArray<cdouble> > > c5_1;
    ASSERT_NO_THROW(c5_1 = r->get_c5());
    ASSERT_TRUE(c5_1);
    std::vector<double> c5_1_1 = GetTestVector<double>("ts3.c5.to_client");
    EXPECT_RRARRAY_EQ(c5_1->front(), AttachRRArray<cdouble>((cdouble*)&c5_1_1[0], 8, false));

    RR_INTRUSIVE_PTR<RRList<RRArray<cdouble> > > c5_2 = AllocateEmptyRRList<RRArray<cdouble> >();
    std::vector<double> c5_2_1 = GetTestVector<double>("ts3.c5.to_service");
    c5_2->push_back(AttachRRArrayCopy((cdouble*)&c5_2_1[0], 8));
    EXPECT_NO_THROW(r->set_c5(c5_2));

    cfloat c7_1(-5.527021e-18, -9.848457e+03);
    EXPECT_NO_THROW(EXPECT_EQ(r->get_c7(), c7_1));

    cfloat c7_2(9.303345e-12, -3.865684e-05);
    EXPECT_NO_THROW(r->set_c7(c7_2));

    RR_INTRUSIVE_PTR<RRArray<cfloat> > c8_1;
    ASSERT_NO_THROW(c8_1 = r->get_c8());
    std::vector<float> c8_1_1 = GetTestVector<float>("ts3.c8.to_client");
    EXPECT_RRARRAY_EQ(c8_1, AttachRRArrayCopy<cfloat>((cfloat*)&c8_1_1[0], 16));

    std::vector<float> c8_2_1 = GetTestVector<float>("ts3.c8.to_service");
    RR_INTRUSIVE_PTR<RRArray<cfloat> > c8_2 = AttachRRArrayCopy((cfloat*)&c8_2_1[0], 16);
    EXPECT_NO_THROW(r->set_c8(c8_2));

    RR_INTRUSIVE_PTR<RRMultiDimArray<cfloat> > c9_1;
    ASSERT_NO_THROW(c9_1 = r->get_c9());
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<uint32_t> > c9_1_1 = GetTestRRArray<uint32_t>("ts3.c9.to_client.dims");
    std::vector<float> c9_1_2 = GetTestVector<float>("ts3.c9.to_client");
    EXPECT_RRARRAY_EQ(c9_1->Dims, c9_1_1);
    EXPECT_RRARRAY_EQ(c9_1->Array, AttachRRArray((cfloat*)&c9_1_2[0], 8, false));

    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<uint32_t> > c9_2_1 = GetTestRRArray<uint32_t>("ts3.c9.to_service.dims");
    std::vector<float> c9_2_2 = GetTestVector<float>("ts3.c9.to_service");
    EXPECT_NO_THROW(r->set_c9(AllocateRRMultiDimArray<cfloat>(c9_2_1, AttachRRArrayCopy((cfloat*)&c9_2_2[0], 8))));
}

void ServiceTestClient2::TestComplexMemories()
{
    std::vector<double> c_m1_1 = GetTestVector<double>("ts3.client.c_m1_1");
    RR_INTRUSIVE_PTR<RRArray<cdouble> > c_m1_2 = AttachRRArrayCopy((cdouble*)&c_m1_1[0], 8);
    EXPECT_NO_THROW(r->get_c_m1()->Write(10, c_m1_2, 0, 8));

    RR_INTRUSIVE_PTR<RRArray<cdouble> > c_m1_3 = AllocateRRArray<cdouble>(8);
    EXPECT_NO_THROW(r->get_c_m1()->Read(10, c_m1_3, 0, 8));

    EXPECT_RRARRAY_EQ(c_m1_2, c_m1_3);

    std::vector<uint64_t> z = boost::assign::list_of(0)(0);
    std::vector<uint64_t> c = boost::assign::list_of(3)(3);

    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<uint32_t> > c_m2_1 = GetTestRRArray<uint32_t>("ts3.client.c_m2_2.dims");
    std::vector<double> c_m2_2 = GetTestVector<double>("ts3.client.c_m2_2");
    RR_INTRUSIVE_PTR<RRMultiDimArray<cdouble> > c_m2_3 =
        AllocateRRMultiDimArray<cdouble>(c_m2_1, AttachRRArrayCopy((cdouble*)&c_m2_2[0], 9));
    EXPECT_NO_THROW(r->get_c_m2()->Write(z, c_m2_3, z, c));

    RR_INTRUSIVE_PTR<RRMultiDimArray<cdouble> > c_m2_4 =
        AllocateRRMultiDimArray<cdouble>(c_m2_1, AllocateRRArray<cdouble>(9));
    EXPECT_NO_THROW(r->get_c_m2()->Read(z, c_m2_4, z, c));

    EXPECT_RRARRAY_EQ(c_m2_3->Dims, c_m2_4->Dims);
    EXPECT_RRARRAY_EQ(c_m2_3->Array, c_m2_4->Array);
}

#define ShouldBeLockedErr(COMMAND) EXPECT_THROW(COMMAND, ObjectLockedException)

void ServiceTestClient2::TestNoLock()
{
    RR_SHARED_PTR<com::robotraconteur::testing::TestService3::obj5> o5 = r->get_nolock_test();

    ShouldBeLockedErr(o5->get_p1(););
    EXPECT_NO_THROW(o5->get_p2());
    EXPECT_NO_THROW(o5->set_p2(0));
    EXPECT_NO_THROW(o5->get_p3());
    ShouldBeLockedErr(o5->set_p1(0););

    ShouldBeLockedErr(o5->f1(););
    EXPECT_NO_THROW(o5->f2());

    ShouldBeLockedErr(o5->get_q1()->Connect(-1)->Close(););
    EXPECT_NO_THROW(o5->get_q2()->Connect(-1)->Close());
    ShouldBeLockedErr(o5->get_w1()->Connect()->Close(););
    EXPECT_NO_THROW(o5->get_w2()->Connect()->Close());

    ShouldBeLockedErr(o5->get_m1()->Length(););

    RR_INTRUSIVE_PTR<RRArray<int32_t> > b1 = AllocateRRArray<int32_t>(100);

    EXPECT_NO_THROW(o5->get_m2()->Length());
    EXPECT_NO_THROW(o5->get_m2()->Read(0, b1, 0, 10));
    EXPECT_NO_THROW(o5->get_m2()->Write(0, b1, 0, 10));

    EXPECT_NO_THROW(o5->get_m3()->Length());
    EXPECT_NO_THROW(o5->get_m3()->Read(0, b1, 0, 10));
    ShouldBeLockedErr(o5->get_m3()->Write(0, b1, 0, 10););
}

void ServiceTestClient2::TestBools()
{
    EXPECT_NO_THROW(EXPECT_NE(r->get_b1().value, 0));
    EXPECT_NO_THROW(r->set_b1(1));

    rr_bool v2_1_1[] = {1, 0, 1, 1, 0, 1, 0};
    RR_INTRUSIVE_PTR<RRArray<rr_bool> > v2_1;
    ASSERT_NO_THROW(v2_1 = r->get_b2());
    EXPECT_RRARRAY_EQ(v2_1, AttachRRArray(v2_1_1, 7, false));

    rr_bool v2_2_1[] = {1, 0, 0, 1, 1, 1, 0, 1};
    EXPECT_NO_THROW(r->set_b2(AttachRRArrayCopy(v2_2_1, 8)));

    rr_bool v3_1_1[] = {0, 1, 1, 0};
    uint32_t v3_1_dims[] = {2, 2};

    RR_INTRUSIVE_PTR<RRMultiDimArray<rr_bool> > v3_1;
    ASSERT_NO_THROW(v3_1 = r->get_b3());
    EXPECT_RRARRAY_EQ(v3_1->Dims, AttachRRArray(v3_1_dims, 2, false));
    EXPECT_RRARRAY_EQ(v3_1->Array, AttachRRArray(v3_1_1, 4, false));

    rr_bool v3_2_1[] = {1, 0};
    uint32_t v3_2_dims[] = {2, 1};

    EXPECT_NO_THROW(r->set_b3(AllocateRRMultiDimArray(AttachRRArrayCopy(v3_2_dims, 2), AttachRRArrayCopy(v3_2_1, 2))));

    RR_INTRUSIVE_PTR<RRList<RRArray<rr_bool> > > v4_1;
    ASSERT_NO_THROW(v4_1 = r->get_b4());
    ASSERT_TRUE(v4_1);
    ASSERT_EQ(v4_1->size(), 1);
    ASSERT_EQ(RRArrayToScalar(v4_1->front()), 1);

    RR_INTRUSIVE_PTR<RRList<RRArray<rr_bool> > > v4_2;
    EXPECT_NO_THROW(v4_2 = AllocateEmptyRRList<RRArray<rr_bool> >());
    v4_2->push_back(ScalarToRRArray<rr_bool>(1));
    EXPECT_NO_THROW(r->set_b4(v4_2));

    RR_INTRUSIVE_PTR<RRList<RRArray<rr_bool> > > v5_1;
    ASSERT_NO_THROW(v5_1 = r->get_b5());
    rr_bool v5_1_1[] = {0, 1, 0, 0};
    ASSERT_TRUE(v5_1);
    ASSERT_EQ(v5_1->size(), 1);
    EXPECT_RRARRAY_EQ(v5_1->front(), AttachRRArray(v5_1_1, 4, false));

    RR_INTRUSIVE_PTR<RRList<RRArray<rr_bool> > > v5_2 = AllocateEmptyRRList<RRArray<rr_bool> >();
    rr_bool v5_2_1[] = {1, 0};
    v5_2->push_back(AttachRRArrayCopy(v5_2_1, 2));
    EXPECT_NO_THROW(r->set_b5(v5_2));

    RR_INTRUSIVE_PTR<RRList<RRMultiDimArray<rr_bool> > > v6_1;
    ASSERT_NO_THROW(v6_1 = r->get_b6());
    rr_bool v6_1_1[] = {0, 1, 1, 0};
    uint32_t v6_1_dims[] = {2, 2};
    ASSERT_TRUE(v6_1);
    EXPECT_RRARRAY_EQ(v6_1->front()->Dims, AttachRRArray(v6_1_dims, 2, false));
    EXPECT_RRARRAY_EQ(v6_1->front()->Array, AttachRRArray(v6_1_1, 4, false));

    rr_bool v6_2_1[] = {1, 0};
    uint32_t v6_2_dims[] = {2, 1};
    RR_INTRUSIVE_PTR<RRList<RRMultiDimArray<rr_bool> > > v6_2 = AllocateEmptyRRList<RRMultiDimArray<rr_bool> >();
    v6_2->push_back(AllocateRRMultiDimArray(AttachRRArrayCopy(v6_2_dims, 2), AttachRRArrayCopy(v6_2_1, 2)));
    EXPECT_NO_THROW(r->set_b6(v6_2));
}

void ServiceTestClient2::TestBoolMemories()
{
    RR_SHARED_PTR<ArrayMemory<rr_bool> > c_m5;
    ASSERT_NO_THROW(c_m5 = r->get_c_m5());

    rr_bool v1_1[] = {1, 0, 0, 1, 1, 0, 0, 0, 1, 1};
    EXPECT_NO_THROW(c_m5->Write(100, AttachRRArrayCopy(v1_1, 10), 1, 8));
    RR_INTRUSIVE_PTR<RRArray<rr_bool> > v2 = AllocateRRArray<rr_bool>(10);
    EXPECT_NO_THROW(c_m5->Read(99, v2, 0, 10));
    for (size_t i = 1; i < 9; i++)
    {
        EXPECT_EQ(v2->at(i), v1_1[i]) << " at index " << i;
    }

    RR_SHARED_PTR<MultiDimArrayMemory<rr_bool> > c_m6;
    EXPECT_NO_THROW(c_m6 = r->get_c_m6());
    uint32_t v3_dims[] = {2, 5};
    RR_INTRUSIVE_PTR<RRMultiDimArray<rr_bool> > v3 =
        AllocateRRMultiDimArray(AttachRRArrayCopy(v3_dims, 2), AttachRRArrayCopy(v1_1, 10));

    std::vector<uint64_t> z = boost::assign::list_of(0)(0);
    std::vector<uint64_t> c = boost::assign::list_of(2)(5);

    EXPECT_NO_THROW(c_m6->Write(z, v3, z, c));

    std::vector<uint32_t> v4_dims = boost::assign::list_of(2)(5);

    RR_INTRUSIVE_PTR<RRMultiDimArray<rr_bool> > v4 = AllocateEmptyRRMultiDimArray<rr_bool>(v4_dims);
    EXPECT_NO_THROW(c_m6->Read(z, v4, z, c));
    EXPECT_RRARRAY_EQ(v3->Dims, v4->Dims);
    EXPECT_RRARRAY_EQ(v3->Array, v4->Array);
}

void ServiceTestClient2::TestExceptionParams()
{
    bool exp1_caught = false;
    try
    {
        r->test_exception_params1();
    }
    catch (InvalidOperationException& exp)
    {
        exp1_caught = true;
        EXPECT_EQ(exp.Message, "test error");
        EXPECT_EQ(exp.ErrorSubName, "my_error");
        EXPECT_TRUE(exp.ErrorParam);
        if (exp.ErrorParam)
        {

            RR_INTRUSIVE_PTR<RRMap<std::string, RRValue> > param_map;
            param_map = RR_DYNAMIC_POINTER_CAST<RRMap<std::string, RRValue> >(exp.ErrorParam);
            ASSERT_TRUE(param_map);
            ASSERT_EQ(param_map->size(), 2);
            EXPECT_NO_THROW(EXPECT_EQ(RRArrayToScalar(rr_cast<RRArray<int32_t> >(param_map->at("param1"))), 10));
            EXPECT_NO_THROW(EXPECT_EQ(RRArrayToString(rr_cast<RRArray<char> >(param_map->at("param2"))), "20"));
        }
    }
    EXPECT_TRUE(exp1_caught);

    bool exp2_caught = false;
    try
    {
        r->test_exception_params2();
    }
    catch (com::robotraconteur::testing::TestService3::test_exception4& exp)
    {
        exp2_caught = true;
        EXPECT_EQ(exp.Message, "test error2");
        EXPECT_EQ(exp.ErrorSubName, "my_error2");
        EXPECT_TRUE(exp.ErrorParam);

        if (exp.ErrorParam)
        {
            RR_INTRUSIVE_PTR<RRMap<std::string, RRValue> > param_map;
            param_map = RR_DYNAMIC_POINTER_CAST<RRMap<std::string, RRValue> >(exp.ErrorParam);
            ASSERT_TRUE(param_map);
            ASSERT_EQ(param_map->size(), 2);
            EXPECT_NO_THROW(EXPECT_EQ(RRArrayToScalar(rr_cast<RRArray<int32_t> >(param_map->at("param1"))), 30));
            EXPECT_NO_THROW(EXPECT_EQ(RRArrayToString(rr_cast<RRArray<char> >(param_map->at("param2"))), "40"));
        }
    }
    EXPECT_TRUE(exp2_caught);
}
} // namespace RobotRaconteurTest
