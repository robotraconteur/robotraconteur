//This file is automatically generated. DO NOT EDIT!

#include <RobotRaconteur.h>
#include <boost/signals2.hpp>
#include <boost/array.hpp>
#include <boost/container/static_vector.hpp>
#include "com__robotraconteur__testing__TestService1.h"
#pragma once

namespace com
{
namespace robotraconteur
{
namespace testing
{
namespace TestService3
{

class teststruct3;
class testroot3;
class obj1;
class obj2;
class obj3;
class obj4;
class obj5;

namespace com__robotraconteur__testing__TestService3Constants 
{
    static const char* strconst="This is a\n \"string constant\" \\/\b\f \r\xc3\xbf tabme\ttabme\n smile! \xf0\x9f\x98\x81";
    static const int32_t int32const=3856384;
    static const int32_t int32const_array[]={182476, 56483, -2947};
    static const int32_t int32hexconst=0x082bc7;
    static const int32_t int32hexconst2=-0x7264c17;
    static const int32_t int32hexconst_array[]={ 0x8274ec, -0x0001, +0xABCDEF, 0xabcdef, 0x012345, 0x6789 };
    static const double doubleconst_array[]={1.5847, 3.14, -548e3, 3452.67e2, 485e-21};
    namespace structconst { static const char* strconst="This is a\n \"string constant\" \\/\b\f \r\xc3\xbf tabme\ttabme\n smile! \xf0\x9f\x98\x81"; static const int32_t int32const_array[]={182476, 56483, -2947}; }
    namespace structconst2 { namespace structconst { static const char* strconst="This is a\n \"string constant\" \\/\b\f \r\xc3\xbf tabme\ttabme\n smile! \xf0\x9f\x98\x81"; static const int32_t int32const_array[]={182476, 56483, -2947}; } static const int32_t int32const=3856384; }
}
    namespace testenum1
    {
    enum testenum1
    {
    value1 = 0,
    value2 = 1,
    value3 = 2,
    anothervalue = -1,
    anothervalue2 = -2,
    anothervalue3 = -3,
    hexval1 = 0x10,
    hexval2 = 0x11,
    neghexval1 = -0x7ffffffb,
    neghexval2 = -0x7ffffffa,
    more_values = -0x7ffffff9
    };
    }

union pixel{
uint8_t a[3];
struct s_type {
uint8_t r;
uint8_t b;
uint8_t g;
} s;
};
BOOST_STATIC_ASSERT(sizeof(pixel) == 3);

union pixel2{
uint8_t a[31];
struct s_type {
uint8_t c;
pixel d;
pixel e;
pixel f[7];
pixel g;
} s;
};
BOOST_STATIC_ASSERT(sizeof(pixel2) == 31);

union quaternion{
double a[4];
struct s_type {
double q0;
double q1;
double q2;
double q3;
} s;
};
BOOST_STATIC_ASSERT(sizeof(quaternion) == 32);

class testpod2 : public RobotRaconteur::RRPod {
public:
int8_t i1;
RobotRaconteur::pod_field_array<int8_t,15,false> i2;
RobotRaconteur::pod_field_array<int8_t,17,true> i3;

virtual std::string RRType() {return "com.robotraconteur.testing.TestService3.testpod2";  }
};

union vector3{
double a[3];
struct s_type {
double x;
double y;
double z;
} s;
};
BOOST_STATIC_ASSERT(sizeof(vector3) == 24);

union transform{
double a[7];
struct s_type {
quaternion rotation;
vector3 translation;
} s;
};
BOOST_STATIC_ASSERT(sizeof(transform) == 56);

class testpod1 : public RobotRaconteur::RRPod {
public:
double d1;
RobotRaconteur::pod_field_array<double,6,false> d2;
RobotRaconteur::pod_field_array<double,6,true> d3;
RobotRaconteur::pod_field_array<double,9,false> d4;
testpod2 s1;
RobotRaconteur::pod_field_array<testpod2,8,false> s2;
RobotRaconteur::pod_field_array<testpod2,9,true> s3;
RobotRaconteur::pod_field_array<testpod2,8,false> s4;
transform t1;
RobotRaconteur::pod_field_array<transform,4,false> t2;
RobotRaconteur::pod_field_array<transform,15,true> t3;
RobotRaconteur::pod_field_array<transform,8,false> t4;

virtual std::string RRType() {return "com.robotraconteur.testing.TestService3.testpod1";  }
};

class teststruct3 : public RobotRaconteur::RRStructure {
public:
testpod1 s1;
RR_INTRUSIVE_PTR<RobotRaconteur::RRPodArray<testpod1> > s2;
RR_INTRUSIVE_PTR<RobotRaconteur::RRPodArray<testpod1> > s3;
RR_INTRUSIVE_PTR<RobotRaconteur::RRPodArray<testpod1> > s4;
RR_INTRUSIVE_PTR<RobotRaconteur::RRPodMultiDimArray<testpod1> > s5;
RR_INTRUSIVE_PTR<RobotRaconteur::RRPodMultiDimArray<testpod1> > s6;
RR_INTRUSIVE_PTR<RobotRaconteur::RRList<RobotRaconteur::RRPodArray<testpod1>  > > s7;
RR_INTRUSIVE_PTR<RobotRaconteur::RRList<RobotRaconteur::RRPodArray<testpod1>  > > s8;
RR_INTRUSIVE_PTR<RobotRaconteur::RRList<RobotRaconteur::RRPodMultiDimArray<testpod1>  > > s9;
RR_INTRUSIVE_PTR<RobotRaconteur::RRValue> s10;
RR_INTRUSIVE_PTR<RobotRaconteur::RRValue> s11;
RR_INTRUSIVE_PTR<RobotRaconteur::RRValue> s12;
RR_INTRUSIVE_PTR<RobotRaconteur::RRValue> s13;
RR_INTRUSIVE_PTR<RobotRaconteur::RRValue> s14;
RR_INTRUSIVE_PTR<RobotRaconteur::RRValue> s15;
transform t1;
RR_INTRUSIVE_PTR<RobotRaconteur::RRNamedArray<transform> > t2;
RR_INTRUSIVE_PTR<RobotRaconteur::RRNamedMultiDimArray<transform> > t3;
RR_INTRUSIVE_PTR<RobotRaconteur::RRValue> t4;
RR_INTRUSIVE_PTR<RobotRaconteur::RRValue> t5;
RR_INTRUSIVE_PTR<RobotRaconteur::RRList<RobotRaconteur::RRNamedArray<transform>  > > t6;
RR_INTRUSIVE_PTR<RobotRaconteur::RRList<RobotRaconteur::RRNamedArray<transform>  > > t7;
RR_INTRUSIVE_PTR<RobotRaconteur::RRList<RobotRaconteur::RRNamedMultiDimArray<transform>  > > t8;
RR_INTRUSIVE_PTR<RobotRaconteur::RRValue> t9;
RR_INTRUSIVE_PTR<RobotRaconteur::RRValue> t10;
RR_INTRUSIVE_PTR<RobotRaconteur::RRValue> t11;

virtual std::string RRType() {return "com.robotraconteur.testing.TestService3.teststruct3";  }
};

#ifndef BOOST_NO_CXX11_TEMPLATE_ALIASES
using teststruct3Ptr = RR_INTRUSIVE_PTR<teststruct3>;
#endif

class testroot3 : public virtual RobotRaconteur::RRObject
{
public:
virtual int32_t get_readme()=0;

virtual void set_writeme(int32_t value)=0;

virtual int32_t get_unknown_modifier()=0;
virtual void set_unknown_modifier(int32_t value)=0;

virtual testenum1::testenum1 get_testenum1_prop()=0;
virtual void set_testenum1_prop(testenum1::testenum1 value)=0;

virtual testpod1 get_testpod1_prop()=0;
virtual void set_testpod1_prop(const testpod1& value)=0;

virtual RR_INTRUSIVE_PTR<teststruct3 > get_teststruct3_prop()=0;
virtual void set_teststruct3_prop(RR_INTRUSIVE_PTR<teststruct3 > value)=0;

virtual RR_INTRUSIVE_PTR<RobotRaconteur::RRList<RobotRaconteur::RRArray<double >  > > get_d1()=0;
virtual void set_d1(RR_INTRUSIVE_PTR<RobotRaconteur::RRList<RobotRaconteur::RRArray<double >  > > value)=0;

virtual RR_INTRUSIVE_PTR<RobotRaconteur::RRList<RobotRaconteur::RRArray<double >  > > get_d2()=0;
virtual void set_d2(RR_INTRUSIVE_PTR<RobotRaconteur::RRList<RobotRaconteur::RRArray<double >  > > value)=0;

virtual RR_INTRUSIVE_PTR<RobotRaconteur::RRMap<int32_t,RobotRaconteur::RRArray<double >  > > get_d3()=0;
virtual void set_d3(RR_INTRUSIVE_PTR<RobotRaconteur::RRMap<int32_t,RobotRaconteur::RRArray<double >  > > value)=0;

virtual RR_INTRUSIVE_PTR<RobotRaconteur::RRMap<int32_t,RobotRaconteur::RRArray<double >  > > get_d4()=0;
virtual void set_d4(RR_INTRUSIVE_PTR<RobotRaconteur::RRMap<int32_t,RobotRaconteur::RRArray<double >  > > value)=0;

virtual RR_INTRUSIVE_PTR<RobotRaconteur::RRList<RobotRaconteur::RRMultiDimArray<double >  > > get_d5()=0;
virtual void set_d5(RR_INTRUSIVE_PTR<RobotRaconteur::RRList<RobotRaconteur::RRMultiDimArray<double >  > > value)=0;

virtual RR_INTRUSIVE_PTR<RobotRaconteur::RRMap<int32_t,RobotRaconteur::RRMultiDimArray<double >  > > get_d6()=0;
virtual void set_d6(RR_INTRUSIVE_PTR<RobotRaconteur::RRMap<int32_t,RobotRaconteur::RRMultiDimArray<double >  > > value)=0;

virtual vector3 get_testnamedarray1()=0;
virtual void set_testnamedarray1(const vector3& value)=0;

virtual transform get_testnamedarray2()=0;
virtual void set_testnamedarray2(const transform& value)=0;

virtual RR_INTRUSIVE_PTR<RobotRaconteur::RRNamedArray<transform> > get_testnamedarray3()=0;
virtual void set_testnamedarray3(RR_INTRUSIVE_PTR<RobotRaconteur::RRNamedArray<transform> > value)=0;

virtual RR_INTRUSIVE_PTR<RobotRaconteur::RRNamedMultiDimArray<transform> > get_testnamedarray4()=0;
virtual void set_testnamedarray4(RR_INTRUSIVE_PTR<RobotRaconteur::RRNamedMultiDimArray<transform> > value)=0;

virtual RR_INTRUSIVE_PTR<RobotRaconteur::RRNamedMultiDimArray<transform> > get_testnamedarray5()=0;
virtual void set_testnamedarray5(RR_INTRUSIVE_PTR<RobotRaconteur::RRNamedMultiDimArray<transform> > value)=0;

virtual RobotRaconteur::cdouble get_c1()=0;
virtual void set_c1(RobotRaconteur::cdouble value)=0;

virtual RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<RobotRaconteur::cdouble > > get_c2()=0;
virtual void set_c2(RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<RobotRaconteur::cdouble > > value)=0;

virtual RR_INTRUSIVE_PTR<RobotRaconteur::RRMultiDimArray<RobotRaconteur::cdouble > > get_c3()=0;
virtual void set_c3(RR_INTRUSIVE_PTR<RobotRaconteur::RRMultiDimArray<RobotRaconteur::cdouble > > value)=0;

virtual RR_INTRUSIVE_PTR<RobotRaconteur::RRList<RobotRaconteur::RRArray<RobotRaconteur::cdouble >  > > get_c4()=0;
virtual void set_c4(RR_INTRUSIVE_PTR<RobotRaconteur::RRList<RobotRaconteur::RRArray<RobotRaconteur::cdouble >  > > value)=0;

virtual RR_INTRUSIVE_PTR<RobotRaconteur::RRList<RobotRaconteur::RRArray<RobotRaconteur::cdouble >  > > get_c5()=0;
virtual void set_c5(RR_INTRUSIVE_PTR<RobotRaconteur::RRList<RobotRaconteur::RRArray<RobotRaconteur::cdouble >  > > value)=0;

virtual RR_INTRUSIVE_PTR<RobotRaconteur::RRList<RobotRaconteur::RRMultiDimArray<RobotRaconteur::cdouble >  > > get_c6()=0;
virtual void set_c6(RR_INTRUSIVE_PTR<RobotRaconteur::RRList<RobotRaconteur::RRMultiDimArray<RobotRaconteur::cdouble >  > > value)=0;

virtual RobotRaconteur::cfloat get_c7()=0;
virtual void set_c7(RobotRaconteur::cfloat value)=0;

virtual RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<RobotRaconteur::cfloat > > get_c8()=0;
virtual void set_c8(RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<RobotRaconteur::cfloat > > value)=0;

virtual RR_INTRUSIVE_PTR<RobotRaconteur::RRMultiDimArray<RobotRaconteur::cfloat > > get_c9()=0;
virtual void set_c9(RR_INTRUSIVE_PTR<RobotRaconteur::RRMultiDimArray<RobotRaconteur::cfloat > > value)=0;

virtual RR_INTRUSIVE_PTR<RobotRaconteur::RRList<RobotRaconteur::RRArray<RobotRaconteur::cfloat >  > > get_c10()=0;
virtual void set_c10(RR_INTRUSIVE_PTR<RobotRaconteur::RRList<RobotRaconteur::RRArray<RobotRaconteur::cfloat >  > > value)=0;

virtual RR_INTRUSIVE_PTR<RobotRaconteur::RRList<RobotRaconteur::RRArray<RobotRaconteur::cfloat >  > > get_c11()=0;
virtual void set_c11(RR_INTRUSIVE_PTR<RobotRaconteur::RRList<RobotRaconteur::RRArray<RobotRaconteur::cfloat >  > > value)=0;

virtual RR_INTRUSIVE_PTR<RobotRaconteur::RRList<RobotRaconteur::RRMultiDimArray<RobotRaconteur::cfloat >  > > get_c12()=0;
virtual void set_c12(RR_INTRUSIVE_PTR<RobotRaconteur::RRList<RobotRaconteur::RRMultiDimArray<RobotRaconteur::cfloat >  > > value)=0;

virtual RobotRaconteur::rr_bool get_b1()=0;
virtual void set_b1(RobotRaconteur::rr_bool value)=0;

virtual RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<RobotRaconteur::rr_bool > > get_b2()=0;
virtual void set_b2(RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<RobotRaconteur::rr_bool > > value)=0;

virtual RR_INTRUSIVE_PTR<RobotRaconteur::RRMultiDimArray<RobotRaconteur::rr_bool > > get_b3()=0;
virtual void set_b3(RR_INTRUSIVE_PTR<RobotRaconteur::RRMultiDimArray<RobotRaconteur::rr_bool > > value)=0;

virtual RR_INTRUSIVE_PTR<RobotRaconteur::RRList<RobotRaconteur::RRArray<RobotRaconteur::rr_bool >  > > get_b4()=0;
virtual void set_b4(RR_INTRUSIVE_PTR<RobotRaconteur::RRList<RobotRaconteur::RRArray<RobotRaconteur::rr_bool >  > > value)=0;

virtual RR_INTRUSIVE_PTR<RobotRaconteur::RRList<RobotRaconteur::RRArray<RobotRaconteur::rr_bool >  > > get_b5()=0;
virtual void set_b5(RR_INTRUSIVE_PTR<RobotRaconteur::RRList<RobotRaconteur::RRArray<RobotRaconteur::rr_bool >  > > value)=0;

virtual RR_INTRUSIVE_PTR<RobotRaconteur::RRList<RobotRaconteur::RRMultiDimArray<RobotRaconteur::rr_bool >  > > get_b6()=0;
virtual void set_b6(RR_INTRUSIVE_PTR<RobotRaconteur::RRList<RobotRaconteur::RRMultiDimArray<RobotRaconteur::rr_bool >  > > value)=0;

virtual void testpod1_func1(const testpod1& s)=0;

virtual testpod1 testpod1_func2()=0;

virtual RR_SHARED_PTR<RobotRaconteur::Generator<double,void > > gen_func1()=0;

virtual RR_SHARED_PTR<RobotRaconteur::Generator<RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<uint8_t > >,void > > gen_func2(const std::string& name)=0;

virtual RR_SHARED_PTR<RobotRaconteur::Generator<void,RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<uint8_t > > > > gen_func3(const std::string& name)=0;

virtual RR_SHARED_PTR<RobotRaconteur::Generator<RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<uint8_t > >,RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<uint8_t > > > > gen_func4()=0;

virtual RR_SHARED_PTR<RobotRaconteur::Generator<RR_INTRUSIVE_PTR<com::robotraconteur::testing::TestService1::teststruct2 >,RR_INTRUSIVE_PTR<com::robotraconteur::testing::TestService1::teststruct2 > > > gen_func5()=0;

virtual RR_SHARED_PTR<obj4 > get_o4()=0;

virtual RR_SHARED_PTR<obj5 > get_nolock_test()=0;

virtual RR_SHARED_PTR<RobotRaconteur::Pipe<int32_t > > get_unreliable1()=0;
virtual void set_unreliable1(RR_SHARED_PTR<RobotRaconteur::Pipe<int32_t > > value)=0;

virtual RR_SHARED_PTR<RobotRaconteur::Pipe<RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<int32_t > > > > get_p1()=0;
virtual void set_p1(RR_SHARED_PTR<RobotRaconteur::Pipe<RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<int32_t > > > > value)=0;

virtual RR_SHARED_PTR<RobotRaconteur::Pipe<RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<int32_t > > > > get_p2()=0;
virtual void set_p2(RR_SHARED_PTR<RobotRaconteur::Pipe<RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<int32_t > > > > value)=0;

virtual RR_SHARED_PTR<RobotRaconteur::Pipe<RR_INTRUSIVE_PTR<RobotRaconteur::RRMultiDimArray<int32_t > > > > get_p3()=0;
virtual void set_p3(RR_SHARED_PTR<RobotRaconteur::Pipe<RR_INTRUSIVE_PTR<RobotRaconteur::RRMultiDimArray<int32_t > > > > value)=0;

virtual RR_SHARED_PTR<RobotRaconteur::Wire<int32_t > > get_peekwire()=0;
virtual void set_peekwire(RR_SHARED_PTR<RobotRaconteur::Wire<int32_t > > value)=0;

virtual RR_SHARED_PTR<RobotRaconteur::Wire<int32_t > > get_pokewire()=0;
virtual void set_pokewire(RR_SHARED_PTR<RobotRaconteur::Wire<int32_t > > value)=0;

virtual RR_SHARED_PTR<RobotRaconteur::Wire<RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<int32_t > > > > get_w1()=0;
virtual void set_w1(RR_SHARED_PTR<RobotRaconteur::Wire<RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<int32_t > > > > value)=0;

virtual RR_SHARED_PTR<RobotRaconteur::Wire<RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<int32_t > > > > get_w2()=0;
virtual void set_w2(RR_SHARED_PTR<RobotRaconteur::Wire<RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<int32_t > > > > value)=0;

virtual RR_SHARED_PTR<RobotRaconteur::Wire<RR_INTRUSIVE_PTR<RobotRaconteur::RRMultiDimArray<int32_t > > > > get_w3()=0;
virtual void set_w3(RR_SHARED_PTR<RobotRaconteur::Wire<RR_INTRUSIVE_PTR<RobotRaconteur::RRMultiDimArray<int32_t > > > > value)=0;

virtual RR_SHARED_PTR<RobotRaconteur::ArrayMemory<double > > get_readmem()=0;

virtual RR_SHARED_PTR<RobotRaconteur::PodArrayMemory<testpod2 > > get_pod_m1()=0;

virtual RR_SHARED_PTR<RobotRaconteur::PodMultiDimArrayMemory<testpod2 > > get_pod_m2()=0;

virtual RR_SHARED_PTR<RobotRaconteur::NamedArrayMemory<transform > > get_namedarray_m1()=0;

virtual RR_SHARED_PTR<RobotRaconteur::NamedMultiDimArrayMemory<transform > > get_namedarray_m2()=0;

virtual RR_SHARED_PTR<RobotRaconteur::ArrayMemory<RobotRaconteur::cdouble > > get_c_m1()=0;

virtual RR_SHARED_PTR<RobotRaconteur::MultiDimArrayMemory<RobotRaconteur::cdouble > > get_c_m2()=0;

virtual RR_SHARED_PTR<RobotRaconteur::ArrayMemory<RobotRaconteur::cdouble > > get_c_m3()=0;

virtual RR_SHARED_PTR<RobotRaconteur::MultiDimArrayMemory<RobotRaconteur::cdouble > > get_c_m4()=0;

virtual RR_SHARED_PTR<RobotRaconteur::ArrayMemory<RobotRaconteur::rr_bool > > get_c_m5()=0;

virtual RR_SHARED_PTR<RobotRaconteur::MultiDimArrayMemory<RobotRaconteur::rr_bool > > get_c_m6()=0;

virtual std::string RRType() {return "com.robotraconteur.testing.TestService3.testroot3";  }
};

#ifndef BOOST_NO_CXX11_TEMPLATE_ALIASES
using testroot3Ptr = RR_SHARED_PTR<testroot3>;
#endif

class obj1 : public virtual RobotRaconteur::RRObject
{
public:
virtual RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double > > get_d1()=0;
virtual void set_d1(RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double > > value)=0;

virtual std::string RRType() {return "com.robotraconteur.testing.TestService3.obj1";  }
};

#ifndef BOOST_NO_CXX11_TEMPLATE_ALIASES
using obj1Ptr = RR_SHARED_PTR<obj1>;
#endif

class obj2 : public virtual obj1
{
public:
virtual RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double > > get_d1()=0;
virtual void set_d1(RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double > > value)=0;

virtual std::string RRType() {return "com.robotraconteur.testing.TestService3.obj2";  }
};

#ifndef BOOST_NO_CXX11_TEMPLATE_ALIASES
using obj2Ptr = RR_SHARED_PTR<obj2>;
#endif

class obj3 : public virtual obj1, public virtual obj2
{
public:
virtual RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double > > get_d1()=0;
virtual void set_d1(RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double > > value)=0;

virtual std::string RRType() {return "com.robotraconteur.testing.TestService3.obj3";  }
};

#ifndef BOOST_NO_CXX11_TEMPLATE_ALIASES
using obj3Ptr = RR_SHARED_PTR<obj3>;
#endif

class obj4 : public virtual com::robotraconteur::testing::TestService1::sub2
{
public:
virtual std::string get_s_ind()=0;
virtual void set_s_ind(const std::string& value)=0;

virtual int32_t get_i_ind()=0;
virtual void set_i_ind(int32_t value)=0;

virtual std::string get_data()=0;
virtual void set_data(const std::string& value)=0;

virtual RR_SHARED_PTR<com::robotraconteur::testing::TestService1::sub3 > get_o3_1(const std::string& ind)=0;

virtual std::string RRType() {return "com.robotraconteur.testing.TestService3.obj4";  }
};

#ifndef BOOST_NO_CXX11_TEMPLATE_ALIASES
using obj4Ptr = RR_SHARED_PTR<obj4>;
#endif

class obj5 : public virtual RobotRaconteur::RRObject
{
public:
virtual double get_p1()=0;
virtual void set_p1(double value)=0;

virtual double get_p2()=0;
virtual void set_p2(double value)=0;

virtual double get_p3()=0;
virtual void set_p3(double value)=0;

virtual int32_t f1()=0;

virtual int32_t f2()=0;

virtual RR_SHARED_PTR<RobotRaconteur::Pipe<int32_t > > get_q1()=0;
virtual void set_q1(RR_SHARED_PTR<RobotRaconteur::Pipe<int32_t > > value)=0;

virtual RR_SHARED_PTR<RobotRaconteur::Pipe<int32_t > > get_q2()=0;
virtual void set_q2(RR_SHARED_PTR<RobotRaconteur::Pipe<int32_t > > value)=0;

virtual RR_SHARED_PTR<RobotRaconteur::Wire<int32_t > > get_w1()=0;
virtual void set_w1(RR_SHARED_PTR<RobotRaconteur::Wire<int32_t > > value)=0;

virtual RR_SHARED_PTR<RobotRaconteur::Wire<int32_t > > get_w2()=0;
virtual void set_w2(RR_SHARED_PTR<RobotRaconteur::Wire<int32_t > > value)=0;

virtual RR_SHARED_PTR<RobotRaconteur::ArrayMemory<int32_t > > get_m1()=0;

virtual RR_SHARED_PTR<RobotRaconteur::ArrayMemory<int32_t > > get_m2()=0;

virtual RR_SHARED_PTR<RobotRaconteur::ArrayMemory<int32_t > > get_m3()=0;

virtual std::string RRType() {return "com.robotraconteur.testing.TestService3.obj5";  }
};

#ifndef BOOST_NO_CXX11_TEMPLATE_ALIASES
using obj5Ptr = RR_SHARED_PTR<obj5>;
#endif

}
}
}
}

