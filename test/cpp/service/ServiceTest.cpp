#ifdef ROBOTRACONTEUR_USE_STDAFX
#include "stdafx.h"
#endif

#include "ServiceTest.h"

#include "service_test_utils.h"
#include "test_vectors.h"

using namespace std;
using namespace boost;
using namespace com::robotraconteur::testing::TestService1;
using namespace com::robotraconteur::testing::TestService2;

using std::exception;

using namespace RobotRaconteur::test;

// cSpell: ignore lstruct, numa, structa

namespace RobotRaconteurTest
{

RobotRaconteurTest_testroot::RobotRaconteurTest_testroot(RR_SHARED_PTR<TcpTransport> tcptransport)
{
    this->tcptransport = tcptransport;
    o1 = RR_MAKE_SHARED<sub1_impl>();
    o5 = RR_MAKE_SHARED<TestService2SubObj>();
    packet_sent = false;
    ack_recv = false;

    m1 = RR_MAKE_SHARED<ArrayMemory<double> >(GetTestRRArray<double>("ts1.m1.initial"));

    uint32_t m2_1a[] = {10, 10, 10, 10, 10};
    uint32_t* m2_1b = new uint32_t[5];
    memcpy(m2_1b, m2_1a, sizeof(m2_1a));
    m2 = RR_MAKE_SHARED<MultiDimArrayMemory<double> >(
        AllocateRRMultiDimArray<double>(AttachRRArray(m2_1b, 5, true), AllocateRRArray<double>(1000000)));

    uint32_t m3_1a[] = {1024, 1024};
    uint32_t* m3_1b = new uint32_t[2];
    memcpy(m3_1b, m3_1a, sizeof(m3_1a));
    m3 = RR_MAKE_SHARED<MultiDimArrayMemory<uint8_t> >(
        AllocateRRMultiDimArray<uint8_t>(AttachRRArray(m3_1b, 2, true), AllocateRRArray<uint8_t>(1024 * 1024)));
}

double RobotRaconteurTest_testroot::get_d1() { return 12.345; }

void RobotRaconteurTest_testroot::set_d1(double value)
{
    ThrowIfFailures thrower;
    EXPECT_EQ(value, 3.456);
    thrower.Throw();
}

RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > RobotRaconteurTest_testroot::get_d2()
{
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > d2_b = GetTestRRArray<double>("ts1.d2.to_client");
    // double* d2_ba=new double[392];
    // memcpy(d2_ba,d2_b,392*sizeof(double));
    return (d2_b);
}

void RobotRaconteurTest_testroot::set_d2(const RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> >& value)
{
    ThrowIfFailures thrower;
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > d2_a = GetTestRRArray<double>("ts1.d2.to_service");
    EXPECT_RRARRAY_EQ(value, d2_a);
    thrower.Throw();
}

RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > RobotRaconteurTest_testroot::get_d3()
{
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > d3_b = GetTestRRArray<double>("ts1.d3.to_client");
    return (d3_b);
}

void RobotRaconteurTest_testroot::set_d3(const RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> >& value)
{
    ThrowIfFailures thrower;
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > d3_a = GetTestRRArray<double>("ts1.d3.to_service");
    EXPECT_RRARRAY_EQ(d3_a, value);
    thrower.Throw();
}

RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > RobotRaconteurTest_testroot::get_d4()
{
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > d4_b = GetTestRRArray<double>("ts1.d4.to_client");
    return (d4_b);
}

void RobotRaconteurTest_testroot::set_d4(const RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> >& value)
{
    ThrowIfFailures thrower;
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > d4_a = GetTestRRArray<double>("ts1.d4.to_service");
    EXPECT_RRARRAY_EQ(d4_a, value);
    thrower.Throw();
}

RR_INTRUSIVE_PTR<RobotRaconteur::RRMultiDimArray<double> > RobotRaconteurTest_testroot::get_d5()
{

    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<uint32_t> > d5_b1 = GetTestRRArray<uint32_t>("ts1.d5.to_client.dims");
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > d5_b2 = GetTestRRArray<double>("ts1.d5.to_client");
    return (AllocateRRMultiDimArray<double>(d5_b1, d5_b2));
}

void RobotRaconteurTest_testroot::set_d5(const RR_INTRUSIVE_PTR<RobotRaconteur::RRMultiDimArray<double> >& value)
{
    ThrowIfFailures thrower;
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<uint32_t> > d5_a1 = GetTestRRArray<uint32_t>("ts1.d5.to_service.dims");
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > d5_a2 = GetTestRRArray<double>("ts1.d5.to_service");
    // r->set_d5(AllocateRRMultiDimArray<double>(d5_a1,d5_a2,AttachRRArray(d5_a3,96,false)));
    EXPECT_RRARRAY_EQ(value->Dims, d5_a1);
    EXPECT_RRARRAY_EQ(value->Array, d5_a2);
    thrower.Throw();
}

RR_INTRUSIVE_PTR<RobotRaconteur::RRMultiDimArray<double> > RobotRaconteurTest_testroot::get_d6()
{

    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<uint32_t> > d5_b1 = GetTestRRArray<uint32_t>("ts1.d6.to_client.dims");
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > d5_b2 = GetTestRRArray<double>("ts1.d6.to_client");

    return (AllocateRRMultiDimArray<double>(d5_b1, d5_b2));
}

void RobotRaconteurTest_testroot::set_d6(const RR_INTRUSIVE_PTR<RobotRaconteur::RRMultiDimArray<double> >& value)
{
    ThrowIfFailures thrower;
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<uint32_t> > d5_a1 = GetTestRRArray<uint32_t>("ts1.d6.to_service.dims");
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > d5_a2 = GetTestRRArray<double>("ts1.d6.to_service");

    // r->set_d5(AllocateRRMultiDimArray<double>(d5_a1,d5_a2,AttachRRArray(d5_a3,96,false)));
    EXPECT_RRARRAY_EQ(value->Dims, d5_a1);
    EXPECT_RRARRAY_EQ(value->Array, d5_a2);
    thrower.Throw();
}

float RobotRaconteurTest_testroot::get_s1() { return 7.8573f; }

void RobotRaconteurTest_testroot::set_s1(float value)
{
    ThrowIfFailures thrower;
    EXPECT_FLOAT_EQ(value, 3847.9283f);
    thrower.Throw();
}

RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<float> > RobotRaconteurTest_testroot::get_s2()
{
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<float> > s2_b = GetTestRRArray<float>("ts1.s2.to_client");
    return (s2_b);
}

void RobotRaconteurTest_testroot::set_s2(const RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<float> >& value)
{
    ThrowIfFailures thrower;
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<float> > s2_a = GetTestRRArray<float>("ts1.s2.to_service");
    EXPECT_RRARRAY_EQ(value, s2_a);
    thrower.Throw();
}

int8_t RobotRaconteurTest_testroot::get_i8_1() { return -66; }

void RobotRaconteurTest_testroot::set_i8_1(int8_t value)
{
    ThrowIfFailures thrower;
    EXPECT_EQ(value, 45);
    thrower.Throw();
}

RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<int8_t> > RobotRaconteurTest_testroot::get_i8_2()
{
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<int8_t> > i8_2b = GetTestRRArray<int8_t>("ts1.i8_2.to_client");
    return (i8_2b);
}

void RobotRaconteurTest_testroot::set_i8_2(const RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<int8_t> >& value)
{
    ThrowIfFailures thrower;
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<int8_t> > i8_2a = GetTestRRArray<int8_t>("ts1.i8_2.to_service");
    EXPECT_RRARRAY_EQ(value, i8_2a);
    thrower.Throw();
}

uint8_t RobotRaconteurTest_testroot::get_u8_1() { return 222; }

void RobotRaconteurTest_testroot::set_u8_1(uint8_t value)
{
    ThrowIfFailures thrower;
    EXPECT_EQ(value, 232);
    thrower.Throw();
}

RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<uint8_t> > RobotRaconteurTest_testroot::get_u8_2()
{
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<uint8_t> > u8_2b = GetTestRRArray<uint8_t>("ts1.u8_2.to_client");
    return (u8_2b);
}

void RobotRaconteurTest_testroot::set_u8_2(const RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<uint8_t> >& value)
{
    ThrowIfFailures thrower;
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<uint8_t> > u8_2a = GetTestRRArray<uint8_t>("ts1.u8_2.to_service");
    EXPECT_RRARRAY_EQ(value, u8_2a);
    thrower.Throw();
}

RR_INTRUSIVE_PTR<RobotRaconteur::RRMultiDimArray<uint8_t> > RobotRaconteurTest_testroot::get_u8_3()
{
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<uint32_t> > u8_3_1b1 = GetTestRRArray<uint32_t>("ts1.u8_3.to_client.dims");
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<uint8_t> > u8_3_1b2 = GetTestRRArray<uint8_t>("ts1.u8_3.to_client");
    return AllocateRRMultiDimArray<uint8_t>(u8_3_1b1, u8_3_1b2);
}

void RobotRaconteurTest_testroot::set_u8_3(const RR_INTRUSIVE_PTR<RobotRaconteur::RRMultiDimArray<uint8_t> >& value)
{
    ThrowIfFailures thrower;
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<uint32_t> > u8_3_1a1 =
        GetTestRRArray<uint32_t>("ts1.u8_3.to_service.dims");
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<uint8_t> > u8_3_1a2 = GetTestRRArray<uint8_t>("ts1.u8_3.to_service");
    // r->set_u8_3(AllocateRRMultiDimArray<uint8_t>(u8_3_1a1,u8_3_1a2));
    EXPECT_RRARRAY_EQ(value->Dims, u8_3_1a1);
    EXPECT_RRARRAY_EQ(value->Array, u8_3_1a2);
    thrower.Throw();
}

int16_t RobotRaconteurTest_testroot::get_i16_1() { return -13428; }

void RobotRaconteurTest_testroot::set_i16_1(int16_t value)
{
    ThrowIfFailures thrower;
    EXPECT_EQ(value, 2387);
    thrower.Throw();
}

RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<int16_t> > RobotRaconteurTest_testroot::get_i16_2()
{
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<int16_t> > i16_2b = GetTestRRArray<int16_t>("ts1.i16_2.to_client");
    return (i16_2b);
}

void RobotRaconteurTest_testroot::set_i16_2(const RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<int16_t> >& value)
{
    ThrowIfFailures thrower;
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<int16_t> > i16_2a = GetTestRRArray<int16_t>("ts1.i16_2.to_service");
    EXPECT_RRARRAY_EQ(value, i16_2a);
    thrower.Throw();
}

uint16_t RobotRaconteurTest_testroot::get_u16_1() { return 60981; }

void RobotRaconteurTest_testroot::set_u16_1(uint16_t value)
{
    ThrowIfFailures thrower;
    EXPECT_EQ(value, 54732);
    thrower.Throw();
}

RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<uint16_t> > RobotRaconteurTest_testroot::get_u16_2()
{
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<uint16_t> > u16_2b = GetTestRRArray<uint16_t>("ts1.u16_2.to_client");
    return (u16_2b);
}

void RobotRaconteurTest_testroot::set_u16_2(const RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<uint16_t> >& value)
{
    ThrowIfFailures thrower;
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<uint16_t> > u16_2a = GetTestRRArray<uint16_t>("ts1.u16_2.to_service");
    EXPECT_RRARRAY_EQ(value, u16_2a);
    thrower.Throw();
}

int32_t RobotRaconteurTest_testroot::get_i32_1() { return 898734; }

void RobotRaconteurTest_testroot::set_i32_1(int32_t value)
{
    ThrowIfFailures thrower;
    EXPECT_EQ(value, -9837284);
    thrower.Throw();
}

RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<int32_t> > RobotRaconteurTest_testroot::get_i32_2()
{
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<int32_t> > i32_1b = GetTestRRArray<int32_t>("ts1.i32_2.to_client");
    return (i32_1b);
}

void RobotRaconteurTest_testroot::set_i32_2(const RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<int32_t> >& value)
{
    ThrowIfFailures thrower;
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<int32_t> > i32_1a = GetTestRRArray<int32_t>("ts1.i32_2.to_service");
    EXPECT_RRARRAY_EQ(value, i32_1a);
    thrower.Throw();
}

RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<int32_t> > RobotRaconteurTest_testroot::get_i32_huge()
{
    RR_INTRUSIVE_PTR<RRArray<int32_t> > o = AllocateRRArray<int32_t>(2621440);
    for (int32_t i = 0; i < 2621440; i++)
    {
        (*o)[i] = i;
    }

    return o;
}

void RobotRaconteurTest_testroot::set_i32_huge(const RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<int32_t> >& value)
{
    ThrowIfFailures thrower;
    EXPECT_EQ(value->size(), 2621440);
    thrower.Throw();
    for (int32_t i = 0; i < 2621440; i++)
    {
        EXPECT_EQ((*value)[i], -i) << " at index " << i;
        if ((*value)[i] != -i)
            break;
    }
    thrower.Throw();
}

uint32_t RobotRaconteurTest_testroot::get_u32_1() { return 547919123u; }

void RobotRaconteurTest_testroot::set_u32_1(uint32_t value)
{
    ThrowIfFailures thrower;
    EXPECT_EQ(value, 1550099195);
    thrower.Throw();
}

RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<uint32_t> > RobotRaconteurTest_testroot::get_u32_2()
{
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<uint32_t> > u32_1b = GetTestRRArray<uint32_t>("ts1.u32_2.to_client");
    return (u32_1b);
}

void RobotRaconteurTest_testroot::set_u32_2(const RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<uint32_t> >& value)
{
    ThrowIfFailures thrower;
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<uint32_t> > u32_1a = GetTestRRArray<uint32_t>("ts1.u32_2.to_service");
    EXPECT_RRARRAY_EQ(value, u32_1a);
    thrower.Throw();
}

int64_t RobotRaconteurTest_testroot::get_i64_1() { return -1357833931563696072; }

void RobotRaconteurTest_testroot::set_i64_1(int64_t value)
{
    ThrowIfFailures thrower;
    EXPECT_EQ(value, 8621740821050813024);
    thrower.Throw();
}

RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<int64_t> > RobotRaconteurTest_testroot::get_i64_2()
{
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<int64_t> > i64_2b = GetTestRRArray<int64_t>("ts1.i64_2.to_client");
    return (i64_2b);
}

void RobotRaconteurTest_testroot::set_i64_2(const RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<int64_t> >& value)
{
    ThrowIfFailures thrower;
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<int64_t> > i64_2a = GetTestRRArray<int64_t>("ts1.i64_2.to_service");
    EXPECT_RRARRAY_EQ(value, i64_2a);
    thrower.Throw();
}

uint64_t RobotRaconteurTest_testroot::get_u64_1() { return 13389861970863644378u; }

void RobotRaconteurTest_testroot::set_u64_1(uint64_t value)
{
    ThrowIfFailures thrower;
    EXPECT_EQ(value, 1465640522145789825);
    thrower.Throw();
}

RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<uint64_t> > RobotRaconteurTest_testroot::get_u64_2()
{
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<uint64_t> > u64_1b = GetTestRRArray<uint64_t>("ts1.u64_2.to_client");
    return (u64_1b);
}

void RobotRaconteurTest_testroot::set_u64_2(const RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<uint64_t> >& value)
{
    ThrowIfFailures thrower;
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<uint64_t> > u64_1a = GetTestRRArray<uint64_t>("ts1.u64_2.to_service");
    EXPECT_RRARRAY_EQ(value, u64_1a);
    thrower.Throw();
}

std::string RobotRaconteurTest_testroot::get_str1() { return "Hello Client!"; }

void RobotRaconteurTest_testroot::set_str1(const std::string& value)
{
    ThrowIfFailures thrower;
    EXPECT_EQ(value, "Hello Server!");
    thrower.Throw();
}

RR_INTRUSIVE_PTR<teststruct1> RobotRaconteurTest_testroot::get_struct1()
{
    RR_INTRUSIVE_PTR<teststruct1> s1(new teststruct1());
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > s1_dat1a = GetTestRRArray<double>("ts1.struct1.dat1.to_client");
    s1->dat1 = s1_dat1a;
    s1->str2 = "Hello world!";
    s1->vec3 = AllocateEmptyRRMap<int32_t, RRArray<char> >();
    s1->vec3->clear();
    s1->vec3->insert(make_pair(1, stringToRRArray("Hello Client!")));
    s1->vec3->insert(make_pair(2, stringToRRArray("Hello Client, again")));
    s1->vec3->insert(make_pair(4372, stringToRRArray("This is yet another test string")));

    s1->dict4 = AllocateEmptyRRMap<string, RRArray<char> >();
    s1->dict4->insert(std::make_pair("teststring1", stringToRRArray("Hello Client!")));
    s1->dict4->insert(std::make_pair("teststring2", stringToRRArray("Hello Client, again")));
    s1->dict4->insert(std::make_pair("anotherstr", stringToRRArray("This is yet another test string")));

    s1->list5 = AllocateEmptyRRList<RRArray<char> >();
    s1->list5->push_back(stringToRRArray("Hello Client!"));
    s1->list5->push_back(stringToRRArray("Hello Client, again"));
    s1->list5->push_back(stringToRRArray("This is yet another test string"));

    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > s1_s1_mydat =
        GetTestRRArray<double>("ts1.struct1.struct1_mydat.to_client");
    s1->struct1 = (new teststruct2());
    s1->struct1->mydat = s1_s1_mydat;

    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > s1_d1a =
        GetTestRRArray<double>("ts1.struct1.dstruct2_1.to_client");
    s1->dstruct2 = AllocateEmptyRRMap<string, teststruct2>();
    RR_INTRUSIVE_PTR<teststruct2> ts1(new teststruct2());
    ts1->mydat = s1_d1a;
    s1->dstruct2->insert(make_pair("test1", ts1));
    RR_INTRUSIVE_PTR<teststruct2> ts2(new teststruct2());
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > s1_d1b =
        GetTestRRArray<double>("ts1.struct1.dstruct2_3.to_client");
    ts2->mydat = s1_d1b;
    s1->dstruct2->insert(make_pair("anothertest", ts2));

    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > s1_d1a_2 =
        GetTestRRArray<double>("ts1.struct1.dstruct2_1.to_client");
    s1->lstruct3 = AllocateEmptyRRList<teststruct2>();
    RR_INTRUSIVE_PTR<teststruct2> ts1_2 = (new teststruct2());
    ts1_2->mydat = s1_d1a_2;
    s1->lstruct3->push_back(ts1_2);
    RR_INTRUSIVE_PTR<teststruct2> ts2_2 = (new teststruct2());
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > s1_d1b_2 =
        GetTestRRArray<double>("ts1.struct1.dstruct2_3.to_client");
    ts2_2->mydat = s1_d1b_2;
    s1->lstruct3->push_back(ts2_2);

    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<uint32_t> > s1_mul_a =
        GetTestRRArray<uint32_t>("ts1.struct1.multidimarray.to_client.dims");
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > s1_mul_b =
        GetTestRRArray<double>("ts1.struct1.multidimarray.to_client");
    s1->multidimarray = AllocateRRMultiDimArray<double>(s1_mul_a, s1_mul_b);

    s1->var3 = stringToRRArray("This is a vartype string");

    return s1;
}

void RobotRaconteurTest_testroot::set_struct1(const RR_INTRUSIVE_PTR<teststruct1>& s2)
{
    ThrowIfFailures thrower;
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > dat1a = GetTestRRArray<double>("ts1.struct1.dat1.to_service");
    EXPECT_RRARRAY_EQ(s2->dat1, dat1a);
    EXPECT_EQ(s2->str2, "Hello world 2!");
    ASSERT_TRUE(s2->vec3);
    EXPECT_EQ(s2->vec3->size(), 4);
    thrower.Throw();
    EXPECT_EQ(RRArrayToString(s2->vec3->at(10)), "Hello Server!");
    EXPECT_EQ(RRArrayToString(s2->vec3->at(11)), "Hello Server, again");
    EXPECT_EQ(RRArrayToString(s2->vec3->at(46372)), "Test string!");
    EXPECT_EQ(RRArrayToString(s2->vec3->at(46373)), "Test string again");

    ASSERT_TRUE(s2->dict4);
    EXPECT_EQ(s2->dict4->size(), 2);
    thrower.Throw();
    EXPECT_EQ(RRArrayToString(s2->dict4->at("cteststring1")), "Hello Server!");
    EXPECT_EQ(RRArrayToString(s2->dict4->at("cteststring2")), "Hello Server, again");

    ASSERT_TRUE(s2->list5);
    EXPECT_EQ(s2->list5->size(), 2);
    thrower.Throw();
    EXPECT_EQ(RRArrayToString(s2->list5->front()), "Hello Server!");
    EXPECT_EQ(RRArrayToString(*(++s2->list5->begin())), "Hello Server, again");

    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > s2_struct1_mydat =
        GetTestRRArray<double>("ts1.struct1.struct1_mydat.to_service");
    EXPECT_RRARRAY_EQ(s2->struct1->mydat, s2_struct1_mydat);

    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > s2_ds2_1a =
        GetTestRRArray<double>("ts1.struct1.dstruct2_1.to_service");
    EXPECT_RRARRAY_EQ(s2->dstruct2->at("ctest1")->mydat, s2_ds2_1a);
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > s2_ds2_2a =
        GetTestRRArray<double>("ts1.struct1.dstruct2_2.to_service");
    EXPECT_RRARRAY_EQ(s2->dstruct2->at("anothertest")->mydat, s2_ds2_2a);

    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > s2_ls3_1a =
        GetTestRRArray<double>("ts1.struct1.dstruct2_1.to_service");
    EXPECT_RRARRAY_EQ(s2->lstruct3->front()->mydat, s2_ls3_1a);
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > s2_ls3_2a =
        GetTestRRArray<double>("ts1.struct1.dstruct2_2.to_service");
    EXPECT_RRARRAY_EQ((*(++s2->lstruct3->begin()))->mydat, s2_ls3_2a);

    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<uint32_t> > s2_mda =
        GetTestRRArray<uint32_t>("ts1.struct1.multidimarray.to_service.dims");
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > s2_mdb =
        GetTestRRArray<double>("ts1.struct1.multidimarray.to_service");
    // s2->multidimarray=AllocateRRMultiDimArray<double>(s2_mda,s2_mdb);
    EXPECT_RRARRAY_EQ(s2_mda, s2->multidimarray->Dims);
    EXPECT_RRARRAY_EQ(s2_mdb, s2->multidimarray->Array);

    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > s2_var3a = GetTestRRArray<double>("ts1.struct1.var3.to_service");
    EXPECT_RRARRAY_EQ(rr_cast<RRArray<double> >(s2->var3), s2_var3a);
    thrower.Throw();
}

RR_INTRUSIVE_PTR<teststruct2> RobotRaconteurTest_testroot::get_struct2()
{
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > a1 = GetTestRRArray<double>("ts1.struct2.to_client");
    RR_INTRUSIVE_PTR<teststruct2> a(new teststruct2());
    a->mydat = a1;
    return a;
}

void RobotRaconteurTest_testroot::set_struct2(const RR_INTRUSIVE_PTR<teststruct2>& value)
{
    ThrowIfFailures thrower;
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > a1 = GetTestRRArray<double>("ts1.struct2.to_service");
    EXPECT_TRUE(value);
    thrower.Throw();
    EXPECT_RRARRAY_EQ(value->mydat, a1);
    thrower.Throw();
}

RR_INTRUSIVE_PTR<RobotRaconteur::RRMap<int32_t, RobotRaconteur::RRArray<double> > > RobotRaconteurTest_testroot::
    get_is_d1()
{
    RR_INTRUSIVE_PTR<RRMap<int32_t, RRArray<double> > > o = AllocateEmptyRRMap<int32_t, RRArray<double> >();
    o->insert(make_pair(9285, ScalarToRRArray(1.643392e-01)));
    o->insert(make_pair(74822, ScalarToRRArray(1.537133e+09)));
    o->insert(make_pair(4, ScalarToRRArray(1.369505e-03)));
    return o;
}

void RobotRaconteurTest_testroot::set_is_d1(
    const RR_INTRUSIVE_PTR<RobotRaconteur::RRMap<int32_t, RobotRaconteur::RRArray<double> > >& value)
{
    ThrowIfFailures thrower;
    EXPECT_TRUE(value);
    thrower.Throw();
    EXPECT_EQ(value->size(), 3);
    EXPECT_NO_THROW(EXPECT_EQ(RRArrayToScalar(value->at(928)), 4.074501e-07));
    EXPECT_NO_THROW(EXPECT_EQ(RRArrayToScalar(value->at(394820)), -4.535303e+05));
    EXPECT_NO_THROW(EXPECT_EQ(RRArrayToScalar(value->at(623)), -2.956241e-20));
    thrower.Throw();
}

RR_INTRUSIVE_PTR<RobotRaconteur::RRMap<std::string, RobotRaconteur::RRArray<double> > > RobotRaconteurTest_testroot::
    get_is_d2()
{
    RR_INTRUSIVE_PTR<RRMap<string, RRArray<double> > > o = AllocateEmptyRRMap<string, RRArray<double> >();
    o->insert(make_pair("testval1", ScalarToRRArray(-1.079664e+16)));
    o->insert(make_pair("testval2", ScalarToRRArray(2.224846e+00)));
    return o;
}

void RobotRaconteurTest_testroot::set_is_d2(
    const RR_INTRUSIVE_PTR<RobotRaconteur::RRMap<std::string, RobotRaconteur::RRArray<double> > >& value)
{
    ThrowIfFailures thrower;
    EXPECT_TRUE(value);
    thrower.Throw();
    EXPECT_EQ(value->size(), 2);
    EXPECT_NO_THROW(EXPECT_EQ(RRArrayToScalar(value->at("testval3")), 5.242474e+10));
    EXPECT_NO_THROW(EXPECT_EQ(RRArrayToScalar(value->at("testval4")), 2.208636e+08));
    thrower.Throw();
}

RR_INTRUSIVE_PTR<RobotRaconteur::RRMap<int32_t, RobotRaconteur::RRArray<double> > > RobotRaconteurTest_testroot::
    get_is_d3()
{
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > a = GetTestRRArray<double>("ts1.is_d3.to_client.a");
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > b = GetTestRRArray<double>("ts1.is_d3.to_client.b");

    RR_INTRUSIVE_PTR<RRMap<int32_t, RRArray<double> > > o = AllocateEmptyRRMap<int32_t, RRArray<double> >();
    o->insert(make_pair(12, a));
    o->insert(make_pair(832, b));
    return o;
}

void RobotRaconteurTest_testroot::set_is_d3(
    const RR_INTRUSIVE_PTR<RobotRaconteur::RRMap<int32_t, RobotRaconteur::RRArray<double> > >& value)
{
    ThrowIfFailures thrower;
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > is_d3_2a = GetTestRRArray<double>("ts1.is_d3.to_service.a");
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > is_d3_2b = GetTestRRArray<double>("ts1.is_d3.to_service.b");

    EXPECT_TRUE(value);
    thrower.Throw();
    EXPECT_EQ(value->size(), 2);
    EXPECT_NO_THROW(EXPECT_RRARRAY_EQ(value->at(47), is_d3_2a));
    EXPECT_NO_THROW(EXPECT_RRARRAY_EQ(value->at(324), is_d3_2b));
    thrower.Throw();
}

RR_INTRUSIVE_PTR<RobotRaconteur::RRMap<std::string, RobotRaconteur::RRArray<double> > > RobotRaconteurTest_testroot::
    get_is_d4()
{
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > is_d4_1a = GetTestRRArray<double>("ts1.is_d4.to_client.a");
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > is_d4_1b = GetTestRRArray<double>("ts1.is_d4.to_client.b");

    RR_INTRUSIVE_PTR<RRMap<string, RRArray<double> > > o = AllocateEmptyRRMap<string, RRArray<double> >();

    o->insert(make_pair("testval1", is_d4_1a));
    o->insert(make_pair("testval2", is_d4_1b));

    return o;
}

void RobotRaconteurTest_testroot::set_is_d4(
    const RR_INTRUSIVE_PTR<RobotRaconteur::RRMap<std::string, RobotRaconteur::RRArray<double> > >& value)
{
    ThrowIfFailures thrower;
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > is_d4_2a = GetTestRRArray<double>("ts1.is_d4.to_service.a");
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > is_d4_2b = GetTestRRArray<double>("ts1.is_d4.to_service.b");

    EXPECT_TRUE(value);
    thrower.Throw();
    EXPECT_EQ(value->size(), 2);
    EXPECT_NO_THROW(EXPECT_RRARRAY_EQ(value->at("testval3"), is_d4_2a));
    EXPECT_NO_THROW(EXPECT_RRARRAY_EQ(value->at("testval4"), is_d4_2b));
    thrower.Throw();
}

RR_INTRUSIVE_PTR<RobotRaconteur::RRMap<int32_t, RobotRaconteur::RRMultiDimArray<double> > >
RobotRaconteurTest_testroot::get_is_d5()
{
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<uint32_t> > is_d5_1_1a =
        GetTestRRArray<uint32_t>("ts1.is_d5.to_client.dims");
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > is_d5_1_1b = GetTestRRArray<double>("ts1.is_d5.to_client");

    RR_INTRUSIVE_PTR<RRMultiDimArray<double> > a = AllocateRRMultiDimArray<double>(is_d5_1_1a, is_d5_1_1b);
    RR_INTRUSIVE_PTR<RRMap<int, RRMultiDimArray<double> > > o = AllocateEmptyRRMap<int, RRMultiDimArray<double> >();
    o->insert(make_pair(564, a));
    return o;
}

void RobotRaconteurTest_testroot::set_is_d5(
    const RR_INTRUSIVE_PTR<RobotRaconteur::RRMap<int32_t, RobotRaconteur::RRMultiDimArray<double> > >& value)
{
    ThrowIfFailures thrower;
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<uint32_t> > is_d5_2_1a =
        GetTestRRArray<uint32_t>("ts1.is_d5.to_service.dims");
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > is_d5_2_1b = GetTestRRArray<double>("ts1.is_d5.to_service");

    EXPECT_EQ(value->size(), 1);
    thrower.Throw();
    RR_INTRUSIVE_PTR<RRMultiDimArray<double> > a = value->at(328);
    EXPECT_NO_THROW(EXPECT_RRARRAY_EQ(a->Dims, is_d5_2_1a));
    EXPECT_NO_THROW(EXPECT_RRARRAY_EQ(a->Array, is_d5_2_1b));
    thrower.Throw();
}

RR_INTRUSIVE_PTR<RobotRaconteur::RRMap<std::string, RobotRaconteur::RRMultiDimArray<double> > >
RobotRaconteurTest_testroot::get_is_d6()
{
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<uint32_t> > is_d6_1_1a =
        GetTestRRArray<uint32_t>("ts1.is_d6.to_client.dims");
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > is_d6_1_1b = GetTestRRArray<double>("ts1.is_d6.to_client");

    RR_INTRUSIVE_PTR<RRMultiDimArray<double> > a = AllocateRRMultiDimArray<double>(is_d6_1_1a, is_d6_1_1b);
    RR_INTRUSIVE_PTR<RRMap<string, RRMultiDimArray<double> > > o =
        AllocateEmptyRRMap<string, RRMultiDimArray<double> >();
    o->insert(make_pair("testval1", a));
    return o;
}

void RobotRaconteurTest_testroot::set_is_d6(
    const RR_INTRUSIVE_PTR<RobotRaconteur::RRMap<std::string, RobotRaconteur::RRMultiDimArray<double> > >& value)
{
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<uint32_t> > is_d6_2_1a =
        GetTestRRArray<uint32_t>("ts1.is_d6.to_service.dims");
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > is_d6_2_1b = GetTestRRArray<double>("ts1.is_d6.to_service");

    ThrowIfFailures thrower;
    EXPECT_TRUE(value);
    thrower.Throw();
    EXPECT_EQ(value->size(), 1);
    RR_INTRUSIVE_PTR<RRMultiDimArray<double> > a;
    EXPECT_NO_THROW(a = value->at("testval2"));
    EXPECT_RRARRAY_EQ(a->Dims, is_d6_2_1a);
    EXPECT_RRARRAY_EQ(a->Array, is_d6_2_1b);
    thrower.Throw();
}

RR_INTRUSIVE_PTR<RobotRaconteur::RRMap<int32_t, RobotRaconteur::RRArray<char> > > RobotRaconteurTest_testroot::
    get_is_str1()
{
    RR_INTRUSIVE_PTR<RRMap<int32_t, RRArray<char> > > o = AllocateEmptyRRMap<int32_t, RRArray<char> >();
    o->insert(make_pair(23, stringToRRArray("Hello server")));
    return o;
}

void RobotRaconteurTest_testroot::set_is_str1(
    const RR_INTRUSIVE_PTR<RobotRaconteur::RRMap<int32_t, RobotRaconteur::RRArray<char> > >& value)
{
    ThrowIfFailures thrower;
    EXPECT_TRUE(value);
    thrower.Throw();
    EXPECT_EQ(value->size(), 1);
    EXPECT_NO_THROW(EXPECT_EQ(RRArrayToString(value->at(24)), "Hello client"));
    thrower.Throw();
}

RR_INTRUSIVE_PTR<RobotRaconteur::RRMap<std::string, RobotRaconteur::RRArray<char> > > RobotRaconteurTest_testroot::
    get_is_str2()
{
    RR_INTRUSIVE_PTR<RRMap<string, RRArray<char> > > o = AllocateEmptyRRMap<string, RRArray<char> >();
    o->insert(make_pair("testval1", stringToRRArray("Hello server")));
    return o;
}

void RobotRaconteurTest_testroot::set_is_str2(
    const RR_INTRUSIVE_PTR<RobotRaconteur::RRMap<std::string, RobotRaconteur::RRArray<char> > >& value)
{
    ThrowIfFailures thrower;
    EXPECT_TRUE(value);
    thrower.Throw();
    EXPECT_EQ(value->size(), 1);
    EXPECT_NO_THROW(EXPECT_EQ(RRArrayToString(value->at("testval2")), "Hello client"));
    thrower.Throw();
}

RR_INTRUSIVE_PTR<RobotRaconteur::RRMap<int32_t, teststruct2> > RobotRaconteurTest_testroot::get_is_struct1()
{
    RR_INTRUSIVE_PTR<RRMap<int32_t, teststruct2> > o = AllocateEmptyRRMap<int32_t, teststruct2>();
    RR_INTRUSIVE_PTR<teststruct2> s(new teststruct2());
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > is_struct1_1a =
        GetTestRRArray<double>("ts1.is_struct1.to_client");
    s->mydat = is_struct1_1a;
    o->insert(make_pair(748, s));
    return o;
}

void RobotRaconteurTest_testroot::set_is_struct1(
    const RR_INTRUSIVE_PTR<RobotRaconteur::RRMap<int32_t, teststruct2> >& value)
{
    ThrowIfFailures thrower;
    EXPECT_TRUE(value);
    thrower.Throw();
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > is_struct1_2_1a =
        GetTestRRArray<double>("ts1.is_struct1.to_service");
    EXPECT_NO_THROW(EXPECT_EQ(value->size(), 1));
    EXPECT_NO_THROW(EXPECT_RRARRAY_EQ(value->at(372)->mydat, is_struct1_2_1a));
    thrower.Throw();
}

RR_INTRUSIVE_PTR<RobotRaconteur::RRMap<std::string, teststruct2> > RobotRaconteurTest_testroot::get_is_struct2()
{
    RR_INTRUSIVE_PTR<RRMap<string, teststruct2> > o = AllocateEmptyRRMap<string, teststruct2>();
    RR_INTRUSIVE_PTR<teststruct2> s(new teststruct2());
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > is_struct2_1a =
        GetTestRRArray<double>("ts1.is_struct2.to_client");
    s->mydat = is_struct2_1a;
    o->insert(make_pair("testval1", s));
    return o;
}

void RobotRaconteurTest_testroot::set_is_struct2(
    const RR_INTRUSIVE_PTR<RobotRaconteur::RRMap<std::string, teststruct2> >& value)
{
    ThrowIfFailures thrower;
    EXPECT_TRUE(value);
    thrower.Throw();
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > is_struct2_2_1a =
        GetTestRRArray<double>("ts1.is_struct2.to_service");
    EXPECT_NO_THROW(EXPECT_EQ(value->size(), 1));
    EXPECT_NO_THROW(EXPECT_RRARRAY_EQ(value->at("testval2")->mydat, is_struct2_2_1a));
    thrower.Throw();
}

// Lists

RR_INTRUSIVE_PTR<RobotRaconteur::RRList<RobotRaconteur::RRArray<double> > > RobotRaconteurTest_testroot::get_list_d1()
{
    RR_INTRUSIVE_PTR<RRList<RRArray<double> > > o = AllocateEmptyRRList<RRArray<double> >();
    o->push_back(ScalarToRRArray(1.643392e-01));
    o->push_back(ScalarToRRArray(1.537133e+09));
    o->push_back(ScalarToRRArray(1.369505e-03));
    return o;
}

void RobotRaconteurTest_testroot::set_list_d1(
    const RR_INTRUSIVE_PTR<RobotRaconteur::RRList<RobotRaconteur::RRArray<double> > >& value)
{
    ThrowIfFailures thrower;
    EXPECT_TRUE(value);
    thrower.Throw();
    EXPECT_NO_THROW(ASSERT_EQ(value->size(), 3));
    EXPECT_NO_THROW(EXPECT_EQ(RRArrayToScalar(value->front()), 4.074501e-07));
    EXPECT_NO_THROW(EXPECT_EQ(RRArrayToScalar(*(++value->begin())), -4.535303e+05));
    EXPECT_NO_THROW(EXPECT_EQ(RRArrayToScalar(*(++(++value->begin()))), -2.956241e-20));
    thrower.Throw();
}

RR_INTRUSIVE_PTR<RobotRaconteur::RRList<RobotRaconteur::RRArray<double> > > RobotRaconteurTest_testroot::get_list_d3()
{
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > a = GetTestRRArray<double>("ts1.is_d3.to_client.a");
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > b = GetTestRRArray<double>("ts1.is_d3.to_client.b");

    RR_INTRUSIVE_PTR<RRList<RRArray<double> > > o = AllocateEmptyRRList<RRArray<double> >();
    o->push_back(a);
    o->push_back(b);
    return o;
}

void RobotRaconteurTest_testroot::set_list_d3(
    const RR_INTRUSIVE_PTR<RobotRaconteur::RRList<RobotRaconteur::RRArray<double> > >& value)
{
    ThrowIfFailures thrower;
    EXPECT_TRUE(value);
    thrower.Throw();
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > is_d3_2a = GetTestRRArray<double>("ts1.is_d3.to_service.a");
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > is_d3_2b = GetTestRRArray<double>("ts1.is_d3.to_service.b");

    EXPECT_NO_THROW(EXPECT_EQ(value->size(), 2));
    thrower.Throw();
    EXPECT_NO_THROW(EXPECT_RRARRAY_EQ(value->front(), is_d3_2a));
    EXPECT_NO_THROW(EXPECT_RRARRAY_EQ(*(++value->begin()), is_d3_2b));
    thrower.Throw();
}

RR_INTRUSIVE_PTR<RobotRaconteur::RRList<RobotRaconteur::RRMultiDimArray<double> > > RobotRaconteurTest_testroot::
    get_list_d5()
{
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<uint32_t> > is_d5_1_1a =
        GetTestRRArray<uint32_t>("ts1.is_d5.to_client.dims");
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > is_d5_1_1b = GetTestRRArray<double>("ts1.is_d5.to_client");

    RR_INTRUSIVE_PTR<RRMultiDimArray<double> > a = AllocateRRMultiDimArray<double>(is_d5_1_1a, is_d5_1_1b);
    RR_INTRUSIVE_PTR<RRList<RRMultiDimArray<double> > > o = AllocateEmptyRRList<RRMultiDimArray<double> >();
    o->push_back(a);
    return o;
}

void RobotRaconteurTest_testroot::set_list_d5(
    const RR_INTRUSIVE_PTR<RobotRaconteur::RRList<RobotRaconteur::RRMultiDimArray<double> > >& value)
{
    ThrowIfFailures thrower;
    EXPECT_TRUE(value);
    thrower.Throw();
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<uint32_t> > is_d5_2_1a =
        GetTestRRArray<uint32_t>("ts1.is_d5.to_service.dims");
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > is_d5_2_1b = GetTestRRArray<double>("ts1.is_d5.to_service");

    EXPECT_NO_THROW(EXPECT_EQ(value->size(), 1));
    RR_INTRUSIVE_PTR<RRMultiDimArray<double> > a = value->front();
    EXPECT_NO_THROW(EXPECT_RRARRAY_EQ(a->Dims, is_d5_2_1a));
    EXPECT_NO_THROW(EXPECT_RRARRAY_EQ(a->Array, is_d5_2_1b));
    thrower.Throw();
}

RR_INTRUSIVE_PTR<RobotRaconteur::RRList<RobotRaconteur::RRArray<char> > > RobotRaconteurTest_testroot::get_list_str1()
{
    RR_INTRUSIVE_PTR<RRList<RRArray<char> > > o = AllocateEmptyRRList<RRArray<char> >();
    o->push_back(stringToRRArray("Hello server"));
    return o;
}

void RobotRaconteurTest_testroot::set_list_str1(
    const RR_INTRUSIVE_PTR<RobotRaconteur::RRList<RobotRaconteur::RRArray<char> > >& value)
{
    ThrowIfFailures thrower;
    EXPECT_TRUE(value);
    thrower.Throw();
    EXPECT_NO_THROW(EXPECT_EQ(value->size(), 1));
    EXPECT_NO_THROW(EXPECT_EQ(RRArrayToString(value->front()), "Hello client"));
    thrower.Throw();
}

RR_INTRUSIVE_PTR<RobotRaconteur::RRList<teststruct2> > RobotRaconteurTest_testroot::get_list_struct1()
{
    RR_INTRUSIVE_PTR<RRList<teststruct2> > o = AllocateEmptyRRList<teststruct2>();
    RR_INTRUSIVE_PTR<teststruct2> s(new teststruct2());
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > is_struct1_1a =
        GetTestRRArray<double>("ts1.is_struct1.to_client");
    s->mydat = is_struct1_1a;
    o->push_back(s);
    return o;
}

void RobotRaconteurTest_testroot::set_list_struct1(const RR_INTRUSIVE_PTR<RobotRaconteur::RRList<teststruct2> >& value)
{
    ThrowIfFailures thrower;
    EXPECT_TRUE(value);
    thrower.Throw();
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > is_struct1_2_1a =
        GetTestRRArray<double>("ts1.is_struct1.to_service");
    EXPECT_NO_THROW(EXPECT_EQ(value->size(), 1));
    thrower.Throw();
    EXPECT_NO_THROW(EXPECT_RRARRAY_EQ(value->front()->mydat, is_struct1_2_1a));
    thrower.Throw();
}

// End lists

RR_INTRUSIVE_PTR<com::robotraconteur::testing::TestService2::ostruct2> RobotRaconteurTest_testroot::get_struct3()
{
    RR_INTRUSIVE_PTR<com::robotraconteur::testing::TestService2::ostruct2> o(
        new com::robotraconteur::testing::TestService2::ostruct2());

    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > struct3a = GetTestRRArray<double>("ts1.struct3.to_client");
    o->a1 = struct3a;

    return o;
}

void RobotRaconteurTest_testroot::set_struct3(
    const RR_INTRUSIVE_PTR<com::robotraconteur::testing::TestService2::ostruct2>& value)
{
    ThrowIfFailures thrower;
    EXPECT_TRUE(value);
    thrower.Throw();
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > struct3_1a = GetTestRRArray<double>("ts1.struct3.to_service");
    EXPECT_NO_THROW(EXPECT_RRARRAY_EQ(value->a1, struct3_1a));
    thrower.Throw();
}

RR_INTRUSIVE_PTR<RobotRaconteur::RRValue> RobotRaconteurTest_testroot::get_var1() { return var1; }

void RobotRaconteurTest_testroot::set_var1(const RR_INTRUSIVE_PTR<RobotRaconteur::RRValue>& value) { var1 = value; }

RR_INTRUSIVE_PTR<RobotRaconteur::RRMap<int32_t, RobotRaconteur::RRValue> > RobotRaconteurTest_testroot::get_var2()
{
    return var2;
}

void RobotRaconteurTest_testroot::set_var2(
    const RR_INTRUSIVE_PTR<RobotRaconteur::RRMap<int32_t, RobotRaconteur::RRValue> >& value)
{
    var2 = value;
}

RR_INTRUSIVE_PTR<RobotRaconteur::RRValue> RobotRaconteurTest_testroot::get_var_num()
{
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<int32_t> > var_numa = GetTestRRArray<int32_t>("ts1.var_num.to_client");
    return (var_numa);
}

void RobotRaconteurTest_testroot::set_var_num(const RR_INTRUSIVE_PTR<RobotRaconteur::RRValue>& value)
{
    ThrowIfFailures thrower;
    EXPECT_TRUE(value);
    thrower.Throw();
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<int32_t> > var_numb = GetTestRRArray<int32_t>("ts1.var_num.to_service");
    EXPECT_NO_THROW(EXPECT_RRARRAY_EQ(rr_cast<RRArray<int32_t> >(value), var_numb));
    thrower.Throw();
}

RR_INTRUSIVE_PTR<RobotRaconteur::RRValue> RobotRaconteurTest_testroot::get_var_str()
{
    return stringToRRArray("Hello Client!");
}

void RobotRaconteurTest_testroot::set_var_str(const RR_INTRUSIVE_PTR<RobotRaconteur::RRValue>& value)
{
    ThrowIfFailures thrower;
    EXPECT_TRUE(value);
    thrower.Throw();
    if (RRArrayToString(rr_cast<RRArray<char> >(value)) != "Hello Server!")
        throw std::exception();
    thrower.Throw();
}

RR_INTRUSIVE_PTR<RobotRaconteur::RRValue> RobotRaconteurTest_testroot::get_var_struct()
{
    RR_INTRUSIVE_PTR<teststruct2> o(new teststruct2());
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > var_structa = GetTestRRArray<double>("ts1.var_struct.to_client");
    o->mydat = var_structa;
    return o;
}

void RobotRaconteurTest_testroot::set_var_struct(const RR_INTRUSIVE_PTR<RobotRaconteur::RRValue>& value)
{
    ThrowIfFailures thrower;
    EXPECT_TRUE(value);
    thrower.Throw();
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > var_struct1_1a =
        GetTestRRArray<double>("ts1.var_struct.to_service");
    EXPECT_NO_THROW(EXPECT_RRARRAY_EQ(rr_cast<teststruct2>(value)->mydat, var_struct1_1a));
    thrower.Throw();
}

RR_INTRUSIVE_PTR<RobotRaconteur::RRValue> RobotRaconteurTest_testroot::get_var_vector()
{
    RR_INTRUSIVE_PTR<RRMap<int32_t, RRValue> > d = AllocateEmptyRRMap<int32_t, RRValue>();
    d->insert(make_pair(10, stringToRRArray("Hello Client!")));
    return d;
}

void RobotRaconteurTest_testroot::set_var_vector(const RR_INTRUSIVE_PTR<RobotRaconteur::RRValue>& value)
{
    ThrowIfFailures thrower;
    EXPECT_TRUE(value);
    thrower.Throw();
    RR_INTRUSIVE_PTR<RRMap<int32_t, RRValue> > d = rr_cast<RRMap<int32_t, RRValue> >(value);
    EXPECT_NO_THROW(EXPECT_EQ(d->size(), 1));
    EXPECT_NO_THROW(EXPECT_EQ(RRArrayToString(rr_cast<RRArray<char> >(d->at(11))), "Hello Server!"));
    thrower.Throw();
}

RR_INTRUSIVE_PTR<RobotRaconteur::RRValue> RobotRaconteurTest_testroot::get_var_dictionary()
{
    RR_INTRUSIVE_PTR<RRMap<string, RRValue> > d = AllocateEmptyRRMap<string, RRValue>();
    d->insert(make_pair("test1", stringToRRArray("Hello Client!")));
    return d;
}

void RobotRaconteurTest_testroot::set_var_dictionary(const RR_INTRUSIVE_PTR<RobotRaconteur::RRValue>& value)
{
    ThrowIfFailures thrower;
    EXPECT_TRUE(value);
    thrower.Throw();
    RR_INTRUSIVE_PTR<RRMap<string, RRValue> > d = rr_cast<RRMap<string, RRValue> >(value);
    EXPECT_NO_THROW(EXPECT_EQ(d->size(), 1));
    EXPECT_NO_THROW(EXPECT_EQ(RRArrayToString(rr_cast<RRArray<char> >(d->at("test2"))), "Hello Server!"));
    thrower.Throw();
}

RR_INTRUSIVE_PTR<RobotRaconteur::RRValue> RobotRaconteurTest_testroot::get_var_list()
{
    RR_INTRUSIVE_PTR<RRList<RRValue> > d = AllocateEmptyRRList<RRValue>();
    d->push_back(stringToRRArray("Hello Client!"));
    return d;
}

void RobotRaconteurTest_testroot::set_var_list(const RR_INTRUSIVE_PTR<RobotRaconteur::RRValue>& value)
{
    ThrowIfFailures thrower;
    EXPECT_TRUE(value);
    thrower.Throw();
    RR_INTRUSIVE_PTR<RRList<RRValue> > d = rr_cast<RRList<RRValue> >(value);
    EXPECT_NO_THROW(EXPECT_EQ(d->size(), 1));
    EXPECT_NO_THROW(EXPECT_EQ(RRArrayToString(rr_cast<RRArray<char> >(d->front())), "Hello Server!"));
    thrower.Throw();
}

RR_INTRUSIVE_PTR<RobotRaconteur::RRValue> RobotRaconteurTest_testroot::get_var_multidimarray()
{

    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<uint32_t> > var_a_1a =
        GetTestRRArray<uint32_t>("ts1.var_multidimarray.to_client.dims");
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > var_a_1b =
        GetTestRRArray<double>("ts1.var_multidimarray.to_client");
    RR_INTRUSIVE_PTR<RRMultiDimArray<double> > var_a_1 = AllocateRRMultiDimArray<double>(var_a_1a, var_a_1b);

    return var_a_1;
}

void RobotRaconteurTest_testroot::set_var_multidimarray(const RR_INTRUSIVE_PTR<RobotRaconteur::RRValue>& value)
{
    ThrowIfFailures thrower;
    EXPECT_TRUE(value);
    thrower.Throw();
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<uint32_t> > var_a_2a =
        GetTestRRArray<uint32_t>("ts1.var_multidimarray.to_service.dims");
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > var_a_2b =
        GetTestRRArray<double>("ts1.var_multidimarray.to_service");

    RR_INTRUSIVE_PTR<RRMultiDimArray<double> > a = rr_cast<RRMultiDimArray<double> >(value);
    EXPECT_NO_THROW(EXPECT_RRARRAY_EQ(a->Dims, var_a_2a));
    EXPECT_NO_THROW(EXPECT_RRARRAY_EQ(a->Array, var_a_2b));
    thrower.Throw();
}

double RobotRaconteurTest_testroot::get_errtest() { throw runtime_error("This is a test"); }

void RobotRaconteurTest_testroot::set_errtest(double value) { throw runtime_error("This is a test"); }

RR_INTRUSIVE_PTR<teststruct1> RobotRaconteurTest_testroot::get_nulltest() { return RR_INTRUSIVE_PTR<teststruct1>(); }

void RobotRaconteurTest_testroot::set_nulltest(const RR_INTRUSIVE_PTR<teststruct1>& value)
{
    ThrowIfFailures thrower;
    EXPECT_FALSE(value);
    thrower.Throw();
}

void RobotRaconteurTest_testroot::func1()
{
    RR_SHARED_PTR<Timer> t = ServerContext::GetCurrentServerContext()->GetNode()->CreateTimer(
        boost::posix_time::milliseconds(100),
        boost::bind(&RobotRaconteurTest_testroot::func1_thread, shared_from_this()), true);
    t->Start();
}

void RobotRaconteurTest_testroot::func1_thread() { get_ev1()(); }

void RobotRaconteurTest_testroot::func2(double d1, double d2)
{
    RR_SHARED_PTR<Timer> t = ServerContext::GetCurrentServerContext()->GetNode()->CreateTimer(
        boost::posix_time::milliseconds(100),
        boost::bind(&RobotRaconteurTest_testroot::func2_thread, shared_from_this(), d1, d2), true);
    t->Start();
}

void RobotRaconteurTest_testroot::func2_thread(double d1, double d2)
{
    boost::this_thread::sleep(boost::posix_time::milliseconds(100));
    RR_INTRUSIVE_PTR<teststruct2> s(new teststruct2());
    s->mydat = ScalarToRRArray(d2);
    get_ev2()(d1, s);
}

double RobotRaconteurTest_testroot::func3(double d1, double d2)
{

    try
    {
        bool isconnectionsecure = tcptransport->IsTransportConnectionSecure(ServerEndpoint::GetCurrentEndpoint());

        if (isconnectionsecure)
        {
            bool ispeerverified = tcptransport->IsSecurePeerIdentityVerified(ServerEndpoint::GetCurrentEndpoint());
            if (!ispeerverified)
            {
                cout << "Peer identity is not verified" << endl;
            }
            else
            {
                cout << "Peer identity is: "
                     << tcptransport->GetSecurePeerIdentity(ServerEndpoint::GetCurrentEndpoint()) << endl;
            }
        }
    }
    catch (std::exception&)
    {}
    return d1 + d2;
}

int32_t RobotRaconteurTest_testroot::meaning_of_life() { return 42; }

void RobotRaconteurTest_testroot::func_errtest() { throw runtime_error("This is a test"); }

void RobotRaconteurTest_testroot::func_errtest1() { throw DataTypeException("This is a test 1"); }

void RobotRaconteurTest_testroot::func_errtest2() { throw testexception1("This is a test 2"); }

void RobotRaconteurTest_testroot::func_errtest3() { throw testexception3("This is a test 3"); }

boost::signals2::signal<void()>& RobotRaconteurTest_testroot::get_ev1() { return ev1; }

boost::signals2::signal<void(double, RR_INTRUSIVE_PTR<teststruct2>)>& RobotRaconteurTest_testroot::get_ev2()
{
    return ev2;
}

RR_SHARED_PTR<sub1> RobotRaconteurTest_testroot::get_o1() { return o1; }

RR_SHARED_PTR<sub1> RobotRaconteurTest_testroot::get_o2(int32_t ind)
{
    boost::recursive_mutex::scoped_lock lock(o2_lock);
    if (o2.count(ind) != 0)
        return o2.at(ind);
    RR_SHARED_PTR<sub1_impl> o = RR_MAKE_SHARED<sub1_impl>();
    o->set_i_ind(ind);
    o2.insert(make_pair(ind, o));
    return o;
}

RR_SHARED_PTR<sub1> RobotRaconteurTest_testroot::get_o3(int32_t ind)
{
    boost::recursive_mutex::scoped_lock lock(o3_lock);
    if (o3.count(ind) != 0)
        return o3.at(ind);
    RR_SHARED_PTR<sub1_impl> o = RR_MAKE_SHARED<sub1_impl>();
    o->set_i_ind(ind);
    o3.insert(make_pair(ind, o));
    return o;
}

RR_SHARED_PTR<sub1> RobotRaconteurTest_testroot::get_o4(const std::string& ind)
{
    boost::recursive_mutex::scoped_lock lock(o3_lock);
    if (o4.count(ind) != 0)
        return o4.at(ind);
    RR_SHARED_PTR<sub1_impl> o = RR_MAKE_SHARED<sub1_impl>();
    o->set_s_ind(ind);
    o4.insert(make_pair(ind, o));
    return o;
}

RR_SHARED_PTR<com::robotraconteur::testing::TestService2::subobj> RobotRaconteurTest_testroot::get_o5() { return o5; }

RR_SHARED_PTR<RobotRaconteur::RRObject> RobotRaconteurTest_testroot::get_o6()
{
    boost::mutex::scoped_lock lock(o6_lock);
    return o6;
}

void RobotRaconteurTest_testroot::o6_op(int32_t op)
{
    boost::mutex::scoped_lock lock(o6_lock);
    try
    {
        ServerContext::GetCurrentServerContext()->ReleaseServicePath(ServerContext::GetCurrentServicePath() + ".o6");
    }
    catch (std::exception&)
    {
        // std::cout << "Error: " << e.what() << std::endl;
    };

    switch (op)
    {
    case 0:
        o6 = RR_MAKE_SHARED<sub1_impl>();
        break;
    case 1:
        o6 = RR_MAKE_SHARED<sub2_impl>();
        break;
    case 2:
        o6 = RR_MAKE_SHARED<TestService2SubObj>();
        break;
    default:
        throw std::exception();
    }
}

RR_SHARED_PTR<RobotRaconteur::Pipe<RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > > > RobotRaconteurTest_testroot::
    get_p1()
{
    return p1;
}

void RobotRaconteurTest_testroot::set_p1(
    const RR_SHARED_PTR<RobotRaconteur::Pipe<RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > > >& value)
{
    p1 = value;
    p1->SetPipeConnectCallback(
        boost::bind(&RobotRaconteurTest_testroot::p1_connect_callback, this, RR_BOOST_PLACEHOLDERS(_1)));
}

void RobotRaconteurTest_testroot::p1_connect_callback(
    RR_SHARED_PTR<PipeEndpoint<RR_INTRUSIVE_PTR<RRArray<double> > > > p)
{
    p->PacketReceivedEvent.connect(
        boost::bind(&RobotRaconteurTest_testroot::p1_packet_received, this, RR_BOOST_PLACEHOLDERS(_1)));
    p->PacketAckReceivedEvent.connect(boost::bind(&RobotRaconteurTest_testroot::p1_packet_ack_received, this,
                                                  RR_BOOST_PLACEHOLDERS(_1), RR_BOOST_PLACEHOLDERS(_2)));

    p->SetRequestPacketAck(true);
}

void RobotRaconteurTest_testroot::p1_packet_received(
    RR_SHARED_PTR<PipeEndpoint<RR_INTRUSIVE_PTR<RRArray<double> > > > p)
{
    RR_SHARED_PTR<RobotRaconteurNode> node = ServerContext::GetCurrentServerContext()->GetNode();
    RobotRaconteurNode::TryPostToThreadPool(
        node, boost::bind(&RobotRaconteurTest_testroot::p1_packet_received_threadfunc, shared_from_this(), p));
}

void RobotRaconteurTest_testroot::p1_packet_received_threadfunc(
    RR_SHARED_PTR<PipeEndpoint<RR_INTRUSIVE_PTR<RRArray<double> > > > p)
{
    boost::this_thread::sleep(boost::posix_time::milliseconds(10));
    boost::recursive_mutex::scoped_lock lock(p1_lock);
    try
    {
        while (p->Available() > 0)
        {
            try
            {
                RR_INTRUSIVE_PTR<RRArray<double> > dat = p->ReceivePacket();
                uint32_t pnum = p->SendPacket(dat);
                if (!packet_sent)
                    packetnum = pnum;
                packet_sent = true;
            }
            catch (std::exception&)
            {}
        }
    }
    catch (std::exception&)
    {}
}

void RobotRaconteurTest_testroot::p1_packet_ack_received(
    const RR_SHARED_PTR<PipeEndpoint<RR_INTRUSIVE_PTR<RRArray<double> > > > p, uint32_t packetnum)
{
    if (packetnum == this->packetnum)
        ack_recv = true;
}

RR_SHARED_PTR<RobotRaconteur::Pipe<RR_INTRUSIVE_PTR<teststruct2> > > RobotRaconteurTest_testroot::get_p2()
{
    return p2;
}

void RobotRaconteurTest_testroot::set_p2(
    const RR_SHARED_PTR<RobotRaconteur::Pipe<RR_INTRUSIVE_PTR<teststruct2> > >& value)
{
    p2 = value;
    p2->SetPipeConnectCallback(
        boost::bind(&RobotRaconteurTest_testroot::p2_connect_callback, this, RR_BOOST_PLACEHOLDERS(_1)));
}

void RobotRaconteurTest_testroot::p2_connect_callback(RR_SHARED_PTR<PipeEndpoint<RR_INTRUSIVE_PTR<teststruct2> > > p)
{
    p->PacketReceivedEvent.connect(
        boost::bind(&RobotRaconteurTest_testroot::p2_packet_received, this, RR_BOOST_PLACEHOLDERS(_1)));
}

void RobotRaconteurTest_testroot::p2_packet_received(RR_SHARED_PTR<PipeEndpoint<RR_INTRUSIVE_PTR<teststruct2> > > p)
{
    RR_SHARED_PTR<RobotRaconteurNode> node = ServerContext::GetCurrentServerContext()->GetNode();
    RobotRaconteurNode::TryPostToThreadPool(
        node, boost::bind(&RobotRaconteurTest_testroot::p2_packet_received_threadfunc, shared_from_this(), p));
}

void RobotRaconteurTest_testroot::p2_packet_received_threadfunc(
    RR_SHARED_PTR<PipeEndpoint<RR_INTRUSIVE_PTR<teststruct2> > > p)
{
    try
    {
        boost::this_thread::sleep(boost::posix_time::milliseconds(100));
        boost::recursive_mutex::scoped_lock lock(p2_lock);
        while (p->Available() > 0)
        {
            RR_INTRUSIVE_PTR<teststruct2> dat = p->ReceivePacket();
            p->SendPacket(dat);
        }
    }
    catch (std::exception&)
    {}
}

void RobotRaconteurTest_testroot::pipe_check_error()
{
    if (!ack_recv)
        throw std::exception();
}

RR_SHARED_PTR<RobotRaconteur::Pipe<double> > RobotRaconteurTest_testroot::get_broadcastpipe()
{
    boost::mutex::scoped_lock lock(broadcastpipe_lock);
    return broadcastpipe_pipe;
}
void RobotRaconteurTest_testroot::set_broadcastpipe(const RR_SHARED_PTR<RobotRaconteur::Pipe<double> >& value)
{
    boost::mutex::scoped_lock lock(broadcastpipe_lock);
    if (broadcastpipe_pipe)
        throw std::runtime_error("Pipe already set");
    broadcastpipe_pipe = value;
    broadcastpipe = RR_MAKE_SHARED<PipeBroadcaster<double> >();
    broadcastpipe->Init(value, 3);

    RR_WEAK_PTR<RobotRaconteurTest_testroot> w = shared_from_this();
    broadcastpipe_timer = ServerContext::GetCurrentServerContext()->GetNode()->CreateTimer(
        boost::posix_time::milliseconds(100),
        boost::bind(&RobotRaconteurTest_testroot::broadcastpipe_timer_handler, w, RR_BOOST_PLACEHOLDERS(_1)));
    broadcastpipe_timer->Start();
}

static void pipe_empty_handler() {}

void RobotRaconteurTest_testroot::broadcastpipe_timer_handler(const RR_WEAK_PTR<RobotRaconteurTest_testroot> obj,
                                                              const TimerEvent& ev)
{
    RR_SHARED_PTR<RobotRaconteurTest_testroot> obj1 = obj.lock();
    if (!obj1)
        return;

    for (size_t i = 0; i < 100; i++)
    {
        obj1->broadcastpipe->AsyncSendPacket((double)i, boost::bind(&pipe_empty_handler));
    }
}

RR_SHARED_PTR<RobotRaconteur::Callback<boost::function<void()> > > RobotRaconteurTest_testroot::get_cb1()
{
    return cb1;
}

void RobotRaconteurTest_testroot::set_cb1(
    const RR_SHARED_PTR<RobotRaconteur::Callback<boost::function<void()> > >& value)
{
    cb1 = value;
}

RR_SHARED_PTR<RobotRaconteur::Callback<boost::function<void(double, double)> > > RobotRaconteurTest_testroot::get_cb2()
{
    return cb2;
}

void RobotRaconteurTest_testroot::set_cb2(
    const RR_SHARED_PTR<RobotRaconteur::Callback<boost::function<void(double, double)> > >& value)
{
    cb2 = value;
}

RR_SHARED_PTR<RobotRaconteur::Callback<boost::function<double(double, double)> > > RobotRaconteurTest_testroot::
    get_cb3()
{
    return cb3;
}

void RobotRaconteurTest_testroot::set_cb3(
    const RR_SHARED_PTR<RobotRaconteur::Callback<boost::function<double(double, double)> > >& value)
{
    cb3 = value;
}

RR_SHARED_PTR<RobotRaconteur::Callback<boost::function<int32_t()> > > RobotRaconteurTest_testroot::
    get_cb_meaning_of_life()
{
    return cb_mol;
}

void RobotRaconteurTest_testroot::set_cb_meaning_of_life(
    const RR_SHARED_PTR<RobotRaconteur::Callback<boost::function<int32_t()> > >& value)
{
    cb_mol = value;
}

RR_SHARED_PTR<RobotRaconteur::Callback<boost::function<void()> > > RobotRaconteurTest_testroot::get_cb_errtest()
{
    return cb_err;
}

void RobotRaconteurTest_testroot::set_cb_errtest(
    const RR_SHARED_PTR<RobotRaconteur::Callback<boost::function<void()> > >& value)
{
    cb_err = value;
}

void RobotRaconteurTest_testroot::test_callbacks()
{
    ThrowIfFailures thrower;
    get_cb1()->GetClientFunction(ServerEndpoint::GetCurrentEndpoint())();
    get_cb2()->GetClientFunction(ServerEndpoint::GetCurrentEndpoint())(739.2, 0.392);
    double res = get_cb3()->GetClientFunction(ServerEndpoint::GetCurrentEndpoint())(34, 45);
    EXPECT_NO_THROW(EXPECT_EQ(res, (34 + 45 + 3.14)));
    EXPECT_NO_THROW(EXPECT_EQ(get_cb_meaning_of_life()->GetClientFunction(ServerEndpoint::GetCurrentEndpoint())(), 42));

    EXPECT_THROW(get_cb_errtest()->GetClientFunction(ServerEndpoint::GetCurrentEndpoint())(), std::exception);

    thrower.Throw();
}

RR_SHARED_PTR<RobotRaconteur::Wire<RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > > > RobotRaconteurTest_testroot::
    get_w1()
{
    return w1;
}

void RobotRaconteurTest_testroot::set_w1(
    const RR_SHARED_PTR<RobotRaconteur::Wire<RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > > >& value)
{
    w1 = value;
    w1->SetWireConnectCallback(
        boost::bind(&RobotRaconteurTest_testroot::w1_connect_callback, this, RR_BOOST_PLACEHOLDERS(_1)));
}

void RobotRaconteurTest_testroot::w1_connect_callback(
    RR_SHARED_PTR<WireConnection<RR_INTRUSIVE_PTR<RRArray<double> > > > wire)
{
    wire->WireValueChanged.connect(boost::bind(&RobotRaconteurTest_testroot::w1_value_changed, this,
                                               RR_BOOST_PLACEHOLDERS(_1), RR_BOOST_PLACEHOLDERS(_2),
                                               RR_BOOST_PLACEHOLDERS(_3)));
}

void RobotRaconteurTest_testroot::w1_value_changed(
    const RR_SHARED_PTR<WireConnection<RR_INTRUSIVE_PTR<RRArray<double> > > > wire,
    RR_INTRUSIVE_PTR<RRArray<double> > value, TimeSpec time)
{
    try
    {
        wire->SetOutValue(wire->GetInValue());
    }
    catch (std::exception&)
    {}
}

RR_SHARED_PTR<RobotRaconteur::Wire<RR_INTRUSIVE_PTR<teststruct2> > > RobotRaconteurTest_testroot::get_w2()
{
    return w2;
}

void RobotRaconteurTest_testroot::set_w2(
    const RR_SHARED_PTR<RobotRaconteur::Wire<RR_INTRUSIVE_PTR<teststruct2> > >& value)
{
    w2 = value;
    w2->SetWireConnectCallback(
        boost::bind(&RobotRaconteurTest_testroot::w2_connect_callback, this, RR_BOOST_PLACEHOLDERS(_1)));
}

void RobotRaconteurTest_testroot::w2_connect_callback(
    RR_SHARED_PTR<WireConnection<RR_INTRUSIVE_PTR<teststruct2> > > wire)
{
    wire->WireValueChanged.connect(boost::bind(&RobotRaconteurTest_testroot::w2_value_changed, this,
                                               RR_BOOST_PLACEHOLDERS(_1), RR_BOOST_PLACEHOLDERS(_2),
                                               RR_BOOST_PLACEHOLDERS(_3)));
}

void RobotRaconteurTest_testroot::w2_value_changed(
    const RR_SHARED_PTR<WireConnection<RR_INTRUSIVE_PTR<teststruct2> > > wire, RR_INTRUSIVE_PTR<teststruct2> value,
    TimeSpec time)
{
    try
    {
        wire->SetOutValue(wire->GetInValue());
    }
    catch (std::exception&)
    {}
}

RR_SHARED_PTR<RobotRaconteur::Wire<RR_INTRUSIVE_PTR<RobotRaconteur::RRMultiDimArray<int32_t> > > >
RobotRaconteurTest_testroot::get_w3()
{
    return w3;
}

void RobotRaconteurTest_testroot::set_w3(
    const RR_SHARED_PTR<RobotRaconteur::Wire<RR_INTRUSIVE_PTR<RobotRaconteur::RRMultiDimArray<int32_t> > > >& value)
{
    w3 = value;
    w3->SetWireConnectCallback(
        boost::bind(&RobotRaconteurTest_testroot::w3_connect_callback, this, RR_BOOST_PLACEHOLDERS(_1)));
}

void RobotRaconteurTest_testroot::w3_connect_callback(
    RR_SHARED_PTR<WireConnection<RR_INTRUSIVE_PTR<RRMultiDimArray<int32_t> > > > wire)
{
    wire->WireValueChanged.connect(boost::bind(&RobotRaconteurTest_testroot::w3_value_changed, this,
                                               RR_BOOST_PLACEHOLDERS(_1), RR_BOOST_PLACEHOLDERS(_2),
                                               RR_BOOST_PLACEHOLDERS(_3)));
}

void RobotRaconteurTest_testroot::w3_value_changed(
    const RR_SHARED_PTR<WireConnection<RR_INTRUSIVE_PTR<RRMultiDimArray<int32_t> > > > wire,
    RR_INTRUSIVE_PTR<RRMultiDimArray<int32_t> > value, TimeSpec time)
{
    try
    {
        wire->SetOutValue(wire->GetInValue());
    }
    catch (std::exception&)
    {}
}

RR_SHARED_PTR<RobotRaconteur::Wire<double> > RobotRaconteurTest_testroot::get_broadcastwire()
{
    boost::mutex::scoped_lock lock(broadcastwire_lock);
    return broadcastwire_wire;
}

void RobotRaconteurTest_testroot::set_broadcastwire(const RR_SHARED_PTR<RobotRaconteur::Wire<double> >& value)
{
    boost::mutex::scoped_lock lock(broadcastwire_lock);
    if (broadcastwire_wire)
        throw std::runtime_error("Pipe already set");
    broadcastwire_wire = value;
    broadcastwire = RR_MAKE_SHARED<WireBroadcaster<double> >();
    broadcastwire->Init(value);

    RR_WEAK_PTR<RobotRaconteurTest_testroot> w = shared_from_this();
    broadcastwire_timer = ServerContext::GetCurrentServerContext()->GetNode()->CreateTimer(
        boost::posix_time::milliseconds(100),
        boost::bind(&RobotRaconteurTest_testroot::broadcastwire_timer_handler, w, RR_BOOST_PLACEHOLDERS(_1)));
    broadcastwire_timer->Start();
}

void RobotRaconteurTest_testroot::broadcastwire_timer_handler(const RR_WEAK_PTR<RobotRaconteurTest_testroot> obj,
                                                              const TimerEvent& ev)
{
    RR_SHARED_PTR<RobotRaconteurTest_testroot> obj1 = obj.lock();
    if (!obj1)
        return;

    for (size_t i = 0; i < 100; i++)
    {
        obj1->broadcastwire->SetOutValue((double)i);
    }
}

RR_SHARED_PTR<RobotRaconteur::ArrayMemory<double> > RobotRaconteurTest_testroot::get_m1() { return m1; }

RR_SHARED_PTR<RobotRaconteur::MultiDimArrayMemory<double> > RobotRaconteurTest_testroot::get_m2() { return m2; }

RR_SHARED_PTR<RobotRaconteur::MultiDimArrayMemory<uint8_t> > RobotRaconteurTest_testroot::get_m3() { return m3; }

void RobotRaconteurTest_testroot::Shutdown()
{
    {
        boost::mutex::scoped_lock lock(broadcastpipe_lock);

        if (broadcastpipe_timer)
            broadcastpipe_timer->TryStop();

        broadcastpipe_timer.reset();
    }
}

// sub1_impl

sub1_impl::sub1_impl() { o2_1 = RR_MAKE_SHARED<sub2_impl>(); }

RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > sub1_impl::get_d1() { return d1; }

void sub1_impl::set_d1(const RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> >& value) { d1 = value; }

RR_INTRUSIVE_PTR<RobotRaconteur::RRMultiDimArray<double> > sub1_impl::get_d2() { return d2; }

void sub1_impl::set_d2(const RR_INTRUSIVE_PTR<RobotRaconteur::RRMultiDimArray<double> >& value) { d2 = value; }

std::string sub1_impl::get_s_ind() { return s_ind; }

void sub1_impl::set_s_ind(const std::string& value) { s_ind = value; }

int32_t sub1_impl::get_i_ind() { return i_ind; }

void sub1_impl::set_i_ind(int32_t value) { i_ind = value; }

RR_SHARED_PTR<sub2> sub1_impl::get_o2_1() { return o2_1; }

RR_SHARED_PTR<sub2> sub1_impl::get_o2_2(int32_t ind)
{
    if (o2_2.count(ind) != 0)
        return o2_2.at(ind);
    RR_SHARED_PTR<sub2_impl> o = RR_MAKE_SHARED<sub2_impl>();
    o->set_i_ind(ind);
    o2_2.insert(make_pair(ind, o));
    return o;
}

RR_SHARED_PTR<sub2> sub1_impl::get_o2_3(const std::string& ind)
{
    if (o2_3.count(ind) != 0)
        return o2_3.at(ind);
    RR_SHARED_PTR<sub2_impl> o = RR_MAKE_SHARED<sub2_impl>();
    o->set_s_ind(ind);
    o2_3.insert(make_pair(ind, o));
    return o;
}

void sub1_impl::RobotRaconteurMonitorEnter() { rrlock.lock(); }

void sub1_impl::RobotRaconteurMonitorEnter(int32_t timeout)
{
    if (timeout == -1)
        RobotRaconteurMonitorEnter();
    else

        rrlock.timed_lock(boost::posix_time::milliseconds(timeout));
}

void sub1_impl::RobotRaconteurMonitorExit() { rrlock.unlock(); }

// sub2_impl

std::string sub2_impl::get_s_ind() { return s_ind; }

void sub2_impl::set_s_ind(const std::string& value) { s_ind = value; }

int32_t sub2_impl::get_i_ind() { return i_ind; }

void sub2_impl::set_i_ind(int32_t value) { i_ind = value; }

std::string sub2_impl::get_data() { return data; }

void sub2_impl::set_data(const std::string& value) { data = value; }

RR_SHARED_PTR<sub3> sub2_impl::get_o3_1(const std::string& ind)
{
    if (o3_1.count(ind) != 0)
        return o3_1.at(ind);
    RR_SHARED_PTR<sub3_impl> o = RR_MAKE_SHARED<sub3_impl>();
    o->ind = ind;
    o3_1.insert(make_pair(ind, o));
    return o;
}

// sub3 impl

std::string sub3_impl::get_ind() { return ind; }

void sub3_impl::set_ind(const std::string& value) {}

std::string sub3_impl::get_data2() { return data2; }

void sub3_impl::set_data2(const std::string& value) { data2 = value; }

double sub3_impl::get_data3() { return data3; }

void sub3_impl::set_data3(double value) { data3 = value; }

double sub3_impl::add(double d) { return d + 42; }

void sub3_impl::async_get_ind(
    boost::function<void(const std::string&, const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>&)> rr_handler,
    int32_t rr_timeout)
{
    ServerContext::GetCurrentServerContext()->GetNode()->GetThreadPool()->Post(
        boost::bind(rr_handler, ind, RR_SHARED_PTR<RobotRaconteurException>()));
}
void sub3_impl::async_set_ind(
    const std::string& value,
    boost::function<void(const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>&)> rr_handler, int32_t rr_timeout)
{
    ServerContext::GetCurrentServerContext()->GetNode()->GetThreadPool()->Post(
        boost::bind(rr_handler, RR_MAKE_SHARED<ServiceException>("Read only property")));
}

void sub3_impl::async_get_data2(
    boost::function<void(const std::string&, const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>&)> rr_handler,
    int32_t rr_timeout)
{
    ServerContext::GetCurrentServerContext()->GetNode()->GetThreadPool()->Post(
        boost::bind(rr_handler, data2, RR_SHARED_PTR<RobotRaconteurException>()));
}

void sub3_impl::async_set_data2(
    const std::string& value,
    boost::function<void(const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>&)> rr_handler, int32_t rr_timeout)
{
    data2 = value;
    ServerContext::GetCurrentServerContext()->GetNode()->GetThreadPool()->Post(
        boost::bind(rr_handler, RR_SHARED_PTR<RobotRaconteurException>()));
}

void sub3_impl::async_get_data3(
    boost::function<void(double, const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>&)> rr_handler,
    int32_t rr_timeout)
{
    ServerContext::GetCurrentServerContext()->GetNode()->GetThreadPool()->Post(
        boost::bind(rr_handler, data3, RR_SHARED_PTR<RobotRaconteurException>()));
}
void sub3_impl::async_set_data3(
    double value, boost::function<void(const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>&)> rr_handler,
    int32_t rr_timeout)
{
    data3 = value;
    ServerContext::GetCurrentServerContext()->GetNode()->GetThreadPool()->Post(
        boost::bind(rr_handler, RR_SHARED_PTR<RobotRaconteurException>()));
}

void sub3_impl::async_add(
    double d, boost::function<void(double, const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>&)> rr_handler,
    int32_t rr_timeout)
{
    double res = d + 42;
    ServerContext::GetCurrentServerContext()->GetNode()->GetThreadPool()->Post(
        boost::bind(rr_handler, res, RR_SHARED_PTR<RobotRaconteurException>()));
}

// TestService2SubObj

double TestService2SubObj::add_val(double v) { return v + 1; }

// support

void RobotRaconteurTestServiceSupport::RegisterServices(RR_SHARED_PTR<TcpTransport> tcptransport,
                                                        RR_SHARED_PTR<RobotRaconteurNode> node)
{
    if (!node)
    {
        node = RobotRaconteurNode::sp();
    }
    testservice = RR_MAKE_SHARED<RobotRaconteurTest_testroot>(tcptransport);
    testservice_auth = RR_MAKE_SHARED<RobotRaconteurTest_testroot>(tcptransport);

    RR_SHARED_PTR<ServerContext> c =
        node->RegisterService("RobotRaconteurTestService", "com.robotraconteur.testing.TestService1", testservice);

    std::map<std::string, RR_INTRUSIVE_PTR<RRValue> > m;
    m.insert(make_pair("test", stringToRRArray("This is a test attribute")));
    m.insert(make_pair("test2", ScalarToRRArray<int32_t>(42)));
    c->SetAttributes(m);

    c->LogCandidateConnectionURLs();

    string authdata =
        "testuser1 0b91dec4fe98266a03b136b59219d0d6 objectlock\ntestuser2 841c4221c2e7e0cefbc0392a35222512 "
        "objectlock\ntestsuperuser 503ed776c50169f681ad7bbc14198b68 objectlock,objectlockoverride";
    RR_SHARED_PTR<PasswordFileUserAuthenticator> p = RR_MAKE_SHARED<PasswordFileUserAuthenticator>(authdata);
    map<string, string> policies;
    policies.insert(make_pair("requirevaliduser", "true"));
    policies.insert(make_pair("allowobjectlock", "true"));

    RR_SHARED_PTR<ServiceSecurityPolicy> s = RR_MAKE_SHARED<ServiceSecurityPolicy>(p, policies);

    node->RegisterService("RobotRaconteurTestService_auth", "com.robotraconteur.testing.TestService1", testservice_auth,
                          s);
}

void RobotRaconteurTestServiceSupport::RegisterSecureServices(RR_SHARED_PTR<TcpTransport> tcptransport,
                                                              const std::vector<NodeID>& allowed_clients)
{
    testservice_auth = RR_MAKE_SHARED<RobotRaconteurTest_testroot>(tcptransport);

    std::vector<std::string> str_clients;
    BOOST_FOREACH (NodeID n, allowed_clients)
    {
        str_clients.push_back(n.ToString());
    }

    string authdata = "testuser1 0b91dec4fe98266a03b136b59219d0d6 objectlock " + boost::join(str_clients, ",") + "\n";
    RR_SHARED_PTR<PasswordFileUserAuthenticator> p = RR_MAKE_SHARED<PasswordFileUserAuthenticator>(authdata, true);
    map<string, string> policies;
    policies.insert(make_pair("requirevaliduser", "true"));
    policies.insert(make_pair("allowobjectlock", "true"));

    RR_SHARED_PTR<ServiceSecurityPolicy> s = RR_MAKE_SHARED<ServiceSecurityPolicy>(p, policies);

    RobotRaconteurNode::s()->RegisterService("RobotRaconteurTestService_auth",
                                             "com.robotraconteur.testing.TestService1", testservice_auth, s);
}

void RobotRaconteurTestServiceSupport::UnregisterServices()
{
    RobotRaconteurNode::s()->CloseService("RobotRaconteurTestService");
    RobotRaconteurNode::s()->CloseService("RobotRaconteurTestService_auth");
}

} // namespace RobotRaconteurTest
