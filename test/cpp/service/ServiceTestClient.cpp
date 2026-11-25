#ifdef ROBOTRACONTEUR_USE_STDAFX
#include "stdafx.h"
#endif

#include "ServiceTestClient.h"
#include "array_compare.h"
#include "multidim_array_util.h"
#include "test_vectors.h"

using namespace std;
using namespace boost;
using namespace RobotRaconteur;
using namespace com::robotraconteur::testing::TestService1;
using namespace com::robotraconteur::testing::TestService2;
using namespace RobotRaconteur::test;

using std::exception;

// cSpell: ignore mydat, lstruct, numa, structa, myind, dimsa

namespace RobotRaconteurTest
{

static void ServiceTestClient_Service_Listener(RR_SHARED_PTR<ClientContext> ctx, ClientServiceListenerEventType evt,
                                               RR_SHARED_PTR<void> p)
{
    std::cout << evt;
    if (evt == ClientServiceListenerEventType_ServicePathReleased)
    {
        std::cout << " path: " << *RR_STATIC_POINTER_CAST<std::string>(p);
    }
    std::cout << endl;
}

void ServiceTestClient::ConnectService(string url)
{

    r = rr_cast<testroot>(RobotRaconteurNode::s()->ConnectService(
        url, "", RR_INTRUSIVE_PTR<RRMap<std::string, RRValue> >(), &ServiceTestClient_Service_Listener));
}

void ServiceTestClient::RunFullTest(string url, string authurl)
{

    ConnectService(url);

    cout << "Test Members" << endl;
    TestProperties();
    TestFunctions();

    TestEvents();
    TestObjRefs();

    TestPipes();
    TestCallbacks();
    TestWires();
    TestMemories();

    DisconnectService();

    // boost::this_thread::sleep(boost::posix_time::milliseconds(100));
    cout << "Test Authentication" << endl;
    TestAuthentication(authurl);

    cout << "Test Object Locks" << endl;
    TestObjectLock(authurl);

    cout << "Test Monitor Locks" << endl;
    TestMonitorLock(url);

    cout << "Test Async" << endl;
    TestAsync(authurl);

    cout << "Done" << endl;
}

void ServiceTestClient::RunMinimalTest(string url)
{

    ASSERT_NO_THROW(ConnectService(url););

    // TestProperties();

    // basic double properties
    EXPECT_NO_THROW(r->set_d1(3.456));
    EXPECT_NO_THROW(EXPECT_DOUBLE_EQ(r->get_d1(), 12.345));
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > d2_a = GetTestRRArray<double>("ts1.d2.to_service");
    EXPECT_NO_THROW(r->set_d2(d2_a));
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > d2_b = GetTestRRArray<double>("ts1.d2.to_client");
    EXPECT_RRARRAY_EQ(d2_b, r->get_d2());
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > d3_a = GetTestRRArray<double>("ts1.d3.to_service");
    EXPECT_NO_THROW(r->set_d3(d3_a));
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > d3_b = GetTestRRArray<double>("ts1.d3.to_client");
    EXPECT_RRARRAY_EQ(d3_b, r->get_d3());
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > d4_a = GetTestRRArray<double>("ts1.d4.to_service");
    EXPECT_NO_THROW(r->set_d4(d4_a));
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > d4_b = GetTestRRArray<double>("ts1.d4.to_client");
    EXPECT_RRARRAY_EQ(d4_b, r->get_d4());

    TestFunctions();

    TestEvents();
    // TestObjRefs();

    /*TestPipes();
    TestCallbacks();
    TestWires();
    TestMemories();*/

    EXPECT_NO_THROW(DisconnectService(););
}

void ServiceTestClient::TestProperties()
{

    // basic double properties
    EXPECT_NO_THROW(r->set_d1(3.456));
    EXPECT_NO_THROW(EXPECT_DOUBLE_EQ(r->get_d1(), 12.345));
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > d2_a = GetTestRRArray<double>("ts1.d2.to_service");
    EXPECT_NO_THROW(r->set_d2(d2_a));
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > d2_b = GetTestRRArray<double>("ts1.d2.to_client");
    EXPECT_RRARRAY_EQ(d2_b, r->get_d2());
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > d3_a = GetTestRRArray<double>("ts1.d3.to_service");
    EXPECT_NO_THROW(r->set_d3(d3_a));
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > d3_b = GetTestRRArray<double>("ts1.d3.to_client");
    EXPECT_RRARRAY_EQ(d3_b, r->get_d3());
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > d4_a = GetTestRRArray<double>("ts1.d4.to_service");
    EXPECT_NO_THROW(r->set_d4(d4_a));
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > d4_b = GetTestRRArray<double>("ts1.d4.to_client");
    EXPECT_RRARRAY_EQ(d4_b, r->get_d4());

    // multi dim arrays

    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<uint32_t> > d5_a1 = GetTestRRArray<uint32_t>("ts1.d5.to_service.dims");
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > d5_a2 = GetTestRRArray<double>("ts1.d5.to_service");
    EXPECT_NO_THROW(r->set_d5(AllocateRRMultiDimArray<double>(d5_a1, d5_a2)));

    RR_INTRUSIVE_PTR<RRMultiDimArray<double> > d5_2 = r->get_d5();
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<uint32_t> > d5_b1 = GetTestRRArray<uint32_t>("ts1.d5.to_client.dims");
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > d5_b2 = GetTestRRArray<double>("ts1.d5.to_client");
    EXPECT_RRARRAY_EQ(d5_2->Dims, d5_b1);
    EXPECT_RRARRAY_EQ(d5_2->Array, d5_b2);

    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<uint32_t> > d6_a1 = GetTestRRArray<uint32_t>("ts1.d6.to_service.dims");
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > d6_a2 = GetTestRRArray<double>("ts1.d6.to_service");
    EXPECT_NO_THROW(r->set_d6(AllocateRRMultiDimArray<double>(d6_a1, d6_a2)));

    RR_INTRUSIVE_PTR<RRMultiDimArray<double> > d6_2 = r->get_d6();
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<uint32_t> > d6_b1 = GetTestRRArray<uint32_t>("ts1.d6.to_client.dims");
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > d6_b2 = GetTestRRArray<double>("ts1.d6.to_client");
    EXPECT_RRARRAY_EQ(d6_2->Dims, d6_b1);
    EXPECT_RRARRAY_EQ(d6_2->Array, d6_b2);

    // Test various number types

    // floats
    EXPECT_NO_THROW(r->set_s1(3847.9283f));
    EXPECT_NO_THROW(EXPECT_FLOAT_EQ(r->get_s1(), 7.8573f));
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<float> > s2_a = GetTestRRArray<float>("ts1.s2.to_service");
    EXPECT_NO_THROW(r->set_s2(s2_a));
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<float> > s2_b = GetTestRRArray<float>("ts1.s2.to_client");
    EXPECT_RRARRAY_EQ(r->get_s2(), s2_b);

    // int8
    EXPECT_NO_THROW(r->set_i8_1(45));
    EXPECT_NO_THROW(EXPECT_EQ(r->get_i8_1(), -66));
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<int8_t> > i8_2a = GetTestRRArray<int8_t>("ts1.i8_2.to_service");
    EXPECT_NO_THROW(r->set_i8_2(i8_2a));
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<int8_t> > i8_2b = GetTestRRArray<int8_t>("ts1.i8_2.to_client");
    EXPECT_RRARRAY_EQ(i8_2b, r->get_i8_2());

    // uint8
    EXPECT_NO_THROW(r->set_u8_1(232));
    EXPECT_NO_THROW(EXPECT_EQ(r->get_u8_1(), 222));
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<uint8_t> > u8_2a = GetTestRRArray<uint8_t>("ts1.u8_2.to_service");
    EXPECT_NO_THROW(r->set_u8_2(u8_2a));
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<uint8_t> > u8_2b = GetTestRRArray<uint8_t>("ts1.u8_2.to_client");
    EXPECT_RRARRAY_EQ(u8_2b, r->get_u8_2());

    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<uint32_t> > u8_3_1a1 =
        GetTestRRArray<uint32_t>("ts1.u8_3.to_service.dims");
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<uint8_t> > u8_3_1a2 = GetTestRRArray<uint8_t>("ts1.u8_3.to_service");
    EXPECT_NO_THROW(r->set_u8_3(AllocateRRMultiDimArray<uint8_t>(u8_3_1a1, u8_3_1a2)));
    RR_INTRUSIVE_PTR<RRMultiDimArray<uint8_t> > u8_3_2 = r->get_u8_3();
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<uint32_t> > u8_3_1b1 = GetTestRRArray<uint32_t>("ts1.u8_3.to_client.dims");
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<uint8_t> > u8_3_1b2 = GetTestRRArray<uint8_t>("ts1.u8_3.to_client");
    EXPECT_RRARRAY_EQ(u8_3_2->Dims, u8_3_1b1);
    EXPECT_RRARRAY_EQ(u8_3_2->Array, u8_3_1b2);

    // int16
    EXPECT_NO_THROW(r->set_i16_1(2387));
    EXPECT_NO_THROW(EXPECT_EQ(r->get_i16_1(), -13428));
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<int16_t> > i16_2a = GetTestRRArray<int16_t>("ts1.i16_2.to_service");
    EXPECT_NO_THROW(r->set_i16_2(i16_2a));
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<int16_t> > i16_2b = GetTestRRArray<int16_t>("ts1.i16_2.to_client");
    EXPECT_RRARRAY_EQ(i16_2b, r->get_i16_2());

    // uint16
    EXPECT_NO_THROW(r->set_u16_1(54732));
    EXPECT_NO_THROW(EXPECT_EQ(r->get_u16_1(), 60981));
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<uint16_t> > u16_2a = GetTestRRArray<uint16_t>("ts1.u16_2.to_service");
    EXPECT_NO_THROW(r->set_u16_2(u16_2a));
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<uint16_t> > u16_2b = GetTestRRArray<uint16_t>("ts1.u16_2.to_client");
    EXPECT_RRARRAY_EQ(u16_2b, r->get_u16_2());

    // int32
    EXPECT_NO_THROW(r->set_i32_1(-9837284));
    EXPECT_NO_THROW(EXPECT_EQ(r->get_i32_1(), 898734));
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<int32_t> > i32_1a = GetTestRRArray<int32_t>("ts1.i32_2.to_service");
    EXPECT_NO_THROW(r->set_i32_2(i32_1a));
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<int32_t> > i32_1b = GetTestRRArray<int32_t>("ts1.i32_2.to_client");
    EXPECT_RRARRAY_EQ(i32_1b, r->get_i32_2());

    // int32 huge
    {
        RR_INTRUSIVE_PTR<RRArray<int32_t> > i32_huge_1 = AllocateRRArray<int32_t>(2621440);
        for (int32_t i = 0; i < 2621440; i++)
        {
            (*i32_huge_1)[i] = -i;
        }
        EXPECT_NO_THROW(r->set_i32_huge(i32_huge_1));
    }
    {
        RR_INTRUSIVE_PTR<RRArray<int32_t> > i32_huge_2 = r->get_i32_huge();
        EXPECT_NO_THROW(EXPECT_EQ(i32_huge_2->size(), 2621440));
        if (i32_huge_2->size() == 2621440)
        {
            for (int32_t i = 0; i < 2621440; i++)
            {
                EXPECT_EQ((*i32_huge_2)[i], i);
                if ((*i32_huge_2)[i] != i)
                {
                    break;
                }
            }
        }
    }

    // uint32
    EXPECT_NO_THROW(r->set_u32_1(1550099195u));
    EXPECT_NO_THROW(EXPECT_EQ(r->get_u32_1(), 547919123u));
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<uint32_t> > u32_1a = GetTestRRArray<uint32_t>("ts1.u32_2.to_service");
    EXPECT_NO_THROW(r->set_u32_2(u32_1a));
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<uint32_t> > u32_1b = GetTestRRArray<uint32_t>("ts1.u32_2.to_client");
    EXPECT_RRARRAY_EQ(u32_1b, r->get_u32_2());

    // int64
    EXPECT_NO_THROW(r->set_i64_1(8621740821050813024));
    EXPECT_NO_THROW(EXPECT_EQ(r->get_i64_1(), -1357833931563696072));
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<int64_t> > i64_2a = GetTestRRArray<int64_t>("ts1.i64_2.to_service");
    EXPECT_NO_THROW(r->set_i64_2(i64_2a));
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<int64_t> > i64_2b = GetTestRRArray<int64_t>("ts1.i64_2.to_client");
    EXPECT_RRARRAY_EQ(i64_2b, r->get_i64_2());

    // uint64
    EXPECT_NO_THROW(r->set_u64_1(1465640522145789825u));
    EXPECT_NO_THROW(EXPECT_EQ(r->get_u64_1(), 13389861970863644378u));
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<uint64_t> > u64_1a = GetTestRRArray<uint64_t>("ts1.u64_2.to_service");
    EXPECT_NO_THROW(r->set_u64_2(u64_1a));
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<uint64_t> > u64_1b = GetTestRRArray<uint64_t>("ts1.u64_2.to_client");
    EXPECT_RRARRAY_EQ(u64_1b, r->get_u64_2());

    // string
    EXPECT_NO_THROW(r->set_str1("Hello Server!"));
    EXPECT_NO_THROW(EXPECT_EQ(r->get_str1(), "Hello Client!"));

    // Read in and check the large structure struct1
    RR_INTRUSIVE_PTR<teststruct1> s1 = r->get_struct1();
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > s1_dat1a = GetTestRRArray<double>("ts1.struct1.dat1.to_client");
    EXPECT_RRARRAY_EQ(s1->dat1, s1_dat1a);
    EXPECT_NO_THROW(EXPECT_EQ(s1->str2, "Hello world!"));
    EXPECT_NO_THROW(EXPECT_EQ(s1->vec3->size(), 3));
    EXPECT_NO_THROW(EXPECT_EQ(RRArrayToString(s1->vec3->at(1)), "Hello Client!"));
    EXPECT_NO_THROW(EXPECT_EQ(RRArrayToString(s1->vec3->at(2)), "Hello Client, again"));
    EXPECT_NO_THROW(EXPECT_EQ(RRArrayToString(s1->vec3->at(4372)), "This is yet another test string"));

    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > s1_s1_mydat =
        GetTestRRArray<double>("ts1.struct1.struct1_mydat.to_client");
    EXPECT_RRARRAY_EQ(s1->struct1->mydat, s1_s1_mydat);
    EXPECT_NO_THROW(EXPECT_EQ(s1->dstruct2->size(), 2));
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > s1_d1a =
        GetTestRRArray<double>("ts1.struct1.dstruct2_1.to_client");
    EXPECT_RRARRAY_EQ(s1->dstruct2->at("test1")->mydat, s1_d1a);
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > s1_d1b =
        GetTestRRArray<double>("ts1.struct1.dstruct2_3.to_client");
    EXPECT_RRARRAY_EQ(s1->dstruct2->at("anothertest")->mydat, s1_d1b);

    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<uint32_t> > s1_mul_a =
        GetTestRRArray<uint32_t>("ts1.struct1.multidimarray.to_client.dims");
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > s1_mul_b =
        GetTestRRArray<double>("ts1.struct1.multidimarray.to_client");
    EXPECT_RRARRAY_EQ(s1->multidimarray->Dims, s1_mul_a);
    EXPECT_RRARRAY_EQ(s1->multidimarray->Array, s1_mul_b);

    // Set large structure for struct1
    RR_INTRUSIVE_PTR<teststruct1> s2(new teststruct1());
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > dat1a = GetTestRRArray<double>("ts1.struct1.dat1.to_service");
    s2->dat1 = dat1a;
    s2->str2 = "Hello world 2!";
    RR_INTRUSIVE_PTR<RRMap<int32_t, RRArray<char> > > s2_3 = AllocateEmptyRRMap<int32_t, RRArray<char> >();
    s2_3->insert(make_pair(10, stringToRRArray("Hello Server!")));
    s2_3->insert(make_pair(11, stringToRRArray("Hello Server, again")));
    s2_3->insert(make_pair(46372, stringToRRArray("Test string!")));
    s2_3->insert(make_pair(46373, stringToRRArray("Test string again")));
    s2->vec3 = s2_3;

    RR_INTRUSIVE_PTR<RRMap<string, RRArray<char> > > s2_4 = AllocateEmptyRRMap<string, RRArray<char> >();
    s2_4->insert(make_pair("cteststring1", stringToRRArray("Hello Server!")));
    s2_4->insert(make_pair("cteststring2", stringToRRArray("Hello Server, again")));
    s2->dict4 = s2_4;

    RR_INTRUSIVE_PTR<RRList<RRArray<char> > > s2_5 = AllocateEmptyRRList<RRArray<char> >();
    s2_5->push_back(stringToRRArray("Hello Server!"));
    s2_5->push_back(stringToRRArray("Hello Server, again"));
    s2->list5 = s2_5;

    s2->struct1 = new teststruct2();
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > s2_struct1_mydat =
        GetTestRRArray<double>("ts1.struct1.struct1_mydat.to_service");
    s2->struct1->mydat = s2_struct1_mydat;

    RR_INTRUSIVE_PTR<RRMap<string, teststruct2> > s2_ds2 = AllocateEmptyRRMap<string, teststruct2>();
    RR_INTRUSIVE_PTR<teststruct2> s2_ds2_1(new teststruct2());
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > s2_ds2_1a =
        GetTestRRArray<double>("ts1.struct1.dstruct2_1.to_service");
    s2_ds2_1->mydat = s2_ds2_1a;
    s2_ds2->insert(make_pair("ctest1", s2_ds2_1));
    RR_INTRUSIVE_PTR<teststruct2> s2_ds2_2(new teststruct2());
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > s2_ds2_2a =
        GetTestRRArray<double>("ts1.struct1.dstruct2_2.to_service");
    s2_ds2_2->mydat = s2_ds2_2a;
    s2_ds2->insert(make_pair("anothertest", s2_ds2_2));
    s2->dstruct2 = s2_ds2;

    RR_INTRUSIVE_PTR<RRList<teststruct2> > s2_ls3 = AllocateEmptyRRList<teststruct2>();
    RR_INTRUSIVE_PTR<teststruct2> s2_ls3_1(new teststruct2());
    // double s2_ls3_1a[]={ 4.122753e+13, -2.656829e-13, 1.813864e-04, -4.675181e-05, 1.759511e-19, 3.517805e+10,
    // -7.912215e+01, 7.708557e-07, 2.434017e-21, -2.540544e+00, -9.412568e+15, -2.124215e-18, 2.797799e+13,
    // -2.240464e-07, 2.780110e-12, -1.025574e-14, -3.762272e-09, -5.715981e-02, 1.839704e-21,
    // -4.719538e-15, 3.148156e-06, 3.483886e-12, 3.484006e-02, -4.544817e-08, 3.200642e+00, 4.503141e+07,
    // -4.077123e+04, -2.776985e+00, -2.900651e-18, -1.463711e+08, -3.460292e-03, 2.348911e-18, -3.704219e+08,
    // -3.275364e+05, 4.613595e-01, 4.867108e+16, 4.114866e-10, 3.070767e+17, 4.662623e+01 };
    s2_ls3_1->mydat = s2_ds2_1a;
    s2_ls3->push_back(s2_ds2_1);
    RR_INTRUSIVE_PTR<teststruct2> s2_ls3_2(new teststruct2());
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > s2_ls3_2a =
        GetTestRRArray<double>("ts1.struct1.dstruct2_2.to_service");
    s2_ls3_2->mydat = s2_ls3_2a;
    s2_ls3->push_back(s2_ds2_2);
    s2->lstruct3 = s2_ls3;

    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<uint32_t> > s2_mda =
        GetTestRRArray<uint32_t>("ts1.struct1.multidimarray.to_service.dims");
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > s2_mdb =
        GetTestRRArray<double>("ts1.struct1.multidimarray.to_service");
    s2->multidimarray = AllocateRRMultiDimArray<double>(s2_mda, s2_mdb);
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > s2_var3a = GetTestRRArray<double>("ts1.struct1.var3.to_service");
    s2->var3 = s2_var3a;

    EXPECT_NO_THROW(r->set_struct1(s2));

    // Test is_d1
    RR_INTRUSIVE_PTR<RRMap<int32_t, RRArray<double> > > is_d1_1 = r->get_is_d1();
    EXPECT_NO_THROW(EXPECT_EQ(is_d1_1->size(), 3));
    EXPECT_NO_THROW(EXPECT_EQ(RRArrayToScalar(is_d1_1->at(9285)), 1.643392e-1));
    EXPECT_NO_THROW(EXPECT_EQ(RRArrayToScalar(is_d1_1->at(74822)), 1.537133e+09));
    EXPECT_NO_THROW(EXPECT_EQ(RRArrayToScalar(is_d1_1->at(4)), 1.369505e-03));

    RR_INTRUSIVE_PTR<RRMap<int32_t, RRArray<double> > > is_d1_2 = AllocateEmptyRRMap<int32_t, RRArray<double> >();
    is_d1_2->insert(make_pair(928, ScalarToRRArray(4.074501e-07)));
    is_d1_2->insert(make_pair(394820, ScalarToRRArray(-4.535303e+05)));
    is_d1_2->insert(make_pair(623, ScalarToRRArray(-2.956241e-20)));
    EXPECT_NO_THROW(r->set_is_d1(is_d1_2));

    // Test is_d2
    RR_INTRUSIVE_PTR<RRMap<string, RRArray<double> > > is_d2_1 = r->get_is_d2();
    EXPECT_NO_THROW(EXPECT_EQ(is_d2_1->size(), 2));
    EXPECT_DOUBLE_EQ(RRArrayToScalar(is_d2_1->at("testval1")), -1.079664e+16);
    EXPECT_DOUBLE_EQ(RRArrayToScalar(is_d2_1->at("testval2")), 2.224846e+00);

    RR_INTRUSIVE_PTR<RRMap<string, RRArray<double> > > is_d2_2 = AllocateEmptyRRMap<string, RRArray<double> >();
    is_d2_2->insert(make_pair("testval3", ScalarToRRArray(5.242474e+10)));
    is_d2_2->insert(make_pair("testval4", ScalarToRRArray(2.208636e+08)));
    EXPECT_NO_THROW(r->set_is_d2(is_d2_2));

    // Test is_d3

    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > is_d3_1a = GetTestRRArray<double>("ts1.is_d3.to_client.a");
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > is_d3_1b = GetTestRRArray<double>("ts1.is_d3.to_client.b");

    RR_INTRUSIVE_PTR<RRMap<int32_t, RRArray<double> > > is_d3_1 = r->get_is_d3();
    EXPECT_EQ(is_d3_1->size(), 2);
    EXPECT_RRARRAY_EQ(is_d3_1->at(12), is_d3_1a);
    EXPECT_RRARRAY_EQ(is_d3_1->at(832), is_d3_1b);

    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > is_d3_2a = GetTestRRArray<double>("ts1.is_d3.to_service.a");
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > is_d3_2b = GetTestRRArray<double>("ts1.is_d3.to_service.b");

    RR_INTRUSIVE_PTR<RRMap<int32_t, RRArray<double> > > is_d3_2 = AllocateEmptyRRMap<int32_t, RRArray<double> >();
    is_d3_2->insert(make_pair(47, is_d3_2a));
    is_d3_2->insert(make_pair(324, is_d3_2b));
    EXPECT_NO_THROW(r->set_is_d3(is_d3_2));

    // Test is_d4

    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > is_d4_1a = GetTestRRArray<double>("ts1.is_d4.to_client.a");
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > is_d4_1b = GetTestRRArray<double>("ts1.is_d4.to_client.b");

    RR_INTRUSIVE_PTR<RRMap<string, RRArray<double> > > is_d4_1 = r->get_is_d4();
    EXPECT_NO_THROW(EXPECT_EQ(is_d4_1->size(), 2));
    EXPECT_RRARRAY_EQ(is_d4_1->at("testval1"), is_d4_1a);
    EXPECT_RRARRAY_EQ(is_d4_1->at("testval2"), is_d4_1b);

    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > is_d4_2a = GetTestRRArray<double>("ts1.is_d4.to_service.a");
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > is_d4_2b = GetTestRRArray<double>("ts1.is_d4.to_service.b");

    RR_INTRUSIVE_PTR<RRMap<string, RRArray<double> > > is_d4_2 = AllocateEmptyRRMap<string, RRArray<double> >();
    is_d4_2->insert(make_pair("testval3", is_d4_2a));
    is_d4_2->insert(make_pair("testval4", is_d4_2b));
    EXPECT_NO_THROW(r->set_is_d4(is_d4_2));

    // Test is_d5
    RR_INTRUSIVE_PTR<RRMap<int, RRMultiDimArray<double> > > is_d5_1 = r->get_is_d5();
    EXPECT_NO_THROW(EXPECT_EQ(is_d5_1->size(), 1));
    RR_INTRUSIVE_PTR<RRMultiDimArray<double> > is_d5_1_1 = is_d5_1->at(564);
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<uint32_t> > is_d5_1_1a =
        GetTestRRArray<uint32_t>("ts1.is_d5.to_client.dims");
    EXPECT_RRARRAY_EQ(is_d5_1_1->Dims, is_d5_1_1a);
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > is_d5_1_1b = GetTestRRArray<double>("ts1.is_d5.to_client");
    EXPECT_RRARRAY_EQ(is_d5_1_1->Array, is_d5_1_1b);

    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<uint32_t> > is_d5_2_1a =
        GetTestRRArray<uint32_t>("ts1.is_d5.to_service.dims");
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > is_d5_2_1b = GetTestRRArray<double>("ts1.is_d5.to_service");
    RR_INTRUSIVE_PTR<RRMultiDimArray<double> > is_d5_2_1 = AllocateRRMultiDimArray<double>(is_d5_2_1a, is_d5_2_1b);
    RR_INTRUSIVE_PTR<RRMap<int32_t, RRMultiDimArray<double> > > is_d5_2 =
        AllocateEmptyRRMap<int32_t, RRMultiDimArray<double> >();
    is_d5_2->insert(make_pair(328, is_d5_2_1));
    EXPECT_NO_THROW(r->set_is_d5(is_d5_2));

    // Test is_d6
    RR_INTRUSIVE_PTR<RRMap<string, RRMultiDimArray<double> > > is_d6_1 = r->get_is_d6();
    EXPECT_NO_THROW(EXPECT_EQ(is_d6_1->size(), 1));
    RR_INTRUSIVE_PTR<RRMultiDimArray<double> > is_d6_1_1 = is_d6_1->at("testval1");
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<uint32_t> > is_d6_1_1a =
        GetTestRRArray<uint32_t>("ts1.is_d6.to_client.dims");
    EXPECT_RRARRAY_EQ(is_d6_1_1->Dims, is_d6_1_1a);
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > is_d6_1_1b = GetTestRRArray<double>("ts1.is_d6.to_client");
    EXPECT_RRARRAY_EQ(is_d6_1_1->Array, is_d6_1_1b);

    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<uint32_t> > is_d6_2_1a =
        GetTestRRArray<uint32_t>("ts1.is_d6.to_service.dims");
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > is_d6_2_1b = GetTestRRArray<double>("ts1.is_d6.to_service");
    RR_INTRUSIVE_PTR<RRMultiDimArray<double> > is_d6_2_1 = AllocateRRMultiDimArray<double>(is_d6_2_1a, is_d6_2_1b);
    RR_INTRUSIVE_PTR<RRMap<string, RRMultiDimArray<double> > > is_d6_2 =
        AllocateEmptyRRMap<string, RRMultiDimArray<double> >();
    is_d6_2->insert(make_pair("testval2", is_d6_2_1));
    EXPECT_NO_THROW(r->set_is_d6(is_d6_2));

    // Test is_struct1
    RR_INTRUSIVE_PTR<RRMap<int32_t, teststruct2> > is_struct1_1 = r->get_is_struct1();
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > is_struct1_1a =
        GetTestRRArray<double>("ts1.is_struct1.to_client");
    EXPECT_RRARRAY_EQ(is_struct1_1->at(748)->mydat, is_struct1_1a);

    RR_INTRUSIVE_PTR<RRMap<int32_t, teststruct2> > is_struct1_2 = AllocateEmptyRRMap<int32_t, teststruct2>();
    RR_INTRUSIVE_PTR<teststruct2> is_struct1_2_1(new teststruct2());
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > is_struct1_2_1a =
        GetTestRRArray<double>("ts1.is_struct1.to_service");
    is_struct1_2_1->mydat = is_struct1_2_1a;
    is_struct1_2->insert(make_pair(372, is_struct1_2_1));
    EXPECT_NO_THROW(r->set_is_struct1(is_struct1_2));

    // Test is_struct2
    RR_INTRUSIVE_PTR<RRMap<string, teststruct2> > is_struct2_1 = r->get_is_struct2();
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > is_struct2_1a =
        GetTestRRArray<double>("ts1.is_struct2.to_client");
    EXPECT_RRARRAY_EQ(is_struct2_1->at("testval1")->mydat, is_struct2_1a);

    RR_INTRUSIVE_PTR<RRMap<string, teststruct2> > is_struct2_2 = AllocateEmptyRRMap<string, teststruct2>();
    RR_INTRUSIVE_PTR<teststruct2> is_struct2_2_1(new teststruct2());
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > is_struct2_2_1a =
        GetTestRRArray<double>("ts1.is_struct2.to_service");
    is_struct2_2_1->mydat = is_struct2_2_1a;
    is_struct2_2->insert(make_pair("testval2", is_struct2_2_1));
    EXPECT_NO_THROW(r->set_is_struct2(is_struct2_2));

    // Test list_d1

    RR_INTRUSIVE_PTR<RRList<RRArray<double> > > list_d1_1 = r->get_list_d1();
    EXPECT_NO_THROW(EXPECT_EQ(list_d1_1->size(), 3));
    EXPECT_NO_THROW(EXPECT_EQ(RRArrayToScalar(list_d1_1->front()), 1.643392e-1));
    EXPECT_NO_THROW(EXPECT_EQ(RRArrayToScalar(*(++list_d1_1->begin())), 1.537133e+09));
    EXPECT_NO_THROW(EXPECT_EQ(RRArrayToScalar(*(++(++list_d1_1->begin()))), 1.369505e-03));

    RR_INTRUSIVE_PTR<RRList<RRArray<double> > > list_d1_2 = AllocateEmptyRRList<RRArray<double> >();
    list_d1_2->push_back(ScalarToRRArray(4.074501e-07));
    list_d1_2->push_back(ScalarToRRArray(-4.535303e+05));
    list_d1_2->push_back(ScalarToRRArray(-2.956241e-20));
    EXPECT_NO_THROW(r->set_list_d1(list_d1_2));

    // Test list_d3

    // double list_d3_1a[]={8.609080e-13, 3.946603e+03, 2.994203e-10, 3.200877e+14, 1.747361e-09, 2.827056e-16,
    // -3.676613e-18, 1.886901e-14, -9.970511e-12, 1.932468e-18, -3.629253e-05, 4.903023e-12,
    // -3.919949e-10, 4.982164e+07, 3.823096e-20, -4.044068e-13, 3.114078e+09, 7.572697e-12, -2.619929e+04,
    // -3.882046e+01}; double list_d3_1b[]={ 4.750899e+00, 3.924377e+18, -2.735066e+17, 4.095362e-21,
    // -2.407932e+09, 4.059499e+10, 1.376975e-10, -8.547220e-21,
    // -1.344568e-20, 2.809398e+03, 2.118944e-06, 2.435328e-03, -1.410999e-12, 9.907226e-04,
    // -9.745948e-20, 1.270118e+15, -2.833333e+05, 1.032636e-10, 5.312574e+13, -2.651512e+02 };

    RR_INTRUSIVE_PTR<RRList<RRArray<double> > > list_d3_1 = r->get_list_d3();
    EXPECT_EQ(list_d3_1->size(), 2);
    EXPECT_RRARRAY_EQ(list_d3_1->front(), is_d3_1a);
    EXPECT_RRARRAY_EQ(*(++list_d3_1->begin()), is_d3_1b);

    // double list_d3_2a[]={ 4.335907e-08, -3.270294e-03, 1.752801e-01, 1.235219e-20, -4.348647e+02, -4.503864e-21,
    // -3.316231e+15, -2.080056e+17, 1.813854e+13,
    // -3.380846e-05, 4.350998e+03, 4.539570e+11, 8.981827e+09, 3.326114e+01, 2.975688e+06,
    // -1.017456e-12, 2.989498e-03, 2.842392e-03, -1.258677e-21, 1.068563e-15 }; double
    // list_d3_2b[]={ 3.239279e+12, 1.047689e+17, -1.236114e+17, -4.002822e-17, 2.657374e-03, 7.383907e-19,
    // -5.067889e-13, -4.195122e-12, 3.642885e-01, -2.946040e+14, 5.522403e-08, 6.603132e+04, 1.464154e+05,
    // -1.851534e-08, 2.808294e-13, -2.702278e-11, 3.850704e-06, -2.453957e+02, -3.015401e-02, 1.654070e+05 };

    RR_INTRUSIVE_PTR<RRList<RRArray<double> > > list_d3_2 = AllocateEmptyRRList<RRArray<double> >();
    list_d3_2->push_back(is_d3_2a);
    list_d3_2->push_back(is_d3_2b);
    EXPECT_NO_THROW(r->set_list_d3(list_d3_2));

    // Test list_d5

    RR_INTRUSIVE_PTR<RRList<RRMultiDimArray<double> > > list_d5_1 = r->get_list_d5();
    EXPECT_NO_THROW(EXPECT_EQ(list_d5_1->size(), 1));
    RR_INTRUSIVE_PTR<RRMultiDimArray<double> > list_d5_1_1 = list_d5_1->front();
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<uint32_t> > list_d5_1_1a =
        GetTestRRArray<uint32_t>("ts1.is_d5.to_client.dims");
    EXPECT_RRARRAY_EQ(list_d5_1_1->Dims, list_d5_1_1a);
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > list_d5_1_1b = GetTestRRArray<double>("ts1.is_d5.to_client");
    EXPECT_RRARRAY_EQ(list_d5_1_1->Array, list_d5_1_1b);

    // int list_d5_2_1a[]={10,20};
    // double list_d5_2_1b[]={ 2.792909e-01, 6.554477e+16, 4.240073e-13, -4.490109e+19, 5.410527e-22, -2.244599e+17,
    // -2.656142e-02, -3.819500e+13, -7.086082e-02, 7.790729e-13, 3.375900e-12, -6.915692e+09,
    // -2.900437e-18, 1.257280e+05, -3.810852e+15, -4.589554e-12, 2.670612e-14, 4.725686e+06,
    // -3.018046e+07, 2.439452e+07, 2.726039e-07, -2.805143e+02, -1.870376e+03, 4.573047e-06, 1.904868e+19,
    // -1.966383e+00, 3.426469e-11, -1.400396e+13, -1.724273e+09, -7.347198e+10, -4.081057e-12, -3.868203e+10,
    // -2.686071e+13, -5.289107e+01, -5.574151e-09, -2.580185e-06, -8.222097e-21, -4.957833e-12, -2.491984e+03,
    // -7.900042e+16, -4.809370e-11, -2.048332e-19, 4.984852e-21, 1.350023e+13, -4.492022e-11,
    // -3.255594e+10, 1.495149e-09, -7.272628e+02, -4.236196e-04, 4.736990e-02,
    // -4.030173e-11, 1.017371e+11, 1.124559e-09, 4.177431e-21, 1.026706e+06, -4.702729e-04, -2.633498e+18,
    // -4.689724e+08, -2.593657e+05, 3.433194e-18, -1.977738e-13,
    // -1.163773e+03, 3.424738e-20, 7.391132e-06, 1.364867e+12, -7.155727e+16, 3.078093e-21, -3.151787e-04,
    // -4.715633e+06, 1.017894e+19, -1.121778e+14, -3.529769e-10, 4.530606e+19, 3.988296e-17,
    // -3.469818e+06, 1.204304e+03, -1.404314e+15, -1.369871e+04, -2.796125e-03, -4.842068e-06,
    // -2.639632e-03, 1.324740e+08, 1.440651e+07, -4.778885e+03, -4.643859e+06, 1.726955e-09,
    // -8.160334e+05, 3.763238e+13, 1.391028e+02, -4.269393e+04, -2.698233e+02,
    // -3.677556e+14, 1.070699e-17, 3.949376e+19, 4.503080e-06, 4.344496e-07, 1.714091e-19, -3.436426e+01, 4.914505e+15,
    // -1.101617e+09, -1.899511e-04, 2.195951e-06, 2.402701e-12, 1.783431e-09,
    // -7.329137e-08, 4.423889e+16, 2.812547e-19, -7.848554e+05, -3.635151e+13, 3.128605e-09,
    // -2.858963e+08, 2.086065e-11, -2.544450e+12, 1.450579e+19,
    // -1.508905e+13, 4.307174e+00, 1.038108e-05, 4.313281e-05, 3.647351e+05, 1.309105e-16, 4.180469e+13, -2.701332e-07,
    // -4.033566e+14, -3.116748e-06, 2.342296e-07, 1.870335e-19, 2.312273e+01, -4.478923e+08,
    // -4.854324e+09, 2.681828e+03, -4.280128e-01, -4.690703e-21, 3.853815e+16, 1.366639e+02, -2.944985e-11,
    // -4.486958e-13, 3.017750e-11, 3.551437e-13, 2.263828e-12, -6.545014e-18,
    // -7.552023e+12, 7.595238e+14, 2.810247e+12, 6.516008e+15, -3.035786e+14, 2.523040e+11,
    // -3.766603e+09, 7.316287e+18, -2.147132e+17, 1.972210e+10, 2.906768e-13, 4.226577e-14,
    // -2.640568e+17, 2.181408e+10, -1.043256e-08, -3.649181e+06, -2.776638e+18, 3.660147e-07, -1.415433e-17,
    // -4.945127e-17, 2.655050e+01, -2.269828e+04, -2.585499e-01, -3.299965e+05, 3.707494e-18, -1.257923e-19,
    // -1.321880e+14, -1.815888e-12, 9.366926e-09, 1.024923e-14, 4.494907e+04, -2.596971e-20,
    // -3.403446e-12, 1.537084e+17, -3.850430e-17, -4.821759e+05, 4.255435e-20, -1.016978e-16, 1.430658e-09,
    // -3.696861e-14, -4.427905e-19, -1.999724e-09, -3.489402e-06, -4.677864e-03, 1.246884e+13,
    // -4.458271e-19, 3.551905e-04, -4.458221e-20, -3.472033e+01, -1.745714e+08, 4.396891e+03, 4.345767e+02,
    // -1.800116e+05, -1.217318e+00, 3.605072e-08, 1.306109e-09, -2.798295e+16, 4.387728e-13,
    // -3.284039e+11, 3.424124e+17 };
    RR_INTRUSIVE_PTR<RRMultiDimArray<double> > list_d5_2_1 = AllocateRRMultiDimArray<double>(is_d5_2_1a, is_d5_2_1b);
    RR_INTRUSIVE_PTR<RRList<RRMultiDimArray<double> > > list_d5_2 = AllocateEmptyRRList<RRMultiDimArray<double> >();
    list_d5_2->push_back(list_d5_2_1);
    EXPECT_NO_THROW(r->set_list_d5(list_d5_2));

    // Test list_str1

    // Test list_struct1

    RR_INTRUSIVE_PTR<RRList<teststruct2> > list_struct1_1 = r->get_list_struct1();
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > list_struct1_1a =
        GetTestRRArray<double>("ts1.is_struct1.to_client");
    EXPECT_RRARRAY_EQ(list_struct1_1->front()->mydat, list_struct1_1a);

    RR_INTRUSIVE_PTR<RRList<teststruct2> > list_struct1_2 = AllocateEmptyRRList<teststruct2>();
    RR_INTRUSIVE_PTR<teststruct2> list_struct1_2_1(new teststruct2());
    // double list_struct1_2_1a[]={ -2.101948e-07, -2.594836e-08, 2.515710e+01, -3.834127e-14, -3.088095e+06,
    // -3.256612e-02, -1.855481e-19, 3.801916e+07, 2.145894e+09, 4.487676e+12, 1.351202e-02,
    // -1.125124e-16, 1.369826e-20, -2.290673e+00, 1.786029e-20, -4.991515e+08, 4.006107e-10, -4.947871e-11,
    // -2.737020e-08, 4.123759e-20 };
    list_struct1_2_1->mydat = is_struct1_2_1a;
    list_struct1_2->push_back(list_struct1_2_1);
    EXPECT_NO_THROW(r->set_list_struct1(list_struct1_2));

    // Test struct3
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > struct3a = GetTestRRArray<double>("ts1.struct3.to_client");
    EXPECT_RRARRAY_EQ(r->get_struct3()->a1, struct3a);

    RR_INTRUSIVE_PTR<ostruct2> struct3_1(new ostruct2());
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > struct3_1a = GetTestRRArray<double>("ts1.struct3.to_service");
    struct3_1->a1 = struct3_1a;
    EXPECT_NO_THROW(r->set_struct3(struct3_1));

    // Test var1  TODO...

    // Test var2 TODO...

    // Test var_num
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<int32_t> > var_numa = GetTestRRArray<int32_t>("ts1.var_num.to_client");
    EXPECT_RRARRAY_EQ(rr_cast<RRArray<int32_t> >(r->get_var_num()), var_numa);
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<int32_t> > var_numb = GetTestRRArray<int32_t>("ts1.var_num.to_service");
    EXPECT_NO_THROW(r->set_var_num(var_numb));

    // Test var_str
    EXPECT_NO_THROW(EXPECT_EQ(RRArrayToString(rr_cast<RRArray<char> >(r->get_var_str())), "Hello Client!"));
    EXPECT_NO_THROW(r->set_var_str(stringToRRArray("Hello Server!")));

    // Test var_struct

    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > var_structa = GetTestRRArray<double>("ts1.var_struct.to_client");
    EXPECT_RRARRAY_EQ(rr_cast<teststruct2>(r->get_var_struct())->mydat, var_structa);

    RR_INTRUSIVE_PTR<teststruct2> var_struct_1(new teststruct2());
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > var_struct1_1a =
        GetTestRRArray<double>("ts1.var_struct.to_service");
    var_struct_1->mydat = var_struct1_1a;
    EXPECT_NO_THROW(r->set_var_struct(var_struct_1));
    // TODO: add in all the other test values

    // Test var_vector
    EXPECT_NO_THROW(EXPECT_EQ(
        RRArrayToString(rr_cast<RRArray<char> >(rr_cast<RRMap<int32_t, RRValue> >(r->get_var_vector())->at(10))),
        "Hello Client!"));
    RR_INTRUSIVE_PTR<RRMap<int32_t, RRValue> > var_vector_1 = AllocateEmptyRRMap<int32_t, RRValue>();
    var_vector_1->insert(make_pair(11, stringToRRArray("Hello Server!")));
    EXPECT_NO_THROW(r->set_var_vector(var_vector_1));

    // Test var_dictionary
    EXPECT_NO_THROW(EXPECT_EQ(RRArrayToString(rr_cast<RRArray<char> >(
                                  rr_cast<RRMap<string, RRValue> >(r->get_var_dictionary())->at("test1"))),
                              "Hello Client!"));
    RR_INTRUSIVE_PTR<RRMap<string, RRValue> > var_dictionary_1 = AllocateEmptyRRMap<string, RRValue>();
    var_dictionary_1->insert(make_pair("test2", stringToRRArray("Hello Server!")));
    EXPECT_NO_THROW(r->set_var_dictionary(var_dictionary_1));

    // Test var_list
    EXPECT_NO_THROW(
        EXPECT_EQ(RRArrayToString(rr_cast<RRArray<char> >(rr_cast<RRList<RRValue> >(r->get_var_list())->front())),
                  "Hello Client!"));
    RR_INTRUSIVE_PTR<RRList<RRValue> > var_list_1 = AllocateEmptyRRList<RRValue>();
    var_list_1->push_back(stringToRRArray("Hello Server!"));
    EXPECT_NO_THROW(r->set_var_list(var_list_1));

    // Test var_multidimarray
    RR_INTRUSIVE_PTR<RRMultiDimArray<double> > var_a_1 = rr_cast<RRMultiDimArray<double> >(r->get_var_multidimarray());
    uint32_t var_a_1a[] = {5, 4};
    EXPECT_RRARRAY_EQ(var_a_1->Dims, AttachRRArray(var_a_1a, 2, false));
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > var_a_1b =
        GetTestRRArray<double>("ts1.var_multidimarray.to_client");
    EXPECT_RRARRAY_EQ(var_a_1->Array, var_a_1b);
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<uint32_t> > var_a_2a =
        GetTestRRArray<uint32_t>("ts1.var_multidimarray.to_service.dims");
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > var_a_2b =
        GetTestRRArray<double>("ts1.var_multidimarray.to_service");
    EXPECT_NO_THROW(r->set_var_multidimarray(AllocateRRMultiDimArray<double>(var_a_2a, var_a_2b)));

    // Test errtest and make sure an std::exception is thrown

    EXPECT_THROW(r->set_errtest(1), RobotRaconteurException);
    EXPECT_THROW(r->get_errtest(), RobotRaconteurException);

    // Test nulltest
    EXPECT_NO_THROW(EXPECT_FALSE(r->get_nulltest()));
    EXPECT_NO_THROW(r->set_nulltest(RR_INTRUSIVE_PTR<teststruct1>()));
}

void ServiceTestClient::TestFunctions()
{
    EXPECT_NO_THROW(r->func1());
    EXPECT_NO_THROW(r->func2(10, 20.34));
    EXPECT_NO_THROW(EXPECT_EQ(r->func3(2, 3.45), 5.45));
    EXPECT_NO_THROW(EXPECT_EQ(r->meaning_of_life(), 42));

    EXPECT_THROW(r->func_errtest(), RobotRaconteurRemoteException);
    EXPECT_THROW(r->func_errtest1(), DataTypeException);
    EXPECT_THROW(r->func_errtest2(), testexception1);
    EXPECT_THROW(r->func_errtest3(), testexception3);
}

void ServiceTestClient::TestEvents()
{

    boost::signals2::connection c1 = r->get_ev1().connect(boost::bind(&ServiceTestClient::ev1_cb, this));
    boost::signals2::connection c2 = r->get_ev2().connect(
        boost::bind(&ServiceTestClient::ev2_cb, this, RR_BOOST_PLACEHOLDERS(_1), RR_BOOST_PLACEHOLDERS(_2)));

    EXPECT_NO_THROW(r->func1());
    EXPECT_NO_THROW(r->func2(27.3, 98.23));

    EXPECT_TRUE(ev1_event.WaitOne(2000));
    EXPECT_TRUE(ev2_event.WaitOne(2000));

    c1.disconnect();
    c2.disconnect();
    r->get_ev1().empty();
    r->get_ev2().empty();
}

void ServiceTestClient::ev1_cb() { ev1_event.Set(); }

void ServiceTestClient::ev2_cb(double d, RR_INTRUSIVE_PTR<teststruct2> s)
{
    if (d == 27.3 && (*s->mydat)[0] == 98.23)
    {
        ev2_event.Set();
    }
}

#define ShouldBeErr_Timeout(COMMAND, TIMEOUT)                                                                          \
    {                                                                                                                  \
        boost::posix_time::ptime should_be_err_timeout_start = boost::posix_time::microsec_clock::universal_time();    \
        boost::posix_time::ptime should_be_err_timeout_end =                                                           \
            should_be_err_timeout_start + boost::posix_time::milliseconds(TIMEOUT);                                    \
        while (true)                                                                                                   \
        {                                                                                                              \
            bool err = false;                                                                                          \
            try                                                                                                        \
            {                                                                                                          \
                COMMAND                                                                                                \
            }                                                                                                          \
            catch (std::exception&)                                                                                    \
            {                                                                                                          \
                err = true;                                                                                            \
                break;                                                                                                 \
            };                                                                                                         \
            if (err == false && boost::posix_time::microsec_clock::universal_time() > should_be_err_timeout_end)       \
                FAIL();                                                                                                \
        }                                                                                                              \
    }

#define ShouldNotBeErr_Timeout(COMMAND, TIMEOUT)                                                                       \
    {                                                                                                                  \
        boost::posix_time::ptime should_be_err_timeout_start = boost::posix_time::microsec_clock::universal_time();    \
        boost::posix_time::ptime should_be_err_timeout_end =                                                           \
            should_be_err_timeout_start + boost::posix_time::milliseconds(TIMEOUT);                                    \
        while (true)                                                                                                   \
        {                                                                                                              \
            try                                                                                                        \
            {                                                                                                          \
                COMMAND;                                                                                               \
                break;                                                                                                 \
            }                                                                                                          \
            catch (std::exception&)                                                                                    \
            {}                                                                                                         \
            if (boost::posix_time::microsec_clock::universal_time() > should_be_err_timeout_end)                       \
            {                                                                                                          \
                EXPECT_NO_THROW(COMMAND);                                                                              \
                break;                                                                                                 \
            }                                                                                                          \
        }                                                                                                              \
    }

void ServiceTestClient::TestObjRefs()
{
    RR_SHARED_PTR<sub1> o1;
    RR_SHARED_PTR<sub1> o2_10;
    RR_SHARED_PTR<sub1> o2_34;
    RR_SHARED_PTR<sub1> o3_1;
    RR_SHARED_PTR<sub1> o4_myind;
    RR_SHARED_PTR<sub1> o4_specialind;

    RR_SHARED_PTR<sub2> o2_10_o2_1_32;

    RR_SHARED_PTR<sub3> o2_10_o2_1_32_o3_ind1;
    RR_SHARED_PTR<sub3> o2_10_o2_1_32_o3_ind2;

    EXPECT_NO_THROW(o1 = r->get_o1());
    EXPECT_NO_THROW(o2_10 = r->get_o2(10));
    EXPECT_NO_THROW(o2_34 = r->get_o2(34));
    EXPECT_NO_THROW(o3_1 = r->get_o3(1));
    EXPECT_NO_THROW(o4_myind = r->get_o4("myind"));
    EXPECT_NO_THROW(o4_specialind = r->get_o4("ind!@#$%^&*().<>     "));

    EXPECT_NO_THROW(o2_10_o2_1_32 = o2_10->get_o2_2(32));

    EXPECT_NO_THROW(o2_10_o2_1_32_o3_ind1 = o2_10_o2_1_32->get_o3_1("ind1"));
    EXPECT_NO_THROW(o2_10_o2_1_32_o3_ind2 = o2_10_o2_1_32->get_o3_1("ind2"));

    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > o_d1a = GetTestRRArray<double>("ts1.client.o_d1a");
    EXPECT_NO_THROW(o1->set_d1(o_d1a));
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > o_d1b = GetTestRRArray<double>("ts1.client.o_d1b");
    EXPECT_NO_THROW(o2_10->set_d1(o_d1b));
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > o_d1c = GetTestRRArray<double>("ts1.client.o_d1c");
    EXPECT_NO_THROW(o2_34->set_d1(o_d1c));
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > o_d1d = GetTestRRArray<double>("ts1.client.o_d1d");
    EXPECT_NO_THROW(o3_1->set_d1(o_d1d));
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > o_d1e = GetTestRRArray<double>("ts1.client.o_d1e");
    EXPECT_NO_THROW(o4_myind->set_d1(o_d1e));
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > o_d1f = GetTestRRArray<double>("ts1.client.o_d1f");
    EXPECT_NO_THROW(o4_specialind->set_d1(o_d1f));
    EXPECT_NO_THROW(o2_10_o2_1_32->set_data("Hello world!"));
    EXPECT_NO_THROW(o2_10_o2_1_32_o3_ind1->set_data2("Test string 1"));
    EXPECT_NO_THROW(o2_10_o2_1_32_o3_ind2->set_data2("Test string 2"));
    EXPECT_EQ(o2_10_o2_1_32_o3_ind2->add(10), 52);

    // Test the varobject type and make sure the ReleaseServicePath works...

    RR_SHARED_PTR<sub1> o6_1;
    RR_SHARED_PTR<sub2> o6_1_1;

    EXPECT_NO_THROW(r->o6_op(0));
    ShouldNotBeErr_Timeout(
        {
            o6_1 = rr_cast<sub1>(r->get_o6());
            o6_1_1 = o6_1->get_o2_1();
            o6_1->set_d1(ScalarToRRArray(0.0));
            o6_1_1->set_data("Hello world!");
        },
        500);

    EXPECT_NO_THROW(r->o6_op(1));
    ShouldBeErr_Timeout({ o6_1->set_d1(ScalarToRRArray(0.0)); }, 1000);
    ShouldBeErr_Timeout({ o6_1_1->set_data("Hello world!"); }, 1000);

    RR_SHARED_PTR<sub2> o6_2 = rr_cast<sub2>(r->get_o6());
    EXPECT_NO_THROW(o6_2->set_data("Hello world!"));

    EXPECT_NO_THROW(r->o6_op(2));
    ShouldBeErr_Timeout({ o6_2->set_data("Hello world!"); }, 1000);

    RR_SHARED_PTR<com::robotraconteur::testing::TestService2::subobj> o6_3 =
        rr_cast<com::robotraconteur::testing::TestService2::subobj>(r->get_o6());
    EXPECT_NO_THROW(o6_3->add_val(2));
}

void ServiceTestClient::TestCallbacks()
{
    cb1_called = false;
    cb2_called = false;
    cb3_called = false;
    cb4_called = false;
    cb5_called = false;

    r->get_cb1()->SetFunction(boost::bind(&ServiceTestClient::cb1_func, this));
    r->get_cb2()->SetFunction(
        boost::bind(&ServiceTestClient::cb2_func, this, RR_BOOST_PLACEHOLDERS(_1), RR_BOOST_PLACEHOLDERS(_2)));
    r->get_cb3()->SetFunction(
        boost::bind(&ServiceTestClient::cb3_func, this, RR_BOOST_PLACEHOLDERS(_1), RR_BOOST_PLACEHOLDERS(_2)));
    r->get_cb_meaning_of_life()->SetFunction(boost::bind(&ServiceTestClient::cb_meaning_of_life_func, this));
    r->get_cb_errtest()->SetFunction(boost::bind(&ServiceTestClient::cb_errtest, this));

    EXPECT_NO_THROW(r->test_callbacks());

    EXPECT_FALSE(!cb1_called || !cb2_called || !cb3_called || !cb4_called || !cb5_called);
}

void ServiceTestClient::cb1_func() { cb1_called = true; }

void ServiceTestClient::cb2_func(double d1, double d2)
{
    if (d1 != 739.2 || d2 != 0.392)
        throw std::exception();
    cb2_called = true;
}

double ServiceTestClient::cb3_func(double d1, double d2)
{
    cb3_called = true;
    return d1 + d2 + 3.14;
}

int ServiceTestClient::cb_meaning_of_life_func()
{
    cb4_called = true;
    return 42;
}

void ServiceTestClient::cb_errtest()
{
    cb5_called = true;
    throw runtime_error("This is a test");
}

void ServiceTestClient::TestPipes()
{
    RR_SHARED_PTR<PipeEndpoint<RR_INTRUSIVE_PTR<RRArray<double> > > > e1;
    RR_SHARED_PTR<PipeEndpoint<RR_INTRUSIVE_PTR<RRArray<double> > > > e2;
    RR_SHARED_PTR<PipeEndpoint<RR_INTRUSIVE_PTR<teststruct2> > > e3;

    ASSERT_NO_THROW(e1 = r->get_p1()->Connect(-1));
    ASSERT_NO_THROW(e2 = r->get_p1()->Connect(3432));
    ASSERT_NO_THROW(e3 = r->get_p2()->Connect(-1));

    e1->SetRequestPacketAck(true);

    boost::signals2::connection ec1 =
        e1->PacketReceivedEvent.connect(boost::bind(&ServiceTestClient::ee1_cb, this, RR_BOOST_PLACEHOLDERS(_1)));
    boost::signals2::connection ec2 = e1->PacketAckReceivedEvent.connect(
        boost::bind(&ServiceTestClient::ee1_ack_cb, this, RR_BOOST_PLACEHOLDERS(_1), RR_BOOST_PLACEHOLDERS(_2)));
    boost::signals2::connection ec3 =
        e2->PacketReceivedEvent.connect(boost::bind(&ServiceTestClient::ee2_cb, this, RR_BOOST_PLACEHOLDERS(_1)));
    boost::signals2::connection ec4 =
        e3->PacketReceivedEvent.connect(boost::bind(&ServiceTestClient::ee3_cb, this, RR_BOOST_PLACEHOLDERS(_1)));

    double d1[] = {1, 2, 3, 4};
    double d2[] = {5, 6, 7, 8};
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > d3 = GetTestRRArray<double>("ts1.client.d3");
    double d4[] = {3.21};
    double d5[] = {4.72};
    double d6[] = {72.34};
    double d7[] = {738.29};
    double d8[] = {89.83};
    {
        boost::mutex::scoped_lock lock(pipe_ack_lock);
        EXPECT_NO_THROW(packetnum = e1->SendPacket(AttachRRArray(d1, 4, false)));
    }
    EXPECT_NO_THROW(e1->SendPacket(AttachRRArray(d2, 4, false)));
    EXPECT_NO_THROW(e1->SendPacket(d3));

    EXPECT_NO_THROW(e2->SendPacket(AttachRRArray(d4, 1, false)));
    EXPECT_NO_THROW(e2->SendPacket(AttachRRArray(d5, 1, false)));
    EXPECT_NO_THROW(e2->SendPacket(AttachRRArray(d6, 1, false)));

    RR_INTRUSIVE_PTR<teststruct2> s1(new teststruct2());
    s1->mydat = AttachRRArray(d7, 1, false);
    RR_INTRUSIVE_PTR<teststruct2> s2(new teststruct2());
    s2->mydat = AttachRRArray(d8, 1, false);

    EXPECT_NO_THROW(e3->SendPacket(s1));
    EXPECT_NO_THROW(e3->SendPacket(s2));

    EXPECT_RRARRAY_EQ(e1->ReceivePacketWait(5000), AttachRRArray(d1, 4, false));
    EXPECT_RRARRAY_EQ(e1->ReceivePacketWait(5000), AttachRRArray(d2, 4, false));

    RR_INTRUSIVE_PTR<RRArray<double> > e1_3_1;
    RR_INTRUSIVE_PTR<RRArray<double> > e1_3_2;
    EXPECT_TRUE(e1->TryReceivePacketWait(e1_3_1, 5000, true));
    EXPECT_TRUE(e1->TryReceivePacketWait(e1_3_2, 5000, true));

    EXPECT_RRARRAY_EQ(e1_3_1, d3);
    EXPECT_RRARRAY_EQ(e1_3_2, d3);

    EXPECT_TRUE(ee2.WaitOne(5000));
    EXPECT_TRUE(ee3.WaitOne(5000));

    EXPECT_RRARRAY_EQ(e2->ReceivePacket(), AttachRRArray(d4, 1, false));
    EXPECT_RRARRAY_EQ(e2->ReceivePacketWait(100), AttachRRArray(d5, 1, false));
    EXPECT_RRARRAY_EQ(e2->ReceivePacketWait(100), AttachRRArray(d6, 1, false));

    EXPECT_RRARRAY_EQ(e3->ReceivePacket()->mydat, AttachRRArray(d7, 1, false));
    EXPECT_RRARRAY_EQ(e3->ReceivePacketWait(100)->mydat, AttachRRArray(d8, 1, false));

    ShouldNotBeErr_Timeout({ r->pipe_check_error(); }, 1000);

    EXPECT_NO_THROW(e1->Close());
    // e2->Close();
    EXPECT_NO_THROW(e3->Close());

    ec1.disconnect();
    ec2.disconnect();
    ec3.disconnect();
    ec4.disconnect();
}

void ServiceTestClient::ee1_cb(RR_SHARED_PTR<PipeEndpoint<RR_INTRUSIVE_PTR<RRArray<double> > > > p) { ee1.Set(); }

void ServiceTestClient::ee1_ack_cb(RR_SHARED_PTR<PipeEndpoint<RR_INTRUSIVE_PTR<RRArray<double> > > > p,
                                   uint32_t packetnum)
{
    boost::mutex::scoped_lock lock(pipe_ack_lock);
    if (packetnum == this->packetnum)
        ack_recv = true;
}

void ServiceTestClient::ee2_cb(RR_SHARED_PTR<PipeEndpoint<RR_INTRUSIVE_PTR<RRArray<double> > > > p) { ee2.Set(); }

void ServiceTestClient::ee3_cb(RR_SHARED_PTR<PipeEndpoint<RR_INTRUSIVE_PTR<teststruct2> > > p) { ee3.Set(); }

void ServiceTestClient::TestWires()
{
    w1_called = false;
    w2_called = false;
    w3_called = false;

    RR_SHARED_PTR<WireConnection<RR_INTRUSIVE_PTR<RRArray<double> > > > w1;
    RR_SHARED_PTR<WireConnection<RR_INTRUSIVE_PTR<teststruct2> > > w2;
    RR_SHARED_PTR<WireConnection<RR_INTRUSIVE_PTR<RRMultiDimArray<int32_t> > > > w3;

    ASSERT_NO_THROW(w1 = r->get_w1()->Connect());
    ASSERT_NO_THROW(w2 = r->get_w2()->Connect());
    ASSERT_NO_THROW(w3 = r->get_w3()->Connect());

    w1->WireValueChanged.connect(boost::bind(&ServiceTestClient::w1_changed, this, RR_BOOST_PLACEHOLDERS(_1),
                                             RR_BOOST_PLACEHOLDERS(_2), RR_BOOST_PLACEHOLDERS(_3)));
    w2->WireValueChanged.connect(boost::bind(&ServiceTestClient::w2_changed, this, RR_BOOST_PLACEHOLDERS(_1),
                                             RR_BOOST_PLACEHOLDERS(_2), RR_BOOST_PLACEHOLDERS(_3)));
    w3->WireValueChanged.connect(boost::bind(&ServiceTestClient::w3_changed, this, RR_BOOST_PLACEHOLDERS(_1),
                                             RR_BOOST_PLACEHOLDERS(_2), RR_BOOST_PLACEHOLDERS(_3)));

    w1->SetInValueLifespan(10000);

    double d0[] = {0.0};
    EXPECT_NO_THROW(w1->SetOutValue(AttachRRArrayCopy(d0, 1)));

    RR_INTRUSIVE_PTR<teststruct2> s1(new teststruct2());
    s1->mydat = AttachRRArrayCopy(d0, 1);
    EXPECT_NO_THROW(w2->SetOutValue(s1));

    int32_t i0[] = {0};
    uint32_t i1[] = {1, 1};
    EXPECT_NO_THROW(
        w3->SetOutValue(AllocateRRMultiDimArray<int32_t>(AttachRRArrayCopy(i1, 2), AttachRRArrayCopy(i0, 1))));

    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > d1 = GetTestRRArray<double>("ts1.client.d1");
    EXPECT_NO_THROW(w1->SetOutValue(d1));

    RR_INTRUSIVE_PTR<teststruct2> s2(new teststruct2());
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > d2 = GetTestRRArray<double>("ts1.client.d2");
    s2->mydat = d2;
    EXPECT_NO_THROW(w2->SetOutValue(s2));

    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<uint32_t> > i2 = GetTestRRArray<uint32_t>("ts1.client.i3.dims");
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<int32_t> > i3 = GetTestRRArray<int32_t>("ts1.client.i3");
    RR_INTRUSIVE_PTR<RRMultiDimArray<int32_t> > a2 = AllocateRRMultiDimArray<int32_t>(i2, i3);
    EXPECT_NO_THROW(w3->SetOutValue(a2));

    // boost::this_thread::sleep(boost::posix_time::milliseconds(100));

    EXPECT_NO_THROW(w1->WaitInValueValid());

    // if (!we1.WaitOne(5000)) throw std::exception();
    EXPECT_TRUE(we2.WaitOne(5000));
    EXPECT_TRUE(we3.WaitOne(5000));

    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > d3 = GetTestRRArray<double>("ts1.client.d1");

    ShouldNotBeErr_Timeout(
        {
            w1->SetOutValue(d1);
            boost::this_thread::sleep(boost::posix_time::milliseconds(5));
            RR_INTRUSIVE_PTR<RRArray<double> > in1;
            TimeSpec in1_time;
            if (!w1->TryGetInValue(in1, in1_time))
            {
                throw std::runtime_error("TryGetInValue failed");
            }
            ca(d3, in1);
        },
        1000);

    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > d4 = GetTestRRArray<double>("ts1.client.d2");
    ShouldNotBeErr_Timeout(
        {
            w2->SetOutValue(s2);
            boost::this_thread::sleep(boost::posix_time::milliseconds(5));
            RR_INTRUSIVE_PTR<teststruct2> in2 = w2->GetInValue();
            ca(in2->mydat, d4);
        },
        500);

    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<int32_t> > i4 = GetTestRRArray<int32_t>("ts1.client.i3");

    ShouldNotBeErr_Timeout(
        {
            w3->SetOutValue(a2);
            RR_INTRUSIVE_PTR<RRMultiDimArray<int32_t> > in3 = w3->GetInValue();
            ca(in3->Dims, i2);
            ca(in3->Array, i4);
        },
        500);

    EXPECT_FALSE(!w1_called || !w2_called || !w3_called);

    w1->SetInValueLifespan(1);
    boost::this_thread::sleep(boost::posix_time::milliseconds(5));
    EXPECT_THROW(w1->GetInValue();, std::exception);

    EXPECT_NO_THROW(w1->Close());
    // w2->Close();
    EXPECT_NO_THROW(w3->Close());
}

void ServiceTestClient::w1_changed(RR_SHARED_PTR<WireConnection<RR_INTRUSIVE_PTR<RRArray<double> > > > c,
                                   RR_INTRUSIVE_PTR<RRArray<double> > value, TimeSpec t)
{
    w1_called = true;
    we1.Set();
}

void ServiceTestClient::w2_changed(RR_SHARED_PTR<WireConnection<RR_INTRUSIVE_PTR<teststruct2> > > c,
                                   RR_INTRUSIVE_PTR<teststruct2> value, TimeSpec t)
{
    w2_called = true;
    we2.Set();
}

void ServiceTestClient::w3_changed(RR_SHARED_PTR<WireConnection<RR_INTRUSIVE_PTR<RRMultiDimArray<int32_t> > > > c,
                                   RR_INTRUSIVE_PTR<RRMultiDimArray<int32_t> > value, TimeSpec t)
{
    w3_called = true;
    we3.Set();
}

void ServiceTestClient::TestMemories()
{
    test_m1();
    test_m2();
    test_m3();
}

void ServiceTestClient::test_m1()
{
    EXPECT_NO_THROW(EXPECT_EQ(r->get_m1()->Length(), 100));
    RR_INTRUSIVE_PTR<RRArray<double> > m1_1 = AllocateRRArray<double>(11);
    memset(m1_1->data(), 0, 11 * sizeof(11));
    EXPECT_NO_THROW(r->get_m1()->Read(10, m1_1, 1, 10));
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > m1_1a = GetTestRRArray<double>("ts1.client.m1_1a");
    EXPECT_RRARRAY_EQ(m1_1, m1_1a);
    std::vector<double> m1_2a = GetTestVector<double>("ts1.client.m1_2a");
    RR_INTRUSIVE_PTR<RRArray<double> > m1_2 = AttachRRArray(&m1_2a[0], 12, false);
    EXPECT_NO_THROW(r->get_m1()->Write(20, m1_2, 2, 8));

    RR_INTRUSIVE_PTR<RRArray<double> > m1_3 = AllocateRRArray<double>(100);
    EXPECT_NO_THROW(r->get_m1()->Read(0, m1_3, 0, 100));

    std::vector<double> m1_4a = GetTestVector<double>("ts1.m1.initial");
    memcpy(&m1_4a[20], &m1_2a[2], 8 * sizeof(double));
    EXPECT_RRARRAY_EQ(m1_3, AttachRRArray(&m1_4a[0], 100, false));
}

void ServiceTestClient::test_m2()
{
    EXPECT_NO_THROW(EXPECT_EQ(r->get_m2()->DimCount(), 5));
    vector<uint64_t> m2_dims = r->get_m2()->Dimensions();
    uint64_t m2_dimsa[] = {10, 10, 10, 10, 10};
    EXPECT_RRVECTOR_EQ(m2_dims, vector<uint64_t>(m2_dimsa, m2_dimsa + 5));

    LFSRSeqGen rng((uint32_t)std::time(0), "multidimarray_test_double");
    RR_INTRUSIVE_PTR<RRMultiDimArray<double> > m1 =
        FillMultiDimArray<double>(rng, boost::assign::list_of(10)(10)(10)(10)(10));
    RR_INTRUSIVE_PTR<RRMultiDimArray<double> > m2 =
        FillMultiDimArray<double>(rng, boost::assign::list_of(2)(8)(8)(2)(2));
    RR_INTRUSIVE_PTR<RRMultiDimArray<double> > m2_1 =
        GetMultiDimArraySub(m2, boost::assign::list_of(0)(2)(0)(0)(0), boost::assign::list_of(1)(5)(5)(2)(1));
    RR_INTRUSIVE_PTR<RRMultiDimArray<double> > m3 = TestingCopyMultiDimArray(m1);
    SetMultiDimArraySub(m3, boost::assign::list_of(2)(2)(3)(3)(4), m2_1);
    RR_INTRUSIVE_PTR<RRMultiDimArray<double> > m4 =
        GetMultiDimArraySub(m3, boost::assign::list_of(4)(2)(2)(8)(0), boost::assign::list_of(2)(2)(1)(1)(10));
    uint32_t m5a[] = {4, 4, 4, 4, 10};
    RR_INTRUSIVE_PTR<RRMultiDimArray<double> > m5 =
        AllocateRRMultiDimArray<double>(AttachRRArray(m5a, 5, false), AllocateEmptyRRArray<double>(2560));
    SetMultiDimArraySub(m5, boost::assign::list_of(2)(1)(2)(1)(0), m4);

    uint64_t zeros[] = {0, 0, 0, 0, 0};
    vector<uint64_t> vzeros(zeros, zeros + 5);
    EXPECT_NO_THROW(r->get_m2()->Write(vzeros, m1, vzeros, RobotRaconteur::RRArrayToVector<uint64_t>(m1->Dims)));

    uint32_t zeros2[] = {0, 0, 0, 0, 0};
    uint32_t tens[] = {10, 10, 10, 10, 10};
    RR_INTRUSIVE_PTR<RRArray<uint32_t> > vzeros2 = AttachRRArrayCopy(zeros2, 5);
    RR_INTRUSIVE_PTR<RRMultiDimArray<double> > m1_2 =
        AllocateRRMultiDimArray<double>(AttachRRArray(tens, 5, false), AllocateRRArray<double>(100000));
    EXPECT_NO_THROW(r->get_m2()->Read(vzeros, m1_2, vzeros, RobotRaconteur::RRArrayToVector<uint64_t>(m1->Dims)));
    EXPECT_RRARRAY_EQ(m1->Dims, m1_2->Dims);
    EXPECT_RRARRAY_EQ(m1->Array, m1_2->Array);

    uint64_t m1a[] = {2, 2, 3, 3, 4};
    uint64_t m1b[] = {0, 2, 0, 0, 0};
    uint64_t m1c[] = {1, 5, 5, 2, 1};
    EXPECT_NO_THROW(r->get_m2()->Write(vector<uint64_t>(m1a, m1a + 5), m2, vector<uint64_t>(m1b, m1b + 5),
                                       vector<uint64_t>(m1c, m1c + 5)));

    RR_INTRUSIVE_PTR<RRMultiDimArray<double> > m2_2 =
        AllocateRRMultiDimArray<double>(AttachRRArray(tens, 5, false), AllocateRRArray<double>(100000));
    EXPECT_NO_THROW(r->get_m2()->Read(vzeros, m2_2, vzeros, RobotRaconteur::RRArrayToVector<uint64_t>(m1->Dims)));
    EXPECT_RRARRAY_EQ(m2_2->Dims, m3->Dims);
    EXPECT_RRARRAY_EQ(m2_2->Array, m3->Array);

    uint32_t m6a[] = {2, 2, 1, 1, 10};
    RR_INTRUSIVE_PTR<RRMultiDimArray<double> > m6 =
        AllocateRRMultiDimArray<double>(AttachRRArray(m6a, 5, false), AllocateRRArray<double>(40));
    uint64_t m4a[] = {4, 2, 2, 8, 0};
    uint64_t m4b[] = {0, 0, 0, 0, 0};
    uint64_t m4c[] = {2, 2, 1, 1, 10};
    EXPECT_NO_THROW(r->get_m2()->Read(vector<uint64_t>(m4a, m4a + 5), m6, vector<uint64_t>(m4b, m4b + 5),
                                      vector<uint64_t>(m4c, m4c + 5)));
    EXPECT_RRARRAY_EQ(m4->Array, m6->Array);

    uint32_t m7a[] = {4, 4, 4, 4, 10};
    RR_INTRUSIVE_PTR<RRMultiDimArray<double> > m7 =
        AllocateRRMultiDimArray<double>(AttachRRArray(m7a, 5, false), AllocateRRArray<double>(2560));
    memset(m7->Array->data(), 0, 2560 * sizeof(double));
    uint64_t m5a1[] = {4, 2, 2, 8, 0};
    uint64_t m5b[] = {2, 1, 2, 1, 0};
    uint64_t m5c[] = {2, 2, 1, 1, 10};
    EXPECT_NO_THROW(r->get_m2()->Read(vector<uint64_t>(m5a1, m5a1 + 5), m7, vector<uint64_t>(m5b, m5b + 5),
                                      vector<uint64_t>(m5c, m5c + 5)));
    EXPECT_RRARRAY_EQ(m5->Array, m7->Array);
}

void ServiceTestClient::test_m3()
{
    LFSRSeqGen rng((uint32_t)std::time(0), "multidimarray_test_byte");
    RR_INTRUSIVE_PTR<RRMultiDimArray<uint8_t> > m1 =
        FillMultiDimArray<uint8_t>(rng, boost::assign::list_of(1024)(1024));
    RR_INTRUSIVE_PTR<RRMultiDimArray<uint8_t> > m2 = FillMultiDimArray<uint8_t>(rng, boost::assign::list_of(450)(450));
    RR_INTRUSIVE_PTR<RRMultiDimArray<uint8_t> > m2_1 =
        GetMultiDimArraySub(m2, boost::assign::list_of(20)(25), boost::assign::list_of(200)(200));
    RR_INTRUSIVE_PTR<RRMultiDimArray<uint8_t> > m3 = TestingCopyMultiDimArray(m1);
    SetMultiDimArraySub(m3, boost::assign::list_of(50)(100), m2_1);
    RR_INTRUSIVE_PTR<RRMultiDimArray<uint8_t> > m4 =
        GetMultiDimArraySub(m3, boost::assign::list_of(65)(800), boost::assign::list_of(200)(200));
    uint32_t m5a[] = {512, 512};
    RR_INTRUSIVE_PTR<RRMultiDimArray<uint8_t> > m5 =
        AllocateRRMultiDimArray<uint8_t>(AttachRRArray(m5a, 2, false), AllocateEmptyRRArray<uint8_t>(512 * 512));
    SetMultiDimArraySub(m5, boost::assign::list_of(100)(230), m4);

    uint64_t zeros[] = {0, 0};
    vector<uint64_t> vzeros(zeros, zeros + 2);
    EXPECT_NO_THROW(r->get_m3()->Write(vzeros, m1, vzeros, RobotRaconteur::RRArrayToVector<uint64_t>(m1->Dims)));

    int32_t zeros2[] = {0, 0};
    uint32_t tens[] = {1024, 1024};
    RR_INTRUSIVE_PTR<RRArray<int32_t> > vzeros2 = AttachRRArrayCopy(zeros2, 2);
    RR_INTRUSIVE_PTR<RRMultiDimArray<uint8_t> > m1_2 =
        AllocateRRMultiDimArray<uint8_t>(AttachRRArray(tens, 2, false), AllocateRRArray<uint8_t>(1024 * 1024));
    EXPECT_NO_THROW(r->get_m3()->Read(vzeros, m1_2, vzeros, RobotRaconteur::RRArrayToVector<uint64_t>(m1->Dims)));
    EXPECT_RRARRAY_EQ(m1->Dims, m1_2->Dims);
    EXPECT_RRARRAY_EQ(m1->Array, m1_2->Array);

    uint64_t m1a[] = {50, 100};
    uint64_t m1b[] = {20, 25};
    uint64_t m1c[] = {200, 200};
    EXPECT_NO_THROW(r->get_m3()->Write(vector<uint64_t>(m1a, m1a + 2), m2, vector<uint64_t>(m1b, m1b + 2),
                                       vector<uint64_t>(m1c, m1c + 2)));

    RR_INTRUSIVE_PTR<RRMultiDimArray<uint8_t> > m2_2 =
        AllocateRRMultiDimArray<uint8_t>(AttachRRArray(tens, 2, false), AllocateRRArray<uint8_t>(1024 * 1024));
    EXPECT_NO_THROW(r->get_m3()->Read(vzeros, m2_2, vzeros, RobotRaconteur::RRArrayToVector<uint64_t>(m1->Dims)));
    EXPECT_RRARRAY_EQ(m2_2->Dims, m3->Dims);
    EXPECT_RRARRAY_EQ(m2_2->Array, m3->Array);

    uint32_t m6a[] = {200, 200};
    RR_INTRUSIVE_PTR<RRMultiDimArray<uint8_t> > m6 =
        AllocateRRMultiDimArray<uint8_t>(AttachRRArray(m6a, 2, false), AllocateRRArray<uint8_t>(200 * 200));
    uint64_t m4a[] = {65, 800};
    uint64_t m4b[] = {0, 0};
    uint64_t m4c[] = {200, 200};
    EXPECT_NO_THROW(r->get_m3()->Read(vector<uint64_t>(m4a, m4a + 2), m6, vector<uint64_t>(m4b, m4b + 2),
                                      vector<uint64_t>(m4c, m4c + 2)));
    EXPECT_RRARRAY_EQ(m4->Array, m6->Array);

    uint32_t m7a[] = {512, 512};
    RR_INTRUSIVE_PTR<RRMultiDimArray<uint8_t> > m7 =
        AllocateRRMultiDimArray<uint8_t>(AttachRRArray(m7a, 2, false), AllocateRRArray<uint8_t>(512 * 512));
    memset(m7->Array->data(), 0, 512 * 512);

    uint64_t m5a1[] = {65, 800};
    uint64_t m5b[] = {100, 230};
    uint64_t m5c[] = {200, 200};
    EXPECT_NO_THROW(r->get_m3()->Read(vector<uint64_t>(m5a1, m5a1 + 2), m7, vector<uint64_t>(m5b, m5b + 2),
                                      vector<uint64_t>(m5c, m5c + 2)));
    EXPECT_RRARRAY_EQ(m5->Array, m7->Array);
}

void ServiceTestClient::DisconnectService() { RobotRaconteurNode::s()->DisconnectService(r); }

void ServiceTestClient::TestAuthentication(string url)
{
    RR_INTRUSIVE_PTR<RRMap<string, RRValue> > cred1 = AllocateEmptyRRMap<string, RRValue>();
    cred1->insert(make_pair("password", stringToRRArray("testpass1")));
    RR_SHARED_PTR<testroot> r1 = rr_cast<testroot>(RobotRaconteurNode::s()->ConnectService(url, "testuser1", cred1));
    r1->func3(2.2, 3.3);
    RobotRaconteurNode::s()->DisconnectService(r1);

    RR_INTRUSIVE_PTR<RRMap<string, RRValue> > cred2 = AllocateEmptyRRMap<string, RRValue>();
    cred2->insert(make_pair("password", stringToRRArray("testpass2")));
    RR_SHARED_PTR<testroot> r2 = rr_cast<testroot>(RobotRaconteurNode::s()->ConnectService(url, "testuser2", cred2));
    r2->func3(2.2, 3.3);
    RobotRaconteurNode::s()->DisconnectService(r2);

    bool err = false;
    RR_SHARED_PTR<testroot> r3;
    try
    {
        r3 = rr_cast<testroot>(RobotRaconteurNode::s()->ConnectService(url, "testuser2", cred1));
        r3->func3(2.2, 3.3);
    }
    catch (AuthenticationException&)
    // catch (std::exception& e)
    {
        err = true;
        try
        {
            RobotRaconteurNode::s()->DisconnectService(r3);
        }
        catch (std::exception&)
        {}
    }

    EXPECT_TRUE(err);

    RR_SHARED_PTR<testroot> r4;
    try
    {
        r4 = rr_cast<testroot>(RobotRaconteurNode::s()->ConnectService(url));
        r4->func3(2.2, 3.3);
    }
    catch (AuthenticationException&)
    {
        err = true;
        try
        {
            RobotRaconteurNode::s()->DisconnectService(r4);
        }
        catch (std::exception&)
        {}
    }

    EXPECT_TRUE(err);
}

void ServiceTestClient::TestObjectLock(string url)
{

    {
        // Unlock all objects before continuing
        RR_INTRUSIVE_PTR<RRMap<string, RRValue> > cred6 = AllocateEmptyRRMap<string, RRValue>();
        cred6->insert(make_pair("password", stringToRRArray("superpass1")));
        RR_SHARED_PTR<testroot> r6 =
            rr_cast<testroot>(RobotRaconteurNode::s()->ConnectService(url, "testsuperuser", cred6));
        RR_SHARED_PTR<sub1> r6_o = r6->get_o1();
        EXPECT_NO_THROW(RobotRaconteurNode::s()->ReleaseObjectLock(r6_o));
    }

    RR_INTRUSIVE_PTR<RRMap<string, RRValue> > cred1 = AllocateEmptyRRMap<string, RRValue>();
    cred1->insert(make_pair("password", stringToRRArray("testpass1")));
    RR_SHARED_PTR<testroot> r1 = rr_cast<testroot>(RobotRaconteurNode::s()->ConnectService(url, "testuser1", cred1));

    RR_INTRUSIVE_PTR<RRMap<string, RRValue> > cred2 = AllocateEmptyRRMap<string, RRValue>();
    cred2->insert(make_pair("password", stringToRRArray("testpass2")));
    RR_SHARED_PTR<testroot> r2 = rr_cast<testroot>(RobotRaconteurNode::s()->ConnectService(url, "testuser1", cred1));

    RR_SHARED_PTR<sub1> r1_o;
    RR_SHARED_PTR<sub2> r1_o_o2;
    RR_SHARED_PTR<sub1> r2_o;
    RR_SHARED_PTR<sub2> r2_o_o2;

    RR_SHARED_PTR<testroot> r3;

    RR_SHARED_PTR<sub1> r3_o;
    RR_SHARED_PTR<sub2> r3_o_o2;

    ASSERT_NO_THROW(r1_o = r1->get_o1());
    ASSERT_NO_THROW(r1_o_o2 = r1_o->get_o2_1());
    ASSERT_NO_THROW(r2_o = r2->get_o1());
    ASSERT_NO_THROW(r2_o_o2 = r2_o->get_o2_1());

    ASSERT_NO_THROW(r3 = rr_cast<testroot>(RobotRaconteurNode::s()->ConnectService(url, "testuser2", cred2)));

    ASSERT_NO_THROW(r3_o = r3->get_o1());
    ASSERT_NO_THROW(r3_o_o2 = r3_o->get_o2_1());

    EXPECT_NO_THROW(r1->func3(2.2, 3.3));

    double d1[] = {1.0};
    RR_INTRUSIVE_PTR<RRArray<double> > d = AttachRRArray(d1, 1, false);
    EXPECT_NO_THROW(r1_o->set_d1(d));
    EXPECT_NO_THROW(r1_o_o2->set_data("Hello world"));

    EXPECT_NO_THROW(r2->func3(2.2, 3.3));
    EXPECT_NO_THROW(r2_o->set_d1(d));
    EXPECT_NO_THROW(r2_o_o2->set_data("Hello world"));

    EXPECT_NO_THROW(r3->func3(2.2, 3.3));
    EXPECT_NO_THROW(r3_o->set_d1(d));
    EXPECT_NO_THROW(r3_o_o2->set_data("Hello world"));

    // Lock the object by username
    EXPECT_NO_THROW(RobotRaconteurNode::s()->RequestObjectLock(r1_o, RobotRaconteurObjectLockFlags_USER_LOCK));

    // Check that all users that should access the objects can
    EXPECT_NO_THROW(r1->func3(2.2, 3.3));
    EXPECT_NO_THROW(r1_o->set_d1(d));
    EXPECT_NO_THROW(r1_o_o2->set_data("Hello world"));

    EXPECT_NO_THROW(r2->func3(2.2, 3.3));
    EXPECT_NO_THROW(r2_o->set_d1(d));
    EXPECT_NO_THROW(r2_o_o2->set_data("Hello world"));

    EXPECT_NO_THROW(r3->func3(2.2, 3.3));

    // Check that objects that shouldn't be able to access the objects can't
    EXPECT_THROW(r3_o->set_d1(d), std::exception);
    EXPECT_THROW(r3_o_o2->set_data("Hello world"), std::exception);

    // Unlock and recheck all
    EXPECT_NO_THROW(RobotRaconteurNode::s()->ReleaseObjectLock(r1_o));

    EXPECT_NO_THROW(r1->func3(2.2, 3.3));
    EXPECT_NO_THROW(r1_o->set_d1(d));
    EXPECT_NO_THROW(r1_o_o2->set_data("Hello world"));

    EXPECT_NO_THROW(r2->func3(2.2, 3.3));
    EXPECT_NO_THROW(r2_o->set_d1(d));
    EXPECT_NO_THROW(r2_o_o2->set_data("Hello world"));

    EXPECT_NO_THROW(r3->func3(2.2, 3.3));
    EXPECT_NO_THROW(r3_o->set_d1(d));
    EXPECT_NO_THROW(r3_o_o2->set_data("Hello world"));

    // Relock, test that the lock is active, and then close the connection.  The lock should release.  The
    // second session is closed first, and should not release the lock.
    EXPECT_NO_THROW(RobotRaconteurNode::s()->RequestObjectLock(r1_o, RobotRaconteurObjectLockFlags_USER_LOCK));

    EXPECT_THROW(r3_o->set_d1(d), std::exception);

    EXPECT_NO_THROW(r2_o->set_d1(d));
    EXPECT_NO_THROW(RobotRaconteurNode::s()->DisconnectService(r2));

    // Object still should be locked
    EXPECT_THROW(r3_o->set_d1(d), std::exception);

    // Now close the session and lock should be released
    EXPECT_NO_THROW(RobotRaconteurNode::s()->DisconnectService(r1));
    EXPECT_NO_THROW(r3_o->set_d1(d));

    // Reconnect the first two test sessions
    EXPECT_NO_THROW(r1 = rr_cast<testroot>(RobotRaconteurNode::s()->ConnectService(url, "testuser1", cred1)));
    EXPECT_NO_THROW(r2 = rr_cast<testroot>(RobotRaconteurNode::s()->ConnectService(url, "testuser1", cred1)));
    EXPECT_NO_THROW(r1_o = r1->get_o1());
    EXPECT_NO_THROW(r1_o_o2 = r1_o->get_o2_1());
    EXPECT_NO_THROW(r2_o = r2->get_o1());
    EXPECT_NO_THROW(r2_o_o2 = r2_o->get_o2_1());

    // Test the exclusive client lock
    EXPECT_NO_THROW(RobotRaconteurNode::s()->RequestObjectLock(r1_o, RobotRaconteurObjectLockFlags_CLIENT_LOCK));
    EXPECT_NO_THROW(r1_o->set_d1(d));
    EXPECT_THROW(r2_o->set_d1(d), std::exception);
    EXPECT_THROW(r3_o->set_d1(d), std::exception);

    // Test the lock override by testsuperpass
    RR_INTRUSIVE_PTR<RRMap<string, RRValue> > cred5 = AllocateEmptyRRMap<string, RRValue>();
    cred5->insert(make_pair("password", stringToRRArray("superpass1")));
    RR_SHARED_PTR<testroot> r5 =
        rr_cast<testroot>(RobotRaconteurNode::s()->ConnectService(url, "testsuperuser", cred5));
    RR_SHARED_PTR<sub1> r5_o = r5->get_o1();
    EXPECT_NO_THROW(RobotRaconteurNode::s()->ReleaseObjectLock(r5_o));

    // Make sure the lock is released
    EXPECT_NO_THROW(r2_o->set_d1(d));
    EXPECT_NO_THROW(r3_o->set_d1(d));

    // Close all connections
    EXPECT_NO_THROW(RobotRaconteurNode::s()->DisconnectService(r1));
    EXPECT_NO_THROW(RobotRaconteurNode::s()->DisconnectService(r2));
    EXPECT_NO_THROW(RobotRaconteurNode::s()->DisconnectService(r3));
    EXPECT_NO_THROW(RobotRaconteurNode::s()->DisconnectService(r5));
}

void ServiceTestClient::TestMonitorLock(string url)
{
    // The monitor lock acquires an exclusive sock to a single thread on the client.
    // This lock is for a single thread, and works for all clients and the service if
    // it is checking for monitor locks.

    RR_SHARED_PTR<testroot> r1;
    EXPECT_NO_THROW(r1 = rr_cast<testroot>(RobotRaconteurNode::s()->ConnectService(url)));
    EXPECT_NO_THROW(r2 = rr_cast<testroot>(RobotRaconteurNode::s()->ConnectService(url)));

    RR_SHARED_PTR<sub1> r1_o;
    RR_SHARED_PTR<sub2> r1_o_o2;
    EXPECT_NO_THROW(r1_o = r1->get_o1());
    EXPECT_NO_THROW(r1_o_o2 = r1_o->get_o2_1());
    EXPECT_NO_THROW(r2_o = r2->get_o1());
    EXPECT_NO_THROW(r2_o_o2 = r2_o->get_o2_1());

    // Make sure everything works
    EXPECT_NO_THROW(r1->func3(2.2, 3.3));

    double d1[] = {1.0};
    RR_INTRUSIVE_PTR<RRArray<double> > d = AttachRRArrayCopy(d1, 1);
    EXPECT_NO_THROW(r1_o->set_d1(d));
    EXPECT_NO_THROW(r1_o_o2->set_data("Hello world"));

    EXPECT_NO_THROW(r2->func3(2.2, 3.3));
    EXPECT_NO_THROW(r2_o->set_d1(d));
    EXPECT_NO_THROW(r2_o_o2->set_data("Hello world"));

    t2 = false;
    threaderr = true;

    // Request lock
    // RobotRaconteurNode::s()->MonitorEnter(r1_o);

    boost::thread t;

    {
        RobotRaconteurNode::ScopedMonitorLock lock2(r1_o);

        // Start thread and run it
        t = boost::thread(boost::bind(&ServiceTestClient::test_monitor_lock_thread, this));

        t1 = true;
        EXPECT_TRUE(e1.WaitOne(1000));
        boost::this_thread::sleep(boost::posix_time::milliseconds(10));
        EXPECT_NO_THROW(r1->func3(2.2, 3.3));
        EXPECT_NO_THROW(r1_o->set_d1(d));
        EXPECT_FALSE(t2);
        t1 = false;
        // RobotRaconteurNode::s()->MonitorExit(r1_o);
    }

    t.join();
    boost::this_thread::sleep(boost::posix_time::milliseconds(100));

    EXPECT_NO_THROW(RobotRaconteurNode::s()->DisconnectService(r1));
    EXPECT_NO_THROW(RobotRaconteurNode::s()->DisconnectService(r2));

    EXPECT_FALSE(threaderr);
}

void ServiceTestClient::test_monitor_lock_thread()
{
    try
    {
        r2->func3(2.2, 3.3);
        r2_o_o2->set_data("Hello world");
        e1.Set();

        {
            RobotRaconteurNode::ScopedMonitorLock lock3(r2_o);
            // RobotRaconteurNode::s()->MonitorEnter(r2_o);
            if (t1)
                throw std::exception();
            t2 = true;
            double d1[] = {1.0};
            RR_INTRUSIVE_PTR<RRArray<double> > d = AttachRRArrayCopy(d1, 1);
            r2_o->set_d1(d);
        }
        // RobotRaconteurNode::s()->MonitorExit(r2_o);
        threaderr = false;
    }
    catch (std::exception&)
    {
        threaderr = true;
    }
}

void ServiceTestClient::TestAsync_err(RR_SHARED_PTR<RobotRaconteurException> exp)
{
    boost::mutex::scoped_lock lock(async_err_lock);
    async_err = exp;
    async_wait.Set();
}

void ServiceTestClient::TestAsync(string url)
{
    RR_INTRUSIVE_PTR<RRMap<string, RRValue> > cred1 = AllocateEmptyRRMap<string, RRValue>();
    cred1->insert(make_pair("password", stringToRRArray("testpass1")));
    ASSERT_NO_THROW(RobotRaconteurNode::s()->AsyncConnectService(
        url, "testuser1", cred1, RR_NULL_FN, "",
        boost::bind(&ServiceTestClient::TestAsync1, this, RR_BOOST_PLACEHOLDERS(_1), RR_BOOST_PLACEHOLDERS(_2))));

    if (!async_wait.WaitOne(1000))
    {
        throw std::runtime_error("Timeout waiting for result");
    }
    boost::mutex::scoped_lock lock(async_err_lock);
    if (async_err)
    {
        throw *async_err;
    }
}

void ServiceTestClient::TestAsync1(RR_SHARED_PTR<RRObject> r, RR_SHARED_PTR<RobotRaconteurException> exp)
{
    ASYNC_TEST_CALL(RR_SHARED_PTR<async_testroot> r1 = rr_cast<async_testroot>(r); r1->async_get_d2(
        boost::bind(&ServiceTestClient::TestAsync2, this, r1, RR_BOOST_PLACEHOLDERS(_1), RR_BOOST_PLACEHOLDERS(_2))););
}

void ServiceTestClient::TestAsync2(RR_SHARED_PTR<async_testroot> r, RR_INTRUSIVE_PTR<RRArray<double> > ret,
                                   RR_SHARED_PTR<RobotRaconteurException> exp)
{
    if (exp)
    {
        TestAsync_err(exp);
        return;
    }
    try
    {
        RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > d2_b = GetTestRRArray<double>("ts1.d2.to_client");
        EXPECT_RRARRAY_EQ(d2_b, ret);
        RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > d2_a = GetTestRRArray<double>("ts1.d2.to_service");
        r->async_set_d2(d2_a, boost::bind(&ServiceTestClient::TestAsync3, this, r, RR_BOOST_PLACEHOLDERS(_1)));
    }
    catch (std::exception&)
    {
        TestAsync_err(RR_MAKE_SHARED<UnknownException>("ERR", "Err occurred"));
    }
}

void ServiceTestClient::TestAsync3(RR_SHARED_PTR<async_testroot> r, RR_SHARED_PTR<RobotRaconteurException> exp)
{
    ASYNC_TEST_CALL(r->async_func1(boost::bind(&ServiceTestClient::TestAsync4, this, r, RR_BOOST_PLACEHOLDERS(_1))););
}

void ServiceTestClient::TestAsync4(RR_SHARED_PTR<async_testroot> r, RR_SHARED_PTR<RobotRaconteurException> exp)
{
    ASYNC_TEST_CALL(r->async_func3(
        2, 3.45,
        boost::bind(&ServiceTestClient::TestAsync5, this, r, RR_BOOST_PLACEHOLDERS(_1), RR_BOOST_PLACEHOLDERS(_2))););
}

void ServiceTestClient::TestAsync5(RR_SHARED_PTR<async_testroot> r, double ret,
                                   RR_SHARED_PTR<RobotRaconteurException> exp)
{
    ASYNC_TEST_CALL(if (ret != 5.45) throw std::runtime_error(""); r->async_func_errtest(
        boost::bind(&ServiceTestClient::TestAsync6, this, r, RR_BOOST_PLACEHOLDERS(_1))););
}

void ServiceTestClient::TestAsync6(RR_SHARED_PTR<async_testroot> r, RR_SHARED_PTR<RobotRaconteurException> exp)
{
    try
    {
        if (!exp)
            throw std::runtime_error("");
        r->async_get_o1(
            boost::bind(&ServiceTestClient::TestAsync7, this, r, RR_BOOST_PLACEHOLDERS(_1), RR_BOOST_PLACEHOLDERS(_2)));
    }
    catch (std::exception&)
    {
        TestAsync_err(RR_MAKE_SHARED<UnknownException>("ERR", "Err occurred"));
    }
}

void ServiceTestClient::TestAsync7(RR_SHARED_PTR<async_testroot> r, RR_SHARED_PTR<sub1> o1,
                                   RR_SHARED_PTR<RobotRaconteurException> exp)
{
    // TODO: fix this test
    ASYNC_TEST_CALL(
        // if (!o1) throw std::runtime_error("");
        // o1->get_d1();

        RR_SHARED_PTR<testroot> r1 = rr_cast<testroot>(r);
        r1->get_p1()->AsyncConnect(-1, boost::bind(&ServiceTestClient::TestAsync8, this, r1, RR_BOOST_PLACEHOLDERS(_1),
                                                   RR_BOOST_PLACEHOLDERS(_2)));

    );
}

void ServiceTestClient::TestAsync8(RR_SHARED_PTR<testroot> r,
                                   RR_SHARED_PTR<PipeEndpoint<RR_INTRUSIVE_PTR<RRArray<double> > > > e1,
                                   RR_SHARED_PTR<RobotRaconteurException> exp)
{
    double d1[] = {1, 2, 3, 4};
    ASYNC_TEST_CALL(

        e1->AsyncSendPacket(AttachRRArrayCopy(d1, 4),
                            boost::bind(&ServiceTestClient::TestAsync9, this, r, e1, RR_BOOST_PLACEHOLDERS(_1),
                                        RR_BOOST_PLACEHOLDERS(_2)));)
}

void ServiceTestClient::TestAsync9(RR_SHARED_PTR<testroot> r,
                                   RR_SHARED_PTR<PipeEndpoint<RR_INTRUSIVE_PTR<RRArray<double> > > > e1, uint32_t pnum,
                                   RR_SHARED_PTR<RobotRaconteurException> exp)
{
    ASYNC_TEST_CALL(
        e1->AsyncClose(boost::bind(&ServiceTestClient::TestAsync10, this, r, e1, RR_BOOST_PLACEHOLDERS(_1))););
}

void ServiceTestClient::TestAsync10(RR_SHARED_PTR<testroot> r,
                                    RR_SHARED_PTR<PipeEndpoint<RR_INTRUSIVE_PTR<RRArray<double> > > > e1,
                                    RR_SHARED_PTR<RobotRaconteurException> exp)
{
    ASYNC_TEST_CALL(r->get_w1()->AsyncConnect(
        boost::bind(&ServiceTestClient::TestAsync11, this, r, RR_BOOST_PLACEHOLDERS(_1), RR_BOOST_PLACEHOLDERS(_2))));
}

void ServiceTestClient::TestAsync11(RR_SHARED_PTR<testroot> r,
                                    RR_SHARED_PTR<WireConnection<RR_INTRUSIVE_PTR<RRArray<double> > > > w1,
                                    RR_SHARED_PTR<RobotRaconteurException> exp)
{
    double d0[] = {0.0};
    ASYNC_TEST_CALL(w1->SetOutValue(AttachRRArrayCopy(d0, 1)); w1->AsyncClose(
        boost::bind(&ServiceTestClient::TestAsync12, this, r, w1, RR_BOOST_PLACEHOLDERS(_1))););
}

void ServiceTestClient::TestAsync12(RR_SHARED_PTR<testroot> r,
                                    RR_SHARED_PTR<WireConnection<RR_INTRUSIVE_PTR<RRArray<double> > > > w1,
                                    RR_SHARED_PTR<RobotRaconteurException> exp)
{
    ASYNC_TEST_CALL(RobotRaconteurNode::s()->AsyncRequestObjectLock(
        r, RobotRaconteurObjectLockFlags_CLIENT_LOCK,
        boost::bind(&ServiceTestClient::TestAsync13, this, r, RR_BOOST_PLACEHOLDERS(_1), RR_BOOST_PLACEHOLDERS(_2))););
}

void ServiceTestClient::TestAsync13(RR_SHARED_PTR<testroot> r, RR_SHARED_PTR<std::string> res,
                                    RR_SHARED_PTR<RobotRaconteurException> exp)
{
    ASYNC_TEST_CALL(if (!res) throw std::runtime_error(""); if (*res != "OK") throw std::runtime_error("");
                    RobotRaconteurNode::s()->AsyncReleaseObjectLock(r, boost::bind(&ServiceTestClient::TestAsync14,
                                                                                   this, r, RR_BOOST_PLACEHOLDERS(_1),
                                                                                   RR_BOOST_PLACEHOLDERS(_2))););
}

void ServiceTestClient::TestAsync14(RR_SHARED_PTR<testroot> r, RR_SHARED_PTR<std::string> res,
                                    RR_SHARED_PTR<RobotRaconteurException> exp)
{
    ASYNC_TEST_CALL(if (!res) throw std::runtime_error(""); if (*res != "OK") throw std::runtime_error("");
                    RobotRaconteurNode::s()->AsyncDisconnectService(
                        rr_cast<RRObject>(r), boost::bind(&ServiceTestClient::TestAsync15, this)););
}

void ServiceTestClient::TestAsync15() { async_wait.Set(); }

void ServiceTestClient::RunSingleThreadTest(string url, RR_BOOST_ASIO_IO_CONTEXT& io_context)
{
    IOContextThreadPool_AsyncResultAdapter<RR_SHARED_PTR<RRObject> > connect_res(io_context);
    ASSERT_NO_THROW(RobotRaconteurNode::s()->AsyncConnectService(
        url, "", RR_INTRUSIVE_PTR<RRMap<std::string, RRValue> >(), RR_NULL_FN, "", connect_res, 5000));
    RR_SHARED_PTR<RRObject> c1 = connect_res.GetResult();
    RR_SHARED_PTR<async_testroot> c = rr_cast<async_testroot>(c1);

    IOContextThreadPool_AsyncResultAdapter<RR_INTRUSIVE_PTR<RRArray<double> > > get_d2(io_context);
    ASSERT_NO_THROW(c->async_get_d2(get_d2));
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > d2_b = GetTestRRArray<double>("ts1.d2.to_client");
    EXPECT_RRARRAY_EQ(d2_b, get_d2.GetResult());

    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > d2_a = GetTestRRArray<double>("ts1.d2.to_service");
    IOContextThreadPool_AsyncResultAdapter<void> set_d2(io_context);
    ASSERT_NO_THROW(c->async_set_d2(d2_a, set_d2));
    set_d2.GetResult();

    IOContextThreadPool_AsyncResultAdapter<void> disconnect_res(io_context);
    ASSERT_NO_THROW(RobotRaconteurNode::s()->AsyncDisconnectService(c1, disconnect_res));
    disconnect_res.GetResult();
}

} // namespace RobotRaconteurTest