namespace RobotRaconteur
{
RRPrimUtilPod(com::robotraconteur::testing::TestService3::testpod1, "com.robotraconteur.testing.TestService3.testpod1");
RRPrimUtilPod(com::robotraconteur::testing::TestService3::testpod2, "com.robotraconteur.testing.TestService3.testpod2");
RRPrimUtilNamedArray(com::robotraconteur::testing::TestService3::vector3, "com.robotraconteur.testing.TestService3.vector3",double);
RRPodStubNamedArrayType(com::robotraconteur::testing::TestService3::vector3);
RRPrimUtilNamedArray(com::robotraconteur::testing::TestService3::quaternion, "com.robotraconteur.testing.TestService3.quaternion",double);
RRPodStubNamedArrayType(com::robotraconteur::testing::TestService3::quaternion);
RRPrimUtilNamedArray(com::robotraconteur::testing::TestService3::transform, "com.robotraconteur.testing.TestService3.transform",double);
RRPodStubNamedArrayType(com::robotraconteur::testing::TestService3::transform);
RRPrimUtilNamedArray(com::robotraconteur::testing::TestService3::pixel, "com.robotraconteur.testing.TestService3.pixel",uint8_t);
RRPodStubNamedArrayType(com::robotraconteur::testing::TestService3::pixel);
RRPrimUtilNamedArray(com::robotraconteur::testing::TestService3::pixel2, "com.robotraconteur.testing.TestService3.pixel2",uint8_t);
RRPodStubNamedArrayType(com::robotraconteur::testing::TestService3::pixel2);
}
